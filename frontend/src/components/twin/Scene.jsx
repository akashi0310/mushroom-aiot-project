import { useRef, useMemo, Suspense } from 'react'
import { Canvas, useFrame } from '@react-three/fiber'
import { OrbitControls, RoundedBox } from '@react-three/drei'
import * as THREE from 'three'

// ─── helpers ─────────────────────────────────────────────────────────────────

function sr(n) { const x = Math.sin(n + 1) * 10000; return x - Math.floor(x) }

const SOIL_H = 0.18

function tH(x, z) {
  return 0.074
    + Math.sin(x * 4.4 + 1.1) * Math.cos(z * 3.7 + 0.6) * 0.036
    + Math.sin(x * 8.8 + z * 6.8) * 0.014
    + Math.cos(x * 5.9 - z * 5.2) * 0.021
}

// ─── box constants ────────────────────────────────────────────────────────────

const HW = 0.96
const HH = 0.78
const HD = 0.66

// ─── terrarium base tray only — no frame edges ───────────────────────────────

function TerrariumBase() {
  return (
    <mesh position={[0, -0.026, 0]} castShadow receiveShadow>
      <boxGeometry args={[HW*2+0.10, 0.052, HD*2+0.10]} />
      <meshStandardMaterial color="#CFD8D2" metalness={0.35} roughness={0.52} />
    </mesh>
  )
}

// ─── glass + condensation drops ───────────────────────────────────────────────

function TerrariumGlass() {
  const W = HW*2, H = HH*2, D = HD*2
  const drops = useMemo(() =>
    Array.from({ length: 16 }, (_, i) => ({
      x:     (sr(i*17)     - 0.5) * W * 0.78,
      y:     (sr(i*17+1)   - 0.5) * H * 0.78,
      r:      0.010 + sr(i*17+2) * 0.020,
      left:   i % 4 === 0,
    }))
  , [W, H])

  return (
    <group position={[0, HH, 0]}>
      <RoundedBox args={[W, H, D]} radius={0.036} smoothness={4} renderOrder={1}>
        <meshStandardMaterial
          color="#C8E4F4" transparent opacity={0.07}
          roughness={0.02} metalness={0.04}
          side={THREE.DoubleSide} depthWrite={false}
        />
      </RoundedBox>
      {drops.map((d, i) => (
        <mesh key={i}
          position={d.left ? [-HW+0.002, d.y, d.x*0.65] : [d.x, d.y, HD+0.003]}
          rotation={d.left ? [0, Math.PI/2, 0] : [0,0,0]}
          renderOrder={2}
        >
          <circleGeometry args={[d.r, 7]} />
          <meshStandardMaterial color="#B8D8F0" transparent opacity={0.40} depthWrite={false} />
        </mesh>
      ))}
    </group>
  )
}

// ─── terrain — solid soil volume + displaced surface ──────────────────────────

function Terrain() {
  const sW = HW*2 - 0.01
  const sD = HD*2 - 0.01

  const geo = useMemo(() => {
    const g = new THREE.PlaneGeometry(sW, sD, 26, 20)
    g.rotateX(-Math.PI / 2)
    const p = g.attributes.position.array
    for (let i = 0; i < p.length; i += 3) p[i+1] = SOIL_H + tH(p[i], p[i+2])
    g.computeVertexNormals()
    return g
  }, [sW, sD])

  return (
    <>
      <mesh geometry={geo} receiveShadow>
        <meshStandardMaterial color="#3C2214" roughness={0.96} />
      </mesh>
      {/* solid block fills the full lower section — visible from all sides */}
      <mesh position={[0, SOIL_H*0.5, 0]}>
        <boxGeometry args={[sW, SOIL_H, sD]} />
        <meshStandardMaterial color="#2C180A" roughness={0.98} />
      </mesh>
    </>
  )
}

// ─── moss patches ─────────────────────────────────────────────────────────────

const MOSS_POS    = [[-0.55,0.12],[-0.28,-0.28],[0.15,0.25],[0.48,-0.15],[-0.72,-0.08],[0.62,0.18],[-0.10,-0.42],[0.35,0.42]]
const MOSS_COLORS = ['#3E7048','#486C50','#527A58','#3A6642']

