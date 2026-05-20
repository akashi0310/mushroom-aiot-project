import { Header } from '../components/layout/Header'
import { StageBadge } from '../components/ui/StageBadge'
import { useGreenhouseSnapshot } from '../hooks/useGreenhouseData'

const CARD = {
  background: '#FFFFFF',
  borderRadius: 10,
  boxShadow: '0 1px 4px rgba(0,0,0,0.06), 0 4px 12px rgba(0,0,0,0.04)',
}

export function TwinPage() {
  const { environment, devices, ai } = useGreenhouseSnapshot()

  return (
    <div style={{ display: 'flex', flexDirection: 'column', flex: 1, minHeight: 0 }}>
      <Header title="3d_twin" />
      <main style={{
        flex: 1, display: 'flex', alignItems: 'center', justifyContent: 'center',
        padding: 24, background: '#F5F6F8',
      }}>
        <div style={{ width: '100%', maxWidth: 520 }}>

          <div style={{ ...CARD, padding: '48px 32px', textAlign: 'center', marginBottom: 16 }}>
            <p style={{
              fontFamily: "'JetBrains Mono',monospace",
              fontSize: 10, fontWeight: 600,
              color: '#C0D0C0', letterSpacing: '0.12em',
              textTransform: 'uppercase', marginBottom: 12,
            }}>
              // three.js · phase_2
            </p>
            <p style={{
              fontFamily: "'JetBrains Mono',monospace",
              fontSize: 22, fontWeight: 700,
              color: '#16A34A', marginBottom: 10,
              letterSpacing: '-0.02em',
            }}>
              R3F scene pending_
            </p>
            <p style={{ fontFamily: "'Inter',sans-serif", fontSize: 13, color: '#9BB09B', lineHeight: 1.6 }}>
              Socket data is live and ready to drive the 3D scene.
              Drop React Three Fiber components here.
            </p>
          </div>

          <div style={{ ...CARD, overflow: 'hidden' }}>
            <div style={{
              padding: '8px 16px', borderBottom: '1px solid #EAEDEA',
              background: '#FAFBFC',
              fontFamily: "'JetBrains Mono',monospace",
              fontSize: 10, fontWeight: 600,
              color: '#9BB09B', letterSpacing: '0.12em',
              textTransform: 'uppercase',
            }}>
              live_socket_data
            </div>
            <div style={{
              padding: '16px',
              fontFamily: "'JetBrains Mono',monospace",
              fontSize: 12, color: '#486448',
              lineHeight: 1.9,
            }}>
              {[
                {
                  key: 'temperature',
                  val: environment?.temperature != null ? `${environment.temperature.toFixed(1)} °C` : 'null',
                  color: environment?.temperature > 30 ? '#DC2626' : '#16A34A',
                },
                {
                  key: 'humidity',
                  val: environment?.humidity != null ? `${environment.humidity.toFixed(1)} %` : 'null',
                  color: '#0891B2',
                },
                {
                  key: 'fan',
                  val: devices?.fan != null ? String(devices.fan) : 'null',
                  color: devices?.fan ? '#D97706' : '#C0D0C0',
                },
                {
                  key: 'mist',
                  val: devices?.mist != null ? String(devices.mist) : 'null',
                  color: devices?.mist ? '#0891B2' : '#C0D0C0',
                },
              ].map(({ key, val, color }) => (
                <div key={key} style={{ display: 'flex', gap: 8 }}>
                  <span style={{ color: '#C0D0C0', minWidth: 130 }}>{key}:</span>
                  <span style={{ color, fontWeight: 600 }}>{val}</span>
                </div>
              ))}
              <div style={{ display: 'flex', gap: 8, alignItems: 'center', marginTop: 4 }}>
                <span style={{ color: '#C0D0C0', minWidth: 130 }}>ai.stage:</span>
                <StageBadge stage={ai?.stage} />
              </div>
            </div>
          </div>

        </div>
      </main>
    </div>
  )
}
