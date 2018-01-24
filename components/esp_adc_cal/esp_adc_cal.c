// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include "soc/efuse_reg.h"
#include "esp_err.h"
#include "esp_log.h"
#include "assert.h"
#include "esp_adc_cal_constants.h"
#include "esp_adc_cal.h"

#define CONFIG_ADC_CAL_EFUSE_TP_DISABLE
/* ----------------------------- Configuration ------------------------------ */

#ifdef CONFIG_ADC_CAL_EFUSE_TP_DISABLE
#define EFUSE_TP_ENABLED        0
#else
#define EFUSE_TP_ENABLED        1
#endif

#ifdef CONFIG_ADC_CAL_EFUSE_VREF_DISABLE
#define EFUSE_VREF_ENABLED      0
#else
#define EFUSE_VREF_ENABLED      1
#endif

#ifdef CONFIG_ADC_CAL_DEFAULT_VREF_DISABLE
#define DEFAULT_VREF_ENABLED    0
#else
#define DEFAULT_VREF_ENABLED    1
#endif

/* ------------------------------ eFuse Access ----------------------------- */

#define BLK3_RESERVED_REG               EFUSE_BLK0_RDATA4_REG

#define VREF_REG                        EFUSE_BLK0_RDATA4_REG
#define VREF_SIGN_BIT                   0x10
#define VREF_MAG_BITS                   0x0F
#define VREF_STEP_SIZE                  7
#define VREF_OFFSET                     1100

#define TP_REG                          EFUSE_BLK3_RDATA3_REG
#define TP_LOW1_OFFSET                  278
#define TP_LOW2_OFFSET                  421
#define TP_HIGH1_OFFSET                 3265
#define TP_HIGH2_OFFSET                 3406
#define TP_LOW_SIGN_BIT                 0x40
#define TP_LOW_MAG_BITS                 0x3F
#define TP_LOW_VOLTAGE                  150
#define TP_HIGH_SIGN_BIT                0x100
#define TP_HIGH_MAG_BITS                0xFF
#define TP_HIGH_VOLTAGE                 850
#define TP_STEP_SIZE                    4

/* -------------------- Linear and LUT mode constants ----------------------- */

#define LIN_COEFF_A_SCALE               65536
#define LIN_COEFF_A_ROUND               (LIN_COEFF_A_SCALE/2)
#define LUT_VREF_IDEAL                  1100
#define LUT_VREF_LOW                    1000
#define LUT_VREF_HIGH                   1200
#define LUT_ADC_STEP_SIZE               128
#define ADC_12_BIT_MAX                  4095

#define ADC_CAL_ASSERT(cond, ret) ({                                        \
            if(!(cond)){                                                    \
                return ret;                                                 \
            }                                                               \
})

#define ESP_ADC_CAL_ERR_STR     "No characterization possible"
static const char* ESP_ADC_CAL_TAG = "esp_adc_cal_log";

extern const uint32_t adc1_lin_tp_atten_scale[4];
extern const uint32_t adc2_lin_tp_atten_scale[4];
extern const uint32_t adc1_lin_tp_atten_offset[4];
extern const uint32_t adc2_lin_tp_atten_offset[4];
extern const uint32_t adc1_lin_vref_atten_scale[4];
extern const uint32_t adc2_lin_vref_atten_scale[4];
extern const uint32_t adc1_lin_vref_atten_offset[4];
extern const uint32_t adc2_lin_vref_atten_offset[4];

extern const esp_adc_cal_lookup_table_t lut_atten0_adc1;
extern const esp_adc_cal_lookup_table_t lut_atten0_adc2;
extern const esp_adc_cal_lookup_table_t lut_atten1_adc1;
extern const esp_adc_cal_lookup_table_t lut_atten1_adc2;
extern const esp_adc_cal_lookup_table_t lut_atten2_adc1;
extern const esp_adc_cal_lookup_table_t lut_atten2_adc2;
extern const esp_adc_cal_lookup_table_t lut_atten3_adc1;
extern const esp_adc_cal_lookup_table_t lut_atten3_adc2;

/* ----------------------- EFuse Access Functions --------------------------- */

//Check if Vref is burned in eFuse
static bool check_efuse_vref()
{
    //Check eFuse for vref
    return (REG_GET_FIELD(VREF_REG, EFUSE_RD_ADC_VREF) != 0) ? true : false;
}

