/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/adc_efuse_hal.h"
#include "hal/adc_ll.h"
#include "hal/adc_types.h"
#include "hal/efuse_ll.h"

typedef enum {
    ADC_EFUSE_CALI_POINT_LOW,
    ADC_EFUSE_CALI_POINT_HIGH,
    ADC_EFUSE_CALI_POINT_INIT,
} adc_efuse_cali_point_t;

typedef struct {
    uint16_t bit;
    uint8_t length;
    int16_t multiplier;
    int16_t base;
    uint8_t depends;
} adc_efuse_cali_map_t;

enum {
    ADC_EFUSE_V1_A10L = 1,
    ADC_EFUSE_V1_A10H = 9,
    ADC_EFUSE_V2_A10H = 17,
    ADC_EFUSE_V2_A10I = 25,
};

static const adc_efuse_cali_map_t s_adc_efuse_cali_map[] = {
    {  0,  0, 0,    0,  0},
    {208,  6, 4, 2231,  0}, {214,  6, 4, 1643,  0}, {220,  6, 4, 1290,  0}, {226,  6, 4,  701,  0},
    {232,  6, 4, 2305,  0}, {238,  6, 4, 1693,  0}, {244,  6, 4, 1343,  0}, {250,  6, 4,  723,  0},
    {144,  8, 4, 5775,  0}, {152,  8, 4, 5693,  0}, {160,  8, 4, 5723,  0}, {168,  8, 4, 6209,  0},
    {176,  8, 4, 5817,  0}, {184,  8, 4, 5703,  0}, {192,  8, 4, 5731,  0}, {200,  8, 4, 6157,  0},
    {197,  6, 2,  169, 19}, {203,  6, 2,  -26, 19}, {209,  9, 2,  126, 22}, {218,  7, 2,  387, 19},
    {225,  7, 2,  177, 22}, {232, 10, 2, 5815,  0}, {242,  7, 2,   27, 22}, {249,  7, 2,  410, 22},
    {147,  8, 2, 1519,  0}, {155,  6, 2,   88, 25}, {161,  5, 2,    8, 26}, {166,  6, 2,   70, 27},
    {172,  8, 2, 1677,  0}, {180,  6, 2,   23, 29}, {186,  5, 2,    6, 30}, {191,  6, 2,   13, 31},
};

static int32_t decode_sign_magnitude(uint32_t value, uint32_t length)
{
    uint32_t sign = 1U << (length - 1);
    return (value & sign) ? -(int32_t)(value ^ sign) : (int32_t)value;
}

static int32_t get_parsed_value(uint32_t tag, bool skip_read)
{
    if (tag == 0) {
        return 0;
    }
    const adc_efuse_cali_map_t *entry = &s_adc_efuse_cali_map[tag];
    int32_t raw = skip_read ? 0 :
                  decode_sign_magnitude(
                      adc_ll_read_efuse_calibration_bits(entry->bit, entry->length), entry->length);
    return raw * entry->multiplier + entry->base +
           get_parsed_value(entry->depends, skip_read);
}

static int get_tag(uint32_t version, uint32_t adc_unit,
                   uint32_t atten, adc_efuse_cali_point_t point)
{
    uint32_t offset;
    if (version == 1 && point == ADC_EFUSE_CALI_POINT_LOW) {
        offset = ADC_EFUSE_V1_A10L;
    } else if (version == 1 && point == ADC_EFUSE_CALI_POINT_HIGH) {
        offset = ADC_EFUSE_V1_A10H;
    } else if (version == 2 && point == ADC_EFUSE_CALI_POINT_HIGH) {
        offset = ADC_EFUSE_V2_A10H;
    } else if (version == 2 && point == ADC_EFUSE_CALI_POINT_INIT) {
        offset = ADC_EFUSE_V2_A10I;
    } else {
        return -1;
    }
    return (int)(offset + adc_unit * 4 + atten);
}

int adc_hal_efuse_get_version(void)
{
    uint32_t version = efuse_ll_get_blk_version_minor();
    if (version < ADC_HAL_EFUSE_CALIB_VER_MIN || version > ADC_HAL_EFUSE_CALIB_VER_MAX) {
        return 0;
    }
    return (int)version;
}

static esp_err_t get_point(int version, uint32_t adc_unit, int atten,
                           adc_efuse_cali_point_t point, uint32_t *out_digi)
{
    if (version < ADC_HAL_EFUSE_CALIB_VER_MIN || version > ADC_HAL_EFUSE_CALIB_VER_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (adc_unit > ADC_UNIT_2 || atten < 0 || atten >= SOC_ADC_ATTEN_NUM) {
        return ESP_ERR_INVALID_ARG;
    }
    int tag = get_tag(version, adc_unit, (uint32_t)atten, point);
    if (tag <= 0) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    *out_digi = (uint32_t)get_parsed_value((uint32_t)tag, false);
    return ESP_OK;
}

uint32_t adc_hal_efuse_get_init_code(int version, uint32_t adc_unit, int atten)
{
    if (version != ADC_HAL_EFUSE_CALIB_VER2) {
        return ADC_HAL_EFUSE_INIT_CODE_INVALID;
    }
    uint32_t code;
    if (get_point(version, adc_unit, atten, ADC_EFUSE_CALI_POINT_INIT, &code) != ESP_OK) {
        return ADC_HAL_EFUSE_INIT_CODE_INVALID;
    }
    return code;
}

esp_err_t adc_hal_efuse_get_cal_voltage(int version, uint32_t adc_unit, int atten,
                                        uint32_t *out_digi, uint32_t *out_vol_mv)
{
    if (!out_digi || !out_vol_mv) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t ret = get_point(version, adc_unit, atten, ADC_EFUSE_CALI_POINT_HIGH, out_digi);
    if (ret != ESP_OK) {
        return ret;
    }
    static const uint32_t high_mv[SOC_ADC_ATTEN_NUM] = {600, 800, 1000, 2000};
    *out_vol_mv = high_mv[atten];
    return ESP_OK;
}

esp_err_t adc_hal_efuse_get_cal_voltage_low(int version, uint32_t adc_unit, int atten,
                                            uint32_t *out_digi, uint32_t *out_vol_mv)
{
    if (!out_digi || !out_vol_mv) {
        return ESP_ERR_INVALID_ARG;
    }
    if (version != ADC_HAL_EFUSE_CALIB_VER1) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    esp_err_t ret = get_point(version, adc_unit, atten, ADC_EFUSE_CALI_POINT_LOW, out_digi);
    if (ret != ESP_OK) {
        return ret;
    }
    *out_vol_mv = 250;
    return ESP_OK;
}
