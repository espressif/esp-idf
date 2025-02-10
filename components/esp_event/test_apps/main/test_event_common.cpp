/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/* This file contains all tests runnable on targets as well as on the host */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "unity.h"

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
    TEST_EVENT_BASE2_MAX
};

static const TickType_t ZERO_DELAY = 0;

static esp_event_loop_args_t test_event_get_default_loop_args(void)
{
    esp_event_loop_args_t loop_config = {
        .queue_size = CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE,
        .task_name = "loop",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 2048,
        .task_core_id = 0 // Linux simulator is single core only
    };

    return loop_config;
}

struct EV_LoopFix {
public:
    EV_LoopFix(size_t queue_size = CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE, const char *task_name = NULL)
    {
        esp_event_loop_args_t loop_args = test_event_get_default_loop_args();
        loop_args.task_name = task_name;
        loop_args.queue_size = queue_size;
        TEST_ESP_OK(esp_event_loop_create(&loop_args, &loop));
    }

    ~EV_LoopFix()
    {
        esp_event_loop_delete(loop);
    }

    esp_event_loop_handle_t loop;
};

TEST_CASE("can create and delete event loop without task", "[event][linux]")
{
    esp_event_loop_handle_t loop;
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();
    loop_args.task_name = NULL;

    TEST_ESP_OK(esp_event_loop_create(&loop_args, &loop));
    TEST_ESP_OK(esp_event_loop_delete(loop));
}

TEST_CASE("can create and delete event loop with task", "[event][linux]")
{
    esp_event_loop_handle_t loop;
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    TEST_ESP_OK(esp_event_loop_create(&loop_args, &loop));
    TEST_ESP_OK(esp_event_loop_delete(loop));
}

TEST_CASE("can post events up to loop's max queue size", "[event][linux]")
{
    EV_LoopFix loop_fix(2);

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                NULL,
                                                0,
                                                pdMS_TO_TICKS(10)));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post_to(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                NULL,
                                                0,
                                                pdMS_TO_TICKS(10)));
}

TEST_CASE("posting to full event loop times out", "[event][linux]")
{
    EV_LoopFix loop_fix(1);

    TEST_ASSERT_EQUAL(ESP_OK,
                      esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, pdMS_TO_TICKS(0)));
    TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT,
                      esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, pdMS_TO_TICKS(0)));
    TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT,
                      esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, pdMS_TO_TICKS(10)));
}

TEST_CASE("can post to loop and run loop without registration", "[event][linux]")
{
    EV_LoopFix loop_fix;

    TEST_ASSERT_EQUAL(ESP_OK,
                      esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, pdMS_TO_TICKS(10)));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));
}

static void test_handler_inc(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    int *target = (int*) event_handler_arg;
    (*target)++;
}

TEST_CASE("registering event handler instance without instance context works", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int count = 0;

    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         s_test_base1,
                                                         TEST_EVENT_BASE1_EV1,
                                                         test_handler_inc,
                                                         &count,
                                                         NULL));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(1, count);
}

/* Register the handler twice to the same base and id but with a different argument
 * (expects to return ESP_OK and log a warning)
 * This aims to verify: 1) Handler's argument to be updated
 *                      2) Registration not to leak memory
 */
TEST_CASE("registering event twice with same handler yields updated handler arg", "[event][linux]")
{
    EV_LoopFix loop_fix;

    int count_first = 0;
    int count_second = 0;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                ESP_EVENT_ANY_BASE,
                                                ESP_EVENT_ANY_ID,
                                                test_handler_inc,
                                                &count_first));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(count_first, 1);

    // overriding the former registration of the same event
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                ESP_EVENT_ANY_BASE,
                                                ESP_EVENT_ANY_ID,
                                                test_handler_inc,
                                                &count_second));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(1, count_first);
    TEST_ASSERT_EQUAL(1, count_second);
}

TEST_CASE("registering event handler instance twice works", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int count_1 = 0;
    int count_2 = 0;
    esp_event_handler_instance_t ctx_1;
    esp_event_handler_instance_t ctx_2;

    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         ESP_EVENT_ANY_BASE,
                                                         ESP_EVENT_ANY_ID,
                                                         test_handler_inc,
                                                         &count_1,
                                                         &ctx_1));
    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         ESP_EVENT_ANY_BASE,
                                                         ESP_EVENT_ANY_ID,
                                                         test_handler_inc,
                                                         &count_2,
                                                         &ctx_2));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, pdMS_TO_TICKS(10)));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(1, count_1);
    TEST_ASSERT_EQUAL(1, count_2);
}

TEST_CASE("registering with ANY_BASE but specific ID fails", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int count = 0;
    int count_instance = 0;
    esp_event_handler_instance_t ctx;

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_event_handler_register_with(loop_fix.loop,
                                                                           ESP_EVENT_ANY_BASE,
                                                                           TEST_EVENT_BASE1_EV1,
                                                                           test_handler_inc,
                                                                           &count));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_event_handler_instance_register_with(loop_fix.loop,
                                                                                    ESP_EVENT_ANY_BASE,
                                                                                    TEST_EVENT_BASE1_EV1,
                                                                                    test_handler_inc,
                                                                                    &count_instance,
                                                                                    &ctx));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(0, count);
    TEST_ASSERT_EQUAL(0, count_instance);
}

TEST_CASE("registration of ANY_BASE and ANY_ID always called", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int count = 0;
    int count_instance = 0;
    esp_event_handler_instance_t ctx;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                ESP_EVENT_ANY_BASE,
                                                ESP_EVENT_ANY_ID,
                                                test_handler_inc,
                                                &count));
    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         ESP_EVENT_ANY_BASE,
                                                         ESP_EVENT_ANY_ID,
                                                         test_handler_inc,
                                                         &count_instance,
                                                         &ctx));

    // handlers should always be triggered
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base2, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base2, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    for (size_t i = 0; i < 4; i++) {
        TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));
    }

    TEST_ASSERT_EQUAL(4, count);
    TEST_ASSERT_EQUAL(4, count_instance);
}

