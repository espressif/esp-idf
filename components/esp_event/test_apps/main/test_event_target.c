/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/* This file contains tests only runnable on the chip targets */

#include <stdbool.h>
#include <string.h>

#include "esp_event.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gptimer.h"

#include "esp_event.h"
#include "esp_event_private.h"
#include "esp_event_internal.h"

#include "esp_heap_caps.h"
#include "esp_timer.h"

#include "sdkconfig.h"
#include "unity.h"
#include "unity_test_utils_memory.h"

#include "test_utils.h"

static const char* TAG = "test_event";

static const TickType_t ZERO_DELAY = 0;

#define TEST_CONFIG_ITEMS_TO_REGISTER        5
#define TEST_CONFIG_TASKS_TO_SPAWN           2
_Static_assert(TEST_CONFIG_TASKS_TO_SPAWN >= 2); // some tests test simultaneous posting of events, etc.

/* Time used in tearDown function to wait for cleaning up memory in background tasks */
#define TEST_CONFIG_TEARDOWN_WAIT           30

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

ESP_EVENT_DECLARE_BASE(s_test_base1);

enum {
    TEST_EVENT_BASE1_EV1,
    TEST_EVENT_BASE1_MAX
};

static BaseType_t get_other_core(void)
{
    return (xPortGetCoreID() + 1) % CONFIG_FREERTOS_NUMBER_OF_CORES;
}

static esp_event_loop_args_t test_event_get_default_loop_args(void)
{
    esp_event_loop_args_t loop_config = {
        .queue_size = CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE,
        .task_name = "loop",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 2048,
        .task_core_id = get_other_core()
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
        TEST_ESP_OK(esp_event_post_to(data->loop, data->base, data->id, NULL, 0, portMAX_DELAY));
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

    for (int i = 0; i < data->num; i++) {
        if (data->is_registration) {
            TEST_ESP_OK(esp_event_handler_register_with(data->loop, data->base, data->id, data->handles[i], NULL));
        } else {
            TEST_ESP_OK(esp_event_handler_unregister_with(data->loop, data->base, data->id, data->handles[i]));
        }
        vTaskDelay(1);
    }

    xSemaphoreGive(arg->done);

    vTaskDelete(NULL);
}

// Ignore this test on QEMU for now since it relies on esp_timer which is based on the host run time on ESP32-QEMU
TEST_CASE("can exit running loop at approximately the set amount of time", "[event][qemu-ignore]")
{
    /* this test aims to verify that running loop does not block indefinitely in cases where
     * events are posted frequently */

    esp_event_loop_handle_t loop;
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    loop_args.task_name = NULL;
    TEST_ESP_OK(esp_event_loop_create(&loop_args, &loop));

    performance_data_t handler_data = {
        .performed = 0,
        .expected = INT32_MAX,
        .done = xSemaphoreCreateBinary()
    };

    TEST_ESP_OK(esp_event_handler_register_with(loop, s_test_base1, TEST_EVENT_BASE1_EV1, test_event_performance_handler, &handler_data));

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
    xTaskCreatePinnedToCore(test_event_post_task, "post", 2048, &post_event_arg, uxTaskPriorityGet(NULL), &post_task, get_other_core());

    int runtime_ms = 10;
    int runtime_us = runtime_ms * 1000;

    int64_t start, diff;
    start = esp_timer_get_time();

    xSemaphoreGive(post_event_arg.start);

    // Run the loop for the runtime_ms set amount of time, regardless of whether events
    // are still being posted to the loop.
    TEST_ESP_OK(esp_event_loop_run(loop, pdMS_TO_TICKS(runtime_ms)));

    diff = (esp_timer_get_time() - start);

    // Threshold is 25 percent.
    TEST_ASSERT_LESS_THAN_INT(runtime_us * 1.25f, diff);

    // Verify that the post task still continues
    TEST_ASSERT_NOT_EQUAL(pdTRUE, xSemaphoreTake(post_event_arg.done, pdMS_TO_TICKS(10)));

    vSemaphoreDelete(post_event_arg.done);
    vSemaphoreDelete(post_event_arg.start);
    vSemaphoreDelete(handler_data.done);
    vTaskDelete(post_task);

    TEST_ESP_OK(esp_event_loop_delete(loop));

    vTaskDelay(pdMS_TO_TICKS(TEST_CONFIG_TEARDOWN_WAIT));
}

TEST_CASE("can register/unregister handlers simultaneously", "[event]")
{
    /* this test aims to verify that the event handlers list remains consistent despite
     * simultaneous access by differenct tasks */

    const char* base = "base";
    int32_t id = 0;

    esp_event_loop_handle_t loop;
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    TEST_ESP_OK(esp_event_loop_create(&loop_args, &loop));

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
            registration_data[i].handles[j] = (void*)(i * TEST_CONFIG_ITEMS_TO_REGISTER) + (j + TEST_CONFIG_ITEMS_TO_REGISTER);
        }

        registration_arg[i].start = xSemaphoreCreateBinary();
        registration_arg[i].done = xSemaphoreCreateBinary();
        registration_arg[i].data = &registration_data[i];

        xTaskCreatePinnedToCore(test_event_simple_handler_registration_task, "register", 2048, &registration_arg[i], uxTaskPriorityGet(NULL), NULL, i % CONFIG_FREERTOS_NUMBER_OF_CORES);
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

        xTaskCreatePinnedToCore(test_event_simple_handler_registration_task, "unregister", 2048, &unregistration_arg[i], uxTaskPriorityGet(NULL), NULL, i % CONFIG_FREERTOS_NUMBER_OF_CORES);
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

    TEST_ESP_OK(esp_event_loop_delete(loop));

    vTaskDelay(pdMS_TO_TICKS(TEST_CONFIG_TEARDOWN_WAIT));
}

