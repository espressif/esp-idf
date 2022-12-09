// Copyright 2019-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include "esp_types.h"
#include "driver/adc.h"
#include "soc/efuse_periph.h"
#include "esp_err.h"
#include "assert.h"
#include "esp_adc_cal.h"
#include "esp_efuse.h"

#define ADC_CAL_CHECK(cond, ret) ({                                         \
            if(!(cond)){                                                    \
                return ret;                                                 \
            }                                                               \
})
/* ------------------------ Characterization Constants ---------------------- */
#define ADC_CHAR_VERSION1_EFUSEVAL 1

static const uint32_t adc1_D_mean_low[] = {2231, 1643, 1290, 701};
static const uint32_t adc2_D_mean_low[] = {2305, 1693, 1343, 723};
static const uint32_t adc1_D_mean_high[] = {5775, 5692, 5725, 6209};
static const uint32_t adc2_D_mean_high[] = {5817, 5703, 5731, 6157};

static const int Dlow_data_length = 6;
static const int Dhigh_data_length = 8;

static const int adc_efuse_block = 2;
static const int adc_calib_ver_block = 2;
static const int adc_calib_ver_word_loc = 4;
static const int adc_calib_ver_offset = 4;
static const int adc_calib_ver_len = 3;

static const int adc1_atten0_Dlow_word_loc = 6;
static const int adc2_atten0_Dlow_word_loc = 7;
static const int adc1_atten0_Dhigh_word_loc = 4;
static const int adc2_atten0_Dhigh_word_loc = 5;

static const int adc1_atten0_Dlow_offset = 16;
static const int adc2_atten0_Dlow_offset = 8;
static const int adc1_atten0_Dhigh_offset = 16;
static const int adc2_atten0_Dhigh_offset = 16;
/* ----------------------- EFuse Access Functions --------------------------- */
/**
 *  Convenience function that reads a few bits from efuse and assembles them.
 *  For example, if the contents of the EFuse are:
 *   Word2: 0x1234 Word3:0x5678
 *   Then, setting base=2, offset=24, len=24 will yield 0x456.
 *  @note does not check for boundaries, make sure parameters are correct
 *  @param blk EFuse Block
 *  @param base the starting word
 *  @param offset the bit offset in the starting word
 *  @param bit how many consecutive bits to fetch
 *  @return the assembled number
 */
static uint32_t get_consecutive_bits_from_blk(int blk, uint32_t base, int offset, int len)
{

    base += offset / 32;
    offset %= 32;
    if (offset + len <= 32 || base == 7) {
        uint32_t result = esp_efuse_read_reg(blk, base);
        result <<= (32 - offset - len);
        result >>= (32 - len);
        return result;
    } else {
        // need to fetch both bytes.
        uint64_t result = ((uint64_t)esp_efuse_read_reg(blk, base + 1) << 32) + esp_efuse_read_reg(blk, base);
        result &= ((uint64_t)1 << (offset + len)) - 1;
        result >>= offset;
        return result;
    }
}

/**
 *  To save space in EFuse, the calibration values for adc are compressed.
 *  The compression scheme is: for X bits of ADC Efuse data,
 *  The actual ADC reading is: BASE_VALUE + 4*ADC_OFFSET
 *  where ADC_OFFSET = bits X-1:0 in Efuse, the highest bit is the sign bit (0:+, 1:-).
 *
 *  The following functions do this conversion.
 *  @param efuse_val raw values read from efuse.
 *  @param adc_num Specifies the channel number. The 2 adc channels each have different calibration values.
 *  @param attem Specifies the attenuation. Different attenuation level have different calibration values.
 */
static uint32_t efuse_low_val_to_d(uint16_t efuse_val, adc_unit_t adc_num, adc_atten_t atten)
{
    // efuse_val is 5 bits + 6th sign bit.
    int32_t rawoffsetval = efuse_val & ((1 << (Dlow_data_length - 1)) - 1);
    // if the sign bit is 1, it means it is a negative sign.
    int32_t offset = (efuse_val & (1 << (Dlow_data_length - 1))) ? (-rawoffsetval * 4) : (rawoffsetval * 4);
    if (adc_num == ADC_UNIT_1) {
        return offset + adc1_D_mean_low[atten - ADC_ATTEN_DB_0];
    } else {
        return offset + adc2_D_mean_low[atten - ADC_ATTEN_DB_0];
    }
}

