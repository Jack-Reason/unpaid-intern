"""Offline LLM eval: golden prompts → expected tool wiring via scripted provider."""

from __future__ import annotations

import json
from pathlib import Path

import pytest

from unpaid_intern.agent import InternAgent
from unpaid_intern.bridge import BridgeClient
from unpaid_intern.mock_game import MockGameServer
from unpaid_intern.providers.base import LLMResponse
from unpaid_intern.schema import ActionCall

FIXTURES = Path(__file__).parent / "fixtures" / "golden_prompts.json"


class ScriptedProvider:
    def __init__(self, response: LLMResponse) -> None:
        self.response = response

    def complete(self, *, system: str, user: str, tools=None) -> LLMResponse:
        assert "Pioneer said" in user or "Observation" in user
        return self.response


def _cases():
    return json.loads(FIXTURES.read_text(encoding="utf-8"))


@pytest.mark.parametrize("case", _cases(), ids=lambda c: c["id"])
def test_golden_offline(case: dict):
    server = MockGameServer()
    server.start()
    try:
        bridge = BridgeClient(server.base_url)
        if case.get("pre_mine_twice"):
            bridge.action(ActionCall(name="mine", arguments={}))
            bridge.action(ActionCall(name="mine", arguments={}))

        expected = case["expected_tools"]
        calls = case["tool_calls"]
        assert [c["name"] for c in calls] == expected

        for name in case.get("forbid_tools", []):
            assert name not in expected

        provider = ScriptedProvider(LLMResponse(tool_calls=calls))
        agent = InternAgent(provider, bridge)
        result = agent.tick(case["prompt"])
        names = [tc["name"] for tc in result["tool_calls"]]
        assert names == expected
        assert all(r.get("ok") for r in result["results"])

        obs = bridge.observe()
        if "assert_job" in case:
            assert obs.intern.job == case["assert_job"]
        if case.get("assert_occupied"):
            assert obs.nearby_nodes[0].occupied is True
    finally:
        server.stop()
