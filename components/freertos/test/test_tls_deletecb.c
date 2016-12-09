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



static void tskdelcb(int no, void *arg)
{
    ets_printf("Delete callback: %d = %p!\n", no,  arg);
}


static void tska(void *pvParameters)
{
    vTaskSetThreadLocalStoragePointerAndDelCallback(xTaskGetCurrentTaskHandle(), 0, (void *)0xAAAAAAAA, tskdelcb);
    while (1) {
        vTaskDelay(10000000 / portTICK_PERIOD_MS);
    }
}

static void tskb(void *pvParameters)
{
    vTaskSetThreadLocalStoragePointerAndDelCallback(xTaskGetCurrentTaskHandle(), 0, (void *)0xBBBBBBBB, tskdelcb);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TaskHandle_t a = (TaskHandle_t)pvParameters;
    printf("Killing task A\n");
    vTaskDelete(a);
    while (1) {
        vTaskDelay(10000000 / portTICK_PERIOD_MS);
    }
}


// TODO: split this thing into separate orthogonal tests
TEST_CASE("Freertos TLS delete cb", "[freertos]")
{
    TaskHandle_t a, b;

    xTaskCreatePinnedToCore(tska  , "tska"  , 2048, NULL, 3, &a, 0);
    xTaskCreatePinnedToCore(tskb  , "tska"  , 2048, a, 3, &b, 0);

    // Let stuff run for 20s
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    printf("Killing task B\n");
    //Shut down b
    vTaskDelete(b);
}

