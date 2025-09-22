/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** TSENS_CTRL_REG register
 *  Tsens configuration.
 */
#define TSENS_CTRL_REG (DR_REG_TSENS_BASE + 0x0)
/** TSENS_OUT : RO; bitpos: [7:0]; default: 0;
 *  Temperature sensor data out.
 */
#define TSENS_OUT    0x000000FFU
#define TSENS_OUT_M  (TSENS_OUT_V << TSENS_OUT_S)
#define TSENS_OUT_V  0x000000FFU
#define TSENS_OUT_S  0
/** TSENS_READY : RO; bitpos: [8]; default: 0;
 *  Indicate temperature sensor out ready.
 */
#define TSENS_READY    (BIT(8))
#define TSENS_READY_M  (TSENS_READY_V << TSENS_READY_S)
#define TSENS_READY_V  0x00000001U
#define TSENS_READY_S  8
/** TSENS_SAMPLE_EN : R/W; bitpos: [9]; default: 0;
 *  Enable sample signal for wakeup module.
 */
#define TSENS_SAMPLE_EN    (BIT(9))
#define TSENS_SAMPLE_EN_M  (TSENS_SAMPLE_EN_V << TSENS_SAMPLE_EN_S)
#define TSENS_SAMPLE_EN_V  0x00000001U
#define TSENS_SAMPLE_EN_S  9
/** TSENS_WAKEUP_MASK : R/W; bitpos: [10]; default: 1;
 *  Wake up signal  mask.
 */
#define TSENS_WAKEUP_MASK    (BIT(10))
#define TSENS_WAKEUP_MASK_M  (TSENS_WAKEUP_MASK_V << TSENS_WAKEUP_MASK_S)
#define TSENS_WAKEUP_MASK_V  0x00000001U
#define TSENS_WAKEUP_MASK_S  10
/** TSENS_INT_EN : R/W; bitpos: [12]; default: 1;
 *  Enable temperature sensor to send out interrupt.
 */
#define TSENS_INT_EN    (BIT(12))
#define TSENS_INT_EN_M  (TSENS_INT_EN_V << TSENS_INT_EN_S)
#define TSENS_INT_EN_V  0x00000001U
#define TSENS_INT_EN_S  12
/** TSENS_IN_INV : R/W; bitpos: [13]; default: 0;
 *  Invert temperature sensor data.
 */
#define TSENS_IN_INV    (BIT(13))
#define TSENS_IN_INV_M  (TSENS_IN_INV_V << TSENS_IN_INV_S)
#define TSENS_IN_INV_V  0x00000001U
#define TSENS_IN_INV_S  13
/** TSENS_CLK_DIV : R/W; bitpos: [21:14]; default: 6;
 *  Temperature sensor clock divider.
 */
#define TSENS_CLK_DIV    0x000000FFU
#define TSENS_CLK_DIV_M  (TSENS_CLK_DIV_V << TSENS_CLK_DIV_S)
#define TSENS_CLK_DIV_V  0x000000FFU
#define TSENS_CLK_DIV_S  14
/** TSENS_POWER_UP : R/W; bitpos: [22]; default: 0;
 *  Temperature sensor power up.
 */
#define TSENS_POWER_UP    (BIT(22))
#define TSENS_POWER_UP_M  (TSENS_POWER_UP_V << TSENS_POWER_UP_S)
#define TSENS_POWER_UP_V  0x00000001U
#define TSENS_POWER_UP_S  22
/** TSENS_POWER_UP_FORCE : R/W; bitpos: [23]; default: 0;
 *  1: dump out & power up controlled by SW,  0: by FSM.
 */
#define TSENS_POWER_UP_FORCE    (BIT(23))
#define TSENS_POWER_UP_FORCE_M  (TSENS_POWER_UP_FORCE_V << TSENS_POWER_UP_FORCE_S)
#define TSENS_POWER_UP_FORCE_V  0x00000001U
#define TSENS_POWER_UP_FORCE_S  23

/** TSENS_INT_RAW_REG register
 *  Tsens interrupt raw registers.
 */
#define TSENS_INT_RAW_REG (DR_REG_TSENS_BASE + 0x8)
/** TSENS_COCPU_TSENS_WAKE_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  Tsens wakeup interrupt raw.
 */
