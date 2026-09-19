/**
 * 场景插画引擎
 * ============================================================
 * 为生平事件与诗作生成「程序化 SVG 意境图」。
 *
 * 为什么不用位图 / AI 生图：
 *   1. 本项目是纯静态导出（`output: 'export'`），加位图会显著增大产物与部署体积；
 *   2. 位图无法随 light/dark 主题变色，夜景插画在浅色主题下会变成一块黑斑；
 *   3. SVG 可无损缩放、可被 CSS 变量染色、可精确控制对比度以保证文字可读；
 *   4. 与 DESIGN.md「图表：轻量自绘 SVG」的既有约定一致，不引入外部依赖。
 *
 * 设计原则：
 *   - **确定性**：同一输入永远得到同一张图（用字符串哈希作种子，不用 Math.random），
 *     否则每次构建产物 hash 都会变，缓存与 diff 全部失效。
 *   - **语义驱动**：先由地点类型（山/水/城/关/祠/墓/乡）与事件类型
 *     （旅/战/贬/隐/交/生/卒）决定「画面构型」，再由年代与文本决定细节，
 *     而不是随机拼图形——插画应当真的在说这件事。
 *   - **可读性优先**：所有插画都被设计为「背景」，前景文字始终压在低对比度区域，
 *     或由容器加遮罩保证对比度。
 *   - **装饰性**：对读屏软件一律 `aria-hidden`，不承载信息，信息仍在文本里。
 */

import type { LifeEventType, Place } from '@/poetry-atlas/types';

/** 画面构型：由地点类型决定 */
export type SceneMotive =
  | 'mountain' // 山岳：望岳、登高、庐山
  | 'river' // 江河水滨：渡江、夜泊、洞庭
  | 'city' // 城郭市井：长安、成都、扬州
  | 'pass' // 关隘蜀道：潼关、剑门
  | 'temple' // 祠庙楼台：武侯祠、岳阳楼
  | 'tomb' // 陵墓归葬
  | 'village' // 村野田舍：羌村、石壕村
  | 'road' // 行旅道路（无地点的漂泊）

/** 时令与天候：影响配色与元素 */
export type SceneWeather =
  | 'clear' // 晴
  | 'mist' // 烟雨云雾
  | 'rain' // 雨
  | 'wind' // 风
  | 'snow' // 雪
  | 'night'; // 夜

export interface SceneSpec {
  motive: SceneMotive;
  weather: SceneWeather;
  /** 用于确定性随机的种子 */
  seed: string;
  /** 画面主色调（十六进制），通常是品牌的衍生色 */
  hue?: string;
}

/* ────────────────────────────────────────────
   确定性随机（基于字符串哈希）
   ──────────────────────────────────────────── */

function hash(str: string): number {
  let h = 2166136261;
  for (let i = 0; i < str.length; i++) {
    h ^= str.charCodeAt(i);
    h = Math.imul(h, 16777619);
  }
  return h >>> 0;
}

/** 返回 [0,1) 的确定性伪随机数序列 */
function rng(seed: string): () => number {
  let s = hash(seed) || 1;
  return () => {
    s ^= s << 13;
    s ^= s >>> 17;
    s ^= s << 5;
    s >>>= 0;
    return s / 4294967296;
  };
}

const range = (r: () => number, min: number, max: number) => min + r() * (max - min);

/* ────────────────────────────────────────────
   语义映射：地点 / 事件 → 画面构型
   ──────────────────────────────────────────── */

/** 按地点类型推断画面构型 */
export function motiveOfPlace(place?: Pick<Place, 'kind' | 'id' | 'historicalName'>): SceneMotive {
  if (!place) return 'road';
  const id = place.id;
  /**
   * 语义覆盖：有些地点的 `kind` 标的是行政层级（county/city），
   * 但画面语义上更该画成别的样子。逐一显式覆盖，避免「石壕村画成一座城」。
   */
  // 村舍：草堂、羌村、石壕村、同谷（注意 tonggu 必须锚定，否则会误伤 tongguan 潼关）
  if (/caotang|qiangcun|sanhaxiao|tonggu$/.test(id)) return 'village';
  // 关隘蜀道（须先于村舍判断，潼关既是 county 又含 gu 音）
  if (/jianmen|tongguan/.test(id)) return 'pass';
  // 祠庙楼台：岳阳楼、武侯祠、谢朓楼、凤凰台、黄鹤楼
  if (/yueyanglou|wuhou|xietiaolou|fenghuangtai|huanghelou/.test(id)) return 'temple';
  // 陵墓归葬：当涂青山、首阳山
  if (/dangtu-qingshan|shouyang/.test(id)) return 'tomb';
  // 江河水滨：岳阳、浔阳、江陵、公安、潭州、耒阳、忠州
  if (/yueyang$|xunyang|jiangling|gongan|changsha-tan|leiyang|zhongxian|changsha$/.test(id)) return 'river';
  // 山岳（含江边高地白帝城）
  if (/emei|lushan|taibai$|songshan|jingtingshan|baidi/.test(id)) return 'mountain';

  switch (place.kind) {
    case 'mountain':
      return 'mountain';
    case 'river':
    case 'lake':
      return 'river';
    case 'pass':
      return 'pass';
    case 'temple':
      return 'temple';
    case 'tomb':
      return 'tomb';
    case 'city':
      return 'city';
    default:
      return 'village';
  }
}

