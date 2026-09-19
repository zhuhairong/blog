'use client';

import { useEffect, useState } from 'react';
import { ArrowRight, Github } from './icons';
import styles from '@/app/page.module.css';

const WORDS = ['代码与算法', '数理与逻辑', '语言与诗词', '好奇心与热爱'];

export default function HeroIntro({ repo }: { repo: string }) {
    const [text, setText] = useState('');
    const [index, setIndex] = useState(0);
    const [deleting, setDeleting] = useState(false);

    useEffect(() => {
        const word = WORDS[index];
        const full = text === word;
        const empty = text === '';

        let delay = deleting ? 45 : 115;
        if (full && !deleting) delay = 1700;
        if (empty && deleting) delay = 300;

        const timer = window.setTimeout(() => {
            if (!deleting) {
                if (full) {
                    setDeleting(true);
                } else {
                    setText(word.slice(0, text.length + 1));
                }
            } else if (empty) {
                setDeleting(false);
                setIndex((i) => (i + 1) % WORDS.length);
            } else {
                setText(word.slice(0, text.length - 1));
            }
        }, delay);

        return () => window.clearTimeout(timer);
    }, [text, deleting, index]);

    return (
        <>
            <div className={styles.badge}>
                <span className={styles.badgeDot} aria-hidden="true" />
                <span>Personal Lab · 持续更新中</span>
            </div>

            <h1 className={styles.title}>
                <span className={styles.titleGreet}>Hi, 我是</span>
                <span className={styles.titleName}>ZHR</span>
            </h1>

            <p className={styles.lead}>
                我把日常折腾沉淀在这里 ——{' '}
                <span className={styles.typed}>
                    {text}
                    <span className={styles.caret} aria-hidden="true" />
                </span>
            </p>

            <p className={styles.desc}>
                从 149 个 C 语言工具模块，到初中英语单词闯关、化学元素游戏、唐诗三百首，
                每一件都是真实用得上、并且持续打磨的东西。
            </p>

            <div className={styles.actions}>
                <a href="#tools" className={`${styles.btn} ${styles.btnPrimary}`}>
                    探索作品
                    <ArrowRight size={18} />
                </a>
                <a
                    href={repo}
                    target="_blank"
                    rel="noreferrer"
                    className={`${styles.btn} ${styles.btnGhost}`}
                >
                    <Github size={18} />
                    GitHub
                </a>
            </div>

            <div className={styles.stats}>
                <div className={styles.stat}>
                    <span className={styles.statNum}>8</span>
                    <span className={styles.statLabel}>实用作品</span>
                </div>
                <span className={styles.statDivider} aria-hidden="true" />
                <div className={styles.stat}>
                    <span className={styles.statNum}>149</span>
                    <span className={styles.statLabel}>C 工具模块</span>
                </div>
                <span className={styles.statDivider} aria-hidden="true" />
                <div className={styles.stat}>
                    <span className={styles.statNum}>111</span>
                    <span className={styles.statLabel}>收录唐诗</span>
                </div>
            </div>
        </>
    );
}
