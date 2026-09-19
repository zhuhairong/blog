/**
 * 派生数据新鲜度守卫
 *
 * 背景（真实事故）：
 *   曾出现「手工重建了 derived 数据，但站点构建时 Next 因缓存复用了旧的 .next 目录，
 *   页面显示的仍是上一版数据」，构建成功、页面正常，却悄悄发布了陈旧内容。
 *
 * 因此：站点构建前必须先重算 derived，再校验 derived 与源数据一致，
 *   不一致即中止。让「没更新数据就构建」这件事在流程上不可能发生。
 *
 * 校验内容：
 *   1. manifest 的 counts 必须与 data/ 下源文件实际条数一致
 *   2. 每个诗人都要有 poet/<id>.json；每个作品都要有 work/<id>.json
 *
 * 运行：npx tsx poetry-atlas/src/pipeline/check-derived.ts
 */

import fs from 'fs';
import path from 'path';
import { fileURLToPath } from 'url';
import { loadDataset } from './validators/validate';
import type { BuildManifest } from '../types/index';

const HERE = path.dirname(fileURLToPath(import.meta.url));
const DERIVED = path.resolve(HERE, '..', '..', 'data', 'derived');

function die(msg: string): never {
  console.error(`\n✗ 派生数据校验失败：${msg}`);
  console.error('  修复：cd poetry-atlas && npm run build\n');
  process.exit(1);
}

const manifestPath = path.join(DERIVED, 'manifest.json');
if (!fs.existsSync(manifestPath)) {
  die(`未找到 ${path.relative(process.cwd(), manifestPath)}，派生数据尚未构建`);
}

let manifest: BuildManifest;
try {
  manifest = JSON.parse(fs.readFileSync(manifestPath, 'utf8')) as BuildManifest;
} catch (e) {
  die(`manifest.json 解析失败：${String(e)}`);
}

const ds = loadDataset();
const actual = {
  poets: ds.poets.length,
  works: ds.works.length,
  places: ds.places.length,
  assertions: ds.assertions.length,
};

// ── 1. 条数一致性 ──
const mismatches: string[] = [];
for (const key of Object.keys(actual) as (keyof typeof actual)[]) {
  const want = actual[key];
  const got = manifest.counts?.[key];
  if (want !== got) mismatches.push(`${key}: 源数据 ${want} 条，产物 ${got} 条`);
}
if (mismatches.length) {
  die(
    `派生数据已过期，与源数据不一致：\n  ${mismatches.join('\n  ')}\n` +
      `  （产物生成于 ${manifest.generatedAt ?? '未知时间'}）`,
  );
}

// ── 2. 分片完整性 ──
const missing: string[] = [];
for (const p of ds.poets) {
  if (!fs.existsSync(path.join(DERIVED, 'poet', `${p.id}.json`))) {
    missing.push(`poet/${p.id}.json`);
  }
}
for (const w of ds.works) {
  if (!fs.existsSync(path.join(DERIVED, 'work', `${w.id}.json`))) {
    missing.push(`work/${w.id}.json`);
  }
}
if (missing.length) {
  die(`缺少 ${missing.length} 个分片：\n  ${missing.slice(0, 10).join('\n  ')}`);
}

console.log(
  `✓ 派生数据新鲜：诗人 ${actual.poets} · 作品 ${actual.works} · 地点 ${actual.places} · 断言 ${actual.assertions}`,
);
