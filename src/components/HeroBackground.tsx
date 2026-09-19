'use client';

import { useEffect, useRef } from 'react';

/**
 * 交互式粒子星空背景：
 * - 粒子缓慢漂浮，临近粒子之间连线
 * - 鼠标移动时产生引力视差，并高亮附近粒子
 * - 支持 prefers-reduced-motion
 */
export default function HeroBackground() {
    const canvasRef = useRef<HTMLCanvasElement | null>(null);

    useEffect(() => {
        const canvas = canvasRef.current;
        if (!canvas) return;
        const ctx = canvas.getContext('2d');
        if (!ctx) return;

        const reduceMotion = window.matchMedia('(prefers-reduced-motion: reduce)').matches;

        let width = 0;
        let height = 0;
        let dpr = 1;
        let rafId = 0;

        const mouse = { x: -9999, y: -9999, active: false };

        type Particle = {
            x: number;
            y: number;
            vx: number;
            vy: number;
            r: number;
            hue: number;
        };
        let particles: Particle[] = [];

        const palette = [265, 285, 320, 205];

        const build = () => {
            const rect = canvas.getBoundingClientRect();
            dpr = Math.min(window.devicePixelRatio || 1, 2);
            width = rect.width;
            height = rect.height;
            canvas.width = Math.floor(width * dpr);
            canvas.height = Math.floor(height * dpr);
            ctx.setTransform(dpr, 0, 0, dpr, 0, 0);

            const density = Math.round((width * height) / 16000);
            const count = Math.max(28, Math.min(110, density));

            particles = Array.from({ length: count }, () => ({
                x: Math.random() * width,
                y: Math.random() * height,
                vx: (Math.random() - 0.5) * 0.22,
                vy: (Math.random() - 0.5) * 0.22,
                r: Math.random() * 1.8 + 0.7,
                hue: palette[Math.floor(Math.random() * palette.length)],
            }));
        };

        const onMouseMove = (e: MouseEvent) => {
            const rect = canvas.getBoundingClientRect();
            mouse.x = e.clientX - rect.left;
            mouse.y = e.clientY - rect.top;
            mouse.active = true;
        };
        const onMouseLeave = () => {
            mouse.active = false;
            mouse.x = -9999;
            mouse.y = -9999;
        };

        const linkDistance = 128;

        const draw = () => {
            ctx.clearRect(0, 0, width, height);

            for (const p of particles) {
                if (!reduceMotion) {
                    p.x += p.vx;
                    p.y += p.vy;
                }

                // 鼠标引力
                if (mouse.active) {
                    const dx = mouse.x - p.x;
                    const dy = mouse.y - p.y;
                    const dist = Math.hypot(dx, dy);
                    if (dist < 190 && dist > 0.001) {
                        const force = (1 - dist / 190) * 0.26;
                        p.x += (dx / dist) * force;
                        p.y += (dy / dist) * force;
                    }
                }

                // 边界回绕
                if (p.x < -20) p.x = width + 20;
                if (p.x > width + 20) p.x = -20;
                if (p.y < -20) p.y = height + 20;
                if (p.y > height + 20) p.y = -20;
            }

            // 连线
            ctx.lineWidth = 0.6;
            for (let i = 0; i < particles.length; i++) {
                for (let j = i + 1; j < particles.length; j++) {
                    const a = particles[i];
                    const b = particles[j];
                    const dx = a.x - b.x;
                    const dy = a.y - b.y;
                    const dist = Math.hypot(dx, dy);
                    if (dist < linkDistance) {
                        const alpha = (1 - dist / linkDistance) * 0.28;
                        ctx.strokeStyle = `hsla(${a.hue}, 85%, 68%, ${alpha})`;
                        ctx.beginPath();
                        ctx.moveTo(a.x, a.y);
                        ctx.lineTo(b.x, b.y);
                        ctx.stroke();
                    }
                }
            }

            // 粒子
            for (const p of particles) {
                let glow = 0;
                if (mouse.active) {
                    const dist = Math.hypot(mouse.x - p.x, mouse.y - p.y);
                    glow = dist < 190 ? 1 - dist / 190 : 0;
                }

                const radius = p.r + glow * 1.6;
                const grad = ctx.createRadialGradient(p.x, p.y, 0, p.x, p.y, radius * 5);
                grad.addColorStop(0, `hsla(${p.hue}, 92%, ${72 + glow * 18}%, ${0.75 + glow * 0.25})`);
                grad.addColorStop(1, `hsla(${p.hue}, 92%, 65%, 0)`);
                ctx.fillStyle = grad;
                ctx.beginPath();
                ctx.arc(p.x, p.y, radius * 5, 0, Math.PI * 2);
                ctx.fill();

                ctx.fillStyle = `hsla(${p.hue}, 95%, 88%, ${0.85 + glow * 0.15})`;
                ctx.beginPath();
                ctx.arc(p.x, p.y, radius, 0, Math.PI * 2);
                ctx.fill();
            }

            rafId = window.requestAnimationFrame(draw);
        };

        build();
        draw();

        window.addEventListener('resize', build);
        window.addEventListener('mousemove', onMouseMove);
        window.addEventListener('mouseleave', onMouseLeave);

        return () => {
            window.cancelAnimationFrame(rafId);
            window.removeEventListener('resize', build);
            window.removeEventListener('mousemove', onMouseMove);
            window.removeEventListener('mouseleave', onMouseLeave);
        };
    }, []);

    return <canvas ref={canvasRef} className="hero-canvas" aria-hidden="true" />;
}
