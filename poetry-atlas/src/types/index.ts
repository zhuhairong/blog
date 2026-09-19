/**
 * 中国诗歌时空图谱 — 核心数据模型
 *
 * 设计原则
 * 1. 考据与展示分离：原始史料（raw）→ 考据断言（assertion）→ 派生索引（derived）
 * 2. 异说并存：同一事实允许多条互斥断言共存，用 confidence 与 groupId 表达
 * 3. 精度透明：地点精度如实标注，绝不把县级史料伪装成乡镇级
 * 4. 可溯源：每条考据必须能回溯到文献依据（source）
 * 5. 静态化：所有数据可序列化为 JSON，供纯静态站点构建期消费
 */

// ─────────────────────────────────────────────
// 通用：置信度与溯源
// ─────────────────────────────────────────────

/**
 * 考据置信度分级。
 * 用于向用户如实传达"这条信息有多可靠"，是学术严谨性的体现。
 */
export type Confidence =
  | 'A' // 确定：作者自述、同时代明确记载、有实物/石刻佐证
  | 'B' // 较可靠：权威年谱、正史、主流学界共识
  | 'C' // 存疑：古今说法不一，或仅晚出记载，学界有争议
  | 'D'; // 传说附会：民间传说、地方附会、明显时代错置

/** 置信度的展示元数据 */
export const CONFIDENCE_META: Record<
  Confidence,
  { label: string; desc: string; color: string }
> = {
  A: { label: '确定', desc: '有直接史料或作者自述佐证', color: '#1D9E75' },
  B: { label: '较可靠', desc: '权威年谱或学界主流共识', color: '#378ADD' },
  C: { label: '存疑', desc: '古今说法不一，尚存争议', color: '#BA7517' },
  D: { label: '传说', desc: '民间传说或地方附会，仅供参考', color: '#A32D2D' },
};

/** 文献依据。所有考据结论都必须挂来源，否则不予收录。 */
export interface Source {
  /** 文献标识，如 'jts-tangshu'、'nianpu-dufu-2018' */
  id: string;
  /** 文献名，如《旧唐书·杜甫传》 */
  title: string;
  /** 作者/编者 */
  author?: string;
  /** 版本信息（出版社、年份），便于学术引用 */
  edition?: string;
  /** 卷次/页码定位 */
  locator?: string;
  /** 外部链接（CTEXT、中国哲学书电子化计划等） */
  url?: string;
  /** 该来源本身的可靠性等级 */
  reliability: Confidence;
}

// ─────────────────────────────────────────────
// 时间：纪年
// ─────────────────────────────────────────────

/**
 * 历史纪年。中国古诗系年常以年号纪年，公历仅作近似，
 * 故同时保存"原始表述"与"归一化数值"。
 */
export interface HistoricalDate {
  /** 公历年份（负数表示公元前）。不确定时给近似值并标记 */
  year: number;
  /** 月份 1-12，未知则省略 */
  month?: number;
  /** 日 1-31，未知则省略 */
  day?: number;
  /** 原始纪年表述，如"开元十四年""元丰七年十月" */
  original: string;
  /** 不确定性：-N/+N 表示前后浮动年数；0 表示确定 */
  fuzziness?: number;
  /** 是否仅为区间下界/上界 */
  certainty: 'exact' | 'approximate' | 'range' | 'unknown';
}

/** 年份区间表达，用于系年不确的作品 */
export interface DateRange {
  from: HistoricalDate;
  to: HistoricalDate;
}

// ─────────────────────────────────────────────
// 地点：古今双轨制
// ─────────────────────────────────────────────

/**
 * 地理精度层级。**本项目核心约定**：
 * 史料记载到什么精度，就只标到什么精度，绝不强行下推。
 */
export type PlacePrecision =
  | 'province' // 省级
  | 'prefecture' // 地级市 / 历史上的"州""府"
  | 'county' // 县级 / 历史上的"县"
  | 'township' // 乡镇级（史料明确记载，如"巩县东二里"）
  | 'site' // 具体地点（草堂、楼阁、渡口、山寺）
  | 'approximate'; // 仅知大区域（如"江南""塞外"）

/**
 * 地点（古今双轨）。
 * 中国历史地名变迁剧烈，必须同时保存"当时叫什么"和"现在在哪"。
 */
