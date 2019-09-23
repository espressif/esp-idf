#include <stdbool.h>
#include <string.h>

#include "esp_event.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"
#include "esp_log.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#include "esp_event.h"
#include "esp_event_private.h"
#include "esp_event_internal.h"

#include "esp_heap_caps.h"

#include "sdkconfig.h"
#include "unity.h"

#include "test_utils.h"

static const char* TAG = "test_event";

#define TEST_CONFIG_ITEMS_TO_REGISTER        5
#define TEST_CONFIG_TASKS_TO_SPAWN           2

#define TEST_CONFIG_WAIT_MULTIPLIER          5

// The initial logging "initializing test" is to ensure mutex allocation is not counted against memory not being freed
// during teardown.
#define TEST_SETUP() \
        ESP_LOGI(TAG, "initializing test"); \
        size_t free_mem_before = heap_caps_get_free_size(MALLOC_CAP_DEFAULT); \
        test_setup(); \
        s_test_core_id = xPortGetCoreID(); \
        s_test_priority = uxTaskPriorityGet(NULL);

#define TEST_TEARDOWN() \
        test_teardown(); \
        vTaskDelay(pdMS_TO_TICKS(CONFIG_ESP_INT_WDT_TIMEOUT_MS * TEST_CONFIG_WAIT_MULTIPLIER)); \
        TEST_ASSERT_EQUAL(free_mem_before, heap_caps_get_free_size(MALLOC_CAP_DEFAULT));

typedef struct {
    void* data;
    SemaphoreHandle_t start;
    SemaphoreHandle_t done;
} task_arg_t;

typedef struct {
    esp_event_base_t base;
    int32_t id;
    esp_event_handler_t* handles;
    int32_t num;
    esp_event_loop_handle_t loop;
    bool is_registration;
} handler_registration_data_t;

typedef struct {
    esp_event_base_t base;
    int32_t id;
    esp_event_loop_handle_t loop;
    int32_t num;
} post_event_data_t;

typedef struct {
    int performed;
    int expected;
    SemaphoreHandle_t done;
} performance_data_t;

typedef struct {
    void* data;
    SemaphoreHandle_t mutex;
} simple_arg_t;

typedef struct {
    int *arr;
    int index;
} ordered_data_t;

static BaseType_t s_test_core_id;
static UBaseType_t s_test_priority;

ESP_EVENT_DECLARE_BASE(s_test_base1);
ESP_EVENT_DECLARE_BASE(s_test_base2);

ESP_EVENT_DEFINE_BASE(s_test_base1);
ESP_EVENT_DEFINE_BASE(s_test_base2);

enum {
    TEST_EVENT_BASE1_EV1,
    TEST_EVENT_BASE1_EV2,
    TEST_EVENT_BASE1_MAX
};

enum {
    TEST_EVENT_BASE2_EV1,
    TEST_EVENT_BASE2_EV2,
    TEST_EVENT_BASE2_MAX
};

static BaseType_t test_event_get_core(void)
{
    static int calls = 0;

    if (portNUM_PROCESSORS > 1) {
        return (s_test_core_id + calls++) % portNUM_PROCESSORS;
    } else {
        return s_test_core_id;
    }
}

static esp_event_loop_args_t test_event_get_default_loop_args(void)
{
    esp_event_loop_args_t loop_config = {
        .queue_size = CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE,
        .task_name = "loop",
        .task_priority = s_test_priority,
        .task_stack_size = 2048,
        .task_core_id = test_event_get_core()
    };

    return loop_config;
}

static void test_event_simple_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (!event_handler_arg) {
        return;
    }
    simple_arg_t* arg = (simple_arg_t*) event_handler_arg;
    xSemaphoreTake(arg->mutex, portMAX_DELAY);

    int* count = (int*) arg->data;

    if (event_data == NULL) {
        (*count)++;
    } else {
        (*count) += *((int*) event_data);
    }

    xSemaphoreGive(arg->mutex);
}

static void test_event_ordered_dispatch(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    int *arg = (int*) event_handler_arg;
    ordered_data_t *data = *((ordered_data_t**) (event_data));

    data->arr[data->index++] = *arg;
}

static void test_event_performance_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    performance_data_t* data = (performance_data_t*) event_handler_arg;

    data->performed++;

    if (data->performed >= data->expected) {
        xSemaphoreGive(data->done);
    }
}

static void test_event_post_task(void* args)
{
    task_arg_t* arg = (task_arg_t*) args;
    post_event_data_t* data = arg->data;

    xSemaphoreTake(arg->start, portMAX_DELAY);

    for (int i = 0; i < data->num; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(data->loop, data->base, data->id, NULL, 0, portMAX_DELAY));
        vTaskDelay(1);
    }

    xSemaphoreGive(arg->done);

    vTaskDelete(NULL);
}

static void test_event_simple_handler_registration_task(void* args)
{
    task_arg_t* arg = (task_arg_t*) args;
    handler_registration_data_t* data = (handler_registration_data_t*) arg->data;

    xSemaphoreTake(arg->start, portMAX_DELAY);

    for(int i = 0; i < data->num; i++) {
        if (data->is_registration) {
            TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(data->loop, data->base, data->id, data->handles[i], NULL));
        } else {
            TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_unregister_with(data->loop, data->base, data->id, data->handles[i]));
        }
        vTaskDelay(1);
    }

    xSemaphoreGive(arg->done);

    vTaskDelete(NULL);
}

