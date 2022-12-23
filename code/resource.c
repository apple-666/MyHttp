//资源模块实现
#include<unistd.h>
#include<sys/syscall.h>
#include<stdio.h>
#include<string.h>
#include"resource.h"
#include"mime.h"

//搜索资源
int searchResource(const char* path){
    return  access(path,R_OK);  //接收绝对或者相对路径 0 存在  -1 不存在
}

//识别类型  0 (成功,strcpy(type,sss))  -1 未识别到
int identifyType(const char* path,char* type){
    // /home/tarena/2107/project/home/c/c.html
    char* suf = strrchr(path,'.');
    if(suf == NULL){
        printf("%d.%ld > 无法获取文件的拓展名\n",getpid(),syscall(SYS_gettid));
        return -1;
    }
    
    for(int i = 0;i < sizeof(s_mime) / sizeof(s_mime[0]);i++){
        if(strcasecmp(suf,s_mime[i].suffix) == 0){
            strcpy(type,s_mime[i].type);
            return 0;
        }
    }
    printf("%d.%ld > 不可是被的资源类型\n",getpid(),syscall(SYS_gettid));
    return -1;
}










