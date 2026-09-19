import { getSortedPostsData } from '@/lib/api';
import { getFeaturedArt } from '@/lib/atlas';
import Header from '@/components/Header';
import Footer from '@/components/Footer';
import HeroBackground from '@/components/HeroBackground';
import HeroIntro from '@/components/HeroIntro';
import Reveal from '@/components/Reveal';
import ToolCard, { type Tool } from '@/components/ToolCard';
import PostCard from '@/components/PostCard';
import { Plus } from '@/components/icons';
import styles from './page.module.css';

const REPO = 'https://github.com/zhuhairong/blog';

const TOOLS: Tool[] = [
    {
        href: '/poetry-atlas',
        title: '中国诗歌时空图谱',
        desc: '从地域与时间两个维度梳理中国诗人与作品。每篇作品的创作地、系年与情境均标注考据依据、地理精度与置信度，学术争议完整保留、诸说并列。',
        icon: '🗺️',
        tags: ['时空地图', '考据溯源', '异说并存'],
        from: '#7c5cff',
        to: '#f472b6',
        glow: '124, 92, 255',
    },
    {
        href: '/c-utils',
        title: 'C-Utils 代码库',
        desc: '149 个轻量级 C 语言工具库，涵盖数据结构、算法、系统编程、网络协议、加密编码等领域。模块化设计，即插即用。',
        icon: '📦',
        tags: ['149 模块', '源码浏览', '分类索引'],
        from: '#8b5cf6',
        to: '#6366f1',
        glow: '139, 92, 246',
    },
    {
        href: '/eng.html',
        title: '英语单词大冒险',
        desc: '人教版八年级下册英语单词学习游戏，包含 8 个单元完整词库。支持单词卡片、选择题、拼写练习等多种学习模式。',
        icon: '📚',
        tags: ['单词卡片', '拼写练习', '成就系统'],
        from: '#6366f1',
        to: '#a855f7',
        glow: '99, 102, 241',
    },
    {
        href: '/cem.html',
        title: '化学元素闯关',
        desc: '初中化学知识点闯关游戏，涵盖 1-36 号元素、离子符号、化合价、化学式等 8 大关卡。随机出题，举一反三。',
        icon: '🧪',
        tags: ['8 大关卡', '随机出题', '知识点仓库'],
        from: '#10b981',
        to: '#059669',
        glow: '16, 185, 129',
    },
    {
        href: '/tang-poems.html',
        title: '唐诗三百首 · 墨韵',
        desc: '精选 111 首经典唐诗，涵盖李白、杜甫、王维、白居易等 35 位名家名作。支持搜索分类，水墨风格沉浸式阅读。',
        icon: '📜',
        tags: ['111 首经典', '分类浏览', '水墨风格'],
        from: '#f59e0b',
        to: '#d97706',
        glow: '245, 158, 11',
    },
    {
        href: '/quadratic.html',
        title: '一元二次方程详解',
        desc: '系统讲解一元二次方程的五种解法：直接开平方法、配方法、公式法、因式分解法、十字交叉法，含可视化图解。',
        icon: '📐',
        tags: ['五种解法', '可视化图解', '交互练习'],
        from: '#0ea5e9',
        to: '#3b82f6',
        glow: '14, 165, 233',
    },
    {
        href: '/english-grammar/index.html',
        title: '初中英语语法闯关',
        desc: '初中英语语法学习工具，涵盖 7 大模块：现在完成时、过去进行时、被动语态、情态动词、不定式与动名词等。',
        icon: '📖',
        tags: ['7 大模块', '闯关学习', '成就系统'],
        from: '#14b8a6',
        to: '#0d9488',
        glow: '20, 184, 166',
    },
    {
        href: '/cloze/index.html',
        title: '英语完型填空训练',
        desc: '中考英语核心词汇完型填空专项训练。涵盖词汇记忆、语境理解、语法搭配等多维度练习，按日计划循序渐进。',
        icon: '✍️',
        tags: ['完型填空', '中考词汇', '每日计划'],
        from: '#f97316',
        to: '#ea580c',
        glow: '249, 115, 22',
    },
    {
        href: '/vocabulary/index.html',
        title: '初中英语单词记忆',
        desc: '初中英语全年级单词学习工具，涵盖七年级到九年级上下册核心词汇。支持英汉互译、拼写练习、易混淆词辨析。',
        icon: '🔤',
        tags: ['全年级词库', '英汉互译', '易混淆词'],
        from: '#38bdf8',
        to: '#0284c7',
        glow: '56, 189, 248',
    },
];

const FEATURED = TOOLS[0];
const REST = TOOLS.slice(1);

