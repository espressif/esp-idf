/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "esp_efuse_rtc_table.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_log.h"
#include "hal/adc_types.h"
#include "hal/efuse_ll.h"
#include "soc/soc_caps.h"

#define RTC_TBL_LOG_TAG "efuse_rtc_table"



/* Note on definition of tags
 *
 * For adc calibration, value = raw * multiplier + offset, but these values are kind of arbitrary so
 * we use a lookup table to do the bookkeeping.
 *
 * The offset of an item can be calculated as follows:
 * PARAM_OFFSET + ADC_NUM(which is the UNIT_COUNT minus 1) * ATTEN_NUM + ATTEN_NUM
 * where PARAM_OFFSET is the index of the first item.
 *
 * ADC, ATTEN form a 2-dim array. For each (version number, extra parameters) tuple we keep a such array,
 * and use if-else statements to choose which array we use.
 * */

#define RTCCALIB_V1_ADCREADINGLOW_OFFSET RTCCALIB_V1IDX_A10L
#define RTCCALIB_V1_ADCREADINGHIGH_OFFSET RTCCALIB_V1IDX_A10H
#define RTCCALIB_V2_ADCREADINGHIGH_OFFSET RTCCALIB_V2IDX_A10H
#define RTCCALIB_V2_ADCREADINGINIT_OFFSET RTCCALIB_V2IDX_A10I

typedef struct {
    const int tag; // should be the same as the index in adc_efuse_raw_map
    const int block;
    const int begin_bit;
    const int length;
    const int multiplier;
    const int base;
    const int depends;
} efuse_map_info_t;

static const efuse_map_info_t adc_efuse_raw_map[] = {
    {0},
    //   INDEXING TAG,       BLOCK,   BEGIN_BIT,  LENGTH,  MULTIPLIER,  OFFSET BASE,             OFFSET DEP
    {RTCCALIB_V1IDX_A10L,         2,         208,      6,           4,         2231,                      0},
    {RTCCALIB_V1IDX_A11L,         2,         214,      6,           4,         1643,                      0},
    {RTCCALIB_V1IDX_A12L,         2,         220,      6,           4,         1290,                      0},
    {RTCCALIB_V1IDX_A13L,         2,         226,      6,           4,          701,                      0},
    {RTCCALIB_V1IDX_A20L,         2,         232,      6,           4,         2305,                      0},
    {RTCCALIB_V1IDX_A21L,         2,         238,      6,           4,         1693,                      0},
    {RTCCALIB_V1IDX_A22L,         2,         244,      6,           4,         1343,                      0},
    {RTCCALIB_V1IDX_A23L,         2,         250,      6,           4,          723,                      0},

    {RTCCALIB_V1IDX_A10H,         2,         144,      8,           4,         5775,                      0},
    {RTCCALIB_V1IDX_A11H,         2,         152,      8,           4,         5693,                      0},
    {RTCCALIB_V1IDX_A12H,         2,         160,      8,           4,         5723,                      0},
    {RTCCALIB_V1IDX_A13H,         2,         168,      8,           4,         6209,                      0},
    {RTCCALIB_V1IDX_A20H,         2,         176,      8,           4,         5817,                      0},
    {RTCCALIB_V1IDX_A21H,         2,         184,      8,           4,         5703,                      0},
    {RTCCALIB_V1IDX_A22H,         2,         192,      8,           4,         5731,                      0},
    {RTCCALIB_V1IDX_A23H,         2,         200,      8,           4,         6157,                      0},

    {RTCCALIB_V2IDX_A10H,         2,         197,      6,           2,          169,     RTCCALIB_V2IDX_A12H},
    {RTCCALIB_V2IDX_A11H,         2,         203,      6,           2,          -26,     RTCCALIB_V2IDX_A12H},
    {RTCCALIB_V2IDX_A12H,         2,         209,      9,           2,          126,     RTCCALIB_V2IDX_A21H},
    {RTCCALIB_V2IDX_A13H,         2,         218,      7,           2,          387,     RTCCALIB_V2IDX_A12H},
    {RTCCALIB_V2IDX_A20H,         2,         225,      7,           2,          177,     RTCCALIB_V2IDX_A21H},
    {RTCCALIB_V2IDX_A21H,         2,         232,     10,           2,         5815,                       0},
    {RTCCALIB_V2IDX_A22H,         2,         242,      7,           2,           27,     RTCCALIB_V2IDX_A21H},
    {RTCCALIB_V2IDX_A23H,         2,         249,      7,           2,          410,     RTCCALIB_V2IDX_A21H},

    {RTCCALIB_V2IDX_A10I,         2,         147,      8,           2,         1519,                       0},
    {RTCCALIB_V2IDX_A11I,         2,         155,      6,           2,           88,     RTCCALIB_V2IDX_A10I},
    {RTCCALIB_V2IDX_A12I,         2,         161,      5,           2,            8,     RTCCALIB_V2IDX_A11I},
    {RTCCALIB_V2IDX_A13I,         2,         166,      6,           2,           70,     RTCCALIB_V2IDX_A12I},
    {RTCCALIB_V2IDX_A20I,         2,         172,      8,           2,         1677,                       0},
    {RTCCALIB_V2IDX_A21I,         2,         180,      6,           2,           23,     RTCCALIB_V2IDX_A20I},
    {RTCCALIB_V2IDX_A22I,         2,         186,      5,           2,            6,     RTCCALIB_V2IDX_A21I},
    {RTCCALIB_V2IDX_A23I,         2,         191,      6,           2,           13,     RTCCALIB_V2IDX_A22I},

    {RTCCALIB_IDX_TMPSENSOR,      2,         135,      9,           1,            0,                       0},

};


