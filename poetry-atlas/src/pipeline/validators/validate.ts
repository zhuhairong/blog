/**
 * 数据校验器
 *
 * 目标：在构建期拦截脏数据，保证随数据集增长质量不塌陷。
 * 运行：npx tsx poetry-atlas/src/pipeline/validate.ts
 *
 * 校验分三级：
 *   error   — 阻断构建，必须修复（如引用不存在的地点、断言无来源）
 *   warning — 允许构建，但需记录（如缺坐标、置信度偏低）
 *   info    — 提示性统计（如某诗人作品过少）
 */

import fs from 'fs';
import path from 'path';
import { fileURLToPath } from 'url';
import type {
  Assertion,
  Place,
  Poet,
  ValidationIssue,
  Work,
  LifeEvent,
} from '../../types/index';

/**
 * 数据根目录。基于本文件位置推导，而非 process.cwd()，
 * 这样无论从哪个目录调用脚本都能正确定位，避免"静默加载 0 条"的陷阱。
 * 路径：src/pipeline/validators/ → 上溯三级 → poetry-atlas/data
 */
const HERE = path.dirname(fileURLToPath(import.meta.url));
const ROOT = path.resolve(HERE, '..', '..', '..', 'data');

/** 读取目录下所有 JSON 文件，容忍目录不存在 */
function readJsonDir<T>(dir: string): T[] {
  const full = path.join(ROOT, dir);
  if (!fs.existsSync(full)) return [];
  return fs
    .readdirSync(full)
    .filter((f) => f.endsWith('.json'))
    .flatMap((f) => {
      const raw = fs.readFileSync(path.join(full, f), 'utf8');
      const parsed = JSON.parse(raw);
      // 支持两种格式：单个对象数组，或 { items: [...] }
      const items = Array.isArray(parsed) ? parsed : (parsed.items ?? []);
      return items as T[];
    });
}

export interface DatasetBundle {
  places: Place[];
  poets: Poet[];
  works: Work[];
  assertions: Assertion[];
  events: LifeEvent[];
}

export function loadDataset(): DatasetBundle {
  const poets = readJsonDir<Poet>('people');
  const works = readJsonDir<Work>('corpus');
  const places = readJsonDir<Place>('places');
  const assertions = readJsonDir<Assertion>('assertions');
  // 生平事件内嵌在诗人文件中，此处展平以便统一校验
  const events = poets.flatMap((p) => p.events ?? []);
  return { places, poets, works, assertions, events };
}

