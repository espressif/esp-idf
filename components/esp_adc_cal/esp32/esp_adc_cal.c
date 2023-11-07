/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_types.h"
#include "driver/adc.h"
#include "soc/efuse_periph.h"
#include "esp_err.h"
#include "assert.h"
#include "esp_adc_cal.h"

/* ----------------------------- Configuration ------------------------------ */
#ifdef CONFIG_ADC_CAL_EFUSE_TP_ENABLE
#define EFUSE_TP_ENABLED        1
#else
#define EFUSE_TP_ENABLED        0
#endif

#ifdef CONFIG_ADC_CAL_EFUSE_VREF_ENABLE
#define EFUSE_VREF_ENABLED      1
#else
#define EFUSE_VREF_ENABLED      0
#endif

#ifdef CONFIG_ADC_CAL_LUT_ENABLE
#define LUT_ENABLED             1
#else
#define LUT_ENABLED             0
#endif

/* ESP32s with both Two Point Values and Vref burned into eFuse are required to
 * also also burn the EFUSE_BLK3_PART_RESERVE flag. A limited set of ESP32s
 * (not available through regular sales channel) DO NOT have the
 * EFUSE_BLK3_PART_RESERVE burned. Moreover, this set of ESP32s represents Vref
 * in Two's Complement format. If this is the case, modify the preprocessor
 * definitions below as follows...
 * #define CHECK_BLK3_FLAG         0        //Do not check BLK3 flag as it is not burned
 * #define VREF_FORMAT             1        //eFuse Vref is in Two's Complement format
 */
#define CHECK_BLK3_FLAG         1
#define VREF_FORMAT             0

/* ------------------------------ eFuse Access ----------------------------- */
#define BLK3_RESERVED_REG               EFUSE_BLK0_RDATA3_REG

#define VREF_REG                        EFUSE_BLK0_RDATA4_REG
#define VREF_MASK                       0x1F
#define VREF_STEP_SIZE                  7
#define VREF_OFFSET                     1100

#define TP_REG                          EFUSE_BLK3_RDATA3_REG
#define TP_LOW1_OFFSET                  278
#define TP_LOW2_OFFSET                  421
#define TP_LOW_MASK                     0x7F
#define TP_LOW_VOLTAGE                  150
#define TP_HIGH1_OFFSET                 3265
#define TP_HIGH2_OFFSET                 3406
#define TP_HIGH_MASK                    0x1FF
#define TP_HIGH_VOLTAGE                 850
#define TP_STEP_SIZE                    4

/* ----------------------- Raw to Voltage Constants ------------------------- */
#define LIN_COEFF_A_SCALE               65536
#define LIN_COEFF_A_ROUND               (LIN_COEFF_A_SCALE/2)

#define LUT_VREF_LOW                    1000
#define LUT_VREF_HIGH                   1200
#define LUT_ADC_STEP_SIZE               64
#define LUT_POINTS                      20
#define LUT_LOW_THRESH                  2880
#define LUT_HIGH_THRESH                 (LUT_LOW_THRESH + LUT_ADC_STEP_SIZE)
#define ADC_12_BIT_RES                  4096

/* ------------------------ Characterization Constants ---------------------- */
static const uint32_t adc1_tp_atten_scale[4] = {65504, 86975, 120389, 224310};
static const uint32_t adc2_tp_atten_scale[4] = {65467, 86861, 120416, 224708};
static const uint32_t adc1_tp_atten_offset[4] = {0, 1, 27, 54};
static const uint32_t adc2_tp_atten_offset[4] = {0, 9, 26, 66};

static const uint32_t adc1_vref_atten_scale[4] = {57431, 76236, 105481, 196602};
static const uint32_t adc2_vref_atten_scale[4] = {57236, 76175, 105678, 197170};
static const uint32_t adc1_vref_atten_offset[4] = {75, 78, 107, 142};
static const uint32_t adc2_vref_atten_offset[4] = {63, 66, 89, 128};

//20 Point lookup tables, covering ADC readings from 2880 to 4096, step size of 64
static const uint32_t lut_adc1_low[LUT_POINTS] = {2240, 2297, 2352, 2405, 2457, 2512, 2564, 2616, 2664, 2709,
                                                  2754, 2795, 2832, 2868, 2903, 2937, 2969, 3000, 3030, 3060};
static const uint32_t lut_adc1_high[LUT_POINTS] = {2667, 2706, 2745, 2780, 2813, 2844, 2873, 2901, 2928, 2956,
                                                   2982, 3006, 3032, 3059, 3084, 3110, 3135, 3160, 3184, 3209};
