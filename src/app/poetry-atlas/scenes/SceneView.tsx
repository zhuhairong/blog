/**
 * 场景插画的 React 包装层。
 *
 * 插画是纯装饰，不承载任何信息（信息仍在文本里），
 * 因此统一 `aria-hidden` + `role="presentation"`，读屏软件会跳过。
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

export interface SceneProps {
  motive: SceneMotive;
  weather: SceneWeather;
  seed: string;
  size?: SceneSize;
  className?: string;
}

export default function Scene({ motive, weather, seed, size = 'card', className }: SceneProps) {
  const { w, h, detail } = SIZES[size];
  const svg = renderScene({ motive, weather, seed }, { width: w, height: h, detail });
  return (
    <div
      className={className}
      aria-hidden="true"
      role="presentation"
      dangerouslySetInnerHTML={{ __html: svg }}
    />
  );
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
