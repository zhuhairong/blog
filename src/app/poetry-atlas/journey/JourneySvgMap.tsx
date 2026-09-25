'use client';

/**
 * 一生行迹地图 · 自绘底图版（未配置天地图密钥时使用）。
 *
 * 与「创作地分布」共用 `geo.tsx` 的投影与底图几何，
 * 保证两张图的中国形状完全一致。
 *
 * 画法上的取舍：站点之间用**直线**连接，不做贝塞尔美化——
 * 行迹是史料推出来的路线，弧线会让人误以为有精确的路径信息。
 */

import type { JourneyStop } from '@/poetry-atlas/types';
import { TONE_UI } from '@/lib/atlas-view';
import styles from '../atlas.module.css';
import { makeProjection, journeyBounds, BasemapDefs, BasemapLayers } from '../geo';
import {
  toneOf,
  radiusOf,
  groupStopsByCoordinate,
  displayedStopOf,
  nextStopInGroup,
} from './journey-util';

interface Props {
  stops: JourneyStop[];
  selectedSeq: number | null;
  onSelect: (seq: number) => void;
  /** 播放到当前站时给一个呼吸圈，帮助视线跟上 */
  playing: boolean;
}

export default function JourneySvgMap({ stops, selectedSeq, onSelect, playing }: Props) {
  /**
   * 取景按该诗人自己的行迹来定，而不是一律画全中国。
   *
   * 李白 33 站全落在四川至江苏之间，画全中国等于把有效信息压进画面一小块，
   * 还会正好落在侧栏底下。省界路径按取景重算，由 `geo` 内的缓存兜住重复计算。
   */
  const proj = makeProjection(journeyBounds(stops));

  /** 行迹线按**每一站**依次连（含同址的重复点），不能按分组连——否则路径会丢段 */
  const all = stops.map((s) => ({
    stop: s,
    ...proj.project(s.coordinates[0], s.coordinates[1]),
  }));

  const live = all.filter((p) => !p.stop.posthumous);
  const after = all.filter((p) => p.stop.posthumous);
  const line = (pts: typeof all) =>
    pts.map((p, i) => `${i === 0 ? 'M' : 'L'}${p.x.toFixed(1)} ${p.y.toFixed(1)}`).join(' ');

  /** 标记按坐标归组：同址多次驻留只画一个点，避免互相盖住 */
  const marks = groupStopsByCoordinate(stops).map((group) => {
    const c = proj.project(group[0]!.coordinates[0], group[0]!.coordinates[1]);
    return { group, x: c.x, y: c.y };
  });

  return (
    <svg
      className={styles.mapSvg}
      viewBox={`0 0 ${proj.width} ${proj.height}`}
      role="img"
      aria-label={`一生行迹地图，共 ${stops.length} 站`}
    >
      <BasemapDefs proj={proj} />
      <BasemapLayers proj={proj} />

      {/* ── 行迹线 ──
          先画一层宽而淡的「光晕」，再叠一层实线，让线在深浅底图上都看得见 */}
      {live.length > 1 && (
        <>
          <path
            d={line(live)}
            fill="none"
            stroke="rgba(124,92,255,0.22)"
            strokeWidth="7"
            strokeLinecap="round"
            strokeLinejoin="round"
          />
          <path
            className={styles.journeyLine}
            d={line(live)}
            fill="none"
            stroke="rgba(167,139,250,0.92)"
            strokeWidth="2"
            strokeLinecap="round"
            strokeLinejoin="round"
          />
        </>
      )}
      {/* 身后事件（迁葬、追谥）另用虚线接续 */}
      {after.length > 0 && live.length > 0 && (
        <path
          d={line([live[live.length - 1]!, ...after])}
          fill="none"
          stroke="rgba(167,139,250,0.5)"
          strokeWidth="1.4"
          strokeDasharray="5 5"
        />
      )}

      {/* ── 站点（按坐标归组，同址多次驻留只画一个） ── */}
      {marks.map(({ group, x, y }) => {
        const s = displayedStopOf(group, selectedSeq);
        const tone = toneOf(s);
        const color = TONE_UI[tone].color;
        const r = radiusOf(s);
        const isActive = s.seq === selectedSeq;
        /* 标签看**整组**而不是当前显示的那一站：标记代表的是「这个地方」，
           若组内任一驻留是生地/卒地/身后，地名就该标出来（否则会出现无名的点） */
        const showLabel =
          isActive ||
          group.some((m) => m.isBirthplace || m.isDeathPlace || m.posthumous);
        const extra = group.length - 1;

        return (
          <g
            key={`${s.placeId}-${group[0]!.seq}`}
            className={`${styles.mapPoint} ${isActive ? styles.journeyStopOn : ''}`}
            onClick={() => onSelect(nextStopInGroup(group, selectedSeq))}
            role="button"
            tabIndex={0}
            aria-label={`第 ${s.seq} 站 ${s.historicalName}，${s.fromYear}–${s.toYear}，${s.works.length} 篇作品${
              extra > 0 ? `（此地共 ${group.length} 站）` : ''
            }`}
            onKeyDown={(e) => {
              if (e.key === 'Enter' || e.key === ' ') {
                e.preventDefault();
                onSelect(nextStopInGroup(group, selectedSeq));
              }
            }}
          >
            {/* 呼吸圈：仅在播放时出现，静态浏览时保持安静 */}
            {playing && isActive && (
              <circle
                className={styles.journeyPulse}
                cx={x}
                cy={y}
                r={r + 6}
                fill="none"
                stroke={color}
                strokeWidth="1.6"
              />
            )}
            {/* 同址还有别的驻留时往外叠圈，让「这里来过不止一次」看得见 */}
            {Array.from({ length: extra }, (_, i) => (
              <circle
                key={`x${i}`}
                cx={x}
                cy={y}
                r={r + 3.4 * (i + 1)}
                fill="none"
                stroke={color}
                strokeWidth="1"
                opacity={0.5 - i * 0.13}
              />
            ))}
            <circle
              cx={x}
              cy={y}
              r={r * 2.4}
              fill={color}
              opacity={isActive ? 0.24 : 0.09}
            />
            {/* 「据作品系年补出」的站点用虚线圈区分，不与年谱有载者混同 */}
            {s.source === 'work' && (
              <circle
                cx={x}
                cy={y}
                r={r + 4}
                fill="none"
                stroke={color}
                strokeWidth="1"
                strokeDasharray="3 3"
                opacity="0.75"
              />
            )}
            {isActive && (
              <circle cx={x} cy={y} r={r + 8} fill="none" stroke={color} strokeWidth="1.6" opacity="0.85" />
            )}
            <circle
              cx={x}
              cy={y}
              r={r}
              fill={color}
              fillOpacity={s.confidence === 'D' ? 0.6 : 0.95}
              stroke="rgba(8,8,13,0.85)"
              strokeWidth="1.2"
            />
            <text
              className={styles.journeySeq}
              x={x}
              y={y + 3.2}
              textAnchor="middle"
              fontSize={r >= 10 ? '9.5' : '8.5'}
            >
              {s.seq}
            </text>
            {showLabel && (
              <text
                className={styles.journeyLabel}
                /* 靠右的站点把标签翻到左边——否则会伸进侧栏底下被遮住 */
                x={x > proj.width * 0.72 ? x - r - 6 : x + r + 6}
                y={y + 4}
                fontSize="11.5"
                textAnchor={x > proj.width * 0.72 ? 'end' : 'start'}
              >
                {s.historicalName}
                {extra > 0 ? `（共 ${group.length} 站）` : ''}
                {s.works.length > 0 ? ` · ${s.works.length}篇` : ''}
              </text>
            )}
          </g>
        );
      })}
    </svg>
  );
}
