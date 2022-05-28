/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Brushed DC Motor handle
 */
typedef struct bdc_motor_t *bdc_motor_handle_t;

/**
 * @brief Enable BDC motor
 *
 * @param motor: BDC Motor handle
 *
 * @return
 *      - ESP_OK: Enable motor successfully
 *      - ESP_ERR_INVALID_ARG: Enable motor failed because of invalid parameters
 *      - ESP_FAIL: Enable motor failed because other error occurred
 */
esp_err_t bdc_motor_enable(bdc_motor_handle_t motor);

/**
 * @brief Disable BDC motor
 *
 * @param motor: BDC Motor handle
 *
 * @return
 *      - ESP_OK: Disable motor successfully
 *      - ESP_ERR_INVALID_ARG: Disable motor failed because of invalid parameters
 *      - ESP_FAIL: Disable motor failed because other error occurred
 */
esp_err_t bdc_motor_disable(bdc_motor_handle_t motor);

/**
 * @brief Set speed for bdc motor
 *
 * @param motor: BDC Motor handle
 * @param speed: BDC speed
 *
 * @return
 *      - ESP_OK: Set motor speed successfully
 *      - ESP_ERR_INVALID_ARG: Set motor speed failed because of invalid parameters
 *      - ESP_FAIL: Set motor speed failed because other error occurred
 */
esp_err_t bdc_motor_set_speed(bdc_motor_handle_t motor, uint32_t speed);

/**
 * @brief Forward BDC motor
 *
 * @param motor: BDC Motor handle
 *
 * @return
 *      - ESP_OK: Forward motor successfully
 *      - ESP_FAIL: Forward motor failed because some other error occurred
 */
esp_err_t bdc_motor_forward(bdc_motor_handle_t motor);

/**
 * @brief Reverse BDC Motor
 *
 * @param strip: BDC Motor handle
 *
 * @return
 *      - ESP_OK: Reverse motor successfully
 *      - ESP_FAIL: Reverse motor failed because some other error occurred
 */
esp_err_t bdc_motor_reverse(bdc_motor_handle_t motor);

/**
 * @brief Stop motor in a coast way (a.k.a Fast Decay)
 *
 * @param motor: BDC Motor handle
 *
 * @return
 *      - ESP_OK: Stop motor successfully
 *      - ESP_FAIL: Stop motor failed because some other error occurred
 */
esp_err_t bdc_motor_coast(bdc_motor_handle_t motor);

/**
 * @brief Stop motor in a brake way (a.k.a Slow Decay)
 *
 * @param motor: BDC Motor handle
 *
 * @return
 *      - ESP_OK: Stop motor successfully
 *      - ESP_FAIL: Stop motor failed because some other error occurred
 */
esp_err_t bdc_motor_brake(bdc_motor_handle_t motor);

/**
 * @brief Free BDC Motor resources
 *
 * @param strip: BDC Motor handle
 *
 * @return
 *      - ESP_OK: Free resources successfully
 *      - ESP_FAIL: Free resources failed because error occurred
 */
esp_err_t bdc_motor_del(bdc_motor_handle_t motor);

/**
 * @brief BDC Motor Configuration
 */
typedef struct {
    uint32_t pwma_gpio_num; /*!< BDC Motor PWM A gpio number */
    uint32_t pwmb_gpio_num; /*!< BDC Motor PWM B gpio number */
    uint32_t pwm_freq_hz;   /*!< PWM frequency, in Hz */
} bdc_motor_config_t;

/**
 * @brief BDC Motor MCPWM specific configuration
 */
typedef struct {
    int group_id;           /*!< MCPWM group number */
    uint32_t resolution_hz; /*!< MCPWM timer resolution */
} bdc_motor_mcpwm_config_t;

/**
 * @brief Create BDC Motor based on MCPWM peripheral
 *
 * @param motor_config: BDC Motor configuration
 * @param mcpwm_config: MCPWM specific configuration
 * @param ret_motor Returned BDC Motor handle
 * @return
 *      - ESP_OK: Create BDC Motor handle successfully
 *      - ESP_ERR_INVALID_ARG: Create BDC Motor handle failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create BDC Motor handle failed because of out of memory
 *      - ESP_FAIL: Create BDC Motor handle failed because some other error
 */
esp_err_t bdc_motor_new_mcpwm_device(const bdc_motor_config_t *motor_config, const bdc_motor_mcpwm_config_t *mcpwm_config, bdc_motor_handle_t *ret_motor);

#ifdef __cplusplus
}
#endif
