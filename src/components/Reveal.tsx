'use client';

import { useEffect, useRef, useState } from 'react';

type Props = {
    children: React.ReactNode;
    delay?: number;
};

/**
 * 元素进入视口时淡入上浮。
 * 通过 IntersectionObserver 订阅外部状态，回调中才 setState（符合 React 规则）。
 * 若环境不支持 IntersectionObserver，则直接以可见状态渲染，避免在 effect 中同步 setState。
 */
export default function Reveal({ children, delay = 0 }: Props) {
    const ref = useRef<HTMLDivElement | null>(null);
    const [supported] = useState(
        () => typeof window !== 'undefined' && typeof IntersectionObserver !== 'undefined'
    );
    const [visible, setVisible] = useState(false);

    useEffect(() => {
        if (!supported) return;

        const el = ref.current;
        if (!el) return;

        const observer = new IntersectionObserver(
            (entries) => {
                for (const entry of entries) {
                    if (entry.isIntersecting) {
                        setVisible(true);
                        observer.unobserve(entry.target);
                    }
                }
            },
            { threshold: 0.08, rootMargin: '0px 0px -40px 0px' }
        );

        observer.observe(el);
        return () => observer.disconnect();
    }, [supported]);

    return (
        <div
            ref={ref}
            className="reveal"
            data-visible={!supported || visible}
            style={delay ? { transitionDelay: `${delay}ms` } : undefined}
        >
            {children}
        </div>
    );
}
