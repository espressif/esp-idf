/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * SAR related peripherals are interdependent. This file
 * provides a united control to these registers, as multiple
 * components require these controls.
 *
 * Related peripherals are:
 * - ADC
 * - PWDET
 */

#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/sar_periph_ctrl.h"
#include "hal/sar_ctrl_ll.h"

static const char *TAG = "sar_periph_ctrl";
extern portMUX_TYPE rtc_spinlock;

/*------------------------------------------------------------------------------
* PWDET Power
*----------------------------------------------------------------------------*/
// This gets incremented when sar_periph_ctrl_pwdet_power_acquire() is called,
// and decremented when sar_periph_ctrl_pwdet_power_release() is called. PWDET
// is powered down when the value reaches zero. Should be modified within critical section.
static int s_pwdet_power_on_cnt;

void sar_periph_ctrl_init(void)
{
    //Put SAR control mux to ON state
    sar_ctrl_ll_set_power_mode(SAR_CTRL_LL_POWER_ON);

    //Add other periph power control initialisation here
}

void sar_periph_ctrl_power_disable(void)
{
    portENTER_CRITICAL_SAFE(&rtc_spinlock);
    sar_ctrl_ll_set_power_mode(SAR_CTRL_LL_POWER_OFF);
    portEXIT_CRITICAL_SAFE(&rtc_spinlock);
}

void sar_periph_ctrl_pwdet_power_acquire(void)
{
    portENTER_CRITICAL_SAFE(&rtc_spinlock);
    s_pwdet_power_on_cnt++;
    if (s_pwdet_power_on_cnt == 1) {
        sar_ctrl_ll_set_power_mode(SAR_CTRL_LL_POWER_ON);
    }
    portEXIT_CRITICAL_SAFE(&rtc_spinlock);
}

void sar_periph_ctrl_pwdet_power_release(void)
{
    portENTER_CRITICAL_SAFE(&rtc_spinlock);
    s_pwdet_power_on_cnt--;
    /* Sanity check */
    if (s_pwdet_power_on_cnt < 0) {
        portEXIT_CRITICAL(&rtc_spinlock);
        ESP_LOGE(TAG, "%s called, but s_pwdet_power_on_cnt == 0", __func__);
        abort();
    } else if (s_pwdet_power_on_cnt == 0) {
        sar_ctrl_ll_set_power_mode(SAR_CTRL_LL_POWER_FSM);
    }
    portEXIT_CRITICAL_SAFE(&rtc_spinlock);
}
