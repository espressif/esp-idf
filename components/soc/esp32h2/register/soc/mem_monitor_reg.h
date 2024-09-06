/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** MEM_MONITOR_LOG_SETTING_REG register
 *  log config register
 */
#define MEM_MONITOR_LOG_SETTING_REG (DR_REG_MEM_MONITOR_BASE + 0x0)
/** MEM_MONITOR_LOG_ENA : R/W; bitpos: [2:0]; default: 0;
 *  enable bus log. BIT0: hp-cpu, BIT1: lp-cpu, BIT2: DMA.823 don't support lp-cpu
 */
#define MEM_MONITOR_LOG_ENA    0x00000007U
#define MEM_MONITOR_LOG_ENA_M  (MEM_MONITOR_LOG_ENA_V << MEM_MONITOR_LOG_ENA_S)
#define MEM_MONITOR_LOG_ENA_V  0x00000007U
#define MEM_MONITOR_LOG_ENA_S  0
/** MEM_MONITOR_LOG_MODE : R/W; bitpos: [6:3]; default: 0;
 *  This field must be onehot. 4'b0001 : WR monitor, 4'b0010: WORD monitor, 4'b0100:
 *  HALFWORD monitor, 4'b1000: BYTE monitor.
 */
#define MEM_MONITOR_LOG_MODE    0x0000000FU
#define MEM_MONITOR_LOG_MODE_M  (MEM_MONITOR_LOG_MODE_V << MEM_MONITOR_LOG_MODE_S)
#define MEM_MONITOR_LOG_MODE_V  0x0000000FU
#define MEM_MONITOR_LOG_MODE_S  3
/** MEM_MONITOR_LOG_MEM_LOOP_ENABLE : R/W; bitpos: [7]; default: 1;
 *  Set 1 enable mem_loop, it will loop write at the range of MEM_START and MEM_END
 */
#define MEM_MONITOR_LOG_MEM_LOOP_ENABLE    (BIT(7))
#define MEM_MONITOR_LOG_MEM_LOOP_ENABLE_M  (MEM_MONITOR_LOG_MEM_LOOP_ENABLE_V << MEM_MONITOR_LOG_MEM_LOOP_ENABLE_S)
#define MEM_MONITOR_LOG_MEM_LOOP_ENABLE_V  0x00000001U
#define MEM_MONITOR_LOG_MEM_LOOP_ENABLE_S  7

/** MEM_MONITOR_LOG_CHECK_DATA_REG register
 *  check data register
 */
#define MEM_MONITOR_LOG_CHECK_DATA_REG (DR_REG_MEM_MONITOR_BASE + 0x4)
/** MEM_MONITOR_LOG_CHECK_DATA : R/W; bitpos: [31:0]; default: 0;
 *  The special check data, when write this special data, it will trigger logging.
 */
#define MEM_MONITOR_LOG_CHECK_DATA    0xFFFFFFFFU
#define MEM_MONITOR_LOG_CHECK_DATA_M  (MEM_MONITOR_LOG_CHECK_DATA_V << MEM_MONITOR_LOG_CHECK_DATA_S)
#define MEM_MONITOR_LOG_CHECK_DATA_V  0xFFFFFFFFU
#define MEM_MONITOR_LOG_CHECK_DATA_S  0

/** MEM_MONITOR_LOG_DATA_MASK_REG register
 *  check data mask register
 */
#define MEM_MONITOR_LOG_DATA_MASK_REG (DR_REG_MEM_MONITOR_BASE + 0x8)
/** MEM_MONITOR_LOG_DATA_MASK : R/W; bitpos: [3:0]; default: 0;
 *  byte mask enable, BIT0 mask the first byte of MEM_MONITOR_LOG_CHECK_DATA, and BIT1
 *  mask second byte, and so on.
 */
#define MEM_MONITOR_LOG_DATA_MASK    0x0000000FU
#define MEM_MONITOR_LOG_DATA_MASK_M  (MEM_MONITOR_LOG_DATA_MASK_V << MEM_MONITOR_LOG_DATA_MASK_S)
#define MEM_MONITOR_LOG_DATA_MASK_V  0x0000000FU
#define MEM_MONITOR_LOG_DATA_MASK_S  0