//Check if Two Point values are burned in eFuse
static bool check_efuse_tp()
{
#ifndef CONFIG_ADC_CAL_NO_BLK3_RESERVE_FLAG
    //BLK3_PART_RESERVE flag must be set
    if(REG_GET_FIELD(BLK3_RESERVED_REG, EFUSE_RD_BLK3_PART_RESERVE) == 0){
        return false;
    }
#endif
    //All TP cal values must be non zero
    if((REG_GET_FIELD(TP_REG, EFUSE_RD_ADC1_TP_LOW) != 0) &&
       (REG_GET_FIELD(TP_REG, EFUSE_RD_ADC2_TP_LOW) != 0) &&
       (REG_GET_FIELD(TP_REG, EFUSE_RD_ADC1_TP_HIGH) != 0) &&
       (REG_GET_FIELD(TP_REG, EFUSE_RD_ADC2_TP_HIGH) != 0)){
        return true;
    } else {
        return false;
    }
}

//Read Vref from eFuse
static uint32_t read_efuse_vref()
{
    //eFuse stores deviation from ideal reference voltage
    uint32_t ret = VREF_OFFSET;       //Ideal vref
    uint32_t bits = REG_GET_FIELD(VREF_REG, EFUSE_ADC_VREF);

    if(bits & VREF_SIGN_BIT){       //Negative deviation from ideal Vref
#ifndef CONFIG_ADC_CAL_NO_BLK3_RESERVE_FLAG
        //Deviation stored in sign-magnitude format
        ret -= (bits & VREF_MAG_BITS) * VREF_STEP_SIZE;
#else
        //Deviation stored in two's complement
        ret -= (((~bits)+1) & VREF_MAG_BITS) * VREF_STEP_SIZE;
#endif
    } else {                        //Positive deviation from ideal Vref
        ret += (bits & VREF_MAG_BITS) * VREF_STEP_SIZE;
    }
    return ret;     //ADC Vref in mV
}

//Read Two Point low reading from eFuse
static uint32_t read_efuse_tp_low(adc_unit_t adc_num)
{
    //ADC reading at 150mV stored in two's complement format
    uint32_t ret;
    uint32_t bits;

    if(adc_num == ADC_UNIT_1){
        ret = TP_LOW1_OFFSET;
        bits = REG_GET_FIELD(TP_REG, EFUSE_RD_ADC1_TP_LOW);
    } else {
        ret = TP_LOW2_OFFSET;
        bits = REG_GET_FIELD(TP_REG, EFUSE_RD_ADC2_TP_LOW);
    }

    //Represented in two's complement format
    if(bits & TP_LOW_SIGN_BIT){     //Negative
        ret -= (((~bits) + 1) & TP_LOW_MAG_BITS) * TP_STEP_SIZE;
    } else {    //Positive
        ret += (bits & TP_LOW_MAG_BITS) * TP_STEP_SIZE;
    }
    return ret;     //Reading of ADC at 150mV
}

//Read Two Point high reading from eFuse
static uint32_t read_efuse_tp_high(adc_unit_t adc_num)
{
    //ADC reading at 850mV stored in two's complement format
    uint32_t ret;
    uint32_t bits;
    if(adc_num == ADC_UNIT_1){
        ret = TP_HIGH1_OFFSET;
        bits = REG_GET_FIELD(TP_REG, EFUSE_RD_ADC1_TP_HIGH);
    } else {
        ret = TP_HIGH2_OFFSET;
        bits = REG_GET_FIELD(TP_REG, EFUSE_RD_ADC2_TP_HIGH);
    }

    //Represented in two's complement format
    if(bits & TP_HIGH_SIGN_BIT){    //Negative
        ret -= (((~bits) + 1) & TP_HIGH_MAG_BITS) * TP_STEP_SIZE;
    } else {    //Positive
        ret += (bits & TP_HIGH_MAG_BITS) * TP_STEP_SIZE;
    }
    return ret;     //Reading of ADC at 850mV
}

/* ----------------------- Characterization Functions ----------------------- */

