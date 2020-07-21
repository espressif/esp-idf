// Copyright 2016-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/pm.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/pm.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/pm.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Power management constraints
 */
typedef enum {
    /**
     * Require CPU frequency to be at the maximum value set via esp_pm_configure.
     * Argument is unused and should be set to 0.
     */
    ESP_PM_CPU_FREQ_MAX,
    /**
     * Require APB frequency to be at the maximum value supported by the chip.
     * Argument is unused and should be set to 0.
     */
    ESP_PM_APB_FREQ_MAX,
    /**
     * Prevent the system from going into light sleep.
     * Argument is unused and should be set to 0.
     */
    ESP_PM_NO_LIGHT_SLEEP,
} esp_pm_lock_type_t;

/**
 * @brief Set implementation-specific power management configuration
 * @param config pointer to implementation-specific configuration structure (e.g. esp_pm_config_esp32)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the configuration values are not correct
 *      - ESP_ERR_NOT_SUPPORTED if certain combination of values is not supported,
 *        or if CONFIG_PM_ENABLE is not enabled in sdkconfig
 */
esp_err_t esp_pm_configure(const void* config);


/**
 * @brief Opaque handle to the power management lock
 */
typedef struct esp_pm_lock* esp_pm_lock_handle_t;


/**
 * @brief Initialize a lock handle for certain power management parameter
 *
 * When lock is created, initially it is not taken.
 * Call esp_pm_lock_acquire to take the lock.
 *
 * This function must not be called from an ISR.
 *
 * @param lock_type Power management constraint which the lock should control
 * @param arg argument, value depends on lock_type, see esp_pm_lock_type_t
 * @param name arbitrary string identifying the lock (e.g. "wifi" or "spi").
 *             Used by the esp_pm_dump_locks function to list existing locks.
 *             May be set to NULL. If not set to NULL, must point to a string which is valid
 *             for the lifetime of the lock.
 * @param[out] out_handle  handle returned from this function. Use this handle when calling
 *                         esp_pm_lock_delete, esp_pm_lock_acquire, esp_pm_lock_release.
 *                         Must not be NULL.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if the lock structure can not be allocated
 *      - ESP_ERR_INVALID_ARG if out_handle is NULL or type argument is not valid
 *      - ESP_ERR_NOT_SUPPORTED if CONFIG_PM_ENABLE is not enabled in sdkconfig
 */
esp_err_t esp_pm_lock_create(esp_pm_lock_type_t lock_type, int arg,
        const char* name, esp_pm_lock_handle_t* out_handle);

/**
 * @brief Take a power management lock
 *
 * Once the lock is taken, power management algorithm will not switch to the
 * mode specified in a call to esp_pm_lock_create, or any of the lower power
 * modes (higher numeric values of 'mode').
 *
 * The lock is recursive, in the sense that if esp_pm_lock_acquire is called
 * a number of times, esp_pm_lock_release has to be called the same number of
 * times in order to release the lock.
 *
 * This function may be called from an ISR.
 *
 * This function is not thread-safe w.r.t. calls to other esp_pm_lock_*
 * functions for the same handle.
 *
 * @param handle handle obtained from esp_pm_lock_create function
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the handle is invalid
 *      - ESP_ERR_NOT_SUPPORTED if CONFIG_PM_ENABLE is not enabled in sdkconfig
 */
esp_err_t esp_pm_lock_acquire(esp_pm_lock_handle_t handle);

/**
 * @brief Release the lock taken using esp_pm_lock_acquire.
 *
 * Call to this functions removes power management restrictions placed when
 * taking the lock.
 *
 * Locks are recursive, so if esp_pm_lock_acquire is called a number of times,
 * esp_pm_lock_release has to be called the same number of times in order to
 * actually release the lock.
 *
 * This function may be called from an ISR.
 *
 * This function is not thread-safe w.r.t. calls to other esp_pm_lock_*
 * functions for the same handle.
 *
 * @param handle handle obtained from esp_pm_lock_create function
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the handle is invalid
 *      - ESP_ERR_INVALID_STATE if lock is not acquired
 *      - ESP_ERR_NOT_SUPPORTED if CONFIG_PM_ENABLE is not enabled in sdkconfig
 */
esp_err_t esp_pm_lock_release(esp_pm_lock_handle_t handle);

/**
 * @brief Delete a lock created using esp_pm_lock
 *
 * The lock must be released before calling this function.
 *
 * This function must not be called from an ISR.
 *
 * @param handle handle obtained from esp_pm_lock_create function
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the handle argument is NULL
 *      - ESP_ERR_INVALID_STATE if the lock is still acquired
 *      - ESP_ERR_NOT_SUPPORTED if CONFIG_PM_ENABLE is not enabled in sdkconfig
 */
esp_err_t esp_pm_lock_delete(esp_pm_lock_handle_t handle);

/**
 * Dump the list of all locks to stderr
 *
 * This function dumps debugging information about locks created using
 * esp_pm_lock_create to an output stream.
 *
 * This function must not be called from an ISR. If esp_pm_lock_acquire/release
 * are called while this function is running, inconsistent results may be
 * reported.
 *
 * @param stream stream to print information to; use stdout or stderr to print
 *               to the console; use fmemopen/open_memstream to print to a
 *               string buffer.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_SUPPORTED if CONFIG_PM_ENABLE is not enabled in sdkconfig
 */
esp_err_t esp_pm_dump_locks(FILE* stream);

#ifdef __cplusplus
}
#endif
