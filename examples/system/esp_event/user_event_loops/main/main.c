/* esp_event (event loop library) basic example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "event_source.h"
#include "esp_event_base.h"

static const char* TAG = "user_event_loops";

// Event loops
esp_event_loop_handle_t loop_with_task;
esp_event_loop_handle_t loop_without_task;

static void application_task(void* args)
{
    // Wait to be started by the main task
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    while(1) {
        ESP_LOGI(TAG, "application_task: running application task");
        esp_event_loop_run(loop_without_task, 100);
        vTaskDelay(10);
    }
}

/* Event source task related definitions */
ESP_EVENT_DEFINE_BASE(TASK_EVENTS);

TaskHandle_t g_task;

static void task_iteration_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    // Two types of data can be passed in to the event handler: the handler specific data and the event-specific data.
    //
    // The handler specific data (handler_args) is a pointer to the original data, therefore, the user should ensure that
    // the memory location it points to is still valid when the handler executes.
    //
    // The event-specific data (event_data) is a pointer to a deep copy of the original data, and is managed automatically.
    int iteration = *((int*) event_data);

    char* loop;

    if (handler_args == loop_with_task) {
        loop = "loop_with_task";
    } else {
        loop = "loop_without_task";
    }

    ESP_LOGI(TAG, "handling %s:%s from %s, iteration %d", base, "TASK_ITERATION_EVENT", loop, iteration);
}

static void task_event_source(void* args)
{
    // Wait to be started by the main task
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    for(int iteration = 1; iteration <= TASK_ITERATIONS_COUNT; iteration++) {
        esp_event_loop_handle_t loop_to_post_to;

        if (iteration % 2 == 0) {
            // if even, post to the event loop with dedicated task
            loop_to_post_to = loop_with_task;
        } else {
            // if odd, post to the event loop without a dedicated task
            loop_to_post_to = loop_without_task;
        }

        ESP_LOGI(TAG, "posting %s:%s to %s, iteration %d out of %d", TASK_EVENTS, "TASK_ITERATION_EVENT",
                loop_to_post_to == loop_with_task ? "loop_with_task" : "loop_without_task",
                iteration, TASK_ITERATIONS_COUNT);

        ESP_ERROR_CHECK(esp_event_post_to(loop_to_post_to, TASK_EVENTS, TASK_ITERATION_EVENT, &iteration, sizeof(iteration), portMAX_DELAY));

        vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD));
    }

    vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD));

    ESP_LOGI(TAG, "deleting task event source");

    vTaskDelete(NULL);
}

/* Example main */
void app_main(void)
{
    ESP_LOGI(TAG, "setting up");

    esp_event_loop_args_t loop_with_task_args = {
        .queue_size = 5,
        .task_name = "loop_task", // task will be created
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 3072,
        .task_core_id = tskNO_AFFINITY
    };

    esp_event_loop_args_t loop_without_task_args = {
        .queue_size = 5,
        .task_name = NULL // no task will be created
    };

    // Create the event loops
    ESP_ERROR_CHECK(esp_event_loop_create(&loop_with_task_args, &loop_with_task));
    ESP_ERROR_CHECK(esp_event_loop_create(&loop_without_task_args, &loop_without_task));

    // Register the handler for task iteration event. Notice that the same handler is used for handling event on different loops.
    // The loop handle is provided as an argument in order for this example to display the loop the handler is being run on.
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(loop_with_task, TASK_EVENTS, TASK_ITERATION_EVENT, task_iteration_handler, loop_with_task, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(loop_without_task, TASK_EVENTS, TASK_ITERATION_EVENT, task_iteration_handler, loop_without_task, NULL));

    // Create the event source task
    TaskHandle_t task_event_source_hdl;
    ESP_LOGI(TAG, "starting event source");
    xTaskCreate(task_event_source, "task_event_source", 3072, NULL, uxTaskPriorityGet(NULL) + 1, &task_event_source_hdl);

    // Create the application task
    TaskHandle_t application_task_hdl;
    ESP_LOGI(TAG, "starting application task");
    xTaskCreate(application_task, "application_task", 3072, NULL, uxTaskPriorityGet(NULL) + 1, &application_task_hdl);

    // Start the event source task first to post an event
    xTaskNotifyGive(task_event_source_hdl);

    // Start the application task to run the event handlers
    xTaskNotifyGive(application_task_hdl);
}