function MossPatches() {
  return (
    <>
      {MOSS_POS.map(([x,z],i) => (
        <mesh key={i}
          position={[x, SOIL_H+tH(x,z)+0.003, z]}
          rotation={[-Math.PI/2, 0, sr(i*11+1)*Math.PI*2]}
          scale={0.7+sr(i*11)*0.7} receiveShadow
        >
          <circleGeometry args={[0.1,9]} />
          <meshStandardMaterial color={MOSS_COLORS[i%MOSS_COLORS.length]} roughness={0.96} />
        </mesh>
      ))}
    </>
  )
}

// ─── grass tufts ──────────────────────────────────────────────────────────────

const GRASS_POS = [[-0.65,0.32],[0.55,-0.38],[-0.20,0.50],[0.72,0.30],[-0.45,-0.50],[0.08,-0.55],[0.82,-0.12]]

function GrassBlade({ x, y, z, seed }) {
  const h = 0.038 + sr(seed+2)*0.028
  return (
    <mesh
      position={[x+(sr(seed+3)-0.5)*0.025, y+h/2, z+(sr(seed+4)-0.5)*0.025]}
      rotation={[(sr(seed)-0.5)*0.35, sr(seed+1)*Math.PI*2, (sr(seed+5)-0.5)*0.2]}
    >
      <coneGeometry args={[0.005, h, 4]} />
      <meshStandardMaterial color={sr(seed)>0.5 ? '#5CB068' : '#4EA85A'} roughness={0.9} />
    </mesh>
  )
}

function GrassTufts() {
  return (
    <>
      {GRASS_POS.map(([gx,gz],gi) =>
        Array.from({length:4},(_,bi) => {
          const seed = gi*50+bi*7
          return <GrassBlade key={bi} x={gx} y={SOIL_H+tH(gx,gz)+0.001} z={gz} seed={seed} />
        })
      )}
    </>
  )
}

// ─── pebbles ──────────────────────────────────────────────────────────────────

const PEBBLE_POS = [[-0.80,0.44],[0.78,-0.42],[0.60,0.52],[-0.30,0.58],[0.88,0.02],[-0.88,-0.38],[0.25,-0.58]]

function Pebbles() {
  return (
    <>
      {PEBBLE_POS.map(([px,pz],i) => (
        <mesh key={i}
          position={[px, SOIL_H+tH(px,pz)+0.020, pz]}
          scale={[(0.75+sr(i*13)*0.5)*0.038,(0.45+sr(i*13+1)*0.35)*0.038,(0.75+sr(i*13+2)*0.5)*0.038]}
          rotation={[sr(i)*Math.PI, sr(i+1)*Math.PI, 0]}
        >
          <icosahedronGeometry args={[1,1]} />
          <meshStandardMaterial color={sr(i*13+3)>0.5 ? '#B8B2AA' : '#A8A49C'} roughness={0.88} />
        </mesh>
      ))}
    </>
  )
}

// ─── chibi mushrooms ──────────────────────────────────────────────────────────

const STAGE_CFG = {
  pinning:      { cap:'#EEC8F4', stem:'#FAF0FC', baseScale:0.26, count:11, spots:false },
  growing:      { cap:'#C4ECD0', stem:'#EEF8F0', baseScale:0.58, count:8,  spots:false },
  mature:       { cap:'#F8EDD4', stem:'#FFF8EC', baseScale:1.00, count:7,  spots:true  },
  overgrown:    { cap:'#E8C882', stem:'#F8EDD4', baseScale:1.30, count:5,  spots:false },
  contaminated: { cap:'#88B460', stem:'#C8CCA0', baseScale:0.72, count:7,  spots:false },
}

function easeOutBack(t) {
  const c1 = 1.4, c3 = c1 + 1
  return 1 + c3 * Math.pow(t - 1, 3) + c1 * Math.pow(t - 1, 2)
}

