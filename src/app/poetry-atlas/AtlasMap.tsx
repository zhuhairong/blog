'use client';

import { useMemo, useState } from 'react';
import Link from 'next/link';
import styles from './atlas.module.css';
import type { DerivedMapPoint } from '@/poetry-atlas/types';
import { CONFIDENCE_UI, PRECISION_LABEL, type PlaceWorkEntry } from '@/lib/atlas-view';

interface Props {
  points: DerivedMapPoint[];
  /** placeId → 该地相关作品（构建期已生成） */
  placeWorks: Record<string, PlaceWorkEntry[]>;
}

/** 中国地理范围（用于把经纬度线性映射到 SVG 视口） */
const BOUNDS = { minLng: 73, maxLng: 136, minLat: 17, maxLat: 54 };

/** 投影：等距圆柱。对全国尺度的示意地图足够，且无依赖、无合规风险 */
function project(lng: number, lat: number, w: number, h: number) {
  const x = ((lng - BOUNDS.minLng) / (BOUNDS.maxLng - BOUNDS.minLng)) * w;
  // 纬度反向：北在上
  const y = ((BOUNDS.maxLat - lat) / (BOUNDS.maxLat - BOUNDS.minLat)) * h;
  return { x, y };
}

/** 按精度决定点的视觉权重：精度越低，视觉越弱 */
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

