#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

void xargs(int margc, char *margv[])
{
    int i;
    char *argv[MAXARG], line[512], *pa, *p;
    p = line;
    pa = p;
    i = margc;

    if (margc == 0)
    {
        fprintf(2, "usage: xargs must have args\n");
        return;
    }

    for (int j = 0; j < margc; j++)
    {
        argv[j] = margv[j];
    }

    while (read(0, p, 1) == 1)
    {
        if (*p == '\n')
        {
            *p = 0;
            argv[i] = pa;
            p = line;
            pa = p;
            i = margc;

            if (fork() == 0)
            {
                exec(argv[0], argv);
                exit(1);
            }
            wait(0);
            argv[i] = 0;
            continue;
        }

        if (*p == ' ')
        {
            if (i >= MAXARG)
            {
                fprintf(2, "usage: xargs exceed MAXARG\n");
                exit(1);
            }

            *p = 0;
            argv[i] = pa;
            pa = p + 1;
            i++;
        }

        p++;

        if (p > line + sizeof(line) - 1)
        {
            fprintf(2, "xargs: exceed max line buffer\n");
            exit(1);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        fprintf(2, "usage: xargs [args] *\n");
        exit(1);
    }
    xargs(argc - 1, argv + 1);
    exit(1);
}
