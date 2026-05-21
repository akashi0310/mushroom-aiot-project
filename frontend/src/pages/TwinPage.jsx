import { Header } from '../components/layout/Header'
import { StageBadge } from '../components/ui/StageBadge'
import { Scene } from '../components/twin/Scene'
import { useGreenhouseSnapshot } from '../hooks/useGreenhouseData'

function DataRow({ label, value, color }) {
  return (
    <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', padding: '5px 0' }}>
      <span style={{ fontFamily: "'JetBrains Mono',monospace", fontSize: 10, color: '#9BB09B', letterSpacing: '0.06em' }}>
        {label}
      </span>
      <span style={{ fontFamily: "'JetBrains Mono',monospace", fontSize: 12, fontWeight: 600, color: color ?? '#1A261A' }}>
        {value ?? '—'}
      </span>
    </div>
  )
}

function StatusDot({ active }) {
  return (
    <span style={{
      display: 'inline-block', width: 7, height: 7, borderRadius: '50%',
      background: active ? '#16A34A' : '#D0D8D0',
      boxShadow: active ? '0 0 6px rgba(22,163,74,0.5)' : 'none',
      marginRight: 6,
      transition: 'all 0.3s',
    }} />
  )
}

export function TwinPage() {
  const { environment, devices, ai } = useGreenhouseSnapshot()

  const fanOn  = devices?.fan  === true
  const mistOn = devices?.mist === true
  const temp   = environment?.temperature
  const hum    = environment?.humidity

  const tempColor = temp == null ? '#9BB09B'
    : temp > 30 ? '#DC2626'
    : temp < 22 ? '#0891B2'
    : '#16A34A'

  const humColor = hum == null ? '#9BB09B'
    : hum < 70 ? '#D97706'
    : hum > 93 ? '#0891B2'
    : '#16A34A'

  return (
    <div style={{ display: 'flex', flexDirection: 'column', flex: 1, minHeight: 0 }}>
      <Header title="3d_twin" />

      <div style={{ flex: 1, position: 'relative', overflow: 'hidden' }}>
        {/* 3D Canvas — fills entire content area */}
        <Scene environment={environment} devices={devices} ai={ai} />

        {/* Floating data panel — top right */}
        <div style={{
          position: 'absolute', top: 16, right: 16,
          background: 'rgba(255,255,255,0.88)',
          backdropFilter: 'blur(14px)',
          WebkitBackdropFilter: 'blur(14px)',
          borderRadius: 12,
          padding: '14px 16px',
          minWidth: 180,
          boxShadow: '0 4px 20px rgba(0,0,0,0.10), 0 1px 4px rgba(0,0,0,0.06)',
          pointerEvents: 'none',
        }}>
          {/* Header */}
          <p style={{
            fontFamily: "'JetBrains Mono',monospace",
            fontSize: 9, fontWeight: 700,
            color: '#9BB09B', letterSpacing: '0.14em',
            textTransform: 'uppercase',
            margin: '0 0 10px',
            paddingBottom: 8,
            borderBottom: '1px solid #EAEDEA',
          }}>
            live data
          </p>

          <DataRow label="temperature" value={temp != null ? `${temp.toFixed(1)} °C` : null} color={tempColor} />
          <DataRow label="humidity"    value={hum  != null ? `${hum.toFixed(1)} %`   : null} color={humColor}  />

          <div style={{ height: 1, background: '#EAEDEA', margin: '8px 0' }} />

          {/* Devices */}
          <div style={{ display: 'flex', flexDirection: 'column', gap: 4 }}>
            <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
              <span style={{ fontFamily: "'JetBrains Mono',monospace", fontSize: 10, color: '#9BB09B', letterSpacing: '0.06em' }}>
                cooling fan
              </span>
              <span style={{ display: 'flex', alignItems: 'center', fontFamily: "'JetBrains Mono',monospace", fontSize: 11, fontWeight: 600, color: fanOn ? '#D97706' : '#9BB09B' }}>
                <StatusDot active={fanOn} />
                {devices == null ? '—' : fanOn ? 'ON' : 'OFF'}
              </span>
            </div>
            <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
              <span style={{ fontFamily: "'JetBrains Mono',monospace", fontSize: 10, color: '#9BB09B', letterSpacing: '0.06em' }}>
                mist system
              </span>
              <span style={{ display: 'flex', alignItems: 'center', fontFamily: "'JetBrains Mono',monospace", fontSize: 11, fontWeight: 600, color: mistOn ? '#0891B2' : '#9BB09B' }}>
                <StatusDot active={mistOn} />
                {devices == null ? '—' : mistOn ? 'ON' : 'OFF'}
              </span>
            </div>
          </div>

          {ai?.stage && (
            <>
              <div style={{ height: 1, background: '#EAEDEA', margin: '8px 0' }} />
              <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', gap: 8 }}>
                <span style={{ fontFamily: "'JetBrains Mono',monospace", fontSize: 10, color: '#9BB09B', letterSpacing: '0.06em' }}>
                  growth stage
                </span>
                <StageBadge stage={ai.stage} />
              </div>
              {ai.confidence != null && (
                <div style={{ marginTop: 8 }}>
                  <div style={{ height: 3, background: '#EAEDEA', borderRadius: 2, overflow: 'hidden' }}>
                    <div style={{
                      height: '100%',
                      width: `${(ai.confidence * 100).toFixed(0)}%`,
                      background: ai.confidence > 0.9 ? '#16A34A' : ai.confidence > 0.75 ? '#D97706' : '#DC2626',
                      borderRadius: 2,
                      transition: 'width 0.5s ease',
                    }} />
                  </div>
                  <p style={{ fontFamily: "'JetBrains Mono',monospace", fontSize: 9, color: '#9BB09B', margin: '3px 0 0', textAlign: 'right' }}>
                    {(ai.confidence * 100).toFixed(0)}% confidence
                  </p>
                </div>
              )}
            </>
          )}
        </div>

        {/* Hint overlay — bottom center */}
        <div style={{
          position: 'absolute', bottom: 16, left: '50%',
          transform: 'translateX(-50%)',
          fontFamily: "'JetBrains Mono',monospace",
          fontSize: 10, color: '#9BB09B',
          background: 'rgba(255,255,255,0.7)',
          backdropFilter: 'blur(8px)',
          WebkitBackdropFilter: 'blur(8px)',
          padding: '5px 12px', borderRadius: 20,
          pointerEvents: 'none',
          letterSpacing: '0.06em',
        }}>
          drag to rotate · scroll to zoom
        </div>
      </div>
    </div>
  )
}
