# ADR-0006: Second licence, client-side driver mod

- **Status:** Accepted (2026-10-30, Australia/Sydney) — **supersedes ADR-0002**; residual storefront automation ToS risk accepted by owner (see Risk acceptance)
- **Deciders:** Jack Reason (product) — architect research attached

## Context

Jack decided the intern is a **real co-op pioneer** on a **second game licence**, joining Jack's session like a normal friend. A **client-side SML mod installed only on the intern's game instance** drives that pioneer via normal movement / look / interact / build-gun inputs (PlayerController / Enhanced Input injection — **not** screen scraping), reads local game state for observations, and talks to the brain over loopback. **Jack's game needs no mod.**

ADR-0002's simplified spawned pawn + AIController is therefore obsolete for the product architecture.

## Decision

1. **Body:** second Steam or Epic account owns a second Satisfactory licence; that client joins Jack's private/listen (or dedicated) session as a normal multiplayer pioneer (`AFGCharacterPlayer` + `AFGPlayerController`).
2. **Driver mod (client-only):** `UnpaidIntern` ships with `"RequiredOnRemote": false` so Jack (host/vanilla) need not install it. Intern client only.
3. **Actuation:** inject into Enhanced Input / `AFGPlayerController` (move, look, jump, interact, build-gun states / hologram confirm). Prefer the same input paths a human uses so replication and anti-cheat surface match co-op play.
4. **Sense:** local observation builder on the intern client (pose, inventory, nearby nodes, chat inbox).
5. **Orders:** chat listener reads Jack's in-game chat (and `/intern` aliases on the intern client); replies via normal / scripted chat as the intern pioneer.
6. **Brain:** unchanged process boundary (ADR-0001) — loopback HTTP + token on the **intern PC** (ADR-0003).

## Alternatives considered

| Alternative | Verdict |
| --- | --- |
| **Spawned pawn + AIController (ADR-0002)** | Superseded. Faster to prototype; wrong product (not a real teammate; weak build-gun parity). |
| **Pure screen capture + OS key injection** | Rejected. Fragile, ToS-hostile optics, no reliable build-gun hologram state, poor MP sync. |
| **Host-side puppet of a remote pawn** | Rejected. Needs host mod; ownership/replication landmines; Jack explicitly wants no mod on his client. |
| **Dedicated-server bot without a client** | Not viable for pioneer body + build gun; FG DS has no interactive pioneer. |

## Research (cited)

### A. Terms of service — automation / bots / multiple accounts

