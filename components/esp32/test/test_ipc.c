#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "esp_ipc.h"
#include "esp_log.h"
#include "sdkconfig.h"

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
    ets_delay_us(3000000);
    vTaskDelay(1);
    while (exit_flag == false) {

    }
    ESP_LOGI("task1", "finish");
    vTaskDelete(NULL);
}

static UBaseType_t func_ipc_priority;

static void test_func_ipc(void *sema)
{
    ets_delay_us(1000000 + xPortGetCoreID() * 100);
    func_ipc_priority = uxTaskPriorityGet(NULL);
    xSemaphoreGive(*(xSemaphoreHandle *)sema);
    ets_printf("test_func_ipc: [%d, %d]\n", func_ipc_priority, xPortGetCoreID());
}

TEST_CASE("Test ipc_task works with the priority of the caller's task", "[ipc]")
{
    UBaseType_t priority = 18;
    func_ipc_priority = 0;
    vTaskPrioritySet(NULL, priority);

    xSemaphoreHandle sema_ipc_done = xSemaphoreCreateBinary();

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
    ets_delay_us(1000000 + xPortGetCoreID() * 100);
    UBaseType_t priority = uxTaskPriorityGet(NULL);
    ets_printf("test_func2_ipc: [callers_priority = %d, priority = %d, cpu = %d]\n", callers_priority, priority, xPortGetCoreID());
}

static void task(void *sema)
{
    int priority = uxTaskPriorityGet(NULL);
    ESP_LOGI("task", "start [priority = %d, cpu = %d]", priority, xPortGetCoreID());
    xSemaphoreTake(*(xSemaphoreHandle *)sema, portMAX_DELAY);
    esp_ipc_call_blocking(!xPortGetCoreID(), test_func2_ipc, &priority);
    xSemaphoreGive(*(xSemaphoreHandle *)sema);
    ESP_LOGI("task", "finish [priority = %d, cpu = %d]", priority, xPortGetCoreID());
    vTaskDelete(NULL);
}

TEST_CASE("Test multiple ipc_calls", "[ipc]")
{
    const int max_tasks = 5;
    UBaseType_t priority = uxTaskPriorityGet(NULL);
    ESP_LOGI("test", "priority = %d, cpu = %d", priority, xPortGetCoreID());
    xSemaphoreHandle sema_ipc_done[max_tasks * portNUM_PROCESSORS];

    for (int task_num = 0; task_num < max_tasks; ++task_num) {
        ++priority;
        ESP_LOGI("test", "task prio = %d", priority);
        for (int cpu_num = 0; cpu_num < portNUM_PROCESSORS; ++cpu_num) {
            sema_ipc_done[task_num * 2 + cpu_num] = xSemaphoreCreateBinary();
            xTaskCreatePinnedToCore(task, "task", 4096, &sema_ipc_done[task_num * 2 + cpu_num], priority, NULL, cpu_num);
        }
    }

    for (int task_num = 0; task_num < max_tasks; ++task_num) {
        for (int cpu_num = 0; cpu_num < portNUM_PROCESSORS; ++cpu_num) {
            xSemaphoreGive(sema_ipc_done[task_num * 2 + cpu_num]);
        }
    }

    for (int task_num = 0; task_num < max_tasks; ++task_num) {
        for (int cpu_num = 0; cpu_num < portNUM_PROCESSORS; ++cpu_num) {
            xSemaphoreTake(sema_ipc_done[task_num * 2 + cpu_num], portMAX_DELAY);
            vSemaphoreDelete(sema_ipc_done[task_num * 2 + cpu_num]);
        }
    }
}
#endif /* !CONFIG_FREERTOS_UNICORE */

#endif // CONFIG_ESP_IPC_USE_CALLERS_PRIORITY
