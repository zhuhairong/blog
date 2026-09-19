'use client';

/**
 * 天地图真实瓦片底图 + 诗人创作地点叠加层。
 *
 * 本组件只在浏览器端加载（父组件用 dynamic(ssr:false) 引入），
 * 因为 Leaflet 在模块初始化阶段就会访问 window。
 */

import { useEffect, useRef } from 'react';
import L from 'leaflet';
import 'leaflet/dist/leaflet.css';
import type { DerivedMapPoint } from '@/poetry-atlas/types';
import {
  TIANDITU_KEY,
  TIANDITU_SUBDOMAINS,
  TIANDITU_ATTRIBUTION,
  TIANDITU_BASEMAPS,
  tiandituTileUrl,
} from './tiles';

interface Props {
  /** 已按精度过滤后的点 */
  points: DerivedMapPoint[];
  selected: string | null;
  onSelect: (placeId: string | null) => void;
  basemap: string;
  /** 底图反色（深色主题适配） */
  dark: boolean;
}

/** 精度 → 视觉权重，与旧自绘底图保持一致的语言 */
const PRECISION_STYLE: Record<DerivedMapPoint['precision'], { r: number; opacity: number }> = {
  site: { r: 7, opacity: 1 },
  township: { r: 6.5, opacity: 1 },
  county: { r: 6, opacity: 0.85 },
  prefecture: { r: 6, opacity: 0.8 },
  province: { r: 5.5, opacity: 0.7 },
  approximate: { r: 5.5, opacity: 0.55 },
};

/** 位置可靠（遗址/乡镇级）用绿色，中心点用琥珀色，其余用蓝色 */
function colorOf(p: DerivedMapPoint) {
  if (p.precision === 'site' || p.precision === 'township') return '#34d399';
  return p.isCentroid ? '#fbbf24' : '#38bdf8';
}

export default function TileAtlasMap({ points, selected, onSelect, basemap, dark }: Props) {
  const elRef = useRef<HTMLDivElement | null>(null);
  const mapRef = useRef<L.Map | null>(null);
  const baseRef = useRef<L.TileLayer | null>(null);
  const annoRef = useRef<L.TileLayer | null>(null);
  const overlayRef = useRef<L.LayerGroup | null>(null);
  const fittedRef = useRef(false);

  /* ── 初始化地图（只做一次） ───────────────────────────── */
  useEffect(() => {
    if (!elRef.current || mapRef.current) return;

    const map = L.map(elRef.current, {
      center: [33.5, 106],
      zoom: 4,
      // 天地图 _w 瓦片集最低为 1 级，全国视图用不到 0 级，下限设 3 更安全
      minZoom: 3,
      maxZoom: 17,
      zoomControl: true,
      attributionControl: true,
      // 不开启 worldCopyJump：避免横向重复世界导致的经度歧义
      worldCopyJump: false,
    });

    L.control.scale({ imperial: false, metric: true, position: 'bottomleft' }).addTo(map);
    mapRef.current = map;

    // 容器尺寸由 CSS aspect-ratio 决定，首帧可能尚未定型，尺寸变化后重算
    const ro = new ResizeObserver(() => map.invalidateSize());
    ro.observe(elRef.current);

    return () => {
      ro.disconnect();
      map.remove();
      mapRef.current = null;
      fittedRef.current = false;
    };
  }, []);

  /* ── 底图切换 ────────────────────────────────────────── */
  useEffect(() => {
    const map = mapRef.current;
    if (!map) return;

    const def = TIANDITU_BASEMAPS.find((b) => b.id === basemap) ?? TIANDITU_BASEMAPS[0];

    baseRef.current?.remove();
    annoRef.current?.remove();

    baseRef.current = L.tileLayer(tiandituTileUrl(def.layer, TIANDITU_KEY), {
      subdomains: TIANDITU_SUBDOMAINS,
      minZoom: 3,
      maxZoom: 17,
      attribution: TIANDITU_ATTRIBUTION,
    }).addTo(map);

    // 注记层单独一层压在底图之上，切换底图时同步换成配套注记
    annoRef.current = L.tileLayer(tiandituTileUrl(def.annotation, TIANDITU_KEY), {
      subdomains: TIANDITU_SUBDOMAINS,
      minZoom: 3,
      maxZoom: 17,
    }).addTo(map);
    annoRef.current.setZIndex(3);
  }, [basemap]);

  /* ── 点位叠加层 ──────────────────────────────────────── */
  useEffect(() => {
    const map = mapRef.current;
    if (!map) return;

    overlayRef.current?.remove();
    const group = L.layerGroup().addTo(map);

    for (const p of points) {
      const st = PRECISION_STYLE[p.precision];
      const color = colorOf(p);
      const isActive = p.placeId === selected;

      // 光晕：低透明度大圆，让点在卫星/地形底图上也能被看见
      L.circleMarker([p.coordinates[1], p.coordinates[0]], {
        radius: st.r * 3.4,
        stroke: false,
        fillColor: color,
        fillOpacity: isActive ? 0.28 : 0.14,
      }).addTo(group);

      // 中心点坐标：虚线圈，明确提示「行政区中心，非精确遗址」
      if (p.isCentroid) {
        L.circleMarker([p.coordinates[1], p.coordinates[0]], {
          radius: st.r + 5,
          color,
          weight: 1.2,
          dashArray: '3 4',
          fill: false,
          opacity: 0.6,
        }).addTo(group);
      }

      // 籍贯/出生地：外加一圈洋红环，体现「人」的维度
      if (p.isBirthplace) {
        L.circleMarker([p.coordinates[1], p.coordinates[0]], {
          radius: st.r + 3.5,
          color: '#f472b6',
          weight: 1.6,
          fill: false,
          opacity: 0.9,
        }).addTo(group);
      }

      const core = L.circleMarker([p.coordinates[1], p.coordinates[0]], {
        radius: isActive ? st.r + 2 : st.r,
        color: isActive ? '#ffffff' : color,
        weight: isActive ? 2 : 1,
        fillColor: color,
        fillOpacity: st.opacity,
      }).addTo(group);

      core.bindTooltip(
        `${p.historicalName}${p.workCount > 1 ? ` · ${p.workCount} 篇` : ''}`,
        { direction: 'top', offset: [0, -8], className: 'atlasTip', opacity: 0.95 },
      );
      core.on('click', () => onSelect(p.placeId));
    }

    group.setZIndex(10);
    overlayRef.current = group;
  }, [points, selected, onSelect]);

  /* ── 首帧自动缩放到全部点位 ──────────────────────────── */
  useEffect(() => {
    const map = mapRef.current;
    if (!map || fittedRef.current || points.length === 0) return;
    fittedRef.current = true;
    const bounds = L.latLngBounds(
      points.map((p) => [p.coordinates[1], p.coordinates[0]] as L.LatLngTuple),
    );
    map.fitBounds(bounds, { padding: [44, 44], maxZoom: 8 });
  }, [points]);

  return (
    <div
      ref={elRef}
      className={`atlasTileRoot${dark ? ' atlasTileDark' : ''}`}
      role="application"
      aria-label="诗人创作地分布地图（天地图底图）"
    />
  );
}
