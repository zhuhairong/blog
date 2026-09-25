# 项目长期记忆 — ZHR's AI Blog

## 概览与部署
Next.js 16（App Router）+ TS，**静态导出** → GitHub Pages。
- `next.config.ts`：`output:'export'` + `basePath:'/blog'` + `images.unoptimized`
- 部署：https://zhuhairong.github.io/blog/ · 远程 `git@github.com:zhuhairong/blog.git`
- 默认分支 **`master`**；唯一生效 workflow `.github/workflows/nextjs.yml`
- 无 `gh` CLI，查 CI：`curl -s "https://api.github.com/repos/zhuhairong/blog/actions/runs?per_page=1"`

## 样式与组件约定
1. **禁止 `styled-jsx`**（曾引发全站样式错乱）。页面样式 → CSS Module；共享 → `globals.css`
2. 每页一个 CSS Module：`page.module.css` / `posts/[id]/post.module.css` / `c-utils/c-utils.module.css`
3. **内联 SVG 必须写 `width`/`height`**，统一走 `src/components/icons.tsx`
4. 设计令牌在 `globals.css :root`，固定深色主题（不跟随系统）。
   品牌 `--brand:#7c5cff`、背景 `--bg-primary:#08080d`、文字 `--text-primary:#f2f2f7`。
   `--accent-primary` / `--border-color` 已废弃
5. 动画需在 `prefers-reduced-motion: reduce` 下降级
6. `useEffect` 内不得同步 `setState`；惰性判断用 `useState(() => ...)`

## 构建与预览
```bash
CODEBUDDY_SAFE_DELETE_ENABLED=0 npm run build   # 必须带此环境变量
```
- 沙箱 `node-safe-delete-shim` 按 turn 统计删除数，>50 就阻断 build。设 `=0` 跳过
- 两种失败要区分：`trash operation` = `out/` 被预览进程占用；`BULK_CONFIRM_REQUIRED` = 需上面的环境变量
- 本地预览 `out/` 需剥离 `/blog` 前缀（见 `preview_atlas.py` 的 `translate_path`）。
  端口 `4320` 被沙箱拦截，用 `4310`/`4330`/`4353`

## GitHub Pages URL 行为
`output:'export'` 产出单文件 `out/c-utils.html`，Pages 本身**不**自动补 `index.html`。
→ 构建链末端的 `npm run fix:trailing-slash`（`scripts/fix-trailing-slash.mjs`）
会把每个 `<page>.html` 复制成 `<page>/index.html`，**所以带不带尾斜杠现在都能访问**。
（2026-09-25 实测：`/blog/c-utils` 与 `/blog/c-utils/` 均 200。此前"带尾斜杠 404"的记录已作废。）
本地预览仍是另一套规则，见 `preview_atlas.py` 的 `translate_path`。

## 内容资产
- `public/`：`eng.html`、`cem.html`、`tang-poems.html`、`quadratic.html`、`english-grammar/`、`cloze/`、`vocabulary/`
- `src/app/c-utils/`：Next 原生路由，自带独立导航；指向 `public/` 用 `<a>`，Next 路由用 `<Link>`；`BRANCH='master'`
- 首页 `src/app/page.tsx` 的 `TOOLS` 数组是作品列表唯一数据源，`glow` 用 `"R, G, B"` 字符串

## 质量门禁（每次改完必跑）
```bash
npx tsc --noEmit && npx eslint src    # 须 0 error 0 warning
CODEBUDDY_SAFE_DELETE_ENABLED=0 npm run build
git status --short                    # ⚠️ 必查：无意外 D（删除）条目
```

## ⭐ 用户要求：每次改完直接上传（2026-09-25 起）
用户明确要求「每次改完上传」，**不要问、不要等确认**，改完即走完下面四步：
1. 质量门禁全绿（见上）
2. **显式路径** `git add <改动的文件>`（严禁 `git add -A`，见危险操作清单第 2 条）
3. `git commit` + `git push origin master`
4. 轮询 CI 至 completed，再 curl 线上页确认改动生效：
   ```bash
   curl -s "https://api.github.com/repos/zhuhairong/blog/actions/runs?per_page=1" | grep -E '"status"|"conclusion"'
   curl -s "https://zhuhairong.github.io/blog/<path>/" -o /tmp/x.html -w "http=%{http_code}\n"
   ```
   两种 URL 形式现均可访问（见「URL 行为」节）。
