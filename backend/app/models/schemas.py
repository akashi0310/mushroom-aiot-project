from datetime import datetime, timezone
from typing import Optional

from pydantic import AliasChoices, BaseModel, Field, field_validator

from app.models.enums import AIStage, MQTTStatus


# ─── Incoming MQTT payloads ───────────────────────────────────────────────────

class EnvironmentPayload(BaseModel):
    """
    Accepts both legacy firmware format {temperature, humidity}
    and new SensorData struct {air_temperature, air_humidity, soil_moisture, timestamp}.
    """
    # Falls back to server time when device does not send timestamp
    timestamp: datetime = Field(default_factory=lambda: datetime.now(timezone.utc))

    air_temperature: float = Field(
        ..., ge=-20, le=80, description="°C",
        validation_alias=AliasChoices("air_temperature", "temperature"),
    )
    air_humidity: float = Field(
        ..., ge=0, le=100, description="%",
        validation_alias=AliasChoices("air_humidity", "humidity"),
    )
    # Optional until firmware sends it
    soil_moisture: float = Field(default=0.0, ge=0, le=100, description="%")

    @field_validator("timestamp", mode="before")
    @classmethod
    def _parse_unix_ts(cls, v):
        if isinstance(v, (int, float)):
            return datetime.fromtimestamp(v, tz=timezone.utc)
        return v


class DevicesPayload(BaseModel):
    fan:  bool
    mist: bool


class AIPayload(BaseModel):
    stage:      AIStage
    confidence: float = Field(..., ge=0, le=1)


# ─── API responses ────────────────────────────────────────────────────────────

# timestamp is now part of the payload itself
EnvironmentRecord = EnvironmentPayload


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
