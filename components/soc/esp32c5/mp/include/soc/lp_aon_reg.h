/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
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
/** LP_AON_GPIO_MUX_SEL : R/W; bitpos: [7:0]; default: 0;
 *  select the lp io 0~7 controlled by hp iomux or lp iomux
 *  1: controlled by lp iomux
 *  0: controlled by hp iomux
 */
#define LP_AON_GPIO_MUX_SEL    0x000000FFU
#define LP_AON_GPIO_MUX_SEL_M  (LP_AON_GPIO_MUX_SEL_V << LP_AON_GPIO_MUX_SEL_S)
#define LP_AON_GPIO_MUX_SEL_V  0x000000FFU
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
/** LP_AON_FORCE_DOWNLOAD_BOOT_STATUS : RO; bitpos: [29]; default: 0;
 *  get force download mode status
 */
#define LP_AON_FORCE_DOWNLOAD_BOOT_STATUS    (BIT(29))
#define LP_AON_FORCE_DOWNLOAD_BOOT_STATUS_M  (LP_AON_FORCE_DOWNLOAD_BOOT_STATUS_V << LP_AON_FORCE_DOWNLOAD_BOOT_STATUS_S)
#define LP_AON_FORCE_DOWNLOAD_BOOT_STATUS_V  0x00000001U
#define LP_AON_FORCE_DOWNLOAD_BOOT_STATUS_S  29
/** LP_AON_FORCE_DOWNLOAD_BOOT : R/W; bitpos: [30]; default: 0;
 *  enable chip entry download mode or not
 *  1: enable
 *  0: no operation
 */
#define LP_AON_FORCE_DOWNLOAD_BOOT    (BIT(30))
#define LP_AON_FORCE_DOWNLOAD_BOOT_M  (LP_AON_FORCE_DOWNLOAD_BOOT_V << LP_AON_FORCE_DOWNLOAD_BOOT_S)
#define LP_AON_FORCE_DOWNLOAD_BOOT_V  0x00000001U
#define LP_AON_FORCE_DOWNLOAD_BOOT_S  30
/** LP_AON_HPSYS_SW_RESET : WT; bitpos: [31]; default: 0;
 *  enable hp system reset by software or not
 *  1: reset
 *  0: no operation
 */
#define LP_AON_HPSYS_SW_RESET    (BIT(31))
#define LP_AON_HPSYS_SW_RESET_M  (LP_AON_HPSYS_SW_RESET_V << LP_AON_HPSYS_SW_RESET_S)
#define LP_AON_HPSYS_SW_RESET_V  0x00000001U
#define LP_AON_HPSYS_SW_RESET_S  31

/** LP_AON_CPUCORE0_CFG_REG register
 *  configure core reset register
 */
#define LP_AON_CPUCORE0_CFG_REG (DR_REG_LP_AON_BASE + 0x38)
/** LP_AON_CPU_CORE0_SW_STALL : R/W; bitpos: [7:0]; default: 0;
 *  enable cpu entry stall status
 *  0x86: entry stall status
 *  Others : no operation
 */
#define LP_AON_CPU_CORE0_SW_STALL    0x000000FFU
#define LP_AON_CPU_CORE0_SW_STALL_M  (LP_AON_CPU_CORE0_SW_STALL_V << LP_AON_CPU_CORE0_SW_STALL_S)
#define LP_AON_CPU_CORE0_SW_STALL_V  0x000000FFU
#define LP_AON_CPU_CORE0_SW_STALL_S  0
/** LP_AON_CPU_CORE0_SW_RESET : WT; bitpos: [28]; default: 0;
 *  enable core reset by software
 *  1: reset
 *  0: no operation
 */
#define LP_AON_CPU_CORE0_SW_RESET    (BIT(28))
#define LP_AON_CPU_CORE0_SW_RESET_M  (LP_AON_CPU_CORE0_SW_RESET_V << LP_AON_CPU_CORE0_SW_RESET_S)
#define LP_AON_CPU_CORE0_SW_RESET_V  0x00000001U
#define LP_AON_CPU_CORE0_SW_RESET_S  28
/** LP_AON_CPU_CORE0_OCD_HALT_ON_RESET : R/W; bitpos: [29]; default: 0;
 *  reserved
 */
