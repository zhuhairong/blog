# 项目长期记忆 — ZHR's AI Blog

## 项目概览
个人博客 + 学习工具集合站。Next.js 16（App Router、Turbopack）+ TypeScript，**静态导出**后由 GitHub Pages 托管。

## 关键配置
- `next.config.ts`：`output: 'export'` + `basePath: '/blog'`，`images.unoptimized: true`。
  - **含义**：站内跳转到 `public/` 下的静态 HTML 必须带 `basePath` 前缀会由 Next 自动处理；但**图片与静态资源的运行时路径需注意 `/blog` 前缀**。构建产物在 `out/`。
- 本地预览导出结果：`out/` 目录本身是站点根目录，但 `basePath` 使实际路由在 `/blog/` 下。
  - 直接 `python -m http.server` 打开 `out/` 时，根路径 `/` 是 200（index.html 在 out 根），但 Next 生成的资源引用是 `/blog/_next/...`，需从 `out/` 的父级或做路径映射才能完全正确加载。
- 仓库默认分支：**`master`**（不是 main）。
  - `.github/workflows/nextjs.yml` 监听 `master` → 实际生效的唯一部署流程。
  - ~~`deploy.yml`（监听 main）~~ 已于 2026-09-19 删除（提交 `784cf21`），它与 nextjs.yml 仅差分支名，从未触发。
- 部署地址：https://zhuhairong.github.io/blog/
- 远程仓库：`git@github.com:zhuhairong/blog.git`
- 环境无 `gh` CLI，查询 Actions 状态用 GitHub REST API：
  `curl -s "https://api.github.com/repos/zhuhairong/blog/actions/runs?per_page=1"`

## 技术约定 / 踩坑

1. **绝对禁止使用 `styled-jsx`**。
   - Server Component 里用它 → 构建失败（`Invalid import 'client-only'`）。
   - 即使在 Client Component 里用它，样式也会被注入到独立 chunk，**极易被 dev 阶段残留的旧 CSS chunk 覆盖**，导致线上样式错乱（曾引发全站"巨型箭头"事故）。
   - → 约定：**页面专属样式放 CSS Module，跨页面共享样式放 `globals.css`。全站零 styled-jsx。**
2. 样式架构（**每页一个 CSS Module，互不干扰**）：
   - `src/app/page.module.css` — 首页（`styles.xxx`）
   - `src/app/posts/[id]/post.module.css` — 文章详情页
   - `src/app/c-utils/c-utils.module.css` — C-Utils 代码库
   - `src/app/globals.css` — 设计令牌、全局重置、`.site-header` / `.site-footer` / `.reveal` / `.container`
   - 构建后正常情况下有 **4 个 CSS chunk**（3 个页面 module + 1 个字体 `@font-face`），每页只加载自己那份。**不要仅凭数量判断残留**，要看内容与引用关系。
   - 内联 `style` 仅允许用于**运行时才确定的值**（如动态分类色标、Prism token 色）。布局样式一律进 CSS Module。
3. **内联 SVG 必须显式声明 `width` / `height`**，否则 SVG 会撑满容器。
   → 统一走 `src/components/icons.tsx`，所有图标均带 `size` 参数。改图标务必保留尺寸属性。
4. 设计令牌（`globals.css` 的 `:root`）：
   - 背景：`--bg-primary: #08080d`、`--bg-surface: #0e0e16`、`--bg-elevated: #14141f`
   - 品牌：`--brand: #7c5cff`、`--brand-light: #a78bfa`、`--pink: #f472b6`、`--cyan: #38bdf8`、`--brand-rgb: 124, 92, 255`
   - 文字：`--text-primary: #f2f2f7`、`--text-secondary: #9d9db4`、`--text-tertiary: #61617a`
   - 其余：`--border`、`--border-strong`、`--border-brand`、`--r-sm|md|lg|xl|full`、`--shadow-*`、`--container: 1120px`
   - **整体固定深色主题**（不跟随系统亮色）。
   - 旧变量名 `--accent-primary` / `--border-color` 已废弃，勿再使用。
5. 动效约定：所有动画需在 `@media (prefers-reduced-motion: reduce)` 下降级。
6. React 约定：不要在 `useEffect` 体内同步调用 `setState`（`react-hooks/set-state-in-effect` 会报 error）。需惰性判断环境时用 `useState(() => typeof window !== 'undefined' && ...)`。

## 常见问题排查

**线上样式错乱 / 元素尺寸失控** → 优先怀疑构建残留：
1. 停掉所有持锁 `out/` 的预览进程
2. 删 `out/` 与 `.next/`（Windows 下若被占用，用 Python `shutil.rmtree`）
3. `npm run build` 后，**逐个检查** `out/_next/static/chunks/*.css` 内容（而非只看数量）：
   - 正常应为 4 个：3 个页面 module + 1 个字体
   - 关键判断：某 CSS 是否被页面引用、是否含**已废弃的旧类名**（如 `tool-card` / `--accent-primary`）