/** 校验主入口 */
export function validate(ds: DatasetBundle): ValidationIssue[] {
  const issues: ValidationIssue[] = [];

  // ── 空数据集守卫 ──
  // 空数据集"校验通过"是危险的假信号：它意味着路径写错却无人察觉。
  // 因此数据为空时必须直接报错，而不是安静地成功。
  if (ds.poets.length === 0 && ds.works.length === 0 && ds.places.length === 0) {
    issues.push({
      severity: 'error',
      code: 'empty-dataset',
      entity: { type: 'dataset', id: 'root' },
      message: `未载入任何数据（数据根目录：${ROOT}）`,
      hint: '检查 data/ 目录是否存在且含 JSON 文件；路径推导错误会导致静默加载 0 条',
    });
    return issues;
  }

  const poetIds = new Set(ds.poets.map((p) => p.id));
  const workIds = new Set(ds.works.map((w) => w.id));
  const placeIds = new Set(ds.places.map((p) => p.id));
  const eventIds = new Set(ds.events.map((e) => e.id));

  const push = (i: ValidationIssue) => issues.push(i);

  // ── 标识唯一性 ──
  const checkUnique = (list: { id: string }[], type: string) => {
    const seen = new Set<string>();
    for (const item of list) {
      if (seen.has(item.id)) {
        push({
          severity: 'error',
          code: 'duplicate-id',
          entity: { type, id: item.id },
          message: `${type} id 重复：${item.id}`,
          hint: 'id 必须全局唯一，检查是否有文件重复定义',
        });
      }
      seen.add(item.id);
    }
  };
  checkUnique(ds.poets, 'poet');
  checkUnique(ds.works, 'work');
  checkUnique(ds.places, 'place');
  checkUnique(ds.assertions, 'assertion');
  checkUnique(ds.events, 'event');

  // ── id 必须为纯 ASCII ──
  //
  // 踩过的坑：作品 id 曾用「dufu-lüye-shuhuai」（ü = U+00FC）。构建期一切正常，
  // 但静态导出时 Next 会把路由段做 encodeURIComponent 存成 URL 段，再以
  // 「编码后的形态」回调给页面，于是 getWorkDetail('dufu-l%C3%BCye-shuhuai')
  // 读不到文件 → notFound() → 该页静默变成 404 错误页，且构建不报任何错。
  // 含非 ASCII 的 id 编码前后不一致（ü → %C3%BC），必然触发此问题；
  // 纯 ASCII id 编码前后相同，故从不暴露。此处前置拦截，避免再次静默降级。
  const asciiIdRe = /^[\x20-\x7e]+$/;
  const checkAscii = (list: { id: string }[], type: string) => {
    for (const item of list) {
      if (asciiIdRe.test(item.id)) continue;
      push({
        severity: 'error',
        code: 'non-ascii-id',
        entity: { type, id: item.id },
        message: `${type} id 含非 ASCII 字符：${item.id}（URL 编码后为 ${encodeURIComponent(item.id)}）`,
        hint: 'id 会被用作静态导出的路由段，编码前后必须一致。请改用纯 ASCII 拼写，例如 旅→lu、ü→u。',
      });
    }
  };
  checkAscii(ds.poets, 'poet');
  checkAscii(ds.works, 'work');
  checkAscii(ds.places, 'place');
  checkAscii(ds.assertions, 'assertion');
  checkAscii(ds.events, 'event');

  // ── 路由段往返一致性：id 经 URL 编码再解码必须回到自身 ──
  for (const w of ds.works) {
    if (encodeURIComponent(w.id) === w.id) continue;
    push({
      severity: 'error',
      code: 'id-not-url-safe',
      entity: { type: 'work', id: w.id },
      message: `作品 id 经 URL 编码后变形：${w.id} → ${encodeURIComponent(w.id)}`,
      hint: '这会导致静态导出时该作品页变成 404 错误页。请改用纯 ASCII 的 id。',
    });
  }

  // ── 地点坐标范围（中国境内）──
  for (const p of ds.places) {
    const [lng, lat] = p.coordinates ?? [];
    if (typeof lng !== 'number' || typeof lat !== 'number') {
      push({
        severity: 'error',
        code: 'invalid-coordinates',
        entity: { type: 'place', id: p.id },
        message: `坐标格式非法：${JSON.stringify(p.coordinates)}`,
      });
      continue;
    }
    if (lng < 73 || lng > 136 || lat < 3 || lat > 54) {
      push({
        severity: 'error',
        code: 'coordinates-out-of-china',
        entity: { type: 'place', id: p.id },
        message: `坐标 [${lng}, ${lat}] 超出中国及周边合理范围`,
        hint: '检查经纬度是否写反，或是否误用了国外坐标',
      });
    }
    if (!p.sources?.length) {
      push({
        severity: 'error',
        code: 'missing-source',
        entity: { type: 'place', id: p.id },
        message: '地点缺少溯源依据',
        hint: '每个地点至少需要一条 sources，这是可商用产品的合规底线',
      });
    }
  }

  // ── 作品：诗人存在性 ──
  for (const w of ds.works) {
    if (!poetIds.has(w.poetId)) {
      push({
        severity: 'error',
        code: 'orphan-work',
        entity: { type: 'work', id: w.id },
        message: `作品引用了不存在的诗人：${w.poetId}`,
      });
    }
    if (!w.content?.length) {
      push({
        severity: 'warning',
        code: 'empty-content',
        entity: { type: 'work', id: w.id },
        message: '作品正文为空',
      });
    }
  }

  // ── 断言：核心校验 ──
  for (const a of ds.assertions) {
    if (!workIds.has(a.workId)) {
      push({
        severity: 'error',
        code: 'orphan-assertion-work',
        entity: { type: 'assertion', id: a.id },
        message: `断言引用了不存在的作品：${a.workId}`,
      });
    }
    if (!poetIds.has(a.poetId)) {
      push({
        severity: 'error',
        code: 'orphan-assertion-poet',
        entity: { type: 'assertion', id: a.id },
        message: `断言引用了不存在的诗人：${a.poetId}`,
      });
    }
    if (a.placeId && !placeIds.has(a.placeId)) {
      push({
        severity: 'error',
        code: 'orphan-assertion-place',
        entity: { type: 'assertion', id: a.id },
        message: `断言引用了不存在的地点：${a.placeId}`,
      });
    }
    if (a.triggerEventId && !eventIds.has(a.triggerEventId)) {
      push({
        severity: 'error',
        code: 'orphan-assertion-event',
        entity: { type: 'assertion', id: a.id },
        message: `断言引用了不存在的生平事件：${a.triggerEventId}`,
      });
    }
    if (!a.rationale?.trim()) {
      push({
        severity: 'error',
        code: 'missing-rationale',
        entity: { type: 'assertion', id: a.id },
        message: '考据断言缺少判断依据（rationale）',
        hint: '必须说明「为何定于此地此时」，否则无法复核',
      });
    }
    if (!a.sources?.length) {
      push({
        severity: 'error',
        code: 'missing-source',
        entity: { type: 'assertion', id: a.id },
        message: '考据断言缺少文献来源',
      });
    }
    // 冗余字段一致性
    const work = ds.works.find((w) => w.id === a.workId);
    if (work && work.poetId !== a.poetId) {
      push({
        severity: 'error',
        code: 'poet-mismatch',
        entity: { type: 'assertion', id: a.id },
        message: `断言的 poetId(${a.poetId}) 与作品的 poetId(${work.poetId}) 不一致`,
      });
    }
    // 时空双缺：既不标地点也不标时间，则断言无信息量
    if (!a.placeId && !a.date && !a.dateRange) {
      push({
        severity: 'warning',
        code: 'assertion-no-temporospatial',
        entity: { type: 'assertion', id: a.id },
        message: '断言既无地点也无时间，信息量为零',
      });
    }
    // 异说组一致性：同组置信度不应全为 A（互斥断言不可能都确定）
    if (a.groupId) {
      const group = ds.assertions.filter((x) => x.groupId === a.groupId);
      if (group.length > 1 && group.every((x) => x.confidence === 'A')) {
        push({
          severity: 'warning',
          code: 'contradictory-group',
          entity: { type: 'assertion', id: a.id },
          message: `异说组 ${a.groupId} 全部标记为「确定」，互斥说法不应同时确定`,
          hint: '至少一条应降为 C（存疑）',
        });
      }
    }
    // 缺坐标的地点关联
    if (a.placeId) {
      const place = ds.places.find((p) => p.id === a.placeId);
      if (place?.isCentroid) {
        push({
          severity: 'info',
          code: 'centroid-location',
          entity: { type: 'assertion', id: a.id },
          message: `地点 ${a.placeId} 使用行政区中心点，地图展示时应弱化精度暗示`,
        });
      }
    }
  }

  // ── 生平事件 ──
  for (const e of ds.events) {
    if (!poetIds.has(e.poetId)) {
      push({
        severity: 'error',
        code: 'orphan-event',
        entity: { type: 'event', id: e.id },
        message: `生平事件引用了不存在的诗人：${e.poetId}`,
      });
    }
    if (e.placeId && !placeIds.has(e.placeId)) {
      push({
        severity: 'error',
        code: 'orphan-event-place',
        entity: { type: 'event', id: e.id },
        message: `生平事件引用了不存在的地点：${e.placeId}`,
      });
    }
  }

  // ── 覆盖率统计 ──
  if (ds.works.length > 0) {
    const coveredWorkIds = new Set(
      ds.assertions.filter((a) => a.placeId).map((a) => a.workId),
    );
    const uncovered = ds.works.filter((w) => !coveredWorkIds.has(w.id));
    if (uncovered.length > 0) {
      push({
        severity: 'info',
        code: 'works-without-location',
        entity: { type: 'dataset', id: 'coverage' },
        message: `${uncovered.length}/${ds.works.length} 篇作品尚未关联地点`,
      });
    }
  }

  return issues;
}

/** 格式化输出 */
export function report(issues: ValidationIssue[]): boolean {
  const errors = issues.filter((i) => i.severity === 'error');
  const warnings = issues.filter((i) => i.severity === 'warning');
  const infos = issues.filter((i) => i.severity === 'info');

  const icon = { error: '✗', warning: '!', info: 'i' } as const;
  for (const level of ['error', 'warning', 'info'] as const) {
    const list = issues.filter((i) => i.severity === level);
    if (!list.length) continue;
    console.log(`\n── ${level.toUpperCase()} (${list.length}) ──`);
    for (const i of list) {
      console.log(`${icon[level]} [${i.code}] ${i.entity.type}:${i.entity.id}`);
      console.log(`   ${i.message}`);
      if (i.hint) console.log(`   → ${i.hint}`);
    }
  }

  console.log(
    `\n合计：${errors.length} error / ${warnings.length} warning / ${infos.length} info`,
  );
  if (!errors.length) console.log('✓ 数据校验通过');
  return errors.length === 0;
}