/** MEM_MONITOR_LOG_MIN_REG register
 *  log boundary register
 */
#define MEM_MONITOR_LOG_MIN_REG (DR_REG_MEM_MONITOR_BASE + 0xc)
/** MEM_MONITOR_LOG_MIN : R/W; bitpos: [31:0]; default: 0;
 *  the min address of log range
 */
#define MEM_MONITOR_LOG_MIN    0xFFFFFFFFU
#define MEM_MONITOR_LOG_MIN_M  (MEM_MONITOR_LOG_MIN_V << MEM_MONITOR_LOG_MIN_S)
#define MEM_MONITOR_LOG_MIN_V  0xFFFFFFFFU
#define MEM_MONITOR_LOG_MIN_S  0

/** MEM_MONITOR_LOG_MAX_REG register
 *  log boundary register
 */
#define MEM_MONITOR_LOG_MAX_REG (DR_REG_MEM_MONITOR_BASE + 0x10)
/** MEM_MONITOR_LOG_MAX : R/W; bitpos: [31:0]; default: 0;
 *  the max address of log range
 */
#define MEM_MONITOR_LOG_MAX    0xFFFFFFFFU
#define MEM_MONITOR_LOG_MAX_M  (MEM_MONITOR_LOG_MAX_V << MEM_MONITOR_LOG_MAX_S)
#define MEM_MONITOR_LOG_MAX_V  0xFFFFFFFFU
#define MEM_MONITOR_LOG_MAX_S  0

/** MEM_MONITOR_LOG_MEM_START_REG register
 *  log message store range register
 */
#define MEM_MONITOR_LOG_MEM_START_REG (DR_REG_MEM_MONITOR_BASE + 0x14)
/** MEM_MONITOR_LOG_MEM_START : R/W; bitpos: [31:0]; default: 0;
 *  the start address of writing logging message
 */
#define MEM_MONITOR_LOG_MEM_START    0xFFFFFFFFU
#define MEM_MONITOR_LOG_MEM_START_M  (MEM_MONITOR_LOG_MEM_START_V << MEM_MONITOR_LOG_MEM_START_S)
#define MEM_MONITOR_LOG_MEM_START_V  0xFFFFFFFFU
#define MEM_MONITOR_LOG_MEM_START_S  0

/** MEM_MONITOR_LOG_MEM_END_REG register
 *  log message store range register
 */
#define MEM_MONITOR_LOG_MEM_END_REG (DR_REG_MEM_MONITOR_BASE + 0x18)
/** MEM_MONITOR_LOG_MEM_END : R/W; bitpos: [31:0]; default: 0;
 *  the end address of writing logging message
 */
#define MEM_MONITOR_LOG_MEM_END    0xFFFFFFFFU
#define MEM_MONITOR_LOG_MEM_END_M  (MEM_MONITOR_LOG_MEM_END_V << MEM_MONITOR_LOG_MEM_END_S)
#define MEM_MONITOR_LOG_MEM_END_V  0xFFFFFFFFU
#define MEM_MONITOR_LOG_MEM_END_S  0

/** MEM_MONITOR_LOG_MEM_CURRENT_ADDR_REG register
 *  current writing address.
 */
#define MEM_MONITOR_LOG_MEM_CURRENT_ADDR_REG (DR_REG_MEM_MONITOR_BASE + 0x1c)
/** MEM_MONITOR_LOG_MEM_CURRENT_ADDR : RO; bitpos: [31:0]; default: 0;
 *  means next writing address
 */
#define MEM_MONITOR_LOG_MEM_CURRENT_ADDR    0xFFFFFFFFU
#define MEM_MONITOR_LOG_MEM_CURRENT_ADDR_M  (MEM_MONITOR_LOG_MEM_CURRENT_ADDR_V << MEM_MONITOR_LOG_MEM_CURRENT_ADDR_S)
#define MEM_MONITOR_LOG_MEM_CURRENT_ADDR_V  0xFFFFFFFFU
#define MEM_MONITOR_LOG_MEM_CURRENT_ADDR_S  0

