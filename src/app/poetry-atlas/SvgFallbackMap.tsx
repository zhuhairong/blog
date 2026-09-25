'use client';

/**
 * 无密钥时的回落底图：真实地理形状的行政区划图（Web 墨卡托）。
 *
 * 用墨卡托而不是等距圆柱，是为了让回落底图与天地图瓦片底图的形状观感一致，
 * 切换密钥前后不至于「换了张不像的图」。
 *
 * 投影与底图几何已抽到 `geo.tsx`，与「一生行迹」地图共用同一份，
 * 保证两张图的中国形状不会各画各的。
 */

import type { DerivedMapPoint } from '@/poetry-atlas/types';
import styles from './atlas.module.css';
import { MAIN_PROJECTION, BasemapDefs, BasemapLayers } from './geo';

/** 全国视图：这张图表现的是「所有诗人加起来」的空间分布，全图才是对的 */
const PROJ = MAIN_PROJECTION;

interface Props {
  points: DerivedMapPoint[];
  selected: string | null;
  onSelect: (placeId: string | null) => void;
}

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
  const projected = points.map((p) => ({
    ...p,
    ...PROJ.project(p.coordinates[0], p.coordinates[1]),
  }));

  return (
    <svg
      className={styles.mapSvg}
      viewBox={`0 0 ${PROJ.width} ${PROJ.height}`}
      role="img"
      aria-label="诗人创作地分布地图（行政区划底图，未配置天地图密钥）"
    >
      <BasemapDefs proj={PROJ} />
      <BasemapLayers proj={PROJ} />

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
