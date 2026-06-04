"""
Supabase persistent storage service.

Tables required (run SQL in Supabase dashboard):
    See docs/supabase_setup.sql

Strategy:
- Latest state stays in AppStore (in-memory) for fast Socket.IO broadcasting
- Every MQTT message is also written here (fire-and-forget)
- History API routes query this instead of in-memory deques
"""
from __future__ import annotations

from supabase import AsyncClient, acreate_client

_client: AsyncClient | None = None


async def init(url: str, key: str) -> None:
    global _client
    _client = await acreate_client(url, key)
    print("[Supabase] Connected")


def _db() -> AsyncClient:
    if _client is None:
        raise RuntimeError("Supabase client not initialised — call init() first")
    return _client


# ─── Writes (fire-and-forget from MQTT thread) ────────────────────────────────

async def insert_environment(data: dict) -> None:
    try:
        await _db().table("environment_readings").insert(data).execute()
    except Exception as exc:
        print(f"[Supabase] insert_environment error: {exc}")


async def insert_devices(data: dict) -> None:
    try:
        await _db().table("device_states").insert(data).execute()
    except Exception as exc:
        print(f"[Supabase] insert_devices error: {exc}")


async def insert_ai(data: dict) -> None:
    try:
        await _db().table("ai_readings").insert(data).execute()
    except Exception as exc:
        print(f"[Supabase] insert_ai error: {exc}")


# ─── Reads (called from async API routes) ────────────────────────────────────

async def get_environment_history(limit: int = 100) -> list[dict]:
    try:
        res = (
            await _db()
            .table("environment_readings")
            .select("timestamp,air_temperature,air_humidity,soil_moisture")
            .order("timestamp", desc=True)
            .limit(limit)
            .execute()
        )
        return list(reversed(res.data))
    except Exception as exc:
        print(f"[Supabase] get_environment_history error: {exc}")
        return []


async def get_devices_history(limit: int = 100) -> list[dict]:
    try:
        res = (
            await _db()
            .table("device_states")
            .select("timestamp,fan,pump")
            .order("timestamp", desc=True)
            .limit(limit)
            .execute()
        )
        return list(reversed(res.data))
    except Exception as exc:
        print(f"[Supabase] get_devices_history error: {exc}")
        return []


async def get_ai_history(limit: int = 100) -> list[dict]:
    try:
        res = (
            await _db()
            .table("ai_readings")
            .select("timestamp,status")
            .order("timestamp", desc=True)
            .limit(limit)
            .execute()
        )
        return list(reversed(res.data))
    except Exception as exc:
        print(f"[Supabase] get_ai_history error: {exc}")
        return []
