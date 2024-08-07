/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_bit_defs.h"
#include "hal/misc.h"
#include "soc/pmu_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LDO_LL_NUM_UNITS            4    // NUmber of LDO units
#define LDO_LL_ADJUSTABLE_CHAN_MASK 0x0F // all the 4 channels can be adjustable

#define LDO_LL_MAX_VOLTAGE_MV 3300
#define LDO_LL_MIN_VOLTAGE_MV 500

/**
 * LDO LL macros, these macros are in the unit of mV
 */
#define LDO_LL_EXT_LDO_DREF_VOL_H_BASE 1000
#define LDO_LL_EXT_LDO_DREF_VOL_H_STEP 100
#define LDO_LL_EXT_LDO_DREF_VOL_L_BASE 500
#define LDO_LL_EXT_LDO_DREF_VOL_L_STEP 50
#define LDO_LL_EXT_LDO_MUL_VOL_BASE    1000
#define LDO_LL_EXT_LDO_MUL_VOL_STEP    250

/**
 * LDO ID to real unit ID
 */
#define LDO_ID2UNIT(ldo_id)    ((ldo_id) - 1)

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
    // TODO [IDF-10754]: also take the calibration parameters into account
    if (voltage_mv <= 500) {
        *dref = 0;
        *mul = 0;
    } else if (voltage_mv <= 900) {
        *mul = 0;
        *dref = (voltage_mv - LDO_LL_EXT_LDO_DREF_VOL_L_BASE) / LDO_LL_EXT_LDO_DREF_VOL_L_STEP;
    } else if (voltage_mv <= 1600) {
        *mul = 1;
        *dref = 6 + (voltage_mv - LDO_LL_EXT_LDO_DREF_VOL_H_BASE) / LDO_LL_EXT_LDO_DREF_VOL_H_STEP;
    } else if (voltage_mv <= 2000) {
        *mul = 4;
        *dref = (voltage_mv / 2 - LDO_LL_EXT_LDO_DREF_VOL_L_BASE) / LDO_LL_EXT_LDO_DREF_VOL_L_STEP;
    } else if (voltage_mv <= 3200) {
        *mul = 4;
        *dref = 9 + (voltage_mv / 2 - LDO_LL_EXT_LDO_DREF_VOL_H_BASE) / LDO_LL_EXT_LDO_DREF_VOL_H_STEP;
    } else {
        *mul = 7;
        *dref = 15;
    }
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
    PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo.xpd = enable;
}

/**
 * @brief Adjust voltage of a LDO unit
 *
 * @param ldo_unit LDO unit
 * @param dref A parameter which controls the internal reference voltage
 * @param mul Multiply factor
 */
__attribute__((always_inline))
static inline void ldo_ll_adjust_voltage(int ldo_unit, uint8_t dref, uint8_t mul)
{
    uint8_t index_array[LDO_LL_NUM_UNITS] = {0, 3, 1, 4};
    PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo_ana.dref = dref;
    PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo_ana.mul = mul;
    /**
     * tieh:
     *  - 0: Vref * Mul
     *  - 1: 3.3V
     *
     * tieh_sel:
     *  - 0: tieh;
     *  - 1: sdmmc0_tieh;
     *  - 2: 3.3V;
     *  - 3: sdmmc1_tieh;
     */
    PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo.tieh_sel = 0;
    PMU.ext_ldo[index_array[ldo_unit]].pmu_ext_ldo.tieh = 0;
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
