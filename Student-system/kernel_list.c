#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kernel_list.h"


int i = 0;

//节点设计
struct node
{
	int data;
	struct list_head list;   //小结构体变量
};

struct node *kernel_node_Init(void)
{
	struct node *p;
	
	p = (struct node *)malloc(sizeof(struct node));
	if(p == NULL)
	{
		printf("malloc failure \n");
		return NULL;
	}
	
	//初始化小结构体，让next prev指向自己
	INIT_LIST_HEAD(&p->list);

	
	p->data = 0;

	return p;
}

void insert_node(struct list_head *list)
{
	
	struct node *new;
	//生成一个节点
	new = kernel_node_Init();
	
	printf("请输入第%d节点数据：\n",++i);
	
	scanf("%d",&new->data);
	printf("第%d节点地址:%p\n",i,new);
	list_add(&new->list,list);
	

}

void display_node(struct node *big_head)
{
	struct list_head *pos; //移动的小结构体中间指针变量
	struct node *tmp;      //大结构体指针蛮
	
	list_for_each(pos, &big_head->list)
	{
		tmp = list_entry(pos, struct node, list);
		printf("tmp:%p;data:%d\n",tmp,tmp->data);
	}
	printf("\n");
	
}

//查找节点
struct node *check_node(int data,struct node *L)
{
	struct list_head *pos,*n;
	struct node *q;
	list_for_each_safe(pos,n, &(L->list))
	{
		q = list_entry(pos, struct node, list);
		if(q->data == data)
		{
			return q;
		}
	}
	return NULL;
}

//移动节点
void move_node(struct node *temp,struct node *ert)
{
	list_move(&(temp->list),&(ert->list));
}


int main(void)
{
	char ch;
	int data;
	struct node *big_head,*temp,*ert;
	
	big_head = kernel_node_Init();
	
	while(1)
	{	
		printf("请选择功能输入：i 插入节点; p 显示链表; f 查找链表; m 移动节点 d 删除节点; a 删除整个链表;q 退出程序\n");
		scanf("%c",&ch);
		if(ch == 'q')
			break;
		
		switch(ch)
		{
			case 'i':
					//插入数据
					insert_node(&big_head->list);
		
				break;			
			case 'p':
					//插入数据
					display_node(big_head);
				break;	

			case 'f':
					printf("请输入你要查找的数据：\n");
					scanf("%d",&data);
					temp = check_node(data,big_head);
					if(temp == NULL)
					{
						printf("节点中查无此数据！\n");
					}
					else
					{
						printf("节点中有此数据！\n");
						printf("temp:%p, data:%d\n",temp,temp->data);
					}
				break;
			case 'm':
					display_node(big_head);
					printf("\n请输入你要移动的数据：\n");
					scanf("%d",&data);
					temp = check_node(data,big_head);
					if(temp == NULL)
					{
						printf("节点中查无此数据！\n");
						break;
					}
					else
					{
						printf("节点中有此数据！\n");
						printf("temp:%p, data:%d\n",temp,temp->data);
					}
					
					printf("请输入你要放置的地方数据：\n");					
					scanf("%d",&data);
					ert = check_node(data,big_head);
					if(ert == NULL)
					{
						printf("节点中查无此数据！\n");
					}
					else
					{
						printf("节点中有此数据！\n");
						printf("temp:%p, data:%d\n",ert,ert->data);
					}
					if(temp != NULL && ert != NULL)
					{
						move_node(temp,ert);
						display_node(big_head);
					}				
				break;
			default:
					printf("无些操作选项，请重新选择功能！\n");
				break;
		}
		while(getchar() != '\n');
	}		
	
	return 0;
}