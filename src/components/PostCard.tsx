import Link from 'next/link';

type Post = {
    id: string;
    title: string;
    date: string;
    excerpt?: string;
    coverImage?: string;
};

function formatDate(value: string) {
    if (!value) return '';
    const parsed = new Date(value);
    if (Number.isNaN(parsed.getTime())) return value;
    return parsed.toLocaleDateString('zh-CN', {
        year: 'numeric',
        month: 'long',
        day: 'numeric',
    });
}

export default function PostCard({ post }: { post: Post }) {
    return (
        <div className="card post-card">
            <Link href={`/posts/${post.id}`}>
                <div className="post-card-top">
                    <span className="post-date">
                        <svg viewBox="0 0 20 20" fill="none" aria-hidden="true">
                            <rect x="3" y="4.5" width="14" height="12" rx="2.5" stroke="currentColor" strokeWidth="1.5" />
                            <path d="M3 8.5h14M7 3v3M13 3v3" stroke="currentColor" strokeWidth="1.5" strokeLinecap="round" />
                        </svg>
                        {formatDate(post.date)}
                    </span>
                    <svg className="post-arrow" viewBox="0 0 20 20" fill="none" aria-hidden="true">
                        <path
                            d="M6 14L14 6M14 6H8M14 6v6"
                            stroke="currentColor"
                            strokeWidth="1.8"
                            strokeLinecap="round"
                            strokeLinejoin="round"
                        />
                    </svg>
                </div>

                <h3 className="post-title">{post.title}</h3>
                {post.excerpt ? <p className="post-excerpt">{post.excerpt}</p> : null}

                <span className="post-more">阅读全文</span>
            </Link>
        </div>
    );
}
