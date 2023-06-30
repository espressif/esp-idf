/**
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

// TODO: IDF-5730 (better to rename and move to wdt_types.h?)
/* The value that needs to be written to LP_WDT_WPROTECT_REG to write-enable the wdt registers */
#define RTC_CNTL_WDT_WKEY_VALUE 0x50D83AA1
/* The value that needs to be written to LP_WDT_SWD_WPROTECT_REG to write-enable the swd registers */
#define LP_WDT_SWD_WKEY_VALUE 0x50D83AA1

/* Possible values for RTC_CNTL_WDT_CPU_RESET_LENGTH and RTC_CNTL_WDT_SYS_RESET_LENGTH */
#define RTC_WDT_RESET_LENGTH_100_NS    0
#define RTC_WDT_RESET_LENGTH_200_NS    1
#define RTC_WDT_RESET_LENGTH_300_NS    2
#define RTC_WDT_RESET_LENGTH_400_NS    3
#define RTC_WDT_RESET_LENGTH_500_NS    4
#define RTC_WDT_RESET_LENGTH_800_NS    5
#define RTC_WDT_RESET_LENGTH_1600_NS   6
#define RTC_WDT_RESET_LENGTH_3200_NS   7

#define LP_WDT_RTC_WDTCONFIG0_REG          (DR_REG_LP_WDT_BASE + 0x0)
/* LP_WDT_WDT_EN : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: .*/
#define LP_WDT_WDT_EN    (BIT(31))
#define LP_WDT_WDT_EN_M  (BIT(31))
#define LP_WDT_WDT_EN_V  0x1
#define LP_WDT_WDT_EN_S  31
/* LP_WDT_WDT_STG0 : R/W ;bitpos:[30:28] ;default: 3'h0 ; */
/*description: 1: interrupt stage en; 2: CPU reset stage en; 3: system reset stage en ; 4: RTC
reset stage en.*/
#define LP_WDT_WDT_STG0    0x00000007
#define LP_WDT_WDT_STG0_M  ((LP_WDT_WDT_STG0_V)<<(LP_WDT_WDT_STG0_S))
#define LP_WDT_WDT_STG0_V  0x7
#define LP_WDT_WDT_STG0_S  28
/* LP_WDT_WDT_STG1 : R/W ;bitpos:[27:25] ;default: 3'h0 ; */
/*description: 1: interrupt stage en; 2: CPU reset stage en; 3: system reset stage en ; 4: RTC
reset stage en.*/
#define LP_WDT_WDT_STG1    0x00000007
#define LP_WDT_WDT_STG1_M  ((LP_WDT_WDT_STG1_V)<<(LP_WDT_WDT_STG1_S))
#define LP_WDT_WDT_STG1_V  0x7
#define LP_WDT_WDT_STG1_S  25
/* LP_WDT_WDT_STG2 : R/W ;bitpos:[24:22] ;default: 3'h0 ; */
/*description: 1: interrupt stage en; 2: CPU reset stage en; 3: system reset stage en ; 4: RTC
reset stage en.*/
#define LP_WDT_WDT_STG2    0x00000007
#define LP_WDT_WDT_STG2_M  ((LP_WDT_WDT_STG2_V)<<(LP_WDT_WDT_STG2_S))
#define LP_WDT_WDT_STG2_V  0x7
#define LP_WDT_WDT_STG2_S  22
/* LP_WDT_WDT_STG3 : R/W ;bitpos:[21:19] ;default: 3'h0 ; */
/*description: 1: interrupt stage en; 2: CPU reset stage en; 3: system reset stage en ; 4: RTC
reset stage en.*/
#define LP_WDT_WDT_STG3    0x00000007
#define LP_WDT_WDT_STG3_M  ((LP_WDT_WDT_STG3_V)<<(LP_WDT_WDT_STG3_S))
#define LP_WDT_WDT_STG3_V  0x7
#define LP_WDT_WDT_STG3_S  19
/* LP_WDT_WDT_CPU_RESET_LENGTH : R/W ;bitpos:[18:16] ;default: 3'h1 ; */
/*description: CPU reset counter length.*/
#define LP_WDT_WDT_CPU_RESET_LENGTH    0x00000007
#define LP_WDT_WDT_CPU_RESET_LENGTH_M  ((LP_WDT_WDT_CPU_RESET_LENGTH_V)<<(LP_WDT_WDT_CPU_RESET_LENGTH_S))
#define LP_WDT_WDT_CPU_RESET_LENGTH_V  0x7
#define LP_WDT_WDT_CPU_RESET_LENGTH_S  16
/* LP_WDT_WDT_SYS_RESET_LENGTH : R/W ;bitpos:[15:13] ;default: 3'h1 ; */
/*description: system reset counter length.*/
#define LP_WDT_WDT_SYS_RESET_LENGTH    0x00000007
#define LP_WDT_WDT_SYS_RESET_LENGTH_M  ((LP_WDT_WDT_SYS_RESET_LENGTH_V)<<(LP_WDT_WDT_SYS_RESET_LENGTH_S))
#define LP_WDT_WDT_SYS_RESET_LENGTH_V  0x7
#define LP_WDT_WDT_SYS_RESET_LENGTH_S  13
/* LP_WDT_WDT_FLASHBOOT_MOD_EN : R/W ;bitpos:[12] ;default: 1'h0 ; */
/*description: enable WDT in flash boot.*/
#define LP_WDT_WDT_FLASHBOOT_MOD_EN    (BIT(12))
#define LP_WDT_WDT_FLASHBOOT_MOD_EN_M  (BIT(12))
#define LP_WDT_WDT_FLASHBOOT_MOD_EN_V  0x1
#define LP_WDT_WDT_FLASHBOOT_MOD_EN_S  12
/* LP_WDT_WDT_PAUSE_IN_SLP : R/W ;bitpos:[11] ;default: 1'd1 ; */
/*description: pause WDT in sleep.*/
#define LP_WDT_WDT_PAUSE_IN_SLP    (BIT(11))
#define LP_WDT_WDT_PAUSE_IN_SLP_M  (BIT(11))
#define LP_WDT_WDT_PAUSE_IN_SLP_V  0x1
#define LP_WDT_WDT_PAUSE_IN_SLP_S  11
/* LP_WDT_WDT_CHIP_RESET_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: wdt reset whole chip enable.*/
#define LP_WDT_WDT_CHIP_RESET_EN    (BIT(10))
#define LP_WDT_WDT_CHIP_RESET_EN_M  (BIT(10))
#define LP_WDT_WDT_CHIP_RESET_EN_V  0x1
#define LP_WDT_WDT_CHIP_RESET_EN_S  10
/* LP_WDT_WDT_CHIP_RESET_WIDTH : R/W ;bitpos:[9:2] ;default: 8'd20 ; */
/*description: chip reset siginal pulse width.*/
#define LP_WDT_WDT_CHIP_RESET_WIDTH    0x000000FF
#define LP_WDT_WDT_CHIP_RESET_WIDTH_M  ((LP_WDT_WDT_CHIP_RESET_WIDTH_V)<<(LP_WDT_WDT_CHIP_RESET_WIDTH_S))
#define LP_WDT_WDT_CHIP_RESET_WIDTH_V  0xFF
#define LP_WDT_WDT_CHIP_RESET_WIDTH_S  2

