/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "assert.h"
#include "esp_types.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "hal/adc_types.h"
#include "hal/efuse_ll.h"
#include "soc/soc_caps.h"
#include "esp_adc/adc_cali_scheme.h"
#include "adc_cali_interface.h"

/* ----------------------------- Configuration ------------------------------ */
#ifdef CONFIG_ADC_CALI_EFUSE_TP_ENABLE
#define EFUSE_TP_ENABLED        1
#else
#define EFUSE_TP_ENABLED        0
#endif

#ifdef CONFIG_ADC_CALI_EFUSE_VREF_ENABLE
#define EFUSE_VREF_ENABLED      1
#else
#define EFUSE_VREF_ENABLED      0
#endif

#ifdef CONFIG_ADC_CALI_LUT_ENABLE
#define LUT_ENABLED             1
#else
#define LUT_ENABLED             0
#endif

/* ESP32s with both Two Point Values and Vref burned into eFuse are required to
 * also burn the EFUSE_BLK3_PART_RESERVE flag. A limited set of ESP32s
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
#define VREF_MASK                       0x1F
#define VREF_STEP_SIZE                  7
#define VREF_OFFSET                     1100

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
                                                  2754, 2795, 2832, 2868, 2903, 2937, 2969, 3000, 3030, 3060
                                                 };
static const uint32_t lut_adc1_high[LUT_POINTS] = {2667, 2706, 2745, 2780, 2813, 2844, 2873, 2901, 2928, 2956,
                                                   2982, 3006, 3032, 3059, 3084, 3110, 3135, 3160, 3184, 3209
                                                  };
static const uint32_t lut_adc2_low[LUT_POINTS] = {2238, 2293, 2347, 2399, 2451, 2507, 2561, 2613, 2662, 2710,
                                                  2754, 2792, 2831, 2869, 2904, 2937, 2968, 2999, 3029, 3059
                                                 };
static const uint32_t lut_adc2_high[LUT_POINTS] = {2657, 2698, 2738, 2774, 2807, 2838, 2867, 2894, 2921, 2946,
                                                   2971, 2996, 3020, 3043, 3067, 3092, 3116, 3139, 3162, 3185
                                                  };

const __attribute__((unused)) static char *TAG = "adc_cali";

/* ----------------------- EFuse Access Functions --------------------------- */
static bool check_efuse_vref(void);
static bool check_efuse_tp(void);
static inline int decode_bits(uint32_t bits, uint32_t mask, bool is_twos_compl);
static uint32_t read_efuse_vref(void);
static uint32_t read_efuse_tp_low(adc_unit_t unit_id);
static uint32_t read_efuse_tp_high(adc_unit_t unit_id);

/* ----------------------- Characterization Functions ----------------------- */
static void characterize_using_two_point(adc_unit_t unit_id,
                                         adc_atten_t atten,
                                         uint32_t high,
                                         uint32_t low,
                                         uint32_t *coeff_a,
                                         uint32_t *coeff_b);
static void characterize_using_vref(adc_unit_t unit_id,
                                    adc_atten_t atten,
                                    uint32_t vref,
                                    uint32_t *coeff_a,
                                    uint32_t *coeff_b);

/* ------------------------ Conversion Functions --------------------------- */
static uint32_t calculate_voltage_linear(uint32_t adc_reading, uint32_t coeff_a, uint32_t coeff_b);
//Only call when ADC reading is above threshold
static uint32_t calculate_voltage_lut(uint32_t adc, uint32_t vref, const uint32_t *low_vref_curve, const uint32_t *high_vref_curve);

static inline uint32_t interpolate_two_points(uint32_t y1, uint32_t y2, uint32_t x_step, uint32_t x)
{
    //Interpolate between two points (x1,y1) (x2,y2) between 'lower' and 'upper' separated by 'step'
    return ((y1 * x_step) + (y2 * x) - (y1 * x) + (x_step / 2)) / x_step;
}

/* ------------------------ Interface Functions --------------------------- */
static esp_err_t cali_raw_to_voltage(void *arg, int raw, int *voltage);

/* ------------------------ Context Structure--------------------------- */
typedef struct {
    adc_unit_t unit_id;                             ///< ADC unit
    adc_atten_t atten;                              ///< ADC attenuation
    adc_bitwidth_t bitwidth;                        ///< ADC bit width
    uint32_t coeff_a;                               ///< Gradient of ADC-Voltage curve
    uint32_t coeff_b;                               ///< Offset of ADC-Voltage curve
    uint32_t vref;                                  ///< Vref used by lookup table
    const uint32_t *low_curve;                      ///< Pointer to low Vref curve of lookup table (NULL if unused)
    const uint32_t *high_curve;                     ///< Pointer to high Vref curve of lookup table (NULL if unused)
    adc_cali_line_fitting_efuse_val_t efuse_val;    ///< Type of calibration value used in characterization
} cali_chars_line_fitting_t;

