#ifndef _PTHREAD_POOL_H_
#define _PTHREAD_POOL_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <errno.h>
#include <pthread.h>
// #include <semaphore.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>

#include <sys/time.h>
#include <sys/stat.h>

typedef  char u8;
typedef unsigned int u32;
typedef unsigned long int u64;

//函数信息结构体（节点）
struct task_info
{
	void *(*function)(void *arg);
	void *arg;
	struct task_info *next;
};


#define MAX_TASKS  500
#define MAX_PTHREAD  30
//线程池管理结构体
struct pthread_pool
{
	
	pthread_mutex_t  lock;				//互斥锁保护任务队列的删除和添加
	pthread_cond_t   cond;				//条件变量唤醒队列中的线程
	u8 			 shutdowan_flag;		//销毁线程池标志位
	
	u32 	 		 wait_tasks;		//任务队列中的任务个数
	u32				 active_pthreads;	//活跃的线程个数（执行任务的人数）

	struct task_info *task_list;		//任务队列的节点	
	pthread_t 		 *thread_tid;		//线程的tid存放地址
	
};

u8 pthread_pool_init(struct pthread_pool *pool,u32 active_pthread_num);

u8 add_task(struct pthread_pool *pool,void *(*new_function)(void *arg),void *new_arg);

u32 add_pthread(struct pthread_pool *pool,u32 add_num);

u32 remove_pthread(struct pthread_pool *pool,u32 remove_num);

u8 destroy_pthread_pool(struct pthread_pool *pool);


#endif
