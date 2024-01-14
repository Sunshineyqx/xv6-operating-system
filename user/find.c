/*
 * @Author: sunshine 
 * @Date: 2024-01-14 17:46:34 
 * @Last Modified by: sunshine
 * @Last Modified time: 2024-01-14 19:21:12
 */

#include <kernel/types.h>
#include <user/user.h>
#include <kernel/stat.h>
#include <kernel/fs.h>
#include "kernel/fcntl.h"


char*
basename(char* path){

    char* prev = 0;
    char* cur = strchr(path, '/');
    while(cur != 0){
        prev = cur;
        cur = strchr(cur + 1, '/');
    }
    return (prev == 0) ? 0 : prev + 1;
}


void
find(char* path, char* target){
    char buf[512];
    char* p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "find: can't open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: can't fstat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
        case T_FILE:
        {
            char* f_name = basename(path);
            if(f_name != 0 && strcmp(f_name, target) == 0){
                printf("%s\n", path);
            }
            close(fd);
            break;
        }
   
        case T_DIR:{
            memset(buf, 0 , sizeof buf);
            if(strlen(path) + 1 + DIRSIZ +1 > sizeof buf){
                fprintf(2, "find: path too long\n");
                break;
            }
            memcpy(buf, path, strlen(path));
            p = buf + strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0){
                    continue;
                }
                memcpy(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                find(buf, target);
            }
            close(fd);
            break;
        }
    }
}

int
main(int argc, char* argv[]){
    if(argc != 3){
        fprintf(2, "Usage: find [directory] [target filename]\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}