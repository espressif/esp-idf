/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
 *  Configures bus access logging
 */
#define MEM_MONITOR_LOG_SETTING_REG (DR_REG_MEM_MONITOR_BASE + 0x0)
/** MEM_MONITOR_LOG_MODE : R/W; bitpos: [3:0]; default: 0;
 *  Configures monitoring modes.
 *  1: Enable write monitoring
 *  2: Enable word monitoring
 *  4: Enable halfword monitoring
 *  8: Enable byte monitoring
 *  Other values: Invalid
 */
#define MEM_MONITOR_LOG_MODE    0x0000000FU
#define MEM_MONITOR_LOG_MODE_M  (MEM_MONITOR_LOG_MODE_V << MEM_MONITOR_LOG_MODE_S)
#define MEM_MONITOR_LOG_MODE_V  0x0000000FU
#define MEM_MONITOR_LOG_MODE_S  0
/** MEM_MONITOR_LOG_MEM_LOOP_ENABLE : R/W; bitpos: [4]; default: 1;
 *  Configures the writing mode for recorded data.
 *  0: Non-loop mode
 *  1: Loop mode
 */
#define MEM_MONITOR_LOG_MEM_LOOP_ENABLE    (BIT(4))
#define MEM_MONITOR_LOG_MEM_LOOP_ENABLE_M  (MEM_MONITOR_LOG_MEM_LOOP_ENABLE_V << MEM_MONITOR_LOG_MEM_LOOP_ENABLE_S)
#define MEM_MONITOR_LOG_MEM_LOOP_ENABLE_V  0x00000001U
#define MEM_MONITOR_LOG_MEM_LOOP_ENABLE_S  4
/** MEM_MONITOR_LOG_CORE_ENA : R/W; bitpos: [15:8]; default: 0;
 *  Configures whether to enable HP CPU bus access logging.
 *  0: Disable
 *  1: Enable
 *  Other values: Invalid
 */
#define MEM_MONITOR_LOG_CORE_ENA    0x000000FFU
#define MEM_MONITOR_LOG_CORE_ENA_M  (MEM_MONITOR_LOG_CORE_ENA_V << MEM_MONITOR_LOG_CORE_ENA_S)
#define MEM_MONITOR_LOG_CORE_ENA_V  0x000000FFU
#define MEM_MONITOR_LOG_CORE_ENA_S  8
/** MEM_MONITOR_LOG_DMA_0_ENA : R/W; bitpos: [23:16]; default: 0;
 *  Configures whether to enable DMA_0 bus access logging.
 *  0: Disable
 *  1: Enable
 *  Other values: Invalid
 */
#define MEM_MONITOR_LOG_DMA_0_ENA    0x000000FFU
#define MEM_MONITOR_LOG_DMA_0_ENA_M  (MEM_MONITOR_LOG_DMA_0_ENA_V << MEM_MONITOR_LOG_DMA_0_ENA_S)
#define MEM_MONITOR_LOG_DMA_0_ENA_V  0x000000FFU
#define MEM_MONITOR_LOG_DMA_0_ENA_S  16
/** MEM_MONITOR_LOG_DMA_1_ENA : R/W; bitpos: [31:24]; default: 0;
 *  Configures whether to enable DMA_1 bus access logging.
 *  0: Disable
 *  1: Enable
 *  Other values: Invalid
 */
#define MEM_MONITOR_LOG_DMA_1_ENA    0x000000FFU
#define MEM_MONITOR_LOG_DMA_1_ENA_M  (MEM_MONITOR_LOG_DMA_1_ENA_V << MEM_MONITOR_LOG_DMA_1_ENA_S)
#define MEM_MONITOR_LOG_DMA_1_ENA_V  0x000000FFU
#define MEM_MONITOR_LOG_DMA_1_ENA_S  24

/** MEM_MONITOR_LOG_SETTING1_REG register
 *  Configures bus access logging
 */
#define MEM_MONITOR_LOG_SETTING1_REG (DR_REG_MEM_MONITOR_BASE + 0x4)
/** MEM_MONITOR_LOG_DMA_2_ENA : R/W; bitpos: [7:0]; default: 0;
 *  Configures whether to enable DMA_2 bus access logging.
 *  0: Disable
 *  1: Enable
 *  Other values: Invalid
 */
