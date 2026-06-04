-- ============================================================
-- Mushroom AIoT — Supabase Database Setup
-- Chạy file này trong: Supabase Dashboard → SQL Editor
-- ============================================================

-- 1. Environment readings (sensor data)
CREATE TABLE IF NOT EXISTS environment_readings (
    id              BIGSERIAL PRIMARY KEY,
    timestamp       TIMESTAMPTZ NOT NULL,
    air_temperature FLOAT       NOT NULL,
    air_humidity    FLOAT       NOT NULL,
    soil_moisture   FLOAT       NOT NULL,
    created_at      TIMESTAMPTZ DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_env_timestamp
    ON environment_readings (timestamp DESC);

-- 2. Device states (fan, pump)
CREATE TABLE IF NOT EXISTS device_states (
    id         BIGSERIAL PRIMARY KEY,
    timestamp  TIMESTAMPTZ NOT NULL,
    fan        BOOLEAN     NOT NULL,
    pump       BOOLEAN     NOT NULL,
    created_at TIMESTAMPTZ DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_dev_timestamp
    ON device_states (timestamp DESC);

-- 3. AI predictions
CREATE TABLE IF NOT EXISTS ai_readings (
    id         BIGSERIAL PRIMARY KEY,
    timestamp  TIMESTAMPTZ NOT NULL,
    status     VARCHAR(20) NOT NULL,  -- healthy | warning | critical
    created_at TIMESTAMPTZ DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_ai_timestamp
    ON ai_readings (timestamp DESC);

-- ============================================================
-- Security: Row Level Security
-- Backend dùng service_role key (trong .env) → bypass RLS và ghi được
-- Anon key (public) → bị chặn hoàn toàn, không đọc/ghi được từ browser
-- ============================================================

ALTER TABLE environment_readings ENABLE ROW LEVEL SECURITY;
ALTER TABLE device_states         ENABLE ROW LEVEL SECURITY;
ALTER TABLE ai_readings            ENABLE ROW LEVEL SECURITY;

-- Chặn tất cả truy cập từ anon/authenticated role (chỉ service_role được phép)
CREATE POLICY "deny_all_environment" ON environment_readings USING (false);
CREATE POLICY "deny_all_devices"     ON device_states         USING (false);
CREATE POLICY "deny_all_ai"          ON ai_readings            USING (false);

-- QUAN TRỌNG: Đổi SUPABASE_KEY trong .env sang service_role key
-- (Supabase Dashboard → Project Settings → API → service_role secret)
-- service_role key bypass RLS nên backend vẫn ghi/đọc bình thường

-- ============================================================
-- Optional: auto-delete old records (keep last 30 days)
-- Uncomment if you want to stay within free tier limits
-- ============================================================

-- CREATE OR REPLACE FUNCTION cleanup_old_records()
-- RETURNS void LANGUAGE plpgsql AS $$
-- BEGIN
--   DELETE FROM environment_readings WHERE timestamp < NOW() - INTERVAL '30 days';
--   DELETE FROM device_states         WHERE timestamp < NOW() - INTERVAL '30 days';
--   DELETE FROM ai_readings            WHERE timestamp < NOW() - INTERVAL '30 days';
-- END;
-- $$;

-- ============================================================
-- Free tier limits reminder:
--   - 500MB database storage
--   - Unlimited API requests
--   - Data is persistent (survives restarts)
-- ============================================================
