from fastapi import APIRouter, Depends

from app.api.deps import get_store
from app.core.store import AppStore
from app.models.schemas import EnvironmentPayload, HistoryResponse

router = APIRouter(prefix="/environment", tags=["environment"])


@router.get("/latest", response_model=EnvironmentPayload | None)
def get_latest(store: AppStore = Depends(get_store)):
    """Latest environment reading."""
    return store.environment


@router.get("/history", response_model=HistoryResponse)
def get_history(limit: int = 100, store: AppStore = Depends(get_store)):
    """Recent environment history (max 200 records in memory)."""
    data = list(store.history_environment)
    return HistoryResponse(data=data[-limit:], count=len(data))
