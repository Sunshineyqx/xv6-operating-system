#include <kernel/types.h>
#include <user/user.h>




void
run_process(int read_fd){

    int pipes[2];
    int init_num = 0;
    int is_forked = 0;
    int process_num = 0;

    while(1){

        int read_bytes = read(read_fd, &process_num, 4);
        //读取完毕
        if(read_bytes == 0){
            close(read_fd);
            if(is_forked){
                close(pipes[1]);
                int child_pid;
                wait(&child_pid);
            }
            exit(0);
        }
        //若未初始化，第一个数初始化自身,  并打印该素数
        if(init_num == 0){
            init_num = process_num;
            printf("prime %d\n", init_num);
            continue;
        }
        //若初始化过了，判断后续数字是否可被自己整除
        if(process_num%init_num != 0){
            //无法整除，是否有子进程可以继续处理该数
            if(!is_forked){
                pipe(pipes);
                is_forked = 1;
                if(fork() == 0){//child
                    close(read_fd);
                    close(pipes[1]);
                    run_process(pipes[0]);
                }
                else{
                    close(pipes[0]);
                }
            }
            write(pipes[1], &process_num, 4);
        }
    }
    exit(1);
}



int
main(int argc, char* argv[]){

    int pipes[2], i;


    pipe(pipes);
    for(i = 2; i <= 35; i++){
        write(pipes[1], &i, 4);
    }
    close(pipes[1]);
    run_process(pipes[0]);

    exit(0);
}