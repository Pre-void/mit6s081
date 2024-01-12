#include "../kernel/types.h"
#include "user.h"

int main(int argc,char *argv[]){
    /**判断参数是否有两个**/
    if(argc != 2){
        fprintf(2,"usage:sleep [tick num]\n");
        exit(1);
    }
    /**转为整数**/
    int ticks = atoi(argv[1]);
    int ret  = sleep(ticks);
    exit(ret);
}
