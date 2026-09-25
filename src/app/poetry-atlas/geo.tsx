/**
 * 自绘底图的投影与几何（客户端安全）。
 *
 * 从 `SvgFallbackMap` 抽出来，是因为「创作地分布」与「一生行迹」两张图
 * 都要画同一套中国行政区划底图。抽出的动机不只是少写一遍代码——
 * 更关键是**底图几何只能有一份**：两份各算一次，早晚会在某次改投影时
 * 只改一处，于是两张图的中国形状对不上。
 *
 * ⚠️ 本模块只能 import JSON 与纯函数，禁止引入 `fs`/`path`——
 * 它会被客户端组件（行迹地图）引用。客户端误引含 fs 的模块会报
 * `Module not found`，而 **tsc 查不出来，只有 build 才暴露**。
 *
 * 绘制范围默认只到 17°N，南海诸岛另设角图 —— 这是中国地图的常规画法。
 * 本图只画中国境内行政区面，不画国界、不画海域断续线；需要严格国家标准
 * 画法（含断续线）时请配置 NEXT_PUBLIC_TIANDITU_KEY 走天地图瓦片。
 */

import basemap from './china-basemap.json';

/* ── 投影 ──────────────────────────────────────────────── */

export interface MapBounds {
  minLng: number;
  maxLng: number;
  minLat: number;
  maxLat: number;
}

/** 全国范围：不含南海诸岛，那部分走角图 */
export const MAIN: MapBounds = { minLng: 73, maxLng: 136, minLat: 17, maxLat: 54 };

/** 墨卡托纵坐标（纬度 → 无量纲 y），裁到 ±85° 避免极点发散 */
export function mercY(lat: number) {
  const l = Math.max(-85, Math.min(85, lat));
  return Math.log(Math.tan(Math.PI / 4 + (l * Math.PI) / 180 / 2));
}

/** 画幅宽高比：与 CSS 的 `.mapCanvas` aspect-ratio 一致，避免留白 */
export const MAP_ASPECT = 1000 / 746;

/** 画幅基准宽度（viewBox 单位），两张图共用 */
export const MAP_W = 1000;

/* ── 南海诸岛角图：位置固定，与主图取景无关 ─────────────── */

const INSET_BOUNDS: MapBounds = { minLng: 108, maxLng: 119, minLat: 3, maxLat: 18.5 };
const INSET_H = 150;
const IY_TOP = mercY(INSET_BOUNDS.maxLat);
const IY_BOT = mercY(INSET_BOUNDS.minLat);
const ISPAN_X = ((INSET_BOUNDS.maxLng - INSET_BOUNDS.minLng) * Math.PI) / 180;
const ISPAN_Y = IY_TOP - IY_BOT;
const INSET_W = Math.round((INSET_H * ISPAN_X) / ISPAN_Y);

const projectInset = (lng: number, lat: number) => ({
  x: INSET.x + ((lng - INSET_BOUNDS.minLng) / (INSET_BOUNDS.maxLng - INSET_BOUNDS.minLng)) * INSET.w,
  y: INSET.y + ((IY_TOP - mercY(lat)) / ISPAN_Y) * INSET.h,
});

/** 小于此尺寸（度）的岛礁退化成点，否则在角图里只是看不见的碎线 */
const DOT_MAX_DIAG = 0.4;

/** 扁平数组 → SVG path */
function toPath(flat: number[], proj: (lng: number, lat: number) => { x: number; y: number }) {
  let d = '';
  for (let i = 0; i < flat.length; i += 2) {
    const { x, y } = proj(flat[i], flat[i + 1]);
    d += `${i === 0 ? 'M' : 'L'}${x.toFixed(1)} ${y.toFixed(1)}`;
  }
  return `${d}Z`;
}

/* ── 投影对象 ───────────────────────────────────────────── */

export interface Projection {
  bounds: MapBounds;
  width: number;
  height: number;
  /** 相对全国视图的放大倍数，用于按比例收细线宽 */
  zoom: number;
  project: (lng: number, lat: number) => { x: number; y: number };
  /** 已按本投影重算好的省界路径 */
  paths: { name: string; d: string }[];
  inset: { x: number; y: number; w: number; h: number };
}

