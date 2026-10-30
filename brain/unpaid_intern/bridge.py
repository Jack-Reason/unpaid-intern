"""HTTP client for the mod's loopback bridge."""

from __future__ import annotations

from typing import Any

import httpx

from unpaid_intern.schema import ActionCall, Observation


class BridgeClient:
    """Talks to the SML mod bridge on 127.0.0.1 only."""

    def __init__(self, base_url: str = "http://127.0.0.1:8765", timeout: float = 10.0) -> None:
        if "127.0.0.1" not in base_url and "localhost" not in base_url:
            raise ValueError("BridgeClient refuses non-loopback base_url (safety)")
        self.base_url = base_url.rstrip("/")
        self.timeout = timeout

    def health(self) -> dict[str, Any]:
        with httpx.Client(timeout=self.timeout) as client:
            resp = client.get(f"{self.base_url}/health")
            resp.raise_for_status()
            return resp.json()

    def observe(self) -> Observation:
        with httpx.Client(timeout=self.timeout) as client:
            resp = client.get(f"{self.base_url}/observe")
            resp.raise_for_status()
            return Observation.model_validate(resp.json())

    def action(self, call: ActionCall) -> dict[str, Any]:
        payload = call.model_dump(exclude_none=True)
        with httpx.Client(timeout=self.timeout) as client:
            resp = client.post(f"{self.base_url}/action", json=payload)
            resp.raise_for_status()
            return resp.json()
