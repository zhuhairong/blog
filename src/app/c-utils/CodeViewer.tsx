'use client';

import { useState, useRef, useEffect } from 'react';
import Link from 'next/link';
import { Highlight, themes } from 'prism-react-renderer';

interface CodeFile {
  name: string;
  type: 'header' | 'source';
  category: string;
  description: string;
  headerContent: string;
  sourceContent: string | null;
  testContent: string | null;
  demoContent: string | null;
}

interface CodeViewerProps {
  codeFiles: CodeFile[];
  categories: string[];
}

export default function CodeViewer({ codeFiles, categories }: CodeViewerProps) {
  const [selectedCategory, setSelectedCategory] = useState('全部');
  const [searchQuery, setSearchQuery] = useState('');
  const [selectedFile, setSelectedFile] = useState<CodeFile | null>(null);
  const [activeTab, setActiveTab] = useState<'header' | 'source' | 'test' | 'demo'>('header');
  const [highlightLine, setHighlightLine] = useState<number | null>(null);
  const codeRef = useRef<HTMLPreElement>(null);
  const codeViewerRef = useRef<HTMLDivElement>(null);

  const filteredFiles = codeFiles.filter(file => {
    const matchesCategory = selectedCategory === '全部' || file.category === selectedCategory;
    const matchesSearch = file.name.toLowerCase().includes(searchQuery.toLowerCase()) ||
                         file.description.toLowerCase().includes(searchQuery.toLowerCase());
    return matchesCategory && matchesSearch;
  });

  useEffect(() => {
    if (highlightLine !== null && codeRef.current) {
      const lineElement = codeRef.current.querySelector(`[data-line="${highlightLine}"]`);
      if (lineElement) {
        lineElement.scrollIntoView({ behavior: 'smooth', block: 'center' });
      }
    }
  }, [highlightLine]);

  const handleFileSelect = (file: CodeFile) => {
    setSelectedFile(file);
    setActiveTab('header');
    setHighlightLine(null);
    setTimeout(() => {
      codeViewerRef.current?.scrollIntoView({ behavior: 'smooth', block: 'start' });
    }, 100);
  };

  const handleFunctionClick = (funcName: string) => {
    if (!selectedFile) return;
    
    const content = activeTab === 'header' 
      ? selectedFile.headerContent 
      : activeTab === 'source' 
        ? selectedFile.sourceContent 
        : activeTab === 'test'
          ? selectedFile.testContent
          : selectedFile.demoContent;
    
    if (!content) return;
    
    const lines = content.split('\n');
    for (let i = 0; i < lines.length; i++) {
      const line = lines[i];
      if (line.includes(funcName) && (line.includes('(') || line.includes('typedef'))) {
        setHighlightLine(i + 1);
        break;
      }
    }
  };

  const categoryColors: Record<string, string> = {
    '数据结构': '#3b82f6',
    '算法': '#8b5cf6',
    '编码': '#ec4899',
    '加密': '#ef4444',
    '系统': '#f59e0b',
    '网络': '#10b981',
    '格式': '#06b6d4',
    '文件': '#84cc16',
    '数学': '#6366f1',
    '工具': '#64748b',
    '其他': '#94a3b8',
  };

  const extractFunctions = (content: string): string[] => {
    const functions: string[] = [];
    const lines = content.split('\n');
    
    for (const line of lines) {
      const funcMatch = line.match(/^\s*(?:static\s+)?(?:inline\s+)?[\w_]+\s+\*?\s*([\w_]+)\s*\(/);
      if (funcMatch && !funcMatch[1].startsWith('_')) {
        functions.push(funcMatch[1]);
      }
      
      const typedefMatch = line.match(/typedef\s+struct\s+([\w_]+)/);
      if (typedefMatch) {
        functions.push(typedefMatch[1]);
      }
    }
    
    return [...new Set(functions)].slice(0, 10);
  };

  const getCurrentContent = () => {
    if (!selectedFile) return '';
    switch (activeTab) {
      case 'header': return selectedFile.headerContent;
      case 'source': return selectedFile.sourceContent || '';
      case 'test': return selectedFile.testContent || '';
      case 'demo': return selectedFile.demoContent || '';
      default: return '';
    }
  };

  return (
    <div className="min-h-screen" style={{ paddingTop: '70px' }}>
      <nav style={{
        position: 'fixed',
        top: 0,
        left: 0,
        right: 0,
        zIndex: 1000,
        background: 'rgba(10, 10, 15, 0.9)',
        backdropFilter: 'blur(20px)',
        WebkitBackdropFilter: 'blur(20px)',
        boxShadow: '0 4px 30px rgba(0, 0, 0, 0.3)',
        padding: '0 20px',
        borderBottom: '1px solid rgba(124, 58, 237, 0.2)',
      }}>
        <div style={{
          maxWidth: '1400px',
          margin: '0 auto',
          display: 'flex',
          justifyContent: 'space-between',
          alignItems: 'center',
          height: '50px',
        }}>
          <Link href="/" style={{
            display: 'flex',
            alignItems: 'center',
            gap: '10px',
            textDecoration: 'none',
            color: '#f0f0f5',
          }}>
            <div style={{
              width: '32px',
              height: '32px',
              background: 'linear-gradient(135deg, #7c3aed 0%, #a855f7 50%, #ec4899 100%)',
              borderRadius: '8px',
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'center',
              color: 'white',
              fontSize: '16px',
            }}>
              📦
            </div>
            <span style={{
              fontWeight: 700,
              fontSize: '16px',
              background: 'linear-gradient(135deg, #7c3aed 0%, #a855f7 50%, #ec4899 100%)',
              WebkitBackgroundClip: 'text',
              WebkitTextFillColor: 'transparent',
              backgroundClip: 'text',
            }}>
              ZHR Blog
            </span>
          </Link>
          <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
            <Link href="/" style={{
              padding: '6px 14px',
              borderRadius: '20px',
              textDecoration: 'none',
              fontSize: '13px',
              fontWeight: 500,
              transition: 'all 0.2s ease',
              color: '#a0a0b0',
              background: 'rgba(124, 58, 237, 0.1)',
              border: '1px solid rgba(124, 58, 237, 0.2)',
            }}>首页</Link>
            <Link href="/eng.html" style={{
              padding: '6px 14px',
              borderRadius: '20px',
              textDecoration: 'none',
              fontSize: '13px',
              fontWeight: 500,
              transition: 'all 0.2s ease',
              color: '#a0a0b0',
              background: 'rgba(124, 58, 237, 0.1)',
              border: '1px solid rgba(124, 58, 237, 0.2)',
            }}>英语</Link>
            <Link href="/cem.html" style={{
              padding: '6px 14px',
              borderRadius: '20px',
              textDecoration: 'none',
              fontSize: '13px',
              fontWeight: 500,
              transition: 'all 0.2s ease',
              color: '#a0a0b0',
              background: 'rgba(124, 58, 237, 0.1)',
              border: '1px solid rgba(124, 58, 237, 0.2)',
            }}>化学</Link>
            <Link href="/tang-poems.html" style={{
              padding: '6px 14px',
              borderRadius: '20px',
              textDecoration: 'none',
              fontSize: '13px',
              fontWeight: 500,
              transition: 'all 0.2s ease',
              color: '#a0a0b0',
              background: 'rgba(124, 58, 237, 0.1)',
              border: '1px solid rgba(124, 58, 237, 0.2)',
            }}>唐诗</Link>
            <Link href="/c-utils" style={{
              padding: '6px 14px',
              borderRadius: '20px',
              textDecoration: 'none',
              fontSize: '13px',
              fontWeight: 500,
              transition: 'all 0.2s ease',
              background: 'linear-gradient(135deg, #7c3aed 0%, #a855f7 100%)',
              color: 'white',
              border: 'none',
            }}>代码库</Link>
          </div>
        </div>
      </nav>

      <div style={{ textAlign: 'center', marginBottom: '3rem' }}>
        <h1 style={{ fontSize: '3rem', marginBottom: '1rem' }}>📦 C-Utils 代码库</h1>
        <p style={{ color: 'var(--text-secondary)', fontSize: '1.1rem', maxWidth: '600px', margin: '0 auto' }}>
          149 个轻量级、模块化的 C 语言工具库，涵盖数据结构、算法、系统编程、网络协议、加密编码等领域
        </p>
      </div>

      <div style={{ display: 'flex', gap: '1rem', marginBottom: '2rem', flexWrap: 'wrap' }}>
        <input
          type="text"
          placeholder="搜索模块名称或描述..."
          value={searchQuery}
          onChange={(e) => setSearchQuery(e.target.value)}
          style={{
            flex: 1,
            minWidth: '200px',
            padding: '0.75rem 1rem',
            borderRadius: '8px',
            border: '1px solid var(--border-color)',
            background: 'var(--bg-secondary)',
            color: 'var(--text-primary)',
            fontSize: '1rem',
          }}
        />
        <select
          value={selectedCategory}
          onChange={(e) => setSelectedCategory(e.target.value)}
          style={{
            padding: '0.75rem 1rem',
            borderRadius: '8px',
            border: '1px solid var(--border-color)',
            background: 'var(--bg-secondary)',
            color: 'var(--text-primary)',
            fontSize: '1rem',
            cursor: 'pointer',
          }}
        >
          <option value="全部">全部分类</option>
          {categories.map(cat => (
            <option key={cat} value={cat}>{cat}</option>
          ))}
        </select>
      </div>

      <div style={{ display: 'grid', gap: '0.75rem', gridTemplateColumns: 'repeat(auto-fill, minmax(180px, 1fr))', marginBottom: '2rem' }}>
        {filteredFiles.map(file => (
          <div
            key={file.name}
            onClick={() => handleFileSelect(file)}
            style={{
              padding: '0.75rem 1rem',
              borderRadius: '8px',
              background: selectedFile?.name === file.name ? 'var(--bg-primary)' : 'var(--bg-secondary)',
              border: `1px solid ${selectedFile?.name === file.name ? 'var(--accent-primary)' : 'transparent'}`,
              cursor: 'pointer',
              transition: 'all 0.2s ease',
            }}
          >
            <div style={{ display: 'flex', alignItems: 'center', gap: '0.25rem', marginBottom: '0.25rem' }}>
              <span style={{
                padding: '0.125rem 0.25rem',
                borderRadius: '3px',
                fontSize: '0.625rem',
                background: categoryColors[file.category] || '#64748b',
                color: 'white',
              }}>
                {file.category}
              </span>
            </div>
            <h3 style={{ fontSize: '1rem', marginBottom: '0', fontWeight: '600' }}>
              {file.name}
            </h3>
          </div>
        ))}
      </div>

      {selectedFile && (
        <div ref={codeViewerRef} style={{
          marginTop: '2rem',
          borderRadius: '16px',
          background: 'var(--bg-secondary)',
          border: '1px solid var(--border-color)',
          overflow: 'hidden',
        }}>
          <div style={{
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'space-between',
            padding: '1rem 1.5rem',
            borderBottom: '1px solid var(--border-color)',
            background: 'var(--bg-primary)',
            flexWrap: 'wrap',
            gap: '1rem',
          }}>
            <h3 style={{ fontSize: '1.5rem', fontWeight: '600' }}>
              📄 {selectedFile.name} - {selectedFile.description}
            </h3>
            <div style={{ display: 'flex', gap: '0.5rem' }}>
              {selectedFile.testContent && (
                <a
                  href={`https://github.com/zhuhairong/blog/blob/main/c_utils/tests/test_${selectedFile.name}.c`}
                  target="_blank"
                  rel="noopener noreferrer"
                  style={{
                    padding: '0.5rem 1rem',
                    borderRadius: '8px',
                    border: 'none',
                    background: '#22c55e',
                    color: 'white',
                    cursor: 'pointer',
                    textDecoration: 'none',
                    fontSize: '0.875rem',
                  }}
                >
                  🔗 GitHub 测试文件
                </a>
              )}
              <button
                onClick={() => {
                  setSelectedFile(null);
                  setHighlightLine(null);
                }}
                style={{
                  padding: '0.5rem 1rem',
                  borderRadius: '8px',
                  border: 'none',
                  background: 'var(--bg-secondary)',
                  color: 'var(--text-primary)',
                  cursor: 'pointer',
                }}
              >
                关闭
              </button>
            </div>
          </div>

          <div style={{ display: 'flex', borderBottom: '1px solid var(--border-color)', flexWrap: 'wrap' }}>
            <button
              onClick={() => { setActiveTab('header'); setHighlightLine(null); }}
              style={{
                padding: '0.75rem 1.5rem',
                border: 'none',
                background: activeTab === 'header' ? 'var(--accent-primary)' : 'transparent',
                color: activeTab === 'header' ? 'white' : 'var(--text-primary)',
                cursor: 'pointer',
                fontWeight: 500,
              }}
            >
              📋 {selectedFile.name}.h
            </button>
            {selectedFile.sourceContent && (
              <button
                onClick={() => { setActiveTab('source'); setHighlightLine(null); }}
                style={{
                  padding: '0.75rem 1.5rem',
                  border: 'none',
                  background: activeTab === 'source' ? 'var(--accent-primary)' : 'transparent',
                  color: activeTab === 'source' ? 'white' : 'var(--text-primary)',
                  cursor: 'pointer',
                  fontWeight: 500,
                }}
              >
                ⚙️ {selectedFile.name}.c
              </button>
            )}
            {selectedFile.testContent && (
              <button
                onClick={() => { setActiveTab('test'); setHighlightLine(null); }}
                style={{
                  padding: '0.75rem 1.5rem',
                  border: 'none',
                  background: activeTab === 'test' ? '#22c55e' : 'transparent',
                  color: activeTab === 'test' ? 'white' : 'var(--text-primary)',
                  cursor: 'pointer',
                  fontWeight: 500,
                }}
              >
                🧪 test_{selectedFile.name}.c
              </button>
            )}
            {selectedFile.demoContent && (
              <button
                onClick={() => { setActiveTab('demo'); setHighlightLine(null); }}
                style={{
                  padding: '0.75rem 1.5rem',
                  border: 'none',
                  background: activeTab === 'demo' ? '#f59e0b' : 'transparent',
                  color: activeTab === 'demo' ? 'white' : 'var(--text-primary)',
                  cursor: 'pointer',
                  fontWeight: 500,
                }}
              >
                🎬 demo_{selectedFile.name}.c
              </button>
            )}
          </div>

          {selectedFile.headerContent && (
            <div style={{ 
              padding: '1rem 1.5rem', 
              background: 'var(--bg-primary)',
              borderBottom: '1px solid var(--border-color)',
            }}>
              <div style={{ 
                fontSize: '0.875rem', 
                color: 'var(--text-secondary)',
                marginBottom: '0.5rem',
              }}>
                快速跳转：
              </div>
              <div style={{ display: 'flex', gap: '0.5rem', flexWrap: 'wrap' }}>
                {extractFunctions(selectedFile.headerContent).map(func => (
                  <button
                    key={func}
                    onClick={() => handleFunctionClick(func)}
                    style={{
                      padding: '0.25rem 0.75rem',
                      borderRadius: '999px',
                      border: '1px solid var(--border-color)',
                      background: 'var(--bg-secondary)',
                      color: 'var(--text-primary)',
                      cursor: 'pointer',
                      fontSize: '0.75rem',
                      transition: 'all 0.2s',
                    }}
                    onMouseEnter={(e) => {
                      e.currentTarget.style.background = 'var(--accent-primary)';
                      e.currentTarget.style.color = 'white';
                    }}
                    onMouseLeave={(e) => {
                      e.currentTarget.style.background = 'var(--bg-secondary)';
                      e.currentTarget.style.color = 'var(--text-primary)';
                    }}
                  >
                    {func}
                  </button>
                ))}
              </div>
            </div>
          )}

          <Highlight
            theme={themes.vsDark}
            code={getCurrentContent()}
            language="c"
          >
            {({ className, style, tokens, getLineProps, getTokenProps }) => (
              <pre ref={codeRef} className={className} style={{
                ...style,
                padding: '1.5rem',
                margin: 0,
                overflow: 'auto',
                maxHeight: '1000px',
                fontSize: '0.875rem',
                lineHeight: 1.6,
              }}>
                {tokens.map((line, i) => (
                  <div 
                    key={i} 
                    {...getLineProps({ line })}
                    data-line={i + 1}
                    style={{
                      display: 'flex',
                      background: highlightLine === i + 1 ? 'rgba(255, 255, 0, 0.2)' : 'transparent',
                      transition: 'background 0.3s',
                    }}
                  >
                    <span style={{
                      width: '50px',
                      minWidth: '50px',
                      color: '#6e7681',
                      textAlign: 'right',
                      paddingRight: '1rem',
                      userSelect: 'none',
                      fontSize: '0.875rem',
                    }}>
                      {i + 1}
                    </span>
                    <span style={{ flex: 1 }}>
                      {line.map((token, key) => (
                        <span key={key} {...getTokenProps({ token })} />
                      ))}
                    </span>
                  </div>
                ))}
              </pre>
            )}
          </Highlight>
        </div>
      )}

      <div style={{ marginTop: '3rem', textAlign: 'center', color: 'var(--text-tertiary)' }}>
        <p>共 {codeFiles.length} 个模块 · {filteredFiles.length} 个匹配</p>
      </div>
    </div>
  );
}