#define MEM_MONITOR_LOG_DMA_2_ENA    0x000000FFU
#define MEM_MONITOR_LOG_DMA_2_ENA_M  (MEM_MONITOR_LOG_DMA_2_ENA_V << MEM_MONITOR_LOG_DMA_2_ENA_S)
#define MEM_MONITOR_LOG_DMA_2_ENA_V  0x000000FFU
#define MEM_MONITOR_LOG_DMA_2_ENA_S  0
/** MEM_MONITOR_LOG_DMA_3_ENA : R/W; bitpos: [15:8]; default: 0;
 *  Configures whether to enable DMA_3 bus access logging.
 *  0: Disable
 *  1: Enable
 *  Other values: Invalid
 */
#define MEM_MONITOR_LOG_DMA_3_ENA    0x000000FFU
#define MEM_MONITOR_LOG_DMA_3_ENA_M  (MEM_MONITOR_LOG_DMA_3_ENA_V << MEM_MONITOR_LOG_DMA_3_ENA_S)
#define MEM_MONITOR_LOG_DMA_3_ENA_V  0x000000FFU
#define MEM_MONITOR_LOG_DMA_3_ENA_S  8

/** MEM_MONITOR_LOG_CHECK_DATA_REG register
 *  Configures data to be monitored for bus access logging
 */
#define MEM_MONITOR_LOG_CHECK_DATA_REG (DR_REG_MEM_MONITOR_BASE + 0x8)
/** MEM_MONITOR_LOG_CHECK_DATA : R/W; bitpos: [31:0]; default: 0;
 *  Configures the data to be monitored during bus accessing.
 */
#define MEM_MONITOR_LOG_CHECK_DATA    0xFFFFFFFFU
#define MEM_MONITOR_LOG_CHECK_DATA_M  (MEM_MONITOR_LOG_CHECK_DATA_V << MEM_MONITOR_LOG_CHECK_DATA_S)
#define MEM_MONITOR_LOG_CHECK_DATA_V  0xFFFFFFFFU
#define MEM_MONITOR_LOG_CHECK_DATA_S  0

/** MEM_MONITOR_LOG_DATA_MASK_REG register
 *  Configures data mask for bus access logging
 */
#define MEM_MONITOR_LOG_DATA_MASK_REG (DR_REG_MEM_MONITOR_BASE + 0xc)
/** MEM_MONITOR_LOG_DATA_MASK : R/W; bitpos: [3:0]; default: 0;
 *  Configures which byte(s) in MEM_MONITOR_LOG_CHECK_DATA_REG to mask. Multiple bytes
 *  can be masked at the same time.
 *  bit[0]: Configures whether to mask the least significant byte of
 *  MEM_MONITOR_LOG_CHECK_DATA_REG.
 *  0: Not mask
 *  1: Mask
 *  bit[1]: Configures whether to mask the second least significant byte of
 *  MEM_MONITOR_LOG_CHECK_DATA_REG.
 *  0: Not mask
 *  1: Mask
 *  bit[2]: Configures whether to mask the second most significant byte of
 *  MEM_MONITOR_LOG_CHECK_DATA_REG.
 *  0: Not mask
 *  1: Mask
 *  bit[3]: Configures whether to mask the most significant byte of
 *  MEM_MONITOR_LOG_CHECK_DATA_REG.
 *  0: Not mask
 *  1: Mask
 */
#define MEM_MONITOR_LOG_DATA_MASK    0x0000000FU
#define MEM_MONITOR_LOG_DATA_MASK_M  (MEM_MONITOR_LOG_DATA_MASK_V << MEM_MONITOR_LOG_DATA_MASK_S)
#define MEM_MONITOR_LOG_DATA_MASK_V  0x0000000FU
#define MEM_MONITOR_LOG_DATA_MASK_S  0

/** MEM_MONITOR_LOG_MIN_REG register
 *  Configures the monitored lower address for bus access logging
 */
#define MEM_MONITOR_LOG_MIN_REG (DR_REG_MEM_MONITOR_BASE + 0x10)
/** MEM_MONITOR_LOG_MIN : R/W; bitpos: [31:0]; default: 0;
 *  Configures the lower bound address of the monitored address space.
 */
#define MEM_MONITOR_LOG_MIN    0xFFFFFFFFU
#define MEM_MONITOR_LOG_MIN_M  (MEM_MONITOR_LOG_MIN_V << MEM_MONITOR_LOG_MIN_S)
#define MEM_MONITOR_LOG_MIN_V  0xFFFFFFFFU
#define MEM_MONITOR_LOG_MIN_S  0

/** MEM_MONITOR_LOG_MAX_REG register
 *  Configures the monitored upper address for bus access logging
 */
