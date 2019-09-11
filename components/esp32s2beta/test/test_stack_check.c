#include "unity.h"

#if CONFIG_COMPILER_STACK_CHECK

static void recur_and_smash(void)
{
    static int cnt;
    volatile uint8_t buf[50];
    volatile int num = sizeof(buf)+10;

    if (cnt++ < 1) {
        recur_and_smash();
    }
    for (int i = 0; i < num; i++) {
        buf[i] = 0;
    }
}


TEST_CASE("stack smashing protection", "[stack_check] [ignore]")
{
    recur_and_smash();
}

#endif
