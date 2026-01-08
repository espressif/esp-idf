/**
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** LP_AON_STORE0_REG register
 *  store the software massege0 in always-on field
 */
#define LP_AON_STORE0_REG (DR_REG_LP_AON_BASE + 0x0)
/** LP_AON_LP_AON_STORE0 : R/W; bitpos: [31:0]; default: 0;
 *  store the software massege0 in always-on field
 */
#define LP_AON_LP_AON_STORE0    0xFFFFFFFFU
#define LP_AON_LP_AON_STORE0_M  (LP_AON_LP_AON_STORE0_V << LP_AON_LP_AON_STORE0_S)
#define LP_AON_LP_AON_STORE0_V  0xFFFFFFFFU
#define LP_AON_LP_AON_STORE0_S  0

/** LP_AON_STORE1_REG register
 *  store the software massege1 in always-on field
 */
#define LP_AON_STORE1_REG (DR_REG_LP_AON_BASE + 0x4)
/** LP_AON_LP_AON_STORE1 : R/W; bitpos: [31:0]; default: 0;
 *  store the software massege1 in always-on field
 */
#define LP_AON_LP_AON_STORE1    0xFFFFFFFFU
#define LP_AON_LP_AON_STORE1_M  (LP_AON_LP_AON_STORE1_V << LP_AON_LP_AON_STORE1_S)
#define LP_AON_LP_AON_STORE1_V  0xFFFFFFFFU
#define LP_AON_LP_AON_STORE1_S  0

/** LP_AON_STORE2_REG register
 *  store the software massege2 in always-on field
 */
#define LP_AON_STORE2_REG (DR_REG_LP_AON_BASE + 0x8)
/** LP_AON_LP_AON_STORE2 : R/W; bitpos: [31:0]; default: 0;
 *  store the software massege2 in always-on field
 */
#define LP_AON_LP_AON_STORE2    0xFFFFFFFFU
#define LP_AON_LP_AON_STORE2_M  (LP_AON_LP_AON_STORE2_V << LP_AON_LP_AON_STORE2_S)
#define LP_AON_LP_AON_STORE2_V  0xFFFFFFFFU
#define LP_AON_LP_AON_STORE2_S  0

/** LP_AON_STORE3_REG register
 *  store the software massege3 in always-on field
 */
#define LP_AON_STORE3_REG (DR_REG_LP_AON_BASE + 0xc)
/** LP_AON_LP_AON_STORE3 : R/W; bitpos: [31:0]; default: 0;
 *  store the software massege3 in always-on field
 */
#define LP_AON_LP_AON_STORE3    0xFFFFFFFFU
#define LP_AON_LP_AON_STORE3_M  (LP_AON_LP_AON_STORE3_V << LP_AON_LP_AON_STORE3_S)
#define LP_AON_LP_AON_STORE3_V  0xFFFFFFFFU
#define LP_AON_LP_AON_STORE3_S  0

/** LP_AON_STORE4_REG register
 *  store the software massege4 in always-on field
 */
#define LP_AON_STORE4_REG (DR_REG_LP_AON_BASE + 0x10)
/** LP_AON_LP_AON_STORE4 : R/W; bitpos: [31:0]; default: 0;
 *  store the software massege4 in always-on field
 */
#define LP_AON_LP_AON_STORE4    0xFFFFFFFFU
#define LP_AON_LP_AON_STORE4_M  (LP_AON_LP_AON_STORE4_V << LP_AON_LP_AON_STORE4_S)
#define LP_AON_LP_AON_STORE4_V  0xFFFFFFFFU
#define LP_AON_LP_AON_STORE4_S  0

/** LP_AON_STORE5_REG register
 *  store the software massege5 in always-on field
 */
#define LP_AON_STORE5_REG (DR_REG_LP_AON_BASE + 0x14)
/** LP_AON_LP_AON_STORE5 : R/W; bitpos: [31:0]; default: 0;
 *  store the software massege5 in always-on field
 */
#define LP_AON_LP_AON_STORE5    0xFFFFFFFFU
#define LP_AON_LP_AON_STORE5_M  (LP_AON_LP_AON_STORE5_V << LP_AON_LP_AON_STORE5_S)
#define LP_AON_LP_AON_STORE5_V  0xFFFFFFFFU
#define LP_AON_LP_AON_STORE5_S  0

/** LP_AON_STORE6_REG register
 *  store the software massege6 in always-on field
 */
#define LP_AON_STORE6_REG (DR_REG_LP_AON_BASE + 0x18)
/** LP_AON_LP_AON_STORE6 : R/W; bitpos: [31:0]; default: 0;
 *  store the software massege6 in always-on field
 */
#define LP_AON_LP_AON_STORE6    0xFFFFFFFFU
#define LP_AON_LP_AON_STORE6_M  (LP_AON_LP_AON_STORE6_V << LP_AON_LP_AON_STORE6_S)
#define LP_AON_LP_AON_STORE6_V  0xFFFFFFFFU
#define LP_AON_LP_AON_STORE6_S  0

/** LP_AON_STORE7_REG register
 *  store the software massege7 in always-on field
 */
#define LP_AON_STORE7_REG (DR_REG_LP_AON_BASE + 0x1c)
/** LP_AON_LP_AON_STORE7 : R/W; bitpos: [31:0]; default: 0;
 *  store the software massege7 in always-on field
 */
#define LP_AON_LP_AON_STORE7    0xFFFFFFFFU
#define LP_AON_LP_AON_STORE7_M  (LP_AON_LP_AON_STORE7_V << LP_AON_LP_AON_STORE7_S)
#define LP_AON_LP_AON_STORE7_V  0xFFFFFFFFU
#define LP_AON_LP_AON_STORE7_S  0

/** LP_AON_STORE8_REG register
 *  store the software massege8 in always-on field
 */
