#include "../kernel/types.h"
#include "user.h"

int main(int argc,char * argv[]){
    int pid;
    int pipes1[2],pipes2[2];
    pipe(pipes1);
    pipe(pipes2);

    int ret =fork();

    if(ret == 0){
        char child_send_buf[] = {'l'};
        char child_recv_buf[] = {'0'};

        pid = getpid();
        close(pipes1[1]);
        close(pipes2[0]);
//        printf("child===%d   %d\n",pipes1[0],pipes1[1]);
//        printf("child===%d   %d\n",pipes2[0],pipes2[1]);
        /**父子线程共享相同的pipes1和pipes2文件描述符，但是各自拥有副本，
         * 子进程关闭pipes1写端口，不影响父进程向pipes1写入数据**/

        /**pipes1[0]是要读取的文件描述符，buf是存数读取的数据，1是要读取的字节数**/
        read(pipes1[0],child_recv_buf,1);
        printf("%d: received ping\n", pid);
        /**ping应该接收到父进程发过来的z就是122**/
        printf("%d: received number %d\n",pid ,child_recv_buf[0]);

        /**pipes1[0]是要写入的文件描述符，buf是要写入的数据，1是要读取的字节数**/
        write(pipes2[1],child_send_buf,1);
        exit(0);
    } else{
        char parent_send_buf[] = {'z'};
        char parent_recv_buf[] = {'0'};

        pid = getpid();
        close(pipes1[0]);
        close(pipes2[1]);

        write(pipes1[1],parent_send_buf,1);
        read(pipes2[0],parent_recv_buf,1);
        printf("%d: received pong\n", pid);
        /**pong应该接收到子进程发过来的字符c，也就是108**/
        printf("%d: received number %d\n",pid ,parent_recv_buf[0]);

        exit(0);
    }
}