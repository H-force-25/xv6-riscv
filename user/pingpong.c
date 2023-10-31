#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    char buf[10];
    int pid;
    int p1[2], p2[2];
    pipe(p1);
    pipe(p2);
    pid = fork();
    if (pid == 0) {
        close(p1[1]); // p1 子读父写
        close(p2[0]); // p2 子写父读
        int ret = read(p1[0], buf, sizeof(buf));
        if (ret > 0) {
            printf("%d: received ping\n", getpid());
            write(p2[1], "pong", 5);
        } else {
            printf("close or error\n");
        }
        exit(0);
    } else if (pid > 0) {
        close(p1[0]);
        close(p2[1]);
        write(p1[1], "ping", 5);
        int ret = read(p2[0], buf, sizeof(buf));
        if (ret > 0) {
            printf("%d: received pong\n", getpid());
        } else {
            printf("close or error\n");
        }
    } else {
        printf("fork error\n");
    }
    exit(0);
}