static const uint32_t lut_adc2_low[LUT_POINTS] = {2238, 2293, 2347, 2399, 2451, 2507, 2561, 2613, 2662, 2710,
                                                  2754, 2792, 2831, 2869, 2904, 2937, 2968, 2999, 3029, 3059};
static const uint32_t lut_adc2_high[LUT_POINTS] = {2657, 2698, 2738, 2774, 2807, 2838, 2867, 2894, 2921, 2946,
                                                   2971, 2996, 3020, 3043, 3067, 3092, 3116, 3139, 3162, 3185};

/* ----------------------- EFuse Access Functions --------------------------- */
static bool check_efuse_vref(void)
{
    //Check if Vref is burned in eFuse
    return (REG_GET_FIELD(VREF_REG, EFUSE_RD_ADC_VREF) != 0) ? true : false;
}

static bool check_efuse_tp(void)
{
    //Check if Two Point values are burned in eFuse
    if (CHECK_BLK3_FLAG && (REG_GET_FIELD(BLK3_RESERVED_REG, EFUSE_RD_BLK3_PART_RESERVE) == 0)) {
        return false;
    }
    //All TP cal values must be non zero
    if ((REG_GET_FIELD(TP_REG, EFUSE_RD_ADC1_TP_LOW) != 0) &&
        (REG_GET_FIELD(TP_REG, EFUSE_RD_ADC2_TP_LOW) != 0) &&
        (REG_GET_FIELD(TP_REG, EFUSE_RD_ADC1_TP_HIGH) != 0) &&
        (REG_GET_FIELD(TP_REG, EFUSE_RD_ADC2_TP_HIGH) != 0)) {
        return true;
    } else {
        return false;
    }
}

static inline int decode_bits(uint32_t bits, uint32_t mask, bool is_twos_compl)
{
    int ret;
    if (bits & (~(mask >> 1) & mask)) {      //Check sign bit (MSB of mask)
        //Negative
        if (is_twos_compl) {
            ret = -(((~bits) + 1) & (mask >> 1));   //2's complement
        } else {
            ret = -(bits & (mask >> 1));    //Sign-magnitude
        }
    } else {
        //Positive
        ret = bits & (mask >> 1);
    }
    return ret;
}

static uint32_t read_efuse_vref(void)
{
    //eFuse stores deviation from ideal reference voltage
    uint32_t ret = VREF_OFFSET;       //Ideal vref
    uint32_t bits = REG_GET_FIELD(VREF_REG, EFUSE_ADC_VREF);
    ret += decode_bits(bits, VREF_MASK, VREF_FORMAT) * VREF_STEP_SIZE;
    return ret;     //ADC Vref in mV
}

static uint32_t read_efuse_tp_low(adc_unit_t adc_num)
{
    //ADC reading at 150mV stored in two's complement format
    uint32_t ret;
    uint32_t bits;

    if (adc_num == ADC_UNIT_1) {
        ret = TP_LOW1_OFFSET;
        bits = REG_GET_FIELD(TP_REG, EFUSE_RD_ADC1_TP_LOW);
    } else {
        ret = TP_LOW2_OFFSET;
        bits = REG_GET_FIELD(TP_REG, EFUSE_RD_ADC2_TP_LOW);
    }
    ret += decode_bits(bits, TP_LOW_MASK, true) * TP_STEP_SIZE;
    return ret;     //Reading of ADC at 150mV
}

static uint32_t read_efuse_tp_high(adc_unit_t adc_num)
{
    //ADC reading at 850mV stored in two's complement format
    uint32_t ret;
    uint32_t bits;

    if (adc_num == ADC_UNIT_1) {
        ret = TP_HIGH1_OFFSET;
        bits = REG_GET_FIELD(TP_REG, EFUSE_RD_ADC1_TP_HIGH);
    } else {
        ret = TP_HIGH2_OFFSET;
        bits = REG_GET_FIELD(TP_REG, EFUSE_RD_ADC2_TP_HIGH);
    }
    ret += decode_bits(bits, TP_HIGH_MASK, true) * TP_STEP_SIZE;
    return ret;     //Reading of ADC at 850mV
}

