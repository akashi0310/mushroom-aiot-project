import asyncio
from contextlib import asynccontextmanager

from fastapi import FastAPI

from app.core.config import settings
from app.services.mqtt import set_event_loop, start_mqtt_thread


@asynccontextmanager
async def lifespan(app: FastAPI):
    loop = asyncio.get_event_loop()
    set_event_loop(loop)
    start_mqtt_thread()

    print(f"\n🍄  Mushroom Backend ready")
    print(f"📡  MQTT  → {settings.mqtt_broker}:{settings.mqtt_port}")
    print(f"🔌  WS    → ws://localhost:{settings.server_port}/socket.io")
    print(f"📖  Docs  → http://localhost:{settings.server_port}/docs\n")

    yield

    print("\n[Shutdown] Goodbye 🍄")
