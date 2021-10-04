/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct step_motor_driver_io_s step_motor_driver_io_t;

typedef step_motor_driver_io_t *step_motor_driver_io_handle_t;

typedef enum {
    STEP_MOTOR_DIRECTION_NEGATIVE = 0, STEP_MOTOR_DIRECTION_POSITIVE
} step_direction;

/**
 * @brief init low part of driver
 * GPIO configuration, Bus initializing...
 */
typedef esp_err_t (*step_motor_driver_io_init)(step_motor_driver_io_t *handle);
/**
 * @brief set rotation direction
 */
typedef esp_err_t (*step_motor_driver_io_set_direction)(step_motor_driver_io_t *handle, step_direction direction);
/**
 * @brief enable/disable sleep mode if supported
 */
typedef esp_err_t (*step_motor_driver_io_enable_sleep)(step_motor_driver_io_t *handle, bool enabled);
/**
 * @brief enable/disable output if supported
 */
typedef esp_err_t (*step_motor_driver_io_enable_output)(step_motor_driver_io_t *handle, bool enabled);
/**
 * @brief set microstep configuration if supported.
 * param microstep is treated as denominator. a input of 16 means 1/16 step
 * should return ESP_ERR_NOT_SUPPORTED if not supported
 */
typedef esp_err_t (*step_motor_driver_io_set_microstep)(step_motor_driver_io_t *handle, uint16_t microstep);
/**
 * @brief reset low part of driver
 */
typedef esp_err_t (*step_motor_driver_io_reset)(step_motor_driver_io_t *handle);
/**
 * @brief deinit low part of driver
 */
typedef esp_err_t (*step_motor_driver_io_deinit)(step_motor_driver_io_t *handle);

/**
 * @brief Driver IC specified control logic
 *
 * leave callback pointer NULL if action is not supported
 */
struct step_motor_driver_io_s {
    step_motor_driver_io_init init;                     /*!< callback to init low part driver */
    step_motor_driver_io_set_direction set_direction;   /*!< callback to set rotate direction */
    step_motor_driver_io_enable_sleep enable_sleep;     /*!< callback to enable sleep mode */
    step_motor_driver_io_enable_output enable_output;   /*!< callback to enable output */
    step_motor_driver_io_set_microstep set_microstep;   /*!< callback to set microstep configuration */
    bool step_triggered_edge;                           /*!< true if step is triggered by positive edge, otherwise false */
    uint32_t pulse_low_period_us;                       /*!< minimum low level pulse width on step pin */
    uint32_t pulse_high_period_us;                      /*!< minimum high level pulse width on step pin */
    step_motor_driver_io_reset trigger_reset;           /*!< callback to trigger a reset on low part driver */
    step_motor_driver_io_deinit deinit;                 /*!< callback to deinit low part driver */
};

#ifdef __cplusplus
}
#endif
