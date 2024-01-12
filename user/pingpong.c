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

        read(pipes1[0],child_recv_buf,1);
        printf("%d: received ping\n", pid);
        printf("%d: received number %d\n",pid ,child_recv_buf);
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
        printf("%d: received number %d\n",pid ,parent_recv_buf);

        exit(0);
    }
}