#define LP_WDT_RTC_WDTCPURST_REG          (DR_REG_LP_WDT_BASE + 0x4)
/* LP_WDT_WDT_CORE0CPU_RESET_EN : R/W ;bitpos:[31] ;default: 1'd0 ; */
/*description: enable WDT reset CORE0 CPU.*/
#define LP_WDT_WDT_CORE0CPU_RESET_EN    (BIT(31))
#define LP_WDT_WDT_CORE0CPU_RESET_EN_M  (BIT(31))
#define LP_WDT_WDT_CORE0CPU_RESET_EN_V  0x1
#define LP_WDT_WDT_CORE0CPU_RESET_EN_S  31
/* LP_WDT_WDT_CORE1CPU_RESET_EN : R/W ;bitpos:[30] ;default: 1'd0 ; */
/*description: enable WDT reset CORE1 CPU.*/
#define LP_WDT_WDT_CORE1CPU_RESET_EN    (BIT(30))
#define LP_WDT_WDT_CORE1CPU_RESET_EN_M  (BIT(30))
#define LP_WDT_WDT_CORE1CPU_RESET_EN_V  0x1
#define LP_WDT_WDT_CORE1CPU_RESET_EN_S  30
/* LP_WDT_WDT_CORE2CPU_RESET_EN : R/W ;bitpos:[29] ;default: 1'd0 ; */
/*description: enable WDT reset CORE2 CPU.*/
#define LP_WDT_WDT_CORE2CPU_RESET_EN    (BIT(29))
#define LP_WDT_WDT_CORE2CPU_RESET_EN_M  (BIT(29))
#define LP_WDT_WDT_CORE2CPU_RESET_EN_V  0x1
#define LP_WDT_WDT_CORE2CPU_RESET_EN_S  29
/* LP_WDT_WDT_CORE3CPU_RESET_EN : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: enable WDT reset CORE3 CPU.*/
#define LP_WDT_WDT_CORE3CPU_RESET_EN    (BIT(28))
#define LP_WDT_WDT_CORE3CPU_RESET_EN_M  (BIT(28))
#define LP_WDT_WDT_CORE3CPU_RESET_EN_V  0x1
#define LP_WDT_WDT_CORE3CPU_RESET_EN_S  28
/* LP_WDT_WDT_LP_CPU_RESET_EN : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: enable WDT reset LP CPU.*/
#define LP_WDT_WDT_LP_CPU_RESET_EN    (BIT(27))
#define LP_WDT_WDT_LP_CPU_RESET_EN_M  (BIT(27))
#define LP_WDT_WDT_LP_CPU_RESET_EN_V  0x1
#define LP_WDT_WDT_LP_CPU_RESET_EN_S  27
/* LP_WDT_WDT_LP_PERI_RESET_EN : R/W ;bitpos:[26] ;default: 1'd0 ; */
/*description: enable WDT reset LP PERI.*/
#define LP_WDT_WDT_LP_PERI_RESET_EN    (BIT(26))
#define LP_WDT_WDT_LP_PERI_RESET_EN_M  (BIT(26))
#define LP_WDT_WDT_LP_PERI_RESET_EN_V  0x1
#define LP_WDT_WDT_LP_PERI_RESET_EN_S  26

