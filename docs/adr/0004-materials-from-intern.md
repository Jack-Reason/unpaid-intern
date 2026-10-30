# ADR-0004: Miner materials from intern inventory

- **Status:** Accepted — Jack D1 (2026-10-30)

## Context

Who pays for `build_miner` changes UX, griefing surface, and FG inventory APIs.

## Decision

Consume materials from the **intern's own inventory**. Dev-only `bFreePlacement` may skip costs locally and must never ship enabled on ficsit.app.

## Alternatives considered

| Alternative | Why rejected |
| --- | --- |
| Host inventory auto-drain | Surprising grief; MP consent issues |
| Always free place | Breaks progression; CSS/community trust risk |
| Shared "party pool" | Extra UI; defer |

## Consequences

- Pros: clear ownership; teaches the intern to mine first; aligns with companion fantasy.
- Cons: player must stock the intern (transfer UX needed soon after MVP).
