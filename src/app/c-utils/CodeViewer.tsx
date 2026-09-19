'use client';

import { useState, useRef, useEffect } from 'react';
import Link from 'next/link';
import { Highlight, themes } from 'prism-react-renderer';
import styles from './c-utils.module.css';

const REPO = 'https://github.com/zhuhairong/blog';
const BRANCH = 'master';

/** 学习工具的顶部导航（public 下的独立静态页，不能用 Next Link） */
const TOP_NAV = [
    { href: '/', label: '首页', external: false },
    { href: '/eng.html', label: '英语', external: true },
    { href: '/cem.html', label: '化学', external: true },
    { href: '/tang-poems.html', label: '唐诗', external: true },
    { href: '/c-utils', label: '代码库', external: false, active: true },
];

interface CodeFile {
    name: string;
    type: 'header' | 'source';
    category: string;
    description: string;
    headerContent: string;
    sourceContent: string | null;
    testContent: string | null;
    demoContent: string | null;
}

interface CodeViewerProps {
    codeFiles: CodeFile[];
    categories: string[];
}

const CATEGORY_COLORS: Record<string, string> = {
    数据结构: '#3b82f6',
    算法: '#8b5cf6',
    编码: '#ec4899',
    加密: '#ef4444',
    系统: '#f59e0b',
    网络: '#10b981',
    格式: '#06b6d4',
    文件: '#84cc16',
    数学: '#6366f1',
    工具: '#64748b',
    其他: '#94a3b8',
};