#define LP_AON_STORE8_REG (DR_REG_LP_AON_BASE + 0x20)
/** LP_AON_LP_AON_STORE8 : R/W; bitpos: [31:0]; default: 0;
 *  store the software massege8 in always-on field
 */
#define LP_AON_LP_AON_STORE8    0xFFFFFFFFU
#define LP_AON_LP_AON_STORE8_M  (LP_AON_LP_AON_STORE8_V << LP_AON_LP_AON_STORE8_S)
#define LP_AON_LP_AON_STORE8_V  0xFFFFFFFFU
#define LP_AON_LP_AON_STORE8_S  0

/** LP_AON_STORE9_REG register
 *  store the software massege9 in always-on field
 */
#define LP_AON_STORE9_REG (DR_REG_LP_AON_BASE + 0x24)
/** LP_AON_LP_AON_STORE9 : R/W; bitpos: [31:0]; default: 0;
 *  store the software massege9 in always-on field
 */
#define LP_AON_LP_AON_STORE9    0xFFFFFFFFU
#define LP_AON_LP_AON_STORE9_M  (LP_AON_LP_AON_STORE9_V << LP_AON_LP_AON_STORE9_S)
#define LP_AON_LP_AON_STORE9_V  0xFFFFFFFFU
#define LP_AON_LP_AON_STORE9_S  0

/** LP_AON_GPIO_MUX_REG register
 *  select the lp io controlled by hp iomux or lp iomux
 */
#define LP_AON_GPIO_MUX_REG (DR_REG_LP_AON_BASE + 0x28)
/** LP_AON_GPIO_MUX_SEL : R/W; bitpos: [5:0]; default: 0;
 *  select the lp io 0~5 controlled by hp iomux or lp iomux
 *  1: controlled by lp iomux
 *  0: controlled by hp iomux
 */
#define LP_AON_GPIO_MUX_SEL    0x0000003FU
#define LP_AON_GPIO_MUX_SEL_M  (LP_AON_GPIO_MUX_SEL_V << LP_AON_GPIO_MUX_SEL_S)
#define LP_AON_GPIO_MUX_SEL_V  0x0000003FU
#define LP_AON_GPIO_MUX_SEL_S  0

/** LP_AON_GPIO_HOLD0_REG register
 *  configure all io hold
 */
#define LP_AON_GPIO_HOLD0_REG (DR_REG_LP_AON_BASE + 0x2c)
/** LP_AON_GPIO_HOLD0 : R/W; bitpos: [31:0]; default: 0;
 *  configure io0~28 hold enable,when io in hold status, all io configure and output
 *  will be latch , input function is useful
 */
#define LP_AON_GPIO_HOLD0    0xFFFFFFFFU
#define LP_AON_GPIO_HOLD0_M  (LP_AON_GPIO_HOLD0_V << LP_AON_GPIO_HOLD0_S)
#define LP_AON_GPIO_HOLD0_V  0xFFFFFFFFU
#define LP_AON_GPIO_HOLD0_S  0

/** LP_AON_GPIO_HOLD1_REG register
 *  reserved
 */
#define LP_AON_GPIO_HOLD1_REG (DR_REG_LP_AON_BASE + 0x30)
/** LP_AON_GPIO_HOLD1 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define LP_AON_GPIO_HOLD1    0xFFFFFFFFU
#define LP_AON_GPIO_HOLD1_M  (LP_AON_GPIO_HOLD1_V << LP_AON_GPIO_HOLD1_S)
#define LP_AON_GPIO_HOLD1_V  0xFFFFFFFFU
#define LP_AON_GPIO_HOLD1_S  0

/** LP_AON_SYS_CFG_REG register
 *  configure system register
 */
#define LP_AON_SYS_CFG_REG (DR_REG_LP_AON_BASE + 0x34)
/** LP_AON_FIB_REG : RO; bitpos: [7:0]; default: 255;
 *  get fib reg information
 */
#define LP_AON_FIB_REG    0x000000FFU
#define LP_AON_FIB_REG_M  (LP_AON_FIB_REG_V << LP_AON_FIB_REG_S)
#define LP_AON_FIB_REG_V  0x000000FFU
#define LP_AON_FIB_REG_S  0
/** LP_AON_HPSYS_SW_RESET : WT; bitpos: [31]; default: 0;
 *  enable hp system reset by software or not
 *  1: reset
 *  0: no operation
 */
#define LP_AON_HPSYS_SW_RESET    (BIT(31))
#define LP_AON_HPSYS_SW_RESET_M  (LP_AON_HPSYS_SW_RESET_V << LP_AON_HPSYS_SW_RESET_S)
#define LP_AON_HPSYS_SW_RESET_V  0x00000001U
#define LP_AON_HPSYS_SW_RESET_S  31

/** LP_AON_CPUCORE_CFG_REG register
 *  configure core reset register
 */
#define LP_AON_CPUCORE_CFG_REG (DR_REG_LP_AON_BASE + 0x38)
/** LP_AON_CPU_CORE0_SW_STALL : R/W; bitpos: [7:0]; default: 0;
 *  enable cpu 0 entry stall status
 *  0x86: entry stall status
 *  Others : no operation
 */
#define LP_AON_CPU_CORE0_SW_STALL    0x000000FFU
#define LP_AON_CPU_CORE0_SW_STALL_M  (LP_AON_CPU_CORE0_SW_STALL_V << LP_AON_CPU_CORE0_SW_STALL_S)
#define LP_AON_CPU_CORE0_SW_STALL_V  0x000000FFU
#define LP_AON_CPU_CORE0_SW_STALL_S  0
/** LP_AON_CPU_CORE0_SW_RESET : WT; bitpos: [8]; default: 0;
 *  enable core 0 reset by software
 *  1: reset
 *  0: no operation
 */
