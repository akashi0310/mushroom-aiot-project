import { NavLink } from 'react-router-dom'

const NAV = [
  { to: '/',            label: 'Dashboard',   icon: '⬡' },
  { to: '/environment', label: 'Environment', icon: '◈' },
  { to: '/devices',     label: 'Devices',     icon: '◉' },
  { to: '/twin',        label: '3D Twin',     icon: '◳' },
]

const S = {
  aside: {
    width: 224, minHeight: '100vh',
    background: '#F5F6F8',
    borderRight: 'none',
    display: 'flex', flexDirection: 'column', flexShrink: 0,
  },
  logoWrap: {
    padding: '28px 20px 22px',
    borderBottom: 'none',
  },
  logoPrefix: {
    fontFamily: "'JetBrains Mono',monospace",
    fontSize: 10, fontWeight: 600,
    color: '#A8C4A8', letterSpacing: '0.12em',
    textTransform: 'uppercase', marginBottom: 8,
  },
  logoDot: {
    display: 'inline-block',
    width: 7, height: 7, borderRadius: '50%',
    background: '#16A34A',
    boxShadow: '0 0 6px rgba(22,163,74,0.4)',
    marginRight: 8, verticalAlign: 'middle',
  },
  logoName: {
    fontFamily: "'JetBrains Mono',monospace",
    fontSize: 17, fontWeight: 700,
    color: '#1A261A', letterSpacing: '-0.01em',
    display: 'flex', alignItems: 'center',
  },
  logoSub: {
    fontFamily: "'Inter',sans-serif",
    fontSize: 11, color: '#7A967A', marginTop: 4,
  },
  nav: { padding: '14px 10px', flex: 1 },
  footer: {
    padding: '14px 20px',
    borderTop: 'none',
    fontSize: 10,
    fontFamily: "'JetBrains Mono',monospace",
    color: '#A8C4A8', letterSpacing: '0.06em',
    display: 'flex', alignItems: 'center', gap: 6,
  },
}

export function Sidebar() {
  return (
    <aside style={S.aside}>
      <div style={S.logoWrap}>
        <p style={S.logoPrefix}>~/mushroom-aiot</p>
        <p style={S.logoName}>
          <span style={S.logoDot} />
          GreenHouse
        </p>
        <p style={S.logoSub}>AIoT Platform · v0.1</p>
      </div>

      <nav style={S.nav}>
        {NAV.map(({ to, label, icon }) => (
          <NavLink key={to} to={to} end={to === '/'}
            style={({ isActive }) => ({
              display: 'flex', alignItems: 'center', gap: 10,
              padding: '9px 12px',
              borderRadius: 6,
              fontSize: 13,
              fontFamily: "'Inter',sans-serif",
              fontWeight: isActive ? 600 : 400,
              color: isActive ? '#16A34A' : '#7A967A',
              background: isActive ? '#FFFFFF' : 'transparent',
              border: 'none',
              boxShadow: isActive ? '0 1px 4px rgba(0,0,0,0.06), 0 2px 8px rgba(0,0,0,0.04)' : 'none',
              textDecoration: 'none',
              transition: 'all 0.15s',
              marginBottom: 2,
            })}
          >
            <span style={{
              fontFamily: "'JetBrains Mono',monospace",
              fontSize: 13,
              opacity: 0.7,
            }}>
              {icon}
            </span>
            {label}
          </NavLink>
        ))}
      </nav>

      <div style={S.footer}>
        <span style={{ color: '#16A34A', fontSize: 8 }}>●</span>
        rack-1 · node_01
      </div>
    </aside>
  )
}
