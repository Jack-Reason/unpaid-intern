# Contributing to unpaid-intern

Thanks for helping FICSIT exploit—er, *onboard*—another intern.

## Ground rules

1. **Sanctioned path only.** Changes must work with [SML](https://ficsit.app/mod/SML) / [ficsit.app](https://ficsit.app) and must not redistribute Coffee Stain game assets, crack DRM, or require pirated clients.
2. **Stay in scope.** This repo is an LLM-powered in-game companion. Factory design advice bots with no body belong elsewhere unless they share the bridge protocol.
3. **No silent griefing.** Any action that dismantles, overwrites, or empties player property needs an explicit confirm path (see `docs/SPEC.md` §6).
4. **Secrets stay local.** Never commit API keys, `.env` files, or save-game paths with personal data.

## How we work (pre-alpha)

- Spec first: update `docs/SPEC.md` when behaviour changes.
- Prefer small PRs with a clear user-story reference (`US-1` … `US-5`).
- Match existing tone in README/chat persona docs: witty, corporate-dystopian, not cruel.

## Dev environment (when code lands)

Follow the official guide — do not invent a parallel toolchain:

- [Required Software](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/dependencies.html) (custom UE 5.6.1, VS 2022, Wwise 2025.1.6.x)
- [Starter Project](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/StarterProject/ObtainStarterProject.html)
- Keep the Starter Project / engine **out of this git repo** (see `.gitignore`)

Bridge and brain will use ordinary Python (or similar) with a localhost-only default bind.

## Pull requests

1. Fork & branch from `main`.
2. Describe the user-visible change and which acceptance criteria you touch.
3. Link any ficsit.app / Discord / docs research you relied on.
4. Wait for review before merging.

## Code of conduct (short)

Be excellent to pioneers and doggos. Harassment, hate, or "jokes" that punch down get you despawned from the project.
