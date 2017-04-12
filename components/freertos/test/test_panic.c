/*
 Test for multicore FreeRTOS. This test spins up threads, fiddles with queues etc.
*/

#include <esp_types.h>
#include <stdio.h>
#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "soc/uart_reg.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"


TEST_CASE("Panic handler", "[freertos][ignore]")
{
    volatile int *i;
    i = (volatile int *)0x0;
    *i = 1;
}

