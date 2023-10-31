#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUFSIZE 512

int main(int argc, char *argv[]) {
    char buf[BUFSIZE+1] = {0};
    uint occupy = 0;           // 表示当前占用缓冲区大小
    char *xargv[MAXARG] = {0}; // 存储从标准输入读取的命令行参数。
    int stdin_end = 0;

    for (int i = 1; i < argc; i++) {
        xargv[i-1] = argv[i];
    }

    while (!(stdin_end && occupy == 0)) {
        if (!stdin_end) {
            int remain_size = BUFSIZE - occupy;
            int read_bytes = read(0, buf + occupy, remain_size);
            if (read_bytes < 0) {
                fprintf(2, "xargs: read returns -1 error\n");
            }
            if (read_bytes == 0) {
                close(0);
                stdin_end = 1;
            }
            occupy += read_bytes; 
        }
        char *line_end = strchr(buf, '\n');
        while (line_end) {
            char xbuf[BUFSIZE+1] = {0};
            memcpy(xbuf, buf, line_end -buf); // 对于每一行，将其复制到一个临时缓冲区xbuf中
            xargv[argc - 1] = xbuf;
            int ret = fork();
            if (ret == 0) {
                if (!stdin_end) 
                    close(0);
                if (exec(argv[1], xargv) < 0) {
                    fprintf(2, "xargs: exec fails with -1\n");
                    exit(1);
                }
            } else {
                // 修建我们的buffer, 把0～line_end的byte移除, 把line_end+1～ 的byte移到队头
                memmove(buf, line_end + 1, occupy - (line_end - buf) - 1);
                occupy -= line_end - buf + 1;
                memset(buf + occupy, 0, BUFSIZE - occupy);
                int pid;
                wait(&pid);

                line_end = strchr(buf, '\n');
            }
        }
    }
    exit(0);
}
