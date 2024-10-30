#include<stdio.h>
#include"link.h"
#include <sys/types.h>
#include<stdlib.h>
#include <dirent.h>
#include"player.h"
#include "main.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>  
#include <signal.h>

  void start_play();    
int shmid;
extern Node * head;
void * g_addr=NULL;  //共享内存映射地址
int g_statr_flag = 0;//表示没又开始播放音乐
int g_suspend_flag = 0 ;//表示没有暂停音乐
//判断是不是MP3文件
int m_mp3_end(const char *name)
{
    const char * ptr=name;  
    while(*ptr!='\0')
    {
        ptr++;
    }
    int i;
    for(i=0;i<4;i++)
    {
        ptr--;
    }
    if(ptr<name)
    {
        return 0;
    }
    return (strcmp(ptr,".mp3")==0)?1:0;
}
void GetMusic()
{
    //打开目录
    DIR *dir = opendir(MUSICPATH);
    if(NULL==dir)
    {
        perror("opendir");
        exit(1);
    }
    //读取mul
    struct dirent* file;
    while((file=readdir(dir))!=NULL)
    {
        if(file->d_type!=8)//不是普通文件
        {
            continue;
        }
        if(!m_mp3_end(file->d_name))//不是mp3文件
        {
            continue;
        }

        printf("%s\n",file->d_name);
        int ret = InsertLink(head,file->d_name);
        if(ret==FAILURE)
        {
            printf("歌曲插入失败\n");
            exit(1);
        }
    }
}
void play_music(const char *name)
{
	pid_t child_pid = fork();
	if (-1 == child_pid)
	{
		perror("fork");
		exit(1);
	}
	else if (0 == child_pid)         //子进程
	{
		
		while (1)
		{
			pid_t grand_pid = vfork();
			if (-1 == grand_pid)
			{
				perror("fork");
				exit(1);
			}
			else if (0 == grand_pid)    //孙进程
			{
				char cur_name[64] = {0};
				shm s;

				//获取共享内存
				int shmid = shmget(SHMKEY, SHMSIZE, 0);
				if (-1 == shmid)
				{
					perror("shmget");
					exit(1);
				}

				//映射
				void *addr = shmat(shmid, NULL, 0);
				if (NULL == addr)
				{
					perror("shmat");
					exit(1);
				}

				if (strlen(name) != 0)      //直接开始播放
				{
					strcpy(cur_name, name);	
				}
				else                        //遍历链表，找到一首歌播放
				{
					//判断播放模式，找到一首歌曲
					memcpy(&s, addr, sizeof(s));
					FindNextMusic(s.cur_name, s.play_mode, cur_name);
				}

				//把信息写入共享内存（父子孙进程号、当前歌曲名）
				memcpy(&s, addr, sizeof(s));
				strcpy(s.cur_name, name);
				s.child_pid = getppid();
				s.grand_pid = getpid();
				memcpy(addr, &s, sizeof(s));
				shmdt(addr);           //解除映射
				char music_path[128] = {0};     //包含路径的歌曲名称
				strcpy(music_path, MUSICPATH);
				strcat(music_path, cur_name);
				printf("歌曲名字 %s\n", music_path);
				execl("/usr/bin/madplay", "madplay", "-q",music_path, NULL);
				printf("1111111111111111111\n");
			}
			else                        //子进程
			{
				int status;
				waitpid(grand_pid, &status, 0);        //回收孙进程
				memset((void *)name, 0, strlen(name));         //歌曲名长度变为0，方便下一次操作

				
				
			}
		}
	}
	else
	{
		return;
	}
}

