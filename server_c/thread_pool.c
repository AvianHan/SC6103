// 线程池管理
// thread pool
#include "thread_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_THREADS 10
#define TASK_QUEUE_SIZE 50

typedef struct {
    void (*function)(void *);
    void *argument;
} Task;

typedef struct {
    Task task_queue[TASK_QUEUE_SIZE];
    int queue_size;
    int queue_front;
    int queue_rear;
    pthread_t threads[MAX_THREADS];
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
    pool.stop = 0;
    pthread_mutex_init(&pool.mutex, NULL);
    pthread_cond_init(&pool.cond, NULL);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&pool.threads[i], NULL, thread_worker, NULL);
    }
}

// 向线程池添加任务
void thread_pool_add_task(void (*function)(void *), void *arg) {
    pthread_mutex_lock(&pool.mutex);

    if (pool.queue_size == TASK_QUEUE_SIZE) {
        printf("Task queue is full, task cannot be added.\n");
        pthread_mutex_unlock(&pool.mutex);
        return;
    }

    Task task;
    task.function = function;
    task.argument = arg;
    pool.task_queue[pool.queue_rear] = task;
    pool.queue_rear = (pool.queue_rear + 1) % TASK_QUEUE_SIZE;
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

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(pool.threads[i], NULL);
    }

    pthread_mutex_destroy(&pool.mutex);
    pthread_cond_destroy(&pool.cond);
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
        pool.queue_front = (pool.queue_front + 1) % TASK_QUEUE_SIZE;
        pool.queue_size--;

        pthread_mutex_unlock(&pool.mutex);

        // 执行任务
        (*(task.function))(task.argument);
    }
    return NULL;
}