#define MEM_MONITOR_LOG_MAX_REG (DR_REG_MEM_MONITOR_BASE + 0x14)
/** MEM_MONITOR_LOG_MAX : R/W; bitpos: [31:0]; default: 0;
 *  Configures the upper bound address of the monitored address space.
 */
#define MEM_MONITOR_LOG_MAX    0xFFFFFFFFU
#define MEM_MONITOR_LOG_MAX_M  (MEM_MONITOR_LOG_MAX_V << MEM_MONITOR_LOG_MAX_S)
#define MEM_MONITOR_LOG_MAX_V  0xFFFFFFFFU
#define MEM_MONITOR_LOG_MAX_S  0

/** MEM_MONITOR_LOG_MON_ADDR_UPDATE_0_REG register
 *  Configures whether to update the monitored address space for HP CPU bus access
 *  logging
 */
#define MEM_MONITOR_LOG_MON_ADDR_UPDATE_0_REG (DR_REG_MEM_MONITOR_BASE + 0x18)
/** MEM_MONITOR_LOG_MON_ADDR_CORE_UPDATE : WT; bitpos: [7:0]; default: 0;
 *  Configures whether to update the monitored address space of the HP CPU bus as the
 *  address space from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG.
 *  0: Not update
 *  1: Update
 *  Other values: Invalid
 */
#define MEM_MONITOR_LOG_MON_ADDR_CORE_UPDATE    0x000000FFU
#define MEM_MONITOR_LOG_MON_ADDR_CORE_UPDATE_M  (MEM_MONITOR_LOG_MON_ADDR_CORE_UPDATE_V << MEM_MONITOR_LOG_MON_ADDR_CORE_UPDATE_S)
#define MEM_MONITOR_LOG_MON_ADDR_CORE_UPDATE_V  0x000000FFU
#define MEM_MONITOR_LOG_MON_ADDR_CORE_UPDATE_S  0
/** MEM_MONITOR_LOG_MON_ADDR_ALL_UPDATE : WT; bitpos: [31]; default: 0;
 *  Configures whether to update the monitored address space of all masters as the
 *  address space from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG.
 *  0: Not update
 *  1: Update
 */
#define MEM_MONITOR_LOG_MON_ADDR_ALL_UPDATE    (BIT(31))
#define MEM_MONITOR_LOG_MON_ADDR_ALL_UPDATE_M  (MEM_MONITOR_LOG_MON_ADDR_ALL_UPDATE_V << MEM_MONITOR_LOG_MON_ADDR_ALL_UPDATE_S)
#define MEM_MONITOR_LOG_MON_ADDR_ALL_UPDATE_V  0x00000001U
#define MEM_MONITOR_LOG_MON_ADDR_ALL_UPDATE_S  31

/** MEM_MONITOR_LOG_MON_ADDR_UPDATE_1_REG register
 *  Configures whether to update the monitored address space for DMA_0 bus access
 *  logging
 */
#define MEM_MONITOR_LOG_MON_ADDR_UPDATE_1_REG (DR_REG_MEM_MONITOR_BASE + 0x1c)
/** MEM_MONITOR_LOG_MON_ADDR_DMA_0_UPDATE : WT; bitpos: [7:0]; default: 0;
 *  Configures whether to update the monitored address space of the DMA_0 bus as the
 *  address space from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG.
 *  0: Not update
 *  1: Update
 *  Other values: Invalid
 */
#define MEM_MONITOR_LOG_MON_ADDR_DMA_0_UPDATE    0x000000FFU
#define MEM_MONITOR_LOG_MON_ADDR_DMA_0_UPDATE_M  (MEM_MONITOR_LOG_MON_ADDR_DMA_0_UPDATE_V << MEM_MONITOR_LOG_MON_ADDR_DMA_0_UPDATE_S)
#define MEM_MONITOR_LOG_MON_ADDR_DMA_0_UPDATE_V  0x000000FFU
#define MEM_MONITOR_LOG_MON_ADDR_DMA_0_UPDATE_S  0
/** MEM_MONITOR_LOG_MON_ADDR_DMA_1_UPDATE : WT; bitpos: [15:8]; default: 0;
 *  Configures whether to update the monitored address space of the DMA_1 bus as the
 *  address space from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG.
 *  0: Not update
 *  1: Update
 *  Other values: Invalid
 */
