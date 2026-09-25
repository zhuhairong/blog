# 中国诗歌时空图谱 · Poetry Atlas

从**地域**与**时间**两个维度，系统映射中国诗人与作品的创作背景。

> 一首诗诞生于特定的人、在特定的地方、于特定的时间、因特定的事。
> 本项目把这四重要素结构化关联起来，并**完整保留学术争议**。

---

## 快速开始

```bash
cd poetry-atlas
npm install
npm run build      # 校验数据 + 生成静态派生分片
```

构建成功后会输出到 `data/derived/`，被前端静态消费。

---

## 目录结构

```
poetry-atlas/
├── docs/
│   └── DESIGN.md            完整设计文档（先读这个）
├── schema/
│   ├── place.schema.json    地点 JSON Schema
│   └── assertion.schema.json 考据断言 JSON Schema
├── data/
│   ├── raw/                 ① 采集层：外部数据原始快照（不手工修改）
│   ├── people/poets.json    ② 考据层：诗人及其生平事件
│   ├── corpus/works.json    ② 考据层：作品文本
│   ├── places/places.json   ② 考据层：地点（古今双轨）
│   ├── assertions/          ② 考据层：考据断言（核心）
│   └── derived/             ③ 派生层：构建期生成的索引分片
└── src/
    ├── types/index.ts       核心类型定义
    └── pipeline/
        ├── build.ts         构建管线
        └── validators/
            └── validate.ts  数据校验器
```

---

## 核心设计：为什么用「考据断言」

传统做法是给每首诗直接挂 `location` 和 `date` 字段。这有个致命问题：

> 《静夜思》写于何处？**扬州说、安陆说、不可考说**三者并存。
> 单一字段要么强行选一个（学术不诚实），要么丢弃其余（信息丢失）。

因此本项目引入 **Assertion（考据断言）**：

**一条断言 = 一种说法。** 同一作品可挂多条互斥断言，用 `groupId` 归组，
前端并列展示为「异说」，由用户判断。

```jsonc
// data/derived/work/libai-jingyesi.json 的实际输出
{
  "title": "静夜思",
  "assertions": [
    { "confidence": "C", "groupId": "jingyesi-place", "placeId": "cn-jiangsu-yangzhou",  "isPrimary": true  },
    { "confidence": "C", "groupId": "jingyesi-place", "placeId": "cn-anhui-chizhou" },
    { "confidence": "C", "groupId": "jingyesi-place", "placeId": null }  // 明确标注"不可考"
  ]
}
```

---

## 置信度分级

| 等级 | 含义 | 判定标准 | 示例 |
|---|---|---|---|
| **A** | 确定 | 作者自述、同时代记载、实物佐证 | 《水调歌头》词序自述「丙辰中秋…作此篇」 |
| **B** | 较可靠 | 权威年谱、正史、学界共识 | 《春望》系于至德二载 |
| **C** | 存疑 | 古今说法不一，或仅晚出记载 | 《静夜思》作地 |
| **D** | 传说 | 民间附会、明显时代错置 | 各类「李白醉草」传说 |

---

## ⚠️ 地理精度：必须诚实标注

**古诗创作地的史料记载，绝大多数只到「州」「县」一级。**

这是客观事实，不是数据缺陷。处理方式是**分级透明标注**：

| 精度 | 含义 | 坐标策略 |
|---|---|---|
| `site` | 具体遗址 | 实测精确坐标，`isCentroid: false` |
| `township` | 乡镇级 | 乡镇中心 |
| `county` | 县级 | 县城中心，`isCentroid: true` |
| `prefecture` | 州府级 | 城市中心 |
| `province` | 省级 | 省会坐标 |
| `approximate` | 大区域 | 区域中心，界面须明确弱化 |

**前端硬性要求**：`isCentroid: true` 的点必须弱化呈现（半透明／虚线圈），
不得让用户误以为是精确遗址位置。这既是学术诚信，也是商用免责底线。

---

## 数据校验规则

`npm run build` 会先校验，**有 error 则中止构建，不写入任何派生数据**。

### 阻断级（error）
- `duplicate-id` — id 重复
- `orphan-*` — 引用了不存在的实体
- `missing-source` — 断言／地点缺少文献来源
- `missing-rationale` — 断言缺少判断依据
- `invalid-coordinates` / `coordinates-out-of-china` — 坐标非法
- `poet-mismatch` — 断言与作品的作者不一致
- `empty-dataset` — **数据为空**（防止路径错误导致静默成功）
- `line-notes-length-mismatch` — 逐句译注与正文行数不等（平行数组错位）
- `missing-line-translation` — 非空正文行缺少白话直译

### 告警级（warning）
- `assertion-no-temporospatial` — 断言既无地点也无时间
- `contradictory-group` — 异说组全部标为 A 级（互斥说法不应同时确定）
- `empty-content` — 作品正文为空

### 提示级（info）
- `centroid-location` — 使用行政区中心点，前端需弱化渲染
- `works-without-location` — 覆盖率统计
- `line-notes-coverage` — 逐句译注覆盖率（未全覆盖时才报）

---

## 如何新增一位诗人

1. **地点**：在 `data/places/places.json` 添加涉及的地点。
   - 必填 `sources`，坐标必须在中国境内 [73-136, 3-54]
   - 史料只到县级就写 `precision: "county"` + `isCentroid: true`，**不要虚标**

2. **诗人**：在 `data/people/poets.json` 添加。
   - `events` 数组是关键——它解释"作品为何写于此地此时"
   - 每个事件都要有 `sources` 和 `confidence`