#define LP_WDT_RTC_WDTCONFIG1_REG          (DR_REG_LP_WDT_BASE + 0x8)
/* LP_WDT_WDT_STG0_HOLD : R/W ;bitpos:[31:0] ;default: 32'd200000 ; */
/*description: .*/
#define LP_WDT_WDT_STG0_HOLD    0xFFFFFFFF
#define LP_WDT_WDT_STG0_HOLD_M  ((LP_WDT_WDT_STG0_HOLD_V)<<(LP_WDT_WDT_STG0_HOLD_S))
#define LP_WDT_WDT_STG0_HOLD_V  0xFFFFFFFF
#define LP_WDT_WDT_STG0_HOLD_S  0

#define LP_WDT_RTC_WDTCONFIG2_REG          (DR_REG_LP_WDT_BASE + 0xC)
/* LP_WDT_WDT_STG1_HOLD : R/W ;bitpos:[31:0] ;default: 32'd80000 ; */
/*description: .*/
#define LP_WDT_WDT_STG1_HOLD    0xFFFFFFFF
#define LP_WDT_WDT_STG1_HOLD_M  ((LP_WDT_WDT_STG1_HOLD_V)<<(LP_WDT_WDT_STG1_HOLD_S))
#define LP_WDT_WDT_STG1_HOLD_V  0xFFFFFFFF
#define LP_WDT_WDT_STG1_HOLD_S  0

#define LP_WDT_RTC_WDTCONFIG3_REG          (DR_REG_LP_WDT_BASE + 0x10)
/* LP_WDT_WDT_STG2_HOLD : R/W ;bitpos:[31:0] ;default: 32'hfff ; */
/*description: .*/
#define LP_WDT_WDT_STG2_HOLD    0xFFFFFFFF
#define LP_WDT_WDT_STG2_HOLD_M  ((LP_WDT_WDT_STG2_HOLD_V)<<(LP_WDT_WDT_STG2_HOLD_S))
#define LP_WDT_WDT_STG2_HOLD_V  0xFFFFFFFF
#define LP_WDT_WDT_STG2_HOLD_S  0

#define LP_WDT_RTC_WDTCONFIG4_REG          (DR_REG_LP_WDT_BASE + 0x14)
/* LP_WDT_WDT_STG3_HOLD : R/W ;bitpos:[31:0] ;default: 32'hfff ; */
/*description: .*/
#define LP_WDT_WDT_STG3_HOLD    0xFFFFFFFF
#define LP_WDT_WDT_STG3_HOLD_M  ((LP_WDT_WDT_STG3_HOLD_V)<<(LP_WDT_WDT_STG3_HOLD_S))
#define LP_WDT_WDT_STG3_HOLD_V  0xFFFFFFFF
#define LP_WDT_WDT_STG3_HOLD_S  0

#define LP_WDT_RTC_WDTFEED_REG          (DR_REG_LP_WDT_BASE + 0x18)
/* LP_WDT_RTC_WDT_FEED : WO ;bitpos:[31] ;default: 1'd0 ; */
/*description: .*/
#define LP_WDT_RTC_WDT_FEED    (BIT(31))
#define LP_WDT_RTC_WDT_FEED_M  (BIT(31))
#define LP_WDT_RTC_WDT_FEED_V  0x1
#define LP_WDT_RTC_WDT_FEED_S  31

#define LP_WDT_RTC_WDTWPROTECT_REG          (DR_REG_LP_WDT_BASE + 0x1C)
/* LP_WDT_WDT_WKEY : R/W ;bitpos:[31:0] ;default: 32'h50d83aa1 ; */
/*description: .*/
#define LP_WDT_WDT_WKEY    0xFFFFFFFF
#define LP_WDT_WDT_WKEY_M  ((LP_WDT_WDT_WKEY_V)<<(LP_WDT_WDT_WKEY_S))
#define LP_WDT_WDT_WKEY_V  0xFFFFFFFF
#define LP_WDT_WDT_WKEY_S  0

