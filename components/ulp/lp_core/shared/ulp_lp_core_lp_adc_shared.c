/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#if SOC_LP_ADC_SUPPORTED

#include "ulp_lp_core_lp_adc_shared.h"
#include "hal/adc_types.h"
#include "hal/adc_ll.h"
#include "hal/adc_hal_common.h"
#include "esp_private/adc_share_hw_ctrl.h"

#define VREF            (1100)  /* Internal Reference voltage in millivolts (1.1V) */
#define INV_ATTEN_0DB   (1000)  /* Inverse of 10^0 * 1000 */
#define INV_ATTEN_2_5DB (1335)  /* Inverse of 10^(-2.5/20) * 1000 */
#define INV_ATTEN_6DB   (1996)  /* Inverse of 10^(-6/20) * 1000 */
#define INV_ATTEN_12DB  (3984)  /* Inverse of 10^(-12/20) * 1000 */

/* ADC unit handles - one for each supported ADC unit */
static adc_oneshot_unit_handle_t s_adc_unit_handles[SOC_ADC_PERIPH_NUM] = {NULL};

/************************************************** Static Helper Functions **************************************************/

static esp_err_t lp_adc_validate_unit(adc_unit_t unit_id)
{
    /* Currently, LP ADC2 does not work during sleep (DIG-396)
     * TODO: Remove this check once DIG-396 is fixed
     */
    if (unit_id != ADC_UNIT_1) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Verify unit ID is within valid range */
    if (unit_id >= SOC_ADC_PERIPH_NUM) {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

static esp_err_t lp_adc_validate_channel(adc_unit_t unit_id, adc_channel_t channel)
{
    if (channel >= SOC_ADC_CHANNEL_NUM(unit_id)) {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

/************************************************** Public API **************************************************/

esp_err_t lp_core_lp_adc_init(adc_unit_t unit_id)
{
    esp_err_t ret = lp_adc_validate_unit(unit_id);
    if (ret != ESP_OK) {
        return ret;
    }

#if IS_ULP_COCPU
    // Not supported from LP core
    return ESP_ERR_NOT_SUPPORTED;
#else
    /* LP ADC is being initialized from the HP core.
     * Hence, we use the standard ADC driver APIs here.
     */

    /* Initialize ADC */
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = unit_id,
        .ulp_mode = ADC_ULP_MODE_LP_CORE, // LP Core will use the ADC
    };

    return (adc_oneshot_new_unit(&init_config, &s_adc_unit_handles[unit_id]));
#endif /* IS_ULP_COCPU */
}

esp_err_t lp_core_lp_adc_deinit(adc_unit_t unit_id)
{
    esp_err_t ret = lp_adc_validate_unit(unit_id);
    if (ret != ESP_OK) {
        return ret;
    }

#if IS_ULP_COCPU
    // Not supported from LP core
    return ESP_ERR_NOT_SUPPORTED;
#else
    return (adc_oneshot_del_unit(s_adc_unit_handles[unit_id]));
#endif /* IS_ULP_COCPU */
}

esp_err_t lp_core_lp_adc_config_channel(adc_unit_t unit_id, adc_channel_t channel, const lp_core_lp_adc_chan_cfg_t *chan_config)
{
    esp_err_t ret = lp_adc_validate_unit(unit_id);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = lp_adc_validate_channel(unit_id, channel);
    if (ret != ESP_OK) {
        return ret;
    }

#if IS_ULP_COCPU
    // Not supported from LP core
    return ESP_ERR_NOT_SUPPORTED;
#else
    adc_oneshot_chan_cfg_t config = {
        .atten = chan_config->atten,
        .bitwidth = chan_config->bitwidth,
    };

    ret = adc_oneshot_config_channel(s_adc_unit_handles[unit_id], channel, &config);
    if (ret != ESP_OK) {
        return ret;
    }

    /* Set the calibration parameters for the ADC unit and channel */
#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    adc_hal_calibration_init(unit_id);
    adc_set_hw_calibration_code(unit_id, chan_config->atten);
#endif /* SOC_ADC_CALIBRATION_V1_SUPPORTED */
#endif /* IS_ULP_COCPU */

    return ESP_OK;
}

esp_err_t lp_core_lp_adc_read_channel_raw(adc_unit_t unit_id, adc_channel_t channel, int *adc_raw)
{
    if (adc_raw == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = lp_adc_validate_unit(unit_id);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = lp_adc_validate_channel(unit_id, channel);
    if (ret != ESP_OK) {
        return ret;
    }

#if IS_ULP_COCPU
    uint32_t event = ADC_LL_EVENT_ADC1_ONESHOT_DONE;

    adc_oneshot_ll_clear_event(event);
    adc_oneshot_ll_disable_all_unit();
    adc_oneshot_ll_enable(unit_id);
    adc_oneshot_ll_set_channel(unit_id, channel);

    adc_oneshot_ll_start(unit_id);
    while (!adc_oneshot_ll_get_event(event)) {
        ;
    }
    *adc_raw = adc_oneshot_ll_get_raw_result(unit_id);

    adc_oneshot_ll_disable_all_unit();
#else
    return (adc_oneshot_read(s_adc_unit_handles[unit_id], channel, adc_raw));
#endif /* IS_ULP_COCPU */

    return ESP_OK;
}

esp_err_t lp_core_lp_adc_read_channel_converted(adc_unit_t unit_id, adc_channel_t channel, int *voltage_mv)
{
    if (voltage_mv == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = lp_adc_validate_unit(unit_id);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = lp_adc_validate_channel(unit_id, channel);
    if (ret != ESP_OK) {
        return ret;
    }

    /* Read the raw ADC value */
    int adc_raw;
    ret = lp_core_lp_adc_read_channel_raw(unit_id, channel, &adc_raw);
    if (ret != ESP_OK) {
        return ret;
    }

    /* On the esp32p4, the ADC raw value can be 12-bit wide. The internal Vref is 1.1V.
     * The formula to convert the raw value to voltage is:
     * voltage = (((raw_value / (2^12 - 1)) * 1.1V) * attenuation)
     *
     * To avoid many floating point calculations, we precompute the attenuation factors
     * and perform the conversion in millivolts instead of volts.
     */

    int measured_voltage = adc_raw * VREF; // millivolts
    measured_voltage /= 4095;

    adc_atten_t atten = adc_ll_get_atten(unit_id, channel);
    switch (atten) {
    case ADC_ATTEN_DB_0:
        *voltage_mv = (measured_voltage * INV_ATTEN_0DB) / 1000;
        break;
    case ADC_ATTEN_DB_2_5:
        *voltage_mv = (measured_voltage * INV_ATTEN_2_5DB) / 1000;
        break;
    case ADC_ATTEN_DB_6:
        *voltage_mv = (measured_voltage * INV_ATTEN_6DB) / 1000;
        break;
    case ADC_ATTEN_DB_12:
        *voltage_mv = (measured_voltage * INV_ATTEN_12DB) / 1000;
        break;
    default:
        ret = ESP_ERR_INVALID_STATE;
    }

    return ret;
}

#endif /* CONFIG_SOC_LP_ADC_SUPPORTED */
