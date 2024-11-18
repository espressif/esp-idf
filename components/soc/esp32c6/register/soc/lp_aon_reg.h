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

/** LP_AON_STORE0_REG register
 *  need_des
 */
#define LP_AON_STORE0_REG (DR_REG_LP_AON_BASE + 0x0)
/** LP_AON_STORE0 : R/W; bitpos: [31:0]; default: 0;
 *  need_des
 */
#define LP_AON_STORE0    0xFFFFFFFFU
#define LP_AON_STORE0_M  (LP_AON_STORE0_V << LP_AON_STORE0_S)
#define LP_AON_STORE0_V  0xFFFFFFFFU
#define LP_AON_STORE0_S  0

/** LP_AON_STORE1_REG register
 *  need_des
 */
#define LP_AON_STORE1_REG (DR_REG_LP_AON_BASE + 0x4)
/** LP_AON_STORE1 : R/W; bitpos: [31:0]; default: 0;
 *  need_des
 */
#define LP_AON_STORE1    0xFFFFFFFFU
#define LP_AON_STORE1_M  (LP_AON_STORE1_V << LP_AON_STORE1_S)
#define LP_AON_STORE1_V  0xFFFFFFFFU
#define LP_AON_STORE1_S  0

/** LP_AON_STORE2_REG register
 *  need_des
 */
#define LP_AON_STORE2_REG (DR_REG_LP_AON_BASE + 0x8)
/** LP_AON_STORE2 : R/W; bitpos: [31:0]; default: 0;
 *  need_des
 */
#define LP_AON_STORE2    0xFFFFFFFFU
#define LP_AON_STORE2_M  (LP_AON_STORE2_V << LP_AON_STORE2_S)
#define LP_AON_STORE2_V  0xFFFFFFFFU
#define LP_AON_STORE2_S  0

/** LP_AON_STORE3_REG register
 *  need_des
 */
#define LP_AON_STORE3_REG (DR_REG_LP_AON_BASE + 0xc)
/** LP_AON_STORE3 : R/W; bitpos: [31:0]; default: 0;
 *  need_des
 */
#define LP_AON_STORE3    0xFFFFFFFFU
#define LP_AON_STORE3_M  (LP_AON_STORE3_V << LP_AON_STORE3_S)
#define LP_AON_STORE3_V  0xFFFFFFFFU
#define LP_AON_STORE3_S  0

/** LP_AON_STORE4_REG register
 *  need_des
 */
#define LP_AON_STORE4_REG (DR_REG_LP_AON_BASE + 0x10)
/** LP_AON_STORE4 : R/W; bitpos: [31:0]; default: 0;
 *  need_des
 */
#define LP_AON_STORE4    0xFFFFFFFFU
#define LP_AON_STORE4_M  (LP_AON_STORE4_V << LP_AON_STORE4_S)
#define LP_AON_STORE4_V  0xFFFFFFFFU
#define LP_AON_STORE4_S  0

/** LP_AON_STORE5_REG register
 *  need_des
 */
#define LP_AON_STORE5_REG (DR_REG_LP_AON_BASE + 0x14)
/** LP_AON_STORE5 : R/W; bitpos: [31:0]; default: 0;
 *  need_des
 */
#define LP_AON_STORE5    0xFFFFFFFFU
#define LP_AON_STORE5_M  (LP_AON_STORE5_V << LP_AON_STORE5_S)
#define LP_AON_STORE5_V  0xFFFFFFFFU
#define LP_AON_STORE5_S  0

/** LP_AON_STORE6_REG register
 *  need_des
 */
#define LP_AON_STORE6_REG (DR_REG_LP_AON_BASE + 0x18)
/** LP_AON_STORE6 : R/W; bitpos: [31:0]; default: 0;
 *  need_des
 */
#define LP_AON_STORE6    0xFFFFFFFFU
#define LP_AON_STORE6_M  (LP_AON_STORE6_V << LP_AON_STORE6_S)
#define LP_AON_STORE6_V  0xFFFFFFFFU
#define LP_AON_STORE6_S  0

/** LP_AON_STORE7_REG register
 *  need_des
 */
