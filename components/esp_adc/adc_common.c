/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/adc_private.h"
#include "driver/gpio.h"
#include "hal/adc_hal.h"
#include "hal/adc_hal_common.h"
#include "hal/adc_hal_conf.h"
#include "soc/adc_periph.h"

//For calibration
#if CONFIG_IDF_TARGET_ESP32S2
#include "esp_efuse_rtc_table.h"
#elif SOC_ADC_CALIBRATION_V1_SUPPORTED
#include "esp_efuse_rtc_calib.h"
#endif

static const char *TAG = "adc_common";
static portMUX_TYPE s_spinlock = portMUX_INITIALIZER_UNLOCKED;
extern portMUX_TYPE rtc_spinlock;

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

/*---------------------------------------------------------------
            ADC IOs
---------------------------------------------------------------*/
esp_err_t adc_io_to_channel(int io_num, adc_unit_t *unit_id, adc_channel_t *channel)
{
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(io_num), ESP_ERR_INVALID_ARG, TAG, "invalid gpio number");
    ESP_RETURN_ON_FALSE(unit_id && channel, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    bool found = false;
    for (int i = 0; i < SOC_ADC_PERIPH_NUM; i++) {
        for (int j = 0; j < SOC_ADC_MAX_CHANNEL_NUM; j++) {
            if (adc_channel_io_map[i][j] == io_num) {
                *channel = j;
                *unit_id = i;
                found = true;
            }
        }
    }
    return (found) ? ESP_OK : ESP_ERR_NOT_FOUND;
}

esp_err_t adc_channel_to_io(adc_unit_t unit_id, adc_channel_t channel, int *io_num)
{
    ESP_RETURN_ON_FALSE(unit_id < SOC_ADC_PERIPH_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid unit");
    ESP_RETURN_ON_FALSE(channel < SOC_ADC_CHANNEL_NUM(unit_id), ESP_ERR_INVALID_ARG, TAG, "invalid channel");
    ESP_RETURN_ON_FALSE(io_num, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    *io_num = adc_channel_io_map[unit_id][channel];
    return ESP_OK;
}

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
/*---------------------------------------------------------------
            ADC Hardware Calibration
---------------------------------------------------------------*/
#if CONFIG_IDF_TARGET_ESP32S2
#define esp_efuse_rtc_calib_get_ver()     esp_efuse_rtc_table_read_calib_version()

static inline uint32_t esp_efuse_rtc_calib_get_init_code(int version, uint32_t adc_unit, int atten)
{
    int tag = esp_efuse_rtc_table_get_tag(version, adc_unit + 1, atten, RTCCALIB_V2_PARAM_VINIT);
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

static __attribute__((constructor)) void adc_hw_calibration(void)
{
    //Calculate all ICode
    for (int i = 0; i < SOC_ADC_PERIPH_NUM; i++) {
        adc_hal_calibration_init(i);
        for (int j = 0; j < SOC_ADC_ATTEN_NUM; j++) {
            /**
             * This may get wrong when attenuations are NOT consecutive on some chips,
             * update this when bringing up the calibration on that chip
             */
            adc_calc_hw_calibration_code(i, j);
        }
    }
}
#endif //#if SOC_ADC_CALIBRATION_V1_SUPPORTED