export interface Place {
  /** 稳定标识，如 'cn-henan-gongyi' */
  id: string;
  /** 历史地名（诗作当时的名称），如"巩县" */
  historicalName: string;
  /** 历史地名所属朝代，如"唐" */
  historicalEra?: string;
  /** 今地名完整路径，如"河南省郑州市巩义市" */
  modernName: string;
  /** 今地名层级链，便于聚合与筛选 */
  modernHierarchy: {
    province: string;
    city?: string;
    district?: string;
    township?: string;
    village?: string;
  };
  /** 经纬度 [lng, lat]。精度不足时为所属行政区的中心点 */
  coordinates: [number, number];
  /** 坐标对应的实际精度，诚实标注 */
  precision: PlacePrecision;
  /** 坐标是否为行政区中心点（而非精确遗址点） */
  isCentroid: boolean;
  /** 别名/异称，如"汴京"="汴梁"="东京" */
  aliases?: string[];
  /** 该地点的地理类型，用于差异化渲染 */
  kind?: 'city' | 'mountain' | 'river' | 'lake' | 'pass' | 'temple' | 'tomb' | 'other';
  /** 考据说明：为何定在此处 */
  note?: string;
  /** 完整溯源 */
  sources: Source[];
}

// ─────────────────────────────────────────────
// 人物：诗人
// ─────────────────────────────────────────────

export interface Poet {
  id: string;
  /** 常用名，如"杜甫" */
  name: string;
  /** 字、号 */
  courtesyName?: string;
  pseudonym?: string;
  /** 别称/异名，用于检索，如"杜工部""少陵野老" */
  aliases: string[];
  /** 朝代：'唐' | '宋' | ... */
  dynasty: string;
  birth?: HistoricalDate;
  death?: HistoricalDate;
  /** 籍贯（祖籍/出生地，注意二者常不同） */
  nativePlace?: {
    placeId: string;
    /** 'ancestral' 祖籍 / 'birth' 出生地 / 'registered' 户籍 */
    type: 'ancestral' | 'birth' | 'registered';
    note?: string;
    confidence: Confidence;
  }[];
  /** 生平大事记，构成"人生轨迹"的骨架 */
  events: LifeEvent[];
  /** 简历式概述，用于列表页 */
  summary: string;
  /** 详情页长文背景 */
  biography?: string;
  /** 外部权威库标识，便于与 CBDB 等对接 */
  externalIds?: {
    cbdb?: string;
    wikidata?: string;
  };
  sources: Source[];
}

/** 生平事件类型 */
export type LifeEventType =
  | 'birth'
  | 'death'
  | 'exam' // 科举及第
  | 'office' // 任职
  | 'demotion' // 贬谪
  | 'travel' // 漫游
  | 'war' // 战乱/兵燹
  | 'seclusion' // 隐居
  | 'marriage'
  | 'meeting' // 交游、会面
  | 'other';

/**
 * 生平事件。它是"作品创作情境"的主要推导依据：
 * 为什么这首诗写于此地此时，往往由一个事件解释。
 */
export interface LifeEvent {
  id: string;
  poetId: string;
  type: LifeEventType;
  /** 事件标题，如"安史之乱爆发""贬为江州司马" */
  title: string;
  /** 事件发生地 */
  placeId?: string;
  /** 事件时间 */
  date: DateRange | HistoricalDate;
  /** 事件描述 */
  description: string;
  confidence: Confidence;
  sources: Source[];
}

// ─────────────────────────────────────────────
// 作品：诗篇
// ─────────────────────────────────────────────

export interface Work {
  id: string;
  title: string;
  /** 别名，如《静夜思》又名《夜思》 */
  aliases?: string[];
  poetId: string;
  /** 正文，逐段/逐句存储以保留结构 */
  content: string[];
  /** 体裁：五言绝句、七言律诗、词牌名等 */
  form?: string;
  /** 词牌名（仅词） */
  cipai?: string;
  /** 出处文集，如《全唐诗》卷226 */
  anthology?: string;
  /** 版本异文备忘 */
  textualVariants?: { text: string; source: Source }[];
  /** 名句摘录，用于列表页展示 */
  famousLines?: string[];
  /** 主题标签：思乡、边塞、送别、怀古…… */
  themes?: string[];
  /** 原始语料来源标识 */
  corpusRef?: { dataset: string; path: string };
}