//Linear characterization using Two Point values
static void characterize_lin_tp(adc_unit_t adc_num,
                                adc_atten_t atten,
                                uint32_t high,
                                uint32_t low,
                                uint32_t *coeff_a,
                                uint32_t *coeff_b)
{
    const uint32_t *atten_scales;
    const uint32_t *atten_offsets;

    if(adc_num == ADC_UNIT_1){   //Using ADC 1
        atten_scales = adc1_lin_tp_atten_scale;
        atten_offsets = adc1_lin_tp_atten_offset;
    } else {    //Using ADC 2
        atten_scales = adc2_lin_tp_atten_scale;
        atten_offsets = adc2_lin_tp_atten_offset;
    }
    //Characterize ADC-Voltage curve as y = (coeff_a * x) + coeff_b
    uint32_t delta_x = high - low;
    uint32_t delta_v = TP_HIGH_VOLTAGE - TP_LOW_VOLTAGE;
    //coeff_a = (delta_v/delta_x) * atten_scale
    *coeff_a = (delta_v * atten_scales[atten] + (delta_x/2)) / delta_x;     //+(delta_x/2) for rounding
    //coeff_b = high_v - ((delta_v/delta_x) * high_x) + atten_offset
    *coeff_b = TP_HIGH_VOLTAGE - ((delta_v * high + (delta_x/2)) / delta_x) + atten_offsets[atten];
}

//Linear characterization using Vref
static void characterize_lin_vref(adc_unit_t adc_num,
                                  adc_atten_t atten,
                                  uint32_t vref,
                                  uint32_t *coeff_a,
                                  uint32_t *coeff_b)
{
    const uint32_t *atten_scales;;
    const uint32_t *atten_offsets;

    if(adc_num == ADC_UNIT_1){   //Using ADC 1
        atten_scales = adc1_lin_vref_atten_scale;
        atten_offsets = adc1_lin_vref_atten_offset;
    } else {    //Using ADC 2
        atten_scales = adc2_lin_vref_atten_scale;
        atten_offsets = adc2_lin_vref_atten_offset;
    }
    //Characterize ADC-Voltage curve as y = (coeff_a * x) + coeff_b
    //coeff_a = (vref/4096) * atten_scale
    *coeff_a = (vref * atten_scales[atten]) / (ADC_12_BIT_MAX + 1);
    *coeff_b = atten_offsets[atten];
}

//LUT characterization
static void characterize_lut(adc_unit_t adc_num,
                             adc_atten_t atten,
                             uint32_t vref,
                             const esp_adc_cal_lookup_table_t **table_ptr)
{
    //Get pointer to the correct lookup table
    if(atten ==  ADC_ATTEN_DB_0){
        *table_ptr = (adc_num == ADC_UNIT_1) ? &lut_atten0_adc1 : &lut_atten0_adc2;
    } else if (atten == ADC_ATTEN_DB_2_5){
        *table_ptr = (adc_num == ADC_UNIT_1) ? &lut_atten1_adc1 : &lut_atten1_adc2;
    } else if (atten == ADC_ATTEN_DB_6){
        *table_ptr = (adc_num == ADC_UNIT_1) ? &lut_atten2_adc1 : &lut_atten2_adc2;
    } else {
        *table_ptr = (adc_num == ADC_UNIT_1) ? &lut_atten3_adc1 : &lut_atten3_adc2;
    }
}

/* ------------------------ Conversion Functions --------------------------- */

//Calculate voltage using linear characterization of the ADC curve
static uint32_t linear_raw_to_voltage(uint32_t adc,
                                      uint32_t gain,
                                      uint32_t offset)
{
    //ADC Curve is characterized as y = coeff_a * adc + coeff_b
    //All gains scaled by 65536
    return (((gain * adc) + LIN_COEFF_A_ROUND) / LIN_COEFF_A_SCALE) + offset;
}