#define LP_AON_CPU_CORE0_SW_RESET    (BIT(8))
#define LP_AON_CPU_CORE0_SW_RESET_M  (LP_AON_CPU_CORE0_SW_RESET_V << LP_AON_CPU_CORE0_SW_RESET_S)
#define LP_AON_CPU_CORE0_SW_RESET_V  0x00000001U
#define LP_AON_CPU_CORE0_SW_RESET_S  8
/** LP_AON_CPU_CORE1_SW_STALL : R/W; bitpos: [23:16]; default: 0;
 *  enable core 1 entry stall status
 *  0x86: entry stall status
 *  Others : no operation
 */
#define LP_AON_CPU_CORE1_SW_STALL    0x000000FFU
#define LP_AON_CPU_CORE1_SW_STALL_M  (LP_AON_CPU_CORE1_SW_STALL_V << LP_AON_CPU_CORE1_SW_STALL_S)
#define LP_AON_CPU_CORE1_SW_STALL_V  0x000000FFU
#define LP_AON_CPU_CORE1_SW_STALL_S  16
/** LP_AON_CPU_CORE1_SW_RESET : WT; bitpos: [24]; default: 0;
 *  enable core1 reset by software
 *  1: reset
 *  0: no operation
 */
#define LP_AON_CPU_CORE1_SW_RESET    (BIT(24))
#define LP_AON_CPU_CORE1_SW_RESET_M  (LP_AON_CPU_CORE1_SW_RESET_V << LP_AON_CPU_CORE1_SW_RESET_S)
#define LP_AON_CPU_CORE1_SW_RESET_V  0x00000001U
#define LP_AON_CPU_CORE1_SW_RESET_S  24
/** LP_AON_SYSTIMER_STALL_SEL : R/W; bitpos: [31]; default: 0;
 *  delete which core run_stall to lp_timer
 *  1: core1
 *  0: core0
 */
#define LP_AON_SYSTIMER_STALL_SEL    (BIT(31))
#define LP_AON_SYSTIMER_STALL_SEL_M  (LP_AON_SYSTIMER_STALL_SEL_V << LP_AON_SYSTIMER_STALL_SEL_S)
#define LP_AON_SYSTIMER_STALL_SEL_V  0x00000001U
#define LP_AON_SYSTIMER_STALL_SEL_S  31

/** LP_AON_IO_MUX_REG register
 *  configure hp iomux reset bypass
 */
#define LP_AON_IO_MUX_REG (DR_REG_LP_AON_BASE + 0x3c)
/** LP_AON_IO_MUX_PULL_LDO_EN : R/W; bitpos: [27]; default: 0;
 *  need_des
 */
#define LP_AON_IO_MUX_PULL_LDO_EN    (BIT(27))
#define LP_AON_IO_MUX_PULL_LDO_EN_M  (LP_AON_IO_MUX_PULL_LDO_EN_V << LP_AON_IO_MUX_PULL_LDO_EN_S)
#define LP_AON_IO_MUX_PULL_LDO_EN_V  0x00000001U
#define LP_AON_IO_MUX_PULL_LDO_EN_S  27
/** LP_AON_IO_MUX_PULL_LDO : R/W; bitpos: [30:28]; default: 0;
 *  need_des
 */
#define LP_AON_IO_MUX_PULL_LDO    0x00000007U
#define LP_AON_IO_MUX_PULL_LDO_M  (LP_AON_IO_MUX_PULL_LDO_V << LP_AON_IO_MUX_PULL_LDO_S)
#define LP_AON_IO_MUX_PULL_LDO_V  0x00000007U
#define LP_AON_IO_MUX_PULL_LDO_S  28
/** LP_AON_IO_MUX_RESET_DISABLE : R/W; bitpos: [31]; default: 0;
 *  bypass hp iomux reset from hp system reset event
 *  1: bypass
 *  0: no operation
 */
#define LP_AON_IO_MUX_RESET_DISABLE    (BIT(31))
#define LP_AON_IO_MUX_RESET_DISABLE_M  (LP_AON_IO_MUX_RESET_DISABLE_V << LP_AON_IO_MUX_RESET_DISABLE_S)
#define LP_AON_IO_MUX_RESET_DISABLE_V  0x00000001U
#define LP_AON_IO_MUX_RESET_DISABLE_S  31

/** LP_AON_EXT_WAKEUP_CNTL_REG register
 *  configure alwayson external io wakeup
 */
#define LP_AON_EXT_WAKEUP_CNTL_REG (DR_REG_LP_AON_BASE + 0x40)
/** LP_AON_EXT_WAKEUP_SEL : R/W; bitpos: [15:0]; default: 0;
 *  enable io0~15 bit map use to external wakeup
 *  1: enable
 *  0: disable
 */
#define LP_AON_EXT_WAKEUP_SEL    0x0000FFFFU
#define LP_AON_EXT_WAKEUP_SEL_M  (LP_AON_EXT_WAKEUP_SEL_V << LP_AON_EXT_WAKEUP_SEL_S)
#define LP_AON_EXT_WAKEUP_SEL_V  0x0000FFFFU
#define LP_AON_EXT_WAKEUP_SEL_S  0
/** LP_AON_EXT_WAKEUP_LV : R/W; bitpos: [31:16]; default: 0;
 *  select external wakeup io level
 *  1: io high level wakeup
 *  0: io low level wakeup
 */
#define LP_AON_EXT_WAKEUP_LV    0x0000FFFFU
#define LP_AON_EXT_WAKEUP_LV_M  (LP_AON_EXT_WAKEUP_LV_V << LP_AON_EXT_WAKEUP_LV_S)
#define LP_AON_EXT_WAKEUP_LV_V  0x0000FFFFU
#define LP_AON_EXT_WAKEUP_LV_S  16