/* ----------------------- Characterization Functions ----------------------- */
static void characterize_using_two_point(adc_unit_t adc_num,
                                         adc_atten_t atten,
                                         uint32_t high,
                                         uint32_t low,
                                         uint32_t *coeff_a,
                                         uint32_t *coeff_b)
{
    const uint32_t *atten_scales;
    const uint32_t *atten_offsets;

    if (adc_num == ADC_UNIT_1) { //Using ADC 1
        atten_scales = adc1_tp_atten_scale;
        atten_offsets = adc1_tp_atten_offset;
    } else {    //Using ADC 2
        atten_scales = adc2_tp_atten_scale;
        atten_offsets = adc2_tp_atten_offset;
    }
    //Characterize ADC-Voltage curve as y = (coeff_a * x) + coeff_b
    uint32_t delta_x = high - low;
    uint32_t delta_v = TP_HIGH_VOLTAGE - TP_LOW_VOLTAGE;
    //Where coeff_a = (delta_v/delta_x) * atten_scale
    *coeff_a = (delta_v * atten_scales[atten] + (delta_x / 2)) / delta_x;   //+(delta_x/2) for rounding
    //Where coeff_b = high_v - ((delta_v/delta_x) * high_x) + atten_offset
    *coeff_b = TP_HIGH_VOLTAGE - ((delta_v * high + (delta_x / 2)) / delta_x) + atten_offsets[atten];
}

static void characterize_using_vref(adc_unit_t adc_num,
                                    adc_atten_t atten,
                                    uint32_t vref,
                                    uint32_t *coeff_a,
                                    uint32_t *coeff_b)
{
    const uint32_t *atten_scales;
    const uint32_t *atten_offsets;

    if (adc_num == ADC_UNIT_1) { //Using ADC 1
        atten_scales = adc1_vref_atten_scale;
        atten_offsets = adc1_vref_atten_offset;
    } else {    //Using ADC 2
        atten_scales = adc2_vref_atten_scale;
        atten_offsets = adc2_vref_atten_offset;
    }
    //Characterize ADC-Voltage curve as y = (coeff_a * x) + coeff_b
    //Where coeff_a = (vref/4096) * atten_scale
    *coeff_a = (vref * atten_scales[atten]) / (ADC_12_BIT_RES);
    *coeff_b = atten_offsets[atten];
}

/* ------------------------ Conversion Functions --------------------------- */
static uint32_t calculate_voltage_linear(uint32_t adc_reading, uint32_t coeff_a, uint32_t coeff_b)
{
    //Where voltage = coeff_a * adc_reading + coeff_b
    return (((coeff_a * adc_reading) + LIN_COEFF_A_ROUND) / LIN_COEFF_A_SCALE) + coeff_b;
}

//Only call when ADC reading is above threshold
static uint32_t calculate_voltage_lut(uint32_t adc, uint32_t vref, const uint32_t *low_vref_curve, const uint32_t *high_vref_curve)
{
    //Get index of lower bound points of LUT
    uint32_t i = (adc - LUT_LOW_THRESH) / LUT_ADC_STEP_SIZE;

    //Let the X Axis be Vref, Y axis be ADC reading, and Z be voltage
    int x2dist = LUT_VREF_HIGH - vref;                 //(x2 - x)
    int x1dist = vref - LUT_VREF_LOW;                  //(x - x1)
    int y2dist = ((i + 1) * LUT_ADC_STEP_SIZE) + LUT_LOW_THRESH - adc;  //(y2 - y)
    int y1dist = adc - ((i * LUT_ADC_STEP_SIZE) + LUT_LOW_THRESH);        //(y - y1)

    //For points for bilinear interpolation
    int q11 = low_vref_curve[i];                    //Lower bound point of low_vref_curve
    int q12 = low_vref_curve[i + 1];                //Upper bound point of low_vref_curve
    int q21 = high_vref_curve[i];                   //Lower bound point of high_vref_curve
    int q22 = high_vref_curve[i + 1];               //Upper bound point of high_vref_curve

    //Bilinear interpolation
    //Where z = 1/((x2-x1)*(y2-y1)) * ( (q11*x2dist*y2dist) + (q21*x1dist*y2dist) + (q12*x2dist*y1dist) + (q22*x1dist*y1dist) )
    int voltage = (q11 * x2dist * y2dist) + (q21 * x1dist * y2dist) + (q12 * x2dist * y1dist) + (q22 * x1dist * y1dist);
    voltage += ((LUT_VREF_HIGH - LUT_VREF_LOW) * LUT_ADC_STEP_SIZE) / 2; //Integer division rounding
    voltage /= ((LUT_VREF_HIGH - LUT_VREF_LOW) * LUT_ADC_STEP_SIZE);    //Divide by ((x2-x1)*(y2-y1))
    return (uint32_t)voltage;
}

static inline uint32_t interpolate_two_points(uint32_t y1, uint32_t y2, uint32_t x_step, uint32_t x)
{
    //Interpolate between two points (x1,y1) (x2,y2) between 'lower' and 'upper' separated by 'step'
    return ((y1 * x_step) + (y2 * x) - (y1 * x) + (x_step / 2)) / x_step;
}

