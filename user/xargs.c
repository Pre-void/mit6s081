#include "../kernel/param.h"
#include "../kernel//types.h"
#include "user.h"

#define buf_size 512

/**echo hello too | xargs echo bye**/
/**echo 123 | xargs cat**/
int main(int argc,char * argv[]){
    char buf[buf_size + 1] = {0};
    /**缓冲区已使用的大小**/
    uint  occupy = 0;
    /**最多支持32个参数**/
    char * xargv[MAXARG] = {0};
    /**输入是否结束**/
    int stdin_end = 0;
    printf("咕噜咕噜====%s\n",argv[0]);

    /**存数命令行输入的除了首参数外的参数**/
    /**
     * !!!!这里的参数是从管道后开始算的也就是
     * 如果执行这条命令echo 123 | xargs cat
     * argv就是 xargs  cat
     * **/
    for (int i = 1; i < argc; ++i) {
//        printf("咕噜咕噜====%s\n",argv[i]);
        xargv[i-1] = argv[i];
    }
    /**标准输入没有结束或者缓冲区仍有内容未被执行**/
    while (!(stdin_end && occupy == 0)){
        if(!stdin_end){
            /**剩余大小**/
            int remain_size = buf_size - occupy;
            /**从管道的输入端口读取，也就是管道前的命令的输出**/
            int read_bytes = read(0,buf+occupy,remain_size);
//            printf("==========%s",buf);
            /**读取错误**/
            if(read_bytes < 0){
                fprintf(2,"xargs: read returns -1 error\n");
            }
            /**没有读到数据,说明读完了**/
            if(read_bytes == 0){
                close(0);
                stdin_end = 1;
            }
            occupy += read_bytes;
        }
        /**在管道输出中查找分隔符**/
        char * line_end = strchr(buf,'\n');

//        printf("++++++++++++%s\n",line_end);

        while (line_end){
            char xbuf[buf_size+1] = {0};
            /**将第一个分隔符前的参数拷贝到xbuf**/
            memcpy(xbuf,buf,line_end-buf);
            /**参数列表中加一个参数**/
            xargv[argc - 1] = xbuf;
            int ret = fork();
            if(ret == 0){
                /**stdin_end为0时，表示输入尚未结束，
                 * 子进程关掉读取端口，避免读取数据**/
                if(!stdin_end){
                    close(0);
                }
                if(exec(argv[1],xargv) < 0){
                    fprintf(2,"xargs: exec fails with -1\n");
                    exit(1);
                }
            } else{
                /**处理了分号前的参数后，将第一个参数后的参数挪到缓冲区头部，
                 * 也就是删去了已经处理了的第一个参数**/
                memmove(buf,line_end + 1,occupy - (line_end - buf) - 1);
                occupy -= line_end - buf + 1;
                /**将除了剩余参数外的部分置为0**/
                memset(buf + occupy,0,buf_size-occupy);
                /**等待子进程运行结束**/
                int pid;
                wait(&pid);
                /**找下一个参数**/
                line_end = strchr(buf,'\n');
            }
        }
    }
    exit(0);
}