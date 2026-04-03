/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/misc.h"
#include "soc/pmu_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LDO_LL_RAIL_VOLTAGE_MV          3300

/**
 * @brief Convert voltage to dref and mul value
 *
 * @param ldo_unit    LDO unit
 * @param voltage_mv  Voltage in mV
 * @param dref        Returned dref value
 * @param mul         Returned mul value
 * @param use_rail_voltage Returned value to indicate if the rail voltage should be used
 */
__attribute__((always_inline))
static inline void ldo_ll_voltage_to_dref_mul(int ldo_unit, int voltage_mv, uint8_t *dref, uint8_t *mul, bool *use_rail_voltage)
{
    (void)ldo_unit;
    // to avoid using FPU, enlarge the constants by 1000 as fixed point
    int K_1000 = 1000;
    int Vos_1000 = 0;
    int C_1000 = 1000;

    // TODO: [ESP32S31] IDF-15510 For efuse calibration.

    // iterate all the possible dref and mul values to find the best match
    int min_voltage_diff = 400000000;
    uint8_t matched_dref = 0;
    uint8_t matched_mul = 0;
    for (uint8_t dref_val = 0; dref_val < 16; dref_val++) {
        int vref_20 = (dref_val < 9) ? (10 + dref_val) : (20 + (dref_val - 9) * 2);
        for (uint8_t mul_val = 0; mul_val < 8; mul_val++) {
            int vout_80000000 = (vref_20 * K_1000 + 20 * Vos_1000) * (4000 + mul_val * C_1000);
            int diff = voltage_mv * 80000 - vout_80000000;
            if (diff < 0) {
                diff = -diff;
            }
            if (diff < min_voltage_diff) {
                min_voltage_diff = diff;
                matched_dref = dref_val;
                matched_mul = mul_val;
            }
        }
    }
    *dref = matched_dref;
    *mul = matched_mul;
    // if the expected voltage is 3.3V, use the rail voltage directly
    *use_rail_voltage = (voltage_mv == LDO_LL_RAIL_VOLTAGE_MV);
}

/**
 * @brief Adjust voltage of a LDO unit
 *
 * @note When bypass is enabled, the input voltage is sourced directly to the output.
 *       The dref and mul values will be ignored.
 *
 * @param ldo_unit LDO unit
 * @param dref A parameter which controls the internal reference voltage
 * @param mul Multiply factor
 * @param use_rail_voltage Use rail voltage directly (i.e. bypass the LDO)
 */
__attribute__((always_inline))
static inline void ldo_ll_adjust_voltage(int ldo_unit, uint8_t dref, uint8_t mul, bool use_rail_voltage)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(PMU.ext_ldo_ctrl, ext_ldo_dref, dref);
    PMU.ext_ldo_ctrl.ext_ldo_mul = mul;
    PMU.ext_ldo_ctrl.ext_ldo_tie_high = use_rail_voltage;
}

/**
 * @brief Enable power on the PSRAM domain
 *
 * @param enable True: enable; False: disable
 */
__attribute__((always_inline))
static inline void ldo_ll_psram_power_enable(bool enable)
{
    PMU.psram_cfg.psram_xpd = enable;
}

/**
 * @brief Enable ripple suppression of a LDO unit
 *
 * @param ldo_unit LDO unit
 * @param enable True: enable; False: disable
 */
__attribute__((always_inline))
static inline void ldo_ll_enable_ripple_suppression(int ldo_unit, bool enable)
{
    (void)ldo_unit;
    PMU.ext_ldo_ctrl.ext_ldo_en_vdet = enable;
}

/**
 * @brief Enable current limit of a LDO unit to avoid inrush current
 *
 * @param ldo_unit LDO unit
 * @param enable True: enable; False: disable
 */
__attribute__((always_inline))
static inline void ldo_ll_enable_current_limit(int ldo_unit, bool enable)
{
    (void)ldo_unit;
    PMU.ext_ldo_ctrl.ext_cur_lim = enable;
}

#ifdef __cplusplus
}
#endif
