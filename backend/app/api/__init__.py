from fastapi import APIRouter

from app.api.routes import ai, devices, environment, health

api_router = APIRouter(prefix="/api")
api_router.include_router(health.router)
api_router.include_router(environment.router)
api_router.include_router(devices.router)
api_router.include_router(ai.router)