/** 角图位置：钉在画幅右下角，尺寸固定 */
const INSET = { x: MAP_W - INSET_W - 16, y: 0, w: INSET_W, h: INSET_H };

/**
 * 省界路径缓存。
 *
 * 路径要按取景重算（每次换一位诗人就是一套新坐标），而 React Compiler
 * 不接受组件内手写的 useMemo，所以缓存放模块作用域、按取景键控。
 * 同一位诗人的页面只会命中一次计算，代价可忽略。
 */
const PATH_CACHE = new Map<string, { name: string; d: string }[]>();

function pathsFor(key: string, proj: (lng: number, lat: number) => { x: number; y: number }) {
  const hit = PATH_CACHE.get(key);
  if (hit) return hit;
  const built = basemap.provinces.map((p) => ({
    name: p.name,
    d: p.rings.map((r) => toPath(r.p, proj)).join(' '),
  }));
  PATH_CACHE.set(key, built);
  return built;
}

/** 按给定经纬范围构造投影；高度由墨卡托真实长宽比推导，避免地图被拉伸 */
export function makeProjection(bounds: MapBounds, width = MAP_W): Projection {
  const yTop = mercY(bounds.maxLat);
  const yBot = mercY(bounds.minLat);
  const spanY = yTop - yBot;
  const spanX = ((bounds.maxLng - bounds.minLng) * Math.PI) / 180;
  const height = Math.round((width * spanY) / spanX);

  const project = (lng: number, lat: number) => ({
    x: ((lng - bounds.minLng) / (bounds.maxLng - bounds.minLng)) * width,
    y: ((yTop - mercY(lat)) / spanY) * height,
  });

  const mainSpanX = ((MAIN.maxLng - MAIN.minLng) * Math.PI) / 180;

  return {
    bounds,
    width,
    height,
    zoom: spanX > 0 ? mainSpanX / spanX : 1,
    project,
    paths: pathsFor(`${bounds.minLng},${bounds.maxLng},${bounds.minLat},${bounds.maxLat}`, project),
    // 角图纵向贴住画幅底边
    inset: { ...INSET, y: height - INSET_H - 22 },
  };
}

/** 全国视图（「创作地分布」用） */
export const MAIN_PROJECTION = makeProjection(MAIN);

/** 南海诸岛：海南省要素里纬度低于 17.5° 的环（海南岛本体不进角图） */
export const ISLAND_GEOMETRY = (() => {
  const hainan = basemap.provinces.find((p) => p.name === '海南省');
  const paths: string[] = [];
  const dots: { x: number; y: number }[] = [];
  if (!hainan) return { paths, dots };
  for (const r of hainan.rings) {
    if (r.bbox[1] >= 17.5) continue;
    const diag = Math.hypot(r.bbox[2] - r.bbox[0], r.bbox[3] - r.bbox[1]);
    if (diag < DOT_MAX_DIAG) {
      // 取包围盒中心当作岛礁位置
      dots.push(projectInset((r.bbox[0] + r.bbox[2]) / 2, (r.bbox[1] + r.bbox[3]) / 2));
    } else {
      paths.push(toPath(r.p, projectInset));
    }
  }
  return { paths, dots };
})();

/* ── 按诗人行迹取景 ─────────────────────────────────────── */

/** 最小经纬跨度：站点过于集中时不至于放大到街道级 */
const MIN_SPAN_LNG = 6;
const MIN_SPAN_LAT = 5;
/** 外扩比例，让边缘站点不贴框 */
const PAD = 0.16;
/**
 * 行迹内容允许占据的画面宽度比例。
 *
 * 侧栏（`.mapDetail`）绝对定位压在画幅右侧约三分之一处，若行迹铺满整个
 * 画幅，东部站点就会被侧栏盖住——李白 33 站全在四川至江苏之间，
 * 恰好是最容易被盖住的一段。故把内容压进左侧 62%，
 * 右侧留给侧栏与站点标签。
 */
const SAFE_W = 0.62;

