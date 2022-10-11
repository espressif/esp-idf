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

/** TRACE_MEM_START_ADDR_REG register
 *  mem start addr
 */
#define TRACE_MEM_START_ADDR_REG (DR_REG_TRACE_BASE + 0x0)
/** TRACE_MEM_STAET_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  The start address of trace memory
 */
#define TRACE_MEM_STAET_ADDR    0xFFFFFFFFU
#define TRACE_MEM_STAET_ADDR_M  (TRACE_MEM_STAET_ADDR_V << TRACE_MEM_STAET_ADDR_S)
#define TRACE_MEM_STAET_ADDR_V  0xFFFFFFFFU
#define TRACE_MEM_STAET_ADDR_S  0

/** TRACE_MEM_END_ADDR_REG register
 *  mem end addr
 */
#define TRACE_MEM_END_ADDR_REG (DR_REG_TRACE_BASE + 0x4)
/** TRACE_MEM_END_ADDR : R/W; bitpos: [31:0]; default: 4294967295;
 *  The end address of trace memory
 */
#define TRACE_MEM_END_ADDR    0xFFFFFFFFU
#define TRACE_MEM_END_ADDR_M  (TRACE_MEM_END_ADDR_V << TRACE_MEM_END_ADDR_S)
#define TRACE_MEM_END_ADDR_V  0xFFFFFFFFU
#define TRACE_MEM_END_ADDR_S  0

/** TRACE_MEM_CURRENT_ADDR_REG register
 *  mem current addr
 */
#define TRACE_MEM_CURRENT_ADDR_REG (DR_REG_TRACE_BASE + 0x8)
/** TRACE_MEM_CURRENT_ADDR : RO; bitpos: [31:0]; default: 0;
 *  current_mem_addr,indicate that next writing addr
 */
#define TRACE_MEM_CURRENT_ADDR    0xFFFFFFFFU
#define TRACE_MEM_CURRENT_ADDR_M  (TRACE_MEM_CURRENT_ADDR_V << TRACE_MEM_CURRENT_ADDR_S)
#define TRACE_MEM_CURRENT_ADDR_V  0xFFFFFFFFU
#define TRACE_MEM_CURRENT_ADDR_S  0

/** TRACE_MEM_ADDR_UPDATE_REG register
 *  mem addr update
 */
#define TRACE_MEM_ADDR_UPDATE_REG (DR_REG_TRACE_BASE + 0xc)
/** TRACE_MEM_CURRENT_ADDR_UPDATE : WT; bitpos: [0]; default: 0;
 *  when set this reg, the current_mem_addr will update to start_addr
 */
#define TRACE_MEM_CURRENT_ADDR_UPDATE    (BIT(0))
#define TRACE_MEM_CURRENT_ADDR_UPDATE_M  (TRACE_MEM_CURRENT_ADDR_UPDATE_V << TRACE_MEM_CURRENT_ADDR_UPDATE_S)
#define TRACE_MEM_CURRENT_ADDR_UPDATE_V  0x00000001U
#define TRACE_MEM_CURRENT_ADDR_UPDATE_S  0

/** TRACE_FIFO_STATUS_REG register
 *  fifo status register
 */
#define TRACE_FIFO_STATUS_REG (DR_REG_TRACE_BASE + 0x10)
/** TRACE_FIFO_EMPTY : RO; bitpos: [0]; default: 1;
 *  1 indicate that fifo is empty
 */
#define TRACE_FIFO_EMPTY    (BIT(0))
#define TRACE_FIFO_EMPTY_M  (TRACE_FIFO_EMPTY_V << TRACE_FIFO_EMPTY_S)
#define TRACE_FIFO_EMPTY_V  0x00000001U
#define TRACE_FIFO_EMPTY_S  0
/** TRACE_WORK_STATUS : RO; bitpos: [1]; default: 0;
 *  mem_full interrupt status
 */
#define TRACE_WORK_STATUS    (BIT(1))
#define TRACE_WORK_STATUS_M  (TRACE_WORK_STATUS_V << TRACE_WORK_STATUS_S)
#define TRACE_WORK_STATUS_V  0x00000001U
#define TRACE_WORK_STATUS_S  1

/** TRACE_INTR_ENA_REG register
 *  interrupt enable register
 */
