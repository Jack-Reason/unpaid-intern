# Windows setup — unpaid-intern (Jack's PC)

Precise steps for compiling the **UnpaidIntern** client-side driver on Windows and running Jack (Steam) + intern (Epic) on one PC. Citations are **docs.ficsit.app** (stable) unless noted. Pin: Satisfactory **1.2** CL ≥ **491125**, SML **3.12.0**, CSS UE **5.6.1**, Wwise **2025.1.6.9117**.

**Not verified on Jack's machine yet** — steps marked **[UNVERIFIED]** need Jack's hardware/account confirmation or a live Windows run. Everything else was checked against the cited pages / GitHub release metadata on 30 Oct 2026 (Australia/Sydney).

---

## 0. Jack inputs needed before you start

| Question | Why |
| --- | --- |
| GPU model + VRAM, system RAM | Two clients + Development Editor; docs warn of **compiler out of heap space** even with 32 GB RAM ([project setup](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/project_setup.html)) |
| Free disk on the drive that will hold engine + starter + game | Docs: **30+ GB** possible once built ([dependencies](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/dependencies.html)) |
| Steam account = Jack? Epic account = intern? | Recommended dual-launcher path ([Testing Resources](https://docs.ficsit.app/satisfactory-modding/latest/Development/TestingResources.html)) |
| Epic + GitHub accounts linked for CSS engine download? | Required ([dependencies — Custom Unreal Engine](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/dependencies.html)) |
| Exact Satisfactory install paths (Steam + Epic) | Alpakit “Copy to Game Path” + Quick Launch script |

Also read the [README storefront-terms notice](../README.md#storefront-terms-read-before-you-run-this) and [ADR-0006 Risk acceptance](adr/0006-second-licence-client-driver.md#risk-acceptance).

---

## 1. Prerequisites

Source: [Required Software](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/dependencies.html).

### 1.1 Satisfactory (two paid copies)

1. Buy / install Satisfactory on **Steam** (Jack) and **Epic** (intern) — or reverse; keep one launcher per account.
2. Launch **each** copy once to the title screen so installs finish.
3. Target game version: **1.2**, CL ≥ **491125** (matches SML 3.12.0 release notes).

### 1.2 Visual Studio 2022 (not 2026)

Docs: *“This version of Satisfactory works best with Visual Studio **2022**, not the latest release version 2026.”*

1. Install VS 2022 Community (Evergreen bootstrapper for 17.14 / VS 2022 — use the link on the dependencies page, not VS 2026).
2. **Workloads** (Option 2 — manual):
   - Desktop & Mobile → **.NET Desktop Development**
   - Desktop & Mobile → **Desktop development with C++**
   - Gaming → **Game development with C++**
3. **Individual components** (search and select exactly):
   - `MSVC v143 - VS 2022 C++ x64/x86 build tools (v14.38-17.8) (Out of Support)`
   - `.NET 8.0 Runtime (Long Term Support)`
   - `.NET Framework 4.8.1 SDK`
4. Optional: import the community installer configuration file linked from the same page (Option 1).

You do **not** need to sign in to Visual Studio to build.

### 1.3 Clang cross-compile toolchain (optional for MVP)

Only needed if you package **Linux dedicated server** targets. Version: **`v25` clang-18.1.0-based** ([dependencies](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/dependencies.html)). **MVP = Windows client only** → skip unless you later need DS.

### 1.4 Custom Unreal Engine 5.6.1 (CSS + community)

1. GitHub account + link it as an Epic Games developer account ([Epic instructions linked from dependencies](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/dependencies.html)). Verify access to `https://github.com/EpicGames/UnrealEngine` (private repo, not 404).
2. Link GitHub to the modding org via **https://linker.ficsit.app/link**.
3. Download the pinned release assets from  
   **https://github.com/satisfactorymodding/UnrealEngine/releases/latest**  
   (after linking — unlinked accounts get GitHub 404):
   - `UnrealEngine-CSS-Editor-Win64.exe`
   - `UnrealEngine-CSS-Editor-Win64-1.bin`
   - `UnrealEngine-CSS-Editor-Win64-2.bin`
   - `UnrealEngine-CSS-Editor-Win64-3.bin`  
   Keep filenames **exact**; all four in one folder. Run the `.exe`. Typical install path: `C:\Program Files\Unreal Engine - CSS\`.
4. Optional: install `Engine\Extras\UnrealVS\` VSIX for VS 2022.

**[UNVERIFIED]** Exact byte sizes / asset SHA of the “latest” release shift over time; confirm the release notes still say **5.6.1-CSS** for Satisfactory 1.2 before installing. Do not use stock Epic Launcher UE.

### 1.5 Wwise 2025.1.6.9117

1. Download Audiokinetic Launcher from [wwise.com](https://www.wwise.com/) (account required).
2. Wwise → Install a new version → Major **`2025.1`** → Version **`2025.1.6.9117`** (or any build that **starts with** `2025.1.6.9117`).
3. Packages: **Authoring**, **SDK (C++)**. Deployment: **Linux**; under Microsoft → Windows → **Visual Studio 2019**, **Visual Studio 2022**, **Game Core**. Leave other preselected items alone; deselect all plugins on the next screen.
4. Do **not** click “update” buttons on the Unreal Engine tab in the launcher later.

### 1.6 Git

Install Git for Windows (or GitHub Desktop). Needed to clone the Starter Project ([clone page](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/StarterProject/StarterProjectViaClone.html)).

### 1.7 Disk space

Plan for **≥ 30 GB** free on the drive holding engine + starter project + Intermediate/Binaries after first editor compile ([dependencies](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/dependencies.html)). Prefer a short ASCII path without unicode (avoids `.pch` path errors).

### 1.8 Python 3.11+ (brain)

For the LLM brain on the same PC as the intern client: Python **3.11+** on PATH ([`scripts/Setup-Brain.ps1`](../scripts/Setup-Brain.ps1)).

---

## 2. Clone SatisfactoryModLoader at SML 3.12.0

Official clone docs say to use branch **`master`** or **`dev`** (whichever updated most recently) for “latest stable” ([StarterProjectViaClone](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/StarterProject/StarterProjectViaClone.html)).  
**This repo pins SML 3.12.0** (Satisfactory 1.2 CL#491125). Prefer the release tag:

```powershell
# Detached HEAD at the release tag is intentional for a pinned build
git clone --branch v3.12.0 --depth 1 https://github.com/satisfactorymodding/SatisfactoryModLoader.git D:\Mods\SatisfactoryModLoader
cd D:\Mods\SatisfactoryModLoader
git describe --tags   # expect v3.12.0
```

Tag verified on GitHub: `v3.12.0` → commit `1a7d2ca3…`, published **2026-06-06** (UTC), “Satisfactory 1.2 (CL#491125) support”.

**Note:** Upstream docs do not document tagging as the default clone path; pinning `v3.12.0` is unpaid-intern’s choice so the Starter Project matches the uplugin `SemVersion: ^3.12.0`. If the tag’s `FactoryGame.uproject` `EngineAssociation` is not `5.6.1-CSS`, stop and ask on Discord / re-check the Updating guide.

---

## 3. Integrate Wwise + generate project files + build Development Editor

Source: [Project Setup](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/project_setup.html).

### 3.1 Integrate Wwise

1. Open Wwise Launcher → Unreal Engine tab → **Open other** → select `FactoryGame.uproject` in the clone.
2. **Integrate Wwise in Project…** → Integration Version dropdown **All** (not Latest) → select **2025.1.6.9117**.
3. Under Wwise Project → triangle → **New project** (clear path) → **Integrate**.
4. Open the generated `*_WwiseProject.wproj` → Project Explorer → SoundBanks → right-click top folder → **Generate Soundbank(s) for all platforms** → Close.

### 3.2 Generate Visual Studio files

Right-click `FactoryGame.uproject` → **Generate Visual Studio project files**.  
If missing, PowerShell (adjust paths):

```powershell
& "C:\Program Files\Unreal Engine - CSS\Engine\Build\BatchFiles\Build.bat" `
  -projectfiles `
  -project="D:\Mods\SatisfactoryModLoader\FactoryGame.uproject" `
  -game -rocket -progress
```

Engine picker must show **`5.6.1-CSS`** (not a binary Epic build). Repair via `SetupScripts\Register.bat` or registry key `HKEY_CURRENT_USER\SOFTWARE\Epic Games\Unreal Engine\Builds` if corrupted ([project setup troubleshooting](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/project_setup.html)).

### 3.3 Build Development Editor

1. Open `FactoryGame.sln`.
2. Dismiss sign-in / unsupported-projects / vulnerabilities banners as docs allow. **Do not** enable Visual Studio Integration Tool or Unreal Engine Test Adapter.
3. Solution Configuration: **Development Editor**, Platform: **Win64**.
4. Solution Explorer → `Games` → **FactoryGame** → right-click → **Build** (and **Set as Startup Project**). Do **not** “Build Solution”.
5. Wait (can take a long time; “out of heap space” → close browsers, retry — progress persists).

### 3.4 Open Unreal Editor

Double-click `FactoryGame.uproject` or open `UnrealEditor.exe` under `Unreal Engine - CSS\Engine\Binaries\Win64\` and browse to the project. First open compiles shaders. If prompted about GeneratedSoundBanks, set Root Output Path to `<WwiseProject>/GeneratedSoundBanks` and Unreal Audio Routing to **Both Wwise and Unreal audio**.

---

## 4. Copy UnpaidIntern into `Mods/`

1. Clone or copy this repo somewhere convenient, e.g. `D:\src\unpaid-intern`.
2. Copy the folder `Mod/UnpaidIntern` into the Starter Project:

```powershell
Copy-Item -Recurse `
  D:\src\unpaid-intern\Mod\UnpaidIntern `
  D:\Mods\SatisfactoryModLoader\Mods\UnpaidIntern
```

Mod plugin files live at `<Starter>/Mods/<ModReference>/` ([Plugin Setup](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/SimpleMod/gameworldmodule.html)).

3. Confirm `UnpaidIntern.uplugin` has `"RequiredOnRemote": false` and SML dependency `^3.12.0`.
4. Close the editor if open; regenerate VS project files (step 3.2); rebuild **Development Editor** so the C++ module links.
5. Reopen the editor. Enable **Show Plugin Content**. Create a Blueprint **Game World Module** parented on `GameWorldModule`, name e.g. `RootGameWorld_UnpaidIntern`, check **Root Module**, compile + save ([same page](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/SimpleMod/gameworldmodule.html)).  
   **[UNVERIFIED]** Exact BP wiring for `/intern` chat command + subsystem registration until Jack’s first editor session with FG headers.

Alternative: Alpakit **Create Mod** wizard for a blank plugin, then replace with our sources — still land files under `Mods/UnpaidIntern/`.

---

## 5. Alpakit → intern game install

Source: [Project Setup — Setting up Alpakit](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/project_setup.html).

1. Toolbar **Alpakit Dev** (alpaca icon) or File → Alpakit Dev.
2. Dev Packaging Settings → **Windows** → Enabled → **Copy to Game Path** → pick the **intern (Epic)** Satisfactory **root** install (example shape: `C:\Program Files\Epic Games\Satisfactory\`). Find the real path via [FAQ / right-click game](https://docs.ficsit.app/satisfactory-modding/latest/faq.html).
3. Optional: **Launch Game Type** matching that install.
4. In the mod list, Alpakit **Satisfactory Mod Loader (SML)** once if SMM has not installed SML yet on the intern profile.
5. Alpakit **Unpaid Intern (UnpaidIntern)**. Shipping build is handled by Alpakit when packaging for game.
6. Confirm files under `<InternGame>\FactoryGame\Mods\UnpaidIntern\` (exact subfolder naming follows SML; if wrong, check Alpakit Log).

**MVP:** Windows **client** only — do not require Linux server packaging.

To uninstall a local Alpakit mod: delete its folder under the game `Mods` directory (SMM may not list Alpakit-copied mods).

---

## 6. Satisfactory Mod Manager (SMM)

Source: [Installing Mods / SMM](https://docs.ficsit.app/satisfactory-modding/latest/ForUsers/SatisfactoryModManager.html).

1. Launch the intern game once to the title screen.
2. Install SMM: https://smm.ficsit.app/ or  
   `winget install SatisfactoryModding.SatisfactoryModManager`
3. Select the **Epic intern** install in SMM’s Install dropdown.
4. Install **SML 3.12.x** for that profile if Alpakit did not already place it.
5. Jack’s Steam install: **do not** install UnpaidIntern (vanilla host). SML optional on Jack — not required for a `RequiredOnRemote: false` client mod.

---

## 7. Two instances: Steam (Jack) + Epic (intern)

Source: [Testing Your Mods](https://docs.ficsit.app/satisfactory-modding/latest/Development/TestingResources.html).

### Preferred: one Steam + one Epic (normal join)

Docs: *“If you own the game on both Epic and Steam you can join one client normally from the other.”*

1. Launch Jack from **Steam** (vanilla).
2. Launch intern from **Epic** (SML + UnpaidIntern).
3. Jack hosts a **private / friends** session (or load save with Session Type suitable for friends).
4. Intern joins from the session browser as a normal friend.

For local IP-only testing without friends list, use Approach A below.

### Approach A: Quick Launch / dual client + IP

1. Copy the **SFLaunch_Advanced.ps1** script from the Testing Resources page; set `$GameDirs` paths for `steam.client` and `epic.client.stable` to Jack’s and intern’s installs; set `exeName` (`FactoryGameSteam` / `FactoryGameEGS` as in the script template).
2. Recommended common args from the script template: `-EpicPortal -log -NewConsole -nosplash`. For dual-process multiplayer the script adds `-CustomConfig=` and `-Multiprocess`.
3. Example (same launcher twice) — or launch Steam and Epic separately for cross-store:

```powershell
# Two Steam copies (IP only) — configure paths first
.\SFLaunch_Advanced.ps1 -multiplayer -loadLatestSave -clientAutoJoin
```

Manual join: host loads save with Session Type **IP**; client console `open 127.0.0.1` ([Multiplayer Testing — Approach A](https://docs.ficsit.app/satisfactory-modding/latest/Development/TestingResources.html)).

Optional windowed args: `-windowed -WinX=0 -WinY=32 ResX=960 ResY=1040` (and a second set for the other window).

**[UNVERIFIED on Jack’s PC]** Exact Steam/Epic install paths, whether both branches are Stable, and whether friends-session join works without IP fallback.

---

## 8. Brain (Ollama default) via scripts

On the **intern** machine (same PC as the intern client — loopback):

```powershell
cd D:\src\unpaid-intern

# Python venv + unpaid-intern[brain]
.\scripts\Setup-Brain.ps1
.\scripts\Run-Tests.ps1

# Ollama (default provider) — install from https://ollama.com/download
# [UNVERIFIED] Jack chooses model size vs VRAM; example:
ollama pull llama3.2

$env:INTERN_LLM_PROVIDER = "ollama"   # default
# Optional override:
# $env:OLLAMA_MODEL = "llama3.2"

# Mock (no game):
.\scripts\Run-Brain.ps1 -Mode mock

# Live loop against driver bridge once implemented:
.\scripts\Run-Brain.ps1 -Mode loop -Bridge http://127.0.0.1:8765
```

OpenAI path (optional): set `INTERN_LLM_PROVIDER=openai` and `OPENAI_API_KEY` as in root README.

Bridge must bind **127.0.0.1 only** + shared token (ADR-0003). Stub today — live HTTP lands with the compiled driver.

---

## 9. Smoke-test checklist

Work through in order. Tick only what you actually saw.

| # | Check | Pass? |
| --- | --- | --- |
| 1 | VS 2022 workloads + MSVC **v14.38-17.8** + .NET 8 + FW 4.8.1 SDK installed | |
| 2 | Engine shows as **5.6.1-CSS**; Wwise integrate + soundbanks OK | |
| 3 | `FactoryGame` **Development Editor** build succeeded | |
| 4 | Editor opens Starter Project; UnpaidIntern visible under Plugins | |
| 5 | Root GameWorld module saved; uplugin `RequiredOnRemote: false` | |
| 6 | Alpakit packaged UnpaidIntern into **Epic intern** install | |
| 7 | SMM shows SML on intern profile; Jack Steam install has **no** UnpaidIntern | |
| 8 | Two clients run; intern joins Jack private/friends (or `127.0.0.1`) | |
| 9 | Intern pioneer is a normal remote player on Jack’s screen | |
| 10 | `Setup-Brain.ps1` + `Run-Tests.ps1` green; `Run-Brain.ps1 -Mode mock` OK | |
| 11 | Ollama installed; model pulled; brain can answer a mock tick | |
| 12 | *(Later M2)* bridge `http://127.0.0.1:8765` observe/action; chat order → follow | |

---

## 10. Unverified / Jack-only items (summary)

| Item | Status |
| --- | --- |
| Jack GPU / RAM adequacy for 2× client + editor | **Needs Jack** |
| Exact Steam + Epic install directories | **Needs Jack** |
| Epic↔GitHub link + successful CSS engine asset download | **Needs Jack** (private release; 404 if unlinked) |
| “Latest” engine release still 5.6.1-CSS when Jack downloads | **Re-check day-of** |
| First editor session: FG header symbol names, `/intern` BP command | **Not run** (no Windows compile in CI) |
| Live Alpakit copy path layout under FactoryGame\Mods | **Not run** |
| Friends-session join Steam↔Epic on Jack’s network | **Needs Jack** |
| Ollama model choice vs VRAM | **Needs Jack** |
| Live bridge smoke (observe/action) | **Blocked on compiled driver** |

---

## Cite index

| Topic | URL |
| --- | --- |
| Required software | https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/dependencies.html |
| Obtain / clone starter | https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/StarterProject/ObtainStarterProject.html |
| Clone details | https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/StarterProject/StarterProjectViaClone.html |
| Project setup (Wwise, VS, Alpakit) | https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/project_setup.html |
| Plugin / GameWorld module | https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/SimpleMod/gameworldmodule.html |
| Testing / dual client | https://docs.ficsit.app/satisfactory-modding/latest/Development/TestingResources.html |
| SMM install | https://docs.ficsit.app/satisfactory-modding/latest/ForUsers/SatisfactoryModManager.html |
| RequiredOnRemote | https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/ReleaseMod.html |
| SML 3.12.0 release | https://github.com/satisfactorymodding/SatisfactoryModLoader/releases/tags/v3.12.0 |
| Engine linker | https://linker.ficsit.app/link |
