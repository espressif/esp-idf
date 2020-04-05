// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include "unity.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "lwip/sockets.h"
#include "sdkconfig.h"
#if !CONFIG_FREERTOS_UNICORE
#include "esp_ipc.h"
#include "esp_freertos_hooks.h"
#endif

const esp_partition_t *get_test_data_partition(void)
{
    /* This finds "flash_test" partition defined in partition_table_unit_test_app.csv */
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
            ESP_PARTITION_SUBTYPE_ANY, "flash_test");
    TEST_ASSERT_NOT_NULL(result); /* means partition table set wrong */
    return result;
}

void test_case_uses_tcpip(void)
{
    // Can be called more than once, does nothing on subsequent calls
    esp_netif_init();

    // Allocate all sockets then free them
    // (First time each socket is allocated some one-time allocations happen.)
    int sockets[CONFIG_LWIP_MAX_SOCKETS];
    for (int i = 0; i < CONFIG_LWIP_MAX_SOCKETS; i++) {
        int type = (i % 2 == 0) ? SOCK_DGRAM : SOCK_STREAM;
        int family = (i % 3 == 0) ? PF_INET6 : PF_INET;
        sockets[i] = socket(family, type, IPPROTO_IP);
    }
    for (int i = 0; i < CONFIG_LWIP_MAX_SOCKETS; i++) {
        close(sockets[i]);
    }

    // Allow LWIP tasks to finish initialising themselves
    vTaskDelay(25 / portTICK_RATE_MS);

    printf("Note: esp_netif_init() has been called. Until next reset, TCP/IP task will periodicially allocate memory and consume CPU time.\n");

    // Reset the leak checker as LWIP allocates a lot of memory on first run
    unity_reset_leak_checks();
    test_utils_set_leak_level(0, TYPE_LEAK_CRITICAL, COMP_LEAK_GENERAL);
    test_utils_set_leak_level(CONFIG_UNITY_CRITICAL_LEAK_LEVEL_LWIP, TYPE_LEAK_CRITICAL, COMP_LEAK_LWIP);
}

// wait user to send "Enter" key or input parameter
static void wait_user_control(char* parameter_buf, uint8_t buf_len)
{
    char *buffer = parameter_buf;
    char sign[5];
    uint8_t buffer_len = buf_len - 1;

    if (parameter_buf == NULL) {
        buffer = sign;
        buffer_len = sizeof(sign) - 1;
    }
    // workaround that unity_gets (UartRxString) will not set '\0' correctly
    bzero(buffer, buffer_len);

    unity_gets(buffer, buffer_len);
}

// signal functions, used for sync between unity DUTs for multiple devices cases
void unity_wait_for_signal_param(const char* signal_name, char* parameter_buf, uint8_t buf_len)
{
    printf("Waiting for signal: [%s]!\n", signal_name);
    if (parameter_buf == NULL) {
        printf("Please press \"Enter\" key once any board send this signal.\n");
    } else {
        printf("Please input parameter value from any board send this signal and press \"Enter\" key.\n");
    }
    wait_user_control(parameter_buf, buf_len);
}

void unity_send_signal_param(const char* signal_name, const char *parameter)
{
    if (parameter == NULL) {
        printf("Send signal: [%s]!\n", signal_name);
    } else {
        printf("Send signal: [%s][%s]!\n", signal_name, parameter);
    }
}

bool unity_util_convert_mac_from_string(const char* mac_str, uint8_t *mac_addr)
{
    uint8_t loop = 0;
    uint8_t tmp = 0;
    const char *start;
    char *stop;

    for (loop = 0; loop < 6; loop++) {
        start = mac_str + loop * 3;
        tmp = strtol(start, &stop, 16);
        if (stop - start == 2 && (*stop == ':' || (*stop == 0 && loop == 5))) {
            mac_addr[loop] = tmp;
        } else {
            return false;
        }
    }
    return true;
}

static size_t test_unity_leak_level[TYPE_LEAK_MAX][COMP_LEAK_ALL] = { 0 };

esp_err_t test_utils_set_leak_level(size_t leak_level, esp_type_leak_t type_of_leak, esp_comp_leak_t component)
{
    if (type_of_leak >= TYPE_LEAK_MAX || component >= COMP_LEAK_ALL) {
        return ESP_ERR_INVALID_ARG;
    }
    test_unity_leak_level[type_of_leak][component] = leak_level;
    return ESP_OK;
}

