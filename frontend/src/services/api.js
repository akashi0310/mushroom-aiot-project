const BASE = import.meta.env.VITE_BACKEND_URL ?? 'http://localhost:8000'

async function get(path) {
  const res = await fetch(`${BASE}${path}`)
  if (!res.ok) throw new Error(`API ${path} → ${res.status}`)
  return res.json()
}

export const api = {
  health:             ()              => get('/api/health'),
  state:              ()              => get('/api/state'),
  environmentHistory: (limit = 100)  => get(`/api/environment/history?limit=${limit}`),
  devicesHistory:     (limit = 100)  => get(`/api/devices/history?limit=${limit}`),
  aiHistory:          (limit = 100)  => get(`/api/ai/history?limit=${limit}`),
}
