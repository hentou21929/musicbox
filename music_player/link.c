#include<stdio.h>
#include<stdlib.h>
#include"main.h"
#include"link.h"
#include"player.h"
#include <string.h>
#include <time.h>

extern struct Node *head;
//初始化双向循环链表
int InitLink()
{   

    head=(Node * ) malloc( sizeof(Node) *1);
    if(head==NULL)
    {
        return FAILURE;
    }
    head->next=head;
    head->prior =head;
    return SUCCESS;
}
int InsertLink(Node *h,const char *name)
{
    if(h==NULL||name==NULL)
    {
        return FAILURE;
    }
    Node *end =h->prior;
    Node *n = (Node*)malloc(sizeof(Node)*1);

    if(n==NULL)
    {
        return FAILURE;
    }
    n->next=h;
    strcpy( n->music_name,name);
    end->next=n;
    n->prior=end;
    h->prior=n;
    //p->next=n;
    return SUCCESS;
}
//根据当前歌曲和播放模式，找到下一首歌
//cur:当前歌曲
//mode:播放模式
//next : 存放下一首歌歌名
void FindNextMusic(const char *cur,int mode,char * next)
{   //单曲循环

     if(mode == CIRCLE)
    {   
        strcpy(next,cur);
        return;
    }else if(mode == SEQUECEMODE)//顺序播放
    {
        Node *p =head->next;
        while(strcmp(p->music_name,cur)!=0)
        {
            p=p->next;
        }
        
       	if (p->next == head)          //忽略头结点
		{
			strcpy(next, head->next->music_name);
		}
		else
		{
			strcpy(next, p->next->music_name);
		}
        return ;
    }else//随机播放 
    {   
        Node * p =head->next;
        srand(time(NULL));
        int num =rand()&3;
        int i ;
        for(i=0;i<num;i++)
        {
            p=p->next;
        }	
        if (p == head)
		{
			strcpy(next, head->next->music_name);
		}
		else
		{
			strcpy(next, p->music_name);
		}

        
        return ;
    }
}
void priorMusic(const char *cur,int mode,char * prior)
{
     //单曲循环

    if(mode == SEQUECEMODE|| mode == CIRCLE)//顺序播放
    {
        Node *p =head->next;
        while(strcmp(p->music_name,cur)!=0)
        {
            p=p->next;
        }
	    if (p == head->next)//忽略头节点
		{
			strcpy(prior, head->prior->music_name);
		}
		else
		{
			strcpy(prior, p->prior->music_name);
		}
        return ;
    }else//随机播放 
    {   
        Node * p =head->next;
        srand(time(NULL));
        int num =rand()&3;
        int i ;
        for(i=0;i<num;i++)
        {
            p=p->next;
        }	
        if (p == head)//忽略头节点
		{
			strcpy(prior, head->next->music_name);
		}
		else
		{
			strcpy(prior, p->music_name);
		}
        return ;
    
    }
}
void	nextMusic(const char *cur,int mode,char * next)
{
    if(mode == SEQUECEMODE|| mode == CIRCLE)//顺序播放
    {
        Node *p =head->next;
        while(strcmp(p->music_name,cur)!=0)
        {
            p=p->next;
        }
	    if (p == head)//忽略头节点
		{
			strcpy(next, head->next->music_name);
		}
		else
		{
			strcpy(next, p->next->music_name);
		}
        return ;
    }else//随机播放 
    {   
        Node * p =head->next;
        srand(time(NULL));
        int num =rand()&3;
        int i ;
        for(i=0;i<num;i++)
        {
            p=p->next;
        }	
        if (p == head)//忽略头节点
		{
			strcpy(next, head->next->music_name);
		}
		else
		{
			strcpy(next, p->music_name);
		}
        return ;
    
    }
}