#define LP_AON_CPU_CORE0_OCD_HALT_ON_RESET    (BIT(29))
#define LP_AON_CPU_CORE0_OCD_HALT_ON_RESET_M  (LP_AON_CPU_CORE0_OCD_HALT_ON_RESET_V << LP_AON_CPU_CORE0_OCD_HALT_ON_RESET_S)
#define LP_AON_CPU_CORE0_OCD_HALT_ON_RESET_V  0x00000001U
#define LP_AON_CPU_CORE0_OCD_HALT_ON_RESET_S  29
/** LP_AON_CPU_CORE0_STAT_VECTOR_SEL : R/W; bitpos: [30]; default: 1;
 *  configure core boot address
 *  1: ROM
 *  0: lp memory
 */
#define LP_AON_CPU_CORE0_STAT_VECTOR_SEL    (BIT(30))
#define LP_AON_CPU_CORE0_STAT_VECTOR_SEL_M  (LP_AON_CPU_CORE0_STAT_VECTOR_SEL_V << LP_AON_CPU_CORE0_STAT_VECTOR_SEL_S)
#define LP_AON_CPU_CORE0_STAT_VECTOR_SEL_V  0x00000001U
#define LP_AON_CPU_CORE0_STAT_VECTOR_SEL_S  30
/** LP_AON_CPU_CORE0_DRESET_MASK : R/W; bitpos: [31]; default: 0;
 *  disable bypass core dreset
 *  1: enable bypass
 *  0: disable bypass
 */
#define LP_AON_CPU_CORE0_DRESET_MASK    (BIT(31))
#define LP_AON_CPU_CORE0_DRESET_MASK_M  (LP_AON_CPU_CORE0_DRESET_MASK_V << LP_AON_CPU_CORE0_DRESET_MASK_S)
#define LP_AON_CPU_CORE0_DRESET_MASK_V  0x00000001U
#define LP_AON_CPU_CORE0_DRESET_MASK_S  31

/** LP_AON_IO_MUX_REG register
 *  configure hp iomux reset bypass
 */
#define LP_AON_IO_MUX_REG (DR_REG_LP_AON_BASE + 0x3c)
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
/** LP_AON_EXT_WAKEUP_STATUS : RO; bitpos: [7:0]; default: 0;
 *  get external wakeup status bitmap
 */
#define LP_AON_EXT_WAKEUP_STATUS    0x000000FFU
#define LP_AON_EXT_WAKEUP_STATUS_M  (LP_AON_EXT_WAKEUP_STATUS_V << LP_AON_EXT_WAKEUP_STATUS_S)
#define LP_AON_EXT_WAKEUP_STATUS_V  0x000000FFU
#define LP_AON_EXT_WAKEUP_STATUS_S  0
/** LP_AON_EXT_WAKEUP_STATUS_CLR : WT; bitpos: [14]; default: 0;
 *  clear external wakeup status
 *  1: clear
 *  0: no operation
 */
#define LP_AON_EXT_WAKEUP_STATUS_CLR    (BIT(14))
#define LP_AON_EXT_WAKEUP_STATUS_CLR_M  (LP_AON_EXT_WAKEUP_STATUS_CLR_V << LP_AON_EXT_WAKEUP_STATUS_CLR_S)
#define LP_AON_EXT_WAKEUP_STATUS_CLR_V  0x00000001U
#define LP_AON_EXT_WAKEUP_STATUS_CLR_S  14
/** LP_AON_EXT_WAKEUP_SEL : R/W; bitpos: [22:15]; default: 0;
 *  enable io0~7 bit map use to external wakeup
 *  1: enable
 *  0: disable
 */
#define LP_AON_EXT_WAKEUP_SEL    0x000000FFU
#define LP_AON_EXT_WAKEUP_SEL_M  (LP_AON_EXT_WAKEUP_SEL_V << LP_AON_EXT_WAKEUP_SEL_S)
#define LP_AON_EXT_WAKEUP_SEL_V  0x000000FFU
#define LP_AON_EXT_WAKEUP_SEL_S  15
/** LP_AON_EXT_WAKEUP_LV : R/W; bitpos: [30:23]; default: 0;
 *  select external wakeup io level
 *  1: io high level wakeup
 *  0: io low level wakeup
 */
