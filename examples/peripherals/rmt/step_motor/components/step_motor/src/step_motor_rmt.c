/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

/*
 * This file contains an implementation of step motor middleware based on rmt peripheral
 */

#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/rmt.h"
#include "step_motor.h"

static const char *TAG = "RMT_STEP_MOTOR";

typedef enum {
    STOPPED = 0,
    SMOOTH_SPEED_UP,
    SMOOTH_KEEP_SPEED,
    SMOOTH_SLOW_DOWN,
    UNLIMITED_LOOP,
    LIMITED_LOOP,
} rmt_step_motor_running_status;

typedef struct {
    step_motor_t base;
    step_motor_driver_io_t *io_driver;
    rmt_channel_t rmt_ch;
    rmt_step_motor_running_status status;
    rmt_item32_t rmt_items_loop;
    uint32_t rmt_items_loop_count;
    rmt_item32_t *rmt_items_speedup;
    rmt_item32_t *rmt_items_speeddown;
    uint32_t rmt_items_smoothstep_count;
    SemaphoreHandle_t notify_semphr;
} rmt_step_motor_t;

static inline float helper_smootherstep_clamp(float x, float lowerlimit, float upperlimit)
{
    if (x < lowerlimit) {
        x = lowerlimit;
    }
    if (x > upperlimit) {
        x = upperlimit;
    }
    return x;
}

// smoothstep formula
// see https://en.wikipedia.org/wiki/Smoothstep
static float helper_smootherstep(float edge0, float edge1, float x)
{
    // Scale, and clamp x to 0..1 range
    x = helper_smootherstep_clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    // Evaluate polynomial
    return x * x * x * (x * (x * 6 - 15) + 10) * (edge1 - edge0) + edge0;
}

static uint16_t helper_speed_to_duration(uint16_t speed)
{
    return (uint16_t) round(1.0 * 1000 * 1000 / speed);
}

static esp_err_t helper_fill_rmt_items(rmt_item32_t *items, uint32_t speed, const step_motor_driver_io_t *io_driver)
{
    items->duration1 = io_driver->step_triggered_edge ? io_driver->pulse_high_period_us : io_driver->pulse_low_period_us;
    items->level1 = io_driver->step_triggered_edge;
    items->level0 = !io_driver->step_triggered_edge;
    uint32_t delay_period = helper_speed_to_duration(speed);
    if (delay_period <= (io_driver->step_triggered_edge ? io_driver->pulse_low_period_us : io_driver->pulse_high_period_us)) {
        ESP_LOGW(TAG, "maximum rate reached, driver will generate another possible highest rate instead");
        items->duration0 = io_driver->step_triggered_edge ? io_driver->pulse_low_period_us : io_driver->pulse_high_period_us;
    } else {
        items->duration0 = delay_period;
    }
    return ESP_OK;
}

static esp_err_t rmt_step_motor_init(step_motor_t *motor)
{
    rmt_step_motor_t *rmt_handle = __containerof(motor, rmt_step_motor_t, base);
    step_motor_driver_io_t *io_driver = rmt_handle->io_driver;
    if (io_driver->init) {
        return io_driver->init(io_driver);
    }
    return ESP_OK;
}

static esp_err_t rmt_step_motor_deinit(step_motor_t *motor)
{
    rmt_step_motor_t *rmt_handle = __containerof(motor, rmt_step_motor_t, base);
    step_motor_driver_io_t *io_driver = rmt_handle->io_driver;
    if (io_driver->deinit) {
        return io_driver->deinit(io_driver);
    }
    return ESP_OK;
}

