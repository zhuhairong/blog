/**
 * 构建管线：考据数据 → 静态 JSON 分片
 *
 * 因为站点是 output:'export' 纯静态导出，没有服务端，
 * 所以所有查询能力必须在构建期"烘焙"成静态资源，前端按需 fetch。
 *
 * 产物（poetry-atlas/data/derived/）：
 *   manifest.json          总清单与统计
 *   map-points.json        地图打点（按作品数排序）
 *   poets-index.json       诗人列表（轻量，供索引页）
 *   poet/<id>.json         诗人详情（含作品、事件、轨迹）
 *   work/<id>.json         作品详情（含全部异说断言）
 *   timeline.json          时间轴聚合（按十年分桶）
 *   themes.json            主题标签聚合
 *
 * 运行：npx tsx poetry-atlas/src/pipeline/build.ts
 */

import fs from 'fs';
import path from 'path';
import { fileURLToPath } from 'url';
import {
  loadDataset,
  validate,
  report,
  type DatasetBundle,
} from './validators/validate';
import type {
  Assertion,
  BuildManifest,
  Confidence,
  DerivedMapPoint,
  DerivedTrajectory,
  LifeEvent,
  LifeEventType,
  Place,
  Poet,
  Work,
} from '../types/index';

/** 输出目录，基于本文件位置推导，保证与调用目录无关 */
const HERE = path.dirname(fileURLToPath(import.meta.url));
const OUT = path.resolve(HERE, '..', '..', 'data', 'derived');

/** 确保输出目录干净（只清 derived，绝不碰其他目录） */
function prepareOut() {
  if (fs.existsSync(OUT)) {
    fs.rmSync(OUT, { recursive: true, force: true });
  }
  fs.mkdirSync(OUT, { recursive: true });
}

function writeJson(relPath: string, data: unknown) {
  const full = path.join(OUT, relPath);
  fs.mkdirSync(path.dirname(full), { recursive: true });
  fs.writeFileSync(full, JSON.stringify(data, null, 2), 'utf8');
  const kb = (fs.statSync(full).size / 1024).toFixed(1);
  console.log(`  ${relPath.padEnd(28)} ${kb.padStart(8)} KB`);
}

/** 按作品聚合出地图打点（含诗人籍贯与生平事件，三者共同构成"地点价值"） */
function buildMapPoints(
  places: Place[],
  assertions: Assertion[],
  poets: Poet[],
): DerivedMapPoint[] {
  const byPlace = new Map<string, Assertion[]>();
  for (const a of assertions) {
    if (!a.placeId) continue;
    const list = byPlace.get(a.placeId) ?? [];
    list.push(a);
    byPlace.set(a.placeId, list);
  }

  /** 籍贯 / 生平事件：这些地点即使暂无作品，也值得上图 */
  const bornPlaces = new Set<string>();
  const eventPlaces = new Set<string>();
  for (const p of poets) {
    for (const n of p.nativePlace ?? []) bornPlaces.add(n.placeId);
    for (const e of p.events ?? []) if (e.placeId) eventPlaces.add(e.placeId);
  }

  const points: DerivedMapPoint[] = [];
  for (const place of places) {
    const list = byPlace.get(place.id) ?? [];
    const isBirth = bornPlaces.has(place.id);
    const isEvent = eventPlaces.has(place.id);
    // 无作品、非籍贯、无生平事件的地点不上图（避免地名词表虚胖）
    if (!list.length && !isBirth && !isEvent) continue;

    const byConfidence: Record<Confidence, number> = { A: 0, B: 0, C: 0, D: 0 };
    const poetIds = new Set<string>();
    let minYear = Infinity;
    let maxYear = -Infinity;

    for (const a of list) {
      byConfidence[a.confidence] = (byConfidence[a.confidence] ?? 0) + 1;
      poetIds.add(a.poetId);
      const y = a.date?.year ?? a.dateRange?.from.year;
      if (typeof y === 'number') {
        minYear = Math.min(minYear, y);
        maxYear = Math.max(maxYear, y);
      }
    }

    // 籍贯/事件地点也要归入诗人，否则地图侧栏点开是空的
    for (const p of poets) {
      const hit =
        (p.nativePlace ?? []).some((n) => n.placeId === place.id) ||
        (p.events ?? []).some((e) => e.placeId === place.id);
      if (hit) poetIds.add(p.id);
    }

    points.push({
      placeId: place.id,
      modernName: place.modernName,
      historicalName: place.historicalName,
      coordinates: place.coordinates,
      precision: place.precision,
      isCentroid: place.isCentroid,
      workCount: list.length,
      isBirthplace: isBirth,
      eventCount: (poets ?? []).reduce(
        (n, p) => n + (p.events ?? []).filter((e) => e.placeId === place.id).length,
        0,
      ),
      byConfidence,
      poetIds: [...poetIds],
      yearRange: [
        minYear === Infinity ? 0 : minYear,
        maxYear === -Infinity ? 0 : maxYear,
      ],
    });
  }

  return points.sort((a, b) => b.workCount - a.workCount);
}

