/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
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
#include "esp_private/regi2c_ctrl.h"
#include "esp_private/critical_section.h"
#include "esp_private/adc_share_hw_ctrl.h"
#include "hal/sar_ctrl_ll.h"
#include "hal/adc_ll.h"

ESP_LOG_ATTR_TAG(TAG, "sar_periph_ctrl");
extern portMUX_TYPE rtc_spinlock;


void sar_periph_ctrl_init(void)
{
    //Put SAR control mux to FSM state
    sar_ctrl_ll_set_power_mode(SAR_CTRL_LL_POWER_FSM);
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

/**
 * This gets incremented when s_sar_power_acquire() is called,
 * and decremented when s_sar_power_release() is called.
 * PWDET and REG_I2C are powered down when the value reaches zero.
 * Should be modified within critical section.
 */
static int s_sar_power_on_cnt;

static void s_sar_power_acquire(void)
{
    esp_os_enter_critical_safe(&rtc_spinlock);
    regi2c_saradc_enable();
    s_sar_power_on_cnt++;
    if (s_sar_power_on_cnt == 1) {
        sar_ctrl_ll_set_power_mode(SAR_CTRL_LL_POWER_ON);
    }
    esp_os_exit_critical_safe(&rtc_spinlock);
}

static void s_sar_power_release(void)
{
    esp_os_enter_critical_safe(&rtc_spinlock);
    s_sar_power_on_cnt--;
    if (s_sar_power_on_cnt < 0) {
        esp_os_exit_critical_safe(&rtc_spinlock);
        ESP_LOGE(TAG, "%s called, but s_sar_power_on_cnt == 0", __func__);
        abort();
    } else if (s_sar_power_on_cnt == 0) {
        sar_ctrl_ll_set_power_mode(SAR_CTRL_LL_POWER_FSM);
    }
    regi2c_saradc_disable();
    esp_os_exit_critical_safe(&rtc_spinlock);
}


/*------------------------------------------------------------------------------
* PWDET Power
*----------------------------------------------------------------------------*/
void sar_periph_ctrl_pwdet_power_acquire(void)
{
    s_sar_power_acquire();
}

void sar_periph_ctrl_pwdet_power_release(void)
{
    s_sar_power_release();
}


/*------------------------------------------------------------------------------
* ADC Power
*----------------------------------------------------------------------------*/
void sar_periph_ctrl_adc_oneshot_power_acquire(void)
{
    s_sar_power_acquire();
}

void sar_periph_ctrl_adc_oneshot_power_release(void)
{
    s_sar_power_release();
}

void sar_periph_ctrl_adc_continuous_power_acquire(void)
{
    s_sar_power_acquire();
}

void sar_periph_ctrl_adc_continuous_power_release(void)
{
    s_sar_power_release();
}

/*------------------------------------------------------------------------------
* ADC Reset
*----------------------------------------------------------------------------*/
void sar_periph_ctrl_adc_reset(void)
{
    PERIPH_RCC_ATOMIC() {
        adc_ll_reset_register();
    }
}

void adc_reset_lock_acquire(void)
{
    // Empty implementation
}

void adc_reset_lock_release(void)
{
    // Empty implementation
}
