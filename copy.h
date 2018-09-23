#ifndef _COPY_H_
#define _COPY_H_

#include "pthread_pool.h"

struct dir_info
{
	u64 reg_filesize;
	u64 other_filesize;
	u32 dir_nums;
	u64 reg_nums;
	u64 other_nums;
	u8 name[4096];
};
//函数信息结构体（节点）
struct path_struct
{
	u8 src_buf[4096];
	u8 dest_buf[4096];
};

extern struct dir_info old_dir_info;
extern struct pthread_pool *pool;
extern	struct dir_info *_dir_info;
extern u64 end,start;
extern u64 fd_from_nums,fd_to_nums,dir_error_nums;
extern u8 finish_percent,finish_percent_copy;
extern struct timeval tv;
extern u8 errno_buf[4096];
u8 nums = 0;


void calculate_file_size(u8 src_buf[4096],struct dir_info *_dir_info)
{	
	
	struct stat src_state;
	stat(src_buf,&src_state);

	if(S_ISDIR(src_state.st_mode))
	{
		if(src_buf[strlen(src_buf)-1] == '/')
			src_buf[strlen(src_buf)-1] = '\0';
		else{
			// printf("src_buf:[%s] S_ISDIR:[%d]\n",src_buf,S_ISDIR(src_state.st_mode));
			_dir_info->dir_nums++;
		}

		DIR *dir_p = opendir( src_buf );
		struct dirent *dirent_p;
		u8 new_src[4096];	

		while( (dirent_p = readdir(dir_p))!=NULL )
		{			
			if(dirent_p->d_name[0] == '.'){
				continue;
			}
		
			bzero(new_src,4096);
			sprintf(new_src,"%s/%s",src_buf,dirent_p->d_name);
	
			if( dirent_p->d_type == 8 )
			{	
				struct stat file_state;
				stat(new_src,&file_state);
				_dir_info->reg_filesize += file_state.st_size;
				_dir_info->reg_nums++;			
			}
			else if( dirent_p->d_type == 4 )
			{	
				calculate_file_size(new_src,_dir_info);	

			}else
			{
				struct stat file_state;
				stat(new_src,&file_state);
				_dir_info->other_filesize += file_state.st_size;
				_dir_info->other_nums++;					
			}	
		}
		closedir(dir_p);
		free(dirent_p);
	}
	else if(S_ISREG(src_state.st_mode))
	{
		_dir_info->reg_nums++;
		_dir_info->reg_filesize += src_state.st_size;	
	}
	else
	{
		_dir_info->other_filesize += src_state.st_size;
		_dir_info->other_nums++;
	}

	return;
}

