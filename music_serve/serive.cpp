#include<iostream>
#include"serive.h"
Player *PlayerServer::p=new Player();  //创建播放器对象
std::list<Node> *PlayerServer::l = new std::list<Node>();
PlayerServer::PlayerServer(const char * ip,int port)
{
    base  = event_base_new();//创建事件集合
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=inet_addr(ip);

    listener = evconnlistener_new_bind(base,listener_cb,base, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,10,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(listener == NULL)
    {
        std::cout<<"evconnlistener_new_bind error"<<std::endl;
    }
    event_base_dispatch(base);//监听集合
}
PlayerServer::~PlayerServer()
{   
    //释放对象
    evconnlistener_free(listener);
    event_base_free(base);

}
void PlayerServer::listener_cb(struct evconnlistener*listener,evutil_socket_t fd,struct sockaddr *addr,int socklen,void *arg )
{   
    struct event_base *base = (struct event_base *) arg;
    std::cout<<"有客户端连接 "<<fd<<std::endl;
    	//创建bufferevent事件
	struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (NULL == bev)
	{
		std::cout << "bufferevent_socket_new error" << std::endl;
	}

	bufferevent_setcb(bev, read_cb, NULL, event_cb, base);
	bufferevent_enable(bev, EV_READ);
}
void PlayerServer:: read_cb(struct bufferevent * bev,void *ctx)
{   
    struct event_base *base = (struct event_base *)ctx;
    char buf [1024] ={0};
    //std::list<Node> *l; // 指向 Node 的链表
    //l=new  std::list<Node>();  //创建链表对象
    size_t ret = bufferevent_read(bev,buf,sizeof(buf));
    if(ret<0)
    {   
        std::cout<<"bufferent_read error"<<std::endl;
    }
    //std::cout << buf << std::endl;
    Json::Reader reader;//解析json格式的对象
    Json::Value val;//解析结果
    bool result= reader.parse(buf,val);
    if(!result)
    {
        std::cout<<"解析json出错"<<std::endl;
        return;
    }
    char cmd[32] = {0};
    strcpy(cmd, val["cmd"].asString().c_str());
    //app发来的消息
    //std::cout<<"333"<<std::endl;
    //std::cout<<cmd<<std::endl;
    sleep(5);
    std::cout<<"servecmd:"<<cmd<<std::endl;
    if (!strcmp( cmd,"bind"))//app发来消息绑定
    {
       // std::cout<<"222"<<std::endl;
        Node n;
        n.app_bev = bev;
        strcpy(n.app_id,val["appid"].asString().c_str());
        strcpy(n.device_id,val["deviceid"].asString().c_str());
        n.online_flag = 0;
        n.app_online_flag=1;
        l->push_back(n);
        //回复app
        val["cmd"]="bind_success";
        std::string str=Json::FastWriter().write(val);
        size_t ret=bufferevent_write(bev,str.c_str(),strlen(str.c_str()));
        if(ret<0)
        {
            std::cout<<"send失败1"<<std::endl;
        }
    }

    if(!strcmp(cmd,"search_bind"))
    {
         std::cout<<"111111111111"<<std::endl;
        //判断链表中是否存在appid
        std::list<Node>::iterator it;
        for(it = l->begin();it!=l->end();it++)
        {
            std::cout<<"appid="<<it->app_id<<std::endl;
            if(!strcpy(it->app_id,val["appid"].asString().c_str()))
            {
                //更新app_bev
                it->app_bev=bev;
                it->app_online_flag =1;//app上线
                //appid 存在
                val["cmd"]="reply_bind";
                val["result"]="yes";
                break;
            }
        }
        if(it == l->end())//appid不存在
        {   
            std::cout<<"appid不存在"<<std::endl;
            val["cmd"]="reply_bind";
         
            val["result"]="no";
                
        }
        std::cout<<"val="<<val["result"]<<std::endl;
        std::string str=Json::FastWriter().write(val);
        size_t ret=bufferevent_write(bev,str.c_str(),strlen(str.c_str()));
        if(ret<0)
        {
            std::cout<<"send失败"<<std::endl;
        }
    }
    else if (!strcmp(cmd,"app_start"))//app发来消息开始播放
    {
        
        p->Player_operation(l,bev,cmd);
    }else if (!strcmp( cmd,"app_stop"))//app发来消息停止播放
    {
         p->Player_operation(l,bev,cmd);
    }else if (!strcmp( cmd,"app_suspend"))//app发来消息暂停播放
    {
        p->Player_operation(l,bev,cmd);
    }else if (!strcmp( cmd,"app_continue"))//app发来消息继续播放
    {
        p->Player_operation(l,bev,cmd);
    }else if (!strcmp( cmd,"app_prior"))//app发来消息上一首
    {
        p->Player_operation(l,bev,cmd);
    }else if (!strcmp( cmd,"app_next"))//app发来消息下一首
    {
        p->Player_operation(l,bev,cmd);
    }else if (!strcmp( cmd,"app_voice_up"))//app发来消息加大音量
    {
        p->Player_operation(l,bev,cmd);
    }else if (!strcmp( cmd,"app_voice_down"))//app发来消息减少音量
    {
        p->Player_operation(l,bev,cmd);
    }else if (!strcmp( cmd,"app_sequence"))//app发来消息顺序播放
    {
        p->Player_operation(l,bev,cmd);
    }else if (!strcmp( cmd,"app_random"))//app发来消息随机播放
    {
        p->Player_operation(l,bev,cmd);
    }else if (!strcmp( cmd,"app_circle"))//app发来消息单曲循环
    {
        p->Player_operation(l,bev,cmd);
    }else if (!strcmp( cmd,"app_music"))//app发来消息获取音乐信息
    {       
        p->Player_operation(l,bev,cmd);
    }//音箱发过来的消息
    else if (!strcmp( cmd,"reply"))//操作成功的回复
    {
          p->Player_reply_result(l,bev,val);

    }else if (!strcmp( cmd,"info"))//音箱的保活消息
    {
        
       p->player_alive_info(l,bev,val,base);
    }else if (!strcmp( cmd,"reply_status"))//回复播放器状态
    {
          p->Player_reply_result(l,bev,val);
    }else if (!strcmp( cmd,"reply_music"))//回复所有音乐
    {
          p->Player_reply_result(l,bev,val);
    }

}
void PlayerServer:: event_cb(struct bufferevent *bev, short what,void *ctx)
{

    if(what & BEV_EVENT_EOF)
    {
        std::cout<<"客户端下线"<<std::endl;
        for(std::list<Node>::iterator it = l->begin();it!=l->end();it++)
        {
            if(it->device_bev == bev)
            {
                std::cout<<"音箱下线"<<std::endl;
                it->online_flag =0;
                event_del(&(it->timeout));//移除定时器
                return;
            }
            if(it->app_bev==bev)
            {
                std::cout<<"app下线"<<std::endl;
                it->app_online_flag=0;
            }
        
        

        }
    }else
    {
        std::cout<<"异常发生"<<std::endl;
    }
}
