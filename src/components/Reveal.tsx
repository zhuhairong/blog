'use client';

import { useEffect, useRef, useState, type ReactNode } from 'react';

type Props = {
    children: ReactNode;
    /** 交错延迟（毫秒） */
    delay?: number;
    className?: string;
};

/** 元素进入视口时淡入上浮 */
export default function Reveal({ children, delay = 0, className = '' }: Props) {
    const ref = useRef<HTMLDivElement | null>(null);
    const [visible, setVisible] = useState(false);

    useEffect(() => {
        const el = ref.current;
        if (!el) return;

        if (typeof IntersectionObserver === 'undefined') {
            setVisible(true);
            return;
        }

        const observer = new IntersectionObserver(
            (entries) => {
                for (const entry of entries) {
                    if (entry.isIntersecting) {
                        setVisible(true);
                        observer.unobserve(entry.target);
                    }
                }
            },
            { threshold: 0.12, rootMargin: '0px 0px -60px 0px' }
        );

        observer.observe(el);
        return () => observer.disconnect();
    }, []);

    return (
        <div
            ref={ref}
            className={`reveal${visible ? ' is-visible' : ''}${className ? ` ${className}` : ''}`}
            style={{ transitionDelay: `${delay}ms` }}
        >
            {children}
        </div>
    );
}
