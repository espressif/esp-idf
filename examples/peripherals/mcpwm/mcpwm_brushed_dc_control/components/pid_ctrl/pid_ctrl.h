/* General Purpose PID example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"

#define LOCATION_PID_DEFAULT_CONFIG(p, i, d, max_o, min_o, max_i, min_i) \
    {                                   \
        .Kp =  p,                       \
        .Ki =  i,                       \
        .Kd =  d,                       \
        .max_output_limit   =  max_o,   \
        .min_output_limit   =  min_o,   \
        .max_integral_limit =  max_i,   \
        .min_integral_limit =  min_i,   \
        .type = PID_LOCATION                \
    }

#define INCREMENT_PID_DEFAULT_CONFIG(p, i, d, max_o, min_o, max_i, min_i, init_out) \
    {                                   \
        .Kp =  p,                       \
        .Ki =  i,                       \
        .Kd =  d,                       \
        .max_output_limit   =  max_o,   \
        .min_output_limit   =  min_o,   \
        .max_integral_limit =  max_i,   \
        .min_integral_limit =  min_i,   \
        .last_output = init_out,        \
        .type = PID_INCREMENT               \
    }

typedef enum {
    PID_INCREMENT = 0,
    PID_LOCATION
} pid_calculate_type_e;

typedef struct pid_ctrl pid_ctrl_t;

struct pid_ctrl{
    /* pid parameters (Set by user) */
    float Kp;                       // PID Kp value
    float Ki;                       // PID Ki value
    float Kd;                       // PID Kd value

    /* history variables */
    float previous_err1;            // e(k-1)
    float previous_err2;            // e(k-2)
    float integral_err;             // Sum of error
    float last_output;              // PID output in last control period

    /* limitation */
    float max_output_limit;         // PID maximum output limitation
    float min_output_limit;         // PID minimum output limitation
    float max_integral_limit;       // PID maximum integral value limitation
    float min_integral_limit;       // PID minimum integral value limitation

    /* PID calculation function type (Set by user) */
    pid_calculate_type_e type;      // PID calculation type

    /* PID calculation function */
    float (*calculate_func)(pid_ctrl_t *pid, float error);  // PID calculation function pointer
};

/**
 * @brief PID initialization
 *
 * @param Kp PID kp value
 * @param Ki PID ki value
 * @param Kd PID kd value
 * @param type PID calculation type
 * @param pid Secondary poiter of pid control struct
 * @return
 *      - ESP_OK: PID initialized successfully
 *      - ESP_ERR_INVALID_ARG: The secondary pointer is NULL
 *      - ESP_ERR_NO_MEM: There is no memory for PID structure
 */
esp_err_t pid_init(float Kp, float Ki, float Kd, pid_calculate_type_e type, pid_ctrl_t **pid);

/**
 * @brief PID deinitialization
 *
 * @param pid Secondary poiter of pid control struct
 */
void pid_deinit(pid_ctrl_t **pid);


/**
 * @brief
 *
 * @param pid The pointer of pid control struct
 * @return
 *      - ESP_OK: PID integral value cleared successfully
 *      - ESP_ERR_INVALID_ARG: The pointer is NULL
 */
esp_err_t pid_integral_clear(pid_ctrl_t *pid);

#ifdef __cplusplus
}
#endif