**本地预览 `basePath: '/blog'`** → 直接用 `python -m http.server` 打开 `out/` 会 404，因为资源路径带 `/blog` 前缀。
需在 server 里剥离前缀：
```python
def translate_path(self, path):
    if path.startswith('/blog'): path = path[5:] or '/'
    return super().translate_path(path)
```
- 本地行为：`/c-utils` → 301，`/c-utils/` → 200
- 线上行为：`/c-utils` → 200，`/c-utils/` → 404（**相反**，见下方 URL 行为表）
- 另：端口 `4320` 被沙箱拦截（`WinError 10013`），用 `4310` / `4330` 正常

## 内容资产
- `src/app/c-utils/CodeViewer.tsx` 为 Next 原生路由（C 源码浏览器），**自带独立顶部导航**（品牌 + 首页/英语/化学/唐诗/代码库），不使用全局 Header/Footer。
  - 其中指向 `public/` 静态页（`/eng.html` 等）的链接必须用原生 `<a>`；指向 Next 路由的用 `<Link>`。
  - `BRANCH = 'master'` 常量用于拼 GitHub 链接（曾误写 main 导致 404）。
- `public/` 下为已编译的独立学习工具（纯 HTML/JS，非 Next 路由）：`eng.html`、`cem.html`、`tang-poems.html`、`quadratic.html`、`english-grammar/`、`cloze/`、`vocabulary/`。
- `src/app/posts/[id]/` 文章详情页：自带 `post.module.css` + Header + Footer。
- `posts/` 存放 Markdown 文章（gray-matter + remark 解析）。
- 首页 `src/app/page.tsx` 中的 `TOOLS` 数组是作品列表的**唯一数据源**，新增工具需在此登记（href / title / desc / icon / tags / from / to / glow）。`glow` 用 `"R, G, B"` 字符串格式以支持 `rgba(var(--glow), a)`。

## GitHub Pages URL 行为（勿误判为 bug）
| URL | 状态 | 说明 |
|---|---|---|
| `/blog/` | 200 | 首页 |
| `/blog/c-utils` | **200** | canonical，Next 生成的形式 |
| `/blog/c-utils/` | 404 | 单文件 `out/c-utils.html`，无目录 |
| `/blog/posts/hello-world` | 200 | 同上 |
| `/blog/posts/` | 404 | 无索引页 |
| `/blog/eng.html` | 200 | public 静态文件 |
| `/blog/eng.html/` | 404 | 加了斜杠就必 404，与是否"目录"无关 |
| `/blog/english-grammar` | 301 → `/blog/english-grammar/index.html` | 真实目录才有补斜杠重定向 |

**根因**：`output: 'export'` 产出 `out/c-utils.html` 这类单文件；GitHub Pages 不像 `python http.server` 那样自动补 `index.html`。
与本地 `python -m http.server` **行为相反**（本地加斜杠才 200）。所以 `Link href="/c-utils"` 是正确写法，**全站链接统一不带尾斜杠**。

## 未跟踪文件（用户资料，未经确认不得删除）
- `初中单词/`（2.0M / 10 文件）— `diff -rq` 验证与 `public/vocabulary/` **逐字节一致**，是发布产物的源工作副本
- `完型填空html/`（1.2M / 22 文件）— `index.html` 与 `public/cloze/index.html` 完全一致
- `public/quadratic-structure.drawio`（24K）— 全站 grep **无引用**，drawio 源图

处理选项：① 纳入版本管理 ② 加 `.gitignore` ③ 原样保留。**必须由用户明确选择后才可操作。**

## 质量门禁（每次改完必跑）
```bash
npx tsc --noEmit    # 类型：须无输出
npx eslint src      # lint：须 0 error 0 warning
npm run build       # 构建：须成功
git status --short  # ⚠️ 必查：确认没有意外的 D（删除）条目
```
当前状态：三项全绿。

## ⚠️ 危险操作清单（务必遵守）

1. **不要对仓库根目录做批量删除**。清理构建产物只用白名单：`.next`、`out`。
   - 曾发生 `.github/workflows/` 被误删的事件（远程仓库未受影响，`git checkout -- .github/` 即可恢复）。
2. 删除前先预演：`git clean -nd`（不实际删除，仅列出）。
3. **收尾前必须 `git status --short` 检查删除条目**，重点盯 `.github/`、`.gitignore`、`next.config.ts`、`tsconfig.json` 等不被构建产物覆盖的文件。
4. 删除 `out/` 若被预览进程占用而失败（`[safe-delete] 操作失败`），先停进程，再用 Python `shutil.rmtree`。
5. Windows 下 `.ps1` / `.bat` 脚本不要写非 ASCII 路径（编码会损坏文件名），改用直接命令。

## Git 同步陷阱

**不要相信 `git status -sb` 的 `[gone]`**。曾出现 `## master...origin/master [gone]`，看着像"远端分支被删"，
实际只是本地缺少远程跟踪引用。**以 `git ls-remote origin refs/heads/master` 为准**：
```bash
git ls-remote origin refs/heads/master   # 真实远端 HEAD
git rev-parse HEAD                       # 本地 HEAD
git log <remote-sha>..HEAD --stat        # 看差了什么（确认不含代码再推）
```