TEST_CASE("registration of ANY_ID not called from wrong base", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int count = 0;
    int count_instance = 0;
    esp_event_handler_instance_t ctx;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                ESP_EVENT_ANY_ID,
                                                test_handler_inc,
                                                &count));
    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         s_test_base1,
                                                         ESP_EVENT_ANY_ID,
                                                         test_handler_inc,
                                                         &count_instance,
                                                         &ctx));

    // handlers shouldn't be triggered with different base
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base2, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base2, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(0, count);
    TEST_ASSERT_EQUAL(0, count_instance);
}

TEST_CASE("registration of ANY_ID called from correct base", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int count = 0;
    int count_instance = 0;
    esp_event_handler_instance_t ctx;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                ESP_EVENT_ANY_ID,
                                                test_handler_inc,
                                                &count));
    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         s_test_base1,
                                                         ESP_EVENT_ANY_ID,
                                                         test_handler_inc,
                                                         &count_instance,
                                                         &ctx));

    // for all events with correct base, it should be triggered
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(2, count);
    TEST_ASSERT_EQUAL(2, count_instance);
}

TEST_CASE("registering specific event posting different base and different event ID", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int count = 0;
    int count_instance = 0;
    esp_event_handler_instance_t ctx;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_inc,
                                                &count));
    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         s_test_base1,
                                                         TEST_EVENT_BASE1_EV1,
                                                         test_handler_inc,
                                                         &count_instance,
                                                         &ctx));

    // handlers should not be triggered with different base and different ID
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base2, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(0, count);
    TEST_ASSERT_EQUAL(0, count_instance);
}

TEST_CASE("registering specific event posting different base", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int count = 0;
    int count_instance = 0;
    esp_event_handler_instance_t ctx;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_inc,
                                                &count));
    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         s_test_base1,
                                                         TEST_EVENT_BASE1_EV1,
                                                         test_handler_inc,
                                                         &count_instance,
                                                         &ctx));

    // handlers should not be triggered with different base
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base2, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base2, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(0, count);
    TEST_ASSERT_EQUAL(0, count_instance);
}

TEST_CASE("registering specific event posting incorrect event ID", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int count = 0;
    int count_instance = 0;
    esp_event_handler_instance_t ctx;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_inc,
                                                &count));
    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         s_test_base1,
                                                         TEST_EVENT_BASE1_EV1,
                                                         test_handler_inc,
                                                         &count_instance,
                                                         &ctx));

    // for incorrect id, it should not be triggered with different ID
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(0, count);
    TEST_ASSERT_EQUAL(0, count_instance);
}

TEST_CASE("registering specific event posting correct event and base", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int count = 0;
    int count_instance = 0;
    esp_event_handler_instance_t ctx;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_inc,
                                                &count));
    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         s_test_base1,
                                                         TEST_EVENT_BASE1_EV1,
                                                         test_handler_inc,
                                                         &count_instance,
                                                         &ctx));

    // for correct event and base, it should be triggered
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(1, count);
    TEST_ASSERT_EQUAL(1, count_instance);
}

TEST_CASE("posting ANY_EVENT or ANY_ID fails", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int count = 0;
    int count_instance = 0;
    esp_event_handler_instance_t ctx;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                ESP_EVENT_ANY_BASE,
                                                ESP_EVENT_ANY_ID,
                                                test_handler_inc,
                                                &count));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                ESP_EVENT_ANY_ID,
                                                test_handler_inc,
                                                &count));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_inc,
                                                &count));

    // normal and "instance" registration are decently close to each other, don't exercise all cases here
    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         ESP_EVENT_ANY_BASE,
                                                         ESP_EVENT_ANY_ID,
                                                         test_handler_inc,
                                                         &count_instance,
                                                         &ctx));

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      esp_event_post_to(loop_fix.loop, ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, NULL, 0, portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      esp_event_post_to(loop_fix.loop, s_test_base1, ESP_EVENT_ANY_ID, NULL, 0, portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      esp_event_post_to(loop_fix.loop, ESP_EVENT_ANY_BASE, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(0, count);
    TEST_ASSERT_EQUAL(0, count_instance);
}

TEST_CASE("posting to loop with multiple registrations at every level", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int count_loop = 0;
    int count_loop_inst = 0;
    int count_base = 0;
    int count_base_inst = 0;
    int count_id = 0; // without instance, no more than one registration for a specific event ID
    int count_id_inst_1 = 0;
    int count_id_inst_2 = 0;
    esp_event_handler_instance_t loop_ctx;
    esp_event_handler_instance_t base_ctx;
    esp_event_handler_instance_t id_ctx_1;
    esp_event_handler_instance_t id_ctx_2;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                ESP_EVENT_ANY_BASE,
                                                ESP_EVENT_ANY_ID,
                                                test_handler_inc,
                                                &count_loop));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                ESP_EVENT_ANY_ID,
                                                test_handler_inc,
                                                &count_base));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_inc,
                                                &count_id));

    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         ESP_EVENT_ANY_BASE,
                                                         ESP_EVENT_ANY_ID,
                                                         test_handler_inc,
                                                         &count_loop_inst,
                                                         &loop_ctx));
    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         s_test_base1,
                                                         ESP_EVENT_ANY_ID,
                                                         test_handler_inc,
                                                         &count_base_inst,
                                                         &base_ctx));
    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         ESP_EVENT_ANY_BASE,
                                                         ESP_EVENT_ANY_ID,
                                                         test_handler_inc,
                                                         &count_id_inst_1,
                                                         &id_ctx_1));
    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         ESP_EVENT_ANY_BASE,
                                                         ESP_EVENT_ANY_ID,
                                                         test_handler_inc,
                                                         &count_id_inst_2,
                                                         &id_ctx_2));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(count_loop, 1);
    TEST_ASSERT_EQUAL(count_loop_inst, 1);
    TEST_ASSERT_EQUAL(count_base, 1);
    TEST_ASSERT_EQUAL(count_base_inst, 1);
    TEST_ASSERT_EQUAL(count_id, 1);
    TEST_ASSERT_EQUAL(count_id_inst_1, 1);
    TEST_ASSERT_EQUAL(count_id_inst_2, 1);
}

