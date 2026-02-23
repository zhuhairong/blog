#include "astar.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ASTAR_DEFAULT_MAX_ITERATIONS 10000

// 简单优先队列实现
typedef struct {
    astar_node_t **nodes;
    size_t size;
    size_t capacity;
} astar_pq_t;

static astar_pq_t* pq_create(size_t capacity) {
    astar_pq_t *pq = malloc(sizeof(astar_pq_t));
    if (!pq) return NULL;
    pq->nodes = malloc(sizeof(astar_node_t*) * capacity);
    if (!pq->nodes) {
        free(pq);
        return NULL;
    }
    pq->size = 0;
    pq->capacity = capacity;
    return pq;
}

static void pq_free(astar_pq_t *pq) {
    if (!pq) return;
    free(pq->nodes);
    free(pq);
}

static void pq_push(astar_pq_t *pq, astar_node_t *node) {
    if (!pq || !node || pq->size >= pq->capacity) return;
    
    size_t i = pq->size++;
    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (pq->nodes[parent]->f <= node->f) break;
        pq->nodes[i] = pq->nodes[parent];
        i = parent;
    }
    pq->nodes[i] = node;
}

static astar_node_t* pq_pop(astar_pq_t *pq) {
    if (!pq || pq->size == 0) return NULL;
    
    astar_node_t *result = pq->nodes[0];
    astar_node_t *last = pq->nodes[--pq->size];
    
    size_t i = 0;
    while (true) {
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;
        size_t smallest = i;
        
        if (left < pq->size && pq->nodes[left]->f < pq->nodes[smallest]->f)
            smallest = left;
        if (right < pq->size && pq->nodes[right]->f < pq->nodes[smallest]->f)
            smallest = right;
        
        if (smallest == i) break;
        pq->nodes[i] = pq->nodes[smallest];
        i = smallest;
    }
    pq->nodes[i] = last;
    return result;
}

static bool pq_contains(astar_pq_t *pq, point_t pos) {
    if (!pq) return false;
    for (size_t i = 0; i < pq->size; i++) {
        if (pq->nodes[i]->pos.x == pos.x && pq->nodes[i]->pos.y == pos.y)
            return true;
    }
    return false;
}

// 节点哈希表（用于快速查找）
#define HASH_SIZE 1024

typedef struct node_entry_s {
    point_t pos;
    astar_node_t *node;
    struct node_entry_s *next;
} node_entry_t;

typedef struct {
    node_entry_t *buckets[HASH_SIZE];
} node_hash_t;

static unsigned int hash_point(point_t p) {
    return ((unsigned int)(p.x * 73856093) ^ (unsigned int)(p.y * 19349663)) % HASH_SIZE;
}

static node_hash_t* hash_create(void) {
    node_hash_t *h = calloc(1, sizeof(node_hash_t));
    return h;
}

static void hash_free(node_hash_t *h) {
    if (!h) return;
    for (int i = 0; i < HASH_SIZE; i++) {
        node_entry_t *entry = h->buckets[i];
        while (entry) {
            node_entry_t *next = entry->next;
            free(entry->node);
            free(entry);
            entry = next;
        }
    }
    free(h);
}

static astar_node_t* hash_get(node_hash_t *h, point_t pos) {
    if (!h) return NULL;
    unsigned int idx = hash_point(pos);
    node_entry_t *entry = h->buckets[idx];
    while (entry) {
        if (entry->pos.x == pos.x && entry->pos.y == pos.y)
            return entry->node;
        entry = entry->next;
    }
    return NULL;
}

static void hash_put(node_hash_t *h, point_t pos, astar_node_t *node) {
    if (!h) return;
    unsigned int idx = hash_point(pos);
    node_entry_t *entry = malloc(sizeof(node_entry_t));
    if (!entry) return;
    entry->pos = pos;
    entry->node = node;
    entry->next = h->buckets[idx];
    h->buckets[idx] = entry;
}

