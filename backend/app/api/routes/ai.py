from fastapi import APIRouter, Depends

from app.api.deps import get_store
from app.core.store import AppStore
from app.models.schemas import AIPayload, HistoryResponse
from app.services import supabase_db

router = APIRouter(prefix="/ai", tags=["ai"])


@router.get("/latest", response_model=AIPayload | None)
def get_latest(store: AppStore = Depends(get_store)):
    """Latest AI prediction (from in-memory, always fast)."""
    return store.ai


@router.get("/history", response_model=HistoryResponse)
async def get_history(limit: int = 100):
    """AI prediction history from Supabase (persistent)."""
    data = await supabase_db.get_ai_history(limit)
    return HistoryResponse(data=data, count=len(data))
