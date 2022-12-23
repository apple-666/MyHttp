//通信模块头文件
#ifndef __SOCKET_H_
#define __SOCKET_H_
//初始化套接字 返回 0成功 -1
int initSocket(short port);
//接收客户机的连接 返回套接字描述符 -1
int acceptClient(void);
//接收客户端http请求 返回指向请求内容的地址
char* recvRequest(int conn);
//发送响应头 0 -1
int sendHead(int conn,const char* head);
//发送响应体 0 -1
int sendBody(int conn,const char* path);
//终结化套接字
void deinitSocket(void);


#endif //__SOCKET_H_
