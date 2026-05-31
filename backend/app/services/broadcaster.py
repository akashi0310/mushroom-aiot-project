import socketio

from app.core.store import store

# Socket.IO server — mounted into FastAPI in main.py
sio = socketio.AsyncServer(async_mode="asgi", cors_allowed_origins="*")


async def broadcast_state() -> None:
    """Emit latest state to all connected Socket.IO clients."""
    await sio.emit("state", store.to_state_dict())


@sio.event
async def connect(sid: str, environ: dict, auth=None):
    print(f"[WS] Client connected: {sid}")
    await sio.emit("state", store.to_state_dict(), to=sid)


@sio.event
async def disconnect(sid: str):
    print(f"[WS] Client disconnected: {sid}")
