/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
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

#include <sys/lock.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/sar_periph_ctrl.h"
#include "esp_private/regi2c_ctrl.h"
#include "esp_private/critical_section.h"
#include "esp_private/adc_share_hw_ctrl.h"
#include "hal/sar_ctrl_ll.h"
#include "hal/adc_ll.h"
#include "hal/temperature_sensor_ll.h"

ESP_LOG_ATTR_TAG(TAG, "sar_periph_ctrl");
extern portMUX_TYPE rtc_spinlock;
static _lock_t adc_reset_lock;


void sar_periph_ctrl_init(void)
{
    //Put SAR control mux to FSM state
    sar_ctrl_ll_set_power_mode(SAR_CTRL_LL_POWER_FSM);

    //Add other periph power control initialisation here
}

void sar_periph_ctrl_power_enable(void)
{
    esp_os_enter_critical_safe(&rtc_spinlock);
    sar_ctrl_ll_set_power_mode(SAR_CTRL_LL_POWER_FSM);
    esp_os_exit_critical_safe(&rtc_spinlock);
}

void sar_periph_ctrl_power_disable(void)
{
    esp_os_enter_critical_safe(&rtc_spinlock);
    sar_ctrl_ll_set_power_mode(SAR_CTRL_LL_POWER_OFF);
    esp_os_exit_critical_safe(&rtc_spinlock);
}


/*------------------------------------------------------------------------------
* PWDET Power
*----------------------------------------------------------------------------*/
static int s_pwdet_power_on_cnt;

void sar_periph_ctrl_pwdet_power_acquire(void)
{
    esp_os_enter_critical_safe(&rtc_spinlock);
    s_pwdet_power_on_cnt++;
    if (s_pwdet_power_on_cnt == 1) {
        sar_ctrl_ll_set_power_mode_from_pwdet(SAR_CTRL_LL_POWER_ON);
    }
    esp_os_exit_critical_safe(&rtc_spinlock);
}

void sar_periph_ctrl_pwdet_power_release(void)
{
    esp_os_enter_critical_safe(&rtc_spinlock);
    s_pwdet_power_on_cnt--;
    /* Sanity check */
    if (s_pwdet_power_on_cnt < 0) {
        esp_os_exit_critical(&rtc_spinlock);
        ESP_LOGE(TAG, "%s called, but s_pwdet_power_on_cnt == 0", __func__);
        abort();
    } else if (s_pwdet_power_on_cnt == 0) {
        sar_ctrl_ll_set_power_mode_from_pwdet(SAR_CTRL_LL_POWER_FSM);
    }
    esp_os_exit_critical_safe(&rtc_spinlock);
}


/*------------------------------------------------------------------------------
* ADC Power
*----------------------------------------------------------------------------*/
static int s_sar_power_on_cnt;

static void s_sar_adc_power_acquire(void)
{
    esp_os_enter_critical_safe(&rtc_spinlock);
    regi2c_saradc_enable();
    s_sar_power_on_cnt++;
    if (s_sar_power_on_cnt == 1) {
        adc_ll_digi_set_power_manage(ADC_LL_POWER_SW_ON);
    }
    esp_os_exit_critical_safe(&rtc_spinlock);
}

static void s_sar_adc_power_release(void)
{
    esp_os_enter_critical_safe(&rtc_spinlock);
    s_sar_power_on_cnt--;
    if (s_sar_power_on_cnt < 0) {
        esp_os_exit_critical_safe(&rtc_spinlock);
        ESP_LOGE(TAG, "%s called, but s_sar_power_on_cnt == 0", __func__);
        abort();
    } else if (s_sar_power_on_cnt == 0) {
        adc_ll_digi_set_power_manage(ADC_LL_POWER_BY_FSM);
    }
    regi2c_saradc_disable();
    esp_os_exit_critical_safe(&rtc_spinlock);
}

void sar_periph_ctrl_adc_oneshot_power_acquire(void)
{
    s_sar_adc_power_acquire();
}

void sar_periph_ctrl_adc_oneshot_power_release(void)
{
    s_sar_adc_power_release();
}

void sar_periph_ctrl_adc_continuous_power_acquire(void)
{
    s_sar_adc_power_acquire();
}

void sar_periph_ctrl_adc_continuous_power_release(void)
{
    s_sar_adc_power_release();
}

/*------------------------------------------------------------------------------
* ADC Reset
*----------------------------------------------------------------------------*/
void sar_periph_ctrl_adc_reset(void)
{
    // Acquire ADC reset lock to prevent temperature sensor readings during ADC reset
    adc_reset_lock_acquire();

    PERIPH_RCC_ATOMIC() {
        // Save temperature sensor related register values before ADC reset
        tsens_ll_reg_values_t saved_tsens_regs = {};
        tsens_ll_backup_registers(&saved_tsens_regs);
        adc_ll_reset_register();
        // Restore temperature sensor related register values after ADC reset
        temperature_sensor_ll_reset_module();
        tsens_ll_restore_registers(&saved_tsens_regs);
    }

    // Release ADC reset lock after ADC reset is complete
    adc_reset_lock_release();
}

void adc_reset_lock_acquire(void)
{
    _lock_acquire(&adc_reset_lock);
}

void adc_reset_lock_release(void)
{
    _lock_release(&adc_reset_lock);
}
