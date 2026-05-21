from app.core.store import AppStore, store


def get_store() -> AppStore:
    """Inject the global store into route handlers."""
    return store


# ─── Auth placeholder (Phase 2) ───────────────────────────────────────────────
# from fastapi import Depends, HTTPException, status
# from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials
#
# bearer = HTTPBearer()
#
# def require_auth(credentials: HTTPAuthorizationCredentials = Depends(bearer)):
#     token = credentials.credentials
#     if not verify_jwt(token):
#         raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED)
#     return token
