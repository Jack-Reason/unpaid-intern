# unpaid-intern — Reuse catalogue

**Last updated:** 2026-10-30 (Australia/Sydney) — ADR-0006 second-licence driver  
**How to read risk:** Low = copy patterns / depend via SMM; Medium = optional soft-dep or licence watch; High = licence clash, unmaintained, or deep engine fork.

Licences verified via GitHub API / ficsit.app where noted. **Always re-check before shipping a binary dependency.**

---

## Summary table

| Component | What we reuse | Licence | Maintained for 1.x / SML 3.x? | Risk | Link |
| --- | --- | --- | --- | --- | --- |
| **SatisfactoryModLoader** | Chat commands, mod subsystems, RCOs, replication guidance, Alpakit, ExampleMod | GPL-3.0 | Yes — **3.12.0** for CL 491125 (2026-06-06); repo active 2026-10 | Low (required platform) | [GitHub](https://github.com/satisfactorymodding/SatisfactoryModLoader) · [docs chat](https://docs.ficsit.app/satisfactory-modding/latest/Development/ModLoader/ChatCommands.html) · [MP/RCO](https://docs.ficsit.app/satisfactory-modding/latest/Development/Satisfactory/Multiplayer.html) |
| **SML client-only / RequiredOnRemote** | `"RequiredOnRemote": false` so Jack needs no mod; SML 3.8+ client-only vs vanilla host | (SML docs) | Yes (3.8 / 3.9 checks) | Low | [ReleaseMod Special Fields](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/ReleaseMod.html) · [3.7→3.8](https://docs.ficsit.app/satisfactory-modding/latest/Development/UpdatingFromSml37.html) |
| **Enhanced Input + AFGPlayerController** | Drive local pioneer (move/look/interact) via game input actions | Epic / FG | CSS UE 5.6.1 | Medium — pin symbols in Starter Project | Engine EnhancedInput · FG player controller |
| **AFGBuildGun / holograms** | Programmatic build-gun state + placement | FG | 1.2 | Medium — reverse carefully | FactoryGame headers in Starter Project |
| **Dual-client testing** | Quick Launch `-multiplayer`, Steam+Epic | SML docs | Yes | Low | [TestingResources](https://docs.ficsit.app/satisfactory-modding/latest/Development/TestingResources.html) |
| **satisfactorymodding/Documentation** | Canonical patterns | Content on docs.ficsit.app | Yes | Low | [GitHub](https://github.com/satisfactorymodding/Documentation) |
| **SML ExampleMod / FeatureTests** | Hybrid BP/C++ module layout, chat, replication samples | GPL-3.0 (SML tree) | Yes | Low — study, don't vendor blindly | [SML](https://github.com/satisfactorymodding/SatisfactoryModLoader) · [SMLFeatureTests](https://github.com/satisfactorymodding/SMLFeatureTests) |
| **Ficsit Remote Monitoring (FRM)** | **Patterns**: HTTP+WS localhost, auth token, `sendChatMessage` / factory queries; optionally soft-depend later for sensors | **No SPDX licence on GitHub** (porisius/FicsitRemoteMonitoring) as of 2026-10-30 | Yes — rewritten for 1.2 FGUserSettings; active 2026-10-26 | **Medium–High** for code copy (unclear licence); **Low** for API-shape inspiration | [GitHub](https://github.com/porisius/FicsitRemoteMonitoring) · [HTTP](https://docs.ficsit.app/ficsitremotemonitoring/latest/webserver.html) · [WS](https://docs.ficsit.app/ficsitremotemonitoring/latest/websockets.html) · [Open Source Examples](https://docs.ficsit.app/satisfactory-modding/latest/Development/OpenSourceExamples.html) |
| **uWebSockets (via FRM)** | Proven UE-mod HTTP/WS embedding approach | Follow upstream uWebSockets licence if we embed similarly | Used in FRM on 1.2 | Medium — prefer minimal HTTP first (our Python mock / UE HTTP) | Cited in [Open Source Examples → FRM](https://docs.ficsit.app/satisfactory-modding/latest/Development/OpenSourceExamples.html) |
| **ALWebsocketLib + Spawn Mob With Websocket** | External process → spawn creature over WS; proves out-of-process control | Check mod page / Almine backup repo (no clear SPDX in backup) | Yes — v1.0.2 for ≥491125 (2026-08-02); needs SML ^3.11.3 | Medium — inspiration + optional dep; Windows client-focused | [ficsit.app](https://ficsit.app/mod/SpawnMobWithWebsocket) · [ALWebsocketLib](https://ficsit.app/mod/ALWebsocketLib) · [backup source](https://github.com/Almine2/Almine_SatisfactoryMods/tree/main/SpawnMobWithWebsocket) |
| **SatisMod_MobSpawner** | Creature spawn via buildable; MP notes | No licence on GitHub | Last push 2025-06 — verify 1.2 rebuild | Medium — study spawn/persist | [GitHub](https://github.com/Solat228/SatisMod_MobSpawner) |
| **HostileScaling** | `AFGCreature` hooks, server-side subsystem, SML 3.12+ | MIT | Claims SML 3.12+ / ≥491125 | Low–Medium — creature AI hooks, not pioneer pawn | [GitHub](https://github.com/mellox/HostileScaling) |
| **KBFL ActorSpawnDescriptor** | Declarative world actor spawn | Per KMods / KBFL licence | Docs current on docs.ficsit.app | Medium — helper lib dependency | [docs](https://docs.ficsit.app/kmods/latest/kbfl/GameWorldModule/ActorSpawnDescriptor/Actos.html) |
| **FicsIt-Networks** | Factory reflection / Lua computers — **not** our brain host | GPL-3.0 | 1.2.0 on SMR (2025-09); repo active 2026-10 | **High as dependency** (GPL + wrong abstraction); Low as "how FG exposes machines" reading | [GitHub](https://github.com/Panakotta00/FicsIt-Networks) · [docs](https://docs.ficsit.app/ficsit-networks/latest/index.html) |
| **satisfactory-docs-parser** | Parse `Docs/en-US.json` → recipes/items | MIT | Updated 2026-09 | Low — offline recipe intel for brain | [GitHub](https://lunafoxfire/satisfactory-docs-parser) → https://github.com/lunafoxfire/satisfactory-docs-parser |
| **satisfactory-recipe-parser / Docs exporters** | Alternate Docs.json importers | Check each repo | Varies | Medium | [Maurdekye](https://github.com/Maurdekye/satisfactory-recipe-parser) · [dmryabov exporter](https://github.com/dmryabov/satisfactory-docs-exporter) |
| **KirkMcDonald satisfactory-calculator data** | Recipe graph reference | Apache-2.0 | Updated 2026-09 | Low for **data reference**; don't scrape live site in-mod | [GitHub](https://github.com/KirkMcDonald/satisfactory-calculator) |
| **Mindcraft** | LLM + tools + Mineflayer loop; RAG prompts | MIT | Very active (2026-10) | Low for **architecture lessons** only (MC ≠ Satisfactory) | [GitHub](https://github.com/mindcraft-bots/mindcraft) |
| **Voyager** | Curriculum + skill library + iterative feedback | MIT | Active | Low for lessons; **avoid** unrestricted code-exec skills in-game | [GitHub](https://github.com/MineDojo/Voyager) |
| **Mineflayer** | Bot API design (observe/act) | MIT | Active | Lessons only | [GitHub](https://github.com/PrismarineJS/mineflayer) |
| **Ollama** | Default local LLM runtime, OpenAI-compat + tool calling | MIT | Active | Low | [ollama.com](https://ollama.com) · [tool calling](https://docs.ollama.com/capabilities/tool-calling) |
| **llama.cpp** | Alt local runtime / `llama-server` tools | MIT | Active | Low | [GitHub](https://github.com/ggml-org/llama.cpp) |
| **OpenAI-compatible HTTP** | BYO cloud (Groq, vLLM, …) | N/A (protocol) | Ubiquitous | Low | SPEC D3 |
| **whisper.cpp / faster-whisper** | Later STT | MIT (whisper.cpp) | Active | Low (post-MVP voice) | ggml whisper.cpp |
| **Piper TTS** | Later offline TTS | MIT | Active | Low (post-MVP) | rhasspy/piper |
| **UE AIModule** (AIController, BT, StateTree, NavMesh) | Movement & job AI | Epic EULA / engine | CSS UE 5.6.1 | Medium — nav quality in factory geometry TBD | Engine AIModule + FG creature AI as reference |
| **ficsit-cli / Mod Manager** | Install & CI packaging later | Check repo licences | Yes | Low | [ficsit-cli](https://github.com/satisfactorymodding/ficsit-cli) · [SMM](https://github.com/satisfactorymodding/SatisfactoryModManager) |

---

## Top reuse picks (recommended)

1. **SML 3.12 APIs** — chat commands, subsystems, RCO vs replicated subsystem split. Non-negotiable.
2. **FRM behavioural patterns** (not vendored source until licence clarified) — loopback HTTP/WS, auth token, chat send, JSON sensors.
3. **Spawn Mob With Websocket / ALWebsocketLib** — proof that external agents can drive in-world actors; use as design reference for our tighter HTTP tool protocol.
4. **HostileScaling (MIT)** — modern SML 3.12 subsystem hook examples (reference only; body is no longer a creature pawn).
5. **Enhanced Input + AFGBuildGun** — driver actuation path (ADR-0006).
6. **Ollama tool calling** — default brain backend on Windows.
7. **Mindcraft / Voyager (lessons)** — tool allow-lists, observation grounding, skill library later; reject free-form code execution against the game.
8. **satisfactory-docs-parser (MIT)** — miner recipe / item class names for `build_miner` validation offline.

## Explicit non-picks

| Item | Why not |
| --- | --- |
| FicsIt-Networks as the brain | GPL coupling + in-game Lua VM is the wrong process boundary (ADR-0001) |
| Copying FRM sources verbatim | Licence not declared on GitHub — ask author or reimplement |
| Spawned AIController pawn as the product body | Superseded by ADR-0006 (kept only as historical research) |
| Pure screen-capture + OS key injection | Fragile; worse ToS optics; no hologram state |
| Voyager-style arbitrary code exec | Catastrophic griefing surface in a physics sandbox |
| Sharing one Epic/Steam login across two clients | Epic ToS: do not share credentials; buy a second licence |

---

## satisfactorymodding org (inventory)

Useful repos beyond SML: Documentation, SatisfactoryModManager, ficsit-cli, SMLFeatureTests, SatisfactoryItemDictionary, smr-api/frontend, satisfactory-modding-ue-action (CI). Full list via GitHub org `satisfactorymodding` (30 public repos as of research pass).
