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
  Place,
  Poet,
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

/** 为诗人构建轨迹线（只取有地点、有时间的断言 + 事件） */
function buildTrajectory(
  poet: Poet,
  assertions: Assertion[],
): DerivedTrajectory {
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
    ];
    const placeIds = new Set(placeIdList);

    writeJson(`poet/${poet.id}.json`, {
      ...poet,
      works,
      assertions: poetAssertions,
      places: ds.places.filter((p) => placeIds.has(p.id)),
      trajectory: buildTrajectory(poet, poetAssertions),
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
