/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Included
 *
 * Alarm Timer
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#include <stdlib.h>

#include "esp_timer.h"

#include "alarm_timer.h"

typedef struct alarm_timer_s {
    esp_timer_handle_t     timer;
    alarm_timer_callback_t cb;
    alarm_timer_arg_t      arg;
} alarm_timer_t;

static void alarm_timer_cb(void *arg)
{
    alarm_timer_t *alarm = (alarm_timer_t *)arg;

    esp_timer_delete(alarm->timer);
    alarm->cb(alarm->arg);
    free(alarm);
}

esp_err_t alarm_timer_schedule(alarm_timer_callback_t cb, alarm_timer_arg_t arg, uint32_t time_ms)
{
    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    alarm_timer_t *alarm = malloc(sizeof(alarm_timer_t));
    if (alarm == NULL) {
        return ESP_ERR_NO_MEM;
    }

    alarm->cb    = cb;
    alarm->arg   = arg;
    alarm->timer = NULL;

    esp_timer_create_args_t timer_args = {
        .callback = alarm_timer_cb,
        .arg      = alarm,
    };

    esp_err_t err = esp_timer_create(&timer_args, &alarm->timer);
    if (err != ESP_OK) {
        free(alarm);
        return err;
    }

    uint64_t time_us = (uint64_t)time_ms * 1000;
    err              = esp_timer_start_once(alarm->timer, time_us);
    if (err != ESP_OK) {
        esp_timer_delete(alarm->timer);
        free(alarm);
        return err;
    }

    return ESP_OK;
}
