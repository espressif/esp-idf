#include <stdio.h>


void blink_dummy_func(void)
{
    static int i;
    printf("blink_dummy_func: Counter = %d\n", i++);
}