/**
 * 为诗人构建轨迹线（只取有地点、有时间的断言 + 事件）。
 *
 * 只负责 `waypoints`；`stops` / `unplacedWorkCount` 由 `buildJourney` 另算，
 * 由调用方拼成完整的 DerivedTrajectory。
 */
function buildTrajectory(
  poet: Poet,
  assertions: Assertion[],
): Omit<DerivedTrajectory, 'stops' | 'unplacedWorkCount'> {
  const waypoints: DerivedTrajectory['waypoints'] = [];
  const seen = new Set<string>();

  for (const e of poet.events ?? []) {
    if (!e.placeId) continue;
    const date = 'year' in e.date ? e.date : e.date.from;
    const key = `${e.placeId}-${date.year}`;
    if (seen.has(key)) continue;
    seen.add(key);
    waypoints.push({ placeId: e.placeId, date, eventId: e.id });
  }

  for (const a of assertions) {
    if (!a.placeId || !a.date) continue;
    const key = `${a.placeId}-${a.date.year}`;
    if (seen.has(key)) continue;
    seen.add(key);
    waypoints.push({ placeId: a.placeId, date: a.date });
  }

  waypoints.sort((x, y) => x.date.year - y.date.year);
  return { poetId: poet.id, waypoints };
}

/** 取事件的起始年（区间取 from） */
function yearOf(e: LifeEvent): number {
  return 'year' in e.date ? e.date.year : e.date.from.year;
}

/** 取事件的结束年（区间取 to，单点即当年） */
function endYearOf(e: LifeEvent): number {
  return 'year' in e.date ? e.date.year : e.date.to.year;
}

/** 同地相邻点合并的年容差 */
const CLUSTER_GAP = 2;

/**
 * 构建「一生行迹」站点。
 *
 * 关键设计：**站点由「生平事件 ∪ 作品系年」共同聚类而成**，不是只取事件。
 *
 * 为什么不只用事件：年谱不可能记全每一次行止。李白 748 年在扬州作
 * 《闻王昌龄左迁龙标遥有此寄》、753 年在宣城作《独坐敬亭山》，
 * 这些年份在事件表里没有对应条目；若只按事件建站，这些诗就只能
 * 「硬塞」进时间最近的站点（把 748 年的诗挂到 725 年的扬州站上），
 * 那是实打实的错误陈述。让作品自己补出站点，才是诚实的做法——
 * 前端用 `source` 字段把「年谱有载」与「据作品推知」分开标注。
 *
 * 聚类规则：按年排序后，与当前簇**同地且年距 ≤ CLUSTER_GAP** 则并入。
 * 于是 742/743/744 长安并成一站（「长安三年」），而 730 年的长安因中间
 * 隔着嵩山另立一站（「一入长安」）。卒后事件（迁葬、追谥）另行成站。
 */
