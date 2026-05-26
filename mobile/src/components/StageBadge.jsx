import { View, Text, StyleSheet } from 'react-native'

const STAGE_CONFIG = {
  pinning:      { color: '#0891B2', bg: 'rgba(8,145,178,0.12)'  },
  growing:      { color: '#16A34A', bg: 'rgba(22,163,74,0.12)'  },
  mature:       { color: '#D97706', bg: 'rgba(217,119,6,0.12)'  },
  overgrown:    { color: '#B45309', bg: 'rgba(180,83,9,0.10)'   },
  contaminated: { color: '#DC2626', bg: 'rgba(220,38,38,0.12)'  },
}

export function StageBadge({ stage }) {
  if (!stage) return <Text style={styles.empty}>—</Text>
  const cfg = STAGE_CONFIG[stage] ?? { color: '#7A967A', bg: 'rgba(122,150,122,0.10)' }
  return (
    <View style={[styles.badge, { backgroundColor: cfg.bg, borderColor: cfg.color + '55' }]}>
      <Text style={[styles.label, { color: cfg.color }]}>{stage.toUpperCase()}</Text>
    </View>
  )
}

const styles = StyleSheet.create({
  empty: { fontFamily: 'monospace', fontSize: 13, color: '#A8C4A8' },
  badge: { paddingHorizontal: 10, paddingVertical: 4, borderRadius: 5, borderWidth: 1, alignSelf: 'flex-start' },
  label: { fontFamily: 'monospace', fontSize: 11, fontWeight: '700', letterSpacing: 1 },
})
