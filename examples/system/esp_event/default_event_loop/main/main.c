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

static const char* TAG = "default_event_loop";

static esp_event_handler_instance_t s_instance;

static int TIMER_START_HANDLER_0 = 0;
static int TIMER_START_HANDLER_1 = 1;

static char* get_id_string(esp_event_base_t base, int32_t id) {
    char* event = "";
    if (base == TIMER_EVENTS) {
        switch(id) {
            case TIMER_EVENT_STARTED:
            event = "TIMER_EVENT_STARTED";
            break;
            case TIMER_EVENT_EXPIRY:
            event = "TIMER_EVENT_EXPIRY";
            break;
            case TIMER_EVENT_STOPPED:
            event = "TIMER_EVENT_STOPPED";
            break;
        }
    } else {
        event = "TASK_ITERATION_EVENT";
    }
    return event;
}

/* Event source periodic timer related definitions */
ESP_EVENT_DEFINE_BASE(TIMER_EVENTS);

esp_timer_handle_t TIMER;

// Callback that will be executed when the timer period lapses. Posts the timer expiry event
// to the default event loop.
static void timer_callback(void* arg)
{
    ESP_LOGI(TAG, "%s:%s: posting to default loop", TIMER_EVENTS, get_id_string(TIMER_EVENTS, TIMER_EVENT_EXPIRY));
    ESP_ERROR_CHECK(esp_event_post(TIMER_EVENTS, TIMER_EVENT_EXPIRY, NULL, 0, portMAX_DELAY));
}

// Handler which executes when the timer started event gets executed by the loop.
static void timer_started_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    int start_handler_num = *((int*) handler_args);
    ESP_LOGI(TAG, "%s:%s: timer_started_handler, instance %d", base, get_id_string(base, id), start_handler_num);
}

// Second handler which executes when the timer started event gets executed by the loop.
static void timer_started_handler_2(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    ESP_LOGI(TAG, "%s:%s: timer_started_handler_2", base, get_id_string(base, id));
}

// Handler which executes when the timer expiry event gets executed by the loop. This handler keeps track of
// how many times the timer expired. When a set number of expiry is reached, the handler stops the timer
// and sends a timer stopped event.
static void timer_expiry_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    static int count = 0;

    count++;

    if (count >= TIMER_EXPIRIES_COUNT) {
        // Stop the timer
        ESP_ERROR_CHECK(esp_timer_stop(TIMER));

        ESP_LOGI(TAG, "%s:%s: posting to default loop", base, get_id_string(base, TIMER_EVENT_STOPPED));

        // Post the event that the timer has been stopped
        ESP_ERROR_CHECK(esp_event_post(TIMER_EVENTS, TIMER_EVENT_STOPPED, NULL, 0, portMAX_DELAY));
    }

    ESP_LOGI(TAG, "%s:%s: timer_expiry_handler, executed %d out of %d times", base, get_id_string(base, id), count, TIMER_EXPIRIES_COUNT);
}

// Handler which executes when any timer event (started, expiry, stopped) get executed by the loop
static void timer_any_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    ESP_LOGI(TAG, "%s:%s: timer_any_handler", base, get_id_string(base, id));
}

// Handler which executes when the timer stopped event gets executed by the loop. Since the timer has been
// stopped, it is safe to delete it.
static void timer_stopped_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    ESP_LOGI(TAG, "%s:%s: timer_stopped_handler", base, get_id_string(base, id));

    // Delete the timer
    esp_timer_delete(TIMER);

    ESP_LOGI(TAG, "%s:%s: deleted timer event source", base, get_id_string(base, id));
}

/* Event source task related definitions */
ESP_EVENT_DEFINE_BASE(TASK_EVENTS);

static void task_iteration_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    int iteration = *((int*) event_data);
    ESP_LOGI(TAG, "%s:%s: task_iteration_handler, executed %d times", base, get_id_string(base, id), iteration);
}

