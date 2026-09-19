'use client';

import Link from 'next/link';
import { useRef, type CSSProperties, type MouseEvent } from 'react';
import { ArrowUpRight } from './icons';
import styles from '@/app/page.module.css';

export type Tool = {
    href: string;
    title: string;
    desc: string;
    icon: string;
    tags: string[];
    from: string;
    to: string;
    glow: string;
};

type Props = {
    tool: Tool;
    featured?: boolean;
};

export default function ToolCard({ tool, featured = false }: Props) {
    const ref = useRef<HTMLAnchorElement | null>(null);

    const handleMove = (e: MouseEvent<HTMLAnchorElement>) => {
        const el = ref.current;
        if (!el) return;
        const rect = el.getBoundingClientRect();
        el.style.setProperty('--mx', `${e.clientX - rect.left}px`);
        el.style.setProperty('--my', `${e.clientY - rect.top}px`);
    };

    const vars = {
        '--glow': tool.glow,
        '--from': tool.from,
        '--to': tool.to,
    } as CSSProperties;

    if (featured) {
        return (
            <Link ref={ref} href={tool.href} onMouseMove={handleMove} className={styles.card} style={vars}>
                <div className={styles.cardBody}>
                    <div className={styles.featuredTop}>
                        <span className={styles.cardIcon} aria-hidden="true">
                            {tool.icon}
                        </span>
                        <div className={styles.featuredMain}>
                            <h3 className={styles.cardTitle}>{tool.title}</h3>
                            <p className={styles.cardDesc}>{tool.desc}</p>
                            <div className={styles.cardTags}>
                                {tool.tags.map((t) => (
                                    <span key={t} className={styles.tag}>
                                        {t}
                                    </span>
                                ))}
                            </div>
                        </div>
                    </div>
                </div>
                <ArrowUpRight size={24} className={styles.cardArrow} />
            </Link>
        );
    }

    return (
        <Link ref={ref} href={tool.href} onMouseMove={handleMove} className={styles.card} style={vars}>
            <div className={styles.cardBody}>
                <div className={styles.cardTop}>
                    <span className={styles.cardIcon} aria-hidden="true">
                        {tool.icon}
                    </span>
                    <ArrowUpRight size={20} className={styles.cardArrow} />
                </div>
                <h3 className={styles.cardTitle}>{tool.title}</h3>
                <p className={styles.cardDesc}>{tool.desc}</p>
                <div className={styles.cardTags}>
                    {tool.tags.map((t) => (
                        <span key={t} className={styles.tag}>
                            {t}
                        </span>
                    ))}
                </div>
            </div>
        </Link>
    );
}