static uint32_t efuse_high_val_to_d (uint16_t efuse_val, adc_unit_t adc_num, adc_atten_t atten)
{
    // efuse_val is 7 bits + 8th sign bit.
    int32_t rawoffsetval = efuse_val & ((1 << (Dhigh_data_length - 1)) - 1);
    int32_t offset = (efuse_val & (1 << (Dhigh_data_length - 1))) ? (-rawoffsetval * 4) : (rawoffsetval * 4);
    if (adc_num == ADC_UNIT_1) {
        return offset + adc1_D_mean_high[atten - ADC_ATTEN_DB_0];
    } else {
        return offset + adc2_D_mean_high[atten - ADC_ATTEN_DB_0];
    }

}

/**
 *  To save space in EFuse, the calibration values for adc are compressed.
 *  The compression scheme is: for X bits of ADC Efuse data,
 *  The actual ADC reading is: BASE_VALUE + 4*ADC_OFFSET
 *  where ADC_OFFSET = bits X-1:0 in Efuse, the highest bit is the sign bit (0:+, 1:-).
 *
 *  The following functions do the reading.
 *  @param efuse_val raw values read from efuse.
 *  @param adc_num Specifies the channel number. The 2 adc channels each have different calibration values.
 *  @param attem Specifies the attenuation. Different attenuation level have different calibration values.
 */
static uint32_t read_efuse_tp_low(adc_unit_t adc_num, adc_atten_t atten)
{
    // this fcn retrieves and decodes the calibration value stored in efuse.
    uint32_t base;
    int offset;
    // may need to move magic numbers out
    if (adc_num == ADC_UNIT_1) {
        // the first value is at the 16th bit of the 6th word of the efuse block 2, each value is 6 bits long.
        base = adc1_atten0_Dlow_word_loc;
        offset = adc1_atten0_Dlow_offset + Dlow_data_length * (atten - ADC_ATTEN_DB_0);

    } else {
        // the first value is at the 8th bit of the 7th word of the efuse block 2, each value is 6 bits long.
        base = adc2_atten0_Dlow_word_loc;
        offset = adc2_atten0_Dlow_offset + Dlow_data_length * (atten - ADC_ATTEN_DB_0);
    }
    uint32_t read_result = get_consecutive_bits_from_blk(adc_efuse_block, base, offset, Dlow_data_length);
    return read_result;
}

static uint32_t read_efuse_tp_high(adc_unit_t adc_num, adc_atten_t atten)
{
    // this fcn retrieves and decodes the calibration value stored in efuse.
    uint32_t base;
    int offset;

    if (adc_num == ADC_UNIT_1) {
        // the first value is at the 16th bit of the 4th word of the efuse block 2, each value is 8 bits long.
        base = adc1_atten0_Dhigh_word_loc;
        offset = adc1_atten0_Dhigh_offset + Dhigh_data_length * (atten - ADC_ATTEN_DB_0);

    } else {
        // the first value is at the 16th bit of the 5th word of the efuse block 2, each value is 8 bits long.
        base = adc2_atten0_Dhigh_word_loc;
        offset = adc2_atten0_Dhigh_offset + Dhigh_data_length * (atten - ADC_ATTEN_DB_0);
    }
    uint32_t read_result = get_consecutive_bits_from_blk(adc_efuse_block, base, offset, Dhigh_data_length);
    return read_result;
}

/* ----------------------- Characterization Functions ----------------------- */

// coeff_a and coeff_b are actually floats
// they are scaled to put them into uint32_t so that the headers do not have to be changed
static const int coeff_a_scaling = 65536;
static const int coeff_b_scaling = 1024;

/**
 *  The Two Point calibration measures the reading at two specific input voltages, and calculates the (assumed linear) relation
 *  between input voltage and ADC response. (Response = A * Vinput + B)
 *  A and B are scaled ints.
 *  @param high The ADC response at the higher voltage of the corresponding attenuation (600mV, 800mV, 1000mV, 2000mV).
 *  @param low The ADC response at the lower voltage of the corresponding attenuation (all 250mV).
 *
 */
static void characterize_using_two_point(adc_unit_t adc_num,
        adc_atten_t atten,
        uint32_t high,
        uint32_t low,
        uint32_t *coeff_a,
        uint32_t *coeff_b)
{
    // once we have recovered the reference high(Dhigh) and low(Dlow) readings, we can calculate a and b from
    // the measured high and low readings
    static const uint32_t v_high[] = {600, 800, 1000, 2000};
    static const uint32_t v_low = 250;
    *coeff_a = coeff_a_scaling * (v_high[atten] - v_low) / (high - low);
    *coeff_b = coeff_b_scaling * (v_low * high - v_high[atten] * low) / (high - low);
}


