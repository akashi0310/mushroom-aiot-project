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

  getControl: () => get('/api/control'),
  setControl: (payload) =>
    fetch(`${BASE}/api/control`, {
      method: 'POST', headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(payload),
    }).then(r => {
      if (r.status === 503) return r.json().then(d => ({ _mqttDown: true, ...d }))
      if (!r.ok) throw new Error(`POST /api/control → ${r.status}`)
      return r.json()
    }),

  sendCommand: (payload) =>
    fetch(`${BASE}/api/control/command`, {
      method: 'POST', headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(payload),
    }).then(r => {
      if (!r.ok) return r.json().then(d => Promise.reject(d))
    }),
}