#define TRACE_INTR_ENA_REG (DR_REG_TRACE_BASE + 0x14)
/** TRACE_FIFO_OVERFLOW_INTR_ENA : R/W; bitpos: [0]; default: 0;
 *  Set 1 enable fifo_overflow interrupt
 */
#define TRACE_FIFO_OVERFLOW_INTR_ENA    (BIT(0))
#define TRACE_FIFO_OVERFLOW_INTR_ENA_M  (TRACE_FIFO_OVERFLOW_INTR_ENA_V << TRACE_FIFO_OVERFLOW_INTR_ENA_S)
#define TRACE_FIFO_OVERFLOW_INTR_ENA_V  0x00000001U
#define TRACE_FIFO_OVERFLOW_INTR_ENA_S  0
/** TRACE_MEM_FULL_INTR_ENA : R/W; bitpos: [1]; default: 0;
 *  Set 1 enable mem_full interrupt
 */
#define TRACE_MEM_FULL_INTR_ENA    (BIT(1))
#define TRACE_MEM_FULL_INTR_ENA_M  (TRACE_MEM_FULL_INTR_ENA_V << TRACE_MEM_FULL_INTR_ENA_S)
#define TRACE_MEM_FULL_INTR_ENA_V  0x00000001U
#define TRACE_MEM_FULL_INTR_ENA_S  1

/** TRACE_INTR_RAW_REG register
 *  interrupt status register
 */
#define TRACE_INTR_RAW_REG (DR_REG_TRACE_BASE + 0x18)
/** TRACE_FIFO_OVERFLOW_INTR_RAW : RO; bitpos: [0]; default: 0;
 *  fifo_overflow interrupt status
 */
#define TRACE_FIFO_OVERFLOW_INTR_RAW    (BIT(0))
#define TRACE_FIFO_OVERFLOW_INTR_RAW_M  (TRACE_FIFO_OVERFLOW_INTR_RAW_V << TRACE_FIFO_OVERFLOW_INTR_RAW_S)
#define TRACE_FIFO_OVERFLOW_INTR_RAW_V  0x00000001U
#define TRACE_FIFO_OVERFLOW_INTR_RAW_S  0
/** TRACE_MEM_FULL_INTR_RAW : RO; bitpos: [1]; default: 0;
 *  mem_full interrupt status
 */
#define TRACE_MEM_FULL_INTR_RAW    (BIT(1))
#define TRACE_MEM_FULL_INTR_RAW_M  (TRACE_MEM_FULL_INTR_RAW_V << TRACE_MEM_FULL_INTR_RAW_S)
#define TRACE_MEM_FULL_INTR_RAW_V  0x00000001U
#define TRACE_MEM_FULL_INTR_RAW_S  1

/** TRACE_INTR_CLR_REG register
 *  interrupt clear register
 */
#define TRACE_INTR_CLR_REG (DR_REG_TRACE_BASE + 0x1c)
/** TRACE_FIFO_OVERFLOW_INTR_CLR : WT; bitpos: [0]; default: 0;
 *  Set 1 clr fifo overflow interrupt
 */
#define TRACE_FIFO_OVERFLOW_INTR_CLR    (BIT(0))
#define TRACE_FIFO_OVERFLOW_INTR_CLR_M  (TRACE_FIFO_OVERFLOW_INTR_CLR_V << TRACE_FIFO_OVERFLOW_INTR_CLR_S)
#define TRACE_FIFO_OVERFLOW_INTR_CLR_V  0x00000001U
#define TRACE_FIFO_OVERFLOW_INTR_CLR_S  0
/** TRACE_MEM_FULL_INTR_CLR : WT; bitpos: [1]; default: 0;
 *  Set 1 clr mem full interrupt
 */
#define TRACE_MEM_FULL_INTR_CLR    (BIT(1))
#define TRACE_MEM_FULL_INTR_CLR_M  (TRACE_MEM_FULL_INTR_CLR_V << TRACE_MEM_FULL_INTR_CLR_S)
#define TRACE_MEM_FULL_INTR_CLR_V  0x00000001U
#define TRACE_MEM_FULL_INTR_CLR_S  1

/** TRACE_TRIGGER_REG register
 *  trigger register
 */
#define TRACE_TRIGGER_REG (DR_REG_TRACE_BASE + 0x20)
/** TRACE_TRIGGER_ON : WT; bitpos: [0]; default: 0;
 *  [0] set 1 start trace.
 */
