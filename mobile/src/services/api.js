const BASE = process.env.EXPO_PUBLIC_BACKEND_URL ?? 'http://localhost:8000'

async function get(path) {
  const res = await fetch(`${BASE}${path}`)
  if (!res.ok) throw new Error(`GET ${path} → ${res.status}`)
  return res.json()
}

export const api = {
  environmentHistory: (limit = 200) => get(`/api/environment/history?limit=${limit}`),
  devicesHistory:     (limit = 200) => get(`/api/devices/history?limit=${limit}`),
  aiHistory:          (limit = 200) => get(`/api/ai/history?limit=${limit}`),
}
