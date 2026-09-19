/**
 * 古画背景层
 * ============================================================
 * 与 `scene.ts` 的程序化 SVG 互补：
 *
 *   · SVG 插画 —— 确定性、随天候变色、体积极小、能表达「事件类型」；
 *   · 古画位图 —— 精美、有出处、能表达「意境」，但颜色固定、
 *                 体积大，且原图是浅色纸本，与暗色站点冲突。
 *
 * 两者叠在一起用：古画在下作纹理，SVG 在上作空气（云气、粒子、渐隐），
 * 于是既有古画的笔触，又保住了主题适配与可读性。
 *
 * 为什么用 CSS filter 而不是在构建期把图染黑：
 *   1. 构建期染色会生成第二份位图，体积翻倍；
 *   2. filter 是 GPU 合成的，零成本；
 *   3. 主题若将来要改浅色，只需换 filter，不必重跑下载管线。
 *   `invert(1)` 把纸本的「白底黑墨」翻成「黑底白墨」，
 *   `hue-rotate(185deg)` 把偏暖的赭石还原为冷调，避免与青蓝主色打架，
 *   再压 brightness/提 contrast，得到接近碑帖拓片的质感。
 */

import { renderScene, renderGlyph, type SceneMotive, type SceneWeather } from './scene';

export type SceneSize = 'hero' | 'card' | 'thumb' | 'banner';

const SIZES: Record<SceneSize, { w: number; h: number; detail: boolean }> = {
  // 诗人页主图：宽幅，细节全开
  hero: { w: 960, h: 300, detail: true },
  // 事件卡缩略图
  card: { w: 320, h: 200, detail: true },
  // 更小的行内缩略
  thumb: { w: 168, h: 120, detail: false },
  // 作品页诗框背景：超宽，关掉粒子以减小体积
  banner: { w: 800, h: 420, detail: true },
};

/** 古画配图的最小信息（来自 data/art/work-art.json） */
export interface ArtPiece {
  objectID: number;
  artTitle: string;
  artArtist: string;
  artDate: string;
  artCredit: string;
  /** 站点内的相对路径（**已含 basePath**，如 /blog/poetry-atlas/art/36131.webp） */
  src: string;
  /** 缩略图路径 */
  srcSmall?: string;
  /** 大都会藏品页，用于「出处」链接 */
  artURL?: string;
  motifs?: string[];
}

export interface SceneProps {
  motive: SceneMotive;
  weather: SceneWeather;
  seed: string;
  size?: SceneSize;
  className?: string;
  /**
   * 古画配图。给了就渲染成「古画 + SVG 云气」的复合背景；
   * 不给则退化成纯 SVG，站点不会因为缺图而破版。
   */
  art?: ArtPiece | null;
  /**
   * 古画的不透明度。诗框背景文字密度高，需要压得比卡片更低；
   * 卡片类可以给高一些让画看清楚。
   */
  artOpacity?: number;
}

export default function Scene({
  motive,
  weather,
  seed,
  size = 'card',
  className,
  art,
  artOpacity,
}: SceneProps) {
  const { w, h, detail } = SIZES[size];
  const svg = renderScene({ motive, weather, seed }, { width: w, height: h, detail });

  // 有古画时，SVG 只保留「天候粒子 + 渐隐」，不再画山形屋舍，
  // 否则两套构图会互相打架（一张图里出现两组山脊）。
  //
  // 但缩略图（thumb）例外：92px 见方的小格子里叠两层只会糊成一团，
  // 古画本身已经足够说明意境，直接不叠 SVG。
  const overlayOnly = Boolean(art) && size !== 'thumb';
  const opacity = artOpacity ?? (size === 'banner' ? 0.42 : 0.62);

  return (
    <div className={className} aria-hidden="true" role="presentation">
      {art && (
        <img
          className="pa-art-layer"
          src={art.src}
          alt=""
          loading="lazy"
          decoding="async"
          style={{ opacity }}
        />
      )}
      {!(art && size === 'thumb') && (
        <div
          className={overlayOnly ? 'pa-art-overlay' : undefined}
          style={overlayOnly ? { position: 'absolute', inset: 0 } : undefined}
          dangerouslySetInnerHTML={{ __html: overlayOnly ? svgAtmosphere(svg) : svg }}
        />
      )}
    </div>
  );
}

/**
 * 从完整 SVG 里剥出「大气层」——只保留粒子、云气与渐隐，
 * 去掉山脊与建筑，供叠在古画之上使用。
 *
 * 做法：按 `<defs>` 里定义的 id 取出渐隐矩形，再补一层粒子。
 * 不去解析整棵 SVG 树（太重），而是直接复用 Scene 的渲染约定：
 * 末尾那层 fade 矩形永远是最后一段。
 */
function svgAtmosphere(svg: string): string {
  // 取 <defs> 与最后那层 fade 矩形：这是不受构图影响的通用层
  //
  // ⚠️ 这里踩过一个坑：`width="${w}"` 是**模板变量**，渲染出来是
  // `width="800"` 这样的字面量，但早期正则写的是 `width="\d+"`，
  // 遇上带小数点的 viewBox 值（如 `width="168.5"`）或后续改成百分比就会漏匹配。
  // 改成 `[^"]+` 后与值的形式解耦，不会再随尺寸约定变化而静默失效。
  // 更关键的是 fade 那一层：它负责把画作的底部压进卡片背景，
  // 一旦漏掉，古画边缘就是一条生硬的直线，与暗色站点割裂。
  const defs = svg.match(/<defs>[\s\S]*?<\/defs>/)?.[0] ?? '';
  const fade = svg.match(/<rect\b[^>]*fill="url\(#[^"]*-fade\)"[^>]*\/>/)?.[0] ?? '';
  const glow = svg.match(/<rect\b[^>]*fill="url\(#[^"]*-glow\)"[^>]*\/>/)?.[0] ?? '';
  const head = svg.match(/<svg[^>]*>/)?.[0] ?? '';
  return `${head}${defs}${fade}${glow}</svg>`;
}

/** 小型符号图：用于卡片头像位、图例等 */
export function SceneGlyph({ motive, seed, size = 40 }: { motive: SceneMotive; seed: string; size?: number }) {
  return (
    <span
      aria-hidden="true"
      role="presentation"
      style={{ display: 'inline-flex', width: size, height: size }}
      dangerouslySetInnerHTML={{ __html: renderGlyph(motive, seed, size) }}
    />
  );
}