#define MEM_MONITOR_LOG_MON_ADDR_DMA_1_UPDATE    0x000000FFU
#define MEM_MONITOR_LOG_MON_ADDR_DMA_1_UPDATE_M  (MEM_MONITOR_LOG_MON_ADDR_DMA_1_UPDATE_V << MEM_MONITOR_LOG_MON_ADDR_DMA_1_UPDATE_S)
#define MEM_MONITOR_LOG_MON_ADDR_DMA_1_UPDATE_V  0x000000FFU
#define MEM_MONITOR_LOG_MON_ADDR_DMA_1_UPDATE_S  8
/** MEM_MONITOR_LOG_MON_ADDR_DMA_2_UPDATE : WT; bitpos: [23:16]; default: 0;
 *  Configures whether to update the monitored address space of the DMA_2 bus as the
 *  address space from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG.
 *  0: Not update
 *  1: Update
 *  Other values: Invalid
 */
#define MEM_MONITOR_LOG_MON_ADDR_DMA_2_UPDATE    0x000000FFU
#define MEM_MONITOR_LOG_MON_ADDR_DMA_2_UPDATE_M  (MEM_MONITOR_LOG_MON_ADDR_DMA_2_UPDATE_V << MEM_MONITOR_LOG_MON_ADDR_DMA_2_UPDATE_S)
#define MEM_MONITOR_LOG_MON_ADDR_DMA_2_UPDATE_V  0x000000FFU
#define MEM_MONITOR_LOG_MON_ADDR_DMA_2_UPDATE_S  16
/** MEM_MONITOR_LOG_MON_ADDR_DMA_3_UPDATE : WT; bitpos: [31:24]; default: 0;
 *  Configures whether to update the monitored address space of the DMA_3 bus as the
 *  address space from MEM_MONITOR_LOG_MIN_REG to MEM_MONITOR_LOG_MAX_REG.
 *  0: Not update
 *  1: Update
 *  Other values: Invalid
 */
#define MEM_MONITOR_LOG_MON_ADDR_DMA_3_UPDATE    0x000000FFU
#define MEM_MONITOR_LOG_MON_ADDR_DMA_3_UPDATE_M  (MEM_MONITOR_LOG_MON_ADDR_DMA_3_UPDATE_V << MEM_MONITOR_LOG_MON_ADDR_DMA_3_UPDATE_S)
#define MEM_MONITOR_LOG_MON_ADDR_DMA_3_UPDATE_V  0x000000FFU
#define MEM_MONITOR_LOG_MON_ADDR_DMA_3_UPDATE_S  24

/** MEM_MONITOR_LOG_MEM_START_REG register
 *  Configures the starting address of the storage memory for recorded data
 */
#define MEM_MONITOR_LOG_MEM_START_REG (DR_REG_MEM_MONITOR_BASE + 0x20)
/** MEM_MONITOR_LOG_MEM_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the starting address of the storage space for recorded data.
 */
#define MEM_MONITOR_LOG_MEM_START    0xFFFFFFFFU
#define MEM_MONITOR_LOG_MEM_START_M  (MEM_MONITOR_LOG_MEM_START_V << MEM_MONITOR_LOG_MEM_START_S)
#define MEM_MONITOR_LOG_MEM_START_V  0xFFFFFFFFU
#define MEM_MONITOR_LOG_MEM_START_S  0

/** MEM_MONITOR_LOG_MEM_END_REG register
 *  Configures the end address of the storage memory for recorded data
 */
#define MEM_MONITOR_LOG_MEM_END_REG (DR_REG_MEM_MONITOR_BASE + 0x24)
/** MEM_MONITOR_LOG_MEM_END : R/W; bitpos: [31:0]; default: 0;
 *  Configures the ending address of the storage space for recorded data.
 */
#define MEM_MONITOR_LOG_MEM_END    0xFFFFFFFFU
#define MEM_MONITOR_LOG_MEM_END_M  (MEM_MONITOR_LOG_MEM_END_V << MEM_MONITOR_LOG_MEM_END_S)
#define MEM_MONITOR_LOG_MEM_END_V  0xFFFFFFFFU
#define MEM_MONITOR_LOG_MEM_END_S  0

/** MEM_MONITOR_LOG_MEM_CURRENT_ADDR_REG register
 *  Represents the address for the next write
 */
#define MEM_MONITOR_LOG_MEM_CURRENT_ADDR_REG (DR_REG_MEM_MONITOR_BASE + 0x28)
/** MEM_MONITOR_LOG_MEM_CURRENT_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the next write.
 */