/** LP_AON_EXT_WAKEUP_CNTL1_REG register
 *  configure alwayson external io wakeup
 */
#define LP_AON_EXT_WAKEUP_CNTL1_REG (DR_REG_LP_AON_BASE + 0x44)
/** LP_AON_EXT_WAKEUP_STATUS : RO; bitpos: [15:0]; default: 0;
 *  get external wakeup status bitmap
 */
#define LP_AON_EXT_WAKEUP_STATUS    0x0000FFFFU
#define LP_AON_EXT_WAKEUP_STATUS_M  (LP_AON_EXT_WAKEUP_STATUS_V << LP_AON_EXT_WAKEUP_STATUS_S)
#define LP_AON_EXT_WAKEUP_STATUS_V  0x0000FFFFU
#define LP_AON_EXT_WAKEUP_STATUS_S  0
/** LP_AON_EXT_WAKEUP_STATUS_CLR : WT; bitpos: [30]; default: 0;
 *  clear external wakeup status
 *  1: clear
 *  0: no operation
 */
#define LP_AON_EXT_WAKEUP_STATUS_CLR    (BIT(30))
#define LP_AON_EXT_WAKEUP_STATUS_CLR_M  (LP_AON_EXT_WAKEUP_STATUS_CLR_V << LP_AON_EXT_WAKEUP_STATUS_CLR_S)
#define LP_AON_EXT_WAKEUP_STATUS_CLR_V  0x00000001U
#define LP_AON_EXT_WAKEUP_STATUS_CLR_S  30
/** LP_AON_EXT_WAKEUP_FILTER : R/W; bitpos: [31]; default: 0;
 *  enable external filter or not
 *  1: enable
 *  0: disable
 */
#define LP_AON_EXT_WAKEUP_FILTER    (BIT(31))
#define LP_AON_EXT_WAKEUP_FILTER_M  (LP_AON_EXT_WAKEUP_FILTER_V << LP_AON_EXT_WAKEUP_FILTER_S)
#define LP_AON_EXT_WAKEUP_FILTER_V  0x00000001U
#define LP_AON_EXT_WAKEUP_FILTER_S  31

/** LP_AON_USB_REG register
 *  configure usb reset bypass
 */
#define LP_AON_USB_REG (DR_REG_LP_AON_BASE + 0x48)
/** LP_AON_USB_RESET_DISABLE : R/W; bitpos: [31]; default: 0;
 *  bypass usb reset from hp system reset event
 *  1: bypass
 *  0: no operation
 */
#define LP_AON_USB_RESET_DISABLE    (BIT(31))
#define LP_AON_USB_RESET_DISABLE_M  (LP_AON_USB_RESET_DISABLE_V << LP_AON_USB_RESET_DISABLE_S)
#define LP_AON_USB_RESET_DISABLE_V  0x00000001U
#define LP_AON_USB_RESET_DISABLE_S  31

/** LP_AON_LPBUS_REG register
 *  Select lp memory bus
 */
#define LP_AON_LPBUS_REG (DR_REG_LP_AON_BASE + 0x4c)
/** LP_AON_FAST_MEM_MUX_FSM_IDLE : RO; bitpos: [28]; default: 1;
 *  get current lp memory bus fsm status
 */
#define LP_AON_FAST_MEM_MUX_FSM_IDLE    (BIT(28))
#define LP_AON_FAST_MEM_MUX_FSM_IDLE_M  (LP_AON_FAST_MEM_MUX_FSM_IDLE_V << LP_AON_FAST_MEM_MUX_FSM_IDLE_S)
#define LP_AON_FAST_MEM_MUX_FSM_IDLE_V  0x00000001U
#define LP_AON_FAST_MEM_MUX_FSM_IDLE_S  28
/** LP_AON_FAST_MEM_MUX_SEL_STATUS : RO; bitpos: [29]; default: 1;
 *  get current lp memory bus mode
 */
#define LP_AON_FAST_MEM_MUX_SEL_STATUS    (BIT(29))
#define LP_AON_FAST_MEM_MUX_SEL_STATUS_M  (LP_AON_FAST_MEM_MUX_SEL_STATUS_V << LP_AON_FAST_MEM_MUX_SEL_STATUS_S)
#define LP_AON_FAST_MEM_MUX_SEL_STATUS_V  0x00000001U
#define LP_AON_FAST_MEM_MUX_SEL_STATUS_S  29
/** LP_AON_FAST_MEM_MUX_SEL_UPDATE : WT; bitpos: [30]; default: 0;
 *  enable reg_fast_mem_sel configure
 *  1: enable
 *  0: no operation
 */
#define LP_AON_FAST_MEM_MUX_SEL_UPDATE    (BIT(30))
#define LP_AON_FAST_MEM_MUX_SEL_UPDATE_M  (LP_AON_FAST_MEM_MUX_SEL_UPDATE_V << LP_AON_FAST_MEM_MUX_SEL_UPDATE_S)
#define LP_AON_FAST_MEM_MUX_SEL_UPDATE_V  0x00000001U
#define LP_AON_FAST_MEM_MUX_SEL_UPDATE_S  30
/** LP_AON_FAST_MEM_MUX_SEL : R/W; bitpos: [31]; default: 1;
 *  select lp memory bus is high speed mode or low speed mode
 *  1: high speed from hp system ahb
 *  0: low speed from lp system
 */
#define LP_AON_FAST_MEM_MUX_SEL    (BIT(31))
#define LP_AON_FAST_MEM_MUX_SEL_M  (LP_AON_FAST_MEM_MUX_SEL_V << LP_AON_FAST_MEM_MUX_SEL_S)
#define LP_AON_FAST_MEM_MUX_SEL_V  0x00000001U
#define LP_AON_FAST_MEM_MUX_SEL_S  31

