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
      </div>

      <div style={{ marginBottom: '4rem' }}>
        <h2 style={{ marginBottom: '1.5rem', display: 'flex', alignItems: 'center', gap: '0.5rem' }}>
          🎮 学习工具
        </h2>
        <div style={{ display: 'grid', gap: '1.5rem', gridTemplateColumns: 'repeat(auto-fill, minmax(320px, 1fr))' }}>
          <Link href="/eng.html" style={{ textDecoration: 'none' }}>
            <div className="tool-card" style={{
              background: 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)',
              borderRadius: '16px',
              padding: '2rem',
              color: 'white',
              cursor: 'pointer',
              boxShadow: '0 4px 20px rgba(102, 126, 234, 0.3)',
            }}>
              <div style={{ fontSize: '3rem', marginBottom: '1rem' }}>📚</div>
              <h3 style={{ fontSize: '1.5rem', marginBottom: '0.75rem', fontWeight: 'bold' }}>英语单词大冒险</h3>
              <p style={{ opacity: 0.9, lineHeight: 1.6 }}>
                人教版八年级下册英语单词学习游戏，包含8个单元完整词库。支持单词卡片、选择题、拼写练习等多种学习模式，让背单词变得有趣！
              </p>
              <div style={{ marginTop: '1rem', display: 'flex', gap: '0.5rem', flexWrap: 'wrap' }}>
                <span style={{ background: 'rgba(255,255,255,0.2)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>单词卡片</span>
                <span style={{ background: 'rgba(255,255,255,0.2)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>拼写练习</span>
                <span style={{ background: 'rgba(255,255,255,0.2)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>成就系统</span>
              </div>
            </div>
          </Link>

          <Link href="/cem.html" style={{ textDecoration: 'none' }}>
            <div className="tool-card" style={{
              background: 'linear-gradient(135deg, #11998e 0%, #38ef7d 100%)',
              borderRadius: '16px',
              padding: '2rem',
              color: 'white',
              cursor: 'pointer',
              boxShadow: '0 4px 20px rgba(17, 153, 142, 0.3)',
            }}>
              <div style={{ fontSize: '3rem', marginBottom: '1rem' }}>🧪</div>
              <h3 style={{ fontSize: '1.5rem', marginBottom: '0.75rem', fontWeight: 'bold' }}>化学元素闯关</h3>
              <p style={{ opacity: 0.9, lineHeight: 1.6 }}>
                初中化学知识点闯关游戏，涵盖1-36号元素、离子符号、化合价、化学式等8大关卡。随机出题，举一反三，轻松掌握化学基础！
              </p>
              <div style={{ marginTop: '1rem', display: 'flex', gap: '0.5rem', flexWrap: 'wrap' }}>
                <span style={{ background: 'rgba(255,255,255,0.2)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>8大关卡</span>
                <span style={{ background: 'rgba(255,255,255,0.2)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>随机出题</span>
                <span style={{ background: 'rgba(255,255,255,0.2)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>知识点仓库</span>
              </div>
            </div>
          </Link>
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