/**
 * 由事件类型叠加修正构型。
 * 战争、贬谪这类事件，画面重心不是「景」而是「境」，需要换构型。
 */
export function motiveOfEvent(type: LifeEventType, place?: Place | null): SceneMotive {
  const base = motiveOfPlace(place ?? undefined);
  switch (type) {
    case 'war':
      // 战乱画成关隘或行旅，不画具体城池的安宁景象
      return base === 'city' ? 'pass' : base;
    case 'travel':
      return base === 'city' ? 'road' : base;
    case 'death':
      return base === 'road' ? 'river' : base;
    case 'birth':
      return base === 'road' ? 'village' : base;
    default:
      return base;
  }
}

/**
 * 由作品主题 / 名句推断画面构型。
 *
 * 作品页的诗框背景没有「事件地点」可用，只有主题标签与名句，
 * 因此需要一条独立的语义通道。返回 `null` 表示主题不足以定向，
 * 由调用方回退到地点构型。
 *
 * ⚠️ 与 `weatherOf` 同理，顺序即优先级：越具体、越决定画面主体的主题排越前。
 *    这条顺序踩过坑，改动前务必重跑 `tools/_motive-sample.ts`：
 *      · 「孤」在送别组里，曾把「孤舟蓑笠翁」判成 road；
 *      · 「望月」在送别组里，曾让「举头望明月」抢在夜色定向之前。
 *    因此**水体与山岳**这两类「有明确画面主体」的主题，必须排在
 *    情绪/处境类主题（送别、漂泊、孤独）之前。
 */
export function motiveOfTheme(text: string): SceneMotive | null {
  // ① 水体：江/湖/舟/渡——画面主体最明确，优先认领
  //    （含「孤舟」「蓑笠翁」等典型渔隐意象）
  if (/江|湖|舟|渡|泊|洞庭|潮|溪|海|川|渔|蓑笠|白水|流水|绿水/.test(text)) return 'river';
  // ② 山岳：登临/望岳/峰岭
  if (/山水|登高|望岳|望山|庐山|泰山|华山|峰|岭|岩|峡|瀑|泉|岱宗|青山/.test(text)) return 'mountain';
  // ③ 边塞 / 征战：玉门关外，画面主体是关隘
  if (/边塞|从军|征战|战乱|征戍|出塞|关山|塞下|兵车|烽火/.test(text)) return 'pass';
  // ④ 怀古 / 咏史：多写故城遗址、祠庙陵阙
  if (/怀古|咏史|古迹|祠|庙|墓|坟|先主|武侯|赤壁|金陵|故宫|楼/.test(text)) return 'temple';
  // ⑤ 送别 / 怀人 / 羁旅：长亭古道，画面主体是路
  if (/送别|离别|赠|寄|怀人|忆|思乡|望月|明月|秋思|羁旅|漂泊|孤/.test(text)) return 'road';
  // ⑥ 田家 / 村居 / 农事
  if (/田园|村|农|桑|田家|茅屋|草堂|贫|隐居|幽居|闲居|归园/.test(text)) return 'village';
  // ⑦ 宫阙 / 都市 / 宴饮 / 乱后
  if (/都市|都城|洛阳|扬州|成都|长安|宴|酒|春望|衰败|乱后/.test(text)) return 'city';
  // ⑧ 墓志 / 挽歌 / 悼亡
  if (/悼亡|挽|哭|哀|吊/.test(text)) return 'tomb';
  return null;
}

/**
 * 由文本线索推断天候——让插画和事件描述对得上。
 *
 * ⚠️ 顺序即优先级，不可随意调换：
 *   「秋夜暴雨」四个字里同时含「秋」（风）、「夜」、「雨」，
 *   若把「秋」放前面就会画成刮风天，而事件其实是暴雨。
 *   实感天气（雪/雨）优先于季节（秋）优先于昼夜（夜）优先于氛围（雾）。
 */
export function weatherOf(text: string): SceneWeather {
  if (/雪|冰|冻|寒/.test(text)) return 'snow';
  if (/暴雨|大雨|雨|霖|湿|漏|涨/.test(text)) return 'rain';
  if (/秋|霜|落木|萧森|萧瑟|悲秋|风/.test(text)) return 'wind';
  if (/夜|月|烛|星|昏|暮|宵/.test(text)) return 'night';
  if (/雾|云|烟|岚|苍茫|萧然|迷/.test(text)) return 'mist';
  return 'clear';
}

/** 组装完整场景规格 */
export function buildScene(opts: {
  seedParts: (string | number | undefined | null)[];
  motive: SceneMotive;
  text?: string;
  hue?: string;
}): SceneSpec {
  const seed = opts.seedParts.filter(Boolean).join('|');
  return {
    seed,
    motive: opts.motive,
    weather: weatherOf(opts.text ?? ''),
    hue: opts.hue,
  };
}