static void test_handler_post_w_task(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    simple_arg_t* arg = (simple_arg_t*) event_handler_arg;

    esp_event_loop_handle_t* loop = (esp_event_loop_handle_t*) event_data;
    int* count = (int*) arg->data;

    (*count)++;

    if (*count <= 2) {
        if (event_base == s_test_base1 && event_id == TEST_EVENT_BASE1_EV1) {
            TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(*loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
        } else{
            xSemaphoreGive((SemaphoreHandle_t) arg->mutex);
        }
    } else {
        // Test that once the queue is full and the handler attempts to post to the same loop,
        // posting does not block indefinitely.
        if (event_base == s_test_base1 && event_id == TEST_EVENT_BASE1_EV1) {
            xSemaphoreTake((SemaphoreHandle_t) arg->mutex, portMAX_DELAY);
            TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, esp_event_post_to(*loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
        }
    }
}

static void test_handler_post_wo_task(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    simple_arg_t* arg = (simple_arg_t*) event_handler_arg;

    esp_event_loop_handle_t* loop = (esp_event_loop_handle_t*) event_data;
    int* count = (int*) arg->data;

    (*count)++;

    if (*count <= 2) {
        if (event_base == s_test_base1 && event_id == TEST_EVENT_BASE1_EV1) {
            TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(*loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
        } else{
            xSemaphoreGive((SemaphoreHandle_t) arg->mutex);
        }
    } else {
        // Test that once the queue is full and the handler attempts to post to the same loop,
        // posting does not block indefinitely.
        if (event_base == s_test_base1 && event_id == TEST_EVENT_BASE1_EV1) {
            xSemaphoreTake((SemaphoreHandle_t) arg->mutex, portMAX_DELAY);
            TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(*loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
            TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, esp_event_post_to(*loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
        }
    }
}

static void test_post_from_handler_loop_task(void* args)
{
    esp_event_loop_handle_t event_loop = (esp_event_loop_handle_t) args;

    while(1) {
        TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_run(event_loop, portMAX_DELAY));
    }
}

static void test_setup(void)
{
    TEST_ASSERT_TRUE(TEST_CONFIG_TASKS_TO_SPAWN >= 2);
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create_default());
}

static void test_teardown(void)
{
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete_default());
}

#define TIMER_DIVIDER         16  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
#define TIMER_INTERVAL0_SEC   (2.0) // sample test interval for the first timer

#if CONFIG_ESP_EVENT_POST_FROM_ISR
static void test_handler_post_from_isr(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    SemaphoreHandle_t *sem = (SemaphoreHandle_t*) event_handler_arg;
    // Event data is just the address value (maybe have been truncated due to casting).
    int *data = (int*) event_data;
    TEST_ASSERT_EQUAL(*data, (int) (*sem));
    xSemaphoreGive(*sem);
}
#endif

#if CONFIG_ESP_EVENT_POST_FROM_ISR
void IRAM_ATTR test_event_on_timer_alarm(void* para)
{
    /* Retrieve the interrupt status and the counter value
       from the timer that reported the interrupt */
    uint64_t timer_counter_value =
        timer_group_get_counter_value_in_isr(TIMER_GROUP_0, TIMER_0);
    timer_group_intr_clr_in_isr(TIMER_GROUP_0, TIMER_0);
    timer_counter_value += (uint64_t) (TIMER_INTERVAL0_SEC * TIMER_SCALE);
    timer_group_set_alarm_value_in_isr(TIMER_GROUP_0, TIMER_0, timer_counter_value);

    int data = (int) para;
    // Posting events with data more than 4 bytes should fail.
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_event_isr_post(s_test_base1, TEST_EVENT_BASE1_EV1, &data, 5, NULL));
    // This should succeedd, as data is int-sized. The handler for the event checks that the passed event data
    // is correct.
    BaseType_t task_unblocked;
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_isr_post(s_test_base1, TEST_EVENT_BASE1_EV1, &data, sizeof(data), &task_unblocked));
    if (task_unblocked == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}
#endif //CONFIG_ESP_EVENT_POST_FROM_ISR

TEST_CASE("can create and delete event loops", "[event]")
{
    /* this test aims to verify that:
    *  - creating loops with and without a task succeeds
    *  - event queue can accomodate the set queue size, and drops the post when exceeded
    *  - deleting loops with unconsumed posts and unregistered handlers (when unregistration is enabled) does not leak memory */

    TEST_SETUP();

    esp_event_loop_handle_t loop1; // with dedicated task
    esp_event_loop_handle_t loop2; // without dedicated task
    esp_event_loop_handle_t loop3; // with leftover post and handlers

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop1));

    loop_args.task_name = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop2));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop3));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop3, s_test_base1, TEST_EVENT_BASE1_EV1, (void*) 0x00000001, NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop3, s_test_base1, TEST_EVENT_BASE1_EV2, (void*) 0x00000002, NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop3, s_test_base2, TEST_EVENT_BASE1_EV1, (void*) 0x00000003, NULL));

    for (int i = 0; i < loop_args.queue_size; i++) {
        int mod = i % 4;

        switch(mod) {
            case 0:
                TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop3, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
            break;
            case 1:
                TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop3, s_test_base2, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
            break;
            case 2:
                TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop3, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
            break;
            case 3:
                TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop3, s_test_base2, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
            break;
            default:
            break;
        }
    }

    TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, esp_event_post_to(loop3, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, pdMS_TO_TICKS(10)));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop2));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop3));

    TEST_TEARDOWN();
}

