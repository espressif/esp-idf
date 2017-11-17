/*
 Tests for the interrupt watchdog
*/

#include <esp_types.h>
#include <stdio.h>
#include "rom/ets_sys.h"
#include "unity.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"



TEST_CASE("Int wdt test", "[esp32][ignore]")
{
    portENTER_CRITICAL_NESTED();
    while(1);
}
