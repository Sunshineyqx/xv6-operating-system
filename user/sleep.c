#include <kernel/types.h>
#include <user/user.h>

/*
* sunshine 2024/1/13
*/




int
main(int argc, char* argv[]){

    if(argc != 2){
        fprintf(2, "Usage: sleep [ticks]\n");
        exit(1);
    }


    int ticks = atoi(argv[1]);
    int ret = sleep(ticks);
    
    exit(ret);
}