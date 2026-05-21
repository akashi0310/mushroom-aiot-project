from fastapi import APIRouter, Depends

from app.api.deps import get_store
from app.core.store import AppStore
from app.models.schemas import AIPayload, HistoryResponse

router = APIRouter(prefix="/ai", tags=["ai"])


@router.get("/latest", response_model=AIPayload | None)
def get_latest(store: AppStore = Depends(get_store)):
    return store.ai


@router.get("/history", response_model=HistoryResponse)
def get_history(limit: int = 100, store: AppStore = Depends(get_store)):
    data = list(store.history_ai)
    return HistoryResponse(data=data[-limit:], count=len(data))
