#ifndef _PROJECT_H_
#define	_PROJECT_H_

#include "kernel_list.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
// #include <curses.h>

#define USR_INFO 		"./usr_info"
#define STUDENT_INFO 	"./student_info"
#define FILE_PATH3 		"./student_info_src"
#define CYAN    		"\033[0;32;32m"//"\033[0;36m"
#define RED     		"\033[0;32;31m"

#define Too_Long  		0x01
#define Too_short   	0x02
#define Space_exist 	0x04

#define Char_examine 	'c'
#define Num_examine 	'n'

#define test 1

typedef unsigned char u8;
typedef unsigned int u32;

typedef struct _usr_info
{
	u8 account[10];
	u8 space[5];       //格式对齐，看得爽一点
	u8 password[10];
	u8 enter;          //格式对齐，看得爽一点
	
} usr_info, *usr_info_p;


typedef struct _student_info	//大结构体
{
	u8 student_ID[10];
	u8 space1;
	u8 name[25];
	u8 space2;
	u8 sex[6];
	u8 space3;
	u8 grade[2];
	u8 space4;
	u8 group[2];
	u8 space5;
	u8 phone_number[12];
	u8 space6[3];
	u8 C_sorce[4];
	u8 space7;
	u8 Mathematics_sorce[4];
	u8 space8;
	u8 English_sorce[4];
	u8 space9;
	u8 Physics_sorce[4];
	u8 space10;
	u8 Analog_sorce[4];
	u8 enter;
	struct list_head list;		//小结构体
	
} student_info;

u8 sign_up();											//注册账号

u8 login_account();							    		//登录账号

void register_student_info();//学生信息录入
						
void rewrite_student_info();//修改学生信息

void display_group_info();//显示班级成员信息

void delet_student_info();//删除学生信息

void search_student_info();//查找学生信息

void sort_student_score();//成绩排名

#endif








