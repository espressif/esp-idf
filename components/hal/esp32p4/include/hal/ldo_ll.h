/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_bit_defs.h"
#include "hal/assert.h"
#include "soc/pmu_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * LDO capabilities
 */
#define LDO_LL_UNIT_NUM    4

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
 * Trick to be adapted to the LDO register structure
 *
 * In pmu_ext_ldo_info_t ext_ldo[6] registers:
 * - ext_ldo[0] is LDO1
 * - ext_ldo[3] is LDO2
 * - ext_ldo[1] is LDO3
 * - ext_ldo[4] is LDO4
 */
#define LDO_ID2INDEX(id)       (uint8_t[]){0,3,1,4}[id]

/**
 * LDO ID to real unit ID
 */
#define LDO_ID2UNIT(ldo_id)    ((ldo_id) - 1)

/**
 * @brief Check if a LDO ID is valid
 *
 * @return True for valid
 */
__attribute__((always_inline))
static inline bool ldo_ll_is_valid_ldo_id(int ldo_id)
{
    return ((ldo_id > 0) && (ldo_id <= LDO_LL_UNIT_NUM));
}

/**
 * @brief Enable a LDO
 *
 * @param ldo_id  LDO ID
 * @param enable  True: enable; False: disable
 */
__attribute__((always_inline))
static inline void ldo_ll_enable(int ldo_id, bool enable)
{
    HAL_ASSERT(ldo_id < LDO_LL_UNIT_NUM);

    PMU.ext_ldo[LDO_ID2INDEX(ldo_id)].pmu_ext_ldo.xpd = enable;
}

/**
 * @brief Enable a LDO
 *
 * @param ldo_id      LDO ID
 * @param voltage_mv  Voltage in mV
 */
__attribute__((always_inline))
static inline void ldo_ll_set_output_voltage_mv(int ldo_id, int voltage_mv)
{
    int dref = 0, mul = 0;

    /**
     * Vout = dref * mul
     *
     * mul reg[2:0]:
     * mul = 1~2.75, step = 0.25

     * dref reg[3:0]:
     * 0~8 : dref = 0.5V ~ 0.9V,  step 50mV
     * 9~15 : dref = 1.0V ~ 1.6V, step 100mV
     */
    if (voltage_mv <= 500) {
        dref = 0;
        mul = 0;
    } else if (voltage_mv <= 900) {
        mul = 0;
        dref = (voltage_mv - LDO_LL_EXT_LDO_DREF_VOL_L_BASE) / LDO_LL_EXT_LDO_DREF_VOL_L_STEP;
    } else if (voltage_mv <= 1600) {
        mul = 1;
        dref = 6 + (voltage_mv - LDO_LL_EXT_LDO_DREF_VOL_H_BASE) / LDO_LL_EXT_LDO_DREF_VOL_H_STEP;
    } else if (voltage_mv <= 2000) {
        mul = 4;
        dref = (voltage_mv / 2 - LDO_LL_EXT_LDO_DREF_VOL_L_BASE) / LDO_LL_EXT_LDO_DREF_VOL_L_STEP;
    } else if (voltage_mv <= 3200) {
        mul = 4;
        dref = 9 + (voltage_mv / 2 - LDO_LL_EXT_LDO_DREF_VOL_H_BASE) / LDO_LL_EXT_LDO_DREF_VOL_H_STEP;
    } else {
        mul = 7;
        dref = 15;
    }
    if (mul > 7) {
        mul = 7;
    }
    if (mul < 0) {
        mul = 0;
    }
    if (dref > 15) {
        dref = 15;
    }
    if (dref < 0) {
        dref = 0;
    }
    /**
     * tieh_sel:
     *  - 0: tieh;
     *  - 1: sdmmc0_tieh;
     *  - 2: 3.3V;
     *  - 3: sdmmc1_tieh;
     *
     * tieh:
     *  - 0: dref * mul
     *  - 1: 3.3V
     *
     * force_tieh_sel:
     *  - 0: efuse
     *  - 1: tieh_sel
     */
    PMU.ext_ldo[LDO_ID2INDEX(ldo_id)].pmu_ext_ldo.tieh_sel = 0;
    PMU.ext_ldo[LDO_ID2INDEX(ldo_id)].pmu_ext_ldo.tieh = 0;
    PMU.ext_ldo[LDO_ID2INDEX(ldo_id)].pmu_ext_ldo.force_tieh_sel = 1;
    PMU.ext_ldo[LDO_ID2INDEX(ldo_id)].pmu_ext_ldo_ana.dref = dref;
    PMU.ext_ldo[LDO_ID2INDEX(ldo_id)].pmu_ext_ldo_ana.mul = mul;
}

#ifdef __cplusplus
}
#endif