export default function Home() {
    const posts = getSortedPostsData();
    // 特色卡的古画背景：与图谱主页主视觉同一幅，形成「进门—进屋」的一致感。
    // 数据可能尚未构建（派生层缺失），此时返回 null，卡片退化为纯渐变，不会裂图。
    const heroArt = getFeaturedArt('atlas-hero');

    return (
        <>
            <Header />

            <section className={styles.hero}>
                <HeroBackground />
                <div className={styles.heroGrid} aria-hidden="true" />
                <span className={`${styles.orb} ${styles.orb1}`} aria-hidden="true" />
                <span className={`${styles.orb} ${styles.orb2}`} aria-hidden="true" />
                <span className={`${styles.orb} ${styles.orb3}`} aria-hidden="true" />

                <div className="container">
                    <div className={styles.heroContent}>
                        <HeroIntro repo={REPO} />
                    </div>
                </div>
            </section>

            <div className="container">
                <section id="tools" className={styles.section}>
                    <Reveal>
                        <div className={styles.sectionHead}>
                            <div>
                                <p className={styles.sectionEyebrow}>Featured</p>
                                <h2 className={styles.sectionTitle}>精选作品</h2>
                            </div>
                        </div>
                    </Reveal>

                    <Reveal delay={60}>
                        <div className={styles.featured}>
                            <ToolCard
                                tool={FEATURED}
                                featured
                                art={
                                    heroArt
                                        ? {
                                              src: heroArt.src,
                                              title: heroArt.artTitle,
                                              artist: heroArt.artArtist,
                                          }
                                        : null
                                }
                            />
                        </div>
                    </Reveal>

                    <div className={styles.toolGrid}>
                        {REST.map((tool, i) => (
                            <Reveal key={tool.href} delay={50 * (i % 3)}>
                                <ToolCard tool={tool} />
                            </Reveal>
                        ))}
                    </div>
                </section>

                <section id="writing" className={styles.section}>
                    <Reveal>
                        <div className={styles.sectionHead}>
                            <div>
                                <p className={styles.sectionEyebrow}>Writings</p>
                                <h2 className={styles.sectionTitle}>最新文章</h2>
                            </div>
                            <a
                                href={`${REPO}/new/master/posts`}
                                target="_blank"
                                rel="noreferrer"
                                className={styles.newPost}
                            >
                                <Plus size={15} />
                                写新文章
                            </a>
                        </div>
                    </Reveal>

                    {posts.length === 0 ? (
                        <Reveal delay={60}>
                            <div className={styles.empty}>
                                <span className={styles.emptyIcon} aria-hidden="true">
                                    📝
                                </span>
                                <p className={styles.emptyText}>还没有文章，去 GitHub 写下第一篇吧。</p>
                            </div>
                        </Reveal>
                    ) : (
                        <div className={styles.postGrid}>
                            {posts.map((post, i) => (
                                <Reveal key={post.id} delay={50 * (i % 3)}>
                                    <PostCard post={post} />
                                </Reveal>
                            ))}
                        </div>
                    )}
                </section>

                <section className={styles.section}>
                    <Reveal>
                        <div className={styles.cta}>
                            <div className={styles.ctaContent}>
                                <h2 className={styles.ctaTitle}>代码、学习、写作，都在这里</h2>
                                <p className={styles.ctaDesc}>
                                    所有项目都开源在 GitHub，欢迎 star、提 issue，或者直接 fork 走改造。
                                </p>
                                <div className={styles.ctaActions}>
                                    <a
                                        href={REPO}
                                        target="_blank"
                                        rel="noreferrer"
                                        className={`${styles.btn} ${styles.btnPrimary}`}
                                    >
                                        <svg
                                            viewBox="0 0 16 16"
                                            width="18"
                                            height="18"
                                            fill="currentColor"
                                            aria-hidden="true"
                                        >
                                            <path d="M8 0C3.58 0 0 3.58 0 8c0 3.54 2.29 6.53 5.47 7.59.4.07.55-.17.55-.38 0-.19-.01-.82-.01-1.49-2.01.37-2.53-.49-2.69-.94-.09-.23-.48-.94-.82-1.13-.28-.15-.68-.52-.01-.53.63-.01 1.08.58 1.23.82.72 1.21 1.87.87 2.33.66.07-.52.28-.87.51-1.07-1.78-.2-3.64-.89-3.64-3.95 0-.87.31-1.59.82-2.15-.08-.2-.36-1.02.08-2.12 0 0 .67-.21 2.2.82.64-.18 1.32-.27 2-.27s1.36.09 2 .27c1.53-1.04 2.2-.82 2.2-.82.44 1.1.16 1.92.08 2.12.51.56.82 1.27.82 2.15 0 3.07-1.87 3.75-3.65 3.95.29.25.54.73.54 1.48 0 1.07-.01 1.93-.01 2.2 0 .21.15.46.55.38A8.01 8.01 0 0 0 16 8c0-4.42-3.58-8-8-8Z" />
                                        </svg>
                                        访问 GitHub 仓库
                                    </a>
                                    <a href="/c-utils" className={`${styles.btn} ${styles.btnGhost}`}>
                                        浏览 C-Utils 源码
                                    </a>
                                </div>
                            </div>
                        </div>
                    </Reveal>
                </section>
            </div>

            <Footer />
        </>
    );
}