#define LP_WDT_RTC_SWD_CONF_REG          (DR_REG_LP_WDT_BASE + 0x20)
/* LP_WDT_SWD_AUTO_FEED_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: automatically feed swd when int comes.*/
#define LP_WDT_SWD_AUTO_FEED_EN    (BIT(31))
#define LP_WDT_SWD_AUTO_FEED_EN_M  (BIT(31))
#define LP_WDT_SWD_AUTO_FEED_EN_V  0x1
#define LP_WDT_SWD_AUTO_FEED_EN_S  31
/* LP_WDT_SWD_DISABLE : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: disabel SWD.*/
#define LP_WDT_SWD_DISABLE    (BIT(30))
#define LP_WDT_SWD_DISABLE_M  (BIT(30))
#define LP_WDT_SWD_DISABLE_V  0x1
#define LP_WDT_SWD_DISABLE_S  30
/* LP_WDT_SWD_FEED : WO ;bitpos:[29] ;default: 1'b0 ; */
/*description: Sw feed swd.*/
#define LP_WDT_SWD_FEED    (BIT(29))
#define LP_WDT_SWD_FEED_M  (BIT(29))
#define LP_WDT_SWD_FEED_V  0x1
#define LP_WDT_SWD_FEED_S  29
/* LP_WDT_SWD_RST_FLAG_CLR : WO ;bitpos:[28] ;default: 1'b0 ; */
/*description: reset swd reset flag.*/
#define LP_WDT_SWD_RST_FLAG_CLR    (BIT(28))
#define LP_WDT_SWD_RST_FLAG_CLR_M  (BIT(28))
#define LP_WDT_SWD_RST_FLAG_CLR_V  0x1
#define LP_WDT_SWD_RST_FLAG_CLR_S  28
/* LP_WDT_SWD_SIGNAL_WIDTH : R/W ;bitpos:[27:18] ;default: 10'd300 ; */
/*description: adjust signal width send to swd.*/
#define LP_WDT_SWD_SIGNAL_WIDTH    0x000003FF
#define LP_WDT_SWD_SIGNAL_WIDTH_M  ((LP_WDT_SWD_SIGNAL_WIDTH_V)<<(LP_WDT_SWD_SIGNAL_WIDTH_S))
#define LP_WDT_SWD_SIGNAL_WIDTH_V  0x3FF
#define LP_WDT_SWD_SIGNAL_WIDTH_S  18
/* LP_WDT_SWD_FEED_INT : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: swd interrupt for feeding.*/
#define LP_WDT_SWD_FEED_INT    (BIT(1))
#define LP_WDT_SWD_FEED_INT_M  (BIT(1))
#define LP_WDT_SWD_FEED_INT_V  0x1
#define LP_WDT_SWD_FEED_INT_S  1
/* LP_WDT_SWD_RESET_FLAG : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: swd reset flag.*/
#define LP_WDT_SWD_RESET_FLAG    (BIT(0))
#define LP_WDT_SWD_RESET_FLAG_M  (BIT(0))
#define LP_WDT_SWD_RESET_FLAG_V  0x1
#define LP_WDT_SWD_RESET_FLAG_S  0

#define LP_WDT_RTC_SWD_WPROTECT_REG          (DR_REG_LP_WDT_BASE + 0x24)
/* LP_WDT_SWD_WKEY : R/W ;bitpos:[31:0] ;default: 32'h8f1d312a ; */
/*description: swd write protect.*/
#define LP_WDT_SWD_WKEY    0xFFFFFFFF
#define LP_WDT_SWD_WKEY_M  ((LP_WDT_SWD_WKEY_V)<<(LP_WDT_SWD_WKEY_S))
#define LP_WDT_SWD_WKEY_V  0xFFFFFFFF
#define LP_WDT_SWD_WKEY_S  0

#define LP_WDT_WDT_CLK_EN_REG          (DR_REG_LP_WDT_BASE + 0x28)
/* LP_WDT_CLK_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: .*/
#define LP_WDT_CLK_EN    (BIT(0))
#define LP_WDT_CLK_EN_M  (BIT(0))
#define LP_WDT_CLK_EN_V  0x1
#define LP_WDT_CLK_EN_S  0

#define LP_WDT_INT_ENA_RTC_W1TS_REG          (DR_REG_LP_WDT_BASE + 0x2C)
/* LP_WDT_RTC_SWD_INT_ENA_W1TS : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: enable super watch dog interrupt.*/
#define LP_WDT_RTC_SWD_INT_ENA_W1TS    (BIT(1))
#define LP_WDT_RTC_SWD_INT_ENA_W1TS_M  (BIT(1))
#define LP_WDT_RTC_SWD_INT_ENA_W1TS_V  0x1
#define LP_WDT_RTC_SWD_INT_ENA_W1TS_S  1
/* LP_WDT_RTC_WDT_INT_ENA_W1TS : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: enable RTC WDT interrupt.*/
#define LP_WDT_RTC_WDT_INT_ENA_W1TS    (BIT(0))
#define LP_WDT_RTC_WDT_INT_ENA_W1TS_M  (BIT(0))
#define LP_WDT_RTC_WDT_INT_ENA_W1TS_V  0x1
#define LP_WDT_RTC_WDT_INT_ENA_W1TS_S  0

