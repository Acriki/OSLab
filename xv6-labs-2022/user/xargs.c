#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user.h"
char buf[1024];
void xargs(int fd, char *argv[], int *pos)
{
    int n;
    char c;
    int i = 0;
    char *p = buf;
    // read one character at a time until a newline appears. 
    while ((n = read(fd, &c, sizeof(char))) > 0) {
        if (c == '\n') {
            buf[i++] = '\0';
            argv[(*pos)++] = p;
            p = &buf[i];
            continue;
        }
        buf[i++] = c;
    }
}

int main(int argc, char *args[])
{
    if (argc < 2) {
        exit(0);
    }
    int pid = fork();
    if (pid == 0) {
        // this child process.
        char *command = args[1];
        char *argv[MAXARG];
        argv[0] = command;
        int i;
        for (i = 2; i < argc; i++) {
            argv[i - 1] = args[i];
        }
        i -= 1;
        xargs(0, argv, &i);
        exec(command, argv);
    } else if (pid > 0) {
        // this parent process.
        wait(0);
    }
    exit(0);
}