// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "esp_err.h"
#include "esp_ipc.h"
#include "esp_attr.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"


static SemaphoreHandle_t s_ipc_mutex;                        // This mutex is used as a global lock for esp_ipc_* APIs
static SemaphoreHandle_t s_ipc_sem[portNUM_PROCESSORS];      // Two semaphores used to wake each of ipc tasks
static SemaphoreHandle_t s_ipc_ack;                          // Semaphore used to acknowledge that task was woken up,
                                                             //   or function has finished running
static volatile esp_ipc_func_t s_func;                       // Function which should be called by high priority task
static void * volatile s_func_arg;                           // Argument to pass into s_func
typedef enum {
    IPC_WAIT_FOR_START,
    IPC_WAIT_FOR_END
} esp_ipc_wait_t;

static volatile esp_ipc_wait_t s_ipc_wait;                   // This variable tells high priority task when it should give
                                                             //   s_ipc_ack semaphore: before s_func is called, or
                                                             //   after it returns

static void IRAM_ATTR ipc_task(void* arg)
{
    const uint32_t cpuid = (uint32_t) arg;
    assert(cpuid == xPortGetCoreID());
    while (true) {
        // Wait for IPC to be initiated.
        // This will be indicated by giving the semaphore corresponding to
        // this CPU.
        if (xSemaphoreTake(s_ipc_sem[cpuid], portMAX_DELAY) != pdTRUE) {
            // TODO: when can this happen?
            abort();
        }

        esp_ipc_func_t func = s_func;
        void* arg = s_func_arg;

        if (s_ipc_wait == IPC_WAIT_FOR_START) {
            xSemaphoreGive(s_ipc_ack);
        }
        (*func)(arg);
        if (s_ipc_wait == IPC_WAIT_FOR_END) {
            xSemaphoreGive(s_ipc_ack);
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
static void esp_ipc_init() __attribute__((constructor));

static void esp_ipc_init()
{
    s_ipc_mutex = xSemaphoreCreateMutex();
    s_ipc_ack = xSemaphoreCreateBinary();
    char task_name[15];
    for (int i = 0; i < portNUM_PROCESSORS; ++i) {
        snprintf(task_name, sizeof(task_name), "ipc%d", i);
        s_ipc_sem[i] = xSemaphoreCreateBinary();
        portBASE_TYPE res = xTaskCreatePinnedToCore(ipc_task, task_name, CONFIG_ESP_IPC_TASK_STACK_SIZE, (void*) i,
                                                    configMAX_PRIORITIES - 1, NULL, i);
        assert(res == pdTRUE);
    }
}

static esp_err_t esp_ipc_call_and_wait(uint32_t cpu_id, esp_ipc_func_t func, void* arg, esp_ipc_wait_t wait_for)
{
    if (cpu_id >= portNUM_PROCESSORS) {
        return ESP_ERR_INVALID_ARG;
    }
    if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        return ESP_ERR_INVALID_STATE;
    }

    xSemaphoreTake(s_ipc_mutex, portMAX_DELAY);

    s_func = func;
    s_func_arg = arg;
    s_ipc_wait = wait_for;
    xSemaphoreGive(s_ipc_sem[cpu_id]);
    xSemaphoreTake(s_ipc_ack, portMAX_DELAY);
    xSemaphoreGive(s_ipc_mutex);
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

