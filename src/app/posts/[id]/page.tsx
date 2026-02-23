import { getAllPostIds, getPostData } from "@/lib/api";
import Header from "@/components/Header";

type Params = {
    params: Promise<{
        id: string;
    }>;
};

export async function generateStaticParams() {
    const paths = getAllPostIds();
    return paths.map((path) => path.params);
}

export default async function Post({ params }: Params) {
    const { id } = await params;
    const postData = await getPostData(id);

    return (
        <>
            <Header />
            <article>
            <div style={{ marginBottom: '3rem', borderBottom: '1px solid var(--border-color)', paddingBottom: '2rem' }}>
                <h1 style={{ fontSize: '3rem', marginBottom: '1rem', lineHeight: '1.1' }}>{postData.title}</h1>
                <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', color: 'var(--text-secondary)' }}>
                    <time dateTime={postData.date}>{postData.date}</time>
                    <a
                        href={`https://github.com/YourUsername/YourRepo/edit/main/posts/${id}.md`}
                        target="_blank"
                        rel="noopener noreferrer"
                        className="btn btn-secondary"
                        style={{ fontSize: '0.875rem', padding: '0.25rem 0.75rem', borderRadius: '4px' }}
                    >
                        Edit this Post on GitHub
                    </a>
                </div>
            </div>

            {postData.coverImage && (
                <img
                    src={postData.coverImage}
                    alt={postData.title}
                    style={{ width: '100%', borderRadius: 'var(--radius-lg)', marginBottom: '2rem' }}
                />
            )}

            <div
                dangerouslySetInnerHTML={{ __html: postData.contentHtml || "" }}
                style={{ fontSize: '1.1rem', lineHeight: '1.8' }}
            />
        </article>
        </>
    );
}
