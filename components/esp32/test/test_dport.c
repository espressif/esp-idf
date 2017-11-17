
#include <esp_types.h>
#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "unity.h"

#include "soc/uart_reg.h"
#include "soc/dport_reg.h"

static volatile bool exit_flag;
static bool dport_test_result;
static bool apb_test_result;

static void accessDPORT(void *pvParameters)
{
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;
    uint32_t dport_date = DPORT_REG_READ(DPORT_DATE_REG);

    dport_test_result = true;

    // although exit flag is set in another task, checking (exit_flag == false) is safe
    while (exit_flag == false) {
        if (dport_date != DPORT_REG_READ(DPORT_DATE_REG)) {
            dport_test_result = false;
            break;
        }
    }

    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

static void accessAPB(void *pvParameters)
{
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;
    uint32_t uart_date = REG_READ(UART_DATE_REG(0));

    apb_test_result = true;

    // although exit flag is set in another task, checking (exit_flag == false) is safe
    while (exit_flag == false) {
        if (uart_date != REG_READ(UART_DATE_REG(0))) {
            apb_test_result = false;
            break;
        }
    }

    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

TEST_CASE("access DPORT and APB at same time", "[esp32]")
{
    int i;
    TaskHandle_t th[2];
    xSemaphoreHandle exit_sema[2];

    for (i=0; i<2; i++) {
        exit_sema[i] = xSemaphoreCreateMutex();
        xSemaphoreTake(exit_sema[i], portMAX_DELAY);
    }

    exit_flag = false;

#ifndef CONFIG_FREERTOS_UNICORE
    printf("assign task accessing DPORT to core 0 and task accessing APB to core 1\n");
    xTaskCreatePinnedToCore(accessDPORT  , "accessDPORT"  , 2048, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, &th[0], 0);
    xTaskCreatePinnedToCore(accessAPB  , "accessAPB"  , 2048, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, &th[1], 1);
#else
    printf("assign task accessing DPORT and accessing APB\n");
    xTaskCreate(accessDPORT  , "accessDPORT"  , 2048, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, &th[0]);
    xTaskCreate(accessAPB  , "accessAPB"  , 2048, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, &th[1]);
#endif

    printf("start wait for 10 seconds\n");
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    // set exit flag to let thread exit
    exit_flag = true;

    for (i=0; i<2; i++) {
        xSemaphoreTake(exit_sema[i], portMAX_DELAY);
        vSemaphoreDelete(exit_sema[i]);
    }

    TEST_ASSERT(dport_test_result == true && apb_test_result == true);
}

