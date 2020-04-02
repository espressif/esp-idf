// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for ADC (esp32 specific part)

#pragma once

#include "hal/adc_ll.h"
#include "hal/adc_types.h"

#include_next "hal/adc_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool conv_limit_en;         /*!<Enable max conversion number detection for digital controller.
                                    If the number of ADC conversion is equal to the `limit_num`, the conversion is stopped. */
    uint32_t conv_limit_num;    /*!<ADC max conversion number for digital controller. */
    uint32_t adc1_pattern_len;  /*!<Pattern table length for digital controller. Range: 0 ~ 16.
                                    The pattern table that defines the conversion rules for each SAR ADC. Each table has 16 items, in which channel selection,
                                    resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
                                    pattern table one by one. For each controller the scan sequence has at most 16 different rules before repeating itself. */
    uint32_t adc2_pattern_len;  /*!<Refer to `adc1_pattern_len` */
    adc_hal_digi_pattern_table_t *adc1_pattern;   /*!<Pointer to pattern table for digital controller. The table size defined by `adc1_pattern_len`. */
    adc_hal_digi_pattern_table_t *adc2_pattern;   /*!<Refer to `adc1_pattern` */
    adc_hal_digi_convert_mode_t conv_mode;        /*!<ADC conversion mode for digital controller. ESP32 only support ADC1 single mode. */
    adc_hal_digi_output_format_t format;      /*!<ADC output data format for digital controller. */
    uint32_t clk_div;           /*!< ADC module clock division factor. ADC clock divided from APB clock.*/
} adc_hal_digi_config_t;

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

/**
 * Set I2S DMA data source for digital controller.
 *
 * @param src i2s data source.
 */
#define adc_hal_digi_set_data_source(src) adc_ll_digi_set_data_source(src)

/**
 * Setting the digital controller.
 *
 * @prarm adc_digi_config_t cfg Pointer to digital controller paramter.
 */
void adc_hal_digi_controller_config(const adc_hal_digi_config_t *cfg);

/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/

/**
 * @brief ADC digital controller initialization.
 */
void adc_hal_digi_init(void);

/**
 * @brief ADC digital controller deinitialization.
 */
void adc_hal_digi_deinit(void);

/*---------------------------------------------------------------
                    Hall sensor setting
---------------------------------------------------------------*/

/**
 * Enable hall sensor.
 */
#define adc_hal_hall_enable() adc_ll_hall_enable()

/**
 * Disable hall sensor.
 */
#define adc_hal_hall_disable() adc_ll_hall_disable()

/**
 *  Start hall convert and return the hall value.
 *
 *  @return Hall value.
 */
int adc_hal_hall_convert(void);

/**
 *  @brief Output ADC2 reference voltage to gpio
 *
 *  This function utilizes the testing mux exclusive to ADC2 to route the
 *  reference voltage one of ADC2's channels.
 *
 *  @param[in]  io    GPIO number
 *  @return
 *                  - true: v_ref successfully routed to selected gpio
 *                  - false: Unsupported gpio
 */
#define adc_hal_vref_output(io) adc_ll_vref_output(io)

#ifdef __cplusplus
}
#endif