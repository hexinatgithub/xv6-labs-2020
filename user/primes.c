#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PRIMENUMBER 35

void initNumbers(int *arr);
void sieve(int fd);
void sendNumbers(int *arr, int fd);

int main(int argc, char *argv[])
{
    int numbers[PRIMENUMBER-1], p[2];
    initNumbers(numbers);
    pipe(p);

    if (fork() == 0)
    {
        // Note: child should close pipe write side, fd close both in parent and child will really close
        close(p[1]);
        sieve(p[0]);
        exit(0);
    }

    sendNumbers(numbers, p[1]);
    close(p[1]);
    close(p[0]);
    wait(0);
    fprintf(1, "^OK$\n");
    return 0;
}

// sieve read first number from fd as multipler, send multiples of multipler from fd to
// the child process util fd is close.
void sieve(int fd)
{
    int multipler, number, p[2], cpid = 0;
    if (read(fd, (char *)&multipler, sizeof(int)) == 0)
    {
        exit(0);
    }
    fprintf(1, "prime %d\n", multipler);
    pipe(p);

    while (read(fd, (char *)&number, sizeof(int)) != 0)
    {
        if ((number % multipler) != 0)
        {
            // create child process run sieve if needed
            if (cpid == 0)
            {
                if ((cpid = fork()) == 0)
                {
                    // Note: child should close pipe write side, fd close both in parent and child will really close
                    close(p[1]);
                    sieve(p[0]);
                    exit(0);
                }
            }

            write(p[1], (char *)&number, sizeof(int));
        }
    }
    close(p[1]);
    close(p[0]);
    wait(0);
    exit(0);
}

void initNumbers(int *arr)
{
    for (int i = 0; i <= (PRIMENUMBER - 2); i++)
    {
        arr[i] = i + 2;
    }
}

void sendNumbers(int *arr, int fd) {
    for (int i = 0; i <= (PRIMENUMBER - 2); i++)
    {
        write(fd, (char *)&arr[i], sizeof(int));
    }
}