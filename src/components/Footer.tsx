import Link from 'next/link';

const LINKS = [
    { href: '/c-utils', label: 'C-Utils' },
    { href: '/vocabulary/index.html', label: '单词记忆' },
    { href: '/cloze/index.html', label: '完型填空' },
    { href: '/tang-poems.html', label: '唐诗三百首' },
];

export default function Footer() {
    return (
        <footer className="footer container">
            <div className="footer-top">
                <div className="footer-brand">
                    <div className="footer-logo" aria-hidden="true">
                        <svg viewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/svg">
                            <rect x="2" y="6" width="36" height="28" rx="4" stroke="currentColor" strokeWidth="2.5" fill="none" />
                            <path d="M10 14h20M10 20h14M10 26h8" stroke="currentColor" strokeWidth="2.5" strokeLinecap="round" />
                        </svg>
                    </div>
                    <div>
                        <p className="footer-title">ZHR&apos;s AI Blog</p>
                        <p className="footer-tagline">朋友来了有好酒。</p>
                    </div>
                </div>

                <nav className="footer-links">
                    {LINKS.map((link) => (
                        <Link key={link.href} href={link.href} className="footer-link">
                            {link.label}
                        </Link>
                    ))}
                    <a
                        href="https://github.com/zhuhairong/blog"
                        target="_blank"
                        rel="noreferrer"
                        className="footer-link"
                    >
                        GitHub
                    </a>
                </nav>
            </div>

            <div className="footer-bottom">
                <p>&copy; {new Date().getFullYear()} ZHR. All rights reserved.</p>
                <p className="footer-built">Built with Next.js</p>
            </div>
        </footer>
    );
}
