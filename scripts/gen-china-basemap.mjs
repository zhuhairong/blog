/**
 * 生成中国行政区划简化边界，供无密钥时的回落底图使用。
 *
 *   node scripts/gen-china-basemap.mjs
 *
 * 数据源：阿里云 DataV.GeoAtlas
 *   https://geo.datav.aliyun.com/areas_v3/bound/100000_full.json
 * 该数据为省级行政区划面，包含台湾省、香港特别行政区、澳门特别行政区；
 * 海南省要素中已含南海诸岛（纬度最低至约 3.8°N，即曾母暗沙一带），
 * 因此绘制时不需要另行补充岛礁，只要把南海部分放进角图即可。
 *
 * 合规说明：本文件只画中国境内的行政区面，不画国界，不画海域断续线；
 * 境外底图（Google / OSM / Mapbox 等）一律不使用。
 * 需要严格国家标准画法（含断续线、境界线）时，请配置天地图密钥走瓦片底图。
 *
 * 产物入版本库：CI 里不再联网抓取，避免构建依赖外部服务。
 */

import { writeFileSync } from 'node:fs';
import { resolve } from 'node:path';

const SRC = 'https://geo.datav.aliyun.com/areas_v3/bound/100000_full.json';
const OUT = resolve(process.cwd(), 'src/app/poetry-atlas/china-basemap.json');

/** 简化容差（度）。0.06 ≈ 6km，全国视图下肉眼无差别 */
const TOLERANCE = 0.06;
/**
 * 环的顶点数与包围盒下限。
 * 阈值必须放得很小：澳门特别行政区本身就不到 0.05° 宽，南海诸岛的多数岛礁
 * 更是只有几十米量级。早先用 0.08° 过滤，结果把澳门整个丢掉了 —— 这属于
 * 领土要素缺失，是必须避免的错误，不是可以接受的「去噪」。
 * 小环经抽稀后只剩 3~4 个顶点，体积代价很小，因此一律保留。
 */
const MIN_POINTS = 3;
const MIN_DIAG = 0.002;

/* ── Douglas-Peucker 抽稀 ─────────────────────────────── */
function perpDist(p, a, b) {
  const [x, y] = p;
  const [x1, y1] = a;
  const [x2, y2] = b;
  const dx = x2 - x1;
  const dy = y2 - y1;
  if (dx === 0 && dy === 0) return Math.hypot(x - x1, y - y1);
  const t = ((x - x1) * dx + (y - y1) * dy) / (dx * dx + dy * dy);
  const cx = x1 + Math.max(0, Math.min(1, t)) * dx;
  const cy = y1 + Math.max(0, Math.min(1, t)) * dy;
  return Math.hypot(x - cx, y - cy);
}

function simplify(points, tol) {
  if (points.length <= 2) return points;
  let maxD = -1;
  let idx = 0;
  const first = points[0];
  const last = points[points.length - 1];
  for (let i = 1; i < points.length - 1; i += 1) {
    const d = perpDist(points[i], first, last);
    if (d > maxD) {
      maxD = d;
      idx = i;
    }
  }
  if (maxD <= tol) return [first, last];
  const left = simplify(points.slice(0, idx + 1), tol);
  const right = simplify(points.slice(idx), tol);
  return left.slice(0, -1).concat(right);
}

/* ── 主流程 ──────────────────────────────────────────── */
const res = await fetch(SRC);
if (!res.ok) throw new Error(`抓取失败：HTTP ${res.status}`);
const geo = await res.json();

const round = (n) => Math.round(n * 100) / 100;

const provinces = [];
let keptRings = 0;
let droppedRings = 0;
let rawPoints = 0;
let outPoints = 0;

for (const f of geo.features) {
  const name = f.properties.name;
  const geom = f.geometry;
  if (!geom) continue;

  // 统一成 Polygon[] 的坐标嵌套：Polygon=[[ring...]], MultiPolygon=[[[ring...]]]
  const polys = geom.type === 'MultiPolygon' ? geom.coordinates : [geom.coordinates];

  const rings = [];
  for (const poly of polys) {
    for (const ring of poly) {
      rawPoints += ring.length;

      // 先按包围盒过滤碎屑
      let mnx = Infinity;
      let mxx = -Infinity;
      let mny = Infinity;
      let mxy = -Infinity;
      for (const [x, y] of ring) {
        if (x < mnx) mnx = x;
        if (x > mxx) mxx = x;
        if (y < mny) mny = y;
        if (y > mxy) mxy = y;
      }
      if (Math.hypot(mxx - mnx, mxy - mny) < MIN_DIAG) {
        droppedRings += 1;
        continue;
      }

      const s = simplify(ring, TOLERANCE);
      if (s.length < MIN_POINTS) {
        droppedRings += 1;
        continue;
      }

      keptRings += 1;
      outPoints += s.length;
      rings.push({
        // 扁平化存储：[lng,lat,lng,lat,...]，体积约为嵌套数组的六成
        p: s.flatMap(([x, y]) => [round(x), round(y)]),
        bbox: [round(mnx), round(mny), round(mxx), round(mxy)],
      });
    }
  }

  if (rings.length) provinces.push({ name, rings });
}

const payload = {
  provenance: {
    source: '阿里云 DataV.GeoAtlas 省级行政区划',
    url: SRC,
    fetchedAt: new Date().toISOString().slice(0, 10),
    note: '仅含中国境内行政区面，不含国界与海域断续线。严格国家标准画法请用天地图瓦片底图。',
  },
  simplify: { tolerance: TOLERANCE, minPoints: MIN_POINTS, minDiagonal: MIN_DIAG },
  stats: { provinces: provinces.length, keptRings, droppedRings, rawPoints, outPoints },
  provinces,
};

writeFileSync(OUT, `${JSON.stringify(payload)}\n`, 'utf8');
console.log(
  `已写入 ${OUT}\n  省级要素 ${provinces.length} / 保留环 ${keptRings} / 丢弃环 ${droppedRings}\n` +
    `  顶点 ${rawPoints} → ${outPoints}（压缩 ${(100 - (outPoints / rawPoints) * 100).toFixed(1)}%）`,
);
