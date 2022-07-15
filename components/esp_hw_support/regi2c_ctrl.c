/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "esp_attr.h"
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "hal/regi2c_ctrl.h"
#include "hal/regi2c_ctrl_ll.h"
#include "esp_hw_log.h"

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

static DRAM_ATTR __attribute__((unused)) const char *TAG = "REGI2C";

uint8_t IRAM_ATTR regi2c_ctrl_read_reg(uint8_t block, uint8_t host_id, uint8_t reg_add)
{
    portENTER_CRITICAL_SAFE(&mux);
    uint8_t value = regi2c_read_reg_raw(block, host_id, reg_add);
    portEXIT_CRITICAL_SAFE(&mux);
    return value;
}

uint8_t IRAM_ATTR regi2c_ctrl_read_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb)
{
    portENTER_CRITICAL_SAFE(&mux);
    uint8_t value = regi2c_read_reg_mask_raw(block, host_id, reg_add, msb, lsb);
    portEXIT_CRITICAL_SAFE(&mux);
    return value;
}

void IRAM_ATTR regi2c_ctrl_write_reg(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data)
{
    portENTER_CRITICAL_SAFE(&mux);
    regi2c_write_reg_raw(block, host_id, reg_add, data);
    portEXIT_CRITICAL_SAFE(&mux);
}

void IRAM_ATTR regi2c_ctrl_write_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data)
{
    portENTER_CRITICAL_SAFE(&mux);
    regi2c_write_reg_mask_raw(block, host_id, reg_add, msb, lsb, data);
    portEXIT_CRITICAL_SAFE(&mux);
}

void IRAM_ATTR regi2c_enter_critical(void)
{
    portENTER_CRITICAL_SAFE(&mux);
}

void IRAM_ATTR regi2c_exit_critical(void)
{
    portEXIT_CRITICAL_SAFE(&mux);
}

/**
 * Restore regi2c analog calibration related configuration registers.
 * This is a workaround, and is fixed on later chips
 */
#if REGI2C_ANA_CALI_PD_WORKAROUND
#include "soc/regi2c_saradc.h"

static DRAM_ATTR uint8_t reg_val[REGI2C_ANA_CALI_BYTE_NUM];

void IRAM_ATTR regi2c_analog_cali_reg_read(void)
{
    for (int i = 0; i < REGI2C_ANA_CALI_BYTE_NUM; i++) {
        reg_val[i] = regi2c_ctrl_read_reg(I2C_SAR_ADC, I2C_SAR_ADC_HOSTID, i);
    }
}

void IRAM_ATTR regi2c_analog_cali_reg_write(void)
{
    for (int i = 0; i < REGI2C_ANA_CALI_BYTE_NUM; i++) {
        regi2c_ctrl_write_reg(I2C_SAR_ADC, I2C_SAR_ADC_HOSTID, i, reg_val[i]);
    }
}


/**
 * REGI2C_SARADC reference count
 */
static int s_i2c_saradc_enable_cnt;

void regi2c_saradc_enable(void)
{
    regi2c_enter_critical();
    s_i2c_saradc_enable_cnt++;
    if (s_i2c_saradc_enable_cnt == 1) {
        regi2c_ctrl_ll_i2c_saradc_enable();
    }
    regi2c_exit_critical();
}

void regi2c_saradc_disable(void)
{
    regi2c_enter_critical();
    s_i2c_saradc_enable_cnt--;
    if (s_i2c_saradc_enable_cnt < 0){
        regi2c_exit_critical();
        ESP_HW_LOGE(TAG, "REGI2C_SARADC is already disabled");
    } else if (s_i2c_saradc_enable_cnt == 0) {
        regi2c_ctrl_ll_i2c_saradc_disable();
    }
    regi2c_exit_critical();

}


#endif   //#if ADC_CALI_PD_WORKAROUND
