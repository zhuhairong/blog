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
    /**
     * 特色卡的古画背景。
     *
     * 由服务端（page.tsx）从 poetry-atlas 派生数据里取出后传入 ——
     * ToolCard 是客户端组件，不能直接读文件系统。
     * 只传路径不传整幅画的元数据，因为卡片上已经有标题与描述，
     * 再多一行署名会把版面压垮；署名在作品页与图谱页各自完整交代。
     */
    art?: { src: string; title: string; artist: string } | null;
};

export default function ToolCard({ tool, featured = false, art }: Props) {
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
                {art && (
                    <img
                        className={styles.cardArt}
                        src={art.src}
                        alt=""
                        aria-hidden="true"
                        loading="lazy"
                        decoding="async"
                        title={art.artist ? `${art.artist} ${art.title}` : art.title}
                    />
                )}
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
