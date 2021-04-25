/* General Purpose PID example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "esp_check.h"
#include "pid_ctrl.h"

static const char *TAG = "pid_ctrl";

static float pid_calc_location(pid_ctrl_t *pid, float error)
{
    float output = 0;
    /* Add current error to the integral error */
    pid->integral_err += error;
    /* If the integral error is out of the range, it will be limited */
    pid->integral_err = pid->integral_err > pid->max_integral_limit ? pid->max_integral_limit : pid->integral_err;
    pid->integral_err = pid->integral_err < pid->min_integral_limit ? pid->min_integral_limit : pid->integral_err;

    /* Calculate the pid control value by location formula */
    /* u(k) = e(k)*Kp + (e(k)-e(k-1))*Kd + integral*Ki */
    output = error * pid->Kp +
        (error - pid->previous_err1) * pid->Kd +
        pid->integral_err * pid->Ki;

    /* If the output is out of the range, it will be limited */
    output = output > pid->max_output_limit ? pid->max_output_limit : output;
    output = output < pid->min_output_limit ? pid->min_output_limit : output;

    /* Update previous error */
    pid->previous_err1 = error;

    return output;
}

static float pid_calc_increment(pid_ctrl_t *pid, float error)
{
    float output = 0;

    /* Calculate the pid control value by increment formula */
    /* du(k) = (e(k)-e(k-1))*Kp + (e(k)-2*e(k-1)+e(k-2))*Kd + e(k)*Ki */
    /* u(k) = du(k) + u(k-1) */
    output = (error-pid->previous_err1)*pid->Kp +
        (error-2*pid->previous_err1+pid->previous_err2)*pid->Kd +
        error*pid->Ki +
        pid->last_output;

    /* If the output is beyond the range, it will be limited */
    output = output > pid->max_output_limit ? pid->max_output_limit : output;
    output = output < pid->min_output_limit ? pid->min_output_limit : output;

    /* Update previous error */
    pid->previous_err2 = pid->previous_err1;
    pid->previous_err1 = error;

    /* Update last output */
    pid->last_output = output;

    return output;
}

esp_err_t pid_integral_clear(pid_ctrl_t *pid)
{
    esp_err_t ret;
     /* Check the input pointer */
    ESP_GOTO_ON_FALSE(pid, ESP_ERR_INVALID_ARG, err, TAG, "Input a NULL pointer\n");

    /* Clear the integral error in pid structure */
    pid->integral_err = 0;
err:
    return ret;
}

esp_err_t pid_init(float Kp, float Ki, float Kd, pid_calculate_type_e type, pid_ctrl_t **pid)
{
    esp_err_t ret = ESP_OK;
    /* Check the input pointer */
    ESP_GOTO_ON_FALSE(pid, ESP_ERR_INVALID_ARG, err, TAG, "Input a NULL pointer\n");

    *pid = calloc(1, sizeof(pid_ctrl_t));
    ESP_GOTO_ON_FALSE((*pid), ESP_ERR_NO_MEM, err, TAG, "There is no memory for PID structure\n");

    /* Set the PID parameters */
    (*pid)->Kp = Kp;
    (*pid)->Ki = Ki;
    (*pid)->Kd = Kd;

    /* Select the PID work mode */
    (*pid)->type = type;

    /* Initialize the pid history variables */
    (*pid)->previous_err1 = 0;
    (*pid)->previous_err2 = 0;
    (*pid)->integral_err  = 0;
    (*pid)->last_output   = 0;

    /* Set pid default limitation */
    (*pid)->max_output_limit   = 100;
    (*pid)->min_output_limit   = -100;
    (*pid)->max_integral_limit = 1000;
    (*pid)->max_integral_limit = -1000;

    /* Set the calculate function according to the PID type */
    (*pid)->calculate_func =
        (*pid)->type == PID_INCREMENT ? pid_calc_increment : pid_calc_location;

err:
    return ret;
}

void pid_deinit(pid_ctrl_t **pid)
{
    if((*pid) != NULL)
    {
        free(*pid);
        *pid = NULL;
    }
}