/* ------------------------- Public API ------------------------------------- */
esp_err_t adc_cali_create_scheme_line_fitting(const adc_cali_line_fitting_config_t *config, adc_cali_handle_t *ret_handle)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(config && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(config->unit_id < SOC_ADC_PERIPH_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid ADC unit");
    ESP_RETURN_ON_FALSE(config->atten < SOC_ADC_ATTEN_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid ADC attenuation");
    ESP_RETURN_ON_FALSE(((config->bitwidth >= SOC_ADC_RTC_MIN_BITWIDTH && config->bitwidth <= SOC_ADC_RTC_MAX_BITWIDTH) || config->bitwidth == ADC_BITWIDTH_DEFAULT), ESP_ERR_INVALID_ARG, TAG, "invalid bitwidth");

    adc_cali_scheme_t *scheme = (adc_cali_scheme_t *)heap_caps_calloc(1, sizeof(adc_cali_scheme_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(scheme, ESP_ERR_NO_MEM, TAG, "no mem for adc calibration scheme");

    cali_chars_line_fitting_t *chars = (cali_chars_line_fitting_t *)heap_caps_calloc(1, sizeof(cali_chars_line_fitting_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(chars, ESP_ERR_NO_MEM, err, TAG, "no memory for the calibration characteristics");

    //Check eFuse if enabled to do so
    if (check_efuse_tp() && EFUSE_TP_ENABLED) {
        //Characterize based on Two Point values
        chars->efuse_val = ADC_CALI_LINE_FITTING_EFUSE_VAL_EFUSE_TP;
        //Characterize based on Two Point values
        uint32_t high = read_efuse_tp_high(config->unit_id);
        uint32_t low = read_efuse_tp_low(config->unit_id);
        characterize_using_two_point(config->unit_id, config->atten, high, low, &chars->coeff_a, &chars->coeff_b);
    } else if (check_efuse_vref() && EFUSE_VREF_ENABLED) {
        //Characterize based on eFuse Vref
        chars->efuse_val = ADC_CALI_LINE_FITTING_EFUSE_VAL_EFUSE_VREF;
        chars->vref = read_efuse_vref();
        characterize_using_vref(config->unit_id, config->atten, chars->vref, &chars->coeff_a, &chars->coeff_b);
    } else {
        //Characterized based on default Vref
        chars->efuse_val = ADC_CALI_LINE_FITTING_EFUSE_VAL_DEFAULT_VREF;
        ESP_GOTO_ON_FALSE(config->default_vref, ESP_ERR_INVALID_ARG, err, TAG, "default vref didn't set");
        chars->vref = config->default_vref;
        characterize_using_vref(config->unit_id, config->atten, chars->vref, &chars->coeff_a, &chars->coeff_b);
    }

    chars->unit_id = config->unit_id;
    chars->atten = config->atten;
    chars->bitwidth = (config->bitwidth == ADC_BITWIDTH_DEFAULT) ? ADC_BITWIDTH_12 : config->bitwidth;
    //Initialize fields for lookup table if necessary
    if (LUT_ENABLED && config->atten == ADC_ATTEN_DB_12) {
        chars->low_curve = (config->unit_id == ADC_UNIT_1) ? lut_adc1_low : lut_adc2_low;
        chars->high_curve = (config->unit_id == ADC_UNIT_1) ? lut_adc1_high : lut_adc2_high;
    } else {
        chars->low_curve = NULL;
        chars->high_curve = NULL;
    }
    scheme->raw_to_voltage = cali_raw_to_voltage;
    scheme->ctx = chars;
    *ret_handle = scheme;

    return ESP_OK;

err:
    if (scheme) {
        free(scheme);
    }
    return ret;
}

esp_err_t adc_cali_scheme_line_fitting_check_efuse(adc_cali_line_fitting_efuse_val_t *cali_val)
{
    ESP_RETURN_ON_FALSE(cali_val, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    if (check_efuse_tp()) {
        *cali_val = ADC_CALI_LINE_FITTING_EFUSE_VAL_EFUSE_TP;
    } else if (check_efuse_vref()) {
        *cali_val = ADC_CALI_LINE_FITTING_EFUSE_VAL_EFUSE_VREF;
    } else {
        *cali_val = ADC_CALI_LINE_FITTING_EFUSE_VAL_DEFAULT_VREF;
    }

    return ESP_OK;
}

esp_err_t adc_cali_delete_scheme_line_fitting(adc_cali_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    free(handle->ctx);
    handle->ctx = NULL;

    free(handle);
    handle = NULL;

    return ESP_OK;
}

/* ------------------------ Interface Functions --------------------------- */
static esp_err_t cali_raw_to_voltage(void *arg, int raw, int *voltage)
{
    //pointers are checked in the upper layer

    cali_chars_line_fitting_t *ctx = arg;

    //Scale adc_rading if not 12 bits wide
    raw = (raw << (ADC_BITWIDTH_12 - ctx->bitwidth));
    if (raw > ADC_12_BIT_RES - 1) {
        raw = ADC_12_BIT_RES - 1;    //Set to 12bit res max
    }

    if (LUT_ENABLED && (ctx->atten == ADC_ATTEN_DB_12) && (raw >= LUT_LOW_THRESH)) {  //Check if in non-linear region
        //Use lookup table to get voltage in non linear portion of ADC_ATTEN_DB_12
        uint32_t lut_voltage = calculate_voltage_lut(raw, ctx->vref, ctx->low_curve, ctx->high_curve);
        if (raw <= LUT_HIGH_THRESH) {   //If ADC is transitioning from linear region to non-linear region
            //Linearly interpolate between linear voltage and lut voltage
            uint32_t linear_voltage = calculate_voltage_linear(raw, ctx->coeff_a, ctx->coeff_b);
            *voltage = interpolate_two_points(linear_voltage, lut_voltage, LUT_ADC_STEP_SIZE, (raw - LUT_LOW_THRESH));
        } else {
            *voltage = lut_voltage;
        }
    } else {
        *voltage = calculate_voltage_linear(raw, ctx->coeff_a, ctx->coeff_b);
    }

    return ESP_OK;
}

/* ----------------------- EFuse Access Functions --------------------------- */
static bool check_efuse_vref(void)
{
    //Check if Vref is burned in eFuse
    return (efuse_ll_get_adc_vref() != 0) ? true : false;
}

static bool check_efuse_tp(void)
{
    //Check if Two Point values are burned in eFuse
    if (CHECK_BLK3_FLAG && (efuse_ll_get_blk3_part_reserve() == 0)) {
        return false;
    }
    //All TP cal values must be non zero
    return efuse_ll_get_adc1_tp_low() &&
           efuse_ll_get_adc2_tp_low() &&
           efuse_ll_get_adc1_tp_high() &&
           efuse_ll_get_adc2_tp_high();
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
    uint32_t bits = efuse_ll_get_adc_vref();
    ret += decode_bits(bits, VREF_MASK, VREF_FORMAT) * VREF_STEP_SIZE;
    return ret;     //ADC Vref in mV
}

static uint32_t read_efuse_tp_low(adc_unit_t unit_id)
{
    //ADC reading at 150mV stored in two's complement format
    uint32_t ret;
    uint32_t bits;

    if (unit_id == ADC_UNIT_1) {
        ret = TP_LOW1_OFFSET;
        bits = efuse_ll_get_adc1_tp_low();
    } else {
        ret = TP_LOW2_OFFSET;
        bits = efuse_ll_get_adc2_tp_low();
    }
    ret += decode_bits(bits, TP_LOW_MASK, true) * TP_STEP_SIZE;
    return ret;     //Reading of ADC at 150mV
}

static uint32_t read_efuse_tp_high(adc_unit_t unit_id)
{
    //ADC reading at 850mV stored in two's complement format
    uint32_t ret;
    uint32_t bits;

    if (unit_id == ADC_UNIT_1) {
        ret = TP_HIGH1_OFFSET;
        bits = efuse_ll_get_adc1_tp_high();
    } else {
        ret = TP_HIGH2_OFFSET;
        bits = efuse_ll_get_adc2_tp_high();
    }
    ret += decode_bits(bits, TP_HIGH_MASK, true) * TP_STEP_SIZE;
    return ret;     //Reading of ADC at 850mV
}

/* ----------------------- Characterization Functions ----------------------- */
static void characterize_using_two_point(adc_unit_t unit_id,
                                         adc_atten_t atten,
                                         uint32_t high,
                                         uint32_t low,
                                         uint32_t *coeff_a,
                                         uint32_t *coeff_b)
{
    const uint32_t *atten_scales;
    const uint32_t *atten_offsets;

    if (unit_id == ADC_UNIT_1) { //Using ADC 1
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

static void characterize_using_vref(adc_unit_t unit_id,
                                    adc_atten_t atten,
                                    uint32_t vref,
                                    uint32_t *coeff_a,
                                    uint32_t *coeff_b)
{
    const uint32_t *atten_scales;
    const uint32_t *atten_offsets;

    if (unit_id == ADC_UNIT_1) { //Using ADC 1
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
