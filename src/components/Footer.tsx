import Link from 'next/link';
import { BrandMark } from './icons';

const LINKS = [
    { href: '/poetry-atlas', label: '诗歌图谱' },
    { href: '/c-utils', label: 'C-Utils' },
    { href: '/vocabulary/index.html', label: '单词记忆' },
    { href: '/cloze/index.html', label: '完型填空' },
    { href: '/tang-poems.html', label: '唐诗三百首' },
];

export default function Footer() {
    return (
        <footer className="site-footer">
            <div className="container">
                <div className="footer-grid">
                    <div className="footer-brand">
                        <span className="footer-mark">
                            <BrandMark size={21} />
                        </span>
                        <div>
                            <p className="footer-name">ZHR&apos;s AI Blog</p>
                            <p className="footer-tagline">朋友来了有好酒。</p>
                        </div>
                    </div>

                    <nav className="footer-nav" aria-label="页脚导航">
                        {LINKS.map((link) => (
                            <Link key={link.href} href={link.href}>
                                {link.label}
                            </Link>
                        ))}
                        <a href="https://github.com/zhuhairong/blog" target="_blank" rel="noreferrer">
                            GitHub
                        </a>
                    </nav>
                </div>

                <div className="footer-bottom">
                    <p>&copy; {new Date().getFullYear()} ZHR. All rights reserved.</p>
                    <p>Built with Next.js</p>
                </div>
            </div>
        </footer>
    );
}