function ChibiMushroom({ px, py, pz, scale=1, rotY=0, capColor, stemColor, spots, delay=0 }) {
  const sh  = 0.095 * scale
  const cr  = 0.088 * scale
  const str = 0.042 * scale
  const groupRef = useRef()
  const elapsed  = useRef(0)

  useFrame((_, dt) => {
    elapsed.current += dt
    const t     = Math.max(0, Math.min(1, (elapsed.current - delay) / 1.8))
    const eased = t < 1 ? easeOutBack(t) : 1
    if (groupRef.current) groupRef.current.scale.setScalar(Math.max(0, eased))
  })

  return (
    <group ref={groupRef} position={[px,py,pz]} rotation={[0,rotY,0]} scale={0}>
      <mesh position={[0,sh+0.002,0]}>
        <cylinderGeometry args={[cr*0.94,cr*0.94,0.004,9]} />
        <meshStandardMaterial color="#E8D8C8" roughness={0.92} />
      </mesh>
      <mesh position={[0,sh*0.5,0]} castShadow>
        <cylinderGeometry args={[str,str*1.15,sh,8]} />
        <meshStandardMaterial color={stemColor} roughness={0.82} />
      </mesh>
      <mesh position={[0,sh+cr*0.3,0]} scale={[1,0.72,1]} castShadow>
        <sphereGeometry args={[cr,11,9,0,Math.PI*2,0,Math.PI*0.84]} />
        <meshStandardMaterial color={capColor} roughness={0.62} />
      </mesh>
      {spots && [0,1,2].map(k => {
        const a=k*Math.PI*2/3+1.2, rd=cr*0.48
        return (
          <mesh key={k} position={[Math.cos(a)*rd,sh+cr*0.72,Math.sin(a)*rd]} scale={[1,0.28,1]}>
            <sphereGeometry args={[cr*0.16,6,5]} />
            <meshStandardMaterial color="#FFFFFF" roughness={0.55} />
          </mesh>
        )
      })}
    </group>
  )
}

const CLUSTER_DEFS = [{cx:-0.38,cz:0.08},{cx:0.30,cz:-0.20},{cx:-0.55,cz:-0.30},{cx:0.55,cz:0.28}]

function MushroomBed({ stage }) {
  const cfg = STAGE_CFG[stage] ?? STAGE_CFG.mature
  const placements = useMemo(() =>
    CLUSTER_DEFS.flatMap(({cx,cz},ci) =>
      Array.from({length:cfg.count},(_,i) => {
        const base = ci*200+i*7
        const x = cx+(sr(base)-0.5)*0.28
        const z = cz+(sr(base+1)-0.5)*0.18
        return { px:x, pz:z, py:SOIL_H+tH(x,z), scale:cfg.baseScale*(0.62+sr(base+2)*0.76), rotY:sr(base+3)*Math.PI*2, delay:ci*0.55+i*0.07 }
      })
    )
  // eslint-disable-next-line react-hooks/exhaustive-deps
  ,[stage])

  return (
    <>
      {placements.map((p,i) => (
        <ChibiMushroom key={`${stage}-${i}`}
          px={p.px} py={p.py} pz={p.pz}
          scale={p.scale} rotY={p.rotY}
          capColor={cfg.cap} stemColor={cfg.stem}
          spots={cfg.spots && p.scale > cfg.baseScale*0.8}
          delay={p.delay}
        />
      ))}
    </>
  )
}

// ─── grow light ───────────────────────────────────────────────────────────────

function GrowLight() {
  return (
    <group position={[0, HH*2-0.05, 0]}>
      <mesh>
        <boxGeometry args={[1.4,0.032,0.06]} />
        <meshStandardMaterial color="#FFE890" emissive="#FFE060" emissiveIntensity={1.8} roughness={0.3} />
      </mesh>
      <pointLight position={[0,-0.06,0]} color="#FFE890" intensity={2.4} distance={2.6} decay={2} />
    </group>
  )
}

// ─── mini fan ─────────────────────────────────────────────────────────────────

