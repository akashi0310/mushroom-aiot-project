from fastapi import APIRouter, Depends, HTTPException, status

from app.api.deps import get_store
from app.core.store import AppStore
from app.models.schemas import ControlPayload
from app.services.mqtt import publish_control

router = APIRouter(prefix="/control", tags=["control"])


@router.get("", response_model=ControlPayload)
def get_control(store: AppStore = Depends(get_store)):
    """Return current control config."""
    return store.control


@router.post("", response_model=ControlPayload)
def set_control(payload: ControlPayload, store: AppStore = Depends(get_store)):
    """
    Update control mode + thresholds.
    Config is stored immediately; published to firmware via MQTT.
    Returns 503 if MQTT is not connected (config still saved).
    """
    store.update_control(payload)

    # Broadcast updated state to all WebSocket clients
    if True:  # always broadcast so FE sees new mode instantly
        import asyncio
        from app.services.broadcaster import broadcast_state
        from app.services.mqtt import _loop
        if _loop and not _loop.is_closed():
            asyncio.run_coroutine_threadsafe(broadcast_state(), _loop)

    if not publish_control(payload):
        raise HTTPException(
            status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
            detail="Config saved but MQTT is not connected — firmware will receive it on next connection.",
        )

    return store.control
