/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_private/critical_section.h"
#include "hal/regi2c_ctrl_ll.h"
#include "esp_hw_log.h"
#include "esp_private/sar_periph_ctrl.h"

ESP_HW_LOG_ATTR_TAG(TAG, "sar_ctrl_common");

DEFINE_CRIT_SECTION_LOCK_STATIC(mux);

static int s_i2c_saradc_enable_cnt;

void regi2c_saradc_enable(void)
{
    esp_os_enter_critical_safe(&mux);
    s_i2c_saradc_enable_cnt++;
    if (s_i2c_saradc_enable_cnt == 1) {
        regi2c_ctrl_ll_i2c_sar_periph_enable();
    }
    esp_os_exit_critical_safe(&mux);
}

void regi2c_saradc_disable(void)
{
    esp_os_enter_critical_safe(&mux);
    s_i2c_saradc_enable_cnt--;
    if (s_i2c_saradc_enable_cnt < 0){
        esp_os_exit_critical_safe(&mux);
        ESP_HW_LOGE(TAG, "REGI2C_SARADC is already disabled");
        return;
    } else if (s_i2c_saradc_enable_cnt == 0) {
        regi2c_ctrl_ll_i2c_sar_periph_disable();
    }
    esp_os_exit_critical_safe(&mux);
}
