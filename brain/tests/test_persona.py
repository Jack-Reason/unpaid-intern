from unpaid_intern.prompts.persona import PERSONA_SYSTEM, build_user_turn


def test_persona_mentions_contractor_and_tools():
    assert "contractor" in PERSONA_SYSTEM.lower() or "Unpaid Intern" in PERSONA_SYSTEM
    assert "build_miner" in PERSONA_SYSTEM
    assert "alternative" in PERSONA_SYSTEM.lower()


def test_build_user_turn_includes_observation():
    text = build_user_turn('{"tick": 1}', "/intern mine")
    assert '"tick": 1' in text
    assert "/intern mine" in text
