"""CLI entrypoints for unpaid-intern brain."""

from __future__ import annotations

import argparse
import json
import sys
import time

from unpaid_intern.agent import InternAgent
from unpaid_intern.bridge import BridgeClient
from unpaid_intern.mock_game import MockGameServer
from unpaid_intern.providers import make_provider
from unpaid_intern.schema import ActionCall


def cmd_mock(_: argparse.Namespace) -> int:
    server = MockGameServer()
    server.start()
    try:
        bridge = BridgeClient(server.base_url)
        print(json.dumps(bridge.health(), indent=2))
        server.state.push_chat("/intern follow me")
        # Deterministic dry-run without a live LLM
        obs = bridge.observe()
        print("observe tick", obs.tick, "nodes", len(obs.nearby_nodes))
        print(bridge.action(ActionCall(name="follow", arguments={"distance_m": 4})))
        print(bridge.action(ActionCall(name="say", arguments={"text": "On it, pioneer."})))
        print(bridge.action(ActionCall(name="mine", arguments={})))
        print("inventory", obs.intern.inventory)
        # refresh
        obs2 = bridge.observe()
        print("job", obs2.intern.job, "iron", obs2.intern.inventory[0].count)
        print("mock dry-run ok")
        return 0
    finally:
        server.stop()


def cmd_tick(args: argparse.Namespace) -> int:
    bridge = BridgeClient(args.bridge)
    provider = make_provider(args.provider)
    agent = InternAgent(provider, bridge)
    result = agent.tick(args.message)
    print(json.dumps(result, indent=2, default=str))
    return 0


def cmd_loop(args: argparse.Namespace) -> int:
    bridge = BridgeClient(args.bridge)
    provider = make_provider(args.provider)
    agent = InternAgent(provider, bridge)
    print(f"looping against {args.bridge} provider={args.provider or 'default'}", flush=True)
    while True:
        try:
            result = agent.tick(None)
            print(json.dumps(result, default=str), flush=True)
        except Exception as exc:  # noqa: BLE001 — keep loop alive for operators
            print(f"tick error: {exc}", file=sys.stderr, flush=True)
        time.sleep(args.interval)
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(prog="unpaid-intern", description="Unpaid intern LLM brain")
    sub = parser.add_subparsers(dest="cmd", required=True)

    p_mock = sub.add_parser("mock", help="Run mock game server dry-run (no LLM)")
    p_mock.set_defaults(func=cmd_mock)

    p_tick = sub.add_parser("tick", help="One brain tick against a live/mock bridge")
    p_tick.add_argument("--bridge", default="http://127.0.0.1:8765")
    p_tick.add_argument("--provider", default=None)
    p_tick.add_argument("-m", "--message", default=None)
    p_tick.set_defaults(func=cmd_tick)

    p_loop = sub.add_parser("loop", help="Poll bridge forever")
    p_loop.add_argument("--bridge", default="http://127.0.0.1:8765")
    p_loop.add_argument("--provider", default=None)
    p_loop.add_argument("--interval", type=float, default=2.0)
    p_loop.set_defaults(func=cmd_loop)

    args = parser.parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    raise SystemExit(main())
