#include "Project.h"
#include "kernel_list.h"




//**************************检查用户信息**************************
void examine_usr_info(u8 input[],u8 *status,u8 mix_size,u8 max_size)
{
	u8 count = 0;
	u32 length = strlen(input);
	*status = 0;
	
	if(length < mix_size|| input[mix_size-1] == '\n')		*status |= Too_short;	
	if( length == max_size)
	{
		if( input[length-1] != '\n')
		{
			if( getchar() != '\n' )
			{
				*status |= Too_Long;
				while(getchar() != '\n');
			}	
		}	
	}			
	while(count != length)
	{
		if(input[count] == ' ')
		{
			*status |= Space_exist;
			break;
		} 
		count++;
	}
	if( input[length-1] == '\n')
	{
		input[length-1] = '\0';
		// input[length] == ' ';
		// printf("%s%c%c|\n",input,input[length],input[length+1]);
		
	}

	length = strlen(input);
	if(length < max_size)
		memset( input + length + 1,' ',max_size - length);

	// printf("str:%s \n",input);	
	// printf("status:%d \n",*status);
	
}

//**************************打印账号错误**************************
void printf_usr_error(u8 error,u8 mode)
{
	//长度错误判断
	if((error & Too_Long) == Too_Long)
	{
		if(mode == 1)
		{
			printf(RED"  *你输入的账号长度过长\n"CYAN);
		}
		else if(mode == 2)
		{
			printf(RED"  *你输入的密码长度过长\n"CYAN);
		}			
	}
	if((error & Too_short) == Too_short)
	{
		if(mode == 1)
		{
			printf(RED"  *你输入的账号长度过短\n"CYAN);
		}
		else if(mode == 2)
		{
			printf(RED"  *你输入的密码长度过短\n"CYAN);
		}			
	}
	//空格错误判断
	if((error & Space_exist) == Space_exist)
	{
		if(mode == 1)
		{
			printf(RED"  *你输入的账号存在空格\n"CYAN);
		}
		else if(mode == 2)
		{
			printf(RED"  *你输入的密码存在空格\n"CYAN);
		}
		
	}
}
//**************************读取账户信息**************************
void read_usr_info(int fd,usr_info_p usr_info_p_buf[],u32 size)
{
	u8 i = 0;
	int nread;
	// printf("%ld   size:%d\n",sizeof(usr_info),size);	
	lseek(fd,0,SEEK_SET);
	while( i != size )
	{
		usr_info_p_buf[i] = (usr_info_p)malloc(sizeof(usr_info));
		nread = read(fd,usr_info_p_buf[i],sizeof(usr_info));
		// printf("strlen[%d]:%ld nread:%d\n",i,strlen(usr_info_p_buf[i]->account),nread);
		// printf("usr_info_p_buf[%d]->account:%s\n",i,usr_info_p_buf[i]->account);
		i++;
	}	
}
//**************************注册账号**************************
u8 sign_up()									
{
	system("clear");
	u8 error_account,error_password;
	u8 *error_account_p = &error_account,*error_password_p = &error_password;
		
	//printf("usr_info_p_buf[%d]->account:%s\n",1,usr_info_p_buf[1]->account);	
	while(1)
	{	
		usr_info input;
		int fd = open(USR_INFO,O_RDONLY | O_CREAT,0777);	
		u32 size = lseek(fd,0,SEEK_END)/sizeof(input);
		usr_info_p usr_info_p_buf[size];
		read_usr_info(fd,usr_info_p_buf,size);
		close(fd);
		


		memset(input.space,' ',sizeof(input.space));
		input.enter = '\n';
		
		printf("\n"CYAN);
		printf("  **********************************************************************\n");
		printf("  *                             ~注册账号~                (q:返回主界面)\n");	
		printf("  *请输入你的账号(6~9个字符):");	
		fgets(input.account,sizeof(input.account),stdin);		//获取输入的账号	
		if(!strcmp(input.account,"q\n")){return 0;}
		examine_usr_info(input.account,error_account_p,6,9);			//检查输入的账号
		printf("  *请输入你的密码(6~9个字符):");
		fgets(input.password,sizeof(input.password),stdin);		//获取输入的密码
		if(!strcmp(input.password,"q\n")){return 0;}
		examine_usr_info(input.password,error_password_p,6,9);			//检查输入的密码
		printf("  *\n  **********************************************************************\n  *\n");
		
		printf_usr_error(error_account,1);
		printf_usr_error(error_password,2);
		
		u8 exist_flag = 0;
		
		if(error_account == 0 && error_password == 0)
		{
	
			for(u8 i=0;i< size ;i++)
			{
				if(!strcmp(usr_info_p_buf[i]->account,input.account))
				{
					printf("  *你输入的账号已经存在\n");
					exist_flag = 1;
					break;
				}
			}

			while(size--)	free(usr_info_p_buf[size]);			//释放内存

			if(!exist_flag)
			{
				printf("  *注册成功\n");
				int fd = open(USR_INFO,O_WRONLY | O_CREAT | O_APPEND,0777);	
				write(fd,&input,sizeof(input));
				close(fd);
			}
		}

		u8 buf[3];
		while(1)
		{
			printf("  *\n  **********************************************************************\n");
			printf("  *                     g:继续注册     q:返回主界面~                    *\n");printf("  *");		
			fgets(buf,3,stdin);
			if(buf[0] == 'q')
				return 0;
			else if(buf[0] == 'g')
			{
				system("clear");
				break;
			}
			else
				printf("  *输入有误请重新输入\n");
			while(getchar() != '\n');
		}	
	}	
}
//**************************匹配账号**************************
u8 match_usr_info(usr_info input_from_usr,usr_info input_from_file)
{
	u8 account_flag,password_flag;
	if(strcmp(input_from_usr.account,input_from_file.account) == 0)
		account_flag = 1;
	else	
		account_flag = 0;

	if(strcmp(input_from_usr.password,input_from_file.password) == 0)
		password_flag = 1;
	else
		password_flag = 0;
		
	return	(account_flag  && password_flag);

}
//**************************登录账号**************************
u8 login_account()							
{
	system("clear");
	u8 error_account = 1,error_password = 1;
	u8 *error_account_p = &error_account,*error_password_p = &error_password;
	u8 match_flag,buf[2];
	
	usr_info input_from_usr,input_from_file;
	memset(input_from_usr.space,' ',sizeof(input_from_usr.space));
	input_from_usr.enter = '\n';		
	
    while(1)
	{
		while( !(error_account == 0 && error_password == 0) )
		{
			bzero(input_from_usr.account,sizeof(input_from_usr.account));
			bzero(input_from_usr.password,sizeof(input_from_usr.password));
			
			printf("\n"CYAN);
			printf("  **********************************************************************\n");
			printf("  *                             ~登录账号~                (q:返回主界面)\n");	
			printf("  *                    账号:");	
			fgets(input_from_usr.account,sizeof(input_from_usr.account),stdin);		//获取输入的账号
			if(!strcmp(input_from_usr.account,"q\n")){return 0;}
			examine_usr_info(input_from_usr.account,error_account_p,6,9);				//检查输入的账号
			printf("  *                    密码:");
			fgets(input_from_usr.password,sizeof(input_from_usr.password),stdin);		//获取输入的密码
			if(!strcmp(input_from_usr.password,"q\n")){return 0;}
			examine_usr_info(input_from_usr.password,error_password_p,6,9);			//检查输入的密码
			printf("  *\n  **********************************************************************\n");system("clear");printf("\n");
			
			printf_usr_error(error_account,1);
			printf_usr_error(error_password,2);
		}
		error_account = error_password = 1;
		int fd = open(USR_INFO,O_RDONLY,0777);
		off_t seek_cur = 0,seek_end = lseek(fd,0,SEEK_END);	
		
		lseek(fd,0,SEEK_SET);	
		do
		{
			read(fd,&input_from_file,sizeof(usr_info));
			seek_cur = lseek(fd,0,SEEK_CUR);
			// printf("%ld     %ld\n",strlen(input_from_file.account),strlen(input_from_file.password));
			// printf("%s|",input_from_file.account);
			// printf("%c|%c|%c|\n",input_from_file.account[7],input_from_file.account[8],input_from_file.account[9]);		
			match_flag = match_usr_info(input_from_usr,input_from_file);		
		}while( !(seek_end == seek_cur || match_flag == 1) );
		close(fd);
		
		if(match_flag)
		{
			system("clear");
			#if	test 	
			printf("\n"CYAN);
			printf("              ******************************************\n");
			printf("              *                                        *\n");
			printf("              *               ~登录成功~               *\n");
			printf("              *                                        *\n");
			printf("              ******************************************\n");
			printf("              ************ 3 秒后自动登录系统***********\n");
			printf("              *******************"RED);printf(" 3 "CYAN);printf("********************\n");
			sleep(1);
			printf("              *******************"RED);printf(" 2 "CYAN);printf("********************\n");
			sleep(1);
			printf("              *******************"RED);printf(" 1 "CYAN);printf("********************\n");
			sleep(1);
			system("clear");
			#else 
			#endif
			return 1;
		}
		else
			printf("  *你输入的账号密码不正确\n\n");			
		
		while(1)
		{
			printf("  **********************************************************************\n");
			printf("  *                     g:继续登录     q:返回主界面~                   *\n");printf("  *");		
			fgets(buf,3,stdin);
			if(buf[0] == 'q')
				return 0;
			else if(buf[0] == 'g')
			{
				system("clear");
				break;
			}
			else
				printf("  *输入有误请重新输入\n");
			while(getchar() != '\n');
		}
	}		
	return 1;
}
//**************************打印学生信息的错误**************************
void printf_usr_error_student_info(u32 error)
{
	u32 status_changes = 1;
	for(int i=0;i<11;i++)
	{
		if( error & ( status_changes << i ) )
		{
			switch(i)
			{
				case 0  :printf("  *学生学号输入格式错误\n");		break;
				case 1  :printf("  *学生姓名输入格式错误\n");		break;
				case 2  :printf("  *学生性别输入格式错误\n");		break;
				case 3  :printf("  *学生年级输入格式错误\n");		break;
				case 4  :printf("  *学生班级输入格式错误\n");		break;
				case 5  :printf("  *学生电话号码输入格式错误\n");	break;
				case 6  :printf("  *学生C语言成绩输入格式错误\n");	break;
				case 7  :printf("  *学生高数成绩输入格式错误\n");	break;
				case 8  :printf("  *学生英语成绩输入格式错误\n");	break;
				case 9  :printf("  *学生物理成绩输入格式错误\n");	break;
				case 10 :printf("  *学生模电成绩输入格式错误\n");	break;
				default:;
			}
		}		
	}	
}
//**************************学生节点初始化**************************
student_info *kernel_node_Init(void)
{
	student_info *p;
	
	p = (student_info *)malloc(sizeof(student_info));
	if(p == NULL)
	{
		printf("  *malloc failure \n");
		return NULL;
	}	
	//初始化大结构体，空格是为了显示整齐
	memset(p,' ',sizeof(student_info));
	p->enter = '\n';
	//初始化小结构体，让next prev指向自己
	INIT_LIST_HEAD(&p->list);

	return p;
}
//**************************插入节点**************************
void insert_node(struct list_head *list,student_info *new)
{
	list_add_tail(&new->list,list);	
}
//**************************查找节点**************************
student_info *check_node(u8 student_ID[],student_info *head)
{	
	//printf("用户输入的ID    ：%s\n",student_ID);

	struct list_head *pos;
	student_info *q;
	list_for_each(pos,&(head->list))
	{
		q = list_entry(pos, student_info , list);

		//printf("学生信息库中的ID：%s\n",q->student_ID);
		if(!strcmp(q->student_ID,student_ID))
		{
			//printf("学生信息库中相同中的ID：%s\n",q->student_ID);
			return q;
		}
	}
	return NULL;
}
//**************************删除节点**************************
void delet_node(student_info *delet)
{
	list_del(&delet->list);
	free(delet);
}
//**************************交换节点**************************
void exchange_node(struct list_head *prev,struct list_head *next)
{
	//exchange_node(&cur->list,&next->list);
	list_del(next);
	list_add_tail(next,prev);	
}
//************************显示学生节点信息*************************
void display_student_node(student_info node,u8 mode,u32 num)
{
	u32 length;
	//*********‘z’显示班级成员信息**************
	if(mode == 'z')
	{
		printf("| ");
		printf("%s",node.student_ID);
		length = sizeof(node.student_ID) - strlen(node.student_ID);
		while(length--)		
			printf(" ");
		printf("%s",node.name);
		length = sizeof(node.name) - strlen(node.name) -10;
		while(length--)		
			printf(" ");
		printf("%s",node.sex);
		length = sizeof(node.sex) - strlen(node.sex)+2;
		while(length--)		
			printf(" ");
		printf("%s",node.grade);
		length = sizeof(node.grade) - strlen(node.grade)+3;
		while(length--)		
			printf(" ");
		printf("%s",node.group);
		length = sizeof(node.group) - strlen(node.group)+2;
		while(length--)		
			printf(" ");
		printf("%s",node.phone_number);
		length = sizeof(node.phone_number) - strlen(node.phone_number)+4;
		while(length--)		
			printf(" ");
		printf("%s",node.C_sorce);
		length = sizeof(node.C_sorce) - strlen(node.C_sorce)+1;
		while(length--)		
			printf(" ");
		printf("%s",node.Mathematics_sorce);
		length = sizeof(node.Mathematics_sorce) - strlen(node.Mathematics_sorce)+1;
		while(length--)		
			printf(" ");
		printf("%s",node.English_sorce);
		length = sizeof(node.English_sorce) - strlen(node.English_sorce)+1;
		while(length--)		
			printf(" ");
		printf("%s",node.Physics_sorce);
		length = sizeof(node.Physics_sorce) - strlen(node.Physics_sorce)+1;
		while(length--)		
			printf(" ");
		printf("%s",node.Analog_sorce);
		length = sizeof(node.Analog_sorce) - strlen(node.Analog_sorce)+1;			
		printf("  |\n");
	}
	else if(mode == 'a')
	{
		printf("| ");
		printf("%s",node.student_ID);
		length = sizeof(node.student_ID) - strlen(node.student_ID);
		while(length--)		
			printf(" ");
		printf("%s",node.name);
		length = sizeof(node.name) - strlen(node.name) -10;
		while(length--)		
			printf(" ");
		printf("%s",node.sex);
		length = sizeof(node.sex) - strlen(node.sex)+2;
		while(length--)		
			printf(" ");
		printf("%s",node.grade);
		length = sizeof(node.grade) - strlen(node.grade)+3;
		while(length--)		
			printf(" ");
		printf("%s",node.group);
		length = sizeof(node.group) - strlen(node.group)+2;
		while(length--)		
			printf(" ");
		printf("%s",node.phone_number);
		length = sizeof(node.phone_number) - strlen(node.phone_number);
		while(length--)		
			printf(" ");
		printf("|        ");
		printf("%s",node.C_sorce);
		length = sizeof(node.C_sorce) - strlen(node.C_sorce) + 1 + 3;
		while(length--)		
			printf(" ");

		printf("%3d",num);printf("        |\n");
	}
	else if(mode == 'b')
	{
		printf("| ");
		printf("%s",node.student_ID);
		length = sizeof(node.student_ID) - strlen(node.student_ID);
		while(length--)		
			printf(" ");
		printf("%s",node.name);
		length = sizeof(node.name) - strlen(node.name) -10;
		while(length--)		
			printf(" ");
		printf("%s",node.sex);
		length = sizeof(node.sex) - strlen(node.sex)+2;
		while(length--)		
			printf(" ");
		printf("%s",node.grade);
		length = sizeof(node.grade) - strlen(node.grade)+3;
		while(length--)		
			printf(" ");
		printf("%s",node.group);
		length = sizeof(node.group) - strlen(node.group)+2;
		while(length--)		
			printf(" ");
		printf("%s",node.phone_number);
		length = sizeof(node.phone_number) - strlen(node.phone_number);
		while(length--)		
			printf(" ");
		printf("|        ");
		printf("%s",node.Mathematics_sorce);
		length = sizeof(node.Mathematics_sorce) - strlen(node.Mathematics_sorce) + 1 + 3;
		while(length--)		
			printf(" ");
		
		printf("%3d",num);printf("        |\n");
	}
	else if(mode == 'c')
	{
		printf("| ");
		printf("%s",node.student_ID);
		length = sizeof(node.student_ID) - strlen(node.student_ID);
		while(length--)		
			printf(" ");
		printf("%s",node.name);
		length = sizeof(node.name) - strlen(node.name) -10;
		while(length--)		
			printf(" ");
		printf("%s",node.sex);
		length = sizeof(node.sex) - strlen(node.sex)+2;
		while(length--)		
			printf(" ");
		printf("%s",node.grade);
		length = sizeof(node.grade) - strlen(node.grade)+3;
		while(length--)		
			printf(" ");
		printf("%s",node.group);
		length = sizeof(node.group) - strlen(node.group)+2;
		while(length--)		
			printf(" ");
		printf("%s",node.phone_number);
		length = sizeof(node.phone_number) - strlen(node.phone_number);
		while(length--)		
			printf(" ");
		printf("|        ");
		printf("%s",node.English_sorce);
		length = sizeof(node.English_sorce) - strlen(node.English_sorce) + 1 + 3;
		while(length--)		
			printf(" ");
		
		printf("%3d",num);printf("        |\n");
	}
	else if(mode == 'd')
	{
		printf("| ");
		printf("%s",node.student_ID);
		length = sizeof(node.student_ID) - strlen(node.student_ID);
		while(length--)		
			printf(" ");
		printf("%s",node.name);
		length = sizeof(node.name) - strlen(node.name) -10;
		while(length--)		
			printf(" ");
		printf("%s",node.sex);
		length = sizeof(node.sex) - strlen(node.sex)+2;
		while(length--)		
			printf(" ");
		printf("%s",node.grade);
		length = sizeof(node.grade) - strlen(node.grade)+3;
		while(length--)		
			printf(" ");
		printf("%s",node.group);
		length = sizeof(node.group) - strlen(node.group)+2;
		while(length--)		
			printf(" ");
		printf("%s",node.phone_number);
		length = sizeof(node.phone_number) - strlen(node.phone_number);
		while(length--)		
			printf(" ");
		printf("|        ");
		printf("%s",node.Physics_sorce);
		length = sizeof(node.Physics_sorce) - strlen(node.Physics_sorce) + 1 + 3;
		while(length--)		
			printf(" ");
		
		printf("%3d",num);printf("        |\n");
	}
	else if(mode == 'e')
	{
		printf("| ");
		printf("%s",node.student_ID);
		length = sizeof(node.student_ID) - strlen(node.student_ID);
		while(length--)		
			printf(" ");
		printf("%s",node.name);
		length = sizeof(node.name) - strlen(node.name) -10;
		while(length--)		
			printf(" ");
		printf("%s",node.sex);
		length = sizeof(node.sex) - strlen(node.sex)+2;
		while(length--)		
			printf(" ");
		printf("%s",node.grade);
		length = sizeof(node.grade) - strlen(node.grade)+3;
		while(length--)		
			printf(" ");
		printf("%s",node.group);
		length = sizeof(node.group) - strlen(node.group)+2;
		while(length--)		
			printf(" ");
		printf("%s",node.phone_number);
		length = sizeof(node.phone_number) - strlen(node.phone_number);
		while(length--)		
			printf(" ");
		printf("|        ");
		printf("%s",node.Analog_sorce);
		length = sizeof(node.Analog_sorce) - strlen(node.Analog_sorce) + 1 + 3;
		while(length--)		
			printf(" ");
		
		printf("%3d",num);printf("        |\n");
	}
	else if(mode == 'f')
	{
		printf("| ");
		printf("%s",node.student_ID);
		length = sizeof(node.student_ID) - strlen(node.student_ID);
		while(length--)		
			printf(" ");
		printf("%s",node.name);
		length = sizeof(node.name) - strlen(node.name) -10;
		while(length--)		
			printf(" ");
		printf("%s",node.sex);
		length = sizeof(node.sex) - strlen(node.sex)+2;
		while(length--)		
			printf(" ");
		printf("%s",node.grade);
		length = sizeof(node.grade) - strlen(node.grade)+3;
		while(length--)		
			printf(" ");
		printf("%s",node.group);
		length = sizeof(node.group) - strlen(node.group)+2;
		while(length--)		
			printf(" ");
		printf("%s",node.phone_number);
		length = sizeof(node.phone_number) - strlen(node.phone_number) ;
		while(length--)		
			printf(" ");
		printf("|        ");
		printf("%3d",(atoi(node.C_sorce)+atoi(node.Mathematics_sorce)+atoi(node.English_sorce)+atoi(node.Physics_sorce)+atoi(node.Analog_sorce)));
		length = 1 + 3;
		while(length--)		
			printf(" ");	
		printf(" %3d",num);printf("        |\n");
	}
	//(atoi(cur->C_sorce)+atoi(cur->Mathematics_sorce)+atoi(cur->English_sorce)+atoi(cur->Physics_sorce)+atoi(cur->Analog_sorce))
}
//**************************读取学生信息**************************
student_info *read_student_info(void)
{
	int fd = open(STUDENT_INFO,O_RDONLY | O_CREAT,0777);
	u32 size = ( lseek(fd,0,SEEK_END) - 0 )/90;		
	if(size == 0)	return NULL;
	u8 i = 0;
	int nread;
	//printf("文件大小：%ld   size:%d\n",lseek(fd,0,SEEK_END),size);//printf("  *读取到的学生信息\n");	
	lseek(fd,0,SEEK_SET);

	student_info *student_info_head = kernel_node_Init();

	while( i != size )
	{			

		student_info *student_info_new_p = kernel_node_Init();
		nread = read(fd,student_info_new_p,90);
		insert_node(&student_info_head->list, student_info_new_p);		
		//printf("strlen[%d]:%ld nread:%d\n",i,strlen(student_info_new_p->student_ID),nread);printf("student_info_new_p[%d]->student_ID:%s\n",i,student_info_new_p->student_ID);
		i++;
	}
	close(fd);	
	return student_info_head;
}
//**************************学生信息检查**************************
u8 examine_student_info(u8 input[],u32 *status,u8 mix_size,u8 max_size,u8 mode,u8 num,student_info *student_info_head,u8 swicth)
{													//9			//9s
	u8 count = 0;
	u32 length = strlen(input);
	u8 judge1,judge2,judge3,judge4;
	u32 status_changes = 1;
	//printf("%d  |%c|",length,input[8]);
	
	if(mode == Num_examine)
	{
		judge1 = '0';judge2 = '9';judge3 = 0;judge4 = 0;
	}
	else if(mode == Char_examine)
	{
		judge1 = 'a';judge2 = 'z';judge3 = 'A';judge4 = 'Z';
	}
	//printf("judge1:%cjudge2:%cjudge3:%cjudge4:%c\n",judge1,judge2,judge3,judge4);
	//*************开始判断****************
	if(length < mix_size || input[mix_size-1] == '\n')
	{
		//printf("  *1");
		*status |= (status_changes << (num - 1)); //printf("* 1 *\n");
		printf_usr_error_student_info(*status);	
		return 1;
	}	
	//printf("length:%d max_size:%d\n",length,max_size);	
	if( length == max_size )
	{
		//printf(" input[%d]:%c\n",length-1,input[length-1]);	
		if( input[length-1] != '\n')
		{
			if( getchar() != '\n' )
			{		
				*status |= (status_changes << (num - 1));//printf("  *2");
				while(getchar() != '\n'); 
				printf_usr_error_student_info(*status);		
				return 1;
			}	
		}	
	}
	if( input[length-1] == '\n')
	{
		input[length-1] = '\0';//printf("%s%c%c|\n",input,input[length],input[length+1]);
	}

	length = strlen(input);//printf("length:%d max_size:%d\n",length,max_size);
	while(count != length)
	{
		if( !((input[count] >= judge1 && input[count] <= judge2) || (input[count] >= judge3 && input[count] <= judge4)) )
		{
			//if(input[count] >= judge1 && input[count] <= judge2)	printf("  ***judge11\n");if(input[count] >= judge3 && input[count] <= judge4)	printf("  ***judge22\n");printf("input[%d]:%c|\n",count,input[count]);printf("  *44");			
			*status |= (status_changes << (num - 1));// printf("  *\n");
			printf_usr_error_student_info(*status);		
			return 1;
		} 
		count++;
	}
	
	if(num == 1 && swicth == 1)
	{
		if(check_node(input,student_info_head) != NULL)
		{
			printf("  *你输入的学生学号已经存在\n");
			return 1;
		}	
	}

	if(num == 4 && input[0] > '4')
	{
		*status |= (status_changes << (num - 1));// printf("  *\n");
		printf_usr_error_student_info(*status);		
		return 1;
	}

	if(num >= 7 && num <= 11 && atoi(input) > 100)
	{
		*status |= (status_changes << (num - 1));// printf("  *\n");
		printf_usr_error_student_info(*status);		
		return 1;
	}

	if(num == 3 && ( strcmp(input,"man") != 0  && strcmp(input,"woman") != 0 && strcmp(input,"gay") != 0 ) )
	{//&& atoi(input) > 100
		
		printf("sex error");
		*status |= (status_changes << (num - 1));// printf("  *\n");
		//printf_usr_error_student_info(*status);		
		return 1;
	}

	//length = strlen(input);
	if(length < max_size)
		memset( input + length + 1,' ',max_size - length);

	*status &= ~(status_changes << (num - 1));// printf("  *\n");
	return 0;
}
//**************************写入学生信息**************************
void write_student_info(student_info *head)
{
	int fd = open(STUDENT_INFO,O_WRONLY | O_CREAT | O_TRUNC,0777);
	if(fd < 0)
	{
		perror("创建文件失败");
	}	

	u8 i = 0;
	int nwrite;

	struct list_head *pos;
	student_info *q;
	list_for_each(pos,&(head->list))
	{
		q = list_entry(pos, student_info , list);
		
		nwrite = write(fd,q,90);	
		// printf("strlen[%d]:%ld nwrite:%d\n",i,strlen(q->student_ID),nwrite);
		// printf("当前的偏移量:%ld\n",lseek(fd,0,SEEK_CUR));
		// printf("head[%d]->student_ID:%s\n",i,q->student_ID);
		i++;
	}

	close(fd);	
	return ;
}
//**************************遍历释放链表内存**************************
void free_each(student_info *student_info_head)
{
	int fd = open(STUDENT_INFO,O_RDONLY | O_CREAT,0777);
	u32 size = ( lseek(fd,0,SEEK_END) - 0 )/90 + 1 ;		
	if(size == 0){	printf(CYAN"********释放内存失败********\n");return;	}	

	student_info *cur = list_entry(student_info_head->list.next, student_info , list);
	student_info *next = cur;
	while(size--)
	{
		cur = next;
		next = list_entry(cur->list.next, student_info , list);
		free(cur);
		//printf(CYAN"********释放内存成功   [%d]********\n",9-size);	
	}
	//printf(CYAN"********释放内存成功********\n");	
}
//**************************学生信息录入**************************
void register_student_info()
{
	system("clear");
	//************************获取权限****************************
	while(1)
	{
		u8 buf[3],flag = 0;
		printf("\n"CYAN);
		printf("  **********************************************************************\n");
		printf("  *                      "RED);printf("该操作需要超级管理员密码"CYAN);printf("                      *\n");
		printf("  *                                                                    *\n");
		printf("  *             [root password]:");	
		fgets(buf,5,stdin);
		if( !strcmp(buf,"abc\n") ){
			system("clear");break;
		}   
			
		else
		{	flag = 1;
			if( buf[0] != '\n' && buf[1] != '\n' && buf[2] != '\n' && buf[3] != '\n')	while(getchar() != '\n');
			printf("  *\n  *");printf(RED"                 密码错误\n"CYAN);
		}

		while(flag)
		{
			printf("  **********************************************************************\n");
			printf("  *                     g:继续输入     q:返回主界面~                   *\n");printf("  *");		
			fgets(buf,3,stdin);
			if(buf[0] == 'q')
				return;
			else if(buf[0] == 'g'){
				system("clear");break;
			}
			else
			{
				if( buf[0] != '\n' && buf[1] != '\n')	while(getchar() != '\n');
				printf(RED"  *输入有误请重新输入\n"CYAN);
			}
		}
	}

	u32 error_student_info;
	u8 exist_flag,swicth = 1,buf[3];
	

	while(1)//error_student_info & 0x07FF)
	{
		//***********读取学生信息结构体存放到链表中***********
		student_info *student_info_head = read_student_info();
		//***********初始化结构体***********
		student_info *input_from_usr_p = kernel_node_Init();
		error_student_info = 0;
		//***********开始录入***********
		printf("\n"CYAN);
		printf("  **********************************************************************\n");
		printf("  *                         ~学生基本信息录入~	       (quit:返回主界面)\n");		
		do{
			printf("  *      学号(9个数字)     :");	
			fgets(input_from_usr_p->student_ID,sizeof(input_from_usr_p->student_ID),stdin);
			if( !strcmp( input_from_usr_p->student_ID ,("quit\n")) )	return;
		}while(examine_student_info(input_from_usr_p->student_ID,&error_student_info,9,9,Num_examine,1,student_info_head,swicth));
		do{
			printf("  *      姓名(25个字母)    :");
			fgets(input_from_usr_p->name,sizeof(input_from_usr_p->name),stdin);
			if( !strcmp( input_from_usr_p->name ,("quit\n")) )	return;		
		}while(examine_student_info(input_from_usr_p->name,&error_student_info,4,24,Char_examine,2,student_info_head,swicth));	
		do{
			printf("  *      性别(man or womam):");
			fgets(input_from_usr_p->sex,sizeof(input_from_usr_p->sex),stdin);
			if( !strcmp( input_from_usr_p->sex ,("quit\n")) )	return;			
		}while(examine_student_info(input_from_usr_p->sex,&error_student_info,3,5,Char_examine,3,student_info_head,swicth));		
		do{
			printf("  *      年级(1~4)         :");
			fgets(input_from_usr_p->grade,sizeof(input_from_usr_p->grade),stdin);
			if( !strcmp( input_from_usr_p->grade ,("quit\n")) )	return;	
		}while(examine_student_info(input_from_usr_p->grade,&error_student_info,1,1,Num_examine,4,student_info_head,swicth));		
		do{
			printf("  *      班别(1~9)         :");
			fgets(input_from_usr_p->group,sizeof(input_from_usr_p->group),stdin);
			if( !strcmp( input_from_usr_p->group ,("quit\n")) )	return;	
		}while(examine_student_info(input_from_usr_p->group,&error_student_info,1,1,Num_examine,5,student_info_head,swicth));
		do{
			printf("  *      电话号码(11个数字):");	
			fgets(input_from_usr_p->phone_number,sizeof(input_from_usr_p->phone_number),stdin);
			if( !strcmp( input_from_usr_p->phone_number ,("quit\n")) )	return;		
		}while(examine_student_info(input_from_usr_p->phone_number,&error_student_info,11,11,Num_examine,6,student_info_head,swicth));	

		printf("  *                   ~学生成绩录入~\n");
		do{
			printf("  *      C语言(0~100)      :");
			fgets(input_from_usr_p->C_sorce,sizeof(input_from_usr_p->C_sorce),stdin);
			if( !strcmp( input_from_usr_p->C_sorce ,("quit\n")) )	return;		
		}while(examine_student_info(input_from_usr_p->C_sorce,&error_student_info,1,3,Num_examine,7,student_info_head,swicth));
		do{
			printf("  *      高数(0~100)       :");
			fgets(input_from_usr_p->Mathematics_sorce,sizeof(input_from_usr_p->Mathematics_sorce),stdin);
			if( !strcmp( input_from_usr_p->Mathematics_sorce ,("quit\n")) )	return;		
		}while(examine_student_info(input_from_usr_p->Mathematics_sorce,&error_student_info,1,3,Num_examine,8,student_info_head,swicth));	
		do{
			printf("  *      英语(0~100)       :");
			fgets(input_from_usr_p->English_sorce,sizeof(input_from_usr_p->English_sorce),stdin);
			if( !strcmp( input_from_usr_p->English_sorce ,("quit\n")) )	return;		
		}while(examine_student_info(input_from_usr_p->English_sorce,&error_student_info,1,3,Num_examine,9,student_info_head,swicth));		
		do{
			printf("  *      物理(0~100)       :");
			fgets(input_from_usr_p->Physics_sorce,sizeof(input_from_usr_p->Physics_sorce),stdin);
			if( !strcmp( input_from_usr_p->Physics_sorce ,("quit\n")) )	return;		
		}while(examine_student_info(input_from_usr_p->Physics_sorce,&error_student_info,1,3,Num_examine,10,student_info_head,swicth));		
		do{
			printf("  *      模电(0~100)       :");
		    fgets(input_from_usr_p->Analog_sorce,sizeof(input_from_usr_p->Analog_sorce),stdin);
			if( !strcmp( input_from_usr_p->Analog_sorce ,("quit\n")) )	return;	
		}while(examine_student_info(input_from_usr_p->Analog_sorce,&error_student_info,1,3,Num_examine,11,student_info_head,swicth));
		printf("  *\n  **********************************************************************\n");		
			
		if(check_node(input_from_usr_p->student_ID,student_info_head) != NULL)
		{
			printf("  *\n  *你输入的学生学号已经存在  *\n\n");
			exist_flag = 1;	
		}else
			exist_flag = 0;				
	
		if(!exist_flag)
		{		
			insert_node(&student_info_head->list,input_from_usr_p);
			write_student_info(student_info_head);

			free_each(student_info_head);
			//printf(CYAN"********释放内存成功********\n");	
			printf("  *\n  *录入成功\n  *\n");
		}	

		while(1)
		{
			printf("  **********************************************************************\n");
			printf("  *                     g:继续录入     q:返回主界面~                   *\n");printf("  *");		
			fgets(buf,3,stdin);
			if(buf[0] == 'q')
				return;
			else if(buf[0] == 'g')
				break;
			else
			{
				if( buf[0] != '\n' && buf[1] != '\n')	while(getchar() != '\n');
				printf(RED"  *输入有误请重新输入\n"CYAN);
			}
		}
	}
}
//************************遍历显示某个班级成员信息**************************
void each_display_info_from_group(student_info *student_info_head,u8 grade,u8 group)
{
	struct list_head *pos;
	student_info *p;
	u32 count=0;
	list_for_each(pos,&(student_info_head->list))
	{
		p = list_entry(pos, student_info , list);

		//printf("学生信息库中的ID：%s\n",q->student_ID);
		if(p->grade[0] == grade && p->group[0] == group)
		{
			display_student_node(*p,'z',0);
			count++;
		}
	}
	if(count == 0)
	{
		system("clear");
		printf("-------------------------------------------------------------------------------------\n");
		printf("|                             没有%c 年级 %c 班                               |\n",grade,group);
	}
	
}
//*************************遍历显示成绩排名信息****************************
void each_display_lesson_sort_info(student_info *student_info_head,u8 mode)
{
	int fd = open(STUDENT_INFO,O_RDONLY | O_CREAT,0777);u32 size = ( lseek(fd,0,SEEK_END) - 0 )/90;close(fd);	
	struct list_head *pos;
	student_info *cur,*next;
	u8 source = 250;
 	u32 num = 0;
 	switch(mode)
 	{

		case 'a':
			//**************冒泡排序法**************
			while(--size)
			{
				list_for_each(pos,&(student_info_head->list))
				{
					cur  = list_entry(pos, student_info , list);
					next = list_entry(pos->next, student_info , list);

					if(atoi(cur->C_sorce) < atoi(next->C_sorce) && next != student_info_head)
						exchange_node(&cur->list,&next->list);			
				}		
			}
			//**************遍历显示链表**************	
			list_for_each(pos,&(student_info_head->list))
			{	
				cur  = list_entry(pos, student_info , list);				
				if( atoi(cur->C_sorce) != source )
				{
					num++;
					source = atoi(cur->C_sorce);
				}
				display_student_node(*cur,mode,num);
			}
			break;
		case 'b':
			//**************冒泡排序法**************
			while(--size)
			{
				list_for_each(pos,&(student_info_head->list))
				{
					cur  = list_entry(pos, student_info , list);
					next = list_entry(pos->next, student_info , list);

					if(atoi(cur->Mathematics_sorce) < atoi(next->Mathematics_sorce) && next != student_info_head)
						exchange_node(&cur->list,&next->list);			
				}		
			}
			//**************遍历显示链表**************	
			list_for_each(pos,&(student_info_head->list))
			{	
				cur  = list_entry(pos, student_info , list);				
				if( atoi(cur->Mathematics_sorce) != source )
				{
					num++;
					source = atoi(cur->Mathematics_sorce);
				}
				display_student_node(*cur,mode,num);
			}
			break;
		case 'c':
			//**************冒泡排序法**************
			while(--size)
			{
				list_for_each(pos,&(student_info_head->list))
				{
					cur  = list_entry(pos, student_info , list);
					next = list_entry(pos->next, student_info , list);

					if(atoi(cur->English_sorce) < atoi(next->English_sorce) && next != student_info_head)
						exchange_node(&cur->list,&next->list);			
				}		
			}
			//**************遍历显示链表**************	
			list_for_each(pos,&(student_info_head->list))
			{	
				cur  = list_entry(pos, student_info , list);				
				if( atoi(cur->English_sorce) != source )
				{
					num++;
					source = atoi(cur->English_sorce);
				}
				display_student_node(*cur,mode,num);
			}
			break;
		case 'd':
			//**************冒泡排序法**************
			while(--size)
			{
				list_for_each(pos,&(student_info_head->list))
				{
					cur  = list_entry(pos, student_info , list);
					next = list_entry(pos->next, student_info , list);

					if(atoi(cur->Physics_sorce) < atoi(next->Physics_sorce) && next != student_info_head)
						exchange_node(&cur->list,&next->list);			
				}		
			}
			//**************遍历显示链表**************	
			list_for_each(pos,&(student_info_head->list))
			{	
				cur  = list_entry(pos, student_info , list);				
				if( atoi(cur->Physics_sorce) != source )
				{
					num++;
					source = atoi(cur->Physics_sorce);
				}
				display_student_node(*cur,mode,num);
			}
			break;
		case 'e':
			//**************冒泡排序法**************
			while(--size)
			{
				list_for_each(pos,&(student_info_head->list))
				{
					cur  = list_entry(pos, student_info , list);
					next = list_entry(pos->next, student_info , list);

					if(atoi(cur->Analog_sorce) < atoi(next->Analog_sorce) && next != student_info_head)
						exchange_node(&cur->list,&next->list);			
				}		
			}
			//**************遍历显示链表**************	
			list_for_each(pos,&(student_info_head->list))
			{	
				cur  = list_entry(pos, student_info , list);				
				if( atoi(cur->Analog_sorce) != source )
				{
					num++;
					source = atoi(cur->Analog_sorce);
				}
				display_student_node(*cur,mode,num);
			}
			break;
		case 'f':
			//**************冒泡排序法**************
			while(--size)
			{
				list_for_each(pos,&(student_info_head->list))
				{
					cur  = list_entry(pos, student_info , list);
					next = list_entry(pos->next, student_info , list);
					
					if( (atoi(cur->C_sorce)+atoi(cur->Mathematics_sorce)+atoi(cur->English_sorce)+atoi(cur->Physics_sorce)+atoi(cur->Analog_sorce))
					  < (atoi(cur->C_sorce)+atoi(cur->Mathematics_sorce)+atoi(cur->English_sorce)+atoi(cur->Physics_sorce)+atoi(cur->Analog_sorce)) 
					  && next != student_info_head )
						exchange_node(&cur->list,&next->list);			
				}		
			}
			//**************遍历显示链表**************	
			list_for_each(pos,&(student_info_head->list))
			{	
				cur  = list_entry(pos, student_info , list);				
				if( atoi(cur->Analog_sorce) != source )
				{
					num++;
					source = atoi(cur->Analog_sorce);
				}
				display_student_node(*cur,mode,num);
			}
    }
	return;		
}
//**************************显示班级成员信息**************************
void display_group_info()
{
	u8 grade,group;
	while(1)
	{
		system("clear");
		printf("\n  **********************************************************************\n");
		printf("  *                          ~显示班级成员信息~	           (q:返回主界面)\n  *\n");
		while(1)
		{	
			u8 buf[3];
			printf("  *输入要查找的班级的年级：(1~4):");	
			fgets(buf,3,stdin);
			if(buf[0] == 'q')
				return;
			else if(buf[0] <= '4' && buf[0] >= '1')
			{
				grade = buf[0];
				break;		
			}
			else
			{
				if( buf[0] != '\n' && buf[1] != '\n')	while(getchar() != '\n');
				printf(RED"  *输入有误请重新输入\n"CYAN);
			}
		}
		while(1)
		{	
			u8 buf[3];
			printf("  *输入要查找的班级的班别：(1~9):");
			fgets(buf,3,stdin);
			if(buf[0] == 'q')
				return;
			else if(buf[0] <= '9' && buf[0] >= '1' && buf[1] == '\n')
			{
				group = buf[0];
				break;		
			}
			else
			{
				if( buf[0] != '\n' && buf[1] != '\n')	while(getchar() != '\n');
				printf(RED"  *输入有误请重新输入\n"CYAN);
			}
		}
		//***********读取学生信息结构体存放到链表中***********
		student_info *student_info_head = read_student_info();

		system("clear");
		printf("\n"CYAN);
		printf("-------------------------------------------------------------------------------------\n");
		printf("|                               %c 年级 %c 班的学生信息                               |\n",grade,group);
		printf("-------------------------------------------------------------------------------------\n");
		printf("|                        基本信息                       |           成绩            |\n");
		printf("-------------------------------------------------------------------------------------\n");
		printf("|    学号      姓名         性别  班级 班别   联系方式  | C语言 高数 英语 物理 模电 |\n");
		printf("-------------------------------------------------------------------------------------\n");
		each_display_info_from_group(student_info_head,grade,group);
		printf("-------------------------------------------------------------------------------------\n");
							
		u8 buf[3];

		while(1)
		{
			printf("  **********************************************************************\n");
			printf("  *                     g:继续查找     q:返回主界面~                   *\n");printf("  *");		
			fgets(buf,3,stdin);
			if(buf[0] == 'q'){
				free_each(student_info_head);return;
			}
			else if(buf[0] == 'g')
				break;
			else
			{
				if( buf[0] != '\n' && buf[1] != '\n')	while(getchar() != '\n');
				printf(RED"  *输入有误请重新输入\n"CYAN);
			}
		}
	}
}
//**************************修改学生信息**************************						
void rewrite_student_info()
{
	system("clear");
	//************************获取权限****************************
	while(1)
	{
		u8 buf[3],flag = 0;
		printf("\n"CYAN);
		printf("  **********************************************************************\n");
		printf("  *                      "RED);printf("该操作需要超级管理员密码"CYAN);printf("                      *\n");
		printf("  *                                                                    *\n");
		printf("  *             [root password]:");		
		fgets(buf,5,stdin);
		if( !strcmp(buf,"abc\n") ){
			system("clear");break;
		}   
			
		else
		{	flag = 1;
			if( buf[0] != '\n' && buf[1] != '\n' && buf[2] != '\n' && buf[3] != '\n')	while(getchar() != '\n');
			printf("  *\n  *");printf(RED"                 密码错误\n"CYAN);
		}

		while(flag)
		{
			printf("  **********************************************************************\n");
			printf("  *                     g:继续输入     q:返回主界面~                   *\n");printf("  *");		
			fgets(buf,3,stdin);
			if(buf[0] == 'q')
				return;
			else if(buf[0] == 'g'){
				system("clear");break;
			}
			else
			{
				if( buf[0] != '\n' && buf[1] != '\n')	while(getchar() != '\n');
				printf(RED"  *输入有误请重新输入\n"CYAN);
			}
		}
	}

	u32 error_student_info;
	u8 match_flag,coutinue_flag,swicth = 0,buf[3];
	
	while(1)
	{  
		//***********读取学生信息结构体存放到链表中***********
		student_info *student_info_head = read_student_info();
		coutinue_flag = 0;swicth = 0;	
		//***********初始化结构体***********
		student_info *input_from_usr_p = kernel_node_Init();
		student_info *input_from_file_p = kernel_node_Init();
		error_student_info = 0;
		//***********开始录入***********
		printf("\n"CYAN);
		printf("  **********************************************************************\n");
		printf("  *                          ~修改学生信息~	           (quit:返回主界面)\n  *\n");	
		do{
			printf("  *输入要修改学生的学号(9个数字):");
			fgets(input_from_usr_p->student_ID,sizeof(input_from_usr_p->student_ID),stdin);
			if( !strcmp( input_from_usr_p->student_ID ,("quit\n")) )	return;
		}while(examine_student_info(input_from_usr_p->student_ID,&error_student_info,9,9,Num_examine,1,student_info_head,swicth));
		printf("  *\n  **********************************************************************\n");		
		
		//input_from_file = *check_node(input_from_usr_p->student_ID,student_info_head);//printf("input_from_file:\n%s  ",input_from_file.student_ID);printf("  %s",input_from_file.name);printf("  %s",input_from_file.sex);printf("  %s",input_from_file.grade);printf("  %s",input_from_file.group);printf("  %s\n",input_from_file.phone_number);

		if(check_node(input_from_usr_p->student_ID,student_info_head) == NULL)
		{
				printf("  *\n  *你输入的学生学号不存在  *\n\n");
				match_flag = 1;	
		}
		else
		{
			input_from_file_p = check_node(input_from_usr_p->student_ID,student_info_head);
			match_flag = 0;
			printf("  *\n  *搜索成功\n  *");
		}

		if(!match_flag)
		{												
			error_student_info = 0;
			swicth = 1;	
			printf("\n"CYAN);
			printf("  **********************************************************************\n");
			printf("  *                          ~要修改的学生信息~            (回车:不修改)\n");
			printf("  *                                                    (quit:返回主界面)\n");
			do{
				printf("  *      原学号              :%s\n",input_from_file_p->student_ID);
				printf("  *      新学号(9个数字)     :");	
				fgets(input_from_usr_p->student_ID,sizeof(input_from_usr_p->student_ID),stdin);
				if( !strcmp( input_from_usr_p->student_ID ,("quit\n")) ){
					free_each(student_info_head);
					return;
				}
				if(input_from_usr_p->student_ID[0] == '\n')
				{
					memcpy(input_from_usr_p->student_ID,input_from_file_p->student_ID,sizeof(input_from_usr_p->student_ID));
					break;
				} 
			}while(examine_student_info(input_from_usr_p->student_ID,&error_student_info,9,9,Num_examine,1,student_info_head,swicth));
			do{
				printf("  *      原姓名              :%s\n",input_from_file_p->name);
				printf("  *      新姓名(25个字母)    :");	
				fgets(input_from_usr_p->name,sizeof(input_from_usr_p->name),stdin);
				if( !strcmp( input_from_usr_p->name ,("quit\n")) ){
					free_each(student_info_head);
					return;
				}
				if(input_from_usr_p->name[0] == '\n')
				{
					memcpy(input_from_usr_p->name,input_from_file_p->name,sizeof(input_from_usr_p->name));
					break;
				}	
			}while(examine_student_info(input_from_usr_p->name,&error_student_info,4,24,Char_examine,2,student_info_head,swicth));
			do{
				printf("  *      原性别              :%s\n",input_from_file_p->sex);
				printf("  *      新性别(man or womam):");
				fgets(input_from_usr_p->sex,sizeof(input_from_usr_p->sex),stdin);
				if( !strcmp( input_from_usr_p->sex ,("quit\n")) ){
					free_each(student_info_head);
					return;
				}
				if(input_from_usr_p->sex[0] == '\n')
				{
					memcpy(input_from_usr_p->sex,input_from_file_p->sex,sizeof(input_from_usr_p->sex));
					break;
				}	
			}while(examine_student_info(input_from_usr_p->sex,&error_student_info,3,5,Char_examine,3,student_info_head,swicth));
			do{
				printf("  *      原年级(1~4)         :%s\n",input_from_file_p->grade);
				printf("  *      新年级(1~4)         :");	
				fgets(input_from_usr_p->grade,sizeof(input_from_usr_p->grade),stdin);
				if( !strcmp( input_from_usr_p->grade ,("quit\n")) ){
					free_each(student_info_head);
					return;
				}
				if(input_from_usr_p->grade[0] == '\n')
				{
					memcpy(input_from_usr_p->grade,input_from_file_p->grade,sizeof(input_from_usr_p->grade));
					break;
				}	
			}while(examine_student_info(input_from_usr_p->grade,&error_student_info,1,1,Num_examine,4,student_info_head,swicth));
			do{
				printf("  *      原班别(1~9)         :%s\n",input_from_file_p->group);
				printf("  *      新班别(1~9)         :");
				fgets(input_from_usr_p->group,sizeof(input_from_usr_p->group),stdin);
				if( !strcmp( input_from_usr_p->group ,("quit\n")) ){
					free_each(student_info_head);
					return;
				}
				if(input_from_usr_p->group[0] == '\n')
				{
					memcpy(input_from_usr_p->group,input_from_file_p->group,sizeof(input_from_usr_p->group));
					break;
				}	
			}while(examine_student_info(input_from_usr_p->group,&error_student_info,1,1,Num_examine,5,student_info_head,swicth));				
			do{
				printf("  *      原电话号码          :%s\n",input_from_file_p->phone_number);
				printf("  *      新电话号码(11个数字):");	
				fgets(input_from_usr_p->phone_number,sizeof(input_from_usr_p->phone_number),stdin);
				if( !strcmp( input_from_usr_p->phone_number ,("quit\n")) ){
					free_each(student_info_head);
					return;
				}
				if(input_from_usr_p->phone_number[0] == '\n')
				{
					memcpy(input_from_usr_p->phone_number,input_from_file_p->phone_number,sizeof(input_from_usr_p->phone_number));
					break;
				}	
			}while(examine_student_info(input_from_usr_p->phone_number,&error_student_info,11,11,Num_examine,6,student_info_head,swicth));
			printf("  *              ~原成绩~         新成绩\n");		
			do{					
				printf("  * C语言(0~100):   %s              ",input_from_file_p->C_sorce);
				fgets(input_from_usr_p->C_sorce,sizeof(input_from_usr_p->C_sorce),stdin);
				if( !strcmp( input_from_usr_p->C_sorce ,("quit\n")) ){
					free_each(student_info_head);
					return;
				}
				if(input_from_usr_p->C_sorce[0] == '\n')
				{
					memcpy(input_from_usr_p->C_sorce,input_from_file_p->C_sorce,sizeof(input_from_usr_p->C_sorce));
					break;
				}	
			}while(examine_student_info(input_from_usr_p->C_sorce,&error_student_info,1,3,Num_examine,7,student_info_head,swicth));				
			do{
				printf("  *  高数(0~100):   %s              ",input_from_file_p->Mathematics_sorce);
				fgets(input_from_usr_p->Mathematics_sorce,sizeof(input_from_usr_p->Mathematics_sorce),stdin);
				if( !strcmp( input_from_usr_p->Mathematics_sorce ,("quit\n")) ){
					free_each(student_info_head);
					return;
				}
				if(input_from_usr_p->Mathematics_sorce[0] == '\n')
				{
					memcpy(input_from_usr_p->Mathematics_sorce,input_from_file_p->Mathematics_sorce,sizeof(input_from_usr_p->Mathematics_sorce));
					break;
				}	
			}while(examine_student_info(input_from_usr_p->Mathematics_sorce,&error_student_info,1,3,Num_examine,8,student_info_head,swicth));
			do{
				printf("  *  英语(0~100):   %s              ",input_from_file_p->English_sorce);
				fgets(input_from_usr_p->English_sorce,sizeof(input_from_usr_p->English_sorce),stdin);
				if( !strcmp( input_from_usr_p->English_sorce ,("quit\n")) ){
					free_each(student_info_head);
					return;
				}
				if(input_from_usr_p->English_sorce[0] == '\n')
				{
					memcpy(input_from_usr_p->English_sorce,input_from_file_p->English_sorce,sizeof(input_from_usr_p->English_sorce));
					break;
				}	
			}while(examine_student_info(input_from_usr_p->English_sorce,&error_student_info,1,3,Num_examine,9,student_info_head,swicth));
			do{
				printf("  *  物理(0~100):   %s              ",input_from_file_p->Physics_sorce);
				fgets(input_from_usr_p->Physics_sorce,sizeof(input_from_usr_p->Physics_sorce),stdin);
				if( !strcmp( input_from_usr_p->Physics_sorce ,("quit\n")) ){
					free_each(student_info_head);
					return;
				}
				if(input_from_usr_p->Physics_sorce[0] == '\n')
				{
					memcpy(input_from_usr_p->Physics_sorce,input_from_file_p->Physics_sorce,sizeof(input_from_usr_p->Physics_sorce));
					break;
				}	
			}while(examine_student_info(input_from_usr_p->Physics_sorce,&error_student_info,1,3,Num_examine,10,student_info_head,swicth));			
			do{
				printf("  *  模电(0~100):   %s              ",input_from_file_p->Analog_sorce);
				fgets(input_from_usr_p->Analog_sorce,sizeof(input_from_usr_p->Analog_sorce),stdin);
				if( !strcmp( input_from_usr_p->Analog_sorce ,("quit\n")) ){
					free_each(student_info_head);
					return;
				}
				if(input_from_usr_p->Analog_sorce[0] == '\n')
				{
					memcpy(input_from_usr_p->Analog_sorce,input_from_file_p->Analog_sorce,sizeof(input_from_usr_p->Analog_sorce));
					break;
				}	
			}while(examine_student_info(input_from_usr_p->Analog_sorce,&error_student_info,1,3,Num_examine,11,student_info_head,swicth));
			printf("  *\n  **********************************************************************\n");		

			printf("  *\n  *录入成功\n  *\n");//printf("input_from_file:\n%s  ",input_from_file_p->student_ID);printf("  %s",input_from_file_p->name);printf("  %s",input_from_file_p->sex);printf("  %s",input_from_file_p->grade);printf("  %s",input_from_file_p->group);printf("  %s\n",input_from_file_p->phone_number);printf("input_from_usr:\n%s  ",input_from_usr_p->student_ID);printf("  %s",input_from_usr_p->name);printf("  %s",input_from_usr_p->sex);printf("  %s",input_from_usr_p->grade);printf("  %s",input_from_usr_p->group);printf("  %s\n",input_from_usr_p->phone_number);
			//*****************写入文件*****************
			delet_node(input_from_file_p);

			insert_node(&student_info_head->list,input_from_usr_p);
			write_student_info(student_info_head);
	
			while(1)
			{
				u8 buf[3];
				printf("  **********************************************************************\n");
				printf("  *                     g:继续修改     q:返回主界面~                   *\n");printf("  *");		
				fgets(buf,3,stdin);
				if(buf[0] == 'q'){
					free_each(student_info_head);
					return;
				}
				else if(buf[0] == 'g')
				{
					coutinue_flag = 1;
					break;
				}
				else
				{
					if( buf[0] != '\n' && buf[1] != '\n')	while(getchar() != '\n');
					printf(RED"  *输入有误请重新输入\n"CYAN);
				}
			}
		}					
		
		while(!coutinue_flag)
		{
			u8 buf[3];	
			printf("  **********************************************************************\n");
			printf("  *                     g:继续修改     q:返回主界面~                   *\n");printf("  *");		
			fgets(buf,3,stdin);
			if(buf[0] == 'q'){
				free_each(student_info_head);
				return;
			}
			else if(buf[0] == 'g')
				break;
			else
			{
				if( buf[0] != '\n' && buf[1] != '\n')	while(getchar() != '\n');
				printf(RED"  *输入有误请重新输入\n"CYAN);
			}
		}
		coutinue_flag = 0;
	}		
}
//**************************删除学生信息**************************
void delet_student_info()
{
	system("clear");
	//************************获取权限****************************
	while(1)
	{
		u8 buf[3],flag = 0;
		printf("\n"CYAN);
		printf("  **********************************************************************\n");
		printf("  *                      "RED);printf("该操作需要超级管理员密码"CYAN);printf("                      *\n");
		printf("  *                                                                    *\n");
		printf("  *             [root password]:");		
		fgets(buf,5,stdin);
		if( !strcmp(buf,"abc\n") ){
			system("clear");break;
		}   
			
		else
		{	flag = 1;
			if( buf[0] != '\n' && buf[1] != '\n' && buf[2] != '\n' && buf[3] != '\n')	while(getchar() != '\n');
			printf("  *\n  *");printf(RED"                 密码错误\n"CYAN);
		}

		while(flag)
		{
			printf("  **********************************************************************\n");
			printf("  *                     g:继续输入     q:返回主界面~                   *\n");printf("  *");		
			fgets(buf,3,stdin);
			if(buf[0] == 'q')
				return;
			else if(buf[0] == 'g'){
				system("clear");break;
			}
			else
			{
				if( buf[0] != '\n' && buf[1] != '\n')	while(getchar() != '\n');
				printf(RED"  *输入有误请重新输入\n"CYAN);
			}
		}
	}

	u32 error_student_info;
	
	while(1)
	{  	
		u8 match_flag,swicth = 0;
		//***********读取学生信息结构体存放到链表中***********
		student_info *student_info_head = read_student_info();
		//***********初始化结构体***********
		student_info *input_from_usr_p = kernel_node_Init(),input_from_file;
		error_student_info = 0;
		//***********开始录入***********
		printf("\n"CYAN);
		printf("  **********************************************************************\n");
		printf("  *                          ~删除学生信息~	           (quit:返回主界面)\n  *\n");	
		do{
			printf("  *输入要删除的学生学号(9个数字):");
			fgets(input_from_usr_p->student_ID,sizeof(input_from_usr_p->student_ID),stdin);
			if( !strcmp( input_from_usr_p->student_ID ,("quit\n")) ){
				free_each(student_info_head);
				return;
			}
		}while(examine_student_info(input_from_usr_p->student_ID,&error_student_info,9,9,Num_examine,1,student_info_head,swicth));
		printf("  *\n  **********************************************************************\n");		
		
		//input_from_file = *check_node(input_from_usr_p->student_ID,student_info_head);
		//printf("input_from_file:\n%s  ",input_from_file.student_ID);printf("  %s",input_from_file.name);printf("  %s",input_from_file.sex);printf("  %s",input_from_file.grade);printf("  %s",input_from_file.group);printf("  %s\n",input_from_file.phone_number);

		if(check_node(input_from_usr_p->student_ID,student_info_head) == NULL)
		{
				printf("  *\n  *你输入的学生学号不存在\n  *\n");
				match_flag = 0;
				free(input_from_usr_p);	
		}else
		{
			student_info *input_from_file_p = check_node(input_from_usr_p->student_ID,student_info_head);
			free(input_from_usr_p);
			delet_node(input_from_file_p);
			match_flag = 1;
			printf("  *\n  *删除成功\n  *\n");
		}

		if(match_flag)
		{												
			//*****************写入文件*****************		
			write_student_info(student_info_head);
			free_each(student_info_head);	
		}			
			
		while(1)  
		{
			u8 buf[3];	
			printf("  **********************************************************************\n");
			printf("  *                     g:继续删除     q:返回主界面~                   *\n");printf("  *");		
			fgets(buf,3,stdin);
			if(buf[0] == 'q'){
				return;
			}
			else if(buf[0] == 'g'){
				system("clear");break;
			}
			else
			{
				if( buf[0] != '\n' && buf[1] != '\n')	while(getchar() != '\n');
				printf(RED"  *输入有误请重新输入\n"CYAN);
			}
		}
	}	
}
//**************************查找学生信息**************************
void search_student_info()
{
	system("clear");
	u32 error_student_info;
	u8 match_flag,swicth = 0;
	
	while(1)
	{  	
		//***********读取学生信息结构体存放到链表中***********
		student_info *student_info_head = read_student_info();
		//***********初始化结构体***********
		student_info *input_from_usr_p  = kernel_node_Init(),input_from_file;
		error_student_info = 0;

		printf("\n"CYAN);
		printf("  **********************************************************************\n");
		printf("  *                          ~查找学生信息~	           (quit:返回主界面)\n  *\n");	
		do{
			printf("  *输入要查找的学生学号(9个数字):");
			fgets(input_from_usr_p->student_ID,sizeof(input_from_usr_p->student_ID),stdin);
			if( !strcmp( input_from_usr_p->student_ID ,("quit\n")) ){
				free_each(student_info_head);
				return;
			}
		}while(examine_student_info(input_from_usr_p->student_ID,&error_student_info,9,9,Num_examine,1,student_info_head,swicth));
		printf("  *\n  **********************************************************************\n");		
		
		printf("input_from_usr_p->student_ID:%s\n",input_from_usr_p->student_ID);		

		if(check_node(input_from_usr_p->student_ID,student_info_head) == NULL)
		{
				printf("  *\n  *你输入的学生学号不存在  *\n  *\n");
				match_flag = 1;	
		}
		else
		{
			input_from_file = *check_node(input_from_usr_p->student_ID,student_info_head);
			match_flag = 0;
			printf("  *\n  *搜索成功\n  *");
		}			
			
		if(!match_flag)
		{									
			printf("\n"CYAN);
			printf("  **********************************************************************\n");
			printf("  *                           ~学生信息~           \n");
			printf("  *         	       学号      :%s\n",input_from_file.student_ID);		
			printf("  *         	       姓名      :%s\n",input_from_file.name);
			printf("  *         	       性别      :%s\n",input_from_file.sex);
			printf("  *         	       年级      :%s\n",input_from_file.grade);
			printf("  *         	       班别      :%s\n",input_from_file.group);
			printf("  *         	       电话号码  :%s\n",input_from_file.phone_number);
			printf("  *         	                ~成绩~\n");		
			printf("  *         	       C语言     :%s\n",input_from_file.C_sorce);
			printf("  *         	       高数      :%s\n",input_from_file.Mathematics_sorce);
			printf("  *         	       英语      :%s\n",input_from_file.English_sorce);
			printf("  *         	       物理      :%s\n",input_from_file.Physics_sorce);
			printf("  *         	       模电      :%s\n",input_from_file.Analog_sorce);
			printf("  *\n  **********************************************************************\n");										
		}		
				
		u8 buf[3];
		while(1)
		{
			printf("  **********************************************************************\n");
			printf("  *                     g:继续查找     q:返回主界面~                   *\n");printf("  *");		
			fgets(buf,3,stdin);
			if(buf[0] == 'q'){
				free(input_from_usr_p);
				free_each(student_info_head);
				return;
			}
			else if(buf[0] == 'g'){
				system("clear");break;
			}
			else
			{
				if( buf[0] != '\n' && buf[1] != '\n')	while(getchar() != '\n');
				printf(RED"  *输入有误请重新输入\n"CYAN);
			}
		}
	}		
}
//**************************成绩排名**************************
void sort_student_score()
{
	system("clear");
	u8 match_flag,lesson;
	//***********读取学生信息结构体存放到链表中***********
	student_info *student_info_head = read_student_info();

	while(1)
	{  	

		printf("\n"CYAN);
		printf("  **********************************************************************\n");
		printf("  *                    ~显示学生成绩排名信息~	         (q:返回主界面)*\n");
		printf("  **********************************************************************\n");
		printf("  *    a:C语言     b:高数     c:英语     d:物理     e:模电    f:总分   *\n");	
		printf("  **********************************************************************\n");	
		while(1)
		{	
			u8 buf[3];
			printf("  *      输入要显示的科目：(1~4):");	
			fgets(buf,3,stdin);
			if(buf[0] == 'q')
				return;
			else if(buf[0] <= 'f' && buf[0] >= 'a' && buf[1] == '\n')
			{
				lesson = buf[0];
				break;		
			}
			else
			{
				if( buf[0] != '\n' && buf[1] != '\n')	while(getchar() != '\n');
				printf("  *\n  ");printf(RED"*输入有误请重新输入\n");printf(CYAN"  *\n");
			}
		}	
		system("clear");
		printf("-------------------------------------------------------------------------------------\n");
		printf("|                                 ");
		switch(lesson){case 'a':printf("C语言");break;case 'b':printf("高数 ");break;case 'c':printf("英语 ");break;case 'd':printf("物理 ");break;case 'e':printf("模电 ");break;case 'f':printf("总分 ");break;				}
		printf(" 成绩排名                                    |\n");
		printf("-------------------------------------------------------------------------------------\n");
		printf("-------------------------------------------------------------------------------------\n");
		printf("|                        基本信息                       |         成绩排名          |\n");
		printf("-------------------------------------------------------------------------------------\n");
		printf("|    学号      姓名         性别  班级 班别   联系方式  |       ");
		switch(lesson){case 'a':printf("C语言");break;case 'b':printf("高数 ");break;case 'c':printf("英语 ");break;case 'd':printf("物理 ");break;case 'e':printf("模电 ");break;case 'f':printf("总分 ");break;				}	
		printf("    排名       |\n");
		printf("-------------------------------------------------------------------------------------\n");	
		//******************遍历学生某科信息*********	
		each_display_lesson_sort_info(student_info_head,lesson);
		printf("-------------------------------------------------------------------------------------\n");	
		
		u8 buf[3];
		while(1)
		{
			printf("  **********************************************************************\n");
			printf("  *                  g:显示其他科目     q:返回主界面~                  *\n");printf("  *");		
			fgets(buf,3,stdin);
			if(buf[0] == 'q' && buf[1] == '\n'){
				free_each(student_info_head);
				return;
			}
			else if(buf[0] == 'g'&& buf[1] == '\n')
				break;
			else
			{
				if( buf[0] != '\n' && buf[1] != '\n')	while(getchar() != '\n');
				printf("  *\n  ");printf(RED"*输入有误请重新输入\n");printf(CYAN"  *\n");
			}
		}
		system("clear");
	}			
}












