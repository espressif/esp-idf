/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_cpu.h"
#include "esp_rom_sys.h"
#include "esp_ipc_isr.h"

#ifdef CONFIG_ESP_IPC_ISR_ENABLE

void esp_test_ipc_isr_asm(void* arg);

TEST_CASE("Test ipc_isr blocking IPC function calls a ASM function", "[ipc]")
{
    int val = 0x5a5a;
    esp_ipc_isr_asm_call_blocking(esp_test_ipc_isr_asm, &val);
    TEST_ASSERT_EQUAL_HEX(val, 0xa5a5);
}

void esp_test_ipc_isr_get_other_core_id(void* arg);


TEST_CASE("Test ipc_isr blocking IPC function calls get_other_core_id", "[ipc]")
{
    int val = 0x5a5a;
    esp_ipc_isr_asm_call_blocking(esp_test_ipc_isr_get_other_core_id, &val);
    TEST_ASSERT_EQUAL_HEX(val, 1);
}

TEST_CASE("Test ipc_isr exception in asm func leads to StoreProhibited not to Unhandled debug exception", "[ipc][reset=StoreProhibited,SW_CPU_RESET]")
{
    esp_ipc_isr_asm_call_blocking(esp_test_ipc_isr_asm, NULL);
}

void esp_test_ipc_isr_get_cycle_count_other_cpu(void* arg);

TEST_CASE("Test ipc_isr blocking IPC function calls get_cycle_count_other_cpu", "[ipc]")
{
    int val = 0x5a5a;
    esp_ipc_isr_asm_call_blocking(esp_test_ipc_isr_get_cycle_count_other_cpu, &val);
    esp_rom_printf("CCOUNT CPU0 = %d\n", esp_cpu_get_cycle_count());
    esp_rom_printf("CCOUNT CPU1 = %d\n", val);
}

static bool volatile s_stop;

static void task_asm(void *arg)
{
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) arg;
    int val;
    int counter = 0;
    printf("task_asm\n");
    while (s_stop == false) {
        val = 0x5a5a;
        esp_ipc_isr_asm_call_blocking(esp_test_ipc_isr_asm, &val);
        TEST_ASSERT_EQUAL_HEX(val, 0xa5a5);
        ++counter;
    }
    printf("task_asm counter = %d\n", counter);
    TEST_ASSERT_GREATER_THAN(1000000, counter);
    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

TEST_CASE("Test ipc_isr two tasks use IPC function calls", "[ipc]")
{
    SemaphoreHandle_t exit_sema[2];
    exit_sema[0] = xSemaphoreCreateBinary();
    exit_sema[1] = xSemaphoreCreateBinary();
    s_stop = false;
    printf("Test start\n");
    xTaskCreatePinnedToCore(task_asm, "task_asm", 2048, &exit_sema[0], UNITY_FREERTOS_PRIORITY - 1, NULL, 0);
    xTaskCreatePinnedToCore(task_asm, "task_asm", 2048, &exit_sema[1], UNITY_FREERTOS_PRIORITY - 1, NULL, 1);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    s_stop = true;
    xSemaphoreTake(exit_sema[0], portMAX_DELAY);
    xSemaphoreTake(exit_sema[1], portMAX_DELAY);
    printf("Test end\n");

    vSemaphoreDelete(exit_sema[0]);
    vSemaphoreDelete(exit_sema[1]);
}
#endif /* CONFIG_ESP_IPC_ISR_ENABLE */
