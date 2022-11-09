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

void sar_periph_ctrl_init(void)
{
    //TODO: IDF-6123
}

void sar_periph_ctrl_power_disable(void)
{
    //TODO: IDF-6123
}