TEST_CASE("can register/unregister handlers for all events/all events for a specific base", "[event]")
{
    /* this test aims to verify that handlers can be registered to be called on all events
     * or for all events with specific bases */

    TEST_SETUP();

    esp_event_loop_handle_t loop;

    int count = 0;

    simple_arg_t arg = {
        .data = &count,
        .mutex = xSemaphoreCreateMutex()
    };

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    loop_args.task_name = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop));

    /* Register the handler twice to the same base and id but with a different argument (expects to return ESP_OK and log a warning)
     * This aims to verify: 1) Handler's argument to be updated
     *                      2) Registration not to leak memory
     */
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, test_event_simple_handler, NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, test_event_simple_handler, &arg));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base1, ESP_EVENT_ANY_ID, test_event_simple_handler, &arg));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_event_handler_register_with(loop, ESP_EVENT_ANY_BASE, TEST_EVENT_BASE1_EV1, test_event_simple_handler, &arg));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base1, TEST_EVENT_BASE1_EV1, test_event_simple_handler, &arg));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base1, TEST_EVENT_BASE1_EV2, test_event_simple_handler, &arg));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base2, TEST_EVENT_BASE1_EV1, test_event_simple_handler, &arg));

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_event_post_to(loop, ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, NULL, 0, portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_event_post_to(loop, s_test_base1, ESP_EVENT_ANY_ID, NULL, 0, portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_event_post_to(loop, ESP_EVENT_ANY_BASE, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY)); // exec loop, base and id level (+3)
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY)); // exec loop, base and id level (+3)

    // Post unknown events. Respective loop level and base level handlers should still execute.
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_MAX, NULL, 0, portMAX_DELAY)); // exec loop and base level (+2)
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base2, TEST_EVENT_BASE2_MAX, NULL, 0, portMAX_DELAY)); // exec loop level (+1)

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_run(loop, pdMS_TO_TICKS(10)));

    TEST_ASSERT_EQUAL(9, count); // 3 + 3 + 2 + 1

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop));

    vSemaphoreDelete(arg.mutex);

    TEST_TEARDOWN();
}

TEST_CASE("can unregister handler", "[event]")
{
    /* this test aims to verify that unregistered handlers no longer execute when events are raised */

    TEST_SETUP();

    esp_event_loop_handle_t loop;
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    loop_args.task_name = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop));

    int count = 0;

    simple_arg_t arg = {
        .data = &count,
        .mutex = xSemaphoreCreateMutex()
    };

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base1, TEST_EVENT_BASE1_EV1, test_event_simple_handler, &arg));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base2, TEST_EVENT_BASE1_EV1, test_event_simple_handler, &arg));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base2, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_run(loop, pdMS_TO_TICKS(10)));

    TEST_ASSERT_EQUAL(2, count);

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_unregister_with(loop, s_test_base1, TEST_EVENT_BASE1_EV1, test_event_simple_handler));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base2, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_run(loop, pdMS_TO_TICKS(10)));

    TEST_ASSERT_EQUAL(3, count);

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop));

    vSemaphoreDelete(arg.mutex);

    TEST_TEARDOWN();
}

TEST_CASE("can exit running loop at approximately the set amount of time", "[event]")
{
    /* this test aims to verify that running loop does not block indefinitely in cases where
     * events are posted frequently */

    TEST_SETUP();

    esp_event_loop_handle_t loop;
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    loop_args.task_name = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop));

    performance_data_t handler_data = {
        .performed = 0,
        .expected = INT32_MAX,
        .done = xSemaphoreCreateBinary()
    };

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base1, TEST_EVENT_BASE1_EV1, test_event_performance_handler, &handler_data));

    post_event_data_t post_event_data = {
        .base = s_test_base1,
        .id = TEST_EVENT_BASE1_EV1,
        .loop = loop,
        .num  = INT32_MAX
    };

    task_arg_t post_event_arg = {
        .data = &post_event_data,
        .done = xSemaphoreCreateBinary(),
        .start = xSemaphoreCreateBinary()
    };

    TaskHandle_t post_task;

    xTaskCreatePinnedToCore(test_event_post_task, "post", 2048, &post_event_arg, s_test_priority, &post_task, test_event_get_core());

    int runtime_ms = 10;
    int runtime_us = runtime_ms * 1000;

    int64_t start, diff;
    start = esp_timer_get_time();

    xSemaphoreGive(post_event_arg.start);

    // Run the loop for the runtime_ms set amount of time, regardless of whether events
    // are still being posted to the loop.
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_run(loop, pdMS_TO_TICKS(runtime_ms)));

    diff = (esp_timer_get_time() - start);

    // Threshold is 25 percent.
    TEST_ASSERT(diff < runtime_us * 1.25f);

    // Verify that the post task still continues
    TEST_ASSERT_NOT_EQUAL(pdTRUE, xSemaphoreTake(post_event_arg.done, pdMS_TO_TICKS(10)));

    vSemaphoreDelete(post_event_arg.done);
    vSemaphoreDelete(post_event_arg.start);
    vSemaphoreDelete(handler_data.done);
    vTaskDelete(post_task);

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop));

    TEST_TEARDOWN();
}

