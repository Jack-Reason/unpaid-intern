"""Bridge protocol / JSON schema contract tests."""

from __future__ import annotations

import json

import pytest
from pydantic import ValidationError

from unpaid_intern.bridge import BridgeClient
from unpaid_intern.mock_game import MockGameServer
from unpaid_intern.schema import (
    TOOL_SPECS,
    ActionCall,
    ActionName,
    Observation,
)


@pytest.fixture()
def mock_bridge():
    server = MockGameServer()
    server.start()
    client = BridgeClient(server.base_url)
    yield server, client
    server.stop()


def test_c01_health_ok(mock_bridge):
    _, client = mock_bridge
    body = client.health()
    assert body["ok"] is True


def test_c02_observe_validates(mock_bridge):
    _, client = mock_bridge
    obs = client.observe()
    assert isinstance(obs, Observation)
    assert obs.tick >= 1
    # Round-trip through JSON (alias for chat "from")
    raw = json.loads(obs.model_dump_json(by_alias=True))
    again = Observation.model_validate(raw)
    assert again.intern.id == obs.intern.id


def test_c03_all_action_names_minimal():
    minimal_args: dict[str, dict] = {
        "say": {"text": "hi"},
        "follow": {},
        "hold": {},
        "goto": {"x": 1.0, "y": 2.0, "z": 3.0},
        "mine": {},
        "build_miner": {},
        "cancel": {},
        "observe": {},
    }
    for name in ActionName.__args__:  # type: ignore[attr-defined]
        call = ActionCall(name=name, arguments=minimal_args[name])
        assert call.name == name


def test_c04_unknown_action_rejected():
    with pytest.raises(ValidationError):
        ActionCall.model_validate({"name": "nuke_factory", "arguments": {}})


def test_c05_business_error_http_200(mock_bridge):
    _, client = mock_bridge
    result = client.action(ActionCall(name="build_miner", arguments={}))
    assert result["ok"] is False
    assert result["error"] == "insufficient_materials"


def test_c06_nearby_nodes_shape(mock_bridge):
    _, client = mock_bridge
    obs = client.observe()
    assert obs.nearby_nodes
    node = obs.nearby_nodes[0]
    assert node.id
    assert node.resource
    assert isinstance(node.occupied, bool)
    assert node.distance_m >= 0


def test_c07_tool_specs_subset_of_actions():
    names = {t["function"]["name"] for t in TOOL_SPECS}
    allowed = set(ActionName.__args__)  # type: ignore[attr-defined]
    # observe may be bridge-only; tools should still be valid ActionNames
    assert names <= allowed


def test_c08_rejects_non_loopback():
    with pytest.raises(ValueError):
        BridgeClient("http://10.0.0.2:8765")
