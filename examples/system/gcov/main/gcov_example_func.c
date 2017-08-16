#include <stdio.h>


void blink_dummy_func(void)
{
    static int i;
    printf("Counter = %d\n", i++);
}