TEST_CASE("can register/unregister handlers simultaneously", "[event]")
{
    /* this test aims to verify that the event handlers list remains consistent despite
     * simultaneous access by differenct tasks */

    TEST_SETUP();

    const char* base = "base";
    int32_t id = 0;

    esp_event_loop_handle_t loop;
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop));

    ESP_LOGI(TAG, "registering handlers");

    handler_registration_data_t* registration_data = calloc(TEST_CONFIG_TASKS_TO_SPAWN, sizeof(*registration_data));
    task_arg_t* registration_arg = calloc(TEST_CONFIG_TASKS_TO_SPAWN, sizeof(*registration_arg));

    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        registration_data[i].base = base;
        registration_data[i].id = id;
        registration_data[i].loop = loop;
        registration_data[i].handles = calloc(TEST_CONFIG_ITEMS_TO_REGISTER, sizeof(esp_event_handler_t));
        registration_data[i].num = TEST_CONFIG_ITEMS_TO_REGISTER;
        registration_data[i].is_registration = true;

        for (int j = 0; j < TEST_CONFIG_ITEMS_TO_REGISTER; j++) {
            registration_data[i].handles[j] = (void*) (i * TEST_CONFIG_ITEMS_TO_REGISTER) + (j + TEST_CONFIG_ITEMS_TO_REGISTER);
        }

        registration_arg[i].start = xSemaphoreCreateBinary();
        registration_arg[i].done = xSemaphoreCreateBinary();
        registration_arg[i].data = &registration_data[i];

        xTaskCreatePinnedToCore(test_event_simple_handler_registration_task, "register", 2048, &registration_arg[i], s_test_priority, NULL, test_event_get_core());
    }

    // Give the semaphores to the spawned registration task
    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        xSemaphoreGive(registration_arg[i].start);
    }

    // Take the same semaphores in order to proceed
    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        xSemaphoreTake(registration_arg[i].done, portMAX_DELAY);
    }

    ESP_LOGI(TAG, "checking consistency of handlers list");

    // Check consistency of events list
    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        for (int j = 0; j < TEST_CONFIG_ITEMS_TO_REGISTER; j++) {
            TEST_ASSERT_TRUE(esp_event_is_handler_registered(loop, base, id, registration_data[i].handles[j]));
        }
    }

    ESP_LOGI(TAG, "unregistering handlers");

    /* Test if tasks can unregister simultaneously */

    // Unregister registered events
    handler_registration_data_t* unregistration_data = calloc(TEST_CONFIG_TASKS_TO_SPAWN, sizeof(*unregistration_data));
    task_arg_t* unregistration_arg = calloc(TEST_CONFIG_TASKS_TO_SPAWN, sizeof(*unregistration_arg));

    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        unregistration_data[i].base = base;
        unregistration_data[i].id = id;
        unregistration_data[i].loop = loop;
        unregistration_data[i].handles = calloc(TEST_CONFIG_ITEMS_TO_REGISTER, sizeof(esp_event_handler_t));
        unregistration_data[i].num = TEST_CONFIG_ITEMS_TO_REGISTER;
        unregistration_data[i].is_registration = false;

        memcpy(unregistration_data[i].handles, registration_data[i].handles, TEST_CONFIG_ITEMS_TO_REGISTER * sizeof(esp_event_handler_t));

        unregistration_arg[i].data = &unregistration_data[i];
        unregistration_arg[i].start = xSemaphoreCreateBinary();
        unregistration_arg[i].done = xSemaphoreCreateBinary();

        xTaskCreatePinnedToCore(test_event_simple_handler_registration_task, "unregister", 2048, &unregistration_arg[i], s_test_priority, NULL, test_event_get_core());
    }

    // Give the semaphores to the spawned unregistration task
    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        xSemaphoreGive(unregistration_arg[i].start);
    }

    // Take the same semaphores in order to proceed
    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        xSemaphoreTake(unregistration_arg[i].done, portMAX_DELAY);
    }

    ESP_LOGI(TAG, "checking consistency of handlers list");

    // Check consistency of events list
    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        for (int j = 0; j < TEST_CONFIG_ITEMS_TO_REGISTER; j++) {
            TEST_ASSERT_FALSE(esp_event_is_handler_registered(loop, base, id, registration_data[i].handles[j]));
        }
    }

    // Do cleanup
    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        free(registration_data[i].handles);
        vSemaphoreDelete(registration_arg[i].start);
        vSemaphoreDelete(registration_arg[i].done);

        free(unregistration_data[i].handles);
        vSemaphoreDelete(unregistration_arg[i].start);
        vSemaphoreDelete(unregistration_arg[i].done);
    }

    free(registration_data);
    free(unregistration_data);
    free(registration_arg);
    free(unregistration_arg);

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop));

    TEST_TEARDOWN();
}

