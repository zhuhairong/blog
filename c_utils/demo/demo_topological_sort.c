#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "topological_sort.h"

void demo_legacy_api(void) {
    printf("\n=== 传统API拓扑排序演示 ===\n");
    
    int adj[10][10] = {0};
    int result[10] = {0};
    
    adj[5][0] = 1;
    adj[5][2] = 1;
    adj[4][0] = 1;
    adj[4][1] = 1;
    adj[2][3] = 1;
    adj[3][1] = 1;
    
    printf("邻接矩阵表示的图:\n");
    printf("  5->0, 5->2, 4->0, 4->1, 2->3, 3->1\n");
    
    bool success = topological_sort(6, adj, result);
    
    if (success) {
        printf("拓扑排序结果: ");
        for (int i = 0; i < 6; i++) {
            printf("%d ", result[i]);
        }
        printf("\n");
    } else {
        printf("拓扑排序失败（图中存在环）\n");
    }
}

void demo_cycle_detection(void) {
    printf("\n=== 环检测演示 ===\n");
    
    int adj[10][10] = {0};
    int result[10] = {0};
    
    adj[0][1] = 1;
    adj[1][2] = 1;
    adj[2][0] = 1;
    
    printf("邻接矩阵表示的图（包含环）:\n");
    printf("  0->1, 1->2, 2->0\n");
    
    bool success = topological_sort(3, adj, result);
    
    if (success) {
        printf("拓扑排序结果: ");
        for (int i = 0; i < 3; i++) {
            printf("%d ", result[i]);
        }
        printf("\n");
    } else {
        printf("检测到环，无法进行拓扑排序\n");
    }
}

void demo_task_scheduling(void) {
    printf("\n=== 任务调度场景演示 ===\n");
    
    int adj[10][10] = {0};
    int result[10] = {0};
    
    printf("任务依赖关系:\n");
    printf("  任务0: 无依赖\n");
    printf("  任务1: 依赖任务0\n");
    printf("  任务2: 依赖任务0\n");
    printf("  任务3: 依赖任务1和任务2\n");
    
    adj[0][1] = 1;
    adj[0][2] = 1;
    adj[1][3] = 1;
    adj[2][3] = 1;
    
    bool success = topological_sort(4, adj, result);
    
    if (success) {
        printf("任务执行顺序: ");
        for (int i = 0; i < 4; i++) {
            printf("任务%d ", result[i]);
        }
        printf("\n");
    } else {
        printf("任务调度失败（存在循环依赖）\n");
    }
}

void demo_course_prerequisites(void) {
    printf("\n=== 课程先修要求演示 ===\n");
    
    int adj[10][10] = {0};
    int result[10] = {0};
    
    printf("课程先修关系:\n");
    printf("  课程0: 数学基础（无先修）\n");
    printf("  课程1: 物理（需要数学基础）\n");
    printf("  课程2: 化学（需要数学基础）\n");
    printf("  课程3: 高级物理（需要物理和化学）\n");
    printf("  课程4: 材料科学（需要高级物理）\n");
    
    adj[0][1] = 1;
    adj[0][2] = 1;
    adj[1][3] = 1;
    adj[2][3] = 1;
    adj[3][4] = 1;
    
    bool success = topological_sort(5, adj, result);
    
    if (success) {
        printf("课程学习顺序: ");
        for (int i = 0; i < 5; i++) {
            printf("课程%d ", result[i]);
        }
        printf("\n");
    } else {
        printf("课程安排失败（存在循环依赖）\n");
    }
}

int main(void) {
    printf("========================================\n");
    printf("    拓扑排序演示程序\n");
    printf("========================================\n");
    
    demo_legacy_api();
    demo_cycle_detection();
    demo_task_scheduling();
    demo_course_prerequisites();
    
    printf("\n=== 拓扑排序演示完成 ===\n");
    return 0;
}
