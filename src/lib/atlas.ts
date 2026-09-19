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
  /** 该作品的古画配图（构建期由 data/art/ 烘焙进来），无则 undefined */
  art?: WorkArt | null;
}

/**
 * 作品配图。
 *
 * 数据来源：大都会艺术博物馆 Open Access（`isPublicDomain: true` = CC0），
 * 可商用。匹配逻辑与素材清单见 `poetry-atlas/data/art/`。
 * `src` / `srcSmall` 已是站点内相对路径，位图存在 `public/poetry-atlas/art/`。
 */
export interface WorkArt {
  /** 大都会藏品编号 */
  objectID: number;
  /** 画作题名（英文著录） */
  artTitle: string;
  /** 作者，佚名则为空串 */
  artArtist: string;
  /** 年代著录 */
  artDate: string;
  /** 收藏来源（credit line），署名时必须一并展示 */
  artCredit: string;
  /** 匹配依据的意境标签，如 ["水","夜"] */
  motifs: string[];
  /** 站点内主图路径 */
  src: string;
  /** 站点内缩略图路径 */
  srcSmall?: string;
  /** 大都会藏品页，用于「查看原件」外链 */
  artURL?: string;
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

/**
 * 取作品配图，并把「派生层里的原始字段」翻译成前端要用的形态。
 *
 * 为什么不在 build.ts 里直接写好 src：
 *   派生数据是「内容层」，不该关心站点路由与 public 目录布局；
 *   路径拼接属于「展示层」，放在这里，将来换 CDN 或改目录只动一处。
 */
export function getArtForWork(workId: string): WorkArt | null {
  const work = getWorkDetail(workId);
  const raw = work?.art as
    | (Omit<WorkArt, 'src' | 'srcSmall'> & { objectID: number })
    | undefined
    | null;
  if (!raw?.objectID) return null;
  return {
    objectID: raw.objectID,
    artTitle: raw.artTitle,
    artArtist: raw.artArtist,
    artDate: raw.artDate,
    artCredit: raw.artCredit,
    motifs: raw.motifs ?? [],
    src: `${ATLAS_ART_BASE}/${raw.objectID}.webp`,
    srcSmall: `${ATLAS_ART_BASE}/${raw.objectID}-sm.webp`,
    artURL: raw.artURL,
  };
}

/**
 * 配图位图在 public 下的目录。
 *
 * ⚠️ 这里必须**手动**拼上 basePath，不能指望 Next 自动加。
 * 原因：`next/image` 会拼 basePath，但配图层用的是裸 `<img>`（为了
 * 配合 CSS filter 做拓片效果，且已 `images.unoptimized`），
 * 而裸 `<img src>` 的字符串 Next 完全不会处理。
 * 站内已有的 `/images/hello-world.jpg` 就是这么漏掉 `/blog` 前缀的，
 * 生产环境一直是 404 —— 同一类坑。
 *
 * 站点部署在 https://<user>.github.io/blog/，所以前缀是 `/blog`。
 */
const BASE_PATH = '/blog';
const ATLAS_ART_BASE = `${BASE_PATH}/poetry-atlas/art`;

/**
 * 策展位取图。
 *
 * 全站有三处「非作品页」也需要古画：图谱主页主视觉、诗人页头图、
 * 博客首页入口卡。它们不对应任何 workId，所以不能走 getArtForWork，
 * 而是从素材清单里按 `featured` 槽位取 —— 槽位由人工指定，
 * 保证主视觉是一张经得起细看的画，而不是算法凑出来的。
 */
export function getFeaturedArt(slot: string): WorkArt | null {
  const catalog = readDerived<
    {
      objectID: number;
      title: string;
      artist: string;
      date: string;
      creditLine: string;
      motifs: string[];
      objectURL?: string;
      featured?: string;
    }[]
  >('art-catalog.json', []);
  const p = catalog.find((x) => x.featured === slot);
  if (!p) return null;
  return {
    objectID: p.objectID,
    artTitle: p.title,
    artArtist: p.artist,
    artDate: p.date,
    artCredit: p.creditLine,
    motifs: p.motifs ?? [],
    src: `${ATLAS_ART_BASE}/${p.objectID}.webp`,
    srcSmall: `${ATLAS_ART_BASE}/${p.objectID}-sm.webp`,
    artURL: p.objectURL,
  };
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