TEST_CASE("can post and run events", "[event]")
{
    /* this test aims to verify that:
     *  - multiple tasks can post to the queue simultaneously
     *  - handlers recieve the appropriate handler arg and associated event data */

    TEST_SETUP();

    esp_event_loop_handle_t loop;

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    loop_args.task_name = NULL;
    loop_args.queue_size = TEST_CONFIG_TASKS_TO_SPAWN * TEST_CONFIG_ITEMS_TO_REGISTER;
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop));

    int count = 0;

    simple_arg_t arg = {
        .data = &count,
        .mutex = xSemaphoreCreateMutex()
    };

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base1, TEST_EVENT_BASE1_EV1, test_event_simple_handler, &arg));

    post_event_data_t* post_event_data = calloc(TEST_CONFIG_TASKS_TO_SPAWN, sizeof(*post_event_data));
    task_arg_t* post_event_arg = calloc(TEST_CONFIG_TASKS_TO_SPAWN, sizeof(*post_event_arg));

    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++)
    {
        post_event_data[i].base = s_test_base1;
        post_event_data[i].id = TEST_EVENT_BASE1_EV1;
        post_event_data[i].loop = loop;
        post_event_data[i].num = TEST_CONFIG_ITEMS_TO_REGISTER;

        post_event_arg[i].data = &post_event_data[i];
        post_event_arg[i].start = xSemaphoreCreateBinary();
        post_event_arg[i].done = xSemaphoreCreateBinary();

        xTaskCreatePinnedToCore(test_event_post_task, "post", 2048, &post_event_arg[i], s_test_priority, NULL, test_event_get_core());
    }

    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        xSemaphoreGive(post_event_arg[i].start);
    }

    // Execute some events as they are posted
    for (int i = 0; i < (TEST_CONFIG_TASKS_TO_SPAWN * TEST_CONFIG_ITEMS_TO_REGISTER) / 2; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_run(loop, pdMS_TO_TICKS(10)));
    }

    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        xSemaphoreTake(post_event_arg[i].done, portMAX_DELAY);
    }

    // Execute the rest
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_run(loop, pdMS_TO_TICKS(10)));

    TEST_ASSERT_EQUAL(TEST_CONFIG_TASKS_TO_SPAWN * TEST_CONFIG_ITEMS_TO_REGISTER, count);

    // Cleanup
    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        vSemaphoreDelete(post_event_arg[i].start);
        vSemaphoreDelete(post_event_arg[i].done);
    }

    free(post_event_data);
    free(post_event_arg);

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop));

    vSemaphoreDelete(arg.mutex);

    TEST_TEARDOWN();
}

static void loop_run_task(void* args)
{
    esp_event_loop_handle_t event_loop = (esp_event_loop_handle_t) args;

    while(1) {
        esp_event_loop_run(event_loop, portMAX_DELAY);
    }
}

static void performance_test(bool dedicated_task)
{
    // rand() seems to do a one-time allocation. Call it here so that the memory it allocates
    // is not counted as a leak.
    unsigned int _rand __attribute__((unused)) = rand();

    TEST_SETUP();

    const char test_base[] = "qwertyuiopasdfghjklzxvbnmmnbvcxzqwertyuiopasdfghjklzxvbnmmnbvcxz";

    #define TEST_CONFIG_BASES  (sizeof(test_base) - 1)
    #define TEST_CONFIG_IDS    (TEST_CONFIG_BASES / 2)

    // Create loop
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();
    esp_event_loop_handle_t loop;

    if (!dedicated_task) {
        loop_args.task_name = NULL;
    }

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop));

    performance_data_t data;

    // Register the handlers
    for (int base = 0; base < TEST_CONFIG_BASES; base++) {
        for (int id = 0; id < TEST_CONFIG_IDS; id++) {
            TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, test_base + base, id, test_event_performance_handler, &data));
        }
    }

    TaskHandle_t mtask = NULL;

    if (!dedicated_task) {
        xTaskCreate(loop_run_task, "loop_run", loop_args.task_stack_size, (void*) loop, loop_args.task_priority, &mtask);
    }

    // Perform performance test
    float running_sum = 0;
    float running_count = 0;

    for (int bases = 1; bases <= TEST_CONFIG_BASES; bases *= 2) {
        for (int ids = 1; ids <= TEST_CONFIG_IDS; ids *= 2) {

            data.performed = 0;
            data.expected = bases * ids;
            data.done = xSemaphoreCreateBinary();

            // Generate randomized list of posts
            int post_bases[TEST_CONFIG_BASES];
            int post_ids[TEST_CONFIG_IDS];

            for (int i = 0; i < bases; i++) {
                post_bases[i] = i;
            }

            for (int i = 0; i < ids; i++) {
                post_ids[i] = i;
            }

            for (int i = 0; i < bases; i++) {
                int rand_a  = rand() % bases;
                int rand_b  = rand() % bases;

                int temp = post_bases[rand_a];
                post_bases[rand_a]= post_bases[rand_b];
                post_bases[rand_b] = temp;
            }

            for (int i = 0; i < ids; i++) {
                int rand_a  = rand() % ids;
                int rand_b  = rand() % ids;

                int temp = post_ids[rand_a];
                post_ids[rand_a]= post_ids[rand_b];
                post_ids[rand_b] = temp;
            }

            // Post the events
            int64_t start = esp_timer_get_time();
            for (int base = 0; base < bases; base++) {
                for (int id = 0; id < ids; id++) {
                    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, test_base + post_bases[base], post_ids[id], NULL, 0, portMAX_DELAY));
                }
            }

            xSemaphoreTake(data.done, portMAX_DELAY);
            int64_t elapsed = esp_timer_get_time() - start;

            // Record data
            TEST_ASSERT_EQUAL(data.expected, data.performed);

            running_count++;
            running_sum += data.performed / (elapsed / (1000000.0));

            vSemaphoreDelete(data.done);
        }
    }

    int average = (int) (running_sum / (running_count));

    if (!dedicated_task) {
        ((esp_event_loop_instance_t*) loop)->task = mtask;
    }

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop));

    TEST_TEARDOWN();

