/* Application Trace to Host Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

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

// allocates memory and puts addresses to the queue
static void alloc_task(void *p)
{
    QueueHandle_t queue = (QueueHandle_t)p;

    xTaskCreatePinnedToCore(free_task, "free", 2048, queue, 5, NULL, portNUM_PROCESSORS-1);

    // here GDB will stop at brekpoint and execute OpenOCD command to start tracing
    heap_trace_start(HEAP_TRACE_ALL);
    for(int i = 1; i < 5; i++) {
        uint32_t sz = 2*i;
        void *p = malloc(sz/2);
        // WARNING: the previous allocated memory is intentionally not deallocated in order to cause memory leak!
        p = malloc(sz);
        ESP_LOGI(TAG, "Task[%p]: allocated %d bytes @ %p", xTaskGetCurrentTaskHandle(), sz, p);
        if (xQueueSend(queue, ( void * )&p, portMAX_DELAY) != pdPASS) {
            ESP_LOGE(TAG, "Failed to send to queue!");
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    // here GDB will stop at brekpoint and execute OpenOCD command to stop tracing
    heap_trace_stop();
    while(1);
}

void app_main()
{
    // redirect log messages to the host using SystemView tracing module
    esp_log_set_vprintf(&esp_sysview_vprintf);
    QueueHandle_t queue = xQueueCreate(10, sizeof(void *));
    if(queue == 0) {
        ESP_LOGE(TAG, "Failed to create queue!");
        return;
    }
    // init host-based heap tracing
    if(heap_trace_init_tohost() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init heap trace!");
        return;
    }
    xTaskCreatePinnedToCore(alloc_task, "alloc", 2048, queue, 5, NULL, 0);
}
