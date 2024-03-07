/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
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
#include "hal/adc_hal_common.h"
#include "hal/adc_ll.h"
#include "esp_private/adc_share_hw_ctrl.h"
#include "esp_private/sar_periph_ctrl.h"
#include "esp_private/periph_ctrl.h"
#include "soc/periph_defs.h"
//For calibration
#if CONFIG_IDF_TARGET_ESP32S2
#include "esp_efuse_rtc_table.h"
#elif SOC_ADC_CALIBRATION_V1_SUPPORTED
#include "esp_efuse_rtc_calib.h"
#endif


static const char *TAG = "adc_share_hw_ctrl";
extern portMUX_TYPE rtc_spinlock;


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
        ESP_EARLY_LOGV(TAG, "Use calibrated val ADC%d atten=%d: %04" PRIX32, adc_n + 1, atten, s_adc_cali_param[adc_n][atten]);
        return ;
    }

    // check if we can fetch the values from eFuse.
    int version = esp_efuse_rtc_calib_get_ver();

    uint32_t init_code = 0;

    if ((version >= ESP_EFUSE_ADC_CALIB_VER_MIN) &&
        (version <= ESP_EFUSE_ADC_CALIB_VER_MAX)) {
        // Guarantee the calibration version before calling efuse function
        init_code = esp_efuse_rtc_calib_get_init_code(version, adc_n, atten);
    }
#if SOC_ADC_SELF_HW_CALI_SUPPORTED
    else {
        ESP_EARLY_LOGD(TAG, "Calibration eFuse is not configured, use self-calibration for ICode");
        sar_periph_ctrl_adc_oneshot_power_acquire();
        portENTER_CRITICAL(&rtc_spinlock);
        adc_ll_pwdet_set_cct(ADC_LL_PWDET_CCT_DEFAULT);
        const bool internal_gnd = true;
        init_code = adc_hal_self_calibration(adc_n, atten, internal_gnd);
        portEXIT_CRITICAL(&rtc_spinlock);
        sar_periph_ctrl_adc_oneshot_power_release();
    }
#else
    else {
        ESP_EARLY_LOGD(TAG, "ICode self-calibration isn't supported");
    }
#endif  //SOC_ADC_SELF_HW_CALI_SUPPORTED

    s_adc_cali_param[adc_n][atten] = init_code;
    ESP_EARLY_LOGV(TAG, "Calib(V%d) ADC%d atten=%d: %04" PRIX32, version, adc_n + 1, atten, init_code);
}

void IRAM_ATTR adc_set_hw_calibration_code(adc_unit_t adc_n, adc_atten_t atten)
{
    adc_hal_set_calibration_param(adc_n, s_adc_cali_param[adc_n][atten]);
}

#if SOC_ADC_CALIB_CHAN_COMPENS_SUPPORTED
static int s_adc_cali_chan_compens[SOC_ADC_MAX_CHANNEL_NUM][SOC_ADC_ATTEN_NUM] = {};
void adc_load_hw_calibration_chan_compens(adc_unit_t adc_n, adc_channel_t chan, adc_atten_t atten)
{
    int version = esp_efuse_rtc_calib_get_ver();
    if ((version >= ESP_EFUSE_ADC_CALIB_VER_MIN) &&
        (version <= ESP_EFUSE_ADC_CALIB_VER_MAX)) {
        // Guarantee the calibration version before calling efuse function
        s_adc_cali_chan_compens[chan][atten] = esp_efuse_rtc_calib_get_chan_compens(version, adc_n, chan, atten);
    }
    // No warning when version doesn't match because should has warned in adc_calc_hw_calibration_code
}

int IRAM_ATTR adc_get_hw_calibration_chan_compens(adc_unit_t adc_n, adc_channel_t chan, adc_atten_t atten)
{
    return s_adc_cali_chan_compens[chan][atten];
}
#endif  // SOC_ADC_CALIB_CHAN_COMPENS_SUPPORTED
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

static portMUX_TYPE s_spinlock = portMUX_INITIALIZER_UNLOCKED;

/*------------------------------------------------------------------------------
* For those who use APB_SARADC periph
*----------------------------------------------------------------------------*/
static int s_adc_digi_ctrlr_cnt;

void adc_apb_periph_claim(void)
{
    portENTER_CRITICAL(&s_spinlock);
    s_adc_digi_ctrlr_cnt++;
    if (s_adc_digi_ctrlr_cnt == 1) {
        //enable ADC digital part
        periph_module_enable(PERIPH_SARADC_MODULE);
        //reset ADC digital part
        periph_module_reset(PERIPH_SARADC_MODULE);
    }

    portEXIT_CRITICAL(&s_spinlock);
}

void adc_apb_periph_free(void)
{
    portENTER_CRITICAL(&s_spinlock);
    s_adc_digi_ctrlr_cnt--;
    if (s_adc_digi_ctrlr_cnt == 0) {
        periph_module_disable(PERIPH_SARADC_MODULE);
    } else if (s_adc_digi_ctrlr_cnt < 0) {
        portEXIT_CRITICAL(&s_spinlock);
        ESP_LOGE(TAG, "%s called, but `s_adc_digi_ctrlr_cnt == 0`", __func__);
        abort();
    }

    portEXIT_CRITICAL(&s_spinlock);
}
