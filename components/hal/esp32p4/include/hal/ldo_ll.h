/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/chip_revision.h"
#include "esp_bit_defs.h"
#include "hal/misc.h"
#include "hal/efuse_hal.h"
#include "hal/pmu_types.h"
#include "soc/pmu_struct.h"
#include "soc/efuse_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LDO_LL_NUM_UNITS            4    // Number of LDO units
#define LDO_LL_ADJUSTABLE_CHAN_MASK 0x0F // all the 4 channels are adjustable by setting "mul" and "dref" registers

#define LDO_LL_MAX_VOLTAGE_MV 3300
#define LDO_LL_MIN_VOLTAGE_MV 500

/**
 * @brief In the analog design, the LDO output "channel" is index from 1, i.e., VO1, VO2, VO3, VO4.
 *        But in software, we mapped them to "LDO unit", which is index from 0, i.e., 0, 1, 2, 3.
 */
#define LDO_ID2UNIT(ldo_id)  ((ldo_id) - 1)

/**
 * @brief LDO unit owner
 */
typedef enum {
    LDO_LL_UNIT_OWNER_HW, // LDO unit is controlled by hardware
    LDO_LL_UNIT_OWNER_SW, // LDO unit is controlled by software
} ldo_ll_unit_owner_t;

/**
 * @brief Check if a LDO channel is valid
 *
 * @param ldo_chan LDO channel ID, note, this is indexed from 1
 * @return True for valid, false for invalid
 */
__attribute__((always_inline))
static inline bool ldo_ll_is_valid_ldo_channel(int ldo_chan)
{
    return (ldo_chan > 0) && (ldo_chan <= LDO_LL_NUM_UNITS);
}

/**
 * @brief Convert voltage to dref and mul value
 *
 * @note Vref = (dref < 9)?(0.5+dref*0.05):(1+(dref-9)*0.1)
 * @note Vout = (Vref*K+Vos)*(1+0.25*mul*C), K, Vos, C are constants saved in the eFuse, for calibration
 *
 * @param ldo_unit    LDO unit
 * @param voltage_mv  Voltage in mV
 * @param dref        Returned dref value
 * @param mul         Returned mul value
 */
__attribute__((always_inline))
static inline void ldo_ll_voltage_to_dref_mul(int ldo_unit, int voltage_mv, uint8_t *dref, uint8_t *mul)
{
    uint8_t efuse_k = 0;
    uint8_t efuse_vos = 0;
    uint8_t efuse_c = 0;
    // to avoid using FPU, enlarge the constants by 1000 as fixed point
    int K_1000 = 1000;
    int Vos_1000 = 0;
    int C_1000 = 1000;

    if (efuse_hal_blk_version() >= 1) {
        // load the calibration values from the eFuse
        if (ldo_unit == 2) {
            efuse_k = EFUSE.rd_mac_sys_3.ldo_vo3_k;
            efuse_vos = EFUSE.rd_mac_sys_3.ldo_vo3_vos;
            efuse_c = EFUSE.rd_mac_sys_3.ldo_vo3_c;
        }
        if (ldo_unit == 3) {
            efuse_k = (EFUSE.rd_mac_sys_4.ldo_vo4_k_1 << 6) + EFUSE.rd_mac_sys_3.ldo_vo4_k;
            efuse_vos = EFUSE.rd_mac_sys_4.ldo_vo4_vos;
            efuse_c = EFUSE.rd_mac_sys_4.ldo_vo4_c;
        }
        // convert the eFuse calibration values to fixed point, note these values are signed
        if (efuse_k) {
            K_1000 = efuse_k & 0x80 ? -1 * (efuse_k & 0x7F) + 975 : efuse_k + 975;
        }
        if (efuse_vos) {
            Vos_1000 = efuse_vos & 0x20 ? -1 * (efuse_vos & 0x1F) - 3 : efuse_vos - 3;
        }
        if (efuse_c) {
            C_1000 = efuse_c & 0x20 ? -1 * (efuse_c & 0x1F) + 990 : efuse_c + 990;
        }
    }

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

    if (efuse_hal_blk_version() >= 1) {
        // For unit0 and unit1, the mul and dref value are calibrated and saved in the efuse, load them when available
        if (ldo_unit == 0 && voltage_mv == 1800) {
            if (EFUSE.rd_mac_sys_2.ldo_vo1_dref && EFUSE.rd_mac_sys_3.ldo_vo1_mul) {
                matched_mul = EFUSE.rd_mac_sys_3.ldo_vo1_mul;
                matched_dref = EFUSE.rd_mac_sys_2.ldo_vo1_dref;
            }
        }
        if (ldo_unit == 1 && voltage_mv == 1900) {
            if (EFUSE.rd_mac_sys_2.ldo_vo2_dref && EFUSE.rd_mac_sys_3.ldo_vo2_mul) {
                matched_mul = EFUSE.rd_mac_sys_3.ldo_vo2_mul;
                matched_dref = EFUSE.rd_mac_sys_2.ldo_vo2_dref;
            }
        }
    }

    *dref = matched_dref;
    *mul = matched_mul;
}

