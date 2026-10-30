"""LLM providers."""

from __future__ import annotations

import os

from unpaid_intern.providers.base import LLMProvider, LLMResponse
from unpaid_intern.providers.ollama import OllamaProvider
from unpaid_intern.providers.openai_compat import OpenAICompatProvider


def make_provider(name: str | None = None) -> LLMProvider:
    """Select provider: 'ollama' (default) or 'openai' / 'openai_compat'."""
    chosen = (name or os.getenv("INTERN_LLM_PROVIDER", "ollama")).lower().strip()
    if chosen in {"openai", "openai_compat", "cloud"}:
        return OpenAICompatProvider()
    return OllamaProvider()


__all__ = [
    "LLMProvider",
    "LLMResponse",
    "OllamaProvider",
    "OpenAICompatProvider",
    "make_provider",
]
