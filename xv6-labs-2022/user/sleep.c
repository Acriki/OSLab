#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(int argc, char *args[])
{
    if (argc  < 2) {
        fprintf(2, "error: one parameter is needed\n");
        exit(0);
    }
    int t = atoi(args[1]);
    fprintf(1, "(nothing happens for a little while)\n");
    sleep(t);
    exit(0);
}