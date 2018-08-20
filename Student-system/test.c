#include<stdio.h>  
#include<curses.h>  
int main()  
{  
    char p[20];  
    printf("Please Input password:");  
    initscr();//该函数是启动curses模式，在此是关闭了终端回显，实现密码效果  
    scanf("%s",p); 
    endwin();//该函数是关闭curses模式。在此即恢复终端回显  
    printf("Your password is:%s\n",p);  
    return 0;  
}  