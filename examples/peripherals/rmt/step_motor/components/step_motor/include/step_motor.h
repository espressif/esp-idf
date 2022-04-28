/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include <stdbool.h>
#include "driver/rmt.h"
#include "hal/rmt_types.h"
#include "esp_err.h"
#include "step_motor_driver_io.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of step motor interface
 */
typedef struct step_motor_s step_motor_t;

typedef step_motor_t *step_motor_handle_t;

/**
 * @brief Declaration of step motor interface
 *
 */
struct step_motor_s {
    esp_err_t (*init)(step_motor_t *handle);
    esp_err_t (*deinit)(step_motor_t *handle);
    esp_err_t (*step)(step_motor_t *handle, uint32_t n, uint32_t speed);
    esp_err_t (*smooth_step)(step_motor_t *handle, uint32_t n, uint32_t speed_steps, uint32_t speed_min, uint32_t speed_max);
    esp_err_t (*set_step)(step_motor_t *handle, uint16_t microstep, bool direction);

    // TODO: other API like sleep, enable_output, reset
};

/**
 * @brief Initialize step motor driver
 *
 * @param handle driver handle
 * @return
 *      - ESP_OK: successfully initialized
 *      - ESP_ERR_INVALID_ARG: wrong parameter
 */
esp_err_t step_motor_init(step_motor_t *handle);

/**
 * @brief Deinitialize driver
 *
 * @param handle driver handle
 * @return
 *      - ESP_OK: Stop playing successfully
 */
esp_err_t step_motor_deinit(step_motor_t *handle);

/**
 * @brief Move n small steps.
 *
 * @note Will block until finish if n is finite steps. But will immediately return if n is UINT32_MAX.
 *
 * @param handle driver handle
 * @param n step count, UINT32_MAX for unlimited, 0 to stop
 * @param speed steps per second
 * @return
 *      - ESP_OK: Recycle memory successfully
 */
esp_err_t step_motor_step(step_motor_t *handle, uint32_t n, uint32_t speed);

/**
 * @brief Move n small steps. Always blocking and take smooth arguments
 *
 *  ^ speed (steps/s)
 *  |                         *********************                       <---- speed_max
 *  |                   *     |                   |     *
 *  |               *         |                   |         *
 *  |             *           |                   |           *
 *  |            *            |                   |            *
 *  |          *     speed    |  n-speed_steps*2  |    speed     *
 *  |      *         steps    |                   |    steps         *    <---- speed_min
 *  |                         |                   |
 *  +-------------------------------------------------------------------> timestamp (s)
 *
 * @param handle driver handle
 * @param n steps
 * @param speed_steps number of sample points during speed smoothing
 * @param speed_min minimal speed, steps per seconds
 * @param speed_max maximum speed, steps per seconds
 * @note may consume lots of ram depending on speed_steps with current implementation (1000 will lead to 8kb of ram usage)
 * @return
 *      - ESP_OK: Recycle memory successfully
 */
esp_err_t step_motor_smooth_step(step_motor_t *handle, uint32_t n, uint32_t speed_steps, uint32_t speed_min, uint32_t speed_max);

/**
 * @brief Set microstep resolution
 *
 * @param handle driver handle
 * @param step_config microstep resolution
 * @param direction rotating direction
 * @return
 *      - ESP_OK: Recycle memory successfully
 */
esp_err_t step_motor_set_step(step_motor_t *handle, uint16_t microstep, bool direction);


// TODO: move out of this header to rmt one (like step_motor_rmt.h)
/**
 * @brief Create step motor instance based on RMT driver
 *
 * @param[in] io_driver step motor low part driver
 * @param[out] ret_handle returned handle of step motor instance
 * @return
 *      - ESP_OK: create step motor instance successfully
 *      - ESP_ERR_INVALID_ARG: wrong parameter
 *      - ESP_ERR_NO_MEM: no memory to allocate instance
 */
esp_err_t step_motor_create_rmt(step_motor_driver_io_t *io_driver, const rmt_config_t *rmt_conf, step_motor_handle_t *ret_handle);

/**
 * @brief Delete step motor instance that previously created
 *
 * @param[in] handle step motor instance to be deleted
 * @return
 *      - ESP_OK: create step motor instance successfully
 *      - ESP_ERR_INVALID_ARG: wrong parameter
 */
esp_err_t step_motor_delete_rmt(step_motor_handle_t handle);

#ifdef __cplusplus
}
#endif
