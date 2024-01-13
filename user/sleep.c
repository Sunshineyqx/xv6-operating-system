#include <kernel/types.h>
#include <user/user.h>

/*
* sunshine 2024/1/13
*/




int
main(int argc, char* argv[]){

    //sleep must have an another argv except argv[0]
    if(argc < 2){
        fprintf(2, "Usage: how much time you want to sleep?\n");
        exit(1);
    }
    //but the arguments can't more than 2
    if(argc > 2){
        fprintf(2, "Usage: too many arguments...\n");
        exit(1);
    }

    int ticks;
    ticks = atoi(argv[1]);
    if(sleep(ticks) < 0){
        fprintf(2, "sleep: an error occur...\n");
        exit(1);
    }
    
    exit(0);
}