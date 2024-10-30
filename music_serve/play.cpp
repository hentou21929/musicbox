#include<iostream>
#include"play.h"
void Player:: player_alive_info(std::list<Node>*l,struct bufferevent * bev,Json::Value val,struct event_base*base)
{   
    char deviceid[8]={0};
    strcpy(deviceid, val ["deviceid"].asString().c_str());
    for(std::list<Node>::iterator it = l->begin(); it != l->end(); ++it)
    {
        std::cout<<it->device_id<<std::endl;
        if(!strcmp(deviceid,it->device_id))
        {
			if (it ->online_flag ==0)//表示设备第一次发生保活消息
			{
				event_assign(&(it->timeout),base,-1,EV_PERSIST,timeout_cb,bev);//设置事件的参数
				struct timeval tv;
				evutil_timerclear(&tv);
				tv.tv_sec = 1;//一秒钟执行一次
				event_add(&(it->timeout),&tv);
				it->online_flag=1; //表示设备在线
			}
            it->device_bev=bev;
        
            it->time =time(NULL);
            std::cout<<"信息更新成功"<<std::endl;
            return;
        }
    }
    std::cout<<"收到保活消息 但是设备未绑定"<<std::endl;
}
//服务器收到app消息，再转发给音箱
void Player:: Player_operation(std::list<Node>*l,struct bufferevent * app_bev,const char *cmd)
{   
    Json::Value val;
	std::cout<<"playcmd:"<<cmd<<std::endl;
    if (!strcmp(cmd, "app_start"))
	{
		val["cmd"] = "start";
		std::cout<<"1"<<std::endl;
	}
	else if (!strcmp(cmd, "app_stop"))
	{
		val["cmd"] = "stop";
	}
	else if (!strcmp(cmd, "app_suspend"))
	{
		val["cmd"] = "suspend";
	}
	else if (!strcmp(cmd, "app_continue"))
	{
		val["cmd"] = "continue";
	}
	else if (!strcmp(cmd, "app_prior"))
	{
		val["cmd"] = "prior";
	}
	else if (!strcmp(cmd, "app_next"))
	{
		val["cmd"] = "next";
	}
	else if (!strcmp(cmd, "app_voice_up"))
	{
		val["cmd"] = "voice_up";
	}
	else if (!strcmp(cmd, "app_voice_down"))
	{
		val["cmd"] = "voice_down";
	}
	else if (!strcmp(cmd, "app_sequence"))
	{
		val["cmd"] = "sequence";
	}
	else if (!strcmp(cmd, "app_random"))
	{
		val["cmd"] = "random";
	}
	else if (!strcmp(cmd, "app_circle"))
	{
		val["cmd"] = "circle";
	}
	else if (!strcmp(cmd, "app_music"))
	{
		val["cmd"] = "music";
	}
    std::string str = Json::FastWriter().write(val);
	size_t ret;
	
	//遍历链表判断是否在线
	for (std::list<Node>::iterator it = l->begin(); it != l->end(); it++)
	{
		if (it->app_bev == app_bev)	
		{
			if (it->online_flag == 1)      //在线
			{
				ret = bufferevent_write(it->device_bev, str.c_str(), strlen(str.c_str()));	
				if (ret < 0)
				{
					std::cout << "bufferevent_write error" << std::endl;
				}
			}
			else
			{
				Json::Value v;
				v["cmd"] = "app_reply";
				v["result"] = "off_line";
				std::string s = Json::FastWriter().write(v);
				ret = bufferevent_write(app_bev, s.c_str(), strlen(s.c_str()));	
				if (ret < 0)
				{
					std::cout << "bufferevent_write error" << std::endl;
				}

			}
		}
		break;
	}
}
//音箱发送的消息
//收到音箱的回复消息，服务器回复app
//参数bev对应device
void Player::   Player_reply_result(std::list<Node>*l,struct bufferevent * bev,Json::Value val)
{
	char cmd[24]={0};
	strcpy(cmd, val ["cmd"].asString().c_str());
	if(!strcpy(cmd,"reply"))
	{
		val["cmd"]="app_reply";
	}else if (!strcpy(cmd,"reply_music"))
	{
		val["cmd"]="app_reply_music";
	}else if (!strcpy(cmd,"reply_status"))
	{
		val["cmd"]="app_reply_status";
	}
	size_t ret;
	std::cout<<cmd<<std::endl;
	std::string str =Json::FastWriter().write(val);
	//遍历链表，找到app对应的bufferent
	for(std::list<Node>::iterator it = l->begin();it!=l->end();it++)
	{
		if(it->device_bev==bev)
		{
			if(it->app_online_flag==1)
			{
				ret = bufferevent_write(it->app_bev,str.c_str(),strlen(str.c_str()));
				if(ret<0)
				{
					std::cout<<"bufferevent_write error"<<std::endl;
				}
			}
			return;
		}
		
	}
	std::cout<<"app不存在"<<std::endl;
}
   void  Player::timeout_cb(evutil_socket_t fd,short event, void *arg )
  {
	std::cout << "定时器事件" << std::endl;
	struct bufferevent *bev = (struct bufferevent*) arg;
	Json::Value val;
	val["cmd"] = "get";
	std::string str = Json::FastWriter().write(val);
	size_t ret = bufferevent_write(bev,str.c_str(),strlen(str.c_str()));
	if(ret <0)
	{
		std::cout<<"bufferent_write error"<<std::endl;
	}
  }