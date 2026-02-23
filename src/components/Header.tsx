'use client';

import Link from 'next/link';

export default function Header() {
    return (
        <header className="header container">
            <Link href="/" className="logo">
                <div className="logo-wrapper">
                    <div className="logo-icon">
                        <svg viewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/svg">
                            <rect x="2" y="6" width="36" height="28" rx="4" stroke="currentColor" strokeWidth="2.5" fill="none"/>
                            <path d="M10 14h20M10 20h14M10 26h8" stroke="currentColor" strokeWidth="2.5" strokeLinecap="round"/>
                        </svg>
                    </div>
                    <div className="logo-text">
                        <span className="logo-title">ZHR</span>
                        <span className="logo-subtitle">Blog</span>
                    </div>
                </div>
            </Link>
            <nav>
                <Link href="/" className="nav-link">
                    <span className="nav-dot"></span>
                    Home
                </Link>
            </nav>
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
                }
                
                .logo-icon svg {
                    width: 24px;
                    height: 24px;
                }
                
                .logo-wrapper:hover .logo-icon {
                    transform: rotate(-5deg) scale(1.05);
                    box-shadow: 0 6px 20px rgba(99, 102, 241, 0.4);
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
                
                .nav-link {
                    display: flex;
                    align-items: center;
                    gap: 0.5rem;
                    padding: 0.5rem 1rem;
                    border-radius: 999px;
                    font-weight: 500;
                    font-size: 0.9rem;
                    color: var(--text-secondary);
                    background: var(--bg-secondary);
                    border: 1px solid var(--border-color);
                    transition: all 0.2s ease;
                }
                
                .nav-link:hover {
                    color: var(--accent-primary);
                    background: var(--bg-primary);
                    border-color: var(--accent-primary);
                    transform: translateY(-1px);
                }
                
                .nav-dot {
                    width: 6px;
                    height: 6px;
                    border-radius: 50%;
                    background: var(--accent-primary);
                    opacity: 0;
                    transform: scale(0);
                    transition: all 0.2s ease;
                }
                
                .nav-link:hover .nav-dot {
                    opacity: 1;
                    transform: scale(1);
                }
                
                @media (max-width: 480px) {
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
                }
            `}</style>
        </header>
    );
}