/** LP_AON_BACKUP_DMA_CFG0_REG register
 *  configure regdma always on register
 */
#define LP_AON_BACKUP_DMA_CFG0_REG (DR_REG_LP_AON_BASE + 0x70)
/** LP_AON_BURST_LIMIT_AON : R/W; bitpos: [4:0]; default: 10;
 *  Set this field to configure max value of burst in single transfer.
 */
#define LP_AON_BURST_LIMIT_AON    0x0000001FU
#define LP_AON_BURST_LIMIT_AON_M  (LP_AON_BURST_LIMIT_AON_V << LP_AON_BURST_LIMIT_AON_S)
#define LP_AON_BURST_LIMIT_AON_V  0x0000001FU
#define LP_AON_BURST_LIMIT_AON_S  0
/** LP_AON_READ_INTERVAL_AON : R/W; bitpos: [11:5]; default: 10;
 *  Set this field to configure read registers' interval time in reading mode.
 */
#define LP_AON_READ_INTERVAL_AON    0x0000007FU
#define LP_AON_READ_INTERVAL_AON_M  (LP_AON_READ_INTERVAL_AON_V << LP_AON_READ_INTERVAL_AON_S)
#define LP_AON_READ_INTERVAL_AON_V  0x0000007FU
#define LP_AON_READ_INTERVAL_AON_S  5
/** LP_AON_BRANCH_LINK_LENGTH_AON : R/W; bitpos: [15:12]; default: 0;
 *  Set this field to configure link address.
 */
#define LP_AON_BRANCH_LINK_LENGTH_AON    0x0000000FU
#define LP_AON_BRANCH_LINK_LENGTH_AON_M  (LP_AON_BRANCH_LINK_LENGTH_AON_V << LP_AON_BRANCH_LINK_LENGTH_AON_S)
#define LP_AON_BRANCH_LINK_LENGTH_AON_V  0x0000000FU
#define LP_AON_BRANCH_LINK_LENGTH_AON_S  12
/** LP_AON_REGDMA_ERROR : RO; bitpos: [18:16]; default: 0;
 *  regdma error code
 */
#define LP_AON_REGDMA_ERROR    0x00000007U
#define LP_AON_REGDMA_ERROR_M  (LP_AON_REGDMA_ERROR_V << LP_AON_REGDMA_ERROR_S)
#define LP_AON_REGDMA_ERROR_V  0x00000007U
#define LP_AON_REGDMA_ERROR_S  16

/** LP_AON_BACKUP_DMA_CFG1_REG register
 *  configure regdma always on register
 */
#define LP_AON_BACKUP_DMA_CFG1_REG (DR_REG_LP_AON_BASE + 0x74)
/** LP_AON_LINK_WAIT_TOUT_THRES_AON : R/W; bitpos: [9:0]; default: 100;
 *  Set this field to configure the number of consecutive links of link list.
 */
#define LP_AON_LINK_WAIT_TOUT_THRES_AON    0x000003FFU
#define LP_AON_LINK_WAIT_TOUT_THRES_AON_M  (LP_AON_LINK_WAIT_TOUT_THRES_AON_V << LP_AON_LINK_WAIT_TOUT_THRES_AON_S)
#define LP_AON_LINK_WAIT_TOUT_THRES_AON_V  0x000003FFU
#define LP_AON_LINK_WAIT_TOUT_THRES_AON_S  0
/** LP_AON_LINK_WORK_TOUT_THRES_AON : R/W; bitpos: [19:10]; default: 100;
 *  Set this field to configure maximum waiting time in waiting mode.
 */
#define LP_AON_LINK_WORK_TOUT_THRES_AON    0x000003FFU
#define LP_AON_LINK_WORK_TOUT_THRES_AON_M  (LP_AON_LINK_WORK_TOUT_THRES_AON_V << LP_AON_LINK_WORK_TOUT_THRES_AON_S)
#define LP_AON_LINK_WORK_TOUT_THRES_AON_V  0x000003FFU
#define LP_AON_LINK_WORK_TOUT_THRES_AON_S  10
/** LP_AON_LINK_BACKUP_TOUT_THRES_AON : R/W; bitpos: [29:20]; default: 100;
 *  Set this field to configure maximum waiting time in backup mode.
 */
#define LP_AON_LINK_BACKUP_TOUT_THRES_AON    0x000003FFU
#define LP_AON_LINK_BACKUP_TOUT_THRES_AON_M  (LP_AON_LINK_BACKUP_TOUT_THRES_AON_V << LP_AON_LINK_BACKUP_TOUT_THRES_AON_S)
#define LP_AON_LINK_BACKUP_TOUT_THRES_AON_V  0x000003FFU
#define LP_AON_LINK_BACKUP_TOUT_THRES_AON_S  20

/** LP_AON_BACKUP_DMA_CFG2_REG register
 *  configure regdma always on register
 */
#define LP_AON_BACKUP_DMA_CFG2_REG (DR_REG_LP_AON_BASE + 0x78)
/** LP_AON_LINK_ADDR_AON : R/W; bitpos: [31:0]; default: 0;
 *  Set this field to configure link address.
 */
#define LP_AON_LINK_ADDR_AON    0xFFFFFFFFU
#define LP_AON_LINK_ADDR_AON_M  (LP_AON_LINK_ADDR_AON_V << LP_AON_LINK_ADDR_AON_S)
#define LP_AON_LINK_ADDR_AON_V  0xFFFFFFFFU
#define LP_AON_LINK_ADDR_AON_S  0

/** LP_AON_MEM_CTRL_REG register
 *  configure rmemory power in lp system register
 */