function MiniFan({ active }) {
  const bladesRef = useRef()
  useFrame((_,dt) => { if (bladesRef.current) bladesRef.current.rotation.z += active ? dt*8.5 : 0 })
  return (
    <group position={[HW-0.04, HH*2-0.12, -HD+0.04]} rotation={[0,-Math.PI*0.25,0]}>
      <mesh rotation={[Math.PI/2,0,0]}>
        <torusGeometry args={[0.085,0.009,7,16]} />
        <meshStandardMaterial color="#A8B0B2" metalness={0.6} roughness={0.4} />
      </mesh>
      <group ref={bladesRef}>
        {[0,1,2,3].map(k => {
          const a=k*Math.PI/2
          return (
            <mesh key={k} position={[Math.cos(a)*0.038,Math.sin(a)*0.038,0]} rotation={[0,0,a+0.3]}>
              <boxGeometry args={[0.07,0.022,0.007]} />
              <meshStandardMaterial color="#C8D0D2" metalness={0.4} roughness={0.45} />
            </mesh>
          )
        })}
        <mesh>
          <sphereGeometry args={[0.014,7,7]} />
          <meshStandardMaterial color="#909898" metalness={0.7} />
        </mesh>
      </group>
    </group>
  )
}

// ─── mist particles ───────────────────────────────────────────────────────────

function MistParticles({ active }) {
  const N = 260
  const [pos, spd, drift] = useMemo(() => {
    const p=new Float32Array(N*3), s=new Float32Array(N), d=new Float32Array(N)
    for (let i=0;i<N;i++) {
      p[i*3]   = (Math.random()-0.5)*(HW*2-0.12)
      p[i*3+1] = SOIL_H + Math.random()*(HH*2-SOIL_H-0.10)
      p[i*3+2] = (Math.random()-0.5)*(HD*2-0.12)
      s[i] = 0.06+Math.random()*0.18
      d[i] = (Math.random()-0.5)*0.04
    }
    return [p,s,d]
  },[])

  const attrRef = useRef()
  useFrame((_,dt) => {
    if (!attrRef.current || !active) return
    const a = attrRef.current.array
    for (let i=0;i<N;i++) {
      a[i*3]   += dt*drift[i]
      a[i*3+1] += dt*spd[i]
      if (a[i*3+1] > HH*2-0.08) a[i*3+1] = SOIL_H+0.05
    }
    attrRef.current.needsUpdate = true
  })

  return (
    <points visible={active}>
      <bufferGeometry>
        <bufferAttribute ref={attrRef} attach="attributes-position" count={N} array={pos} itemSize={3} />
      </bufferGeometry>
      <pointsMaterial color="#D0E8F8" size={0.016} transparent opacity={0.45} sizeAttenuation depthWrite={false} />
    </points>
  )
}

// ─── scene assembly ───────────────────────────────────────────────────────────

function SceneContent({ environment, devices, ai }) {
  const stage  = ai?.stage ?? null
  const fanOn  = devices?.fan  === true
  const mistOn = devices?.mist === true
  const temp   = environment?.temperature ?? 24

  const warmth = Math.min(Math.max((temp-18)/20,0),1)
  const ambCol = new THREE.Color(0.96+warmth*0.04, 0.97-warmth*0.03, 1.00-warmth*0.10)

  return (
    <>
      <color attach="background" args={['#EDF2ED']} />
      <fog attach="fog" color="#EDF2ED" near={6} far={16} />
      <ambientLight color={ambCol} intensity={0.62} />
      <directionalLight position={[3,6,4]} intensity={0.7} castShadow shadow-mapSize={[1024,1024]} shadow-camera-far={14} />
      <pointLight position={[-2,3,-1]} intensity={0.3} color="#F0F4FF" />
      <OrbitControls target={[0,HH,0]} minDistance={1.8} maxDistance={5.5} maxPolarAngle={Math.PI*0.76} enablePan={false} />

      <mesh position={[0,-0.022,0]} receiveShadow>
        <boxGeometry args={[3.2,0.04,2.6]} />
        <meshStandardMaterial color="#E6E0D8" roughness={0.82} />
      </mesh>

      <TerrariumBase />
      <TerrariumGlass />
      <Terrain />
      <MossPatches />
      <GrassTufts />
      <Pebbles />
      <MushroomBed stage={stage} />
      <GrowLight />
      <MiniFan active={fanOn} />
      <MistParticles active={mistOn} />
    </>
  )
}

export function Scene({ environment, devices, ai }) {
  return (
    <Canvas camera={{ position:[2.6,2.2,3.0], fov:40 }} shadows style={{ width:'100%', height:'100%' }}>
      <Suspense fallback={null}>
        <SceneContent environment={environment} devices={devices} ai={ai} />
      </Suspense>
    </Canvas>
  )
}
