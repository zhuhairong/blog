import Link from 'next/link';

type Post = {
    id: string;
    title: string;
    date: string;
    excerpt?: string;
    coverImage?: string;
};

export default function PostCard({ post }: { post: Post }) {
    return (
        <div className="card">
            <Link href={`/posts/${post.id}`}>
                <h3>{post.title}</h3>
                <p className="text-secondary" style={{ fontSize: '0.9rem', marginBottom: '0.5rem' }}>
                    {post.date}
                </p>
                <p>{post.excerpt}</p>
            </Link>
        </div>
    );
}
