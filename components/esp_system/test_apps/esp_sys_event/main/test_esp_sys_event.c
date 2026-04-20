/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "esp_system.h"
#include "esp_private/esp_sys_event.h"
#include "esp_private/esp_sys_event_internal.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "test_esp_sys_event";

// ============== Static SHUTDOWN handlers ==============
//
// SHUTDOWN is used as the static-handler test target because this test app
// never calls esp_restart(), so handlers registered here are inert except
// when a test deliberately triggers the SHUTDOWN event via the framework.
// This avoids needing a private test event with its own linker section.

static int s_static_call_order[10];
static int s_static_call_count;
static void *s_static_last_ctx;

ESP_SHUTDOWN_HANDLER_REGISTER(test_shutdown_100, 100)
{
    (void)user_arg;
    s_static_call_order[s_static_call_count++] = 100;
    s_static_last_ctx = ctx;
    ESP_LOGI(TAG, "test_shutdown_100 called, ctx=%p", ctx);
    return ESP_OK;
}

ESP_SHUTDOWN_HANDLER_REGISTER(test_shutdown_200, 200)
{
    (void)user_arg;
    s_static_call_order[s_static_call_count++] = 200;
    s_static_last_ctx = ctx;
    ESP_LOGI(TAG, "test_shutdown_200 called, ctx=%p", ctx);
    return ESP_OK;
}

ESP_SHUTDOWN_HANDLER_REGISTER(test_shutdown_150, 150)
{
    (void)user_arg;
    s_static_call_order[s_static_call_count++] = 150;
    s_static_last_ctx = ctx;
    ESP_LOGI(TAG, "test_shutdown_150 called, ctx=%p", ctx);
    return ESP_OK;
}

static void reset_static_state(void)
{
    s_static_call_count = 0;
    memset(s_static_call_order, 0, sizeof(s_static_call_order));
    s_static_last_ctx = NULL;
}

static bool is_test_shutdown_handler(esp_sys_event_fn_t handler)
{
    return handler == test_shutdown_100
           || handler == test_shutdown_150
           || handler == test_shutdown_200;
}

static size_t count_test_shutdown_handlers(void)
{
    size_t count = 0;

    ESP_SYS_EVENT_FOREACH(h, ESP_SYS_EVENT_SHUTDOWN) {
        if (is_test_shutdown_handler(h->handler)) {
            count++;
        }
    }

    return count;
}

// ============== Dynamic handlers ==============

static int s_dynamic_handler_call_count;
static void *s_dynamic_last_arg;
static void *s_dynamic_last_ctx;

static esp_err_t dynamic_handler_1(void *user_arg, void *ctx)
{
    s_dynamic_handler_call_count++;
    s_dynamic_last_arg = user_arg;
    s_dynamic_last_ctx = ctx;
    ESP_LOGI(TAG, "dynamic_handler_1 called, user_arg=%p ctx=%p", user_arg, ctx);
    return ESP_OK;
}

static esp_err_t dynamic_handler_2(void *user_arg, void *ctx)
{
    s_dynamic_handler_call_count++;
    s_dynamic_last_arg = user_arg;
    s_dynamic_last_ctx = ctx;
    ESP_LOGI(TAG, "dynamic_handler_2 called, user_arg=%p ctx=%p", user_arg, ctx);
    return ESP_OK;
}

static esp_err_t dynamic_handler_error(void *user_arg, void *ctx)
{
    (void)user_arg; (void)ctx;
    s_dynamic_handler_call_count++;
    ESP_LOGI(TAG, "dynamic_handler_error called, returning error");
    return ESP_FAIL;
}

// ============== Test cases ==============

TEST_CASE("esp_sys_event static registration - priority ordering", "[esp_sys_event]")
{
    reset_static_state();
    int test_ctx = 42;

    // Trigger via the IRAM-safe static-only entrypoint
    esp_sys_event_trigger_static(ESP_SYS_EVENT_SHUTDOWN, &test_ctx);

    // All three ESP_SHUTDOWN_HANDLER_REGISTER entries from this file must run
    TEST_ASSERT_EQUAL(3, s_static_call_count);

    // Sorted by priority: 100, 150, 200
    TEST_ASSERT_EQUAL(100, s_static_call_order[0]);
    TEST_ASSERT_EQUAL(150, s_static_call_order[1]);
    TEST_ASSERT_EQUAL(200, s_static_call_order[2]);

    // Context was forwarded
    TEST_ASSERT_EQUAL_PTR(&test_ctx, s_static_last_ctx);
}

