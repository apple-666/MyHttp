//http模块头文件
#ifndef __HTTP_H_
#define __HTTP_H_
#include<limits.h>
#include<sys/types.h>
//数据类型，http请求中解析出来的关键数据
typedef struct tag_HttpRequest{
    char method[32];//请求方法
    char path[PATH_MAX + 1];//资源路径
    char protocol[32];//协议版本
    char connection[32];//连接状态
} HTTP_REQUEST;

//解析http请求
int parseRequest(const char* req,HTTP_REQUEST* hreq);

//构造响应头所需要数据
typedef struct tag_HttpRespond{
    char protocol[32];//协议版本
    int status;//状态码
    char desc[128];//状态描述
    char type[64];//类型
    off_t length;//长度
    char connection[32];//连接状态
} HTTP_RESPOND;

//构造http响应
int constructHead(const HTTP_RESPOND* hres,char* head);
#endif //__HTTP_H_


