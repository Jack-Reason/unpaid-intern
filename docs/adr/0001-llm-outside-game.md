# ADR-0001: LLM runs outside the game process

- **Status:** Accepted (2026-10-30)
- **Deciders:** Jack Reason (product), architect pass

## Context

We need tool-calling LLMs (local Ollama and BYO cloud) without tying model updates to Alpakit rebuilds, and without risking game hitching on token generation.

## Decision

Run the brain as a **separate Python process**. The SML mod exposes a loopback JSON bridge (`/observe`, `/action`). The brain owns providers, persona, and planning.

## Alternatives considered

| Alternative | Why rejected |
| --- | --- |
| In-process UE LLM plugin | Huge binary; GPU driver fights; no hot-swap; poor Python ecosystem |
| FicsIt-Networks Lua as brain | GPL coupling; weak LLM tooling; wrong abstraction (factory PLC ≠ companion) |
| FRM-only + external dashboard brain | FRM is monitor-centric; licence unclear for deep fork; we need first-class actions |

## Consequences

- Pros: crash isolation; MIT brain code; provider-agnostic; testable with mock bridge.
- Cons: two processes to run; Windows UX must make "start brain" obvious; schema sync required (`bridge_protocol`).
