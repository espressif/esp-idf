#include <stdio.h>


void some_dummy_func(void)
{
    static int i;
    printf("some_dummy_func: Counter = %d\n", i++);
    i++;
}