#define LP_WDT_INT_ENA_RTC_W1TC_REG          (DR_REG_LP_WDT_BASE + 0x30)
/* LP_WDT_RTC_SWD_INT_ENA_W1TC : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: enable super watch dog interrupt.*/
#define LP_WDT_RTC_SWD_INT_ENA_W1TC    (BIT(1))
#define LP_WDT_RTC_SWD_INT_ENA_W1TC_M  (BIT(1))
#define LP_WDT_RTC_SWD_INT_ENA_W1TC_V  0x1
#define LP_WDT_RTC_SWD_INT_ENA_W1TC_S  1
/* LP_WDT_RTC_WDT_INT_ENA_W1TC : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: enable RTC WDT interrupt.*/
#define LP_WDT_RTC_WDT_INT_ENA_W1TC    (BIT(0))
#define LP_WDT_RTC_WDT_INT_ENA_W1TC_M  (BIT(0))
#define LP_WDT_RTC_WDT_INT_ENA_W1TC_V  0x1
#define LP_WDT_RTC_WDT_INT_ENA_W1TC_S  0

#define LP_WDT_INT_ENA_RTC_REG          (DR_REG_LP_WDT_BASE + 0x34)
/* LP_WDT_RTC_XTAL32K_DEAD_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: enable xtal32k_dead  interrupt.*/
#define LP_WDT_RTC_XTAL32K_DEAD_INT_ENA    (BIT(2))
#define LP_WDT_RTC_XTAL32K_DEAD_INT_ENA_M  (BIT(2))
#define LP_WDT_RTC_XTAL32K_DEAD_INT_ENA_V  0x1
#define LP_WDT_RTC_XTAL32K_DEAD_INT_ENA_S  2
/* LP_WDT_RTC_SWD_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: enable super watch dog interrupt.*/
#define LP_WDT_RTC_SWD_INT_ENA    (BIT(1))
#define LP_WDT_RTC_SWD_INT_ENA_M  (BIT(1))
#define LP_WDT_RTC_SWD_INT_ENA_V  0x1
#define LP_WDT_RTC_SWD_INT_ENA_S  1
/* LP_WDT_RTC_WDT_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: enable RTC WDT interrupt.*/
#define LP_WDT_RTC_WDT_INT_ENA    (BIT(0))
#define LP_WDT_RTC_WDT_INT_ENA_M  (BIT(0))
#define LP_WDT_RTC_WDT_INT_ENA_V  0x1
#define LP_WDT_RTC_WDT_INT_ENA_S  0

#define LP_WDT_INT_RAW_RTC_REG          (DR_REG_LP_WDT_BASE + 0x38)
/* LP_WDT_RTC_XTAL32K_DEAD_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: xtal32k dead detection interrupt raw.*/
#define LP_WDT_RTC_XTAL32K_DEAD_INT_RAW    (BIT(2))
#define LP_WDT_RTC_XTAL32K_DEAD_INT_RAW_M  (BIT(2))
#define LP_WDT_RTC_XTAL32K_DEAD_INT_RAW_V  0x1
#define LP_WDT_RTC_XTAL32K_DEAD_INT_RAW_S  2
/* LP_WDT_RTC_SWD_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: super watch dog interrupt raw.*/
#define LP_WDT_RTC_SWD_INT_RAW    (BIT(1))
#define LP_WDT_RTC_SWD_INT_RAW_M  (BIT(1))
#define LP_WDT_RTC_SWD_INT_RAW_V  0x1
#define LP_WDT_RTC_SWD_INT_RAW_S  1
/* LP_WDT_RTC_WDT_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: RTC WDT interrupt raw.*/
#define LP_WDT_RTC_WDT_INT_RAW    (BIT(0))
#define LP_WDT_RTC_WDT_INT_RAW_M  (BIT(0))
#define LP_WDT_RTC_WDT_INT_RAW_V  0x1
#define LP_WDT_RTC_WDT_INT_RAW_S  0

#define LP_WDT_INT_SWD_ST_RTC_REG          (DR_REG_LP_WDT_BASE + 0x3C)
/* LP_WDT_RTC_XTAL32K_DEAD_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: xtal32k dead detection interrupt state.*/
#define LP_WDT_RTC_XTAL32K_DEAD_INT_ST    (BIT(2))
#define LP_WDT_RTC_XTAL32K_DEAD_INT_ST_M  (BIT(2))
#define LP_WDT_RTC_XTAL32K_DEAD_INT_ST_V  0x1
#define LP_WDT_RTC_XTAL32K_DEAD_INT_ST_S  2
/* LP_WDT_RTC_SWD_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: super watch dog interrupt state.*/
#define LP_WDT_RTC_SWD_INT_ST    (BIT(1))
#define LP_WDT_RTC_SWD_INT_ST_M  (BIT(1))
#define LP_WDT_RTC_SWD_INT_ST_V  0x1
#define LP_WDT_RTC_SWD_INT_ST_S  1
/* LP_WDT_RTC_WDT_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: watch dog interrupt state.*/
#define LP_WDT_RTC_WDT_INT_ST    (BIT(0))
#define LP_WDT_RTC_WDT_INT_ST_M  (BIT(0))
#define LP_WDT_RTC_WDT_INT_ST_V  0x1
#define LP_WDT_RTC_WDT_INT_ST_S  0

