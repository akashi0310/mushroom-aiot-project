from enum import Enum


class HealthStatus(str, Enum):
    healthy  = "healthy"
    warning  = "warning"
    critical = "critical"


class MQTTStatus(str, Enum):
    connected    = "connected"
    disconnected = "disconnected"
    error        = "error"
