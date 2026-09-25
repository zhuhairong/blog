import type { Metadata } from 'next';
import Link from 'next/link';
import { notFound } from 'next/navigation';
import Header from '@/components/Header';
import Footer from '@/components/Footer';
import Reveal from '@/components/Reveal';
import styles from '../../atlas.module.css';
import {
  CONFIDENCE_UI,
  PRECISION_LABEL,
  RELATED_COLOR,
  RELATED_LABEL,
  EVENT_LABEL,
  formatDateFull,
  formatSource,
  getAllWorkIds,
  getPoetDetail,
  getRelatedOf,
  getWorkDetail,
  groupAssertions,
} from '@/lib/atlas';
import Scene from '../../scenes/SceneView';
import { motiveOfPlace, motiveOfEvent, motiveOfTheme, weatherOf } from '../../scenes/scene';
import { getArtForWork } from '@/lib/atlas';
import type { Assertion, Place } from '@/poetry-atlas/types';

export function generateStaticParams() {
  return getAllWorkIds().map((id) => ({ id }));
}

export async function generateMetadata({
  params,
}: {
  params: Promise<{ id: string }>;
}): Promise<Metadata> {
  const { id } = await params;
  const work = getWorkDetail(id);
  if (!work) return { title: '作品未找到' };
  return {
    title: `${work.title} · 创作背景考据 | 诗歌图谱`,
    description: work.famousLines?.[0] ?? undefined,
  };
}

/** 取断言的地点展示文本 */
function placeText(a: Assertion, places: Place[]): { main: string; sub?: string; precision?: Place['precision']; isCentroid?: boolean } {
  if (!a.placeId) return { main: '创作地不可考' };
  const p = places.find((x) => x.id === a.placeId);
  if (!p) return { main: '未知地点' };
  return {
    main: p.historicalName,
    sub: `今${p.modernName}`,
    precision: p.precision,
    isCentroid: p.isCentroid,
  };
}

