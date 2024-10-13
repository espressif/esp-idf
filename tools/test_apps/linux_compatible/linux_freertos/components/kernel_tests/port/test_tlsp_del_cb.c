/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"

/*
Test TLSP Deletion Callbacks

Purpose:
    - Test that TLSP Deletion Callbacks can be registered
    - Test that TLSP Deletion Callbacks are called when a task is deleted

Procedure:
    - Create a task on each core along with an array of integers to act as TLSP data
    - Each task should initialize their integers to a particular value (i.e., the index value)
    - Each task should register those integers as TLSPs along with a deletion callback
    - Each task should self delete to trigger the TLSP deletion callback
    - The TLSP deletion callback should indicate that it has run by negating the integer values

Expected:
    - The TLSP deletion callback should check that the correct TLSP is provided by checking the TLSPs initialization
    value (i.e., should be set to the index value)
    - After deletion, the integer values should be negated to indicate deletion callback execution
*/

#define NUM_TLSP        CONFIG_FREERTOS_THREAD_LOCAL_STORAGE_POINTERS

static void tlsp_del_cb(int index, void *tlsp)
{
    int *val = (int *)tlsp;

    // Check that the TLSP's initialization value is correct
    TEST_ASSERT_EQUAL(index, *val);
    // Set the TLSP's value again to a negative value to indicate that the del cb has ran
    *val = -*val;
}

static void tlsp_task(void *arg)
{
    int *tlsps = (int *)arg;

    for (int index = 0; index < NUM_TLSP; index++) {
        // Initialize the TLSPs to a positive value
        tlsps[index] = index;
        // Set TLSPs and deletion callbacks
        vTaskSetThreadLocalStoragePointerAndDelCallback(NULL, index, &tlsps[index], tlsp_del_cb);
    }

    // Self delete to trigger TLSP del cb
    vTaskDelete(NULL);
}

TEST_CASE("Test TLSP deletion callbacks", "[freertos]")
{
    TaskHandle_t tasks[configNUM_CORES];
    int tlsps[configNUM_CORES][NUM_TLSP];

    for (int i = 0; i < configNUM_CORES; i++) {
        TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(tlsp_task, "tlsp_tsk", configMINIMAL_STACK_SIZE * 2, (void *)&tlsps[i], CONFIG_UNITY_FREERTOS_PRIORITY - 1, &tasks[i], i));
    }
    // Significant delay to let tasks run and delete themselves
    vTaskDelay(pdMS_TO_TICKS(100));

    // Check the values of the TLSPs to see if the del cb have ran
    for (int i = 0; i < configNUM_CORES; i++) {
        for (int index = 0; index < NUM_TLSP; index++) {
            // Del cb should have set the TLSP to a negative value
            TEST_ASSERT_EQUAL(-index, tlsps[i][index]);
        }
    }
}
