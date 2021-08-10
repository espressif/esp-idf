/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <sys/param.h>
#include "esp_check.h"
#include "esp_log.h"
#include "pid_ctrl.h"

static const char *TAG = "pid_ctrl";

typedef struct pid_ctrl_block_t pid_ctrl_block_t;
typedef float (*pid_cal_func_t)(pid_ctrl_block_t *pid, float error);

struct pid_ctrl_block_t {
    float Kp; // PID Kp value
    float Ki; // PID Ki value
    float Kd; // PID Kd value
    float previous_err1; // e(k-1)
    float previous_err2; // e(k-2)
    float integral_err;  // Sum of error
    float last_output;  // PID output in last control period
    float max_output;   // PID maximum output limitation
    float min_output;   // PID minimum output limitation
    float max_integral; // PID maximum integral value limitation
    float min_integral; // PID minimum integral value limitation
    pid_cal_func_t calculate_func; // calculation function, depends on actual PID type set by user
};

static float pid_calc_positional(pid_ctrl_block_t *pid, float error)
{
    float output = 0;
    /* Add current error to the integral error */
    pid->integral_err += error;
    /* If the integral error is out of the range, it will be limited */
    pid->integral_err = MIN(pid->integral_err, pid->max_integral);
    pid->integral_err = MAX(pid->integral_err, pid->min_integral);

    /* Calculate the pid control value by location formula */
    /* u(k) = e(k)*Kp + (e(k)-e(k-1))*Kd + integral*Ki */
    output = error * pid->Kp +
             (error - pid->previous_err1) * pid->Kd +
             pid->integral_err * pid->Ki;

    /* If the output is out of the range, it will be limited */
    output = MIN(output, pid->max_output);
    output = MAX(output, pid->min_output);

    /* Update previous error */
    pid->previous_err1 = error;

    return output;
}

static float pid_calc_incremental(pid_ctrl_block_t *pid, float error)
{
    float output = 0;

    /* Calculate the pid control value by increment formula */
    /* du(k) = (e(k)-e(k-1))*Kp + (e(k)-2*e(k-1)+e(k-2))*Kd + e(k)*Ki */
    /* u(k) = du(k) + u(k-1) */
    output = (error - pid->previous_err1) * pid->Kp +
             (error - 2 * pid->previous_err1 + pid->previous_err2) * pid->Kd +
             error * pid->Ki +
             pid->last_output;

    /* If the output is beyond the range, it will be limited */
    output = MIN(output, pid->max_output);
    output = MAX(output, pid->min_output);

    /* Update previous error */
    pid->previous_err2 = pid->previous_err1;
    pid->previous_err1 = error;

    /* Update last output */
    pid->last_output = output;

    return output;
}

esp_err_t pid_new_control_block(const pid_ctrl_config_t *config, pid_ctrl_block_handle_t *ret_pid)
{
    esp_err_t ret = ESP_OK;
    pid_ctrl_block_t *pid = NULL;
    /* Check the input pointer */
    ESP_GOTO_ON_FALSE(config && ret_pid, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    pid = calloc(1, sizeof(pid_ctrl_block_t));
    ESP_GOTO_ON_FALSE(pid, ESP_ERR_NO_MEM, err, TAG, "no mem for PID control block");
    ESP_GOTO_ON_ERROR(pid_update_parameters(pid, &config->init_param), err, TAG, "init PID parameters failed");
    *ret_pid = pid;
    return ret;

err:
    if (pid) {
        free(pid);
    }
    return ret;
}

esp_err_t pid_del_control_block(pid_ctrl_block_handle_t pid)
{
    ESP_RETURN_ON_FALSE(pid, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    free(pid);
    return ESP_OK;
}

esp_err_t pid_compute(pid_ctrl_block_handle_t pid, float input_error, float *ret_result)
{
    ESP_RETURN_ON_FALSE(pid && ret_result, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    *ret_result = pid->calculate_func(pid, input_error);
    return ESP_OK;
}

esp_err_t pid_update_parameters(pid_ctrl_block_handle_t pid, const pid_ctrl_parameter_t *params)
{
    ESP_RETURN_ON_FALSE(pid && params, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    pid->Kp = params->kp;
    pid->Ki = params->ki;
    pid->Kd = params->kd;
    pid->max_output = params->max_output;
    pid->min_output = params->min_output;
    pid->max_integral = params->max_integral;
    pid->min_integral = params->min_integral;
    /* Set the calculate function according to the PID type */
    switch (params->cal_type) {
    case PID_CAL_TYPE_INCREMENTAL:
        pid->calculate_func = pid_calc_incremental;
        break;
    case PID_CAL_TYPE_POSITIONAL:
        pid->calculate_func = pid_calc_positional;
        break;
    default:
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_ARG, TAG, "invalid PID calculation type:%d", params->cal_type);
    }
    return ESP_OK;
}
