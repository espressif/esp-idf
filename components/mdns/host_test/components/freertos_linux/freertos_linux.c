// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void * create_q(void);

void destroy_q(void* q);

bool send_q(void* q, uint8_t *data, size_t len);

bool recv_q(void* q, uint8_t *data, size_t len, uint32_t ms);

static uint64_t s_semaphore_data = 0;

struct queue_handle {
    size_t item_size;
    void * q;
};

QueueHandle_t xQueueCreate( uint32_t uxQueueLength, uint32_t uxItemSize )
{
    struct queue_handle * h = calloc(1, sizeof(struct queue_handle));
    h->item_size = uxItemSize;
    h->q = create_q();
    return (QueueHandle_t)h;
}

uint32_t xQueueSend(QueueHandle_t xQueue, const void * pvItemToQueue, TickType_t xTicksToWait)
{
    struct queue_handle * h = xQueue;
    return send_q(h->q, (uint8_t*)pvItemToQueue, h->item_size) ? pdTRUE : pdFAIL;
}

uint32_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait)
{
    struct queue_handle * h = xQueue;
    return recv_q(h->q, (uint8_t*)pvBuffer, h->item_size, xTicksToWait) ? pdTRUE : pdFAIL;
}

BaseType_t xSemaphoreGive( QueueHandle_t xQueue)
{
    return xQueueSend(xQueue, &s_semaphore_data, portMAX_DELAY);
}

BaseType_t xSemaphoreTake( QueueHandle_t xQueue, TickType_t pvTask )
{
    return xQueueReceive(xQueue, &s_semaphore_data, portMAX_DELAY);
}

void vQueueDelete( QueueHandle_t xQueue )
{
    struct queue_handle * h = xQueue;
    if (h->q) {
        destroy_q(h->q);
    }
    free(xQueue);
}

QueueHandle_t xSemaphoreCreateBinary(void)
{
    QueueHandle_t sempaphore =  xQueueCreate(1, 1);
    return sempaphore;
}

QueueHandle_t xSemaphoreCreateMutex(void)
{
    QueueHandle_t sempaphore =  xQueueCreate(1, 1);
    if (sempaphore) {
        xSemaphoreGive(sempaphore);
    }
    return sempaphore;
}

void vTaskDelete(TaskHandle_t *task)
{
    if (task == NULL) {
        pthread_exit(0);
    }
    void *thread_rval = NULL;
    pthread_join((pthread_t)task, &thread_rval);
}

TickType_t xTaskGetTickCount( void )
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return spec.tv_nsec / 1000000 + spec.tv_sec * 1000;
}

void vTaskDelay( const TickType_t xTicksToDelay )
{
    usleep(xTicksToDelay*1000);
}

void * pthread_task(void * params)
{
    struct {
        void * const param;
        TaskFunction_t task;
        bool started;
    } *pthread_params = params;

    void * const param = pthread_params->param;
    TaskFunction_t task = pthread_params->task;
    pthread_params->started = true;

    task(param);

    return NULL;
}

BaseType_t xTaskCreatePinnedToCore(	TaskFunction_t pvTaskCode,
                                       const char * const pcName,
                                       const uint32_t usStackDepth,
                                       void * const pvParameters,
                                       UBaseType_t uxPriority,
                                       TaskHandle_t * const pvCreatedTask,
                                       const BaseType_t xCoreID)
{
    xTaskCreate(pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pvCreatedTask);
    return pdTRUE;
}


void xTaskCreate(TaskFunction_t pvTaskCode, const char * const pcName, const uint32_t usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pvCreatedTask)
{
    pthread_t new_thread = (pthread_t)NULL;
    pthread_attr_t attr;
    struct {
        void * const param;
        TaskFunction_t task;
        bool started;
    } pthread_params = { .param = pvParameters, .task = pvTaskCode};
    int res = pthread_attr_init(&attr);
    assert(res == 0);
    res = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    assert(res == 0);
    res = pthread_create(&new_thread, &attr, pthread_task, &pthread_params);
    assert(res == 0);

    if (pvCreatedTask) {
        *pvCreatedTask = (void*)new_thread;
    }

    // just wait till the task started so we can unwind params from the stack
    while (pthread_params.started == false) {
        usleep(1000);
    }
}

uint32_t esp_get_free_heap_size(void)
{
    return 0;
}

uint32_t esp_random(void)
{
    return rand();
}

void xTaskNotifyGive(TaskHandle_t task)
{

}

BaseType_t xTaskNotifyWait(uint32_t bits_entry_clear, uint32_t bits_exit_clear, uint32_t *value, TickType_t wait_time )
{
    return true;
}

TaskHandle_t xTaskGetCurrentTaskHandle(void)
{
    return NULL;
}
