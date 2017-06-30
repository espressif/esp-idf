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


static TaskHandle_t s_ipc_tasks[portNUM_PROCESSORS];         // Two high priority tasks, one for each CPU
static SemaphoreHandle_t s_ipc_mutex;                        // This mutex is used as a global lock for esp_ipc_* APIs
static SemaphoreHandle_t s_ipc_sem[portNUM_PROCESSORS];      // Two semaphores used to wake each of s_ipc_tasks
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

void esp_ipc_init()
{
    s_ipc_mutex = xSemaphoreCreateMutex();
    s_ipc_ack = xSemaphoreCreateBinary();
    const char* task_names[2] = {"ipc0", "ipc1"};
    for (int i = 0; i < portNUM_PROCESSORS; ++i) {
        s_ipc_sem[i] = xSemaphoreCreateBinary();
        xTaskCreatePinnedToCore(ipc_task, task_names[i], CONFIG_IPC_TASK_STACK_SIZE, (void*) i,
                                configMAX_PRIORITIES - 1, &s_ipc_tasks[i], i);
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
    s_ipc_wait = IPC_WAIT_FOR_START;
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

