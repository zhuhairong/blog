/**
 * 诗歌图谱 · 数据访问层
 *
 * 所有数据在构建期从 poetry-atlas/data/derived/ 读取（静态导出无服务端）。
 * 本模块使用 Node 内置模块，**仅限 Server Component 使用**。
 *
 * ⚠️ 客户端组件请改用 @/lib/atlas-view（纯展示辅助，无 Node 依赖）。
 */

import fs from 'fs';
import path from 'path';
import type {
  Assertion,
  Confidence,
  DerivedMapPoint,
  DerivedTrajectory,
  Place,
  Poet,
  Work,
} from '@/poetry-atlas/types';

// 展示辅助统一由 atlas-view 提供，此处再导出以便服务端一处引用
export {
  CONFIDENCE_UI,
  PRECISION_LABEL,
  EVENT_LABEL,
  RELATED_LABEL,
  RELATED_COLOR,
  formatDate,
  formatDateFull,
  formatSource,
} from './atlas-view';

// 局部也需要该类型（re-export 不会引入本地作用域）
import type { PlaceWorkEntry, RelatedWorkEntry } from './atlas-view';

/** 派生数据目录 */
const DERIVED = path.join(process.cwd(), 'poetry-atlas', 'data', 'derived');

/** 派生层可能尚未构建，读取时优雅降级 */
function readDerived<T>(rel: string, fallback: T): T {
  const full = path.join(DERIVED, rel);
  if (!fs.existsSync(full)) return fallback;
  try {
    return JSON.parse(fs.readFileSync(full, 'utf8')) as T;
  } catch {
    return fallback;
  }
}

// ─────────────────────────────────────────────
// 类型：派生层返回结构
// ─────────────────────────────────────────────

export interface PoetIndexEntry {
  id: string;
  name: string;
  dynasty: string;
  courtesyName?: string;
  aliases: string[];
  birthYear: number | null;
  deathYear: number | null;
  summary: string;
  workCount: number;
  locatedWorkCount: number;
}

export interface PoetDetail extends Poet {
  works: Work[];
  assertions: Assertion[];
  places: Place[];
  trajectory: DerivedTrajectory;
}

export interface WorkDetail extends Work {
  assertions: Assertion[];
  places: Place[];
}

export interface TimelineBucket {
  decade: number;
  count: number;
  workIds: string[];
  poetIds: string[];
}

/** 异说分组：把同一 groupId 的断言聚在一起 */
export interface AssertionGroup {
  /** groupId，无分组时用断言 id 兜底 */
  key: string;
  /** 该组是否包含互斥异说（多于一条） */
  contested: boolean;
  /**** 组内断言按主流优先排序 */
  items: Assertion[];
}

// ─────────────────────────────────────────────
// 读取接口
// ─────────────────────────────────────────────

export function getPoetsIndex(): PoetIndexEntry[] {
  return readDerived<PoetIndexEntry[]>('poets-index.json', []);
}

export function getMapPoints(): DerivedMapPoint[] {
  return readDerived<DerivedMapPoint[]>('map-points.json', []);
}

/** 地点 → 作品清单（构建期已生成，前端直接消费） */
export function getPlaceWorks(): Record<string, PlaceWorkEntry[]> {
  return readDerived<Record<string, PlaceWorkEntry[]>>('place-works.json', {});
}

/** 作品 → 关联作品（同期 / 同地 / 同事件），用于把作品织成网 */
export function getRelatedWorks(): Record<string, RelatedWorkEntry[]> {
  return readDerived<Record<string, RelatedWorkEntry[]>>('related-works.json', {});
}

/** 取单篇作品的关联作品 */
export function getRelatedOf(workId: string): RelatedWorkEntry[] {
  return getRelatedWorks()[workId] ?? [];
}

export function getTimeline(): TimelineBucket[] {
  return readDerived<TimelineBucket[]>('timeline.json', []);
}

export function getPoetDetail(id: string): PoetDetail | null {
  return readDerived<PoetDetail | null>(`poet/${id}.json`, null);
}

export function getWorkDetail(id: string): WorkDetail | null {
  return readDerived<WorkDetail | null>(`work/${id}.json`, null);
}

/** 所有诗人 id，供 generateStaticParams 使用 */
export function getAllPoetIds(): string[] {
  const dir = path.join(DERIVED, 'poet');
  if (!fs.existsSync(dir)) return [];
  return fs
    .readdirSync(dir)
    .filter((f) => f.endsWith('.json'))
    .map((f) => f.replace(/\.json$/, ''));
}

/** 所有作品 id，供 generateStaticParams 使用 */
export function getAllWorkIds(): string[] {
  const dir = path.join(DERIVED, 'work');
  if (!fs.existsSync(dir)) return [];
  return fs
    .readdirSync(dir)
    .filter((f) => f.endsWith('.json'))
    .map((f) => f.replace(/\.json$/, ''));
}

/** 数据集是否已构建 */
export function isAtlasBuilt(): boolean {
  return fs.existsSync(path.join(DERIVED, 'manifest.json'));
}

// ─────────────────────────────────────────────
// 展示辅助
// ─────────────────────────────────────────────

/** 把断言按 groupId 聚合为「异说组」 */
export function groupAssertions(assertions: Assertion[]): AssertionGroup[] {
  const map = new Map<string, Assertion[]>();
  for (const a of assertions) {
    const key = a.groupId ?? `solo-${a.id}`;
    const list = map.get(key) ?? [];
    list.push(a);
    map.set(key, list);
  }
  return [...map.entries()].map(([key, items]) => {
    // 主流说法优先，其次按置信度从高到低
    const order: Record<Confidence, number> = { A: 0, B: 1, C: 2, D: 3 };
    const sorted = [...items].sort((x, y) => {
      if (x.isPrimary !== y.isPrimary) return x.isPrimary ? -1 : 1;
      return order[x.confidence] - order[y.confidence];
    });
    return { key, contested: items.length > 1, items: sorted };
  });
}
