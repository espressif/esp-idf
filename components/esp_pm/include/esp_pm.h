/*
 * SPDX-FileCopyrightText: 2016-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Power management config
 *
 * Pass a pointer to this structure as an argument to esp_pm_configure function.
 */
typedef struct {
    int max_freq_mhz;         /*!< Maximum CPU frequency, in MHz */
    int min_freq_mhz;         /*!< Minimum CPU frequency to use when no locks are taken, in MHz */
    bool light_sleep_enable;  /*!< Enter light sleep when no locks are taken */
} esp_pm_config_t;

/**
 * backward compatibility
 * newer chips no longer require this typedef
 */
typedef esp_pm_config_t esp_pm_config_esp32_t   __attribute__((deprecated("please use esp_pm_config_t instead")));
typedef esp_pm_config_t esp_pm_config_esp32s2_t __attribute__((deprecated("please use esp_pm_config_t instead")));
typedef esp_pm_config_t esp_pm_config_esp32s3_t __attribute__((deprecated("please use esp_pm_config_t instead")));
typedef esp_pm_config_t esp_pm_config_esp32c3_t __attribute__((deprecated("please use esp_pm_config_t instead")));
typedef esp_pm_config_t esp_pm_config_esp32c2_t __attribute__((deprecated("please use esp_pm_config_t instead")));
typedef esp_pm_config_t esp_pm_config_esp32c6_t __attribute__((deprecated("please use esp_pm_config_t instead")));

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
 * @brief Get implementation-specific power management configuration
 * @param config pointer to implementation-specific configuration structure (e.g. esp_pm_config_esp32)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the pointer is null
 */
esp_err_t esp_pm_get_configuration(void* config);

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

#if CONFIG_PM_LIGHT_SLEEP_CALLBACKS
/**
 * @brief Function prototype for light sleep callback functions (if CONFIG_FREERTOS_USE_TICKLESS_IDLE)
 *
 * @param sleep_time_us supplied by the power management framework.
 * For entry callback, sleep_time_us indicates the expected sleep time in us
 * For exit callback, sleep_time_us indicates the actual sleep time in us
 * @param arg is the user provided argument while registering callbacks
 *
 * @return
 *      - ESP_OK allow entry light sleep mode
 */
typedef esp_err_t (*esp_pm_light_sleep_cb_t)(int64_t sleep_time_us, void *arg);

typedef struct {
    /**
     * Callback function defined by internal developers.
     */
    esp_pm_light_sleep_cb_t enter_cb;
    esp_pm_light_sleep_cb_t exit_cb;
    /**
     * Input parameters of callback function defined by internal developers.
     */
    void *enter_cb_user_arg;
    void *exit_cb_user_arg;
    /**
     * Execution priority of callback function defined by internal developers.
     * The smaller the priority, the earlier it executes when call esp_sleep_execute_event_callbacks.
     * If functions have the same priority, the function registered first will be executed first.
     */
    uint32_t enter_cb_prior;
    uint32_t exit_cb_prior;
} esp_pm_sleep_cbs_register_config_t;

/**
 * @brief Register entry or exit callbacks for light sleep (if CONFIG_FREERTOS_USE_TICKLESS_IDLE)
 * @param cbs_conf Config struct containing entry or exit callbacks function and corresponding argument
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the input parameter enter_cb and exit_cb in cbs_conf are NULL
 *      - ESP_ERR_NO_MEM if the remaining memory is insufficient to support malloc
 *      - ESP_FAIL if register the same function repeatedly
 *
 * @note These callback functions are called from IDLE task context hence they cannot call any blocking functions
 */
esp_err_t esp_pm_light_sleep_register_cbs(esp_pm_sleep_cbs_register_config_t *cbs_conf);

/**
 * @brief Unregister entry or exit callbacks for light sleep (if CONFIG_FREERTOS_USE_TICKLESS_IDLE)
 * @param cbs_conf Config struct containing entry or exit callbacks function and corresponding argument
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the input parameter enter_cb and exit_cb in cbs_conf are NULL
 */
esp_err_t esp_pm_light_sleep_unregister_cbs(esp_pm_sleep_cbs_register_config_t *cbs_conf);
#endif

#ifdef __cplusplus
}
#endif
