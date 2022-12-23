//http模块实现
#include<unistd.h>
#include<sys/syscall.h> // 进程和硬件设备做交互的接口
#include<stdio.h>
#define __USE_GNU
#include<string.h>
#include<strings.h>
#include<time.h>
#include"http.h"

// 解析请求
int parseRequest(const char* req,HTTP_REQUEST* hreq){
    /*GET /common/site_scripts.js HTTP/1.1\r\n
      Host: 127.0.0.1\r\n
      User-Agent: Mozilla/5.0 \r\n
      Accept: text/html\r\n
      Accept-Language: zh-CN\r\n
      Accept-Encoding: gzip, deflate\r\n
      Connection: keep-alive\r\n
      Referer: http://127.0.0.1/index.html\r\n\r\n
    */
    // sscanf 将req中的%s 赋值 给 hrep->method
    sscanf(req,"%s%s%s",hreq->method,hreq->path,hreq->protocol);
    char* connection = strcasestr(req,"connection");
    if(connection){
        // %*s 表示 "Connection:" 不做赋值 
        // 后面的 %s 赋值给 hrep->connection
        sscanf(connection,"%*s%s",hreq->connection);
    }   // SYS_gettid 拿区分线程的tid
    printf("%d.%ld > [%s][%s][%s][%s]\n",getpid(),syscall(SYS_gettid),
            hreq->method,hreq->path,hreq->protocol,hreq->connection);
    if(strcasecmp(hreq->method,"get")){
        // 不等于get 
        printf("%d.%ld > 无效的方法\n",getpid(),syscall(SYS_gettid));
        return -1;
    }
    if(strcasecmp(hreq->protocol,"http/1.1") && 
            strcasecmp(hreq->protocol,"http/1.0")){
        printf("%d.%ld > 无效的协议版本\n",getpid(),syscall(SYS_gettid));
        return -1;
    }
    return 0;
}

//构造响应
int constructHead(const HTTP_RESPOND* hres,char* head){
    /*HTTP/1.1 404 Not Fond!\r\n
      Server: LaoZhangWebServer 1.0\r\n
      Date: Fri,05 Nov 2021 12:06:58\r\n
      Content-Type: text/html\r\n
      Content-Length: 391\r\n
      Connection: keep-alive\r\n\r\n*/
    char dateTime[32];
    time_t now = time(NULL);
    strftime(dateTime,sizeof(dateTime),"%a %d %b %Y %T",gmtime(&now));
        
    sprintf(head,
            "%s %d %s\r\n"
            "Server: LaozhangWebserver 2.0\r\n"
            "Date: %s\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %ld\r\n"
            "Connection: %s\r\n\r\n",
            hres->protocol,hres->status,hres->desc,dateTime,hres->type,
            hres->length,hres->connection);
    return 0;
}





