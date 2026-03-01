#include "threadpool.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

typedef struct task_s {
    int id;
    void (*func)(void*);
    void *arg;
    threadpool_priority_t priority;
    struct task_s *next;
    bool completed;
    bool cancelled;
} task_t;

struct threadpool_s {
    pthread_mutex_t lock;
    pthread_cond_t  notify;
    pthread_cond_t  task_done;
    pthread_t      *threads;
    task_t         *task_head;
    task_t         *completed_head;
    int             thread_count;
    int             active_count;
    int             queue_size;
    int             completed_count;
    int             next_task_id;
    bool            shutdown;
    bool            paused;
};

static void* threadpool_worker(void *arg) {
    threadpool_t *pool = (threadpool_t*)arg;
    while (true) {
        pthread_mutex_lock(&(pool->lock));
        
        while ((pool->queue_size == 0 || pool->paused) && !pool->shutdown) {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }
        
        if (pool->shutdown && pool->queue_size == 0) {
            pthread_mutex_unlock(&(pool->lock));
            break;
        }

        task_t *task = pool->task_head;
        if (task && !task->cancelled) {
            pool->task_head = task->next;
            pool->queue_size--;
            pool->active_count++;
            pthread_mutex_unlock(&(pool->lock));
            
            (*(task->func))(task->arg);
            
            pthread_mutex_lock(&(pool->lock));
            pool->active_count--;
            pool->completed_count++;
            task->completed = true;
            task->next = pool->completed_head;
            pool->completed_head = task;
            pthread_cond_broadcast(&(pool->task_done));
            pthread_mutex_unlock(&(pool->lock));
        } else if (task && task->cancelled) {
            pool->task_head = task->next;
            pool->queue_size--;
            free(task);
            pthread_cond_broadcast(&(pool->task_done));
            pthread_mutex_unlock(&(pool->lock));
        } else {
            pthread_mutex_unlock(&(pool->lock));
        }
    }
    pthread_exit(NULL);
    return NULL;
}

threadpool_t* threadpool_create(int num_threads) {
    if (num_threads <= 0) {
        num_threads = sysconf(_SC_NPROCESSORS_ONLN);
        if (num_threads <= 0) num_threads = 1;
    }
    
    threadpool_t *pool = malloc(sizeof(threadpool_t));
    if (!pool) return NULL;
    
    memset(pool, 0, sizeof(threadpool_t));
    pool->thread_count = num_threads;
    pool->queue_size = 0;
    pool->task_head = NULL;
    pool->completed_head = NULL;
    pool->shutdown = false;
    pool->paused = false;
    pool->active_count = 0;
    pool->completed_count = 0;
    pool->next_task_id = 1;
    
    pool->threads = malloc(sizeof(pthread_t) * num_threads);
    if (!pool->threads) {
        free(pool);
        return NULL;
    }
    
    pthread_mutex_init(&(pool->lock), NULL);
    pthread_cond_init(&(pool->notify), NULL);
    pthread_cond_init(&(pool->task_done), NULL);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&(pool->threads[i]), NULL, threadpool_worker, (void*)pool);
    }
    
    return pool;
}

int threadpool_add_task(threadpool_t *pool, void (*func)(void*), void *arg) {
    return threadpool_add_task_with_priority(pool, func, arg, THREADPOOL_PRIORITY_NORMAL);
}

int threadpool_add_task_with_priority(threadpool_t *pool, void (*func)(void*), 
                                       void *arg, threadpool_priority_t priority) {
    if (!pool || !func) return 0;
    
    task_t *task = malloc(sizeof(task_t));
    if (!task) return 0;
    
    task->func = func;
    task->arg = arg;
    task->priority = priority;
    task->next = NULL;
    task->completed = false;
    task->cancelled = false;

    pthread_mutex_lock(&(pool->lock));
    task->id = pool->next_task_id++;
    
    if (pool->task_head == NULL) {
        pool->task_head = task;
    } else {
        task_t *curr = pool->task_head;
        task_t *prev = NULL;
        while (curr && curr->priority >= priority) {
            prev = curr;
            curr = curr->next;
        }
        if (prev) {
            task->next = curr;
            prev->next = task;
        } else {
            task->next = pool->task_head;
            pool->task_head = task;
        }
    }
    pool->queue_size++;
    pthread_cond_signal(&(pool->notify));
    pthread_mutex_unlock(&(pool->lock));
    
    return task->id;
}

int threadpool_add_task_with_callback(threadpool_t *pool, void *(*func)(void*), 
                                       void *arg, threadpool_result_cb callback) {
    (void)func;
    (void)arg;
    (void)callback;
    return 0;
}

