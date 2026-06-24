/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "esp_log.h"
#include "sdkconfig.h"

#if !CONFIG_FREERTOS_UNICORE
static int my_printf(const char *fmt, ...)
{
    va_list list;
    va_start(list, fmt);
    int result = vprintf(fmt, list);
    va_end(list);
    return result;
}

static void test_task(void* arg)
{
    for (unsigned i = 0; i < 5; i++) {
        printf("00000000%d\n", i);
    }
    SemaphoreHandle_t * p_done = (SemaphoreHandle_t *) arg;
    xSemaphoreGive(*p_done);
    vTaskDelay(1);
    vTaskDelete(NULL);
}

TEST_CASE("Test flockfile/funlockfile", "[newlib]")
{
    SemaphoreHandle_t done = xSemaphoreCreateBinary();
    flockfile(stdout);
    {
        xTaskCreatePinnedToCore(&test_task, "test_task", 4096, &done, 5, NULL, 1);
        // make sure test_task is already running and is actually blocked by flockfile
        TEST_ASSERT_FALSE(xSemaphoreTake(done, 100 / portTICK_PERIOD_MS));
        for (unsigned i = 0; i < 5; i++) {
            my_printf(LOG_ANSI_COLOR_BOLD_BACKGROUND(LOG_ANSI_COLOR_BLACK, LOG_ANSI_COLOR_BG_RED) "I ");
            my_printf("(%d) ", 737);
            my_printf("[%s]: ", "TAG");
            my_printf("%s %d ", "message ", i);
            my_printf("%s\n", LOG_ANSI_COLOR_RESET);
        }
        // test_task is still blocked by flockfile
        TEST_ASSERT_FALSE(xSemaphoreTake(done, 100 / portTICK_PERIOD_MS));
    }
    funlockfile(stdout);
    // after funlockfile, test_task can print a msg and complete test_task
    TEST_ASSERT_TRUE(xSemaphoreTake(done, portMAX_DELAY));
    vSemaphoreDelete(done);
}
#endif // !CONFIG_FREERTOS_UNICORE
