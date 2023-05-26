/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/lock.h>
#include "unity.h"
#include "test_utils.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#if defined(_RETARGETABLE_LOCKING)

static void locking_task(void* arg)
{
    _LOCK_T lock = (_LOCK_T) arg;
    __lock_acquire(lock);
    __lock_release(lock);
    vTaskSuspend(NULL);
}

static void recursive_locking_task(void* arg)
{
    _LOCK_T lock = (_LOCK_T) arg;
    __lock_acquire_recursive(lock);
    __lock_release_recursive(lock);
    vTaskSuspend(NULL);
}

static void test_inner_normal(_LOCK_T lock)
{
    /* Acquire the lock */
    __lock_acquire(lock);

    /* Create another task to try acquire same lock */
    TaskHandle_t task_hdl;
    TEST_ASSERT(xTaskCreate(&locking_task, "locking_task", 2048, lock, UNITY_FREERTOS_PRIORITY, &task_hdl));
    vTaskDelay(2);
    /* It should get blocked */
    TEST_ASSERT_EQUAL(eBlocked, eTaskGetState(task_hdl));

    /* Once we release the lock, the task should succeed and suspend itself */
    __lock_release(lock);
    vTaskDelay(2);
    TEST_ASSERT_EQUAL(eSuspended, eTaskGetState(task_hdl));
    vTaskDelete(task_hdl);

    /* Can not recursively acquire the lock from same task */
    TEST_ASSERT_EQUAL(0, __lock_try_acquire(lock));
    TEST_ASSERT_EQUAL(-1, __lock_try_acquire(lock));
    __lock_release(lock);
}

static void test_inner_recursive(_LOCK_T lock)
{
    /* Acquire the lock */
    __lock_acquire_recursive(lock);

    /* Create another task to try acquire same lock */
    TaskHandle_t task_hdl;
    TEST_ASSERT(xTaskCreate(&recursive_locking_task, "locking_task", 2048, lock, UNITY_FREERTOS_PRIORITY, &task_hdl));
    vTaskDelay(2);
    /* It should get blocked */
    TEST_ASSERT_EQUAL(eBlocked, eTaskGetState(task_hdl));

    /* Once we release the lock, the task should succeed and suspend itself */
    __lock_release_recursive(lock);
    vTaskDelay(2);
    TEST_ASSERT_EQUAL(eSuspended, eTaskGetState(task_hdl));
    vTaskDelete(task_hdl);

    /* Try recursively acquiring the lock */
    TEST_ASSERT_EQUAL(0, __lock_try_acquire_recursive(lock));
    TEST_ASSERT_EQUAL(0, __lock_try_acquire_recursive(lock));
    __lock_release_recursive(lock);
    __lock_release_recursive(lock);
}

TEST_CASE("Retargetable static locks", "[newlib_locks]")
{
    StaticSemaphore_t semaphore;
    _LOCK_T lock = (_LOCK_T) xSemaphoreCreateMutexStatic(&semaphore);
    test_inner_normal(lock);
}

TEST_CASE("Retargetable static recursive locks", "[newlib_locks]")
{
    StaticSemaphore_t semaphore;
    _LOCK_T lock = (_LOCK_T) xSemaphoreCreateRecursiveMutexStatic(&semaphore);
    test_inner_recursive(lock);
}

TEST_CASE("Retargetable dynamic locks", "[newlib_locks]")
{
    _LOCK_T lock;
    __lock_init(lock);
    test_inner_normal(lock);
    __lock_close(lock);
}

TEST_CASE("Retargetable dynamic recursive locks", "[newlib_locks]")
{
    _LOCK_T lock;
    __lock_init_recursive(lock);
    test_inner_recursive(lock);
    __lock_close_recursive(lock);
}

#endif // _RETARGETABLE_LOCKING
