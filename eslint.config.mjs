import { defineConfig, globalIgnores } from "eslint/config";
import nextVitals from "eslint-config-next/core-web-vitals";
import nextTs from "eslint-config-next/typescript";

const eslintConfig = defineConfig([
  ...nextVitals,
  ...nextTs,
  // Override default ignores of eslint-config-next.
  globalIgnores([
    // Default ignores of eslint-config-next:
    ".next/**",
    "out/**",
    "build/**",
    "next-env.d.ts",
  ]),
  {
    rules: {
      // 本站点是 `output: 'export'` + `images.unoptimized: true` 的纯静态导出，
      // 没有服务端图片优化管线：next/image 在这种配置下最终就是渲染一个原生
      // `<img>`，不会带来任何 LCP 或体积收益，反而多一层运行时开销。
      // 站点内所有图片都已在构建前压成 WebP 并手工指定尺寸，所以这条规则
      // 在本项目是误报，显式关闭，避免长期挂着警告掩盖真实问题。
      "@next/next/no-img-element": "off",
    },
  },
]);

export default eslintConfig;