**Steam Subscriber Agreement** ([store.steampowered.com/subscriber_agreement](https://store.steampowered.com/subscriber_agreement/english/), §Online conduct, cheating and automation):

> You may not use Cheats, mods, hacks, or any other unauthorized third-party software, to modify any Subscription Marketplace process, the process of Steam account creation or otherwise in interacting with or controlling the processes or user interface of Steam Content and Services, except to the degree expressly permitted.

> **C. Automation** — You may not use any form of scripts, bots, macros, or other non-human-controlled systems (“Automation”) to interact with Content and Services on Steam in any manner, including but not limited to: Automating the Steam account creation process, Faking gameplay statistics (e.g., inflated wins or losses, XP, playtime), Earning rewards or progress without genuine user input, Participating in adjudication systems (like peer reviews or “overwatch”) through automated means…

**Epic Games Terms of Service** ([legal.epicgames.com](https://legal.epicgames.com/en-US/epicgames/tos)):

> You may not share your Epic Games Account credentials with others. You can only use your own Epic Games Account that is registered to you.

> …you agree not to: … **Use bot software or services to automate your use of Licensed Products**; …

**Coffee Stain / Krafton EULA:** No distinct public “Satisfactory EULA” with a clearer automation carve-out was found beyond Steam/Epic storefront terms and CSS’s general endorsement of the **SML/ficsit.app** community path ([wiki Modding](https://satisfactory.wiki.gg/wiki/Modding)). Treat storefront ToS as binding for each launcher copy.

**Verdict (product risk, not legal advice):**

| Factor | Reading |
| --- | --- |
| Second **paid** licence, real account, private co-op | Multiple accounts that each own the game are normal; **do not share credentials** (Epic quote). |
| Client-side **SML mod** | Same sanctioned ecosystem CSS endorses; not a cracked client. |
| **Input automation / bot** driving the second pioneer | **Red flag.** SSA “Automation” and Epic “bot software…automate” are broad. Private/friends session + unpaid OSS + no public ladder/rewards **reduces** practical enforcement risk but **does not** create a written exemption. |
| Jack decision needed | Explicit acceptance of residual ToS risk, or narrow the driver to assistive tooling with more human-in-the-loop. |

### B. SML client-only / `RequiredOnRemote`

From [Releasing A Mod — Special Fields](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/ReleaseMod.html):

> **RequiredOnRemote** — Controls if the mod is required to be on both sides in multiplayer. … This field is optional, and **defaults to `true`**. If you aren’t using this behavior, you should exclude the field.  
> **RemoteVersionRange** — A Semver range of versions accepted from the remote side… defaults to your `SemVersion` if unspecified…

From [Upgrading SML 3.7 → 3.8](https://docs.ficsit.app/satisfactory-modding/latest/Development/UpdatingFromSml37.html):

> **SML Capable of Running Client-Side Only** — It is now possible have SML installed on your client even when the server does not have SML installed. This means you can create **client-side mods that work when connected to vanilla servers**.

From [Upgrading SML 3.8 → 3.9](https://docs.ficsit.app/satisfactory-modding/latest/Development/UpdatingFromSml38.html):

> Clients will refuse to join servers that are missing mods present on the client side. The existing `RequiredOnRemote` and `RemoteVersionRange` uplugin fields control this behavior.

**Implication for Jack:** set `"RequiredOnRemote": false` on UnpaidIntern. Intern client may join Jack's **unmodded** host. Jack installs nothing. (SML itself must still be present on the intern client; host need not run SML for a client-only mod — per 3.8 note / Infinite Nudge precedent.)

### C. Driving pioneer / build gun from a mod

- Character / controller: `AFGCharacterPlayer`, `AFGPlayerController` (FactoryGame). Bind / inject via **Enhanced Input** (`UEnhancedInputLocalPlayerSubsystem`, mapping contexts; `OnPlayerInputInitialized` / `SetMappingContextBound` patterns in FG). Prefer injecting the same `UInputAction`s the game uses over raw axis hacks.
- Build gun: `AFGBuildGun` state machine + hologram placement (`DoMultiStepPlacement` / recipe + transform validation). Community mods and Docs.json recipe classes (`Desc_*`, `Recipe_*`) inform material checks; placement should go through build-gun / FG spawn paths so clearance and replication match vanilla.
- Chat: SML chat commands + scripted chat; FRM `getChatMessages` / `sendChatMessage` as behavioural precedent ([docs](https://docs.ficsit.app/satisfactory-modding/latest/Development/ModLoader/ChatCommands.html)).

Exact FG symbol names are pinned when the Starter Project headers are available on Jack's Windows PC; this ADR locks **intent** (input-path injection, not OS SendInput / OCR).

### D. Two instances on one PC vs second machine

| Approach | Notes | Cite / practice |
| --- | --- | --- |
| **Steam + Epic dual ownership** | Best one-PC story: one launcher per account, natural multiplayer join | [Testing Resources — Approach A](https://docs.ficsit.app/satisfactory-modding/latest/Development/TestingResources.html): “If you own the game on both Epic and Steam you can join one client normally from the other.” |
| **SML Quick Launch `-multiplayer`** | Two copies detached from launcher; IP session `127.0.0.1`; `-Multiprocess`, `-CustomConfig=` | Same page; Quick Launch script |
| **Sandboxie / multi-box Steam** | Fragile, ToS-grey for same-launcher double | Avoid as primary |
| **Second PC or cloud GPU VM** | Cleanest isolation; intern PC runs driver+brain; Jack's PC stays vanilla | Recommended if one box can't sustain two clients |
| **Headless / low graphics** | Client is **not** a dedicated server — still needs a windowed client. Useful flags: `-windowed -ResX=… -ResY=… -nosplash -noaudio`; UE `-nullrhi` is for DS/tools, **not** a playable pioneer client | [Launch arguments wiki](https://satisfactory.wiki.gg/wiki/Launch_arguments); TestingResources windowed examples |

**Recommendation:** Prefer **Steam copy (Jack) + Epic copy (intern)** on one strong PC for MVP, else **second PC / cloud GPU** for the intern. Do not rely on Sandboxie. Intern client: small windowed resolution; brain on same machine as intern client (loopback).

## Consequences

- Pros: true co-op pioneer; Jack installs nothing; build gun / inventory / chat are vanilla-shaped; brain architecture unchanged.
- Cons: **second licence cost**; dual-client hardware cost; **ToS automation residual risk**; driver must reverse FG input/build-gun carefully; client-only limits (no server-authoritative cheats from the mod).
- Follow-ups: pin FG API names in Starter Project; choose Steam+Epic vs 2PC (hardware); decide private-session-only distribution language on ficsit.app. ToS residual risk: **accepted** (see below).

## Supersedes

- [ADR-0002](0002-intern-pawn-ai.md) — Simplified intern pawn + AIController

## Risk acceptance

**Date:** 30 Oct 2026 (Australia/Sydney)  
**Owner:** Jack Reason  

Jack **accepted** the residual Steam Subscriber Agreement / Epic Games Terms risk that a client-side automation driver on a second paid pioneer may be read as “Automation” / “bot software” under storefront ToS. There is **no** written CSS/Steam/Epic exemption for this use case; acceptance is a product decision, not legal advice.

**Mitigations kept in force:**

1. **Separate paid licence** — intern runs on its own Steam or Epic account that owns Satisfactory; no free-to-play / shared-licence tricks.
2. **No credential sharing** — Jack and intern each use their own account credentials (Epic ToS).
3. **Private sessions only** — friends / private / IP listen sessions; not public matchmaking, ladders, or reward farming.
4. **README storefront-terms notice** — users are told to read their Steam/Epic terms before running the driver; see root `README.md`.

Distribution and ficsit.app copy must keep the same private-session framing.