TEST_CASE("unregistering ANY_BASE and specific ID fails", "[event][linux]")
{
    EV_LoopFix loop_fix;
    esp_event_handler_instance_t handler_inst = (esp_event_handler_instance_t) 1; // avoid triggering NULL check

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_event_handler_unregister_with(loop_fix.loop,
                                                                             ESP_EVENT_ANY_BASE,
                                                                             TEST_EVENT_BASE1_EV1,
                                                                             test_handler_inc));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_event_handler_instance_unregister_with(loop_fix.loop,
                                                                                      ESP_EVENT_ANY_BASE,
                                                                                      TEST_EVENT_BASE1_EV1,
                                                                                      handler_inst));
}

TEST_CASE("unregistering NULL instance fails", "[event][linux]")
{
    EV_LoopFix loop_fix;
    esp_event_handler_instance_t handler_inst = NULL;

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_event_handler_instance_unregister_with(loop_fix.loop,
                                                                                      ESP_EVENT_ANY_BASE,
                                                                                      ESP_EVENT_ANY_ID,
                                                                                      handler_inst));
}

TEST_CASE("unregistered handler is not executed", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int loop_count = 0;
    int base_count = 0;
    int id_count = 0;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                ESP_EVENT_ANY_BASE,
                                                ESP_EVENT_ANY_ID,
                                                test_handler_inc,
                                                &loop_count));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                ESP_EVENT_ANY_ID,
                                                test_handler_inc,
                                                &base_count));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_inc,
                                                &id_count));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(loop_count, 1);
    TEST_ASSERT_EQUAL(base_count, 1);
    TEST_ASSERT_EQUAL(id_count, 1);

    TEST_ESP_OK(esp_event_handler_unregister_with(loop_fix.loop,
                                                  ESP_EVENT_ANY_BASE,
                                                  ESP_EVENT_ANY_ID,
                                                  test_handler_inc));
    TEST_ESP_OK(esp_event_handler_unregister_with(loop_fix.loop,
                                                  s_test_base1,
                                                  ESP_EVENT_ANY_ID,
                                                  test_handler_inc));
    TEST_ESP_OK(esp_event_handler_unregister_with(loop_fix.loop,
                                                  s_test_base1,
                                                  TEST_EVENT_BASE1_EV1,
                                                  test_handler_inc));
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(loop_count, 1);
    TEST_ASSERT_EQUAL(base_count, 1);
    TEST_ASSERT_EQUAL(id_count, 1);
}

TEST_CASE("unregistered handler instance is not executed", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int loop_count = 0;
    int base_count = 0;
    int id_count = 0;
    esp_event_handler_instance_t loop_handler_inst;
    esp_event_handler_instance_t base_handler_inst;
    esp_event_handler_instance_t id_handler_inst;

    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         ESP_EVENT_ANY_BASE,
                                                         ESP_EVENT_ANY_ID,
                                                         test_handler_inc,
                                                         &loop_count,
                                                         &loop_handler_inst));
    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         s_test_base1,
                                                         ESP_EVENT_ANY_ID,
                                                         test_handler_inc,
                                                         &base_count,
                                                         &base_handler_inst));
    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         s_test_base1,
                                                         TEST_EVENT_BASE1_EV1,
                                                         test_handler_inc,
                                                         &id_count,
                                                         &id_handler_inst));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(loop_count, 1);
    TEST_ASSERT_EQUAL(base_count, 1);
    TEST_ASSERT_EQUAL(id_count, 1);

    TEST_ESP_OK(esp_event_handler_instance_unregister_with(loop_fix.loop,
                                                           ESP_EVENT_ANY_BASE,
                                                           ESP_EVENT_ANY_ID,
                                                           loop_handler_inst));
    TEST_ESP_OK(esp_event_handler_instance_unregister_with(loop_fix.loop,
                                                           s_test_base1,
                                                           ESP_EVENT_ANY_ID,
                                                           base_handler_inst));
    TEST_ESP_OK(esp_event_handler_instance_unregister_with(loop_fix.loop,
                                                           s_test_base1,
                                                           TEST_EVENT_BASE1_EV1,
                                                           id_handler_inst));
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(loop_count, 1);
    TEST_ASSERT_EQUAL(base_count, 1);
    TEST_ASSERT_EQUAL(id_count, 1);
}

TEST_CASE("unregistering handler does not influence other handlers", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int unregister_count = 0;
    int different_id_count = 0;
    int different_base_count = 0;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_inc,
                                                &unregister_count));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base2,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_inc,
                                                &different_id_count));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV2,
                                                test_handler_inc,
                                                &different_base_count));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base2, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    for (size_t i = 0; i < 3; i++) {
        TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));
    }

    TEST_ASSERT_EQUAL(unregister_count, 1);
    TEST_ASSERT_EQUAL(different_id_count, 1);
    TEST_ASSERT_EQUAL(different_base_count, 1);

    TEST_ESP_OK(esp_event_handler_unregister_with(loop_fix.loop,
                                                  s_test_base1,
                                                  TEST_EVENT_BASE1_EV1,
                                                  test_handler_inc));
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base2, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    for (size_t i = 0; i < 3; i++) {
        TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));
    }

    TEST_ASSERT_EQUAL(unregister_count, 1);
    TEST_ASSERT_EQUAL(different_id_count, 2);
    TEST_ASSERT_EQUAL(different_base_count, 2);
}

TEST_CASE("unregistering ESP_EVENT_ANY_ID does not affect other handlers with same base", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int any_id_count = 0;
    int specific_id_count = 0;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                ESP_EVENT_ANY_ID,
                                                test_handler_inc,
                                                &any_id_count));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_inc,
                                                &specific_id_count));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(any_id_count, 1);
    TEST_ASSERT_EQUAL(specific_id_count, 1);

    TEST_ESP_OK(esp_event_handler_unregister_with(loop_fix.loop, s_test_base1, ESP_EVENT_ANY_ID, test_handler_inc));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(any_id_count, 1);
    TEST_ASSERT_EQUAL(specific_id_count, 2);
}

