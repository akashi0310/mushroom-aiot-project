from datetime import datetime
from typing import Optional

from pydantic import BaseModel, Field

from app.models.enums import AIStage, MQTTStatus


# ─── Incoming MQTT payloads ───────────────────────────────────────────────────

class EnvironmentPayload(BaseModel):
    temperature: float = Field(..., ge=-20, le=80, description="°C")
    humidity:    float = Field(..., ge=0,   le=100, description="%")


class DevicesPayload(BaseModel):
    fan:  bool
    mist: bool


class AIPayload(BaseModel):
    stage:      AIStage
    confidence: float = Field(..., ge=0, le=1)


# ─── API responses ────────────────────────────────────────────────────────────

class EnvironmentRecord(EnvironmentPayload):
    timestamp: datetime


class DevicesRecord(DevicesPayload):
    timestamp: datetime


class AIRecord(AIPayload):
    timestamp: datetime


class StateResponse(BaseModel):
    environment:  Optional[EnvironmentPayload] = None
    devices:      Optional[DevicesPayload]     = None
    ai:           Optional[AIPayload]          = None
    last_updated: Optional[datetime]           = None
    mqtt_status:  MQTTStatus                   = MQTTStatus.disconnected


class HistoryResponse(BaseModel):
    data:  list
    count: int


class HealthResponse(BaseModel):
    status:      str
    mqtt_status: MQTTStatus
    last_updated: Optional[datetime]
    history_counts: dict[str, int]
