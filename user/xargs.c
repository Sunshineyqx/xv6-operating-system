/*
 * @Author: sunshine 
 * @Date: 2024-01-14 20:42:12 
 * @Last Modified by: sunshine
 * @Last Modified time: 2024-01-14 21:40:26
 */
#include <kernel/types.h>
#include <kernel/param.h>
#include <user/user.h>
#define buf_size 512


int
main(int argc, char* argv[]){

    int stdin_end = 0;
    uint unchecked_bytes = 0;
    char buf[buf_size + 1] = {0};
    char *xargv[MAXARG] = {0};

    for(int i  = 1; i < argc; i++){
        xargv[i-1] = argv[i];
    }
    while(stdin_end == 0 || unchecked_bytes != 0){
        //process data in
        if(!stdin_end){
            int remain_bytes = buf_size-unchecked_bytes;
            int read_bytes = read(0, buf+unchecked_bytes, remain_bytes);
            if(read_bytes < 0){
                fprintf(2, "xargs: read return -1 error\n");
                exit(1);
            }
            else if(read_bytes == 0){
                close(0);
                stdin_end = 1;
            }
            unchecked_bytes += read_bytes;
        }
        //process buf
        char *line_end = strchr(buf, '\n');
        while(line_end){
            char line[buf_size + 1] = {0};
            memcpy(line, buf, line_end - buf);
            xargv[argc - 1] = line;
            int ret = fork();
            if(ret == 0){//child
                if(!stdin_end) close(0);
                if(exec(argv[1], xargv) < 0){
                    fprintf(2, "xargs: exec error\n");
                    exit(1);
                }
            }
            else{//parent
                memmove(buf, line_end + 1, unchecked_bytes - (line_end - buf) -1);
                unchecked_bytes -= line_end - buf + 1;
                memset(buf + unchecked_bytes, 0, buf_size - unchecked_bytes);
                int pid;
                wait(&pid);
                line_end = strchr(buf, '\n');
            }
        }
    }
    exit(0);
}