TEST_CASE("unregistering ESP_EVENT_ANY_BASE does not affect handlers with specific base", "[event][linux]")
{
    EV_LoopFix loop_fix;
    int any_base_count = 0;
    int any_id_count = 0;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                ESP_EVENT_ANY_BASE,
                                                ESP_EVENT_ANY_ID,
                                                test_handler_inc,
                                                &any_base_count));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                ESP_EVENT_ANY_ID,
                                                test_handler_inc,
                                                &any_id_count));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(any_base_count, 1);
    TEST_ASSERT_EQUAL(any_id_count, 1);

    TEST_ESP_OK(esp_event_handler_unregister_with(loop_fix.loop,
                                                  ESP_EVENT_ANY_BASE,
                                                  ESP_EVENT_ANY_ID,
                                                  test_handler_inc));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(any_base_count, 1);
    TEST_ASSERT_EQUAL(any_id_count, 2);
}

typedef struct {
    esp_event_handler_instance_t context;
    esp_event_loop_handle_t loop;
    int count;
} unregister_test_data_t;

static void test_handler_unregister_itself(void* event_handler_arg,
                                           esp_event_base_t event_base,
                                           int32_t event_id,
                                           void* event_data)
{
    unregister_test_data_t *test_data = (unregister_test_data_t*) event_handler_arg;

    (test_data->count)++;

    // Unregister this handler for this event
    TEST_ESP_OK(esp_event_handler_unregister_with(test_data->loop,
                                                  event_base,
                                                  event_id,
                                                  test_handler_unregister_itself));
}

TEST_CASE("handler can unregister itself", "[event][linux]")
{
    EV_LoopFix loop_fix;

    unregister_test_data_t test_data = {
        .context = NULL,
        .loop = loop_fix.loop,
        .count = 0,
    };

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_unregister_itself, &test_data));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(1, test_data.count);

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop,
                                  s_test_base1,
                                  TEST_EVENT_BASE1_EV1,
                                  &loop_fix.loop,
                                  sizeof(&loop_fix.loop),
                                  portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, pdMS_TO_TICKS(10)));

    TEST_ASSERT_EQUAL(1, test_data.count);
}

static void test_handler_instance_unregister_itself(void* event_handler_arg,
                                                    esp_event_base_t event_base,
                                                    int32_t event_id,
                                                    void* event_data)
{
    unregister_test_data_t *test_data = (unregister_test_data_t*) event_handler_arg;

    (test_data->count)++;

    // Unregister this handler for this event
    TEST_ESP_OK(esp_event_handler_instance_unregister_with(test_data->loop, event_base, event_id, test_data->context));
}

TEST_CASE("handler instance can unregister itself", "[event][linux]")
{
    EV_LoopFix loop_fix;

    unregister_test_data_t test_data = {
        .context = NULL,
        .loop = loop_fix.loop,
        .count = 0,
    };

    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         s_test_base1,
                                                         TEST_EVENT_BASE1_EV1,
                                                         test_handler_instance_unregister_itself,
                                                         &test_data,
                                                         &(test_data.context)));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(1, test_data.count);

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop,
                                  s_test_base1,
                                  TEST_EVENT_BASE1_EV1,
                                  &loop_fix.loop,
                                  sizeof(&loop_fix.loop),
                                  portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(1, test_data.count);
}

typedef struct {
    size_t counter;
    size_t test_data[4];
} ordered_dispatch_test_data_t;

static void test_event_ordered_dispatch(void* event_handler_arg,
                                        esp_event_base_t event_base,
                                        int32_t event_id,
                                        void* event_data,
                                        size_t handler_index)
{
    ordered_dispatch_test_data_t *test_data = (ordered_dispatch_test_data_t*) event_handler_arg;
    test_data->test_data[handler_index] = test_data->counter;
    (test_data->counter)++;
}

static void test_event_ordered_dispatch_0(void* event_handler_arg,
                                          esp_event_base_t event_base,
                                          int32_t event_id,
                                          void* event_data)
{
    test_event_ordered_dispatch(event_handler_arg, event_base, event_id, event_data, 0);
}

static void test_event_ordered_dispatch_1(void* event_handler_arg,
                                          esp_event_base_t event_base,
                                          int32_t event_id,
                                          void* event_data)
{
    test_event_ordered_dispatch(event_handler_arg, event_base, event_id, event_data, 1);
}

static void test_event_ordered_dispatch_2(void* event_handler_arg,
                                          esp_event_base_t event_base,
                                          int32_t event_id,
                                          void* event_data)
{
    test_event_ordered_dispatch(event_handler_arg, event_base, event_id, event_data, 2);
}

static void test_event_ordered_dispatch_3(void* event_handler_arg,
                                          esp_event_base_t event_base,
                                          int32_t event_id,
                                          void* event_data)
{
    test_event_ordered_dispatch(event_handler_arg, event_base, event_id, event_data, 3);
}

TEST_CASE("events handlers for specific ID are dispatched in the order they are registered", "[event][linux]")
{
    EV_LoopFix loop_fix;

    ordered_dispatch_test_data_t test_data = {
        .counter = 0,
        .test_data = {},
    };

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_event_ordered_dispatch_0,
                                                &test_data));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_event_ordered_dispatch_1,
                                                &test_data));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_event_ordered_dispatch_2,
                                                &test_data));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_event_ordered_dispatch_3,
                                                &test_data));

    esp_event_dump(stdout);

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    // Expected data executing the posts above
    size_t ref_arr[4];
    for (size_t i = 0; i < 4; i++) {
        ref_arr[i] = i;
    }

    TEST_ASSERT_EQUAL_INT_ARRAY(ref_arr, test_data.test_data, 4);
}

