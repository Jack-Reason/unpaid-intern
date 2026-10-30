# ADR-0002: Simplified intern pawn + AIController

- **Status:** Accepted for MVP (2026-10-30)
- **Deciders:** architect recommendation — **Jack confirm** on fidelity

## Context

Spawning creatures / mobs is proven (Mob Spawner, Spawn Mob With Websocket, HostileScaling). A full second pioneer (`AFGCharacter` + player controller + build gun parity) is a much deeper FG hook surface and multiplayer ownership problem.

## Decision

MVP intern is a **mod pawn** (player-like mesh + nameplate) possessed by a custom **`AAIController`** (Behavior Tree or StateTree for jobs). Not a second logged-in Steam/Epic pioneer. Build/mine are **mod-executed actions**, not necessarily vanilla build-gun input injection.

## Alternatives considered

| Alternative | Why deferred / rejected |
| --- | --- |
| Full FG player character puppet | High risk to schedule; replication/ownership landmines |
| Reuse FGCreature AI as-is | Wrong presentation; combat-centric |
| Soft-depend on Spawn Mob With Websocket only | Creature ≠ teammate; still need our action protocol |

## Consequences

- Pros: shippable M2–M3; clear server authority; matches "mod-spawned coworker" fiction.
- Cons: animation/build FX may look "moddy" until polished; Jack may later demand pioneer parity (revisit post-M3).