#define LP_WDT_INT_CLR_RTC_REG          (DR_REG_LP_WDT_BASE + 0x40)
/* LP_WDT_RTC_XTAL32K_DEAD_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Clear RTC WDT interrupt state.*/
#define LP_WDT_RTC_XTAL32K_DEAD_INT_CLR    (BIT(2))
#define LP_WDT_RTC_XTAL32K_DEAD_INT_CLR_M  (BIT(2))
#define LP_WDT_RTC_XTAL32K_DEAD_INT_CLR_V  0x1
#define LP_WDT_RTC_XTAL32K_DEAD_INT_CLR_S  2
/* LP_WDT_RTC_SWD_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Clear super watch dog interrupt state.*/
#define LP_WDT_RTC_SWD_INT_CLR    (BIT(1))
#define LP_WDT_RTC_SWD_INT_CLR_M  (BIT(1))
#define LP_WDT_RTC_SWD_INT_CLR_V  0x1
#define LP_WDT_RTC_SWD_INT_CLR_S  1
/* LP_WDT_RTC_WDT_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Clear RTC WDT interrupt state.*/
#define LP_WDT_RTC_WDT_INT_CLR    (BIT(0))
#define LP_WDT_RTC_WDT_INT_CLR_M  (BIT(0))
#define LP_WDT_RTC_WDT_INT_CLR_V  0x1
#define LP_WDT_RTC_WDT_INT_CLR_S  0

