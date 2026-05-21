import { io } from 'socket.io-client'

const BACKEND_URL = import.meta.env.VITE_BACKEND_URL ?? 'http://localhost:8000'

// Singleton — one connection for the entire app lifetime
const socket = io(BACKEND_URL, {
  autoConnect: false,   // manually connect in useSocket hook
  reconnection: true,
  reconnectionDelay: 2000,
  reconnectionAttempts: 10,
  transports: ['websocket', 'polling'],
})

export default socket
