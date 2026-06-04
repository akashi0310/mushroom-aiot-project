import { io } from 'socket.io-client'
import { getToken } from '../store/useAuthStore'

const BASE = process.env.EXPO_PUBLIC_BACKEND_URL ?? 'http://localhost:8000'

const socket = io(BASE, {
  autoConnect: false,
  transports: ['websocket', 'polling'],
  reconnection: true,
  reconnectionDelay: 2000,
  reconnectionAttempts: 10,
  auth: (cb) => cb({ token: `Bearer ${getToken() ?? ''}` }),
})

export default socket
