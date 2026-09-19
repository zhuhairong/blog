'use client';

import Link from 'next/link';
import { useEffect, useState } from 'react';

const NAV = [
    { href: '/#tools', label: '作品' },
    { href: '/#writing', label: '文章' },
    { href: '/c-utils', label: 'C-Utils' },
];

export default function Header() {
    const [scrolled, setScrolled] = useState(false);
    const [progress, setProgress] = useState(0);

    useEffect(() => {
        const onScroll = () => {
            const top = window.scrollY;
            setScrolled(top > 12);

            const max = document.documentElement.scrollHeight - window.innerHeight;
            setProgress(max > 0 ? Math.min(1, top / max) : 0);
        };

        onScroll();
        window.addEventListener('scroll', onScroll, { passive: true });
        window.addEventListener('resize', onScroll);
        return () => {
            window.removeEventListener('scroll', onScroll);
            window.removeEventListener('resize', onScroll);
        };
    }, []);

    return (
        <header className={`header container${scrolled ? ' is-scrolled' : ''}`}>
            <Link href="/" className="logo" aria-label="返回首页">
                <div className="logo-wrapper">
                    <div className="logo-icon">
                        <svg viewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/svg" aria-hidden="true">
                            <rect x="2" y="6" width="36" height="28" rx="4" stroke="currentColor" strokeWidth="2.5" fill="none" />
                            <path d="M10 14h20M10 20h14M10 26h8" stroke="currentColor" strokeWidth="2.5" strokeLinecap="round" />
                        </svg>
                    </div>
                    <div className="logo-text">
                        <span className="logo-title">ZHR</span>
                        <span className="logo-subtitle">Blog</span>
                    </div>
                </div>
            </Link>

            <nav className="nav-list">
                {NAV.map((item) => (
                    <Link key={item.href} href={item.href} className="nav-link">
                        <span className="nav-dot" />
                        {item.label}
                    </Link>
                ))}
            </nav>

            <div className="header-progress" aria-hidden="true">
                <span style={{ transform: `scaleX(${progress})` }} />
            </div>

            <style jsx>{`
                .logo-wrapper {
                    display: flex;
                    align-items: center;
                    gap: 0.75rem;
                }

                .logo-icon {
                    width: 40px;
                    height: 40px;
                    display: flex;
                    align-items: center;
                    justify-content: center;
                    background: linear-gradient(135deg, #6366f1 0%, #8b5cf6 50%, #ec4899 100%);
                    border-radius: 12px;
                    color: white;
                    transition: transform 0.3s ease, box-shadow 0.3s ease;
                    box-shadow: 0 4px 15px rgba(99, 102, 241, 0.3);
                    flex-shrink: 0;
                }

                .logo-icon svg {
                    width: 24px;
                    height: 24px;
                }

                .logo-wrapper:hover .logo-icon {
                    transform: rotate(-5deg) scale(1.05);
                    box-shadow: 0 6px 20px rgba(99, 102, 241, 0.45);
                }

                .logo-text {
                    display: flex;
                    flex-direction: column;
                    line-height: 1.1;
                }

                .logo-title {
                    font-size: 1.5rem;
                    font-weight: 800;
                    letter-spacing: -0.02em;
                    background: linear-gradient(135deg, #6366f1 0%, #8b5cf6 50%, #ec4899 100%);
                    -webkit-background-clip: text;
                    -webkit-text-fill-color: transparent;
                    background-clip: text;
                }

                .logo-subtitle {
                    font-size: 0.7rem;
                    font-weight: 600;
                    letter-spacing: 0.15em;
                    text-transform: uppercase;
                    color: var(--text-tertiary);
                    margin-top: 2px;
                }

                .nav-list {
                    display: flex;
                    align-items: center;
                    gap: 0.5rem;
                }

                .nav-link {
                    display: flex;
                    align-items: center;
                    gap: 0.45rem;
                    padding: 0.5rem 1rem;
                    border-radius: 999px;
                    font-weight: 500;
                    font-size: 0.9rem;
                    color: var(--text-secondary);
                    border: 1px solid transparent;
                    transition: all 0.22s ease;
                    position: relative;
                }

                .nav-link:hover {
                    color: var(--text-primary);
                    background: rgba(124, 58, 237, 0.12);
                    border-color: var(--border-color);
                    transform: translateY(-1px);
                }

                .nav-dot {
                    width: 5px;
                    height: 5px;
                    border-radius: 50%;
                    background: var(--accent-secondary);
                    opacity: 0;
                    transform: scale(0);
                    transition: all 0.22s ease;
                }

                .nav-link:hover .nav-dot {
                    opacity: 1;
                    transform: scale(1);
                }

                .header-progress {
                    position: absolute;
                    left: 0;
                    right: 0;
                    bottom: -1px;
                    height: 2px;
                    background: transparent;
                    overflow: hidden;
                }

                .header-progress span {
                    display: block;
                    height: 100%;
                    width: 100%;
                    transform-origin: 0 50%;
                    transform: scaleX(0);
                    background: linear-gradient(90deg, #7c3aed, #a855f7 45%, #ec4899 80%, #38bdf8);
                    transition: transform 0.12s linear;
                }

                @media (max-width: 560px) {
                    .logo-title {
                        font-size: 1.25rem;
                    }

                    .logo-icon {
                        width: 36px;
                        height: 36px;
                        border-radius: 10px;
                    }

                    .logo-icon svg {
                        width: 20px;
                        height: 20px;
                    }

                    .nav-link {
                        padding: 0.45rem 0.7rem;
                        font-size: 0.82rem;
                    }
                }

                @media (max-width: 420px) {
                    .nav-link:not(:first-child) {
                        display: none;
                    }
                }
            `}</style>
        </header>
    );
}