/**
 * 由站点坐标推出取景范围。
 *
 * 为什么要取景而不是一律画全中国：「创作地分布」要表现全国分布，
 * 全图是对的；但单看一位诗人，全中国往往有三分之二是他没去过的地方，
 * 而他的行迹挤在东部一隅——李白 33 站全落在四川至江苏之间，
 * 画全中国等于把有效信息压进画面的一小块，还会被侧栏盖住。
 *
 * 算法：取站点包围盒 → 补足最小跨度 → 左/上下外扩 → 按右侧安全区定右界
 * → 按画幅比例补齐（纵向对称扩、横向只向右扩，保证内容只会更靠左）
 * → 平移收进全国范围内。
 */
export function journeyBounds(stops: { coordinates: [number, number] }[]): MapBounds {
  if (!stops.length) return MAIN;

  let cMinLng = Infinity;
  let cMaxLng = -Infinity;
  let cMinLat = Infinity;
  let cMaxLat = -Infinity;
  for (const s of stops) {
    cMinLng = Math.min(cMinLng, s.coordinates[0]);
    cMaxLng = Math.max(cMaxLng, s.coordinates[0]);
    cMinLat = Math.min(cMinLat, s.coordinates[1]);
    cMaxLat = Math.max(cMaxLat, s.coordinates[1]);
  }

  // 补足最小跨度（围绕中心对称扩）
  const growTo = (lo: number, hi: number, min: number): [number, number] =>
    hi - lo >= min ? [lo, hi] : [(lo + hi) / 2 - min / 2, (lo + hi) / 2 + min / 2];
  [cMinLng, cMaxLng] = growTo(cMinLng, cMaxLng, MIN_SPAN_LNG);
  [cMinLat, cMaxLat] = growTo(cMinLat, cMaxLat, MIN_SPAN_LAT);

  const padLng = (cMaxLng - cMinLng) * PAD;
  const padLat = (cMaxLat - cMinLat) * PAD;

  let minLng = cMinLng - padLng;
  let maxLng = minLng + (cMaxLng - minLng) / SAFE_W;
  let minLat = cMinLat - padLat;
  let maxLat = cMaxLat + padLat;

  // 按画幅比例补齐长边。墨卡托纵向非线性，迭代三次足够收敛
  for (let i = 0; i < 3; i += 1) {
    const spanX = ((maxLng - minLng) * Math.PI) / 180;
    const spanY = mercY(maxLat) - mercY(minLat);
    if (spanX / spanY > MAP_ASPECT) {
      // 画面太扁 → 纵向对称扩（不影响横向安全区）
      const need = spanX / MAP_ASPECT;
      const cLat = (minLat + maxLat) / 2;
      const dLat = (((need - spanY) / 2) * Math.cos((cLat * Math.PI) / 180) * 180) / Math.PI;
      minLat -= dLat;
      maxLat += dLat;
    } else {
      // 画面太窄 → 只向右扩，内容只会更靠左，安全区只会更宽裕
      maxLng += (spanY * MAP_ASPECT * 180) / Math.PI - (maxLng - minLng);
    }
  }

  // 平移收进全国范围（保持跨度，不缩放，以免破坏刚补好的比例）
  const shift = (lo: number, hi: number, limitLo: number, limitHi: number): [number, number] => {
    if (hi - lo >= limitHi - limitLo) return [limitLo, limitHi];
    let l = lo;
    let h = hi;
    if (l < limitLo) {
      h += limitLo - l;
      l = limitLo;
    }
    if (h > limitHi) {
      l -= h - limitHi;
      h = limitHi;
    }
    return [l, h];
  };
  [minLng, maxLng] = shift(minLng, maxLng, MAIN.minLng, MAIN.maxLng);
  [minLat, maxLat] = shift(minLat, maxLat, MAIN.minLat, MAIN.maxLat);

  return { minLng, maxLng, minLat, maxLat };
}

/* ── 共享的底图 SVG 片段 ────────────────────────────────── */

/**
 * 底图 `<defs>`：陆地阴影 + 角图裁剪框。
 *
 * 裁剪框必须与 `BasemapLayers` 用**同一个**投影里的 inset 坐标——
 * 角图纵向贴画幅底边，画幅高度随取景变化，写死就会裁错位置。
 */
