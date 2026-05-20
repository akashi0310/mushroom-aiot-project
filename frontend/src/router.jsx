import { Route, Routes } from 'react-router-dom'
import { DashboardPage }   from './pages/DashboardPage'
import { DevicesPage }     from './pages/DevicesPage'
import { EnvironmentPage } from './pages/EnvironmentPage'
import { TwinPage }        from './pages/TwinPage'

export function AppRouter() {
  return (
    <Routes>
      <Route path="/"            element={<DashboardPage />}   />
      <Route path="/environment" element={<EnvironmentPage />} />
      <Route path="/devices"     element={<DevicesPage />}     />
      <Route path="/twin"        element={<TwinPage />}        />
    </Routes>
  )
}
