/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * Additions Copyright 2016 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "threads_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the provided mutex
 *
 * Call this function to initialize the mutex
 *
 * @param IoT_Mutex_t - pointer to the mutex to be initialized
 * @return IoT_Error_t - error code indicating result of operation
 */
IoT_Error_t aws_iot_thread_mutex_init(IoT_Mutex_t *pMutex) {

    pMutex->mutex = xSemaphoreCreateRecursiveMutex();
    return pMutex->mutex ? SUCCESS : MUTEX_INIT_ERROR;
}

/**
 * @brief Lock the provided mutex
 *
 * Call this function to lock the mutex before performing a state change
 * Blocking, thread will block until lock request fails
 *
 * @param IoT_Mutex_t - pointer to the mutex to be locked
 * @return IoT_Error_t - error code indicating result of operation
 */
IoT_Error_t aws_iot_thread_mutex_lock(IoT_Mutex_t *pMutex) {
    xSemaphoreTakeRecursive(pMutex->mutex, portMAX_DELAY);
    return SUCCESS;
}

/**
 * @brief Try to lock the provided mutex
 *
 * Call this function to attempt to lock the mutex before performing a state change
 * Non-Blocking, immediately returns with failure if lock attempt fails
 *
 * @param IoT_Mutex_t - pointer to the mutex to be locked
 * @return IoT_Error_t - error code indicating result of operation
 */
IoT_Error_t aws_iot_thread_mutex_trylock(IoT_Mutex_t *pMutex) {
    if (xSemaphoreTakeRecursive(pMutex->mutex, 0)) {
        return SUCCESS;
    } else {
        return MUTEX_LOCK_ERROR;
    }
}

/**
 * @brief Unlock the provided mutex
 *
 * Call this function to unlock the mutex before performing a state change
 *
 * @param IoT_Mutex_t - pointer to the mutex to be unlocked
 * @return IoT_Error_t - error code indicating result of operation
 */
IoT_Error_t aws_iot_thread_mutex_unlock(IoT_Mutex_t *pMutex) {
    if (xSemaphoreGiveRecursive(pMutex->mutex)) {
        return SUCCESS;
    } else {
        return MUTEX_UNLOCK_ERROR;
    }
}

/**
 * @brief Destroy the provided mutex
 *
 * Call this function to destroy the mutex
 *
 * @param IoT_Mutex_t - pointer to the mutex to be destroyed
 * @return IoT_Error_t - error code indicating result of operation
 */
IoT_Error_t aws_iot_thread_mutex_destroy(IoT_Mutex_t *pMutex) {
    vSemaphoreDelete(pMutex->mutex);
    return SUCCESS;
}

#ifdef __cplusplus
}
#endif

