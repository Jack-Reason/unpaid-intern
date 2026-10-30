"""In-process mock of the mod bridge for offline brain tests."""

from __future__ import annotations

import json
import threading
from http.server import BaseHTTPRequestHandler, HTTPServer
from typing import Any

from unpaid_intern.schema import (
    ActionCall,
    ChatMessage,
    HostState,
    InternState,
    InventoryItem,
    Observation,
    ResourceNode,
    Vec3,
)


class MockGameState:
    def __init__(self) -> None:
        self.observation = Observation(
            tick=1,
            session={"mode": "mock", "bridge": "python-mock"},
            intern=InternState(
                location=Vec3(x=0, y=0, z=0, yaw=90),
                inventory=[InventoryItem(item="Desc_OreIron_C", count=0)],
                job="idle",
            ),
            host=HostState(location=Vec3(x=100, y=0, z=0, yaw=0)),
            nearby_nodes=[
                ResourceNode(
                    id="ResNode_Iron_42",
                    resource="Desc_OreIron_C",
                    purity="normal",
                    occupied=False,
                    distance_m=18.5,
                )
            ],
            chat_inbox=[],
        )
        self.actions: list[dict[str, Any]] = []
        self.chat_mode = "commands"  # or "always"
        self.free_placement = False

    def push_chat(self, text: str, from_name: str = "Jack") -> None:
        self.observation.chat_inbox.append(
            ChatMessage.model_validate({"from": from_name, "text": text})
        )

    def handle_action(self, payload: dict[str, Any]) -> dict[str, Any]:
        call = ActionCall.model_validate(payload)
        self.actions.append(call.model_dump())
        name = call.name
        args = call.arguments

        if name == "say":
            return {"ok": True, "echo": args.get("text", "")}
        if name == "follow":
            self.observation.intern.job = "follow"
            return {"ok": True}
        if name == "hold" or name == "cancel":
            self.observation.intern.job = "hold"
            return {"ok": True}
        if name == "goto":
            self.observation.intern.job = "goto"
            if self.observation.intern.location:
                self.observation.intern.location.x = float(args.get("x", 0))
                self.observation.intern.location.y = float(args.get("y", 0))
                self.observation.intern.location.z = float(args.get("z", 0))
            return {"ok": True}
        if name == "mine":
            self.observation.intern.job = "mine"
            # Simulate gathering into intern inventory
            inv = self.observation.intern.inventory
            if inv:
                inv[0].count += 5
            else:
                self.observation.intern.inventory = [
                    InventoryItem(item="Desc_OreIron_C", count=5)
                ]
            return {"ok": True, "gathered": 5}
        if name == "build_miner":
            node_id = args.get("node_id") or (
                self.observation.nearby_nodes[0].id if self.observation.nearby_nodes else None
            )
            confirm = bool(args.get("confirm"))
            node = next((n for n in self.observation.nearby_nodes if n.id == node_id), None)
            if node and node.occupied and not confirm:
                return {"ok": False, "error": "needs_confirm"}
            if self.free_placement:
                if node:
                    node.occupied = True
                self.observation.intern.job = "build_miner"
                return {"ok": True, "dev_free_placement": True, "node_id": node_id}
            # Materials from intern inventory. Stub cost: count >= 10 iron ore.
            iron = next(
                (i for i in self.observation.intern.inventory if i.item == "Desc_OreIron_C"),
                None,
            )
            if not iron or iron.count < 10:
                return {"ok": False, "error": "insufficient_materials"}
            iron.count -= 10
            if node:
                node.occupied = True
            self.observation.intern.job = "build_miner"
            return {"ok": True, "node_id": node_id}
        if name == "observe":
            return {"ok": True, "observation": self.observation.model_dump(by_alias=True)}
        return {"ok": False, "error": f"unknown_action:{name}"}


def _make_handler(state: MockGameState):
    class Handler(BaseHTTPRequestHandler):
        def log_message(self, format: str, *args: Any) -> None:  # noqa: A003
            return

        def _read_json(self) -> dict[str, Any]:
            length = int(self.headers.get("Content-Length", "0"))
            raw = self.rfile.read(length) if length else b"{}"
            return json.loads(raw.decode("utf-8") or "{}")

        def _write_json(self, code: int, payload: dict[str, Any]) -> None:
            body = json.dumps(payload).encode("utf-8")
            self.send_response(code)
            self.send_header("Content-Type", "application/json")
            self.send_header("Content-Length", str(len(body)))
            self.end_headers()
            self.wfile.write(body)

        def do_GET(self) -> None:  # noqa: N802
            if self.path == "/health":
                self._write_json(200, {"ok": True, "service": "unpaid-intern-mock"})
                return
            if self.path == "/observe":
                state.observation.tick += 1
                self._write_json(200, state.observation.model_dump(by_alias=True))
                return
            self._write_json(404, {"ok": False, "error": "not_found"})

        def do_POST(self) -> None:  # noqa: N802
            if self.path == "/action":
                payload = self._read_json()
                result = state.handle_action(payload)
                # Business failures stay HTTP 200 with ok:false (easier for the brain).
                self._write_json(200, result)
                return
            self._write_json(404, {"ok": False, "error": "not_found"})

    return Handler


class MockGameServer:
    def __init__(self, host: str = "127.0.0.1", port: int = 0) -> None:
        self.state = MockGameState()
        self._httpd = HTTPServer((host, port), _make_handler(self.state))
        self._thread: threading.Thread | None = None

    @property
    def port(self) -> int:
        return self._httpd.server_address[1]

    @property
    def base_url(self) -> str:
        return f"http://127.0.0.1:{self.port}"

    def start(self) -> None:
        self._thread = threading.Thread(target=self._httpd.serve_forever, daemon=True)
        self._thread.start()

    def stop(self) -> None:
        self._httpd.shutdown()
        if self._thread:
            self._thread.join(timeout=2)
