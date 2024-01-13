#include "../kernel/types.h"
#include "../kernel/fcntl.h"
#include "../kernel/fs.h"
#include "../kernel/stat.h"
#include "user.h"


char * basename(char * pathname){
    char * prev = 0;
    /**找到第一个'/'出现的位置，返回字符对应的指针，如果没有，返回NULL**/
    char * curr = strchr(pathname,'/');
    /**等同于!= NULL**/
    while (curr != 0){
        prev = curr;
        curr = strchr(curr + 1,'/');
    }
    return prev;
}

void find(char * curr_path,char * target){
    //    printf("===========%s\n",curr_path);
    char buf[512],* p;
    int fd;
    /**struct dirent {
    ushort inum; 文件的索引节点号，代表该文件在文件系统中的唯一标识。
    char name[DIRSIZ];  文件的名称
   };**/
    struct dirent de;
    /**
         * struct stat {
              int dev;     文件所在的磁盘设备
              uint ino;    文件的索引节点号
              short type;  文件类型 代表文件/目录
              short nlink; 链接数量
              uint64 size; 文件大小
          };
         *
     * **/
    struct stat st;
    /**int open(const char*, int);
     * 第一个参数是文件路径
     * 第二个参数指定文件标志位O_RDONLY（只读）、O_WRONLY（只写）、O_RDWR（读写）等。
     * 以只读方式打开文件夹，若打开失败，返回-1，程序结束**/
    if((fd = open(curr_path,O_RDONLY)) < 0){
        fprintf(2,"find: cannot open %s\n",curr_path);
        return;
    }
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", curr_path);
        close(fd);
        return;
    }

    switch (st.type) {
        char * f_name;
        case T_FILE:  /**文件类型**/
            /**获取文件名，返回的是curr_path前的/的指针**/
           f_name = basename(curr_path);
           int match = 1;
            /**如果basename没有找到文件名，或者文件名不匹配**/
            if(f_name == 0 || strcmp(f_name + 1,target)!= 0){
               match = 0;
           }
           if(match){
               printf("%s\n",curr_path);
           }
           /**找到了那个文件就结束任务，关闭文件描述符**/
           close(fd);
           break;
        case T_DIR:  /**如果是文件夹类型**/
            memset(buf,0,sizeof(buf));  /**初始化缓冲区**/
            uint curr_path_len = strlen(curr_path);
            memcpy(buf,curr_path,curr_path_len); /**将路径复制到缓冲区buf**/
            buf[curr_path_len] = '/'; /**加斜杠为了构建新路径**/
            p = buf + curr_path_len + 1; /**指向路径末尾**/
            /**有几个目录项或文件就循环几次**/
            while (read(fd,&de,sizeof(de)) == sizeof(de)){
                //                printf("%s   %d\n", de.name,de.inum);
                /**文件无效或者当前目录或者父目录**/
                if(de.inum == 0 || strcmp(de.name,".") || strcmp(de.name,"..") == 0){
                    continue;
                }
                /**文件名复制到p**/
                memcpy(p,de.name,DIRSIZ);
                //                printf("===========%s\n",buf);
                /**字符串结束符**/
                p[DIRSIZ] = 0;
                /**递归查找**/
                find(buf,target);
            }
            close(fd);
            break;
    }

}

int main(int argc,char * argv[]){
    if(argc != 3){
        fprintf(2,"usage: find [directory] [target filename]\n");
        exit(1);
    }
    find(argv[1],argv[2]);
    exit(0);
}