#define LP_WDT_RTC_EXT_XTL_CONF_REG          (DR_REG_LP_WDT_BASE + 0x44)
/* LP_WDT_XTL_EXT_CTR_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: .*/
#define LP_WDT_XTL_EXT_CTR_EN    (BIT(31))
#define LP_WDT_XTL_EXT_CTR_EN_M  (BIT(31))
#define LP_WDT_XTL_EXT_CTR_EN_V  0x1
#define LP_WDT_XTL_EXT_CTR_EN_S  31
/* LP_WDT_XTL_EXT_CTR_LV : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: 0: power down XTAL at high level; 1: power down XTAL at low level.*/
#define LP_WDT_XTL_EXT_CTR_LV    (BIT(30))
#define LP_WDT_XTL_EXT_CTR_LV_M  (BIT(30))
#define LP_WDT_XTL_EXT_CTR_LV_V  0x1
#define LP_WDT_XTL_EXT_CTR_LV_S  30
/* LP_WDT_RTC_XTAL32K_GPIO_SEL : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: XTAL_32K sel. ; 0: external XTAL_32K; 1: CLK from RTC pad X32P_C.*/
#define LP_WDT_RTC_XTAL32K_GPIO_SEL    (BIT(23))
#define LP_WDT_RTC_XTAL32K_GPIO_SEL_M  (BIT(23))
#define LP_WDT_RTC_XTAL32K_GPIO_SEL_V  0x1
#define LP_WDT_RTC_XTAL32K_GPIO_SEL_S  23
/* LP_WDT_RTC_WDT_STATE : RO ;bitpos:[22:20] ;default: 3'h0 ; */
/*description: state of 32k_wdt.*/
#define LP_WDT_RTC_WDT_STATE    0x00000007
#define LP_WDT_RTC_WDT_STATE_M  ((LP_WDT_RTC_WDT_STATE_V)<<(LP_WDT_RTC_WDT_STATE_S))
#define LP_WDT_RTC_WDT_STATE_V  0x7
#define LP_WDT_RTC_WDT_STATE_S  20
/* LP_WDT_DAC_XTAL_32K : R/W ;bitpos:[19:17] ;default: 3'd3 ; */
/*description: DAC_XTAL_32K.*/
#define LP_WDT_DAC_XTAL_32K    0x00000007
#define LP_WDT_DAC_XTAL_32K_M  ((LP_WDT_DAC_XTAL_32K_V)<<(LP_WDT_DAC_XTAL_32K_S))
#define LP_WDT_DAC_XTAL_32K_V  0x7
#define LP_WDT_DAC_XTAL_32K_S  17
/* LP_WDT_XPD_XTAL_32K : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: XPD_XTAL_32K.*/
#define LP_WDT_XPD_XTAL_32K    (BIT(16))
#define LP_WDT_XPD_XTAL_32K_M  (BIT(16))
#define LP_WDT_XPD_XTAL_32K_V  0x1
#define LP_WDT_XPD_XTAL_32K_S  16
/* LP_WDT_DRES_XTAL_32K : R/W ;bitpos:[15:13] ;default: 3'd3 ; */
/*description: DRES_XTAL_32K.*/
#define LP_WDT_DRES_XTAL_32K    0x00000007
#define LP_WDT_DRES_XTAL_32K_M  ((LP_WDT_DRES_XTAL_32K_V)<<(LP_WDT_DRES_XTAL_32K_S))
#define LP_WDT_DRES_XTAL_32K_V  0x7
#define LP_WDT_DRES_XTAL_32K_S  13
/* LP_WDT_DGM_XTAL_32K : R/W ;bitpos:[12:10] ;default: 3'd3 ; */
/*description: xtal_32k gm control.*/
#define LP_WDT_DGM_XTAL_32K    0x00000007
#define LP_WDT_DGM_XTAL_32K_M  ((LP_WDT_DGM_XTAL_32K_V)<<(LP_WDT_DGM_XTAL_32K_S))
#define LP_WDT_DGM_XTAL_32K_V  0x7
#define LP_WDT_DGM_XTAL_32K_S  10
/* LP_WDT_DBUF_XTAL_32K : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: 0: single-end buffer 1: differential buffer.*/
#define LP_WDT_DBUF_XTAL_32K    (BIT(9))
#define LP_WDT_DBUF_XTAL_32K_M  (BIT(9))
#define LP_WDT_DBUF_XTAL_32K_V  0x1
#define LP_WDT_DBUF_XTAL_32K_S  9
/* LP_WDT_ENCKINIT_XTAL_32K : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: apply an internal clock to help xtal 32k to start.*/
#define LP_WDT_ENCKINIT_XTAL_32K    (BIT(8))
#define LP_WDT_ENCKINIT_XTAL_32K_M  (BIT(8))
#define LP_WDT_ENCKINIT_XTAL_32K_V  0x1
#define LP_WDT_ENCKINIT_XTAL_32K_S  8
/* LP_WDT_XTAL32K_XPD_FORCE : R/W ;bitpos:[7] ;default: 1'b1 ; */
/*description: Xtal 32k xpd control by sw or fsm.*/
#define LP_WDT_XTAL32K_XPD_FORCE    (BIT(7))
#define LP_WDT_XTAL32K_XPD_FORCE_M  (BIT(7))
#define LP_WDT_XTAL32K_XPD_FORCE_V  0x1
#define LP_WDT_XTAL32K_XPD_FORCE_S  7
/* LP_WDT_XTAL32K_AUTO_RETURN : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: xtal 32k switch back xtal when xtal is restarted.*/
#define LP_WDT_XTAL32K_AUTO_RETURN    (BIT(6))
#define LP_WDT_XTAL32K_AUTO_RETURN_M  (BIT(6))
#define LP_WDT_XTAL32K_AUTO_RETURN_V  0x1
#define LP_WDT_XTAL32K_AUTO_RETURN_S  6
/* LP_WDT_XTAL32K_AUTO_RESTART : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: xtal 32k restart xtal when xtal is dead.*/
#define LP_WDT_XTAL32K_AUTO_RESTART    (BIT(5))
#define LP_WDT_XTAL32K_AUTO_RESTART_M  (BIT(5))
#define LP_WDT_XTAL32K_AUTO_RESTART_V  0x1
#define LP_WDT_XTAL32K_AUTO_RESTART_S  5
/* LP_WDT_XTAL32K_AUTO_BACKUP : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: xtal 32k switch to back up clock when xtal is dead.*/
#define LP_WDT_XTAL32K_AUTO_BACKUP    (BIT(4))
#define LP_WDT_XTAL32K_AUTO_BACKUP_M  (BIT(4))
#define LP_WDT_XTAL32K_AUTO_BACKUP_V  0x1
#define LP_WDT_XTAL32K_AUTO_BACKUP_S  4
/* LP_WDT_XTAL32K_EXT_CLK_FO : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: xtal 32k external xtal clock force on.*/
#define LP_WDT_XTAL32K_EXT_CLK_FO    (BIT(3))
#define LP_WDT_XTAL32K_EXT_CLK_FO_M  (BIT(3))
#define LP_WDT_XTAL32K_EXT_CLK_FO_V  0x1
#define LP_WDT_XTAL32K_EXT_CLK_FO_S  3
/* LP_WDT_XTAL32K_WDT_RESET : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: xtal 32k watch dog sw reset.*/
#define LP_WDT_XTAL32K_WDT_RESET    (BIT(2))
#define LP_WDT_XTAL32K_WDT_RESET_M  (BIT(2))
#define LP_WDT_XTAL32K_WDT_RESET_V  0x1
#define LP_WDT_XTAL32K_WDT_RESET_S  2
/* LP_WDT_XTAL32K_WDT_CLK_FO : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: xtal 32k watch dog clock force on.*/
#define LP_WDT_XTAL32K_WDT_CLK_FO    (BIT(1))
#define LP_WDT_XTAL32K_WDT_CLK_FO_M  (BIT(1))
#define LP_WDT_XTAL32K_WDT_CLK_FO_V  0x1
#define LP_WDT_XTAL32K_WDT_CLK_FO_S  1
/* LP_WDT_XTAL32K_WDT_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: xtal 32k watch dog enable.*/
#define LP_WDT_XTAL32K_WDT_EN    (BIT(0))
#define LP_WDT_XTAL32K_WDT_EN_M  (BIT(0))
#define LP_WDT_XTAL32K_WDT_EN_V  0x1
#define LP_WDT_XTAL32K_WDT_EN_S  0

