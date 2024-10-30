#include<stdio.h>
 /* According to POSIX.1-2001 */
#include <sys/select.h>
       /* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include"player.h"
#include <json-c/json.h>
#include <string.h>
#include <sys/socket.h>
#include "socket.h"
#include <errno.h>
#include <string.h>
#include"socket.h"
#include<errno.h>
#include"device.h"
extern int g_buttonfd;
extern int g_sockfd;
void start_play();
void stop_play();
void suspend_play();
void prior_play();
void  next_play();
void voice_dowm();
void voice_up();
void continue_play();
void set_mode(int a);
int g_maxfd = 0;
fd_set readfd;
void parse_message(const char *m, char *c)
{
	struct json_object *obj = json_tokener_parse(m);

	struct json_object *json;
	json_object_object_get_ex(obj, "cmd", &json);
	//json = json_object_object_get(obj, "cmd");
	strcpy(c, json_object_get_string(json));
}

void show()
{
            printf("1开始播放\n");
            printf("2结束播放\n");
            printf("3暂停播放\n");
            printf("4继续播放\n");
            printf("5上一首\n");
            printf("6下一首\n");
            printf("7增加音量\n");
            printf("8减少音量\n");
            printf("9顺序播放\n");
            printf("10随机播放\n");
            printf("11单曲循环\n");

}
void m_select()
{
    show();
    fd_set tmpfd;
    int ret;
   // int maxfd = (g_buttonfd>g_sockfd) ? g_buttonfd:g_sockfd;
   
    FD_ZERO (&readfd);
    FD_ZERO (&tmpfd);
    //FD_SET(g_buttonfd,&readfd);
    FD_SET(g_sockfd,&readfd);
  //  FD_SET(0,&readfd);//标准IO
    char message[1024]={0};
    while(1)
    {
        tmpfd=readfd;
        ret  = select(g_maxfd+1,&tmpfd,NULL,NULL,NULL);
        if(ret==-1 && errno !=EINTR)
        {
            perror("select");
        }
        if(FD_ISSET(g_sockfd,&tmpfd))//tcp有数据可读
        {
            memset(message,0,sizeof(message));
            ret =recv(g_sockfd,message,sizeof(message),0);
            if(-1==ret)
            {
                perror("recv");
            }
            char cmd[64]={0};
            parse_message(message,cmd);
            if(!strcmp(cmd,"start"))
            {
                socket_start_play();
            }
            else if(!strcmp(cmd,"stop"))
            {
                socket_stop_play();
            }else if(!strcmp(cmd,"suspend"))
            {
                socket_suspend_play();
            }else if(!strcmp(cmd,"continue"))
            {
                socket_continue_play();
            }else if(!strcmp(cmd,"prior"))
            {
                socket_prior_play();
            }else if(!strcmp(cmd,"next"))
            {
                socket_next_play();
            }else if(!strcmp(cmd,"voice_up"))
            {
                socket_voice_up();
            }else if(!strcmp(cmd,"voice_down"))
            {
                socket_voice_down();
            }else if(!strcmp(cmd,"sequence"))
            {
                socket_mode_play(SEQUECEMODE);
            }
            else if(!strcmp(cmd,"random"))
            {
                socket_mode_play(RANDOM);
            }else if(!strcmp(cmd,"circle"))
            {
                socket_mode_play(CIRCLE);
            }else if(!strcmp(cmd,"get"))
            {
               socket_get_status();
            }else if(!strcmp(cmd,"music"))//获取所有音乐
            {
                socket_get_music();
            }
        }else if(FD_ISSET(g_buttonfd,&tmpfd))//收到按键数据
        {
            int id ;
            id =get_key_id();
            switch(id)
            {
                case 1:
                    start_play();
                    break;
                case 2:
                    stop_play();
                    break;
                case 3:
                    suspend_play();
                    break;
                case 4:
                    continue_play();
                    break;
                case 5:
                    prior_play();
                    break;
                case 6:
                    next_play();
                    break;
                case 0:
                    break;
                            
            }
        }
        /*else if(FD_ISSET(0,&tmpfd))//收到键盘数据
        {   
            int func;
            scanf("%d",&func);
            switch(func)
            {
                case 1:
                    start_play();
                    break;
                case 2:
                    stop_play();
                    break;
                case 3:
                    suspend_play();
                    break;
                case 4 :
                    continue_play();
                    break;
                case 5:
                    prior_play();
                    break;
                case 6:
                    next_play();
                    break;

                case 7:
                    voice_up();
                    break;
                case 8:
                    voice_dowm();
                    break;
                case 9:
                    set_mode(SEQUECEMODE);
                    break;
                case 10:
                    set_mode(RANDOM);
                    break;
                case 11:
                    set_mode(CIRCLE);
                    break;
                default:
                    printf("未知的操作或无效的输入。\n");
                    break;                    
                
            }
        }*/

    }
}