#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
    ESP_LOGI(TAG, "events dispatched/second with profiling enabled: %d", average);
    // Enabling profiling will slow down event dispatch, so the set threshold
    // is not valid when it is enabled.
#else
#ifndef CONFIG_ESP32_SPIRAM_SUPPORT
    TEST_PERFORMANCE_GREATER_THAN(EVENT_DISPATCH, "%d", average);
#else
    TEST_PERFORMANCE_GREATER_THAN(EVENT_DISPATCH_PSRAM, "%d", average);
#endif // CONFIG_ESP32_SPIRAM_SUPPORT
#endif // CONFIG_ESP_EVENT_LOOP_PROFILING
}

TEST_CASE("performance test - dedicated task", "[event]")
{
    performance_test(true);
}

TEST_CASE("performance test - no dedicated task", "[event]")
{
    performance_test(false);
}

TEST_CASE("can post to loop from handler - dedicated task", "[event]")
{
    TEST_SETUP();

    esp_event_loop_handle_t loop_w_task;

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    int count;

    simple_arg_t arg = {
        .data = &count,
        .mutex = xSemaphoreCreateBinary()
    };

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop_w_task));

    count = 0;

    // Test that a handler can post to a different loop while there is still slots on the queue
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop_w_task, s_test_base1, TEST_EVENT_BASE1_EV1, test_handler_post_w_task, &arg));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop_w_task, s_test_base1, TEST_EVENT_BASE1_EV2, test_handler_post_w_task, &arg));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop_w_task, s_test_base1, TEST_EVENT_BASE1_EV1, &loop_w_task, sizeof(&loop_w_task), portMAX_DELAY));

    xSemaphoreTake(arg.mutex, portMAX_DELAY);

    TEST_ASSERT_EQUAL(2, count);

    // Test that other tasks can still post while  there is still slots in the queue, while handler is executing
    count = 100;

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop_w_task, s_test_base1, TEST_EVENT_BASE1_EV1, &loop_w_task, sizeof(&loop_w_task), portMAX_DELAY));

    for (int i = 0; i < loop_args.queue_size; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop_w_task, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    }

    TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, esp_event_post_to(loop_w_task, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0,
                         pdMS_TO_TICKS(CONFIG_ESP_INT_WDT_TIMEOUT_MS * TEST_CONFIG_WAIT_MULTIPLIER)));

    xSemaphoreGive(arg.mutex);

    vTaskDelay(pdMS_TO_TICKS(CONFIG_ESP_INT_WDT_TIMEOUT_MS * TEST_CONFIG_WAIT_MULTIPLIER));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop_w_task));

    vSemaphoreDelete(arg.mutex);

    TEST_TEARDOWN();
}

TEST_CASE("can post to loop from handler - no dedicated task", "[event]")
{
    TEST_SETUP();

    esp_event_loop_handle_t loop_wo_task;

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    int count;

    simple_arg_t arg = {
        .data = &count,
        .mutex = xSemaphoreCreateBinary()
    };

    count = 0;

    loop_args.queue_size = 1;
    loop_args.task_name = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop_wo_task));

    TaskHandle_t mtask;

    xTaskCreate(test_post_from_handler_loop_task, "task", 2584, (void*) loop_wo_task, s_test_priority, &mtask);

    // Test that a handler can post to a different loop while there is still slots on the queue
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop_wo_task, s_test_base1, TEST_EVENT_BASE1_EV1, test_handler_post_wo_task, &arg));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop_wo_task, s_test_base1, TEST_EVENT_BASE1_EV2, test_handler_post_wo_task, &arg));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop_wo_task, s_test_base1, TEST_EVENT_BASE1_EV1, &loop_wo_task, sizeof(&loop_wo_task), portMAX_DELAY));

    xSemaphoreTake(arg.mutex, portMAX_DELAY);

    TEST_ASSERT_EQUAL(2, count);

    count = 100;

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop_wo_task, s_test_base1, TEST_EVENT_BASE1_EV1, &loop_wo_task, sizeof(&loop_wo_task), portMAX_DELAY));

    vTaskDelay(pdMS_TO_TICKS(CONFIG_ESP_INT_WDT_TIMEOUT_MS * TEST_CONFIG_WAIT_MULTIPLIER));

    // For loop without tasks, posting is more restrictive. Posting should wait until execution of handler finishes
    TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, esp_event_post_to(loop_wo_task, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0,
                         pdMS_TO_TICKS(CONFIG_ESP_INT_WDT_TIMEOUT_MS * TEST_CONFIG_WAIT_MULTIPLIER)));

    xSemaphoreGive(arg.mutex);

    vTaskDelay(pdMS_TO_TICKS(CONFIG_ESP_INT_WDT_TIMEOUT_MS * TEST_CONFIG_WAIT_MULTIPLIER));

    vTaskDelete(mtask);

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop_wo_task));

    vSemaphoreDelete(arg.mutex);

    TEST_TEARDOWN();
}

