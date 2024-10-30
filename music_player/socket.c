//网络模块
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include"main.h"
#include<pthread.h>
#include <string.h>
#include"device.h"
#include<unistd.h>
#include <netinet/in.h>
#include"socket.h"
#include <arpa/inet.h>
#include<signal.h>
#include <json-c/json.h>
#include "player.h"
  /* According to POSIX.1-2001 */
#include <sys/select.h>
#include"link.h"
       /* According to earlier standards */
#include <sys/time.h>
extern int g_maxfd;
extern int g_suspend_flag;
extern int g_statr_flag;
extern  int g_sockfd;
extern fd_set readfd;
extern void *g_addr;
extern Node*head;
//5秒执行一次,向服务器发送alive 字符串

void send_server(int sig)
{
    struct json_object * json=json_object_new_object();
    json_object_object_add(json,"cmd",json_object_new_string("info"));
    json_object_object_add(json,"status",json_object_new_string("alive"));
    json_object_object_add(json,"deviceid",json_object_new_string("001"));
    const char *buf =json_object_to_json_string(json);
    int ret = send(g_sockfd,buf,strlen(buf),0);
    if(-1==ret)
    {
        perror("send");
    }
    alarm(5);
}
void *cnnect_cb(void *arg)
{
    int count = 5,ret;
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port =SERVER_PORT;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    while(count--)
    {
        ret = connect(g_sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
        if(ret==-1)
        {   sleep(5);
            continue;
        }
        //连接成功
        led_on(0);
        led_on(1);
        led_on(2);
        led_on(3);
        //5秒后向进程发送SIGALRM信号

       
        alarm(5);
        signal(SIGALRM,send_server);
        //连接服务器成功,fd 添加到集合
        FD_SET(g_sockfd,&readfd);
        
        break;    
    }
    return NULL;
}
int InitSocket()
{
    g_sockfd =socket(AF_INET,SOCK_STREAM,0);
    if(g_sockfd==-1)
    {
        return FAILURE;    
    }
    //启动线程向服务器发起连接
    pthread_t tid;
    int ret = pthread_create  (&tid,NULL,cnnect_cb,NULL);
    if(ret !=0)
    {
        return FAILURE;
    }
    if(g_sockfd>g_maxfd)
    {
        g_maxfd = g_sockfd;
    }
    return SUCCESS;
}
 void socket_start_play()
 {
    start_play();
    struct json_object * json=json_object_new_object();
    json_object_object_add(json,"cmd",json_object_new_string("reply"));
    json_object_object_add(json,"result",json_object_new_string("start_success"));
    
    const char*buf=json_object_to_json_string(json);
    int ret = send(g_sockfd,buf,strlen(buf),0);
    if(ret==-1)
    {
        perror("send");
    }
 }
  void socket_stop_play()
 {
    stop_play();
    struct json_object * json=json_object_new_object();
    json_object_object_add(json,"cmd",json_object_new_string("reply"));
    json_object_object_add(json,"result",json_object_new_string("stop_success"));
    const char*buf=json_object_to_json_string(json);
    int ret = send(g_sockfd,buf,strlen(buf),0);
    if(ret==-1)
    {
        perror("send");
    }
 }
  void socket_suspend_play()
 {
    suspend_play();
    struct json_object * json=json_object_new_object();
    json_object_object_add(json,"cmd",json_object_new_string("reply"));
    json_object_object_add(json,"result",json_object_new_string("suspend_success"));
    const char*buf=json_object_to_json_string(json);
    int ret = send(g_sockfd,buf,strlen(buf),0);
    if(ret==-1)
    {
        perror("send");
    }
 }
  void socket_continue_play()
 {
    continue_play();
    struct json_object * json=json_object_new_object();
    json_object_object_add(json,"cmd",json_object_new_string("reply"));
    json_object_object_add(json,"result",json_object_new_string("continue_success"));
    const char*buf=json_object_to_json_string(json);
    int ret = send(g_sockfd,buf,strlen(buf),0);
    if(ret==-1)
    {
        perror("send");
    }
 }
  void socket_prior_play()
 {
    prior_play();
    struct json_object * json=json_object_new_object();
    json_object_object_add(json,"cmd",json_object_new_string("reply"));
    json_object_object_add(json,"result",json_object_new_string("success"));
    const char*buf=json_object_to_json_string(json);
    int ret = send(g_sockfd,buf,strlen(buf),0);
    if(ret==-1)
    {
        perror("send");
    }
 }
  void socket_next_play()
 {
    next_play();
    struct json_object * json=json_object_new_object();
    json_object_object_add(json,"cmd",json_object_new_string("reply"));
    json_object_object_add(json,"result",json_object_new_string("success"));
    const char*buf=json_object_to_json_string(json);
    int ret = send(g_sockfd,buf,strlen(buf),0);
    if(ret==-1)
    {
        perror("send");
    }
 }
  void socket_voice_up()
 {
    voice_up();
    struct json_object * json=json_object_new_object();
    json_object_object_add(json,"cmd",json_object_new_string("reply"));
    json_object_object_add(json,"result",json_object_new_string("success"));
    const char*buf=json_object_to_json_string(json);
    int ret = send(g_sockfd,buf,strlen(buf),0);
    if(ret==-1)
    {
        perror("send");
    }
 }
  void socket_voice_down()
 {
    voice_dowm();
    struct json_object * json=json_object_new_object();
    json_object_object_add(json,"cmd",json_object_new_string("reply"));
    json_object_object_add(json,"result",json_object_new_string("success"));
    const char*buf=json_object_to_json_string(json);
    int ret = send(g_sockfd,buf,strlen(buf),0);
    if(ret==-1)
    {
        perror("send");
    }
 }
  void socket_mode_play(int a)
 {
    set_mode(a);
    struct json_object * json=json_object_new_object();
    json_object_object_add(json,"cmd",json_object_new_string("reply"));
    json_object_object_add(json,"result",json_object_new_string("success"));
    const char*buf=json_object_to_json_string(json);
    int ret = send(g_sockfd,buf,strlen(buf),0);
    if(ret==-1)
    {
        perror("send");
    }
 }
 void socket_get_status()
{
    //获取播放状态，当前歌曲名
     struct json_object * json=json_object_new_object();
    json_object_object_add(json,"cmd",json_object_new_string("reply_status"));
    if(g_statr_flag==1&&g_suspend_flag==0)
    {
        json_object_object_add(json,"status",json_object_new_string("statr"));

    }else if(g_statr_flag==1&&g_suspend_flag==1)//暂停状态
    {
        json_object_object_add(json,"status",json_object_new_string("suspend"));

    }else if(g_suspend_flag==0)
    {
        json_object_object_add(json,"status",json_object_new_string("stop"));
    }
    json_object_object_add(json,"voice",json_object_new_int(get_volume()));
    shm s;
    memset(&s,0,sizeof(s));
    memcpy(&s,g_addr,sizeof(s));
     json_object_object_add(json,"musie_name",json_object_new_string(s.cur_name));
    const char *buf = json_object_to_json_string(json);

    int ret = send(g_sockfd,buf,strlen(buf),0);
    
     if(ret==-1)
    {
        perror("send");
    }
}
void  socket_get_music()
{
    struct json_object * json=json_object_new_object();
    json_object_object_add(json,"cmd",json_object_new_string("reply_music"));
    struct json_object * array=json_object_new_array();
    Node * p=head->next;
    while(p!=head)
    {   
        json_object_array_add(array,json_object_new_string(p->music_name));
        p=p->next;
    }
    json_object_object_add(json,"music",array);
    const char *buf = json_object_to_json_string(json);

    int ret = send(g_sockfd,buf,strlen(buf),0);
    
     if(ret==-1)
    {
        perror("send");
    }

}