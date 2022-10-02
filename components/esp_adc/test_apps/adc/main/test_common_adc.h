/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
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
        ADC Calibration
---------------------------------------------------------------*/
/**
 * @brief Initialise ADC Calibration
 *
 * @param[out] out_handle    ADC calibration handle
 *
 * @return
 *        - True  Calibration success
 *        - False Calibration fail
 */
bool test_adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_bitwidth_t bitwidth, adc_cali_handle_t *out_handle);

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
#define ADC_TEST_LOW_VAL         2147
#define ADC_TEST_LOW_THRESH      100

#define ADC_TEST_HIGH_VAL        4095
#define ADC_TEST_HIGH_THRESH     0
#endif

/**
 * @brief Set ADC IO level
 *
 * @param[in] unit      ADC unit
 * @param[in] channel   ADC channel
 * @param[in] level     IO level. True: high; False: low
 */
void test_adc_set_io_level(adc_unit_t unit, adc_channel_t channel, bool level);

#ifdef __cplusplus
}
#endif