TEST_CASE("ESP_SYS_EVENT_FOREACH - iterates static handlers", "[esp_sys_event]")
{
    int count = 0;
    int test_handler_count = 0;

    ESP_SYS_EVENT_FOREACH(h, ESP_SYS_EVENT_SHUTDOWN) {
        TEST_ASSERT_NOT_NULL(h->handler);
        if (is_test_shutdown_handler(h->handler)) {
            test_handler_count++;
        }
        count++;
    }

    // Other components may also register static SHUTDOWN handlers.
    TEST_ASSERT_GREATER_OR_EQUAL(3, count);
    TEST_ASSERT_EQUAL(3, test_handler_count);
}

TEST_CASE("esp_sys_event dynamic registration - register and trigger", "[esp_sys_event]")
{
    s_dynamic_handler_call_count = 0;
    s_dynamic_last_arg = NULL;
    s_dynamic_last_ctx = NULL;

    int user_data = 123;
    int event_ctx = 456;
    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_register(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, &user_data));
    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_register(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_2, &user_data));

    esp_sys_event_trigger_dynamic(ESP_SYS_EVENT_SHUTDOWN, &event_ctx);

    TEST_ASSERT_EQUAL(2, s_dynamic_handler_call_count);
    TEST_ASSERT_EQUAL_PTR(&user_data, s_dynamic_last_arg);
    TEST_ASSERT_EQUAL_PTR(&event_ctx, s_dynamic_last_ctx);

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_unregister(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, &user_data));
    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_unregister(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_2, &user_data));
}

TEST_CASE("esp_sys_event dynamic registration - unregister", "[esp_sys_event]")
{
    s_dynamic_handler_call_count = 0;

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_register(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, NULL));

    esp_sys_event_trigger_dynamic(ESP_SYS_EVENT_SHUTDOWN, NULL);
    TEST_ASSERT_EQUAL(1, s_dynamic_handler_call_count);

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_unregister(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, NULL));

    esp_sys_event_trigger_dynamic(ESP_SYS_EVENT_SHUTDOWN, NULL);
    TEST_ASSERT_EQUAL(1, s_dynamic_handler_call_count);

    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, esp_sys_event_unregister(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, NULL));
}

TEST_CASE("esp_sys_event dynamic registration - error handling", "[esp_sys_event]")
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sys_event_register(ESP_SYS_EVENT_MAX, dynamic_handler_1, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sys_event_register(ESP_SYS_EVENT_SHUTDOWN, NULL, NULL));

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_register(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_sys_event_register(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, NULL));
    // Same handler with different user_arg is allowed
    int x;
    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_register(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, &x));
    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_unregister(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_unregister(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, &x));

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sys_event_unregister(ESP_SYS_EVENT_MAX, dynamic_handler_1, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sys_event_unregister(ESP_SYS_EVENT_SHUTDOWN, NULL, NULL));
}

TEST_CASE("esp_sys_event dynamic registration - handler error does not abort dispatch", "[esp_sys_event]")
{
    s_dynamic_handler_call_count = 0;

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_register(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_error, NULL));

    esp_sys_event_trigger_dynamic(ESP_SYS_EVENT_SHUTDOWN, NULL);
    TEST_ASSERT_EQUAL(1, s_dynamic_handler_call_count);

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_unregister(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_error, NULL));
}

TEST_CASE("esp_sys_event dynamic registration - event isolation", "[esp_sys_event]")
{
    s_dynamic_handler_call_count = 0;

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_register(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_register(ESP_SYS_EVENT_SYSTEM_INIT_SECONDARY, dynamic_handler_2, NULL));

    esp_sys_event_trigger_dynamic(ESP_SYS_EVENT_SHUTDOWN, NULL);
    TEST_ASSERT_EQUAL(1, s_dynamic_handler_call_count);

    esp_sys_event_trigger_dynamic(ESP_SYS_EVENT_SYSTEM_INIT_SECONDARY, NULL);
    TEST_ASSERT_EQUAL(2, s_dynamic_handler_call_count);

    esp_sys_event_trigger_dynamic(ESP_SYS_EVENT_SYSTEM_INIT_CORE, NULL);
    TEST_ASSERT_EQUAL(2, s_dynamic_handler_call_count);

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_unregister(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_unregister(ESP_SYS_EVENT_SYSTEM_INIT_SECONDARY, dynamic_handler_2, NULL));
}

