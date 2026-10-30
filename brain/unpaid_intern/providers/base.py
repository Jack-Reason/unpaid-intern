"""LLM provider protocol."""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Any, Protocol


@dataclass
class LLMResponse:
    text: str | None = None
    tool_calls: list[dict[str, Any]] = field(default_factory=list)
    raw: Any = None


class LLMProvider(Protocol):
    def complete(
        self,
        *,
        system: str,
        user: str,
        tools: list[dict[str, Any]] | None = None,
    ) -> LLMResponse: ...