/* ------------------------- Public API ------------------------------------- */
esp_err_t esp_adc_cal_check_efuse(esp_adc_cal_value_t source)
{
    if (source == ESP_ADC_CAL_VAL_EFUSE_TP) {
        return (check_efuse_tp()) ? ESP_OK : ESP_ERR_NOT_SUPPORTED;
    } else if (source == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        return (check_efuse_vref()) ? ESP_OK : ESP_ERR_NOT_SUPPORTED;
    } else {
        return ESP_ERR_INVALID_ARG;
    }
}

esp_adc_cal_value_t esp_adc_cal_characterize(adc_unit_t adc_num,
                                             adc_atten_t atten,
                                             adc_bits_width_t bit_width,
                                             uint32_t default_vref,
                                             esp_adc_cal_characteristics_t *chars)
{
    //Check parameters
    assert((adc_num == ADC_UNIT_1) || (adc_num == ADC_UNIT_2));
    assert(chars != NULL);
    assert(bit_width < ADC_WIDTH_MAX);

    //Check eFuse if enabled to do so
    bool efuse_tp_present = check_efuse_tp();
    bool efuse_vref_present = check_efuse_vref();
    esp_adc_cal_value_t ret;

    if (efuse_tp_present && EFUSE_TP_ENABLED) {
        //Characterize based on Two Point values
        uint32_t high = read_efuse_tp_high(adc_num);
        uint32_t low = read_efuse_tp_low(adc_num);
        characterize_using_two_point(adc_num, atten, high, low, &chars->coeff_a, &chars->coeff_b);
        ret = ESP_ADC_CAL_VAL_EFUSE_TP;
    } else if (efuse_vref_present && EFUSE_VREF_ENABLED) {
        //Characterize based on eFuse Vref
        uint32_t vref = read_efuse_vref();
        characterize_using_vref(adc_num, atten, vref, &chars->coeff_a, &chars->coeff_b);
        ret = ESP_ADC_CAL_VAL_EFUSE_VREF;
    } else {
        //Characterized based on default Vref
        characterize_using_vref(adc_num, atten, default_vref, &chars->coeff_a, &chars->coeff_b);
        ret = ESP_ADC_CAL_VAL_DEFAULT_VREF;
    }

    //Initialized remaining fields
    chars->adc_num = adc_num;
    chars->atten = atten;
    chars->bit_width = bit_width;
    chars->vref = (EFUSE_VREF_ENABLED && efuse_vref_present) ? read_efuse_vref() : default_vref;
    //Initialize fields for lookup table if necessary
    if (LUT_ENABLED && atten == ADC_ATTEN_DB_12) {
        chars->low_curve = (adc_num == ADC_UNIT_1) ? lut_adc1_low : lut_adc2_low;
        chars->high_curve = (adc_num == ADC_UNIT_1) ? lut_adc1_high : lut_adc2_high;
    } else {
        chars->low_curve = NULL;
        chars->high_curve = NULL;
    }
    return ret;
}

uint32_t esp_adc_cal_raw_to_voltage(uint32_t adc_reading, const esp_adc_cal_characteristics_t *chars)
{
    assert(chars != NULL);

    //Scale adc_rading if not 12 bits wide
    adc_reading = (adc_reading << (ADC_WIDTH_BIT_12 - chars->bit_width));
    if (adc_reading > ADC_12_BIT_RES - 1) {
        adc_reading = ADC_12_BIT_RES - 1;    //Set to 12bit res max
    }

    if (LUT_ENABLED && (chars->atten == ADC_ATTEN_DB_12) && (adc_reading >= LUT_LOW_THRESH)) {  //Check if in non-linear region
        //Use lookup table to get voltage in non linear portion of ADC_ATTEN_DB_12
        uint32_t lut_voltage = calculate_voltage_lut(adc_reading, chars->vref, chars->low_curve, chars->high_curve);
        if (adc_reading <= LUT_HIGH_THRESH) {   //If ADC is transitioning from linear region to non-linear region
            //Linearly interpolate between linear voltage and lut voltage
            uint32_t linear_voltage = calculate_voltage_linear(adc_reading, chars->coeff_a, chars->coeff_b);
            return interpolate_two_points(linear_voltage, lut_voltage, LUT_ADC_STEP_SIZE, (adc_reading - LUT_LOW_THRESH));
        } else {
            return lut_voltage;
        }
    } else {
        return calculate_voltage_linear(adc_reading, chars->coeff_a, chars->coeff_b);
    }
}
