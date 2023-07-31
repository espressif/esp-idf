/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "esp_err.h"
#include "esp_ipc.h"
#include "esp_private/esp_ipc_isr.h"
#include "esp_attr.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#if !defined(CONFIG_FREERTOS_UNICORE) || defined(CONFIG_APPTRACE_GCOV_ENABLE)

#if CONFIG_COMPILER_OPTIMIZATION_NONE
#define IPC_STACK_SIZE (CONFIG_ESP_IPC_TASK_STACK_SIZE + 0x100)
#else
#define IPC_STACK_SIZE (CONFIG_ESP_IPC_TASK_STACK_SIZE)
#endif //CONFIG_COMPILER_OPTIMIZATION_NONE

static DRAM_ATTR StaticSemaphore_t s_ipc_mutex_buffer[portNUM_PROCESSORS];
static DRAM_ATTR StaticSemaphore_t s_ipc_ack_buffer[portNUM_PROCESSORS];

static TaskHandle_t s_ipc_task_handle[portNUM_PROCESSORS];
static SemaphoreHandle_t s_ipc_mutex[portNUM_PROCESSORS];    // This mutex is used as a global lock for esp_ipc_* APIs
static SemaphoreHandle_t s_ipc_ack[portNUM_PROCESSORS];      // Semaphore used to acknowledge that task was woken up,
static volatile esp_ipc_func_t s_func[portNUM_PROCESSORS] = { 0 };   // Function which should be called by high priority task
static void * volatile s_func_arg[portNUM_PROCESSORS];       // Argument to pass into s_func
typedef enum {
    IPC_WAIT_NO = 0,
    IPC_WAIT_FOR_START,
    IPC_WAIT_FOR_END,
} esp_ipc_wait_t;

#if CONFIG_APPTRACE_GCOV_ENABLE
static volatile esp_ipc_func_t s_gcov_func = NULL;           // Gcov dump starter function which should be called by high priority task
static void * volatile s_gcov_func_arg;                      // Argument to pass into s_gcov_func
#endif

static void IRAM_ATTR ipc_task(void* arg)
{
    const int cpuid = (int) arg;

    assert(cpuid == xPortGetCoreID());
#ifdef CONFIG_ESP_IPC_ISR_ENABLE
    esp_ipc_isr_init();
#endif

    while (true) {
        uint32_t ipc_wait;
        xTaskNotifyWait(0, ULONG_MAX, &ipc_wait, portMAX_DELAY);

#if CONFIG_APPTRACE_GCOV_ENABLE
        if (s_gcov_func) {
            (*s_gcov_func)(s_gcov_func_arg);
            s_gcov_func = NULL;
            /* we can not interfer with IPC calls so no need for further processing */
            // esp_ipc API and gcov_from_isr APIs can be processed together if they came at the same time
            if (ipc_wait == IPC_WAIT_NO) {
                continue;
            }
        }
#endif // CONFIG_APPTRACE_GCOV_ENABLE

#ifndef CONFIG_FREERTOS_UNICORE
        if (s_func[cpuid]) {
            // we need to cache s_func, s_func_arg and ipc_ack variables locally
            // because they can be changed by a subsequent IPC call (after xTaskNotify(caller_task_handle)).
            esp_ipc_func_t func = s_func[cpuid];
            s_func[cpuid] = NULL;
            void* func_arg = s_func_arg[cpuid];
            SemaphoreHandle_t ipc_ack = s_ipc_ack[cpuid];

            if (ipc_wait == IPC_WAIT_FOR_START) {
                xSemaphoreGive(ipc_ack);
                (*func)(func_arg);
            } else if (ipc_wait == IPC_WAIT_FOR_END) {
                (*func)(func_arg);
                xSemaphoreGive(ipc_ack);
            } else {
                abort();
            }
        }
#endif // !CONFIG_FREERTOS_UNICORE
    }
    // TODO: currently this is unreachable code. Introduce esp_ipc_uninit
    // function which will signal to both tasks that they can shut down.
    // Not critical at this point, we don't have a use case for stopping
    // IPC yet.
    // Also need to delete the semaphore here.
    vTaskDelete(NULL);
}

/*
 * Initialize inter-processor call module. This function is called automatically
 * on CPU start and should not be called from the application.
 *
 * This function start two tasks, one on each CPU. These tasks are started
 * with high priority. These tasks are normally inactive, waiting until one of
 * the esp_ipc_call_* functions to be used. One of these tasks will be
 * woken up to execute the callback provided to esp_ipc_call_nonblocking or
 * esp_ipc_call_blocking.
 */
