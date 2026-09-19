'use client';

/**
 * 无密钥时的回落底图：真实地理形状的行政区划图（Web 墨卡托）。
 *
 * 用墨卡托而不是等距圆柱，是为了让回落底图与天地图瓦片底图的形状观感一致，
 * 切换密钥前后不至于「换了张不像的图」。
 *
 * 绘制范围只到 17°N，南海诸岛另设角图 —— 这是中国地图的常规画法。
 * 本图只画中国境内行政区面，不画国界、不画海域断续线；需要严格国家标准
 * 画法（含断续线）时请配置 NEXT_PUBLIC_TIANDITU_KEY 走天地图瓦片。
 */

import { useMemo } from 'react';
import type { DerivedMapPoint } from '@/poetry-atlas/types';
import styles from './atlas.module.css';
import basemap from './china-basemap.json';

interface Props {
  points: DerivedMapPoint[];
  selected: string | null;
  onSelect: (placeId: string | null) => void;
}

/* ── 投影 ──────────────────────────────────────────────── */

/** 主图范围：不含南海诸岛，那部分走角图 */
const MAIN = { minLng: 73, maxLng: 136, minLat: 17, maxLat: 54 };
/** 南海诸岛角图范围 */
const INSET_BOUNDS = { minLng: 108, maxLng: 119, minLat: 3, maxLat: 18.5 };

/** 墨卡托纵坐标（纬度 → 无量纲 y），裁到 ±85° 避免极点发散 */
function mercY(lat: number) {
  const l = Math.max(-85, Math.min(85, lat));
  return Math.log(Math.tan(Math.PI / 4 + (l * Math.PI) / 180 / 2));
}

const W = 1000;
const Y_TOP = mercY(MAIN.maxLat);
const Y_BOT = mercY(MAIN.minLat);
const SPAN_X = ((MAIN.maxLng - MAIN.minLng) * Math.PI) / 180;
const SPAN_Y = Y_TOP - Y_BOT;
/** 高度按墨卡托的真实长宽比推导，避免地图被横向拉伸 */
const H = Math.round((W * SPAN_Y) / SPAN_X);

const projectMain = (lng: number, lat: number) => ({
  x: ((lng - MAIN.minLng) / (MAIN.maxLng - MAIN.minLng)) * W,
  y: ((Y_TOP - mercY(lat)) / SPAN_Y) * H,
});

/** 角图：尺寸同样按墨卡托比例推导 */
const INSET_H = 150;
const IY_TOP = mercY(INSET_BOUNDS.maxLat);
const IY_BOT = mercY(INSET_BOUNDS.minLat);
const ISPAN_X = ((INSET_BOUNDS.maxLng - INSET_BOUNDS.minLng) * Math.PI) / 180;
const ISPAN_Y = IY_TOP - IY_BOT;
const INSET_W = Math.round((INSET_H * ISPAN_X) / ISPAN_Y);
const INSET = { x: W - INSET_W - 16, y: H - INSET_H - 22, w: INSET_W, h: INSET_H };

const projectInset = (lng: number, lat: number) => ({
  x: INSET.x + ((lng - INSET_BOUNDS.minLng) / (INSET_BOUNDS.maxLng - INSET_BOUNDS.minLng)) * INSET.w,
  y: INSET.y + ((IY_TOP - mercY(lat)) / ISPAN_Y) * INSET.h,
});

/** 扁平数组 → SVG path */
function toPath(flat: number[], proj: (lng: number, lat: number) => { x: number; y: number }) {
  let d = '';
  for (let i = 0; i < flat.length; i += 2) {
    const { x, y } = proj(flat[i], flat[i + 1]);
    d += `${i === 0 ? 'M' : 'L'}${x.toFixed(1)} ${y.toFixed(1)}`;
  }
  return `${d}Z`;
}

/** 小于此尺寸（度）的岛礁退化成点，否则在角图里只是看不见的碎线 */
const DOT_MAX_DIAG = 0.4;

/* ── 精度样式（与瓦片底图保持一致的语言）───────────────── */
const PRECISION_STYLE: Record<
  DerivedMapPoint['precision'],
  { r: number; fillOpacity: number; ring: boolean }
> = {
  site: { r: 7, fillOpacity: 1, ring: true },
  township: { r: 6.5, fillOpacity: 1, ring: true },
  county: { r: 6, fillOpacity: 0.85, ring: true },
  prefecture: { r: 6, fillOpacity: 0.8, ring: true },
  province: { r: 5.5, fillOpacity: 0.7, ring: false },
  approximate: { r: 5.5, fillOpacity: 0.55, ring: false },
};

