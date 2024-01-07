#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"

void find(char *path, char *target)
{
    int fd;
    struct stat st;
    struct dirent de;
    char buf[512], *p;
    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0)
        {
            printf("ls: cannot stat %s\n", buf);
            continue;
        }
        switch (st.type)
        {
        case T_FILE:
            if (strcmp(de.name, target) == 0)
            {
                printf("%s\n", buf);
            }
            break;

        case T_DIR:
            if (strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0)
            {
                find(buf, target);
            }
            break;
        }
    }
    close(fd);
}

int main(int argc, char *args[])
{
    if (argc < 3) {
        fprintf(2, "find must have two args\n");
        exit(0);
    }
    find(args[1], args[2]);
    exit(0);
}