static void task_event_source(void* args)
{
    for(int iteration = 1; iteration <= TASK_ITERATIONS_COUNT; iteration++) {

        ESP_LOGI(TAG, "%s:%s: posting to default loop, %d out of %d", TASK_EVENTS,
                get_id_string(TASK_EVENTS, TASK_ITERATION_EVENT), iteration, TASK_ITERATIONS_COUNT);

        // Post that the loop has iterated. Notice that the iteration count is passed to the handler. Take note
        // that data passed during event posting is a deep copy of the original data.
        ESP_ERROR_CHECK(esp_event_post(TASK_EVENTS, TASK_ITERATION_EVENT, &iteration, sizeof(iteration), portMAX_DELAY));

        if (iteration == TASK_ITERATIONS_UNREGISTER) {
            ESP_LOGI(TAG, "%s:%s: unregistering task_iteration_handler", TASK_EVENTS, get_id_string(TASK_EVENTS, TASK_ITERATION_EVENT));
            ESP_ERROR_CHECK(esp_event_handler_instance_unregister(TASK_EVENTS, TASK_ITERATION_EVENT, s_instance));
        }

        vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD));
    }

    vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD));

    ESP_LOGI(TAG, "%s:%s: deleting task event source", TASK_EVENTS, get_id_string(TASK_EVENTS, TASK_ITERATION_EVENT));

    vTaskDelete(NULL);
}

/* Handler for all events */
static void all_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    ESP_LOGI(TAG, "%s:%s: all_event_handler", base, get_id_string(base, id));
}

/* Example main */
void app_main(void)
{
    ESP_LOGI(TAG, "setting up");

    // Create the default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Register the specific timer event handlers. Timer start handler is registered twice.
    ESP_ERROR_CHECK(esp_event_handler_instance_register(TIMER_EVENTS, TIMER_EVENT_STARTED, timer_started_handler, &TIMER_START_HANDLER_0, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(TIMER_EVENTS, TIMER_EVENT_STARTED, timer_started_handler, &TIMER_START_HANDLER_1, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(TIMER_EVENTS, TIMER_EVENT_STARTED, timer_started_handler_2, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(TIMER_EVENTS, TIMER_EVENT_EXPIRY, timer_expiry_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(TIMER_EVENTS, TIMER_EVENT_STOPPED, timer_stopped_handler, NULL, NULL));

    // Register the handler for all timer family events. This will execute if the timer is started, expired or is stopped.
    ESP_ERROR_CHECK(esp_event_handler_instance_register(TIMER_EVENTS, ESP_EVENT_ANY_ID, timer_any_handler, NULL, NULL));

    // Register the handler for task iteration event; need to pass instance handle for later unregistration.
    ESP_ERROR_CHECK(esp_event_handler_instance_register(TASK_EVENTS, TASK_ITERATION_EVENT, task_iteration_handler, NULL, &s_instance));

    // Register the handler for all event. This will execute if either the timer events or the task iteration event
    // is posted to the default loop.
    ESP_ERROR_CHECK(esp_event_handler_instance_register(ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, all_event_handler, NULL, NULL));

    // Create and start the event sources
    esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
    };

    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &TIMER));

    ESP_LOGI(TAG, "starting event sources");

     // Create the event source task with the same priority as the current task
    xTaskCreate(task_event_source, "task_event_source", 4096, NULL, uxTaskPriorityGet(NULL), NULL);

    ESP_ERROR_CHECK(esp_timer_start_periodic(TIMER, TIMER_PERIOD));

    // Post the timer started event
    ESP_LOGI(TAG, "%s:%s: posting to default loop", TIMER_EVENTS, get_id_string(TIMER_EVENTS, TIMER_EVENT_STARTED));
    ESP_ERROR_CHECK(esp_event_post(TIMER_EVENTS, TIMER_EVENT_STARTED, NULL, 0, portMAX_DELAY));
}
