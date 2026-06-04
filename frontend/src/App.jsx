import { useEffect } from 'react'
import { BrowserRouter, useLocation, useNavigate } from 'react-router-dom'
import { Sidebar }       from './components/layout/Sidebar'
import { useSocket }     from './hooks/useSocket'
import { AppRouter }     from './router'
import { useAuthStore }  from './store/useAuthStore'

function AuthGuard({ children }) {
  const token    = useAuthStore((s) => s.token)
  const navigate = useNavigate()
  const location = useLocation()

  useEffect(() => {
    if (!token && location.pathname !== '/login') {
      navigate('/login', { replace: true })
    }
  }, [token, location.pathname, navigate])

  return children
}

function AppShell() {
  const token = useAuthStore((s) => s.token)
  useSocket()

  if (!token) {
    // Render only the router (login page) without sidebar
    return <AppRouter />
  }

  return (
    <div style={{ display: 'flex', minHeight: '100vh', background: '#F5F6F8', color: '#1A261A' }}>
      <Sidebar />
      <div style={{ display: 'flex', flexDirection: 'column', flex: 1, minWidth: 0 }}>
        <AppRouter />
      </div>
    </div>
  )
}

export default function App() {
  return (
    <BrowserRouter>
      <AuthGuard>
        <AppShell />
      </AuthGuard>
    </BrowserRouter>
  )
}