static void test_event_simple_handler_template(void* handler_arg, esp_event_base_t base, int32_t id, void* event_arg)
{
    int* count = (int*) handler_arg;
    (*count)++;
}

static void test_event_simple_handler_1(void* handler_arg, esp_event_base_t base, int32_t id, void* event_arg)
{
    test_event_simple_handler_template(handler_arg, base, id, event_arg);
}

static void test_event_simple_handler_3(void* handler_arg, esp_event_base_t base, int32_t id, void* event_arg)
{
    test_event_simple_handler_template(handler_arg, base, id, event_arg);
}

static void test_event_simple_handler_2(void* handler_arg, esp_event_base_t base, int32_t id, void* event_arg)
{
    test_event_simple_handler_template(handler_arg, base, id, event_arg);
}

static void test_registration_from_handler_hdlr(void* handler_arg, esp_event_base_t base, int32_t id, void* event_arg)
{
    esp_event_loop_handle_t* loop = (esp_event_loop_handle_t*) event_arg;
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(*loop, s_test_base1, TEST_EVENT_BASE1_EV2, test_event_simple_handler_1, handler_arg));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(*loop, s_test_base1, TEST_EVENT_BASE1_EV2, test_event_simple_handler_2, handler_arg));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(*loop, s_test_base1, TEST_EVENT_BASE1_EV2, test_event_simple_handler_3, handler_arg));
}

static void test_unregistration_from_handler_hdlr(void* handler_arg, esp_event_base_t base, int32_t id, void* event_arg)
{
    esp_event_loop_handle_t* loop = (esp_event_loop_handle_t*) event_arg;
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_unregister_with(*loop, s_test_base1, TEST_EVENT_BASE1_EV2, test_event_simple_handler_1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_unregister_with(*loop, s_test_base1, TEST_EVENT_BASE1_EV2, test_event_simple_handler_2));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_unregister_with(*loop, s_test_base1, TEST_EVENT_BASE1_EV2, test_event_simple_handler_3));
}

TEST_CASE("can register from handler", "[event]")
{
    TEST_SETUP();

    esp_event_loop_handle_t loop;

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    loop_args.task_name = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop));

    int count = 0;

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base1, TEST_EVENT_BASE1_EV1, test_registration_from_handler_hdlr, &count));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base2, TEST_EVENT_BASE2_EV1, test_unregistration_from_handler_hdlr, &count));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV1, &loop, sizeof(&loop), portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_run(loop, pdMS_TO_TICKS(10)));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_run(loop, pdMS_TO_TICKS(10)));

    TEST_ASSERT_EQUAL(3, count);

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base2, TEST_EVENT_BASE2_EV1, &loop, sizeof(&loop), portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_run(loop, pdMS_TO_TICKS(10)));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_run(loop, pdMS_TO_TICKS(10)));

    TEST_ASSERT_EQUAL(3, count);

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop));

    TEST_TEARDOWN();
}

static void test_create_loop_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    if (id == TEST_EVENT_BASE1_EV1) {
        TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, (esp_event_loop_handle_t*) handler_args));
    } else {
        TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(*((esp_event_loop_handle_t*) handler_args)));
    }
}

TEST_CASE("can create and delete loop from handler", "[event]")
{
    TEST_SETUP();

    esp_event_loop_handle_t loop;
    esp_event_loop_handle_t test_loop;

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    loop_args.task_name = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base1, TEST_EVENT_BASE1_EV1, test_create_loop_handler, &test_loop));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base1, TEST_EVENT_BASE1_EV2, test_create_loop_handler, &test_loop));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_run(loop, pdMS_TO_TICKS(10)));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_run(loop, pdMS_TO_TICKS(10)));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop));

    TEST_TEARDOWN();
}

