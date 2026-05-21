import asyncio
import json
import threading
import time
from datetime import datetime, timezone

import paho.mqtt.client as mqtt

from app.core.config import settings
from app.core.store import store
from app.models.enums import MQTTStatus
from app.models.schemas import AIPayload, DevicesPayload, EnvironmentPayload

_loop: asyncio.AbstractEventLoop | None = None


def set_event_loop(loop: asyncio.AbstractEventLoop) -> None:
    global _loop
    _loop = loop


def _push_state() -> None:
    """Thread-safe: schedule a broadcast on the asyncio event loop."""
    if _loop and not _loop.is_closed():
        from app.services.broadcaster import broadcast_state
        asyncio.run_coroutine_threadsafe(broadcast_state(), _loop)


# ─── paho callbacks ───────────────────────────────────────────────────────────

def _on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        store.set_mqtt_status(MQTTStatus.connected)
        client.subscribe(settings.topic_wildcard)
        print(f"[MQTT] Connected → subscribed to {settings.topic_wildcard}")
    else:
        store.set_mqtt_status(MQTTStatus.error)
        print(f"[MQTT] Connection failed rc={rc}")


def _on_disconnect(client, userdata, rc, properties=None, reason=None):
    store.set_mqtt_status(MQTTStatus.disconnected)
    print("[MQTT] Disconnected")


def _on_message(client, userdata, msg):
    topic = msg.topic
    raw   = msg.payload.decode("utf-8")

    try:
        data = json.loads(raw)
    except json.JSONDecodeError:
        print(f"[MQTT] Bad JSON on {topic}: {raw[:80]}")
        return

    ts = datetime.now(timezone.utc)

    try:
        if topic == settings.topic_environment:
            payload = EnvironmentPayload(**data)
            store.update_environment(payload, ts)
            print(f"[ENV]  {payload.temperature}°C  {payload.humidity}%")

        elif topic == settings.topic_devices:
            payload = DevicesPayload(**data)
            store.update_devices(payload, ts)
            print(f"[DEV]  fan={payload.fan}  mist={payload.mist}")

        elif topic == settings.topic_ai:
            payload = AIPayload(**data)
            store.update_ai(payload, ts)
            print(f"[AI]   stage={payload.stage}  conf={payload.confidence}")

    except Exception as exc:
        print(f"[MQTT] Validation error on {topic}: {exc}")
        return

    _push_state()


# ─── Thread entry point ───────────────────────────────────────────────────────

def _run_mqtt():
    client = mqtt.Client(
        mqtt.CallbackAPIVersion.VERSION2,
        client_id=f"mushroom-backend-{int(time.time())}",
    )
    client.on_connect    = _on_connect
    client.on_disconnect = _on_disconnect
    client.on_message    = _on_message

    print(f"[MQTT] Connecting to {settings.mqtt_broker}:{settings.mqtt_port} ...")
    client.connect(settings.mqtt_broker, settings.mqtt_port, keepalive=60)
    client.loop_forever()


def start_mqtt_thread() -> threading.Thread:
    t = threading.Thread(target=_run_mqtt, daemon=True, name="mqtt-thread")
    t.start()
    return t
