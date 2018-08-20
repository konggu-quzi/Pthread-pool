#include "Project.h"

//#if	test// #else// #endif

int main(void)
{
	
	u8 login_flag = 0,sign_up_flag = 0,go_in_system_flag,quit_flag,second_flag = 1;
	u8 buf[3];
	system("clear");
	//************************进入学生登录系统************************
	while(1)
	{
		
		printf("\n"CYAN);
		printf("              *****************************************\n");
		printf("              *                                       *\n");
		printf("              *         ~欢迎进入学生登录系统~        *\n");
		printf("              *                                       *\n");
		printf("              *      s:注册    i:登录   q:退出系统    *\n");
		printf("              *                                       *\n");
		printf("              *****************************************\n");
		printf("\n                         ");	
		
		fgets(buf,3,stdin);
		  
		if(buf[1] == '\n')	
		{
			switch(buf[0])
			{
				case 's':
					sign_up_flag = sign_up();system("clear");
					break;
				case 'i':
					login_flag = login_account();system("clear");
					break;
				case 'q':
					system("clear");
					return 0;
				default:printf(RED"\n              invalue input,Plz try again!\n"CYAN);
			}
		}
		else
		{
			if( buf[0] != '\n' && buf[1] != '\n')	while(getchar() != '\n');
			printf(RED"\n              invalue input,Plz try again!\n"CYAN);			
		}
	
		if(sign_up_flag || login_flag)
		{
			second_flag = 1;
			system("clear");
			sign_up_flag = login_flag = 0;
		}
		else
			second_flag = 0;

		//************************进入学生管理系统************************	
		while(second_flag)
		{		
			printf("\n"CYAN);
			printf("  **********************************************************************\n");
			printf("  *                                                                    *\n");
			printf("  *                       ~欢迎进入学生管理系统~                       *\n");
			printf("  *                                                                    *\n");
			printf("  *        w:学生信息录入    e:修改学生信息   r:显示班级学生信息       *\n");
			printf("  *        s:删除学生信息    d:查找学生信息   f:成绩排名               *\n");
			printf("  *                                                                    *\n");
			printf("  *                          q:退出登录                                *\n");
			printf("  *                                                                    *\n");
			printf("  **********************************************************************\n");
			printf("\n                              ");

			fgets(buf,3,stdin);
			if(buf[1] == '\n')	
			{
				//printf("%s",buf);
				switch(buf[0])
				{
					case 'w'://学生信息录入
						register_student_info();system("clear");
						break;
					case 'e'://修改学生信息
						rewrite_student_info();system("clear");
						break;
					case 'r'://显示学生信息
					    display_group_info();system("clear");
						break;
					case 's'://删除学生信息
						delet_student_info();system("clear");
						break;
					case 'd'://查找学生信息
						search_student_info();system("clear");
						break;
					case 'f'://成绩排名
						sort_student_score();system("clear");
						break;
					case 'q'://退出登录
						system("clear");
						second_flag = 0;
						break;	
					default:printf(RED"\n              invalue input,Plz try again!\n"CYAN);
				}
			}
			else
			{
				if( buf[0] != '\n' && buf[1] != '\n')	while(getchar() != '\n');
				printf(RED"\n              invalue input,Plz try again!\n"CYAN);			
			}
		}		
	}
	return 0;
}