#define LP_AON_STORE7_REG (DR_REG_LP_AON_BASE + 0x1c)
/** LP_AON_STORE7 : R/W; bitpos: [31:0]; default: 0;
 *  need_des
 */
#define LP_AON_STORE7    0xFFFFFFFFU
#define LP_AON_STORE7_M  (LP_AON_STORE7_V << LP_AON_STORE7_S)
#define LP_AON_STORE7_V  0xFFFFFFFFU
#define LP_AON_STORE7_S  0

/** LP_AON_STORE8_REG register
 *  need_des
 */
#define LP_AON_STORE8_REG (DR_REG_LP_AON_BASE + 0x20)
/** LP_AON_STORE8 : R/W; bitpos: [31:0]; default: 0;
 *  need_des
 */
#define LP_AON_STORE8    0xFFFFFFFFU
#define LP_AON_STORE8_M  (LP_AON_STORE8_V << LP_AON_STORE8_S)
#define LP_AON_STORE8_V  0xFFFFFFFFU
#define LP_AON_STORE8_S  0

/** LP_AON_STORE9_REG register
 *  need_des
 */
#define LP_AON_STORE9_REG (DR_REG_LP_AON_BASE + 0x24)
/** LP_AON_STORE9 : R/W; bitpos: [31:0]; default: 0;
 *  need_des
 */
#define LP_AON_STORE9    0xFFFFFFFFU
#define LP_AON_STORE9_M  (LP_AON_STORE9_V << LP_AON_STORE9_S)
#define LP_AON_STORE9_V  0xFFFFFFFFU
#define LP_AON_STORE9_S  0

/** LP_AON_GPIO_MUX_REG register
 *  need_des
 */
#define LP_AON_GPIO_MUX_REG (DR_REG_LP_AON_BASE + 0x28)
/** LP_AON_GPIO_MUX_SEL : R/W; bitpos: [7:0]; default: 0;
 *  need_des
 */
#define LP_AON_GPIO_MUX_SEL    0x000000FFU
#define LP_AON_GPIO_MUX_SEL_M  (LP_AON_GPIO_MUX_SEL_V << LP_AON_GPIO_MUX_SEL_S)
#define LP_AON_GPIO_MUX_SEL_V  0x000000FFU
#define LP_AON_GPIO_MUX_SEL_S  0

/** LP_AON_GPIO_HOLD0_REG register
 *  need_des
 */
#define LP_AON_GPIO_HOLD0_REG (DR_REG_LP_AON_BASE + 0x2c)
/** LP_AON_GPIO_HOLD0 : R/W; bitpos: [31:0]; default: 0;
 *  need_des
 */
#define LP_AON_GPIO_HOLD0    0xFFFFFFFFU
#define LP_AON_GPIO_HOLD0_M  (LP_AON_GPIO_HOLD0_V << LP_AON_GPIO_HOLD0_S)
#define LP_AON_GPIO_HOLD0_V  0xFFFFFFFFU
#define LP_AON_GPIO_HOLD0_S  0

/** LP_AON_GPIO_HOLD1_REG register
 *  need_des
 */
#define LP_AON_GPIO_HOLD1_REG (DR_REG_LP_AON_BASE + 0x30)
/** LP_AON_GPIO_HOLD1 : R/W; bitpos: [31:0]; default: 0;
 *  need_des
 */
#define LP_AON_GPIO_HOLD1    0xFFFFFFFFU
#define LP_AON_GPIO_HOLD1_M  (LP_AON_GPIO_HOLD1_V << LP_AON_GPIO_HOLD1_S)
#define LP_AON_GPIO_HOLD1_V  0xFFFFFFFFU
#define LP_AON_GPIO_HOLD1_S  0

/** LP_AON_SYS_CFG_REG register
 *  need_des
 */
#define LP_AON_SYS_CFG_REG (DR_REG_LP_AON_BASE + 0x34)
/** LP_AON_FORCE_DOWNLOAD_BOOT : R/W; bitpos: [30]; default: 0;
 *  need_des
 */