/* ────────────────────────────────────────────
   配色：随天候 / 昼夜变化的天空与地表
   ──────────────────────────────────────────── */

type Palette = {
  skyTop: string;
  skyBottom: string;
  ridgeFar: string;
  ridgeMid: string;
  ridgeNear: string;
  ground: string;
  accent: string;
  /** 是否属于「暗底」——暗底插画需要更强的外发光才不与文字糊在一起 */
  dark: boolean;
};

const PALETTES: Record<SceneWeather, Palette> = {
  clear: {
    skyTop: '#1b1a33',
    skyBottom: '#3b2f5e',
    ridgeFar: '#4b3f73',
    ridgeMid: '#392f5c',
    ridgeNear: '#282042',
    ground: '#1d1832',
    accent: '#fbbf24',
    dark: true,
  },
  mist: {
    skyTop: '#191a2e',
    skyBottom: '#333a5c',
    ridgeFar: '#4a5178',
    ridgeMid: '#373d5c',
    ridgeNear: '#262a42',
    ground: '#1b1d2e',
    accent: '#94a3b8',
    dark: true,
  },
  rain: {
    skyTop: '#12131f',
    skyBottom: '#232a44',
    ridgeFar: '#333b58',
    ridgeMid: '#262c44',
    ridgeNear: '#191d30',
    ground: '#12141f',
    accent: '#7dd3fc',
    dark: true,
  },
  wind: {
    skyTop: '#241b2e',
    skyBottom: '#4a3044',
    ridgeFar: '#5c3f57',
    ridgeMid: '#452f45',
    ridgeNear: '#2e1f2e',
    ground: '#221a26',
    accent: '#f0abfc',
    dark: true,
  },
  snow: {
    skyTop: '#1c2233',
    skyBottom: '#3d4a68',
    ridgeFar: '#5b6a8a',
    ridgeMid: '#46536f',
    ridgeNear: '#333d54',
    ground: '#232838',
    accent: '#e0e7ff',
    dark: true,
  },
  night: {
    skyTop: '#0b0c17',
    skyBottom: '#1a1b38',
    ridgeFar: '#2c2d52',
    ridgeMid: '#1f2040',
    ridgeNear: '#141529',
    ground: '#0d0e1a',
    accent: '#c4b5fd',
    dark: true,
  },
};

export function paletteOf(weather: SceneWeather): Palette {
  return PALETTES[weather];
}

/* ────────────────────────────────────────────
   图形片段生成
   ──────────────────────────────────────────── */

/** 远山：多层折线山脊，越远越淡（空气透视） */
function ridges(r: () => number, p: Palette, w: number, h: number, layer: number, baseY: number): string {
  const peaks = 4 + Math.floor(r() * 3) + layer;
  const amp = (0.22 - layer * 0.055) * h;
  const pts: string[] = [`M -20 ${h}`];
  for (let i = 0; i <= peaks; i++) {
    const x = (i / peaks) * (w + 40) - 20;
    // 峰高带一点噪声，避免整齐得像锯齿
    const n = Math.sin(i * 1.7 + layer) * 0.28 + (r() - 0.5) * 0.5;
    const y = baseY - amp * (0.55 + n * 0.5) - (i % 2 === 0 ? amp * 0.34 : 0);
    pts.push(`L ${x.toFixed(1)} ${Math.max(6, y).toFixed(1)}`);
  }
  pts.push(`L ${w + 20} ${h}`, 'Z');
  const color = layer === 0 ? p.ridgeFar : layer === 1 ? p.ridgeMid : p.ridgeNear;
  return `<path d="${pts.join(' ')}" fill="${color}"/>`;
}

/** 水面：横向波纹 */
function water(r: () => number, p: Palette, w: number, h: number, y: number): string {
  let out = `<rect x="0" y="${y}" width="${w}" height="${h - y}" fill="url(#waterGrad)"/>`;
  const lines = 5 + Math.floor(r() * 4);
  for (let i = 0; i < lines; i++) {
    const yy = y + ((i + 1) / (lines + 1)) * (h - y);
    const x0 = range(r, 0.02, 0.3) * w;
    const len = range(r, 0.12, 0.4) * w;
    out += `<path d="M ${x0.toFixed(1)} ${yy.toFixed(1)} q ${(len / 2).toFixed(1)} ${range(r, -3, 3).toFixed(1)} ${len.toFixed(1)} 0" stroke="${p.accent}" stroke-opacity="${range(r, 0.1, 0.24).toFixed(2)}" stroke-width="1" fill="none" stroke-linecap="round"/>`;
  }
  return out;
}

