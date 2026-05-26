import { io } from 'socket.io-client'

const BASE = process.env.EXPO_PUBLIC_BACKEND_URL ?? 'http://localhost:8000'

const socket = io(BASE, { autoConnect: false, transports: ['websocket'] })

export default socket
