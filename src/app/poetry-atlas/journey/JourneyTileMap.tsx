'use client';

/**
 * 一生行迹地图 · 天地图真实瓦片版。
 *
 * 只在浏览器端加载（父组件用 dynamic(ssr:false) 引入），
 * 因为 Leaflet 在模块初始化阶段就会访问 window。
 *
 * 与「创作地分布」的瓦片图共用 `tiles.ts` 的底图配置与投影约定。
 */

import { useEffect, useRef } from 'react';
import L from 'leaflet';
import 'leaflet/dist/leaflet.css';
import type { JourneyStop } from '@/poetry-atlas/types';
import { TONE_UI } from '@/lib/atlas-view';
import {
  TIANDITU_KEY,
  TIANDITU_SUBDOMAINS,
  TIANDITU_ATTRIBUTION,
  TIANDITU_BASEMAPS,
  tiandituTileUrl,
} from '../tiles';
import { toneOf, radiusOf, groupStopsByCoordinate, displayedStopOf, nextStopInGroup, arcControl, arcAt, arcSamples, type Pt } from './journey-util';

interface Props {
  stops: JourneyStop[];
  selectedSeq: number | null;
  onSelect: (seq: number) => void;
  basemap: string;
  /** 底图反色（深色主题适配） */
  dark: boolean;
  playing: boolean;
}

/** 弧线外凸上限、箭头位置、放箭头的最小段长——与自绘版保持一致
 *  （自绘版按 viewBox 单位，这里换算成度：1000 单位约合 12°，故 30 单位 ≈ 0.36°） */
const ARC_MAX_DEG = 0.36;
const ARC_MIN_SEG_DEG = 0.36;
const ARROW_T = 0.58;

/**
 * 两点间的弧线，在**按纬度校正后的平面**里求控制点。
 *
 * 直接用经纬度当平面坐标会让弧往南北向偏——同一度数，经度对应的实际距离
 * 随纬度缩短。乘上 cos(纬度) 转成等距局部坐标再求，弧才是对称的。
 */
function flatArc(a: JourneyStop, b: JourneyStop) {
  const cosLat = Math.max(
    0.2,
    Math.cos((((a.coordinates[1] + b.coordinates[1]) / 2) * Math.PI) / 180),
  );
  const A: Pt = { x: a.coordinates[0] * cosLat, y: a.coordinates[1] };
  const B: Pt = { x: b.coordinates[0] * cosLat, y: b.coordinates[1] };
  const C = arcControl(A, B, ARC_MAX_DEG);
  const toLL = (p: Pt) => [p.y, p.x / cosLat] as L.LatLngTuple;
  return { A, B, C, toLL, segLen: Math.hypot(B.x - A.x, B.y - A.y) };
}