/** 城郭：城墙 + 城门 + 塔楼剪影 */
function cityscape(r: () => number, p: Palette, w: number, h: number, y: number): string {
  let out = '';
  const wallH = h * 0.1;
  out += `<rect x="0" y="${y - wallH}" width="${w}" height="${wallH + 4}" fill="${p.ridgeNear}"/>`;
  // 城垛
  const merlon = 16;
  for (let x = 0; x < w; x += merlon * 2) {
    out += `<rect x="${x}" y="${y - wallH - merlon * 0.42}" width="${merlon}" height="${merlon * 0.42}" fill="${p.ridgeNear}"/>`;
  }
  // 塔楼
  const towers = 2 + Math.floor(r() * 2);
  for (let i = 0; i < towers; i++) {
    const tw = range(r, 34, 52);
    const th = range(r, 44, 78);
    const tx = i === 0 ? range(r, w * 0.08, w * 0.2) : range(r, w * 0.68, w * 0.84);
    out += `<rect x="${tx.toFixed(1)}" y="${(y - wallH - th).toFixed(1)}" width="${tw.toFixed(1)}" height="${th.toFixed(1)}" fill="${p.ridgeMid}"/>`;
    // 檐
    out += `<path d="M ${(tx - 7).toFixed(1)} ${(y - wallH - th).toFixed(1)} h ${(tw + 14).toFixed(1)} l -5 -9 h -${(tw + 4).toFixed(1)} z" fill="${p.ridgeFar}"/>`;
  }
  return out;
}

/** 关隘：两山夹峙 + 关门 */
function passGate(r: () => number, p: Palette, w: number, h: number, y: number): string {
  const cx = w * 0.5;
  const gateW = w * 0.13;
  const gateH = h * 0.2;
  let out = '';
  // 两侧峭壁
  out += `<path d="M -10 ${h} L -10 ${h * 0.16} Q ${cx - gateW * 1.9} ${h * 0.3} ${cx - gateW * 0.6} ${y} L ${cx - gateW * 0.6} ${h} Z" fill="${p.ridgeMid}"/>`;
  out += `<path d="M ${w + 10} ${h} L ${w + 10} ${h * 0.16} Q ${cx + gateW * 1.9} ${h * 0.3} ${cx + gateW * 0.6} ${y} L ${cx + gateW * 0.6} ${h} Z" fill="${p.ridgeMid}"/>`;
  // 关门
  out += `<rect x="${cx - gateW / 2}" y="${y - gateH}" width="${gateW}" height="${gateH}" fill="${p.ridgeNear}"/>`;
  out += `<path d="M ${cx - gateW / 2 - 9} ${y - gateH} h ${gateW + 18} l -6 -11 h -${gateW + 6} z" fill="${p.ridgeFar}"/>`;
  out += `<path d="M ${cx - gateW * 0.22} ${y} v -${gateH * 0.6} a ${gateW * 0.22} ${gateW * 0.22} 0 0 1 ${gateW * 0.44} 0 v ${gateH * 0.6} z" fill="${p.accent}" fill-opacity="0.32"/>`;
  return out;
}

/** 楼台祠庙：重檐屋顶 */
function pavilion(r: () => number, p: Palette, w: number, h: number, y: number): string {
  const cx = w * 0.5;
  const bw = w * 0.3;
  const bh = h * 0.17;
  let out = '';
  // 台基
  out += `<rect x="${cx - bw / 2 - 10}" y="${y - 9}" width="${bw + 20}" height="11" fill="${p.ridgeNear}"/>`;
  // 柱身
  out += `<rect x="${cx - bw / 2}" y="${y - 9 - bh}" width="${bw}" height="${bh}" fill="${p.ridgeMid}"/>`;
  for (let i = 1; i < 4; i++) {
    const px = cx - bw / 2 + (bw / 4) * i;
    out += `<rect x="${px - 2}" y="${y - 9 - bh}" width="4" height="${bh}" fill="${p.ridgeNear}" fill-opacity="0.7"/>`;
  }
  // 双重檐
  out += `<path d="M ${cx - bw * 0.78} ${y - 9 - bh} h ${bw * 1.56} l -13 -15 h -${(bw * 1.56 - 26).toFixed(1)} z" fill="${p.ridgeFar}"/>`;
  out += `<path d="M ${cx - bw * 0.6} ${y - 24 - bh} h ${bw * 1.2} l -11 -13 h -${(bw * 1.2 - 22).toFixed(1)} z" fill="${p.ridgeFar}"/>`;
  // 檐角起翘
  const tilt = r() > 0.45;
  if (tilt) {
    out += `<path d="M ${cx - bw * 0.78} ${y - 9 - bh} q -12 -3 -17 -11" stroke="${p.ridgeFar}" stroke-width="2.5" fill="none" stroke-linecap="round"/>`;
    out += `<path d="M ${cx + bw * 0.78} ${y - 9 - bh} q 12 -3 17 -11" stroke="${p.ridgeFar}" stroke-width="2.5" fill="none" stroke-linecap="round"/>`;
  }
  return out;
}

