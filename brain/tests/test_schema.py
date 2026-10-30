from unpaid_intern.schema import TOOL_SPECS, ActionCall, Observation


def test_tool_specs_cover_mvp_actions():
    names = {t["function"]["name"] for t in TOOL_SPECS}
    assert {"say", "follow", "hold", "goto", "mine", "build_miner", "cancel"} <= names


def test_action_call_roundtrip():
    call = ActionCall(name="follow", arguments={"distance_m": 4.0})
    data = call.model_dump()
    assert data["name"] == "follow"
    assert ActionCall.model_validate(data).arguments["distance_m"] == 4.0


def test_observation_chat_alias():
    obs = Observation.model_validate(
        {
            "tick": 3,
            "chat_inbox": [{"from": "Jack", "text": "/intern mine"}],
        }
    )
    assert obs.chat_inbox[0].from_ == "Jack"
    dumped = obs.model_dump(by_alias=True)
    assert dumped["chat_inbox"][0]["from"] == "Jack"