static void esp_ipc_init(void) __attribute__((constructor));

static void esp_ipc_init(void)
{
    char task_name[] = "ipcX"; // up to 10 ipc tasks/cores (0-9)

    for (int i = 0; i < portNUM_PROCESSORS; ++i) {
        task_name[3] = i + (char)'0';
        s_ipc_mutex[i] = xSemaphoreCreateMutexStatic(&s_ipc_mutex_buffer[i]);
        s_ipc_ack[i] = xSemaphoreCreateBinaryStatic(&s_ipc_ack_buffer[i]);
        BaseType_t res = xTaskCreatePinnedToCore(ipc_task, task_name, IPC_STACK_SIZE, (void*) i,
                                                    configMAX_PRIORITIES - 1, &s_ipc_task_handle[i], i);
        assert(res == pdTRUE);
        (void)res;
    }
}

static esp_err_t esp_ipc_call_and_wait(uint32_t cpu_id, esp_ipc_func_t func, void* arg, esp_ipc_wait_t wait_for)
{
    if (cpu_id >= portNUM_PROCESSORS) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_ipc_task_handle[cpu_id] == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        return ESP_ERR_INVALID_STATE;
    }

#ifdef CONFIG_ESP_IPC_USES_CALLERS_PRIORITY
    TaskHandle_t task_handler = xTaskGetCurrentTaskHandle();
    UBaseType_t priority_of_current_task = uxTaskPriorityGet(task_handler);
    UBaseType_t priority_of_running_ipc_task = uxTaskPriorityGet(s_ipc_task_handle[cpu_id]);
    if (priority_of_running_ipc_task < priority_of_current_task) {
        vTaskPrioritySet(s_ipc_task_handle[cpu_id], priority_of_current_task);
    }

    xSemaphoreTake(s_ipc_mutex[cpu_id], portMAX_DELAY);
    vTaskPrioritySet(s_ipc_task_handle[cpu_id], priority_of_current_task);
#else
    xSemaphoreTake(s_ipc_mutex[0], portMAX_DELAY);
#endif

    s_func[cpu_id] = func;
    s_func_arg[cpu_id] = arg;
    xTaskNotify(s_ipc_task_handle[cpu_id], wait_for, eSetValueWithOverwrite);
    xSemaphoreTake(s_ipc_ack[cpu_id], portMAX_DELAY);

#ifdef CONFIG_ESP_IPC_USES_CALLERS_PRIORITY
    xSemaphoreGive(s_ipc_mutex[cpu_id]);
#else
    xSemaphoreGive(s_ipc_mutex[0]);
#endif
    return ESP_OK;
}

esp_err_t esp_ipc_call(uint32_t cpu_id, esp_ipc_func_t func, void* arg)
{
    return esp_ipc_call_and_wait(cpu_id, func, arg, IPC_WAIT_FOR_START);
}

esp_err_t esp_ipc_call_blocking(uint32_t cpu_id, esp_ipc_func_t func, void* arg)
{
    return esp_ipc_call_and_wait(cpu_id, func, arg, IPC_WAIT_FOR_END);
}

// currently this is only called from gcov component
// the top level guarantees that the next call will be only after the previous one has completed
#if CONFIG_APPTRACE_GCOV_ENABLE
esp_err_t esp_ipc_start_gcov_from_isr(uint32_t cpu_id, esp_ipc_func_t func, void* arg)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        return ESP_ERR_INVALID_STATE;
    }

    // Since it is called from an interrupt, it can not wait for a mutex to be released.
    if (s_gcov_func == NULL) {
        s_gcov_func_arg = arg;
        s_gcov_func = func;

        // If the target task already has a notification pending then its notification value is not updated (WithoutOverwrite).
        xTaskNotifyFromISR(s_ipc_task_handle[cpu_id], IPC_WAIT_NO, eSetValueWithoutOverwrite, NULL);
        return ESP_OK;
    }

    // the previous call was not completed
    return ESP_FAIL;
}
#endif // CONFIG_APPTRACE_GCOV_ENABLE

#endif // !defined(CONFIG_FREERTOS_UNICORE) || defined(CONFIG_APPTRACE_GCOV_ENABLE)
