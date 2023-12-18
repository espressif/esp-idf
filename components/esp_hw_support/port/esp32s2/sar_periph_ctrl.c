/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/sar_periph_ctrl.h"
#include "esp_log.h"
#include "soc/apb_saradc_struct.h"
#include "soc/sens_struct.h"
#include "private_include/regi2c_saradc.h"
#include "driver/temp_sensor.h"
#include "regi2c_ctrl.h"


extern __attribute__((unused)) portMUX_TYPE rtc_spinlock;


/*------------------------------------------------------------------------------------------------------------
-----------------------------------------Temperature Sensor---------------------------------------------------
------------------------------------------------------------------------------------------------------------*/
static const char *TAG_TSENS = "temperature_sensor";
#include "regi2c_ctrl.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#define INT_NOT_USED 999999

static int s_record_min = INT_NOT_USED;
static int s_record_max = INT_NOT_USED;
static int s_temperature_sensor_power_cnt;

static uint8_t s_tsens_idx = 2; // Index for temperature attribute, set 2(middle) as default value

void temperature_sensor_power_acquire(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    s_temperature_sensor_power_cnt++;
    if (s_temperature_sensor_power_cnt == 1) {
        SENS.sar_tctrl.tsens_power_up_force = true;
        SENS.sar_tctrl2.tsens_xpd_force = true;
        SENS.sar_tctrl.tsens_power_up = true;
        CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_SAR_I2C_FORCE_PD_M);
        SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_SAR_I2C_FORCE_PU_M);
        CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, I2C_SAR_M);
        SET_PERI_REG_MASK(ANA_CONFIG2_REG, ANA_SAR_CFG2_M);
        SENS.sar_tctrl2.tsens_clkgate_en = 1;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
}

void temperature_sensor_power_release(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    s_temperature_sensor_power_cnt--;
    /* Sanity check */
    if (s_temperature_sensor_power_cnt < 0) {
        portEXIT_CRITICAL(&rtc_spinlock);
        ESP_LOGE(TAG_TSENS, "%s called, but s_temperature_sensor_power_cnt == 0", __func__);
        abort();
    } else if (s_temperature_sensor_power_cnt == 0) {
        SENS.sar_tctrl.tsens_power_up_force = false;
        SENS.sar_tctrl2.tsens_xpd_force = false;
        SENS.sar_tctrl.tsens_power_up = false;
        SENS.sar_tctrl2.tsens_clkgate_en = 0;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
}

static int temperature_sensor_get_raw_value(void)
{
    SENS.sar_tctrl.tsens_dump_out = 1;
    while (!SENS.sar_tctrl.tsens_ready) {
    }
    SENS.sar_tctrl.tsens_dump_out = 0;
    int raw_value = SENS.sar_tctrl.tsens_out;
    return (TSENS_ADC_FACTOR * raw_value - TSENS_DAC_FACTOR * dac_offset[s_tsens_idx].offset - TSENS_SYS_OFFSET);
}

void temp_sensor_sync_tsens_idx(int tsens_idx)
{
    s_tsens_idx = tsens_idx;
}

int16_t temp_sensor_get_raw_value(bool *range_changed)
{
    portENTER_CRITICAL(&rtc_spinlock);

    int degree = temperature_sensor_get_raw_value();
    uint8_t temperature_dac;

    // 1. Check whether temperature value is in range
    if (s_record_min != INT_NOT_USED && degree >= s_record_min && degree <= s_record_max) {
        // If degree is in range, not needed to do any check to save time. Otherwise, choose proper range and record.
        if (range_changed != NULL) {
            *range_changed = false;
        }
        portEXIT_CRITICAL(&rtc_spinlock);
        return degree;
    }

    // 2. If temperature value is not in range, adjust to proper range
    if (degree >= dac_offset[1].range_max) {
        s_tsens_idx = 0;
    } else if (degree >= dac_offset[2].range_max && degree < dac_offset[1].range_max) {
        s_tsens_idx = 1;
    } else if (degree <= dac_offset[2].range_min && degree > dac_offset[3].range_min) {
        s_tsens_idx = 3;
    } else if (degree <= dac_offset[3].range_min) {
        s_tsens_idx = 4;
    } else {
        s_tsens_idx = 2;
    }
    ESP_EARLY_LOGD(TAG_TSENS, "range changed, change to index %d", s_tsens_idx);
    temperature_dac = dac_offset[s_tsens_idx].set_val;
    s_record_min = dac_offset[s_tsens_idx].range_min;
    s_record_max = dac_offset[s_tsens_idx].range_max;

    REGI2C_WRITE_MASK(I2C_SAR_ADC, I2C_SARADC_TSENS_DAC, temperature_dac);

    // 3. Then, read value again
    // Before reading the temperature value, ticks need to be delayed, otherwise a wrong value will be returned.
    // As what has been recommended and tested, 300us is a good interval to get the correct value after adjust range.
    esp_rom_delay_us(300);
    degree = temperature_sensor_get_raw_value();
    if (range_changed != NULL) {
        *range_changed = true;
    }

    portEXIT_CRITICAL(&rtc_spinlock);
    return degree;
}
