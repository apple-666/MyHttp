//线程模块头文件
#ifndef __CLIENT_H_
#define __CLIENT_H_
//线程的参数
typedef struct clientArg{
    int conn;//通信套接字
    const char* home;//资源在本地的存储位置 
}CA;

//线程过程函数
void* client(void* arg); // arg 是 CA结构体存放了套接字+home位置

#endif //__CLIENT_H_