#define LP_AON_MEM_CTRL_REG (DR_REG_LP_AON_BASE + 0x7c)
/** LP_AON_EFUSE_MEM_LP_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures efuse memory low power mode in low power stage.
 *  0(default): deep sleep
 *  1: light sleep
 *  2: shut down
 *  3: disable low power stage
 */
#define LP_AON_EFUSE_MEM_LP_MODE    0x00000003U
#define LP_AON_EFUSE_MEM_LP_MODE_M  (LP_AON_EFUSE_MEM_LP_MODE_V << LP_AON_EFUSE_MEM_LP_MODE_S)
#define LP_AON_EFUSE_MEM_LP_MODE_V  0x00000003U
#define LP_AON_EFUSE_MEM_LP_MODE_S  0
/** LP_AON_EFUSE_MEM_LP_EN : R/W; bitpos: [2]; default: 0;
 *  Set this bit to power down efuse memory.
 */
#define LP_AON_EFUSE_MEM_LP_EN    (BIT(2))
#define LP_AON_EFUSE_MEM_LP_EN_M  (LP_AON_EFUSE_MEM_LP_EN_V << LP_AON_EFUSE_MEM_LP_EN_S)
#define LP_AON_EFUSE_MEM_LP_EN_V  0x00000001U
#define LP_AON_EFUSE_MEM_LP_EN_S  2
/** LP_AON_EFUSE_MEM_FORCE_CTRL : R/W; bitpos: [3]; default: 0;
 *  Set this bit to force software control efuse memory, disable hardware control.
 */
#define LP_AON_EFUSE_MEM_FORCE_CTRL    (BIT(3))
#define LP_AON_EFUSE_MEM_FORCE_CTRL_M  (LP_AON_EFUSE_MEM_FORCE_CTRL_V << LP_AON_EFUSE_MEM_FORCE_CTRL_S)
#define LP_AON_EFUSE_MEM_FORCE_CTRL_V  0x00000001U
#define LP_AON_EFUSE_MEM_FORCE_CTRL_S  3
/** LP_AON_HUK_MEM_LP_MODE : R/W; bitpos: [5:4]; default: 2;
 *  Configures huk memory low power mode in low power stage.
 *  0: deep sleep
 *  1: light sleep
 *  2(default): shut down
 *  3: disable low power stage
 */
#define LP_AON_HUK_MEM_LP_MODE    0x00000003U
#define LP_AON_HUK_MEM_LP_MODE_M  (LP_AON_HUK_MEM_LP_MODE_V << LP_AON_HUK_MEM_LP_MODE_S)
#define LP_AON_HUK_MEM_LP_MODE_V  0x00000003U
#define LP_AON_HUK_MEM_LP_MODE_S  4
/** LP_AON_HUK_MEM_LP_EN : R/W; bitpos: [6]; default: 0;
 *  Set this bit to power down huk memory.
 */
#define LP_AON_HUK_MEM_LP_EN    (BIT(6))
#define LP_AON_HUK_MEM_LP_EN_M  (LP_AON_HUK_MEM_LP_EN_V << LP_AON_HUK_MEM_LP_EN_S)
#define LP_AON_HUK_MEM_LP_EN_V  0x00000001U
#define LP_AON_HUK_MEM_LP_EN_S  6
/** LP_AON_HUK_MEM_FORCE_CTRL : R/W; bitpos: [7]; default: 0;
 *  Set this bit to force software control huk memory, disable hardware control.
 */
#define LP_AON_HUK_MEM_FORCE_CTRL    (BIT(7))
#define LP_AON_HUK_MEM_FORCE_CTRL_M  (LP_AON_HUK_MEM_FORCE_CTRL_V << LP_AON_HUK_MEM_FORCE_CTRL_S)
#define LP_AON_HUK_MEM_FORCE_CTRL_V  0x00000001U
#define LP_AON_HUK_MEM_FORCE_CTRL_S  7

/** LP_AON_HP_MEM_CTRL_REG register
 *  configure rmemory power in lp system register
 */
#define LP_AON_HP_MEM_CTRL_REG (DR_REG_LP_AON_BASE + 0x80)
/** LP_AON_MODEM_MEM_LP_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures modem memory low power mode in low power stage.
 *  0(default): deep sleep
 *  1: light sleep
 *  2: shut down
 *  3: disable low power stage
 */
#define LP_AON_MODEM_MEM_LP_MODE    0x00000003U
#define LP_AON_MODEM_MEM_LP_MODE_M  (LP_AON_MODEM_MEM_LP_MODE_V << LP_AON_MODEM_MEM_LP_MODE_S)
#define LP_AON_MODEM_MEM_LP_MODE_V  0x00000003U
#define LP_AON_MODEM_MEM_LP_MODE_S  0
/** LP_AON_MODEM_MEM_LP_EN : R/W; bitpos: [2]; default: 0;
 *  Set this bit to power down modem memory.
 */
#define LP_AON_MODEM_MEM_LP_EN    (BIT(2))
#define LP_AON_MODEM_MEM_LP_EN_M  (LP_AON_MODEM_MEM_LP_EN_V << LP_AON_MODEM_MEM_LP_EN_S)
#define LP_AON_MODEM_MEM_LP_EN_V  0x00000001U
#define LP_AON_MODEM_MEM_LP_EN_S  2
/** LP_AON_MODEM_MEM_FORCE_CTRL : R/W; bitpos: [3]; default: 0;
 *  Set this bit to force software control modem memory, disable hardware control.
 */
#define LP_AON_MODEM_MEM_FORCE_CTRL    (BIT(3))
#define LP_AON_MODEM_MEM_FORCE_CTRL_M  (LP_AON_MODEM_MEM_FORCE_CTRL_V << LP_AON_MODEM_MEM_FORCE_CTRL_S)
#define LP_AON_MODEM_MEM_FORCE_CTRL_V  0x00000001U
#define LP_AON_MODEM_MEM_FORCE_CTRL_S  3
/** LP_AON_MMU_MEM_LP_MODE : R/W; bitpos: [5:4]; default: 0;
 *  Configures mmu memory low power mode in low power stage.
 *  0: deep sleep
 *  1: light sleep
 *  2(default): shut down
 *  3: disable low power stage
 */
