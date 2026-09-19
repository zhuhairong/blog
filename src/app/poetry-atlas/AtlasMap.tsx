'use client';

import { useCallback, useMemo, useState } from 'react';
import dynamic from 'next/dynamic';
import Link from 'next/link';
import styles from './atlas.module.css';
import type { DerivedMapPoint } from '@/poetry-atlas/types';
import { CONFIDENCE_UI, PRECISION_LABEL, type PlaceWorkEntry } from '@/lib/atlas-view';
import { TIANDITU_KEY, TIANDITU_BASEMAPS } from './tiles';

/**
 * Leaflet 在模块初始化阶段就会访问 window，静态导出时服务端预渲染会炸。
 * 用 ssr:false 把它推迟到浏览器端。
 */
const TileAtlasMap = dynamic(() => import('./TileAtlasMap'), {
  ssr: false,
  loading: () => <div className={styles.mapLoading}>正在加载地图…</div>,
});

/**
 * 回落底图带一份 60KB 的行政区划边界。拆成独立 chunk，
 * 配了密钥的站点就不会为这份永远不会渲染的数据付流量。
 * 不用 ssr:false —— 保留预渲染，未配密钥时首屏直接就有图，不闪。
 */
const SvgFallbackMap = dynamic(() => import('./SvgFallbackMap'));

interface Props {
  points: DerivedMapPoint[];
  /** placeId → 该地相关作品（构建期已生成） */
  placeWorks: Record<string, PlaceWorkEntry[]>;
}

