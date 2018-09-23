
#include "pthread_pool.h"

u32 old_active_pthreads;

void *get_task(void *arg)
{
	struct pthread_pool *pool = (struct pthread_pool *)arg;
	struct task_info 	*my_task_p = NULL;
	
	while(!pool->shutdowan_flag)
	{
		
		pthread_mutex_lock(&pool->lock); 	//上锁(上不了锁就阻塞)
		if(pool->wait_tasks == 0 && pool->shutdowan_flag == 0)
		{
			// printf("*******堵塞******\n");
			// printf("*******任务队列中任务的个数:%d******\n",pool->wait_tasks);
			pthread_cond_wait(&pool->cond,&pool->lock);	//进入条件变量等待队列
			pthread_mutex_unlock(&pool->lock);	//解锁
		}	
		else if(pool->shutdowan_flag != 1)
		{	
			// printf("*******运行******\n");
			my_task_p = pool->task_list->next;		
			pool->task_list->next = my_task_p->next;		
			my_task_p->next = NULL;
			pool->wait_tasks--;
			// printf("wait_tasks--:[%d]\n",pool->wait_tasks);
			pthread_mutex_unlock(&pool->lock);	//解锁
			//此时线程在执行函数，是否可以被取消？	
			(my_task_p->function)(my_task_p->arg);
			free(my_task_p->arg);
			// printf("****111****\n");	
			free(my_task_p);
			// printf("****222****\n");	
		}else
			pthread_mutex_unlock(&pool->lock);	//解锁
		
		pthread_mutex_lock(&pool->lock); 	//上锁(上不了锁就阻塞)
		if(old_active_pthreads != pool->active_pthreads)
		{
			
			old_active_pthreads--;
			// printf("我被选中删除了,old_active_pthreads:%d\n",old_active_pthreads);
			pthread_mutex_unlock(&pool->lock);	//解锁
			pthread_exit(NULL);//"我被选中删除了");
		}
		pthread_mutex_unlock(&pool->lock);	//解锁
		
		if(pool->shutdowan_flag)
		{
			//printf("*******退出*****\n");
			// pthread_mutex_unlock(&pool->lock);	//解锁
			pthread_exit(NULL);
		}
		
	}
	//printf("*******退出*****\n");
	pthread_mutex_unlock(&pool->lock);	//解锁
	pthread_exit(NULL);	
}

u8 pthread_pool_init(struct pthread_pool *pool,u32 active_pthread_num)
{	
	
	/************互斥锁初始化************/
	pthread_mutex_init(&pool->lock,NULL);	
	/***********条件变量初始化***********/
	pthread_cond_init(&pool->cond,NULL);
	
	pool->shutdowan_flag = 0;
	pool->wait_tasks = 0;
	pool->active_pthreads = old_active_pthreads = active_pthread_num;
	pool->task_list = malloc(sizeof(struct task_info));  //！！！！
	pool->task_list->next = NULL;

	/***********任务队列初始化***********/
	pool->thread_tid = malloc( active_pthread_num*sizeof(pthread_t) ); //申请活跃线程个数的空间
	int i = 0;
	while(i != active_pthread_num)
	{
		
		if( pthread_create( ((pool->thread_tid) + i) ,NULL,get_task,pool) )
			return 0;
		//printf("thread_tid[%d]:%ld\n",i, *(pool->thread_tid + i) );
		i++;	
	}
	// printf("*******初始化线程池成功******\n");
	return 1;
}

u8 add_task(struct pthread_pool *pool,void *(*new_function)(void *arg),void *new_arg)
{
	pthread_mutex_lock(&pool->lock); 	//上锁(上不了锁就阻塞)
	if(pool->wait_tasks >= MAX_TASKS){
		pthread_mutex_unlock(&pool->lock);	//解锁
		return 0;
	}
	
	struct task_info *new = malloc(sizeof(struct task_info));
		
	new->function = new_function;
	new->arg = new_arg;
	new->next = NULL;
		
	pool->wait_tasks++;
	// printf("wait_tasks:[%d]\n",pool->wait_tasks);
			
	struct task_info *cur =  pool->task_list;
	while(cur->next != NULL)
		cur = cur->next;

	cur->next = new;
	
	pthread_cond_signal(&pool->cond);//while(1);
	pthread_mutex_unlock(&pool->lock);	//解锁
	//printf("添加任务成功，当前任务数量:%d\n",pool->wait_tasks);	
	return 1;
}