void start_play()
{
    if(g_statr_flag==1)//已经开始播放
    {
        return ;
    }
    //获取歌曲名称
    if(head->next==NULL)//链表为空
    {
        return ;
    }
    //开始播放音乐
  char name[128]={0};
  strcpy(name,head->next->music_name);
  play_music(name);
  g_statr_flag = 1;

}
void stop_play()
{
	if (g_statr_flag == 0)
	{
		return;
	}

	//读取共享内存，获取pid
	shm s;
	memset(&s, 0, sizeof(s));
	memcpy(&s, g_addr, sizeof(s));

	//kill(s.grand_pid, SIGKILL);    //结束子进程
	//kill(s.child_pid, SIGKILL);    //结束孙进程
	
	kill(s.child_pid, SIGKILL);      //结束子进程
	kill(s.grand_pid, SIGKILL);      //结束孙进程
	
	g_statr_flag = 0;
}
void suspend_play()//暂停播放
{
	if(g_suspend_flag==1||g_statr_flag==0)
	{
		return;
	}
	//读取共享内存获取pid
	shm s;
	memset(&s, 0, sizeof(s));
	memcpy(&s, g_addr, sizeof(s));
	//暂停孙进程
	kill(s.grand_pid,SIGSTOP);
	//暂停子进程
	kill(s.child_pid,SIGSTOP);
	g_suspend_flag=1;
}
void continue_play()
{
	if(g_statr_flag==0||g_suspend_flag==0)
	{
		return;
	}
		//读取共享内存获取pid
	shm s;
	memset(&s, 0, sizeof(s));
	memcpy(&s, g_addr, sizeof(s));
	//继续孙进程
	kill(s.grand_pid,SIGCONT);
	//继续子进程
	kill(s.child_pid,SIGCONT);
	g_suspend_flag=0;
}
void prior_play()
{
	if(g_statr_flag==0||g_suspend_flag==1)
	{
		return;
	}
		//读取共享内存获取pid
	shm s;
	memset(&s, 0, sizeof(s));
	memcpy(&s, g_addr, sizeof(s));
	kill(s.child_pid, SIGKILL);      //结束子进程
	kill(s.grand_pid, SIGKILL);      //结束孙进程
	g_statr_flag = 0;
	char name[64]={0};
	priorMusic(s.cur_name,s.play_mode,name);
	play_music(name);
	g_statr_flag=1;
}
void  next_play()
{
	if(g_statr_flag==0||g_suspend_flag==1)
	{
		return;
	}
		//读取共享内存获取pid
	shm s;
	memset(&s, 0, sizeof(s));
	memcpy(&s, g_addr, sizeof(s));
	kill(s.child_pid, SIGKILL);      //结束子进程
	kill(s.grand_pid, SIGKILL);      //结束孙进程
	g_statr_flag = 0;
	char name[64]={0};
	nextMusic(s.cur_name,s.play_mode,name);
	play_music(name);
	g_statr_flag=1;
}
void voice_dowm() {


	int result = system("amixer set Master 5%-");
    if (result == -1) {
        perror("执行命令失败");
    }
	result = system("amixer get Master");
    if (result == -1) {
        perror("执行命令失败");
    }
	
	
}

void voice_up() {
    int result = system("amixer set Master 5%+");
    if (result == -1) {
        perror("执行命令失败");
    }
		result = system("amixer get Master");
    if (result == -1) {
        perror("执行命令失败");
    }
	
}
void set_mode(int a)
{
	shm s;
	memset(&s, 0, sizeof(s));
	memcpy(&s, g_addr, sizeof(s));
	s.play_mode=a;
	memcpy(g_addr,&s,sizeof(s));
	printf("%d模式修改成功\n",a);
}
// 获取当前音量的函数
// 获取当前音量的函数
int get_volume() {
        FILE *fp;
    char command[] = "amixer get Master";
    char buffer[128];
    int volume = -1; // 默认音量为-1代表出错

    // 使用 popen 执行命令
    fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to run command\n");
        return volume; // 返回-1表示出错
    }

    // 读取命令输出
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // 查找“Front Left:”的音量
        if (strstr(buffer, "Front Left:") != NULL) {
            // 提取音量值
            char *volume_str = strstr(buffer, "[") + 1; // 找到 [ 的位置，然后移动到下一个字符
            sscanf(volume_str, "%d", &volume);
            break;
        }
    }

    // 关闭命令管道
    pclose(fp);

    return volume; // 返回音量值
}

int InitShm()
{   
    //创建共享内存
     shmid = shmget(SHMKEY,SHMSIZE,IPC_CREAT | IPC_EXCL | 0666);
    if(shmid == -1)
    {
        printf("11111111111\n");
        return FAILURE;
    }
    //映射
    g_addr = shmat(shmid,NULL,0);
    if(g_addr==NULL)
    {   
        printf("22222222222\n");
        return FAILURE;
    }
    //初始化共享内存数据
    shm s;
    s.play_mode =SEQUECEMODE;
    s.ppid=getpid();

    memcpy((char *)g_addr,&s,sizeof(s) );
return SUCCESS;

}
