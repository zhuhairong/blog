import Link from 'next/link';
import { ArrowRight, Calendar } from './icons';
import styles from '@/app/page.module.css';

type Post = {
    id: string;
    title: string;
    date: string;
    excerpt?: string;
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
        <div className={styles.postCard}>
            <Link href={`/posts/${post.id}`} className={styles.postCardLink}>
                <div className={styles.postCardTop}>
                    <span className={styles.postDate}>
                        <Calendar size={14} />
                        {formatDate(post.date)}
                    </span>
                    <span className={styles.postArrow} aria-hidden="true">
                        <svg viewBox="0 0 24 24" width="20" height="20" fill="none" aria-hidden="true">
                            <path
                                d="M7 17L17 7M17 7H8M17 7v9"
                                stroke="currentColor"
                                strokeWidth="2"
                                strokeLinecap="round"
                                strokeLinejoin="round"
                            />
                        </svg>
                    </span>
                </div>

                <h3 className={styles.postTitle}>{post.title}</h3>
                {post.excerpt ? <p className={styles.postExcerpt}>{post.excerpt}</p> : null}

                <span className={styles.postMore}>
                    阅读全文
                    <ArrowRight size={15} />
                </span>
            </Link>
        </div>
    );
}
