import { getSortedPostsData } from "@/lib/api";
import PostCard from "@/components/PostCard";
import Link from "next/link";
import Header from "@/components/Header";

export default function Home() {
  const allPostsData = getSortedPostsData();
  return (
    <>
      <Header />
      <section>
      <div style={{ padding: '4rem 0', textAlign: 'center', marginBottom: '4rem' }}>
        <h1 style={{ fontSize: '3.5rem', marginBottom: '1.5rem' }}>Welcome to ZHR&apos;s AI Blog</h1>
        <p style={{ fontSize: '1.25rem', color: 'var(--text-secondary)', maxWidth: '600px', margin: '0 auto 2.5rem auto' }}>
          朋友来了有好酒。
        </p>
      </div>

      <div style={{ marginBottom: '4rem' }}>
        <Link href="/c-utils" style={{ textDecoration: 'none' }}>
          <div className="tool-card" style={{
            background: 'linear-gradient(135deg, #1e1b4b 0%, #312e81 50%, #4c1d95 100%)',
            borderRadius: '20px',
            padding: '2rem',
            color: 'white',
            cursor: 'pointer',
            boxShadow: '0 8px 32px rgba(124, 58, 237, 0.3)',
            border: '1px solid rgba(139, 92, 246, 0.3)',
          }}>
            <div style={{ fontSize: '3rem', marginBottom: '1rem' }}>📦</div>
            <h3 style={{ fontSize: '1.5rem', marginBottom: '0.75rem', fontWeight: 'bold' }}>C-Utils 代码库</h3>
            <p style={{ opacity: 0.9, lineHeight: 1.6 }}>
              149个轻量级C语言工具库，涵盖数据结构、算法、系统编程、网络协议、加密编码等领域。模块化设计，即插即用！
            </p>
            <div style={{ marginTop: '1rem', display: 'flex', gap: '0.5rem', flexWrap: 'wrap' }}>
              <span style={{ background: 'rgba(255,255,255,0.15)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>149模块</span>
              <span style={{ background: 'rgba(255,255,255,0.15)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>源码浏览</span>
              <span style={{ background: 'rgba(255,255,255,0.15)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>分类索引</span>
            </div>
          </div>
        </Link>
      </div>

      <div style={{ marginBottom: '4rem' }}>
        <h2 style={{ marginBottom: '1.5rem', display: 'flex', alignItems: 'center', gap: '0.5rem' }}>
          🎮 学习工具
        </h2>
        <div style={{ display: 'grid', gap: '1.5rem', gridTemplateColumns: 'repeat(auto-fill, minmax(320px, 1fr))' }}>
          <Link href="/eng.html" style={{ textDecoration: 'none' }}>
            <div className="tool-card" style={{
              background: 'linear-gradient(135deg, #312e81 0%, #4338ca 50%, #6366f1 100%)',
              borderRadius: '20px',
              padding: '2rem',
              color: 'white',
              cursor: 'pointer',
              boxShadow: '0 8px 32px rgba(99, 102, 241, 0.3)',
              border: '1px solid rgba(99, 102, 241, 0.3)',
            }}>
              <div style={{ fontSize: '3rem', marginBottom: '1rem' }}>📚</div>
              <h3 style={{ fontSize: '1.5rem', marginBottom: '0.75rem', fontWeight: 'bold' }}>英语单词大冒险</h3>
              <p style={{ opacity: 0.9, lineHeight: 1.6 }}>
                人教版八年级下册英语单词学习游戏，包含8个单元完整词库。支持单词卡片、选择题、拼写练习等多种学习模式，让背单词变得有趣！
              </p>
              <div style={{ marginTop: '1rem', display: 'flex', gap: '0.5rem', flexWrap: 'wrap' }}>
                <span style={{ background: 'rgba(255,255,255,0.15)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>单词卡片</span>
                <span style={{ background: 'rgba(255,255,255,0.15)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>拼写练习</span>
                <span style={{ background: 'rgba(255,255,255,0.15)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>成就系统</span>
              </div>
            </div>
          </Link>

          <Link href="/cem.html" style={{ textDecoration: 'none' }}>
            <div className="tool-card" style={{
              background: 'linear-gradient(135deg, #064e3b 0%, #047857 50%, #059669 100%)',
              borderRadius: '20px',
              padding: '2rem',
              color: 'white',
              cursor: 'pointer',
              boxShadow: '0 8px 32px rgba(16, 185, 129, 0.3)',
              border: '1px solid rgba(16, 185, 129, 0.3)',
            }}>
              <div style={{ fontSize: '3rem', marginBottom: '1rem' }}>🧪</div>
              <h3 style={{ fontSize: '1.5rem', marginBottom: '0.75rem', fontWeight: 'bold' }}>化学元素闯关</h3>
              <p style={{ opacity: 0.9, lineHeight: 1.6 }}>
                初中化学知识点闯关游戏，涵盖1-36号元素、离子符号、化合价、化学式等8大关卡。随机出题，举一反三，轻松掌握化学基础！
              </p>
              <div style={{ marginTop: '1rem', display: 'flex', gap: '0.5rem', flexWrap: 'wrap' }}>
                <span style={{ background: 'rgba(255,255,255,0.15)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>8大关卡</span>
                <span style={{ background: 'rgba(255,255,255,0.15)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>随机出题</span>
                <span style={{ background: 'rgba(255,255,255,0.15)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>知识点仓库</span>
              </div>
            </div>
          </Link>

          <Link href="/tang-poems.html" style={{ textDecoration: 'none' }}>
            <div className="tool-card" style={{
              background: 'linear-gradient(135deg, #4a1d96 0%, #7c2d12 50%, #b45309 100%)',
              borderRadius: '20px',
              padding: '2rem',
              color: 'white',
              cursor: 'pointer',
              boxShadow: '0 8px 32px rgba(180, 83, 9, 0.3)',
              border: '1px solid rgba(180, 83, 9, 0.3)',
            }}>
              <div style={{ fontSize: '3rem', marginBottom: '1rem' }}>📜</div>
              <h3 style={{ fontSize: '1.5rem', marginBottom: '0.75rem', fontWeight: 'bold' }}>唐诗三百首 · 墨韵</h3>
              <p style={{ opacity: 0.9, lineHeight: 1.6 }}>
                精选30首经典唐诗，涵盖李白、杜甫、白居易等名家名作。支持搜索、分类浏览，水墨风格沉浸式阅读，感受千年诗词之美！
              </p>
              <div style={{ marginTop: '1rem', display: 'flex', gap: '0.5rem', flexWrap: 'wrap' }}>
                <span style={{ background: 'rgba(255,255,255,0.15)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>30首经典</span>
                <span style={{ background: 'rgba(255,255,255,0.15)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>分类浏览</span>
                <span style={{ background: 'rgba(255,255,255,0.15)', padding: '0.25rem 0.75rem', borderRadius: '999px', fontSize: '0.875rem' }}>水墨风格</span>
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
    </>
  );
}