/**
 * @brief Set owner of a LDO unit
 *
 * @note Even if the LDO unit is controlled by hardware, its voltage can still be changed by software by `ldo_ll_adjust_voltage`
 *
 * @param ldo_unit LDO unit
 * @param owner Owner of the LDO unit
 */
__attribute__((always_inline))
static inline void ldo_ll_set_owner(int ldo_unit, ldo_ll_unit_owner_t owner)
{
    uint8_t index_array[LDO_LL_NUM_UNITS] = {0, 3, 1, 4};
    /*
     * force_tieh_sel:
     *  - 0: efuse, i.e. by hardware
     *  - 1: tieh_sel, i.e. by software
     */
    PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo.force_tieh_sel = owner;
    /**
     * tieh_sel:
     *  - 0: tieh;
     *  - 1: sdmmc0_tieh;
     *  - 2: 3.3V;
     *  - 3: sdmmc1_tieh;
     */
    PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo.tieh_sel = 0;
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
 * @param bypass True: bypass; False: not bypass.
 */
__attribute__((always_inline))
static inline void ldo_ll_adjust_voltage(int ldo_unit, uint8_t dref, uint8_t mul, bool bypass)
{
    uint8_t index_array[LDO_LL_NUM_UNITS] = {0, 3, 1, 4};
    /**
     * tieh:
     *  - 0: Vref * Mul
     *  - 1: 3.3V
     */
    PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo.tieh = bypass;
    PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo_ana.dref = dref;
    PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo_ana.mul = mul;
}

/**
 * @brief Enable a LDO unit
 *
 * @param ldo_unit LDO unit
 * @param enable  True: enable; False: disable
 */
__attribute__((always_inline))
static inline void ldo_ll_enable(int ldo_unit, bool enable)
{
    uint8_t index_array[LDO_LL_NUM_UNITS] = {0, 3, 1, 4};
    if (ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 100) && (ldo_unit == 0)) {
        // If chip_rev >= v1.0, slp_mem_dbias[3] is used to control the volt output of VO1.
        PMU.hp_sys[PMU_MODE_HP_ACTIVE].regulator0.xpd_0p1a = (enable ? 8 : 0);
    }
    PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo.xpd = enable;
}

/**
 * @brief Enable power on delay of a LDO unit
 *
 * @param ldo_unit LDO unit
 * @param enable True: enable; False: disable
 */
__attribute__((always_inline))
static inline void ldo_ll_enable_power_on_delay(int ldo_unit, bool enable)
{
    uint8_t index_array[LDO_LL_NUM_UNITS] = {0, 3, 1, 4};
    PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo.tieh_pos_en = enable;
}

/**
 * @brief Enable power off delay of a LDO unit
 *
 * @param ldo_unit LDO unit
 * @param enable True: enable; False: disable
 */
__attribute__((always_inline))
static inline void ldo_ll_enable_power_off_delay(int ldo_unit, bool enable)
{
    uint8_t index_array[LDO_LL_NUM_UNITS] = {0, 3, 1, 4};
    PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo.tieh_neg_en = enable;
}

/**
 * @brief Set power on delay target of a LDO unit
 *
 * @param ldo_unit LDO unit
 * @param target0 Target 0
 * @param target1 Target 1
 */
__attribute__((always_inline))
static inline void ldo_ll_set_delay_target(int ldo_unit, uint8_t target0, uint8_t target1)
{
    uint8_t index_array[LDO_LL_NUM_UNITS] = {0, 3, 1, 4};
    HAL_FORCE_MODIFY_U32_REG_FIELD(PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo, target0, target0);
    HAL_FORCE_MODIFY_U32_REG_FIELD(PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo, target1, target1);
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
    uint8_t index_array[LDO_LL_NUM_UNITS] = {0, 3, 1, 4};
    PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo_ana.en_cur_lim = enable;
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
    uint8_t index_array[LDO_LL_NUM_UNITS] = {0, 3, 1, 4};
    PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo_ana.en_vdet = enable;
}

#ifdef __cplusplus
}
#endif