#define LP_AON_EXT_WAKEUP_LV    0x000000FFU
#define LP_AON_EXT_WAKEUP_LV_M  (LP_AON_EXT_WAKEUP_LV_V << LP_AON_EXT_WAKEUP_LV_S)
#define LP_AON_EXT_WAKEUP_LV_V  0x000000FFU
#define LP_AON_EXT_WAKEUP_LV_S  23
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
#define LP_AON_USB_REG (DR_REG_LP_AON_BASE + 0x44)
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
#define LP_AON_LPBUS_REG (DR_REG_LP_AON_BASE + 0x48)
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

/** LP_AON_SDIO_ACTIVE_REG register
 *  configure sdio act dnum
 */
#define LP_AON_SDIO_ACTIVE_REG (DR_REG_LP_AON_BASE + 0x4c)
/** LP_AON_SDIO_ACT_DNUM : R/W; bitpos: [31:22]; default: 10;
 *  reserved
 */
#define LP_AON_SDIO_ACT_DNUM    0x000003FFU
#define LP_AON_SDIO_ACT_DNUM_M  (LP_AON_SDIO_ACT_DNUM_V << LP_AON_SDIO_ACT_DNUM_S)
#define LP_AON_SDIO_ACT_DNUM_V  0x000003FFU
#define LP_AON_SDIO_ACT_DNUM_S  22

/** LP_AON_LPCORE_REG register
 *  configure etm wakeup register
 */
#define LP_AON_LPCORE_REG (DR_REG_LP_AON_BASE + 0x50)
/** LP_AON_LPCORE_ETM_WAKEUP_FLAG_CLR : WT; bitpos: [0]; default: 0;
 *  clear etm wakeup latch
 */
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG_CLR    (BIT(0))
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG_CLR_M  (LP_AON_LPCORE_ETM_WAKEUP_FLAG_CLR_V << LP_AON_LPCORE_ETM_WAKEUP_FLAG_CLR_S)
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG_CLR_V  0x00000001U
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG_CLR_S  0
/** LP_AON_LPCORE_ETM_WAKEUP_FLAG : R/WTC/SS; bitpos: [1]; default: 0;
 *  latch etmwakeup event
 */
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG    (BIT(1))
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG_M  (LP_AON_LPCORE_ETM_WAKEUP_FLAG_V << LP_AON_LPCORE_ETM_WAKEUP_FLAG_S)
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG_V  0x00000001U
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG_S  1
/** LP_AON_LPCORE_DISABLE : R/W; bitpos: [31]; default: 0;
 *  disable lp core
 *  1:disable
 *  0:no operation
 */
#define LP_AON_LPCORE_DISABLE    (BIT(31))
#define LP_AON_LPCORE_DISABLE_M  (LP_AON_LPCORE_DISABLE_V << LP_AON_LPCORE_DISABLE_S)
#define LP_AON_LPCORE_DISABLE_V  0x00000001U
#define LP_AON_LPCORE_DISABLE_S  31

/** LP_AON_SAR_CCT_REG register
 *  configure sar cct
 */
#define LP_AON_SAR_CCT_REG (DR_REG_LP_AON_BASE + 0x54)
/** LP_AON_SAR2_PWDET_CCT : R/W; bitpos: [31:29]; default: 0;
 *  configure sar cct
 */
#define LP_AON_SAR2_PWDET_CCT    0x00000007U
#define LP_AON_SAR2_PWDET_CCT_M  (LP_AON_SAR2_PWDET_CCT_V << LP_AON_SAR2_PWDET_CCT_S)
#define LP_AON_SAR2_PWDET_CCT_V  0x00000007U
#define LP_AON_SAR2_PWDET_CCT_S  29

/** LP_AON_MODEM_BUS_REG register
 *  configure modem sync bridge
 */
#define LP_AON_MODEM_BUS_REG (DR_REG_LP_AON_BASE + 0x58)
/** LP_AON_MODEM_SYNC_BRIDGE_EN : R/W; bitpos: [31]; default: 0;
 *  enable modem sync bridge or not
 *  1: enable
 *  0: disable
 */
#define LP_AON_MODEM_SYNC_BRIDGE_EN    (BIT(31))
#define LP_AON_MODEM_SYNC_BRIDGE_EN_M  (LP_AON_MODEM_SYNC_BRIDGE_EN_V << LP_AON_MODEM_SYNC_BRIDGE_EN_S)
#define LP_AON_MODEM_SYNC_BRIDGE_EN_V  0x00000001U
#define LP_AON_MODEM_SYNC_BRIDGE_EN_S  31

