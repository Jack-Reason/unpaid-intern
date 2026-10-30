# ADR-0005: Constrained tool calling (not codegen)

- **Status:** Accepted (2026-10-30)

## Context

Voyager and Mindcraft show strong results when agents write code against Mineflayer. In Satisfactory, arbitrary code or unrestricted console would grief saves and bypass safety.

## Decision

Expose a **fixed allow-list** of JSON tools (`say`, `follow`, `hold`, `goto`, `mine`, `build_miner`, `cancel`, `observe`). Models must tool-call; free-form prose becomes `say` only. No in-game code execution sandbox in MVP.

## Alternatives considered

| Alternative | Why rejected |
| --- | --- |
| Voyager-style skill code exec | Unsafe; hard to moderate |
| FIN Lua emission | GPL + dual runtime; still sandbox hell |
| Natural language only parsed by regex | Brittle; poor i18n |

## Consequences

- Pros: testable contracts; safety reviews tractable; matches Ollama/OpenAI tools.
- Cons: new abilities need schema + mod executor work (by design).