#define TSENS_COCPU_TSENS_WAKE_INT_RAW    (BIT(0))
#define TSENS_COCPU_TSENS_WAKE_INT_RAW_M  (TSENS_COCPU_TSENS_WAKE_INT_RAW_V << TSENS_COCPU_TSENS_WAKE_INT_RAW_S)
#define TSENS_COCPU_TSENS_WAKE_INT_RAW_V  0x00000001U
#define TSENS_COCPU_TSENS_WAKE_INT_RAW_S  0

/** TSENS_INT_ST_REG register
 *  Tsens interrupt status registers.
 */
#define TSENS_INT_ST_REG (DR_REG_TSENS_BASE + 0xc)
/** TSENS_COCPU_TSENS_WAKE_INT_ST : RO; bitpos: [0]; default: 0;
 *  Tsens wakeup interrupt status.
 */
#define TSENS_COCPU_TSENS_WAKE_INT_ST    (BIT(0))
#define TSENS_COCPU_TSENS_WAKE_INT_ST_M  (TSENS_COCPU_TSENS_WAKE_INT_ST_V << TSENS_COCPU_TSENS_WAKE_INT_ST_S)
#define TSENS_COCPU_TSENS_WAKE_INT_ST_V  0x00000001U
#define TSENS_COCPU_TSENS_WAKE_INT_ST_S  0

/** TSENS_INT_ENA_REG register
 *  Tsens interrupt enable registers.
 */
#define TSENS_INT_ENA_REG (DR_REG_TSENS_BASE + 0x10)
/** TSENS_COCPU_TSENS_WAKE_INT_ENA : R/WTC; bitpos: [0]; default: 0;
 *  Tsens wakeup interrupt enable.
 */
#define TSENS_COCPU_TSENS_WAKE_INT_ENA    (BIT(0))
#define TSENS_COCPU_TSENS_WAKE_INT_ENA_M  (TSENS_COCPU_TSENS_WAKE_INT_ENA_V << TSENS_COCPU_TSENS_WAKE_INT_ENA_S)
#define TSENS_COCPU_TSENS_WAKE_INT_ENA_V  0x00000001U
#define TSENS_COCPU_TSENS_WAKE_INT_ENA_S  0

/** TSENS_INT_CLR_REG register
 *  Tsens interrupt clear registers.
 */
#define TSENS_INT_CLR_REG (DR_REG_TSENS_BASE + 0x14)
/** TSENS_COCPU_TSENS_WAKE_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Tsens wakeup interrupt clear.
 */
#define TSENS_COCPU_TSENS_WAKE_INT_CLR    (BIT(0))
#define TSENS_COCPU_TSENS_WAKE_INT_CLR_M  (TSENS_COCPU_TSENS_WAKE_INT_CLR_V << TSENS_COCPU_TSENS_WAKE_INT_CLR_S)
#define TSENS_COCPU_TSENS_WAKE_INT_CLR_V  0x00000001U
#define TSENS_COCPU_TSENS_WAKE_INT_CLR_S  0

/** TSENS_CLK_CONF_REG register
 *  Tsens regbank configuration registers.
 */
#define TSENS_CLK_CONF_REG (DR_REG_TSENS_BASE + 0x18)
/** TSENS_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Tsens regbank clock gating enable.
 */
#define TSENS_CLK_EN    (BIT(0))
#define TSENS_CLK_EN_M  (TSENS_CLK_EN_V << TSENS_CLK_EN_S)
#define TSENS_CLK_EN_V  0x00000001U
#define TSENS_CLK_EN_S  0

/** TSENS_INT_ENA_W1TS_REG register
 *  Tsens wakeup interrupt enable assert.
 */
#define TSENS_INT_ENA_W1TS_REG (DR_REG_TSENS_BASE + 0x1c)
/** TSENS_COCPU_TSENS_WAKE_INT_ENA_W1TS : WT; bitpos: [0]; default: 0;
 *  Write 1 to this field to assert interrupt enable.
 */
#define TSENS_COCPU_TSENS_WAKE_INT_ENA_W1TS    (BIT(0))
#define TSENS_COCPU_TSENS_WAKE_INT_ENA_W1TS_M  (TSENS_COCPU_TSENS_WAKE_INT_ENA_W1TS_V << TSENS_COCPU_TSENS_WAKE_INT_ENA_W1TS_S)
#define TSENS_COCPU_TSENS_WAKE_INT_ENA_W1TS_V  0x00000001U
#define TSENS_COCPU_TSENS_WAKE_INT_ENA_W1TS_S  0

/** TSENS_INT_ENA_W1TC_REG register
 *  Tsens wakeup interrupt enable deassert.
 */
