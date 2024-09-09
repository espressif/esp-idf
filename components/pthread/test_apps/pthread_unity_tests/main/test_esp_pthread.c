/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <pthread.h>
#include "sdkconfig.h"
#include "esp_pthread.h"
#include "esp_heap_caps.h"
#include "unity.h"

TEST_CASE("esp_pthread_get_default_config creates correct stack memory capabilities", "[cfg]")
{
    esp_pthread_cfg_t default_config = esp_pthread_get_default_config();

    // The default must always be internal, 8-bit accessible RAM
    TEST_ASSERT_EQUAL_HEX(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL, default_config.stack_alloc_caps);
}

TEST_CASE("null pointers are rejected", "[cfg]")
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_pthread_set_cfg(NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_pthread_get_cfg(NULL));
}

TEST_CASE("wrong heap caps are rejected", "[cfg]")
{
    esp_pthread_cfg_t default_config = esp_pthread_get_default_config();
    TEST_ASSERT_EQUAL(ESP_OK, esp_pthread_set_cfg(&default_config)); // make sure we have saved a known value

    // Test the rejection of wrong values
    default_config.stack_alloc_caps = MALLOC_CAP_32BIT;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_pthread_set_cfg(&default_config));

    default_config.stack_alloc_caps = MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_pthread_set_cfg(&default_config));

    // check that saved values are unaltered
    esp_pthread_cfg_t retrieved_config;
    TEST_ASSERT_EQUAL(ESP_OK, esp_pthread_get_cfg(&retrieved_config));
    TEST_ASSERT_EQUAL_HEX(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL, retrieved_config.stack_alloc_caps);
}

// On Linux, we silently adjust the stack size since pthread on Linux
// requires a minimum stack size of 0x4000.
#if !CONFIG_IDF_TARGET_LINUX
TEST_CASE("invalid stack size is rejected", "[cfg]")
{
    esp_pthread_cfg_t default_config = esp_pthread_get_default_config();
    TEST_ASSERT_EQUAL(ESP_OK, esp_pthread_set_cfg(&default_config)); // make sure we have saved a known value

    // Test the rejection of wrong values
    default_config.stack_size = PTHREAD_STACK_MIN - 1;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_pthread_set_cfg(&default_config));

    // check that saved values are unaltered
    esp_pthread_cfg_t retrieved_config;
    TEST_ASSERT_EQUAL(ESP_OK, esp_pthread_get_cfg(&retrieved_config));
    TEST_ASSERT_EQUAL(CONFIG_PTHREAD_TASK_STACK_SIZE_DEFAULT, retrieved_config.stack_size);
}
#endif // !CONFIG_IDF_TARGET_LINUX

TEST_CASE("correct memory is accepted", "[cfg]")
{
    esp_pthread_cfg_t default_config = esp_pthread_get_default_config();

    default_config.stack_alloc_caps = MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_pthread_set_cfg(&default_config));
}

TEST_CASE("configuration is preserved inside pthread", "[cfg]")
{
    esp_pthread_cfg_t saved_config;
    esp_pthread_cfg_t retrieved_config;
    saved_config.stack_size = PTHREAD_STACK_MIN;
    saved_config.prio = 5;
    saved_config.inherit_cfg = true;
    saved_config.thread_name = "test_esp_pthread";
    saved_config.pin_to_core = 0;
    saved_config.stack_alloc_caps = MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL;

    TEST_ASSERT_EQUAL(ESP_OK, esp_pthread_set_cfg(&saved_config));
    TEST_ASSERT_EQUAL(ESP_OK, esp_pthread_get_cfg(&retrieved_config));

    TEST_ASSERT_EQUAL(saved_config.stack_size, retrieved_config.stack_size);
    TEST_ASSERT_EQUAL(saved_config.prio, retrieved_config.prio);
    TEST_ASSERT_EQUAL(saved_config.inherit_cfg, retrieved_config.inherit_cfg);
    TEST_ASSERT_EQUAL(saved_config.thread_name, retrieved_config.thread_name);
    TEST_ASSERT_EQUAL(saved_config.pin_to_core, retrieved_config.pin_to_core);
    TEST_ASSERT_EQUAL(saved_config.stack_alloc_caps, retrieved_config.stack_alloc_caps);

    esp_pthread_cfg_t cfg = esp_pthread_get_default_config();
    TEST_ASSERT_EQUAL(ESP_OK, esp_pthread_set_cfg(&cfg));
}
