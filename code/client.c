//线程模块实现
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/stat.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"http.h"
#include"socket.h"
#include"resource.h"
#include"client.h"

//线程过程函数，负责和客户端的通信过程
void* client(void* arg){
    CA* ca = (CA*)arg;
    printf("%d.%ld > 客户机线程开始\n",getpid(),syscall(SYS_gettid));
    for(;;){
        printf("%d.%ld > 接收http请求\n",getpid(),syscall(SYS_gettid));
        // 接收http请求，拿到http请求头内容 = req
        char* req = recvRequest(ca->conn);
        if(req == NULL){
            break;
        }
        printf("%d.%ld > 请求电文：\n%s\n",getpid(),syscall(SYS_gettid),req); 

        printf("%d.%ld > 解析请求\n",getpid(),syscall(SYS_gettid));
        // 这是自定义的请求结构体，准备解析req
        HTTP_REQUEST hreq = {};
        if(parseRequest(req,&hreq) == -1){
            free(req);//解析失败，需释放存储http请求的存储区
            break;
        }
        free(req);//解析成功，不需要存储区了，释放

        //资源路径
        char root[PATH_MAX+1] = {};//资源本地路径
        char path[PATH_MAX+1] = {};//拼接之后，具体的文件路径
        
        // root  /home/tarena/2107/project/home
        strcpy(root,ca->home);
        if(root[strlen(root) - 1] == '/'){ // 末尾有/的 去掉
            root[strlen(root) - 1] = '\0';
        }
        
        strcpy(path,root);
        strcat(path,hreq.path);// /home/tarena/2107/project/home/c/c.html
        //如果解析出来的路径hreq.path 是 / ，应该给客户端首页文件
        if(strcmp(hreq.path,"/") == 0){
            strcat(path,"index.html"); // /home/tarena/2107/project/home/index.html
        }   

        printf("%d.%ld > 资源路径\n%s\n",getpid(),syscall(SYS_gettid),path);

        //构造响应头所需结构体
        HTTP_RESPOND hres = {"HTTP/1.1",200,"OK","text/html"};

        //搜索资源
        if(searchResource(path) == -1){
            hres.status = 404;
            strcpy(hres.desc,"Not Found");
            strcpy(path,root);
            strcat(path,"/404.html");// /home/tarena/2107/project/home/404.html
        }else if(identifyType(path,hres.type) == -1){
            hres.status = 404;
            strcpy(hres.desc,"Not Found");
            strcpy(path,root);
            strcat(path,"/404.html");// /home/tarena/2107/project/home/404.html
        }
-------------------------------------------

        //长度
        struct stat st;
        if(stat(path,&st) == -1){
            perror("stat");
            break;
        }
        hres.length = st.st_size;

        // 一般情况下，请求头中会有connection信息，所以让respon=reques
        // 否则按 http版本协议来
        if(strlen(hreq.connection)){
            strcpy(hres.connection,hreq.connection);
        }else if(strcasecmp(hreq.protocol,"http/1.0") == 0){
        //连接状态 1.0为短连接  1.1长连接

            strcpy(hres.connection,"close");        
        }else{
            strcpy(hres.connection,"keep-alive");
        }

        printf("%d.%ld > 构造响应头\n",getpid(),syscall(SYS_gettid));
        char head[1024] = {};
        constructHead(&hres,head);
        
        printf("%d.%ld > 响应电文\n%s\n",getpid(),syscall(SYS_gettid),head);

        //发送响应头
        if(sendHead(ca->conn,head) == -1){
            break;
        }
        //发送响应体
        if(sendBody(ca->conn,path) == -1){
            break;
        }

        if(strcasecmp(hres.connection,"close") == 0){
            break;
        }
    }
        
    close(ca->conn);
    free(ca);
    printf("%d.%ld > 客户机线程结束\n",getpid(),syscall(SYS_gettid));
    return NULL;
}












