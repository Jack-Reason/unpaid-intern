import pytest

from unpaid_intern.bridge import BridgeClient
from unpaid_intern.mock_game import MockGameServer
from unpaid_intern.schema import ActionCall


@pytest.fixture()
def mock_bridge():
    server = MockGameServer()
    server.start()
    client = BridgeClient(server.base_url)
    yield server, client
    server.stop()


def test_health_and_observe(mock_bridge):
    server, client = mock_bridge
    assert client.health()["ok"] is True
    obs = client.observe()
    assert obs.tick >= 2
    assert len(obs.nearby_nodes) == 1


def test_follow_mine_build_with_materials(mock_bridge):
    server, client = mock_bridge
    assert client.action(ActionCall(name="follow", arguments={}))["ok"] is True
    # Mine twice to gather enough "iron ore" for stub cost
    client.action(ActionCall(name="mine", arguments={}))
    client.action(ActionCall(name="mine", arguments={}))
    obs = client.observe()
    assert obs.intern.inventory[0].count >= 10
    result = client.action(ActionCall(name="build_miner", arguments={}))
    assert result["ok"] is True
    obs2 = client.observe()
    assert obs2.nearby_nodes[0].occupied is True
    assert obs2.intern.inventory[0].count == obs.intern.inventory[0].count - 10


def test_build_miner_insufficient_materials(mock_bridge):
    _, client = mock_bridge
    result = client.action(ActionCall(name="build_miner", arguments={}))
    assert result["ok"] is False
    assert result["error"] == "insufficient_materials"


def test_build_miner_free_placement_dev_flag(mock_bridge):
    server, client = mock_bridge
    server.state.free_placement = True
    result = client.action(ActionCall(name="build_miner", arguments={}))
    assert result["ok"] is True
    assert result.get("dev_free_placement") is True


def test_bridge_rejects_non_loopback():
    with pytest.raises(ValueError):
        BridgeClient("http://192.168.1.10:8765")
