/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "unity.h"
#include "unity_test_runner.h"

#include "esp_system.h"

static jmp_buf env;
static uint32_t token;

static void jump_back_shutdown_handler(void)
{
    longjmp(env, 1);
}

static void dummy_shutdown_handler_0(void) { }
static void dummy_shutdown_handler_1(void) { }
static void dummy_shutdown_handler_2(void) { }
static void dummy_shutdown_handler_3(void) { }
static void dummy_shutdown_handler_4(void) { }

static void action(void)
{
    token++;
}

static void cleanup(void)
{
    esp_unregister_shutdown_handler(jump_back_shutdown_handler);
    esp_unregister_shutdown_handler(dummy_shutdown_handler_0);
    esp_unregister_shutdown_handler(dummy_shutdown_handler_1);
    esp_unregister_shutdown_handler(dummy_shutdown_handler_2);
    esp_unregister_shutdown_handler(dummy_shutdown_handler_3);
    esp_unregister_shutdown_handler(dummy_shutdown_handler_4);
    esp_unregister_shutdown_handler(action);
}

TEST_CASE("reset_reason", "[esp_system]")
{
    TEST_ASSERT_EQUAL(ESP_RST_POWERON, esp_reset_reason());
}

TEST_CASE("unregister_handler_works", "[esp_system]")
{
    token = 0;
    // for some reason, the handlers are executed in reverse order of adding handlers, so we always
    // register the jumping handler at first to make it execute last
    TEST_ASSERT_EQUAL(ESP_OK, esp_register_shutdown_handler(jump_back_shutdown_handler));
    TEST_ASSERT_EQUAL(ESP_OK, esp_register_shutdown_handler(action));
    TEST_ASSERT_EQUAL(ESP_OK, esp_unregister_shutdown_handler(action));

    if (setjmp(env) == 0) {
        esp_restart();
    }

    // fist unregister before any assert to avoid skipping by assert's longjmp
    cleanup();

    TEST_ASSERT_EQUAL(0, token);
}

TEST_CASE("register_shutdown_handler_twice_fails", "[esp_system]")
{
    TEST_ASSERT_EQUAL(ESP_OK, esp_register_shutdown_handler(jump_back_shutdown_handler));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_register_shutdown_handler(jump_back_shutdown_handler));

    cleanup();
}

TEST_CASE("register_shutdown_handler_works", "[esp_system]")
{
    token = 0;
    TEST_ASSERT_EQUAL(esp_register_shutdown_handler(jump_back_shutdown_handler), ESP_OK);
    TEST_ASSERT_EQUAL(esp_register_shutdown_handler(action), ESP_OK);

    if (setjmp(env) == 0) {
        esp_restart();
    }

    cleanup();

    TEST_ASSERT_EQUAL(1, token);
}

TEST_CASE("register_too_many_shutdown_handler_fails", "[esp_system]")
{
    TEST_ASSERT_EQUAL(ESP_OK, esp_register_shutdown_handler(dummy_shutdown_handler_0));
    TEST_ASSERT_EQUAL(ESP_OK, esp_register_shutdown_handler(dummy_shutdown_handler_1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_register_shutdown_handler(dummy_shutdown_handler_2));
    TEST_ASSERT_EQUAL(ESP_OK, esp_register_shutdown_handler(dummy_shutdown_handler_3));
    TEST_ASSERT_EQUAL(ESP_OK, esp_register_shutdown_handler(dummy_shutdown_handler_4));

    TEST_ASSERT_EQUAL(esp_register_shutdown_handler(jump_back_shutdown_handler), ESP_ERR_NO_MEM);

    cleanup();
}

TEST_CASE("heap_size_stubs", "[esp_system]")
{
    TEST_ASSERT_EQUAL(UINT32_MAX, esp_get_free_heap_size());
    TEST_ASSERT_EQUAL(UINT32_MAX, esp_get_free_internal_heap_size());
    TEST_ASSERT_EQUAL(UINT32_MAX, esp_get_minimum_free_heap_size());
}

void app_main(void)
{
    printf("Running esp_system host test app");
    unity_run_menu();
}
