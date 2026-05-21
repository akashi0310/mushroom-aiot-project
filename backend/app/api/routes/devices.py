from fastapi import APIRouter, Depends

from app.api.deps import get_store
from app.core.store import AppStore
from app.models.schemas import DevicesPayload, HistoryResponse

router = APIRouter(prefix="/devices", tags=["devices"])


@router.get("/latest", response_model=DevicesPayload | None)
def get_latest(store: AppStore = Depends(get_store)):
    return store.devices


@router.get("/history", response_model=HistoryResponse)
def get_history(limit: int = 100, store: AppStore = Depends(get_store)):
    data = list(store.history_devices)
    return HistoryResponse(data=data[-limit:], count=len(data))
