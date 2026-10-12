/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/adc_hal_common.h"

#if ADC_LL_SW_WEIGHT_CALIBRATION_SUPPORTED
#include <string.h>
#include "hal/adc_periph.h"

#define ADC_HAL_SW_WEIGHT_RAW_MASK     ((1U << ADC_HAL_SW_WEIGHT_BIT_NUM) - 1U)
#define ADC_HAL_SW_WEIGHT_CYCLES       5

static int32_t s_adc_hal_sw_weights[SOC_ADC_PERIPH_NUM][ADC_HAL_SW_WEIGHT_BIT_NUM];
static bool s_adc_hal_sw_calibrated[SOC_ADC_PERIPH_NUM];
static uint32_t s_adc_hal_sw_bit_counts[ADC_HAL_SW_WEIGHT_CONFIG_NUM][2][ADC_HAL_SW_WEIGHT_BIT_NUM];

static uint32_t adc_hal_sw_weight_fold(const int32_t *weights, uint32_t raw)
{
    uint32_t raw_data = raw & ADC_HAL_SW_WEIGHT_RAW_MASK;
    int64_t code = 0;
    for (int i = 0; i < ADC_HAL_SW_WEIGHT_BIT_NUM; i++) {
        if ((raw_data >> (ADC_HAL_SW_WEIGHT_BIT_NUM - 1 - i)) & 1U) {
            code += weights[i];
        }
    }
    if (code <= 0) {
        return 0;
    }
    return (uint32_t)((code + ADC_HAL_SW_WEIGHT_SCALE / 2) / ADC_HAL_SW_WEIGHT_SCALE);
}

uint32_t adc_hal_sw_weight_apply(adc_unit_t adc_n, uint32_t raw)
{
    return adc_hal_sw_weight_fold(s_adc_hal_sw_weights[adc_n], raw);
}

bool adc_hal_sw_weight_is_calibrated(adc_unit_t adc_n)
{
    return s_adc_hal_sw_calibrated[adc_n];
}

int32_t adc_hal_sw_weight_get(adc_unit_t adc_n, int bit_from_msb)
{
    return s_adc_hal_sw_weights[adc_n][bit_from_msb];
}

static uint32_t adc_hal_sw_weight_sample_once(adc_unit_t adc_n)
{
    uint32_t event = (adc_n == ADC_UNIT_1) ? ADC_LL_EVENT_ADC1_ONESHOT_DONE : ADC_LL_EVENT_ADC2_ONESHOT_DONE;
    adc_oneshot_ll_clear_event(event);
    adc_oneshot_ll_start(adc_n, true);
    while (!adc_oneshot_ll_get_event(event)) {
    }
    return adc_oneshot_ll_get_raw_result(adc_n) & ADC_HAL_SW_WEIGHT_RAW_MASK;
}

/* Histogram of one raw sample, MSB first:
 *   bit[b] += 1  if  raw[(BIT_NUM-1)-b] is set.
 * Later Σ weight[b] * count[b] equals the sum of weighted codes over
 * the N samples of this (cal_sel, polarity), without storing each raw.
 */
static void adc_hal_sw_weight_add_sample(int cfg, bool pol, uint32_t raw)
{
    uint32_t *bit_counts = s_adc_hal_sw_bit_counts[cfg][pol ? 1 : 0];
    uint32_t raw_data = raw & ADC_HAL_SW_WEIGHT_RAW_MASK;
    for (int b = 0; b < ADC_HAL_SW_WEIGHT_BIT_NUM; b++) {
        if ((raw_data >> (ADC_HAL_SW_WEIGHT_BIT_NUM - 1 - b)) & 1U) {
            bit_counts[b]++;
        }
    }
}

/* For each cal_sel (bit10..bit16), five refinement cycles:
 *   code_pol = Σ weight[b] * count[cfg][pol][b]
 *   weight[(BIT_NUM-1)-bit] += (code1 - code0) / (2 * N)
 * Each cfg writes back immediately so the next bit uses the new weights.
 */