TEST_CASE("esp_sys_event_get_static_handlers - returns valid bounds", "[esp_sys_event]")
{
    esp_sys_event_iter_t iter = esp_sys_event_get_static_handlers(ESP_SYS_EVENT_SHUTDOWN);

    TEST_ASSERT_NOT_NULL(iter.start);
    TEST_ASSERT_NOT_NULL(iter.end);
    TEST_ASSERT(iter.start <= iter.end);
    // The 3 ESP_SHUTDOWN_HANDLER_REGISTER entries above must be present.
    TEST_ASSERT_GREATER_OR_EQUAL_size_t((size_t)3, (size_t)(iter.end - iter.start));
    TEST_ASSERT_EQUAL((size_t)3, count_test_shutdown_handlers());

    iter = esp_sys_event_get_static_handlers(ESP_SYS_EVENT_MAX);
    TEST_ASSERT_NULL(iter.start);
    TEST_ASSERT_NULL(iter.end);
}

TEST_CASE("esp_sys_event_trigger - calls both static and dynamic handlers", "[esp_sys_event]")
{
    reset_static_state();
    s_dynamic_handler_call_count = 0;

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_register(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, NULL));

    esp_sys_event_trigger(ESP_SYS_EVENT_SHUTDOWN, NULL);

    // All 3 static SHUTDOWN handlers fired
    TEST_ASSERT_EQUAL(3, s_static_call_count);
    // Dynamic handler fired
    TEST_ASSERT_EQUAL(1, s_dynamic_handler_call_count);

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_unregister(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, NULL));
}

// ============== handler_dump tests ==============

TEST_CASE("esp_sys_event_handler_dump - invalid id returns error", "[esp_sys_event]")
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sys_event_handler_dump(ESP_SYS_EVENT_MAX, stdout));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      esp_sys_event_handler_dump((esp_sys_event_id_t)(ESP_SYS_EVENT_MAX + 1), stdout));
}

TEST_CASE("esp_sys_event_handler_dump - null stream skips output", "[esp_sys_event]")
{
    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_handler_dump(ESP_SYS_EVENT_SHUTDOWN, NULL));
}

TEST_CASE("esp_sys_event_handler_dump - lists dynamic handlers with correct pointer", "[esp_sys_event]")
{
    char buf[512];
    memset(buf, 0, sizeof(buf));
    FILE *f = fmemopen(buf, sizeof(buf) - 1, "w");
    TEST_ASSERT_NOT_NULL(f);

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_register(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_register(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_2, NULL));

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_handler_dump(ESP_SYS_EVENT_SHUTDOWN, f));
    fclose(f);

    ESP_LOGI(TAG, "dump output:\n%s", buf);

    char expected_0[64], expected_1[64];
    snprintf(expected_0, sizeof(expected_0), "%p", (void *)dynamic_handler_1);
    snprintf(expected_1, sizeof(expected_1), "%p", (void *)dynamic_handler_2);
    bool event_found = strstr(buf, "Event") != NULL;
    bool handler_1_found = strstr(buf, expected_0) != NULL;
    bool handler_2_found = strstr(buf, expected_1) != NULL;

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_unregister(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_unregister(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_2, NULL));

    TEST_ASSERT_TRUE(event_found);
    TEST_ASSERT_TRUE_MESSAGE(handler_1_found, "dynamic_handler_1 pointer not found in dump");
    TEST_ASSERT_TRUE_MESSAGE(handler_2_found, "dynamic_handler_2 pointer not found in dump");
}

TEST_CASE("esp_sys_event_handler_dump - static handlers appear before dynamic", "[esp_sys_event]")
{
    // SHUTDOWN has 3 static handlers registered in this file, so static[0]
    // is guaranteed present.
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    FILE *f = fmemopen(buf, sizeof(buf) - 1, "w");
    TEST_ASSERT_NOT_NULL(f);

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_register(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, NULL));

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_handler_dump(ESP_SYS_EVENT_SHUTDOWN, f));
    fclose(f);

    ESP_LOGI(TAG, "dump output:\n%s", buf);

    const char *static0 = strstr(buf, "static[0]");
    const char *dynamic0 = strstr(buf, "dynamic[");
    bool static_before_dynamic = static0 != NULL && dynamic0 != NULL && static0 < dynamic0;

    TEST_ASSERT_EQUAL(ESP_OK, esp_sys_event_unregister(ESP_SYS_EVENT_SHUTDOWN, dynamic_handler_1, NULL));

    TEST_ASSERT_NOT_NULL_MESSAGE(static0, "static[0] not found in dump");
    TEST_ASSERT_NOT_NULL_MESSAGE(dynamic0, "dynamic handler not found in dump");
    TEST_ASSERT_TRUE_MESSAGE(static_before_dynamic, "static handlers must appear before dynamic handlers");
}
