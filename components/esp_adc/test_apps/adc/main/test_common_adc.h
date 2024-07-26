/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "unity.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "esp_private/adc_private.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX
#define MAX(a, b)         (((a) > (b)) ? (a) : (b))
#endif

/*---------------------------------------------------------------
        ADC Level
---------------------------------------------------------------*/
/**
 * We use weak pulldown, `ADC_TEST_LOW_THRESH` may vary.
 * If connect to GND, `ADC_TEST_LOW_THRESH` can be smaller
 */
#if CONFIG_IDF_TARGET_ESP32
#define ADC_TEST_LOW_VAL         0
#define ADC_TEST_LOW_THRESH      10

#define ADC_TEST_HIGH_VAL        4095
#define ADC_TEST_HIGH_VAL_DMA    4095
#define ADC_TEST_HIGH_THRESH     10

#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_TEST_LOW_VAL         0
#define ADC_TEST_LOW_THRESH      35

#define ADC_TEST_HIGH_VAL        8191
#define ADC_TEST_HIGH_VAL_DMA    4095
#define ADC_TEST_HIGH_THRESH     10

#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_TEST_LOW_VAL         0
#define ADC_TEST_LOW_THRESH      60     //This is due to ADC2 accuracy is not as good as ADC1, and also we use weak pulldown

#define ADC_TEST_HIGH_VAL        4095
#define ADC_TEST_HIGH_VAL_DMA    4095
#define ADC_TEST_HIGH_THRESH     10

#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_TEST_LOW_VAL         0
#define ADC_TEST_LOW_THRESH      15

#define ADC_TEST_HIGH_VAL        4095
#define ADC_TEST_HIGH_VAL_DMA    4095
#define ADC_TEST_HIGH_THRESH     0

#elif CONFIG_IDF_TARGET_ESP32C2
#define ADC_TEST_LOW_VAL         0
#define ADC_TEST_LOW_THRESH      15

#define ADC_TEST_HIGH_VAL        3400
#define ADC_TEST_HIGH_THRESH     200

#elif CONFIG_IDF_TARGET_ESP32C6
#define ADC_TEST_LOW_VAL         0
#define ADC_TEST_LOW_THRESH      15

#define ADC_TEST_HIGH_VAL        3350
#define ADC_TEST_HIGH_VAL_DMA    4081
#define ADC_TEST_HIGH_THRESH     200

#elif CONFIG_IDF_TARGET_ESP32H2
#define ADC_TEST_LOW_VAL         0
#define ADC_TEST_LOW_THRESH      17

#define ADC_TEST_HIGH_VAL        3390
#define ADC_TEST_HIGH_VAL_DMA    4081
#define ADC_TEST_HIGH_THRESH     200

#elif CONFIG_IDF_TARGET_ESP32P4
#define ADC_TEST_LOW_VAL         3100
#define ADC_TEST_LOW_THRESH      200

#define ADC_TEST_HIGH_VAL        4095
#define ADC_TEST_HIGH_VAL_DMA    4095
#define ADC_TEST_HIGH_THRESH     200

#elif CONFIG_IDF_TARGET_ESP32C5
#define ADC_TEST_LOW_VAL         2169
#define ADC_TEST_LOW_THRESH      200

#define ADC_TEST_HIGH_VAL        4095
#define ADC_TEST_HIGH_VAL_DMA    4095
#define ADC_TEST_HIGH_THRESH     200

#endif

/*---------------------------------------------------------------
        ADC Attenuation
---------------------------------------------------------------*/
#if CONFIG_IDF_TARGET_ESP32C2
#define TEST_ATTEN_NUMS                 2
extern adc_atten_t g_test_atten[TEST_ATTEN_NUMS];
#else
#define TEST_ATTEN_NUMS                 4
extern adc_atten_t g_test_atten[TEST_ATTEN_NUMS];
#endif

/*---------------------------------------------------------------
        ADC Filter
---------------------------------------------------------------*/
#if SOC_ADC_DIG_IIR_FILTER_SUPPORTED
#define TEST_FILTER_COEFF_NUMS          5
extern adc_digi_iir_filter_coeff_t g_test_filter_coeff[TEST_FILTER_COEFF_NUMS];
#endif

/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
/**
 * @brief Initialise ADC Calibration
 *
 * @param[in]  unit         ADC unit
 * @param[in]  channel      ADC channel
 * @param[in]  atten        ADC attenuation
 * @param[in]  bitwidth     ADC bit width
 * @param[out] out_handle   ADC calibration handle
 *
 * @return
 *        - True  Calibration success
 *        - False Calibration fail
 */
bool test_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_bitwidth_t bitwidth, adc_cali_handle_t *out_handle);

/**
 * @brief De-initialise ADC Calibration
 *
 * @param[in] handle    ADC calibration handle
 */
void test_adc_calibration_deinit(adc_cali_handle_t handle);

/*---------------------------------------------------------------
        ADC GPIO
---------------------------------------------------------------*/
/**
 * @brief Set ADC IO level
 *
 * @param[in] unit      ADC unit
 * @param[in] channel   ADC channel
 * @param[in] level     IO level. True: high; False: low
 */
void test_adc_set_io_level(adc_unit_t unit, adc_channel_t channel, bool level);

/**
 * @brief Set ADC IO to a middle level
 *
 * @note We don't expect this IO to have a fixed level among different chips.
 *       We just need the IO to be stable at a certain level, which is neither 0 nor overflow.
 *
 * @param[in] unit      ADC unit
 * @param[in] channel   ADC channel
 */
void test_adc_set_io_middle(adc_unit_t unit, adc_channel_t channel);

#ifdef __cplusplus
}
#endif
