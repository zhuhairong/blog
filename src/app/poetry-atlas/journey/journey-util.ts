/**
 * 行迹地图的共享展示规则（客户端安全，无 JSX）。
 *
 * 自绘 SVG 版与天地图瓦片版必须用**同一套**取色与取径规则，
 * 否则切换密钥前后站点的大小与颜色会变——那会让人以为换了数据。
 */

import type { JourneyStop } from '@/poetry-atlas/types';
import { EVENT_TONE, type EventTone } from '@/lib/atlas-view';

/** 站点主色：取站内首个事件类型的视觉分组 */
export function toneOf(stop: JourneyStop): EventTone {
  for (const t of stop.types) {
    const tone = EVENT_TONE[t];
    if (tone) return tone;
  }
  return 'other';
}

/** 点半径随该站作品数增长，让「高产之地」自己浮出来 */
export function radiusOf(stop: JourneyStop): number {
  return 7.5 + Math.min(stop.works.length, 5) * 1.4;
}

/**
 * 把坐标相同的站点归为一组。
 *
 * 同一地点的多次驻留（如苏轼三度还眉山：1037 生、1057 归葬、1065 守制）投影后
 * 落在同一像素上，逐个画会互相盖住，只剩最上面那个编号可见——看起来像丢了两站。
 * 归组后每组只画一个标记，编号随选中项变化。
 *
 * ⚠️ 只归组，**不做任何偏移**：为了分辨而把点挪开就等于虚标地理精度，
 * 与「史料到哪级标哪级」的铁律冲突。分辨靠编号 + 「共 N 站」标注解决。
 */
export function groupStopsByCoordinate(stops: JourneyStop[]): JourneyStop[][] {
  const byCoord = new Map<string, JourneyStop[]>();
  for (const s of stops) {
    const key = `${s.coordinates[0].toFixed(4)},${s.coordinates[1].toFixed(4)}`;
    const arr = byCoord.get(key);
    if (arr) arr.push(s);
    else byCoord.set(key, [s]);
  }
  return [...byCoord.values()].map((g) => [...g].sort((a, b) => a.seq - b.seq));
}

/** 组内该显示哪一站：选中项在组内就用它，否则用组内最早的一站 */
export function displayedStopOf(
  group: JourneyStop[],
  selectedSeq: number | null,
): JourneyStop {
  return group.find((s) => s.seq === selectedSeq) ?? group[0]!;
}

/** 组内点一下跳到哪一站：在组内就顺延到下一次驻留（循环），不在就回到最早一站 */
export function nextStopInGroup(
  group: JourneyStop[],
  selectedSeq: number | null,
): number {
  const i = group.findIndex((s) => s.seq === selectedSeq);
  if (i < 0) return group[0]!.seq;
  return group[(i + 1) % group.length]!.seq;
}

/* ─────────── 弧线几何（两版底图共用） ───────────
 *
 * 站点之间画弧线而不是直线，弧线中点附近加箭头表示时间方向。
 * 几何只认平面坐标，SVG 版传投影后的 x/y、瓦片版传经纬度，两边形状一致。
 */

export interface Pt {
  x: number;
  y: number;
}

/** 弧线外凸比例（相对弦长）与绝对上限（单位同入参坐标） */
export const ARC_BULGE = 0.2;

/**
 * 二次贝塞尔的控制点：自弦中点向**行进方向的左手侧**外凸。
 *
 * 关键在于方向取自 a→b：往返两程的方向相反，法向随之翻转，
 * 于是去程与回程自动分居弦的两侧、形成梭形，不会叠在一起。
 */
export function arcControl(a: Pt, b: Pt, maxOff = Infinity): Pt {
  const dx = b.x - a.x;
  const dy = b.y - a.y;
  const dist = Math.hypot(dx, dy);
  if (dist === 0) return { x: (a.x + b.x) / 2, y: (a.y + b.y) / 2 };
  const off = Math.min(dist * ARC_BULGE, maxOff);
  return {
    x: (a.x + b.x) / 2 + (-dy / dist) * off,
    y: (a.y + b.y) / 2 + (dx / dist) * off,
  };
}

/** 二次贝塞尔在参数 t 处的点与切向角（弧度） */
export function arcAt(
  a: Pt,
  c: Pt,
  b: Pt,
  t: number,
): { x: number; y: number; angle: number } {
  const mt = 1 - t;
  const x = mt * mt * a.x + 2 * mt * t * c.x + t * t * b.x;
  const y = mt * mt * a.y + 2 * mt * t * c.y + t * t * b.y;
  const tx = 2 * mt * (c.x - a.x) + 2 * t * (b.x - c.x);
  const ty = 2 * mt * (c.y - a.y) + 2 * t * (b.y - c.y);
  return { x, y, angle: Math.atan2(ty, tx) };
}

/** 把二次贝塞尔采样成折线（Leaflet 没有原生曲线，用它近似） */
export function arcSamples(a: Pt, c: Pt, b: Pt, n = 16): Pt[] {
  const out: Pt[] = [];
  for (let i = 0; i <= n; i++) {
    const t = i / n;
    const mt = 1 - t;
    out.push({
      x: mt * mt * a.x + 2 * mt * t * c.x + t * t * b.x,
      y: mt * mt * a.y + 2 * mt * t * c.y + t * t * b.y,
    });
  }
  return out;
}
