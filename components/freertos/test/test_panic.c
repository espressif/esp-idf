/*
 Test for multicore FreeRTOS. This test spins up threads, fiddles with queues etc.
*/

#include <esp_types.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"

TEST_CASE("Panic handler", "[freertos][ignore]")
{
    volatile int *i;
    i = (volatile int *)0x0;
    *i = 1;
}

