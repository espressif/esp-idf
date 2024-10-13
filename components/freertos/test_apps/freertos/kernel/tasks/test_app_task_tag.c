/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "test_utils.h"

#if CONFIG_FREERTOS_USE_APPLICATION_TASK_TAG

static BaseType_t tag_cb(void *arg)
{
    BaseType_t *tag_cb_called = (BaseType_t *)arg;
    *tag_cb_called = pdTRUE;
    return pdTRUE;
}

TEST_CASE("Test application task tag", "[freertos]")
{
    BaseType_t tag_cb_called = pdFALSE;

    // Set the app task tag for current task
    vTaskSetApplicationTaskTag(NULL, tag_cb);
    // Check app task tag is correct
    TEST_ASSERT_EQUAL(tag_cb, xTaskGetApplicationTaskTag(NULL));
    // Test the app task tag by calling it
    TEST_ASSERT_EQUAL(pdTRUE, xTaskCallApplicationTaskHook(NULL, (void *)&tag_cb_called));
    TEST_ASSERT_EQUAL(pdTRUE, tag_cb_called);
}

#endif // CONFIG_FREERTOS_USE_APPLICATION_TASK_TAG
