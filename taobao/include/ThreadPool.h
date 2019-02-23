#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#define _DEF_LIMIT 10
#define TIME_OUT   3

class task_t
{
	public:
		void* (*task)(void*);
		void* arg;
};

class ThreadPool
{
	public:
		bool shutdown;//线程池开关

		int thread_max;//最大线程数
		int thread_min;//最小
		int thread_alive;//活着的线程数
		int thread_busy;//忙线程数
		int thread_kill;//要杀死的线程数
//任务队列
		int queue_max;//任务最大数
		int queue_head;//队列头
		int queue_tail;//队列尾
		int queue_cur;//当前位置
		task_t* task;//任务
//条件变量和锁
		pthread_cond_t not_empty;//不为空
		pthread_cond_t not_full;//不为满
		pthread_mutex_t lock_task;//用于任务的锁

		pthread_t* arr_pthread_tid;//线程id数组
		pthread_t manage_tid;//管理者线程id
	public:
		ThreadPool();
		~ThreadPool();
	public:
		bool InitThreadPool(int max,int min,int queue_max);
		bool DestroyThreadPool();
		bool Addtask(void* (*task)(void*),void* arg);
		static void* ConsumeTask(void*);
		bool if_thread_alive(pthread_t tid);
		static void* ManagerThreadPool(void*);
};
/*
ThreadPool* InitThreadPool(int max,int min,int queue_max);
bool DestroyThreadPool(ThreadPool*);
void* ManagerThreadPool(void*);
bool Addtask(ThreadPool*,void* (*task)(void*),void* arg);
void* ConsumeTask(void*);
bool if_thread_alive(pthread_t tid);
*/
#endif

