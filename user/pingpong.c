#include <kernel/types.h>
#include <user/user.h>



int
main(int argc, char* argv[]){

    int pid;
    char buf[] = "y";
    int pipe1[2], pipe2[2];
    pipe(pipe1);
    pipe(pipe2);

    if(fork() == 0){//child
        close(pipe1[0]);
        close(pipe2[1]);

        pid = getpid();
        if(read(pipe2[0], buf, 1) != 1){
            fprintf(2, "failed to read in child\n");
            exit(1);
        };
        fprintf(1, "%d: received ping\n", pid);
        if(write(pipe1[1], buf, 1) != 1){
            fprintf(2, "failed to write in child\n");
            exit(1);
        };

        close(pipe1[1]);
        close(pipe2[0]);
        exit(0);
    }
    else //parent
    {
        close(pipe1[1]);
        close(pipe2[0]);

        pid = getpid();
        if(write(pipe2[1], buf, 1) != 1){
            fprintf(2, "failed to write in parent\n");
            exit(1);
        };
        if(read(pipe1[0], buf, 1) != 1){
            fprintf(2, "failed to read in parent\n");
            exit(1);
        }
        fprintf(1, "%d: received pong\n", pid);

        close(pipe1[0]);
        close(pipe2[1]);
        exit(1);
    }


    exit(0);
}