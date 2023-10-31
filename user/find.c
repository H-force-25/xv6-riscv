#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char *path) {
    char *prev = 0, *cur = strchr(path, '/');
    while (cur != 0) {
        prev = cur;
        cur = strchr(cur+1, '/');
    }
    return prev;
} 

void find(char *path, char *filename) {
    char buf[512], *p;
    int fd;
    struct stat st; 
    struct dirent de;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }
    if (fstat(fd, &st) < 0) {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }
    switch (st.type) {
        case T_FILE: {
            char *f_name = fmtname(path);
            int isOK = 1;
            if (f_name == 0 || strcmp(f_name+1, filename) != 0) {
                isOK = 0;
            }
            if (isOK) {
                printf("%s\n", path);
            } 
            close(fd);
            break;
        }
        case T_DIR: {
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find: path too long\n");
                break;
            }
            memset(buf, 0, sizeof(buf));
            uint path_len = strlen(path);
            memcpy(buf, path, path_len);
            buf[path_len] = '/';
            p = buf + path_len + 1;
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                    continue;
                }
                // printf("test:%s\n", de.name);
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                find(buf, filename);
            }
            close(fd);
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: find [DIR] [filename]\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}