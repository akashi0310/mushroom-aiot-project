from datetime import datetime, timedelta, timezone

from jose import JWTError, jwt
from passlib.context import CryptContext

from app.core.config import settings

_pwd_ctx = CryptContext(schemes=["bcrypt"], deprecated="auto")


# ─── Password ────────────────────────────────────────────────────────────────

def verify_password(plain: str, hashed: str) -> bool:
    return _pwd_ctx.verify(plain, hashed)


def hash_password(plain: str) -> str:
    return _pwd_ctx.hash(plain)


def authenticate_user(username: str, password: str) -> bool:
    if username != settings.auth_username:
        return False
    if settings.auth_password_hash:
        return verify_password(password, settings.auth_password_hash)
    # Fallback: plain-text comparison (dev mode only; set auth_password_hash in prod)
    return password == settings.auth_password


# ─── JWT ─────────────────────────────────────────────────────────────────────

def create_access_token(username: str) -> str:
    expire = datetime.now(timezone.utc) + timedelta(hours=settings.auth_token_expire_hours)
    return jwt.encode(
        {"sub": username, "exp": expire},
        settings.auth_secret_key,
        algorithm=settings.auth_algorithm,
    )


def decode_token(token: str) -> str | None:
    """Return username if token is valid, else None."""
    try:
        payload = jwt.decode(
            token,
            settings.auth_secret_key,
            algorithms=[settings.auth_algorithm],
        )
        return payload.get("sub")
    except JWTError:
        return None
