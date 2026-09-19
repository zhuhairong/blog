import type { Metadata } from 'next';
import Link from 'next/link';
import Header from '@/components/Header';
import Footer from '@/components/Footer';
import Reveal from '@/components/Reveal';
import AtlasMap from './AtlasMap';
import Scene, { SceneGlyph } from './scenes/SceneView';
import { motiveOfPlace, motiveOfEvent } from './scenes/scene';
import styles from './atlas.module.css';
import {
  CONFIDENCE_UI,
  PRECISION_LABEL,
  getMapPoints,
  getPlaceWorks,
  getPoetDetail,
  getPoetsIndex,
  getFeaturedArt,
  isAtlasBuilt,
} from '@/lib/atlas';
import type { Confidence, PlacePrecision } from '@/poetry-atlas/types';

export const metadata: Metadata = {
  title: '诗歌图谱 · 中国诗人与作品的时空映射',
  description:
    '从地域与时间两个维度，系统梳理中国诗人与作品的创作背景。每篇作品的创作地、系年与情境均标注考据依据与置信度等级。',
  keywords: ['中国诗歌', '诗人', '时空图谱', '唐诗', '宋词', '创作地点', '系年', '考据'],
};

const PRECISION_DESC: Record<PlacePrecision, string> = {
  site: '有确切遗址，坐标可靠',
  township: '史料明确记载到乡镇',
  county: '史料仅记到县，坐标取县城中心',
  prefecture: '史料仅记到州府，坐标取城市中心',
  province: '仅知所属省级区域，位置粗略',
  approximate: '仅知大区域，位置高度不确定',
};