TEST_CASE("events handlers for specific base are dispatched in the order they are registered", "[event][linux]")
{
    EV_LoopFix loop_fix;

    ordered_dispatch_test_data_t test_data = {
        .counter = 0,
        .test_data = {},
    };

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                ESP_EVENT_ANY_ID,
                                                test_event_ordered_dispatch_0,
                                                &test_data));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                ESP_EVENT_ANY_ID,
                                                test_event_ordered_dispatch_1,
                                                &test_data));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                ESP_EVENT_ANY_ID,
                                                test_event_ordered_dispatch_2,
                                                &test_data));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                ESP_EVENT_ANY_ID,
                                                test_event_ordered_dispatch_3,
                                                &test_data));

    esp_event_dump(stdout);

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    // Expected data executing the posts above
    size_t ref_arr[4];
    for (size_t i = 0; i < 4; i++) {
        ref_arr[i] = i;
    }

    TEST_ASSERT_EQUAL_INT_ARRAY(ref_arr, test_data.test_data, 4);
}

TEST_CASE("events handlers for any base are dispatched in the order they are registered", "[event][linux]")
{
    EV_LoopFix loop_fix;

    ordered_dispatch_test_data_t test_data = {
        .counter = 0,
        .test_data = {},
    };

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                ESP_EVENT_ANY_BASE,
                                                ESP_EVENT_ANY_ID,
                                                test_event_ordered_dispatch_0,
                                                &test_data));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                ESP_EVENT_ANY_BASE,
                                                ESP_EVENT_ANY_ID,
                                                test_event_ordered_dispatch_1,
                                                &test_data));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                ESP_EVENT_ANY_BASE,
                                                ESP_EVENT_ANY_ID,
                                                test_event_ordered_dispatch_2,
                                                &test_data));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                ESP_EVENT_ANY_BASE,
                                                ESP_EVENT_ANY_ID,
                                                test_event_ordered_dispatch_3,
                                                &test_data));

    esp_event_dump(stdout);

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    // Expected data executing the posts above
    size_t ref_arr[4];
    for (size_t i = 0; i < 4; i++) {
        ref_arr[i] = i;
    }

    TEST_ASSERT_EQUAL_INT_ARRAY(ref_arr, test_data.test_data, 4);
}

static void test_create_loop_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    if (id == TEST_EVENT_BASE1_EV1) {
        TEST_ESP_OK(esp_event_loop_create(&loop_args, (esp_event_loop_handle_t*) handler_args));
    } else {
        TEST_ESP_OK(esp_event_loop_delete(*((esp_event_loop_handle_t*) handler_args)));
    }
}

TEST_CASE("can create and delete loop from handler", "[event][linux]")
{
    EV_LoopFix loop_fix;
    esp_event_loop_handle_t test_loop;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_create_loop_handler,
                                                &test_loop));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV2,
                                                test_create_loop_handler,
                                                &test_loop));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));
}

typedef struct {
    esp_event_loop_handle_t loop;
    int count;
} posting_handler_data_t;

static void test_handler_post(void* handler_arg, esp_event_base_t base, int32_t id, void* event_arg)
{
    posting_handler_data_t* data = (posting_handler_data_t*) handler_arg;
    TEST_ESP_OK(esp_event_post_to(data->loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    data->count++;
}

TEST_CASE("can post to loop from handler", "[event][linux]")
{
    EV_LoopFix loop_fix;

    posting_handler_data_t arg = {
        .loop = loop_fix.loop,
        .count = 0,
    };
    int secondary_handler_count = 0;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_post,
                                                &arg));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV2,
                                                test_handler_inc,
                                                &secondary_handler_count));

    // Test that a handler can post to a different loop while there is still slots on the queue
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(1, arg.count);
    TEST_ASSERT_EQUAL(1, secondary_handler_count);
}

TEST_CASE("can post to loop from handler instance", "[event][linux]")
{
    EV_LoopFix loop_fix;
    esp_event_handler_instance_t ctx;

    posting_handler_data_t arg = {
        .loop = loop_fix.loop,
        .count = 0,
    };
    int secondary_handler_count = 0;

    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         s_test_base1,
                                                         TEST_EVENT_BASE1_EV1,
                                                         test_handler_post,
                                                         &arg,
                                                         &ctx));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV2,
                                                test_handler_inc,
                                                &secondary_handler_count));

    // Test that a handler can post to a different loop while there is still slots on the queue
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(1, arg.count);
    TEST_ASSERT_EQUAL(1, secondary_handler_count);
}

static void test_handler_post_timeout(void* handler_arg, esp_event_base_t base, int32_t id, void* event_arg)
{
    posting_handler_data_t* data = (posting_handler_data_t*) handler_arg;
    TEST_ESP_OK(esp_event_post_to(data->loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, esp_event_post_to(data->loop,
                                                         s_test_base1,
                                                         TEST_EVENT_BASE1_EV2,
                                                         NULL,
                                                         0,
                                                         1));
    data->count++;
}

TEST_CASE("posting to loop from handler times out", "[event][linux]")
{
    EV_LoopFix loop_fix(1);

    posting_handler_data_t arg = {
        .loop = loop_fix.loop,
        .count = 0,
    };
    int secondary_handler_count = 0;

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_post_timeout,
                                                &arg));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV2,
                                                test_handler_inc,
                                                &secondary_handler_count));

    // Test that a handler can post to a different loop while there is still slots on the queue
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(1, arg.count);
    TEST_ASSERT_EQUAL(1, secondary_handler_count);
}

static void test_handler_take_sem(void* handler_arg, esp_event_base_t base, int32_t id, void* event_arg)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t) handler_arg;
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(sem, portMAX_DELAY));
}

static void test_handler_give_sem(void* handler_arg, esp_event_base_t base, int32_t id, void* event_arg)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t) handler_arg;
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreGive(sem));
}

const TickType_t ONE_TICK = 1;