/** 陵墓：封土 + 墓碑 + 松柏 */
function tombScene(r: () => number, p: Palette, w: number, h: number, y: number): string {
  const cx = w * 0.5;
  let out = '';
  // 封土
  out += `<path d="M ${cx - w * 0.14} ${y} q ${w * 0.14} -${h * 0.11} ${w * 0.28} 0 z" fill="${p.ridgeMid}"/>`;
  // 墓碑
  out += `<rect x="${cx - 9}" y="${y - h * 0.13}" width="18" height="${h * 0.13}" rx="2" fill="${p.ridgeFar}"/>`;
  out += `<path d="M ${cx - 9} ${y - h * 0.13} a 9 9 0 0 1 18 0 z" fill="${p.ridgeFar}"/>`;
  // 松柏
  const trees = 2 + Math.floor(r() * 3);
  for (let i = 0; i < trees; i++) {
    const tx = cx + (i % 2 === 0 ? -1 : 1) * range(r, w * 0.16, w * 0.36);
    const th = range(r, h * 0.16, h * 0.26);
    out += `<path d="M ${tx.toFixed(1)} ${y} L ${(tx - 13).toFixed(1)} ${y} L ${tx.toFixed(1)} ${(y - th).toFixed(1)} L ${(tx + 13).toFixed(1)} ${y} Z" fill="${p.ridgeNear}"/>`;
    out += `<rect x="${tx - 2}" y="${y - 3}" width="4" height="7" fill="${p.ridgeNear}"/>`;
  }
  return out;
}

/** 村舍：茅檐低小的三两间屋 + 篱笆 */
function hamlet(r: () => number, p: Palette, w: number, h: number, y: number): string {
  let out = '';
  const homes = 2 + Math.floor(r() * 2);
  for (let i = 0; i < homes; i++) {
    const hw = range(r, w * 0.14, w * 0.2);
    const hh = range(r, h * 0.09, h * 0.14);
    const hx = w * 0.14 + i * (w * 0.26) + range(r, -12, 12);
    out += `<rect x="${hx.toFixed(1)}" y="${(y - hh).toFixed(1)}" width="${hw.toFixed(1)}" height="${hh.toFixed(1)}" fill="${p.ridgeMid}"/>`;
    // 茅草顶
    out += `<path d="M ${(hx - 12).toFixed(1)} ${(y - hh).toFixed(1)} h ${(hw + 24).toFixed(1)} l -14 -${(hh * 0.62).toFixed(1)} h -${(hw - 4).toFixed(1)} z" fill="${p.ridgeNear}"/>`;
    // 窗（暖色灯，仅夜色/雨夜）
    if (r() > 0.4) {
      out += `<rect x="${(hx + hw * 0.52).toFixed(1)}" y="${(y - hh * 0.62).toFixed(1)}" width="9" height="9" fill="${p.accent}" fill-opacity="0.5"/>`;
    }
  }
  // 篱笆
  out += `<path d="M ${w * 0.06} ${y} v -13 M ${w * 0.11} ${y} v -13 M ${w * 0.16} ${y} v -13" stroke="${p.ridgeNear}" stroke-width="2" fill="none"/>`;
  return out;
}

/** 行旅：一条路，一个远远的孤影 */
function roadScene(r: () => number, p: Palette, w: number, h: number, y: number): string {
  const cx = w * 0.5 + range(r, -w * 0.1, w * 0.1);
  let out = '';
  out += `<path d="M ${cx - 8} ${h} Q ${cx - 3} ${y + (h - y) * 0.5} ${cx} ${y} Q ${cx + 3} ${y + (h - y) * 0.5} ${cx + 8} ${h} Z" fill="${p.ridgeNear}" fill-opacity="0.85"/>`;
  // 孤影
  out += `<path d="M ${cx} ${y - 15} a 3.4 3.4 0 1 0 0.1 0 z M ${cx - 4} ${y - 3} l 4 -8 l 4 8 M ${cx - 4} ${y} h 8" stroke="${p.ridgeFar}" stroke-width="1.8" fill="none" stroke-linecap="round"/>`;
  return out;
}

/** 日 / 月 */
function celestial(r: () => number, p: Palette, w: number, h: number, night: boolean): string {
  const cx = range(r, w * 0.16, w * 0.82);
  const cy = range(r, h * 0.13, h * 0.3);
  const rad = range(r, 13, 21);
  if (night) {
    // 月：一弯残月
    return `<path d="M ${cx} ${cy - rad} a ${rad} ${rad} 0 1 0 ${rad * 0.72} ${rad * 1.72} a ${rad * 0.86} ${rad * 0.86} 0 1 1 -${rad * 0.72} -${rad * 1.72} z" fill="${p.accent}" fill-opacity="0.92"/>`;
  }
  return `<circle cx="${cx.toFixed(1)}" cy="${cy.toFixed(1)}" r="${rad}" fill="${p.accent}" fill-opacity="0.85"/>`;
}

/** 星点（仅夜色） */
function stars(r: () => number, w: number, h: number, count: number): string {
  let out = '';
  for (let i = 0; i < count; i++) {
    const x = r() * w;
    const y = r() * h * 0.55;
    const rad = range(r, 0.6, 1.7);
    out += `<circle cx="${x.toFixed(1)}" cy="${y.toFixed(1)}" r="${rad.toFixed(1)}" fill="#fff" fill-opacity="${range(r, 0.18, 0.62).toFixed(2)}"/>`;
  }
  return out;
}