//Calculate voltage using a lookup table
static uint32_t lut_raw_to_voltage(uint32_t adc, uint32_t vref, const esp_adc_cal_lookup_table_t *table)
{
    //Get index of lower bound points of LUT
    uint32_t i = (adc / LUT_ADC_STEP_SIZE);

    //Let the X Axis be Vref, Y axis be ADC reading, and Z be voltage
    int x2dist = LUT_VREF_HIGH - vref;                 //(x2 - x)
    int x1dist = vref - LUT_VREF_LOW;                  //(x - x1)
    int y2dist = ((i+1) * LUT_ADC_STEP_SIZE) - adc;    //(y2 - y)
    int y1dist = adc - (i * LUT_ADC_STEP_SIZE);        //(y - y1)

    //For points for bilinear interpolation
    int q11 = (int)table->low_vref_curve[i];                    //Lower bound point of low_vref_curve
    int q12 = (int)table->low_vref_curve[i+1];                  //Upper bound point of low_vref_curve
    int q21 = (int)table->high_vref_curve[i];                   //Lower bound point of high_vref_curve
    int q22 = (int)table->high_vref_curve[i+1];                 //Upper bound point of high_vref_curve

    //Bilinear interpolation
    //z = 1/((x2-x1)*(y2-y1)) * ( (q11*x2dist*y2dist) + (q21*x1dist*y2dist) + (q12*x2dist*y1dist) + (q22*x1dist*y1dist) )
    int voltage = (q11*x2dist*y2dist) + (q21*x1dist*y2dist) + (q12*x2dist*y1dist) + (q22*x1dist*y1dist);
    voltage += ((LUT_VREF_HIGH - LUT_VREF_LOW) * LUT_ADC_STEP_SIZE)/2;  //Integer division rounding
    voltage /= ((LUT_VREF_HIGH - LUT_VREF_LOW) * LUT_ADC_STEP_SIZE);    //Divide by ((x2-x1)*(y2-y1))
    return (uint32_t)voltage;
}

/* ------------------------- Public API ------------------------------------- */

esp_err_t esp_adc_cal_check_efuse(esp_adc_cal_value_t source)
{
    if(source == ESP_ADC_CAL_VAL_EFUSE_TP){
        return (check_efuse_tp()) ? ESP_OK : ESP_ERR_NOT_SUPPORTED;
    } else if (source == ESP_ADC_CAL_VAL_EFUSE_VREF){
        return (check_efuse_vref()) ? ESP_OK : ESP_ERR_NOT_SUPPORTED;
    } else {
        return ESP_ERR_INVALID_ARG;
    }
}

esp_adc_cal_value_t esp_adc_cal_characterize(adc_unit_t adc_num,
                                             adc_atten_t atten,
                                             esp_adc_cal_mode_t mode,
                                             uint32_t vref_default,
                                             esp_adc_cal_characteristics_t *chars)
{
    assert((adc_num == ADC_UNIT_1) || (adc_num == ADC_UNIT_2));
    assert(chars != NULL);

    //Check eFuse if enabled to do so
    bool efuse_tp_present = check_efuse_tp();
    bool efuse_vref_present = check_efuse_vref();
    esp_adc_cal_value_t ret;

    if(mode == ESP_ADC_CAL_MODE_LIN){
        if(efuse_tp_present && EFUSE_TP_ENABLED){
            uint32_t high = read_efuse_tp_high(adc_num);
            uint32_t low = read_efuse_tp_low(adc_num);
            characterize_lin_tp(adc_num, atten, high, low, &chars->linear_chars.coeff_a, &chars->linear_chars.coeff_b);
            ret = ESP_ADC_CAL_VAL_EFUSE_TP;
        } else if(efuse_vref_present && EFUSE_VREF_ENABLED){
            uint32_t vref = read_efuse_vref();
            characterize_lin_vref(adc_num, atten, vref, &chars->linear_chars.coeff_a, &chars->linear_chars.coeff_b);
            ret = ESP_ADC_CAL_VAL_EFUSE_VREF;
        } else if(DEFAULT_VREF_ENABLED){
            characterize_lin_vref(adc_num, atten, vref_default, &chars->linear_chars.coeff_a, &chars->linear_chars.coeff_b);
            ret = ESP_ADC_CAL_VAL_DEFAULT_VREF;
        } else {
            goto err;
        }
    } else if (mode == ESP_ADC_CAL_MODE_LUT){
        if(efuse_vref_present && EFUSE_VREF_ENABLED){
            uint32_t vref = read_efuse_vref();
            chars->lut_chars.vref = vref;
            characterize_lut(adc_num, atten, vref, &chars->lut_chars.table);
            ret = ESP_ADC_CAL_VAL_EFUSE_VREF;
        } else if(DEFAULT_VREF_ENABLED){
            chars->lut_chars.vref = vref_default;
            characterize_lut(adc_num, atten, vref_default, &chars->lut_chars.table);
            ret = ESP_ADC_CAL_VAL_DEFAULT_VREF;
        } else{
            goto err;
        }
    } else {
        goto err;
    }
    chars->mode = mode;
    chars->adc_num = adc_num;
    return ret;

    err:        //No possible characterization
    // usually only occurs if users manually disable calibration values and modes in menuconfig
    ESP_LOGE(ESP_ADC_CAL_TAG, ESP_ADC_CAL_ERR_STR);
    abort();
    return ESP_ADC_CAL_VAL_DEFAULT_VREF; //Should not reach this point, added to suppress Eclipse warnings
}

