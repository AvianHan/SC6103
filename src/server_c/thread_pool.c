// 线程池管理
// thread_pool.c
#include <stdint.h>  // 添加这个头文件来定义 uint8_t 和 uint32_t
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


#define MAX_THREADS 10

typedef struct {
    void (*function)(void *);
    void *argument;
} Task;

typedef struct {
    Task *task_queue;
    int queue_size;
    int queue_front;
    int queue_rear;
    int queue_capacity;
    pthread_t *threads;
    int num_threads;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int stop;
} ThreadPool;

static ThreadPool pool;

void *thread_worker(void *arg);

// 初始化线程池
void thread_pool_init(int num_threads) {
    if (num_threads > MAX_THREADS) {
        num_threads = MAX_THREADS;
    }
    pool.queue_size = 0;
    pool.queue_front = 0;
    pool.queue_rear = 0;
    pool.queue_capacity = 100; // 默认任务队列容量为100
    pool.stop = 0;
    pool.num_threads = num_threads;
    
    pool.task_queue = (Task *)malloc(pool.queue_capacity * sizeof(Task));
    if (pool.task_queue == NULL) {
        perror("Failed to allocate memory for task queue");
        exit(EXIT_FAILURE);
    }

    pool.threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    if (pool.threads == NULL) {
        perror("Failed to allocate memory for threads");
        free(pool.task_queue);
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&pool.mutex, NULL);
    pthread_cond_init(&pool.cond, NULL);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&pool.threads[i], NULL, thread_worker, NULL);
    }
}

// 向线程池添加任务
void thread_pool_add_task(void (*function)(void *), void *arg) {
    pthread_mutex_lock(&pool.mutex);

    if (pool.queue_size == pool.queue_capacity) {
        printf("Task queue is full, task cannot be added.\n");
        pthread_mutex_unlock(&pool.mutex);
        return;
    }

    Task task;
    task.function = function;
    task.argument = arg;
    pool.task_queue[pool.queue_rear] = task;
    pool.queue_rear = (pool.queue_rear + 1) % pool.queue_capacity;
    pool.queue_size++;

    pthread_cond_signal(&pool.cond);
    pthread_mutex_unlock(&pool.mutex);
}

// 销毁线程池
void thread_pool_destroy() {
    pthread_mutex_lock(&pool.mutex);
    pool.stop = 1;
    pthread_cond_broadcast(&pool.cond);
    pthread_mutex_unlock(&pool.mutex);

    for (int i = 0; i < pool.num_threads; i++) {
        pthread_join(pool.threads[i], NULL);
    }

    pthread_mutex_destroy(&pool.mutex);
    pthread_cond_destroy(&pool.cond);
    
    free(pool.task_queue);
    pool.task_queue = NULL;

    free(pool.threads);
    pool.threads = NULL;
}

// 线程池中的线程执行的工作函数
void *thread_worker(void *arg) {
    while (1) {
        pthread_mutex_lock(&pool.mutex);

        while (pool.queue_size == 0 && !pool.stop) {
            pthread_cond_wait(&pool.cond, &pool.mutex);
        }

        if (pool.stop) {
            pthread_mutex_unlock(&pool.mutex);
            break;
        }

        Task task = pool.task_queue[pool.queue_front];
        pool.queue_front = (pool.queue_front + 1) % pool.queue_capacity;
        pool.queue_size--;

        pthread_mutex_unlock(&pool.mutex);

        // 执行任务
        (*(task.function))(task.argument);
    }
    return NULL;
}