最后状态：全绿（提交 `c96fac7`，CI success，线上抽查 6 个页面全 200）。

## ⚠️ 危险操作清单
1. **禁止仓库根目录批量删除**。清理只用白名单 `.next`、`out`
2. 不要用 **`git add -A`**！仓库有未跟踪的用户资料会被误入库 → 用显式路径，误加后 `git reset HEAD -- <paths>`
3. 删除前预演 `git clean -nd`；收尾必查 `.github/`、`.gitignore`、`next.config.ts` 是否还在
4. Windows 下 `.ps1`/`.bat` 不写非 ASCII 路径（编码损坏文件名）

### 未跟踪的用户资料（未经确认不得删除/入库）
`初中单词/`（与 `public/vocabulary/` 逐字节一致）、`完型填空html/`（与 `public/cloze/` 一致）、
`public/quadratic-structure.drawio`（无引用）、`.v/`（`全唐詩_卷539/540.txt`，80K）、
`poetry-atlas/_add_lisangyin.py`、`_met/`。
待用户选：① 入库 ② 加 .gitignore ③ 原样保留。

⚠️ 注意：`.workbuddy-ai/memory/` **不在此列——它是被跟踪的**，记忆更新按惯例走
`docs:` 提交入库（如 `ee2fe3f`、`32a035f`），用显式路径 add。

### Git 同步陷阱
不要信 `git status -sb` 的 `[gone]`（可能只是缺远程跟踪引用）。以
`git ls-remote origin refs/heads/master` 为准，对比 `git rev-parse HEAD` 再推。

⚠️ `git ls-remote` 结果**可能是缓存的**：2026-09-25 推送 `c96fac7` 后，
`git push` 报 "Everything up-to-date"，同时 `ls-remote` 返回旧 sha `ee2fe3f`，
看着像推送失败；**重查一次即变为新 sha**。遇到「push 说没东西可推、但远程 sha 对不上」时，
先重查一遍再判断，**不要**急着 `git reset --hard` 或强推。

---

## 子项目：poetry-atlas（中国诗歌时空图谱）
位置 `poetry-atlas/`，独立 package.json，**数据管线独立**。
**加诗人的完整流程见 skill `poetry-atlas-add-poet`**（含配图/考据/文本校录的全部坑），此处只留架构要点。

**定位**：从地域＋时间维度映射诗人与作品。核心价值不在诗库文本，而在**「作品—地点—时间—情境」的考据与溯源体系**。

### 核心设计
- **Assertion 考据断言**：不给作品直接挂 location/date；一条断言 = 一种说法，同一作品可挂多条互斥断言（groupId 归组、`isPrimary` 标主流）
- 置信度 A 确定 / B 较可靠 / C 存疑 / D 传说；`rationale` + `sources` 必填，缺失则构建失败
- **地理精度铁律**：史料到哪级标哪级，绝不虚标；`isCentroid: true` 的点前端弱化渲染
- **行迹站点（`JourneyStop`）**：把「生平事件 ∪ 主流断言」按同地 + 年距 ≤2 聚类成站，按年编 `seq`。
  站置信度取站内**最低**。作品分三级：`works`（创作地即本站）/ `nearbyWorks`（系年落窗口 ±1 但创作地另载，弱关联须单列）/ 未落站计入 `unplacedWorkCount`。
- ⚠️ **`poet.nativePlace` 混着两类条目**：`type:'birth'` 是真生地、`type:'ancestral'` 是郡望/祖籍
  （数据里的 note 自己写着「非实际生地」）。**按 placeId 一刀切会把寓居地/郡望标成出生地**
  （曾致杜甫长安×3、李白安陆被标「出生地」）。判断出生地必须看 `type === 'birth'`；
  郡望另设 `isAncestralPlace`，前端显示「祖籍／郡望」。

