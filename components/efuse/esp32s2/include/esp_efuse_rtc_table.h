/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "sdkconfig.h"

#define RTCCALIB_ESP32S2_ADCCOUNT 2
#define RTCCALIB_ESP32S2_ATTENCOUNT 4

#define RTCCALIB_V1_PARAM_VLOW 0
#define RTCCALIB_V1_PARAM_VHIGH 1
#define RTCCALIB_V2_PARAM_VHIGH 0
#define RTCCALIB_V2_PARAM_VINIT 1

// these are the tags. Either use them directly or use esp_efuse_rtc_table_get_tag to calculate
// the corresponding tag.
#define RTCCALIB_V1IDX_A10L 1
#define RTCCALIB_V1IDX_A11L 2
#define RTCCALIB_V1IDX_A12L 3
#define RTCCALIB_V1IDX_A13L 4
#define RTCCALIB_V1IDX_A20L 5
#define RTCCALIB_V1IDX_A21L 6
#define RTCCALIB_V1IDX_A22L 7
#define RTCCALIB_V1IDX_A23L 8
#define RTCCALIB_V1IDX_A10H 9
#define RTCCALIB_V1IDX_A11H 10
#define RTCCALIB_V1IDX_A12H 11
#define RTCCALIB_V1IDX_A13H 12
#define RTCCALIB_V1IDX_A20H 13
#define RTCCALIB_V1IDX_A21H 14
#define RTCCALIB_V1IDX_A22H 15
#define RTCCALIB_V1IDX_A23H 16
#define RTCCALIB_V2IDX_A10H 17
#define RTCCALIB_V2IDX_A11H 18
#define RTCCALIB_V2IDX_A12H 19
#define RTCCALIB_V2IDX_A13H 20
#define RTCCALIB_V2IDX_A20H 21
#define RTCCALIB_V2IDX_A21H 22
#define RTCCALIB_V2IDX_A22H 23
#define RTCCALIB_V2IDX_A23H 24
#define RTCCALIB_V2IDX_A10I 25
#define RTCCALIB_V2IDX_A11I 26
#define RTCCALIB_V2IDX_A12I 27
#define RTCCALIB_V2IDX_A13I 28
#define RTCCALIB_V2IDX_A20I 29
#define RTCCALIB_V2IDX_A21I 30
#define RTCCALIB_V2IDX_A22I 31
#define RTCCALIB_V2IDX_A23I 32
#define RTCCALIB_IDX_TMPSENSOR 33

/**
 * @brief Get rtc calibration version.
 */
int esp_efuse_rtc_table_read_calib_version(void);

/**
 * @brief Helper function to calculate a tag from human-readable parameters.
 * Tag is used to index the desired data from the efuse.
 * For example, (1, 1, 3, 1) yields the tag RTCCALIB_V1IDX_A13H
 * extra params are used for identification when a adc_num-atten combination has
 * multiple efuse values.
 * @param adc_channel_num verbatim numbering of the ADC channel. For channel 1, use 1 and not 0.
 * @param atten attenuation. use the enum value.
 * @param version the version of the scheme to index for.
 * @param extra_params defined differently for each version.
 * */
int esp_efuse_rtc_table_get_tag(int version, int adc_channel_num, int atten, int extra_params);

/**
 * @brief Fetches a raw value from efuse and does signed bit parsing
 * @param tag tag obtained with esp_efuse_rtc_table_get_tag
 *
 * */
int esp_efuse_rtc_table_get_raw_efuse_value(int tag);

/**
 * @brief Fetches a raw value from efuse and resolve it to get
 * the original number that it meant to represent.
 *
 * @param tag tag obtained with esp_efuse_rtc_table_get_tag
 * @param use_zero_inputs Does not perform the raw value fetching before resolving the number,
 * but proceed as if all zeros were read from efuse.
 *
 * */
int esp_efuse_rtc_table_get_parsed_efuse_value(int tag, bool skip_efuse_reading);

#ifdef __cplusplus
}
#endif
