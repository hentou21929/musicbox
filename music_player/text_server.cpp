#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <json-c/json.h>
#include <pthread.h>

void *receive(void *arg)
{
	int fd =*(int *) arg;
	char buf [1024]={0};
	while(1)
	{
		int ret = recv(fd,buf,sizeof(buf),0);
		struct json_object *json =json_tokener_parse(buf);
		struct json_object *obj;
		json_object_object_get_ex(json,"cmd",&obj);
		char cmd[32]={0};
		strcpy(cmd,json_object_get_string(obj));
		if(!strcmp(cmd,"reply"))
		{
			printf("操作成功\n");
			printf("%s\n",buf);
		}else if(!strcmp(cmd,"info"))
		{
			printf("%s\n",buf);
		}
		else if(!strcmp(cmd,"reply_status"))
		{
			printf("%s\n",buf);
		}
		else if(!strcmp(cmd,"reply_music"))
		{
			printf("%s\n",buf);
		}
	}
}
int main()
{	
	//创建socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);   //ipv4协议  流式套接字   具体的协议类型
	if (-1 == sockfd)
	{
		perror("socket");
		exit(1);
	}

	int opt = 1; 
	setsockopt(sockfd,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));   //地址可以被重复绑定

	struct sockaddr_in server_addr;    //保存服务器的信息

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = 8000;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //127.0.0.1回环ip 表示本机 测试时候可以用 
	//绑定信息
	int ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (-1 == ret)
	{
		perror("bind");
		exit(1);
	}

	//设置监听队列
	ret = listen(sockfd, 10);
	if (-1 == ret)
	{
		perror("listen");
		exit(1);
	}

	printf("等待客户端的连接...\n");

	struct sockaddr_in client_addr;   //用于保存客户端的信息
	int length = sizeof(client_addr);
	//接受连接（建立TCP连接）
	int fd = accept(sockfd, (struct sockaddr *)&client_addr,(socklen_t *) &length);
	if (-1 == fd)
	{
		perror("accept");
		exit(1);
	}
	printf("接受客户端的连接 %d\n", fd);
	pthread_t tid;
	ret = pthread_create(&tid,NULL,receive,&fd);
	
	char *buf = (char *)malloc(sizeof(char) * 1024);

	//从fd接收消息，TCP连接相当于一个文件，fd就是文件描述符，从fd读取数据，就是从TCP连接接收数据
	while(1)
	{	
		scanf("%s",buf);
		struct json_object *json =json_object_new_object();
		json_object_object_add(json,"cmd",json_object_new_string(buf));
		const char * s = json_object_to_json_string(json);
		ret = send(fd,s,strlen(s),0);

	}

	close(fd);
	close(sockfd);

	return 0;
}
