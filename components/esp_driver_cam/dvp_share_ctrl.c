/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

bool dvp_signal_used;
static portMUX_TYPE s_spinlock = portMUX_INITIALIZER_UNLOCKED;

esp_err_t dvp_shared_ctrl_claim_io_signals(void)
{
    esp_err_t ret = ESP_ERR_NOT_FOUND;
    portENTER_CRITICAL(&s_spinlock);
    if (!dvp_signal_used) {
        dvp_signal_used = true;
        ret = ESP_OK;
    }
    portEXIT_CRITICAL(&s_spinlock);

    return ret;
}

esp_err_t dvp_shared_ctrl_declaim_io_signals(void)
{
    portENTER_CRITICAL(&s_spinlock);
    dvp_signal_used = false;
    portEXIT_CRITICAL(&s_spinlock);

    return ESP_OK;
}