TEST_CASE("can post and run events simultaneously", "[event]")
{
    /* this test aims to verify that:
     *  - multiple tasks can post to the queue simultaneously
     *  - handlers recieve the appropriate handler arg and associated event data */

    esp_event_loop_handle_t loop;

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    loop_args.task_name = NULL;
    loop_args.queue_size = TEST_CONFIG_TASKS_TO_SPAWN * TEST_CONFIG_ITEMS_TO_REGISTER;
    TEST_ESP_OK(esp_event_loop_create(&loop_args, &loop));

    int count = 0;

    simple_arg_t arg = {
        .data = &count,
        .mutex = xSemaphoreCreateMutex()
    };

    TEST_ESP_OK(esp_event_handler_register_with(loop, s_test_base1, TEST_EVENT_BASE1_EV1, test_event_simple_handler, &arg));

    post_event_data_t* post_event_data = calloc(TEST_CONFIG_TASKS_TO_SPAWN, sizeof(*post_event_data));
    task_arg_t* post_event_arg = calloc(TEST_CONFIG_TASKS_TO_SPAWN, sizeof(*post_event_arg));

    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        post_event_data[i].base = s_test_base1;
        post_event_data[i].id = TEST_EVENT_BASE1_EV1;
        post_event_data[i].loop = loop;
        post_event_data[i].num = TEST_CONFIG_ITEMS_TO_REGISTER;

        post_event_arg[i].data = &post_event_data[i];
        post_event_arg[i].start = xSemaphoreCreateBinary();
        post_event_arg[i].done = xSemaphoreCreateBinary();

        xTaskCreatePinnedToCore(test_event_post_task, "post", 2048, &post_event_arg[i], uxTaskPriorityGet(NULL), NULL, i % CONFIG_FREERTOS_NUMBER_OF_CORES);
    }

    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        xSemaphoreGive(post_event_arg[i].start);
    }

    // Execute some events as they are posted
    for (int i = 0; i < (TEST_CONFIG_TASKS_TO_SPAWN * TEST_CONFIG_ITEMS_TO_REGISTER) / 2; i++) {
        TEST_ESP_OK(esp_event_loop_run(loop, pdMS_TO_TICKS(10)));
    }

    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        xSemaphoreTake(post_event_arg[i].done, portMAX_DELAY);
    }

    // Execute the rest
    for (size_t i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN * TEST_CONFIG_ITEMS_TO_REGISTER; i++) {
        TEST_ESP_OK(esp_event_loop_run(loop, ZERO_DELAY));
    }

    TEST_ASSERT_EQUAL(TEST_CONFIG_TASKS_TO_SPAWN * TEST_CONFIG_ITEMS_TO_REGISTER, count);

    // Cleanup
    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        vSemaphoreDelete(post_event_arg[i].start);
        vSemaphoreDelete(post_event_arg[i].done);
    }

    free(post_event_data);
    free(post_event_arg);

    TEST_ESP_OK(esp_event_loop_delete(loop));

    vSemaphoreDelete(arg.mutex);

    vTaskDelay(pdMS_TO_TICKS(TEST_CONFIG_TEARDOWN_WAIT));
}