#define LP_AON_FORCE_DOWNLOAD_BOOT    (BIT(30))
#define LP_AON_FORCE_DOWNLOAD_BOOT_M  (LP_AON_FORCE_DOWNLOAD_BOOT_V << LP_AON_FORCE_DOWNLOAD_BOOT_S)
#define LP_AON_FORCE_DOWNLOAD_BOOT_V  0x00000001U
#define LP_AON_FORCE_DOWNLOAD_BOOT_S  30
/** LP_AON_HPSYS_SW_RESET : WT; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_AON_HPSYS_SW_RESET    (BIT(31))
#define LP_AON_HPSYS_SW_RESET_M  (LP_AON_HPSYS_SW_RESET_V << LP_AON_HPSYS_SW_RESET_S)
#define LP_AON_HPSYS_SW_RESET_V  0x00000001U
#define LP_AON_HPSYS_SW_RESET_S  31

/** LP_AON_CPUCORE0_CFG_REG register
 *  need_des
 */
#define LP_AON_CPUCORE0_CFG_REG (DR_REG_LP_AON_BASE + 0x38)
/** LP_AON_CPU_CORE0_SW_STALL : R/W; bitpos: [7:0]; default: 0;
 *  need_des
 */
#define LP_AON_CPU_CORE0_SW_STALL    0x000000FFU
#define LP_AON_CPU_CORE0_SW_STALL_M  (LP_AON_CPU_CORE0_SW_STALL_V << LP_AON_CPU_CORE0_SW_STALL_S)
#define LP_AON_CPU_CORE0_SW_STALL_V  0x000000FFU
#define LP_AON_CPU_CORE0_SW_STALL_S  0
/** LP_AON_CPU_CORE0_SW_RESET : WT; bitpos: [28]; default: 0;
 *  need_des
 */
#define LP_AON_CPU_CORE0_SW_RESET    (BIT(28))
#define LP_AON_CPU_CORE0_SW_RESET_M  (LP_AON_CPU_CORE0_SW_RESET_V << LP_AON_CPU_CORE0_SW_RESET_S)
#define LP_AON_CPU_CORE0_SW_RESET_V  0x00000001U
#define LP_AON_CPU_CORE0_SW_RESET_S  28
/** LP_AON_CPU_CORE0_OCD_HALT_ON_RESET : R/W; bitpos: [29]; default: 0;
 *  need_des
 */
#define LP_AON_CPU_CORE0_OCD_HALT_ON_RESET    (BIT(29))
#define LP_AON_CPU_CORE0_OCD_HALT_ON_RESET_M  (LP_AON_CPU_CORE0_OCD_HALT_ON_RESET_V << LP_AON_CPU_CORE0_OCD_HALT_ON_RESET_S)
#define LP_AON_CPU_CORE0_OCD_HALT_ON_RESET_V  0x00000001U
#define LP_AON_CPU_CORE0_OCD_HALT_ON_RESET_S  29
/** LP_AON_CPU_CORE0_STAT_VECTOR_SEL : R/W; bitpos: [30]; default: 1;
 *  need_des
 */
#define LP_AON_CPU_CORE0_STAT_VECTOR_SEL    (BIT(30))
#define LP_AON_CPU_CORE0_STAT_VECTOR_SEL_M  (LP_AON_CPU_CORE0_STAT_VECTOR_SEL_V << LP_AON_CPU_CORE0_STAT_VECTOR_SEL_S)
#define LP_AON_CPU_CORE0_STAT_VECTOR_SEL_V  0x00000001U
#define LP_AON_CPU_CORE0_STAT_VECTOR_SEL_S  30
/** LP_AON_CPU_CORE0_DRESET_MASK : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_AON_CPU_CORE0_DRESET_MASK    (BIT(31))
#define LP_AON_CPU_CORE0_DRESET_MASK_M  (LP_AON_CPU_CORE0_DRESET_MASK_V << LP_AON_CPU_CORE0_DRESET_MASK_S)
#define LP_AON_CPU_CORE0_DRESET_MASK_V  0x00000001U
#define LP_AON_CPU_CORE0_DRESET_MASK_S  31

/** LP_AON_IO_MUX_REG register
 *  need_des
 */
