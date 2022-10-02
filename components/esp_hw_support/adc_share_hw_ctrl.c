/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * ADC is shared by multiple components, including:
 * - esp_phy
 * - esp_wifi
 * - driver
 *
 * However, usages of above components are different.
 * Therefore, we put the common used parts into `esp_hw_support`, including:
 * - adc power maintainance
 * - adc hw calibration settings
 * - adc locks, to prevent concurrently using adc hw
 */

#include <esp_types.h>
#include "sdkconfig.h"
#include "sys/lock.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "hal/adc_types.h"
#include "hal/adc_hal.h"
#include "hal/adc_hal_common.h"
#include "hal/adc_hal_conf.h"
#include "esp_private/adc_share_hw_ctrl.h"
//For calibration
#if CONFIG_IDF_TARGET_ESP32S2
#include "esp_efuse_rtc_table.h"
#elif SOC_ADC_CALIBRATION_V1_SUPPORTED
#include "esp_efuse_rtc_calib.h"
#endif


static const char *TAG = "adc_share_hw_ctrl";
extern portMUX_TYPE rtc_spinlock;

/*------------------------------------------------------------------------------
* ADC Power
*----------------------------------------------------------------------------*/
// This gets incremented when adc_power_acquire() is called, and decremented when
// adc_power_release() is called. ADC is powered down when the value reaches zero.
// Should be modified within critical section (ADC_ENTER/EXIT_CRITICAL).
static int s_adc_power_on_cnt;

static void adc_power_on_internal(void)
{
    /* Set the power always on to increase precision. */
    adc_hal_set_power_manage(ADC_POWER_SW_ON);
}

void adc_power_acquire(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    s_adc_power_on_cnt++;
    if (s_adc_power_on_cnt == 1) {
        adc_power_on_internal();
    }
    portEXIT_CRITICAL(&rtc_spinlock);
}

static void adc_power_off_internal(void)
{
#if CONFIG_IDF_TARGET_ESP32
    adc_hal_set_power_manage(ADC_POWER_SW_OFF);
#else
    adc_hal_set_power_manage(ADC_POWER_BY_FSM);
#endif
}

void adc_power_release(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    s_adc_power_on_cnt--;
    /* Sanity check */
    if (s_adc_power_on_cnt < 0) {
        portEXIT_CRITICAL(&rtc_spinlock);
        ESP_LOGE(TAG, "%s called, but s_adc_power_on_cnt == 0", __func__);
        abort();
    } else if (s_adc_power_on_cnt == 0) {
        adc_power_off_internal();
    }
    portEXIT_CRITICAL(&rtc_spinlock);
}


#if SOC_ADC_CALIBRATION_V1_SUPPORTED
/*---------------------------------------------------------------
            ADC Hardware Calibration
---------------------------------------------------------------*/
#if CONFIG_IDF_TARGET_ESP32S2
#define esp_efuse_rtc_calib_get_ver()     esp_efuse_rtc_table_read_calib_version()

static inline uint32_t esp_efuse_rtc_calib_get_init_code(int version, uint32_t adc_unit, int atten)
{
    int tag = esp_efuse_rtc_table_get_tag(version, adc_unit, atten, RTCCALIB_V2_PARAM_VINIT);
    return esp_efuse_rtc_table_get_parsed_efuse_value(tag, false);
}
#endif

static uint32_t s_adc_cali_param[SOC_ADC_PERIPH_NUM][SOC_ADC_ATTEN_NUM] = {};

void adc_calc_hw_calibration_code(adc_unit_t adc_n, adc_atten_t atten)
{
    if (s_adc_cali_param[adc_n][atten]) {
        ESP_EARLY_LOGV(TAG, "Use calibrated val ADC%d atten=%d: %04X", adc_n + 1, atten, s_adc_cali_param[adc_n][atten]);
        return ;
    }

    // check if we can fetch the values from eFuse.
    int version = esp_efuse_rtc_calib_get_ver();

    uint32_t init_code = 0;

    if (version == ESP_EFUSE_ADC_CALIB_VER) {
        init_code = esp_efuse_rtc_calib_get_init_code(version, adc_n, atten);
    } else {
        ESP_EARLY_LOGD(TAG, "Calibration eFuse is not configured, use self-calibration for ICode");
        adc_power_acquire();
        portENTER_CRITICAL(&rtc_spinlock);
        adc_ll_pwdet_set_cct(ADC_HAL_PWDET_CCT_DEFAULT);
        const bool internal_gnd = true;
        init_code = adc_hal_self_calibration(adc_n, atten, internal_gnd);
        portEXIT_CRITICAL(&rtc_spinlock);
        adc_power_release();
    }

    s_adc_cali_param[adc_n][atten] = init_code;
    ESP_EARLY_LOGV(TAG, "Calib(V%d) ADC%d atten=%d: %04X", version, adc_n + 1, atten, init_code);
}

void IRAM_ATTR adc_set_hw_calibration_code(adc_unit_t adc_n, adc_atten_t atten)
{
    adc_hal_set_calibration_param(adc_n, s_adc_cali_param[adc_n][atten]);
}
#endif //#if SOC_ADC_CALIBRATION_V1_SUPPORTED


/*---------------------------------------------------------------
            ADC Hardware Locks
---------------------------------------------------------------*/
static _lock_t adc1_lock;
static _lock_t adc2_lock;

esp_err_t adc_lock_acquire(adc_unit_t adc_unit)
{
    if (adc_unit == ADC_UNIT_1) {
        _lock_acquire(&adc1_lock);
    }

    if (adc_unit == ADC_UNIT_2) {
        _lock_acquire(&adc2_lock);
    }

    return ESP_OK;
}

esp_err_t adc_lock_release(adc_unit_t adc_unit)
{
    if (adc_unit == ADC_UNIT_2) {
        ESP_RETURN_ON_FALSE(((uint32_t *)adc2_lock != NULL), ESP_ERR_INVALID_STATE, TAG, "adc2 lock release without acquiring");
        _lock_release(&adc2_lock);
    }

    if (adc_unit == ADC_UNIT_1) {
        ESP_RETURN_ON_FALSE(((uint32_t *)adc1_lock != NULL), ESP_ERR_INVALID_STATE, TAG, "adc1 lock release without acquiring");
        _lock_release(&adc1_lock);
    }

    return ESP_OK;
}

esp_err_t adc_lock_try_acquire(adc_unit_t adc_unit)
{
    if (adc_unit == ADC_UNIT_1) {
        if (_lock_try_acquire(&adc1_lock) == -1) {
            return ESP_ERR_TIMEOUT;
        }
    }

    if (adc_unit == ADC_UNIT_2) {
        if (_lock_try_acquire(&adc2_lock) == -1) {
            return ESP_ERR_TIMEOUT;
        }
    }

    return ESP_OK;
}

esp_err_t adc2_wifi_acquire(void)
{
#if CONFIG_IDF_TARGET_ESP32
    /* Wi-Fi module will use adc2. Use locks to avoid conflicts. */
    adc_lock_acquire(ADC_UNIT_2);
    ESP_LOGD(TAG, "Wi-Fi takes adc2 lock.");
#endif

    return ESP_OK;
}

esp_err_t adc2_wifi_release(void)
{
#if CONFIG_IDF_TARGET_ESP32
    return adc_lock_release(ADC_UNIT_2);
#endif

    return ESP_OK;
}
