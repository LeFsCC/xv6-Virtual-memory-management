#include "types.h"
#include "stat.h"
#include "user.h"

int sig = 19260817;
int main()
{
    printf(1, "================================\n");
    printf(1, "Memory sharing test started.\n");

    if (make_shrmem(sig) == 0)
        printf(1, "[P] Share memory created.\n");
    else
    {
        printf(1, "[P] Share memory creating failed.\n");
        exit();
    }

    char *content = "Hello child proc, I'm your father!";
    printf(1, "[P] Writing message to child...\n");
    if (write_shrmem(sig, content) != 0)
    {
        printf(1, "Error!\n");
        exit();
    }
    if (fork() == 0) // This is child.
    {
        char *read = malloc(4096);
        if (read_shrmem(sig, read) != 0)
        {
            printf(1, "Error!\n");
            free(read);
            exit();
        }
        printf(1, "[C] Recv: %s\n", read);
        char *write = "Hello parent proc, I'm your child!";
        printf(1, "[C] Writing message to parent...\n");
        if (write_shrmem(sig, write) != 0)
        {
            printf(1, "Error!\n");
            free(read);
            exit();
        }
        free(read);
        printf(1, "Child start sleeping\n");
        sleep(100);
        exit();
    }
    else // This is parent.
    {
        sleep(100);
        char *read = malloc(4096);
        if (read_shrmem(sig, read) != 0)
        {
            printf(1, "Error!\n");
            free(read);
            exit();
        }
        printf(1, "[P] Recv: %s\n", read);
        free(read);
        wait();
    }

    if (remove_shrmem(sig) == 0)
        printf(1, "[P] Share memory removed.\n");
    else
    {
        printf(1, "[P] Share memory removing failed.\n");
        exit();
    }

    printf(1, "Memory sharing test finished.\n");
    printf(1, "================================\n");
    return 0;
}