function buildJourney(
  poet: Poet,
  works: Work[],
  assertions: Assertion[],
  places: Place[],
): { stops: DerivedTrajectory['stops']; unplacedWorkCount: number } {
  const placeById = new Map(places.map((p) => [p.id, p]));
  const deathYear = poet.death?.year ?? null;
  const isPosthumous = (y: number) => deathYear !== null && y > deathYear;

  /** 时间轴上的一个「落点」，来源可以是事件，也可以是作品的系年 */
  interface Point {
    placeId: string;
    year: number;
    toYear: number;
    event?: LifeEvent;
    assertion?: Assertion;
    work?: Work;
  }

  const points: Point[] = [];
  for (const e of poet.events ?? []) {
    if (!e.placeId) continue;
    points.push({
      placeId: e.placeId,
      year: yearOf(e),
      toYear: endYearOf(e),
      event: e,
    });
  }
  /**
   * 作品落点**只取主流断言**（`isPrimary`）。
   *
   * 不能把互斥异说都当落点：李白《闻王昌龄左迁龙标遥有此寄》有「扬州」与
   * 「金陵」两说，若两说各建一站，地图上就会凭空多出一次并不存在的行止。
   * 异说是「同一件事的不同说法」，不是「多去了一趟」——这正是本数据模型
   * 用 groupId 归组、isPrimary 标主流的用意。
   */
  const workById = new Map(works.map((w) => [w.id, w]));
  const seenPoint = new Set<string>();
  for (const a of assertions) {
    if (!a.isPrimary) continue;
    const y = a.date?.year ?? a.dateRange?.from.year;
    if (!a.placeId || typeof y !== 'number') continue;
    const key = `${a.placeId}|${y}`;
    if (seenPoint.has(key)) continue;
    seenPoint.add(key);
    points.push({
      placeId: a.placeId,
      year: y,
      toYear: a.dateRange?.to.year ?? y,
      assertion: a,
      work: workById.get(a.workId),
    });
  }
  points.sort((p, q) => p.year - q.year || p.toYear - q.toYear);

  // ── 1. 初次聚类：同地且年距 ≤ CLUSTER_GAP 则并入 ──
  let clusters: Point[][] = [];
  for (const p of points) {
    const cur = clusters[clusters.length - 1];
    if (cur) {
      const head = cur[0]!;
      const maxEnd = Math.max(...cur.map((c) => c.toYear));
      const samePlace = head.placeId === p.placeId;
      const contiguous = p.year <= maxEnd + CLUSTER_GAP;
      const sameEra = isPosthumous(head.year) === isPosthumous(p.year);
      if (samePlace && contiguous && sameEra) {
        cur.push(p);
        continue;
      }
    }
    clusters.push([p]);
  }

  /**
   * ── 2. 二次合并：同一地点的簇必须并成一个 ──
   *
   * 初次聚类是**单遍顺序扫描**，会被夹在中间的别处落点打断：
   * 李白 742–743 在长安、743 年在长安兴庆宫、744 年又在长安，
   * 于是「长安三年」被兴庆宫切成两段；724 年峨眉的事件与作品断言之间
   * 夹了一次夷陵，同地同年也裂成两站。
   *
   * 故补一轮：同地、同身后属性、且「时间窗重叠或相距 ≤ CLUSTER_GAP」
   * 或「在序列上相邻」的两个簇，一律合并，直到不再变化。
   * 相邻即并这一条专治「同地但年份有缺口」的情形——
   * 李白 701 年生于绵州昌隆、705–718 年仍在此读书，中间没有去过别处，
   * 就该是一站（701–718）而不是两站。
   */
  const windowGap = (a: Point[], b: Point[]) => {
    const aFrom = Math.min(...a.map((x) => x.year));
    const aTo = Math.max(...a.map((x) => x.toYear));
    const bFrom = Math.min(...b.map((x) => x.year));
    const bTo = Math.max(...b.map((x) => x.toYear));
    if (bFrom <= aTo && aFrom <= bTo) return 0; // 重叠
    return bFrom > aTo ? bFrom - aTo : aFrom - bTo;
  };

  for (let changed = true; changed; ) {
    changed = false;
    outer: for (let i = 0; i < clusters.length; i += 1) {
      for (let j = i + 1; j < clusters.length; j += 1) {
        const a = clusters[i]!;
        const b = clusters[j]!;
        if (a[0]!.placeId !== b[0]!.placeId) continue;
        const aMin = Math.min(...a.map((x) => x.year));
        const bMin = Math.min(...b.map((x) => x.year));
        if (isPosthumous(aMin) !== isPosthumous(bMin)) continue;
        const adjacent = j === i + 1;
        if (!adjacent && windowGap(a, b) > CLUSTER_GAP) continue;
        clusters[i] = [...a, ...b];
        clusters.splice(j, 1);
        changed = true;
        break outer;
      }
    }
  }

  // 合并后重新按时间排序，保证编号即行迹顺序
  clusters = clusters
    .map((pts) => pts.sort((p, q) => p.year - q.year))
    .sort((a, b) => a[0]!.year - b[0]!.year);

  // ── 3. 簇 → 站点 ──
  const CONF_RANK = { A: 0, B: 1, C: 2, D: 3 } as const;
  const stops: DerivedTrajectory['stops'] = clusters.map((pts, i) => {
    const placeId = pts[0]!.placeId;
    const place = placeById.get(placeId);
    const evs = pts.map((x) => x.event).filter((x): x is LifeEvent => Boolean(x));
    const asrts = pts.map((x) => x.assertion).filter((x): x is Assertion => Boolean(x));

    const types: LifeEventType[] = [];
    for (const e of evs) if (!types.includes(e.type)) types.push(e.type);
    if (asrts.length && !types.includes('creation')) types.push('creation');

    let confidence: Confidence = 'A';
    for (const e of evs) {
      if (CONF_RANK[e.confidence] > CONF_RANK[confidence]) confidence = e.confidence;
    }
    for (const a of asrts) {
      if (CONF_RANK[a.confidence] > CONF_RANK[confidence]) confidence = a.confidence;
    }

    const fromYear = Math.min(...pts.map((x) => x.year));
    const toYear = Math.max(...pts.map((x) => x.toYear));

    // 标题：有事件用事件，纯作品站则点出代表作品
    let title: string;
    if (evs.length) {
      title = evs.length > 1 ? `${evs[0]!.title} 等 ${evs.length} 事` : evs[0]!.title;
    } else {
      const rep = asrts.find((a) => a.isPrimary) ?? asrts[0]!;
      const t = workById.get(rep.workId)?.title ?? rep.workId;
      title = asrts.length > 1 ? `作《${t}》等 ${asrts.length} 篇` : `作《${t}》`;
    }

    const description = evs.length
      ? evs.map((e) => e.description).join(' ')
      : asrts
          .map((a) => a.circumstance ?? a.rationale)
          .filter(Boolean)
          .join(' ');

    return {
      seq: i + 1,
      placeId,
      historicalName: place?.historicalName ?? placeId,
      modernName: place?.modernName ?? '',
      coordinates: place?.coordinates ?? ([0, 0] as [number, number]),
      precision: place?.precision ?? 'approximate',
      isCentroid: place?.isCentroid ?? true,
      fromYear,
      toYear,
      eventIds: evs.map((e) => e.id),
      title,
      types,
      confidence,
      description,
      // ⚠️ `nativePlace` 里混着两类条目：`birth` 是真生地，`ancestral` 是郡望/祖籍
      // （数据里的 note 自己写明「非实际生地」）。早先这里不分类型一律算「出生地」，
      // 结果杜甫的长安（京兆杜氏郡望）、李白的安陆（寓居地）都被标成了出生地。
      // 两者必须分开，前端也分开显示。
      isBirthplace:
        evs.some((e) => e.type === 'birth') ||
        (poet.nativePlace ?? []).some(
          (n) => n.placeId === placeId && n.type === 'birth',
        ),
      isAncestralPlace: (poet.nativePlace ?? []).some(
        (n) => n.placeId === placeId && n.type === 'ancestral',
      ),
      isDeathPlace: evs.some((e) => e.type === 'death'),
      posthumous: isPosthumous(fromYear),
      source: evs.length === 0 ? 'work' : asrts.length ? 'mixed' : 'event',
      works: [],
      nearbyWorks: [],
    };
  });

  // ── 3. 挂作品 ──
  const byPlace = new Map<string, typeof stops>();
  for (const s of stops) {
    byPlace.set(s.placeId, [...(byPlace.get(s.placeId) ?? []), s]);
  }

  let unplacedWorkCount = 0;
  for (const w of works) {
    const list = assertions.filter((a) => a.workId === w.id);
    const primary = list.find((a) => a.isPrimary) ?? list[0];
    const placeId = primary?.placeId ?? null;
    const year = primary?.date?.year ?? primary?.dateRange?.from.year ?? null;

    const ref = {
      workId: w.id,
      title: w.title,
      year: year ?? null,
      form: w.form,
      famousLine: w.famousLines?.[0],
    };

    // ① 创作地即某站，且系年落在该站时间窗内 —— 强关联
    if (placeId && byPlace.has(placeId)) {
      const cands = byPlace.get(placeId)!;
      const hit =
        year === null
          ? cands[0]!
          : cands.find((s) => year >= s.fromYear && year <= s.toYear);
      if (hit) {
        hit.works.push(ref);
        continue;
      }
    }

    // ② 创作地另载或未系地，但系年落在某站时间窗内 —— 弱关联，须明示
    if (year !== null) {
      const host = stops.find(
        (s) => !s.posthumous && year >= s.fromYear - 1 && year <= s.toYear + 1,
      );
      if (host) {
        host.nearbyWorks.push({
          ...ref,
          placeName: placeId ? (placeById.get(placeId)?.historicalName ?? null) : null,
        });
        continue;
      }
    }
    unplacedWorkCount += 1;
  }

  // 站内作品按年排序，无年者置后
  const byYear = (a: { year: number | null }, b: { year: number | null }) =>
    (a.year ?? 9999) - (b.year ?? 9999);
  for (const s of stops) {
    s.works.sort(byYear);
    s.nearbyWorks.sort(byYear);
  }

  return { stops, unplacedWorkCount };
}