#define LP_AON_MMU_MEM_LP_MODE    0x00000003U
#define LP_AON_MMU_MEM_LP_MODE_M  (LP_AON_MMU_MEM_LP_MODE_V << LP_AON_MMU_MEM_LP_MODE_S)
#define LP_AON_MMU_MEM_LP_MODE_V  0x00000003U
#define LP_AON_MMU_MEM_LP_MODE_S  4
/** LP_AON_MMU_MEM_LP_EN : R/W; bitpos: [6]; default: 0;
 *  Set this bit to power down mmu memory.
 */
#define LP_AON_MMU_MEM_LP_EN    (BIT(6))
#define LP_AON_MMU_MEM_LP_EN_M  (LP_AON_MMU_MEM_LP_EN_V << LP_AON_MMU_MEM_LP_EN_S)
#define LP_AON_MMU_MEM_LP_EN_V  0x00000001U
#define LP_AON_MMU_MEM_LP_EN_S  6
/** LP_AON_MMU_MEM_FORCE_CTRL : R/W; bitpos: [7]; default: 0;
 *  Set this bit to force software control mmu memory, disable hardware control.
 */
#define LP_AON_MMU_MEM_FORCE_CTRL    (BIT(7))
#define LP_AON_MMU_MEM_FORCE_CTRL_M  (LP_AON_MMU_MEM_FORCE_CTRL_V << LP_AON_MMU_MEM_FORCE_CTRL_S)
#define LP_AON_MMU_MEM_FORCE_CTRL_V  0x00000001U
#define LP_AON_MMU_MEM_FORCE_CTRL_S  7
/** LP_AON_HP_SRAM_MEM_LP_MODE : R/W; bitpos: [17:16]; default: 0;
 *  Configures hp_sram memory low power mode in low power stage.
 *  0(default): deep sleep
 *  1: light sleep
 *  2: shut down
 *  3: disable low power stage
 */
#define LP_AON_HP_SRAM_MEM_LP_MODE    0x00000003U
#define LP_AON_HP_SRAM_MEM_LP_MODE_M  (LP_AON_HP_SRAM_MEM_LP_MODE_V << LP_AON_HP_SRAM_MEM_LP_MODE_S)
#define LP_AON_HP_SRAM_MEM_LP_MODE_V  0x00000003U
#define LP_AON_HP_SRAM_MEM_LP_MODE_S  16
/** LP_AON_HP_SRAM_MEM_LP_EN : R/W; bitpos: [24:18]; default: 0;
 *  Set this bit to power down hp_sram memory.
 */
#define LP_AON_HP_SRAM_MEM_LP_EN    0x0000007FU
#define LP_AON_HP_SRAM_MEM_LP_EN_M  (LP_AON_HP_SRAM_MEM_LP_EN_V << LP_AON_HP_SRAM_MEM_LP_EN_S)
#define LP_AON_HP_SRAM_MEM_LP_EN_V  0x0000007FU
#define LP_AON_HP_SRAM_MEM_LP_EN_S  18
/** LP_AON_HP_SRAM_MEM_FORCE_CTRL : R/W; bitpos: [31:25]; default: 0;
 *  Set this bit to force software control hp_sram memory, disable hardware control.
 */
#define LP_AON_HP_SRAM_MEM_FORCE_CTRL    0x0000007FU
#define LP_AON_HP_SRAM_MEM_FORCE_CTRL_M  (LP_AON_HP_SRAM_MEM_FORCE_CTRL_V << LP_AON_HP_SRAM_MEM_FORCE_CTRL_S)
#define LP_AON_HP_SRAM_MEM_FORCE_CTRL_V  0x0000007FU
#define LP_AON_HP_SRAM_MEM_FORCE_CTRL_S  25

/** LP_AON_IO_LDO_CFG_REG register
 *  need_des
 */
#define LP_AON_IO_LDO_CFG_REG (DR_REG_LP_AON_BASE + 0x84)
/** LP_AON_IO_LDO_3P3_SW : R/W; bitpos: [21]; default: 0;
 *  need_des
 */
#define LP_AON_IO_LDO_3P3_SW    (BIT(21))
#define LP_AON_IO_LDO_3P3_SW_M  (LP_AON_IO_LDO_3P3_SW_V << LP_AON_IO_LDO_3P3_SW_S)
#define LP_AON_IO_LDO_3P3_SW_V  0x00000001U
#define LP_AON_IO_LDO_3P3_SW_S  21
/** LP_AON_IO_LDO_3P3_SW_EN : R/W; bitpos: [22]; default: 0;
 *  need_des
 */
#define LP_AON_IO_LDO_3P3_SW_EN    (BIT(22))
#define LP_AON_IO_LDO_3P3_SW_EN_M  (LP_AON_IO_LDO_3P3_SW_EN_V << LP_AON_IO_LDO_3P3_SW_EN_S)
#define LP_AON_IO_LDO_3P3_SW_EN_V  0x00000001U
#define LP_AON_IO_LDO_3P3_SW_EN_S  22
/** LP_AON_IO_LDO_ADJUST_SW : R/W; bitpos: [30:23]; default: 0;
 *  need_des
 */
