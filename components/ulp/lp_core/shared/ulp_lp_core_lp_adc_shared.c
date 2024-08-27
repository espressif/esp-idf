/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#if SOC_LP_ADC_SUPPORTED

#include "ulp_lp_core_lp_adc_shared.h"
#include "hal/adc_types.h"
#include "hal/adc_ll.h"

#define VREF            (1100)  /* Internal Reference voltage in millivolts (1.1V) */
#define INV_ATTEN_0DB   (1000)  /* Inverse of 10^0 * 1000 */
#define INV_ATTEN_2_5DB (1335)  /* Inverse of 10^(-2.5/20) * 1000 */
#define INV_ATTEN_6DB   (1996)  /* Inverse of 10^(-6/20) * 1000 */
#define INV_ATTEN_12DB  (3984)  /* Inverse of 10^(-12/20) * 1000 */

adc_oneshot_unit_handle_t s_adc1_handle;

esp_err_t lp_core_lp_adc_init(adc_unit_t unit_id)
{
    if (unit_id != ADC_UNIT_1) {
        // TODO: LP ADC2 does not work during sleep (DIG-396)
        // For now, we do not allow LP ADC2 usage.
        return ESP_ERR_INVALID_ARG;
    }

#if IS_ULP_COCPU
    // Not supported
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

    return (adc_oneshot_new_unit(&init_config, &s_adc1_handle));
#endif /* IS_ULP_COCPU */
}

esp_err_t lp_core_lp_adc_deinit(adc_unit_t unit_id)
{
    if (unit_id != ADC_UNIT_1) {
        return ESP_ERR_INVALID_ARG;
    }

#if IS_ULP_COCPU
    // Not supported
    return ESP_ERR_NOT_SUPPORTED;
#else
    return (adc_oneshot_del_unit(s_adc1_handle));
#endif /* IS_ULP_COCPU */
}

esp_err_t lp_core_lp_adc_config_channel(adc_unit_t unit_id, adc_channel_t channel, const lp_core_lp_adc_chan_cfg_t *chan_config)
{
    if (unit_id != ADC_UNIT_1) {
        // TODO: LP ADC2 does not work during sleep (DIG-396)
        // For now, we do not allow LP ADC2 usage.
        return ESP_ERR_INVALID_ARG;
    }
#if IS_ULP_COCPU
    // Not supported
    return ESP_ERR_NOT_SUPPORTED;
#else
    adc_oneshot_chan_cfg_t config = {
        .atten = chan_config->atten,
        .bitwidth = chan_config->bitwidth,
    };

    return (adc_oneshot_config_channel(s_adc1_handle, channel, &config));
#endif /* IS_ULP_COCPU */
}

esp_err_t lp_core_lp_adc_read_channel_raw(adc_unit_t unit_id, adc_channel_t channel, int *adc_raw)
{
    if (unit_id != ADC_UNIT_1 || adc_raw == NULL) {
        return ESP_ERR_INVALID_ARG;
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
    return (adc_oneshot_read(s_adc1_handle, channel, adc_raw));
#endif /* IS_ULP_COCPU */

    return ESP_OK;
}

esp_err_t lp_core_lp_adc_read_channel_converted(adc_unit_t unit_id, adc_channel_t channel, int *voltage_mv)
{
    esp_err_t ret = ESP_OK;

    if (unit_id != ADC_UNIT_1 || voltage_mv == NULL) {
        return ESP_ERR_INVALID_ARG;
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