// assume n != 0 and speed is within considerable range
static esp_err_t rmt_step_motor_step_impl(step_motor_t *motor, uint32_t n, uint32_t speed)
{
    rmt_step_motor_t *rmt_handle = __containerof(motor, rmt_step_motor_t, base);

    ESP_ERROR_CHECK(rmt_set_tx_loop_mode(rmt_handle->rmt_ch, true));
    ESP_ERROR_CHECK(rmt_enable_tx_loop_autostop(rmt_handle->rmt_ch, true));

    rmt_handle->rmt_items_loop_count = n;
    if ((rmt_handle->rmt_items_loop_count) > 1023) {
        (rmt_handle->rmt_items_loop_count) -= 1023;
        ESP_ERROR_CHECK(rmt_set_tx_loop_count(rmt_handle->rmt_ch, 1023));
    } else {
        ESP_ERROR_CHECK(rmt_set_tx_loop_count(rmt_handle->rmt_ch, rmt_handle->rmt_items_loop_count));
        rmt_handle->rmt_items_loop_count = 0;
    }
    helper_fill_rmt_items(&rmt_handle->rmt_items_loop, speed, rmt_handle->io_driver);

    rmt_handle->status = LIMITED_LOOP;

    rmt_write_items(rmt_handle->rmt_ch, &rmt_handle->rmt_items_loop, 1, false);
    xSemaphoreTake(rmt_handle->notify_semphr, portMAX_DELAY);
    return ESP_OK;
}

static esp_err_t rmt_step_motor_step(step_motor_t *handle, uint32_t n, uint32_t speed)
{
    rmt_step_motor_t *rmt_handle = __containerof(handle, rmt_step_motor_t, base);

    ESP_ERROR_CHECK(rmt_tx_stop(rmt_handle->rmt_ch));

    if (n == UINT32_MAX) {   // forever loop, non-blocking
        ESP_ERROR_CHECK(rmt_set_tx_loop_count(rmt_handle->rmt_ch, 0));
        ESP_ERROR_CHECK(rmt_enable_tx_loop_autostop(rmt_handle->rmt_ch, false));
        ESP_ERROR_CHECK(rmt_set_tx_loop_mode(rmt_handle->rmt_ch, true));
        helper_fill_rmt_items(&rmt_handle->rmt_items_loop, speed, rmt_handle->io_driver);
        rmt_handle->status = UNLIMITED_LOOP;
        ESP_ERROR_CHECK(rmt_write_items(rmt_handle->rmt_ch, &rmt_handle->rmt_items_loop, 1, false));
        return ESP_OK;
    } else if (n == 0) {     // break the forever loop
        rmt_handle->status = STOPPED;
        ESP_ERROR_CHECK(rmt_tx_stop(rmt_handle->rmt_ch));
        ESP_ERROR_CHECK(rmt_set_tx_loop_mode(rmt_handle->rmt_ch, false));
        return ESP_OK;
    } else {                 // normally move n steps
        ESP_RETURN_ON_FALSE(helper_speed_to_duration(speed) > 1, ESP_ERR_INVALID_ARG, TAG,
                            "speed too fast");
        return rmt_step_motor_step_impl(handle, n, speed);
    }
}

