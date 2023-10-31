#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int fd_R) {
    int printNum = 0;
    int acceptNum = 0;
    int forked = 0; // 标识当前进程是否创建了子线程
    int p[2];
    while (1) {
        int readBytes = read(fd_R, &acceptNum, sizeof(int));
        // 递归终止条件
        if (readBytes == 0) {
            close(fd_R);
            if (forked) {
                close(p[1]);
                wait(0);
            }
            exit(0);
        }

        // 递归逻辑处理
        if (printNum == 0) {
            printNum = acceptNum;
            printf("prime %d\n", printNum);
        }

        if (acceptNum % printNum != 0) {
            if (!forked) { // 只有第一次创建子线程时进入
                pipe(p);
                int pid = fork();
                if (pid == 0) {
                    close(p[1]);
                    close(fd_R);
                    primes(p[0]);
                } else {
                    forked = 1;
                    close(p[0]);                  
                }
            }
            write(p[1], &acceptNum, sizeof(int));
        }
    }
}

int main(int argc, char *argv[]) {
    int p[2];
    pipe(p);
    for (int i = 2; i <= 35; ++ i) {
        write(p[1], &i, sizeof(int));
    }
    close(p[1]);
    primes(p[0]);
    exit(0);
}