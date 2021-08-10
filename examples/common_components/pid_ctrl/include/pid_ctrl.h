/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief PID calculation type
 *
 */
typedef enum {
    PID_CAL_TYPE_INCREMENTAL, /*!< Incremental PID control */
    PID_CAL_TYPE_POSITIONAL,  /*!< Positional PID control */
} pid_calculate_type_t;

/**
 * @brief Type of PID control block handle
 *
 */
typedef struct pid_ctrl_block_t *pid_ctrl_block_handle_t;

/**
 * @brief PID control parameters
 *
 */
typedef struct {
    float kp;                      // PID Kp parameter
    float ki;                      // PID Ki parameter
    float kd;                      // PID Kd parameter
    float max_output;              // PID maximum output limitation
    float min_output;              // PID minimum output limitation
    float max_integral;            // PID maximum integral value limitation
    float min_integral;            // PID minimum integral value limitation
    pid_calculate_type_t cal_type; // PID calculation type
} pid_ctrl_parameter_t;

/**
 * @brief PID control configuration
 *
 */
typedef struct {
    pid_ctrl_parameter_t init_param; // Initial parameters
} pid_ctrl_config_t;

/**
 * @brief Create a new PID control session, returns the handle of control block
 *
 * @param[in] config PID control configuration
 * @param[out] ret_pid Returned PID control block handle
 * @return
 *      - ESP_OK: Created PID control block successfully
 *      - ESP_ERR_INVALID_ARG: Created PID control block failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Created PID control block failed because out of memory
 */
esp_err_t pid_new_control_block(const pid_ctrl_config_t *config, pid_ctrl_block_handle_t *ret_pid);

/**
 * @brief Delete the PID control block
 *
 * @param[in] pid PID control block handle, created by `pid_new_control_block()`
 * @return
 *      - ESP_OK: Delete PID control block successfully
 *      - ESP_ERR_INVALID_ARG: Delete PID control block failed because of invalid argument
 */
esp_err_t pid_del_control_block(pid_ctrl_block_handle_t pid);

/**
 * @brief Update PID parameters
 *
 * @param[in] pid PID control block handle, created by `pid_new_control_block()`
 * @param[in] params PID parameters
 * @return
 *      - ESP_OK: Update PID parameters successfully
 *      - ESP_ERR_INVALID_ARG: Update PID parameters failed because of invalid argument
 */
esp_err_t pid_update_parameters(pid_ctrl_block_handle_t pid, const pid_ctrl_parameter_t *params);

/**
 * @brief Input error and get PID control result
 *
 * @param[in] pid PID control block handle, created by `pid_new_control_block()`
 * @param[in] input_error error data that feed to the PID controller
 * @param[out] ret_result result after PID calculation
 * @return
 *      - ESP_OK: Run a PID compute successfully
 *      - ESP_ERR_INVALID_ARG: Run a PID compute failed because of invalid argument
 */
esp_err_t pid_compute(pid_ctrl_block_handle_t pid, float input_error, float *ret_result);

#ifdef __cplusplus
}
#endif
