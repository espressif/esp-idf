/*
 See if xPortInIsrContext works
*/

#include <esp_types.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "esp_intr_alloc.h"
#include "xtensa/hal.h"

static volatile int in_int_context, int_handled;


static void testint(void *arg) {
    xthal_set_ccompare(1, xthal_get_ccount()+8000000000);
    ets_printf("INT!\n");
    if (xPortInIsrContext()) in_int_context++;
    int_handled++;
}


static void testthread(void *arg) {
    intr_handle_t handle;
    in_int_context=0;
    int_handled=0;
    TEST_ASSERT(!xPortInIsrContext());
    xthal_set_ccompare(1, xthal_get_ccount()+8000000);
    esp_err_t err = esp_intr_alloc(ETS_INTERNAL_TIMER1_INTR_SOURCE, 0, &testint, NULL, &handle);
    TEST_ASSERT_EQUAL_HEX32(ESP_OK, err);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT(int_handled);
    TEST_ASSERT(in_int_context);
    TEST_ASSERT_EQUAL_HEX32( ESP_OK, esp_intr_free(handle) );
    vTaskDelete(NULL);
}


TEST_CASE("xPortInIsrContext test", "[freertos]")
{
    xTaskCreatePinnedToCore(testthread, "tst" , 4096, NULL, 3, NULL, 0);
    vTaskDelay(150 / portTICK_PERIOD_MS);
#if portNUM_PROCESSORS == 2
    xTaskCreatePinnedToCore(testthread, "tst" , 4096, NULL, 3, NULL, 1);
    vTaskDelay(150 / portTICK_PERIOD_MS);
#endif
}

