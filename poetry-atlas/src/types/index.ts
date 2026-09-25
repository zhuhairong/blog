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

/**
 * 生平事件类型。
 *
 * ⚠️ 本联合是**白名单**：`validate.ts` 会校验每条事件的 type 属于此处，
 * 否则构建失败。新增类型时必须同步 `src/lib/atlas-view.ts` 的 EVENT_LABEL，
 * 否则前端会退化成直接显示英文原文（曾因此让「study」「exile」等词
 * 赤裸裸地出现在诗人页上）。
 */
export type LifeEventType =
  | 'birth'
  | 'death'
  | 'exam' // 科举及第
  | 'office' // 任职
  | 'demotion' // 贬谪去职
  | 'exile' // 流放（罪贬远州，如李白长流夜郎）
  | 'travel' // 漫游
  | 'war' // 战乱/兵燹
  | 'seclusion' // 隐居
  | 'marriage'
  | 'meeting' // 交游、会面、干谒
  | 'study' // 读书求学
  | 'mourning' // 丁忧守制
  | 'creation' // 创作高峰：一次留下数篇代表作
  | 'recognition' // 身后荣典（追谥、赠官、立碑、迁葬）
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

/**
 * 逐句译注。
 *
 * 与 `Work.content` **等长同序**——第 i 条对第 i 行，构建期强校验，
 * 长度不符直接报 error。之所以用平行数组而不是嵌进 content，
 * 是为了让「原文」保持纯净：原文来自公有领域的语料快照，
 * 译注是本站编者自撰的现代解读，两者版权属性不同，不该混在一处。
 */
export interface LineNote {
  /** 白话直译。原文为空行时可留空串（前端跳过渲染） */
  trans: string;
  /** 难点注：典故、生僻字词、古今异义、地名官职。无难点则省略 */
  note?: string;
}

export interface Work {
  id: string;
  title: string;
  /** 别名，如《静夜思》又名《夜思》 */
  aliases?: string[];
  poetId: string;
  /** 正文，逐段/逐句存储以保留结构 */
  content: string[];
  /**
   * 逐句译注，长度必须与 content 相等。
   *
   * ⚠️ 译注为编者自撰，不得抄录现代点校本的注释——`docs/DESIGN.md`
   * 已明确：诗词原文属公有领域，但**现代标点、校勘、注释有版权**。
   */
  lineNotes?: LineNote[];
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

/** 行迹站点上挂的作品引用（构建期烘焙，前端不必再查作品表） */
export interface JourneyWorkRef {
  workId: string;
  title: string;
  /** 系年，未知为 null */
  year: number | null;
  form?: string;
  /** 名句，用于侧栏展示 */
  famousLine?: string;
  /**
   * 创作地历史名。**仅弱关联作品有值**——用来如实说明
   * 「这首诗系年落在此站，但创作地另载于某处」。
   */
  placeName?: string | null;
}

/**
 * 行迹站点：把「同一地点、时间连续」的生平事件合并为一站。
 *
 * 为什么要合并：李白 742/743/744 三年都在长安，若原样铺成三个点，
 * 地图上会叠成一坨，也看不出「长安三年」这个人生段落。
 * 合并的边界是**同地且连续**——李白 730 年也到过长安，但中间隔了嵩山，
 * 所以「一入长安」与「长安三年」是两个站，不会被错误地并成一段。
 */
export interface JourneyStop {
  /** 顺序号，从 1 开始，即行迹上的第几站 */
  seq: number;
  placeId: string;
  /** 历史地名与今地名（构建期查好，前端不再持有 places 表） */
  historicalName: string;
  modernName: string;
  coordinates: [number, number];
  precision: PlacePrecision;
  isCentroid: boolean;
  /** 本站的时间窗（取站内事件的年跨度） */
  fromYear: number;
  toYear: number;
  /** 合并进本站的事件 id */
  eventIds: string[];
  /** 站点标题：单事件取事件标题，多事件取「首题 等 N 事」 */
  title: string;
  /** 站内涉及的事件类型（去重），用于着色 */
  types: LifeEventType[];
  /**
   * 该站从何而来：
   *  - `event` 纯由生平事件构成（年谱骨架）
   *  - `work`  年谱无载，由作品系年补出（如李白 748 年在扬州作《闻王昌龄…》）
   *  - `mixed` 两者兼有
   * 前端须区分展示——「年谱有载」与「据作品推知」的可靠程度不同。
   */
  source: 'event' | 'work' | 'mixed';
  /** 置信度取站内**最低**（最保守）——不拿一条 A 级事件给整站背书 */
  confidence: Confidence;
  /** 站内事件描述拼接 */
  description: string;
  isBirthplace: boolean;
  /**
   * 本站是诗人的郡望／祖籍所在（`nativePlace` 中 `type: 'ancestral'` 的条目）。
   *
   * 与 `isBirthplace` 严格分开：郡望是家族标榜的远祖地，诗人本人可能从未到过，
   * 例如杜甫的京兆杜氏（长安）、李白的陇西成纪。混为一谈会把寓居地错标成出生地。
   */
  isAncestralPlace?: boolean;
  isDeathPlace: boolean;
  /** 身后事件（卒后迁葬、追谥等）。行迹折线在此断开，另行渲染 */
  posthumous: boolean;
  /** 创作地即本站的作品 */
  works: JourneyWorkRef[];
  /** 系年落在本站时间窗内、但创作地另载（或未系地）的作品。弱关联 */
  nearbyWorks: JourneyWorkRef[];
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
  /** 合并后的行迹站点，前端地图直接消费 */
  stops: JourneyStop[];
  /** 未落到任何站点的作品数（既非本地创作、系年也不在任何时间窗内） */
  unplacedWorkCount: number;
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