#define LP_AON_IO_MUX_REG (DR_REG_LP_AON_BASE + 0x3c)
/** LP_AON_IO_MUX_RESET_DISABLE : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_AON_IO_MUX_RESET_DISABLE    (BIT(31))
#define LP_AON_IO_MUX_RESET_DISABLE_M  (LP_AON_IO_MUX_RESET_DISABLE_V << LP_AON_IO_MUX_RESET_DISABLE_S)
#define LP_AON_IO_MUX_RESET_DISABLE_V  0x00000001U
#define LP_AON_IO_MUX_RESET_DISABLE_S  31

/** LP_AON_EXT_WAKEUP_CNTL_REG register
 *  need_des
 */
#define LP_AON_EXT_WAKEUP_CNTL_REG (DR_REG_LP_AON_BASE + 0x40)
/** LP_AON_EXT_WAKEUP_STATUS : RO; bitpos: [7:0]; default: 0;
 *  need_des
 */
#define LP_AON_EXT_WAKEUP_STATUS    0x000000FFU
#define LP_AON_EXT_WAKEUP_STATUS_M  (LP_AON_EXT_WAKEUP_STATUS_V << LP_AON_EXT_WAKEUP_STATUS_S)
#define LP_AON_EXT_WAKEUP_STATUS_V  0x000000FFU
#define LP_AON_EXT_WAKEUP_STATUS_S  0
/** LP_AON_EXT_WAKEUP_STATUS_CLR : WT; bitpos: [14]; default: 0;
 *  need_des
 */
#define LP_AON_EXT_WAKEUP_STATUS_CLR    (BIT(14))
#define LP_AON_EXT_WAKEUP_STATUS_CLR_M  (LP_AON_EXT_WAKEUP_STATUS_CLR_V << LP_AON_EXT_WAKEUP_STATUS_CLR_S)
#define LP_AON_EXT_WAKEUP_STATUS_CLR_V  0x00000001U
#define LP_AON_EXT_WAKEUP_STATUS_CLR_S  14
/** LP_AON_EXT_WAKEUP_SEL : R/W; bitpos: [22:15]; default: 0;
 *  need_des
 */
#define LP_AON_EXT_WAKEUP_SEL    0x000000FFU
#define LP_AON_EXT_WAKEUP_SEL_M  (LP_AON_EXT_WAKEUP_SEL_V << LP_AON_EXT_WAKEUP_SEL_S)
#define LP_AON_EXT_WAKEUP_SEL_V  0x000000FFU
#define LP_AON_EXT_WAKEUP_SEL_S  15
/** LP_AON_EXT_WAKEUP_LV : R/W; bitpos: [30:23]; default: 0;
 *  need_des
 */
#define LP_AON_EXT_WAKEUP_LV    0x000000FFU
#define LP_AON_EXT_WAKEUP_LV_M  (LP_AON_EXT_WAKEUP_LV_V << LP_AON_EXT_WAKEUP_LV_S)
#define LP_AON_EXT_WAKEUP_LV_V  0x000000FFU
#define LP_AON_EXT_WAKEUP_LV_S  23
/** LP_AON_EXT_WAKEUP_FILTER : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_AON_EXT_WAKEUP_FILTER    (BIT(31))
#define LP_AON_EXT_WAKEUP_FILTER_M  (LP_AON_EXT_WAKEUP_FILTER_V << LP_AON_EXT_WAKEUP_FILTER_S)
#define LP_AON_EXT_WAKEUP_FILTER_V  0x00000001U
#define LP_AON_EXT_WAKEUP_FILTER_S  31

/** LP_AON_USB_REG register
 *  need_des
 */
#define LP_AON_USB_REG (DR_REG_LP_AON_BASE + 0x44)
/** LP_AON_USB_RESET_DISABLE : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_AON_USB_RESET_DISABLE    (BIT(31))
#define LP_AON_USB_RESET_DISABLE_M  (LP_AON_USB_RESET_DISABLE_V << LP_AON_USB_RESET_DISABLE_S)
#define LP_AON_USB_RESET_DISABLE_V  0x00000001U
#define LP_AON_USB_RESET_DISABLE_S  31

/** LP_AON_LPBUS_REG register
 *  need_des
 */