TEST_CASE("can post and run events simultaneously with instances", "[event]")
{
    /* this test aims to verify that:
     *  - multiple tasks can post to the queue simultaneously
     *  - handlers recieve the appropriate handler arg and associated event data */

    esp_event_loop_handle_t loop;

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    loop_args.task_name = NULL;
    loop_args.queue_size = TEST_CONFIG_TASKS_TO_SPAWN * TEST_CONFIG_ITEMS_TO_REGISTER;
    TEST_ESP_OK(esp_event_loop_create(&loop_args, &loop));

    int count = 0;

    simple_arg_t arg = {
        .data = &count,
        .mutex = xSemaphoreCreateMutex()
    };

    esp_event_handler_instance_t ctx;

    TEST_ESP_OK(esp_event_handler_instance_register_with(loop, s_test_base1, TEST_EVENT_BASE1_EV1, test_event_simple_handler, &arg, &ctx));

    post_event_data_t* post_event_data = calloc(TEST_CONFIG_TASKS_TO_SPAWN, sizeof(*post_event_data));
    task_arg_t* post_event_arg = calloc(TEST_CONFIG_TASKS_TO_SPAWN, sizeof(*post_event_arg));

    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        post_event_data[i].base = s_test_base1;
        post_event_data[i].id = TEST_EVENT_BASE1_EV1;
        post_event_data[i].loop = loop;
        post_event_data[i].num = TEST_CONFIG_ITEMS_TO_REGISTER;

        post_event_arg[i].data = &post_event_data[i];
        post_event_arg[i].start = xSemaphoreCreateBinary();
        post_event_arg[i].done = xSemaphoreCreateBinary();

        xTaskCreatePinnedToCore(test_event_post_task, "post", 2048, &post_event_arg[i], uxTaskPriorityGet(NULL), NULL, i % CONFIG_FREERTOS_NUMBER_OF_CORES);
    }

    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        xSemaphoreGive(post_event_arg[i].start);
    }

    // Execute some events as they are posted
    for (int i = 0; i < (TEST_CONFIG_TASKS_TO_SPAWN * TEST_CONFIG_ITEMS_TO_REGISTER) / 2; i++) {
        TEST_ESP_OK(esp_event_loop_run(loop, pdMS_TO_TICKS(10)));
    }

    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        xSemaphoreTake(post_event_arg[i].done, portMAX_DELAY);
    }

    // Execute the rest, we use the maximum number of events because we don't know
    // if any events have been dispatched before
    for (size_t i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN * TEST_CONFIG_ITEMS_TO_REGISTER; i++) {
        TEST_ESP_OK(esp_event_loop_run(loop, ZERO_DELAY));
    }

    TEST_ASSERT_EQUAL(TEST_CONFIG_TASKS_TO_SPAWN * TEST_CONFIG_ITEMS_TO_REGISTER, count);

    // Cleanup
    for (int i = 0; i < TEST_CONFIG_TASKS_TO_SPAWN; i++) {
        vSemaphoreDelete(post_event_arg[i].start);
        vSemaphoreDelete(post_event_arg[i].done);
    }

    free(post_event_data);
    free(post_event_arg);

    TEST_ESP_OK(esp_event_loop_delete(loop));

    vSemaphoreDelete(arg.mutex);

    vTaskDelay(pdMS_TO_TICKS(TEST_CONFIG_TEARDOWN_WAIT));
}

