/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "esp_check.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "wiegand_io.h"

static const char *TAG = "wiegand_io";

typedef struct wiegand_interface_t {
    SemaphoreHandle_t sem;
    gptimer_handle_t interval_timer;
    gptimer_handle_t pulse_timer;
    gpio_num_t data_pin0;
    gpio_num_t data_pin1;
    uint8_t pulse_width_us;
    uint16_t interval_width_us;
    uint64_t bit_mask;
    uint64_t *current_data;
} wiegand_interface_t;

static bool IRAM_ATTR interval_timer_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    wiegand_io_handle_t wiegand_io = (wiegand_io_handle_t)user_data;
    if (wiegand_io->bit_mask == 1) {
        gptimer_stop(timer);
    }

    gptimer_set_raw_count(wiegand_io->pulse_timer, 0);
    gptimer_start(wiegand_io->pulse_timer);

    if (*wiegand_io->current_data & wiegand_io->bit_mask) {
        gpio_set_level(wiegand_io->data_pin1, 0);
    } else {
        gpio_set_level(wiegand_io->data_pin0, 0);
    }

    return false;
}

static bool IRAM_ATTR pulse_timer_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t task_woken = pdFALSE;
    wiegand_io_handle_t wiegand_io = (wiegand_io_handle_t)user_data;
    gpio_set_level(wiegand_io->data_pin0, 1);
    gpio_set_level(wiegand_io->data_pin1, 1);

    gptimer_stop(timer);

    if (!(wiegand_io->bit_mask >>= 1)) {
        xSemaphoreGiveFromISR(wiegand_io->sem, &task_woken);
    }

    return task_woken == pdTRUE;
}

static esp_err_t set_alarm_action(gptimer_handle_t timer, uint64_t count, bool reload)
{
    gptimer_alarm_config_t alarm_config  = {
        .reload_count = 0,
        .alarm_count = count,
        .flags.auto_reload_on_alarm = reload,
    };
    ESP_RETURN_ON_ERROR(gptimer_set_alarm_action(timer, &alarm_config), TAG, "set alarm action failed!");

    return ESP_OK;
}

static esp_err_t setup_timer(gptimer_handle_t *timer, gptimer_alarm_cb_t alarm_cb, wiegand_io_handle_t wiegand_io, uint64_t count, bool reload)
{
    esp_err_t ret = ESP_OK;
    gptimer_config_t timer_config  = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1 tick = 1us
    };
    ESP_RETURN_ON_ERROR(gptimer_new_timer(&timer_config, timer), TAG, "add new timer failed!");

    gptimer_event_callbacks_t cbs = {
        .on_alarm = alarm_cb,
    };
    ESP_GOTO_ON_ERROR(gptimer_register_event_callbacks(*timer, &cbs, wiegand_io), err, TAG, "register callbacks failed!");
    ESP_GOTO_ON_ERROR(set_alarm_action(*timer, count, reload), err, TAG, "set alarm action failed!");
    ESP_GOTO_ON_ERROR(gptimer_enable(*timer), err, TAG, "enable timer failed!");

    return ESP_OK;

err:
    if (*timer) {
        gptimer_del_timer(*timer);
    }
    return ret;
}

esp_err_t wiegand_new_io(const wiegand_io_config_t *config, wiegand_io_handle_t *ret_handle)
{
    esp_err_t ret = ESP_OK;
    wiegand_interface_t *wiegand_io = NULL;
    ESP_RETURN_ON_FALSE(config && ret_handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    wiegand_io = heap_caps_calloc(1, sizeof(wiegand_interface_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    ESP_RETURN_ON_FALSE(wiegand_io, ESP_ERR_NO_MEM, TAG, "no mem for wiegand_io");

    wiegand_io->sem = xSemaphoreCreateBinaryWithCaps(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    wiegand_io->data_pin0 = config->data_pin0;
    wiegand_io->data_pin1 = config->data_pin1;
    wiegand_io->pulse_width_us = config->pulse_width_us;
    wiegand_io->interval_width_us = config->interval_width_us;

    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << wiegand_io->data_pin0) | (1ULL << wiegand_io->data_pin1),
    };
    ESP_GOTO_ON_ERROR(gpio_config(&io_conf), err, TAG, "gpio config failed!");

    // data pins are high by default
    gpio_set_level(wiegand_io->data_pin0, 1);
    gpio_set_level(wiegand_io->data_pin1, 1);

    // Setup the pulse timer
    ESP_GOTO_ON_ERROR(setup_timer(&wiegand_io->pulse_timer, pulse_timer_cb, wiegand_io, wiegand_io->pulse_width_us, false), err, TAG, "setup pulse timer failed!");
    // Setup the interval timer
    ESP_GOTO_ON_ERROR(setup_timer(&wiegand_io->interval_timer, interval_timer_cb, wiegand_io, wiegand_io->interval_width_us, true), err, TAG, "setup interval timer failed!");

    // return handle
    *ret_handle = wiegand_io;
    xSemaphoreGive(wiegand_io->sem);

    return ESP_OK;

err:
    if (wiegand_io->pulse_timer) {
        gptimer_disable(wiegand_io->pulse_timer);
        gptimer_del_timer(wiegand_io->pulse_timer);
    }
    if (wiegand_io->interval_timer) {
        gptimer_disable(wiegand_io->interval_timer);
        gptimer_del_timer(wiegand_io->interval_timer);
    }
    if (wiegand_io) {
        free(wiegand_io);
    }
    return ret;

}

esp_err_t wiegand_io_send(wiegand_io_handle_t wiegand_io, void *data, uint8_t num_of_bits)
{
    ESP_RETURN_ON_FALSE(wiegand_io && num_of_bits && data, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(num_of_bits > 0 && num_of_bits <= 64, ESP_ERR_INVALID_ARG, TAG, "bits is out of range [1,64]");

    xSemaphoreTake(wiegand_io->sem, portMAX_DELAY);
    wiegand_io->current_data = (uint64_t *)data;
    wiegand_io->bit_mask = 1ULL << (num_of_bits - 1);

    ESP_RETURN_ON_ERROR(gptimer_set_raw_count(wiegand_io->interval_timer, 0), TAG, "set timer raw count failed!");
    ESP_RETURN_ON_ERROR(gptimer_start(wiegand_io->interval_timer), TAG, "start timer failed!");

    return ESP_OK;
}
