import { lazy, Suspense } from 'react'
import { Route, Routes } from 'react-router-dom'
import { DashboardPage }   from './pages/DashboardPage'
import { DevicesPage }     from './pages/DevicesPage'
import { EnvironmentPage } from './pages/EnvironmentPage'

// Three.js is large — load only when user visits the twin page
const TwinPage = lazy(() => import('./pages/TwinPage').then(m => ({ default: m.TwinPage })))

function TwinFallback() {
  return (
    <div style={{
      flex: 1, display: 'flex', alignItems: 'center', justifyContent: 'center',
      background: '#F5F6F8',
      fontFamily: "'JetBrains Mono',monospace",
      fontSize: 12, color: '#9BB09B',
    }}>
      loading scene...
    </div>
  )
}

export function AppRouter() {
  return (
    <Routes>
      <Route path="/"            element={<DashboardPage />}   />
      <Route path="/environment" element={<EnvironmentPage />} />
      <Route path="/devices"     element={<DevicesPage />}     />
      <Route path="/twin"        element={
        <Suspense fallback={<TwinFallback />}>
          <TwinPage />
        </Suspense>
      } />
    </Routes>
  )
}