static void wait_taken(SemaphoreHandle_t sem, TickType_t delay_ticks_if_not_taken)
{
    while (xSemaphoreTake(sem, ONE_TICK) == pdTRUE) {
        xSemaphoreGive(sem);
        vTaskDelay(delay_ticks_if_not_taken);
    }
}

TEST_CASE("can post while handler is executing - dedicated task", "[event][linux]")
{
    EV_LoopFix loop_fix(1, "loop_task");

    SemaphoreHandle_t ev1_sem = xSemaphoreCreateBinary();
    SemaphoreHandle_t ev2_sem = xSemaphoreCreateBinary();
    TEST_ASSERT(ev1_sem);
    TEST_ASSERT(ev2_sem);

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_take_sem,
                                                ev1_sem));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV2,
                                                test_handler_give_sem,
                                                ev2_sem));

    // Trigger waiting by sending first event
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));

    // Wait until semaphore is actually taken, which means handler is running and blocked
    wait_taken(ev1_sem, 2);

    // Check that event can be posted while handler is running (waiting on the semaphore)
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV2, NULL, 0, portMAX_DELAY));

    // Now the event queue has to be full, expect timeout
    TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, esp_event_post_to(loop_fix.loop,
                                                         s_test_base1,
                                                         TEST_EVENT_BASE1_EV2,
                                                         NULL,
                                                         0,
                                                         1));

    // Run all events
    xSemaphoreGive(ev1_sem);

    // verify that test_handler_give_sem() has run, too
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(ev2_sem, portMAX_DELAY));

    vSemaphoreDelete(ev1_sem);
    vSemaphoreDelete(ev2_sem);
}

static void test_post_from_handler_loop_task(void* args)
{
    esp_event_loop_handle_t event_loop = (esp_event_loop_handle_t) args;

    TEST_ESP_OK(esp_event_loop_run(event_loop, portMAX_DELAY));
}

static void never_run(void* handler_arg, esp_event_base_t base, int32_t id, void* event_arg)
{
    TEST_ASSERT(false);
}

TEST_CASE("can not post while handler is executing - no dedicated task", "[event][linux]")
{
    EV_LoopFix loop_fix(1);

    SemaphoreHandle_t sem = xSemaphoreCreateBinary();
    SemaphoreHandle_t wait_done_sem = xSemaphoreCreateBinary();
    TEST_ASSERT(sem);

    TaskHandle_t mtask;
    xTaskCreate(test_post_from_handler_loop_task,
                "task",
                2584,
                (void*) loop_fix.loop,
                uxTaskPriorityGet(NULL) + 1,
                &mtask);

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_take_sem,
                                                sem));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV2,
                                                never_run,
                                                NULL));
    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base2,
                                                TEST_EVENT_BASE1_EV1,
                                                test_handler_give_sem,
                                                wait_done_sem));

    // Trigger waiting by sending first event
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));

    // Wait until semaphore is actually taken, which means handler is running and blocked
    wait_taken(sem, 2);

    // For loop without tasks, posting is more restrictive. Posting should wait until execution of handler finishes
    TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, esp_event_post_to(loop_fix.loop,
                                                         s_test_base1,
                                                         TEST_EVENT_BASE1_EV2,
                                                         NULL,
                                                         0,
                                                         1));

    // Being running all events
    xSemaphoreGive(sem);

    // wait until all events have run
    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base2, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    xSemaphoreTake(wait_done_sem, portMAX_DELAY);

    // Clean up
    vTaskDelete(mtask);
    vSemaphoreDelete(wait_done_sem);
    vSemaphoreDelete(sem);
}

struct EventData {
    EventData(size_t expected_event_data_size) : expected_size(expected_event_data_size) { }
    constexpr static size_t MAX_SIZE = 16;
    void *event_arg; // only test for nullptr!
    size_t expected_size;
    uint8_t event_data[MAX_SIZE];
};

static void save_ev_data(void* handler_arg, esp_event_base_t base, int32_t id, void* event_arg)
{
    EventData *ev_data = (EventData *) handler_arg;
    ev_data->event_arg = event_arg;
    if (ev_data->expected_size > 0) {
        memcpy(ev_data->event_data, event_arg, ev_data->expected_size);
    }
}

TEST_CASE("event data null", "[event][linux]")
{
    EV_LoopFix loop_fix;
    EventData saved_ev_data(0);

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                save_ev_data,
                                                &saved_ev_data));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop, s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_EQUAL(NULL, saved_ev_data.event_arg);
}

TEST_CASE("event data one nonzero byte", "[event][linux]")
{
    EV_LoopFix loop_fix;
    uint8_t ev_data = 47;
    EventData saved_ev_data(1);

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                save_ev_data,
                                                &saved_ev_data));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop,
                                  s_test_base1,
                                  TEST_EVENT_BASE1_EV1,
                                  &ev_data,
                                  sizeof(ev_data),
                                  portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_NOT_EQUAL(NULL, saved_ev_data.event_arg);
    TEST_ASSERT_EQUAL(47, saved_ev_data.event_data[0]);
}

TEST_CASE("event data one zero byte", "[event][linux]")
{
    EV_LoopFix loop_fix;
    uint8_t ev_data = 0;
    EventData saved_ev_data(1);
    saved_ev_data.event_data[0] = 47;

    esp_event_handler_instance_t ctx;

    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         s_test_base2,
                                                         TEST_EVENT_BASE1_EV1,
                                                         save_ev_data,
                                                         &saved_ev_data,
                                                         &ctx));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop,
                                  s_test_base2,
                                  TEST_EVENT_BASE1_EV1,
                                  &ev_data,
                                  sizeof(ev_data),
                                  portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_NOT_EQUAL(NULL, saved_ev_data.event_arg);
    TEST_ASSERT_EQUAL(0, saved_ev_data.event_data[0]);
}

TEST_CASE("event data many bytes", "[event][linux]")
{
    EV_LoopFix loop_fix;
    uint8_t ev_data[EventData::MAX_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EventData saved_ev_data(16);

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                save_ev_data,
                                                &saved_ev_data));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop,
                                  s_test_base1,
                                  TEST_EVENT_BASE1_EV1,
                                  &ev_data,
                                  sizeof(ev_data),
                                  portMAX_DELAY));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_NOT_EQUAL(NULL, saved_ev_data.event_arg);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(ev_data, saved_ev_data.event_data, EventData::MAX_SIZE);
}

