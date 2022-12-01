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
#include <stdatomic.h>

#if !defined(CONFIG_FREERTOS_UNICORE) || defined(CONFIG_APPTRACE_GCOV_ENABLE)

#if CONFIG_COMPILER_OPTIMIZATION_NONE
#define IPC_STACK_SIZE (CONFIG_ESP_IPC_TASK_STACK_SIZE + 0x100)
#else
#define IPC_STACK_SIZE (CONFIG_ESP_IPC_TASK_STACK_SIZE)
#endif //CONFIG_COMPILER_OPTIMIZATION_NONE

static DRAM_ATTR StaticSemaphore_t s_ipc_mutex_buffer[portNUM_PROCESSORS];

static TaskHandle_t s_ipc_task_handle[portNUM_PROCESSORS];
static SemaphoreHandle_t s_ipc_mutex[portNUM_PROCESSORS];    // This mutex is used as a global lock for esp_ipc_* APIs
static atomic_uintptr_t s_ipc_ack[portNUM_PROCESSORS] = { ATOMIC_VAR_INIT(0), ATOMIC_VAR_INIT(0) };      // Task handle used to acknowledge that task was woken up,
                                                             // or function has finished running
static atomic_uintptr_t s_func[portNUM_PROCESSORS] = { ATOMIC_VAR_INIT(0), ATOMIC_VAR_INIT(0) };   // Function which should be called by high priority task
static atomic_uintptr_t s_func_arg[portNUM_PROCESSORS] = { ATOMIC_VAR_INIT(0), ATOMIC_VAR_INIT(0) };       // Argument to pass into s_func

typedef enum {
    IPC_WAIT_FOR_START,
    IPC_WAIT_FOR_END
} esp_ipc_wait_t;

static atomic_int s_ipc_wait[portNUM_PROCESSORS] = { ATOMIC_VAR_INIT(IPC_WAIT_FOR_START), ATOMIC_VAR_INIT(IPC_WAIT_FOR_START) };// This variable tells high priority task when it should give
                                                             //   s_ipc_ack handle: before s_func is called, or
                                                             //   after it returns

#if CONFIG_APPTRACE_GCOV_ENABLE
static volatile esp_ipc_func_t s_gcov_func = NULL;           // Gcov dump starter function which should be called by high priority task
static void * volatile s_gcov_func_arg;                      // Argument to pass into s_gcov_func
#endif

static void IRAM_ATTR ipc_task(void* arg)
{
    const int cpuid = (int)((intptr_t) arg);
    esp_ipc_func_t func;
    void* func_arg;
    esp_ipc_wait_t ipc_wait;
    TaskHandle_t ipc_ack;

    assert(cpuid == xPortGetCoreID());
#ifdef CONFIG_ESP_IPC_ISR_ENABLE
    esp_ipc_isr_init();
#endif
    while (true) {
        // Wait for IPC to be initiated.
        // This will be indicated by giving the task handle corresponding to
        // this CPU.
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == 0) {
            // TODO: when can this happen?
            abort();
        }

#if CONFIG_APPTRACE_GCOV_ENABLE
        if (s_gcov_func) {
            (*s_gcov_func)(s_gcov_func_arg);
            s_gcov_func = NULL;
            /* we can not interfer with IPC calls so no need for further processing */
            continue;
        }
#endif
        // we need to cache s_func, s_func_arg and s_ipc_wait variables locally
        // because they can be changed by a subsequent IPC call.
        func = (esp_ipc_func_t)atomic_exchange(&s_func[cpuid], 0);
        if (func) {
            func_arg = (void*)atomic_load(&s_func_arg[cpuid]);
            ipc_wait = (esp_ipc_wait_t)atomic_load(&s_ipc_wait[cpuid]);
            ipc_ack = (TaskHandle_t)atomic_exchange(&s_ipc_ack[cpuid], 0);

            if ((ipc_wait == IPC_WAIT_FOR_START) && ipc_ack) {
                xTaskNotifyGive(ipc_ack);
            }

            (*func)(func_arg);

            if ((ipc_wait == IPC_WAIT_FOR_END) && ipc_ack) {
                xTaskNotifyGive(ipc_ack);
            }
        }

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
        task_name[3] = i + 0x30;
        s_ipc_mutex[i] = xSemaphoreCreateMutexStatic(&s_ipc_mutex_buffer[i]);
        portBASE_TYPE res = xTaskCreatePinnedToCore(ipc_task, task_name, IPC_STACK_SIZE, (void*) i,
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

    TaskHandle_t task_handler = xTaskGetCurrentTaskHandle();
#ifdef CONFIG_ESP_IPC_USES_CALLERS_PRIORITY
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

    atomic_store(&s_func[cpu_id], (uintptr_t)func);
    atomic_store(&s_func_arg[cpu_id], (uintptr_t)arg);
    atomic_store(&s_ipc_wait[cpu_id], (int)wait_for);
    atomic_store(&s_ipc_ack[cpu_id], (uintptr_t)task_handler);
    xTaskNotifyGive(s_ipc_task_handle[cpu_id]);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
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
#if CONFIG_APPTRACE_GCOV_ENABLE
esp_err_t esp_ipc_start_gcov_from_isr(uint32_t cpu_id, esp_ipc_func_t func, void* arg)
{
    portBASE_TYPE ret = pdFALSE;

    if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Lock IPC to avoid interferring with normal IPC calls, e.g.
       avoid situation when esp_ipc_start_gcov_from_isr() is called from IRQ
       in the middle of IPC call between `s_func` and `s_func_arg` modification. See esp_ipc_call_and_wait() */
#ifdef CONFIG_ESP_IPC_USES_CALLERS_PRIORITY
    ret = xSemaphoreTakeFromISR(s_ipc_mutex[cpu_id], NULL);
#else
    ret = xSemaphoreTakeFromISR(s_ipc_mutex[0], NULL);
#endif
    if (ret != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    s_gcov_func = func;
    s_gcov_func_arg = arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyGiveFromISR(s_ipc_task_handle[cpu_id], &xHigherPriorityTaskWoken);

#ifdef CONFIG_ESP_IPC_USES_CALLERS_PRIORITY
    xSemaphoreGiveFromISR(s_ipc_mutex[cpu_id], NULL);
#else
    xSemaphoreGiveFromISR(s_ipc_mutex[0], NULL);
#endif
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    return ret == pdTRUE ? ESP_OK : ESP_FAIL;
}
#endif // CONFIG_APPTRACE_GCOV_ENABLE

#endif // !defined(CONFIG_FREERTOS_UNICORE) || defined(CONFIG_APPTRACE_GCOV_ENABLE)
