const STAGE_CONFIG = {
  pinning:      { color: '#0891B2', bg: 'rgba(8,145,178,0.08)',   border: 'rgba(8,145,178,0.22)'   },
  growing:      { color: '#16A34A', bg: 'rgba(22,163,74,0.08)',   border: 'rgba(22,163,74,0.22)'   },
  mature:       { color: '#D97706', bg: 'rgba(217,119,6,0.08)',   border: 'rgba(217,119,6,0.22)'   },
  overgrown:    { color: '#B45309', bg: 'rgba(180,83,9,0.06)',    border: 'rgba(180,83,9,0.18)'    },
  contaminated: { color: '#DC2626', bg: 'rgba(220,38,38,0.08)',   border: 'rgba(220,38,38,0.22)'   },
}

export function StageBadge({ stage }) {
  if (!stage) return (
    <span style={{
      fontFamily: "'JetBrains Mono',monospace",
      fontSize: 11, color: '#A8C4A8',
    }}>—</span>
  )
  const cfg = STAGE_CONFIG[stage] ?? { color: '#7A967A', bg: 'rgba(122,150,122,0.08)', border: 'rgba(122,150,122,0.2)' }
  return (
    <span style={{
      display: 'inline-block',
      padding: '4px 10px',
      borderRadius: 4,
      border: `1px solid ${cfg.border}`,
      background: cfg.bg,
      color: cfg.color,
      fontFamily: "'JetBrains Mono',monospace",
      fontSize: 11, fontWeight: 600,
      letterSpacing: '0.1em',
      textTransform: 'uppercase',
    }}>
      {stage}
    </span>
  )
}
