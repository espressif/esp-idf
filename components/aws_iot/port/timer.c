/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * Additions Copyright 2016 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file timer.c
 * @brief FreeRTOS implementation of the timer interface uses ticks.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>

#include "timer_platform.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

const static char *TAG = "aws_timer";

bool has_timer_expired(Timer *timer) {
    uint32_t now = xTaskGetTickCount();
    bool expired = (now - timer->start_ticks) >= timer->timeout_ticks;

    /* AWS IoT SDK isn't very RTOS friendly because it polls for "done
       timers" a lot without ever sleeping on them. So we hack in some
       amount of sleeping here: if it seems like the caller is polling
       an unexpired timer in a tight loop then we delay a tick to let
       things progress elsewhere.
    */
    if(!expired && now == timer->last_polled_ticks) {
        vTaskDelay(1);
    }
    timer->last_polled_ticks = now;
    return expired;
}

void countdown_ms(Timer *timer, uint32_t timeout) {
    timer->start_ticks = xTaskGetTickCount();
    timer->timeout_ticks = timeout / portTICK_PERIOD_MS;
    timer->last_polled_ticks = 0;
}

uint32_t left_ms(Timer *timer) {
    uint32_t now = xTaskGetTickCount();
    uint32_t elapsed = now - timer->start_ticks;
    if (elapsed < timer->timeout_ticks) {
        return (timer->timeout_ticks - elapsed) * portTICK_PERIOD_MS;
    } else {
        return 0;
    }
}

void countdown_sec(Timer *timer, uint32_t timeout) {
    if (timeout > UINT32_MAX / 1000) {
        ESP_LOGE(TAG, "timeout is out of range: %ds", timeout);
    }
    countdown_ms(timer, timeout * 1000);
}

void init_timer(Timer *timer) {
    timer->start_ticks = 0;
    timer->timeout_ticks = 0;
    timer->last_polled_ticks = 0;
}

#ifdef __cplusplus
}
#endif
