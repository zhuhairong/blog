import type { Metadata } from "next";
import { Geist, Geist_Mono } from "next/font/google";
import "./globals.css";

const geistSans = Geist({
  variable: "--font-geist-sans",
  subsets: ["latin"],
});

const geistMono = Geist_Mono({
  variable: "--font-geist-mono",
  subsets: ["latin"],
});

export const metadata: Metadata = {
  title: "ZHR's AI Blog | 代码 · 学习 · 写作",
  description:
    "ZHR 的个人实验室：149 个 C 语言工具模块、初中英语与化学闯关学习工具、唐诗三百首，以及技术笔记与思考。",
  keywords: ["ZHR", "博客", "C 语言", "学习工具", "唐诗", "英语单词", "Next.js"],
  authors: [{ name: "ZHR" }],
  openGraph: {
    title: "ZHR's AI Blog | 代码 · 学习 · 写作",
    description:
      "149 个 C 语言工具模块、初中英语与化学闯关学习工具、唐诗三百首，以及技术笔记与思考。",
    type: "website",
  },
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="zh-CN">
      <body className={`${geistSans.variable} ${geistMono.variable}`}>
        <main className="container" style={{ minHeight: '80vh' }}>
          {children}
        </main>
      </body>
    </html>
  );
}
