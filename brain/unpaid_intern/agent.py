"""Brain loop: observation + player text → LLM → tool/actions."""

from __future__ import annotations

import json
from typing import Any

from unpaid_intern.bridge import BridgeClient
from unpaid_intern.prompts.persona import PERSONA_SYSTEM, build_user_turn
from unpaid_intern.providers.base import LLMProvider, LLMResponse
from unpaid_intern.schema import TOOL_SPECS, ActionCall, Observation


class InternAgent:
    def __init__(self, provider: LLMProvider, bridge: BridgeClient) -> None:
        self.provider = provider
        self.bridge = bridge

    def plan(self, observation: Observation, player_text: str | None = None) -> LLMResponse:
        user = build_user_turn(
            observation.model_dump_json(by_alias=True, indent=2),
            player_text,
        )
        return self.provider.complete(system=PERSONA_SYSTEM, user=user, tools=TOOL_SPECS)

    def apply_tool_calls(self, tool_calls: list[dict[str, Any]]) -> list[dict[str, Any]]:
        results: list[dict[str, Any]] = []
        for tc in tool_calls:
            name = tc.get("name")
            if not name:
                continue
            args = tc.get("arguments") or {}
            if isinstance(args, str):
                try:
                    args = json.loads(args)
                except json.JSONDecodeError:
                    args = {}
            call = ActionCall(name=name, arguments=args, id=tc.get("id"))
            results.append(self.bridge.action(call))
        return results

    def tick(self, player_text: str | None = None) -> dict[str, Any]:
        obs = self.bridge.observe()
        # Prefer explicit player_text; else drain chat_inbox last message if command-like
        text = player_text
        if text is None and obs.chat_inbox:
            text = obs.chat_inbox[-1].text
        response = self.plan(obs, text)
        applied: list[dict[str, Any]] = []
        if response.tool_calls:
            applied = self.apply_tool_calls(response.tool_calls)
        elif response.text:
            # Model spoke without tools — still say it in-game
            applied = [
                self.bridge.action(ActionCall(name="say", arguments={"text": response.text}))
            ]
        return {
            "observation_tick": obs.tick,
            "text": response.text,
            "tool_calls": response.tool_calls,
            "results": applied,
        }
