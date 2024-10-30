#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "main.h"
#include<sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>  // 包含 read 和 perror 函数的声明
#include <fcntl.h>   // 如需打开文件描述符，可能需要此头文件
extern int g_buttonfd;
extern int g_ledfd;
extern int g_maxfd;
extern int g_mixerfd;
int InitDriver()
{
   /* g_buttonfd =open("/dev/buttons",O_RDONLY);
    if(g_buttonfd==-1)
    {
        return FAILURE;
    }
    g_ledfd  =open("/dev/leds",O_WRONLY);
    if(-1==g_ledfd)
    {
        return FAILURE;
    }
    //所有led灭
   
    int i;
    for( i =0;i<4;i++)
    {
        ioctl(g_ledfd,0,i);
    }
    */
    g_mixerfd=open("/dev/mixer",O_WRONLY);
    
    if(g_mixerfd==-1)
    {
        return FAILURE;
    }
    if(g_mixerfd > g_maxfd)
    {
        g_maxfd = g_mixerfd;
    }
    return SUCCESS;
}
void led_on(int which)
{
    ioctl(g_ledfd,1,which);
}
void led_off(int which)
{
    ioctl(g_ledfd,0,which);
}
int get_key_id()
{   
    char buttons[6]={'0','0','0','0','0','0'};
    char cur_button[6]={0};
    int ret;
    ret=read(g_buttonfd,cur_button,sizeof(cur_button));
    if(-1==ret)
    {
        perror("read");
    }
    int i;
    for(i=0;i<6;i++)
    {
        if(cur_button[i]!=buttons[i]);
        return i+1;
    }
    return 0;
    
}