#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main()
{
    int pip1[2], pip2[2];
    char c[] = {'a'};
    if (pipe(pip1) < 0) {
        fprintf(2, "open pipe failed\n");
        exit(1);
    }
    if (pipe(pip2) < 0) {
        fprintf(2, "open pipe failed\n");
        exit(1);
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork error\n");
        exit(1);
    } else if (pid == 0) {
        // child process
        close(pip1[0]);
        close(pip2[1]);
        int r = read(pip2[0], c, 1);
        if (r != 1) {
            fprintf(2, "error to read pipe\n");
            exit(1);
        } else {
            printf("%d: received ping\n", getpid());
        }
        int w = write(pip1[1], c, 1);
        if (w != 1) {
            fprintf(2, "error to write pipe\n");
            exit(1);
        }
        exit(0);
    } else {
        // parent process
        close(pip1[1]);
        close(pip2[0]);
        int w = write(pip2[1], c, 1);
        if (w != 1) {
            fprintf(2, "error to write pipe\n");
            exit(1);
        }
        int r = read(pip1[0], c, 1);
        if (r != 1) {
            fprintf(2, "error to read pipe\n");
            exit(1);
        } else {
            printf("%d: received pong\n", getpid());
        }
        exit(0);
    }
}