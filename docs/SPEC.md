# unpaid-intern — Product Spec (pre-alpha)

**Last updated:** 2026-10-30 (Australia/Sydney)  
**Status:** living draft — **second-licence client driver (ADR-0006)**  
**Companion docs:** [ARCHITECTURE.md](ARCHITECTURE.md) · [REUSE.md](REUSE.md) · [TEST-PLAN.md](TEST-PLAN.md) · [adr/](adr/)  
**Game target:** Satisfactory **1.2** (CL ≥ 491125) + **SML 3.12.0** (intern client)  
**Primary platform:** Windows 10/11 (PowerShell scripts, Windows paths). Linux dedicated server is nice-to-have only.

---

## 1. Goals

1. Ship an **open-source**, **community-distributed** (ficsit.app) companion that **is** a second player: real pioneer body on a **second paid licence**, chat, obedience, mild pushback.
2. Stay on the **Coffee Stain–endorsed community path**: SML + Mod Manager + ficsit.app on the **intern's** client only. No DRM cracks, no asset redistribution. Jack installs **nothing**.
3. Keep the **LLM outside the game process**: the driver mod exposes observations and actions; a local bridge talks to a provider-agnostic brain.
4. Character: **FICSIT's expendable unpaid intern** — witty, slightly traumatised by corporate safety posters, helpful, legally a contractor.
5. Drive the intern pioneer via **PlayerController / Enhanced Input / build-gun** injection — not screen scraping.

## 2. Non-goals (for now)

