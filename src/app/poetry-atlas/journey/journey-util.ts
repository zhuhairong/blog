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
