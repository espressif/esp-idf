#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include <esp_spi_flash.h>
#include <esp_attr.h>
#include <esp_flash_encrypt.h>

#include "../cache_utils.h"

static QueueHandle_t result_queue;

static IRAM_ATTR void cache_test_task(void *arg)
{
    bool do_disable = (bool)arg;
    bool result;
    if(do_disable) {
        spi_flash_disable_interrupts_caches_and_other_cpu();
    }
    result = spi_flash_cache_enabled();
    if (do_disable) {
        spi_flash_enable_interrupts_caches_and_other_cpu();
    }

    TEST_ASSERT( xQueueSendToBack(result_queue, &result, 0) );
    vTaskDelete(NULL);
}

TEST_CASE("spi_flash_cache_enabled() works on both CPUs", "[spi_flash]")
{
    result_queue = xQueueCreate(1, sizeof(bool));

    for(int cpu = 0; cpu < portNUM_PROCESSORS; cpu++) {
        for(int disable = 0; disable <= 1; disable++) {
            bool do_disable = disable;
            bool result;
            printf("Testing cpu %d disabled %d\n", cpu, do_disable);

            xTaskCreatePinnedToCore(cache_test_task, "cache_check_task",
                                    2048, (void *)do_disable, configMAX_PRIORITIES-1, NULL, cpu);

            TEST_ASSERT( xQueueReceive(result_queue, &result, 2) );
            TEST_ASSERT_EQUAL(!do_disable, result);
        }
    }

    vQueueDelete(result_queue);
}