/** 按十年分桶的时间轴 */
function buildTimeline(assertions: Assertion[]) {
  const buckets = new Map<number, { count: number; workIds: string[]; poetIds: string[] }>();
  for (const a of assertions) {
    const y = a.date?.year;
    if (typeof y !== 'number') continue;
    const decade = Math.floor(y / 10) * 10;
    const b = buckets.get(decade) ?? { count: 0, workIds: [], poetIds: [] };
    b.count += 1;
    b.workIds.push(a.workId);
    if (!b.poetIds.includes(a.poetId)) b.poetIds.push(a.poetId);
    buckets.set(decade, b);
  }
  return [...buckets.entries()]
    .map(([decade, v]) => ({ decade, ...v }))
    .sort((a, b) => a.decade - b.decade);
}

function main() {
  console.log('╔════════════════════════════════════════════╗');
  console.log('║  中国诗歌时空图谱 · 构建管线               ║');
  console.log('╚════════════════════════════════════════════╝\n');

  const ds: DatasetBundle = loadDataset();
  console.log('数据载入：');
  console.log(`  诗人 ${ds.poets.length} · 作品 ${ds.works.length} · 地点 ${ds.places.length} · 断言 ${ds.assertions.length} · 事件 ${ds.events.length}\n`);

  const issues = validate(ds);
  const ok = report(issues);
  if (!ok) {
    console.error('\n✗ 存在阻断性错误，构建中止。请修复后重试。');
    process.exit(1);
  }

  console.log('\n生成派生产物：');
  prepareOut();

  const mapPoints = buildMapPoints(ds.places, ds.assertions, ds.poets);

  // 诗人索引（轻量）
  const poetsIndex = ds.poets.map((p) => ({
    id: p.id,
    name: p.name,
    dynasty: p.dynasty,
    courtesyName: p.courtesyName,
    aliases: p.aliases,
    birthYear: p.birth?.year ?? null,
    deathYear: p.death?.year ?? null,
    summary: p.summary,
    workCount: ds.works.filter((w) => w.poetId === p.id).length,
    locatedWorkCount: new Set(
      ds.assertions.filter((a) => a.poetId === p.id && a.placeId).map((a) => a.workId),
    ).size,
  }));
  writeJson('poets-index.json', poetsIndex);

  writeJson('map-points.json', mapPoints);
  writeJson('timeline.json', buildTimeline(ds.assertions));

  // placeId → 该地诞生的作品。前端地图侧栏直接消费，避免反查。
  const placeWorks: Record<
    string,
    { workId: string; title: string; poetId: string; poetName: string; year: number | null }[]
  > = {};
  for (const a of ds.assertions) {
    if (!a.placeId) continue;
    const work = ds.works.find((w) => w.id === a.workId);
    const poet = ds.poets.find((p) => p.id === a.poetId);
    if (!work || !poet) continue;
    const list = placeWorks[a.placeId] ?? [];
    if (!list.some((x) => x.workId === work.id)) {
      list.push({
        workId: work.id,
        title: work.title,
        poetId: poet.id,
        poetName: poet.name,
        year: a.date?.year ?? a.dateRange?.from.year ?? null,
      });
    }
    placeWorks[a.placeId] = list;
  }
  writeJson('place-works.json', placeWorks);

  // 作品 → 同期同地作品索引。
  // 这是「串联」的关键：一首诗不是孤立的，它总有邻居——
  // 同一段人生、同一个地方写下的其他作品。前端据此把作品织成网。
  const relatedWorks: Record<
    string,
    {
      workId: string;
      title: string;
      poetId: string;
      poetName: string;
      year: number | null;
      placeId: string | null;
      placeName: string | null;
      /** 关联类型：同期 / 同地 / 同事 */
      reason: 'same-place' | 'same-time' | 'same-event';
    }[]
  > = {};

  /** 每篇作品取其「主流断言」作为时空坐标，无主流则取第一条 */
  const primaryOf = new Map<string, (typeof ds.assertions)[number]>();
  for (const w of ds.works) {
    const list = ds.assertions.filter((a) => a.workId === w.id);
    if (!list.length) continue;
    primaryOf.set(w.id, list.find((a) => a.isPrimary) ?? list[0]!);
  }

  const placeName = (id?: string | null) =>
    id ? (ds.places.find((p) => p.id === id)?.historicalName ?? null) : null;

  for (const w of ds.works) {
    const self = primaryOf.get(w.id);
    const out: (typeof relatedWorks)[string] = [];

    for (const other of ds.works) {
      if (other.id === w.id) continue;
      const o = primaryOf.get(other.id);
      if (!o) continue;

      const selfYear = self?.date?.year ?? self?.dateRange?.from.year ?? null;
      const otherYear = o.date?.year ?? o.dateRange?.from.year ?? null;

      let reason: 'same-place' | 'same-time' | 'same-event' | null = null;

      // 优先生命事件关联：写于同一段人生际遇下的作品，关联最紧
      if (self?.triggerEventId && self.triggerEventId === o.triggerEventId) {
        reason = 'same-event';
      } else if (self?.placeId && self.placeId === o.placeId) {
        reason = 'same-place';
      } else if (selfYear !== null && otherYear !== null && Math.abs(selfYear - otherYear) <= 2) {
        reason = 'same-time';
      }



      if (!reason) continue;

      out.push({
        workId: other.id,
        title: other.title,
        poetId: other.poetId,
        poetName: ds.poets.find((p) => p.id === other.poetId)?.name ?? other.poetId,
        year: otherYear,
        placeId: o.placeId ?? null,
        placeName: placeName(o.placeId),
        reason,
      });

      if (out.length >= 6) break;
    }

    // 同一事件 / 同一地点 优先展示
    const rank = { 'same-event': 0, 'same-place': 1, 'same-time': 2 } as const;
    out.sort((x, y) => rank[x.reason] - rank[y.reason]);
    relatedWorks[w.id] = out;
  }
  writeJson('related-works.json', relatedWorks);

  // 诗人 → 交游网络。李白一生交友极广，这是理解他诗作的重要维度。
  const poetNetwork: Record<
    string,
    { poetId: string; name: string; note: string; sourceIds: string[] }[]
  > = {};
  for (const poet of ds.poets) {
    const edges = new Map<string, { note: string; sourceIds: string[] }>();
    for (const a of ds.assertions.filter((x) => x.poetId === poet.id)) {
      for (const rid of a.relatedPoetIds ?? []) {
        if (!edges.has(rid)) {
          edges.set(rid, {
            note: '同见于作品断言',
            sourceIds: a.sources.map((s) => s.id),
          });
        }
      }
    }
    poetNetwork[poet.id] = [...edges.entries()].map(([rid, v]) => ({
      poetId: rid,
      name: ds.poets.find((p) => p.id === rid)?.name ?? rid,
      note: v.note,
      sourceIds: [...new Set(v.sourceIds)],
    }));
  }
  writeJson('poet-network.json', poetNetwork);

  // 主题聚合
  const themeMap = new Map<string, { workIds: string[]; poetIds: string[] }>();
  for (const w of ds.works) {
    for (const t of w.themes ?? []) {
      const e = themeMap.get(t) ?? { workIds: [], poetIds: [] };
      e.workIds.push(w.id);
      if (!e.poetIds.includes(w.poetId)) e.poetIds.push(w.poetId);
      themeMap.set(t, e);
    }
  }
  writeJson(
    'themes.json',
    [...themeMap.entries()]
      .map(([theme, v]) => ({ theme, workCount: v.workIds.length, ...v }))
      .sort((a, b) => b.workCount - a.workCount),
  );

  /**
   * ── 配图索引 ──
   *
   * 站点的插画分两条腿：
   *   1. `scene.ts` 的程序化 SVG（确定性、随主题变色）——用于生平事件、诗人页轨迹；
   *   2. 这里的公版古画位图——用于诗框背景，因为它「精美」，而这正是 SVG 做不到的。
   *
   * 位图一旦入库就不可变，所以「哪首诗配哪幅画」必须在构建期定死并落盘，
   * 不能放到前端随机挑（否则每次刷新图都变，且无法被缓存）。
   * 匹配逻辑由 `tools/art-match.ts` 离线算好后写入 `data/art/work-art.json`，
   * 这里只做「数据存在与否」的兼容处理：没有配图数据时静默跳过，
   * 让站点退化成纯 SVG 版，而不是构建失败。
   */
  const artDir = path.resolve(HERE, '..', '..', 'data', 'art');
  const artByWork = new Map<string, unknown>();
  let artCatalog: unknown[] = [];
  let artMatches: { workId: string }[] = [];
  try {
    const catalogFile = path.join(artDir, 'art-catalog.json');
    const matchFile = path.join(artDir, 'work-art.json');
    if (fs.existsSync(catalogFile) && fs.existsSync(matchFile)) {
      artCatalog = JSON.parse(fs.readFileSync(catalogFile, 'utf8'));
      artMatches = JSON.parse(fs.readFileSync(matchFile, 'utf8'));
      for (const m of artMatches) artByWork.set(m.workId, m);
      writeJson('art-catalog.json', artCatalog);
      writeJson('work-art.json', artMatches);
      console.log(`  ${'配图'.padEnd(28)} ${String(artMatches.length).padStart(8)} 首`);
    } else {
      console.log('  （无配图数据，跳过；插画回退为程序化 SVG）');
    }
  } catch (e) {
    console.log('  ! 配图数据解析失败，已跳过：', (e as Error).message);
  }

  // 逐诗人 / 逐作品详情分片
  let poetFiles = 0;
  let workFiles = 0;
  for (const poet of ds.poets) {
    const works = ds.works.filter((w) => w.poetId === poet.id);
    const poetAssertions = ds.assertions.filter((a) => a.poetId === poet.id);
    const placeIdList: string[] = [
      ...poetAssertions.map((a) => a.placeId).filter((id): id is string => Boolean(id)),
      ...(poet.events ?? [])
        .map((e) => e.placeId)
        .filter((id): id is string => Boolean(id)),
      // ⚠️ 籍贯（出生地/祖籍）也必须进来。
      //    否则 poet.places 里没有对应地点，前端只能显示裸 placeId，
      //    「祖籍赵郡栾城」会显示成「祖籍 cn-hebei-luancheng」。
      ...(poet.nativePlace ?? []).map((n) => n.placeId),
    ];
    const placeIds = new Set(placeIdList);

    const journey = buildJourney(poet, works, poetAssertions, ds.places);

    writeJson(`poet/${poet.id}.json`, {
      ...poet,
      works,
      assertions: poetAssertions,
      places: ds.places.filter((p) => placeIds.has(p.id)),
      trajectory: {
        ...buildTrajectory(poet, poetAssertions),
        stops: journey.stops,
        unplacedWorkCount: journey.unplacedWorkCount,
      },
    });
    poetFiles += 1;
  }

  for (const work of ds.works) {
    const assertions = ds.assertions.filter((a) => a.workId === work.id);
    const placeIds = new Set(assertions.map((a) => a.placeId).filter(Boolean) as string[]);
    writeJson(`work/${work.id}.json`, {
      ...work,
      assertions,
      places: ds.places.filter((p) => placeIds.has(p.id)),
      // 该作品的配图（公版古画）。无则为 undefined，前端回退到 SVG 插画。
      art: artByWork.get(work.id),
    });
    workFiles += 1;
  }

  const manifest: BuildManifest = {
    generatedAt: new Date().toISOString(),
    counts: {
      poets: ds.poets.length,
      works: ds.works.length,
      places: ds.places.length,
      assertions: ds.assertions.length,
    },
    warnings: issues.filter((i) => i.severity !== 'info'),
  };
  writeJson('manifest.json', manifest);

  const total = fs
    .readdirSync(OUT, { recursive: true })
    .filter((f) => String(f).endsWith('.json')).length;

  console.log(`\n✓ 构建完成：${total} 个 JSON 分片（诗人详情 ${poetFiles}，作品详情 ${workFiles}）`);
  console.log(`  输出目录：poetry-atlas/data/derived/`);
}

// 仅在直接运行时执行（被 import 时不自执行）
const isMain =
  process.argv[1] && fileURLToPath(import.meta.url).endsWith(path.basename(process.argv[1]));
if (isMain) main();

export { main as buildAtlas };