/** 雨丝 */
function rain(r: () => number, p: Palette, w: number, h: number, count: number): string {
  let out = '';
  for (let i = 0; i < count; i++) {
    const x = r() * w;
    const y = r() * h * 0.8;
    const len = range(r, 8, 22);
    out += `<path d="M ${x.toFixed(1)} ${y.toFixed(1)} l -${(len * 0.22).toFixed(1)} ${len.toFixed(1)}" stroke="${p.accent}" stroke-opacity="${range(r, 0.1, 0.3).toFixed(2)}" stroke-width="0.9" stroke-linecap="round"/>`;
  }
  return out;
}

/** 飞雪 */
function snow(r: () => number, p: Palette, w: number, h: number, count: number): string {
  let out = '';
  for (let i = 0; i < count; i++) {
    const x = r() * w;
    const y = r() * h * 0.85;
    out += `<circle cx="${x.toFixed(1)}" cy="${y.toFixed(1)}" r="${range(r, 0.9, 2.3).toFixed(1)}" fill="${p.accent}" fill-opacity="${range(r, 0.25, 0.65).toFixed(2)}"/>`;
  }
  return out;
}

/** 云雾横带 */
function mistBands(r: () => number, p: Palette, w: number, h: number, count: number): string {
  let out = '';
  for (let i = 0; i < count; i++) {
    const y = h * (0.28 + i * 0.13) + range(r, -12, 12);
    const bh = range(r, 9, 20);
    const x = range(r, -w * 0.15, w * 0.3);
    const bw = range(r, w * 0.5, w * 1.15);
    out += `<rect x="${x.toFixed(1)}" y="${y.toFixed(1)}" width="${bw.toFixed(1)}" height="${bh.toFixed(1)}" rx="${(bh / 2).toFixed(1)}" fill="${p.accent}" fill-opacity="${range(r, 0.05, 0.13).toFixed(2)}"/>`;
  }
  return out;
}

/** 风：斜飞的落叶 */
function leaves(r: () => number, p: Palette, w: number, h: number, count: number): string {
  let out = '';
  for (let i = 0; i < count; i++) {
    const x = r() * w;
    const y = r() * h * 0.7;
    const s = range(r, 3, 6.5);
    out += `<path d="M ${x.toFixed(1)} ${y.toFixed(1)} q ${s.toFixed(1)} ${(-s * 0.7).toFixed(1)} ${(s * 1.8).toFixed(1)} ${(-s * 0.2).toFixed(1)} q ${(-s * 0.9).toFixed(1)} ${(s * 0.9).toFixed(1)} ${(-s * 1.8).toFixed(1)} ${(s * 0.2).toFixed(1)} z" fill="${p.accent}" fill-opacity="${range(r, 0.16, 0.42).toFixed(2)}"/>`;
  }
  return out;
}

/* ────────────────────────────────────────────
   主入口：生成完整 SVG
   ──────────────────────────────────────────── */

export interface RenderOptions {
  width?: number;
  height?: number;
  /** 是否显示天候粒子（缩略图可以关掉以省体积） */
  detail?: boolean;
}

/**
 * 生成一张场景插画 SVG 字符串。
 * 输出不含 `<svg>` 外层以外的任何包装，可直接 dangerouslySetInnerHTML。
 */