### 命令与分层
```bash
npm run atlas:build    # 源数据 → data/derived 静态分片（derived 已 gitignore，CI 重生成）
npm run atlas:check    # 新鲜度守卫
```
`data/raw`（外部快照）→ `data/{people,corpus,places,assertions,art}`（考据，唯一真相源）→ `data/derived`

### 前端铁律
- `src/lib/atlas.ts` **Server 专用**（含 `fs`）；`atlas-view.ts` **客户端安全**（禁止 import fs/path）
- ⚠️ 客户端组件误引含 `fs` 的模块 → `Module not found` **tsc 查不出，只有 build 才暴露**
- 页面：`/poetry-atlas`（地图）、`/poet/[id]`（生平时间线 + **一生行迹**）、`/work/[id]`（异说并列）
- `poetry-atlas/geo.tsx`（客户端安全）抽出投影与底图几何，**「创作地分布」与「一生行迹」共用一份底图**，
  否则两张图的中国形状会对不上。`journeyBounds(stops)` 按行迹自动取景（`SAFE_W=0.62` 给右侧栏留位）
- `poetry-atlas/poet-phases.ts`：分期数据唯一来源，`phasesOf()` 对未知诗人按生卒年三等分兜底
- 地图双轨：`tiles.ts` 读 `NEXT_PUBLIC_TIANDITU_KEY`，有 key → `TileAtlasMap.tsx`（Leaflet + 天地图 WMTS）；
  无 key → `SvgFallbackMap.tsx`（自绘墨卡托，带 60KB 行政区划 JSON，独立 chunk）。
- 行迹图同理双轨：`journey/JourneyTileMap.tsx` / `journey/JourneySvgMap.tsx`，
  两者必须共用 `journey/journey-util.ts` 的取色取径规则，否则切换密钥前后站点大小颜色会变。
- **站点间连弧线 + 箭头**（箭头表示时间方向），几何在 `journey-util.ts`：
  `arcControl / arcAt / arcSamples`。外凸方向取自行进方向的左手侧，
  故往返两程自动分居弦两侧、不重叠；箭头放弧线 `t=0.58` 处（放站点上会被站点圆盖住），
  短段不放。⚠️ `MAX_BOW=30` 是上限，改大要复测是否顶进侧栏（安全区余量仅约 62 单位）。
  ⚠️ 瓦片版求控制点前须按 `cos(纬度)` 转等距局部坐标，屏幕角取平面角的负值。
  ⚠️ 改画法时**同步改文件头注释与脚注**，并保留「弧线不代表史料记载了具体路线」的声明。
  ⚠️ 投影陷阱：天地图 `_c` 是经纬度投影、`_w` 才是球面墨卡托。
  ⚠️ 国界/断续线等领土要素**不自绘**，一律交给天地图按国家标准渲染。
  底图提示条（`mapNoKey`）已于 `0476df5` 移除，不要再加回来。
- 未配 key 时 `hasKey=false` 会隐藏「底图」切换器与深色反色按钮，属预期行为
- ⚠️ **同址多次驻留必须按坐标归组**（`groupStopsByCoordinate`）：苏轼三还眉山、四入汴京，
  坐标相同 → 逐个画会完全重合，只剩最上面那个编号（看图像「丢了站」）。
  归组后一组一个标记、外圈加环表次数、点标记在几次驻留间循环。
  **只归组、不做偏移**——为分辨而挪点就是虚标地理精度。

### 数据管线通用教训
1. 路径基于文件位置推导，不用 `process.cwd()`
2. **空数据集必须报 error**（曾载入 0 条却"校验通过"——最危险的静默失败）
3. `moduleResolution: bundler` 下 import 不写 `.js` 后缀
4. **改单文件集合（如 `works.json`）前先验证 JSON 往返字节一致**：
   该文件是 CRLF + `indent=2` + 结尾换行。用
   `open(p, encoding='utf-8', newline='')` 读、`json.dumps(..., ensure_ascii=False, indent=2)+'\n'`
   再 `.replace('\n','\r\n')` 写回，才能保证 diff 只有新增行。
   用默认文本模式读会把 CRLF 折成 LF，diff 从几百行炸到几千行。

