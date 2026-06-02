import { useEffect, useState } from 'react'
import { Header } from '../components/layout/Header'
import { api } from '../services/api'
import { useGreenhouseStore } from '../store/useGreenhouseStore'

const MONO = "'JetBrains Mono',monospace"
const CARD = {
  background: '#FFFFFF',
  borderRadius: 10,
  boxShadow: '0 1px 4px rgba(0,0,0,0.06), 0 4px 12px rgba(0,0,0,0.04)',
}

const MODES = [
  {
    key: 'off',
    label: 'OFF',
    desc: 'All actuators forced off. Classifier ignored.',
    color: '#DC2626',
    bg:   'rgba(220,38,38,0.07)',
    border: 'rgba(220,38,38,0.25)',
  },
  {
    key: 'auto',
    label: 'AUTO',
    desc: 'ML classifier decides pump and fan automatically.',
    color: '#16A34A',
    bg:   'rgba(22,163,74,0.07)',
    border: 'rgba(22,163,74,0.25)',
  },
  {
    key: 'manual',
    label: 'MANUAL',
    desc: 'Custom thresholds override the classifier.',
    color: '#D97706',
    bg:   'rgba(217,119,6,0.07)',
    border: 'rgba(217,119,6,0.25)',
  },
]

const DEFAULT_THRESHOLDS = {
  temp_fan_on:     30.0,
  humidity_fan_on: 50.0,
  soil_pump_on:    25.0,
}

function SectionLabel({ children }) {
  return (
    <p style={{
      fontFamily: MONO, fontSize: 10, fontWeight: 600,
      color: '#9BB09B', letterSpacing: '0.12em',
      textTransform: 'uppercase', margin: '0 0 12px',
      paddingBottom: 8, borderBottom: '1px solid #EAEDEA',
    }}>
      {children}
    </p>
  )
}

function ThresholdSlider({ label, hint, value, min, max, step = 0.5, unit, onChange }) {
  return (
    <div style={{ marginBottom: 20 }}>
      <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: 6 }}>
        <div>
          <span style={{ fontFamily: MONO, fontSize: 11, fontWeight: 600, color: '#1A261A' }}>
            {label}
          </span>
          <span style={{ fontFamily: MONO, fontSize: 10, color: '#9BB09B', marginLeft: 8 }}>
            {hint}
          </span>
        </div>
        <span style={{
          fontFamily: MONO, fontSize: 13, fontWeight: 700, color: '#1A261A',
          minWidth: 56, textAlign: 'right',
        }}>
          {Number(value).toFixed(1)}{unit}
        </span>
      </div>
      <input
        type="range" min={min} max={max} step={step}
        value={value}
        onChange={(e) => onChange(Number(e.target.value))}
        style={{ width: '100%', accentColor: '#D97706', cursor: 'pointer', height: 4 }}
      />
      <div style={{
        display: 'flex', justifyContent: 'space-between', marginTop: 4,
        fontFamily: MONO, fontSize: 9, color: '#C0D0C0',
      }}>
        <span>{min}{unit}</span>
        <span>{max}{unit}</span>
      </div>
    </div>
  )
}