size_t test_utils_get_leak_level(esp_type_leak_t type_of_leak, esp_comp_leak_t component)
{
    size_t leak_level = 0;
    if (type_of_leak >= TYPE_LEAK_MAX || component > COMP_LEAK_ALL) {
        leak_level = 0;
    } else {
        if (component == COMP_LEAK_ALL) {
            for (int comp = 0; comp < COMP_LEAK_ALL; ++comp) {
                leak_level += test_unity_leak_level[type_of_leak][comp];
            }
        } else {
            leak_level = test_unity_leak_level[type_of_leak][component];
        }
    }
    return leak_level;
}

#define EXHAUST_MEMORY_ENTRIES 100

struct test_utils_exhaust_memory_record_s {
    int *entries[EXHAUST_MEMORY_ENTRIES];
};

test_utils_exhaust_memory_rec test_utils_exhaust_memory(uint32_t caps, size_t limit)
{
    int idx = 0;
    test_utils_exhaust_memory_rec rec = calloc(1, sizeof(struct test_utils_exhaust_memory_record_s));
    TEST_ASSERT_NOT_NULL_MESSAGE(rec, "test_utils_exhaust_memory: not enough free memory to allocate record structure!");

    while (idx < EXHAUST_MEMORY_ENTRIES) {
        size_t free_caps = heap_caps_get_largest_free_block(caps);
        if (free_caps <= limit) {
            return rec; // done!
        }
        rec->entries[idx] = heap_caps_malloc(free_caps - limit, caps);
        TEST_ASSERT_NOT_NULL_MESSAGE(rec->entries[idx],
                                     "test_utils_exhaust_memory: something went wrong while freeing up memory, is another task using heap?");
        heap_caps_check_integrity_all(true);
        idx++;
    }

    TEST_FAIL_MESSAGE("test_utils_exhaust_memory: The heap with the requested caps is too fragmented, increase EXHAUST_MEMORY_ENTRIES or defrag the heap!");
    abort();
}

void test_utils_free_exhausted_memory(test_utils_exhaust_memory_rec rec)
{
    for (int i = 0; i < EXHAUST_MEMORY_ENTRIES; i++) {
        free(rec->entries[i]);
    }
    free(rec);
}

#if !CONFIG_FREERTOS_UNICORE
static SemaphoreHandle_t test_sem;

static bool test_idle_hook_func(void)
{
    if (test_sem) {
        xSemaphoreGive(test_sem);
    }
    return true;
}

static void test_task_delete_func(void *arg)
{
    vTaskDelete(arg);
}
#endif // !CONFIG_FREERTOS_UNICORE

void test_utils_task_delete(TaskHandle_t thandle)
{
    /* Self deletion can not free up associated task dynamic memory immediately,
     * hence not recommended for test scenarios */
    TEST_ASSERT_NOT_NULL_MESSAGE(thandle, "test_utils_task_delete: handle is NULL");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(thandle, xTaskGetCurrentTaskHandle(), "test_utils_task_delete: handle is of currently executing task");

#if CONFIG_FREERTOS_UNICORE
    vTaskDelete(thandle);
#else // CONFIG_FREERTOS_UNICORE
    const BaseType_t tsk_affinity = xTaskGetAffinity(thandle);
    const uint32_t core_id = xPortGetCoreID();

    printf("Task_affinity: 0x%x, current_core: %d\n", tsk_affinity, core_id);

    if (tsk_affinity == tskNO_AFFINITY) {
        /* For no affinity case, we wait for idle hook to trigger on different core */
        esp_err_t ret = esp_register_freertos_idle_hook_for_cpu(test_idle_hook_func, !core_id);
        TEST_ASSERT_EQUAL_MESSAGE(ret, ESP_OK, "test_utils_task_delete: failed to register idle hook");
        vTaskDelete(thandle);
        test_sem = xSemaphoreCreateBinary();
        TEST_ASSERT_NOT_NULL_MESSAGE(test_sem, "test_utils_task_delete: failed to create semaphore");
        xSemaphoreTake(test_sem, portMAX_DELAY);
        esp_deregister_freertos_idle_hook_for_cpu(test_idle_hook_func, !core_id);
        vSemaphoreDelete(test_sem);
        test_sem = NULL;
    } else if (tsk_affinity != core_id) {
        /* Task affinity and current core are differnt, schedule IPC call (to delete task)
         * on core where task is pinned to */
        esp_ipc_call_blocking(tsk_affinity, test_task_delete_func, thandle);
    } else {
        /* Task affinity and current core are same, so we can safely proceed for deletion */
        vTaskDelete(thandle);
    }
#endif // !CONFIG_FREERTOS_UNICORE
}
