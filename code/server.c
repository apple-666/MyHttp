//服务器模块实现
#include<pthread.h>
#include<sys/resource.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"server.h"
#include"socket.h"
#include"signals.h"
#include"client.h"
//将进程的文件描述符数设置为极限值
static int initMaxFiles(void){
    struct rlimit rl;
    //获取当前进程最大文件描述符数
    if(getrlimit(RLIMIT_NOFILE,&rl) == -1){
        perror("getrlimit");
        return -1;
    }
    //如果当前进程描述符数未达极限值
    if(rl.rlim_cur < rl.rlim_max){
        //则将当前进程文件描述符数设置为极限
        rl.rlim_cur = rl.rlim_max;
        if(setrlimit(RLIMIT_NOFILE,&rl) == -1){
            perror("setrlimit");
            return -1;
        }
    }
    return 0;
}

//初始化服务器
int initServer(short port){
    //初始化套接字
    if(initSocket(port) == -1){
        return -1;
    }
    //初始化信号
    if(initSignals() == -1){
        return-1;
    }
    //初始化最大文件描述符数
    if(initMaxFiles() == -1){
        return -1;
    }
    return 0;
}

//运行服务器
int runServer(const char* home){
    for(;;){
        //接客户端的连接请求
        int conn = acceptClient();
        if(conn == -1){
            return -1;
        }

        //开线程负责通信
        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
        CA* ca = malloc(sizeof(CA));
        ca->conn = conn;
        ca->home = home;
        int errno = pthread_create(&tid,&attr,client,ca);
        if(errno){
            fprintf(stderr,"pthread_create:%s\n",strerror(errno));
            return -1;
        }
    }
    return 0;
}

//终结化服务器
 void deinitServer(void){
    deinitSocket();
 }







