#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>

#include "dijkstra.h"
#include "kruskal.h"
#include "prim.h"
#include "tarjan_scc.h"
#include "terminal.h"
#include "json.h"
#include "argparse.h"

#define MAX_NODES 50
#define MAX_EDGES 500
#define MAX_NAME_LEN 32
#define INFINITY_VAL 1000000

typedef struct {
    char name[MAX_NAME_LEN];
    int x, y;
} graph_node_t;

typedef struct {
    int from;
    int to;
    int weight;
} graph_edge_t;

typedef struct {
    graph_node_t nodes[MAX_NODES];
    int node_count;
    graph_edge_t edges[MAX_EDGES];
    int edge_count;
    bool directed;
} graph_t;

typedef struct {
    int step;
    char description[256];
    int visited[MAX_NODES];
    int visited_count;
    int current_node;
    int path[MAX_NODES];
    int path_count;
    int distances[MAX_NODES];
    int highlight_edge_from;
    int highlight_edge_to;
} algorithm_step_t;

typedef struct {
    algorithm_step_t *steps;
    int step_count;
    int step_capacity;
} algorithm_history_t;

static void graph_init(graph_t *g) {
    memset(g, 0, sizeof(graph_t));
    g->directed = false;
}

static int graph_find_node(graph_t *g, const char *name) {
    for (int i = 0; i < g->node_count; i++) {
        if (strcmp(g->nodes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static int graph_add_node(graph_t *g, const char *name) {
    if (g->node_count >= MAX_NODES) return -1;
    if (graph_find_node(g, name) >= 0) return -1;
    
    int idx = g->node_count++;
    strncpy(g->nodes[idx].name, name, MAX_NAME_LEN - 1);
    g->nodes[idx].x = idx % 5;
    g->nodes[idx].y = idx / 5;
    return idx;
}

static bool graph_add_edge(graph_t *g, int from, int to, int weight) {
    if (from < 0 || from >= g->node_count || to < 0 || to >= g->node_count) {
        return false;
    }
    if (g->edge_count >= MAX_EDGES) return false;
    
    g->edges[g->edge_count].from = from;
    g->edges[g->edge_count].to = to;
    g->edges[g->edge_count].weight = weight;
    g->edge_count++;
    return true;
}

static bool graph_load_json(graph_t *g, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return false;
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *content = malloc(size + 1);
    if (!content) {
        fclose(fp);
        return false;
    }
    
    fread(content, 1, size, fp);
    content[size] = '\0';
    fclose(fp);
    
    json_value_t *json = json_parse(content);
    free(content);
    
    if (!json || json->type != JSON_OBJECT) {
        if (json) json_free(json);
        return false;
    }
    
    graph_init(g);
    
    json_value_t *directed_val = json_object_get(json, "directed");
    if (directed_val && directed_val->type == JSON_BOOL) {
        g->directed = directed_val->u.boolean;
    }
    
    json_value_t *nodes_val = json_object_get(json, "nodes");
    if (nodes_val && nodes_val->type == JSON_ARRAY) {
        for (size_t i = 0; i < nodes_val->u.array.count; i++) {
            json_value_t *node = nodes_val->u.array.items[i];
            if (node->type == JSON_STRING) {
                graph_add_node(g, json_as_string(node));
            } else if (node->type == JSON_OBJECT) {
                json_value_t *name_val = json_object_get(node, "name");
                if (name_val && name_val->type == JSON_STRING) {
                    int idx = graph_add_node(g, json_as_string(name_val));
                    json_value_t *x_val = json_object_get(node, "x");
                    json_value_t *y_val = json_object_get(node, "y");
                    if (idx >= 0) {
                        if (x_val && x_val->type == JSON_NUMBER) {
                            g->nodes[idx].x = (int)x_val->u.number;
                        }
                        if (y_val && y_val->type == JSON_NUMBER) {
                            g->nodes[idx].y = (int)y_val->u.number;
                        }
                    }
                }
            }
        }
    }
    
    json_value_t *edges_val = json_object_get(json, "edges");
    if (edges_val && edges_val->type == JSON_ARRAY) {
        for (size_t i = 0; i < edges_val->u.array.count; i++) {
            json_value_t *edge = edges_val->u.array.items[i];
            if (edge->type == JSON_OBJECT) {
                json_value_t *from_val = json_object_get(edge, "from");
                json_value_t *to_val = json_object_get(edge, "to");
                json_value_t *weight_val = json_object_get(edge, "weight");
                
                int from_idx = -1, to_idx = -1, weight = 1;
                
                if (from_val && from_val->type == JSON_STRING) {
                    from_idx = graph_find_node(g, json_as_string(from_val));
                }
                if (to_val && to_val->type == JSON_STRING) {
                    to_idx = graph_find_node(g, json_as_string(to_val));
                }
                if (weight_val && weight_val->type == JSON_NUMBER) {
                    weight = (int)weight_val->u.number;
                }
                
                if (from_idx >= 0 && to_idx >= 0) {
                    graph_add_edge(g, from_idx, to_idx, weight);
                }
            }
        }
    }
    
    json_free(json);
    return true;
}

static void print_graph_ascii(graph_t *g, int *highlight_nodes, int highlight_count,
                              int highlight_edge_from, int highlight_edge_to) {
    printf("\n");
    term_printf(TERM_ANSI_CYAN, "图结构 (%s):\n", g->directed ? "有向图" : "无向图");
    term_printf(TERM_ANSI_CYAN, "────────────────────────────────────────────────────────────────\n");
    
    printf("节点: ");
    for (int i = 0; i < g->node_count; i++) {
        bool highlighted = false;
        for (int j = 0; j < highlight_count; j++) {
            if (highlight_nodes[j] == i) {
                highlighted = true;
                break;
            }
        }
        if (highlighted) {
            term_printf(TERM_ANSI_GREEN, "[%s] ", g->nodes[i].name);
        } else {
            printf("%s ", g->nodes[i].name);
        }
    }
    printf("\n\n边:\n");
    
    for (int i = 0; i < g->edge_count; i++) {
        graph_edge_t *e = &g->edges[i];
        bool highlight = (e->from == highlight_edge_from && e->to == highlight_edge_to) ||
                        (!g->directed && e->from == highlight_edge_to && e->to == highlight_edge_from);
        
        if (highlight) {
            term_printf(TERM_ANSI_YELLOW, "  %s --%d--> %s\n", 
                       g->nodes[e->from].name, e->weight, g->nodes[e->to].name);
        } else {
            printf("  %s --%d--> %s\n", 
                   g->nodes[e->from].name, e->weight, g->nodes[e->to].name);
        }
    }
    printf("\n");
}

static void print_distances(graph_t *g, int *dist, int src) {
    printf("\n距离表 (从 %s 出发):\n", g->nodes[src].name);
    printf("────────────────────────────────────────────────────────────────\n");
    for (int i = 0; i < g->node_count; i++) {
        if (dist[i] == INFINITY_VAL) {
            printf("  %s: ∞\n", g->nodes[i].name);
        } else {
            printf("  %s: %d\n", g->nodes[i].name, dist[i]);
        }
    }
}

static void print_path(graph_t *g, int *prev, int src, int dst) {
    if (prev[dst] == -1 && src != dst) {
        printf("无法到达 %s\n", g->nodes[dst].name);
        return;
    }
    
    int path[MAX_NODES];
    int path_len = 0;
    int current = dst;
    
    while (current != -1) {
        path[path_len++] = current;
        if (current == src) break;
        current = prev[current];
    }
    
    printf("路径: ");
    for (int i = path_len - 1; i >= 0; i--) {
        term_printf(TERM_ANSI_GREEN, "%s", g->nodes[path[i]].name);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
}

static void run_dijkstra(graph_t *g, int src, int dst, bool verbose) {
    printf("\n");
    term_printf(TERM_ANSI_CYAN, "════════════════════════════════════════════════════════════════\n");
    term_printf(TERM_ANSI_CYAN, "                    Dijkstra 最短路径算法                         \n");
    term_printf(TERM_ANSI_CYAN, "════════════════════════════════════════════════════════════════\n");
    
    int dist[MAX_NODES];
    int prev[MAX_NODES];
    bool visited[MAX_NODES] = {false};
    
    for (int i = 0; i < g->node_count; i++) {
        dist[i] = INFINITY_VAL;
        prev[i] = -1;
    }
    dist[src] = 0;
    
    if (verbose) {
        printf("\n初始化:\n");
        printf("  起点: %s, 终点: %s\n", g->nodes[src].name, g->nodes[dst].name);
        printf("  初始距离: %s = 0, 其他 = ∞\n", g->nodes[src].name);
    }
    
    for (int count = 0; count < g->node_count; count++) {
        int min_dist = INFINITY_VAL;
        int u = -1;
        
        for (int i = 0; i < g->node_count; i++) {
            if (!visited[i] && dist[i] < min_dist) {
                min_dist = dist[i];
                u = i;
            }
        }
        
        if (u == -1) break;
        visited[u] = true;
        
        if (verbose) {
            printf("\n步骤 %d: 选择节点 %s (距离: %d)\n", 
                   count + 1, g->nodes[u].name, dist[u]);
        }
        
        for (int e = 0; e < g->edge_count; e++) {
            graph_edge_t *edge = &g->edges[e];
            int v = -1;
            
            if (edge->from == u) v = edge->to;
            else if (!g->directed && edge->to == u) v = edge->from;
            
            if (v >= 0 && !visited[v]) {
                int new_dist = dist[u] + edge->weight;
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;
                    prev[v] = u;
                    
                    if (verbose) {
                        printf("  更新 %s: %d -> %d (通过 %s)\n", 
                               g->nodes[v].name, 
                               new_dist - edge->weight, new_dist, 
                               g->nodes[u].name);
                    }
                }
            }
        }
    }
    
    printf("\n结果:\n");
    printf("────────────────────────────────────────────────────────────────\n");
    
    if (verbose) {
        print_distances(g, dist, src);
    }
    
    printf("\n从 %s 到 %s:\n", g->nodes[src].name, g->nodes[dst].name);
    if (dist[dst] == INFINITY_VAL) {
        term_printf(TERM_ANSI_RED, "无法到达\n");
    } else {
        printf("  最短距离: ");
        term_printf(TERM_ANSI_GREEN, "%d\n", dist[dst]);
        printf("  ");
        print_path(g, prev, src, dst);
    }
    
    int highlight_nodes[2] = {src, dst};
    print_graph_ascii(g, highlight_nodes, 2, -1, -1);
}

static void run_kruskal(graph_t *g, bool verbose) {
    printf("\n");
    term_printf(TERM_ANSI_CYAN, "════════════════════════════════════════════════════════════════\n");
    term_printf(TERM_ANSI_CYAN, "                    Kruskal 最小生成树算法                        \n");
    term_printf(TERM_ANSI_CYAN, "════════════════════════════════════════════════════════════════\n");
    
    kruskal_edge_t *edges = malloc(g->edge_count * sizeof(kruskal_edge_t));
    if (!edges) {
        term_printf(TERM_ANSI_RED, "内存分配失败\n");
        return;
    }
    
    for (int i = 0; i < g->edge_count; i++) {
        edges[i].u = g->edges[i].from;
        edges[i].v = g->edges[i].to;
        edges[i].w = g->edges[i].weight;
    }
    
    int total_weight = kruskal_mst(g->node_count, g->edge_count, edges);
    free(edges);
    
    if (total_weight < 0) {
        term_printf(TERM_ANSI_RED, "算法执行失败: 图可能不连通\n");
        return;
    }
    
    printf("\n最小生成树:\n");
    printf("────────────────────────────────────────────────────────────────\n");
    printf("  总权重: ");
    term_printf(TERM_ANSI_GREEN, "%d\n", total_weight);
    
    print_graph_ascii(g, NULL, 0, -1, -1);
}

static void run_prim(graph_t *g, bool verbose) {
    printf("\n");
    term_printf(TERM_ANSI_CYAN, "════════════════════════════════════════════════════════════════\n");
    term_printf(TERM_ANSI_CYAN, "                    Prim 最小生成树算法                           \n");
    term_printf(TERM_ANSI_CYAN, "════════════════════════════════════════════════════════════════\n");
    
    prim_error_t error;
    prim_config_t config = prim_default_config();
    config.compute_edges = true;
    
    prim_graph_t *pg = prim_graph_create(g->node_count, &config, &error);
    if (!pg) {
        term_printf(TERM_ANSI_RED, "创建图失败\n");
        return;
    }
    
    for (int i = 0; i < g->edge_count; i++) {
        prim_graph_add_edge(pg, g->edges[i].from, g->edges[i].to, 
                           g->edges[i].weight, &error);
        if (!g->directed) {
            prim_graph_add_edge(pg, g->edges[i].to, g->edges[i].from, 
                               g->edges[i].weight, &error);
        }
    }
    
    prim_result_t result;
    if (prim_mst_ex(pg, &result, &error)) {
        printf("\n最小生成树:\n");
        printf("────────────────────────────────────────────────────────────────\n");
        printf("  总权重: ");
        term_printf(TERM_ANSI_GREEN, "%d\n", result.total_weight);
        printf("  边数: %zu\n", result.edge_count);
        printf("  连通性: %s\n", result.connected ? "连通" : "不连通");
        
        printf("\nMST边:\n");
        for (size_t i = 0; i < result.edge_count; i++) {
            prim_edge_t *e = &result.edges[i];
            printf("  %s --%d--> %s\n", 
                   g->nodes[e->src].name, e->weight, g->nodes[e->dest].name);
        }
        
        prim_result_free(&result);
    } else {
        term_printf(TERM_ANSI_RED, "算法执行失败\n");
    }
    
    prim_graph_destroy(pg);
}

static void run_tarjan(graph_t *g, bool verbose) {
    printf("\n");
    term_printf(TERM_ANSI_CYAN, "════════════════════════════════════════════════════════════════\n");
    term_printf(TERM_ANSI_CYAN, "                    Tarjan 强连通分量算法                         \n");
    term_printf(TERM_ANSI_CYAN, "════════════════════════════════════════════════════════════════\n");
    
    tarjan_graph_t tg;
    memset(&tg, 0, sizeof(tg));
    tg.n = g->node_count;
    
    for (int i = 0; i < g->edge_count; i++) {
        int from = g->edges[i].from;
        int to = g->edges[i].to;
        tg.adj[from][tg.adj_size[from]++] = to;
    }
    
    int scc_map[MAX_NODES];
    int scc_count = tarjan_scc(&tg, scc_map);
    
    printf("\n强连通分量数量: ");
    term_printf(TERM_ANSI_GREEN, "%d\n", scc_count);
    printf("────────────────────────────────────────────────────────────────\n");
    
    for (int scc = 0; scc < scc_count; scc++) {
        printf("\nSCC %d: ", scc + 1);
        for (int i = 0; i < g->node_count; i++) {
            if (scc_map[i] == scc) {
                term_printf(TERM_ANSI_GREEN, "%s ", g->nodes[i].name);
            }
        }
        printf("\n");
    }
    
    if (scc_count == 1) {
        printf("\n");
        term_printf(TERM_ANSI_GREEN, "图是强连通的\n");
    }
}

static void create_demo_graph(graph_t *g) {
    graph_init(g);
    
    const char *names[] = {"A", "B", "C", "D", "E", "F"};
    for (int i = 0; i < 6; i++) {
        graph_add_node(g, names[i]);
    }
    
    graph_add_edge(g, 0, 1, 4);
    graph_add_edge(g, 0, 2, 2);
    graph_add_edge(g, 1, 2, 1);
    graph_add_edge(g, 1, 3, 5);
    graph_add_edge(g, 2, 3, 8);
    graph_add_edge(g, 2, 4, 10);
    graph_add_edge(g, 3, 4, 2);
    graph_add_edge(g, 3, 5, 6);
    graph_add_edge(g, 4, 5, 3);
}

static void print_help(const char *prog) {
    term_printf(TERM_ANSI_CYAN, "\n图算法可视化工具 v1.0\n");
    term_printf(TERM_ANSI_CYAN, "════════════════════════════════════════════════════════════════\n\n");
    
    printf("用法: %s <command> [options]\n\n", prog);
    
    printf("命令:\n");
    printf("  demo        使用演示图运行算法\n");
    printf("  run         运行指定算法\n");
    printf("  info        显示图信息\n\n");
    
    printf("选项:\n");
    printf("  -a, --algorithm <name>   算法名称 (dijkstra, kruskal, prim, tarjan)\n");
    printf("  -s, --start <node>       起始节点 (用于Dijkstra)\n");
    printf("  -e, --end <node>         目标节点 (用于Dijkstra)\n");
    printf("  -f, --file <path>        图文件路径 (JSON格式)\n");
    printf("  -v, --verbose            详细输出\n");
    printf("  -h, --help               显示帮助信息\n\n");
    
    printf("示例:\n");
    printf("  %s demo                           # 使用演示图运行所有算法\n", prog);
    printf("  %s run -a dijkstra -s A -e E      # 运行Dijkstra算法\n", prog);
    printf("  %s run -a kruskal                 # 运行Kruskal算法\n", prog);
    printf("  %s run -f graph.json -a prim      # 从文件加载图并运行Prim算法\n", prog);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }
    
    const char *command = argv[1];
    
    if (strcmp(command, "-h") == 0 || strcmp(command, "--help") == 0) {
        print_help(argv[0]);
        return 0;
    }
    
    const char *algorithm = NULL;
    const char *start_node = "A";
    const char *end_node = "E";
    const char *graph_file = NULL;
    bool verbose = false;
    
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--algorithm") == 0) {
            if (i + 1 < argc) algorithm = argv[++i];
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--start") == 0) {
            if (i + 1 < argc) start_node = argv[++i];
        } else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--end") == 0) {
            if (i + 1 < argc) end_node = argv[++i];
        } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if (i + 1 < argc) graph_file = argv[++i];
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        }
    }
    
    graph_t graph;
    
    if (graph_file) {
        if (!graph_load_json(&graph, graph_file)) {
            fprintf(stderr, "错误: 无法加载图文件 %s\n", graph_file);
            return 1;
        }
        printf("已加载图: %s\n", graph_file);
    } else {
        create_demo_graph(&graph);
        printf("使用演示图\n");
    }
    
    printf("\n");
    term_printf(TERM_ANSI_CYAN, "════════════════════════════════════════════════════════════════\n");
    term_printf(TERM_ANSI_CYAN, "                    图算法可视化工具 v1.0                         \n");
    term_printf(TERM_ANSI_CYAN, "════════════════════════════════════════════════════════════════\n");
    
    print_graph_ascii(&graph, NULL, 0, -1, -1);
    
    if (strcmp(command, "demo") == 0) {
        int src = graph_find_node(&graph, start_node);
        int dst = graph_find_node(&graph, end_node);
        
        if (src >= 0 && dst >= 0) {
            run_dijkstra(&graph, src, dst, verbose);
        }
        run_kruskal(&graph, verbose);
        run_prim(&graph, verbose);
        run_tarjan(&graph, verbose);
        
    } else if (strcmp(command, "run") == 0) {
        if (!algorithm) {
            fprintf(stderr, "错误: 请指定算法名称 (-a)\n");
            return 1;
        }
        
        if (strcmp(algorithm, "dijkstra") == 0) {
            int src = graph_find_node(&graph, start_node);
            int dst = graph_find_node(&graph, end_node);
            
            if (src < 0) {
                fprintf(stderr, "错误: 找不到起始节点 %s\n", start_node);
                return 1;
            }
            if (dst < 0) {
                fprintf(stderr, "错误: 找不到目标节点 %s\n", end_node);
                return 1;
            }
            
            run_dijkstra(&graph, src, dst, verbose);
            
        } else if (strcmp(algorithm, "kruskal") == 0) {
            run_kruskal(&graph, verbose);
            
        } else if (strcmp(algorithm, "prim") == 0) {
            run_prim(&graph, verbose);
            
        } else if (strcmp(algorithm, "tarjan") == 0) {
            run_tarjan(&graph, verbose);
            
        } else {
            fprintf(stderr, "错误: 未知算法 '%s'\n", algorithm);
            return 1;
        }
        
    } else if (strcmp(command, "info") == 0) {
        printf("\n图信息:\n");
        printf("────────────────────────────────────────────────────────────────\n");
        printf("  类型: %s\n", graph.directed ? "有向图" : "无向图");
        printf("  节点数: %d\n", graph.node_count);
        printf("  边数: %d\n", graph.edge_count);
        
    } else {
        fprintf(stderr, "错误: 未知命令 '%s'\n", command);
        print_help(argv[0]);
        return 1;
    }
    
    return 0;
}
