/**
 * 天地图（国家地理信息公共服务平台）底图配置
 *
 * 合规说明
 * ────────────────────────────────────────────────────────
 * 本站点只使用天地图作为底图服务。国界、行政界线、台湾及南海诸岛等
 * 领土要素一律由天地图按国家标准画法渲染，本项目不自绘、不改动、
 * 不叠加任何境外底图（Google / OSM / Mapbox / Bing 等一律不使用）。
 *
 * 投影说明（易错点）
 * ────────────────────────────────────────────────────────
 * 天地图提供两套投影，后缀含义与直觉相反，务必记牢：
 *   _c = 经纬度投影（CGCS2000 geographic）
 *   _w = 球面墨卡托（≈ EPSG:3857）  ← Leaflet 默认 CRS，本项目用这套
 * 若误用 _c，瓦片会与默认 CRS 错位。
 *
 * 层级对齐
 * ────────────────────────────────────────────────────────
 * 已核对官方 GetCapabilities：_w 的 TileMatrixSet 共 18 级（id 1..18），
 * id=1 时矩阵为 2×2、id=18 时为 2^18×2^18；按 96dpi 换算，id=1 的地面
 * 分辨率约 78265 m/px，与 Web 墨卡托 z=1 的 78271.5 m/px 一致。
 * 故 TILEMATRIX 与 Leaflet 的 {z} 直接相等，无需 zoomOffset。
 * 该集不含 z=0，因此 minZoom 设为 3（全国视图本就用不到更低层级）。
 */

/** 底图类型：底图瓦片 + 配套注记瓦片，两者必须成对加载 */
export interface TileBasemap {
  id: 'vec' | 'ter' | 'img';
  label: string;
  /** 底图图层名（WMTS 的 LAYER 参数） */
  layer: string;
  /** 注记图层名 */
  annotation: string;
  hint: string;
}

export const TIANDITU_BASEMAPS: TileBasemap[] = [
  {
    id: 'vec',
    layer: 'vec',
    annotation: 'cva',
    label: '矢量',
    hint: '行政界线、水系、居民点清晰，适合看创作地的政区归属',
  },
  {
    id: 'ter',
    layer: 'ter',
    annotation: 'cta',
    label: '地形晕渲',
    hint: '带山地明暗与高程感，最能看出「蜀道难」「岱宗夫如何」的地形',
  },
  {
    id: 'img',
    layer: 'img',
    annotation: 'cia',
    label: '影像',
    hint: '卫星影像，最写实，但唐代地名与今地对应需靠注记层辨认',
  },
];

/** 天地图二级域名 t0–t7，随机分发以分散请求 */
export const TIANDITU_SUBDOMAINS = '01234567';

/**
 * 读取密钥。Next.js 会把 NEXT_PUBLIC_* 在构建期内联进产物，
 * 静态导出场景只能走这条路径（运行时 process.env 在浏览器里不存在）。
 *
 * 未配置时返回空串 —— 调用方据此回落到自绘 SVG 底图，站点不会白屏。
 */
export const TIANDITU_KEY = process.env.NEXT_PUBLIC_TIANDITU_KEY ?? '';

/**
 * 构造天地图 WMTS 瓦片 URL 模板。
 * {z}/{x}/{y} 由 Leaflet 替换；{s} 由 Leaflet 按 subdomains 轮询替换。
 */
export function tiandituTileUrl(layer: string, key: string): string {
  return (
    `https://t{s}.tianditu.gov.cn/${layer}_w/wmts` +
    `?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0` +
    `&LAYER=${layer}&STYLE=default&TILEMATRIXSET=w&FORMAT=tiles` +
    `&TILEMATRIX={z}&TILEROW={y}&TILECOL={x}` +
    `&tk=${encodeURIComponent(key)}`
  );
}

/** 天地图要求标注数据来源 */
export const TIANDITU_ATTRIBUTION = '© <a href="https://www.tianditu.gov.cn/" target="_blank" rel="noreferrer">天地图</a>';