void *copy_file(void *arg)
{
	// pthread_mutex_lock(&pool->lock); 	//上锁(上不了锁就阻塞)
	struct path_struct *file_path =(struct path_struct *)arg;

	struct stat info;	stat(file_path->src_buf,&info);	

	int fd_from = open(file_path->src_buf,O_RDONLY);
	if(fd_from == -1)
	{
		fd_from_nums++;_dir_info->reg_nums--;strcpy(errno_buf,file_path->src_buf);
		printf("***1\n");
		while(1);
		return NULL;
	}
	int fd_to = open(file_path->dest_buf,O_RDWR|O_CREAT|O_TRUNC,info.st_mode);
	if(fd_to == -1)
	{
		fd_to_nums++;_dir_info->reg_nums--;strcpy(errno_buf,file_path->dest_buf);
		printf("***2\n");
		while(1);
		return NULL;
	}		

	
	strcpy(_dir_info->name,file_path->src_buf);
	
	system("clear");
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\t");
	finish_percent = finish_percent_copy = 100*((double)old_dir_info.reg_filesize - (double)_dir_info->reg_filesize) / (double)old_dir_info.reg_filesize;
	while(finish_percent_copy--)	{printf("■");}
	printf(" <%d%%>\n",finish_percent);
	printf("\n\t名称:%s\n",_dir_info->name);
	printf("\n\t线程数:%d\t任务数:%d\n",pool->active_pthreads,pool->wait_tasks);
	printf("\n\t时间:%2ld分%2ld秒\n",(end-start)/60,(end-start)%60);
	printf("\n\t剩余项目:%5ld (%3.1lf MB)\n"
		,_dir_info->reg_nums,((double)_dir_info->reg_filesize)/1024/1024);
	printf("\n\t打开失败文件数: %ld\t创建失败文件数: %ld\n",fd_from_nums,fd_to_nums);		
	printf("\n\t创建失败文件夹数: %ld\n",dir_error_nums);		
	printf("\n\n");

	char buf[4096];
	int nread,nwrite;
	
	while((nread = read(fd_from,buf,4096)) > 0)  //读取源文件的内容
	{
		if( write(fd_to,buf,nread) == -1)	  //把读到的全部写进目标文件
		{
			break;
		}
	}
	gettimeofday(&tv,NULL);end = tv.tv_sec;

	_dir_info->reg_nums--;
	_dir_info->reg_filesize -= info.st_size;	
	close(fd_to);
	close(fd_from);	
	
	// printf("file_path:[%s]          剩余项目：[%ld]\n",file_path->src_buf,_dir_info->reg_nums);

	// printf("\t***:[%ld]   [%ld]   \n",_dir_info->reg_filesize,old_dir_info.reg_filesize);
	// pthread_mutex_unlock(&pool->lock);	//解锁
	return NULL;
}

int copy_dir(struct path_struct *_path,struct pthread_pool *pool)
{
	struct stat info;	stat(_path->src_buf,&info);	
	
	/*********新建文件夹*********/ 
	if(mkdir(_path->dest_buf,info.st_mode) == -1)	dir_error_nums++;
	// _dir_info->dir_nums--;

	DIR *dir_p = opendir(_path->src_buf);
	struct dirent *dirent_p;
	/***************遍历源文件夹****************/	
	while( (dirent_p = readdir(dir_p))!=NULL )
	{		

		if(dirent_p->d_name[0] == '.'){
			continue;
		}

		struct path_struct *new_path = malloc(sizeof(struct path_struct));
		memset(new_path,0,sizeof(struct path_struct));	

		sprintf(new_path->src_buf,"%s/%s",_path->src_buf,dirent_p->d_name);//new_path->src_buf[length_src_buf] = '\0';;	
		sprintf(new_path->dest_buf,"%s/%s",_path->dest_buf,dirent_p->d_name);//new_path->dest_buf[length_dest_path] = '\0';
		//打印路径检查
		// printf("[%d]--->src_buf:[%s]   len:[%ld]   d_name:[%s]   len:[%ld]\n--->dest_buf:[%s]   len:[%ld]\n",kk,new_path->src_buf,strlen(new_path->src_buf),dirent_p->d_name,strlen(dirent_p->d_name),new_path->dest_buf,strlen(new_path->dest_buf));	
		
		struct stat tmpstat;
		stat(new_path->src_buf,&tmpstat);
		
		if(S_ISREG(tmpstat.st_mode))
		{	
			// printf("--->src_buf:[%s]   len:[%ld]   d_name:[%s]   len:[%ld]\n--->dest_buf:[%s]   len:[%ld]\n",new_path->src_buf,strlen(new_path->src_buf),dirent_p->d_name,strlen(dirent_p->d_name),new_path->dest_buf,strlen(new_path->dest_buf));	
			// printf("d_name:[%s]\n",dirent_p->d_name);
			//把复制的任务丢到任务链表
			if(add_task(pool,copy_file,new_path) == 0){
				add_pthread(pool, 1);
				// printf("----------------------add_pthread\n");	
				while(add_task(pool,copy_file,new_path) == 0){
					sleep(1);
				}
			}
	
		}	
		else if( S_ISDIR(tmpstat.st_mode) )
		{
			copy_dir(new_path,pool);			
		}else{
			_dir_info->other_nums--;
		}	
	}	
	closedir(dir_p);
	free(dirent_p);
	return 0;
}

#endif

