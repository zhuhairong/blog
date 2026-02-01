import Link from 'next/link';

export default function Header() {
    return (
        <header className="header container">
            <Link href="/" className="logo">
                <h2>My Blog</h2>
            </Link>
            <nav>
                <Link href="/" className="btn">
                    Home
                </Link>
            </nav>
        </header>
    );
}