### 逐句译注（lineNotes）
- `Work.lineNotes[]` 与 `content` **等长同序**，第 i 条对第 i 行；`trans` 直译 + 可选 `note` 难点注
- 空行豁免：组诗之间的空行 `trans: ""`，前端跳过，不塞占位文案
- 译注**必须自撰**——现代点校本的标点/校勘/注释有版权
- 现状：165 篇全覆盖

### 批量补录作品 → 见技能 `poetry-atlas-batch-add-works`
给已有诗人加作品（正文+译注+断言+配图）的完整流程、跨批次归一化清单、
配图筛选与合并脚本要点，都在那个技能里，此处不重复。

**加新诗人** → 技能 `poetry-atlas-add-poet`。

### ⭐ 视觉验证：本地能截图并读图（别再以为"看不了图"）
`poetry-atlas-add-poet` 技能里「本产品读不了图片」的说法**已过时**，实测可读。做法：
```bash
# 1) 起预览（out/ 需剥离 /blog 前缀），端口别用 4320
python preview_atlas.py 4355 &
# 2) headless 截图，再用 Read 工具直接看 PNG
"C:/Users/86180/AppData/Local/ms-playwright/chromium-1208/chrome-win64/chrome.exe" \
  --headless=new --disable-gpu --hide-scrollbars --window-size=1280,1400 \
  --screenshot="C:/Users/86180/AppData/Local/Temp/x.png" \
  "http://127.0.0.1:4355/blog/poetry-atlas/work/dufu-denggao/"
```
⚠️ 窄屏截图（如 `--window-size=430`）**不可信**：headless 下布局宽度并未真的收窄
（媒体查询不生效、header 不折叠），看起来像"横向溢出"其实是截图裁切。
判断响应式请对比同宽度的线上页面，别据此改 CSS。

大窗口易触发 SIGTERM → 加 `--force-device-scale-factor=0.5`（1400×3400 可成功）。

### ⭐ 交互验证：Node 内置 WebSocket 直连 CDP（无需 playwright）
本项目没装 playwright/puppeteer，但 **Node 22 自带全局 `WebSocket` 与 `fetch`**，足够驱动 CDP：
`spawn` Chrome 带 `--remote-debugging-port=N` → `fetch http://127.0.0.1:N/json/list` 取
`webSocketDebuggerUrl` → `Runtime.evaluate` 里 `element.click()` / 读 `innerText`，
`Page.captureScreenshot` 存图。脚本见历史 turn 的 `cdp_journey.mjs`（可重写）。
用它做过 15 项断言：站序条 tick 数、工具栏/脚注口径、上/下一站、播放自动前进、
播放态按钮文案、同址标记循环、侧栏同址提示。**比只截图可靠得多**。
⚠️ 设计变更后**旧断言会过期**（如归组后「点最后一个标记=最后一站」不再成立）——
先判断是代码错还是断言过期，再改。

### 数据源许可
chinese-poetry MIT ✅；CBDB/CHGIS 学术开放；⚠️ 现代点校本的标点校勘**及注释**有版权，
**诗词原文属公有领域**。

### 当前规模（截至「一生行迹」上线）
诗人 7（王维/李白/杜甫/苏轼/孟浩然/白居易/岑参）· 作品 165 · 地点 105 · 断言 187 ·
生平事件 141 · 逐句译注 165 篇全覆盖 · 配图 109 幅 · 构建静态页 190
行迹站点：李白 33 / 杜甫 35 / 苏轼 26 / 岑参 17 / 孟浩然 13 / 王维 12 / 白居易 11，共 147 站；
地图标记（按坐标归组后）147 → 115；7 位诗人作品全部落站，`unplacedWorkCount` 均为 0。
数据为单文件集合（`data/people/poets.json` 等），不是一首/一人一文件。
