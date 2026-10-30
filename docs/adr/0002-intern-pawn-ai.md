# ADR-0002: Simplified intern pawn + AIController

- **Status:** **Superseded** by [ADR-0006](0006-second-licence-client-driver.md) (2026-10-30)
- **Deciders:** architect recommendation — Jack later chose second-licence pioneer

## Context

Spawning creatures / mobs is proven (Mob Spawner, Spawn Mob With Websocket, HostileScaling). A full second pioneer (`AFGCharacter` + player controller + build gun parity) is a much deeper FG hook surface and multiplayer ownership problem.

## Decision (historical)

MVP intern was planned as a **mod pawn** (player-like mesh + nameplate) possessed by a custom **`AAIController`**. Not a second logged-in Steam/Epic pioneer. Build/mine were **mod-executed actions**, not vanilla build-gun input injection.

## Why superseded

Jack decided (2026-10-30) the intern runs on a **second game licence** as a real pioneer joining Jack's session. A **client-side driver mod** injects PlayerController / Enhanced Input on the intern's own client. See ADR-0006.

## Alternatives considered (at the time)

| Alternative | Why deferred / rejected then |
| --- | --- |
| Full FG player character puppet | High risk to schedule; replication/ownership landmines |
| Reuse FGCreature AI as-is | Wrong presentation; combat-centric |
| Soft-depend on Spawn Mob With Websocket only | Creature ≠ teammate; still need our action protocol |

## Consequences (historical)

- Pros: shippable M2–M3; clear server authority; matches "mod-spawned coworker" fiction.
- Cons: animation/build FX may look "moddy"; Jack later demanded pioneer parity → ADR-0006.