static esp_err_t rmt_step_motor_smoothstep(step_motor_t *handle, uint32_t n, uint32_t speed_steps, uint32_t speed_min,
        uint32_t speed_max)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(speed_min <= speed_max, ESP_ERR_INVALID_ARG, TAG, "max speed lower than min speed");
    ESP_RETURN_ON_FALSE(n > speed_steps * 2, ESP_ERR_INVALID_ARG, TAG, "too few steps. consider lower speed_steps");
    ESP_RETURN_ON_FALSE(helper_speed_to_duration(speed_min) < 1 << 15, ESP_ERR_INVALID_ARG, TAG, "min speed too low");
    ESP_RETURN_ON_FALSE(helper_speed_to_duration(speed_max) > 1, ESP_ERR_INVALID_ARG, TAG, "max speed too high");

    rmt_step_motor_t *rmt_handle = __containerof(handle, rmt_step_motor_t, base);
    rmt_handle->rmt_items_speedup = malloc(sizeof(rmt_item32_t) * speed_steps);
    ESP_RETURN_ON_FALSE(rmt_handle->rmt_items_speedup != NULL, ESP_ERR_NO_MEM, TAG,
                        "failed to allocate rmt_items_speedup");
    rmt_handle->rmt_items_speeddown = malloc(sizeof(rmt_item32_t) * speed_steps);
    ESP_GOTO_ON_FALSE(rmt_handle->rmt_items_speeddown != NULL, ESP_ERR_NO_MEM, err_free_speedup, TAG,
                      "failed to allocate rmt_items_speeddown");
    ESP_GOTO_ON_ERROR(rmt_tx_stop(rmt_handle->rmt_ch), err_free_speeddown, TAG, "failed to stop rmt tx");

    // prepare speed tables
    for (int i = 0; i < speed_steps; ++i) {
        helper_fill_rmt_items(&rmt_handle->rmt_items_speedup[i],
                              (uint16_t)helper_smootherstep(
                                  (float)speed_min,
                                  (float)speed_max,
                                  (float)speed_min + ( (float)i / (float)speed_steps) * (float)(speed_max - speed_min))
                              , rmt_handle->io_driver
                             );
    }
    for (int i = 0; i < speed_steps; ++i) {
        helper_fill_rmt_items(&rmt_handle->rmt_items_speeddown[i],
                              speed_max + speed_min - (uint16_t)helper_smootherstep(
                                  (float)speed_min,
                                  (float)speed_max,
                                  (float)speed_min + ((float) i / (float)speed_steps) * (float)(speed_max - speed_min)
                              )
                              , rmt_handle->io_driver
                             );
    }
    rmt_handle->rmt_items_smoothstep_count = speed_steps;
    // prepare continuous phase rmt payload
    helper_fill_rmt_items(&rmt_handle->rmt_items_loop, speed_max, rmt_handle->io_driver);
    rmt_handle->rmt_items_loop_count = n - speed_steps * 2;
    // set status to be checked inside ISR
    rmt_handle->status = SMOOTH_SPEED_UP;
    // start transmitting
    ESP_ERROR_CHECK(rmt_write_items(rmt_handle->rmt_ch, rmt_handle->rmt_items_speedup, speed_steps, false));

    // waiting for transfer done
    xSemaphoreTake(rmt_handle->notify_semphr, portMAX_DELAY);

err_free_speeddown:
    free(rmt_handle->rmt_items_speeddown);
err_free_speedup:
    free(rmt_handle->rmt_items_speedup);
    return ret;
}

static esp_err_t rmt_step_motor_set_step(step_motor_t *handle, uint16_t microstep, bool direction)
{
    rmt_step_motor_t *rmt_handle = __containerof(handle, rmt_step_motor_t, base);
    step_motor_driver_io_t *io_driver = rmt_handle->io_driver;
    if (io_driver->set_direction) {
        ESP_ERROR_CHECK(io_driver->set_direction(io_driver, direction));
    }
    if (io_driver->set_microstep) {
        ESP_ERROR_CHECK(io_driver->set_microstep(io_driver, microstep));
    }
    // at least 200ns delay as described in datasheet
    esp_rom_delay_us(1);
    return ESP_OK;
}