/** LP_AON_SPRAM_CTRL_REG register
 *  configure lp memory power status
 */
#define LP_AON_SPRAM_CTRL_REG (DR_REG_LP_AON_BASE + 0x60)
/** LP_AON_SPRAM_MEM_AUX_CTRL : R/W; bitpos: [31:0]; default: 8304;
 *  configure lp memory power status
 */
#define LP_AON_SPRAM_MEM_AUX_CTRL    0xFFFFFFFFU
#define LP_AON_SPRAM_MEM_AUX_CTRL_M  (LP_AON_SPRAM_MEM_AUX_CTRL_V << LP_AON_SPRAM_MEM_AUX_CTRL_S)
#define LP_AON_SPRAM_MEM_AUX_CTRL_V  0xFFFFFFFFU
#define LP_AON_SPRAM_MEM_AUX_CTRL_S  0

/** LP_AON_SPRF_CTRL_REG register
 *  configure memory in lp system power status
 */
#define LP_AON_SPRF_CTRL_REG (DR_REG_LP_AON_BASE + 0x64)
/** LP_AON_SPRF_MEM_AUX_CTRL : R/W; bitpos: [31:0]; default: 8304;
 *  configure memory in lp system power status
 */
#define LP_AON_SPRF_MEM_AUX_CTRL    0xFFFFFFFFU
#define LP_AON_SPRF_MEM_AUX_CTRL_M  (LP_AON_SPRF_MEM_AUX_CTRL_V << LP_AON_SPRF_MEM_AUX_CTRL_S)
#define LP_AON_SPRF_MEM_AUX_CTRL_V  0xFFFFFFFFU
#define LP_AON_SPRF_MEM_AUX_CTRL_S  0

/** LP_AON_DEBUG_SEL0_REG register
 *  reserved
 */
#define LP_AON_DEBUG_SEL0_REG (DR_REG_LP_AON_BASE + 0x68)
/** LP_AON_LP_DEBUG_SEL0 : R/W; bitpos: [6:0]; default: 0;
 *  need des
 */
#define LP_AON_LP_DEBUG_SEL0    0x0000007FU
#define LP_AON_LP_DEBUG_SEL0_M  (LP_AON_LP_DEBUG_SEL0_V << LP_AON_LP_DEBUG_SEL0_S)
#define LP_AON_LP_DEBUG_SEL0_V  0x0000007FU
#define LP_AON_LP_DEBUG_SEL0_S  0
/** LP_AON_LP_DEBUG_SEL1 : R/W; bitpos: [13:7]; default: 0;
 *  need des
 */
#define LP_AON_LP_DEBUG_SEL1    0x0000007FU
#define LP_AON_LP_DEBUG_SEL1_M  (LP_AON_LP_DEBUG_SEL1_V << LP_AON_LP_DEBUG_SEL1_S)
#define LP_AON_LP_DEBUG_SEL1_V  0x0000007FU
#define LP_AON_LP_DEBUG_SEL1_S  7
/** LP_AON_LP_DEBUG_SEL2 : R/W; bitpos: [20:14]; default: 0;
 *  need des
 */
#define LP_AON_LP_DEBUG_SEL2    0x0000007FU
#define LP_AON_LP_DEBUG_SEL2_M  (LP_AON_LP_DEBUG_SEL2_V << LP_AON_LP_DEBUG_SEL2_S)
#define LP_AON_LP_DEBUG_SEL2_V  0x0000007FU
#define LP_AON_LP_DEBUG_SEL2_S  14
/** LP_AON_LP_DEBUG_SEL3 : R/W; bitpos: [27:21]; default: 0;
 *  need des
 */
#define LP_AON_LP_DEBUG_SEL3    0x0000007FU
#define LP_AON_LP_DEBUG_SEL3_M  (LP_AON_LP_DEBUG_SEL3_V << LP_AON_LP_DEBUG_SEL3_S)
#define LP_AON_LP_DEBUG_SEL3_V  0x0000007FU
#define LP_AON_LP_DEBUG_SEL3_S  21

/** LP_AON_DEBUG_SEL1_REG register
 *  need des
 */
