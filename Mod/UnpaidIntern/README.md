# UnpaidIntern (SML client-side driver)

Scaffold for the **intern client's** input driver + loopback bridge.  
**Jack's game does not install this mod** (`RequiredOnRemote: false` — see [ADR-0006](../../docs/adr/0006-second-licence-client-driver.md)).

## Status

**Not compiled.** Structural skeleton for SML 3.12 / Satisfactory 1.2. Copy into the [Starter Project](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/StarterProject/ObtainStarterProject.html) on Windows.

## Layout

```
UnpaidIntern/
├── UnpaidIntern.uplugin          # RequiredOnRemote: false
└── Source/UnpaidIntern/
    ├── UnpaidIntern.Build.cs
    ├── Public/
    │   ├── UIInternSubsystem.h   # client hub (no pawn spawn)
    │   ├── UIInputDriver.h       # Enhanced Input inject
    │   ├── UIBuildGunController.h
    │   ├── UIObservationBuilder.h
    │   ├── UIChatListener.h      # Jack's orders → brain; replies via chat
    │   ├── UIInternBridge.h
    │   └── …
    └── Private/ …
```

## Runtime pieces

| Piece | Class | Notes |
| --- | --- | --- |
| Subsystem | `UUIInternSubsystem` | Client-only; ticks input driver; dispatches actions |
| Input | `UUIInputDriver` | follow / hold / goto via movement input → Enhanced Input later |
| Build gun | `UUIBuildGunController` | `AFGBuildGun` hologram placement (stub) |
| Observe | `UUIObservationBuilder` | local state → JSON |
| Chat | `UUIChatListener` | inbox + reply as this pioneer |
| Bridge | `UUIInternBridge` | **Stub** — bind `127.0.0.1` only when implemented |

## Compile on Windows (intern PC)

1. VS 2022, CSS UE 5.6.1, Wwise per [Required Software](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/dependencies.html).
2. Starter Project for SML 3.12.
3. Copy to `Mods/UnpaidIntern/`.
4. Generate VS project → Development Editor → Blueprint GameWorldModule + `/intern` chat command.
5. Alpakit **Windows client** only for MVP; install via SMM on the **intern** profile.
6. Confirm uplugin has `"RequiredOnRemote": false` so Jack's vanilla host accepts the join ([docs](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/ReleaseMod.html)).

Materials for `build_miner` come from the **intern pioneer's inventory**. `bFreePlacement` is dev-only.