bool threadpool_cancel_task(threadpool_t *pool, int task_id) {
    if (!pool || task_id <= 0) return false;
    
    pthread_mutex_lock(&(pool->lock));
    task_t *curr = pool->task_head;
    while (curr) {
        if (curr->id == task_id) {
            curr->cancelled = true;
            pthread_mutex_unlock(&(pool->lock));
            return true;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&(pool->lock));
    return false;
}

bool threadpool_wait_all(threadpool_t *pool, int timeout_ms) {
    if (!pool) return false;
    
    pthread_mutex_lock(&(pool->lock));
    if (timeout_ms < 0) {
        while (pool->queue_size > 0 || pool->active_count > 0) {
            pthread_cond_wait(&(pool->task_done), &(pool->lock));
        }
    } else {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }
        
        while (pool->queue_size > 0 || pool->active_count > 0) {
            int ret = pthread_cond_timedwait(&(pool->task_done), &(pool->lock), &ts);
            if (ret == ETIMEDOUT) {
                pthread_mutex_unlock(&(pool->lock));
                return false;
            }
        }
    }
    pthread_mutex_unlock(&(pool->lock));
    return true;
}

bool threadpool_wait_task(threadpool_t *pool, int task_id, int timeout_ms) {
    if (!pool || task_id <= 0) return false;
    
    pthread_mutex_lock(&(pool->lock));
    if (timeout_ms < 0) {
        while (true) {
            task_t *curr = pool->completed_head;
            while (curr) {
                if (curr->id == task_id) {
                    pthread_mutex_unlock(&(pool->lock));
                    return true;
                }
                curr = curr->next;
            }
            pthread_cond_wait(&(pool->task_done), &(pool->lock));
        }
    } else {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }
        
        while (true) {
            task_t *curr = pool->completed_head;
            while (curr) {
                if (curr->id == task_id) {
                    pthread_mutex_unlock(&(pool->lock));
                    return true;
                }
                curr = curr->next;
            }
            int ret = pthread_cond_timedwait(&(pool->task_done), &(pool->lock), &ts);
            if (ret == ETIMEDOUT) {
                pthread_mutex_unlock(&(pool->lock));
                return false;
            }
        }
    }
}

void threadpool_pause(threadpool_t *pool) {
    if (!pool) return;
    pthread_mutex_lock(&(pool->lock));
    pool->paused = true;
    pthread_mutex_unlock(&(pool->lock));
}

void threadpool_resume(threadpool_t *pool) {
    if (!pool) return;
    pthread_mutex_lock(&(pool->lock));
    pool->paused = false;
    pthread_cond_broadcast(&(pool->notify));
    pthread_mutex_unlock(&(pool->lock));
}

int threadpool_resize(threadpool_t *pool, int new_num_threads) {
    if (!pool || new_num_threads <= 0) return 0;
    
    pthread_mutex_lock(&(pool->lock));
    int old_count = pool->thread_count;
    
    // 只能减少线程数，不能增加线程数
    // 因为增加线程数需要重新分配线程数组，这会导致复杂的同步问题
    if (new_num_threads < old_count) {
        pool->thread_count = new_num_threads;
    }
    
    pthread_mutex_unlock(&(pool->lock));
    
    return pool->thread_count;
}

int threadpool_get_thread_count(const threadpool_t *pool) {
    if (!pool) return 0;
    return pool->thread_count;
}

int threadpool_get_active_count(const threadpool_t *pool) {
    if (!pool) return 0;
    return pool->active_count;
}

int threadpool_get_pending_count(const threadpool_t *pool) {
    if (!pool) return 0;
    return pool->queue_size;
}

int threadpool_get_completed_count(const threadpool_t *pool) {
    if (!pool) return 0;
    return pool->completed_count;
}

bool threadpool_is_paused(const threadpool_t *pool) {
    if (!pool) return false;
    return pool->paused;
}

bool threadpool_is_shutdown(const threadpool_t *pool) {
    if (!pool) return true;
    return pool->shutdown;
}

void threadpool_cleanup_completed(threadpool_t *pool) {
    if (!pool) return;
    
    pthread_mutex_lock(&(pool->lock));
    task_t *curr = pool->completed_head;
    while (curr) {
        task_t *next = curr->next;
        free(curr);
        curr = next;
    }
    pool->completed_head = NULL;
    pool->completed_count = 0;
    pthread_mutex_unlock(&(pool->lock));
}

void threadpool_destroy(threadpool_t *pool) {
    if (!pool) return;
    
    pthread_mutex_lock(&(pool->lock));
    pool->shutdown = true;
    pthread_cond_broadcast(&(pool->notify));
    pthread_cond_broadcast(&(pool->task_done));
    pthread_mutex_unlock(&(pool->lock));

    for (int i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    
    pthread_mutex_lock(&(pool->lock));
    task_t *curr = pool->task_head;
    while (curr) {
        task_t *next = curr->next;
        free(curr);
        curr = next;
    }
    pool->task_head = NULL;
    
    curr = pool->completed_head;
    while (curr) {
        task_t *next = curr->next;
        free(curr);
        curr = next;
    }
    pool->completed_head = NULL;
    pthread_mutex_unlock(&(pool->lock));
    
    free(pool->threads);
    pthread_mutex_destroy(&(pool->lock));
    pthread_cond_destroy(&(pool->notify));
    pthread_cond_destroy(&(pool->task_done));
    free(pool);
}