export function ControlPage() {
  const storeControl = useGreenhouseStore((s) => s.control)

  const [mode,       setMode]       = useState('auto')
  const [thresholds, setThresholds] = useState(DEFAULT_THRESHOLDS)
  const [loading,    setLoading]    = useState(false)
  const [status,     setStatus]     = useState(null)  // null | 'ok' | 'warn' | 'error'
  const [statusMsg,  setStatusMsg]  = useState('')

  // Seed from store (comes via Socket.IO) or REST on first load
  useEffect(() => {
    if (storeControl) {
      setMode(storeControl.mode)
      setThresholds({ ...DEFAULT_THRESHOLDS, ...storeControl.thresholds })
    } else {
      api.getControl()
        .then((d) => {
          setMode(d.mode)
          setThresholds({ ...DEFAULT_THRESHOLDS, ...d.thresholds })
        })
        .catch(() => {})
    }
  }, [storeControl])

  function handleThreshold(key, value) {
    setThresholds((prev) => ({ ...prev, [key]: value }))
  }

  async function handleApply() {
    setLoading(true)
    setStatus(null)
    try {
      const payload = { mode, thresholds }
      const res = await api.setControl(payload)
      if (res._mqttDown) {
        setStatus('warn')
        setStatusMsg('Config saved — MQTT offline, firmware will receive it on reconnect.')
      } else {
        setStatus('ok')
        setStatusMsg('Config applied. Firmware updated.')
      }
    } catch {
      setStatus('error')
      setStatusMsg('Failed to apply config. Check backend connection.')
    } finally {
      setLoading(false)
      setTimeout(() => setStatus(null), 4000)
    }
  }

  const activeMode = MODES.find((m) => m.key === mode)

  return (
    <div style={{ display: 'flex', flexDirection: 'column', flex: 1, minHeight: 0 }}>
      <Header title="control" />
      <main style={{ flex: 1, padding: 24, overflowY: 'auto', background: '#F5F6F8' }}>

        {/* Mode selector */}
        <section style={{ marginBottom: 28 }}>
          <SectionLabel>// actuator_mode</SectionLabel>
          <div style={{ display: 'grid', gridTemplateColumns: 'repeat(3, 1fr)', gap: 12 }}>
            {MODES.map((m) => {
              const active = mode === m.key
              return (
                <button
                  key={m.key}
                  onClick={() => setMode(m.key)}
                  style={{
                    ...CARD,
                    padding: '20px 16px',
                    border: `2px solid ${active ? m.border : 'transparent'}`,
                    background: active ? m.bg : '#FFFFFF',
                    cursor: 'pointer', textAlign: 'left',
                    transition: 'all 0.18s',
                    outline: 'none',
                  }}
                >
                  <div style={{
                    display: 'flex', alignItems: 'center', gap: 8, marginBottom: 8,
                  }}>
                    <span style={{
                      width: 8, height: 8, borderRadius: '50%',
                      background: active ? m.color : '#C0D0C0',
                      boxShadow: active ? `0 0 6px ${m.color}` : 'none',
                      transition: 'all 0.2s', flexShrink: 0,
                    }} />
                    <span style={{
                      fontFamily: MONO, fontSize: 12, fontWeight: 700,
                      color: active ? m.color : '#9BB09B',
                      letterSpacing: '0.10em',
                    }}>
                      {m.label}
                    </span>
                  </div>
                  <p style={{
                    fontFamily: "'Inter',sans-serif", fontSize: 12,
                    color: active ? '#1A261A' : '#9BB09B',
                    margin: 0, lineHeight: 1.5,
                  }}>
                    {m.desc}
                  </p>
                </button>
              )
            })}
          </div>
        </section>

        {/* Manual thresholds — only shown in manual mode */}
        {mode === 'manual' && (
          <section style={{ marginBottom: 28 }}>
            <SectionLabel>// manual_thresholds</SectionLabel>
            <div style={{ ...CARD, padding: '24px 28px' }}>
              <ThresholdSlider
                label="temp_fan_on"
                hint="fan turns ON above"
                value={thresholds.temp_fan_on}
                min={15} max={45} step={0.5} unit="°C"
                onChange={(v) => handleThreshold('temp_fan_on', v)}
              />
              <ThresholdSlider
                label="humidity_fan_on"
                hint="fan turns ON below"
                value={thresholds.humidity_fan_on}
                min={20} max={95} step={1} unit="%"
                onChange={(v) => handleThreshold('humidity_fan_on', v)}
              />
              <ThresholdSlider
                label="soil_pump_on"
                hint="pump turns ON below"
                value={thresholds.soil_pump_on}
                min={5} max={60} step={1} unit="%"
                onChange={(v) => handleThreshold('soil_pump_on', v)}
              />
            </div>
          </section>
        )}

        {/* Apply button + status */}
        <div style={{ display: 'flex', alignItems: 'center', gap: 16 }}>
          <button
            onClick={handleApply}
            disabled={loading}
            style={{
              fontFamily: MONO, fontSize: 11, fontWeight: 700,
              letterSpacing: '0.10em', textTransform: 'uppercase',
              padding: '10px 24px', borderRadius: 6,
              border: 'none',
              background: loading ? '#C0D0C0' : (activeMode?.color ?? '#16A34A'),
              color: '#FFFFFF',
              cursor: loading ? 'not-allowed' : 'pointer',
              transition: 'background 0.2s',
            }}
          >
            {loading ? 'applying...' : 'apply_config'}
          </button>

          {status && (
            <span style={{
              fontFamily: MONO, fontSize: 11,
              color: status === 'ok' ? '#16A34A' : status === 'warn' ? '#D97706' : '#DC2626',
            }}>
              {status === 'ok' ? '✓' : status === 'warn' ? '⚠' : '✕'} {statusMsg}
            </span>
          )}
        </div>

        {/* Current config summary */}
        {storeControl && (
          <div style={{ marginTop: 32 }}>
            <SectionLabel>// active_config</SectionLabel>
            <div style={{ ...CARD, padding: '16px 20px' }}>
              <pre style={{
                fontFamily: MONO, fontSize: 11, color: '#7A967A',
                margin: 0, lineHeight: 1.8,
              }}>
                {JSON.stringify(storeControl, null, 2)}
              </pre>
            </div>
          </div>
        )}

      </main>
    </div>
  )
}