export default function AtlasMap({ points, placeWorks }: Props) {
  const [selected, setSelected] = useState<string | null>(null);
  const [precisionFilter, setPrecisionFilter] = useState<Set<string>>(new Set());
  const [basemap, setBasemap] = useState<string>('ter');
  const [dark, setDark] = useState(false);

  const hasKey = TIANDITU_KEY.length > 0;

  /** 精度过滤器：空集合表示全显示 */
  const visible = useMemo(
    () =>
      points.filter(
        (p) => precisionFilter.size === 0 || precisionFilter.has(p.precision),
      ),
    [points, precisionFilter],
  );

  const active = points.find((p) => p.placeId === selected) ?? null;

  const togglePrecision = (key: string) => {
    setPrecisionFilter((prev) => {
      const next = new Set(prev);
      if (next.has(key)) next.delete(key);
      else next.add(key);
      return next;
    });
  };

  const handleSelect = useCallback((placeId: string | null) => {
    setSelected((prev) => (prev === placeId ? null : placeId));
  }, []);

  const precisions = [...new Set(points.map((p) => p.precision))];
  const activeBasemap =
    TIANDITU_BASEMAPS.find((b) => b.id === basemap) ?? TIANDITU_BASEMAPS[0];

  return (
    <div className={styles.mapWrap}>
      <div className={styles.mapToolbar}>
        <span className={styles.mapHint} style={{ marginLeft: 0, marginRight: 8 }}>
          精度筛选
        </span>
        {precisions.map((key) => {
          const on = precisionFilter.size === 0 || precisionFilter.has(key);
          return (
            <button
              key={key}
              type="button"
              className={`${styles.chip} ${on && precisionFilter.size > 0 ? styles.chipOn : ''}`}
              onClick={() => togglePrecision(key)}
              aria-pressed={precisionFilter.has(key)}
            >
              <span
                className={styles.chipSwatch}
                style={{
                  background:
                    key === 'site' || key === 'township'
                      ? 'var(--green)'
                      : key === 'county' || key === 'prefecture'
                        ? 'var(--amber)'
                        : 'var(--text-tertiary)',
                }}
                aria-hidden="true"
              />
              {PRECISION_LABEL[key]}
            </button>
          );
        })}

        {hasKey && (
          <>
            <span className={styles.mapToolbarDivider} aria-hidden="true" />
            <span className={styles.mapHint} style={{ marginLeft: 0, marginRight: 8 }}>
              底图
            </span>
            {TIANDITU_BASEMAPS.map((b) => (
              <button
                key={b.id}
                type="button"
                title={b.hint}
                className={`${styles.chip} ${basemap === b.id ? styles.chipOn : ''}`}
                onClick={() => setBasemap(b.id)}
                aria-pressed={basemap === b.id}
              >
                {b.label}
              </button>
            ))}
            <button
              type="button"
              className={`${styles.chip} ${dark ? styles.chipOn : ''}`}
              onClick={() => setDark((d) => !d)}
              aria-pressed={dark}
              title="把底图反色以适配深色站点主题（仅改变配色，不改变任何地理要素）"
            >
              {dark ? '深色底图' : '原色底图'}
            </button>
          </>
        )}

        <span className={styles.mapHint}>
          {visible.length} / {points.length} 个地点
        </span>
      </div>

      <div className={styles.mapCanvas}>
        {hasKey ? (
          <TileAtlasMap
            points={visible}
            selected={selected}
            onSelect={handleSelect}
            basemap={basemap}
            dark={dark}
          />
        ) : (
          <>
            <SvgFallbackMap points={visible} selected={selected} onSelect={handleSelect} />
            <div className={styles.mapNoKey}>
              <strong>当前为示意底图</strong>
              <span>
                未配置天地图密钥，故未加载真实地理底图。配置
                <code>NEXT_PUBLIC_TIANDITU_KEY</code> 后此处会自动切换为天地图矢量／地形晕渲／影像瓦片。
              </span>
            </div>
          </>
        )}

        {/* 侧栏详情 */}
        <aside className={styles.mapDetail}>
          {!active ? (
            <p className={styles.mapDetailEmpty}>
              点击地图上的地点，查看该地诞生的作品、诗人，以及坐标精度说明。
              <br />
              <br />
              虚线圆圈表示该坐标是<strong style={{ color: 'var(--amber)' }}>行政区中心点</strong>
              ，仅代表大致区域，并非精确创作位置。
              {hasKey && (
                <>
                  <br />
                  <br />
                  当前底图：<strong>{activeBasemap.label}</strong>。{activeBasemap.hint}。
                </>
              )}
            </p>
          ) : (
            <>
              <h3 className={styles.detailPlace}>{active.historicalName}</h3>
              <p className={styles.detailModern}>{active.modernName}</p>

              <div className={styles.detailMeta}>
                <span className={styles.tag}>{PRECISION_LABEL[active.precision]}</span>
                {active.isCentroid && <span className={`${styles.tag} ${styles.tagWarn}`}>中心点坐标</span>}
                {active.isBirthplace && <span className={`${styles.tag} ${styles.tagBirth}`}>诗人籍贯/生地</span>}
                <span className={styles.tag}>
                  {active.yearRange[0]}–{active.yearRange[1]}
                </span>
              </div>

              {/* 置信度分布 —— 让用户一眼看到这个点的可靠性构成 */}
              <div className={styles.detailMeta}>
                {(['A', 'B', 'C', 'D'] as const).map((c) =>
                  active.byConfidence[c] > 0 ? (
                    <span
                      key={c}
                      className={styles.tag}
                      style={{
                        color: CONFIDENCE_UI[c].color,
                        borderColor: `${CONFIDENCE_UI[c].color}44`,
                      }}
                    >
                      {c} {CONFIDENCE_UI[c].label} × {active.byConfidence[c]}
                    </span>
                  ) : null,
                )}
              </div>

              <div className={styles.detailWorks}>
                {(placeWorks[active.placeId] ?? []).map((w) => (
                  <Link
                    key={w.workId}
                    href={`/poetry-atlas/work/${w.workId}`}
                    className={styles.detailWorkLink}
                  >
                    <span>{w.title}</span>
                    <span className={styles.detailWorkMeta}>
                      {w.poetName}
                      {w.year ? ` · ${w.year}` : ''}
                    </span>
                  </Link>
                ))}
                {!(placeWorks[active.placeId] ?? []).length && (
                  <p className={styles.mapDetailEmpty}>该地点暂无作品关联。</p>
                )}
              </div>
            </>
          )}
        </aside>
      </div>
    </div>
  );
}
