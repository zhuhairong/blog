import Link from 'next/link';
import { getAllPostIds, getPostData } from '@/lib/api';
import Header from '@/components/Header';
import Footer from '@/components/Footer';
import { ArrowRight, Calendar } from '@/components/icons';
import styles from './post.module.css';

const REPO = 'https://github.com/zhuhairong/blog';

type Params = {
    params: Promise<{ id: string }>;
};

export async function generateStaticParams() {
    const paths = getAllPostIds();
    return paths.map((path) => path.params);
}

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

export default async function Post({ params }: Params) {
    const { id } = await params;
    const post = await getPostData(id);

    return (
        <>
            <Header />

            <div className="container">
                <article className={styles.article}>
                    <Link href="/" className={styles.back}>
                        <ArrowRight size={16} className={styles.backIcon} />
                        返回首页
                    </Link>

                    <header className={styles.head}>
                        <h1 className={styles.title}>{post.title}</h1>
                        <div className={styles.meta}>
                            <span className={styles.date}>
                                <Calendar size={15} />
                                <time dateTime={post.date}>{formatDate(post.date)}</time>
                            </span>
                            <a
                                href={`${REPO}/edit/master/posts/${id}.md`}
                                target="_blank"
                                rel="noreferrer"
                                className={styles.edit}
                            >
                                在 GitHub 编辑
                            </a>
                        </div>
                    </header>

                    {post.coverImage && (
                        // eslint-disable-next-line @next/next/no-img-element
                        <img className={styles.cover} src={post.coverImage} alt={post.title} />
                    )}

                    <div
                        className={styles.content}
                        dangerouslySetInnerHTML={{ __html: post.contentHtml || '' }}
                    />
                </article>
            </div>

            <Footer />
        </>
    );
}