/** MEM_MONITOR_LOG_MEM_ADDR_UPDATE_REG register
 *  writing address update
 */
#define MEM_MONITOR_LOG_MEM_ADDR_UPDATE_REG (DR_REG_MEM_MONITOR_BASE + 0x20)
/** MEM_MONITOR_LOG_MEM_ADDR_UPDATE : WT; bitpos: [0]; default: 0;
 *  Set 1 to updata MEM_MONITOR_LOG_MEM_CURRENT_ADDR, when set 1,
 *  MEM_MONITOR_LOG_MEM_CURRENT_ADDR will update to MEM_MONITOR_LOG_MEM_START
 */
#define MEM_MONITOR_LOG_MEM_ADDR_UPDATE    (BIT(0))
#define MEM_MONITOR_LOG_MEM_ADDR_UPDATE_M  (MEM_MONITOR_LOG_MEM_ADDR_UPDATE_V << MEM_MONITOR_LOG_MEM_ADDR_UPDATE_S)
#define MEM_MONITOR_LOG_MEM_ADDR_UPDATE_V  0x00000001U
#define MEM_MONITOR_LOG_MEM_ADDR_UPDATE_S  0

/** MEM_MONITOR_LOG_MEM_FULL_FLAG_REG register
 *  full flag status register
 */
#define MEM_MONITOR_LOG_MEM_FULL_FLAG_REG (DR_REG_MEM_MONITOR_BASE + 0x24)
/** MEM_MONITOR_LOG_MEM_FULL_FLAG : RO; bitpos: [0]; default: 0;
 *  1 means memory write loop at least one time at the range of MEM_START and MEM_END
 */
#define MEM_MONITOR_LOG_MEM_FULL_FLAG    (BIT(0))
#define MEM_MONITOR_LOG_MEM_FULL_FLAG_M  (MEM_MONITOR_LOG_MEM_FULL_FLAG_V << MEM_MONITOR_LOG_MEM_FULL_FLAG_S)
#define MEM_MONITOR_LOG_MEM_FULL_FLAG_V  0x00000001U
#define MEM_MONITOR_LOG_MEM_FULL_FLAG_S  0
/** MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG : WT; bitpos: [1]; default: 0;
 *  Set 1 to clr MEM_MONITOR_LOG_MEM_FULL_FLAG
 */
#define MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG    (BIT(1))
#define MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG_M  (MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG_V << MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG_S)
#define MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG_V  0x00000001U
#define MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG_S  1

/** MEM_MONITOR_CLOCK_GATE_REG register
 *  clock gate force on register
 */
#define MEM_MONITOR_CLOCK_GATE_REG (DR_REG_MEM_MONITOR_BASE + 0x28)
/** MEM_MONITOR_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set 1 to force on the clk of mem_monitor register
 */
#define MEM_MONITOR_CLK_EN    (BIT(0))
#define MEM_MONITOR_CLK_EN_M  (MEM_MONITOR_CLK_EN_V << MEM_MONITOR_CLK_EN_S)
#define MEM_MONITOR_CLK_EN_V  0x00000001U
#define MEM_MONITOR_CLK_EN_S  0

/** MEM_MONITOR_DATE_REG register
 *  version register
 */
#define MEM_MONITOR_DATE_REG (DR_REG_MEM_MONITOR_BASE + 0x3fc)
/** MEM_MONITOR_DATE : R/W; bitpos: [27:0]; default: 35660096;
 *  version register
 */
#define MEM_MONITOR_DATE    0x0FFFFFFFU
#define MEM_MONITOR_DATE_M  (MEM_MONITOR_DATE_V << MEM_MONITOR_DATE_S)
#define MEM_MONITOR_DATE_V  0x0FFFFFFFU
#define MEM_MONITOR_DATE_S  0

#ifdef __cplusplus
}
#endif
