# UnpaidIntern (SML mod)

Scaffold for the in-game body + loopback bridge.

## Status

**Not compiled.** These sources are a structural skeleton aligned with SML 3.12 / Satisfactory 1.2 docs. They will not build until copied into the [Satisfactory Modding Starter Project](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/StarterProject/ObtainStarterProject.html) and opened with the custom CSS Unreal Engine.

## Layout

```
UnpaidIntern/
├── UnpaidIntern.uplugin
└── Source/UnpaidIntern/
    ├── UnpaidIntern.Build.cs
    ├── Public/   UIIntern*.h
    └── Private/  UIIntern*.cpp
```

## Intended runtime behaviour

| Piece | Class | Notes |
| --- | --- | --- |
| Module | `FUnpaidInternModule` | Standard `IMPLEMENT_MODULE` |
| Settings | `UUIInternSettings` | Port 8765, follow distance, chat mode, `bFreePlacement` (dev only) |
| Body | `AUIInternCharacter` | Follow / hold / goto stubs |
| Subsystem | `UUIInternSubsystem` | Spawn beside host; dispatch `follow`,`goto`,`say`,`mine`,`build_miner` |
| Bridge | `UUIInternBridge` | **Stub** — must bind `127.0.0.1` only when implemented |
| Chat | `/intern …` | Register via `AChatCommandInstance` on Game World Module (TODO) |

## Compile on Windows (Jack's PC)

1. Install VS 2022, custom UE 5.6.1 (CSS), Wwise 2025.1.6.x per [Required Software](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/dependencies.html).
2. Obtain the Starter Project (git clone of SatisfactoryModLoader repo tagged for SML 3.12).
3. Copy this folder to `StarterProject/Mods/UnpaidIntern/` (or Alpakit Create Mod then merge sources).
4. Right-click `FactoryGame.uproject` → Generate Visual Studio project files.
5. Build **Development Editor**, open editor, create Blueprint `RootGameWorld_UnpaidIntern` (parent `GameWorldModule`, Root Module checked).
6. Create Blueprint chat command child of `AChatCommandInstance` (`intern`), wire `Execute Command` → `UUIInternSubsystem::HandleInternChatCommand`.
7. Alpakit package for Windows client; install via Satisfactory Mod Manager profile.

Materials for `build_miner` come from the **intern's inventory**. Set `bFreePlacement=true` only while developing.
