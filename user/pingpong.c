#include <kernel/types.h>
#include <user/user.h>



int
main(int argc, char* argv[]){

    int child_pid;
    int pipe1[2], pipe2[2];
    pipe(pipe1);
    pipe(pipe2);

    child_pid = fork();
    if(child_pid == 0){//child
        close(pipe1[0]);
        close(pipe2[1]);

        char buf[1];
        int pid = getpid();
        read(pipe2[0], buf, 1);
        fprintf(1, "%d: received ping\n", pid);
        write(pipe1[1], buf, 1);

        close(pipe1[1]);
        close(pipe2[0]);
        exit(0);
    }
    else //parent
    {
        close(pipe1[1]);
        close(pipe2[0]);

        char buf[2] = "y";
        int pid = getpid();

        write(pipe2[1], buf, 1);
        read(pipe1[0], buf, 1);
        buf[2] = 0;
        fprintf(1, "%d: received pong\n", pid);

        close(pipe1[0]);
        close(pipe2[1]);
        exit(1);
    }


    exit(0);
}