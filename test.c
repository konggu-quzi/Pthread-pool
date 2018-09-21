#include "pthread_pool.h"


int main() //主函数
{
//直接调用mkdir函数
//建立一个名为zhidao的文件夹
//权限为0777，即拥有者权限为读、写、执行
//拥有者所在组的权限为读、写、执行
//其它用户的权限为读、写、执行
    // mkdir("../test2/test1",0777);
	//int fd_from = open("../进程和线程的区别.png",O_RDONLY|O_CREAT,0777);
	
	int fd_from = open("./进程和线程的区别.png",O_RDONLY,0777);
	u64 size = lseek(fd_from,0,SEEK_END);
	printf("size:%ld\n",size);
	lseek(fd_from,0,SEEK_SET);
	u8 buf[size];
	read(fd_from,buf,size);
	close(fd_from);

	int fd_to = open("../test2/进程和线程的区别.png",O_WRONLY|O_CREAT,0777);
	write(fd_to,buf,size);
	close(fd_to);
	printf("fd_from:%d\n",fd_from);printf("fd_to:%d\n",fd_to);
    return 0;
}