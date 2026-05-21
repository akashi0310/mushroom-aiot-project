import { BrowserRouter } from 'react-router-dom'
import { Sidebar }   from './components/layout/Sidebar'
import { useSocket } from './hooks/useSocket'
import { AppRouter } from './router'

function AppShell() {
  useSocket()
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
      <AppShell />
    </BrowserRouter>
  )
}