export function renderScene(spec: SceneSpec, opts: RenderOptions = {}): string {
  const w = opts.width ?? 480;
  const h = opts.height ?? 270;
  const detail = opts.detail ?? true;

  const r = rng(spec.seed);
  const p = paletteOf(spec.weather);
  const night = spec.weather === 'night';
  const groundY = h * 0.72;
  const uid = `s${hash(spec.seed).toString(36)}`;

  // 地平线：不同构型的地平线高度不同
  const horizon = spec.motive === 'mountain' ? h * 0.82 : spec.motive === 'river' ? h * 0.66 : groundY;

  let inner = '';

  // ① 天空
  inner += `<rect width="${w}" height="${h}" fill="url(#${uid}-sky)"/>`;

  // ② 星 / 月 / 日
  if (night && detail) inner += stars(r, w, h, 42);
  if (spec.weather !== 'rain') inner += celestial(r, p, w, h, night);

  // ③ 云气
  if (spec.weather === 'mist') inner += mistBands(r, p, w, h, 4);
  else inner += mistBands(r, p, w, h, 1);

  // ④ 远山（山岳构型多加一层主峰）
  if (spec.motive === 'mountain') {
    inner += ridges(r, p, w, h, 0, horizon);
    inner += ridges(r, p, w, h, 1, horizon);
    // 主峰：一座明显更高的尖峰
    const px = range(r, w * 0.28, w * 0.68);
    const ph = h * range(r, 0.5, 0.66);
    inner += `<path d="M ${(px - w * 0.24).toFixed(1)} ${horizon} L ${px.toFixed(1)} ${(horizon - ph).toFixed(1)} L ${(px + w * 0.26).toFixed(1)} ${horizon} Z" fill="${p.ridgeMid}"/>`;
    inner += `<path d="M ${px.toFixed(1)} ${(horizon - ph).toFixed(1)} L ${(px - w * 0.07).toFixed(1)} ${(horizon - ph * 0.6).toFixed(1)} L ${(px + w * 0.06).toFixed(1)} ${(horizon - ph * 0.62).toFixed(1)} Z" fill="${p.ridgeFar}" fill-opacity="0.55"/>`;
  } else if (spec.motive === 'pass') {
    inner += ridges(r, p, w, h, 0, horizon);
    inner += passGate(r, p, w, h, horizon);
  } else if (spec.motive === 'river') {
    inner += ridges(r, p, w, h, 0, horizon * 0.98);
    inner += ridges(r, p, w, h, 1, horizon);
    inner += water(r, p, w, h, horizon);
  } else {
    inner += ridges(r, p, w, h, 0, horizon);
    inner += ridges(r, p, w, h, 1, horizon);
  }

  // ⑤ 地表与主景
  inner += `<rect x="0" y="${horizon}" width="${w}" height="${h - horizon}" fill="${p.ground}"/>`;

  switch (spec.motive) {
    case 'city':
      inner += cityscape(r, p, w, h, horizon);
      break;
    case 'temple':
      inner += pavilion(r, p, w, h, horizon);
      break;
    case 'tomb':
      inner += tombScene(r, p, w, h, horizon);
      break;
    case 'village':
      inner += hamlet(r, p, w, h, horizon);
      break;
    case 'road':
      inner += roadScene(r, p, w, h, horizon);
      break;
    case 'mountain':
    case 'river':
    case 'pass':
    default:
      // 山/水/关的远景层次已足够，补一层近岸剪影
      inner += `<path d="M 0 ${h} L 0 ${(horizon + h * 0.06).toFixed(1)} Q ${(w * 0.24).toFixed(1)} ${(horizon + h * 0.01).toFixed(1)} ${(w * 0.5).toFixed(1)} ${(horizon + h * 0.07).toFixed(1)} T ${w} ${(horizon + h * 0.03).toFixed(1)} L ${w} ${h} Z" fill="${p.ridgeNear}" fill-opacity="0.55"/>`;
      break;
  }

  // ⑥ 天候粒子（最上层）
  if (detail) {
    if (spec.weather === 'rain') inner += rain(r, p, w, h, 90);
    if (spec.weather === 'snow') inner += snow(r, p, w, h, 90);
    if (spec.weather === 'wind') inner += leaves(r, p, w, h, 22);
    if (spec.weather === 'night') inner += rain(r, p, w, h, 0); // 占位以保持分支对称
  }

  // ⑦ 上下渐隐，便于与卡片背景融合
  inner += `<rect width="${w}" height="${h}" fill="url(#${uid}-fade)"/>`;

  const accent = spec.hue ?? p.accent;

  return `<svg viewBox="0 0 ${w} ${h}" width="100%" height="100%" preserveAspectRatio="xMidYMid slice" role="presentation" aria-hidden="true" focusable="false" xmlns="http://www.w3.org/2000/svg">
<defs>
<linearGradient id="${uid}-sky" x1="0" y1="0" x2="0" y2="1">
<stop offset="0%" stop-color="${p.skyTop}"/>
<stop offset="100%" stop-color="${p.skyBottom}"/>
</linearGradient>
<linearGradient id="${uid}-waterGrad" x1="0" y1="0" x2="0" y2="1">
<stop offset="0%" stop-color="${p.ridgeNear}" stop-opacity="0.9"/>
<stop offset="100%" stop-color="${p.skyTop}" stop-opacity="0.95"/>
</linearGradient>
<linearGradient id="${uid}-fade" x1="0" y1="0" x2="0" y2="1">
<stop offset="0%" stop-color="${p.skyTop}" stop-opacity="0.5"/>
<stop offset="38%" stop-color="${p.skyTop}" stop-opacity="0"/>
<stop offset="100%" stop-color="${p.ground}" stop-opacity="0.72"/>
</linearGradient>
<radialGradient id="${uid}-glow" cx="50%" cy="30%" r="70%">
<stop offset="0%" stop-color="${accent}" stop-opacity="0.2"/>
<stop offset="100%" stop-color="${accent}" stop-opacity="0"/>
</radialGradient>
</defs>
${inner}
<rect width="${w}" height="${h}" fill="url(#${uid}-glow)"/>
</svg>`;
}

/**
 * 供列表/卡片使用的小型符号图。
 * 与 `renderScene` 不同，glyph 只画轮廓、用 `currentColor` 取色，
 * 因此可以直接跟随父级文字颜色，不需要为主题准备两套配色。
 */
