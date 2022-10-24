/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
 * - Temp Sensor
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/sar_periph_ctrl.h"
#include "hal/sar_ctrl_ll.h"

extern portMUX_TYPE rtc_spinlock;

void sar_periph_ctrl_init(void)
{
    //Put SAR control mux to FSM state
    sar_ctrl_ll_set_power_mode(SAR_CTRL_LL_POWER_FSM);

    //Add other periph power control initialisation here
}

void sar_periph_ctrl_power_disable(void)
{
    portENTER_CRITICAL_SAFE(&rtc_spinlock);
    sar_ctrl_ll_set_power_mode(SAR_CTRL_LL_POWER_OFF);
    portEXIT_CRITICAL_SAFE(&rtc_spinlock);
}
