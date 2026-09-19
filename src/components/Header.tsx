'use client';

import Link from 'next/link';
import { useEffect, useState } from 'react';
import { BrandMark } from './icons';

const NAV = [
    { href: '/#tools', label: '作品' },
    { href: '/#writing', label: '文章' },
    { href: '/c-utils', label: 'C-Utils' },
];

export default function Header() {
    const [scrolled, setScrolled] = useState(false);
    const [progress, setProgress] = useState(0);

    useEffect(() => {
        let frame = 0;

        const update = () => {
            const top = window.scrollY;
            const max = document.documentElement.scrollHeight - window.innerHeight;
            setScrolled(top > 8);
            setProgress(max > 8 ? Math.min(1, top / max) : 0);
            frame = 0;
        };

        const onScroll = () => {
            if (frame) return;
            frame = window.requestAnimationFrame(update);
        };

        update();
        window.addEventListener('scroll', onScroll, { passive: true });
        window.addEventListener('resize', onScroll);

        return () => {
            if (frame) window.cancelAnimationFrame(frame);
            window.removeEventListener('scroll', onScroll);
            window.removeEventListener('resize', onScroll);
        };
    }, []);

    return (
        <header className="site-header" data-scrolled={scrolled}>
            <div className="container site-header-inner">
                <Link href="/" className="brand" aria-label="返回首页">
                    <span className="brand-mark">
                        <BrandMark size={21} />
                    </span>
                    <span className="brand-text">
                        <span className="brand-name">ZHR</span>
                        <span className="brand-sub">Blog</span>
                    </span>
                </Link>

                <nav className="nav" aria-label="主导航">
                    {NAV.map((item) => (
                        <Link key={item.href} href={item.href} className="nav-link">
                            <span className="nav-dot" aria-hidden="true" />
                            {item.label}
                        </Link>
                    ))}
                </nav>
            </div>

            <div className="header-progress" aria-hidden="true">
                <span style={{ transform: `scaleX(${progress})` }} />
            </div>
        </header>
    );
}