export function BasemapDefs({ proj }: { proj: Projection }) {
  const { inset } = proj;
  return (
    <defs>
      {/* 陆地阴影，让陆块从海面上浮起来 */}
      <filter id="atlasLandGlow" x="-10%" y="-10%" width="120%" height="120%">
        <feDropShadow dx="0" dy="2" stdDeviation="5" floodColor="#000" floodOpacity="0.45" />
      </filter>
      <clipPath id="atlasInsetClip">
        <rect x={inset.x} y={inset.y} width={inset.w} height={inset.h} rx="6" />
      </clipPath>
    </defs>
  );
}

/**
 * 底图图层：省界填充、省界描边、经纬网、南海诸岛角图。
 *
 * 两张地图共用同一份，保证「创作地分布」与「一生行迹」的底子完全一致。
 * 各图只需再叠加自己的图层（散点 / 行迹线）。
 */
export function BasemapLayers({ proj, grid = true }: { proj: Projection; grid?: boolean }) {
  const { width: W, height: H, project, paths, inset } = proj;
  // 放大后线宽要按比例收细，否则省界会糊成一片
  const lineW = Math.max(0.35, 0.9 / proj.zoom);
  const gridW = Math.max(0.35, 1 / proj.zoom);
  const gridStep = proj.zoom > 4 ? 2 : 5;

  const lngTicks: number[] = [];
  for (let l = Math.ceil(proj.bounds.minLng / gridStep) * gridStep; l <= proj.bounds.maxLng; l += gridStep) {
    lngTicks.push(l);
  }
  const latTicks: number[] = [];
  for (let l = Math.ceil(proj.bounds.minLat / gridStep) * gridStep; l <= proj.bounds.maxLat; l += gridStep) {
    latTicks.push(l);
  }

  return (
    <>
      {/* 省界：先描边后填充，边界线始终是连续的 */}
      <g filter="url(#atlasLandGlow)">
        {paths.map((p) => (
          <path key={`f-${p.name}`} d={p.d} fill="rgba(255,255,255,0.05)" stroke="none" />
        ))}
      </g>
      <g fill="none" stroke="rgba(255,255,255,0.16)" strokeWidth={lineW} strokeLinejoin="round">
        {paths.map((p) => (
          <path key={`s-${p.name}`} d={p.d} />
        ))}
      </g>

      {/* 经纬网：淡到几乎看不见，只用于定位 */}
      {grid && (
        <g stroke="rgba(255,255,255,0.05)" strokeWidth={gridW}>
          {lngTicks.map((lng) => {
            const { x } = project(lng, proj.bounds.minLat);
            return <line key={`m${lng}`} x1={x} y1="0" x2={x} y2={H} />;
          })}
          {latTicks.map((lat) => {
            const { y } = project(proj.bounds.minLng, lat);
            return <line key={`p${lat}`} x1="0" y1={y} x2={W} y2={y} />;
          })}
        </g>
      )}

      {/* ── 南海诸岛角图 ── */}
      <g>
        <rect
          x={inset.x}
          y={inset.y}
          width={inset.w}
          height={inset.h}
          rx="6"
          fill="rgba(10,10,16,0.86)"
          stroke="rgba(255,255,255,0.2)"
          strokeWidth="1"
        />
        <g clipPath="url(#atlasInsetClip)">
          <g fill="none" stroke="rgba(255,255,255,0.18)" strokeWidth="0.7">
            {ISLAND_GEOMETRY.paths.map((d, i) => (
              <path key={`ip${i}`} d={d} />
            ))}
          </g>
          <g fill="rgba(255,255,255,0.22)">
            {ISLAND_GEOMETRY.dots.map((d, i) => (
              <circle key={`id${i}`} cx={d.x} cy={d.y} r="1.1" />
            ))}
          </g>
          <text x={inset.x + 6} y={inset.y + 14} fill="rgba(255,255,255,0.5)" fontSize="10">
            南海诸岛
          </text>
        </g>
      </g>
    </>
  );
}
