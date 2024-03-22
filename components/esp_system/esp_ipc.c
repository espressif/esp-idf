/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "esp_err.h"
#include "esp_ipc.h"
#include "esp_private/esp_ipc_isr.h"
#include "esp_attr.h"
#include "esp_cpu.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define IPC_MAX_PRIORITY (configMAX_PRIORITIES - 1)

#if !defined(CONFIG_FREERTOS_UNICORE) || defined(CONFIG_APPTRACE_GCOV_ENABLE)

#if CONFIG_COMPILER_OPTIMIZATION_NONE
#define IPC_STACK_SIZE (CONFIG_ESP_IPC_TASK_STACK_SIZE + 0x100)
#else
#define IPC_STACK_SIZE (CONFIG_ESP_IPC_TASK_STACK_SIZE)
#endif //CONFIG_COMPILER_OPTIMIZATION_NONE

static DRAM_ATTR StaticSemaphore_t s_ipc_mutex_buffer[CONFIG_FREERTOS_NUMBER_OF_CORES];
static DRAM_ATTR StaticSemaphore_t s_ipc_ack_buffer[CONFIG_FREERTOS_NUMBER_OF_CORES];

static TaskHandle_t s_ipc_task_handle[CONFIG_FREERTOS_NUMBER_OF_CORES];
static SemaphoreHandle_t s_ipc_mutex[CONFIG_FREERTOS_NUMBER_OF_CORES];    // This mutex is used as a global lock for esp_ipc_* APIs
static SemaphoreHandle_t s_ipc_ack[CONFIG_FREERTOS_NUMBER_OF_CORES];      // Semaphore used to acknowledge that task was woken up,
static volatile esp_ipc_func_t s_func[CONFIG_FREERTOS_NUMBER_OF_CORES] = { 0 };   // Function which should be called by high priority task
static void * volatile s_func_arg[CONFIG_FREERTOS_NUMBER_OF_CORES];       // Argument to pass into s_func
typedef enum {
    IPC_WAIT_NO = 0,
    IPC_WAIT_FOR_START,
    IPC_WAIT_FOR_END,
} esp_ipc_wait_t;

static esp_ipc_wait_t volatile s_wait_for[portNUM_PROCESSORS];

static volatile esp_ipc_func_t s_no_block_func[portNUM_PROCESSORS] = { 0 };
static volatile bool s_no_block_func_and_arg_are_ready[portNUM_PROCESSORS] = { 0 };
static void * volatile s_no_block_func_arg[portNUM_PROCESSORS];

static void IRAM_ATTR ipc_task(void* arg)
{
    const int cpuid = (int) arg;

    assert(cpuid == xPortGetCoreID());
#ifdef CONFIG_ESP_IPC_ISR_ENABLE
    esp_ipc_isr_init();
#endif

    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (s_no_block_func_and_arg_are_ready[cpuid] && s_no_block_func[cpuid]) {
            (*s_no_block_func[cpuid])(s_no_block_func_arg[cpuid]);
            s_no_block_func_and_arg_are_ready[cpuid] = false;
            s_no_block_func[cpuid] = NULL;
        }

#ifndef CONFIG_FREERTOS_UNICORE
        if (s_func[cpuid]) {
            // we need to cache s_func, s_func_arg and ipc_ack variables locally
            // because they can be changed by a subsequent IPC call (after xTaskNotify(caller_task_handle)).
            esp_ipc_func_t func = s_func[cpuid];
            void* func_arg = s_func_arg[cpuid];
            esp_ipc_wait_t ipc_wait = s_wait_for[cpuid];
            SemaphoreHandle_t ipc_ack = s_ipc_ack[cpuid];
            s_func[cpuid] = NULL;

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

    for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; ++i) {
        task_name[3] = i + (char)'0';
        s_ipc_mutex[i] = xSemaphoreCreateMutexStatic(&s_ipc_mutex_buffer[i]);
        s_ipc_ack[i] = xSemaphoreCreateBinaryStatic(&s_ipc_ack_buffer[i]);
        BaseType_t res = xTaskCreatePinnedToCore(ipc_task, task_name, IPC_STACK_SIZE, (void*) i,
                                                 IPC_MAX_PRIORITY, &s_ipc_task_handle[i], i);
        assert(res == pdTRUE);
        (void)res;
    }
}

static esp_err_t esp_ipc_call_and_wait(uint32_t cpu_id, esp_ipc_func_t func, void* arg, esp_ipc_wait_t wait_for)
{
    if (cpu_id >= CONFIG_FREERTOS_NUMBER_OF_CORES) {
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

    s_func_arg[cpu_id] = arg;
    s_wait_for[cpu_id] = wait_for;
    // s_func must be set after all other parameters. The ipc_task use this as indicator of the IPC is prepared.
    s_func[cpu_id] = func;
    xTaskNotifyGive(s_ipc_task_handle[cpu_id]);
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

esp_err_t esp_ipc_call_nonblocking(uint32_t cpu_id, esp_ipc_func_t func, void* arg)
{
    if (cpu_id >= portNUM_PROCESSORS || s_ipc_task_handle[cpu_id] == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cpu_id == xPortGetCoreID() && xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        return ESP_ERR_INVALID_STATE;
    }

    // Since it can be called from an interrupt or Scheduler is Suspened, it can not wait for a mutex to be released.
    if (esp_cpu_compare_and_set((volatile uint32_t *)&s_no_block_func[cpu_id], 0, (uint32_t)func)) {
        s_no_block_func_arg[cpu_id] = arg;
        s_no_block_func_and_arg_are_ready[cpu_id] = true;

        if (xPortInIsrContext()) {
            vTaskNotifyGiveFromISR(s_ipc_task_handle[cpu_id], NULL);
        } else {
#ifdef CONFIG_ESP_IPC_USES_CALLERS_PRIORITY
            vTaskPrioritySet(s_ipc_task_handle[cpu_id], IPC_MAX_PRIORITY);
#endif
            xTaskNotifyGive(s_ipc_task_handle[cpu_id]);
        }
        return ESP_OK;
    }

    // the previous call was not completed
    return ESP_FAIL;
}

#endif // !defined(CONFIG_FREERTOS_UNICORE) || defined(CONFIG_APPTRACE_GCOV_ENABLE)
