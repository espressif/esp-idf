/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "step_motor.h"

esp_err_t step_motor_init(step_motor_t *handle)
{
    return handle->init(handle);
}

esp_err_t step_motor_deinit(step_motor_t *handle)
{
    return handle->deinit(handle);
}

esp_err_t step_motor_step(step_motor_t *handle, uint32_t n, uint32_t speed)
{
    return handle->step(handle, n, speed);
}

esp_err_t step_motor_smooth_step(step_motor_t *handle, uint32_t n, uint32_t speed_steps, uint32_t speed_min, uint32_t speed_max)
{
    return handle->smooth_step(handle, n, speed_steps, speed_min, speed_max);
}

esp_err_t step_motor_set_step(step_motor_t *handle, uint16_t microstep, bool direction)
{
    return handle->set_step(handle, microstep, direction);
}