3. **作品**：在 `data/corpus/works.json` 添加。
   - `content` 一律**一联一条**（律诗 4 条、绝句 2 条、《长恨歌》60 条）
   - 同时要写 `lineNotes`：与 `content` **等长同序**的逐句译注，
     第 i 条对第 i 行，长度不符构建直接报错
   - `lineNotes[i].trans` 是白话直译；有典故／生僻字／古今异义时再加 `lineNotes[i].note`
   - ⚠️ 译注**必须自撰**。诗词原文属公有领域，但现代点校本的标点、校勘、
     **注释**有版权（见 `docs/DESIGN.md`），抄点校本即为侵权

4. **考据断言**：在 `data/assertions/assertions.json` 添加。
   - `rationale` 写清**判断依据**：「为何定于此地此时」
   - `sources` 列出支撑文献
   - 若存在异说，多条断言共用同一 `groupId`，主流说法标 `isPrimary: true`
   - 宁可标 C（存疑），也不要为了好看而虚标 A

5. 运行 `npm run build` 验证。

---

## 数据源与许可

| 数据源 | 内容 | 许可 | 可商用 |
|---|---|---|---|
| [chinese-poetry](https://github.com/chinese-poetry/chinese-poetry) | 5.5万唐诗 + 26万宋诗 + 2.1万宋词 | **MIT** | ✅ |
| [CBDB](https://chinese- CBDB.hsites.harvard.edu/) 哈佛中国历代人物传记库 | 65万人物 | 学术开放，需引用 | 需确认 |
| [CHGIS](https://chgis.fas.harvard.edu/) 复旦／哈佛历史地理 | 历代政区地名 | 学术开放，需引用 | 需确认 |

**版权注意**：
- 诗词**原文**属公有领域，可自由使用
- 但中华书局等**现代点校本**的标点、校勘有版权，不可直接复制
- 现代注本、鉴赏辞典为专有内容，商用需授权
- 考据结论可引用观点但须注明出处

---

## 地图合规提醒

⚠️ 在中国大陆提供地图服务须使用**有测绘资质**的数据源，未经批准不得使用境外地图。

**本项目的做法**（见 `src/app/poetry-atlas/tiles.ts`）：

- 底图**只用天地图**（国家地理信息公共服务平台，具备甲级测绘资质）。
  国界、行政界线、台湾及南海诸岛等领土要素一律由天地图按国家标准画法渲染，
  本项目不自绘界线、不改动、不叠加境外底图。
- **明确不用**：Google Maps、Apple Maps、Bing（境外版）、OpenStreetMap、
  Mapbox 等无资质数据源。
- 密钥走 `NEXT_PUBLIC_TIANDITU_KEY`，构建期内联进静态产物。
  未配置时不报错，自动回落到自绘的等距圆柱示意底图（`SvgFallbackMap.tsx`），
  该示意图只是点位相对位置的示意，**不含任何界线画法**，因此也不涉及界线合规问题。
- 投影陷阱：天地图 `_c` 是经纬度投影、`_w` 才是球面墨卡托。
  Leaflet 默认 CRS 为 EPSG:3857，必须用 `_w`，否则整张图错位。
  已核对官方 GetCapabilities：`_w` 的 TileMatrix 编号与 Leaflet 的 zoom 一一对应，无需 zoomOffset。

申请密钥：<https://console.tianditu.gov.cn/> → 应用管理 → 创建新应用（类型选「浏览器端」）。

---

## 当前状态

数据集：**7 位诗人 / 165 篇作品 / 105 个地点 / 187 条考据断言 / 141 条生平事件**
逐句译注覆盖 165 篇全部正文行；配图 109 幅（大都会公版古画，同一诗人内不重复）。

已验证：
- ✅ 类型检查与 ESLint 通过（`npx tsc --noEmit` / `npx eslint src`）
- ✅ 构建管线跑通（校验 0 error / 0 warning）
- ✅ 校验器能正确拦截孤儿引用、缺来源、缺依据、译注与正文行数不符
- ✅ 异说并存机制正常（《静夜思》3 条互斥断言；《忆秦娥》《菩萨蛮》并列作者归属异说）
- ✅ 精度分级生效（草堂 `site` vs 密州 `county` 中心点）

下一步见 `docs/DESIGN.md` 的路线图。

---

## 「串联」机制：让作品不再是孤岛

一首诗总有邻居——同一段人生际遇、同一片地方、或相近之时写下的作品。
本项目把这层关系在**构建期**算好，前端直接消费。

### 三类关联强度

| 关联 | 含义 | 强度 |
|---|---|---|
| `same-event` | 由**同一个生平事件**触发（如「赐金放还」同时催生《行路难》与《月下独酌》）| 最强 |
| `same-place` | 写于**同一地点**（如长安任内多篇）| 中 |
| `same-time` | 创作年相差 **±2 年内** | 弱 |

产物：`data/derived/related-works.json`

### 三层串联在界面上如何呈现

1. **作品页「创作缘起」**——把作品挂回触发它的生平事件，含事件描述、
   地点（带精度）、以及事件本身的文献溯源。读者先看到「他当时正经历什么」。
2. **作品页「与之相连」**——关联作品卡片，按关联强度着色排序。
3. **诗人页人生分期**——生平事件与作品清单都按
   蜀中成长 / 出蜀漫游 / 长安三年 / 梁宋东鲁 / 安史乱中 / 晚年漂泊
   六个阶段归组，让「作品清单」呈现为「人生轨迹的形状」。

### 设计要点：为什么关联要在构建期算

静态导出没有服务端，任何「反查」都要遍历全量数据。
把关联预先算进 JSON 分片，前端只做渲染，页面加载零计算。
这与「地点→作品」用 `place-works.json` 是同一思路。
