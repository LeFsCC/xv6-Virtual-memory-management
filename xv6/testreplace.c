#include "types.h"
#include "stat.h"
#include "user.h"

// 递归调用，每次申请内存空间64KB
void rec(int n)
{
    if (n <= 0)
    return;
    
    if (n % 100 == 0 || n < 5)
        printf(1, "Recursion No.%d\n", n);

    int array[16384];
    int i;

    for (i = 0; i < 16384; i++)
        (void)(array[i] = i);

    rec(n - 1);
}

int main()
{
    printf(1, "Test started.\n");
    //512次恰好会发生页面置换并且内存空间可用
    int time =512;
    printf(1, "This test will recurse %d times.\n",time);
    rec(time);

    printf(1, "Test finished.\n");
    return 0;
}