export default function AtlasMap({ points, placeWorks }: Props) {
  const [selected, setSelected] = useState<string | null>(null);
  const [precisionFilter, setPrecisionFilter] = useState<Set<string>>(new Set());

  const W = 1000;
  const H = 620;

  const projected = useMemo(
    () =>
      points.map((p) => ({
        ...p,
        ...project(p.coordinates[0], p.coordinates[1], W, H),
      })),
    [points],
  );

  /** 精度过滤器：空集合表示全显示 */
  const visible = projected.filter(
    (p) => precisionFilter.size === 0 || precisionFilter.has(p.precision),
  );

  const active = projected.find((p) => p.placeId === selected) ?? null;

  const togglePrecision = (key: string) => {
    setPrecisionFilter((prev) => {
      const next = new Set(prev);
      if (next.has(key)) next.delete(key);
      else next.add(key);
      return next;
    });
  };

  const precisions = [...new Set(points.map((p) => p.precision))];

  return (
    <div className={styles.mapWrap}>
      <div className={styles.mapToolbar}>
        <span className={styles.mapHint} style={{ marginLeft: 0, marginRight: 8 }}>
          精度筛选
        </span>
        {precisions.map((key) => {
          const on = precisionFilter.size === 0 || precisionFilter.has(key);
          return (
            <button
              key={key}
              type="button"
              className={`${styles.chip} ${on && precisionFilter.size > 0 ? styles.chipOn : ''}`}
              onClick={() => togglePrecision(key)}
              aria-pressed={precisionFilter.has(key)}
            >
              <span
                className={styles.chipSwatch}
                style={{
                  background:
                    key === 'site' || key === 'township'
                      ? 'var(--green)'
                      : key === 'county' || key === 'prefecture'
                        ? 'var(--amber)'
                        : 'var(--text-tertiary)',
                }}
                aria-hidden="true"
              />
              {PRECISION_LABEL[key]}
            </button>
          );
        })}
        <span className={styles.mapHint}>
          {visible.length} / {points.length} 个地点
        </span>
      </div>

      <div className={styles.mapCanvas}>
        <svg
          className={styles.mapSvg}
          viewBox={`0 0 ${W} ${H}`}
          role="img"
          aria-label="诗人创作地分布示意地图"
        >
          {/* 网格背景，纯装饰 */}
          <g stroke="rgba(255,255,255,0.035)" strokeWidth="1">
            {[0.2, 0.4, 0.6, 0.8].map((f) => (
              <line key={`h${f}`} x1="0" y1={H * f} x2={W} y2={H * f} />
            ))}
            {[0.2, 0.4, 0.6, 0.8].map((f) => (
              <line key={`v${f}`} x1={W * f} y1="0" x2={W * f} y2={H} />
            ))}
          </g>

          {/* 点 */}
          {visible.map((p) => {
            const st = PRECISION_STYLE[p.precision];
            const isActive = p.placeId === selected;
            // 低精度用偏暖色提示"位置不精确"，高精度用绿色表示可靠
            const reliable = p.precision === 'site' || p.precision === 'township';
            const color = reliable ? '#34d399' : p.isCentroid ? '#fbbf24' : '#38bdf8';

            return (
              <g
                key={p.placeId}
                className={styles.mapPoint}
                onClick={() => setSelected(isActive ? null : p.placeId)}
                role="button"
                tabIndex={0}
                aria-label={`${p.historicalName}，${p.workCount} 篇作品`}
                onKeyDown={(e) => {
                  if (e.key === 'Enter' || e.key === ' ') {
                    e.preventDefault();
                    setSelected(isActive ? null : p.placeId);
                  }
                }}
              >
                {/* 光晕 */}
                <circle cx={p.x} cy={p.y} r={st.r * 3.2} fill={color} opacity={isActive ? 0.2 : 0.08} />
                {/* 中心点用虚线圈明确标注"非精确位置" */}
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
                {/* 精确遗址用实线外圈强调 */}
                {!p.isCentroid && st.ring && (
                  <circle cx={p.x} cy={p.y} r={st.r + 3} fill="none" stroke={color} strokeWidth="1" opacity="0.5" />
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

        {/* 侧栏详情 */}
        <aside className={styles.mapDetail}>
          {!active ? (
            <p className={styles.mapDetailEmpty}>
              点击地图上的地点，查看该地诞生的作品、诗人，以及坐标精度说明。
              <br />
              <br />
              虚线圆圈表示该坐标是<strong style={{ color: 'var(--amber)' }}>行政区中心点</strong>
              ，仅代表大致区域，并非精确创作位置。
            </p>
          ) : (
            <>
              <h3 className={styles.detailPlace}>{active.historicalName}</h3>
              <p className={styles.detailModern}>{active.modernName}</p>

              <div className={styles.detailMeta}>
                <span className={styles.tag}>{PRECISION_LABEL[active.precision]}</span>
                {active.isCentroid && <span className={`${styles.tag} ${styles.tagWarn}`}>中心点坐标</span>}
                <span className={styles.tag}>
                  {active.yearRange[0]}–{active.yearRange[1]}
                </span>
              </div>

              {/* 置信度分布 —— 让用户一眼看到这个点的可靠性构成 */}
              <div className={styles.detailMeta}>
                {(['A', 'B', 'C', 'D'] as const).map((c) =>
                  active.byConfidence[c] > 0 ? (
                    <span
                      key={c}
                      className={styles.tag}
                      style={{
                        color: CONFIDENCE_UI[c].color,
                        borderColor: `${CONFIDENCE_UI[c].color}44`,
                      }}
                    >
                      {c} {CONFIDENCE_UI[c].label} × {active.byConfidence[c]}
                    </span>
                  ) : null,
                )}
              </div>

              <div className={styles.detailWorks}>
                {(placeWorks[active.placeId] ?? []).map((w) => (
                  <Link
                    key={w.workId}
                    href={`/poetry-atlas/work/${w.workId}`}
                    className={styles.detailWorkLink}
                  >
                    <span>{w.title}</span>
                    <span className={styles.detailWorkMeta}>
                      {w.poetName}
                      {w.year ? ` · ${w.year}` : ''}
                    </span>
                  </Link>
                ))}
                {!(placeWorks[active.placeId] ?? []).length && (
                  <p className={styles.mapDetailEmpty}>该地点暂无作品关联。</p>
                )}
              </div>
            </>
          )}
        </aside>
      </div>
    </div>
  );
}
