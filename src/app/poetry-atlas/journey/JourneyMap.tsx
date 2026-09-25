'use client';

/**
 * 一生行迹地图。
 *
 * 把「作品—地点—时间」三者的关系按**时间顺序**摊开：站点按年编号连成一条线，
 * 点开任一站就能看到当年此地发生了什么事、写下了哪些诗。
 * 与图谱主页的「创作地分布」互补——那张图回答「诗写在哪里」，
 * 这张图回答「他一生的路是怎么走的」。
 *
 * 底图双轨与主页一致：有天地图密钥走真实瓦片，否则走自绘墨卡托。
 */

import { useCallback, useEffect, useMemo, useState } from 'react';
import dynamic from 'next/dynamic';
import Link from 'next/link';
import type { JourneyStop, LifeEvent } from '@/poetry-atlas/types';
import {
  CONFIDENCE_UI,
  EVENT_LABEL,
  PRECISION_LABEL,
  TONE_UI,
  formatSource,
  type EventTone,
} from '@/lib/atlas-view';
import { phaseLabelOf } from '../poet-phases';
import { TIANDITU_KEY, TIANDITU_BASEMAPS } from '../tiles';
import { toneOf, groupStopsByCoordinate } from './journey-util';
import styles from '../atlas.module.css';

/** 播放时每站停留时长（毫秒）。够看清地名，又不至于等到不耐烦 */
const STEP_MS = 1150;

const JourneyTileMap = dynamic(() => import('./JourneyTileMap'), {
  ssr: false,
  loading: () => <div className={styles.mapLoading}>正在加载地图…</div>,
});

/**
 * 自绘版保留预渲染：未配密钥时首屏直接就有图，不闪。
 * 它带一份 60KB 的行政区划边界（见 geo.tsx），拆成独立 chunk，
 * 配了密钥的站点就不必为这份不会渲染的数据付流量。
 */
const JourneySvgMap = dynamic(() => import('./JourneySvgMap'));

interface Props {
  poetId: string;
  poetName: string;
  stops: JourneyStop[];
  /** 生平事件全量，供侧栏展开站点内的每一件事 */
  events: LifeEvent[];
  birthYear: number | null;
  deathYear: number | null;
  /** 该诗人既无创作地、系年也落不进任何站的篇数，用来说明「并非全部作品都上了图」 */
  unplacedWorkCount?: number;
}

