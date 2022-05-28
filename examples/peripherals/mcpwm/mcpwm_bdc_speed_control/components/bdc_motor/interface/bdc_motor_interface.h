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

typedef struct bdc_motor_t bdc_motor_t; /*!< Type of BDC motor */

/**
 * @brief BDC motor interface definition
 */
struct bdc_motor_t {
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
    esp_err_t (*enable)(bdc_motor_t *motor);

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
    esp_err_t (*disable)(bdc_motor_t *motor);

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
    esp_err_t (*set_speed)(bdc_motor_t *motor, uint32_t speed);

    /**
     * @brief Forward BDC motor
     *
     * @param motor: BDC Motor handle
     *
     * @return
     *      - ESP_OK: Forward motor successfully
     *      - ESP_FAIL: Forward motor failed because some other error occurred
     */
    esp_err_t (*forward)(bdc_motor_t *motor);

    /**
     * @brief Reverse BDC Motor
     *
     * @param motor: BDC Motor handle
     *
     * @return
     *      - ESP_OK: Reverse motor successfully
     *      - ESP_FAIL: Reverse motor failed because some other error occurred
     */
    esp_err_t (*reverse)(bdc_motor_t *motor);

    /**
     * @brief Stop motor in a coast way (a.k.a Fast Decay)
     *
     * @param motor: BDC Motor handle
     *
     * @return
     *      - ESP_OK: Stop motor successfully
     *      - ESP_FAIL: Stop motor failed because some other error occurred
     */
    esp_err_t (*coast)(bdc_motor_t *motor);

    /**
     * @brief Stop motor in a brake way (a.k.a Slow Decay)
     *
     * @param motor: BDC Motor handle
     *
     * @return
     *      - ESP_OK: Stop motor successfully
     *      - ESP_FAIL: Stop motor failed because some other error occurred
     */
    esp_err_t (*brake)(bdc_motor_t *motor);

    /**
     * @brief Free BDC Motor handle resources
     *
     * @param motor: BDC Motor handle
     *
     * @return
     *      - ESP_OK: Free resources successfully
     *      - ESP_FAIL: Free resources failed because error occurred
     */
    esp_err_t (*del)(bdc_motor_t *motor);
};

#ifdef __cplusplus
}
#endif
