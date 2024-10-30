#ifndef SERVER_H
#define SERVER_H
#include <event.h>
#include <string.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <jsoncpp/json/json.h>
#include <list>
#include <unistd.h>
#include <time.h>
#include "play.h"
#define IP "10.0.20.16"
#define PORT 8000
class PlayerServer {
private:
    struct event_base *base = NULL; // 事件集合
    struct evconnlistener *listener = NULL;
    static Player *p;
    static std::list<Node> *l;
public:
    // 带默认参数的构造函数
    PlayerServer(const char *ip = IP, int port = PORT);
    
    // 析构函数
    ~PlayerServer();

private:
    // 静态回调函数
    static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg);
    static void read_cb(struct bufferevent *bev, void *ctx);
    static void event_cb(struct bufferevent *bev, short what, void *ctx);
};

#endif // SERVER_H