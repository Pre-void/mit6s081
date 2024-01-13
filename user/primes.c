#include "../kernel/types.h"
#include "user.h"


void runprocess(int listenfd){
    int my_num = 0;
    int forked = 0;
    int passed_num = 0;
    int pipes[2];

    while (1){
        int read_bytes = read(listenfd,&passed_num,4);

        if(read_bytes == 0){
            close(listenfd);
            if(forked){
                close(pipes[1]);
                int child_pid;
                wait(&child_pid);
            }
            exit(0);
        }

        if(my_num == 0){
            my_num = passed_num;
            printf("prime %d\n",my_num);
        }

        if(passed_num % my_num != 0){
            if(!forked){
                pipe[pipes];
                forked = 1;
                int ret = fork();
                if(ret == 0){
                    close(pipes[1]);
                    close(listenfd);
                    runprocess(pipes[0]);
                } else{
                    close(pipes[0]);
                }
            }
            write(pipes[1],&passed_num,4);
        }
    }
}

int main(int argc,char * argv[]){
    int pipes[2];
    pipe(pipes);
    for (int i = 2; i < =35; ++i) {
        write(pipes[1],&i,4);
    }
    close(pipes[1]);
    runprocess(pipes[0]);
    exit(0);
}