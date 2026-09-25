import type { Metadata } from 'next';
import Link from 'next/link';
import { notFound } from 'next/navigation';
import Header from '@/components/Header';
import Footer from '@/components/Footer';
import Reveal from '@/components/Reveal';
import Scene, { SceneGlyph } from '../../scenes/SceneView';
import { motiveOfEvent, motiveOfPlace, weatherOf } from '../../scenes/scene';
import styles from '../../atlas.module.css';
import {
  EVENT_LABEL,
  CONFIDENCE_UI,
  formatDateFull,
  formatSource,
  getAllPoetIds,
  getArtForWork,
  getFeaturedArt,
  getPoetDetail,
  groupAssertions,
} from '@/lib/atlas';
import { phasesOf, phaseLabelOf } from '../../poet-phases';
import JourneyMap from '../../journey/JourneyMap';
import type { LifeEvent } from '@/poetry-atlas/types';

/**
 * 生平分期已抽到 `../../poet-phases`，因为「一生行迹」地图（客户端组件）
 * 也要用它按阶段着色与标注。数据源只有那一处，避免两边漂移。
 */

/** 静态导出：预生成所有诗人页面 */
export function generateStaticParams() {
  return getAllPoetIds().map((id) => ({ id }));
}

export async function generateMetadata({
  params,
}: {
  params: Promise<{ id: string }>;
}): Promise<Metadata> {
  const { id } = await params;
  const poet = getPoetDetail(id);
  if (!poet) return { title: '诗人未找到' };
  return {
    title: `${poet.name} · 生平轨迹与创作分布 | 诗歌图谱`,
    description: poet.summary,
  };
}

/** 事件按时间排序（支持单点日期与区间） */
function orderEvents(events: LifeEvent[]): LifeEvent[] {
  return [...events].sort((a, b) => {
    const ya = 'year' in a.date ? a.date.year : a.date.from.year;
    const yb = 'year' in b.date ? b.date.year : b.date.from.year;
    return ya - yb;
  });
}

function eventYear(e: LifeEvent): string {
  if ('year' in e.date) return formatDateFull(e.date);
  return `${formatDateFull(e.date.from)} – ${formatDateFull(e.date.to)}`;
}