export default async function WorkPage({
  params,
}: {
  params: Promise<{ id: string }>;
}) {
  const { id } = await params;
  const work = getWorkDetail(id);
  if (!work) notFound();

  const groups = groupAssertions(work.assertions);
  const contestedGroups = groups.filter((g) => g.contested);
  const hasContest = contestedGroups.length > 0;

  // ── 串联：把这篇文章挂回诗人的人生 ──
  const poet = getPoetDetail(work.poetId);
  const related = getRelatedOf(work.id);

  /**
   * 取本项目最重要的「主流断言」的触发事件，用来在诗前先讲清楚
   * 「当时他正经历什么」——这是理解创作情境的前置语境。
   */
  const primaryAssertion =
    work.assertions.find((a) => a.isPrimary) ?? work.assertions[0] ?? null;
  const triggerEvent = primaryAssertion?.triggerEventId
    ? (poet?.events.find((e) => e.id === primaryAssertion.triggerEventId) ?? null)
    : null;
  const triggerPlace =
    triggerEvent?.placeId && poet
      ? (poet.places.find((p) => p.id === triggerEvent.placeId) ?? null)
      : null;
  const triggerYear = triggerEvent
    ? 'year' in triggerEvent.date
      ? formatDateFull(triggerEvent.date)
      : `${formatDateFull(triggerEvent.date.from)} – ${formatDateFull(triggerEvent.date.to)}`
    : null;

  /**
   * ── 插画规格推导 ──
   *
   * 作品页有两处插画，走两条不同的语义通道：
   *
   *  · 诗框背景（poemScene）：诗正文是全页最需要静气的部分，配图取「意境」
   *    而非「叙事」。构型优先看主题标签与名句（山水/江月/田园/边塞），
   *    主题给不出方向时才回退到主流断言的地点类型。天候把主题、名句、
   *    正文串起来一起嗅探，这样《春夜喜雨》会下雨、《江雪》会落雪。
   *
   *  · 创作缘起小图（contextScene）：这里讲的是具体事件，构型直接来自
   *    事件类型 + 事件地点，与诗人页生平轨迹用的是同一套推导，
   *    两处看到同一事件时画面语言一致。
   */
  const primaryPlace =
    (primaryAssertion?.placeId
      ? work.places.find((p) => p.id === primaryAssertion.placeId)
      : undefined) ?? undefined;

  const themeText = [
    ...(work.themes ?? []),
    ...(work.famousLines ?? []),
    work.title,
  ].join(' ');
  const poemMotive =
    motiveOfTheme(themeText) ?? motiveOfPlace(primaryPlace);
  const poemWeather = weatherOf([themeText, work.content.join('')].join(' '));

  const contextMotive = triggerEvent
    ? motiveOfEvent(triggerEvent.type, triggerPlace)
    : null;
  const contextWeather = triggerEvent
    ? weatherOf(`${triggerEvent.title} ${triggerEvent.description}`)
    : null;

  /**
   * 古画配图：只用在「诗框背景」这一处。
   *
   * 创作缘起那块讲的是具体事件，程序化 SVG 已经在准确表达「事件类型 + 地点」
   * （战乱画关隘、贬谪画道路），换成一张意境古画反而丢失了这层语义；
   * 而诗框背景本来就要的是「静气」，古画正好补上 SVG 缺的那份笔触。
   * 各司其职，不混用。
   */
  const art = getArtForWork(work.id);

  return (
    <>
      <Header />
      <main className={`${styles.atlas} container`}>
        <nav className={styles.breadcrumb} aria-label="面包屑">
          <Link href="/poetry-atlas">诗歌图谱</Link>
          <span>/</span>
          <Link href={`/poetry-atlas/poet/${work.poetId}`}>诗人</Link>
          <span>/</span>
          <span>{work.title}</span>
        </nav>

        {/* ── 标题 ── */}
        <div className={styles.workTitleBlock}>
          <h1 className={styles.workTitleMain}>{work.title}</h1>
          <div className={styles.workRowTags}>
            {work.form && <span className={styles.tag}>{work.form}</span>}
            {work.cipai && <span className={styles.tag}>词牌 · {work.cipai}</span>}
            {work.anthology && <span className={styles.tag}>{work.anthology}</span>}
            {(work.themes ?? []).map((t) => (
              <span key={t} className={styles.tag}>
                {t}
              </span>
            ))}
          </div>
        </div>

        {/* ── 生平背景：把作品挂回人生 ── */}
        {triggerEvent && (
          <Reveal>
            <div className={styles.contextBlock}>
              {contextMotive && contextWeather && (
                <Scene
                  motive={contextMotive}
                  weather={contextWeather}
                  seed={`ctx|${work.id}|${triggerEvent.id}`}
                  size="card"
                  className={styles.contextScene}
                />
              )}
              <div className={styles.contextBody}>
                <div className={styles.contextHead}>
                  <span className={styles.contextKey}>创作缘起</span>
                  <span className={styles.contextTitle}>{triggerEvent.title}</span>
                  {triggerYear && <span className={styles.contextDate}>{triggerYear}</span>}
                  <span
                    className={styles.tag}
                    style={{
                      color: CONFIDENCE_UI[triggerEvent.confidence].color,
                      borderColor: `${CONFIDENCE_UI[triggerEvent.confidence].color}44`,
                    }}
                  >
                    {EVENT_LABEL[triggerEvent.type] ?? triggerEvent.type} ·
                    {CONFIDENCE_UI[triggerEvent.confidence].label}
                  </span>
                </div>
                <p className={styles.contextDesc}>{triggerEvent.description}</p>
                {triggerPlace && (
                  <p className={styles.contextPlace}>
                    <span aria-hidden="true">◈</span>
                    {triggerPlace.historicalName} · 今{triggerPlace.modernName}
                    <span style={{ color: 'var(--text-tertiary)' }}>
                      （{PRECISION_LABEL[triggerPlace.precision]}）
                    </span>
                  </p>
                )}
                {triggerEvent.sources.length > 0 && (
                  <div className={styles.miniSources}>
                    {triggerEvent.sources.map((s) => (
                      <span key={s.id} className={styles.miniSource}>
                        {s.reliability} · {formatSource(s)}
                      </span>
                    ))}
                  </div>
                )}
              </div>
            </div>
          </Reveal>
        )}

        {/* ── 正文 ── */}
        <Reveal>
          <div className={styles.poemBox}>
            <Scene
              motive={poemMotive}
              weather={poemWeather}
              seed={`poem|${work.id}|${poemMotive}|${poemWeather}`}
              size="banner"
              className={styles.poemBoxScene}
              art={art}
            />
            <div className={styles.poemBoxBody}>
              {work.content.map((line, i) => {
                /*
                 * 逐句译注与正文是平行数组（第 i 条对第 i 行）。
                 * 数据层已强校验等长，这里仍做一次可选链兜底：
                 * 万一某篇尚未配译注，也只退化为「无小字」，不该炸页。
                 */
                const note = work.lineNotes?.[i];
                return (
                  <div key={i} className={styles.poemLineGroup}>
                    <p className={styles.poemLine}>{line}</p>
                    {note?.trans ? (
                      <div className={styles.poemLineNote}>
                        <p className={styles.poemLineTrans}>{note.trans}</p>
                        {note.note && (
                          <p className={styles.poemLineGloss}>
                            <span className={styles.poemLineGlossKey} aria-hidden="true">
                              注
                            </span>
                            {note.note}
                          </p>
                        )}
                      </div>
                    ) : null}
                  </div>
                );
              })}

              {work.textualVariants?.length ? (
                <div className={styles.variantNote}>
                  <strong>版本异文：</strong>
                  {work.textualVariants.map((v, i) => (
                    <p key={i} style={{ marginTop: 6 }}>
                      「{v.text}」
                      <br />
                      <span style={{ opacity: 0.75 }}>—— 据{v.source.title}</span>
                    </p>
                  ))}
                </div>
              ) : null}
            </div>
          </div>
        </Reveal>

        {/* ── 配图出处 ──
            公版素材的署名是硬要求：CC0 虽不强制，但学术产品必须交代来源，
            否则用户无从核验这张画是什么、凭什么配这首诗。 */}
        {art && (
          <p className={styles.artCredit}>
            <span className={styles.artCreditKey}>配图</span>
            <span>
              {art.artArtist ? `${art.artArtist} ` : ''}
              {art.artTitle}
              {art.artDate ? `（${art.artDate}）` : ''}
            </span>
            <span className={styles.artCreditSrc}>大都会艺术博物馆 · 公有领域</span>
            {art.artURL && (
              <a
                className={styles.artCreditLink}
                href={art.artURL}
                target="_blank"
                rel="noreferrer noopener"
              >
                查看原件
              </a>
            )}
          </p>
        )}

        {/* ── 异说警告 ── */}
        {hasContest && (
          <Reveal>
            <div className={styles.contestedBanner}>
              <span className={styles.contestedIcon} aria-hidden="true">
                !
              </span>
              <p className={styles.contestedText}>
                此作品的创作时空存在<strong>学术争议</strong>。下方并列展示
                {contestedGroups.reduce((n, g) => n + g.items.length, 0)} 种说法，
                各自标注文献依据与置信度，请勿将其中任一视为唯一结论。
              </p>
            </div>
          </Reveal>
        )}

        {/* ── 考据断言（核心）── */}
        <div className={styles.sectionHead}>
          <h2 className={styles.sectionTitle}>
            {hasContest ? '创作背景 · 诸说并列' : '创作背景'}
          </h2>
          <p className={styles.sectionNote}>{work.assertions.length} 条考据记录</p>
        </div>

        {work.assertions.length === 0 ? (
          <div className={styles.empty}>
            <p>此作品尚无考据记录。</p>
            <p style={{ marginTop: 8, fontSize: 12.5 }}>
              在 <code className={styles.code}>poetry-atlas/data/assertions/</code> 中补充断言后重新构建即可。
            </p>
          </div>
        ) : (
          <div className={styles.assertionGroup}>
            {groups.map((group, gi) => (
              <Reveal key={group.key} delay={gi * 70}>
                <div className={styles.assertionGroup}>
                  {group.contested && (
                    <p
                      style={{
                        fontSize: 12.5,
                        color: 'var(--amber)',
                        marginBottom: -4,
                      }}
                    >
                      ▸ 异说组：以下 {group.items.length} 种说法互斥
                    </p>
                  )}
                  {group.items.map((a, ai) => {
                    const conf = CONFIDENCE_UI[a.confidence];
                    const pl = placeText(a, work.places);
                    return (
                      <article
                        key={a.id}
                        className={`${styles.assertionCard} ${
                          a.isPrimary ? styles.assertionPrimary : ''
                        }`}
                      >
                        <div className={styles.assertionHead}>
                          <span
                            className={styles.confBadge}
                            style={{
                              background: `${conf.color}1f`,
                              color: conf.color,
                              border: `1px solid ${conf.color}44`,
                            }}
                          >
                            {a.confidence} · {conf.label}
                          </span>
                          {a.isPrimary && <span className={styles.primaryFlag}>主流说法</span>}
                          <span className={styles.assertionSerial}>
                            {ai + 1} / {group.items.length}
                          </span>
                        </div>

                        <div className={styles.assertionFacts}>
                          <div className={styles.factItem}>
                            <span className={styles.factKey}>创作地点</span>
                            <span
                              className={`${styles.factVal} ${
                                a.placeId ? '' : styles.factValMuted
                              }`}
                            >
                              {pl.main}
                              {pl.sub && (
                                <span
                                  style={{
                                    display: 'block',
                                    fontSize: 12,
                                    color: 'var(--text-tertiary)',
                                    marginTop: 2,
                                  }}
                                >
                                  {pl.sub}
                                </span>
                              )}
                            </span>
                          </div>

                          <div className={styles.factItem}>
                            <span className={styles.factKey}>系年</span>
                            <span
                              className={`${styles.factVal} ${
                                a.date || a.dateRange ? '' : styles.factValMuted
                              }`}
                            >
                              {a.date
                                ? formatDateFull(a.date)
                                : a.dateRange
                                  ? `${formatDateFull(a.dateRange.from)} – ${formatDateFull(a.dateRange.to)}`
                                  : '年代不详'}
                            </span>
                          </div>

                          {pl.precision && (
                            <div className={styles.factItem}>
                              <span className={styles.factKey}>地理精度</span>
                              <span className={styles.factVal}>
                                {PRECISION_LABEL[pl.precision]}
                                {pl.isCentroid && (
                                  <span
                                    style={{
                                      display: 'block',
                                      fontSize: 11.5,
                                      color: 'var(--amber)',
                                      marginTop: 2,
                                    }}
                                  >
                                    坐标为行政区中心点
                                  </span>
                                )}
                              </span>
                            </div>
                          )}

                          {a.setting && (
                            <div className={styles.factItem}>
                              <span className={styles.factKey}>情境要素</span>
                              <span className={styles.factVal}>{a.setting}</span>
                            </div>
                          )}
                        </div>

                        {a.circumstance && (
                          <div className={styles.circumstance}>
                            <span className={styles.circumstanceKey}>创作情境</span>
                            <p className={styles.circumstanceText}>{a.circumstance}</p>
                          </div>
                        )}

                        <div className={styles.rationaleBox}>
                          <span className={styles.rationaleKey}>判断依据</span>
                          <p className={styles.rationaleText}>{a.rationale}</p>
                        </div>

                        <div className={styles.sourceList}>
                          {a.sources.map((s) => (
                            <div key={s.id} className={styles.sourceItem}>
                              <span
                                className={styles.sourceRel}
                                style={{ color: CONFIDENCE_UI[s.reliability].color }}
                              >
                                {s.reliability}
                              </span>
                              <span className={styles.sourceText}>
                                {formatSource(s)}
                                {s.edition && `（${s.edition}）`}
                              </span>
                            </div>
                          ))}
                        </div>
                      </article>
                    );
                  })}
                </div>
              </Reveal>
            ))}
          </div>
        )}

        {/* ── 串联：关联作品 ── */}
        {related.length > 0 && (
          <>
            <div className={styles.sectionHead}>
              <h2 className={styles.sectionTitle}>与之相连</h2>
              <p className={styles.sectionNote}>
                同一段人生际遇、同一片地方、或相近之时写下的作品
              </p>
            </div>
            <Reveal>
              <div className={styles.relatedWrap}>
                {related.map((r) => (
                  <Link
                    key={`${r.workId}-${r.reason}`}
                    href={`/poetry-atlas/work/${r.workId}`}
                    className={styles.relatedCard}
                  >
                    <span
                      className={styles.relatedReason}
                      style={{
                        color: RELATED_COLOR[r.reason],
                        background: `${RELATED_COLOR[r.reason]}1a`,
                        border: `1px solid ${RELATED_COLOR[r.reason]}3d`,
                      }}
                    >
                      {RELATED_LABEL[r.reason]}
                    </span>
                    <p className={styles.relatedTitle}>{r.title}</p>
                    <p className={styles.relatedMeta}>
                      {r.placeName ?? '创作地未定'}
                      {r.year ? ` · ${r.year}` : ''}
                    </p>
                  </Link>
                ))}
              </div>
            </Reveal>
          </>
        )}
      </main>
      <Footer />
    </>
  );
}