uint32_t esp_adc_cal_raw_to_voltage(uint32_t adc_reading,
                                    adc_bits_width_t bit_width,
                                    const esp_adc_cal_characteristics_t *chars)
{
    assert(chars != NULL);
    //Scale adc_rading if not 12 bits wide
    if(bit_width != ADC_WIDTH_BIT_12){
        adc_reading = (adc_reading << (ADC_WIDTH_BIT_12 - bit_width));
        //If adc_reading is out of 12bit range
        if(adc_reading > ADC_12_BIT_MAX){
            adc_reading = ADC_12_BIT_MAX;    //Set to 12bit max
        }
    }

    //Convert ADC reading to voltage in mV
    if(chars->mode == ESP_ADC_CAL_MODE_LUT){    //Lookup Table
        assert(chars->lut_chars.table != NULL);
        return lut_raw_to_voltage(adc_reading, chars->lut_chars.vref, chars->lut_chars.table);
    } else {
        return linear_raw_to_voltage(adc_reading, chars->linear_chars.coeff_a, chars->linear_chars.coeff_b);
    }
}

esp_err_t adc_to_voltage(adc_channel_t channel,
                         adc_bits_width_t bit_width,
                         const esp_adc_cal_characteristics_t *chars,
                         uint32_t *voltage)
{
    //Check parameters
    ADC_CAL_ASSERT(chars != NULL, ESP_ERR_INVALID_ARG);
    ADC_CAL_ASSERT(voltage != NULL, ESP_ERR_INVALID_ARG);

    if(chars->adc_num == ADC_UNIT_1){
        //Check channel is valid on ADC1
        ADC_CAL_ASSERT((adc1_channel_t)channel < ADC1_CHANNEL_MAX, ESP_ERR_INVALID_ARG);
        uint32_t adc_reading = (uint32_t)adc1_get_raw(channel);     //Todo: get_raw function to change
        *voltage =  esp_adc_cal_raw_to_voltage(adc_reading, bit_width, chars);
    } else {
        //Check channel is valid on ADC2
        ADC_CAL_ASSERT((adc2_channel_t)channel < ADC2_CHANNEL_MAX, ESP_ERR_INVALID_ARG);
        int adc_reading;
        if(adc2_get_raw(channel, bit_width, &adc_reading) != ESP_OK){
            //Timed out waiting for ADC2
            return ESP_ERR_TIMEOUT;
        }
        *voltage = esp_adc_cal_raw_to_voltage((uint32_t)adc_reading, bit_width, chars);
    }
    return ESP_OK;
}

/* ------------------------ Deprecated API --------------------------------- */

void esp_adc_cal_get_characteristics(uint32_t vref,
                                     adc_atten_t atten,
                                     adc_bits_width_t bit_width,
                                     esp_adc_cal_characteristics_t *chars)
{
    //Default to ADC1 and LUT mode
    assert(chars != NULL);

    //bit_width parameter unused, kept due to legacy API
    chars->mode = ESP_ADC_CAL_MODE_LUT;
    chars->lut_chars.vref = vref;
    characterize_lut(ADC_UNIT_1, atten, vref, &chars->lut_chars.table);
    chars->adc_num = ADC_UNIT_1;
}

uint32_t adc1_to_voltage(adc1_channel_t channel, const esp_adc_cal_characteristics_t *chars)
{
    assert(chars != NULL);
    uint32_t voltage = 0;
    adc_to_voltage((adc_channel_t) channel, ADC_WIDTH_BIT_12, chars, &voltage);
    return voltage;
}