- Official Coffee Stain API / first-party mod support (does not exist yet; CSS endorses community mods — [wiki](https://satisfactory.wiki.gg/wiki/Modding)).
- A **mod-spawned** fake pioneer / AIController puppet (superseded; see ADR-0002 → ADR-0006).
- Pure OCR / OS-level keyboard bots.
- Full factory design AI, train scheduling, or base-wide automation (post-MVP).
- Combat AI, vehicle driving, or hypertube routing in MVP.
- Cloud-hosted "always-on" brains that phone home without the player's explicit config.
- Console platforms (CSS has stated console modding is not planned).
- Requiring matching mods on Jack's client.

## 3. MVP user stories & acceptance criteria

Chosen MVP: *second pioneer joins Jack, follows, chats in character, mines nodes, builds miners.*

### US-1 — Join as a real pioneer

**As** Jack, **I want** the intern to join my private session as a normal multiplayer pioneer (second licence), **so that** I have a visible teammate without installing a mod.

| # | Acceptance criterion |
| --- | --- |
| AC-1.1 | Intern client (second account/licence) joins Jack's session; pioneer body appears via vanilla MP spawn. |
| AC-1.2 | Display name / badge readable as the unpaid intern (account name or in-game nametag convention). |
| AC-1.3 | Driver mod loaded **only** on intern client (`RequiredOnRemote: false`); Jack's client has no UnpaidIntern. |
| AC-1.4 | Works with Jack as listen-host; dedicated-server both-join is M2+. |

### US-2 — Follow me

**As** Jack, **I want** the intern to follow me on command, **so that** he stays useful without micromanagement.

| # | Acceptance criterion |
| --- | --- |
| AC-2.1 | Chat or tool `follow` steers the intern pioneer (input driver) toward Jack at a configurable distance (default ~3–5 m). |
| AC-2.2 | `follow stop` / `hold` zeros movement input and holds. |
| AC-2.3 | Stuck recovery after N seconds (stop + chat; teleport-assist optional later). |
| AC-2.4 | Following does not consume Jack's inventory or build budget. |

### US-3 — Chat in character

**As** Jack, **I want** to text in Satisfactory chat and get witty in-character replies from the intern pioneer, **so that** it feels like co-op with a personality.

| # | Acceptance criterion |
| --- | --- |
| AC-3.1 | Default **command-only**: `/intern …` or address patterns forward to the brain. Toggle enables always-listen for Jack's chat. |
| AC-3.2 | Replies appear in in-game chat **as the intern pioneer** (normal chat path / SML scripted chat). |
| AC-3.3 | Persona stays on-brand; jailbreaks refused in-character. |
| AC-3.4 | Latency target: first planning step < 3 s on a local mid-tier model; hard timeout with canned "radio static". |

### US-4 — Mine a resource node

**As** Jack, **I want** to order the intern to mine a nearby node, **so that** he gathers by hand while I do other work.

| # | Acceptance criterion |
| --- | --- |
| AC-4.1 | `mine` (nearest or `node_id`) approaches via input driver and hand-mines. |
| AC-4.2 | Mined items go into the **intern pioneer's** inventory (or configurable drop-at-feet). |
| AC-4.3 | Stops on `cancel`, empty node, full inventory, or max-duration safety timer. |
| AC-4.4 | Does not mine nodes claimed/occupied by another extractor unless Jack confirms. |

### US-5 — Build a miner on a node

**As** Jack, **I want** the intern to place a Miner Mk.1 (or highest unlocked) on a valid node via the **build gun**, **so that** setup work is shared.

| # | Acceptance criterion |
| --- | --- |
| AC-5.1 | `build_miner` uses `UIBuildGunController` → `AFGBuildGun` hologram placement with unlocked recipes. |
| AC-5.2 | Consumes materials from the **intern pioneer's inventory** (dev-only free-placement flag may skip costs). |
| AC-5.3 | Fails loudly in chat if materials, unlocks, or clearance are missing. |
| AC-5.4 | Destructive overwrite requires explicit confirm. |

## 4. Action / tool API (sketch)

Unchanged wire format (brain ↔ bridge). Semantics now mean "drive local pioneer" not "command spawned pawn".

```json
{
  "id": "call_01",
  "name": "follow",
  "arguments": { "target": "host", "distance_m": 4.0 }
}
```

### 4.1 Tools (MVP)

| Tool | Args (sketch) | Effect |
| --- | --- | --- |
| `say` | `text: string` | Chat as intern pioneer |
| `follow` | `target?: "host"`, `distance_m?: number` | Input-drive follow Jack |
| `hold` | — | Zero move; hold |
| `goto` | `x,y,z` **or** `landmark_id` | Steer to point |
| `mine` | `node_id?: string`, `max_seconds?: number` | Approach + hand-mine |
| `build_miner` | `node_id?: string`, `tier?: "auto"\|1\|2\|3`, `confirm?: bool` | Build-gun place miner |
| `cancel` | `job_id?: string` | Cancel current job |
| `observe` | `radius_m?: number` | Force observation refresh |

### 4.2 Observation schema (sketch)

```json
{
  "tick": 123456,
  "session": { "game_version": "1.2", "sml": "3.12.0", "mode": "client", "topology": "second_licence" },
  "intern": {
    "id": "unpaid-intern-1",
    "location": { "x": 0, "y": 0, "z": 0, "yaw": 90 },
    "inventory": [{ "item": "Desc_OreIron_C", "count": 12 }],
    "job": { "name": "follow", "status": "running" },
    "health": 100
  },
  "host": {
    "location": { "x": 100, "y": 0, "z": 0, "yaw": 0 },
    "look_target": null
  },
  "nearby_nodes": [
    {
      "id": "ResNode_Iron_42",
      "resource": "Desc_OreIron_C",
      "purity": "normal",
      "occupied": false,
      "distance_m": 18.5
    }
  ],
  "chat_inbox": [
    { "from": "Jack", "text": "@intern mine that iron", "ts": "2026-10-30T05:00:00Z" }
  ]
}
```

## 5. LLM providers

Provider-agnostic (Ollama default; OpenAI-compatible BYO). Config in bridge env — never in the `.pak`.

## 6. Safety

| Rule | Behaviour |
| --- | --- |
| No griefing | No dismantle / attack / empty storage unless allow-listed **and** confirmed |
| Confirm destructive | Occupied-node overwrite → `confirm: true` after warning `say` |
| Localhost default | Bridge binds `127.0.0.1` on intern machine only |
| Rate limits | Max N builds / mines / chat lines per minute |
| Human override | `/intern hold`, `cancel`, `mute` always local |
| No secret exfil | Tools are the only I/O |
| ToS posture | Private/friends session; second paid licence; no public ladder farming — Jack still accepts residual automation risk (ADR-0006) |

## 7. Research notes (cited)

| Topic | Finding | Source |
| --- | --- | --- |
| Game / SML | Sat **1.2** CL#491125; SML **3.12.0** | [SML release](https://github.com/satisfactorymodding/SatisfactoryModLoader/releases/tag/v3.12.0) |
| Client-only mods | `RequiredOnRemote` default **true**; set **false**; SML 3.8+ client-only vs vanilla host; SML 3.9 bidirectional checks | [ReleaseMod](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/ReleaseMod.html), [SML 3.7→3.8](https://docs.ficsit.app/satisfactory-modding/latest/Development/UpdatingFromSml37.html), [SML 3.8→3.9](https://docs.ficsit.app/satisfactory-modding/latest/Development/UpdatingFromSml38.html) |
| Dual clients | Quick Launch `-multiplayer`; Steam+Epic dual ownership | [TestingResources](https://docs.ficsit.app/satisfactory-modding/latest/Development/TestingResources.html) |
| Launch flags | `-windowed -ResX -ResY -nosplash -noaudio` | [Wiki Launch arguments](https://satisfactory.wiki.gg/wiki/Launch_arguments) |
| SSA Automation | Bots/macros restricted | [Steam SSA](https://store.steampowered.com/subscriber_agreement/english/) |
| Epic bots / accounts | “Use bot software…automate”; no sharing credentials | [Epic ToS](https://legal.epicgames.com/en-US/epicgames/tos) |
| Full write-up | ADR-0006 | [adr/0006-…](adr/0006-second-licence-client-driver.md) |

## 8. Risks & open questions

### Top risks

1. **ToS automation clauses** — SSA + Epic bot language (ADR-0006). Mitigate: private session, paid second licence, Jack ack, transparent mod page.
2. **FG input / build-gun reverse-engineering** — Enhanced Input + `AFGBuildGun` must be wired carefully in Starter Project.
3. **Dual-client hardware** — two full clients are heavy; plan Steam+Epic or second PC.
4. **Chat addressing UX** — flat chat; `/intern` + always-listen toggle.

### Decisions

| ID | Decision |
| --- | --- |
| D1 | **Materials** for `build_miner` from the **intern pioneer's inventory**. Dev-only `bFreePlacement` never ships enabled. |
| D2 | **Chat mode** toggle: command-only (**default**) ↔ always-listen. |
| D3 | **Brain** provider-agnostic: Ollama default + BYO OpenAI-compat. |
| D4 | **Primary OS:** Windows 10/11. |
| D5 | Dedicated-server both-join deferred past listen-host MVP. |
| D6 | **Topology:** second licence + client-side driver (**ADR-0006**); supersedes spawned-pawn ADR-0002. |

## 9. Milestones

| Milestone | Deliverable | Exit criteria |
| --- | --- | --- |
| **M0 — Spec & skeleton** | README, LICENSE, SPEC, … | Done |
| **M1 — Scaffold** | Brain/mock + driver **skeleton** (`RequiredOnRemote: false`) | Tests green; mod not yet compiled |
| **M2 — Chat + join** | Intern joins Jack; chat in↔out; persona | AC-1.*, AC-3.* |
| **M3 — Mine & miner** | Input-drive mine + build-gun miner; ficsit.app prerelease | AC-4.*, AC-5.* |

## 10. Licence & trademarks

Code and docs: MIT © 2026 Jack Reason.  
Satisfactory © Coffee Stain Studios / Krafton — this project is unofficial.
