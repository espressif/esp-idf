/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "esp_attr.h"
#include "esp_private/regi2c_ctrl.h"
#include "esp_private/sar_periph_ctrl.h"
#include "esp_private/sar_periph_ctrl.h"
#include "freertos/FreeRTOS.h"


/*
 * This file is used to override the hooks provided by the PHY lib for some system features.
 * Call phy_override() so that this file will be linked.
 */

static bool s_wifi_adc_xpd_flag;
#if CONFIG_SOC_TEMP_SENSOR_SUPPORTED    // TODO: [ESP32C5] IDF-8727 remove me when fix IDF-8727
static bool s_wifi_pwdet_xpd_flag;
static bool s_wifi_tsens_xpd_flag;
#endif

void include_esp_phy_override(void)
{
    /* When this empty function is called, all functions below will be linked. */
}

/* Coordinate ADC power with other modules. */
// It seems that it is only required on ESP32, but we still compile it for all chips, in case it is
// called by PHY unexpectedly.
void set_xpd_sar(bool en)
{
    if (s_wifi_adc_xpd_flag == en) {
        /* ignore repeated calls to set_xpd_sar when the state is already correct */
        return;
    }

    s_wifi_adc_xpd_flag = en;
    if (en) {
        sar_periph_ctrl_pwdet_power_acquire();
    } else {
        sar_periph_ctrl_pwdet_power_release();
    }
}

//add spinlock protection
IRAM_ATTR void phy_i2c_enter_critical(void)
{
    regi2c_enter_critical();
}

IRAM_ATTR void phy_i2c_exit_critical(void)
{
    regi2c_exit_critical();
}

void phy_set_pwdet_power(bool en)
{
#if CONFIG_SOC_TEMP_SENSOR_SUPPORTED    // TODO: [ESP32C5] IDF-8727 remove me when fix IDF-8727
    if (s_wifi_pwdet_xpd_flag == en) {
        /* ignore repeated calls to phy_set_pwdet_power when the state is already correct */
        return;
    }

    s_wifi_pwdet_xpd_flag = en;
    if (en) {
        sar_periph_ctrl_pwdet_power_acquire();
    } else {
        sar_periph_ctrl_pwdet_power_release();
    }
#endif
}

void IRAM_ATTR  phy_set_tsens_power(bool en)
{
#if CONFIG_SOC_TEMP_SENSOR_SUPPORTED    // TODO: [ESP32C5] IDF-8727 remove me when fix IDF-8727
    if (s_wifi_tsens_xpd_flag == en) {
        /* ignore repeated calls to phy_set_tsens_power when the state is already correct */
        return;
    }

    s_wifi_tsens_xpd_flag = en;
    if (en) {
        temperature_sensor_power_acquire();
    } else {
        temperature_sensor_power_release();
    }
#endif
}

int16_t phy_get_tsens_value(void)
{
#if CONFIG_SOC_TEMP_SENSOR_SUPPORTED    // TODO: [ESP32C5] IDF-8727 remove me when fix IDF-8727
    return temp_sensor_get_raw_value(NULL);
#else
    return 0;
#endif
}
