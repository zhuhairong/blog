'use client';

import { useEffect, useRef, useState } from 'react';

const WORDS = ['代码与算法', '数理与逻辑', '语言与诗词', '好奇心与热爱'];

export default function HeroIntro() {
    const [text, setText] = useState('');
    const [wordIndex, setWordIndex] = useState(0);
    const [deleting, setDeleting] = useState(false);

    const [typed, setTyped] = useState(false);

    useEffect(() => {
        const current = WORDS[wordIndex];
        const done = text === current;
        const cleared = text === '';

        let delay = deleting ? 45 : 110;
        if (done && !deleting) delay = 1600;
        if (cleared && deleting) delay = 320;

        const timer = window.setTimeout(() => {
            if (!deleting) {
                if (done) {
                    setDeleting(true);
                } else {
                    setText(current.slice(0, text.length + 1));
                }
            } else {
                if (cleared) {
                    setDeleting(false);
                    setWordIndex((i) => (i + 1) % WORDS.length);
                } else {
                    setText(current.slice(0, text.length - 1));
                }
            }
        }, delay);

        return () => window.clearTimeout(timer);
    }, [text, deleting, wordIndex]);

    useEffect(() => {
        const timer = window.setTimeout(() => setTyped(true), 700);
        return () => window.clearTimeout(timer);
    }, []);

    return (
        <div className="hero-intro">
            <div className="hero-badge">
                <span className="hero-badge-dot" />
                <span>Personal Lab · 持续更新中</span>
            </div>

            <h1 className="hero-title">
                <span className="hero-title-line">Hi, 我是</span>
                <span className="hero-title-name">ZHR</span>
            </h1>

            <p className="hero-subtitle">
                一个把日常折腾沉淀下来的地方 —— 我用它记录关于
                <span className="hero-typed">
                    {text}
                    <span className={`hero-caret${typed ? ' is-visible' : ''}`} />
                </span>
            </p>

            <p className="hero-desc">
                这里堆着我写的工具、学习游戏和技术笔记。从 C 语言工具库到初中英语单词闯关，
                每一件都是真实用得上的东西。
            </p>

            <div className="hero-actions">
                <a href="#tools" className="hero-btn hero-btn-primary">
                    <span>探索作品</span>
                    <svg viewBox="0 0 20 20" fill="none" aria-hidden="true">
                        <path d="M4 10h12M11 5l5 5-5 5" stroke="currentColor" strokeWidth="1.8" strokeLinecap="round" strokeLinejoin="round" />
                    </svg>
                </a>
                <a href="#writing" className="hero-btn hero-btn-ghost">
                    <span>阅读文章</span>
                </a>
            </div>

            <div className="hero-stats">
                <div className="hero-stat">
                    <strong>8</strong>
                    <span>实用作品</span>
                </div>
                <div className="hero-stat-divider" />
                <div className="hero-stat">
                    <strong>149</strong>
                    <span>C 工具模块</span>
                </div>
                <div className="hero-stat-divider" />
                <div className="hero-stat">
                    <strong>111</strong>
                    <span>收录唐诗</span>
                </div>
            </div>
        </div>
    );
}
