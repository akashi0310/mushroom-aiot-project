from enum import Enum


class AIStage(str, Enum):
    pinning      = "pinning"
    growing      = "growing"
    mature       = "mature"
    overgrown    = "overgrown"
    contaminated = "contaminated"


class MQTTStatus(str, Enum):
    connected    = "connected"
    disconnected = "disconnected"
    error        = "error"