#define LP_AON_IO_LDO_ADJUST_SW    0x000000FFU
#define LP_AON_IO_LDO_ADJUST_SW_M  (LP_AON_IO_LDO_ADJUST_SW_V << LP_AON_IO_LDO_ADJUST_SW_S)
#define LP_AON_IO_LDO_ADJUST_SW_V  0x000000FFU
#define LP_AON_IO_LDO_ADJUST_SW_S  23
/** LP_AON_IO_LDO_ADJUST_SW_EN : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_AON_IO_LDO_ADJUST_SW_EN    (BIT(31))
#define LP_AON_IO_LDO_ADJUST_SW_EN_M  (LP_AON_IO_LDO_ADJUST_SW_EN_V << LP_AON_IO_LDO_ADJUST_SW_EN_S)
#define LP_AON_IO_LDO_ADJUST_SW_EN_V  0x00000001U
#define LP_AON_IO_LDO_ADJUST_SW_EN_S  31

/** LP_AON_SRAM_USAGE_CONF_REG register
 *  HP memory usage configuration register
 */
#define LP_AON_SRAM_USAGE_CONF_REG (DR_REG_LP_AON_BASE + 0x98)
/** LP_AON_DCACHE_USAGE : R/W; bitpos: [0]; default: 0;
 *  hp system memory is split to 7 layers(Layer0 ~ Layer6) in total, this field is
 *  used to control the first layer(Layer0) usage. 0: cpu use hp-memory. 1: dcache use
 *  hp-mmory.
 *  By default, dcache is closed, and typically users can enable dcache after
 *  boot-loader, but before user's BIN start running.
 */
#define LP_AON_DCACHE_USAGE    (BIT(0))
#define LP_AON_DCACHE_USAGE_M  (LP_AON_DCACHE_USAGE_V << LP_AON_DCACHE_USAGE_S)
#define LP_AON_DCACHE_USAGE_V  0x00000001U
#define LP_AON_DCACHE_USAGE_S  0
/** LP_AON_ICACHE1_USAGE : R/W; bitpos: [1]; default: 1;
 *  hp system memory is split to 7 layers(Layer0 ~ Layer6) in total, this field is
 *  used to control the last layer(Layer6) usage. 0: cpu use hp-memory. 1: icache1 use
 *  hp-mmory.
 *  By default, icache1 is not disabled, and the last layer memory belongs to icache1.
 *  Typically users can set this bit to 0 to disable icache1 in boot-loader.
 */
#define LP_AON_ICACHE1_USAGE    (BIT(1))
#define LP_AON_ICACHE1_USAGE_M  (LP_AON_ICACHE1_USAGE_V << LP_AON_ICACHE1_USAGE_S)
#define LP_AON_ICACHE1_USAGE_V  0x00000001U
#define LP_AON_ICACHE1_USAGE_S  1

/** LP_AON_PUF_CONF_REG register
 *  PUF mem control config register
 */
#define LP_AON_PUF_CONF_REG (DR_REG_LP_AON_BASE + 0x19c)
/** LP_AON_PUF_SW : R/W; bitpos: [0]; default: 1;
 *  puf mem power switch control signal
 */
#define LP_AON_PUF_SW    (BIT(0))
#define LP_AON_PUF_SW_M  (LP_AON_PUF_SW_V << LP_AON_PUF_SW_S)
#define LP_AON_PUF_SW_V  0x00000001U
#define LP_AON_PUF_SW_S  0
/** LP_AON_PUF_ISO_EN : R/W; bitpos: [1]; default: 0;
 *  iso enable signal for puf mem
 */
#define LP_AON_PUF_ISO_EN    (BIT(1))
#define LP_AON_PUF_ISO_EN_M  (LP_AON_PUF_ISO_EN_V << LP_AON_PUF_ISO_EN_S)
#define LP_AON_PUF_ISO_EN_V  0x00000001U
#define LP_AON_PUF_ISO_EN_S  1
/** LP_AON_PUF_MEM_DISCHARGE : R/W; bitpos: [2]; default: 0;
 *  discharge signal for puf mem
 */
#define LP_AON_PUF_MEM_DISCHARGE    (BIT(2))
#define LP_AON_PUF_MEM_DISCHARGE_M  (LP_AON_PUF_MEM_DISCHARGE_V << LP_AON_PUF_MEM_DISCHARGE_S)
#define LP_AON_PUF_MEM_DISCHARGE_V  0x00000001U
#define LP_AON_PUF_MEM_DISCHARGE_S  2

/** LP_AON_DATE_REG register
 *  reserved
 */
#define LP_AON_DATE_REG (DR_REG_LP_AON_BASE + 0x3fc)
/** LP_AON_CLK_EN : R/W; bitpos: [5]; default: 0;
 *  0: DREG_LDO control by SW;
 *  1: DREG_LDO control by HW;
 */
#define LP_AON_DREG_LDO_HW  BIT(5)
#define LP_AON_DREG_LDO_HW_M  (LP_AON_DREG_LDO_HW_V << LP_AON_DREG_LDO_HW_S)
#define LP_AON_DREG_LDO_HW_V  0x00000001U
#define LP_AON_DREG_LDO_HW_S  31

/** LP_AON_IO_LDO_ADJUST_SW : R/W; bitpos: [30:23]; default: 0;
 *  need_des
 */
#define LP_AON_DREG_LDO_SW    0x0000000FU
#define LP_AON_DREG_LDO_SW_M  (LP_AON_DREG_LDO_SW_V << LP_AON_DREG_LDO_SW_S)
#define LP_AON_DREG_LDO_SW_V  0x0000000FU
#define LP_AON_DREG_LDO_SW_S  11

/** LP_AON_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  version register
 */
#define LP_AON_CLK_EN    (BIT(31))
#define LP_AON_CLK_EN_M  (LP_AON_CLK_EN_V << LP_AON_CLK_EN_S)
#define LP_AON_CLK_EN_V  0x00000001U
#define LP_AON_CLK_EN_S  31

#ifdef __cplusplus
}
#endif