TEST_CASE("event data one byte is copied on post", "[event][linux]")
{
    EV_LoopFix loop_fix;
    uint8_t ev_data = 47;
    EventData saved_ev_data(1);

    esp_event_handler_instance_t ctx;

    TEST_ESP_OK(esp_event_handler_instance_register_with(loop_fix.loop,
                                                         s_test_base2,
                                                         TEST_EVENT_BASE1_EV1,
                                                         save_ev_data,
                                                         &saved_ev_data,
                                                         &ctx));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop,
                                  s_test_base2,
                                  TEST_EVENT_BASE1_EV1,
                                  &ev_data,
                                  sizeof(ev_data),
                                  portMAX_DELAY));
    ev_data = 42;
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_NOT_EQUAL(NULL, saved_ev_data.event_arg);
    TEST_ASSERT_EQUAL(47, saved_ev_data.event_data[0]);
}

TEST_CASE("event data many bytes are copied on post", "[event][linux]")
{
    EV_LoopFix loop_fix;
    uint8_t ev_data[EventData::MAX_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uint8_t ev_data_expected[EventData::MAX_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EventData saved_ev_data(16);

    TEST_ESP_OK(esp_event_handler_register_with(loop_fix.loop,
                                                s_test_base1,
                                                TEST_EVENT_BASE1_EV1,
                                                save_ev_data,
                                                &saved_ev_data));

    TEST_ESP_OK(esp_event_post_to(loop_fix.loop,
                                  s_test_base1,
                                  TEST_EVENT_BASE1_EV1,
                                  &ev_data,
                                  sizeof(ev_data),
                                  portMAX_DELAY));
    memset(ev_data, 0, sizeof(ev_data));
    TEST_ESP_OK(esp_event_loop_run(loop_fix.loop, ZERO_DELAY));

    TEST_ASSERT_NOT_EQUAL(NULL, saved_ev_data.event_arg);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(ev_data_expected, saved_ev_data.event_data, EventData::MAX_SIZE);
}

TEST_CASE("default loop: registering fails on uninitialized default loop", "[event][default][linux]")
{
    esp_event_handler_instance_t instance;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_event_handler_instance_register(s_test_base1,
                                                                                 TEST_EVENT_BASE1_EV1,
                                                                                 never_run,
                                                                                 NULL,
                                                                                 &instance));
}

TEST_CASE("default loop: can create and delete loop", "[event][default][linux]")
{
    TEST_ESP_OK(esp_event_loop_create_default());

    TEST_ESP_OK(esp_event_loop_delete_default());
}

TEST_CASE("default loop: registering/unregistering event works", "[event][default][linux]")
{
    TEST_ESP_OK(esp_event_loop_create_default());

    if (TEST_PROTECT()) {
        SemaphoreHandle_t waiter = xSemaphoreCreateBinary();

        esp_event_handler_instance_t instance = NULL;
        TEST_ESP_OK(esp_event_handler_instance_register(s_test_base1,
                                                        TEST_EVENT_BASE1_EV1,
                                                        test_handler_give_sem,
                                                        waiter,
                                                        &instance));
        TEST_ASSERT(instance);
        TEST_ESP_OK(esp_event_post(s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));

        TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(waiter, pdMS_TO_TICKS(50)));

        TEST_ESP_OK(esp_event_handler_instance_unregister(s_test_base1,
                                                          TEST_EVENT_BASE1_EV1,
                                                          instance));
        TEST_ESP_OK(esp_event_post(s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));

        TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreTake(waiter, pdMS_TO_TICKS(50)));

        vSemaphoreDelete(waiter);
    }

    TEST_ESP_OK(esp_event_loop_delete_default());
}

TEST_CASE("default event loop: registering event handler instance without instance context works", "[event][default][linux]")
{
    TEST_ESP_OK(esp_event_loop_create_default());

    if (TEST_PROTECT()) {
        SemaphoreHandle_t waiter = xSemaphoreCreateBinary();

        TEST_ESP_OK(esp_event_handler_instance_register(ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, test_handler_give_sem, waiter, NULL));
        TEST_ESP_OK(esp_event_post(s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, portMAX_DELAY));

        TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(waiter, pdMS_TO_TICKS(50)));

        vSemaphoreDelete(waiter);
    }

    TEST_ESP_OK(esp_event_loop_delete_default());
}

#if CONFIG_ESP_EVENT_LOOP_PROFILING
static void handler_all(void* arg, esp_event_base_t event_base, int32_t event_id, void* data)
{
    printf("base=%s, id=%" PRId32 ", data=%p\n", event_base, event_id, data);
}

static void handler_base(void* arg, esp_event_base_t event_base, int32_t event_id, void* data)
{
    printf("base=%s, id=%" PRId32 ", data=%p\n", event_base, event_id, data);
}

static void handler_id(void* arg, esp_event_base_t event_base, int32_t event_id, void* data)
{
    printf("base=%s, id=%" PRId32 ", data=%p\n", event_base, event_id, data);
}

/* This test executes the following steps:
 * 1) register handler_id for a event id A and event base B,
 * 2) register handler_base for a given event base,
 * 3) register handler_all for on the loop level,
 * 4) post an event (A, B),
 * 5) call esp_event_dump and make sure by parsing the string IN PYTEST ENVIRONMENT that
 * all handlers profiling data is printed
 * 6) unregister the handlers successfully */
