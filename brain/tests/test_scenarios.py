"""Mock-game scenario tests for MVP stories."""

from __future__ import annotations

from unpaid_intern.agent import InternAgent
from unpaid_intern.bridge import BridgeClient
from unpaid_intern.mock_game import MockGameServer
from unpaid_intern.providers.base import LLMResponse
from unpaid_intern.schema import ActionCall


class ScriptedProvider:
    def __init__(self, *responses: LLMResponse) -> None:
        self._queue = list(responses)
        self.calls = 0

    def complete(self, *, system: str, user: str, tools=None) -> LLMResponse:
        self.calls += 1
        if not self._queue:
            return LLMResponse(text="(scripted provider exhausted)")
        return self._queue.pop(0)


def _run(server: MockGameServer):
    bridge = BridgeClient(server.base_url)
    return bridge


def test_s01_mine_that_node():
    server = MockGameServer()
    server.start()
    try:
        bridge = _run(server)
        provider = ScriptedProvider(
            LLMResponse(
                tool_calls=[
                    {"name": "say", "arguments": {"text": "On the iron, pioneer."}},
                    {"name": "mine", "arguments": {"node_id": "ResNode_Iron_42"}},
                ]
            )
        )
        agent = InternAgent(provider, bridge)
        before = bridge.observe().intern.inventory[0].count
        result = agent.tick("/intern mine that iron")
        assert all(r.get("ok") for r in result["results"])
        after = bridge.observe()
        assert after.intern.inventory[0].count == before + 5
        assert after.intern.job == "mine"
    finally:
        server.stop()


def test_s02_build_miner_happy_path():
    server = MockGameServer()
    server.start()
    try:
        bridge = _run(server)
        # Stock materials via two mines
        bridge.action(ActionCall(name="mine", arguments={}))
        bridge.action(ActionCall(name="mine", arguments={}))
        provider = ScriptedProvider(
            LLMResponse(
                tool_calls=[
                    {"name": "say", "arguments": {"text": "Miner going down."}},
                    {"name": "build_miner", "arguments": {"node_id": "ResNode_Iron_42"}},
                ]
            )
        )
        agent = InternAgent(provider, bridge)
        result = agent.tick("/intern build a miner")
        assert all(r.get("ok") for r in result["results"])
        obs = bridge.observe()
        assert obs.nearby_nodes[0].occupied is True
        assert obs.intern.inventory[0].count == 0
    finally:
        server.stop()


def test_s03_build_miner_blocked_without_mats():
    server = MockGameServer()
    server.start()
    try:
        bridge = _run(server)
        provider = ScriptedProvider(
            LLMResponse(
                tool_calls=[
                    {
                        "name": "say",
                        "arguments": {"text": "I'm short on plates — mining first."},
                    },
                    {"name": "build_miner", "arguments": {}},
                ]
            )
        )
        agent = InternAgent(provider, bridge)
        result = agent.tick("/intern build miner now")
        assert result["results"][0]["ok"] is True
        assert result["results"][1]["ok"] is False
        assert result["results"][1]["error"] == "insufficient_materials"
    finally:
        server.stop()


def test_s04_overwrite_needs_confirm():
    server = MockGameServer()
    server.start()
    try:
        bridge = _run(server)
        server.state.observation.nearby_nodes[0].occupied = True
        server.state.observation.intern.inventory[0].count = 20
        denied = bridge.action(
            ActionCall(name="build_miner", arguments={"confirm": False})
        )
        assert denied["ok"] is False
        assert denied["error"] == "needs_confirm"
        allowed = bridge.action(
            ActionCall(name="build_miner", arguments={"confirm": True})
        )
        assert allowed["ok"] is True
    finally:
        server.stop()


def test_s05_cancel_clears_job():
    server = MockGameServer()
    server.start()
    try:
        bridge = _run(server)
        bridge.action(ActionCall(name="mine", arguments={}))
        assert bridge.observe().intern.job == "mine"
        assert bridge.action(ActionCall(name="cancel", arguments={}))["ok"] is True
        assert bridge.observe().intern.job == "idle"
    finally:
        server.stop()


def test_s06_follow_then_hold():
    server = MockGameServer()
    server.start()
    try:
        bridge = _run(server)
        bridge.action(ActionCall(name="follow", arguments={"distance_m": 4}))
        assert bridge.observe().intern.job == "follow"
        bridge.action(ActionCall(name="hold", arguments={}))
        assert bridge.observe().intern.job == "hold"
    finally:
        server.stop()
