"""ADR-0006: second-licence topology + input-driver action intents on the mock."""

from __future__ import annotations

from unpaid_intern.mock_game import MockGameState
from unpaid_intern.schema import Observation


def test_observation_accepts_second_licence_session_fields() -> None:
    obs = Observation.model_validate(
        {
            "tick": 1,
            "session": {
                "mode": "client",
                "topology": "second_licence",
                "sml": "3.12.0",
            },
            "intern": {"id": "unpaid-intern-1", "job": "follow"},
            "host": {},
            "nearby_nodes": [],
            "chat_inbox": [{"from": "Jack", "text": "/intern follow"}],
        }
    )
    assert obs.session["topology"] == "second_licence"
    assert obs.chat_inbox[0].from_ == "Jack"


def test_follow_hold_goto_mine_intents_on_mock_state() -> None:
    state = MockGameState()
    state.observation.session["topology"] = "second_licence"
    state.observation.session["mode"] = "client"

    assert state.handle_action({"name": "follow", "arguments": {"distance_m": 4}}).get("ok")
    assert state.observation.intern.job == "follow"

    assert state.handle_action({"name": "hold", "arguments": {}}).get("ok")
    assert state.observation.intern.job == "hold"

    assert state.handle_action(
        {"name": "goto", "arguments": {"x": 10.0, "y": 20.0, "z": 30.0}}
    ).get("ok")
    assert state.observation.intern.job == "goto"
    assert state.observation.intern.location is not None
    assert state.observation.intern.location.x == 10.0

    assert state.handle_action({"name": "mine", "arguments": {}}).get("ok")
    assert state.observation.intern.job == "mine"
    assert state.observation.intern.inventory[0].count == 5


def test_build_miner_confirm_path_still_driver_compatible() -> None:
    state = MockGameState()
    state.observation.nearby_nodes[0].occupied = True
    blocked = state.handle_action(
        {"name": "build_miner", "arguments": {"node_id": "ResNode_Iron_42", "confirm": False}}
    )
    assert blocked["ok"] is False
    assert blocked["error"] == "needs_confirm"

    state.handle_action({"name": "mine", "arguments": {}})
    state.handle_action({"name": "mine", "arguments": {}})
    ok = state.handle_action(
        {"name": "build_miner", "arguments": {"node_id": "ResNode_Iron_42", "confirm": True}}
    )
    assert ok["ok"] is True
    assert state.observation.nearby_nodes[0].occupied is True