#define LP_AON_LPBUS_REG (DR_REG_LP_AON_BASE + 0x48)
/** LP_AON_FAST_MEM_WPULSE : R/W; bitpos: [18:16]; default: 0;
 *  This field controls fast memory WPULSE parameter.
 */
#define LP_AON_FAST_MEM_WPULSE    0x00000007U
#define LP_AON_FAST_MEM_WPULSE_M  (LP_AON_FAST_MEM_WPULSE_V << LP_AON_FAST_MEM_WPULSE_S)
#define LP_AON_FAST_MEM_WPULSE_V  0x00000007U
#define LP_AON_FAST_MEM_WPULSE_S  16
/** LP_AON_FAST_MEM_WA : R/W; bitpos: [21:19]; default: 4;
 *  This field controls fast memory WA parameter.
 */
#define LP_AON_FAST_MEM_WA    0x00000007U
#define LP_AON_FAST_MEM_WA_M  (LP_AON_FAST_MEM_WA_V << LP_AON_FAST_MEM_WA_S)
#define LP_AON_FAST_MEM_WA_V  0x00000007U
#define LP_AON_FAST_MEM_WA_S  19
/** LP_AON_FAST_MEM_RA : R/W; bitpos: [23:22]; default: 0;
 *  This field controls fast memory RA parameter.
 */
#define LP_AON_FAST_MEM_RA    0x00000003U
#define LP_AON_FAST_MEM_RA_M  (LP_AON_FAST_MEM_RA_V << LP_AON_FAST_MEM_RA_S)
#define LP_AON_FAST_MEM_RA_V  0x00000003U
#define LP_AON_FAST_MEM_RA_S  22
/** LP_AON_FAST_MEM_MUX_FSM_IDLE : RO; bitpos: [28]; default: 1;
 *  need_des
 */
#define LP_AON_FAST_MEM_MUX_FSM_IDLE    (BIT(28))
#define LP_AON_FAST_MEM_MUX_FSM_IDLE_M  (LP_AON_FAST_MEM_MUX_FSM_IDLE_V << LP_AON_FAST_MEM_MUX_FSM_IDLE_S)
#define LP_AON_FAST_MEM_MUX_FSM_IDLE_V  0x00000001U
#define LP_AON_FAST_MEM_MUX_FSM_IDLE_S  28
/** LP_AON_FAST_MEM_MUX_SEL_STATUS : RO; bitpos: [29]; default: 1;
 *  need_des
 */
#define LP_AON_FAST_MEM_MUX_SEL_STATUS    (BIT(29))
#define LP_AON_FAST_MEM_MUX_SEL_STATUS_M  (LP_AON_FAST_MEM_MUX_SEL_STATUS_V << LP_AON_FAST_MEM_MUX_SEL_STATUS_S)
#define LP_AON_FAST_MEM_MUX_SEL_STATUS_V  0x00000001U
#define LP_AON_FAST_MEM_MUX_SEL_STATUS_S  29
/** LP_AON_FAST_MEM_MUX_SEL_UPDATE : WT; bitpos: [30]; default: 0;
 *  need_des
 */
#define LP_AON_FAST_MEM_MUX_SEL_UPDATE    (BIT(30))
#define LP_AON_FAST_MEM_MUX_SEL_UPDATE_M  (LP_AON_FAST_MEM_MUX_SEL_UPDATE_V << LP_AON_FAST_MEM_MUX_SEL_UPDATE_S)
#define LP_AON_FAST_MEM_MUX_SEL_UPDATE_V  0x00000001U
#define LP_AON_FAST_MEM_MUX_SEL_UPDATE_S  30
/** LP_AON_FAST_MEM_MUX_SEL : R/W; bitpos: [31]; default: 1;
 *  need_des
 */
