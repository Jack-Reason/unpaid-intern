# unpaid-intern — Product Spec (pre-alpha)

**Last updated:** 2026-10-30 (Australia/Sydney)  
**Status:** living draft  
**Game target:** Satisfactory **1.2** (CL ≥ 491125) + **SML 3.12.0**

---

## 1. Goals

1. Ship an **open-source**, **community-distributed** (ficsit.app) companion that feels like a second player: body, chat, obedience, mild pushback.
2. Stay on the **Coffee Stain–endorsed community path**: SML + Mod Manager + ficsit.app. No DRM cracks, no asset redistribution, no second-licence "fake multiplayer client."
3. Keep the **LLM outside the game process**: the mod exposes observations and actions; a local bridge talks to a provider-agnostic brain.
4. Character: **FICSIT's expendable unpaid intern** — witty, slightly traumatised by corporate safety posters, helpful, legally a contractor.

## 2. Non-goals (for now)

- Official Coffee Stain API / first-party mod support (does not exist yet; CSS endorses community mods but does not maintain them — [wiki](https://satisfactory.wiki.gg/wiki/Modding)).
- A second Steam/Epic client logged in as another pioneer.
- Full factory design AI, train scheduling, or base-wide automation (post-MVP).
- Combat AI, vehicle driving, or hypertube routing in MVP.
- Cloud-hosted "always-on" brains that phone home without the player's explicit config.
- Console platforms (CSS has stated console modding is not planned).

## 3. MVP user stories & acceptance criteria

Chosen MVP: *spawns beside player, follows, chats in character, mines nodes, builds miners.*

### US-1 — Spawn beside me

**As** the host player, **I want** the intern to appear next to me when I enable the mod / start a session, **so that** I have a visible teammate.

| # | Acceptance criterion |
| --- | --- |
| AC-1.1 | On session start (or via chat command `/intern spawn`), a pioneer-like pawn appears within ~5 m of the host, on navigable ground. |
| AC-1.2 | The body is clearly distinguishable (nameplate / colour / badge: "UNPAID INTERN"). |
| AC-1.3 | Despawn via `/intern despawn` removes the body without crashing the session. |
| AC-1.4 | Works in single-player and listen-host; dedicated-server behaviour is M2+. |

### US-2 — Follow me

**As** the player, **I want** the intern to follow me on command, **so that** he stays useful without micromanagement.

| # | Acceptance criterion |
| --- | --- |
| AC-2.1 | Chat or tool `follow` causes the intern to path-follow the host at a configurable distance (default ~3–5 m). |
| AC-2.2 | `follow stop` / `hold` stops movement and holds position. |
| AC-2.3 | The intern does not clip through major world collision; stuck recovery after N seconds (teleport-to-player fallback is acceptable for MVP). |
| AC-2.4 | Following does not consume player inventory or build budget. |

### US-3 — Chat in character

**As** the player, **I want** to text the intern in Satisfactory chat and get witty in-character replies, **so that** it feels like co-op with a personality.

| # | Acceptance criterion |
| --- | --- |
| AC-3.1 | Messages addressed to the intern (prefix `@intern` / `/intern` / configurable wake word, **or** all chat when "always listen" is on) are forwarded to the brain. |
| AC-3.2 | Replies appear in in-game chat under the intern's display name (SML supports scripted chat; FRM's [`sendChatMessage`](https://docs.ficsit.app/ficsitremotemonitoring/latest/json/Write/sendChatMessage.html) is a proven pattern). |
| AC-3.3 | Persona stays on-brand; the brain refuses out-of-character jailbreaks without breaking immersion ("HR says I can't discuss that, pioneer"). |
| AC-3.4 | Latency target: first token of reply planning < 3 s on a local mid-tier model; hard timeout with a canned "radio static" line. |

### US-4 — Mine a resource node

**As** the player, **I want** to order the intern to mine a nearby node, **so that** he gathers by hand while I do other work.

| # | Acceptance criterion |
| --- | --- |
| AC-4.1 | `mine` with an implicit "nearest valid node" or explicit node id from observations starts hand-mining. |
| AC-4.2 | Mined items go into the intern's inventory (or a configurable drop-at-feet mode). |
| AC-4.3 | Stops on `cancel`, empty node, full inventory, or max-duration safety timer. |
| AC-4.4 | Does not mine nodes claimed/occupied by another extractor unless the player confirms. |

### US-5 — Build a miner on a node

**As** the player, **I want** the intern to place a Miner Mk.1 (or highest unlocked) on a valid node, **so that** setup work is shared.

| # | Acceptance criterion |
| --- | --- |
| AC-5.1 | `build_miner` selects a valid empty node (nearest or specified) and places a miner using **the host's** unlocked recipes / build gun rules as closely as the API allows. |
| AC-5.2 | Consumes the correct materials from a configured source (host inventory **or** intern inventory — **open decision**, see §8). |
| AC-5.3 | Fails loudly in chat if materials, unlocks, or clearance are missing (no silent no-op). |
| AC-5.4 | Destructive overwrite of an existing building requires explicit confirm (see Safety). |

## 4. Action / tool API (sketch)

All actions are JSON tool calls from brain → bridge → mod. The mod returns a structured result.

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
| `say` | `text: string` | Post in-game chat as the intern |
| `follow` | `target?: "host"`, `distance_m?: number` | Start follow |
| `hold` | — | Stop moving; hold pose |
| `goto` | `x,y,z` **or** `landmark_id` | Path to point |
| `mine` | `node_id?: string`, `max_seconds?: number` | Hand-mine |
| `build_miner` | `node_id?: string`, `tier?: "auto"\|1\|2\|3`, `confirm?: bool` | Place miner |
| `cancel` | `job_id?: string` | Cancel current job |
| `observe` | `radius_m?: number` | Force observation refresh |

### 4.2 Observation schema (sketch)

Emitted periodically and on significant events:

```json
{
  "tick": 123456,
  "session": { "game_version": "1.2", "sml": "3.12.0", "mode": "listen_host" },
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

Field names should align with community conventions where practical (FRM-style `ClassName` / location objects) to ease reuse and debugging.

## 5. LLM providers

Provider-agnostic by design:

- **OpenAI-compatible HTTP** (OpenAI, Azure, Groq, Together, vLLM, …)
- **Anthropic Messages API**
- **Local**: Ollama, LM Studio, llama.cpp server

Config lives in the bridge (e.g. `.env` / YAML), never in the shipped `.pak`. Default recommendation for privacy: local model first; cloud opt-in.

System prompt includes persona, tool schemas, safety rules, and a short world primer. Context window budget: observations truncated by distance / relevance.

## 6. Safety

| Rule | Behaviour |
| --- | --- |
| No griefing | Intern never dismantles, attacks players, or empties storages unless the action is in the allow-list **and** confirmed when destructive |
| Confirm destructive | `build_miner` over occupied node, mass dismantle (future), flush inventory → require `confirm: true` after a warning `say` |
| Localhost default | Bridge binds `127.0.0.1` only unless user opts into LAN with a token (FRM-style auth is a good model) |
| Rate limits | Max N builds / mines / chat lines per minute |
| Human override | `/intern hold`, `/intern cancel`, `/intern mute` always work even if the brain is wedged |
| No secret exfil | Brain cannot read arbitrary host files; tools are the only I/O |

## 7. Research notes (cited)

| Topic | Finding | Source |
| --- | --- | --- |
| Game / SML versions | Satisfactory **1.2** CL#491125; SML **3.12.0** (2026-06-06) | [SML release](https://github.com/satisfactorymodding/SatisfactoryModLoader/releases/tag/v3.12.0), [ficsit.app/SML](https://ficsit.app/mod/SML) |
| CSS stance | Not officially supported; heavily endorsed; community SML/ficsit.app is the path | [Wiki Modding](https://satisfactory.wiki.gg/wiki/Modding), [CSS Q&A Feb 2026](https://archive.satisfactory.video/transcriptions/2026-02-17/q-a-any-native-support-for-any-of-the-circuitry-mod-features-planned/), [Snutt Mar 2024](https://archive.satisfactory.video/transcriptions/2024-03-22/official-mod-support/) |
| How mods are built | Custom **UE 5.6.1**, C++ and/or Blueprints, **Wwise 2025.1.6.x**, Alpakit, Starter Project = SML repo | [Required Software](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/dependencies.html), [C++ modding](https://docs.ficsit.app/satisfactory-modding/latest/Development/Cpp/index.html) |
| NPC / mob spawn precedent | Mob spawners; **Spawn Mob With Websocket** (ALWebsocketLib); no full co-op pioneer bot found | [ficsit.app Spawn Mob With Websocket](https://ficsit.app/mod/9RvQQM81Sbcrht), Discord help threads via Answer Overflow |
| Chat hook | SML chat commands; scripted chat sender-colour fixes in SML 3.12; FRM `getChatMessages` / `sendChatMessage` | [Chat Commands](https://docs.ficsit.app/satisfactory-modding/latest/Development/ModLoader/ChatCommands.html), [FRM sendChatMessage](https://docs.ficsit.app/ficsitremotemonitoring/latest/json/Write/sendChatMessage.html) |
| External process I/O | FRM HTTP+WS localhost:8080; FicsIt-Networks Internet Card; uWebSockets in FRM | [FRM web server](https://docs.ficsit.app/ficsitremotemonitoring/latest/webserver.html), [Open Source Examples](https://docs.ficsit.app/satisfactory-modding/latest/Development/OpenSourceExamples.html) |
| Dedicated servers | SML on Win/Linux DS; clients must match server mods | [Dedicated Server Setup](https://docs.ficsit.app/satisfactory-modding/latest/ForUsers/DedicatedServerSetup.html) |

## 8. Risks & open questions

### Top risks

1. **Controllable pioneer pawn feasibility** — Spawning creatures is proven; puppeting a full pioneer (build gun, hand mining animation, multiplayer replication) may need deep FGCharacter hooks. Mitigate: start with a simplified humanoid / reused player mesh under AI controller; escalate to full pioneer parity later.
2. **Build authority & materials** — Who pays for the miner (host inventory vs intern)? Build gun hologram validation may not expose cleanly. Mitigate: MVP can "cheat" place via mod API with an explicit config flag, then tighten to vanilla rules.
3. **Chat addressing UX** — Satisfactory chat is flat; without a reliable mention system, the brain may over-hear or miss orders. Mitigate: hard `/intern …` command channel for orders + optional free-chat persona mode.

### Open questions for Jack

See also the shortlist in the README handoff; expand here as decisions land.

| ID | Question | Options |
| --- | --- | --- |
| Q1 | Materials source for `build_miner`? | A) Host inventory B) Intern inventory C) Creative/cheat place (dev only) |
| Q2 | Always-listen chat vs `@intern` / `/intern` only? | A) Commands only (safer) B) Always listen C) Toggle |
| Q3 | Default brain for first public build? | A) Local-only (Ollama) B) BYO cloud key C) Both with local default |
| Q4 | Multiplayer ambition after MVP? | A) Listen-host only B) Dedicated server with replicated intern C) Per-client private intern |

## 9. Milestones

| Milestone | Deliverable | Exit criteria |
| --- | --- | --- |
| **M0 — Spec & skeleton** *(this commit)* | README, LICENSE, SPEC, CONTRIBUTING, .gitignore on GitHub | Repo public; docs reviewed by Jack |
| **M1 — Body & follow** | SML mod spawns pawn; follow/hold; localhost heartbeat to bridge | AC-1.*, AC-2.* in SP |
| **M2 — Chat loop** | Chat in ↔ brain ↔ chat out; persona locked; provider-agnostic brain stub | AC-3.* with a local model |
| **M3 — Mine & miner** | `mine` + `build_miner` tools; safety confirms; ficsit.app prerelease | AC-4.*, AC-5.*; tagged prerelease on SMR |

Post-M3 backlog (not scheduled): combat assist, vehicles, blueprint paste, multi-intern, FRM interop, dedicated-server hardening.

## 10. Licence & trademarks

Code and docs: MIT © 2026 Jack Reason.  
Satisfactory © Coffee Stain Studios / Krafton — this project is unofficial.
