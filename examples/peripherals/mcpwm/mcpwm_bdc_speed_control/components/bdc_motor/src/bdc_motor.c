/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "esp_check.h"
#include "bdc_motor.h"
#include "bdc_motor_interface.h"

static const char *TAG = "bdc_motor";

esp_err_t bdc_motor_enable(bdc_motor_handle_t motor)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return motor->enable(motor);
}

esp_err_t bdc_motor_disable(bdc_motor_handle_t motor)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return motor->disable(motor);
}

esp_err_t bdc_motor_set_speed(bdc_motor_handle_t motor, uint32_t speed)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return motor->set_speed(motor, speed);
}

esp_err_t bdc_motor_forward(bdc_motor_handle_t motor)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return motor->forward(motor);
}

esp_err_t bdc_motor_reverse(bdc_motor_handle_t motor)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return motor->reverse(motor);
}

esp_err_t bdc_motor_coast(bdc_motor_handle_t motor)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return motor->coast(motor);
}

esp_err_t bdc_motor_brake(bdc_motor_handle_t motor)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return motor->brake(motor);
}

esp_err_t bdc_motor_del(bdc_motor_handle_t motor)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return motor->del(motor);
}
