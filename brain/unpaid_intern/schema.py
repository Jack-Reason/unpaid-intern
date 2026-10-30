"""Action / observation schemas shared with the SML bridge."""

from __future__ import annotations

from typing import Any, Literal

from pydantic import BaseModel, Field

ActionName = Literal[
    "say",
    "follow",
    "hold",
    "goto",
    "mine",
    "build_miner",
    "cancel",
    "observe",
]


class Vec3(BaseModel):
    x: float
    y: float
    z: float
    yaw: float | None = None


class InventoryItem(BaseModel):
    item: str
    count: int


class ResourceNode(BaseModel):
    id: str
    resource: str
    purity: str = "normal"
    occupied: bool = False
    distance_m: float


class ChatMessage(BaseModel):
    from_: str = Field(alias="from")
    text: str
    ts: str | None = None

    model_config = {"populate_by_name": True}


class InternState(BaseModel):
    id: str = "unpaid-intern-1"
    location: Vec3 | None = None
    inventory: list[InventoryItem] = Field(default_factory=list)
    job: str = "idle"
    health: float = 100.0


class HostState(BaseModel):
    location: Vec3 | None = None
    look_target: Vec3 | None = None


class Observation(BaseModel):
    tick: int = 0
    session: dict[str, Any] = Field(default_factory=dict)
    intern: InternState = Field(default_factory=InternState)
    host: HostState = Field(default_factory=HostState)
    nearby_nodes: list[ResourceNode] = Field(default_factory=list)
    chat_inbox: list[ChatMessage] = Field(default_factory=list)


class ActionCall(BaseModel):
    name: ActionName
    arguments: dict[str, Any] = Field(default_factory=dict)
    id: str | None = None


TOOL_SPECS: list[dict[str, Any]] = [
    {
        "type": "function",
        "function": {
            "name": "say",
            "description": "Speak in Satisfactory chat as the unpaid intern.",
            "parameters": {
                "type": "object",
                "properties": {"text": {"type": "string"}},
                "required": ["text"],
            },
        },
    },
    {
        "type": "function",
        "function": {
            "name": "follow",
            "description": "Follow the host pioneer.",
            "parameters": {
                "type": "object",
                "properties": {
                    "target": {"type": "string", "default": "host"},
                    "distance_m": {"type": "number", "default": 4.0},
                },
            },
        },
    },
    {
        "type": "function",
        "function": {
            "name": "hold",
            "description": "Stop moving and hold position.",
            "parameters": {"type": "object", "properties": {}},
        },
    },
    {
        "type": "function",
        "function": {
            "name": "goto",
            "description": "Path to a world location.",
            "parameters": {
                "type": "object",
                "properties": {
                    "x": {"type": "number"},
                    "y": {"type": "number"},
                    "z": {"type": "number"},
                },
                "required": ["x", "y", "z"],
            },
        },
    },
    {
        "type": "function",
        "function": {
            "name": "mine",
            "description": "Hand-mine a resource node (nearest if node_id omitted).",
            "parameters": {
                "type": "object",
                "properties": {
                    "node_id": {"type": "string"},
                    "max_seconds": {"type": "number"},
                },
            },
        },
    },
    {
        "type": "function",
        "function": {
            "name": "build_miner",
            "description": (
                "Place a miner on a node using the intern's own inventory materials. "
                "Set confirm=true if overwriting an occupied node."
            ),
            "parameters": {
                "type": "object",
                "properties": {
                    "node_id": {"type": "string"},
                    "tier": {"type": "string"},
                    "confirm": {"type": "boolean"},
                },
            },
        },
    },
    {
        "type": "function",
        "function": {
            "name": "cancel",
            "description": "Cancel the current job.",
            "parameters": {"type": "object", "properties": {"job_id": {"type": "string"}}},
        },
    },
]
