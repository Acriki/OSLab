#include "kernel/types.h"
#include "user.h"

void pipeline(int fd)
{
    int result;
    int pip[2];
    if (read(fd, &result, sizeof(int)) == sizeof(int)) {
        int prime = result;
        printf("prime %d\n", result);
        pipe(pip);
        int pid = fork();
        if (pid == 0) {
            // this child process
            // fd will be copied, both parent and child process need to close.
            close(fd);
            close(pip[1]);
            pipeline(pip[0]);
        } else if (pid > 0) {
            // this parent process
            while (read(fd, &result, sizeof(int)) == sizeof(int)) {
                if (result % prime) {
                    write(pip[1], &result, sizeof(int));
                }
            }
            close(fd);
            close(pip[0]);
            close(pip[1]);
            // wait for child process exit.
            wait(0);
        }
    }
    exit(0);
}

int main()
{
    int pip[2];
    pipe(pip);
    for (int i = 2; i <= 35; i++) {
        write(pip[1], &i, sizeof(int));
    }
    close(pip[1]);
    pipeline(pip[0]);
    exit(0);
}