"""
静态预览服务器 — 模拟 GitHub Pages 的路由行为

用途：本地预览 Next.js 静态导出产物（out/）。

需要处理两个差异：
1. basePath 剥离：站点部署在 /blog 下，资源路径带该前缀，而 out/ 是其根
2. 扩展名补全：GitHub Pages 访问 /blog/poetry-atlas/work/xxx 时，
   实际返回的是 xxx.html（扁平文件），而非目录。python 内置 server 不会这样做，
   会造成目录列表干扰，故需显式补全。

用法：python preview_atlas.py [port]
"""

import http.server
import os
import socketserver
import sys

PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 4350
ROOT = os.path.dirname(os.path.abspath(__file__))


class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=os.path.join(ROOT, 'out'), **kwargs)

    def translate_path(self, path):
        # 1) 剥离 /blog 前缀
        if path.startswith('/blog'):
            path = path[5:] or '/'

        # 2) 去掉查询串
        path = path.split('?', 1)[0].split('#', 1)[0]

        # 3) 模拟 GitHub Pages：无扩展名的路径优先匹配同名 .html 文件
        fs_path = super().translate_path(path)
        if not os.path.isdir(fs_path):
            if not os.path.splitext(fs_path)[1] and os.path.isfile(fs_path + '.html'):
                return fs_path + '.html'

        # 4) 若命中 Next 的 RSC 目录（只含 __next.*.txt，无 index.html），
        #    回退到扁平 .html，避免出现目录列表
        if os.path.isdir(fs_path) and not os.path.isfile(os.path.join(fs_path, 'index.html')):
            candidate = fs_path.rstrip('/\\') + '.html'
            if os.path.isfile(candidate):
                return candidate

        return fs_path

    def log_message(self, fmt, *args):
        pass  # 静默


socketserver.TCPServer.allow_reuse_address = True
with socketserver.TCPServer(("127.0.0.1", PORT), Handler) as httpd:
    print(f"previewing out/ at http://127.0.0.1:{PORT}/blog/")
    httpd.serve_forever()
