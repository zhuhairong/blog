'use client';

import { useEffect, useRef } from 'react';
import styles from '@/app/page.module.css';

type Particle = {
    x: number;
    y: number;
    vx: number;
    vy: number;
    r: number;
    hue: number;
};

const HUES = [258, 275, 320, 200];

export default function HeroBackground() {
    const canvasRef = useRef<HTMLCanvasElement | null>(null);

    useEffect(() => {
        const canvas = canvasRef.current;
        if (!canvas) return;
        const ctx = canvas.getContext('2d');
        if (!ctx) return;

        const reduce = window.matchMedia('(prefers-reduced-motion: reduce)').matches;

        let w = 0;
        let h = 0;
        let raf = 0;
        let particles: Particle[] = [];
        const mouse = { x: -9999, y: -9999, active: false };

        const resize = () => {
            const rect = canvas.getBoundingClientRect();
            const dpr = Math.min(window.devicePixelRatio || 1, 2);
            w = rect.width;
            h = rect.height;
            canvas.width = Math.floor(w * dpr);
            canvas.height = Math.floor(h * dpr);
            ctx.setTransform(dpr, 0, 0, dpr, 0, 0);

            const count = Math.max(22, Math.min(80, Math.round((w * h) / 22000)));
            particles = Array.from({ length: count }, () => ({
                x: Math.random() * w,
                y: Math.random() * h,
                vx: (Math.random() - 0.5) * 0.2,
                vy: (Math.random() - 0.5) * 0.2,
                r: Math.random() * 1.6 + 0.8,
                hue: HUES[Math.floor(Math.random() * HUES.length)],
            }));
        };

        const onMove = (e: globalThis.MouseEvent) => {
            const rect = canvas.getBoundingClientRect();
            mouse.x = e.clientX - rect.left;
            mouse.y = e.clientY - rect.top;
            mouse.active = true;
        };

        const onLeave = () => {
            mouse.active = false;
            mouse.x = -9999;
            mouse.y = -9999;
        };

        const LINK = 120;
        const PULL = 170;

        const frame = () => {
            ctx.clearRect(0, 0, w, h);

            for (const p of particles) {
                if (!reduce) {
                    p.x += p.vx;
                    p.y += p.vy;
                }

                if (mouse.active) {
                    const dx = mouse.x - p.x;
                    const dy = mouse.y - p.y;
                    const d = Math.hypot(dx, dy);
                    if (d < PULL && d > 0.001) {
                        const f = (1 - d / PULL) * 0.22;
                        p.x += (dx / d) * f;
                        p.y += (dy / d) * f;
                    }
                }

                if (p.x < -20) p.x = w + 20;
                if (p.x > w + 20) p.x = -20;
                if (p.y < -20) p.y = h + 20;
                if (p.y > h + 20) p.y = -20;
            }

            ctx.lineWidth = 0.6;
            for (let i = 0; i < particles.length; i++) {
                for (let j = i + 1; j < particles.length; j++) {
                    const a = particles[i];
                    const b = particles[j];
                    const d = Math.hypot(a.x - b.x, a.y - b.y);
                    if (d < LINK) {
                        ctx.strokeStyle = `hsla(${a.hue}, 80%, 70%, ${(1 - d / LINK) * 0.22})`;
                        ctx.beginPath();
                        ctx.moveTo(a.x, a.y);
                        ctx.lineTo(b.x, b.y);
                        ctx.stroke();
                    }
                }
            }

            for (const p of particles) {
                let glow = 0;
                if (mouse.active) {
                    const d = Math.hypot(mouse.x - p.x, mouse.y - p.y);
                    if (d < PULL) glow = 1 - d / PULL;
                }

                const r = p.r + glow * 1.4;
                const g = ctx.createRadialGradient(p.x, p.y, 0, p.x, p.y, r * 5);
                g.addColorStop(0, `hsla(${p.hue}, 88%, ${74 + glow * 14}%, ${0.7 + glow * 0.3})`);
                g.addColorStop(1, `hsla(${p.hue}, 88%, 68%, 0)`);
                ctx.fillStyle = g;
                ctx.beginPath();
                ctx.arc(p.x, p.y, r * 5, 0, Math.PI * 2);
                ctx.fill();

                ctx.fillStyle = `hsla(${p.hue}, 92%, 88%, ${0.85 + glow * 0.15})`;
                ctx.beginPath();
                ctx.arc(p.x, p.y, r, 0, Math.PI * 2);
                ctx.fill();
            }

            raf = window.requestAnimationFrame(frame);
        };

        resize();
        frame();

        window.addEventListener('resize', resize);
        window.addEventListener('mousemove', onMove);
        document.addEventListener('mouseleave', onLeave);

        return () => {
            window.cancelAnimationFrame(raf);
            window.removeEventListener('resize', resize);
            window.removeEventListener('mousemove', onMove);
            document.removeEventListener('mouseleave', onLeave);
        };
    }, []);

    return <canvas ref={canvasRef} className={styles.heroCanvas} aria-hidden="true" />;
}
