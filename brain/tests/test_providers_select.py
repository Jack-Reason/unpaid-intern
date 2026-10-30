
import pytest

from unpaid_intern.providers import make_provider
from unpaid_intern.providers.ollama import OllamaProvider
from unpaid_intern.providers.openai_compat import OpenAICompatProvider


def test_default_provider_is_ollama(monkeypatch):
    monkeypatch.delenv("INTERN_LLM_PROVIDER", raising=False)
    p = make_provider()
    assert isinstance(p, OllamaProvider)


def test_openai_provider_requires_key(monkeypatch):
    monkeypatch.setenv("INTERN_LLM_PROVIDER", "openai")
    monkeypatch.delenv("OPENAI_API_KEY", raising=False)
    with pytest.raises(ValueError):
        make_provider("openai")


def test_openai_provider_with_key(monkeypatch):
    monkeypatch.setenv("OPENAI_API_KEY", "sk-test")
    p = make_provider("openai")
    assert isinstance(p, OpenAICompatProvider)