u32 add_pthread(struct pthread_pool *pool,u32 add_num)
{
	pthread_mutex_lock(&pool->lock); 	//上锁(上不了锁就阻塞)
	if(pool->active_pthreads + add_num > MAX_PTHREAD){
		pthread_mutex_unlock(&pool->lock);	//解锁
		return 0;
	}	
	
	pthread_t *old_tid = pool->thread_tid;
	pool->thread_tid = malloc( (add_num + pool->active_pthreads)*sizeof(pthread_t) );
	
	int i = 0;	
	while(i != (pool->active_pthreads + add_num) )
	{
		if( i >= pool->active_pthreads)
		{
			if( pthread_create( ((pool->thread_tid) + i) ,NULL,get_task,pool) )
				return 0;
			//printf("thread_tid[%d]:%ld\n",i, *(pool->thread_tid + i) );
		}
		else
		{
			*(pool->thread_tid + i) = *(old_tid + i);
			//printf("thread_tid[%d]:%ld\n",i, *(pool->thread_tid + i) );
		}
		i++;
	}
	
	free(old_tid);
	pool->active_pthreads += add_num;
	old_active_pthreads = pool->active_pthreads;
	printf("*******添加%d个活跃线程成功******\n",add_num);
	pthread_mutex_unlock(&pool->lock);	//解锁
	return add_num;	
}

u32 remove_pthread(struct pthread_pool *pool,u32 remove_num)
{
	if(remove_num > (old_active_pthreads - 1)){
		printf("删除的活跃线程太多了\n");return 0;
	}
	
	u32 sum_pthreads = pool->active_pthreads;
	u32 old_pthreads = old_active_pthreads;
	pool->active_pthreads -= remove_num;
	do{
		pthread_cond_broadcast(&pool->cond);
		int i = 0;
		while(i != sum_pthreads)
		{
			if(*(pool->thread_tid + i) != 0)
			{
				//if( pthread_tryjoin_np(*(pool->thread_tid + i),NULL) == 0)	//&exit_buf);
				{
					//printf("thread_tid[%d]:%ld线程已经删除了！\n",i,*(pool->thread_tid + i) );
					*(pool->thread_tid + i) =  0;
					old_pthreads--;
				}	
			}	
			i++;
		}
	}while( old_pthreads !=  pool->active_pthreads );
	
	pthread_t *old_tid = pool->thread_tid;
	pool->thread_tid = malloc( (sum_pthreads - remove_num)*sizeof(pthread_t) );
	
	int i = 1;
	while(sum_pthreads--)
	{
		if(*(old_tid + sum_pthreads) != 0)
		{
			*(pool->thread_tid + old_active_pthreads - i) = *(old_tid + sum_pthreads);
			i++;
		}		
	}
	
	// i = 0;printf("剩余的线程为:\n");while(i != old_active_pthreads){printf("thread_tid[%d]:%ld线程\n",i,*(pool->thread_tid + i) );i++;}
	
	free(old_tid);	
	printf("*******删除%d个线程成功*******\n",remove_num);
	
}

u8 destroy_pthread_pool(struct pthread_pool *pool)
{	
	// printf("*******销毁线程池../.******\n");
	pool->shutdowan_flag = 1;
	pthread_cond_broadcast(&pool->cond);
	// printf("wait_tasks--:[%d]\n",pool->wait_tasks);
	while(pool->active_pthreads--)
	{
		// void *exit_buf;
		//printf("active_pthreads:%d\n",pool->active_pthreads);
		pthread_join(*(pool->thread_tid + pool->active_pthreads),NULL);//&exit_buf);
		// printf(" %ld 线程已经退出了！\n",*(pool->thread_tid + pool->active_pthreads));		
	}
	
	pthread_mutex_destroy(&pool->lock);
	pthread_cond_destroy(&pool->cond);
	

	free(pool->task_list);//printf("*******222*******\n");
	free(pool->thread_tid);//printf("*******333*******\n");
	free(pool);//printf("*******444*******\n");
	// printf("*******销毁线程池成功******\n");
	return 1;
}






