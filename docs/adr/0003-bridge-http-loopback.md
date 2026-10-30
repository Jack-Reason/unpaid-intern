# ADR-0003: Loopback HTTP + bearer token bridge

- **Status:** Accepted (2026-10-30)

## Context

FRM proves HTTP+WebSocket inside a Satisfactory mod (uWebSockets) with auth tokens. Spawn Mob With Websocket proves WS-driven spawning via ALWebsocketLib. We need a small, testable action/observe API.

## Decision

Ship our **own** minimal HTTP bridge on `127.0.0.1` with a bearer token:

- `GET /health`, `GET /observe`, `POST /action`
- Business failures: HTTP 200 + `{ok:false,error}`
- Optional WebSocket push later; not required for MVP
- Do **not** hard-depend on FRM at runtime for MVP

## Alternatives considered

| Alternative | Why rejected for MVP |
| --- | --- |
| Require FRM installed | Extra deps; licence ambiguity for deep integration; version skew |
| Raw TCP / shared memory | Worse debuggability; reinvent JSON framing |
| Unix domain sockets | Poor Windows story (our primary OS) |
| Bind `0.0.0.0` by default | Accidental LAN exposure |

## Consequences

- Pros: Python `httpx` + mock server already green; mirrors FRM ops familiarity.
- Cons: We maintain HTTP server code in UE (start thin; consider uWS later with clear licence).
