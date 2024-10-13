/* Application Trace to Host Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "sdkconfig.h"
#include <inttypes.h>
#include "esp_sysview_trace.h"
#include "esp_heap_trace.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


static const char *TAG = "example";

// waits on queue for memory addresses and frees memory allocated by 'alloc_task'
static void free_task(void *p)
{
    QueueHandle_t queue = (QueueHandle_t)p;
    while (1) {
        void *p = NULL;
        if (xQueueReceive(queue, ( void * )&p, portMAX_DELAY) != pdPASS) {
            ESP_LOGE(TAG, "Failed to send to queue!");
        } else {
            ESP_LOGI(TAG, "Task[%p]: free memory @ %p", xTaskGetCurrentTaskHandle(), p);
            free(p);
        }
    }
}

struct alloc_args {
    int             idx;
    TaskHandle_t    waiter;
};

// allocates memory and puts addresses to the queue
static void alloc_task(void *p)
{
    struct alloc_args *task_args = (struct alloc_args *)p;
    char task_name[20];

    QueueHandle_t queue = xQueueCreate(10, sizeof(void *));
    if(queue == 0) {
        ESP_LOGE(TAG, "Failed to create queue!");
        return;
    }
    snprintf(task_name, sizeof(task_name), "free%d", task_args->idx);
    xTaskCreatePinnedToCore(free_task, task_name, 2500, queue, 5, NULL, CONFIG_FREERTOS_NUMBER_OF_CORES-1);

    // here GDB will stop at brekpoint and execute OpenOCD command to start tracing
    for(int i = 1; i < 10; i++) {
        uint32_t sz = 2*i*(task_args->idx + 1);
        void *p = malloc(sz/2);
        // WARNING: the previous allocated memory is intentionally not deallocated in order to cause memory leak!
        p = malloc(sz);
        ESP_LOGI(TAG, "Task[%p]: allocated %"PRIu32" bytes @ %p", xTaskGetCurrentTaskHandle(), sz, p);
        if (xQueueSend(queue, ( void * )&p, portMAX_DELAY) != pdPASS) {
            ESP_LOGE(TAG, "Failed to send to queue!");
        }
        vTaskDelay(30/portTICK_PERIOD_MS);
    }
    xTaskNotifyGive(task_args->waiter);
    while(1){
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    const int num_allocers = 3;
    char task_name[20];
    // redirect log messages to the host using SystemView tracing module
    esp_log_set_vprintf(&esp_sysview_vprintf);
    // init host-based heap tracing
    if(heap_trace_init_tohost() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init heap trace!");
        return;
    }
    heap_trace_start(HEAP_TRACE_ALL);
    for (int i = 0; i < num_allocers; i++) {
        struct alloc_args *task_args = malloc(sizeof(struct alloc_args));
        if (task_args == NULL) {
            ESP_LOGE(TAG, "Failed to alloc task args!");
            heap_trace_stop();
            return;
        }
        task_args->idx = i;
        task_args->waiter = xTaskGetCurrentTaskHandle();
        snprintf(task_name, sizeof(task_name), "alloc%d", i);
        xTaskCreatePinnedToCore(alloc_task, task_name, 2500, task_args, 5, NULL, 0);
    }
    for (int i = 0; i < num_allocers; i++) {
        ESP_LOGI(TAG, "Wait notify %d", i);
        uint32_t val = ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        ESP_LOGI(TAG, "Got notify val %"PRIu32, val);
    }
    // here GDB will stop at brekpoint and execute OpenOCD command to stop tracing
    heap_trace_stop();
}
