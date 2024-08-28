/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "unity_test_runner.h"
#include "memory_checks.h"
#include "esp_heap_caps.h"
#include "esp_pthread.h"
#include <errno.h>
#include "pthread.h"

static void *exit_task(void *args)
{
    pthread_exit(NULL);
}

void setUp(void)
{
    esp_pthread_cfg_t config = esp_pthread_get_default_config();
    TEST_ASSERT_EQUAL(ESP_OK, esp_pthread_set_cfg(&config));

    /* Needed to allocate the lazy allocated locks */
    pthread_t pthread_object = (pthread_t)NULL;
    TEST_ASSERT_EQUAL_INT(0, pthread_create(&pthread_object, NULL, exit_task, NULL));
    TEST_ASSERT_EQUAL_INT(0, pthread_join(pthread_object, NULL));

    test_utils_record_free_mem();
}

void tearDown(void)
{
    test_utils_finish_and_evaluate_leaks(0, 0);
}

TEST_CASE("esp_pthread_get_default_config creates correct stack memory capabilities", "[set_cfg]")
{
    esp_pthread_cfg_t default_config = esp_pthread_get_default_config();

    // The default must always be internal, 8-bit accessible RAM
    TEST_ASSERT_EQUAL_HEX(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL, default_config.stack_alloc_caps);
}

TEST_CASE("correct memory is accepted", "[set_cfg]")
{
    esp_pthread_cfg_t default_config = esp_pthread_get_default_config();

    default_config.stack_alloc_caps = MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_pthread_set_cfg(&default_config));
}

TEST_CASE("Setting stack with heap caps 0 sets the default value", "[set_cfg]")
{
    esp_pthread_cfg_t config = { .stack_size = 4096 }; // all other values are set to 0

    TEST_ASSERT_EQUAL(ESP_OK, esp_pthread_set_cfg(&config));

    TEST_ASSERT_EQUAL(ESP_OK, esp_pthread_get_cfg(&config));
    TEST_ASSERT_EQUAL(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL, config.stack_alloc_caps);
}

TEST_CASE("Setting stack with non 8-bit caps fails", "[set_cfg]")
{
    esp_pthread_cfg_t config = esp_pthread_get_default_config();
    config.stack_alloc_caps = MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_pthread_set_cfg(&config));
}

static void *check_stack_in_spiram(void *arg)
{
    int ret_value;
    if (esp_ptr_internal(&ret_value)) {
        ret_value = 0;
    } else {
        ret_value = 1;
    }
    vTaskDelay(2); // ensure the test task has time to continue execution
    pthread_exit((void *) ret_value);
    return NULL;
}

TEST_CASE("pthread_create fails because out of PSRAM", "[psram]")
{
    esp_pthread_cfg_t config = esp_pthread_get_default_config();
    config.stack_alloc_caps = MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM;
    config.stack_size = 0xFFFFFFFF; // far larger than the virtual address space on ESP32
    pthread_t pthread_object = (pthread_t)NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_pthread_set_cfg(&config));

    TEST_ASSERT_EQUAL(ENOMEM, pthread_create(&pthread_object, NULL, check_stack_in_spiram, NULL));
}

TEST_CASE("pthread create large PSRAM stack", "[psram]")
{
    esp_pthread_cfg_t config = esp_pthread_get_default_config();
    config.stack_alloc_caps = MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM;
    config.stack_size = 0x80000; // value is too large for any internal RAM on current chips
    int res = -1;
    int thread_rval = -1;
    pthread_t pthread_object = (pthread_t)NULL;

    TEST_ASSERT_EQUAL(ESP_OK, esp_pthread_set_cfg(&config));

    res = pthread_create(&pthread_object, NULL, check_stack_in_spiram, NULL);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = pthread_join(pthread_object, (void*) &thread_rval);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(1, thread_rval);

    // Add a short delay to allow the idle task to free any remaining memory
    vTaskDelay(2);
}

TEST_CASE("pthread with stack in internal RAM", "[psram]")
{
    int res = -1;
    int thread_rval = -1;
    pthread_t pthread_object = (pthread_t)NULL;

    res = pthread_create(&pthread_object, NULL, check_stack_in_spiram, NULL);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = pthread_join(pthread_object, (void*) &thread_rval);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(0, thread_rval);

    // Add a short delay to allow the idle task to free any remaining memory
    vTaskDelay(2);
}

void app_main(void)
{
    vTaskPrioritySet(NULL, CONFIG_UNITY_FREERTOS_PRIORITY);
    printf("pthread PSRAM Test");
    unity_run_menu();
}
