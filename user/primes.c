#include "../kernel/types.h"
#include "user.h"


void runprocess(int listenfd){
    int my_num = 0;
    int forked = 0;
    int passed_num = 0;/**从管道中读取的数组**/
    int pipes[2];  /**下一轮要用的管道**/

    while (1){
        /**passed_num是从管道中读取出的第一个数字
        * read_byes存读取的字节数**/
        int read_bytes = read(listenfd,&passed_num,4);
        /**如果读取的字节数为0，说明管道里没有数字了**/
        if(read_bytes == 0){
            /**关闭读端口，销毁管道**/
            close(listenfd);
            if(forked){
                /**因为管道中没有数字了，也就不用往下一轮的管道中写入数据了，就关掉写端口**/
                close(pipes[1]);
                int child_pid;
                wait(&child_pid);
            }
            exit(0);
        }
        /**my_num是0说明passed_num是管道中第一个数字，就是素数，打印出来**/
        if(my_num == 0){
            my_num = passed_num;
            printf("prime %d\n",my_num);
        }
        /**如果当前遍历到的passed_num不是my_num的倍数，就写入下一轮的管道中**/
        if(passed_num % my_num != 0){
            /**如果还没有fork子进程**/
            if(!forked){
                pipe(pipes);
                forked = 1;
                int ret = fork();
                if(ret == 0){
                    /**关闭子进程要用到的管道的写入端口，
                     * 因为子进程对这个管道只需读，不需要写
                     * 也不影响父进程写入，**/
                    close(pipes[1]);
                    /**子进程不需要读上一轮的管道，关掉读端口
                     * 不影响父进程读**/
                    close(listenfd);
                    /**子进程递归，读取晒过一轮后的管道里的数据**/
                    runprocess(pipes[0]);
                } else{
                    /**父进程不读下一轮的管道，关掉**/
                    close(pipes[0]);
                }
            }
            /**每个没筛掉的数字都写入下一轮的管道**/
            write(pipes[1],&passed_num,4);
        }
    }
}

int main(int argc,char * argv[]){
    int pipes[2];
    pipe(pipes);
    /**向管道中写入2-35**/
    for (int i = 2; i <=35; ++i) {
        write(pipes[1],&i,4);
    }
    /**关闭写入端口 **/
    close(pipes[1]);
    runprocess(pipes[0]);
    exit(0);
}