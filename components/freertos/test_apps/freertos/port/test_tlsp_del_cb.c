/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#if CONFIG_FREERTOS_SMP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "test_utils.h"

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
    TaskHandle_t tasks[portNUM_PROCESSORS];
    int tlsps[portNUM_PROCESSORS][NUM_TLSP];

    for (int i = 0; i < portNUM_PROCESSORS; i++) {
        TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(tlsp_task, "tlsp_tsk", configMINIMAL_STACK_SIZE * 2, (void *)&tlsps[i], UNITY_FREERTOS_PRIORITY - 1, &tasks[i], i));
    }
    // Significant delay to let tasks run and delete themselves
    vTaskDelay(pdMS_TO_TICKS(100));

    // Check the values of the TLSPs to see if the del cb have ran
    for (int i = 0; i < portNUM_PROCESSORS; i++) {
        for (int index = 0; index < NUM_TLSP; index++) {
            // Del cb should have set the TLSP to a negative value
            TEST_ASSERT_EQUAL(-index, tlsps[i][index]);
        }
    }
}

#else // CONFIG_FREERTOS_SMP

// Todo: Remove IDF FreeRTOS Test Case

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "test_utils.h"

/* --------Test backported thread local storage pointer and deletion cb feature----------
 * vTaskSetThreadLocalStoragePointerAndDelCallback()
 * pvTaskGetThreadLocalStoragePointer(),
 *
 * This test creates a task and set's the task's TLSPs. The task is then deleted
 * which should trigger the deletion cb.
 */

#define NO_OF_TLSP  configNUM_THREAD_LOCAL_STORAGE_POINTERS
#define TLSP_SET_BASE    0x0F       //0b1111 to be bit shifted by index
#define TLSP_DEL_BASE    0x05       //0b0101 to be bit shifted by index

//The variables pointed to by Thread Local Storage Pointer
static uint32_t task_storage[portNUM_PROCESSORS][NO_OF_TLSP] = {0};

/* If static task cleanup is defined, can't set index 0 even if the calling task is not a pthread,
   as the cleanup is called for every task.
*/
#if defined(CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP)
static const int skip_index = 0; /*PTHREAD_TLS_INDEX*/
#else
static const int skip_index = -1;
#endif

static void del_cb(int index, void *ptr)
{
    *((uint32_t *)ptr) = (TLSP_DEL_BASE << index);   //Indicate deletion by setting task storage element to a unique value
}

static void task_cb(void *arg)
{
    int core = xPortGetCoreID();
    for(int i = 0; i < NO_OF_TLSP; i++){
        if (i == skip_index) {
            continue;
        }

        task_storage[core][i] = (TLSP_SET_BASE << i);   //Give each element of task_storage a unique number
        vTaskSetThreadLocalStoragePointerAndDelCallback(NULL, i, (void *)&task_storage[core][i], del_cb);   //Set each TLSP to point to a task storage element
    }

    for(int i = 0; i < NO_OF_TLSP; i++){
        if (i == skip_index) {
            continue;
        }

        uint32_t * tlsp = (uint32_t *)pvTaskGetThreadLocalStoragePointer(NULL, i);
        TEST_ASSERT_EQUAL(*tlsp, (TLSP_SET_BASE << i)); //Check if TLSP points to the correct task storage element by checking unique value
    }

    vTaskDelete(NULL);      //Delete Task to Trigger TSLP deletion callback
}

TEST_CASE("Test FreeRTOS thread local storage pointers and del cb", "[freertos]")
{
    //Create Task
    for(int core = 0; core < portNUM_PROCESSORS; core++){
        xTaskCreatePinnedToCore(task_cb, "task", 1024, NULL, UNITY_FREERTOS_PRIORITY+1, NULL, core);
    }
    vTaskDelay(10);     //Delay long enough for tasks to run to completion

    for(int core = 0; core < portNUM_PROCESSORS; core++){
        for(int i = 0; i < NO_OF_TLSP; i++){
            if (i == skip_index) {
                continue;
            }
            TEST_ASSERT_EQUAL((TLSP_DEL_BASE << i), task_storage[core][i]);     //Check del_cb ran by checking task storage for unique value
        }
    }
}

#endif // CONFIG_FREERTOS_SMP