#define LP_AON_DEBUG_SEL1_REG (DR_REG_LP_AON_BASE + 0x6c)
/** LP_AON_LP_DEBUG_SEL4 : R/W; bitpos: [6:0]; default: 0;
 *  need des
 */
#define LP_AON_LP_DEBUG_SEL4    0x0000007FU
#define LP_AON_LP_DEBUG_SEL4_M  (LP_AON_LP_DEBUG_SEL4_V << LP_AON_LP_DEBUG_SEL4_S)
#define LP_AON_LP_DEBUG_SEL4_V  0x0000007FU
#define LP_AON_LP_DEBUG_SEL4_S  0

/** LP_AON_BACKUP_DMA_CFG0_REG register
 *  configure regdma always on register
 */
#define LP_AON_BACKUP_DMA_CFG0_REG (DR_REG_LP_AON_BASE + 0x70)
/** LP_AON_BURST_LIMIT_AON : R/W; bitpos: [4:0]; default: 10;
 *  Set this field to configure max value of burst in signle transfer.
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
/** LP_AON_AON_BYPASS : R/W; bitpos: [31]; default: 0;
 *  reserved
 */
#define LP_AON_AON_BYPASS    (BIT(31))
#define LP_AON_AON_BYPASS_M  (LP_AON_AON_BYPASS_V << LP_AON_AON_BYPASS_S)
#define LP_AON_AON_BYPASS_V  0x00000001U
#define LP_AON_AON_BYPASS_S  31

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
/** LP_AON_LP_MEM_FORCE_PD : R/W; bitpos: [0]; default: 0;
 *  force off lp memory
 */
#define LP_AON_LP_MEM_FORCE_PD    (BIT(0))
#define LP_AON_LP_MEM_FORCE_PD_M  (LP_AON_LP_MEM_FORCE_PD_V << LP_AON_LP_MEM_FORCE_PD_S)
#define LP_AON_LP_MEM_FORCE_PD_V  0x00000001U
#define LP_AON_LP_MEM_FORCE_PD_S  0
/** LP_AON_LP_MEM_FORCE_PU : R/W; bitpos: [1]; default: 1;
 *  force on lp memory
 */
#define LP_AON_LP_MEM_FORCE_PU    (BIT(1))
#define LP_AON_LP_MEM_FORCE_PU_M  (LP_AON_LP_MEM_FORCE_PU_V << LP_AON_LP_MEM_FORCE_PU_S)
#define LP_AON_LP_MEM_FORCE_PU_V  0x00000001U
#define LP_AON_LP_MEM_FORCE_PU_S  1
/** LP_AON_HUK_MEM_FORCE_PD : R/W; bitpos: [2]; default: 1;
 *  force off huk memory
 */
#define LP_AON_HUK_MEM_FORCE_PD    (BIT(2))
#define LP_AON_HUK_MEM_FORCE_PD_M  (LP_AON_HUK_MEM_FORCE_PD_V << LP_AON_HUK_MEM_FORCE_PD_S)
#define LP_AON_HUK_MEM_FORCE_PD_V  0x00000001U
#define LP_AON_HUK_MEM_FORCE_PD_S  2
/** LP_AON_HUK_MEM_FORCE_PU : R/W; bitpos: [3]; default: 0;
 *  force on huk memory
 */
#define LP_AON_HUK_MEM_FORCE_PU    (BIT(3))
#define LP_AON_HUK_MEM_FORCE_PU_M  (LP_AON_HUK_MEM_FORCE_PU_V << LP_AON_HUK_MEM_FORCE_PU_S)
#define LP_AON_HUK_MEM_FORCE_PU_V  0x00000001U
#define LP_AON_HUK_MEM_FORCE_PU_S  3

/** LP_AON_DATE_REG register
 *  reserved
 */
#define LP_AON_DATE_REG (DR_REG_LP_AON_BASE + 0x3fc)
/** LP_AON_DATE : R/W; bitpos: [30:0]; default: 36774512;
 *  version register
 */
#define LP_AON_DATE    0x7FFFFFFFU
#define LP_AON_DATE_M  (LP_AON_DATE_V << LP_AON_DATE_S)
#define LP_AON_DATE_V  0x7FFFFFFFU
#define LP_AON_DATE_S  0
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