export default function JourneyMap({
  poetId,
  poetName,
  stops,
  events,
  birthYear,
  deathYear,
  unplacedWorkCount = 0,
}: Props) {
  // 默认落在第 1 站：打开就看到「从哪儿出发」，比空面板更像个开头
  const [selectedSeq, setSelectedSeq] = useState<number | null>(stops.length ? 1 : null);
  const [playing, setPlaying] = useState(false);
  const [basemap, setBasemap] = useState<string>('ter');
  const [dark, setDark] = useState(false);

  const hasKey = TIANDITU_KEY.length > 0;
  const eventById = useMemo(() => new Map(events.map((e) => [e.id, e])), [events]);
  const active = stops.find((s) => s.seq === selectedSeq) ?? null;

  /** 当前站所属的「同址分组」——同一地点的多次驻留会共用地图上一个标记 */
  const activeSiblings = useMemo(() => {
    if (!active) return [];
    const g = groupStopsByCoordinate(stops).find((grp) =>
      grp.some((s) => s.seq === active.seq),
    );
    return (g ?? []).filter((s) => s.seq !== active.seq);
  }, [stops, active]);

  const phaseOf = useCallback(
    (year: number) => phaseLabelOf(poetId, year, birthYear, deathYear),
    [poetId, birthYear, deathYear],
  );

  const atEnd = selectedSeq === stops.length;
  /**
   * 按钮上该显示「暂停」吗。
   *
   * 走到末站后定时器自然不再排下一拍（见下面的 effect），但 `playing`
   * 仍为 true；若直接拿它当按钮态，末站会一直显示「暂停」，点下去却毫无反应。
   * 故按「在播放且未到末站」判定。
   */
  const isPlaying = playing && !atEnd;

  /* ── 播放：每一拍只排下一拍，到末站即自然停下，不循环 ──
   *
   * 用「依赖 selectedSeq 的 setTimeout」而不是「setInterval + 另写一个
   * effect 去关掉它」：后者必须在 effect 体内同步 setState，会触发级联渲染，
   * 也为 react-hooks 规则所禁止。 */
  useEffect(() => {
    if (!playing || selectedSeq === null || selectedSeq >= stops.length) return;
    const id = window.setTimeout(() => {
      setSelectedSeq((s) => Math.min((s ?? 0) + 1, stops.length));
    }, STEP_MS);
    return () => window.clearTimeout(id);
  }, [playing, selectedSeq, stops.length]);

  const step = (d: number) =>
    setSelectedSeq((cur) => {
      const next = (cur ?? 1) + d;
      return Math.max(1, Math.min(next, stops.length));
    });

  /** 站点覆盖到的视觉分组，用于图例（只列实际出现的，不列空分类） */
  const tonesInUse = useMemo(() => {
    const set = new Set<EventTone>();
    for (const s of stops) set.add(toneOf(s));
    return [...set];
  }, [stops]);

  const totalWorks = stops.reduce((n, s) => n + s.works.length, 0);
  const nearbyWorks = stops.reduce((n, s) => n + s.nearbyWorks.length, 0);
  const locatedWorks = totalWorks + nearbyWorks;

  if (!stops.length) {
    return (
      <div className={styles.mapWrap}>
        <p className={styles.mapDetailEmpty} style={{ padding: 22 }}>
          该诗人暂无带地点的生平事件，无法绘制行迹。
        </p>
      </div>
    );
  }

  return (
    <div className={styles.mapWrap}>
      {/* ── 工具栏 ── */}
      <div className={styles.mapToolbar}>
        <div className={styles.journeyNav}>
          <button
            type="button"
            className={styles.journeyNavBtn}
            onClick={() => {
              setPlaying(false);
              step(-1);
            }}
            disabled={selectedSeq === 1}
            aria-label="上一站"
          >
            ‹
          </button>
          <button
            type="button"
            className={`${styles.journeyNavBtn} ${styles.journeyNavPlay}`}
            onClick={() => {
              // 已停在末站时按下播放：从头开始，省得先手动退回起点
              if (!isPlaying && atEnd) setSelectedSeq(1);
              setPlaying(!isPlaying);
            }}
            aria-pressed={isPlaying}
          >
            {isPlaying ? '暂停' : '播放一生'}
          </button>
          <button
            type="button"
            className={styles.journeyNavBtn}
            onClick={() => {
              setPlaying(false);
              step(1);
            }}
            disabled={selectedSeq === stops.length}
            aria-label="下一站"
          >
            ›
          </button>
        </div>

        <span className={styles.journeyCounter}>
          第 <strong>{selectedSeq ?? 1}</strong> / {stops.length} 站
        </span>

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

        {/* 汇总口径与脚注保持一致：这里报「上图的篇数」（含弱关联），
            否则「23 篇」会被读成该诗人作品总数，与脚注的「26 篇」对不上 */}
        <span className={styles.mapHint}>
          {stops.length} 站 · {locatedWorks} 篇作品
          {unplacedWorkCount > 0 && `（另 ${unplacedWorkCount} 篇未系地）`}
        </span>
      </div>

      {/* ── 图例 ── */}
      <div className={styles.journeyLegend}>
        {tonesInUse.map((t) => (
          <span key={t} className={styles.journeyLegendItem}>
            <span
              className={styles.journeyLegendDot}
              style={{ background: TONE_UI[t].color }}
              aria-hidden="true"
            />
            {TONE_UI[t].label}
          </span>
        ))}
        <span className={styles.journeyLegendItem}>
          <span className={styles.journeyLegendRing} aria-hidden="true" />
          据作品系年补出（年谱无载）
        </span>
      </div>

      {/* ── 地图 + 侧栏 ── */}
      <div className={styles.mapCanvas}>
        {hasKey ? (
          <JourneyTileMap
            stops={stops}
            selectedSeq={selectedSeq}
            onSelect={setSelectedSeq}
            basemap={basemap}
            dark={dark}
            playing={isPlaying}
          />
        ) : (
          <JourneySvgMap
            stops={stops}
            selectedSeq={selectedSeq}
            onSelect={setSelectedSeq}
            playing={isPlaying}
          />
        )}

        <aside className={styles.mapDetail}>
          {!active ? (
            <p className={styles.mapDetailEmpty}>
              点击地图上的站点，查看 {poetName} 当年在此地做了什么、写下了哪些诗。
            </p>
          ) : (
            <>
              <div className={styles.journeyStopHead}>
                <span className={styles.journeyStopSeq}>第 {active.seq} 站</span>
                <span className={styles.journeyStopYears}>
                  {active.fromYear === active.toYear
                    ? `${active.fromYear} 年`
                    : `${active.fromYear}–${active.toYear} 年`}
                </span>
              </div>
              <h3 className={styles.detailPlace}>{active.historicalName}</h3>
              {active.modernName && (
                <p className={styles.detailModern}>今{active.modernName}</p>
              )}

              <div className={styles.detailMeta}>
                <span className={styles.tag}>{phaseOf(active.fromYear)}</span>
                {active.types.map((t) => (
                  <span key={t} className={styles.tag}>
                    {EVENT_LABEL[t] ?? t}
                  </span>
                ))}
                <span
                  className={styles.tag}
                  style={{
                    color: CONFIDENCE_UI[active.confidence].color,
                    borderColor: `${CONFIDENCE_UI[active.confidence].color}44`,
                  }}
                >
                  {active.confidence} · {CONFIDENCE_UI[active.confidence].label}
                </span>
                {active.isBirthplace && (
                  <span className={`${styles.tag} ${styles.tagBirth}`}>出生地</span>
                )}
                {active.isAncestralPlace && (
                  <span className={styles.tag}>祖籍／郡望</span>
                )}
                {active.isDeathPlace && <span className={styles.tag}>卒地</span>}
                {active.posthumous && <span className={styles.tag}>身后事</span>}
                {active.source === 'work' && (
                  <span className={`${styles.tag} ${styles.tagWarn}`}>
                    据作品系年补出
                  </span>
                )}
                {active.isCentroid && (
                  <span className={`${styles.tag} ${styles.tagWarn}`}>
                    {PRECISION_LABEL[active.precision]}坐标
                  </span>
                )}
              </div>

              {/* 同址多站提示：地图上只画了一个标记，得说明还有哪几站在这儿 */}
              {activeSiblings.length > 0 && (
                <p className={styles.journeyEventNote}>
                  同一地点另有第{' '}
                  {activeSiblings.map((s) => s.seq).join('、')} 站（
                  {activeSiblings
                    .map((s) =>
                      s.fromYear === s.toYear
                        ? `${s.fromYear} 年`
                        : `${s.fromYear}–${s.toYear} 年`,
                    )
                    .join('、')}
                  ），点地图标记或站序条可切换。
                </p>
              )}

              {/* ── 此地所作 ──
                  排在事件之前：侧栏只有 296px 宽且要滚动，而「这一站写了什么诗」
                  才是这张图要回答的问题；事件是佐证，不该把作品挤到折线以下 */}
              {active.works.length > 0 && (
                <>
                  <p className={styles.journeySectionLabel}>
                    在此地作 <strong>{active.works.length}</strong> 篇
                  </p>
                  <div className={styles.detailWorks}>
                    {active.works.map((w) => (
                      <Link
                        key={w.workId}
                        href={`/poetry-atlas/work/${w.workId}`}
                        className={styles.detailWorkLink}
                      >
                        <span>{w.title}</span>
                        <span className={styles.detailWorkMeta}>
                          {[w.form, w.year].filter(Boolean).join(' · ')}
                        </span>
                      </Link>
                    ))}
                  </div>
                </>
              )}

              {/* ── 系年同期、创作地另载 ──
                  这类关联比「写于此地」弱，必须分开说，不能混进上面的清单 */}
              {active.nearbyWorks.length > 0 && (
                <>
                  <p className={styles.journeySectionLabel}>
                    同期所作（创作地另载）
                  </p>
                  <p className={styles.journeyEventNote}>
                    系年落在本站时间窗内，但史料把创作地记在别处或未系地，
                    故不计入「此地所作」。
                  </p>
                  <div className={styles.detailWorks}>
                    {active.nearbyWorks.map((w) => (
                      <Link
                        key={w.workId}
                        href={`/poetry-atlas/work/${w.workId}`}
                        className={`${styles.detailWorkLink} ${styles.journeyWeakLink}`}
                      >
                        <span>{w.title}</span>
                        <span className={styles.detailWorkMeta}>
                          {w.year ? `${w.year}` : ''}
                          {w.placeName ? ` · 另载于${w.placeName}` : ' · 创作地未定'}
                        </span>
                      </Link>
                    ))}
                  </div>
                </>
              )}

              {/* ── 此站发生的事 ── */}
              <p className={styles.journeySectionLabel}>此站发生的事</p>
              {active.eventIds.length > 0 ? (
                <div className={styles.journeyEvents}>
                  {active.eventIds.map((id) => {
                    const e = eventById.get(id);
                    if (!e) return null;
                    return (
                      <div key={id} className={styles.journeyEvent}>
                        <p className={styles.journeyEventTitle}>
                          <span className={styles.journeyEventType}>
                            {EVENT_LABEL[e.type] ?? e.type}
                          </span>
                          {e.title}
                        </p>
                        <p className={styles.journeyEventDesc}>{e.description}</p>
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
                    );
                  })}
                </div>
              ) : (
                <p className={styles.journeyEventNote}>
                  年谱未载此地此事，本站由作品系年补出。
                </p>
              )}
            </>
          )}
        </aside>
      </div>

      {/* ── 站序条：一眼看完一生，并可跳转 ── */}
      <div className={styles.journeyStrip}>
        {(() => {
          const nodes: React.ReactNode[] = [];
          let lastPhase = '';
          for (const s of stops) {
            const label = phaseOf(s.fromYear);
            if (label !== lastPhase) {
              lastPhase = label;
              nodes.push(
                <span key={`ph-${label}-${s.seq}`} className={styles.journeyStripPhase}>
                  {label}
                </span>,
              );
            }
            const tone = toneOf(s);
            nodes.push(
              <button
                key={`t-${s.seq}`}
                type="button"
                className={`${styles.journeyTick} ${
                  s.seq === selectedSeq ? styles.journeyTickOn : ''
                } ${s.works.length ? styles.journeyTickHasWorks : ''}`}
                style={{ '--pin': TONE_UI[tone].color } as React.CSSProperties}
                onClick={() => {
                  setPlaying(false);
                  setSelectedSeq(s.seq);
                }}
                title={`第 ${s.seq} 站 ${s.historicalName} · ${s.fromYear}–${s.toYear}${
                  s.works.length ? ` · ${s.works.length} 篇` : ''
                }`}
                aria-label={`跳到第 ${s.seq} 站 ${s.historicalName}`}
                aria-current={s.seq === selectedSeq}
              >
                {s.seq}
              </button>,
            );
          }
          return nodes;
        })()}
      </div>

      <p className={styles.journeyFootnote}>
        行迹依年谱与作品系年重建，共 {stops.length} 站，覆盖 {locatedWorks} 篇作品
        {unplacedWorkCount > 0 && `（另有 ${unplacedWorkCount} 篇未系地，未入图）`}。
        其中 {totalWorks} 篇可系于具体站点，即侧栏的「在此地作」
        {nearbyWorks > 0 && `；另 ${nearbyWorks} 篇系年同期而史料把创作地记在别处，单列为「同期所作」`}。
        虚线圆圈的站点是<strong>年谱无载、据作品系年补出</strong>的，与有明确记载的行止可靠程度不同。
        站点之间连弧线、弧上加箭头，只表示先后次序与行进方向，
        <strong>并不代表史料记载了具体路线</strong>。
        同一地点的多次驻留（如三度还乡）坐标相同，合并为一个标记、外圈加环表示次数，
        点标记或站序条可在几次驻留间切换；<strong>坐标本身不作偏移</strong>，以免虚标地理精度。
      </p>
    </div>
  );
}
