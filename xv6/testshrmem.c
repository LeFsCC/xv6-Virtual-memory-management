#include "types.h"
#include "stat.h"
#include "user.h"
int times = 20;
int main()
{
    printf(1, "================================\n");
    printf(1, "Memory sharing test started.\n");
    for (int i = 1; i <= times; i++)
    {
        if (make_shrmem(i) == 0)
            printf(1, "[P] Share memory %d created.\n", i);
        else
        {
            printf(1, "[P] Share memory %d creating failed.\n", i);
            exit();
        }
        char *content = "Hello child proc, I'm your father!";
        printf(1, "[P] Writing message to child... on share page %d\n", i);
        if (write_shrmem(i, content) != 0)
        {
            printf(1, "Error!\n");
            exit();
        }
    }
    if (fork() == 0) // This is child.
    {
        for (int i = 1; i <= times; i++)
        {
            char *read = malloc(4096);
            if (read_shrmem(i, read) != 0)
            {
                printf(1, "Error!\n");
                free(read);
                exit();
            }
            printf(1, "[C] Recv: %s\n", read);
            char *write = "Hello parent proc, I'm your child!";
            printf(1, "[C] Writing message to parent... on share page %d\n", i);
            if (write_shrmem(i, write) != 0)
            {
                printf(1, "Error!\n");
                free(read);
                exit();
            }
            free(read);
        }
        printf(1, "Child start sleeping\n");
        sleep(100);
        exit();
    }
    else // This is parent.
    {
        sleep(times * 20);
        for (int i = 1; i <= times; i++)
        {
            char *read = malloc(4096);
            if (read_shrmem(i, read) != 0)
            {
                printf(1, "Error!\n");
                free(read);
                exit();
            }
            printf(1, "[P] Recv: %s from share page %d\n", read, i);
            free(read);
        }
        wait();
    }
    for (int i = 1; i <= times; i++)
    {
        if (remove_shrmem(i) == 0)
            printf(1, "[P] Share memory %d removed.\n", i);
        else
        {
            printf(1, "[P] Share memory %d removing failed.\n", i);
            exit();
        }
    }
    printf(1, "Memory sharing test finished.\n");
    printf(1, "================================\n");
    return 0;
}
