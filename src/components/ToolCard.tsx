'use client';

import Link from 'next/link';
import { useRef, type MouseEvent } from 'react';

export type Tool = {
    href: string;
    title: string;
    desc: string;
    icon: string;
    tags: string[];
    /** 图标渐变主色 */
    from: string;
    to: string;
    /** 卡片辉光色 */
    glow: string;
    /** 分类 */
    category: 'code' | 'study';
};

export default function ToolCard({ tool, featured = false }: { tool: Tool; featured?: boolean }) {
    const ref = useRef<HTMLAnchorElement | null>(null);

    const handleMove = (e: MouseEvent<HTMLAnchorElement>) => {
        const el = ref.current;
        if (!el) return;
        const rect = el.getBoundingClientRect();
        el.style.setProperty('--mx', `${e.clientX - rect.left}px`);
        el.style.setProperty('--my', `${e.clientY - rect.top}px`);
    };

    return (
        <Link
            ref={ref}
            href={tool.href}
            onMouseMove={handleMove}
            className={`tool-card-modern${featured ? ' is-featured' : ''}`}
            style={
                {
                    '--tool-from': tool.from,
                    '--tool-to': tool.to,
                    '--tool-glow': tool.glow,
                } as React.CSSProperties
            }
        >
            <span className="tool-card-border" aria-hidden="true" />
            <span className="tool-card-spotlight" aria-hidden="true" />

            <div className="tool-card-inner">
                <div className="tool-card-head">
                    <div className="tool-card-icon">
                        <span aria-hidden="true">{tool.icon}</span>
                    </div>
                    <svg className="tool-card-arrow" viewBox="0 0 20 20" fill="none" aria-hidden="true">
                        <path
                            d="M6 14L14 6M14 6H8M14 6v6"
                            stroke="currentColor"
                            strokeWidth="1.8"
                            strokeLinecap="round"
                            strokeLinejoin="round"
                        />
                    </svg>
                </div>

                <h3 className="tool-card-title">{tool.title}</h3>
                <p className="tool-card-desc">{tool.desc}</p>

                <div className="tool-card-tags">
                    {tool.tags.map((tag) => (
                        <span key={tag} className="tool-tag">
                            {tag}
                        </span>
                    ))}
                </div>
            </div>
        </Link>
    );
}