export default function JourneyTileMap({
  stops,
  selectedSeq,
  onSelect,
  basemap,
  dark,
  playing,
}: Props) {
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
      worldCopyJump: false,
    });

    L.control.scale({ imperial: false, metric: true, position: 'bottomleft' }).addTo(map);
    mapRef.current = map;

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

    annoRef.current = L.tileLayer(tiandituTileUrl(def.annotation, TIANDITU_KEY), {
      subdomains: TIANDITU_SUBDOMAINS,
      minZoom: 3,
      maxZoom: 17,
    }).addTo(map);
    annoRef.current.setZIndex(3);
  }, [basemap]);

  /* ── 行迹线与站点 ────────────────────────────────────── */
  useEffect(() => {
    const map = mapRef.current;
    if (!map) return;

    overlayRef.current?.remove();
    const group = L.layerGroup().addTo(map);

    const latlng = (s: JourneyStop) => [s.coordinates[1], s.coordinates[0]] as L.LatLngTuple;
    const live = stops.filter((s) => !s.posthumous);
    const after = stops.filter((s) => s.posthumous);
    const afterChain: JourneyStop[] = live.length ? [live[live.length - 1]!, ...after] : after;

    /** 一串站连成弧线（Leaflet 没有原生曲线，用采样折线近似） */
    const arcLatLngs = (chain: JourneyStop[]) => {
      const pts: L.LatLngTuple[] = [];
      for (let i = 1; i < chain.length; i++) {
        const { A, B, C, toLL } = flatArc(chain[i - 1]!, chain[i]!);
        const samples = arcSamples(A, C, B, 16);
        // 每段的首点与前一段末点重合，跳过以免重复
        for (let k = i === 1 ? 0 : 1; k < samples.length; k++) pts.push(toLL(samples[k]!));
      }
      return pts;
    };

    /** 弧线中点附近的箭头，朝向即时间方向 */
    const addArrows = (chain: JourneyStop[], extraClass: string) => {
      for (let i = 1; i < chain.length; i++) {
        const { A, B, C, toLL, segLen } = flatArc(chain[i - 1]!, chain[i]!);
        if (segLen < ARC_MIN_SEG_DEG) continue;
        const p = arcAt(A, C, B, ARROW_T);
        // 平面坐标里 y 向北，屏幕上向北是「上」，故屏幕角取负
        const deg = ((-p.angle * 180) / Math.PI).toFixed(1);
        const icon = L.divIcon({
          className: '',
          iconSize: [14, 12],
          iconAnchor: [7, 6],
          html:
            `<svg class="atlasJourneyArrow${extraClass}" width="14" height="12" viewBox="0 0 14 12"` +
            ` style="transform:rotate(${deg}deg)"><path d="M1 1.6 L13 6 L1 10.4 Z"/></svg>`,
        });
        L.marker(toLL(p), { icon, keyboard: false, interactive: false }).addTo(group);
      }
    };

    // 行迹线：先宽而淡的光晕，再叠实线，深浅底图上都看得见
    if (live.length > 1) {
      const line = arcLatLngs(live);
      L.polyline(line, { color: '#a78bfa', weight: 7, opacity: 0.18, lineJoin: 'round' }).addTo(group);
      L.polyline(line, { color: '#a78bfa', weight: 2, opacity: 0.92, lineJoin: 'round' }).addTo(group);
    }
    // 身后事件（迁葬、追谥）另用虚线接续
    if (afterChain.length > 1) {
      L.polyline(arcLatLngs(afterChain), {
        color: '#a78bfa',
        weight: 1.4,
        opacity: 0.5,
        dashArray: '5 5',
      }).addTo(group);
    }
    if (live.length > 1) addArrows(live, '');
    if (afterChain.length > 1) addArrows(afterChain, ' isAfter');

    // 站点：用 divIcon 承载序号，序号即行迹顺序。
    // 按坐标归组——同址多次驻留（苏轼三还眉山等）位置重合，逐个画会互相盖住。
    for (const g of groupStopsByCoordinate(stops)) {
      const s = displayedStopOf(g, selectedSeq);
      const color = TONE_UI[toneOf(s)].color;
      const r = radiusOf(s);
      const isActive = s.seq === selectedSeq;
      const size = r * 2;

      const icon = L.divIcon({
        className: '',
        iconSize: [size, size],
        iconAnchor: [r, r],
        html:
          `<span class="atlasJourneyPin${isActive ? ' isOn' : ''}${playing && isActive ? ' isPlaying' : ''}` +
          `${s.source === 'work' ? ' isWork' : ''}${g.length > 1 ? ' isMulti' : ''}"` +
          ` style="--pin:${color};width:${size}px;height:${size}px;font-size:${r >= 10 ? 9.5 : 8.5}px">` +
          `${s.seq}</span>`,
      });

      const mk = L.marker(latlng(s), { icon, keyboard: false });
      mk.bindTooltip(
        `${s.historicalName} · ${s.fromYear}–${s.toYear}` +
          (s.works.length ? ` · ${s.works.length} 篇` : '') +
          (g.length > 1 ? ` · 此地共 ${g.length} 站` : ''),
        { direction: 'top', offset: [0, -r - 4], className: 'atlasTip', opacity: 0.95 },
      );
      mk.on('click', () => onSelect(nextStopInGroup(g, selectedSeq)));
      mk.addTo(group);
    }

    group.setZIndex(10);
    overlayRef.current = group;
    // onSelect 是父级传下来的 setState（引用稳定），列入依赖不会造成反复重建
  }, [stops, selectedSeq, playing, onSelect]);

  /* ── 首帧自动缩放到全部站点 ──────────────────────────── */
  useEffect(() => {
    const map = mapRef.current;
    if (!map || fittedRef.current || stops.length === 0) return;
    fittedRef.current = true;
    const bounds = L.latLngBounds(
      stops.map((s) => [s.coordinates[1], s.coordinates[0]] as L.LatLngTuple),
    );
    map.fitBounds(bounds, { padding: [48, 48], maxZoom: 9 });
  }, [stops]);

  /* ── 选中站点时平滑移动视野 ──────────────────────────── */
  useEffect(() => {
    const map = mapRef.current;
    if (!map || selectedSeq === null) return;
    const s = stops.find((x) => x.seq === selectedSeq);
    if (!s) return;
    // 已在地图可视范围内就不动，避免每次点击都跳动
    if (map.getBounds().pad(-0.25).contains([s.coordinates[1], s.coordinates[0]])) return;
    map.panTo([s.coordinates[1], s.coordinates[0]], { animate: true, duration: 0.5 });
  }, [selectedSeq, stops]);

  return (
    <div
      ref={elRef}
      className={`atlasTileRoot${dark ? ' atlasTileDark' : ''}`}
      role="application"
      aria-label="诗人一生行迹地图（天地图底图）"
    />
  );
}
