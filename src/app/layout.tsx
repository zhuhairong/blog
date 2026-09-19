import type { Metadata, Viewport } from 'next';
import { Geist, Geist_Mono } from 'next/font/google';
import './globals.css';

const geistSans = Geist({
    variable: '--font-geist-sans',
    subsets: ['latin'],
    display: 'swap',
});

const geistMono = Geist_Mono({
    variable: '--font-geist-mono',
    subsets: ['latin'],
    display: 'swap',
});

export const metadata: Metadata = {
    title: "ZHR's AI Blog | 代码 · 学习 · 写作",
    description:
        'ZHR 的个人实验室：149 个 C 语言工具模块、初中英语与化学闯关学习工具、唐诗三百首，以及技术笔记与思考。',
    keywords: ['ZHR', '博客', 'C 语言', '学习工具', '唐诗', '英语单词', 'Next.js'],
    authors: [{ name: 'ZHR' }],
    openGraph: {
        title: "ZHR's AI Blog | 代码 · 学习 · 写作",
        description:
            '149 个 C 语言工具模块、初中英语与化学闯关学习工具、唐诗三百首，以及技术笔记与思考。',
        type: 'website',
    },
};

export const viewport: Viewport = {
    width: 'device-width',
    initialScale: 1,
    themeColor: '#08080d',
};

export default function RootLayout({
    children,
}: Readonly<{
    children: React.ReactNode;
}>) {
    return (
        <html lang="zh-CN">
            {/*
              关键首屏样式（Critical CSS）内联。

              背景：本项目是纯静态导出，样式全部走外部 CSS chunk。浏览器必须先下载
              完 CSS 才能正确渲染，在这段窗口期页面会用「浏览器默认样式」绘制 ——
              表现为白底黑字的一闪（FOUC），视觉上像「颜色主题失效」，刷新即恢复
              （第二次命中缓存）。实测产物中 <style> 块数为 0，即完全没有关键样式。

              这里只内联「首帧必需」的最小集合：根变量 + html/body 底色与字色。
              不含任何布局或组件样式，因此不会与外部 CSS 冲突（值完全一致），
              也不会随组件改动而失效。体积约 0.4KB，Gzip 后不足 0.2KB。
            */}
            <head>
                <style
                    dangerouslySetInnerHTML={{
                        __html: `:root{--bg-primary:#08080d;--bg-surface:#0e0e16;--text-primary:#f2f2f7;}html{background-color:#08080d;color-scheme:dark}body{margin:0;min-height:100vh;background-color:#08080d;color:#f2f2f7;}`,
                    }}
                />
            </head>
            <body className={`${geistSans.variable} ${geistMono.variable}`}>
                {children}
            </body>
        </html>
    );
}