int esp_efuse_rtc_table_read_calib_version(void)
{
    return efuse_ll_get_blk_version_minor();
}

int esp_efuse_rtc_table_get_tag(int version, int adc_num, int atten, int extra_params)
{
    assert(adc_num <= ADC_UNIT_2);
    int index = (adc_num == ADC_UNIT_1) ? 0 : 1;
    int param_offset; // used to index which (adc_num, atten) array to use.
    if (version == 1 && extra_params == RTCCALIB_V1_PARAM_VLOW) { // Volage LOW, Version 1
        param_offset = RTCCALIB_V1_ADCREADINGLOW_OFFSET;
    } else if (version == 1 && extra_params == RTCCALIB_V1_PARAM_VHIGH) {
        param_offset = RTCCALIB_V1_ADCREADINGHIGH_OFFSET;
    } else if (version == 2 && extra_params == RTCCALIB_V2_PARAM_VHIGH) {
        param_offset = RTCCALIB_V2_ADCREADINGHIGH_OFFSET;
    } else if (version == 2 && extra_params == RTCCALIB_V2_PARAM_VINIT) {
        param_offset = RTCCALIB_V2_ADCREADINGINIT_OFFSET;
    } else {
        return -1;
    }

    int result = param_offset + index * RTCCALIB_ESP32S2_ATTENCOUNT + atten;
    ESP_EARLY_LOGV(RTC_TBL_LOG_TAG, "V%d ADC%d ATTEN%d PARAM%d -> %d", version, adc_num + 1, atten, extra_params, result);
    return result;
}

/*
 * Converts a signed-bit int to a normal (2-complement) int.
 * */
static int signed_bit_to_int(uint32_t number, int len)
{
    if (number >> (len - 1)) {
        // first bit is set, unset that bit and negate the number.
        number = -(number ^ (1 << (len - 1)));
    }
    return number;
}

int esp_efuse_rtc_table_get_raw_efuse_value(int tag)
{
    assert(tag > 0);
    if (tag == 0) {
        return 0;
    }
    uint32_t val = 0;
    esp_efuse_read_block(adc_efuse_raw_map[tag].block, &val, adc_efuse_raw_map[tag].begin_bit, adc_efuse_raw_map[tag].length);
    int result = signed_bit_to_int(val, adc_efuse_raw_map[tag].length);
    ESP_EARLY_LOGV(RTC_TBL_LOG_TAG, "Fetching raw for tag %d @blk%d bit%d len%d: %d", tag, adc_efuse_raw_map[tag].block, adc_efuse_raw_map[tag].begin_bit, adc_efuse_raw_map[tag].length,
                   result);
    return result;
}

int esp_efuse_rtc_table_get_parsed_efuse_value(int tag, bool skip_efuse_reading)
{
    assert(tag >= 0);
    if (tag == 0) {
        return 0;    // tag 0 is the dummy tag and has no value. (used by depends)
    }

    int efuse_val = 0;
    if (!skip_efuse_reading) {
        efuse_val = esp_efuse_rtc_table_get_raw_efuse_value(tag) * adc_efuse_raw_map[tag].multiplier;
    }
    int result = efuse_val + adc_efuse_raw_map[tag].base +
                 esp_efuse_rtc_table_get_parsed_efuse_value(adc_efuse_raw_map[tag].depends, skip_efuse_reading);

    ESP_EARLY_LOGV(RTC_TBL_LOG_TAG, "Parsed efuse val for tag %d: %d", tag, result);
    return result;
}