#define MEM_MONITOR_LOG_MEM_CURRENT_ADDR    0xFFFFFFFFU
#define MEM_MONITOR_LOG_MEM_CURRENT_ADDR_M  (MEM_MONITOR_LOG_MEM_CURRENT_ADDR_V << MEM_MONITOR_LOG_MEM_CURRENT_ADDR_S)
#define MEM_MONITOR_LOG_MEM_CURRENT_ADDR_V  0xFFFFFFFFU
#define MEM_MONITOR_LOG_MEM_CURRENT_ADDR_S  0

/** MEM_MONITOR_LOG_MEM_ADDR_UPDATE_REG register
 *  Updates the address for the next write with the starting address for the recorded
 *  data
 */
#define MEM_MONITOR_LOG_MEM_ADDR_UPDATE_REG (DR_REG_MEM_MONITOR_BASE + 0x2c)
/** MEM_MONITOR_LOG_MEM_ADDR_UPDATE : WT; bitpos: [0]; default: 0;
 *  Configures whether to update the value in MEM_MONITOR_LOG_MEM_START_REG to the
 *  value of MEM_MONITOR_LOG_MEM_CURRENT_ADDR_REG.
 *  0: Not update
 *  1: Update
 */
#define MEM_MONITOR_LOG_MEM_ADDR_UPDATE    (BIT(0))
#define MEM_MONITOR_LOG_MEM_ADDR_UPDATE_M  (MEM_MONITOR_LOG_MEM_ADDR_UPDATE_V << MEM_MONITOR_LOG_MEM_ADDR_UPDATE_S)
#define MEM_MONITOR_LOG_MEM_ADDR_UPDATE_V  0x00000001U
#define MEM_MONITOR_LOG_MEM_ADDR_UPDATE_S  0

/** MEM_MONITOR_LOG_MEM_FULL_FLAG_REG register
 *  Represents logging buffer overflow status register
 */
#define MEM_MONITOR_LOG_MEM_FULL_FLAG_REG (DR_REG_MEM_MONITOR_BASE + 0x30)
/** MEM_MONITOR_LOG_MEM_FULL_FLAG : RO; bitpos: [0]; default: 0;
 *  Represents whether data overflows the storage space.
 *  0: Not Overflow
 *  1: Overflow
 */
#define MEM_MONITOR_LOG_MEM_FULL_FLAG    (BIT(0))
#define MEM_MONITOR_LOG_MEM_FULL_FLAG_M  (MEM_MONITOR_LOG_MEM_FULL_FLAG_V << MEM_MONITOR_LOG_MEM_FULL_FLAG_S)
#define MEM_MONITOR_LOG_MEM_FULL_FLAG_V  0x00000001U
#define MEM_MONITOR_LOG_MEM_FULL_FLAG_S  0
/** MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG : WT; bitpos: [1]; default: 0;
 *  Configures whether to clear theMEM_MONITOR_LOG_MEM_FULL_FLAG flag bit.
 *  0: Not clear (default)
 *  1: Clear
 */
#define MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG    (BIT(1))
#define MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG_M  (MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG_V << MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG_S)
#define MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG_V  0x00000001U
#define MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG_S  1

/** MEM_MONITOR_CLOCK_GATE_REG register
 *  Clock gating control register
 */
#define MEM_MONITOR_CLOCK_GATE_REG (DR_REG_MEM_MONITOR_BASE + 0x34)
/** MEM_MONITOR_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether to enable the register clock gating.
 *  0: Disable
 *  1: Enable
 */
#define MEM_MONITOR_CLK_EN    (BIT(0))
#define MEM_MONITOR_CLK_EN_M  (MEM_MONITOR_CLK_EN_V << MEM_MONITOR_CLK_EN_S)
#define MEM_MONITOR_CLK_EN_V  0x00000001U
#define MEM_MONITOR_CLK_EN_S  0

/** MEM_MONITOR_DATE_REG register
 *  Version control register
 */
#define MEM_MONITOR_DATE_REG (DR_REG_MEM_MONITOR_BASE + 0x3fc)
/** MEM_MONITOR_DATE : R/W; bitpos: [27:0]; default: 36733248;
 *  Version control register.
 */
#define MEM_MONITOR_DATE    0x0FFFFFFFU
#define MEM_MONITOR_DATE_M  (MEM_MONITOR_DATE_V << MEM_MONITOR_DATE_S)
#define MEM_MONITOR_DATE_V  0x0FFFFFFFU
#define MEM_MONITOR_DATE_S  0

#ifdef __cplusplus
}
#endif
