from unpaid_intern.agent import InternAgent
from unpaid_intern.bridge import BridgeClient
from unpaid_intern.mock_game import MockGameServer
from unpaid_intern.providers.base import LLMResponse


class ScriptedProvider:
    def __init__(self, response: LLMResponse) -> None:
        self.response = response
        self.calls = 0

    def complete(self, *, system: str, user: str, tools=None) -> LLMResponse:
        self.calls += 1
        assert "Unpaid Intern" in system or "unpaid" in system.lower()
        assert "Observation" in user
        return self.response


def test_agent_applies_tool_calls():
    server = MockGameServer()
    server.start()
    try:
        bridge = BridgeClient(server.base_url)
        provider = ScriptedProvider(
            LLMResponse(
                text=None,
                tool_calls=[
                    {"name": "say", "arguments": {"text": "Right behind you."}},
                    {"name": "follow", "arguments": {"distance_m": 3.5}},
                ],
            )
        )
        agent = InternAgent(provider, bridge)
        result = agent.tick("/intern follow")
        assert provider.calls == 1
        assert len(result["results"]) == 2
        assert all(r.get("ok") for r in result["results"])
        obs = bridge.observe()
        assert obs.intern.job == "follow"
    finally:
        server.stop()


def test_agent_falls_back_to_say_on_plaintext():
    server = MockGameServer()
    server.start()
    try:
        bridge = BridgeClient(server.base_url)
        provider = ScriptedProvider(LLMResponse(text="Copy that, pioneer."))
        agent = InternAgent(provider, bridge)
        result = agent.tick("hello")
        assert result["results"][0]["ok"] is True
        assert result["results"][0]["echo"] == "Copy that, pioneer."
    finally:
        server.stop()
