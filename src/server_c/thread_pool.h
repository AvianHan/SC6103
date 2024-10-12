// 线程池头文件
// thread pool
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

// 初始化线程池
void thread_pool_init(int num_threads);

// 向线程池添加任务
void thread_pool_add_task(void (*function)(void *), void *arg);

// 销毁线程池
void thread_pool_destroy();

#endif // THREAD_POOL_H