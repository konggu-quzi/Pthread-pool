#include "pthread_pool.h"
#include "copy.h"


struct dir_info old_dir_info;
struct dir_info *_dir_info;
struct pthread_pool *pool;
u64 end,start;
u64 fd_from_nums = 0,fd_to_nums = 0,dir_error_nums = 0;
u8 errno_buf[4096];
struct timeval tv;
u8 finish_percent,finish_percent_copy;
		



void *real_time_display(void *arg)							
{

	struct dir_info *_dir_info = (struct dir_info *)arg;	
	u8 flag = 1;
	while(1)
	{	
		system("clear");
		printf("\n\n\n\n\n\n\n\n\n");
		printf("\t----------------------------------------\n");
		if(flag==1){
			printf("\t|        正在检索文件信息.             |\n");flag=2;
		}else if(flag==2){
			printf("\t|        正在检索文件信息..            |\n");flag=3;
		}else if(flag==3){
			printf("\t|        正在检索文件信息...           |\n");flag=4;
		}else if(flag==4){
			printf("\t|        正在检索文件信息....          |\n");flag=1;
		}	
		printf("\t----------------------------------------\n");
		printf("\t|           文件夹数量:%4d            |\n",_dir_info->dir_nums);
		printf("\t----------------------------------------\n");
		printf("\t|    类型    |   数量   |     大小     |\n");
		printf("\t----------------------------------------\n");
		printf("\t|  普通文件  |   %4ld   |   %7.1lf MB |\n",
			_dir_info->reg_nums,((double)_dir_info->reg_filesize)/1024/1024);
		printf("\t|  其他文件  |   %4ld   |   %7.1lf MB |\n",
			_dir_info->other_nums,((double)_dir_info->other_filesize)/1024/1024);
		printf("\t----------------------------------------\n");
		usleep(50000);usleep(50000);usleep(10000);
	}	
		
}

int main(int argc, char **argv)
{	
	if(argc != 3)
	{
		printf("Please run : ./%s xxx yyy\n",argv[0]);
		return -1;
	}
	
	system("clear");
	/*********线程池初始化*********/
	pool = malloc(sizeof(struct pthread_pool));
	if(!pthread_pool_init(pool,10))
	{
		perror("初始化线程池失败");
	}
	usleep(1000);
	/*********任务参数初始化*********/
	struct path_struct *path = malloc(sizeof(struct path_struct));
	struct path_struct *old_path = malloc(sizeof(struct path_struct));
	strcpy(old_path->src_buf,argv[1]);
	strcpy(old_path->dest_buf,argv[2]);
	
	strcpy(path->src_buf,argv[1]);
	/******修正目标地址******/
	if(argv[2][strlen(argv[2])-1] == '/')	argv[2][strlen(argv[2])-1] = '\0';
	if(path->src_buf[strlen(path->src_buf)-1] != '/')
	{
		u32 src_length= strlen(path->src_buf);
		while(src_length--){
			if(path->src_buf[src_length] == '/'){
				break;}}

		u32 dir_name_length = strlen(path->src_buf)-src_length-1;
		u8 dir_name[dir_name_length];		
		int i = 0;
		while( i != dir_name_length){
			dir_name[i] = path->src_buf[src_length+i+1];
			i++;
		}
		dir_name[i] = '\0';			
		sprintf(path->dest_buf,"%s/%s",argv[2],dir_name);
		
	}else{
		strcpy(path->dest_buf,argv[2]);
	}
	
	struct stat srcstat;
	stat(path->src_buf,&srcstat);
	
	pthread_t display_tid;
	_dir_info = malloc(sizeof(struct dir_info));
	memset(_dir_info,0,sizeof(struct dir_info));
	
	/********开始计时********/
	gettimeofday(&tv,NULL);start = tv.tv_sec;
	
	/********遍历文件********/
	pthread_create(&display_tid, NULL, real_time_display , _dir_info);
	calculate_file_size(path->src_buf,_dir_info);
	pthread_cancel(display_tid);	
	memcpy(&old_dir_info,_dir_info,sizeof(struct dir_info));
	
	/********开始复制********/
	if(S_ISREG(srcstat.st_mode))//如果为普通文件,则拷贝
	{
		//printf("1src_buf:[%s]   dest_buf:[%s]\n",path->src_buf,path->dest_buf);return 0;
		while(add_task(pool,copy_file,path) == 0);	
	}
	else if(S_ISDIR(srcstat.st_mode))//如果为目录，则递归
	{
		//printf("2src_buf:[%s]   dest_buf:[%s]\n",path->src_buf,path->dest_buf);return 0;
		copy_dir(path,pool);
	}
	/********复制完成标志********/
	// while(1);
	while(pool->wait_tasks != 0);
			
	destroy_pthread_pool(pool);
	gettimeofday(&tv,NULL);end = tv.tv_sec;

	// system("clear");
	printf("\n\n\n\n\n\n\n\n");
	printf("\t----------------------------------------\n");
	printf("\t|             Copy finish!             |\n");
	printf("\t----------------------------------------\n\n");
	printf("\t  cp -r %s %s\n\n",old_path->src_buf,old_path->dest_buf);	
	printf("\t----------------------------------------\n");
	printf("\t|            复制的文件信息            |\n");
	printf("\t----------------------------------------\n");
	printf("\t|           文件夹数量:%4d            |\n",old_dir_info.dir_nums);
	printf("\t----------------------------------------\n");
	printf("\t|    类型    |   数量   |     大小     |\n");
	printf("\t----------------------------------------\n");
	printf("\t|  普通文件  |   %4ld   |   %7.1lf MB |\n"
		,old_dir_info.reg_nums,(double)(old_dir_info.reg_filesize/1024)/1024);
	printf("\t|  其他文件  |   %4ld   |   %7.1lf MB |\n"
		,old_dir_info.other_nums,(double)(old_dir_info.other_filesize/1024)/1024);
	printf("\t----------------------------------------\n");printf("\n\t");//██	
	u8 finish_percent,finish_percent_copy;
	finish_percent = finish_percent_copy = 100*(double)((old_dir_info.reg_filesize - _dir_info->reg_filesize) / old_dir_info.reg_filesize);
	// if(_dir_info->reg_filesize == 0) finish_percent_copy = 100;
	while(finish_percent_copy--){printf("■");}
	printf(" <%d%%>\n",finish_percent);
	printf("\n\t打开失败文件数: %ld\t创建失败文件数: %ld\n",fd_from_nums,fd_to_nums);		
	printf("\n\t创建失败文件夹数: %ld\n",dir_error_nums);
	printf("\n\t复制失败文件: %s\n",errno_buf);

	// printf("\n\t***[%ld]   [%ld]   \n",_dir_info->reg_filesize,old_dir_info.reg_filesize);
	printf("\n\t花费时间:%2ld分%2ld秒\n\n\n\n\n\n",(end-start)/60,(end-start)%60);




}




