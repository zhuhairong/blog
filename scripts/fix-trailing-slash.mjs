/**
 * 为静态导出补一份 index.html，使带尾斜杠的 URL 也能访问。
 *
 * 背景：next.config.ts 未设 trailingSlash（默认 false），静态导出产出的是
 * 扁平的 `out/poetry-atlas/work/dufu-wangyue.html`，同时会生成一个同名目录
 * ——但那个目录里只有 RSC 的 .txt 载荷，没有 index.html。
 * 于是 GitHub Pages 上：
 *     /blog/poetry-atlas/work/dufu-wangyue      ✅ 命中 .html
 *     /blog/poetry-atlas/work/dufu-wangyue/     ❌ 目录里没有 index.html → 404
 *
 * 本脚本把每个 X.html 复制一份到 X/index.html，两种写法都能命中。
 * 这是纯增量操作：不改动 next.config.ts，不改动任何现有 URL，
 * 已有的无尾斜杠链接行为完全不变。
 *
 * 幂等：重复执行只覆盖，不会层层嵌套（跳过所有 index.html）。
 */
import { copyFileSync, mkdirSync, readdirSync, statSync } from 'node:fs';
import { dirname, join, relative, resolve } from 'node:path';

const OUT = resolve(process.cwd(), 'out');

// 不需要补尾斜杠版本的：根 index、404、以及下划线开头的内部文件
const SKIP = new Set(['index.html', '404.html']);

function walk(dir) {
  const out = [];
  for (const name of readdirSync(dir)) {
    const full = join(dir, name);
    if (statSync(full).isDirectory()) {
      out.push(...walk(full));
    } else if (name.endsWith('.html')) {
      out.push(full);
    }
  }
  return out;
}

let copied = 0;
let skipped = 0;

for (const file of walk(OUT)) {
  const name = file.slice(file.lastIndexOf('\\') + 1).split('/').pop();
  if (SKIP.has(name) || name.startsWith('_')) {
    skipped += 1;
    continue;
  }
  const targetDir = join(dirname(file), name.replace(/\.html$/, ''));
  const target = join(targetDir, 'index.html');
  mkdirSync(targetDir, { recursive: true });
  copyFileSync(file, target);
  copied += 1;
}

console.log(
  `[trailing-slash] 已复制 ${copied} 个 index.html，跳过 ${skipped} 个（相对 ${relative(process.cwd(), OUT)}）`
);
