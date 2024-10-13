/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#if !CONFIG_FREERTOS_UNICORE
#include "esp_ipc.h"
#include "esp_private/esp_ipc.h"
#endif
#include "esp_log.h"
#include "esp_rom_sys.h"

#if !CONFIG_FREERTOS_UNICORE
static void test_func_ipc_cb(void *arg)
{
    vTaskDelay(50);
    int *val = (int *)arg;
    *val = 0xa5a5;
}

TEST_CASE("Test blocking IPC function call", "[ipc]")
{
    int val = 0x5a5a;
    esp_ipc_call_blocking(!xPortGetCoreID(), test_func_ipc_cb, &val);
    TEST_ASSERT_EQUAL_HEX(val, 0xa5a5);
}

#ifdef CONFIG_ESP_IPC_USES_CALLERS_PRIORITY
static volatile bool exit_flag;

static void task1(void *sema)
{
    ESP_LOGI("task1", "start");
    esp_rom_delay_us(3000000);
    vTaskDelay(1);
    while (exit_flag == false) {

    }
    ESP_LOGI("task1", "finish");
    vTaskDelete(NULL);
}

static UBaseType_t func_ipc_priority;

static void test_func_ipc(void *sema)
{
    esp_rom_delay_us(1000000 + xPortGetCoreID() * 100);
    func_ipc_priority = uxTaskPriorityGet(NULL);
    xSemaphoreGive(*(SemaphoreHandle_t *)sema);
    esp_rom_printf("test_func_ipc: [%d, %d]\n", func_ipc_priority, xPortGetCoreID());
}

TEST_CASE("Test ipc_task works with the priority of the caller's task", "[ipc]")
{
    UBaseType_t priority = 18;
    func_ipc_priority = 0;
    vTaskPrioritySet(NULL, priority);

    SemaphoreHandle_t sema_ipc_done = xSemaphoreCreateBinary();

    exit_flag = false;
    xTaskCreatePinnedToCore(task1, "task1", 4096, NULL, priority + 2, NULL, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    ESP_LOGI("test", "Start IPC call in IPC_WAIT_FOR_START mode");
    esp_ipc_call(1, test_func_ipc, &sema_ipc_done);
    ESP_LOGI("test", "Waiting for IPC finish");
    xSemaphoreTake(sema_ipc_done, 4000 / portTICK_PERIOD_MS);
    ESP_LOGI("test", "Stop task1");
    exit_flag = true;
    xSemaphoreTake(sema_ipc_done, portMAX_DELAY);
    vSemaphoreDelete(sema_ipc_done);
    ESP_LOGI("test", "Check ipc_priority with priority caller's task. Should be the same");
    vTaskPrioritySet(NULL, 5);
    TEST_ASSERT_EQUAL(priority, func_ipc_priority);
}

static void test_func2_ipc(void *arg)
{
    int callers_priority = *(int *)arg;
    esp_rom_delay_us(10000 + xPortGetCoreID() * 100);
    UBaseType_t priority = uxTaskPriorityGet(NULL);
    esp_rom_printf("test_func2_ipc: [callers_priority = %d, priority = %d, cpu = %d]\n", callers_priority, priority, xPortGetCoreID());
}

typedef struct {
    SemaphoreHandle_t start;
    SemaphoreHandle_t done;
} test_semaphore_args_t;

static void task(void *semaphore_args)
{
    test_semaphore_args_t* test_semaphore = (test_semaphore_args_t*) semaphore_args;
    int priority = uxTaskPriorityGet(NULL);
    ESP_LOGI("task", "start [priority = %d, cpu = %d]", priority, xPortGetCoreID());
    xSemaphoreTake(test_semaphore->start, portMAX_DELAY);
    esp_ipc_call_blocking(!xPortGetCoreID(), test_func2_ipc, &priority);
    ESP_LOGI("task", "finish [priority = %d, cpu = %d]", priority, xPortGetCoreID());
    xSemaphoreGive(test_semaphore->done);
    vTaskDelete(NULL);
}

TEST_CASE("Test multiple ipc_calls", "[ipc]")
{
    const int max_tasks = 5;
    UBaseType_t priority = uxTaskPriorityGet(NULL);
    ESP_LOGI("test", "priority = %d, cpu = %d", priority, xPortGetCoreID());
    test_semaphore_args_t test_semaphore[max_tasks * CONFIG_FREERTOS_NUMBER_OF_CORES];

    for (int task_num = 0; task_num < max_tasks; ++task_num) {
        ++priority;
        ESP_LOGI("test", "task prio = %d", priority);
        for (int cpu_num = 0; cpu_num < CONFIG_FREERTOS_NUMBER_OF_CORES; ++cpu_num) {
            unsigned i = task_num * 2 + cpu_num;
            test_semaphore[i].start = xSemaphoreCreateBinary();
            test_semaphore[i].done = xSemaphoreCreateBinary();
            xTaskCreatePinnedToCore(task, "task", 4096, &test_semaphore[i], priority, NULL, cpu_num);
        }
    }

    for (int i = 0; i < max_tasks * CONFIG_FREERTOS_NUMBER_OF_CORES; ++i) {
        xSemaphoreGive(test_semaphore[i].start);
    }

    for (int i = 0; i < max_tasks * CONFIG_FREERTOS_NUMBER_OF_CORES; ++i) {
        xSemaphoreTake(test_semaphore[i].done, portMAX_DELAY);
        vSemaphoreDelete(test_semaphore[i].done);
        vSemaphoreDelete(test_semaphore[i].start);
    }
}
#endif /* CONFIG_ESP_IPC_USES_CALLERS_PRIORITY */

static void test_func_ipc_cb2(void *arg)
{
    vTaskDelay(100 / portTICK_PERIOD_MS);
    int *val = (int *)arg;
    *val = *val + 1;
}

static void test_func_ipc_cb3(void *arg)
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    int *val = (int *)arg;
    *val = *val + 1;
}

