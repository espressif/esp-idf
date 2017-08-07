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
#include "driver/adc.h"

#include "esp_adc_cal.h"

static const esp_adc_cal_lookup_table_t *table_ptrs[4] = {&esp_adc_cal_table_atten_0,
                                                          &esp_adc_cal_table_atten_1,
                                                          &esp_adc_cal_table_atten_2,
                                                          &esp_adc_cal_table_atten_3};

uint32_t get_adc_vref_from_efuse()
{
    //TODO: Replaced with read to eFuse once ATE confirms location of 5 bits
    return 0;
}

void esp_adc_cal_get_characteristics(uint32_t v_ref,
                                     adc_atten_t atten,
                                     adc_bits_width_t bit_width,
                                     esp_adc_cal_characteristics_t *chars)
{
    chars->v_ref = v_ref;
    chars->table = table_ptrs[atten];
    chars->bit_width = bit_width;
    if (v_ref >= ADC_CAL_LOW_V_REF) {
        chars->gain = ((chars->v_ref - ADC_CAL_LOW_V_REF)
                      * chars->table->gain_m)
                      + chars->table->gain_c;
        chars->offset = (((chars->v_ref - ADC_CAL_LOW_V_REF)
                        * chars->table->offset_m)
                        + chars->table->offset_c
                        + ((1 << ADC_CAL_OFFSET_SCALE) / 2))
                        >> ADC_CAL_OFFSET_SCALE; //Bit shift to cancel 2^10 multiplier
        chars->ideal_offset = (((ADC_CAL_IDEAL_V_REF - ADC_CAL_LOW_V_REF)
                              * chars->table->offset_m)
                              + chars->table->offset_c
                              + ((1 << ADC_CAL_OFFSET_SCALE) / 2)) //Rounding
                              >> ADC_CAL_OFFSET_SCALE;
    } else { //For case where v_ref is smaller than low bound resulting in negative
        chars->gain = chars->table->gain_c
                      - ((ADC_CAL_LOW_V_REF - chars->v_ref)
                      * chars->table->gain_m);
        chars->offset = (chars->table->offset_c
                        - ((chars->v_ref - ADC_CAL_LOW_V_REF)
                        * chars->table->offset_m)
                        + ((1 << ADC_CAL_OFFSET_SCALE) / 2))  //Rounding
                        >> ADC_CAL_OFFSET_SCALE; //Bit shift to cancel 2^10 multiplier
        chars->ideal_offset = (chars->table->offset_c
                              - ((ADC_CAL_IDEAL_V_REF - ADC_CAL_LOW_V_REF)
                              * chars->table->offset_m)
                              + ((1 << ADC_CAL_OFFSET_SCALE) / 2)) //Rounding
                              >> ADC_CAL_OFFSET_SCALE;
    }
}

static uint32_t esp_adc_cal_interpolate_round(uint32_t lower, uint32_t upper,
                                              uint32_t step, uint32_t point)
{
    //Interpolate 'point' between 'lower' and 'upper' seperated by 'step'
    return ((lower * step) - (lower * point) + (upper * point) + (step / 2)) / step;
}

uint32_t esp_adc_cal_raw_to_voltage(uint32_t adc,
                                    const esp_adc_cal_characteristics_t *chars)
{
    //Scale ADC to 12 bit width (0 to 4095)
    adc <<= (ADC_WIDTH_12Bit - chars->bit_width);
    uint32_t i = (adc >> chars->table->bit_shift);  //find index for lut voltages
    //Refernce LUT to obtain voltage using index
    uint32_t voltage = esp_adc_cal_interpolate_round(chars->table->voltage[i],
                       chars->table->voltage[i + 1],
                       (1 << chars->table->bit_shift),
                       adc - (i << chars->table->bit_shift));
    /*
     * Apply Gain, scaling(bit shift) and offset to interpolated voltage
     * v_true = (((v_id - off_id)*gain)*scaling) + off_true
     */
    if (voltage > chars->ideal_offset) {
        voltage = (voltage - chars->ideal_offset) * chars->gain;
        voltage += (1 << ADC_CAL_GAIN_SCALE) / 2;  //For rounding when scaled
        voltage >>= ADC_CAL_GAIN_SCALE;
        voltage += chars->offset;
    } else { //For case where voltage is less than ideal offset leading to negative value
        voltage = ((chars->ideal_offset - voltage) * chars->gain);
        voltage += (1 << ADC_CAL_GAIN_SCALE) / 2;   //For rounding when scaled
        voltage >>= ADC_CAL_GAIN_SCALE;
        voltage = chars->offset - voltage;
    }

    return voltage;
}

uint32_t adc1_to_voltage(adc1_channel_t channel, const esp_adc_cal_characteristics_t *chars)
{
    return esp_adc_cal_raw_to_voltage((uint32_t)adc1_get_raw(channel), chars);
}

