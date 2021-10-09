/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#include "hal/gpio_types.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "step_motor_driver_io_a4988.h"

static const char *TAG = "A4988_IO";

#define A4988_RESPONSE_DELAY_MS 10

static esp_err_t a4988_init(step_motor_driver_io_t *handle)
{
    step_motor_driver_io_a4988_t *a4988_motor = __containerof(handle, step_motor_driver_io_a4988_t, base);
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = BIT64(a4988_motor->conf.direction_pin) |
                           BIT64(a4988_motor->conf.sleep_pin) |
                           BIT64(a4988_motor->conf.reset_pin) |
                           BIT64(a4988_motor->conf.ms3_pin) |
                           BIT64(a4988_motor->conf.ms2_pin) |
                           BIT64(a4988_motor->conf.ms1_pin) |
                           BIT64(a4988_motor->conf.enable_pin);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.direction_pin, 0));
    ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.sleep_pin, 0));    // default sleep
    ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.reset_pin, 0));    // keep reset
    ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms3_pin, 0));      // 1/1 phase
    ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms2_pin, 0));
    ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms1_pin, 0));
    ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.enable_pin, 1));   // disable by default
    vTaskDelay(pdMS_TO_TICKS(A4988_RESPONSE_DELAY_MS));
    ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.reset_pin, 1));
    ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.sleep_pin, 1));
    ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.enable_pin, 0));
    vTaskDelay(pdMS_TO_TICKS(A4988_RESPONSE_DELAY_MS));
    return ESP_OK;
}

static esp_err_t a4988_set_direction(step_motor_driver_io_t *handle, step_direction direction)
{
    step_motor_driver_io_a4988_t *a4988_motor = __containerof(handle, step_motor_driver_io_a4988_t, base);
    ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.direction_pin, direction));
    return ESP_OK;
}

static esp_err_t a4988_enable_sleep(step_motor_driver_io_t *handle, bool enabled)
{
    step_motor_driver_io_a4988_t *a4988_motor = __containerof(handle, step_motor_driver_io_a4988_t, base);
    ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.sleep_pin, enabled));
    return ESP_OK;
}

static esp_err_t a4988_enable_output(step_motor_driver_io_t *handle, bool enabled)
{
    step_motor_driver_io_a4988_t *a4988_motor = __containerof(handle, step_motor_driver_io_a4988_t, base);
    ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.enable_pin, enabled));
    return ESP_OK;
}

static esp_err_t a4988_set_microstep(step_motor_driver_io_t *handle, uint16_t microstep)
{
    step_motor_driver_io_a4988_t *a4988_motor = __containerof(handle, step_motor_driver_io_a4988_t, base);
    switch (microstep) {
    case 1:
        ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms3_pin, 0));
        ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms2_pin, 0));
        ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms1_pin, 0));
        break;
    case 2:
        ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms3_pin, 0));
        ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms2_pin, 0));
        ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms1_pin, 1));
        break;
    case 4:
        ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms3_pin, 0));
        ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms2_pin, 1));
        ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms1_pin, 0));
        break;
    case 8:
        ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms3_pin, 0));
        ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms2_pin, 1));
        ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms1_pin, 1));
        break;
    case 16:
        ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms3_pin, 1));
        ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms2_pin, 1));
        ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.ms1_pin, 1));
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_OK;
}

static esp_err_t a4988_reset(step_motor_driver_io_t *handle)
{
    step_motor_driver_io_a4988_t *a4988_motor = __containerof(handle, step_motor_driver_io_a4988_t, base);
    ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.reset_pin, 0));
    vTaskDelay(pdMS_TO_TICKS(A4988_RESPONSE_DELAY_MS));
    ESP_ERROR_CHECK(gpio_set_level(a4988_motor->conf.reset_pin, 1));
    return ESP_OK;
}

static esp_err_t a4988_deinit(step_motor_driver_io_t *handle)
{
    step_motor_driver_io_a4988_t *a4988_motor = __containerof(handle, step_motor_driver_io_a4988_t, base);
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = BIT64(a4988_motor->conf.direction_pin) |
                           BIT64(a4988_motor->conf.sleep_pin) |
                           BIT64(a4988_motor->conf.reset_pin) |
                           BIT64(a4988_motor->conf.ms3_pin) |
                           BIT64(a4988_motor->conf.ms2_pin) |
                           BIT64(a4988_motor->conf.ms1_pin) |
                           BIT64(a4988_motor->conf.enable_pin);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    return ESP_OK;
}

esp_err_t step_motor_new_a4988_io_driver(const step_motor_io_a4988_conf_t *conf, step_motor_driver_io_handle_t *handle)
{
    esp_err_t ret = ESP_OK;
    step_motor_driver_io_a4988_t *a4988 = NULL;
    ESP_GOTO_ON_FALSE(conf, ESP_ERR_INVALID_ARG, err, TAG, "configuration can't be null");
    ESP_GOTO_ON_FALSE(handle, ESP_ERR_INVALID_ARG, err, TAG, "can't assign handle to null");

    a4988 = calloc(1, sizeof(step_motor_driver_io_a4988_t));
    ESP_GOTO_ON_FALSE(a4988, ESP_ERR_NO_MEM, err, TAG, "allocate context memory failed");
    memcpy(&a4988->conf, conf, sizeof(step_motor_io_a4988_conf_t));

    a4988->base.init = a4988_init;
    a4988->base.deinit = a4988_deinit;
    a4988->base.set_direction = a4988_set_direction;
    a4988->base.set_microstep = a4988_set_microstep;
    a4988->base.enable_sleep = a4988_enable_sleep;
    a4988->base.enable_output = a4988_enable_output;
    a4988->base.trigger_reset = a4988_reset;
    a4988->base.step_triggered_edge = 1;
    a4988->base.pulse_high_period_us = 1;
    a4988->base.pulse_low_period_us = 1;

    *handle = &(a4988->base);
    return ESP_OK;

err:
    if (a4988) {
        free(a4988);
    }
    return ret;
}

esp_err_t step_motor_delete_a4988_io_driver(step_motor_driver_io_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_STATE, TAG, "empty handle");
    step_motor_driver_io_a4988_t *a4988 = __containerof(handle, step_motor_driver_io_a4988_t, base);
    free(a4988);
    return ESP_OK;
}