static void adc_hal_sw_weight_compute(adc_unit_t adc_n)
{
    for (int cycle = 0; cycle < ADC_HAL_SW_WEIGHT_CYCLES; cycle++) {
        for (int cfg = 0; cfg < ADC_HAL_SW_WEIGHT_CONFIG_NUM; cfg++) {
            int64_t code1_sum = 0;
            int64_t code0_sum = 0;
            for (int b = 0; b < ADC_HAL_SW_WEIGHT_BIT_NUM; b++) {
                code1_sum += (int64_t)s_adc_hal_sw_weights[adc_n][b] * s_adc_hal_sw_bit_counts[cfg][1][b];
                code0_sum += (int64_t)s_adc_hal_sw_weights[adc_n][b] * s_adc_hal_sw_bit_counts[cfg][0][b];
            }
            const int idx = (ADC_HAL_SW_WEIGHT_BIT_NUM - 1) - adc_hal_sw_weight_cal_configs[cfg].bit;
            s_adc_hal_sw_weights[adc_n][idx] +=
                (int32_t)((code1_sum - code0_sum) / (2 * ADC_HAL_SW_WEIGHT_SAMPLES));
        }
    }
    s_adc_hal_sw_calibrated[adc_n] = true;
}

static void adc_hal_sw_weight_restore(adc_unit_t adc_n)
{
    adc_ll_set_cal_sel(adc_n, 0, 0);
    adc_ll_set_pol_sel(adc_n, false);
    adc_ll_set_cal_done(adc_n, true);
    adc_ll_weight_accumulator_bypass(adc_n, true);
    adc_oneshot_ll_stop(adc_n);
    adc_oneshot_ll_disable_all_unit();
}

void adc_hal_sw_weight_calibrate(adc_unit_t adc_n, adc_oneshot_clk_src_t clk_src)
{
    adc_ll_digi_clk_sel(clk_src);
    adc_ll_digi_controller_clk_div(ADC_LL_CLKM_DIV_NUM_DEFAULT, ADC_LL_CLKM_DIV_B_DEFAULT, ADC_LL_CLKM_DIV_A_DEFAULT);
    adc_ll_digi_set_clk_div(ADC_LL_DIGI_SAR_CLK_DIV_DEFAULT);
    adc_ll_weight_accumulator_bypass(adc_n, true);

    if (s_adc_hal_sw_calibrated[adc_n]) {
        adc_hal_sw_weight_restore(adc_n);
        return;
    }

    memcpy(s_adc_hal_sw_weights[adc_n], adc_hal_sw_weight_ideal_weights,
           sizeof(s_adc_hal_sw_weights[adc_n]));
    memset(s_adc_hal_sw_bit_counts, 0, sizeof(s_adc_hal_sw_bit_counts));

    /* Internal weight calibration, not a user-channel conversion:
     * 1. Force oneshot to channel 0 and clear cal_done (raw-cal mode).
     * 2. For each cal_sel (bit10..bit16): set polarity 1 then 0, take 2048
     *    samples, and count how often each raw bit is 1.
     * 3. Refine the ideal weights from those histograms, then restore the
     *    analog path and set cal_done so later reads use the weights.
     */
    adc_ll_set_power_manage(adc_n, ADC_LL_POWER_SW_ON);
    adc_hal_set_controller(adc_n, ADC_HAL_SINGLE_READ_MODE);
    adc_oneshot_ll_set_channel(adc_n, ADC_CHANNEL_0);
    adc_oneshot_ll_enable(adc_n);
    adc_ll_set_cal_done(adc_n, false);

    for (int cfg = 0; cfg < ADC_HAL_SW_WEIGHT_CONFIG_NUM; cfg++) {
        for (int pol = 1; pol >= 0; pol--) {
            const adc_hal_sw_weight_cal_config_t *cc = &adc_hal_sw_weight_cal_configs[cfg];
            adc_ll_set_cal_sel(adc_n, cc->cal_sel, cc->cal_sel_1);
            adc_ll_set_pol_sel(adc_n, (bool)pol);
            for (int n = 0; n < ADC_HAL_SW_WEIGHT_SAMPLES; n++) {
                adc_hal_sw_weight_add_sample(cfg, (bool)pol, adc_hal_sw_weight_sample_once(adc_n));
            }
        }
    }

    adc_hal_sw_weight_compute(adc_n);
    adc_hal_sw_weight_restore(adc_n);
}

#endif // ADC_LL_SW_WEIGHT_CALIBRATION_SUPPORTED