#define LP_AON_FAST_MEM_MUX_SEL    (BIT(31))
#define LP_AON_FAST_MEM_MUX_SEL_M  (LP_AON_FAST_MEM_MUX_SEL_V << LP_AON_FAST_MEM_MUX_SEL_S)
#define LP_AON_FAST_MEM_MUX_SEL_V  0x00000001U
#define LP_AON_FAST_MEM_MUX_SEL_S  31

/** LP_AON_SDIO_ACTIVE_REG register
 *  need_des
 */
#define LP_AON_SDIO_ACTIVE_REG (DR_REG_LP_AON_BASE + 0x4c)
/** LP_AON_SDIO_ACT_DNUM : R/W; bitpos: [31:22]; default: 10;
 *  need_des
 */
#define LP_AON_SDIO_ACT_DNUM    0x000003FFU
#define LP_AON_SDIO_ACT_DNUM_M  (LP_AON_SDIO_ACT_DNUM_V << LP_AON_SDIO_ACT_DNUM_S)
#define LP_AON_SDIO_ACT_DNUM_V  0x000003FFU
#define LP_AON_SDIO_ACT_DNUM_S  22

/** LP_AON_LPCORE_REG register
 *  need_des
 */
#define LP_AON_LPCORE_REG (DR_REG_LP_AON_BASE + 0x50)
/** LP_AON_LPCORE_ETM_WAKEUP_FLAG_CLR : WT; bitpos: [0]; default: 0;
 *  need_des
 */
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG_CLR    (BIT(0))
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG_CLR_M  (LP_AON_LPCORE_ETM_WAKEUP_FLAG_CLR_V << LP_AON_LPCORE_ETM_WAKEUP_FLAG_CLR_S)
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG_CLR_V  0x00000001U
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG_CLR_S  0
/** LP_AON_LPCORE_ETM_WAKEUP_FLAG : R/WTC/SS; bitpos: [1]; default: 0;
 *  need_des
 */
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG    (BIT(1))
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG_M  (LP_AON_LPCORE_ETM_WAKEUP_FLAG_V << LP_AON_LPCORE_ETM_WAKEUP_FLAG_S)
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG_V  0x00000001U
#define LP_AON_LPCORE_ETM_WAKEUP_FLAG_S  1
/** LP_AON_LPCORE_DISABLE : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_AON_LPCORE_DISABLE    (BIT(31))
#define LP_AON_LPCORE_DISABLE_M  (LP_AON_LPCORE_DISABLE_V << LP_AON_LPCORE_DISABLE_S)
#define LP_AON_LPCORE_DISABLE_V  0x00000001U
#define LP_AON_LPCORE_DISABLE_S  31

/** LP_AON_SAR_CCT_REG register
 *  need_des
 */
#define LP_AON_SAR_CCT_REG (DR_REG_LP_AON_BASE + 0x54)
/** LP_AON_SAR2_PWDET_CCT : R/W; bitpos: [31:29]; default: 0;
 *  need_des
 */
#define LP_AON_SAR2_PWDET_CCT    0x00000007U
#define LP_AON_SAR2_PWDET_CCT_M  (LP_AON_SAR2_PWDET_CCT_V << LP_AON_SAR2_PWDET_CCT_S)
#define LP_AON_SAR2_PWDET_CCT_V  0x00000007U
#define LP_AON_SAR2_PWDET_CCT_S  29

/** LP_AON_DATE_REG register
 *  need_des
 */
#define LP_AON_DATE_REG (DR_REG_LP_AON_BASE + 0x3fc)
/** LP_AON_DATE : R/W; bitpos: [30:0]; default: 35672704;
 *  need_des
 */
#define LP_AON_DATE    0x7FFFFFFFU
#define LP_AON_DATE_M  (LP_AON_DATE_V << LP_AON_DATE_S)
#define LP_AON_DATE_V  0x7FFFFFFFU
#define LP_AON_DATE_S  0
/** LP_AON_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_AON_CLK_EN    (BIT(31))
#define LP_AON_CLK_EN_M  (LP_AON_CLK_EN_V << LP_AON_CLK_EN_S)
#define LP_AON_CLK_EN_V  0x00000001U
#define LP_AON_CLK_EN_S  31

#ifdef __cplusplus
}
#endif
