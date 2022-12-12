/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdbool.h>
#include <string.h>

#include "esp_event.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "esp_event.h"
#include "esp_event_private.h"
#include "esp_event_internal.h"

#include "esp_heap_caps.h"

#include "sdkconfig.h"
#include "unity.h"

typedef struct {
    void* data;
    SemaphoreHandle_t mutex;
} simple_arg_t;

static const char* TAG = "test_event";

ESP_EVENT_DECLARE_BASE(s_default_test_base1);
ESP_EVENT_DECLARE_BASE(s_default_test_base2);

ESP_EVENT_DEFINE_BASE(s_default_test_base1);
ESP_EVENT_DEFINE_BASE(s_default_test_base2);

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

// The initial logging "initializing test" is to ensure mutex allocation is not counted against memory not being freed
// during teardown.
#define TEST_SETUP() \
        ESP_LOGI(TAG, "initializing test");

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

TEST_CASE("default loop: can create and delete loop", "[event]")
{
    TEST_SETUP();

    TEST_ESP_OK(esp_event_loop_create_default());

    TEST_ESP_OK(esp_event_loop_delete_default());
}

TEST_CASE("default loop: registering fails on uninitialized default loop", "[event]")
{
    TEST_SETUP();

    esp_event_handler_instance_t instance;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_event_handler_instance_register(s_default_test_base1,
            TEST_EVENT_BASE1_EV1,
            test_event_simple_handler,
            NULL,
            &instance));
}

TEST_CASE("default loop: registering/unregistering event works", "[event]")
{
    TEST_SETUP();

    int count = 0;

    simple_arg_t arg = {
        .data = &count,
        .mutex = xSemaphoreCreateMutex()
    };

    TEST_ESP_OK(esp_event_loop_create_default());

    esp_event_handler_instance_t instance;
    TEST_ESP_OK(esp_event_handler_instance_register(s_default_test_base1,
            TEST_EVENT_BASE1_EV1,
            test_event_simple_handler,
            &arg,
            &instance));
    TEST_ASSERT(instance);
    TEST_ESP_OK(esp_event_post(s_default_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));

    vTaskDelay(10);

    TEST_ASSERT_EQUAL(1, count);

    TEST_ESP_OK(esp_event_handler_instance_unregister(s_default_test_base1,
            TEST_EVENT_BASE1_EV1,
            &instance));

    vTaskDelay(10);

    TEST_ASSERT_EQUAL(1, count);

    TEST_ESP_OK(esp_event_loop_delete_default());

    vSemaphoreDelete(arg.mutex);
}

TEST_CASE("default event loop: registering event handler instance without instance context works", "[event]") {
    TEST_SETUP();

    int count_1 = 0;

    simple_arg_t arg_1 = {
        .data = &count_1,
        .mutex = xSemaphoreCreateMutex()
    };

    TEST_ESP_OK(esp_event_loop_create_default());

    TEST_ESP_OK(esp_event_handler_instance_register(ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, test_event_simple_handler, &arg_1, NULL));

    TEST_ESP_OK(esp_event_post(s_default_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));

    vTaskDelay(10);

    TEST_ASSERT_EQUAL(1, count_1);

    TEST_ESP_OK(esp_event_loop_delete_default());

    vSemaphoreDelete(arg_1.mutex);
}
