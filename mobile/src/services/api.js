const BASE = process.env.EXPO_PUBLIC_BACKEND_URL ?? 'http://localhost:8000'

async function get(path) {
  const res = await fetch(`${BASE}${path}`)
  if (!res.ok) throw new Error(`GET ${path} → ${res.status}`)
  return res.json()
}

async function post(path, body) {
  const res = await fetch(`${BASE}${path}`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(body),
  })
  if (res.status === 204) return null
  const data = await res.json()
  if (!res.ok) throw data
  return data
}

export const api = {
  environmentHistory: (limit = 200) => get(`/api/environment/history?limit=${limit}`),
  devicesHistory:     (limit = 200) => get(`/api/devices/history?limit=${limit}`),
  aiHistory:          (limit = 200) => get(`/api/ai/history?limit=${limit}`),

  getControl: ()        => get('/api/control'),
  setControl: (payload) => post('/api/control', payload).catch(e => {
    // 503 = config saved, MQTT offline — treat as warning not error
    if (e?.detail) return { _mqttDown: true, ...e }
    throw e
  }),
  sendCommand: (payload) => post('/api/control/command', payload),
}
