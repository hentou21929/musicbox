#ifndef PLAY_H
#define PLAY_H
#include<list>
#include"node.h"
#include <jsoncpp/json/json.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include<string.h>
#include<time.h>
#include<event2/util.h>
class Player
{
private:
public:
    void player_alive_info(std::list<Node>*l,struct bufferevent * bev,Json::Value val,struct event_base*base);
    void Player_operation(std::list<Node>*l,struct bufferevent * app_bev,const char * cmd);
    void Player_reply_result(std::list<Node>*l,struct bufferevent * bev,Json::Value val);
    static void timeout_cb(evutil_socket_t fd,short event, void *arg );
};
#endif
