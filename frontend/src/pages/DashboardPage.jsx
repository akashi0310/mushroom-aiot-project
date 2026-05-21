import { Header } from '../components/layout/Header'
import { MetricCard } from '../components/ui/MetricCard'
import { StageBadge } from '../components/ui/StageBadge'
import { useGreenhouseSnapshot } from '../hooks/useGreenhouseData'

const CARD = {
  background: '#FFFFFF',
  borderRadius: 10,
  boxShadow: '0 1px 4px rgba(0,0,0,0.06), 0 4px 12px rgba(0,0,0,0.04)',
}

function SectionLabel({ children }) {
  return (
    <p style={{
      fontFamily: "'JetBrains Mono',monospace",
      fontSize: 10, fontWeight: 600,
      color: '#9BB09B',
      letterSpacing: '0.12em',
      textTransform: 'uppercase',
      margin: '0 0 12px',
      paddingBottom: 8,
      borderBottom: '1px solid #EAEDEA',
    }}>
      {children}
    </p>
  )
}

export function DashboardPage() {
  const { environment, devices, ai, lastUpdated } = useGreenhouseSnapshot()

  const tempColor = !environment ? '#1A261A'
    : environment.temperature > 30 ? '#DC2626'
    : environment.temperature < 22 ? '#0891B2'
    : '#16A34A'

  const humColor = !environment ? '#1A261A'
    : environment.humidity < 70  ? '#D97706'
    : environment.humidity > 93  ? '#0891B2'
    : '#16A34A'

  const ts = lastUpdated
    ? new Date(lastUpdated).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' })
    : null

  return (
    <div style={{ display: 'flex', flexDirection: 'column', flex: 1, minHeight: 0 }}>
      <Header title="dashboard" />
      <main style={{ flex: 1, padding: 24, overflowY: 'auto', background: '#F5F6F8' }}>

        {/* status bar */}
        <div style={{
          ...CARD,
          display: 'flex', alignItems: 'center', justifyContent: 'space-between',
          marginBottom: 28,
          padding: '10px 16px',
          fontSize: 11,
          fontFamily: "'JetBrains Mono',monospace",
        }}>
          <span style={{ color: '#9BB09B' }}>rack-1 / environment</span>
          <span style={{ color: ts ? '#16A34A' : '#C0D0C0' }}>
            {ts ? `last_update: ${ts}` : 'awaiting data...'}
          </span>
        </div>

        {/* environment */}
        <section style={{ marginBottom: 28 }}>
          <SectionLabel>// environment</SectionLabel>
          <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit,minmax(160px,1fr))', gap: 12 }}>
            <MetricCard
              label="temperature"
              value={environment?.temperature?.toFixed(1)}
              unit="°C"
              color={tempColor}
              accent={environment?.temperature > 30}
              note={environment?.temperature > 30 ? '⚠ above threshold' : environment?.temperature < 22 ? '↓ below optimal' : '✓ optimal range'}
            />
            <MetricCard
              label="humidity"
              value={environment?.humidity?.toFixed(1)}
              unit="%"
              color={humColor}
              note={environment?.humidity < 70 ? '⚠ too dry' : environment?.humidity > 93 ? '⚠ too humid' : '✓ optimal range'}
            />
          </div>
        </section>

        {/* devices */}
        <section style={{ marginBottom: 28 }}>
          <SectionLabel>// devices</SectionLabel>
          <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit,minmax(160px,1fr))', gap: 12 }}>
            <MetricCard
              label="cooling fan"
              value={devices ? (devices.fan ? 'ON' : 'OFF') : null}
              color={devices?.fan ? '#D97706' : '#9BB09B'}
              accent={devices?.fan}
              note={devices?.fan ? 'relay: active' : 'relay: idle'}
            />
            <MetricCard
              label="mist system"
              value={devices ? (devices.mist ? 'ON' : 'OFF') : null}
              color={devices?.mist ? '#0891B2' : '#9BB09B'}
              accent={devices?.mist}
              note={devices?.mist ? 'relay: active' : 'relay: idle'}
            />
          </div>
        </section>

        {/* ai */}
        <section>
          <SectionLabel>// ai_classification</SectionLabel>
          <div style={{ ...CARD, padding: '20px 24px' }}>
            <div style={{ display: 'flex', alignItems: 'center', gap: 28, flexWrap: 'wrap' }}>
              <div>
                <p style={{
                  fontFamily: "'JetBrains Mono',monospace",
                  fontSize: 10, fontWeight: 600,
                  color: '#9BB09B', letterSpacing: '0.12em',
                  textTransform: 'uppercase', margin: '0 0 8px',
                }}>
                  growth_stage
                </p>
                <StageBadge stage={ai?.stage} />
              </div>

              {ai?.confidence != null && (
                <div>
                  <p style={{
                    fontFamily: "'JetBrains Mono',monospace",
                    fontSize: 10, fontWeight: 600,
                    color: '#9BB09B', letterSpacing: '0.12em',
                    textTransform: 'uppercase', margin: '0 0 8px',
                  }}>
                    confidence
                  </p>
                  <p style={{
                    fontFamily: "'JetBrains Mono',monospace",
                    fontSize: 28, fontWeight: 700,
                    color: '#1A261A', margin: 0,
                    letterSpacing: '-0.02em',
                  }}>
                    {(ai.confidence * 100).toFixed(0)}
                    <span style={{ fontSize: 13, color: '#C0D0C0', marginLeft: 4, fontWeight: 400 }}>%</span>
                  </p>
                </div>
              )}

              {ai?.confidence != null && (
                <div style={{ flex: 1, minWidth: 160 }}>
                  <div style={{ height: 5, background: '#EAEDEA', borderRadius: 3, overflow: 'hidden' }}>
                    <div style={{
                      height: '100%',
                      width: `${(ai.confidence * 100).toFixed(0)}%`,
                      background: ai.confidence > 0.9 ? '#16A34A' : ai.confidence > 0.75 ? '#D97706' : '#DC2626',
                      borderRadius: 3,
                      transition: 'width 0.5s ease',
                    }} />
                  </div>
                </div>
              )}
            </div>
          </div>
        </section>

      </main>
    </div>
  )
}