export default function CodeViewer({ codeFiles, categories }: CodeViewerProps) {
    const [selectedCategory, setSelectedCategory] = useState('全部');
    const [searchQuery, setSearchQuery] = useState('');
    const [selectedFile, setSelectedFile] = useState<CodeFile | null>(null);
    const [activeTab, setActiveTab] = useState<'header' | 'source' | 'test' | 'demo'>('header');
    const [highlightLine, setHighlightLine] = useState<number | null>(null);
    const codeRef = useRef<HTMLPreElement>(null);
    const codeViewerRef = useRef<HTMLDivElement>(null);

    const filteredFiles = codeFiles.filter((file) => {
        const matchesCategory = selectedCategory === '全部' || file.category === selectedCategory;
        const q = searchQuery.toLowerCase();
        const matchesSearch =
            file.name.toLowerCase().includes(q) || file.description.toLowerCase().includes(q);
        return matchesCategory && matchesSearch;
    });

    useEffect(() => {
        if (highlightLine !== null && codeRef.current) {
            const el = codeRef.current.querySelector(`[data-line="${highlightLine}"]`);
            if (el) el.scrollIntoView({ behavior: 'smooth', block: 'center' });
        }
    }, [highlightLine]);

    const handleFileSelect = (file: CodeFile) => {
        setSelectedFile(file);
        setActiveTab('header');
        setHighlightLine(null);
        setTimeout(() => {
            codeViewerRef.current?.scrollIntoView({ behavior: 'smooth', block: 'start' });
        }, 100);
    };

    const getCurrentContent = () => {
        if (!selectedFile) return '';
        switch (activeTab) {
            case 'header':
                return selectedFile.headerContent;
            case 'source':
                return selectedFile.sourceContent || '';
            case 'test':
                return selectedFile.testContent || '';
            case 'demo':
                return selectedFile.demoContent || '';
            default:
                return '';
        }
    };

    const handleFunctionClick = (funcName: string) => {
        const content = getCurrentContent();
        if (!content) return;

        const lines = content.split('\n');
        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            if (line.includes(funcName) && (line.includes('(') || line.includes('typedef'))) {
                setHighlightLine(i + 1);
                break;
            }
        }
    };

    const extractFunctions = (content: string): string[] => {
        const functions: string[] = [];
        for (const line of content.split('\n')) {
            const funcMatch = line.match(
                /^\s*(?:static\s+)?(?:inline\s+)?[\w_]+\s+\*?\s*([\w_]+)\s*\(/
            );
            if (funcMatch && !funcMatch[1].startsWith('_')) {
                functions.push(funcMatch[1]);
            }
            const typedefMatch = line.match(/typedef\s+struct\s+([\w_]+)/);
            if (typedefMatch) functions.push(typedefMatch[1]);
        }
        return [...new Set(functions)].slice(0, 10);
    };

    const tabs = selectedFile
        ? [
              { key: 'header' as const, label: `📋 ${selectedFile.name}.h`, show: true, cls: '' },
              {
                  key: 'source' as const,
                  label: `⚙️ ${selectedFile.name}.c`,
                  show: !!selectedFile.sourceContent,
                  cls: '',
              },
              {
                  key: 'test' as const,
                  label: `🧪 test_${selectedFile.name}.c`,
                  show: !!selectedFile.testContent,
                  cls: styles.tabTest,
              },
              {
                  key: 'demo' as const,
                  label: `🎬 demo_${selectedFile.name}.c`,
                  show: !!selectedFile.demoContent,
                  cls: styles.tabDemo,
              },
          ].filter((t) => t.show)
        : [];

    return (
        <div className={styles.page}>
            <div className={styles.topbar}>
                <div className={styles.topbarInner}>
                    <Link href="/" className={styles.brand}>
                        <span className={styles.brandMark} aria-hidden="true">
                            📦
                        </span>
                        <span className={styles.brandText}>
                            <span className={styles.brandName}>ZHR</span>
                            <span className={styles.brandSub}>C-Utils</span>
                        </span>
                    </Link>

                    <nav className={styles.topNav} aria-label="导航">
                        {TOP_NAV.map((item) => {
                            const cls = `${styles.topNavLink}${
                                'active' in item && item.active ? ` ${styles.topNavLinkActive}` : ''
                            }`;
                            return item.external ? (
                                <a key={item.href} href={item.href} className={cls}>
                                    {item.label}
                                </a>
                            ) : (
                                <Link key={item.href} href={item.href} className={cls}>
                                    {item.label}
                                </Link>
                            );
                        })}
                    </nav>
                </div>
            </div>

            <div className={styles.head}>
                <h1 className={styles.headTitle}>📦 C-Utils 代码库</h1>
                <p className={styles.headDesc}>
                    149 个轻量级、模块化的 C 语言工具库，涵盖数据结构、算法、系统编程、网络协议、加密编码等领域
                </p>
            </div>

            <div className={styles.controls}>
                <input
                    type="text"
                    className={styles.search}
                    placeholder="搜索模块名称或描述..."
                    value={searchQuery}
                    onChange={(e) => setSearchQuery(e.target.value)}
                    aria-label="搜索模块"
                />
                <select
                    className={styles.select}
                    value={selectedCategory}
                    onChange={(e) => setSelectedCategory(e.target.value)}
                    aria-label="按分类筛选"
                >
                    <option value="全部">全部分类</option>
                    {categories.map((cat) => (
                        <option key={cat} value={cat}>
                            {cat}
                        </option>
                    ))}
                </select>
            </div>

            <div className={styles.grid}>
                {filteredFiles.map((file) => {
                    const active = selectedFile?.name === file.name;
                    return (
                        <button
                            key={file.name}
                            type="button"
                            onClick={() => handleFileSelect(file)}
                            className={`${styles.moduleCard}${active ? ` ${styles.moduleCardActive}` : ''}`}
                        >
                            <span
                                className={styles.moduleBadge}
                                style={{ background: CATEGORY_COLORS[file.category] || '#64748b' }}
                            >
                                {file.category}
                            </span>
                            <span className={styles.moduleName}>{file.name}</span>
                        </button>
                    );
                })}
            </div>

            {selectedFile && (
                <div ref={codeViewerRef} className={styles.viewer}>
                    <div className={styles.viewerHead}>
                        <h3 className={styles.viewerTitle}>
                            📄 {selectedFile.name} — {selectedFile.description}
                        </h3>
                        <div className={styles.viewerActions}>
                            {selectedFile.testContent && (
                                <a
                                    href={`${REPO}/blob/${BRANCH}/c_utils/tests/test_${selectedFile.name}.c`}
                                    target="_blank"
                                    rel="noreferrer"
                                    className={styles.btnGit}
                                >
                                    GitHub 测试文件
                                </a>
                            )}
                            <button
                                type="button"
                                className={styles.btnClose}
                                onClick={() => {
                                    setSelectedFile(null);
                                    setHighlightLine(null);
                                }}
                            >
                                关闭
                            </button>
                        </div>
                    </div>

                    <div className={styles.tabs}>
                        {tabs.map((tab) => (
                            <button
                                key={tab.key}
                                type="button"
                                onClick={() => {
                                    setActiveTab(tab.key);
                                    setHighlightLine(null);
                                }}
                                className={`${styles.tab}${tab.cls ? ` ${tab.cls}` : ''}${
                                    activeTab === tab.key ? ` ${styles.tabActive}` : ''
                                }`}
                            >
                                {tab.label}
                            </button>
                        ))}
                    </div>

                    {selectedFile.headerContent && (
                        <div className={styles.jump}>
                            <div className={styles.jumpLabel}>快速跳转</div>
                            <div className={styles.jumpList}>
                                {extractFunctions(selectedFile.headerContent).map((func) => (
                                    <button
                                        key={func}
                                        type="button"
                                        className={styles.jumpChip}
                                        onClick={() => handleFunctionClick(func)}
                                    >
                                        {func}
                                    </button>
                                ))}
                            </div>
                        </div>
                    )}

                    <Highlight theme={themes.vsDark} code={getCurrentContent()} language="c">
                        {({ className, style, tokens, getLineProps, getTokenProps }) => (
                            <pre ref={codeRef} className={`${className} ${styles.code}`} style={style}>
                                {tokens.map((line, i) => (
                                    <div
                                        key={i}
                                        {...getLineProps({ line })}
                                        data-line={i + 1}
                                        className={`${styles.codeLine}${
                                            highlightLine === i + 1
                                                ? ` ${styles.codeLineHighlight}`
                                                : ''
                                        }`}
                                    >
                                        <span className={styles.lineNo}>{i + 1}</span>
                                        <span className={styles.lineCode}>
                                            {line.map((token, key) => (
                                                <span key={key} {...getTokenProps({ token })} />
                                            ))}
                                        </span>
                                    </div>
                                ))}
                            </pre>
                        )}
                    </Highlight>
                </div>
            )}

            <div className={styles.foot}>
                共 {codeFiles.length} 个模块 · {filteredFiles.length} 个匹配
            </div>
        </div>
    );
}
