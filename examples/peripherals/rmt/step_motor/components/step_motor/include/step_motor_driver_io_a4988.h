/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include "esp_err.h"
#include "step_motor_driver_io.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A4988 configuration
 */
typedef struct step_motor_io_a4988_conf_s {
    gpio_num_t direction_pin;
    gpio_num_t sleep_pin;
    gpio_num_t reset_pin;
    gpio_num_t ms3_pin;
    gpio_num_t ms2_pin;
    gpio_num_t ms1_pin;
    gpio_num_t enable_pin;
} step_motor_io_a4988_conf_t;

/**
 * @brief A4988 low part driver handle
 */
typedef struct step_motor_driver_io_a4988_s {
    step_motor_driver_io_t base;
    step_motor_io_a4988_conf_t conf;
} step_motor_driver_io_a4988_t;

/**
 * @brief create an A4988 driver handle
 */
esp_err_t step_motor_new_a4988_io_driver(const step_motor_io_a4988_conf_t *conf, step_motor_driver_io_handle_t *handle);

/**
 * @brief delete an A4988 driver handle
 */
esp_err_t step_motor_delete_a4988_io_driver(step_motor_driver_io_handle_t handle);

#ifdef __cplusplus
}
#endif
