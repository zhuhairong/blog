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

## GitHub Pages URL 行为（勿误判为 bug）
`output:'export'` 产出单文件 `out/c-utils.html`，Pages 不自动补 `index.html`。
→ **全站链接统一不带尾斜杠**。`/blog/c-utils` 200、`/blog/c-utils/` 404（与本地 http.server **相反**）。

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
最近状态：全绿（提交 `26f208a`）。

## ⚠️ 危险操作清单
1. **禁止仓库根目录批量删除**。清理只用白名单 `.next`、`out`
2. 不要用 **`git add -A`**！仓库有未跟踪的用户资料会被误入库 → 用显式路径，误加后 `git reset HEAD -- <paths>`
3. 删除前预演 `git clean -nd`；收尾必查 `.github/`、`.gitignore`、`next.config.ts` 是否还在
4. Windows 下 `.ps1`/`.bat` 不写非 ASCII 路径（编码损坏文件名）

### 未跟踪的用户资料（未经确认不得删除/入库）
`初中单词/`（与 `public/vocabulary/` 逐字节一致）、`完型填空html/`（与 `public/cloze/` 一致）、
`public/quadratic-structure.drawio`（无引用）。待用户选：① 入库 ② 加 .gitignore ③ 原样保留。

### Git 同步陷阱
不要信 `git status -sb` 的 `[gone]`（可能只是缺远程跟踪引用）。以
`git ls-remote origin refs/heads/master` 为准，对比 `git rev-parse HEAD` 再推。

---

## 子项目：poetry-atlas（中国诗歌时空图谱）
位置 `poetry-atlas/`，独立 package.json，**数据管线独立**。
**加诗人的完整流程见 skill `poetry-atlas-add-poet`**（含配图/考据/文本校录的全部坑），此处只留架构要点。

**定位**：从地域＋时间维度映射诗人与作品。核心价值不在诗库文本，而在**「作品—地点—时间—情境」的考据与溯源体系**。

### 核心设计
- **Assertion 考据断言**：不给作品直接挂 location/date；一条断言 = 一种说法，同一作品可挂多条互斥断言（groupId 归组、`isPrimary` 标主流）
- 置信度 A 确定 / B 较可靠 / C 存疑 / D 传说；`rationale` + `sources` 必填，缺失则构建失败
- **地理精度铁律**：史料到哪级标哪级，绝不虚标；`isCentroid: true` 的点前端弱化渲染

### 命令与分层
```bash
npm run atlas:build    # 源数据 → data/derived 静态分片（derived 已 gitignore，CI 重生成）
npm run atlas:check    # 新鲜度守卫
```
`data/raw`（外部快照）→ `data/{people,corpus,places,assertions,art}`（考据，唯一真相源）→ `data/derived`

### 前端铁律
- `src/lib/atlas.ts` **Server 专用**（含 `fs`）；`atlas-view.ts` **客户端安全**（禁止 import fs/path）
- ⚠️ 客户端组件误引含 `fs` 的模块 → `Module not found` **tsc 查不出，只有 build 才暴露**
- 页面：`/poetry-atlas`（SVG 自绘地图）、`/poet/[id]`（生平时间线）、`/work/[id]`（异说并列）
- 地图：原型用自绘 SVG 等距圆柱投影（零依赖、无合规风险）；正式版若需底图须先解决测绘资质

### 数据管线通用教训
1. 路径基于文件位置推导，不用 `process.cwd()`
2. **空数据集必须报 error**（曾载入 0 条却"校验通过"——最危险的静默失败）
3. `moduleResolution: bundler` 下 import 不写 `.js` 后缀

### 数据源许可
chinese-poetry MIT ✅；CBDB/CHGIS 学术开放；⚠️ 现代点校本的标点校勘有版权，**诗词原文属公有领域**。

### 当前规模（提交 26f208a）
诗人 6（王维/李白/杜甫/苏轼/孟浩然/白居易）· 作品 100 · 地点 90 · 断言 110 · 静态页 113