// 启发函数实现
int astar_heuristic_manhattan(point_t a, point_t b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

int astar_heuristic_euclidean(point_t a, point_t b) {
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return (int)sqrt(dx * dx + dy * dy);
}

int astar_heuristic_diagonal(point_t a, point_t b) {
    int dx = abs(a.x - b.x);
    int dy = abs(a.y - b.y);
    int min = dx < dy ? dx : dy;
    int max = dx > dy ? dx : dy;
    return min * 14 + (max - min) * 10; // 对角线代价约1.4，直线代价1
}

int astar_heuristic_chebyshev(point_t a, point_t b) {
    int dx = abs(a.x - b.x);
    int dy = abs(a.y - b.y);
    return dx > dy ? dx : dy;
}

int astar_heuristic(point_t a, point_t b, astar_heuristic_type_t type) {
    switch (type) {
        case ASTAR_MANHATTAN: return astar_heuristic_manhattan(a, b);
        case ASTAR_EUCLIDEAN: return astar_heuristic_euclidean(a, b);
        case ASTAR_DIAGONAL:  return astar_heuristic_diagonal(a, b);
        case ASTAR_CHEBYSHEV: return astar_heuristic_chebyshev(a, b);
        default: return astar_heuristic_manhattan(a, b);
    }
}

// 创建结果
astar_result_t* astar_result_create(void) {
    astar_result_t *result = calloc(1, sizeof(astar_result_t));
    return result;
}

void astar_result_free(astar_result_t *result) {
    if (!result) return;
    free(result->path);
    free(result);
}

// 重建路径
static point_t* reconstruct_path(astar_node_t *goal_node, size_t *path_len) {
    // 计算路径长度
    size_t len = 0;
    astar_node_t *current = goal_node;
    while (current) {
        len++;
        current = current->parent;
    }
    
    point_t *path = malloc(sizeof(point_t) * len);
    if (!path) return NULL;
    
    current = goal_node;
    for (size_t i = len; i > 0; i--) {
        path[i - 1] = current->pos;
        current = current->parent;
    }
    
    *path_len = len;
    return path;
}

// 获取邻居（8方向）
static const int dx[8] = {0, 1, 1, 1, 0, -1, -1, -1};
static const int dy[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
static const int move_cost[8] = {10, 14, 10, 14, 10, 14, 10, 14};

// A* 搜索主函数
astar_result_t astar_search(
    void *world,
    point_t start,
    point_t goal,
    astar_is_walkable_fn is_walkable,
    astar_cost_fn cost_fn,
    astar_heuristic_type_t heuristic_type,
    int max_iterations
) {
    astar_result_t result = {0};
    
    if (!is_walkable) return result;
    if (max_iterations <= 0) max_iterations = ASTAR_DEFAULT_MAX_ITERATIONS;
    
    // 检查起点和终点是否可行走
    if (!is_walkable(world, start) || !is_walkable(world, goal)) {
        return result;
    }
    
    // 起点就是终点
    if (start.x == goal.x && start.y == goal.y) {
        result.found = true;
        result.path_len = 1;
        result.path = malloc(sizeof(point_t));
        if (result.path) {
            result.path[0] = start;
        }
        return result;
    }
    
    astar_pq_t *open_set = pq_create(1024);
    node_hash_t *all_nodes = hash_create();
    
    if (!open_set || !all_nodes) {
        pq_free(open_set);
        hash_free(all_nodes);
        return result;
    }
    
    // 创建起点节点
    astar_node_t *start_node = calloc(1, sizeof(astar_node_t));
    if (!start_node) {
        pq_free(open_set);
        hash_free(all_nodes);
        return result;
    }
    
    start_node->pos = start;
    start_node->g = 0;
    start_node->h = astar_heuristic(start, goal, heuristic_type);
    start_node->f = start_node->g + start_node->h;
    start_node->parent = NULL;
    
    pq_push(open_set, start_node);
    hash_put(all_nodes, start, start_node);
    
    int iterations = 0;
    
    while (open_set->size > 0 && iterations < max_iterations) {
        iterations++;
        
        astar_node_t *current = pq_pop(open_set);
        if (!current) break;
        
        result.nodes_expanded++;
        
        // 找到目标
        if (current->pos.x == goal.x && current->pos.y == goal.y) {
            result.found = true;
            result.path = reconstruct_path(current, &result.path_len);
            break;
        }
        
        // 扩展邻居
        for (int i = 0; i < 8; i++) {
            point_t neighbor_pos = {
                current->pos.x + dx[i],
                current->pos.y + dy[i]
            };
            
            if (!is_walkable(world, neighbor_pos)) continue;
            
            int tentative_g = current->g + (cost_fn ? cost_fn(world, current->pos, neighbor_pos) : move_cost[i]);
            
            astar_node_t *neighbor = hash_get(all_nodes, neighbor_pos);
            
            if (!neighbor) {
                neighbor = calloc(1, sizeof(astar_node_t));
                if (!neighbor) continue;
                
                neighbor->pos = neighbor_pos;
                neighbor->g = tentative_g;
                neighbor->h = astar_heuristic(neighbor_pos, goal, heuristic_type);
                neighbor->f = neighbor->g + neighbor->h;
                neighbor->parent = current;
                
                pq_push(open_set, neighbor);
                hash_put(all_nodes, neighbor_pos, neighbor);
            } else if (tentative_g < neighbor->g) {
                neighbor->g = tentative_g;
                neighbor->f = neighbor->g + neighbor->h;
                neighbor->parent = current;
            }
        }
    }
    
    pq_free(open_set);
    hash_free(all_nodes);
    
    return result;
}

// 简化版搜索
astar_result_t astar_search_simple(
    void *world,
    point_t start,
    point_t goal,
    astar_is_walkable_fn is_walkable,
    int width,
    int height
) {
    (void)width;
    (void)height;
    return astar_search(world, start, goal, is_walkable, NULL, ASTAR_MANHATTAN, 10000);
}

// 路径操作
void astar_result_reverse(astar_result_t *result) {
    if (!result || !result->path || result->path_len <= 1) return;
    
    for (size_t i = 0; i < result->path_len / 2; i++) {
        point_t temp = result->path[i];
        result->path[i] = result->path[result->path_len - 1 - i];
        result->path[result->path_len - 1 - i] = temp;
    }
}

bool astar_path_contains(const astar_result_t *result, point_t point) {
    if (!result || !result->path) return false;
    
    for (size_t i = 0; i < result->path_len; i++) {
        if (result->path[i].x == point.x && result->path[i].y == point.y)
            return true;
    }
    return false;
}
