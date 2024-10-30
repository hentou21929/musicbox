#ifndef PLAYER_H
#define PLAYER_H
#include <unistd.h>
#define MUSICPATH  "/home/hhh/Music/"
void GetMusic();
int InitShm();
#define SHMKEY      1234
#define SHMSIZE     4069
#define SEQUECEMODE 1
#define RANDOM      2
#define CIRCLE      3
#define COMMAND_SIZE 128
void start_play();
void stop_play();
void suspend_play();//暂停播放
void continue_play();
void prior_play();
void  next_play();
void voice_dowm();
void voice_up();
void set_mode(int a);
int get_volume();
//共享内存数据
struct shm
{
    int play_mode;
    char cur_name[64];
    pid_t ppid;
    pid_t child_pid;
    pid_t grand_pid;
};
typedef struct shm shm;

#endif