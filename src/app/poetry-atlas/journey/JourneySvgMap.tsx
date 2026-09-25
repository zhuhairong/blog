'use client';

/**
 * 一生行迹地图 · 自绘底图版（未配置天地图密钥时使用）。
 *
 * 与「创作地分布」共用 `geo.tsx` 的投影与底图几何，
 * 保证两张图的中国形状完全一致。
 *
 * 画法上的取舍：站点之间连**弧线**，弧线中点附近加**箭头**，箭头即时间方向。
 * 弧线只是把两点间的先后关系画得可读，并不表示史料记载了具体路线；
 * 外凸方向由行进方向决定，故往返两程自然分居弦的两侧、不会重叠。
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
  arcControl,
  arcAt,
  type Pt,
} from './journey-util';

interface Props {
  stops: JourneyStop[];
  selectedSeq: number | null;
  onSelect: (seq: number) => void;
  /** 播放到当前站时给一个呼吸圈，帮助视线跟上 */
  playing: boolean;
}

/** 弧线外凸的绝对上限（viewBox 单位）。取景已给右侧栏留出安全区，
 *  上限须小于那 62 单位的余量，否则弧线会伸进侧栏底下。 */
const MAX_BOW = 30;
/** 箭头放在弧线的这个比例处——避开两端站点圆，落在可见的弧段上 */
const ARROW_T = 0.58;
/** 短于这个长度的段不放箭头，否则密集处会挤成一团 */
const ARROW_MIN_SEG = 30;
/** 箭头三角（指向 +x，靠 transform 旋转到切向） */
const ARROW_D = 'M-5 -4 L6.5 0 L-5 4 Z';

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

  /** 逐段外凸的弧线路径 */
  const arcPath = (pts: Pt[]) => {
    if (pts.length < 2) return '';
    let d = `M${pts[0]!.x.toFixed(1)} ${pts[0]!.y.toFixed(1)}`;
    for (let i = 1; i < pts.length; i++) {
      const c = arcControl(pts[i - 1]!, pts[i]!, MAX_BOW);
      d += ` Q${c.x.toFixed(1)} ${c.y.toFixed(1)} ${pts[i]!.x.toFixed(1)} ${pts[i]!.y.toFixed(1)}`;
    }
    return d;
  };

  /** 每段弧线上的箭头位置与朝向 */
  const arrowsOf = (pts: Pt[]) => {
    const out: { key: string; x: number; y: number; deg: number }[] = [];
    for (let i = 1; i < pts.length; i++) {
      const a = pts[i - 1]!;
      const b = pts[i]!;
      if (Math.hypot(b.x - a.x, b.y - a.y) < ARROW_MIN_SEG) continue;
      const p = arcAt(a, arcControl(a, b, MAX_BOW), b, ARROW_T);
      out.push({ key: `a${i}`, x: p.x, y: p.y, deg: (p.angle * 180) / Math.PI });
    }
    return out;
  };

  const liveD = arcPath(live);
  const liveArrows = arrowsOf(live);
  const afterPts: Pt[] = live.length ? [live[live.length - 1]!, ...after] : after;
  const afterD = arcPath(afterPts);
  const afterArrows = arrowsOf(afterPts);

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
          先画一层宽而淡的「光晕」，再叠一层实线，让线在深浅底图上都看得见。
          两层用同一条弧线路径，避免视觉上错位。 */}
      {liveD && (
        <>
          <path
            d={liveD}
            fill="none"
            stroke="rgba(124,92,255,0.22)"
            strokeWidth="7"
            strokeLinecap="round"
            strokeLinejoin="round"
          />
          <path
            className={styles.journeyLine}
            d={liveD}
            fill="none"
            stroke="rgba(167,139,250,0.92)"
            strokeWidth="2"
            strokeLinecap="round"
            strokeLinejoin="round"
          />
        </>
      )}
      {/* 身后事件（迁葬、追谥）另用虚线接续 */}
      {afterD && live.length > 0 && (
        <path
          d={afterD}
          fill="none"
          stroke="rgba(167,139,250,0.5)"
          strokeWidth="1.4"
          strokeDasharray="5 5"
        />
      )}

      {/* ── 方向箭头 ──
          沿线排布、不放在站点上（站点圆会盖住它），朝向即「下一站往哪走」 */}
      <g className={styles.journeyArrows} aria-hidden="true">
        {liveArrows.map((a) => (
          <path
            key={a.key}
            className={styles.journeyArrow}
            d={ARROW_D}
            transform={`translate(${a.x.toFixed(1)} ${a.y.toFixed(1)}) rotate(${a.deg.toFixed(1)})`}
          />
        ))}
        {afterArrows.map((a) => (
          <path
            key={a.key}
            className={`${styles.journeyArrow} ${styles.journeyArrowAfter}`}
            d={ARROW_D}
            transform={`translate(${a.x.toFixed(1)} ${a.y.toFixed(1)}) rotate(${a.deg.toFixed(1)})`}
          />
        ))}
      </g>

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