TEST_CASE("Test ipc_task can not wake up blocking task early", "[ipc]")
{
    int val1 = 20;
    esp_ipc_call(1, test_func_ipc_cb2, &val1);
    TEST_ASSERT_EQUAL(20, val1);

    int val2 = 30;
    esp_ipc_call_blocking(1, test_func_ipc_cb3, &val2);
    TEST_ASSERT_EQUAL(21, val1);
    TEST_ASSERT_EQUAL(31, val2);
}

TEST_CASE("Test ipc call nonblocking", "[ipc]")
{
    int val_for_1_call = 20;
    TEST_ESP_OK(esp_ipc_call_nonblocking(1, test_func_ipc_cb2, (void*)&val_for_1_call));
    TEST_ASSERT_EQUAL(20, val_for_1_call);

    int val_for_2_call = 30;
    TEST_ESP_ERR(ESP_FAIL, esp_ipc_call_nonblocking(1, test_func_ipc_cb3, (void*)&val_for_2_call));

    vTaskDelay(150 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(21, val_for_1_call);

    TEST_ESP_OK(esp_ipc_call_nonblocking(1, test_func_ipc_cb3, (void*)&val_for_2_call));

    vTaskDelay(550 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(31, val_for_2_call);
}

static void test_func_ipc_cb4(void *arg)
{
    int *val = (int *)arg;
    *val = *val + 1;
}

TEST_CASE("Test ipc call nonblocking when FreeRTOS Scheduler is suspended", "[ipc]")
{
#ifdef CONFIG_FREERTOS_SMP
    //Note: Scheduler suspension behavior changed in FreeRTOS SMP
    vTaskPreemptionDisable(NULL);
#else
    // Disable scheduler on the current CPU
    vTaskSuspendAll();
#endif // CONFIG_FREERTOS_SMP

    volatile int value = 20;
    TEST_ESP_OK(esp_ipc_call_nonblocking(1, test_func_ipc_cb4, (void*)&value));
    while (value == 20) { };
    TEST_ASSERT_EQUAL(21, value);

#ifdef CONFIG_FREERTOS_SMP
    //Note: Scheduler suspension behavior changed in FreeRTOS SMP
    vTaskPreemptionEnable(NULL);
#else
    xTaskResumeAll();
#endif
}
#endif /* !CONFIG_FREERTOS_UNICORE */