TEST_CASE("events are dispatched in the order they are registered", "[event]")
{
    TEST_SETUP();

    esp_event_loop_handle_t loop;
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    loop_args.task_name = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop));

    int id_arr[7];

    for (int i = 0; i < 7; i++) {
        id_arr[i] = i;
    }

    int data_arr[12] = {0};

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base2, TEST_EVENT_BASE2_EV1, test_event_ordered_dispatch, id_arr + 0));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, test_event_ordered_dispatch, id_arr + 1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base1, ESP_EVENT_ANY_ID, test_event_ordered_dispatch, id_arr + 2));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base2, TEST_EVENT_BASE2_EV2, test_event_ordered_dispatch, id_arr + 3));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base1, TEST_EVENT_BASE1_EV1, test_event_ordered_dispatch, id_arr + 4));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base2, ESP_EVENT_ANY_ID, test_event_ordered_dispatch, id_arr + 5));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base1, TEST_EVENT_BASE1_EV2, test_event_ordered_dispatch, id_arr + 6));

    esp_event_dump(stdout);

    ordered_data_t data = {
        .arr = data_arr,
        .index = 0
    };

    ordered_data_t* dptr = &data;

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base2, TEST_EVENT_BASE1_EV2, &dptr, sizeof(dptr), portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV1, &dptr, sizeof(dptr), portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV2, &dptr, sizeof(dptr), portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base2, TEST_EVENT_BASE1_EV1, &dptr, sizeof(dptr), portMAX_DELAY));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_run(loop, pdMS_TO_TICKS(10)));

    // Expected data executing the posts above
    int ref_arr[12] = {1, 3, 5, 1, 2, 4, 1, 2, 6, 0, 1, 5};

    for (int i = 0; i < 12; i++) {
        TEST_ASSERT_EQUAL(ref_arr[i], data_arr[i]);
    }

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop));

    TEST_TEARDOWN();
}

#if CONFIG_ESP_EVENT_POST_FROM_ISR
TEST_CASE("can properly prepare event data posted to loop", "[event]")
{
    TEST_SETUP();

    esp_event_loop_handle_t loop;
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    loop_args.task_name = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop));

    esp_event_post_instance_t post;
    esp_event_loop_instance_t* loop_def = (esp_event_loop_instance_t*) loop;

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(loop_def->queue, &post, portMAX_DELAY));
    TEST_ASSERT_EQUAL(false, post.data_set);
    TEST_ASSERT_EQUAL(false, post.data_allocated);
    TEST_ASSERT_EQUAL(NULL, post.data.ptr);

    int sample = 0;
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_isr_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV1, &sample, sizeof(sample), NULL));
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(loop_def->queue, &post, portMAX_DELAY));
    TEST_ASSERT_EQUAL(true, post.data_set);
    TEST_ASSERT_EQUAL(false, post.data_allocated);
    TEST_ASSERT_EQUAL(false, post.data.val);

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop));

    TEST_TEARDOWN();
}

TEST_CASE("can post events from interrupt handler", "[event]")
{
    SemaphoreHandle_t sem = xSemaphoreCreateBinary();

    /* Select and initialize basic parameters of the timer */
    timer_config_t config;
    config.divider = TIMER_DIVIDER;
    config.counter_dir = TIMER_COUNT_UP;
    config.counter_en = TIMER_PAUSE;
    config.alarm_en = TIMER_ALARM_EN;
    config.intr_type = TIMER_INTR_LEVEL;
    config.auto_reload = false;
    timer_init(TIMER_GROUP_0, TIMER_0, &config);

    /* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TIMER_INTERVAL0_SEC * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, test_event_on_timer_alarm,
        (void *) sem, ESP_INTR_FLAG_IRAM, NULL);

    timer_start(TIMER_GROUP_0, TIMER_0);

    TEST_SETUP();

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register(s_test_base1, TEST_EVENT_BASE1_EV1,
                                                        test_handler_post_from_isr, &sem));

    xSemaphoreTake(sem, portMAX_DELAY);

    TEST_TEARDOWN();
}
#endif

#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
TEST_CASE("can dump event loop profile", "[event]")
{
    /* this test aims to verify that dumping event loop statistics succeed */

    TEST_SETUP();

    esp_event_loop_handle_t loop;

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    loop_args.task_name = NULL;
    loop_args.queue_size = 5;
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create(&loop_args, &loop));

    int count = 0;

    simple_arg_t arg = {
        .data = &count,
        .mutex = xSemaphoreCreateMutex()
    };

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, test_event_simple_handler, &arg));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base1, ESP_EVENT_ANY_ID, test_event_simple_handler, &arg));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base1, TEST_EVENT_BASE1_EV1, test_event_simple_handler, &arg));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base1, TEST_EVENT_BASE1_EV2, test_event_simple_handler, &arg));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base2, TEST_EVENT_BASE1_EV1, test_event_simple_handler, &arg));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_handler_register_with(loop, s_test_base2, TEST_EVENT_BASE1_EV2, test_event_simple_handler, &arg));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, 1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base2, TEST_EVENT_BASE1_EV1, NULL, 0, 1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, 1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base2, TEST_EVENT_BASE1_EV2, NULL, 0, 1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, 1));
    TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, esp_event_post_to(loop, s_test_base2, TEST_EVENT_BASE1_EV1, NULL, 0, 1));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_run(loop, pdMS_TO_TICKS(10)));

    // 5 invocations of loop-levlel handlers + 3 invocation of base-level handlers (s_test_base1) +
    // 5 invocations of respective event-level handlers
    TEST_ASSERT_EQUAL(13, count);

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_dump(stdout));

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete(loop));

    vSemaphoreDelete(arg.mutex);

    TEST_TEARDOWN();
}
#endif
