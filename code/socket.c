//通信模块实现
#include<unistd.h>
#include<fcntl.h>
#include<sys/syscall.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"socket.h"

static int s_sock = -1;//侦听套接字

//初始化套接字
int initSocket(short port){
    printf("%d > 创建套接字\n",getpid());
    s_sock = socket(AF_INET,SOCK_STREAM,0);
    if(s_sock == -1){
        perror("socket");
        return -1;
    }

    //设置套接字端口复用，当服务器重启时，可以重新绑定端口
    printf("%d > 设置套接字\n",getpid());
    int on = 1;
    if(setsockopt(s_sock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) == -1){
        perror("setsockopt");
        return -1;
    }
    
    printf("%d > 组织地址结构\n",getpid());
    struct sockaddr_in ser;
    ser.sin_family = AF_INET;
    ser.sin_port = htons(port);
    ser.sin_addr.s_addr = INADDR_ANY;

    printf("%d > 绑定套接字\n",getpid());
    if(bind(s_sock,(struct sockaddr*)&ser,sizeof(ser)) == -1){
        perror("bind");
        return -1;
    }

    printf("%d > 开启侦听\n",getpid());
    if(listen(s_sock,1024) == -1){
        perror("listen");
        return -1;
    }
    return 0;
}

//接收客户端的连接
int acceptClient(void){
    printf("%d > 接收客户端的连接\n",getpid());
    struct sockaddr_in cli;//用来输出客户端的地址结构
    socklen_t len = sizeof(cli);
    // accept会阻塞等待 返回套接字描述符
    int conn = accept(s_sock,(struct sockaddr*)&cli,&len);
    if(conn == -1){
        perror("accept");
        return -1;
    }
    return conn; 
}

//接收http请求
char* recvRequest(int conn){
    char* req = NULL;//动态分配的缓冲区起始地址
    ssize_t len = 0;//缓冲区的长度
    // 接收一次http请求，因为http请求头内容很长所以用 循环分段接收
    for(;;){
        char buf[1024] = {};
        ssize_t size = recv(conn,buf,sizeof(buf)-sizeof(buf[0]),0);
        if(size == -1){
            perror("recv");
            free(req);
            return NULL;
        }
    
        if(size == 0){
            printf("%d.%ld > 客户端关闭套接字\n",getpid(),syscall(SYS_gettid));
            free(req);
            return NULL;
        }

        req = realloc(req,len + size + 1);
        memcpy(req+len,buf,size + 1);
        len = len + size;

        if(strstr(req,"\r\n\r\n")){ // 出现了\r\n\r\n的位置
            break;
        }
    }    
    return req;
}

//发送响应头
int sendHead(int conn,const char* head){
    if(send(conn,head,strlen(head),0) == -1){
        perror("send");
        return -1;
    }
    return 0;
}

//发送响应体
int sendBody(int conn,const char* path){
    //打开文件
    int fd = open(path,O_RDONLY);
    if(fd == -1){
        perror("open");
        return -1;
    }
    char buf[1024];
    ssize_t len;
    while((len = read(fd,buf,sizeof(buf)-sizeof(buf[0]))) > 0){
        if(send(conn,buf,len,0) == -1){
            perror("send");
            return -1;
        } 
    }
    if(len == -1){
        perror("read");
        return -1;
    }
    close(fd);
    return 0;
}

//终结化套接字
void deinitSocket(void){
    close(s_sock);
}













