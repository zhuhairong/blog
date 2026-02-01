import { getSortedPostsData } from "@/lib/api";
import PostCard from "@/components/PostCard";
import Link from "next/link";

export default function Home() {
  const allPostsData = getSortedPostsData();
  return (
    <section>
      <div style={{ padding: '4rem 0', textAlign: 'center', marginBottom: '4rem' }}>
        <h1 style={{ fontSize: '4rem', marginBottom: '1.5rem' }}>Welcome to My Blog</h1>
        <p style={{ fontSize: '1.25rem', color: 'var(--text-secondary)', maxWidth: '600px', margin: '0 auto 2.5rem auto' }}>
          A space for thoughts, ideas, and stories. Built with Next.js and Markdown.
        </p>
        <div style={{ display: 'flex', gap: '1rem', justifyContent: 'center' }}>
          <a
            href="https://github.com/new"
            target="_blank"
            rel="noopener noreferrer"
            className="btn"
          >
            + Create New Post (GitHub)
          </a>
        </div>
      </div>

      <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '2rem' }}>
        <h2>Latest Writings</h2>
      </div>

      <div style={{ display: 'grid', gap: '2rem', gridTemplateColumns: 'repeat(auto-fill, minmax(320px, 1fr))' }}>
        {allPostsData.map((post) => (
          <PostCard key={post.id} post={post} />
        ))}
      </div>
    </section>
  );
}