/* ------------------------- Public API ------------------------------------- */
esp_err_t esp_adc_cal_check_efuse(esp_adc_cal_value_t source)
{
    if (source != ESP_ADC_CAL_VAL_EFUSE_TP) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    uint8_t adc1_atten0_dh = get_consecutive_bits_from_blk(adc_efuse_block, adc1_atten0_Dhigh_word_loc, adc1_atten0_Dhigh_offset, Dhigh_data_length);
    uint8_t adc2_atten0_dh = get_consecutive_bits_from_blk(adc_efuse_block, adc2_atten0_Dhigh_word_loc, adc2_atten0_Dhigh_offset, Dhigh_data_length);
    if (!adc1_atten0_dh || !adc2_atten0_dh) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    uint8_t adc_encoding_version = get_consecutive_bits_from_blk(adc_calib_ver_block, adc_calib_ver_word_loc, adc_calib_ver_offset, adc_calib_ver_len);
    if (adc_encoding_version != 1) {
        // current version only accepts encoding ver 1.
        return ESP_ERR_INVALID_VERSION;
    }
    return ESP_OK;
}

esp_adc_cal_value_t esp_adc_cal_characterize(adc_unit_t adc_num,
        adc_atten_t atten,
        adc_bits_width_t bit_width,
        uint32_t default_vref,
        esp_adc_cal_characteristics_t *chars)
{
    // Check parameters
    assert((adc_num == ADC_UNIT_1) || (adc_num == ADC_UNIT_2));
    assert(chars != NULL);
    assert(bit_width == ADC_WIDTH_BIT_13);

    // Characterize based on efuse Two Point values. If these values are not present in efuse,
    // or efuse values are of a version that we do not recognize, automatically assume default values.
    uint32_t adc_calib_high, adc_calib_low;
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        adc_calib_high = read_efuse_tp_high(adc_num, atten);
        adc_calib_low = read_efuse_tp_low(adc_num, atten);
    } else {
        adc_calib_high = 0;
        adc_calib_low = 0;
    }
    uint32_t high = efuse_high_val_to_d(adc_calib_high, adc_num, atten);
    uint32_t low = efuse_low_val_to_d(adc_calib_low, adc_num, atten);
    characterize_using_two_point(adc_num, atten, high, low, &(chars->coeff_a), &(chars->coeff_b));
    // Initialize remaining fields
    chars->adc_num = adc_num;
    chars->atten = atten;
    chars->bit_width = bit_width;

    // these values are not used as the corresponding calibration themes are deprecated.
    chars->vref = 0;
    chars->low_curve = NULL;
    chars->high_curve = NULL;

    // in esp32s2 we only use the two point method to calibrate the adc.
    return ESP_ADC_CAL_VAL_EFUSE_TP;
}

uint32_t esp_adc_cal_raw_to_voltage(uint32_t adc_reading, const esp_adc_cal_characteristics_t *chars)
{
    ADC_CAL_CHECK(chars != NULL, ESP_ERR_INVALID_ARG);

    return adc_reading * chars->coeff_a / coeff_a_scaling + chars->coeff_b / coeff_b_scaling;
}

esp_err_t esp_adc_cal_get_voltage(adc_channel_t channel,
                                  const esp_adc_cal_characteristics_t *chars,
                                  uint32_t *voltage)
{
    // Check parameters
    ADC_CAL_CHECK(chars != NULL, ESP_ERR_INVALID_ARG);
    ADC_CAL_CHECK(voltage != NULL, ESP_ERR_INVALID_ARG);

    int adc_reading;
    if (chars->adc_num == ADC_UNIT_1) {
        //Check if channel is valid on ADC1
        ADC_CAL_CHECK((adc1_channel_t)channel < ADC1_CHANNEL_MAX, ESP_ERR_INVALID_ARG);
        adc_reading = adc1_get_raw(channel);
    } else {
        //Check if channel is valid on ADC2
        ADC_CAL_CHECK((adc2_channel_t)channel < ADC2_CHANNEL_MAX, ESP_ERR_INVALID_ARG);
        if (adc2_get_raw(channel, chars->bit_width, &adc_reading) != ESP_OK) {
            return ESP_ERR_TIMEOUT;     //Timed out waiting for ADC2
        }
    }
    *voltage = esp_adc_cal_raw_to_voltage((uint32_t)adc_reading, chars);
    return ESP_OK;
}