#define LP_WDT_RTC_XTAL32K_CLK_FACTOR_REG          (DR_REG_LP_WDT_BASE + 0x48)
/* LP_WDT_XTAL32K_CLK_FACTOR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: xtal 32k watch dog backup clock factor.*/
#define LP_WDT_XTAL32K_CLK_FACTOR    0xFFFFFFFF
#define LP_WDT_XTAL32K_CLK_FACTOR_M  ((LP_WDT_XTAL32K_CLK_FACTOR_V)<<(LP_WDT_XTAL32K_CLK_FACTOR_S))
#define LP_WDT_XTAL32K_CLK_FACTOR_V  0xFFFFFFFF
#define LP_WDT_XTAL32K_CLK_FACTOR_S  0

#define LP_WDT_RTC_XTAL32K_CONF_REG          (DR_REG_LP_WDT_BASE + 0x5C)
/* LP_WDT_XTAL32K_STABLE_THRES : R/W ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: if restarted xtal32k period is smaller than this, it is regarded as stable.*/
#define LP_WDT_XTAL32K_STABLE_THRES    0x0000000F
#define LP_WDT_XTAL32K_STABLE_THRES_M  ((LP_WDT_XTAL32K_STABLE_THRES_V)<<(LP_WDT_XTAL32K_STABLE_THRES_S))
#define LP_WDT_XTAL32K_STABLE_THRES_V  0xF
#define LP_WDT_XTAL32K_STABLE_THRES_S  28
/* LP_WDT_XTAL32K_WDT_TIMEOUT : R/W ;bitpos:[27:20] ;default: 8'hff ; */
/*description: If no clock detected for this amount of time,32k is regarded as dead.*/
#define LP_WDT_XTAL32K_WDT_TIMEOUT    0x000000FF
#define LP_WDT_XTAL32K_WDT_TIMEOUT_M  ((LP_WDT_XTAL32K_WDT_TIMEOUT_V)<<(LP_WDT_XTAL32K_WDT_TIMEOUT_S))
#define LP_WDT_XTAL32K_WDT_TIMEOUT_V  0xFF
#define LP_WDT_XTAL32K_WDT_TIMEOUT_S  20
/* LP_WDT_XTAL32K_RESTART_WAIT : R/W ;bitpos:[19:4] ;default: 16'h0 ; */
/*description: cycles to wait to repower on xtal 32k.*/
#define LP_WDT_XTAL32K_RESTART_WAIT    0x0000FFFF
#define LP_WDT_XTAL32K_RESTART_WAIT_M  ((LP_WDT_XTAL32K_RESTART_WAIT_V)<<(LP_WDT_XTAL32K_RESTART_WAIT_S))
#define LP_WDT_XTAL32K_RESTART_WAIT_V  0xFFFF
#define LP_WDT_XTAL32K_RESTART_WAIT_S  4
/* LP_WDT_XTAL32K_RETURN_WAIT : R/W ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: cycles to wait to return noral xtal 32k.*/
#define LP_WDT_XTAL32K_RETURN_WAIT    0x0000000F
#define LP_WDT_XTAL32K_RETURN_WAIT_M  ((LP_WDT_XTAL32K_RETURN_WAIT_V)<<(LP_WDT_XTAL32K_RETURN_WAIT_S))
#define LP_WDT_XTAL32K_RETURN_WAIT_V  0xF
#define LP_WDT_XTAL32K_RETURN_WAIT_S  0

#define LP_WDT_RTC_EFUSE_FORCE_REG          (DR_REG_LP_WDT_BASE + 0x60)
/* LP_WDT_WDT_FLASHBOOT_EFUSE_FORCE : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: lp_wdt flashboot en default choose efuse control bit.*/
#define LP_WDT_WDT_FLASHBOOT_EFUSE_FORCE    (BIT(1))
#define LP_WDT_WDT_FLASHBOOT_EFUSE_FORCE_M  (BIT(1))
#define LP_WDT_WDT_FLASHBOOT_EFUSE_FORCE_V  0x1
#define LP_WDT_WDT_FLASHBOOT_EFUSE_FORCE_S  1
/* LP_WDT_SWD_DISABLE_EFUSE_FORCE : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: swd disable default choose efuse control bit.*/
#define LP_WDT_SWD_DISABLE_EFUSE_FORCE    (BIT(0))
#define LP_WDT_SWD_DISABLE_EFUSE_FORCE_M  (BIT(0))
#define LP_WDT_SWD_DISABLE_EFUSE_FORCE_V  0x1
#define LP_WDT_SWD_DISABLE_EFUSE_FORCE_S  0


#ifdef __cplusplus
}
#endif