TEST_CASE("profiling reports valid values", "[event][default]")
{
    TEST_ESP_OK(esp_event_loop_create_default());

    /* register handler for event base 1 and event id 1 */
    TEST_ESP_OK(esp_event_handler_register(s_test_base1, TEST_EVENT_BASE1_EV1, handler_id, NULL));

    /* register handler for event base 1 and all event ids */
    TEST_ESP_OK(esp_event_handler_register(s_test_base1, ESP_EVENT_ANY_ID, handler_base, NULL));

    /* register handler for all event bases and all event ids */
    TEST_ESP_OK(esp_event_handler_register(ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, handler_all, NULL));

    /* post an event on event base 1, event id 1 */
    TEST_ESP_OK(esp_event_post(s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, pdMS_TO_TICKS(1000)));

    /* post an event 1 from base 1 and check the dump.
     * - 3 handlers invoked, exec time is not 0 */
    esp_event_dump(stdout);

    /* unregister handlers */
    TEST_ESP_OK(esp_event_handler_unregister(ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, handler_all));
    TEST_ESP_OK(esp_event_handler_unregister(s_test_base1, ESP_EVENT_ANY_ID, handler_base));
    TEST_ESP_OK(esp_event_handler_unregister(s_test_base1, TEST_EVENT_BASE1_EV1, handler_id));

    /* delete loop */
    TEST_ESP_OK(esp_event_loop_delete_default());
}

static void handler_id2(void* arg, esp_event_base_t event_base, int32_t event_id, void* data)
{
    printf("event received: base=%s, id=%" PRId32 ", data=%p\n", event_base, event_id, data);

    /* self unregistering handler */
    TEST_ESP_OK(esp_event_handler_unregister(s_test_base1, TEST_EVENT_BASE1_EV1, handler_id2));

    /* register a new handler on id level */
    TEST_ESP_OK(esp_event_handler_register(s_test_base1, TEST_EVENT_BASE1_EV1, handler_id, NULL));
}

/* This test executes the following steps:
 * 1) register handler_id2 for a given event id and event base,
 * 2) post an event to trigger the handler_id2,
 * 3) unregister the handler_id2 during the execution of the handler_id2 and register
 * handler_id instead.
 * 4) call esp_event_dump and make sure by parsing the string IN PYTEST ENVIRONMENT that
 * 1 handler profiling data is printed
 * 5) unregister the handler_id successfully */
TEST_CASE("esp_event_dump does not show self unregistered handler", "[event][default]")
{
    TEST_ESP_OK(esp_event_loop_create_default());

    /* register handler for event base 1 and event id 1 */
    TEST_ESP_OK(esp_event_handler_register(s_test_base1, TEST_EVENT_BASE1_EV1, handler_id2, NULL));

    /* post an event on event base 1, event id 1 */
    TEST_ESP_OK(esp_event_post(s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, pdMS_TO_TICKS(1000)));

    /* post an event 1 from base 1 and check the dump.
     * - 1 handler invoked 0 times, exec time is 0 us */
    esp_event_dump(stdout);

    /* unregister handler id */
    TEST_ESP_OK(esp_event_handler_unregister(s_test_base1, TEST_EVENT_BASE1_EV1, handler_id));

    /* delete loop */
    TEST_ESP_OK(esp_event_loop_delete_default());
}

static SemaphoreHandle_t s_event_mutex;
static StaticSemaphore_t s_event_mutex_buf;
static size_t s_handler_triggered = 0;

static void self_unregistering_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* data)
{
    xSemaphoreTake(s_event_mutex, portMAX_DELAY);

    /* self unregistering handler */
    TEST_ESP_OK(esp_event_handler_unregister(s_test_base1, TEST_EVENT_BASE1_EV1, self_unregistering_handler));

    s_handler_triggered++;

    xSemaphoreGive(s_event_mutex);
}

/* This test makes sure that once a handler is unregistered, it is never called again.
 * This test follows the update in esp event unregistration process where a handler can
 * be marked as unregistered but not removed directly from the list it belongs to.
 * The test creates a handler triggered by a specific combination of event base / event id
 * and generates 2 consecutive events that should trigger the handler. The handler unregisters
 * itself. Make sure that the second event does not trigger the handler again.
 *
 * Both event posts need to be queued before the unregistration happens, to make sure that event if
 * the handler is still present in the handlers lists when the second event triggers, it will not
 * be called. To make sure of that, the execution of the handler is blocked by a mutex released from
 * test after the 2 events are posted. */
TEST_CASE("self unregistered handlers are never called again after they return", "[event][default]")
{
    s_event_mutex = xSemaphoreCreateMutexStatic(&s_event_mutex_buf);
    TEST_ASSERT_NOT_NULL(s_event_mutex);

    esp_err_t ret = esp_event_loop_create_default();
    printf("esp_event_loop_create_default %d\n", ret);
    TEST_ESP_OK(ret);

    /* register handler for event base 1 and event id 1 */
    ret = esp_event_handler_register(s_test_base1, TEST_EVENT_BASE1_EV1, self_unregistering_handler, NULL);
    printf("esp_event_handler_register %d\n", ret);
    TEST_ESP_OK(ret);

    /* take the mutex to block the execution of the self_unregistering_handler */
    xSemaphoreTake(s_event_mutex, portMAX_DELAY);

    /* post 2 times the event on event base 1, event id 1 */
    ret = esp_event_post(s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, pdMS_TO_TICKS(1000));
    printf("esp_event_post %d\n", ret);
    TEST_ESP_OK(ret);
    ret = esp_event_post(s_test_base1, TEST_EVENT_BASE1_EV1, NULL, 0, pdMS_TO_TICKS(1000));
    printf("esp_event_post %d\n", ret);
    TEST_ESP_OK(ret);

    /* release the mutex to execute the self_unregistering_handler */
    xSemaphoreGive(s_event_mutex);

    /* make sure the handler was called only once */
    TEST_ASSERT(s_handler_triggered == 1);

    /* delete mutex */
    vSemaphoreDelete(s_event_mutex);

    /* reset the static variable in case the test gets called once more */
    s_handler_triggered = 0;

    /* delete loop */
    ret = esp_event_loop_delete_default();
    printf("esp_event_loop_delete_default %d\n", ret);
    TEST_ESP_OK(ret);
}
#endif // CONFIG_ESP_EVENT_LOOP_PROFILING
