#include "ThreadPool.h"


ThreadPool::ThreadPool()
{
	shutdown = false;

	thread_alive = 0;
	thread_busy = 0;
	thread_kill = 0;
	
	queue_head = 0;
	queue_tail = 0;
	queue_cur = 0;
}

ThreadPool::~ThreadPool()
{
	DestroyThreadPool();
}

bool ThreadPool::InitThreadPool(int max,int min,int queue_max)
{
//	if((pool = (ThreadPool*)malloc(sizeof(ThreadPool))) == NULL)
//		return NULL;

//	if((task = (task_t*)malloc(sizeof(task_t))) == NULL)
//		return NULL;

	int err;
	this->thread_max = max;
	this->thread_min = min;
	this->queue_max = queue_max;
	
	this->task = new task_t[queue_max];


	if(pthread_cond_init(&not_empty,NULL) != 0 || pthread_cond_init(&not_full,NULL) != 0 || pthread_mutex_init(&lock_task,NULL) != 0)
	{
		perror("pool->cond pool->mutex init faild");
		return false;
	}

	if((arr_pthread_tid = new pthread_t[max]) == NULL)
	{
		perror("pool->arr_pthread_tid init faild");
		return false;
	}
	bzero(arr_pthread_tid,sizeof(pthread_t)*max);
	//创建管理者线程
	manage_tid = 0;
	if((err = pthread_create(&manage_tid,NULL,ManagerThreadPool,(void*)this)) > 0)
	{
		printf("create pthread faild err = %s\n",strerror(err));
		return false;
	}
	//创建消费者线程
	for(int i=0;i<min;i++)
	{
		if((err = pthread_create(&arr_pthread_tid[i],NULL,ConsumeTask,(void*)this)) > 0)
		{
			printf("create pthread faild err = %s\n",strerror(err));
			return false;
		}
		++thread_alive;
	}
	return true;
}
bool ThreadPool::DestroyThreadPool()
{
	int err;
	pthread_mutex_lock(&lock_task);
	shutdown = true;
	pthread_mutex_unlock(&lock_task);

	for(int i=0;i<thread_alive;i++)
	{
		pthread_cond_signal(&not_empty);
	}
	sleep(TIME_OUT);

	if((err = pthread_mutex_destroy(&lock_task)) > 0)
		return false;

	if((err = pthread_cond_destroy(&not_full)) > 0)
		return false;

	if((pthread_cond_destroy(&not_empty)) > 0)
		return false;

	delete[] arr_pthread_tid;
	delete[] task;
	//free(pool);
	return true;
}
bool ThreadPool::Addtask(void* (*task)(void*),void* arg)
{
	if(task == NULL)
		return false;
	
	if(shutdown == true)
		return false;

	pthread_mutex_lock(&lock_task);
	//任务队列满了 阻塞在not_full的条件变量上
	if(queue_cur == queue_max)
		pthread_cond_wait(&not_full,&lock_task);

	if(shutdown == true)
	{
		pthread_mutex_unlock(&lock_task);
		return false;
	}

	this->task[queue_head].arg = arg;
	this->task[queue_head].task = task;
	queue_head = (queue_head + 1) % queue_max;
	++queue_cur;

	pthread_cond_signal(&not_empty);
	pthread_mutex_unlock(&lock_task);

	return true;
}
void* ThreadPool::ConsumeTask(void* arg)
{
	if(arg == NULL)
		return NULL;
	ThreadPool* pthis = (ThreadPool*)arg;
	task_t m_task;

   	while(pthis->shutdown != true)
   	{
		pthread_mutex_lock(&pthis->lock_task);
		if(pthis->queue_cur == 0)
			pthread_cond_wait(&pthis->not_empty,&pthis->lock_task);
		
		if(pthis->shutdown == true)
		{
			pthread_mutex_unlock(&pthis->lock_task);
			pthread_exit(NULL);
		}
		if((pthis->thread_kill > 0) && (pthis->thread_alive > pthis->thread_min))
		{
			--pthis->thread_alive;
			--pthis->thread_kill;
			pthread_mutex_unlock(&pthis->lock_task);
			pthread_exit(NULL);
		}

		m_task.task = pthis->task[pthis->queue_tail].task;
		m_task.arg = pthis->task[pthis->queue_tail].arg;

		pthis->queue_tail = (pthis->queue_tail + 1) % pthis->queue_max;
		--pthis->queue_cur;

		pthread_cond_signal(&pthis->not_full);
		++pthis->thread_busy;
		pthread_mutex_unlock(&pthis->lock_task);

		(*m_task.task)(m_task.arg);

		pthread_mutex_lock(&pthis->lock_task);
		--pthis->thread_busy;
		pthread_mutex_unlock(&pthis->lock_task);
   }
   return NULL;
}

bool ThreadPool::if_thread_alive(pthread_t tid)
{
	if((pthread_kill(tid,0)) == -1)
	{
		if(errno == ESRCH)
			return false;
	}
	return true;
}

void* ThreadPool::ManagerThreadPool(void* arg)
{
	if(arg == NULL)
		return NULL;
	ThreadPool* pthis = (ThreadPool*)arg;
	int alive;
	int busy;

	while(pthis->shutdown != true)
	{
		pthread_mutex_lock(&pthis->lock_task);
		alive = pthis->thread_alive;
		busy = pthis->thread_busy;
		pthread_mutex_unlock(&pthis->lock_task);
		//增加

		if(((float)busy / alive * 100 >= (float)70 ) && (pthis->thread_max >= alive + pthis->thread_min))
		{
			int add = 0;
			for(int i = 0;i<pthis->thread_max;i++)
			{
				pthread_mutex_lock(&pthis->lock_task);
				if(!(pthis->arr_pthread_tid[i]) || !pthis->if_thread_alive(pthis->arr_pthread_tid[i]))
				{
					if((pthread_create(&pthis->arr_pthread_tid[i],NULL,ConsumeTask,(void*)pthis)) >0)
						return NULL;
					++pthis->thread_alive;
					++add;
				}
				pthread_mutex_unlock(&pthis->lock_task);

				if(add == _DEF_LIMIT)
					break;
			}
		}
		//缩减
		if(busy * 2 < alive - busy && alive > pthis->thread_min)
		{
			pthread_mutex_lock(&pthis->lock_task);
			pthis->thread_kill = _DEF_LIMIT;
			pthread_mutex_unlock(&pthis->lock_task);
			for(int j = 0;j<_DEF_LIMIT;j++)
			{
				pthread_cond_signal(&pthis->not_empty);
			}
		}
		sleep(TIME_OUT);
	}
	return NULL;
}