#define TRACE_TRIGGER_ON    (BIT(0))
#define TRACE_TRIGGER_ON_M  (TRACE_TRIGGER_ON_V << TRACE_TRIGGER_ON_S)
#define TRACE_TRIGGER_ON_V  0x00000001U
#define TRACE_TRIGGER_ON_S  0
/** TRACE_TRIGGER_OFF : WT; bitpos: [1]; default: 0;
 *  set 1 stop trace.
 */
#define TRACE_TRIGGER_OFF    (BIT(1))
#define TRACE_TRIGGER_OFF_M  (TRACE_TRIGGER_OFF_V << TRACE_TRIGGER_OFF_S)
#define TRACE_TRIGGER_OFF_V  0x00000001U
#define TRACE_TRIGGER_OFF_S  1
/** TRACE_MEM_LOOP : R/W; bitpos: [2]; default: 1;
 *  if this reg is 1, trace will loop wrtie trace_mem.  If is 0, when mem_current_addr
 *  at mem_end_addr, it will stop at the mem_end_addr
 */
#define TRACE_MEM_LOOP    (BIT(2))
#define TRACE_MEM_LOOP_M  (TRACE_MEM_LOOP_V << TRACE_MEM_LOOP_S)
#define TRACE_MEM_LOOP_V  0x00000001U
#define TRACE_MEM_LOOP_S  2
/** TRACE_RESTART_ENA : R/W; bitpos: [3]; default: 1;
 *  enable encoder auto-restart, when lost package, the encoder will end, if enable
 *  auto-restart, when fifo empty, encoder will restart and send a sync package.
 */
#define TRACE_RESTART_ENA    (BIT(3))
#define TRACE_RESTART_ENA_M  (TRACE_RESTART_ENA_V << TRACE_RESTART_ENA_S)
#define TRACE_RESTART_ENA_V  0x00000001U
#define TRACE_RESTART_ENA_S  3

/** TRACE_RESYNC_PROLONGED_REG register
 *  resync configuration register
 */
#define TRACE_RESYNC_PROLONGED_REG (DR_REG_TRACE_BASE + 0x24)
/** TRACE_RESYNC_PROLONGED : R/W; bitpos: [23:0]; default: 128;
 *  count number, when count to this value, send a sync package
 */
#define TRACE_RESYNC_PROLONGED    0x00FFFFFFU
#define TRACE_RESYNC_PROLONGED_M  (TRACE_RESYNC_PROLONGED_V << TRACE_RESYNC_PROLONGED_S)
#define TRACE_RESYNC_PROLONGED_V  0x00FFFFFFU
#define TRACE_RESYNC_PROLONGED_S  0
/** TRACE_RESYNC_MODE : R/W; bitpos: [24]; default: 0;
 *  resyc mode sel: 0: default, cycle count   1: package num count
 */
#define TRACE_RESYNC_MODE    (BIT(24))
#define TRACE_RESYNC_MODE_M  (TRACE_RESYNC_MODE_V << TRACE_RESYNC_MODE_S)
#define TRACE_RESYNC_MODE_V  0x00000001U
#define TRACE_RESYNC_MODE_S  24

/** TRACE_CLOCK_GATE_REG register
 *  Clock gate control register
 */
#define TRACE_CLOCK_GATE_REG (DR_REG_TRACE_BASE + 0x28)
/** TRACE_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  The bit is used to enable clock gate when access all registers in this module.
 */
#define TRACE_CLK_EN    (BIT(0))
#define TRACE_CLK_EN_M  (TRACE_CLK_EN_V << TRACE_CLK_EN_S)
#define TRACE_CLK_EN_V  0x00000001U
#define TRACE_CLK_EN_S  0

/** TRACE_DATE_REG register
 *  Version control register
 */
#define TRACE_DATE_REG (DR_REG_TRACE_BASE + 0x3fc)
/** TRACE_DATE : R/W; bitpos: [27:0]; default: 35663920;
 *  version control register. Note that this default value stored is the latest date
 *  when the hardware logic was updated.
 */
#define TRACE_DATE    0x0FFFFFFFU
#define TRACE_DATE_M  (TRACE_DATE_V << TRACE_DATE_S)
#define TRACE_DATE_V  0x0FFFFFFFU
#define TRACE_DATE_S  0

#ifdef __cplusplus
}
#endif