#define TSENS_INT_ENA_W1TC_REG (DR_REG_TSENS_BASE + 0x20)
/** TSENS_COCPU_TSENS_WAKE_INT_ENA_W1TC : WT; bitpos: [0]; default: 0;
 *  Write 1 to this field to deassert interrupt enable.
 */
#define TSENS_COCPU_TSENS_WAKE_INT_ENA_W1TC    (BIT(0))
#define TSENS_COCPU_TSENS_WAKE_INT_ENA_W1TC_M  (TSENS_COCPU_TSENS_WAKE_INT_ENA_W1TC_V << TSENS_COCPU_TSENS_WAKE_INT_ENA_W1TC_S)
#define TSENS_COCPU_TSENS_WAKE_INT_ENA_W1TC_V  0x00000001U
#define TSENS_COCPU_TSENS_WAKE_INT_ENA_W1TC_S  0

/** TSENS_WAKEUP_CTRL_REG register
 *  Tsens wakeup control registers.
 */
#define TSENS_WAKEUP_CTRL_REG (DR_REG_TSENS_BASE + 0x24)
/** TSENS_WAKEUP_TH_LOW : R/W; bitpos: [7:0]; default: 0;
 *  Lower threshold.
 */
#define TSENS_WAKEUP_TH_LOW    0x000000FFU
#define TSENS_WAKEUP_TH_LOW_M  (TSENS_WAKEUP_TH_LOW_V << TSENS_WAKEUP_TH_LOW_S)
#define TSENS_WAKEUP_TH_LOW_V  0x000000FFU
#define TSENS_WAKEUP_TH_LOW_S  0
/** TSENS_WAKEUP_TH_HIGH : R/W; bitpos: [21:14]; default: 255;
 *  Upper threshold.
 */
#define TSENS_WAKEUP_TH_HIGH    0x000000FFU
#define TSENS_WAKEUP_TH_HIGH_M  (TSENS_WAKEUP_TH_HIGH_V << TSENS_WAKEUP_TH_HIGH_S)
#define TSENS_WAKEUP_TH_HIGH_V  0x000000FFU
#define TSENS_WAKEUP_TH_HIGH_S  14
/** TSENS_WAKEUP_OVER_UPPER_TH : RO; bitpos: [29]; default: 0;
 *  Indicates that this wakeup event arose from exceeding upper threshold.
 */
#define TSENS_WAKEUP_OVER_UPPER_TH    (BIT(29))
#define TSENS_WAKEUP_OVER_UPPER_TH_M  (TSENS_WAKEUP_OVER_UPPER_TH_V << TSENS_WAKEUP_OVER_UPPER_TH_S)
#define TSENS_WAKEUP_OVER_UPPER_TH_V  0x00000001U
#define TSENS_WAKEUP_OVER_UPPER_TH_S  29
/** TSENS_WAKEUP_EN : R/W; bitpos: [30]; default: 0;
 *  Tsens wakeup enable.
 */
#define TSENS_WAKEUP_EN    (BIT(30))
#define TSENS_WAKEUP_EN_M  (TSENS_WAKEUP_EN_V << TSENS_WAKEUP_EN_S)
#define TSENS_WAKEUP_EN_V  0x00000001U
#define TSENS_WAKEUP_EN_S  30
/** TSENS_WAKEUP_MODE : R/W; bitpos: [31]; default: 0;
 *  0:absolute value comparison mode. 1: relative value comparison mode.
 */
#define TSENS_WAKEUP_MODE    (BIT(31))
#define TSENS_WAKEUP_MODE_M  (TSENS_WAKEUP_MODE_V << TSENS_WAKEUP_MODE_S)
#define TSENS_WAKEUP_MODE_V  0x00000001U
#define TSENS_WAKEUP_MODE_S  31

/** TSENS_SAMPLE_RATE_REG register
 *  Hardware automatic sampling control registers.
 */
#define TSENS_SAMPLE_RATE_REG (DR_REG_TSENS_BASE + 0x28)
/** TSENS_SAMPLE_RATE : R/W; bitpos: [15:0]; default: 20;
 *  Hardware automatic sampling rate.
 */
#define TSENS_SAMPLE_RATE    0x0000FFFFU
#define TSENS_SAMPLE_RATE_M  (TSENS_SAMPLE_RATE_V << TSENS_SAMPLE_RATE_S)
#define TSENS_SAMPLE_RATE_V  0x0000FFFFU
#define TSENS_SAMPLE_RATE_S  0

#ifdef __cplusplus
}
#endif