export default function SvgFallbackMap({ points, selected, onSelect }: Props) {
  /** 省界路径只在首次渲染时算一次 */
  const provincePaths = useMemo(
    () =>
      basemap.provinces.map((p) => ({
        name: p.name,
        d: p.rings.map((r) => toPath(r.p, projectMain)).join(' '),
      })),
    [],
  );

  /** 南海诸岛：海南省要素里纬度低于 17.5° 的环 */
  const islandGeometry = useMemo(() => {
    const hainan = basemap.provinces.find((p) => p.name === '海南省');
    if (!hainan) return { paths: [] as string[], dots: [] as { x: number; y: number }[] };
    const paths: string[] = [];
    const dots: { x: number; y: number }[] = [];
    for (const r of hainan.rings) {
      if (r.bbox[1] >= 17.5) continue; // 海南岛本体不进角图
      const diag = Math.hypot(r.bbox[2] - r.bbox[0], r.bbox[3] - r.bbox[1]);
      if (diag < DOT_MAX_DIAG) {
        // 取包围盒中心当作岛礁位置
        dots.push(projectInset((r.bbox[0] + r.bbox[2]) / 2, (r.bbox[1] + r.bbox[3]) / 2));
      } else {
        paths.push(toPath(r.p, projectInset));
      }
    }
    return { paths, dots };
  }, []);

  const projected = points.map((p) => ({
    ...p,
    ...projectMain(p.coordinates[0], p.coordinates[1]),
  }));

  return (
    <svg
      className={styles.mapSvg}
      viewBox={`0 0 ${W} ${H}`}
      role="img"
      aria-label="诗人创作地分布地图（行政区划底图，未配置天地图密钥）"
    >
      <defs>
        {/* 陆地阴影，让陆块从海面上浮起来 */}
        <filter id="atlasLandGlow" x="-10%" y="-10%" width="120%" height="120%">
          <feDropShadow dx="0" dy="2" stdDeviation="5" floodColor="#000" floodOpacity="0.45" />
        </filter>
        <clipPath id="atlasInsetClip">
          <rect
            x={INSET.x}
            y={INSET.y}
            width={INSET.w}
            height={INSET.h}
            rx="6"
          />
        </clipPath>
      </defs>

      {/* 省界：先描边后填充，边界线始终是连续的 */}
      <g filter="url(#atlasLandGlow)">
        {provincePaths.map((p) => (
          <path key={`f-${p.name}`} d={p.d} fill="rgba(255,255,255,0.05)" stroke="none" />
        ))}
      </g>
      <g fill="none" stroke="rgba(255,255,255,0.16)" strokeWidth="0.9" strokeLinejoin="round">
        {provincePaths.map((p) => (
          <path key={`s-${p.name}`} d={p.d} />
        ))}
      </g>

      {/* 经纬网：淡到几乎看不见，只用于定位 */}
      <g stroke="rgba(255,255,255,0.04)" strokeWidth="1">
        {[80, 90, 100, 110, 120, 130].map((lng) => {
          const { x } = projectMain(lng, MAIN.minLat);
          return <line key={`m${lng}`} x1={x} y1="0" x2={x} y2={H} />;
        })}
        {[20, 30, 40, 50].map((lat) => {
          const { y } = projectMain(MAIN.minLng, lat);
          return <line key={`p${lat}`} x1="0" y1={y} x2={W} y2={y} />;
        })}
      </g>

      {/* ── 南海诸岛角图 ── */}
      <g>
        <rect
          x={INSET.x}
          y={INSET.y}
          width={INSET.w}
          height={INSET.h}
          rx="6"
          fill="rgba(10,10,16,0.86)"
          stroke="rgba(255,255,255,0.2)"
          strokeWidth="1"
        />
        <g clipPath="url(#atlasInsetClip)">
          <g fill="none" stroke="rgba(255,255,255,0.18)" strokeWidth="0.7">
            {islandGeometry.paths.map((d, i) => (
              <path key={`ip${i}`} d={d} />
            ))}
          </g>
          <g fill="rgba(255,255,255,0.22)">
            {islandGeometry.dots.map((d, i) => (
              <circle key={`id${i}`} cx={d.x} cy={d.y} r="1.1" />
            ))}
          </g>
          <text
            x={INSET.x + 6}
            y={INSET.y + 14}
            fill="rgba(255,255,255,0.5)"
            fontSize="10"
          >
            南海诸岛
          </text>
        </g>
      </g>

      {/* ── 诗人创作地点 ── */}
      {projected.map((p) => {
        const st = PRECISION_STYLE[p.precision];
        const isActive = p.placeId === selected;
        const reliable = p.precision === 'site' || p.precision === 'township';
        const color = reliable ? '#34d399' : p.isCentroid ? '#fbbf24' : '#38bdf8';

        return (
          <g
            key={p.placeId}
            className={styles.mapPoint}
            onClick={() => onSelect(isActive ? null : p.placeId)}
            role="button"
            tabIndex={0}
            aria-label={`${p.historicalName}，${p.workCount} 篇作品`}
            onKeyDown={(e) => {
              if (e.key === 'Enter' || e.key === ' ') {
                e.preventDefault();
                onSelect(isActive ? null : p.placeId);
              }
            }}
          >
            <circle cx={p.x} cy={p.y} r={st.r * 3.2} fill={color} opacity={isActive ? 0.2 : 0.08} />
            {p.isCentroid && (
              <circle
                className={styles.centroidRing}
                cx={p.x}
                cy={p.y}
                r={st.r + 5}
                stroke={color}
                strokeWidth="1.2"
              />
            )}
            {!p.isCentroid && st.ring && (
              <circle
                cx={p.x}
                cy={p.y}
                r={st.r + 3}
                fill="none"
                stroke={color}
                strokeWidth="1"
                opacity="0.5"
              />
            )}
            <circle
              className="pointCore"
              cx={p.x}
              cy={p.y}
              r={isActive ? st.r + 2 : st.r}
              fill={color}
              fillOpacity={st.fillOpacity}
            />
            {isActive && (
              <circle cx={p.x} cy={p.y} r={st.r + 9} fill="none" stroke={color} strokeWidth="1.5" opacity="0.7" />
            )}
            <text className={styles.pointLabel} x={p.x + st.r + 8} y={p.y + 4}>
              {p.historicalName}
              {p.workCount > 1 ? ` (${p.workCount})` : ''}
            </text>
          </g>
        );
      })}
    </svg>
  );
}
