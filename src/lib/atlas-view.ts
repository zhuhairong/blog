/**
 * 诗歌图谱 · 展示辅助（客户端安全）
 *
 * 本模块不得引入任何 Node 内置模块（fs / path 等），
 * 因为客户端组件（如 AtlasMap）需要复用这里的常量与格式化函数。
 *
 * 数据读取请用 @/lib/atlas（仅限 Server Component）。
 */

import type { Confidence, HistoricalDate, Place, Source } from '@/poetry-atlas/types';

/** 地点 → 作品清单条目（构建期由管线生成，客户端直接消费） */
export interface PlaceWorkEntry {
  workId: string;
  title: string;
  poetId: string;
  poetName: string;
  year: number | null;
}

/** 关联作品的关联类型 */
export type RelatedReason = 'same-place' | 'same-time' | 'same-event';

/** 作品 → 关联作品（构建期生成） */
export interface RelatedWorkEntry {
  workId: string;
  title: string;
  poetId: string;
  poetName: string;
  year: number | null;
  placeId: string | null;
  placeName: string | null;
  reason: RelatedReason;
}

/** 关联类型的中文说明 */
export const RELATED_LABEL: Record<RelatedReason, string> = {
  'same-event': '同一段人生际遇',
  'same-place': '写于同一地方',
  'same-time': '写于相近之时',
};

/** 关联类型的强调色 */
export const RELATED_COLOR: Record<RelatedReason, string> = {
  'same-event': '#a78bfa',
  'same-place': '#34d399',
  'same-time': '#38bdf8',
};

/** 置信度展示元数据 */
export const CONFIDENCE_UI: Record<
  Confidence,
  { label: string; desc: string; color: string }
> = {
  A: { label: '确定', desc: '有直接史料或作者自述佐证', color: '#34d399' },
  B: { label: '较可靠', desc: '权威年谱或学界主流共识', color: '#38bdf8' },
  C: { label: '存疑', desc: '古今说法不一，尚存争议', color: '#fbbf24' },
  D: { label: '传说', desc: '民间传说或地方附会，仅供参考', color: '#f87171' },
};

/** 地理精度标签 */
export const PRECISION_LABEL: Record<Place['precision'], string> = {
  site: '具体遗址',
  township: '乡镇级',
  county: '县级',
  prefecture: '州府级',
  province: '省级',
  approximate: '大区域',
};

/** 生平事件类型标签 */
export const EVENT_LABEL: Record<string, string> = {
  birth: '出生',
  death: '逝世',
  exam: '科举',
  office: '任职',
  demotion: '贬谪',
  travel: '漫游',
  war: '战乱',
  seclusion: '隐居',
  marriage: '婚配',
  meeting: '交游',
  other: '其他',
};

/** 格式化历史纪年 */
export function formatDate(d?: HistoricalDate): string {
  if (!d) return '年代不详';
  if (d.certainty === 'unknown') return d.original || '年代不详';
  const parts: string[] = [`${d.year}年`];
  if (d.month) parts.push(`${d.month}月`);
  if (d.day) parts.push(`${d.day}日`);
  const approx = d.certainty === 'approximate' || d.fuzziness ? '约' : '';
  return `${approx}${parts.join('')}`;
}

/** 格式化纪年（含原始表述） */
export function formatDateFull(d?: HistoricalDate): string {
  if (!d) return '年代不详';
  const norm = formatDate(d);
  return d.original && d.original !== norm ? `${norm}（${d.original}）` : norm;
}

/** 取来源的简短引用文本 */
export function formatSource(s: Source): string {
  const bits = [s.title];
  if (s.locator) bits.push(s.locator);
  if (s.author) bits.push(s.author);
  return bits.join(' · ');
}