// ─────────────────────────────────────────────
// 核心：考据断言
// ─────────────────────────────────────────────

/**
 * 考据断言（Assertion）。
 *
 * 这是本数据模型最关键的设计：**作品与时空的关联不是事实，而是断言。**
 * 一条断言表达"某文献/某学者认为，某作品创作于某地某时，情境如何"。
 *
 * 因此同一作品可挂多条断言，例如《静夜思》：
 *   - 断言1：作于扬州旅舍，开元十四年，置信度 C（异说）
 *   - 断言2：作于安陆，置信度 C（异说）
 * 前端并存展示，标注来源，由用户判断。这是学术诚信的底线。
 */
export interface Assertion {
  id: string;
  /** 断言的主体：作品 */
  workId: string;
  /** 关联诗人（冗余存储，便于按诗人聚合，需校验一致性） */
  poetId: string;

  // ── 时空坐标 ──
  /** 创作地点。可能不止一处（如"途经某地时作"） */
  placeId?: string;
  /** 系年。可以是精确年份，也可以是区间 */
  date?: HistoricalDate;
  dateRange?: DateRange;

  // ── 情境 ──
  /** 创作情境描述：当时发生了什么、心绪如何 */
  circumstance?: string;
  /** 触发本作品的具体事件 */
  triggerEventId?: string;
  /** 交游对象（与谁同游、赠答何人） */
  relatedPoetIds?: string[];
  /** 气候、节令、场合等氛围要素 */
  setting?: string;

  // ── 史料 ──
  /** 判断依据说明（为何定于此地此时） */
  rationale: string;
  /** 支撑文献 */
  sources: Source[];
  confidence: Confidence;

  /**
   * 异说分组。同一作品的互斥断言共享同一个 groupId，
   * 前端据此把它们并列展示为"异说"，而非当成多个事实。
   */
  groupId?: string;
  /** 该断言是否为当前学界较主流的一种（用于默认选中） */
  isPrimary?: boolean;
}

// ─────────────────────────────────────────────
// 派生索引（构建期生成，不入版本库）
// ─────────────────────────────────────────────

/** 供前端地图使用的聚合点 */
export interface DerivedMapPoint {
  placeId: string;
  /** 今地名，供地图弹窗直接展示，避免前端再查一次 */
  modernName: string;
  /** 历史地名，用于标注"当时叫xx" */
  historicalName: string;
  coordinates: [number, number];
  precision: PlacePrecision;
  /** 是否为行政区中心点。前端据此弱化渲染（半透明/虚线圈），避免误示为精确遗址 */
  isCentroid: boolean;
  /** 该点关联的作品数 */
  workCount: number;
  /** 是否为某位诗人的籍贯/出生地。地图上用特殊符号标记，体现"人"的维度 */
  isBirthplace?: boolean;
  /** 该点关联的生平事件数（出生、贬谪、任职、交游等） */
  eventCount?: number;
  /** 按置信度分的作品数，用于图层过滤 */
  byConfidence: Record<Confidence, number>;
  /** 涉及的诗人 */
  poetIds: string[];
  /** 年代跨度 */
  yearRange: [number, number];
}

/** 诗人轨迹线 */
export interface DerivedTrajectory {
  poetId: string;
  /** 按时间排序的驻留点 */
  waypoints: {
    placeId: string;
    date: HistoricalDate;
    eventId?: string;
  }[];
}

/** 构建产物清单 */
export interface BuildManifest {
  generatedAt: string;
  counts: {
    poets: number;
    works: number;
    places: number;
    assertions: number;
  };
  /** 数据完整性问题（缺坐标、无来源等） */
  warnings: ValidationIssue[];
}

// ─────────────────────────────────────────────
// 校验
// ─────────────────────────────────────────────

export type IssueSeverity = 'error' | 'warning' | 'info';

export interface ValidationIssue {
  severity: IssueSeverity;
  /** 问题类别，如 'missing-source'、'orphan-place' */
  code: string;
  /** 出问题的实体 */
  entity: { type: string; id: string };
  message: string;
  /** 修复建议 */
  hint?: string;
}