export default async function PoetPage({
  params,
}: {
  params: Promise<{ id: string }>;
}) {
  const { id } = await params;
  const poet = getPoetDetail(id);
  if (!poet) notFound();

  const events = orderEvents(poet.events ?? []);
  const placeById = new Map(poet.places.map((p) => [p.id, p]));

  /** 该诗人的人生分期与「年份 → 分期」查询（兜底分期按其自身生卒年推导） */
  const phases = phasesOf(poet.id, poet.birth?.year, poet.death?.year);
  const phaseOf = (year: number) =>
    phaseLabelOf(poet.id, year, poet.birth?.year, poet.death?.year);

  /** 交游：从断言的 relatedPoetIds 中提取出「与谁有关」 */
  const companions = new Map<string, { name: string; years: number[]; notes: string[] }>();
  for (const a of poet.assertions) {
    for (const rid of a.relatedPoetIds ?? []) {
      const entry = companions.get(rid) ?? { name: rid, years: [], notes: [] };
      const y = a.date?.year;
      if (typeof y === 'number') entry.years.push(y);
      if (a.circumstance) entry.notes.push(a.circumstance);
      companions.set(rid, entry);
    }
  }

  // 作品 → 其断言（用于在列表上标注地点）
  const assertionsByWork = new Map<string, typeof poet.assertions>();
  for (const a of poet.assertions) {
    const list = assertionsByWork.get(a.workId) ?? [];
    list.push(a);
    assertionsByWork.set(a.workId, list);
  }

  /**
   * 作品配图查表。
   *
   * 诗人页会渲染该诗人全部作品，逐条调 `getArtForWork` 会重复读 15 次
   * 同一份 work/<id>.json（每篇作品的配图就存在那个分片里）。
   * 这里一次性建索引，列表里直接命中 Map。
   */
  const artByWork = new Map<string, ReturnType<typeof getArtForWork>>();
  for (const w of poet.works) artByWork.set(w.id, getArtForWork(w.id));
  const artOf = (workId: string) => artByWork.get(workId) ?? null;

  /** 诗人页头图的策展位配图（人工指定，非算法匹配） */
  const poetArt = getFeaturedArt(`poet-${poet.id}`);

  return (
    <>
      <Header />
      <main className={`${styles.atlas} container`}>
        <nav className={styles.breadcrumb} aria-label="面包屑">
          <Link href="/poetry-atlas">诗歌图谱</Link>
          <span>/</span>
          <span>{poet.name}</span>
        </nav>

        {/* ── 诗人信息 ── */}
        <div className={styles.poetHero}>
          <span className={styles.poetHeroAvatar} aria-hidden="true">
            {poet.name.slice(0, 1)}
          </span>
          <div className={styles.poetHeroMain}>
            <h1 className={styles.poetHeroName}>{poet.name}</h1>
            <p className={styles.poetHeroSub}>
              {poet.courtesyName && <>字{poet.courtesyName} · </>}
              {poet.pseudonym && <>号{poet.pseudonym} · </>}
              {poet.dynasty} · {poet.birth?.year ?? '?'}–{poet.death?.year ?? '?'}
            </p>
            <p className={styles.poetHeroSummary}>{poet.summary}</p>
            {/* 籍贯：祖籍与出生地常不同（如苏轼生于眉山，郡望赵郡栾城），
                分开标注才谈得上考据精度。置信度按数据原样标出。 */}
            {poet.nativePlace && poet.nativePlace.length > 0 && (
              <p className={styles.poetHeroOrigin}>
                {poet.nativePlace.map((n, i) => {
                  const place = placeById.get(n.placeId);
                  const verb =
                    n.type === 'birth' ? '生于' : n.type === 'ancestral' ? '祖籍' : '籍';
                  return (
                    <span key={`${n.type}-${n.placeId}`}>
                      {i > 0 && <span className={styles.originSep}> · </span>}
                      {verb}
                      {place?.historicalName ?? n.placeId}
                      {place?.modernName && (
                        <span className={styles.originModern}>（今{place.modernName}）</span>
                      )}
                      <span className={styles.originConf}>{n.confidence}</span>
                    </span>
                  );
                })}
              </p>
            )}
          </div>
        </div>

        {/*
          主图：取该诗人第一个有地点的事件作构型，天候由其代表作的主题决定。
          这里是「一人一张」的视觉名片——不同诗人的主图必然不同，
          因为构型来自各自真实的行迹与境遇。
        */}
        {events.length > 0 && (
          <div className={styles.poetBanner}>
            <Scene
              size="hero"
              seed={`banner|${poet.id}`}
              motive={motiveOfEvent(events[0]!.type, placeById.get(events[0]!.placeId ?? '') ?? null)}
              weather={weatherOf(
                (poet.works[0]?.themes ?? []).join('') + poet.summary + events[0]!.description,
              )}
              art={getFeaturedArt(`poet-${poet.id}`)}
              artOpacity={0.46}
            />
            <div className={styles.poetBannerCap}>
              <SceneGlyph
                motive={motiveOfEvent(events[0]!.type, placeById.get(events[0]!.placeId ?? '') ?? null)}
                seed={poet.id}
                size={18}
              />
              <span>{poet.works.length} 篇作品 · {events.length} 个生平节点</span>
            </div>
            {/* 头图的古画署名：与作品页同理，公版素材也要交代来源 */}
            {poetArt && (
              <span className={styles.poetBannerCredit}>
                {poetArt.artArtist ? `${poetArt.artArtist} ` : ''}
                {poetArt.artTitle}
                <span className={styles.poetBannerCreditSrc}> · 大都会博物馆 公有领域</span>
              </span>
            )}
          </div>
        )}

        {/* ── 一生行迹（地图）──
            放在头图之后、文字之前：这是整页唯一的「一眼看完一生」的视图，
            比任何一段概述都更快建立空间感。作品在图上按站归位，
            点开站点即可看到此地所作，与下方的作品清单互为详略。 */}
        <div className={styles.sectionHead} id="journey">
          <h2 className={styles.sectionTitle}>一生行迹</h2>
          <p className={styles.sectionNote}>
            按年代连成一线 · 点站点看该地所作 · 可播放一生
          </p>
        </div>

        <Reveal>
          <JourneyMap
            poetId={poet.id}
            poetName={poet.name}
            stops={poet.trajectory.stops ?? []}
            events={events}
            birthYear={poet.birth?.year ?? null}
            deathYear={poet.death?.year ?? null}
            unplacedWorkCount={poet.trajectory.unplacedWorkCount ?? 0}
          />
        </Reveal>

        {/* ── 传记背景 ── */}
        {poet.biography && (
          <>
            <div className={styles.sectionHead} style={{ marginTop: 0 }}>
              <h2 className={styles.sectionTitle}>生平概略</h2>
            </div>
            <Reveal>
              <div className={styles.assertionCard} style={{ cursor: 'default' }}>
                <p style={{ fontSize: 14.5, lineHeight: 1.95, color: 'var(--text-secondary)' }}>
                  {poet.biography}
                </p>
              </div>
            </Reveal>
          </>
        )}

        {/* ── 生平轨迹 ── */}
        <div className={styles.sectionHead}>
          <h2 className={styles.sectionTitle}>生平轨迹</h2>
          <p className={styles.sectionNote}>{events.length} 个关键节点</p>
        </div>

        <Reveal>
          <div className={styles.trajectory}>
            {events.map((e, i) => {
              const place = e.placeId ? placeById.get(e.placeId) : undefined;
              const year = 'year' in e.date ? e.date.year : e.date.from.year;
              const prev = i > 0 ? events[i - 1] : undefined;
              const prevYear = prev
                ? 'year' in prev.date
                  ? prev.date.year
                  : prev.date.from.year
                : null;
              const startPhase =
                prevYear === null || phaseOf(prevYear) !== phaseOf(year);
              const phaseLabel = phaseOf(year);

              // 事件插画：构型取自事件发生地的地类，天候取自事件描述里的实感线索
              const motive = motiveOfEvent(e.type, place ?? null);
              const weather = weatherOf(`${e.title} ${e.description}`);

              return (
                <div key={e.id}>
                  {startPhase && (
                    <p className={styles.phaseMark}>
                      <span aria-hidden="true">▘</span>
                      {phaseLabel}
                    </p>
                  )}
                  <div className={`${styles.trajItem} ${styles.trajItemWithScene}`}>
                    <span className={styles.trajDot} aria-hidden="true" />
                    <div className={styles.trajScene}>
                      <Scene size="thumb" seed={`ev|${poet.id}|${e.id}`} motive={motive} weather={weather} />
                    </div>
                    <div className={styles.trajBody}>
                      <p className={styles.trajDate}>
                        {eventYear(e)}
                        <span style={{ marginLeft: 8 }}>
                          {EVENT_LABEL[e.type] ?? e.type}
                        </span>
                        <span
                          style={{
                            marginLeft: 8,
                            color: CONFIDENCE_UI[e.confidence].color,
                          }}
                        >
                          {e.confidence} · {CONFIDENCE_UI[e.confidence].label}
                        </span>
                      </p>
                      <p className={styles.trajTitle}>{e.title}</p>
                      <p className={styles.trajDesc}>{e.description}</p>
                      {place && (
                        <p className={styles.trajPlace}>
                          <span aria-hidden="true">◈</span>
                          {place.historicalName} · 今{place.modernName}
                        </p>
                      )}
                      {e.sources.length > 0 && (
                        <div className={styles.miniSources}>
                          {e.sources.map((s) => (
                            <span key={s.id} className={styles.miniSource}>
                              {s.reliability} · {formatSource(s)}
                            </span>
                          ))}
                        </div>
                      )}
                    </div>
                  </div>
                </div>
              );
            })}
          </div>
        </Reveal>

        {/* ── 作品（按人生阶段分组）── */}
        <div className={styles.sectionHead}>
          <h2 className={styles.sectionTitle}>作品</h2>
          <p className={styles.sectionNote}>
            {poet.works.length} 篇 · 按人生阶段归拢，点击查看创作背景与异说
          </p>
        </div>

        {(() => {
          // 按创作年归入人生阶段，让作品呈现出「人生轨迹」的形状
          const buckets = new Map<string, typeof poet.works>();
          for (const w of poet.works) {
            const as = assertionsByWork.get(w.id) ?? [];
            const primary = groupAssertions(as)[0]?.items[0];
            const y = primary?.date?.year ?? primary?.dateRange?.from.year;
            const label = typeof y === 'number' ? phaseOf(y) : 'unknown';
            buckets.set(label, [...(buckets.get(label) ?? []), w]);
          }

          const order = [...phases.map((p) => p.label), 'unknown'];
          const rendered: React.ReactNode[] = [];

          for (const key of order) {
            const list = buckets.get(key);
            if (!list?.length) continue;
            const label = key === 'unknown' ? '创作年代不详' : key;

            rendered.push(
              <div key={key} className={styles.workPhase}>
                <p className={styles.phaseMark}>
                  <span aria-hidden="true">▘</span>
                  {label}
                  <span style={{ color: 'var(--text-tertiary)', marginLeft: 8 }}>
                    {list.length} 篇
                  </span>
                </p>
                <div className={styles.workList}>
                  {list.map((w, i) => {
                    const as = assertionsByWork.get(w.id) ?? [];
                    const wGroups = groupAssertions(as);
                    const primary = wGroups[0]?.items[0];
                    const place = primary?.placeId
                      ? placeById.get(primary.placeId)
                      : undefined;
                    const contested = wGroups.some((g) => g.contested);

                    return (
                      <Reveal key={w.id} delay={i * 40}>
                        <Link
                          href={`/poetry-atlas/work/${w.id}`}
                          className={styles.workRow}
                        >
                          {/* 作品插画：构图取自「系地」，天候取自主题与名句；
                              有古画配图时由古画提供笔触，SVG 只叠大气层 */}
                          <div className={styles.workRowScene}>
                            <Scene
                              size="thumb"
                              seed={`work|${w.id}`}
                              motive={motiveOfPlace(place)}
                              weather={weatherOf(
                                (w.themes ?? []).join('') + (w.famousLines?.[0] ?? ''),
                              )}
                              art={artOf(w.id)}
                            />
                          </div>
                          <div className={styles.workRowMain}>
                            <div className={styles.workRowTop}>
                              <span className={styles.workRowTitle}>{w.title}</span>
                              {w.form && (
                                <span className={styles.workRowForm}>{w.form}</span>
                              )}
                              {contested && (
                                <span
                                  className={styles.workRowForm}
                                  style={{ color: 'var(--amber)' }}
                                >
                                  存异说
                                </span>
                              )}
                              <span className={styles.workRowPlace}>
                                {place ? place.historicalName : '创作地未定'}
                                {primary?.date ? ` · ${primary.date.year}` : ''}
                              </span>
                            </div>

                            {w.famousLines?.[0] && (
                              <p className={styles.workRowLine}>
                                「{w.famousLines[0]}」
                              </p>
                            )}

                            <div className={styles.workRowTags}>
                              {(w.themes ?? []).map((t) => (
                                <span key={t} className={styles.tag}>
                                  {t}
                                </span>
                              ))}
                            </div>
                          </div>
                        </Link>
                      </Reveal>
                    );
                  })}
                </div>
              </div>,
            );
          }

          return rendered;
        })()}
      </main>
      <Footer />
    </>
  );
}