static void loop_run_task(void* args)
{
    esp_event_loop_handle_t event_loop = (esp_event_loop_handle_t) args;

    while (1) {
        esp_event_loop_run(event_loop, portMAX_DELAY);
    }
}

static void performance_test(bool dedicated_task)
{
    // rand() seems to do a one-time allocation. Call it here so that the memory it allocates
    // is not counted as a leak.
    unsigned int _rand __attribute__((unused)) = rand();

    const char test_base[] = "qwertyuiopasdfghjklzxvbnmmnbvcxz";

#define TEST_CONFIG_BASES  (sizeof(test_base) - 1)
#define TEST_CONFIG_IDS    (TEST_CONFIG_BASES / 2)

    // Create loop
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();
    esp_event_loop_handle_t loop;

    if (!dedicated_task) {
        loop_args.task_name = NULL;
    }

    TEST_ESP_OK(esp_event_loop_create(&loop_args, &loop));

    performance_data_t data;

    // Register the handlers
    for (int base = 0; base < TEST_CONFIG_BASES; base++) {
        for (int id = 0; id < TEST_CONFIG_IDS; id++) {
            TEST_ESP_OK(esp_event_handler_register_with(loop, test_base + base, id, test_event_performance_handler, &data));
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
                post_bases[rand_a] = post_bases[rand_b];
                post_bases[rand_b] = temp;
            }

            for (int i = 0; i < ids; i++) {
                int rand_a  = rand() % ids;
                int rand_b  = rand() % ids;

                int temp = post_ids[rand_a];
                post_ids[rand_a] = post_ids[rand_b];
                post_ids[rand_b] = temp;
            }

            // Post the events
            int64_t start = esp_timer_get_time();
            for (int base = 0; base < bases; base++) {
                for (int id = 0; id < ids; id++) {
                    TEST_ESP_OK(esp_event_post_to(loop, test_base + post_bases[base], post_ids[id], NULL, 0, portMAX_DELAY));
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

    int average = (int)(running_sum / (running_count));

    if (!dedicated_task) {
        ((esp_event_loop_instance_t*) loop)->task = mtask;
    }

    TEST_ESP_OK(esp_event_loop_delete(loop));

    vTaskDelay(pdMS_TO_TICKS(TEST_CONFIG_TEARDOWN_WAIT));

#ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
    ESP_LOGI(TAG, "events dispatched/second with profiling enabled: %d", average);
    // Enabling profiling will slow down event dispatch, so the set threshold
    // is not valid when it is enabled.
#else
#ifndef CONFIG_SPIRAM
    TEST_PERFORMANCE_GREATER_THAN(EVENT_DISPATCH, "%d", average);
#else
    TEST_PERFORMANCE_GREATER_THAN(EVENT_DISPATCH_PSRAM, "%d", average);
#endif // CONFIG_SPIRAM
#endif // CONFIG_ESP_EVENT_LOOP_PROFILING
}

TEST_CASE("performance test - dedicated task", "[event][qemu-ignore]")
{
    performance_test(true);
}

TEST_CASE("performance test - no dedicated task", "[event][qemu-ignore]")
{
    performance_test(false);
}

#if CONFIG_ESP_EVENT_POST_FROM_ISR
TEST_CASE("data posted normally is correctly set internally", "[event][intr]")
{
    esp_event_loop_handle_t loop;
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    loop_args.task_name = NULL;
    TEST_ESP_OK(esp_event_loop_create(&loop_args, &loop));

    esp_event_post_instance_t post;
    esp_event_loop_instance_t* loop_def = (esp_event_loop_instance_t*) loop;

    TEST_ESP_OK(esp_event_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(loop_def->queue, &post, portMAX_DELAY));
    TEST_ASSERT_EQUAL(false, post.data_set);
    TEST_ASSERT_EQUAL(false, post.data_allocated);
    TEST_ASSERT_EQUAL(NULL, post.data.ptr);

    TEST_ESP_OK(esp_event_loop_delete(loop));

    vTaskDelay(pdMS_TO_TICKS(TEST_CONFIG_TEARDOWN_WAIT));
}

TEST_CASE("data posted from ISR is correctly set internally", "[event][intr]")
{
    esp_event_loop_handle_t loop;
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    loop_args.task_name = NULL;
    TEST_ESP_OK(esp_event_loop_create(&loop_args, &loop));

    esp_event_post_instance_t post;
    esp_event_loop_instance_t* loop_def = (esp_event_loop_instance_t*) loop;
    int sample = 0;
    TEST_ESP_OK(esp_event_isr_post_to(loop, s_test_base1, TEST_EVENT_BASE1_EV1, &sample, sizeof(sample), NULL));
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(loop_def->queue, &post, portMAX_DELAY));
    TEST_ASSERT_EQUAL(true, post.data_set);
    TEST_ASSERT_EQUAL(false, post.data_allocated);
    TEST_ASSERT_EQUAL(false, post.data.val);

    TEST_ESP_OK(esp_event_loop_delete(loop));

    vTaskDelay(pdMS_TO_TICKS(TEST_CONFIG_TEARDOWN_WAIT));
}

static void test_handler_post_from_isr(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    SemaphoreHandle_t *sem = (SemaphoreHandle_t*) event_handler_arg;
    // Event data is just the address value (maybe have been truncated due to casting).
    int *data = (int*) event_data;
    TEST_ASSERT_EQUAL(*data, (int)(*sem));
    xSemaphoreGive(*sem);
}

bool test_event_on_timer_alarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    int data = (int)user_ctx;
    gptimer_stop(timer);
    // Posting events with data more than 4 bytes should fail.
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_event_isr_post(s_test_base1, TEST_EVENT_BASE1_EV1, &data, 5, NULL));
    // This should succeedd, as data is int-sized. The handler for the event checks that the passed event data
    // is correct.
    BaseType_t task_unblocked;
    TEST_ESP_OK(esp_event_isr_post(s_test_base1, TEST_EVENT_BASE1_EV1, &data, sizeof(data), &task_unblocked));
    return task_unblocked == pdTRUE;
}

TEST_CASE("can post events from interrupt handler", "[event][intr]")
{
    /* Lazy allocated resources in gptimer/intr_alloc */
    unity_utils_set_leak_level(160);

    TEST_ESP_OK(esp_event_loop_create_default());

    SemaphoreHandle_t sem = xSemaphoreCreateBinary();
    gptimer_handle_t gptimer = NULL;
    /* Select and initialize basic parameters of the timer */
    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
    };
    TEST_ESP_OK(gptimer_new_timer(&config, &gptimer));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 500000,
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = test_event_on_timer_alarm
    };
    TEST_ESP_OK(gptimer_register_event_callbacks(gptimer, &cbs, sem));
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));
    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_start(gptimer));

    TEST_ESP_OK(esp_event_handler_register(s_test_base1, TEST_EVENT_BASE1_EV1,
                                           test_handler_post_from_isr, &sem));

    xSemaphoreTake(sem, portMAX_DELAY);

    vTaskDelay(pdMS_TO_TICKS(TEST_CONFIG_TEARDOWN_WAIT));

    vSemaphoreDelete(sem);
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));

    TEST_ESP_OK(esp_event_loop_delete_default());

    vTaskDelay(2);
}

#endif // CONFIG_ESP_EVENT_POST_FROM_ISR
