#include <stdio.h>
#include "../c_utils/tarjan_scc.h"

int main() {
    printf("=== Tarjan SCC Demo ===\n\n");
    
    // 测试基本Tarjan SCC算法
    printf("1. 基本Tarjan SCC算法测试:\n");
    tarjan_graph_t g;
    g.n = 8;
    
    // 初始化邻接表
    for (int i = 0; i < g.n; i++) {
        g.adj_size[i] = 0;
    }
    
    // 添加边
    g.adj[0][g.adj_size[0]++] = 1;
    g.adj[1][g.adj_size[1]++] = 2;
    g.adj[1][g.adj_size[1]++] = 4;
    g.adj[2][g.adj_size[2]++] = 3;
    g.adj[3][g.adj_size[3]++] = 2;
    g.adj[4][g.adj_size[4]++] = 5;
    g.adj[5][g.adj_size[5]++] = 6;
    g.adj[6][g.adj_size[6]++] = 4;
    g.adj[6][g.adj_size[6]++] = 7;
    
    int scc_map[TARJAN_MAX_NODES];
    int scc_count = tarjan_scc(&g, scc_map);
    
    printf("   强连通分量数量: %d\n", scc_count);
    printf("   节点所属SCC: ");
    for (int i = 0; i < g.n; i++) {
        printf("%d:%d ", i, scc_map[i]);
    }
    printf("\n");
    
    // 测试另一个图
    printf("\n2. 测试另一个图:\n");
    tarjan_graph_t g2;
    g2.n = 6;
    
    // 初始化邻接表
    for (int i = 0; i < g2.n; i++) {
        g2.adj_size[i] = 0;
    }
    
    // 添加边
    g2.adj[0][g2.adj_size[0]++] = 1;
    g2.adj[1][g2.adj_size[1]++] = 2;
    g2.adj[2][g2.adj_size[2]++] = 0;
    g2.adj[2][g2.adj_size[2]++] = 3;
    g2.adj[3][g2.adj_size[3]++] = 4;
    g2.adj[4][g2.adj_size[4]++] = 5;
    g2.adj[5][g2.adj_size[5]++] = 3;
    
    int scc_map2[TARJAN_MAX_NODES];
    int scc_count2 = tarjan_scc(&g2, scc_map2);
    
    printf("   强连通分量数量: %d\n", scc_count2);
    printf("   节点所属SCC: ");
    for (int i = 0; i < g2.n; i++) {
        printf("%d:%d ", i, scc_map2[i]);
    }
    printf("\n");
    
    // 测试单个节点的图
    printf("\n3. 测试单个节点的图:\n");
    tarjan_graph_t g3;
    g3.n = 1;
    g3.adj_size[0] = 0;
    
    int scc_map3[TARJAN_MAX_NODES];
    int scc_count3 = tarjan_scc(&g3, scc_map3);
    
    printf("   强连通分量数量: %d\n", scc_count3);
    printf("   节点所属SCC: %d:%d\n", 0, scc_map3[0]);
    
    // 测试没有边的图
    printf("\n4. 测试没有边的图:\n");
    tarjan_graph_t g4;
    g4.n = 4;
    
    // 初始化邻接表
    for (int i = 0; i < g4.n; i++) {
        g4.adj_size[i] = 0;
    }
    
    int scc_map4[TARJAN_MAX_NODES];
    int scc_count4 = tarjan_scc(&g4, scc_map4);
    
    printf("   强连通分量数量: %d\n", scc_count4);
    printf("   节点所属SCC: ");
    for (int i = 0; i < g4.n; i++) {
        printf("%d:%d ", i, scc_map4[i]);
    }
    printf("\n");
    
    printf("\n=== Tarjan SCC Demo 完成 ===\n");
    return 0;
}