export default function PoetryAtlasPage() {
  if (!isAtlasBuilt()) {
    return (
      <>
        <Header />
        <main className={`${styles.atlas} container`}>
          <div className={styles.pageHead}>
            <span className={styles.eyebrow}>
              <span className={styles.eyebrowDot} />
              Poetry Atlas
            </span>
            <h1 className={styles.title}>中国诗歌时空图谱</h1>
          </div>
          <div className={styles.notBuilt}>
            <p>数据集尚未构建。请在项目根目录执行：</p>
            <p style={{ marginTop: 12 }}>
              <code className={styles.code}>cd poetry-atlas && npm install && npm run build</code>
            </p>
            <p style={{ marginTop: 12 }}>
              构建后派生数据会写入{' '}
              <code className={styles.code}>poetry-atlas/data/derived/</code>，本页将自动呈现。
            </p>
          </div>
        </main>
        <Footer />
      </>
    );
  }

  const poets = getPoetsIndex();
  const points = getMapPoints();
  const placeWorks = getPlaceWorks();

  const totalWorks = poets.reduce((sum, p) => sum + p.workCount, 0);
  const locatedWorks = poets.reduce((sum, p) => sum + p.locatedWorkCount, 0);
  const precisePoints = points.filter((p) => !p.isCentroid).length;

  /**
   * ── 插画规格 ──
   *
   * 首页 banner 取「山岳」构型：中国诗歌的空间想象总归离不开山与水，
   * 它作为全域叙事是安全的，不会指向某一位诗人或某一处地方。
   *
   * 诗人卡片的符号图则由**该诗人生平驻留点的地点类型**决定：
   * 统计其所有事件的所在地类型，取出现最多的一类作为构型，
   * 于是杜甫卡上是村野与关隘、李白卡上是江山——一眼看出其人其诗的地理底色。
   * 事件数据在诗人详情分片里，这里按需读取，避免首页加载全量数据。
   */
  const poetMotiveCache = new Map<string, ReturnType<typeof motiveOfPlace>>();
  const poetMotive = (poetId: string) => {
    const cached = poetMotiveCache.get(poetId);
    if (cached) return cached;

    const detail = getPoetDetail(poetId);
    const counts = new Map<string, number>();
    for (const ev of detail?.events ?? []) {
      const place = detail?.places.find((pl) => pl.id === ev.placeId);
      const key = motiveOfEvent(ev.type, place ?? null);
      counts.set(key, (counts.get(key) ?? 0) + 1);
    }

    let best: ReturnType<typeof motiveOfPlace> = 'road';
    let bestN = 0;
    for (const [k, n] of counts) {
      if (n > bestN) {
        best = k as ReturnType<typeof motiveOfPlace>;
        bestN = n;
      }
    }
    poetMotiveCache.set(poetId, best);
    return best;
  };

  return (
    <>
      <Header />
      <main className={`${styles.atlas} container`}>
        <div className={styles.atlasHero}>
          <Scene
            motive="mountain"
            weather="mist"
            seed="atlas|hero"
            size="hero"
            className={styles.atlasHeroScene}
            art={getFeaturedArt('atlas-hero')}
            artOpacity={0.5}
          />
          <div className={styles.atlasHeroBody}>
            <div className={styles.pageHead}>
              <span className={styles.eyebrow}>
                <span className={styles.eyebrowDot} />
                Poetry Atlas
              </span>
              <h1 className={styles.title}>中国诗歌时空图谱</h1>
              <p className={styles.subtitle}>
                一首诗诞生于特定的人、在特定的地方、于特定的时间、因特定的事。
                本图谱把这四重要素结构化关联，并完整保留学术争议 ——
                同一作品的不同说法并列呈现，每条结论都标注依据与置信度。
              </p>
            </div>
          </div>
        </div>

        <div className={styles.stats}>
          <div className={styles.stat}>
            <span className={styles.statNum}>{poets.length}</span>
            <span className={styles.statLabel}>位诗人</span>
          </div>
          <div className={styles.stat}>
            <span className={styles.statNum}>{totalWorks}</span>
            <span className={styles.statLabel}>篇作品</span>
          </div>
          <div className={styles.stat}>
            <span className={styles.statNum}>{locatedWorks}</span>
            <span className={styles.statLabel}>篇已系地</span>
          </div>
          <div className={styles.stat}>
            <span className={styles.statNum}>{points.length}</span>
            <span className={styles.statLabel}>个创作地</span>
          </div>
          <div className={styles.stat}>
            <span className={styles.statNum}>{precisePoints}</span>
            <span className={styles.statLabel}>处精确遗址</span>
          </div>
        </div>

        {/* ── 地图 ── */}
        <div className={styles.sectionHead}>
          <h2 className={styles.sectionTitle}>创作地分布</h2>
          <p className={styles.sectionNote}>点击地点查看详情 · 虚线圆圈表示坐标为行政区中心点</p>
        </div>

        <Reveal>
          <AtlasMap points={points} placeWorks={placeWorks} />
        </Reveal>

        {/* ── 精度说明 ── */}
        <div className={styles.sectionHead}>
          <h2 className={styles.sectionTitle}>地理精度说明</h2>
          <p className={styles.sectionNote}>史料记到什么精度，我们就只标到什么精度</p>
        </div>

        <Reveal>
          <div className={styles.confidenceLegend}>
            {(Object.keys(PRECISION_LABEL) as PlacePrecision[]).map((key) => {
              const reliable = key === 'site' || key === 'township';
              const color = reliable
                ? '#34d399'
                : key === 'county' || key === 'prefecture'
                  ? '#fbbf24'
                  : '#61617a';
              const count = points.filter((p) => p.precision === key).length;
              return (
                <div key={key} className={styles.legendItem}>
                  <span
                    className={styles.legendBadge}
                    style={{
                      background: `${color}22`,
                      color,
                      border: `1px solid ${color}44`,
                    }}
                    aria-hidden="true"
                  >
                    ●
                  </span>
                  <div>
                    <p className={styles.legendLabel}>
                      {PRECISION_LABEL[key]}
                      {count > 0 && (
                        <span style={{ color: 'var(--text-tertiary)', fontWeight: 400 }}>
                          {' '}
                          · {count}
                        </span>
                      )}
                    </p>
                    <p className={styles.legendDesc}>{PRECISION_DESC[key]}</p>
                  </div>
                </div>
              );
            })}
          </div>
        </Reveal>

        {/* ── 置信度说明 ── */}
        <div className={styles.sectionHead}>
          <h2 className={styles.sectionTitle}>考据置信度分级</h2>
          <p className={styles.sectionNote}>每条结论都标注可靠程度，不掩饰不确定性</p>
        </div>

        <Reveal>
          <div className={styles.confidenceLegend}>
            {(Object.keys(CONFIDENCE_UI) as Confidence[]).map((key) => {
              const c = CONFIDENCE_UI[key];
              return (
                <div key={key} className={styles.legendItem}>
                  <span
                    className={styles.legendBadge}
                    style={{
                      background: `${c.color}22`,
                      color: c.color,
                      border: `1px solid ${c.color}44`,
                    }}
                    aria-hidden="true"
                  >
                    {key}
                  </span>
                  <div>
                    <p className={styles.legendLabel}>{c.label}</p>
                    <p className={styles.legendDesc}>{c.desc}</p>
                  </div>
                </div>
              );
            })}
          </div>
        </Reveal>

        {/* ── 诗人 ── */}
        <div className={styles.sectionHead}>
          <h2 className={styles.sectionTitle}>诗人</h2>
          <p className={styles.sectionNote}>点击进入生平轨迹与作品分布</p>
        </div>

        <div className={styles.poetGrid}>
          {poets.map((p, i) => (
            <Reveal key={p.id} delay={i * 60}>
              <Link href={`/poetry-atlas/poet/${p.id}`} className={styles.poetCard}>
                <Scene
                  motive={poetMotive(p.id)}
                  weather="clear"
                  seed={`atlas|card|${p.id}`}
                  size="card"
                  className={styles.poetCardScene}
                />
                <div className={styles.poetCardInner}>
                  <div className={styles.poetCardHead}>
                    <span className={styles.poetAvatar} aria-hidden="true">
                      {p.name.slice(0, 1)}
                    </span>
                    <div>
                      <p className={styles.poetName}>{p.name}</p>
                      <p className={styles.poetYears}>
                        {p.courtesyName ? `字${p.courtesyName} · ` : ''}
                        {p.birthYear ?? '?'}–{p.deathYear ?? '?'}
                      </p>
                    </div>
                    <span className={styles.dynastyTag}>{p.dynasty}</span>
                  </div>
                  <p className={styles.poetSummary}>{p.summary}</p>
                  <div className={styles.poetCardFoot}>
                    <span className={styles.poetCardGlyph}>
                      <SceneGlyph motive={poetMotive(p.id)} seed={`glyph|${p.id}`} size={15} />
                    </span>
                    <span>
                      作品 <strong>{p.workCount}</strong>
                    </span>
                    <span>
                      已系地 <strong>{p.locatedWorkCount}</strong>
                    </span>
                  </div>
                </div>
              </Link>
            </Reveal>
          ))}
        </div>
      </main>
      <Footer />
    </>
  );
}