static IRAM_ATTR void rmt_tx_loop_intr(rmt_channel_t channel, void *args)
{
    rmt_step_motor_t *rmt_step_motor = (rmt_step_motor_t *) args;

    // smoothstep speedup stage finished
    if (rmt_step_motor->status == SMOOTH_SPEED_UP) {
        rmt_step_motor->status = SMOOTH_KEEP_SPEED;
        rmt_set_tx_loop_mode(rmt_step_motor->rmt_ch, true);
        rmt_enable_tx_loop_autostop(rmt_step_motor->rmt_ch, true);
        rmt_set_tx_intr_en(rmt_step_motor->rmt_ch, 0);
        // continue and configure loop count
    }

    if (rmt_step_motor->status == SMOOTH_KEEP_SPEED || rmt_step_motor->status == LIMITED_LOOP) {
        // loop count not 0, continuing looping
        if ((rmt_step_motor->rmt_items_loop_count) != 0) {
            if ((rmt_step_motor->rmt_items_loop_count) > 1023) {
                (rmt_step_motor->rmt_items_loop_count) -= 1023;
                rmt_set_tx_loop_count(rmt_step_motor->rmt_ch, 1023);
            } else {
                rmt_set_tx_loop_count(rmt_step_motor->rmt_ch, rmt_step_motor->rmt_items_loop_count);
                rmt_step_motor->rmt_items_loop_count = 0;
            }
            rmt_write_items(rmt_step_motor->rmt_ch, &rmt_step_motor->rmt_items_loop, 1, false);
            return;
        }
    }

    // smoothstep keep speed stage finished
    if (rmt_step_motor->status == SMOOTH_KEEP_SPEED) {
        rmt_step_motor->status = SMOOTH_SLOW_DOWN;
        rmt_set_tx_loop_mode(rmt_step_motor->rmt_ch, false);
        rmt_enable_tx_loop_autostop(rmt_step_motor->rmt_ch, false);
        rmt_set_tx_intr_en(rmt_step_motor->rmt_ch, 1);
        rmt_write_items(rmt_step_motor->rmt_ch, rmt_step_motor->rmt_items_speeddown, rmt_step_motor->rmt_items_smoothstep_count, false);
        return;
    }

    if (rmt_step_motor->status == LIMITED_LOOP || rmt_step_motor->status == SMOOTH_SLOW_DOWN) {
        rmt_step_motor->status = STOPPED;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(rmt_step_motor->notify_semphr, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    }
}

esp_err_t step_motor_create_rmt(step_motor_driver_io_t *io_driver, const rmt_config_t *rmt_conf, step_motor_handle_t *ret_handle)
{
    esp_err_t ret = ESP_OK;
    rmt_step_motor_t *rmt_step_motor = NULL;

    ESP_RETURN_ON_ERROR(rmt_config(rmt_conf), TAG, "Failed to configure RMT");
    ESP_RETURN_ON_ERROR(rmt_driver_install(rmt_conf->channel, 0, 0), TAG, "Failed to install RMT driver");

    ESP_GOTO_ON_FALSE(io_driver, ESP_ERR_INVALID_ARG, err, TAG, "configuration can't be null");
    ESP_GOTO_ON_FALSE(ret_handle, ESP_ERR_INVALID_ARG, err, TAG, "can't assign handle to null");

    rmt_step_motor = calloc(1, sizeof(rmt_step_motor_t));
    ESP_GOTO_ON_FALSE(rmt_step_motor, ESP_ERR_NO_MEM, err, TAG, "allocate context memory failed");
    rmt_step_motor->rmt_ch = rmt_conf->channel;

    rmt_step_motor->notify_semphr = xSemaphoreCreateBinary();
    ESP_GOTO_ON_FALSE(rmt_step_motor, ESP_ERR_NO_MEM, err, TAG, "allocate semaphore memory failed");

    rmt_step_motor->io_driver = io_driver;

    // register tx end callback function, which got invoked when tx loop comes to the end
    rmt_register_tx_end_callback(rmt_tx_loop_intr, rmt_step_motor);

    rmt_step_motor->base.init = rmt_step_motor_init;
    rmt_step_motor->base.deinit = rmt_step_motor_deinit;
    rmt_step_motor->base.step = rmt_step_motor_step;
    rmt_step_motor->base.set_step = rmt_step_motor_set_step;
    rmt_step_motor->base.smooth_step = rmt_step_motor_smoothstep;

    *ret_handle = &(rmt_step_motor->base);
    return ESP_OK;

err:
    if (rmt_step_motor) {
        if (rmt_step_motor->notify_semphr) {
            vSemaphoreDelete(rmt_step_motor->notify_semphr);
        }
        free(rmt_step_motor);
    }
    return ret;
}

esp_err_t step_motor_delete_rmt(step_motor_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_STATE, TAG, "empty handle");
    rmt_step_motor_t *rmt_handle = __containerof(handle, rmt_step_motor_t, base);
    ESP_RETURN_ON_ERROR(rmt_driver_uninstall(rmt_handle->rmt_ch), TAG, "Failed to uninstall RMT driver");
    vSemaphoreDelete(rmt_handle->notify_semphr);
    free(rmt_handle);
    return ESP_OK;
}