export function renderGlyph(motive: SceneMotive, seed: string, size = 48): string {
  const w = size;
  const h = size;
  let inner = '';

  switch (motive) {
    case 'mountain':
      inner += `<path d="M ${w * 0.06} ${h} L ${w * 0.4} ${h * 0.24} L ${w * 0.62} ${h * 0.62} L ${w * 0.78} ${h * 0.42} L ${w * 0.96} ${h} Z" fill="currentColor"/>`;
      break;
    case 'river':
      inner += `<path d="M ${w * 0.08} ${h * 0.62} L ${w * 0.38} ${h * 0.26} L ${w * 0.6} ${h * 0.6} L ${w * 0.92} ${h * 0.3} L ${w * 0.92} ${h * 0.72} L ${w * 0.08} ${h * 0.72} Z" fill="currentColor" fill-opacity="0.72"/>`;
      inner += `<path d="M ${w * 0.12} ${h * 0.82} h ${w * 0.76} M ${w * 0.2} ${h * 0.9} h ${w * 0.6}" stroke="currentColor" stroke-width="1.6" stroke-linecap="round" fill="none"/>`;
      break;
    case 'city':
      inner += `<rect x="${w * 0.12}" y="${h * 0.5}" width="${w * 0.76}" height="${h * 0.28}" fill="currentColor"/>`;
      for (let i = 0; i < 5; i++) {
        inner += `<rect x="${(w * 0.12 + i * w * 0.16).toFixed(1)}" y="${h * 0.44}" width="${w * 0.08}" height="${h * 0.06}" fill="currentColor"/>`;
      }
      inner += `<rect x="${w * 0.4}" y="${h * 0.24}" width="${w * 0.2}" height="${h * 0.26}" fill="currentColor"/>`;
      break;
    case 'pass':
      inner += `<path d="M ${w * 0.04} ${h} L ${w * 0.04} ${h * 0.2} L ${w * 0.36} ${h * 0.52} L ${w * 0.36} ${h} Z" fill="currentColor"/>`;
      inner += `<path d="M ${w * 0.96} ${h} L ${w * 0.96} ${h * 0.2} L ${w * 0.64} ${h * 0.52} L ${w * 0.64} ${h} Z" fill="currentColor"/>`;
      inner += `<rect x="${w * 0.44}" y="${h * 0.6}" width="${w * 0.12}" height="${h * 0.2}" fill="currentColor" fill-opacity="0.65"/>`;
      break;
    case 'temple':
      inner += `<rect x="${w * 0.28}" y="${h * 0.56}" width="${w * 0.44}" height="${h * 0.24}" fill="currentColor" fill-opacity="0.8"/>`;
      inner += `<path d="M ${w * 0.16} ${h * 0.56} h ${w * 0.68} l -${w * 0.1} -${h * 0.12} h -${w * 0.48} z" fill="currentColor"/>`;
      inner += `<path d="M ${w * 0.26} ${h * 0.44} h ${w * 0.48} l -${w * 0.08} -${h * 0.1} h -${w * 0.32} z" fill="currentColor"/>`;
      break;
    case 'tomb':
      inner += `<path d="M ${w * 0.2} ${h * 0.8} q ${w * 0.3} -${h * 0.22} ${w * 0.6} 0 z" fill="currentColor" fill-opacity="0.75"/>`;
      inner += `<rect x="${w * 0.44}" y="${h * 0.42}" width="${w * 0.12}" height="${h * 0.38}" rx="1" fill="currentColor"/>`;
      break;
    case 'village':
      inner += `<rect x="${w * 0.16}" y="${h * 0.6}" width="${w * 0.3}" height="${h * 0.2}" fill="currentColor" fill-opacity="0.82"/>`;
      inner += `<path d="M ${w * 0.1} ${h * 0.6} h ${w * 0.42} l -${w * 0.09} -${h * 0.14} h -${w * 0.24} z" fill="currentColor"/>`;
      inner += `<rect x="${w * 0.56}" y="${h * 0.66}" width="${w * 0.24}" height="${h * 0.14}" fill="currentColor" fill-opacity="0.6"/>`;
      inner += `<path d="M ${w * 0.52} ${h * 0.66} h ${w * 0.32} l -${w * 0.07} -${h * 0.1} h -${w * 0.18} z" fill="currentColor" fill-opacity="0.75"/>`;
      break;
    case 'road':
    default:
      inner += `<path d="M ${w * 0.42} ${h} Q ${w * 0.48} ${h * 0.6} ${w * 0.5} ${h * 0.4} Q ${w * 0.52} ${h * 0.6} ${w * 0.58} ${h} Z" fill="currentColor" fill-opacity="0.6"/>`;
      inner += `<circle cx="${w * 0.5}" cy="${h * 0.34}" r="${w * 0.055}" fill="currentColor"/>`;
      inner += `<path d="M ${w * 0.5} ${h * 0.4} l -${w * 0.09} ${h * 0.24} M ${w * 0.5} ${h * 0.4} l ${w * 0.09} ${h * 0.24} M ${w * 0.5} ${h * 0.64} h ${w * 0.13}" stroke="currentColor" stroke-width="2.2" stroke-linecap="round" fill="none"/>`;
      break;
  }

  return `<svg viewBox="0 0 ${w} ${h}" width="${size}" height="${size}" role="presentation" aria-hidden="true" focusable="false" xmlns="http://www.w3.org/2000/svg">${inner}</svg>`;
}
