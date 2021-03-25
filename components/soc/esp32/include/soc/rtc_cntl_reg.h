// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_RTC_CNTL_REG_H_
#define _SOC_RTC_CNTL_REG_H_

/* The value that needs to be written to RTC_CNTL_WDT_WKEY to write-enable the wdt registers */
#define RTC_CNTL_WDT_WKEY_VALUE 0x50D83AA1

/* Possible values for RTC_CNTL_WDT_CPU_RESET_LENGTH and RTC_CNTL_WDT_SYS_RESET_LENGTH */
#define RTC_WDT_RESET_LENGTH_100_NS    0
#define RTC_WDT_RESET_LENGTH_200_NS    1
#define RTC_WDT_RESET_LENGTH_300_NS    2
#define RTC_WDT_RESET_LENGTH_400_NS    3
#define RTC_WDT_RESET_LENGTH_500_NS    4
#define RTC_WDT_RESET_LENGTH_800_NS    5
#define RTC_WDT_RESET_LENGTH_1600_NS   6
#define RTC_WDT_RESET_LENGTH_3200_NS   7

#include "soc.h"
#define RTC_CNTL_OPTIONS0_REG          (DR_REG_RTCCNTL_BASE + 0x0)
/* RTC_CNTL_SW_SYS_RST : WO ;bitpos:[31] ;default: 1'd0 ; */
/*description: SW system reset*/
#define RTC_CNTL_SW_SYS_RST  (BIT(31))
#define RTC_CNTL_SW_SYS_RST_M  (BIT(31))
#define RTC_CNTL_SW_SYS_RST_V  0x1
#define RTC_CNTL_SW_SYS_RST_S  31
/* RTC_CNTL_DG_WRAP_FORCE_NORST : R/W ;bitpos:[30] ;default: 1'd0 ; */
/*description: digital core force no reset in deep sleep*/
#define RTC_CNTL_DG_WRAP_FORCE_NORST  (BIT(30))
#define RTC_CNTL_DG_WRAP_FORCE_NORST_M  (BIT(30))
#define RTC_CNTL_DG_WRAP_FORCE_NORST_V  0x1
#define RTC_CNTL_DG_WRAP_FORCE_NORST_S  30
/* RTC_CNTL_DG_WRAP_FORCE_RST : R/W ;bitpos:[29] ;default: 1'd0 ; */
/*description: digital wrap force reset in deep sleep*/
#define RTC_CNTL_DG_WRAP_FORCE_RST  (BIT(29))
#define RTC_CNTL_DG_WRAP_FORCE_RST_M  (BIT(29))
#define RTC_CNTL_DG_WRAP_FORCE_RST_V  0x1
#define RTC_CNTL_DG_WRAP_FORCE_RST_S  29
/* RTC_CNTL_ANALOG_FORCE_NOISO : R/W ;bitpos:[28] ;default: 1'd1 ; */
/*description: */
#define RTC_CNTL_ANALOG_FORCE_NOISO  (BIT(28))
#define RTC_CNTL_ANALOG_FORCE_NOISO_M  (BIT(28))
#define RTC_CNTL_ANALOG_FORCE_NOISO_V  0x1
#define RTC_CNTL_ANALOG_FORCE_NOISO_S  28
/* RTC_CNTL_PLL_FORCE_NOISO : R/W ;bitpos:[27] ;default: 1'd1 ; */
/*description: */
#define RTC_CNTL_PLL_FORCE_NOISO  (BIT(27))
#define RTC_CNTL_PLL_FORCE_NOISO_M  (BIT(27))
#define RTC_CNTL_PLL_FORCE_NOISO_V  0x1
#define RTC_CNTL_PLL_FORCE_NOISO_S  27
/* RTC_CNTL_XTL_FORCE_NOISO : R/W ;bitpos:[26] ;default: 1'd1 ; */
/*description: */
#define RTC_CNTL_XTL_FORCE_NOISO  (BIT(26))
#define RTC_CNTL_XTL_FORCE_NOISO_M  (BIT(26))
#define RTC_CNTL_XTL_FORCE_NOISO_V  0x1
#define RTC_CNTL_XTL_FORCE_NOISO_S  26
/* RTC_CNTL_ANALOG_FORCE_ISO : R/W ;bitpos:[25] ;default: 1'd0 ; */
/*description: */
#define RTC_CNTL_ANALOG_FORCE_ISO  (BIT(25))
#define RTC_CNTL_ANALOG_FORCE_ISO_M  (BIT(25))
#define RTC_CNTL_ANALOG_FORCE_ISO_V  0x1
#define RTC_CNTL_ANALOG_FORCE_ISO_S  25
/* RTC_CNTL_PLL_FORCE_ISO : R/W ;bitpos:[24] ;default: 1'd0 ; */
/*description: */
#define RTC_CNTL_PLL_FORCE_ISO  (BIT(24))
#define RTC_CNTL_PLL_FORCE_ISO_M  (BIT(24))
#define RTC_CNTL_PLL_FORCE_ISO_V  0x1
#define RTC_CNTL_PLL_FORCE_ISO_S  24
/* RTC_CNTL_XTL_FORCE_ISO : R/W ;bitpos:[23] ;default: 1'd0 ; */
/*description: */
#define RTC_CNTL_XTL_FORCE_ISO  (BIT(23))
#define RTC_CNTL_XTL_FORCE_ISO_M  (BIT(23))
#define RTC_CNTL_XTL_FORCE_ISO_V  0x1
#define RTC_CNTL_XTL_FORCE_ISO_S  23
/* RTC_CNTL_BIAS_CORE_FORCE_PU : R/W ;bitpos:[22] ;default: 1'd1 ; */
/*description: BIAS_CORE force power up*/
#define RTC_CNTL_BIAS_CORE_FORCE_PU  (BIT(22))
#define RTC_CNTL_BIAS_CORE_FORCE_PU_M  (BIT(22))
#define RTC_CNTL_BIAS_CORE_FORCE_PU_V  0x1
#define RTC_CNTL_BIAS_CORE_FORCE_PU_S  22
/* RTC_CNTL_BIAS_CORE_FORCE_PD : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: BIAS_CORE force power down*/
#define RTC_CNTL_BIAS_CORE_FORCE_PD  (BIT(21))
#define RTC_CNTL_BIAS_CORE_FORCE_PD_M  (BIT(21))
#define RTC_CNTL_BIAS_CORE_FORCE_PD_V  0x1
#define RTC_CNTL_BIAS_CORE_FORCE_PD_S  21
/* RTC_CNTL_BIAS_CORE_FOLW_8M : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: BIAS_CORE follow CK8M*/
#define RTC_CNTL_BIAS_CORE_FOLW_8M  (BIT(20))
#define RTC_CNTL_BIAS_CORE_FOLW_8M_M  (BIT(20))
#define RTC_CNTL_BIAS_CORE_FOLW_8M_V  0x1
#define RTC_CNTL_BIAS_CORE_FOLW_8M_S  20
/* RTC_CNTL_BIAS_I2C_FORCE_PU : R/W ;bitpos:[19] ;default: 1'd1 ; */
/*description: BIAS_I2C force power up*/
#define RTC_CNTL_BIAS_I2C_FORCE_PU  (BIT(19))
#define RTC_CNTL_BIAS_I2C_FORCE_PU_M  (BIT(19))
#define RTC_CNTL_BIAS_I2C_FORCE_PU_V  0x1
#define RTC_CNTL_BIAS_I2C_FORCE_PU_S  19
/* RTC_CNTL_BIAS_I2C_FORCE_PD : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: BIAS_I2C force power down*/
#define RTC_CNTL_BIAS_I2C_FORCE_PD  (BIT(18))
#define RTC_CNTL_BIAS_I2C_FORCE_PD_M  (BIT(18))
#define RTC_CNTL_BIAS_I2C_FORCE_PD_V  0x1
#define RTC_CNTL_BIAS_I2C_FORCE_PD_S  18
/* RTC_CNTL_BIAS_I2C_FOLW_8M : R/W ;bitpos:[17] ;default: 1'd0 ; */
/*description: BIAS_I2C follow CK8M*/
#define RTC_CNTL_BIAS_I2C_FOLW_8M  (BIT(17))
#define RTC_CNTL_BIAS_I2C_FOLW_8M_M  (BIT(17))
#define RTC_CNTL_BIAS_I2C_FOLW_8M_V  0x1
#define RTC_CNTL_BIAS_I2C_FOLW_8M_S  17
/* RTC_CNTL_BIAS_FORCE_NOSLEEP : R/W ;bitpos:[16] ;default: 1'd1 ; */
/*description: BIAS_SLEEP force no sleep*/
#define RTC_CNTL_BIAS_FORCE_NOSLEEP  (BIT(16))
#define RTC_CNTL_BIAS_FORCE_NOSLEEP_M  (BIT(16))
#define RTC_CNTL_BIAS_FORCE_NOSLEEP_V  0x1
#define RTC_CNTL_BIAS_FORCE_NOSLEEP_S  16
/* RTC_CNTL_BIAS_FORCE_SLEEP : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: BIAS_SLEEP force sleep*/
#define RTC_CNTL_BIAS_FORCE_SLEEP  (BIT(15))
#define RTC_CNTL_BIAS_FORCE_SLEEP_M  (BIT(15))
#define RTC_CNTL_BIAS_FORCE_SLEEP_V  0x1
#define RTC_CNTL_BIAS_FORCE_SLEEP_S  15
/* RTC_CNTL_BIAS_SLEEP_FOLW_8M : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: BIAS_SLEEP follow CK8M*/
#define RTC_CNTL_BIAS_SLEEP_FOLW_8M  (BIT(14))
#define RTC_CNTL_BIAS_SLEEP_FOLW_8M_M  (BIT(14))
#define RTC_CNTL_BIAS_SLEEP_FOLW_8M_V  0x1
#define RTC_CNTL_BIAS_SLEEP_FOLW_8M_S  14
/* RTC_CNTL_XTL_FORCE_PU : R/W ;bitpos:[13] ;default: 1'd1 ; */
/*description: crystall force power up*/
#define RTC_CNTL_XTL_FORCE_PU  (BIT(13))
#define RTC_CNTL_XTL_FORCE_PU_M  (BIT(13))
#define RTC_CNTL_XTL_FORCE_PU_V  0x1
#define RTC_CNTL_XTL_FORCE_PU_S  13
/* RTC_CNTL_XTL_FORCE_PD : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: crystall force power down*/
#define RTC_CNTL_XTL_FORCE_PD  (BIT(12))
#define RTC_CNTL_XTL_FORCE_PD_M  (BIT(12))
#define RTC_CNTL_XTL_FORCE_PD_V  0x1
#define RTC_CNTL_XTL_FORCE_PD_S  12
/* RTC_CNTL_BBPLL_FORCE_PU : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: BB_PLL force power up*/
#define RTC_CNTL_BBPLL_FORCE_PU  (BIT(11))
#define RTC_CNTL_BBPLL_FORCE_PU_M  (BIT(11))
#define RTC_CNTL_BBPLL_FORCE_PU_V  0x1
#define RTC_CNTL_BBPLL_FORCE_PU_S  11
/* RTC_CNTL_BBPLL_FORCE_PD : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: BB_PLL force power down*/
#define RTC_CNTL_BBPLL_FORCE_PD  (BIT(10))
#define RTC_CNTL_BBPLL_FORCE_PD_M  (BIT(10))
#define RTC_CNTL_BBPLL_FORCE_PD_V  0x1
#define RTC_CNTL_BBPLL_FORCE_PD_S  10
/* RTC_CNTL_BBPLL_I2C_FORCE_PU : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: BB_PLL_I2C force power up*/
#define RTC_CNTL_BBPLL_I2C_FORCE_PU  (BIT(9))
#define RTC_CNTL_BBPLL_I2C_FORCE_PU_M  (BIT(9))
#define RTC_CNTL_BBPLL_I2C_FORCE_PU_V  0x1
#define RTC_CNTL_BBPLL_I2C_FORCE_PU_S  9
/* RTC_CNTL_BBPLL_I2C_FORCE_PD : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: BB_PLL _I2C force power down*/
#define RTC_CNTL_BBPLL_I2C_FORCE_PD  (BIT(8))
#define RTC_CNTL_BBPLL_I2C_FORCE_PD_M  (BIT(8))
#define RTC_CNTL_BBPLL_I2C_FORCE_PD_V  0x1
#define RTC_CNTL_BBPLL_I2C_FORCE_PD_S  8
/* RTC_CNTL_BB_I2C_FORCE_PU : R/W ;bitpos:[7] ;default: 1'd0 ; */
/*description: BB_I2C force power up*/
#define RTC_CNTL_BB_I2C_FORCE_PU  (BIT(7))
#define RTC_CNTL_BB_I2C_FORCE_PU_M  (BIT(7))
#define RTC_CNTL_BB_I2C_FORCE_PU_V  0x1
#define RTC_CNTL_BB_I2C_FORCE_PU_S  7
/* RTC_CNTL_BB_I2C_FORCE_PD : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: BB_I2C force power down*/
#define RTC_CNTL_BB_I2C_FORCE_PD  (BIT(6))
#define RTC_CNTL_BB_I2C_FORCE_PD_M  (BIT(6))
#define RTC_CNTL_BB_I2C_FORCE_PD_V  0x1
#define RTC_CNTL_BB_I2C_FORCE_PD_S  6
/* RTC_CNTL_SW_PROCPU_RST : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: PRO CPU SW reset*/
#define RTC_CNTL_SW_PROCPU_RST  (BIT(5))
#define RTC_CNTL_SW_PROCPU_RST_M  (BIT(5))
#define RTC_CNTL_SW_PROCPU_RST_V  0x1
#define RTC_CNTL_SW_PROCPU_RST_S  5
/* RTC_CNTL_SW_APPCPU_RST : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: APP CPU SW reset*/
#define RTC_CNTL_SW_APPCPU_RST  (BIT(4))
#define RTC_CNTL_SW_APPCPU_RST_M  (BIT(4))
#define RTC_CNTL_SW_APPCPU_RST_V  0x1
#define RTC_CNTL_SW_APPCPU_RST_S  4
/* RTC_CNTL_SW_STALL_PROCPU_C0 : R/W ;bitpos:[3:2] ;default: 2'b0 ; */
/*description: {reg_sw_stall_procpu_c1[5:0]   reg_sw_stall_procpu_c0[1:0]} ==
 0x86 will stall PRO CPU*/
#define RTC_CNTL_SW_STALL_PROCPU_C0  0x00000003
#define RTC_CNTL_SW_STALL_PROCPU_C0_M  ((RTC_CNTL_SW_STALL_PROCPU_C0_V)<<(RTC_CNTL_SW_STALL_PROCPU_C0_S))
#define RTC_CNTL_SW_STALL_PROCPU_C0_V  0x3
#define RTC_CNTL_SW_STALL_PROCPU_C0_S  2
/* RTC_CNTL_SW_STALL_APPCPU_C0 : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: {reg_sw_stall_appcpu_c1[5:0]   reg_sw_stall_appcpu_c0[1:0]} ==
 0x86 will stall APP CPU*/
#define RTC_CNTL_SW_STALL_APPCPU_C0  0x00000003
#define RTC_CNTL_SW_STALL_APPCPU_C0_M  ((RTC_CNTL_SW_STALL_APPCPU_C0_V)<<(RTC_CNTL_SW_STALL_APPCPU_C0_S))
#define RTC_CNTL_SW_STALL_APPCPU_C0_V  0x3
#define RTC_CNTL_SW_STALL_APPCPU_C0_S  0

#define RTC_CNTL_SLP_TIMER0_REG          (DR_REG_RTCCNTL_BASE + 0x4)
/* RTC_CNTL_SLP_VAL_LO : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: RTC sleep timer low 32 bits*/
#define RTC_CNTL_SLP_VAL_LO  0xFFFFFFFF
#define RTC_CNTL_SLP_VAL_LO_M  ((RTC_CNTL_SLP_VAL_LO_V)<<(RTC_CNTL_SLP_VAL_LO_S))
#define RTC_CNTL_SLP_VAL_LO_V  0xFFFFFFFF
#define RTC_CNTL_SLP_VAL_LO_S  0

#define RTC_CNTL_SLP_TIMER1_REG          (DR_REG_RTCCNTL_BASE + 0x8)
/* RTC_CNTL_MAIN_TIMER_ALARM_EN : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: timer alarm enable bit*/
#define RTC_CNTL_MAIN_TIMER_ALARM_EN  (BIT(16))
#define RTC_CNTL_MAIN_TIMER_ALARM_EN_M  (BIT(16))
#define RTC_CNTL_MAIN_TIMER_ALARM_EN_V  0x1
#define RTC_CNTL_MAIN_TIMER_ALARM_EN_S  16
/* RTC_CNTL_SLP_VAL_HI : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: RTC sleep timer high 16 bits*/
#define RTC_CNTL_SLP_VAL_HI  0x0000FFFF
#define RTC_CNTL_SLP_VAL_HI_M  ((RTC_CNTL_SLP_VAL_HI_V)<<(RTC_CNTL_SLP_VAL_HI_S))
#define RTC_CNTL_SLP_VAL_HI_V  0xFFFF
#define RTC_CNTL_SLP_VAL_HI_S  0

#define RTC_CNTL_TIME_UPDATE_REG          (DR_REG_RTCCNTL_BASE + 0xc)
/* RTC_CNTL_TIME_UPDATE : WO ;bitpos:[31] ;default: 1'h0 ; */
/*description: Set 1: to update register with RTC timer*/
#define RTC_CNTL_TIME_UPDATE  (BIT(31))
#define RTC_CNTL_TIME_UPDATE_M  (BIT(31))
#define RTC_CNTL_TIME_UPDATE_V  0x1
#define RTC_CNTL_TIME_UPDATE_S  31
/* RTC_CNTL_TIME_VALID : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: To indicate the register is updated*/
#define RTC_CNTL_TIME_VALID  (BIT(30))
#define RTC_CNTL_TIME_VALID_M  (BIT(30))
#define RTC_CNTL_TIME_VALID_V  0x1
#define RTC_CNTL_TIME_VALID_S  30

#define RTC_CNTL_TIME0_REG          (DR_REG_RTCCNTL_BASE + 0x10)
/* RTC_CNTL_TIME_LO : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: RTC timer low 32 bits*/
#define RTC_CNTL_TIME_LO  0xFFFFFFFF
#define RTC_CNTL_TIME_LO_M  ((RTC_CNTL_TIME_LO_V)<<(RTC_CNTL_TIME_LO_S))
#define RTC_CNTL_TIME_LO_V  0xFFFFFFFF
#define RTC_CNTL_TIME_LO_S  0

#define RTC_CNTL_TIME1_REG          (DR_REG_RTCCNTL_BASE + 0x14)
/* RTC_CNTL_TIME_HI : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: RTC timer high 16 bits*/
#define RTC_CNTL_TIME_HI  0x0000FFFF
#define RTC_CNTL_TIME_HI_M  ((RTC_CNTL_TIME_HI_V)<<(RTC_CNTL_TIME_HI_S))
#define RTC_CNTL_TIME_HI_V  0xFFFF
#define RTC_CNTL_TIME_HI_S  0

#define RTC_CNTL_STATE0_REG          (DR_REG_RTCCNTL_BASE + 0x18)
/* RTC_CNTL_SLEEP_EN : R/W ;bitpos:[31] ;default: 1'd0 ; */
/*description: sleep enable bit*/
#define RTC_CNTL_SLEEP_EN  (BIT(31))
#define RTC_CNTL_SLEEP_EN_M  (BIT(31))
#define RTC_CNTL_SLEEP_EN_V  0x1
#define RTC_CNTL_SLEEP_EN_S  31
/* RTC_CNTL_SLP_REJECT : R/W ;bitpos:[30] ;default: 1'd0 ; */
/*description: sleep reject bit*/
#define RTC_CNTL_SLP_REJECT  (BIT(30))
#define RTC_CNTL_SLP_REJECT_M  (BIT(30))
#define RTC_CNTL_SLP_REJECT_V  0x1
#define RTC_CNTL_SLP_REJECT_S  30
/* RTC_CNTL_SLP_WAKEUP : R/W ;bitpos:[29] ;default: 1'd0 ; */
/*description: sleep wakeup bit*/
#define RTC_CNTL_SLP_WAKEUP  (BIT(29))
#define RTC_CNTL_SLP_WAKEUP_M  (BIT(29))
#define RTC_CNTL_SLP_WAKEUP_V  0x1
#define RTC_CNTL_SLP_WAKEUP_S  29
/* RTC_CNTL_SDIO_ACTIVE_IND : RO ;bitpos:[28] ;default: 1'd0 ; */
/*description: SDIO active indication*/
#define RTC_CNTL_SDIO_ACTIVE_IND  (BIT(28))
#define RTC_CNTL_SDIO_ACTIVE_IND_M  (BIT(28))
#define RTC_CNTL_SDIO_ACTIVE_IND_V  0x1
#define RTC_CNTL_SDIO_ACTIVE_IND_S  28
/* RTC_CNTL_ULP_CP_SLP_TIMER_EN : R/W ;bitpos:[24] ;default: 1'd0 ; */
/*description: ULP-coprocessor timer enable bit*/
#define RTC_CNTL_ULP_CP_SLP_TIMER_EN  (BIT(24))
#define RTC_CNTL_ULP_CP_SLP_TIMER_EN_M  (BIT(24))
#define RTC_CNTL_ULP_CP_SLP_TIMER_EN_V  0x1
#define RTC_CNTL_ULP_CP_SLP_TIMER_EN_S  24
/* RTC_CNTL_TOUCH_SLP_TIMER_EN : R/W ;bitpos:[23] ;default: 1'd0 ; */
/*description: touch timer enable bit*/
#define RTC_CNTL_TOUCH_SLP_TIMER_EN  (BIT(23))
#define RTC_CNTL_TOUCH_SLP_TIMER_EN_M  (BIT(23))
#define RTC_CNTL_TOUCH_SLP_TIMER_EN_V  0x1
#define RTC_CNTL_TOUCH_SLP_TIMER_EN_S  23
/* RTC_CNTL_APB2RTC_BRIDGE_SEL : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: 1: APB to RTC using bridge   0: APB to RTC using sync*/
#define RTC_CNTL_APB2RTC_BRIDGE_SEL  (BIT(22))
#define RTC_CNTL_APB2RTC_BRIDGE_SEL_M  (BIT(22))
#define RTC_CNTL_APB2RTC_BRIDGE_SEL_V  0x1
#define RTC_CNTL_APB2RTC_BRIDGE_SEL_S  22
/* RTC_CNTL_ULP_CP_WAKEUP_FORCE_EN : R/W ;bitpos:[21] ;default: 1'd1 ; */
/*description: ULP-coprocessor force wake up*/
#define RTC_CNTL_ULP_CP_WAKEUP_FORCE_EN  (BIT(21))
#define RTC_CNTL_ULP_CP_WAKEUP_FORCE_EN_M  (BIT(21))
#define RTC_CNTL_ULP_CP_WAKEUP_FORCE_EN_V  0x1
#define RTC_CNTL_ULP_CP_WAKEUP_FORCE_EN_S  21
/* RTC_CNTL_TOUCH_WAKEUP_FORCE_EN : R/W ;bitpos:[20] ;default: 1'd1 ; */
/*description: touch controller force wake up*/
#define RTC_CNTL_TOUCH_WAKEUP_FORCE_EN  (BIT(20))
#define RTC_CNTL_TOUCH_WAKEUP_FORCE_EN_M  (BIT(20))
#define RTC_CNTL_TOUCH_WAKEUP_FORCE_EN_V  0x1
#define RTC_CNTL_TOUCH_WAKEUP_FORCE_EN_S  20

#define RTC_CNTL_TIMER1_REG          (DR_REG_RTCCNTL_BASE + 0x1c)
/* RTC_CNTL_PLL_BUF_WAIT : R/W ;bitpos:[31:24] ;default: 8'd40 ; */
/*description: PLL wait cycles in slow_clk_rtc*/
#define RTC_CNTL_PLL_BUF_WAIT  0x000000FF
#define RTC_CNTL_PLL_BUF_WAIT_M  ((RTC_CNTL_PLL_BUF_WAIT_V)<<(RTC_CNTL_PLL_BUF_WAIT_S))
#define RTC_CNTL_PLL_BUF_WAIT_V  0xFF
#define RTC_CNTL_PLL_BUF_WAIT_S  24
#define RTC_CNTL_PLL_BUF_WAIT_DEFAULT  20
/* RTC_CNTL_XTL_BUF_WAIT : R/W ;bitpos:[23:14] ;default: 10'd80 ; */
/*description: XTAL wait cycles in slow_clk_rtc*/
#define RTC_CNTL_XTL_BUF_WAIT  0x000003FF
#define RTC_CNTL_XTL_BUF_WAIT_M  ((RTC_CNTL_XTL_BUF_WAIT_V)<<(RTC_CNTL_XTL_BUF_WAIT_S))
#define RTC_CNTL_XTL_BUF_WAIT_V  0x3FF
#define RTC_CNTL_XTL_BUF_WAIT_S  14
#define RTC_CNTL_XTL_BUF_WAIT_DEFAULT  20
/* RTC_CNTL_CK8M_WAIT : R/W ;bitpos:[13:6] ;default: 8'h10 ; */
/*description: CK8M wait cycles in slow_clk_rtc*/
#define RTC_CNTL_CK8M_WAIT  0x000000FF
#define RTC_CNTL_CK8M_WAIT_M  ((RTC_CNTL_CK8M_WAIT_V)<<(RTC_CNTL_CK8M_WAIT_S))
#define RTC_CNTL_CK8M_WAIT_V  0xFF
#define RTC_CNTL_CK8M_WAIT_S  6
#define RTC_CNTL_CK8M_WAIT_DEFAULT  20
/* RTC_CNTL_CPU_STALL_WAIT : R/W ;bitpos:[5:1] ;default: 5'd1 ; */
/*description: CPU stall wait cycles in fast_clk_rtc*/
#define RTC_CNTL_CPU_STALL_WAIT  0x0000001F
#define RTC_CNTL_CPU_STALL_WAIT_M  ((RTC_CNTL_CPU_STALL_WAIT_V)<<(RTC_CNTL_CPU_STALL_WAIT_S))
#define RTC_CNTL_CPU_STALL_WAIT_V  0x1F
#define RTC_CNTL_CPU_STALL_WAIT_S  1
/* RTC_CNTL_CPU_STALL_EN : R/W ;bitpos:[0] ;default: 1'd1 ; */
/*description: CPU stall enable bit*/
#define RTC_CNTL_CPU_STALL_EN  (BIT(0))
#define RTC_CNTL_CPU_STALL_EN_M  (BIT(0))
#define RTC_CNTL_CPU_STALL_EN_V  0x1
#define RTC_CNTL_CPU_STALL_EN_S  0

#define RTC_CNTL_TIMER2_REG          (DR_REG_RTCCNTL_BASE + 0x20)
/* RTC_CNTL_MIN_TIME_CK8M_OFF : R/W ;bitpos:[31:24] ;default: 8'h1 ; */
/*description: minimal cycles in slow_clk_rtc for CK8M in power down state*/
#define RTC_CNTL_MIN_TIME_CK8M_OFF  0x000000FF
#define RTC_CNTL_MIN_TIME_CK8M_OFF_M  ((RTC_CNTL_MIN_TIME_CK8M_OFF_V)<<(RTC_CNTL_MIN_TIME_CK8M_OFF_S))
#define RTC_CNTL_MIN_TIME_CK8M_OFF_V  0xFF
#define RTC_CNTL_MIN_TIME_CK8M_OFF_S  24
/* RTC_CNTL_ULPCP_TOUCH_START_WAIT : R/W ;bitpos:[23:15] ;default: 9'h10 ; */
/*description: wait cycles in slow_clk_rtc before ULP-coprocessor / touch controller
 start to work*/
#define RTC_CNTL_ULPCP_TOUCH_START_WAIT  0x000001FF
#define RTC_CNTL_ULPCP_TOUCH_START_WAIT_M  ((RTC_CNTL_ULPCP_TOUCH_START_WAIT_V)<<(RTC_CNTL_ULPCP_TOUCH_START_WAIT_S))
#define RTC_CNTL_ULPCP_TOUCH_START_WAIT_V  0x1FF
#define RTC_CNTL_ULPCP_TOUCH_START_WAIT_S  15

#define RTC_CNTL_TIMER3_REG          (DR_REG_RTCCNTL_BASE + 0x24)
/* RTC_CNTL_ROM_RAM_POWERUP_TIMER : R/W ;bitpos:[31:25] ;default: 7'd10 ; */
/*description: */
#define RTC_CNTL_ROM_RAM_POWERUP_TIMER  0x0000007F
#define RTC_CNTL_ROM_RAM_POWERUP_TIMER_M  ((RTC_CNTL_ROM_RAM_POWERUP_TIMER_V)<<(RTC_CNTL_ROM_RAM_POWERUP_TIMER_S))
#define RTC_CNTL_ROM_RAM_POWERUP_TIMER_V  0x7F
#define RTC_CNTL_ROM_RAM_POWERUP_TIMER_S  25
/* RTC_CNTL_ROM_RAM_WAIT_TIMER : R/W ;bitpos:[24:16] ;default: 9'h16 ; */
/*description: */
#define RTC_CNTL_ROM_RAM_WAIT_TIMER  0x000001FF
#define RTC_CNTL_ROM_RAM_WAIT_TIMER_M  ((RTC_CNTL_ROM_RAM_WAIT_TIMER_V)<<(RTC_CNTL_ROM_RAM_WAIT_TIMER_S))
#define RTC_CNTL_ROM_RAM_WAIT_TIMER_V  0x1FF
#define RTC_CNTL_ROM_RAM_WAIT_TIMER_S  16
/* RTC_CNTL_WIFI_POWERUP_TIMER : R/W ;bitpos:[15:9] ;default: 7'h5 ; */
/*description: */
#define RTC_CNTL_WIFI_POWERUP_TIMER  0x0000007F
#define RTC_CNTL_WIFI_POWERUP_TIMER_M  ((RTC_CNTL_WIFI_POWERUP_TIMER_V)<<(RTC_CNTL_WIFI_POWERUP_TIMER_S))
#define RTC_CNTL_WIFI_POWERUP_TIMER_V  0x7F
#define RTC_CNTL_WIFI_POWERUP_TIMER_S  9
/* RTC_CNTL_WIFI_WAIT_TIMER : R/W ;bitpos:[8:0] ;default: 9'h8 ; */
/*description: */
#define RTC_CNTL_WIFI_WAIT_TIMER  0x000001FF
#define RTC_CNTL_WIFI_WAIT_TIMER_M  ((RTC_CNTL_WIFI_WAIT_TIMER_V)<<(RTC_CNTL_WIFI_WAIT_TIMER_S))
#define RTC_CNTL_WIFI_WAIT_TIMER_V  0x1FF
#define RTC_CNTL_WIFI_WAIT_TIMER_S  0

#define RTC_CNTL_TIMER4_REG          (DR_REG_RTCCNTL_BASE + 0x28)
/* RTC_CNTL_DG_WRAP_POWERUP_TIMER : R/W ;bitpos:[31:25] ;default: 7'h8 ; */
/*description: */
#define RTC_CNTL_DG_WRAP_POWERUP_TIMER  0x0000007F
#define RTC_CNTL_DG_WRAP_POWERUP_TIMER_M  ((RTC_CNTL_DG_WRAP_POWERUP_TIMER_V)<<(RTC_CNTL_DG_WRAP_POWERUP_TIMER_S))
#define RTC_CNTL_DG_WRAP_POWERUP_TIMER_V  0x7F
#define RTC_CNTL_DG_WRAP_POWERUP_TIMER_S  25
/* RTC_CNTL_DG_WRAP_WAIT_TIMER : R/W ;bitpos:[24:16] ;default: 9'h20 ; */
/*description: */
#define RTC_CNTL_DG_WRAP_WAIT_TIMER  0x000001FF
#define RTC_CNTL_DG_WRAP_WAIT_TIMER_M  ((RTC_CNTL_DG_WRAP_WAIT_TIMER_V)<<(RTC_CNTL_DG_WRAP_WAIT_TIMER_S))
#define RTC_CNTL_DG_WRAP_WAIT_TIMER_V  0x1FF
#define RTC_CNTL_DG_WRAP_WAIT_TIMER_S  16
/* RTC_CNTL_POWERUP_TIMER : R/W ;bitpos:[15:9] ;default: 7'h5 ; */
/*description: */
#define RTC_CNTL_POWERUP_TIMER  0x0000007F
#define RTC_CNTL_POWERUP_TIMER_M  ((RTC_CNTL_POWERUP_TIMER_V)<<(RTC_CNTL_POWERUP_TIMER_S))
#define RTC_CNTL_POWERUP_TIMER_V  0x7F
#define RTC_CNTL_POWERUP_TIMER_S  9
/* RTC_CNTL_WAIT_TIMER : R/W ;bitpos:[8:0] ;default: 9'h8 ; */
/*description: */
#define RTC_CNTL_WAIT_TIMER  0x000001FF
#define RTC_CNTL_WAIT_TIMER_M  ((RTC_CNTL_WAIT_TIMER_V)<<(RTC_CNTL_WAIT_TIMER_S))
#define RTC_CNTL_WAIT_TIMER_V  0x1FF
#define RTC_CNTL_WAIT_TIMER_S  0

#define RTC_CNTL_TIMER5_REG          (DR_REG_RTCCNTL_BASE + 0x2c)
/* RTC_CNTL_RTCMEM_POWERUP_TIMER : R/W ;bitpos:[31:25] ;default: 7'h9 ; */
/*description: */
#define RTC_CNTL_RTCMEM_POWERUP_TIMER  0x0000007F
#define RTC_CNTL_RTCMEM_POWERUP_TIMER_M  ((RTC_CNTL_RTCMEM_POWERUP_TIMER_V)<<(RTC_CNTL_RTCMEM_POWERUP_TIMER_S))
#define RTC_CNTL_RTCMEM_POWERUP_TIMER_V  0x7F
#define RTC_CNTL_RTCMEM_POWERUP_TIMER_S  25
/* RTC_CNTL_RTCMEM_WAIT_TIMER : R/W ;bitpos:[24:16] ;default: 9'h14 ; */
/*description: */
#define RTC_CNTL_RTCMEM_WAIT_TIMER  0x000001FF
#define RTC_CNTL_RTCMEM_WAIT_TIMER_M  ((RTC_CNTL_RTCMEM_WAIT_TIMER_V)<<(RTC_CNTL_RTCMEM_WAIT_TIMER_S))
#define RTC_CNTL_RTCMEM_WAIT_TIMER_V  0x1FF
#define RTC_CNTL_RTCMEM_WAIT_TIMER_S  16
/* RTC_CNTL_MIN_SLP_VAL : R/W ;bitpos:[15:8] ;default: 8'h80 ; */
/*description: minimal sleep cycles in slow_clk_rtc*/
#define RTC_CNTL_MIN_SLP_VAL  0x000000FF
#define RTC_CNTL_MIN_SLP_VAL_M  ((RTC_CNTL_MIN_SLP_VAL_V)<<(RTC_CNTL_MIN_SLP_VAL_S))
#define RTC_CNTL_MIN_SLP_VAL_V  0xFF
#define RTC_CNTL_MIN_SLP_VAL_S  8
#define RTC_CNTL_MIN_SLP_VAL_MIN 2
/* RTC_CNTL_ULP_CP_SUBTIMER_PREDIV : R/W ;bitpos:[7:0] ;default: 8'd1 ; */
/*description: */
#define RTC_CNTL_ULP_CP_SUBTIMER_PREDIV  0x000000FF
#define RTC_CNTL_ULP_CP_SUBTIMER_PREDIV_M  ((RTC_CNTL_ULP_CP_SUBTIMER_PREDIV_V)<<(RTC_CNTL_ULP_CP_SUBTIMER_PREDIV_S))
#define RTC_CNTL_ULP_CP_SUBTIMER_PREDIV_V  0xFF
#define RTC_CNTL_ULP_CP_SUBTIMER_PREDIV_S  0

#define RTC_CNTL_ANA_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x30)
/* RTC_CNTL_PLL_I2C_PU : R/W ;bitpos:[31] ;default: 1'd0 ; */
/*description: 1: PLL_I2C power up   otherwise power down*/
#define RTC_CNTL_PLL_I2C_PU  (BIT(31))
#define RTC_CNTL_PLL_I2C_PU_M  (BIT(31))
#define RTC_CNTL_PLL_I2C_PU_V  0x1
#define RTC_CNTL_PLL_I2C_PU_S  31
/* RTC_CNTL_CKGEN_I2C_PU : R/W ;bitpos:[30] ;default: 1'd0 ; */
/*description: 1: CKGEN_I2C power up   otherwise power down*/
#define RTC_CNTL_CKGEN_I2C_PU  (BIT(30))
#define RTC_CNTL_CKGEN_I2C_PU_M  (BIT(30))
#define RTC_CNTL_CKGEN_I2C_PU_V  0x1
#define RTC_CNTL_CKGEN_I2C_PU_S  30
/* RTC_CNTL_RFRX_PBUS_PU : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: 1: RFRX_PBUS power up   otherwise power down*/
#define RTC_CNTL_RFRX_PBUS_PU  (BIT(28))
#define RTC_CNTL_RFRX_PBUS_PU_M  (BIT(28))
#define RTC_CNTL_RFRX_PBUS_PU_V  0x1
#define RTC_CNTL_RFRX_PBUS_PU_S  28
/* RTC_CNTL_TXRF_I2C_PU : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: 1: TXRF_I2C power up   otherwise power down*/
#define RTC_CNTL_TXRF_I2C_PU  (BIT(27))
#define RTC_CNTL_TXRF_I2C_PU_M  (BIT(27))
#define RTC_CNTL_TXRF_I2C_PU_V  0x1
#define RTC_CNTL_TXRF_I2C_PU_S  27
/* RTC_CNTL_PVTMON_PU : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: 1: PVTMON power up   otherwise power down*/
#define RTC_CNTL_PVTMON_PU  (BIT(26))
#define RTC_CNTL_PVTMON_PU_M  (BIT(26))
#define RTC_CNTL_PVTMON_PU_V  0x1
#define RTC_CNTL_PVTMON_PU_S  26
/* RTC_CNTL_BBPLL_CAL_SLP_START : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: start BBPLL calibration during sleep*/
#define RTC_CNTL_BBPLL_CAL_SLP_START  (BIT(25))
#define RTC_CNTL_BBPLL_CAL_SLP_START_M  (BIT(25))
#define RTC_CNTL_BBPLL_CAL_SLP_START_V  0x1
#define RTC_CNTL_BBPLL_CAL_SLP_START_S  25
/* RTC_CNTL_PLLA_FORCE_PU : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: PLLA force power up*/
#define RTC_CNTL_PLLA_FORCE_PU  (BIT(24))
#define RTC_CNTL_PLLA_FORCE_PU_M  (BIT(24))
#define RTC_CNTL_PLLA_FORCE_PU_V  0x1
#define RTC_CNTL_PLLA_FORCE_PU_S  24
/* RTC_CNTL_PLLA_FORCE_PD : R/W ;bitpos:[23] ;default: 1'b1 ; */
/*description: PLLA force power down*/
#define RTC_CNTL_PLLA_FORCE_PD  (BIT(23))
#define RTC_CNTL_PLLA_FORCE_PD_M  (BIT(23))
#define RTC_CNTL_PLLA_FORCE_PD_V  0x1
#define RTC_CNTL_PLLA_FORCE_PD_S  23

#define RTC_CNTL_RESET_STATE_REG          (DR_REG_RTCCNTL_BASE + 0x34)
/* RTC_CNTL_PROCPU_STAT_VECTOR_SEL : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: PRO CPU state vector sel*/
#define RTC_CNTL_PROCPU_STAT_VECTOR_SEL  (BIT(13))
#define RTC_CNTL_PROCPU_STAT_VECTOR_SEL_M  (BIT(13))
#define RTC_CNTL_PROCPU_STAT_VECTOR_SEL_V  0x1
#define RTC_CNTL_PROCPU_STAT_VECTOR_SEL_S  13
/* RTC_CNTL_APPCPU_STAT_VECTOR_SEL : R/W ;bitpos:[12] ;default: 1'b1 ; */
/*description: APP CPU state vector sel*/
#define RTC_CNTL_APPCPU_STAT_VECTOR_SEL  (BIT(12))
#define RTC_CNTL_APPCPU_STAT_VECTOR_SEL_M  (BIT(12))
#define RTC_CNTL_APPCPU_STAT_VECTOR_SEL_V  0x1
#define RTC_CNTL_APPCPU_STAT_VECTOR_SEL_S  12
/* RTC_CNTL_RESET_CAUSE_APPCPU : RO ;bitpos:[11:6] ;default: 0 ; */
/*description: reset cause of APP CPU*/
#define RTC_CNTL_RESET_CAUSE_APPCPU  0x0000003F
#define RTC_CNTL_RESET_CAUSE_APPCPU_M  ((RTC_CNTL_RESET_CAUSE_APPCPU_V)<<(RTC_CNTL_RESET_CAUSE_APPCPU_S))
#define RTC_CNTL_RESET_CAUSE_APPCPU_V  0x3F
#define RTC_CNTL_RESET_CAUSE_APPCPU_S  6
/* RTC_CNTL_RESET_CAUSE_PROCPU : RO ;bitpos:[5:0] ;default: 0 ; */
/*description: reset cause of PRO CPU*/
#define RTC_CNTL_RESET_CAUSE_PROCPU  0x0000003F
#define RTC_CNTL_RESET_CAUSE_PROCPU_M  ((RTC_CNTL_RESET_CAUSE_PROCPU_V)<<(RTC_CNTL_RESET_CAUSE_PROCPU_S))
#define RTC_CNTL_RESET_CAUSE_PROCPU_V  0x3F
#define RTC_CNTL_RESET_CAUSE_PROCPU_S  0

#define RTC_CNTL_WAKEUP_STATE_REG          (DR_REG_RTCCNTL_BASE + 0x38)
/* RTC_CNTL_GPIO_WAKEUP_FILTER : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: enable filter for gpio wakeup event*/
#define RTC_CNTL_GPIO_WAKEUP_FILTER  (BIT(22))
#define RTC_CNTL_GPIO_WAKEUP_FILTER_M  (BIT(22))
#define RTC_CNTL_GPIO_WAKEUP_FILTER_V  0x1
#define RTC_CNTL_GPIO_WAKEUP_FILTER_S  22
/* RTC_CNTL_WAKEUP_ENA : R/W ;bitpos:[21:11] ;default: 11'b1100 ; */
/*description: wakeup enable bitmap*/
#define RTC_CNTL_WAKEUP_ENA  0x000007FF
#define RTC_CNTL_WAKEUP_ENA_M  ((RTC_CNTL_WAKEUP_ENA_V)<<(RTC_CNTL_WAKEUP_ENA_S))
#define RTC_CNTL_WAKEUP_ENA_V  0x7FF
#define RTC_CNTL_WAKEUP_ENA_S  11
/* RTC_CNTL_WAKEUP_CAUSE : RO ;bitpos:[10:0] ;default: 11'h0 ; */
/*description: wakeup cause*/
#define RTC_CNTL_WAKEUP_CAUSE  0x000007FF
#define RTC_CNTL_WAKEUP_CAUSE_M  ((RTC_CNTL_WAKEUP_CAUSE_V)<<(RTC_CNTL_WAKEUP_CAUSE_S))
#define RTC_CNTL_WAKEUP_CAUSE_V  0x7FF
#define RTC_CNTL_WAKEUP_CAUSE_S  0

#define RTC_CNTL_INT_ENA_REG          (DR_REG_RTCCNTL_BASE + 0x3c)
/* RTC_CNTL_MAIN_TIMER_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: enable RTC main timer interrupt*/
#define RTC_CNTL_MAIN_TIMER_INT_ENA  (BIT(8))
#define RTC_CNTL_MAIN_TIMER_INT_ENA_M  (BIT(8))
#define RTC_CNTL_MAIN_TIMER_INT_ENA_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_ENA_S  8
/* RTC_CNTL_BROWN_OUT_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: enable brown out interrupt*/
#define RTC_CNTL_BROWN_OUT_INT_ENA  (BIT(7))
#define RTC_CNTL_BROWN_OUT_INT_ENA_M  (BIT(7))
#define RTC_CNTL_BROWN_OUT_INT_ENA_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_ENA_S  7
/* RTC_CNTL_TOUCH_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: enable touch interrupt*/
#define RTC_CNTL_TOUCH_INT_ENA  (BIT(6))
#define RTC_CNTL_TOUCH_INT_ENA_M  (BIT(6))
#define RTC_CNTL_TOUCH_INT_ENA_V  0x1
#define RTC_CNTL_TOUCH_INT_ENA_S  6
/* RTC_CNTL_ULP_CP_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: enable ULP-coprocessor interrupt*/
#define RTC_CNTL_ULP_CP_INT_ENA  (BIT(5))
#define RTC_CNTL_ULP_CP_INT_ENA_M  (BIT(5))
#define RTC_CNTL_ULP_CP_INT_ENA_V  0x1
#define RTC_CNTL_ULP_CP_INT_ENA_S  5
/* RTC_CNTL_TIME_VALID_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: enable RTC time valid interrupt*/
#define RTC_CNTL_TIME_VALID_INT_ENA  (BIT(4))
#define RTC_CNTL_TIME_VALID_INT_ENA_M  (BIT(4))
#define RTC_CNTL_TIME_VALID_INT_ENA_V  0x1
#define RTC_CNTL_TIME_VALID_INT_ENA_S  4
/* RTC_CNTL_WDT_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: enable RTC WDT interrupt*/
#define RTC_CNTL_WDT_INT_ENA  (BIT(3))
#define RTC_CNTL_WDT_INT_ENA_M  (BIT(3))
#define RTC_CNTL_WDT_INT_ENA_V  0x1
#define RTC_CNTL_WDT_INT_ENA_S  3
/* RTC_CNTL_SDIO_IDLE_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: enable SDIO idle interrupt*/
#define RTC_CNTL_SDIO_IDLE_INT_ENA  (BIT(2))
#define RTC_CNTL_SDIO_IDLE_INT_ENA_M  (BIT(2))
#define RTC_CNTL_SDIO_IDLE_INT_ENA_V  0x1
#define RTC_CNTL_SDIO_IDLE_INT_ENA_S  2
/* RTC_CNTL_SLP_REJECT_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: enable sleep reject interrupt*/
#define RTC_CNTL_SLP_REJECT_INT_ENA  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ENA_M  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ENA_V  0x1
#define RTC_CNTL_SLP_REJECT_INT_ENA_S  1
/* RTC_CNTL_SLP_WAKEUP_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: enable sleep wakeup interrupt*/
#define RTC_CNTL_SLP_WAKEUP_INT_ENA  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_M  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_V  0x1
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_S  0

#define RTC_CNTL_INT_RAW_REG          (DR_REG_RTCCNTL_BASE + 0x40)
/* RTC_CNTL_MAIN_TIMER_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: RTC main timer interrupt raw*/
#define RTC_CNTL_MAIN_TIMER_INT_RAW  (BIT(8))
#define RTC_CNTL_MAIN_TIMER_INT_RAW_M  (BIT(8))
#define RTC_CNTL_MAIN_TIMER_INT_RAW_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_RAW_S  8
/* RTC_CNTL_BROWN_OUT_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: brown out interrupt raw*/
#define RTC_CNTL_BROWN_OUT_INT_RAW  (BIT(7))
#define RTC_CNTL_BROWN_OUT_INT_RAW_M  (BIT(7))
#define RTC_CNTL_BROWN_OUT_INT_RAW_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_RAW_S  7
/* RTC_CNTL_TOUCH_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: touch interrupt raw*/
#define RTC_CNTL_TOUCH_INT_RAW  (BIT(6))
#define RTC_CNTL_TOUCH_INT_RAW_M  (BIT(6))
#define RTC_CNTL_TOUCH_INT_RAW_V  0x1
#define RTC_CNTL_TOUCH_INT_RAW_S  6
/* RTC_CNTL_ULP_CP_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: ULP-coprocessor interrupt raw*/
#define RTC_CNTL_ULP_CP_INT_RAW  (BIT(5))
#define RTC_CNTL_ULP_CP_INT_RAW_M  (BIT(5))
#define RTC_CNTL_ULP_CP_INT_RAW_V  0x1
#define RTC_CNTL_ULP_CP_INT_RAW_S  5
/* RTC_CNTL_TIME_VALID_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: RTC time valid interrupt raw*/
#define RTC_CNTL_TIME_VALID_INT_RAW  (BIT(4))
#define RTC_CNTL_TIME_VALID_INT_RAW_M  (BIT(4))
#define RTC_CNTL_TIME_VALID_INT_RAW_V  0x1
#define RTC_CNTL_TIME_VALID_INT_RAW_S  4
/* RTC_CNTL_WDT_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: RTC WDT interrupt raw*/
#define RTC_CNTL_WDT_INT_RAW  (BIT(3))
#define RTC_CNTL_WDT_INT_RAW_M  (BIT(3))
#define RTC_CNTL_WDT_INT_RAW_V  0x1
#define RTC_CNTL_WDT_INT_RAW_S  3
/* RTC_CNTL_SDIO_IDLE_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: SDIO idle interrupt raw*/
#define RTC_CNTL_SDIO_IDLE_INT_RAW  (BIT(2))
#define RTC_CNTL_SDIO_IDLE_INT_RAW_M  (BIT(2))
#define RTC_CNTL_SDIO_IDLE_INT_RAW_V  0x1
#define RTC_CNTL_SDIO_IDLE_INT_RAW_S  2
/* RTC_CNTL_SLP_REJECT_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: sleep reject interrupt raw*/
#define RTC_CNTL_SLP_REJECT_INT_RAW  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_RAW_M  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_RAW_V  0x1
#define RTC_CNTL_SLP_REJECT_INT_RAW_S  1
/* RTC_CNTL_SLP_WAKEUP_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: sleep wakeup interrupt raw*/
#define RTC_CNTL_SLP_WAKEUP_INT_RAW  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_RAW_M  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_RAW_V  0x1
#define RTC_CNTL_SLP_WAKEUP_INT_RAW_S  0

#define RTC_CNTL_INT_ST_REG          (DR_REG_RTCCNTL_BASE + 0x44)
/* RTC_CNTL_MAIN_TIMER_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: RTC main timer interrupt state*/
#define RTC_CNTL_MAIN_TIMER_INT_ST  (BIT(8))
#define RTC_CNTL_MAIN_TIMER_INT_ST_M  (BIT(8))
#define RTC_CNTL_MAIN_TIMER_INT_ST_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_ST_S  8
/* RTC_CNTL_BROWN_OUT_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: brown out interrupt state*/
#define RTC_CNTL_BROWN_OUT_INT_ST  (BIT(7))
#define RTC_CNTL_BROWN_OUT_INT_ST_M  (BIT(7))
#define RTC_CNTL_BROWN_OUT_INT_ST_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_ST_S  7
/* RTC_CNTL_TOUCH_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: touch interrupt state*/
#define RTC_CNTL_TOUCH_INT_ST  (BIT(6))
#define RTC_CNTL_TOUCH_INT_ST_M  (BIT(6))
#define RTC_CNTL_TOUCH_INT_ST_V  0x1
#define RTC_CNTL_TOUCH_INT_ST_S  6
/* RTC_CNTL_SAR_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: ULP-coprocessor interrupt state*/
#define RTC_CNTL_SAR_INT_ST  (BIT(5))
#define RTC_CNTL_SAR_INT_ST_M  (BIT(5))
#define RTC_CNTL_SAR_INT_ST_V  0x1
#define RTC_CNTL_SAR_INT_ST_S  5
/* RTC_CNTL_TIME_VALID_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: RTC time valid interrupt state*/
#define RTC_CNTL_TIME_VALID_INT_ST  (BIT(4))
#define RTC_CNTL_TIME_VALID_INT_ST_M  (BIT(4))
#define RTC_CNTL_TIME_VALID_INT_ST_V  0x1
#define RTC_CNTL_TIME_VALID_INT_ST_S  4
/* RTC_CNTL_WDT_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: RTC WDT interrupt state*/
#define RTC_CNTL_WDT_INT_ST  (BIT(3))
#define RTC_CNTL_WDT_INT_ST_M  (BIT(3))
#define RTC_CNTL_WDT_INT_ST_V  0x1
#define RTC_CNTL_WDT_INT_ST_S  3
/* RTC_CNTL_SDIO_IDLE_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: SDIO idle interrupt state*/
#define RTC_CNTL_SDIO_IDLE_INT_ST  (BIT(2))
#define RTC_CNTL_SDIO_IDLE_INT_ST_M  (BIT(2))
#define RTC_CNTL_SDIO_IDLE_INT_ST_V  0x1
#define RTC_CNTL_SDIO_IDLE_INT_ST_S  2
/* RTC_CNTL_SLP_REJECT_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: sleep reject interrupt state*/
#define RTC_CNTL_SLP_REJECT_INT_ST  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ST_M  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ST_V  0x1
#define RTC_CNTL_SLP_REJECT_INT_ST_S  1
/* RTC_CNTL_SLP_WAKEUP_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: sleep wakeup interrupt state*/
#define RTC_CNTL_SLP_WAKEUP_INT_ST  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ST_M  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ST_V  0x1
#define RTC_CNTL_SLP_WAKEUP_INT_ST_S  0

#define RTC_CNTL_INT_CLR_REG          (DR_REG_RTCCNTL_BASE + 0x48)
/* RTC_CNTL_MAIN_TIMER_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: Clear RTC main timer interrupt state*/
#define RTC_CNTL_MAIN_TIMER_INT_CLR  (BIT(8))
#define RTC_CNTL_MAIN_TIMER_INT_CLR_M  (BIT(8))
#define RTC_CNTL_MAIN_TIMER_INT_CLR_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_CLR_S  8
/* RTC_CNTL_BROWN_OUT_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Clear brown out interrupt state*/
#define RTC_CNTL_BROWN_OUT_INT_CLR  (BIT(7))
#define RTC_CNTL_BROWN_OUT_INT_CLR_M  (BIT(7))
#define RTC_CNTL_BROWN_OUT_INT_CLR_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_CLR_S  7
/* RTC_CNTL_TOUCH_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Clear touch interrupt state*/
#define RTC_CNTL_TOUCH_INT_CLR  (BIT(6))
#define RTC_CNTL_TOUCH_INT_CLR_M  (BIT(6))
#define RTC_CNTL_TOUCH_INT_CLR_V  0x1
#define RTC_CNTL_TOUCH_INT_CLR_S  6
/* RTC_CNTL_SAR_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Clear ULP-coprocessor interrupt state*/
#define RTC_CNTL_SAR_INT_CLR  (BIT(5))
#define RTC_CNTL_SAR_INT_CLR_M  (BIT(5))
#define RTC_CNTL_SAR_INT_CLR_V  0x1
#define RTC_CNTL_SAR_INT_CLR_S  5
/* RTC_CNTL_TIME_VALID_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Clear RTC time valid interrupt state*/
#define RTC_CNTL_TIME_VALID_INT_CLR  (BIT(4))
#define RTC_CNTL_TIME_VALID_INT_CLR_M  (BIT(4))
#define RTC_CNTL_TIME_VALID_INT_CLR_V  0x1
#define RTC_CNTL_TIME_VALID_INT_CLR_S  4
/* RTC_CNTL_WDT_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Clear RTC WDT interrupt state*/
#define RTC_CNTL_WDT_INT_CLR  (BIT(3))
#define RTC_CNTL_WDT_INT_CLR_M  (BIT(3))
#define RTC_CNTL_WDT_INT_CLR_V  0x1
#define RTC_CNTL_WDT_INT_CLR_S  3
/* RTC_CNTL_SDIO_IDLE_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Clear SDIO idle interrupt state*/
#define RTC_CNTL_SDIO_IDLE_INT_CLR  (BIT(2))
#define RTC_CNTL_SDIO_IDLE_INT_CLR_M  (BIT(2))
#define RTC_CNTL_SDIO_IDLE_INT_CLR_V  0x1
#define RTC_CNTL_SDIO_IDLE_INT_CLR_S  2
/* RTC_CNTL_SLP_REJECT_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Clear sleep reject interrupt state*/
#define RTC_CNTL_SLP_REJECT_INT_CLR  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_CLR_M  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_CLR_V  0x1
#define RTC_CNTL_SLP_REJECT_INT_CLR_S  1
/* RTC_CNTL_SLP_WAKEUP_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Clear sleep wakeup interrupt state*/
#define RTC_CNTL_SLP_WAKEUP_INT_CLR  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_CLR_M  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_CLR_V  0x1
#define RTC_CNTL_SLP_WAKEUP_INT_CLR_S  0

#define RTC_CNTL_STORE0_REG          (DR_REG_RTCCNTL_BASE + 0x4c)
/* RTC_CNTL_SCRATCH0 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: 32-bit general purpose retention register*/
#define RTC_CNTL_SCRATCH0  0xFFFFFFFF
#define RTC_CNTL_SCRATCH0_M  ((RTC_CNTL_SCRATCH0_V)<<(RTC_CNTL_SCRATCH0_S))
#define RTC_CNTL_SCRATCH0_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH0_S  0

#define RTC_CNTL_STORE1_REG          (DR_REG_RTCCNTL_BASE + 0x50)
/* RTC_CNTL_SCRATCH1 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: 32-bit general purpose retention register*/
#define RTC_CNTL_SCRATCH1  0xFFFFFFFF
#define RTC_CNTL_SCRATCH1_M  ((RTC_CNTL_SCRATCH1_V)<<(RTC_CNTL_SCRATCH1_S))
#define RTC_CNTL_SCRATCH1_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH1_S  0

#define RTC_CNTL_STORE2_REG          (DR_REG_RTCCNTL_BASE + 0x54)
/* RTC_CNTL_SCRATCH2 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: 32-bit general purpose retention register*/
#define RTC_CNTL_SCRATCH2  0xFFFFFFFF
#define RTC_CNTL_SCRATCH2_M  ((RTC_CNTL_SCRATCH2_V)<<(RTC_CNTL_SCRATCH2_S))
#define RTC_CNTL_SCRATCH2_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH2_S  0

#define RTC_CNTL_STORE3_REG          (DR_REG_RTCCNTL_BASE + 0x58)
/* RTC_CNTL_SCRATCH3 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: 32-bit general purpose retention register*/
#define RTC_CNTL_SCRATCH3  0xFFFFFFFF
#define RTC_CNTL_SCRATCH3_M  ((RTC_CNTL_SCRATCH3_V)<<(RTC_CNTL_SCRATCH3_S))
#define RTC_CNTL_SCRATCH3_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH3_S  0

#define RTC_CNTL_EXT_XTL_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x5c)
/* RTC_CNTL_XTL_EXT_CTR_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: enable control XTAL by external pads*/
#define RTC_CNTL_XTL_EXT_CTR_EN  (BIT(31))
#define RTC_CNTL_XTL_EXT_CTR_EN_M  (BIT(31))
#define RTC_CNTL_XTL_EXT_CTR_EN_V  0x1
#define RTC_CNTL_XTL_EXT_CTR_EN_S  31
/* RTC_CNTL_XTL_EXT_CTR_LV : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: 0: power down XTAL at high level  1: power down XTAL at low level*/
#define RTC_CNTL_XTL_EXT_CTR_LV  (BIT(30))
#define RTC_CNTL_XTL_EXT_CTR_LV_M  (BIT(30))
#define RTC_CNTL_XTL_EXT_CTR_LV_V  0x1
#define RTC_CNTL_XTL_EXT_CTR_LV_S  30

#define RTC_CNTL_EXT_WAKEUP_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x60)
/* RTC_CNTL_EXT_WAKEUP1_LV : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: 0: external wakeup at low level  1: external wakeup at high level*/
#define RTC_CNTL_EXT_WAKEUP1_LV  (BIT(31))
#define RTC_CNTL_EXT_WAKEUP1_LV_M  (BIT(31))
#define RTC_CNTL_EXT_WAKEUP1_LV_V  0x1
#define RTC_CNTL_EXT_WAKEUP1_LV_S  31
/* RTC_CNTL_EXT_WAKEUP0_LV : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: 0: external wakeup at low level  1: external wakeup at high level*/
#define RTC_CNTL_EXT_WAKEUP0_LV  (BIT(30))
#define RTC_CNTL_EXT_WAKEUP0_LV_M  (BIT(30))
#define RTC_CNTL_EXT_WAKEUP0_LV_V  0x1
#define RTC_CNTL_EXT_WAKEUP0_LV_S  30

#define RTC_CNTL_SLP_REJECT_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x64)
/* RTC_CNTL_REJECT_CAUSE : RO ;bitpos:[31:28] ;default: 4'b0 ; */
/*description: sleep reject cause*/
#define RTC_CNTL_REJECT_CAUSE  0x0000000F
#define RTC_CNTL_REJECT_CAUSE_M  ((RTC_CNTL_REJECT_CAUSE_V)<<(RTC_CNTL_REJECT_CAUSE_S))
#define RTC_CNTL_REJECT_CAUSE_V  0xF
#define RTC_CNTL_REJECT_CAUSE_S  28
/* RTC_CNTL_DEEP_SLP_REJECT_EN : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: enable reject for deep sleep*/
#define RTC_CNTL_DEEP_SLP_REJECT_EN  (BIT(27))
#define RTC_CNTL_DEEP_SLP_REJECT_EN_M  (BIT(27))
#define RTC_CNTL_DEEP_SLP_REJECT_EN_V  0x1
#define RTC_CNTL_DEEP_SLP_REJECT_EN_S  27
/* RTC_CNTL_LIGHT_SLP_REJECT_EN : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: enable reject for light sleep*/
#define RTC_CNTL_LIGHT_SLP_REJECT_EN  (BIT(26))
#define RTC_CNTL_LIGHT_SLP_REJECT_EN_M  (BIT(26))
#define RTC_CNTL_LIGHT_SLP_REJECT_EN_V  0x1
#define RTC_CNTL_LIGHT_SLP_REJECT_EN_S  26
/* RTC_CNTL_SDIO_REJECT_EN : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: enable SDIO reject*/
#define RTC_CNTL_SDIO_REJECT_EN  (BIT(25))
#define RTC_CNTL_SDIO_REJECT_EN_M  (BIT(25))
#define RTC_CNTL_SDIO_REJECT_EN_V  0x1
#define RTC_CNTL_SDIO_REJECT_EN_S  25
/* RTC_CNTL_GPIO_REJECT_EN : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: enable GPIO reject*/
#define RTC_CNTL_GPIO_REJECT_EN  (BIT(24))
#define RTC_CNTL_GPIO_REJECT_EN_M  (BIT(24))
#define RTC_CNTL_GPIO_REJECT_EN_V  0x1
#define RTC_CNTL_GPIO_REJECT_EN_S  24

#define RTC_CNTL_CPU_PERIOD_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x68)
/* RTC_CNTL_CPUPERIOD_SEL : R/W ;bitpos:[31:30] ;default: 2'b00 ; */
/*description: CPU period sel*/
#define RTC_CNTL_CPUPERIOD_SEL  0x00000003
#define RTC_CNTL_CPUPERIOD_SEL_M  ((RTC_CNTL_CPUPERIOD_SEL_V)<<(RTC_CNTL_CPUPERIOD_SEL_S))
#define RTC_CNTL_CPUPERIOD_SEL_V  0x3
#define RTC_CNTL_CPUPERIOD_SEL_S  30
/* RTC_CNTL_CPUSEL_CONF : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: CPU sel option*/
#define RTC_CNTL_CPUSEL_CONF  (BIT(29))
#define RTC_CNTL_CPUSEL_CONF_M  (BIT(29))
#define RTC_CNTL_CPUSEL_CONF_V  0x1
#define RTC_CNTL_CPUSEL_CONF_S  29

#define RTC_CNTL_SDIO_ACT_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x6c)
/* RTC_CNTL_SDIO_ACT_DNUM : R/W ;bitpos:[31:22] ;default: 10'b0 ; */
/*description: */
#define RTC_CNTL_SDIO_ACT_DNUM  0x000003FF
#define RTC_CNTL_SDIO_ACT_DNUM_M  ((RTC_CNTL_SDIO_ACT_DNUM_V)<<(RTC_CNTL_SDIO_ACT_DNUM_S))
#define RTC_CNTL_SDIO_ACT_DNUM_V  0x3FF
#define RTC_CNTL_SDIO_ACT_DNUM_S  22

#define RTC_CNTL_CLK_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x70)
/* RTC_CNTL_ANA_CLK_RTC_SEL : R/W ;bitpos:[31:30] ;default: 2'd0 ; */
/*description: slow_clk_rtc sel. 0: SLOW_CK  1: CK_XTAL_32K  2: CK8M_D256_OUT*/
#define RTC_CNTL_ANA_CLK_RTC_SEL  0x00000003
#define RTC_CNTL_ANA_CLK_RTC_SEL_M  ((RTC_CNTL_ANA_CLK_RTC_SEL_V)<<(RTC_CNTL_ANA_CLK_RTC_SEL_S))
#define RTC_CNTL_ANA_CLK_RTC_SEL_V  0x3
#define RTC_CNTL_ANA_CLK_RTC_SEL_S  30
/* RTC_CNTL_FAST_CLK_RTC_SEL : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: fast_clk_rtc sel. 0: XTAL div 4  1: CK8M*/
#define RTC_CNTL_FAST_CLK_RTC_SEL  (BIT(29))
#define RTC_CNTL_FAST_CLK_RTC_SEL_M  (BIT(29))
#define RTC_CNTL_FAST_CLK_RTC_SEL_V  0x1
#define RTC_CNTL_FAST_CLK_RTC_SEL_S  29
/* RTC_CNTL_SOC_CLK_SEL : R/W ;bitpos:[28:27] ;default: 2'd0 ; */
/*description: SOC clock sel. 0: XTAL  1: PLL  2: CK8M  3: APLL*/
#define RTC_CNTL_SOC_CLK_SEL  0x00000003
#define RTC_CNTL_SOC_CLK_SEL_M  ((RTC_CNTL_SOC_CLK_SEL_V)<<(RTC_CNTL_SOC_CLK_SEL_S))
#define RTC_CNTL_SOC_CLK_SEL_V  0x3
#define RTC_CNTL_SOC_CLK_SEL_S  27
#define RTC_CNTL_SOC_CLK_SEL_XTL    0
#define RTC_CNTL_SOC_CLK_SEL_PLL    1
#define RTC_CNTL_SOC_CLK_SEL_8M     2
#define RTC_CNTL_SOC_CLK_SEL_APLL   3
/* RTC_CNTL_CK8M_FORCE_PU : R/W ;bitpos:[26] ;default: 1'd0 ; */
/*description: CK8M force power up*/
#define RTC_CNTL_CK8M_FORCE_PU  (BIT(26))
#define RTC_CNTL_CK8M_FORCE_PU_M  (BIT(26))
#define RTC_CNTL_CK8M_FORCE_PU_V  0x1
#define RTC_CNTL_CK8M_FORCE_PU_S  26
/* RTC_CNTL_CK8M_FORCE_PD : R/W ;bitpos:[25] ;default: 1'd0 ; */
/*description: CK8M force power down*/
#define RTC_CNTL_CK8M_FORCE_PD  (BIT(25))
#define RTC_CNTL_CK8M_FORCE_PD_M  (BIT(25))
#define RTC_CNTL_CK8M_FORCE_PD_V  0x1
#define RTC_CNTL_CK8M_FORCE_PD_S  25
/* RTC_CNTL_CK8M_DFREQ : R/W ;bitpos:[24:17] ;default: 8'd0 ; */
/*description: CK8M_DFREQ*/
#define RTC_CNTL_CK8M_DFREQ  0x000000FF
#define RTC_CNTL_CK8M_DFREQ_M  ((RTC_CNTL_CK8M_DFREQ_V)<<(RTC_CNTL_CK8M_DFREQ_S))
#define RTC_CNTL_CK8M_DFREQ_V  0xFF
#define RTC_CNTL_CK8M_DFREQ_S  17
#define RTC_CNTL_CK8M_DFREQ_DEFAULT 172
/* RTC_CNTL_CK8M_FORCE_NOGATING : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: CK8M force no gating during sleep*/
#define RTC_CNTL_CK8M_FORCE_NOGATING  (BIT(16))
#define RTC_CNTL_CK8M_FORCE_NOGATING_M  (BIT(16))
#define RTC_CNTL_CK8M_FORCE_NOGATING_V  0x1
#define RTC_CNTL_CK8M_FORCE_NOGATING_S  16
/* RTC_CNTL_XTAL_FORCE_NOGATING : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: XTAL force no gating during sleep*/
#define RTC_CNTL_XTAL_FORCE_NOGATING  (BIT(15))
#define RTC_CNTL_XTAL_FORCE_NOGATING_M  (BIT(15))
#define RTC_CNTL_XTAL_FORCE_NOGATING_V  0x1
#define RTC_CNTL_XTAL_FORCE_NOGATING_S  15
/* RTC_CNTL_CK8M_DIV_SEL : R/W ;bitpos:[14:12] ;default: 3'd2 ; */
/*description: divider = reg_ck8m_div_sel + 1*/
#define RTC_CNTL_CK8M_DIV_SEL  0x00000007
#define RTC_CNTL_CK8M_DIV_SEL_M  ((RTC_CNTL_CK8M_DIV_SEL_V)<<(RTC_CNTL_CK8M_DIV_SEL_S))
#define RTC_CNTL_CK8M_DIV_SEL_V  0x7
#define RTC_CNTL_CK8M_DIV_SEL_S  12
/* RTC_CNTL_CK8M_DFREQ_FORCE : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: */
#define RTC_CNTL_CK8M_DFREQ_FORCE  (BIT(11))
#define RTC_CNTL_CK8M_DFREQ_FORCE_M  (BIT(11))
#define RTC_CNTL_CK8M_DFREQ_FORCE_V  0x1
#define RTC_CNTL_CK8M_DFREQ_FORCE_S  11
/* RTC_CNTL_DIG_CLK8M_EN : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: enable CK8M for digital core (no relationship with RTC core)*/
#define RTC_CNTL_DIG_CLK8M_EN  (BIT(10))
#define RTC_CNTL_DIG_CLK8M_EN_M  (BIT(10))
#define RTC_CNTL_DIG_CLK8M_EN_V  0x1
#define RTC_CNTL_DIG_CLK8M_EN_S  10
/* RTC_CNTL_DIG_CLK8M_D256_EN : R/W ;bitpos:[9] ;default: 1'd1 ; */
/*description: enable CK8M_D256_OUT for digital core (no relationship with RTC core)*/
#define RTC_CNTL_DIG_CLK8M_D256_EN  (BIT(9))
#define RTC_CNTL_DIG_CLK8M_D256_EN_M  (BIT(9))
#define RTC_CNTL_DIG_CLK8M_D256_EN_V  0x1
#define RTC_CNTL_DIG_CLK8M_D256_EN_S  9
/* RTC_CNTL_DIG_XTAL32K_EN : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: enable CK_XTAL_32K for digital core (no relationship with RTC core)*/
#define RTC_CNTL_DIG_XTAL32K_EN  (BIT(8))
#define RTC_CNTL_DIG_XTAL32K_EN_M  (BIT(8))
#define RTC_CNTL_DIG_XTAL32K_EN_V  0x1
#define RTC_CNTL_DIG_XTAL32K_EN_S  8
/* RTC_CNTL_ENB_CK8M_DIV : R/W ;bitpos:[7] ;default: 1'd0 ; */
/*description: 1: CK8M_D256_OUT is actually CK8M  0: CK8M_D256_OUT is CK8M divided by 256*/
#define RTC_CNTL_ENB_CK8M_DIV  (BIT(7))
#define RTC_CNTL_ENB_CK8M_DIV_M  (BIT(7))
#define RTC_CNTL_ENB_CK8M_DIV_V  0x1
#define RTC_CNTL_ENB_CK8M_DIV_S  7
/* RTC_CNTL_ENB_CK8M : R/W ;bitpos:[6] ;default: 1'd0 ; */
/*description: disable CK8M and CK8M_D256_OUT*/
#define RTC_CNTL_ENB_CK8M  (BIT(6))
#define RTC_CNTL_ENB_CK8M_M  (BIT(6))
#define RTC_CNTL_ENB_CK8M_V  0x1
#define RTC_CNTL_ENB_CK8M_S  6
/* RTC_CNTL_CK8M_DIV : R/W ;bitpos:[5:4] ;default: 2'b01 ; */
/*description: CK8M_D256_OUT divider. 00: div128  01: div256  10: div512  11: div1024.*/
#define RTC_CNTL_CK8M_DIV  0x00000003
#define RTC_CNTL_CK8M_DIV_M  ((RTC_CNTL_CK8M_DIV_V)<<(RTC_CNTL_CK8M_DIV_S))
#define RTC_CNTL_CK8M_DIV_V  0x3
#define RTC_CNTL_CK8M_DIV_S  4

#define RTC_CNTL_SDIO_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x74)
/* RTC_CNTL_XPD_SDIO_REG : R/W ;bitpos:[31] ;default: 1'd0 ; */
/*description: SW option for XPD_SDIO_REG. Only active when reg_sdio_force = 1*/
#define RTC_CNTL_XPD_SDIO_REG  (BIT(31))
#define RTC_CNTL_XPD_SDIO_REG_M  (BIT(31))
#define RTC_CNTL_XPD_SDIO_REG_V  0x1
#define RTC_CNTL_XPD_SDIO_REG_S  31
/* RTC_CNTL_DREFH_SDIO : R/W ;bitpos:[30:29] ;default: 2'b00 ; */
/*description: SW option for DREFH_SDIO. Only active when reg_sdio_force = 1*/
#define RTC_CNTL_DREFH_SDIO  0x00000003
#define RTC_CNTL_DREFH_SDIO_M  ((RTC_CNTL_DREFH_SDIO_V)<<(RTC_CNTL_DREFH_SDIO_S))
#define RTC_CNTL_DREFH_SDIO_V  0x3
#define RTC_CNTL_DREFH_SDIO_S  29
/* RTC_CNTL_DREFM_SDIO : R/W ;bitpos:[28:27] ;default: 2'b00 ; */
/*description: SW option for DREFM_SDIO. Only active when reg_sdio_force = 1*/
#define RTC_CNTL_DREFM_SDIO  0x00000003
#define RTC_CNTL_DREFM_SDIO_M  ((RTC_CNTL_DREFM_SDIO_V)<<(RTC_CNTL_DREFM_SDIO_S))
#define RTC_CNTL_DREFM_SDIO_V  0x3
#define RTC_CNTL_DREFM_SDIO_S  27
/* RTC_CNTL_DREFL_SDIO : R/W ;bitpos:[26:25] ;default: 2'b01 ; */
/*description: SW option for DREFL_SDIO. Only active when reg_sdio_force = 1*/
#define RTC_CNTL_DREFL_SDIO  0x00000003
#define RTC_CNTL_DREFL_SDIO_M  ((RTC_CNTL_DREFL_SDIO_V)<<(RTC_CNTL_DREFL_SDIO_S))
#define RTC_CNTL_DREFL_SDIO_V  0x3
#define RTC_CNTL_DREFL_SDIO_S  25
/* RTC_CNTL_REG1P8_READY : RO ;bitpos:[24] ;default: 1'd0 ; */
/*description: read only register for REG1P8_READY*/
#define RTC_CNTL_REG1P8_READY  (BIT(24))
#define RTC_CNTL_REG1P8_READY_M  (BIT(24))
#define RTC_CNTL_REG1P8_READY_V  0x1
#define RTC_CNTL_REG1P8_READY_S  24
/* RTC_CNTL_SDIO_TIEH : R/W ;bitpos:[23] ;default: 1'd1 ; */
/*description: SW option for SDIO_TIEH. Only active when reg_sdio_force = 1*/
#define RTC_CNTL_SDIO_TIEH  (BIT(23))
#define RTC_CNTL_SDIO_TIEH_M  (BIT(23))
#define RTC_CNTL_SDIO_TIEH_V  0x1
#define RTC_CNTL_SDIO_TIEH_S  23
/* RTC_CNTL_SDIO_FORCE : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: 1: use SW option to control SDIO_REG  0: use state machine*/
#define RTC_CNTL_SDIO_FORCE  (BIT(22))
#define RTC_CNTL_SDIO_FORCE_M  (BIT(22))
#define RTC_CNTL_SDIO_FORCE_V  0x1
#define RTC_CNTL_SDIO_FORCE_S  22
/* RTC_CNTL_SDIO_PD_EN : R/W ;bitpos:[21] ;default: 1'd1 ; */
/*description: power down SDIO_REG in sleep. Only active when reg_sdio_force = 0*/
#define RTC_CNTL_SDIO_PD_EN  (BIT(21))
#define RTC_CNTL_SDIO_PD_EN_M  (BIT(21))
#define RTC_CNTL_SDIO_PD_EN_V  0x1
#define RTC_CNTL_SDIO_PD_EN_S  21

#define RTC_CNTL_BIAS_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x78)
/* RTC_CNTL_RST_BIAS_I2C : R/W ;bitpos:[31] ;default: 1'd0 ; */
/*description: RST_BIAS_I2C*/
#define RTC_CNTL_RST_BIAS_I2C  (BIT(31))
#define RTC_CNTL_RST_BIAS_I2C_M  (BIT(31))
#define RTC_CNTL_RST_BIAS_I2C_V  0x1
#define RTC_CNTL_RST_BIAS_I2C_S  31
/* RTC_CNTL_DEC_HEARTBEAT_WIDTH : R/W ;bitpos:[30] ;default: 1'd0 ; */
/*description: DEC_HEARTBEAT_WIDTH*/
#define RTC_CNTL_DEC_HEARTBEAT_WIDTH  (BIT(30))
#define RTC_CNTL_DEC_HEARTBEAT_WIDTH_M  (BIT(30))
#define RTC_CNTL_DEC_HEARTBEAT_WIDTH_V  0x1
#define RTC_CNTL_DEC_HEARTBEAT_WIDTH_S  30
/* RTC_CNTL_INC_HEARTBEAT_PERIOD : R/W ;bitpos:[29] ;default: 1'd0 ; */
/*description: INC_HEARTBEAT_PERIOD*/
#define RTC_CNTL_INC_HEARTBEAT_PERIOD  (BIT(29))
#define RTC_CNTL_INC_HEARTBEAT_PERIOD_M  (BIT(29))
#define RTC_CNTL_INC_HEARTBEAT_PERIOD_V  0x1
#define RTC_CNTL_INC_HEARTBEAT_PERIOD_S  29
/* RTC_CNTL_DEC_HEARTBEAT_PERIOD : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: DEC_HEARTBEAT_PERIOD*/
#define RTC_CNTL_DEC_HEARTBEAT_PERIOD  (BIT(28))
#define RTC_CNTL_DEC_HEARTBEAT_PERIOD_M  (BIT(28))
#define RTC_CNTL_DEC_HEARTBEAT_PERIOD_V  0x1
#define RTC_CNTL_DEC_HEARTBEAT_PERIOD_S  28
/* RTC_CNTL_INC_HEARTBEAT_REFRESH : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: INC_HEARTBEAT_REFRESH*/
#define RTC_CNTL_INC_HEARTBEAT_REFRESH  (BIT(27))
#define RTC_CNTL_INC_HEARTBEAT_REFRESH_M  (BIT(27))
#define RTC_CNTL_INC_HEARTBEAT_REFRESH_V  0x1
#define RTC_CNTL_INC_HEARTBEAT_REFRESH_S  27
/* RTC_CNTL_ENB_SCK_XTAL : R/W ;bitpos:[26] ;default: 1'd0 ; */
/*description: ENB_SCK_XTAL*/
#define RTC_CNTL_ENB_SCK_XTAL  (BIT(26))
#define RTC_CNTL_ENB_SCK_XTAL_M  (BIT(26))
#define RTC_CNTL_ENB_SCK_XTAL_V  0x1
#define RTC_CNTL_ENB_SCK_XTAL_S  26
/* RTC_CNTL_DBG_ATTEN : R/W ;bitpos:[25:24] ;default: 2'b00 ; */
/*description: DBG_ATTEN*/
#define RTC_CNTL_DBG_ATTEN  0x00000003
#define RTC_CNTL_DBG_ATTEN_M  ((RTC_CNTL_DBG_ATTEN_V)<<(RTC_CNTL_DBG_ATTEN_S))
#define RTC_CNTL_DBG_ATTEN_V  0x3
#define RTC_CNTL_DBG_ATTEN_S  24
#define RTC_CNTL_DBG_ATTEN_DEFAULT  3
#define RTC_CNTL_REG          (DR_REG_RTCCNTL_BASE + 0x7c)
/* RTC_CNTL_FORCE_PU : R/W ;bitpos:[31] ;default: 1'd1 ; */
/*description: RTC_REG force power up*/
#define RTC_CNTL_FORCE_PU  (BIT(31))
#define RTC_CNTL_FORCE_PU_M  (BIT(31))
#define RTC_CNTL_FORCE_PU_V  0x1
#define RTC_CNTL_FORCE_PU_S  31
/* RTC_CNTL_FORCE_PD : R/W ;bitpos:[30] ;default: 1'd0 ; */
/*description: RTC_REG force power down (for RTC_REG power down means decrease
 the voltage to 0.8v or lower )*/
#define RTC_CNTL_FORCE_PD  (BIT(30))
#define RTC_CNTL_FORCE_PD_M  (BIT(30))
#define RTC_CNTL_FORCE_PD_V  0x1
#define RTC_CNTL_FORCE_PD_S  30
/* RTC_CNTL_DBOOST_FORCE_PU : R/W ;bitpos:[29] ;default: 1'd1 ; */
/*description: RTC_DBOOST force power up*/
#define RTC_CNTL_DBOOST_FORCE_PU  (BIT(29))
#define RTC_CNTL_DBOOST_FORCE_PU_M  (BIT(29))
#define RTC_CNTL_DBOOST_FORCE_PU_V  0x1
#define RTC_CNTL_DBOOST_FORCE_PU_S  29
/* RTC_CNTL_DBOOST_FORCE_PD : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: RTC_DBOOST force power down*/
#define RTC_CNTL_DBOOST_FORCE_PD  (BIT(28))
#define RTC_CNTL_DBOOST_FORCE_PD_M  (BIT(28))
#define RTC_CNTL_DBOOST_FORCE_PD_V  0x1
#define RTC_CNTL_DBOOST_FORCE_PD_S  28
/* RTC_CNTL_DBIAS_WAK : R/W ;bitpos:[27:25] ;default: 3'd4 ; */
/*description: RTC_DBIAS during wakeup*/
#define RTC_CNTL_DBIAS_WAK  0x00000007
#define RTC_CNTL_DBIAS_WAK_M  ((RTC_CNTL_DBIAS_WAK_V)<<(RTC_CNTL_DBIAS_WAK_S))
#define RTC_CNTL_DBIAS_WAK_V  0x7
#define RTC_CNTL_DBIAS_WAK_S  25
/* RTC_CNTL_DBIAS_SLP : R/W ;bitpos:[24:22] ;default: 3'd4 ; */
/*description: RTC_DBIAS during sleep*/
#define RTC_CNTL_DBIAS_SLP  0x00000007
#define RTC_CNTL_DBIAS_SLP_M  ((RTC_CNTL_DBIAS_SLP_V)<<(RTC_CNTL_DBIAS_SLP_S))
#define RTC_CNTL_DBIAS_SLP_V  0x7
#define RTC_CNTL_DBIAS_SLP_S  22
/* RTC_CNTL_SCK_DCAP : R/W ;bitpos:[21:14] ;default: 8'd0 ; */
/*description: SCK_DCAP*/
#define RTC_CNTL_SCK_DCAP  0x000000FF
#define RTC_CNTL_SCK_DCAP_M  ((RTC_CNTL_SCK_DCAP_V)<<(RTC_CNTL_SCK_DCAP_S))
#define RTC_CNTL_SCK_DCAP_V  0xFF
#define RTC_CNTL_SCK_DCAP_S  14
#define RTC_CNTL_SCK_DCAP_DEFAULT   255
/* RTC_CNTL_DIG_DBIAS_WAK : R/W ;bitpos:[13:11] ;default: 3'd4 ; */
/*description: DIG_REG_DBIAS during wakeup*/
#define RTC_CNTL_DIG_DBIAS_WAK  0x00000007
#define RTC_CNTL_DIG_DBIAS_WAK_M  ((RTC_CNTL_DIG_DBIAS_WAK_V)<<(RTC_CNTL_DIG_DBIAS_WAK_S))
#define RTC_CNTL_DIG_DBIAS_WAK_V  0x7
#define RTC_CNTL_DIG_DBIAS_WAK_S  11
/* RTC_CNTL_DIG_DBIAS_SLP : R/W ;bitpos:[10:8] ;default: 3'd4 ; */
/*description: DIG_REG_DBIAS during sleep*/
#define RTC_CNTL_DIG_DBIAS_SLP  0x00000007
#define RTC_CNTL_DIG_DBIAS_SLP_M  ((RTC_CNTL_DIG_DBIAS_SLP_V)<<(RTC_CNTL_DIG_DBIAS_SLP_S))
#define RTC_CNTL_DIG_DBIAS_SLP_V  0x7
#define RTC_CNTL_DIG_DBIAS_SLP_S  8
/* RTC_CNTL_SCK_DCAP_FORCE : R/W ;bitpos:[7] ;default: 1'd0 ; */
/*description: N/A*/
#define RTC_CNTL_SCK_DCAP_FORCE  (BIT(7))
#define RTC_CNTL_SCK_DCAP_FORCE_M  (BIT(7))
#define RTC_CNTL_SCK_DCAP_FORCE_V  0x1
#define RTC_CNTL_SCK_DCAP_FORCE_S  7

/* Approximate mapping of voltages to RTC_CNTL_DBIAS_WAK, RTC_CNTL_DBIAS_SLP,
 * RTC_CNTL_DIG_DBIAS_WAK, RTC_CNTL_DIG_DBIAS_SLP values.
 * Valid if RTC_CNTL_DBG_ATTEN is 0.
 */
#define RTC_CNTL_DBIAS_0V90 0
#define RTC_CNTL_DBIAS_0V95 1
#define RTC_CNTL_DBIAS_1V00 2
#define RTC_CNTL_DBIAS_1V05 3
#define RTC_CNTL_DBIAS_1V10 4
#define RTC_CNTL_DBIAS_1V15 5
#define RTC_CNTL_DBIAS_1V20 6
#define RTC_CNTL_DBIAS_1V25 7

#define RTC_CNTL_PWC_REG          (DR_REG_RTCCNTL_BASE + 0x80)
/* RTC_CNTL_PD_EN : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: enable power down rtc_peri in sleep*/
#define RTC_CNTL_PD_EN  (BIT(20))
#define RTC_CNTL_PD_EN_M  (BIT(20))
#define RTC_CNTL_PD_EN_V  0x1
#define RTC_CNTL_PD_EN_S  20
/* RTC_CNTL_FORCE_PU : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: rtc_peri force power up*/
#define RTC_CNTL_PWC_FORCE_PU  (BIT(19))
#define RTC_CNTL_PWC_FORCE_PU_M  (BIT(19))
#define RTC_CNTL_PWC_FORCE_PU_V  0x1
#define RTC_CNTL_PWC_FORCE_PU_S  19
/* RTC_CNTL_FORCE_PD : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: rtc_peri force power down*/
#define RTC_CNTL_PWC_FORCE_PD  (BIT(18))
#define RTC_CNTL_PWC_FORCE_PD_M  (BIT(18))
#define RTC_CNTL_PWC_FORCE_PD_V  0x1
#define RTC_CNTL_PWC_FORCE_PD_S  18
/* RTC_CNTL_SLOWMEM_PD_EN : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: enable power down RTC memory in sleep*/
#define RTC_CNTL_SLOWMEM_PD_EN  (BIT(17))
#define RTC_CNTL_SLOWMEM_PD_EN_M  (BIT(17))
#define RTC_CNTL_SLOWMEM_PD_EN_V  0x1
#define RTC_CNTL_SLOWMEM_PD_EN_S  17
/* RTC_CNTL_SLOWMEM_FORCE_PU : R/W ;bitpos:[16] ;default: 1'b1 ; */
/*description: RTC memory force power up*/
#define RTC_CNTL_SLOWMEM_FORCE_PU  (BIT(16))
#define RTC_CNTL_SLOWMEM_FORCE_PU_M  (BIT(16))
#define RTC_CNTL_SLOWMEM_FORCE_PU_V  0x1
#define RTC_CNTL_SLOWMEM_FORCE_PU_S  16
/* RTC_CNTL_SLOWMEM_FORCE_PD : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: RTC memory force power down*/
#define RTC_CNTL_SLOWMEM_FORCE_PD  (BIT(15))
#define RTC_CNTL_SLOWMEM_FORCE_PD_M  (BIT(15))
#define RTC_CNTL_SLOWMEM_FORCE_PD_V  0x1
#define RTC_CNTL_SLOWMEM_FORCE_PD_S  15
/* RTC_CNTL_FASTMEM_PD_EN : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: enable power down fast RTC memory in sleep*/
#define RTC_CNTL_FASTMEM_PD_EN  (BIT(14))
#define RTC_CNTL_FASTMEM_PD_EN_M  (BIT(14))
#define RTC_CNTL_FASTMEM_PD_EN_V  0x1
#define RTC_CNTL_FASTMEM_PD_EN_S  14
/* RTC_CNTL_FASTMEM_FORCE_PU : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: Fast RTC memory force power up*/
#define RTC_CNTL_FASTMEM_FORCE_PU  (BIT(13))
#define RTC_CNTL_FASTMEM_FORCE_PU_M  (BIT(13))
#define RTC_CNTL_FASTMEM_FORCE_PU_V  0x1
#define RTC_CNTL_FASTMEM_FORCE_PU_S  13
/* RTC_CNTL_FASTMEM_FORCE_PD : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Fast RTC memory force power down*/
#define RTC_CNTL_FASTMEM_FORCE_PD  (BIT(12))
#define RTC_CNTL_FASTMEM_FORCE_PD_M  (BIT(12))
#define RTC_CNTL_FASTMEM_FORCE_PD_V  0x1
#define RTC_CNTL_FASTMEM_FORCE_PD_S  12
/* RTC_CNTL_SLOWMEM_FORCE_LPU : R/W ;bitpos:[11] ;default: 1'b1 ; */
/*description: RTC memory force no PD*/
#define RTC_CNTL_SLOWMEM_FORCE_LPU  (BIT(11))
#define RTC_CNTL_SLOWMEM_FORCE_LPU_M  (BIT(11))
#define RTC_CNTL_SLOWMEM_FORCE_LPU_V  0x1
#define RTC_CNTL_SLOWMEM_FORCE_LPU_S  11
/* RTC_CNTL_SLOWMEM_FORCE_LPD : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: RTC memory force PD*/
#define RTC_CNTL_SLOWMEM_FORCE_LPD  (BIT(10))
#define RTC_CNTL_SLOWMEM_FORCE_LPD_M  (BIT(10))
#define RTC_CNTL_SLOWMEM_FORCE_LPD_V  0x1
#define RTC_CNTL_SLOWMEM_FORCE_LPD_S  10
/* RTC_CNTL_SLOWMEM_FOLW_CPU : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: 1: RTC memory  PD following CPU  0: RTC memory PD following RTC state machine*/
#define RTC_CNTL_SLOWMEM_FOLW_CPU  (BIT(9))
#define RTC_CNTL_SLOWMEM_FOLW_CPU_M  (BIT(9))
#define RTC_CNTL_SLOWMEM_FOLW_CPU_V  0x1
#define RTC_CNTL_SLOWMEM_FOLW_CPU_S  9
/* RTC_CNTL_FASTMEM_FORCE_LPU : R/W ;bitpos:[8] ;default: 1'b1 ; */
/*description: Fast RTC memory force no PD*/
#define RTC_CNTL_FASTMEM_FORCE_LPU  (BIT(8))
#define RTC_CNTL_FASTMEM_FORCE_LPU_M  (BIT(8))
#define RTC_CNTL_FASTMEM_FORCE_LPU_V  0x1
#define RTC_CNTL_FASTMEM_FORCE_LPU_S  8
/* RTC_CNTL_FASTMEM_FORCE_LPD : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Fast RTC memory force PD*/
#define RTC_CNTL_FASTMEM_FORCE_LPD  (BIT(7))
#define RTC_CNTL_FASTMEM_FORCE_LPD_M  (BIT(7))
#define RTC_CNTL_FASTMEM_FORCE_LPD_V  0x1
#define RTC_CNTL_FASTMEM_FORCE_LPD_S  7
/* RTC_CNTL_FASTMEM_FOLW_CPU : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: 1: Fast RTC memory PD following CPU  0: fast RTC memory PD following
 RTC state machine*/
#define RTC_CNTL_FASTMEM_FOLW_CPU  (BIT(6))
#define RTC_CNTL_FASTMEM_FOLW_CPU_M  (BIT(6))
#define RTC_CNTL_FASTMEM_FOLW_CPU_V  0x1
#define RTC_CNTL_FASTMEM_FOLW_CPU_S  6
/* RTC_CNTL_FORCE_NOISO : R/W ;bitpos:[5] ;default: 1'd1 ; */
/*description: rtc_peri force no ISO*/
#define RTC_CNTL_FORCE_NOISO  (BIT(5))
#define RTC_CNTL_FORCE_NOISO_M  (BIT(5))
#define RTC_CNTL_FORCE_NOISO_V  0x1
#define RTC_CNTL_FORCE_NOISO_S  5
/* RTC_CNTL_FORCE_ISO : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: rtc_peri force ISO*/
#define RTC_CNTL_FORCE_ISO  (BIT(4))
#define RTC_CNTL_FORCE_ISO_M  (BIT(4))
#define RTC_CNTL_FORCE_ISO_V  0x1
#define RTC_CNTL_FORCE_ISO_S  4
/* RTC_CNTL_SLOWMEM_FORCE_ISO : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: RTC memory force ISO*/
#define RTC_CNTL_SLOWMEM_FORCE_ISO  (BIT(3))
#define RTC_CNTL_SLOWMEM_FORCE_ISO_M  (BIT(3))
#define RTC_CNTL_SLOWMEM_FORCE_ISO_V  0x1
#define RTC_CNTL_SLOWMEM_FORCE_ISO_S  3
/* RTC_CNTL_SLOWMEM_FORCE_NOISO : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: RTC memory force no ISO*/
#define RTC_CNTL_SLOWMEM_FORCE_NOISO  (BIT(2))
#define RTC_CNTL_SLOWMEM_FORCE_NOISO_M  (BIT(2))
#define RTC_CNTL_SLOWMEM_FORCE_NOISO_V  0x1
#define RTC_CNTL_SLOWMEM_FORCE_NOISO_S  2
/* RTC_CNTL_FASTMEM_FORCE_ISO : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Fast RTC memory force ISO*/
#define RTC_CNTL_FASTMEM_FORCE_ISO  (BIT(1))
#define RTC_CNTL_FASTMEM_FORCE_ISO_M  (BIT(1))
#define RTC_CNTL_FASTMEM_FORCE_ISO_V  0x1
#define RTC_CNTL_FASTMEM_FORCE_ISO_S  1
/* RTC_CNTL_FASTMEM_FORCE_NOISO : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: Fast RTC memory force no ISO*/
#define RTC_CNTL_FASTMEM_FORCE_NOISO  (BIT(0))
#define RTC_CNTL_FASTMEM_FORCE_NOISO_M  (BIT(0))
#define RTC_CNTL_FASTMEM_FORCE_NOISO_V  0x1
#define RTC_CNTL_FASTMEM_FORCE_NOISO_S  0

/* Useful groups of RTC_CNTL_PWC_REG bits */
#define RTC_CNTL_MEM_FORCE_ISO    \
    (RTC_CNTL_SLOWMEM_FORCE_ISO | RTC_CNTL_FASTMEM_FORCE_ISO)
#define RTC_CNTL_MEM_FORCE_NOISO  \
    (RTC_CNTL_SLOWMEM_FORCE_NOISO | RTC_CNTL_FASTMEM_FORCE_NOISO)
#define RTC_CNTL_MEM_PD_EN        \
    (RTC_CNTL_SLOWMEM_PD_EN | RTC_CNTL_FASTMEM_PD_EN)
#define RTC_CNTL_MEM_FORCE_PU     \
    (RTC_CNTL_SLOWMEM_FORCE_PU | RTC_CNTL_FASTMEM_FORCE_PU)
#define RTC_CNTL_MEM_FORCE_PD     \
    (RTC_CNTL_SLOWMEM_FORCE_PD | RTC_CNTL_FASTMEM_FORCE_PD)
#define RTC_CNTL_MEM_FOLW_CPU     \
    (RTC_CNTL_SLOWMEM_FOLW_CPU | RTC_CNTL_FASTMEM_FOLW_CPU)
#define RTC_CNTL_MEM_FORCE_LPU    \
    (RTC_CNTL_SLOWMEM_FORCE_LPU | RTC_CNTL_FASTMEM_FORCE_LPU)
#define RTC_CNTL_MEM_FORCE_LPD    \
    (RTC_CNTL_SLOWMEM_FORCE_LPD | RTC_CNTL_FASTMEM_FORCE_LPD)

#define RTC_CNTL_DIG_PWC_REG          (DR_REG_RTCCNTL_BASE + 0x84)
/* RTC_CNTL_DG_WRAP_PD_EN : R/W ;bitpos:[31] ;default: 0 ; */
/*description: enable power down digital core in sleep*/
#define RTC_CNTL_DG_WRAP_PD_EN  (BIT(31))
#define RTC_CNTL_DG_WRAP_PD_EN_M  (BIT(31))
#define RTC_CNTL_DG_WRAP_PD_EN_V  0x1
#define RTC_CNTL_DG_WRAP_PD_EN_S  31
/* RTC_CNTL_WIFI_PD_EN : R/W ;bitpos:[30] ;default: 0 ; */
/*description: enable power down wifi in sleep*/
#define RTC_CNTL_WIFI_PD_EN  (BIT(30))
#define RTC_CNTL_WIFI_PD_EN_M  (BIT(30))
#define RTC_CNTL_WIFI_PD_EN_V  0x1
#define RTC_CNTL_WIFI_PD_EN_S  30
/* RTC_CNTL_INTER_RAM4_PD_EN : R/W ;bitpos:[29] ;default: 0 ; */
/*description: enable power down internal SRAM 4 in sleep*/
#define RTC_CNTL_INTER_RAM4_PD_EN  (BIT(29))
#define RTC_CNTL_INTER_RAM4_PD_EN_M  (BIT(29))
#define RTC_CNTL_INTER_RAM4_PD_EN_V  0x1
#define RTC_CNTL_INTER_RAM4_PD_EN_S  29
/* RTC_CNTL_INTER_RAM3_PD_EN : R/W ;bitpos:[28] ;default: 0 ; */
/*description: enable power down internal SRAM 3 in sleep*/
#define RTC_CNTL_INTER_RAM3_PD_EN  (BIT(28))
#define RTC_CNTL_INTER_RAM3_PD_EN_M  (BIT(28))
#define RTC_CNTL_INTER_RAM3_PD_EN_V  0x1
#define RTC_CNTL_INTER_RAM3_PD_EN_S  28
/* RTC_CNTL_INTER_RAM2_PD_EN : R/W ;bitpos:[27] ;default: 0 ; */
/*description: enable power down internal SRAM 2 in sleep*/
#define RTC_CNTL_INTER_RAM2_PD_EN  (BIT(27))
#define RTC_CNTL_INTER_RAM2_PD_EN_M  (BIT(27))
#define RTC_CNTL_INTER_RAM2_PD_EN_V  0x1
#define RTC_CNTL_INTER_RAM2_PD_EN_S  27
/* RTC_CNTL_INTER_RAM1_PD_EN : R/W ;bitpos:[26] ;default: 0 ; */
/*description: enable power down internal SRAM 1 in sleep*/
#define RTC_CNTL_INTER_RAM1_PD_EN  (BIT(26))
#define RTC_CNTL_INTER_RAM1_PD_EN_M  (BIT(26))
#define RTC_CNTL_INTER_RAM1_PD_EN_V  0x1
#define RTC_CNTL_INTER_RAM1_PD_EN_S  26
/* RTC_CNTL_INTER_RAM0_PD_EN : R/W ;bitpos:[25] ;default: 0 ; */
/*description: enable power down internal SRAM 0 in sleep*/
#define RTC_CNTL_INTER_RAM0_PD_EN  (BIT(25))
#define RTC_CNTL_INTER_RAM0_PD_EN_M  (BIT(25))
#define RTC_CNTL_INTER_RAM0_PD_EN_V  0x1
#define RTC_CNTL_INTER_RAM0_PD_EN_S  25
/* RTC_CNTL_ROM0_PD_EN : R/W ;bitpos:[24] ;default: 0 ; */
/*description: enable power down ROM in sleep*/
#define RTC_CNTL_ROM0_PD_EN  (BIT(24))
#define RTC_CNTL_ROM0_PD_EN_M  (BIT(24))
#define RTC_CNTL_ROM0_PD_EN_V  0x1
#define RTC_CNTL_ROM0_PD_EN_S  24
/* RTC_CNTL_DG_WRAP_FORCE_PU : R/W ;bitpos:[20] ;default: 1'd1 ; */
/*description: digital core force power up*/
#define RTC_CNTL_DG_WRAP_FORCE_PU  (BIT(20))
#define RTC_CNTL_DG_WRAP_FORCE_PU_M  (BIT(20))
#define RTC_CNTL_DG_WRAP_FORCE_PU_V  0x1
#define RTC_CNTL_DG_WRAP_FORCE_PU_S  20
/* RTC_CNTL_DG_WRAP_FORCE_PD : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: digital core force power down*/
#define RTC_CNTL_DG_WRAP_FORCE_PD  (BIT(19))
#define RTC_CNTL_DG_WRAP_FORCE_PD_M  (BIT(19))
#define RTC_CNTL_DG_WRAP_FORCE_PD_V  0x1
#define RTC_CNTL_DG_WRAP_FORCE_PD_S  19
/* RTC_CNTL_WIFI_FORCE_PU : R/W ;bitpos:[18] ;default: 1'd1 ; */
/*description: wifi force power up*/
#define RTC_CNTL_WIFI_FORCE_PU  (BIT(18))
#define RTC_CNTL_WIFI_FORCE_PU_M  (BIT(18))
#define RTC_CNTL_WIFI_FORCE_PU_V  0x1
#define RTC_CNTL_WIFI_FORCE_PU_S  18
/* RTC_CNTL_WIFI_FORCE_PD : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: wifi force power down*/
#define RTC_CNTL_WIFI_FORCE_PD  (BIT(17))
#define RTC_CNTL_WIFI_FORCE_PD_M  (BIT(17))
#define RTC_CNTL_WIFI_FORCE_PD_V  0x1
#define RTC_CNTL_WIFI_FORCE_PD_S  17
/* RTC_CNTL_INTER_RAM4_FORCE_PU : R/W ;bitpos:[16] ;default: 1'd1 ; */
/*description: internal SRAM 4 force power up*/
#define RTC_CNTL_INTER_RAM4_FORCE_PU  (BIT(16))
#define RTC_CNTL_INTER_RAM4_FORCE_PU_M  (BIT(16))
#define RTC_CNTL_INTER_RAM4_FORCE_PU_V  0x1
#define RTC_CNTL_INTER_RAM4_FORCE_PU_S  16
/* RTC_CNTL_INTER_RAM4_FORCE_PD : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: internal SRAM 4 force power down*/
#define RTC_CNTL_INTER_RAM4_FORCE_PD  (BIT(15))
#define RTC_CNTL_INTER_RAM4_FORCE_PD_M  (BIT(15))
#define RTC_CNTL_INTER_RAM4_FORCE_PD_V  0x1
#define RTC_CNTL_INTER_RAM4_FORCE_PD_S  15
/* RTC_CNTL_INTER_RAM3_FORCE_PU : R/W ;bitpos:[14] ;default: 1'd1 ; */
/*description: internal SRAM 3 force power up*/
#define RTC_CNTL_INTER_RAM3_FORCE_PU  (BIT(14))
#define RTC_CNTL_INTER_RAM3_FORCE_PU_M  (BIT(14))
#define RTC_CNTL_INTER_RAM3_FORCE_PU_V  0x1
#define RTC_CNTL_INTER_RAM3_FORCE_PU_S  14
/* RTC_CNTL_INTER_RAM3_FORCE_PD : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: internal SRAM 3 force power down*/
#define RTC_CNTL_INTER_RAM3_FORCE_PD  (BIT(13))
#define RTC_CNTL_INTER_RAM3_FORCE_PD_M  (BIT(13))
#define RTC_CNTL_INTER_RAM3_FORCE_PD_V  0x1
#define RTC_CNTL_INTER_RAM3_FORCE_PD_S  13
/* RTC_CNTL_INTER_RAM2_FORCE_PU : R/W ;bitpos:[12] ;default: 1'd1 ; */
/*description: internal SRAM 2 force power up*/
#define RTC_CNTL_INTER_RAM2_FORCE_PU  (BIT(12))
#define RTC_CNTL_INTER_RAM2_FORCE_PU_M  (BIT(12))
#define RTC_CNTL_INTER_RAM2_FORCE_PU_V  0x1
#define RTC_CNTL_INTER_RAM2_FORCE_PU_S  12
/* RTC_CNTL_INTER_RAM2_FORCE_PD : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: internal SRAM 2 force power down*/
#define RTC_CNTL_INTER_RAM2_FORCE_PD  (BIT(11))
#define RTC_CNTL_INTER_RAM2_FORCE_PD_M  (BIT(11))
#define RTC_CNTL_INTER_RAM2_FORCE_PD_V  0x1
#define RTC_CNTL_INTER_RAM2_FORCE_PD_S  11
/* RTC_CNTL_INTER_RAM1_FORCE_PU : R/W ;bitpos:[10] ;default: 1'd1 ; */
/*description: internal SRAM 1 force power up*/
#define RTC_CNTL_INTER_RAM1_FORCE_PU  (BIT(10))
#define RTC_CNTL_INTER_RAM1_FORCE_PU_M  (BIT(10))
#define RTC_CNTL_INTER_RAM1_FORCE_PU_V  0x1
#define RTC_CNTL_INTER_RAM1_FORCE_PU_S  10
/* RTC_CNTL_INTER_RAM1_FORCE_PD : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: internal SRAM 1 force power down*/
#define RTC_CNTL_INTER_RAM1_FORCE_PD  (BIT(9))
#define RTC_CNTL_INTER_RAM1_FORCE_PD_M  (BIT(9))
#define RTC_CNTL_INTER_RAM1_FORCE_PD_V  0x1
#define RTC_CNTL_INTER_RAM1_FORCE_PD_S  9
/* RTC_CNTL_INTER_RAM0_FORCE_PU : R/W ;bitpos:[8] ;default: 1'd1 ; */
/*description: internal SRAM 0 force power up*/
#define RTC_CNTL_INTER_RAM0_FORCE_PU  (BIT(8))
#define RTC_CNTL_INTER_RAM0_FORCE_PU_M  (BIT(8))
#define RTC_CNTL_INTER_RAM0_FORCE_PU_V  0x1
#define RTC_CNTL_INTER_RAM0_FORCE_PU_S  8
/* RTC_CNTL_INTER_RAM0_FORCE_PD : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: internal SRAM 0 force power down*/
#define RTC_CNTL_INTER_RAM0_FORCE_PD  (BIT(7))
#define RTC_CNTL_INTER_RAM0_FORCE_PD_M  (BIT(7))
#define RTC_CNTL_INTER_RAM0_FORCE_PD_V  0x1
#define RTC_CNTL_INTER_RAM0_FORCE_PD_S  7
/* RTC_CNTL_ROM0_FORCE_PU : R/W ;bitpos:[6] ;default: 1'd1 ; */
/*description: ROM force power up*/
#define RTC_CNTL_ROM0_FORCE_PU  (BIT(6))
#define RTC_CNTL_ROM0_FORCE_PU_M  (BIT(6))
#define RTC_CNTL_ROM0_FORCE_PU_V  0x1
#define RTC_CNTL_ROM0_FORCE_PU_S  6
/* RTC_CNTL_ROM0_FORCE_PD : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: ROM force power down*/
#define RTC_CNTL_ROM0_FORCE_PD  (BIT(5))
#define RTC_CNTL_ROM0_FORCE_PD_M  (BIT(5))
#define RTC_CNTL_ROM0_FORCE_PD_V  0x1
#define RTC_CNTL_ROM0_FORCE_PD_S  5
/* RTC_CNTL_LSLP_MEM_FORCE_PU : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: memories in digital core force no PD in sleep*/
#define RTC_CNTL_LSLP_MEM_FORCE_PU  (BIT(4))
#define RTC_CNTL_LSLP_MEM_FORCE_PU_M  (BIT(4))
#define RTC_CNTL_LSLP_MEM_FORCE_PU_V  0x1
#define RTC_CNTL_LSLP_MEM_FORCE_PU_S  4
/* RTC_CNTL_LSLP_MEM_FORCE_PD : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: memories in digital core force PD in sleep*/
#define RTC_CNTL_LSLP_MEM_FORCE_PD  (BIT(3))
#define RTC_CNTL_LSLP_MEM_FORCE_PD_M  (BIT(3))
#define RTC_CNTL_LSLP_MEM_FORCE_PD_V  0x1
#define RTC_CNTL_LSLP_MEM_FORCE_PD_S  3

/* Useful groups of RTC_CNTL_DIG_PWC_REG bits */
#define RTC_CNTL_CPU_ROM_RAM_PD_EN \
    (RTC_CNTL_INTER_RAM4_PD_EN | RTC_CNTL_INTER_RAM3_PD_EN |\
     RTC_CNTL_INTER_RAM2_PD_EN | RTC_CNTL_INTER_RAM1_PD_EN |\
     RTC_CNTL_INTER_RAM0_PD_EN | RTC_CNTL_ROM0_PD_EN)
#define RTC_CNTL_CPU_ROM_RAM_FORCE_PU \
    (RTC_CNTL_INTER_RAM4_FORCE_PU | RTC_CNTL_INTER_RAM3_FORCE_PU |\
     RTC_CNTL_INTER_RAM2_FORCE_PU | RTC_CNTL_INTER_RAM1_FORCE_PU |\
     RTC_CNTL_INTER_RAM0_FORCE_PU | RTC_CNTL_ROM0_FORCE_PU)
#define RTC_CNTL_CPU_ROM_RAM_FORCE_PD \
    (RTC_CNTL_INTER_RAM4_FORCE_PD | RTC_CNTL_INTER_RAM3_FORCE_PD |\
     RTC_CNTL_INTER_RAM2_FORCE_PD | RTC_CNTL_INTER_RAM1_FORCE_PD |\
     RTC_CNTL_INTER_RAM0_FORCE_PD | RTC_CNTL_ROM0_FORCE_PD

#define RTC_CNTL_DIG_ISO_REG          (DR_REG_RTCCNTL_BASE + 0x88)
/* RTC_CNTL_DG_WRAP_FORCE_NOISO : R/W ;bitpos:[31] ;default: 1'd1 ; */
/*description: digital core force no ISO*/
#define RTC_CNTL_DG_WRAP_FORCE_NOISO  (BIT(31))
#define RTC_CNTL_DG_WRAP_FORCE_NOISO_M  (BIT(31))
#define RTC_CNTL_DG_WRAP_FORCE_NOISO_V  0x1
#define RTC_CNTL_DG_WRAP_FORCE_NOISO_S  31
/* RTC_CNTL_DG_WRAP_FORCE_ISO : R/W ;bitpos:[30] ;default: 1'd0 ; */
/*description: digital core force ISO*/
#define RTC_CNTL_DG_WRAP_FORCE_ISO  (BIT(30))
#define RTC_CNTL_DG_WRAP_FORCE_ISO_M  (BIT(30))
#define RTC_CNTL_DG_WRAP_FORCE_ISO_V  0x1
#define RTC_CNTL_DG_WRAP_FORCE_ISO_S  30
/* RTC_CNTL_WIFI_FORCE_NOISO : R/W ;bitpos:[29] ;default: 1'd1 ; */
/*description: wifi force no ISO*/
#define RTC_CNTL_WIFI_FORCE_NOISO  (BIT(29))
#define RTC_CNTL_WIFI_FORCE_NOISO_M  (BIT(29))
#define RTC_CNTL_WIFI_FORCE_NOISO_V  0x1
#define RTC_CNTL_WIFI_FORCE_NOISO_S  29
/* RTC_CNTL_WIFI_FORCE_ISO : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: wifi force ISO*/
#define RTC_CNTL_WIFI_FORCE_ISO  (BIT(28))
#define RTC_CNTL_WIFI_FORCE_ISO_M  (BIT(28))
#define RTC_CNTL_WIFI_FORCE_ISO_V  0x1
#define RTC_CNTL_WIFI_FORCE_ISO_S  28
/* RTC_CNTL_INTER_RAM4_FORCE_NOISO : R/W ;bitpos:[27] ;default: 1'd1 ; */
/*description: internal SRAM 4 force no ISO*/
#define RTC_CNTL_INTER_RAM4_FORCE_NOISO  (BIT(27))
#define RTC_CNTL_INTER_RAM4_FORCE_NOISO_M  (BIT(27))
#define RTC_CNTL_INTER_RAM4_FORCE_NOISO_V  0x1
#define RTC_CNTL_INTER_RAM4_FORCE_NOISO_S  27
/* RTC_CNTL_INTER_RAM4_FORCE_ISO : R/W ;bitpos:[26] ;default: 1'd0 ; */
/*description: internal SRAM 4 force ISO*/
#define RTC_CNTL_INTER_RAM4_FORCE_ISO  (BIT(26))
#define RTC_CNTL_INTER_RAM4_FORCE_ISO_M  (BIT(26))
#define RTC_CNTL_INTER_RAM4_FORCE_ISO_V  0x1
#define RTC_CNTL_INTER_RAM4_FORCE_ISO_S  26
/* RTC_CNTL_INTER_RAM3_FORCE_NOISO : R/W ;bitpos:[25] ;default: 1'd1 ; */
/*description: internal SRAM 3 force no ISO*/
#define RTC_CNTL_INTER_RAM3_FORCE_NOISO  (BIT(25))
#define RTC_CNTL_INTER_RAM3_FORCE_NOISO_M  (BIT(25))
#define RTC_CNTL_INTER_RAM3_FORCE_NOISO_V  0x1
#define RTC_CNTL_INTER_RAM3_FORCE_NOISO_S  25
/* RTC_CNTL_INTER_RAM3_FORCE_ISO : R/W ;bitpos:[24] ;default: 1'd0 ; */
/*description: internal SRAM 3 force ISO*/
#define RTC_CNTL_INTER_RAM3_FORCE_ISO  (BIT(24))
#define RTC_CNTL_INTER_RAM3_FORCE_ISO_M  (BIT(24))
#define RTC_CNTL_INTER_RAM3_FORCE_ISO_V  0x1
#define RTC_CNTL_INTER_RAM3_FORCE_ISO_S  24
/* RTC_CNTL_INTER_RAM2_FORCE_NOISO : R/W ;bitpos:[23] ;default: 1'd1 ; */
/*description: internal SRAM 2 force no ISO*/
#define RTC_CNTL_INTER_RAM2_FORCE_NOISO  (BIT(23))
#define RTC_CNTL_INTER_RAM2_FORCE_NOISO_M  (BIT(23))
#define RTC_CNTL_INTER_RAM2_FORCE_NOISO_V  0x1
#define RTC_CNTL_INTER_RAM2_FORCE_NOISO_S  23
/* RTC_CNTL_INTER_RAM2_FORCE_ISO : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: internal SRAM 2 force ISO*/
#define RTC_CNTL_INTER_RAM2_FORCE_ISO  (BIT(22))
#define RTC_CNTL_INTER_RAM2_FORCE_ISO_M  (BIT(22))
#define RTC_CNTL_INTER_RAM2_FORCE_ISO_V  0x1
#define RTC_CNTL_INTER_RAM2_FORCE_ISO_S  22
/* RTC_CNTL_INTER_RAM1_FORCE_NOISO : R/W ;bitpos:[21] ;default: 1'd1 ; */
/*description: internal SRAM 1 force no ISO*/
#define RTC_CNTL_INTER_RAM1_FORCE_NOISO  (BIT(21))
#define RTC_CNTL_INTER_RAM1_FORCE_NOISO_M  (BIT(21))
#define RTC_CNTL_INTER_RAM1_FORCE_NOISO_V  0x1
#define RTC_CNTL_INTER_RAM1_FORCE_NOISO_S  21
/* RTC_CNTL_INTER_RAM1_FORCE_ISO : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: internal SRAM 1 force ISO*/
#define RTC_CNTL_INTER_RAM1_FORCE_ISO  (BIT(20))
#define RTC_CNTL_INTER_RAM1_FORCE_ISO_M  (BIT(20))
#define RTC_CNTL_INTER_RAM1_FORCE_ISO_V  0x1
#define RTC_CNTL_INTER_RAM1_FORCE_ISO_S  20
/* RTC_CNTL_INTER_RAM0_FORCE_NOISO : R/W ;bitpos:[19] ;default: 1'd1 ; */
/*description: internal SRAM 0 force no ISO*/
#define RTC_CNTL_INTER_RAM0_FORCE_NOISO  (BIT(19))
#define RTC_CNTL_INTER_RAM0_FORCE_NOISO_M  (BIT(19))
#define RTC_CNTL_INTER_RAM0_FORCE_NOISO_V  0x1
#define RTC_CNTL_INTER_RAM0_FORCE_NOISO_S  19
/* RTC_CNTL_INTER_RAM0_FORCE_ISO : R/W ;bitpos:[18] ;default: 1'd0 ; */
/*description: internal SRAM 0 force ISO*/
#define RTC_CNTL_INTER_RAM0_FORCE_ISO  (BIT(18))
#define RTC_CNTL_INTER_RAM0_FORCE_ISO_M  (BIT(18))
#define RTC_CNTL_INTER_RAM0_FORCE_ISO_V  0x1
#define RTC_CNTL_INTER_RAM0_FORCE_ISO_S  18
/* RTC_CNTL_ROM0_FORCE_NOISO : R/W ;bitpos:[17] ;default: 1'd1 ; */
/*description: ROM force no ISO*/
#define RTC_CNTL_ROM0_FORCE_NOISO  (BIT(17))
#define RTC_CNTL_ROM0_FORCE_NOISO_M  (BIT(17))
#define RTC_CNTL_ROM0_FORCE_NOISO_V  0x1
#define RTC_CNTL_ROM0_FORCE_NOISO_S  17
/* RTC_CNTL_ROM0_FORCE_ISO : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: ROM force ISO*/
#define RTC_CNTL_ROM0_FORCE_ISO  (BIT(16))
#define RTC_CNTL_ROM0_FORCE_ISO_M  (BIT(16))
#define RTC_CNTL_ROM0_FORCE_ISO_V  0x1
#define RTC_CNTL_ROM0_FORCE_ISO_S  16
/* RTC_CNTL_DG_PAD_FORCE_HOLD : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: digital pad force hold*/
#define RTC_CNTL_DG_PAD_FORCE_HOLD  (BIT(15))
#define RTC_CNTL_DG_PAD_FORCE_HOLD_M  (BIT(15))
#define RTC_CNTL_DG_PAD_FORCE_HOLD_V  0x1
#define RTC_CNTL_DG_PAD_FORCE_HOLD_S  15
/* RTC_CNTL_DG_PAD_FORCE_UNHOLD : R/W ;bitpos:[14] ;default: 1'd1 ; */
/*description: digital pad force un-hold*/
#define RTC_CNTL_DG_PAD_FORCE_UNHOLD  (BIT(14))
#define RTC_CNTL_DG_PAD_FORCE_UNHOLD_M  (BIT(14))
#define RTC_CNTL_DG_PAD_FORCE_UNHOLD_V  0x1
#define RTC_CNTL_DG_PAD_FORCE_UNHOLD_S  14
/* RTC_CNTL_DG_PAD_FORCE_ISO : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: digital pad force ISO*/
#define RTC_CNTL_DG_PAD_FORCE_ISO  (BIT(13))
#define RTC_CNTL_DG_PAD_FORCE_ISO_M  (BIT(13))
#define RTC_CNTL_DG_PAD_FORCE_ISO_V  0x1
#define RTC_CNTL_DG_PAD_FORCE_ISO_S  13
/* RTC_CNTL_DG_PAD_FORCE_NOISO : R/W ;bitpos:[12] ;default: 1'd1 ; */
/*description: digital pad force no ISO*/
#define RTC_CNTL_DG_PAD_FORCE_NOISO  (BIT(12))
#define RTC_CNTL_DG_PAD_FORCE_NOISO_M  (BIT(12))
#define RTC_CNTL_DG_PAD_FORCE_NOISO_V  0x1
#define RTC_CNTL_DG_PAD_FORCE_NOISO_S  12
/* RTC_CNTL_DG_PAD_AUTOHOLD_EN : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: digital pad enable auto-hold*/
#define RTC_CNTL_DG_PAD_AUTOHOLD_EN  (BIT(11))
#define RTC_CNTL_DG_PAD_AUTOHOLD_EN_M  (BIT(11))
#define RTC_CNTL_DG_PAD_AUTOHOLD_EN_V  0x1
#define RTC_CNTL_DG_PAD_AUTOHOLD_EN_S  11
/* RTC_CNTL_CLR_DG_PAD_AUTOHOLD : WO ;bitpos:[10] ;default: 1'd0 ; */
/*description: wtite only register to clear digital pad auto-hold*/
#define RTC_CNTL_CLR_DG_PAD_AUTOHOLD  (BIT(10))
#define RTC_CNTL_CLR_DG_PAD_AUTOHOLD_M  (BIT(10))
#define RTC_CNTL_CLR_DG_PAD_AUTOHOLD_V  0x1
#define RTC_CNTL_CLR_DG_PAD_AUTOHOLD_S  10
/* RTC_CNTL_DG_PAD_AUTOHOLD : RO ;bitpos:[9] ;default: 1'd0 ; */
/*description: read only register to indicate digital pad auto-hold status*/
#define RTC_CNTL_DG_PAD_AUTOHOLD  (BIT(9))
#define RTC_CNTL_DG_PAD_AUTOHOLD_M  (BIT(9))
#define RTC_CNTL_DG_PAD_AUTOHOLD_V  0x1
#define RTC_CNTL_DG_PAD_AUTOHOLD_S  9
/* RTC_CNTL_DIG_ISO_FORCE_ON : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: */
#define RTC_CNTL_DIG_ISO_FORCE_ON  (BIT(8))
#define RTC_CNTL_DIG_ISO_FORCE_ON_M  (BIT(8))
#define RTC_CNTL_DIG_ISO_FORCE_ON_V  0x1
#define RTC_CNTL_DIG_ISO_FORCE_ON_S  8
/* RTC_CNTL_DIG_ISO_FORCE_OFF : R/W ;bitpos:[7] ;default: 1'd0 ; */
/*description: */
#define RTC_CNTL_DIG_ISO_FORCE_OFF  (BIT(7))
#define RTC_CNTL_DIG_ISO_FORCE_OFF_M  (BIT(7))
#define RTC_CNTL_DIG_ISO_FORCE_OFF_V  0x1
#define RTC_CNTL_DIG_ISO_FORCE_OFF_S  7

/* Useful groups of RTC_CNTL_DIG_ISO_REG bits */
#define RTC_CNTL_CPU_ROM_RAM_FORCE_ISO   \
    (RTC_CNTL_INTER_RAM4_FORCE_ISO | RTC_CNTL_INTER_RAM3_FORCE_ISO |\
     RTC_CNTL_INTER_RAM2_FORCE_ISO | RTC_CNTL_INTER_RAM1_FORCE_ISO |\
     RTC_CNTL_INTER_RAM0_FORCE_ISO | RTC_CNTL_ROM0_FORCE_ISO)
#define RTC_CNTL_CPU_ROM_RAM_FORCE_NOISO \
    (RTC_CNTL_INTER_RAM4_FORCE_NOISO | RTC_CNTL_INTER_RAM3_FORCE_NOISO |\
     RTC_CNTL_INTER_RAM2_FORCE_NOISO | RTC_CNTL_INTER_RAM1_FORCE_NOISO |\
     RTC_CNTL_INTER_RAM0_FORCE_NOISO | RTC_CNTL_ROM0_FORCE_NOISO)

#define RTC_CNTL_WDTCONFIG0_REG          (DR_REG_RTCCNTL_BASE + 0x8c)
/* RTC_CNTL_WDT_EN : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: enable RTC WDT*/
#define RTC_CNTL_WDT_EN  (BIT(31))
#define RTC_CNTL_WDT_EN_M  (BIT(31))
#define RTC_CNTL_WDT_EN_V  0x1
#define RTC_CNTL_WDT_EN_S  31
/* RTC_CNTL_WDT_STG0 : R/W ;bitpos:[30:28] ;default: 3'h0 ; */
/*description: 1: interrupt stage en  2: CPU reset stage en  3: system reset
 stage en  4: RTC reset stage en*/
#define RTC_CNTL_WDT_STG0  0x00000007
#define RTC_CNTL_WDT_STG0_M  ((RTC_CNTL_WDT_STG0_V)<<(RTC_CNTL_WDT_STG0_S))
#define RTC_CNTL_WDT_STG0_V  0x7
#define RTC_CNTL_WDT_STG0_S  28
/* RTC_CNTL_WDT_STG1 : R/W ;bitpos:[27:25] ;default: 3'h0 ; */
/*description: 1: interrupt stage en  2: CPU reset stage en  3: system reset
 stage en  4: RTC reset stage en*/
#define RTC_CNTL_WDT_STG1  0x00000007
#define RTC_CNTL_WDT_STG1_M  ((RTC_CNTL_WDT_STG1_V)<<(RTC_CNTL_WDT_STG1_S))
#define RTC_CNTL_WDT_STG1_V  0x7
#define RTC_CNTL_WDT_STG1_S  25
/* RTC_CNTL_WDT_STG2 : R/W ;bitpos:[24:22] ;default: 3'h0 ; */
/*description: 1: interrupt stage en  2: CPU reset stage en  3: system reset
 stage en  4: RTC reset stage en*/
#define RTC_CNTL_WDT_STG2  0x00000007
#define RTC_CNTL_WDT_STG2_M  ((RTC_CNTL_WDT_STG2_V)<<(RTC_CNTL_WDT_STG2_S))
#define RTC_CNTL_WDT_STG2_V  0x7
#define RTC_CNTL_WDT_STG2_S  22
/* RTC_CNTL_WDT_STG3 : R/W ;bitpos:[21:19] ;default: 3'h0 ; */
/*description: 1: interrupt stage en  2: CPU reset stage en  3: system reset
 stage en  4: RTC reset stage en*/
#define RTC_CNTL_WDT_STG3  0x00000007
#define RTC_CNTL_WDT_STG3_M  ((RTC_CNTL_WDT_STG3_V)<<(RTC_CNTL_WDT_STG3_S))
#define RTC_CNTL_WDT_STG3_V  0x7
#define RTC_CNTL_WDT_STG3_S  19
/* RTC_CNTL_WDT_EDGE_INT_EN : R/W ;bitpos:[18] ;default: 1'h0 ; */
/*description: N/A*/
#define RTC_CNTL_WDT_EDGE_INT_EN  (BIT(18))
#define RTC_CNTL_WDT_EDGE_INT_EN_M  (BIT(18))
#define RTC_CNTL_WDT_EDGE_INT_EN_V  0x1
#define RTC_CNTL_WDT_EDGE_INT_EN_S  18
/* RTC_CNTL_WDT_LEVEL_INT_EN : R/W ;bitpos:[17] ;default: 1'h0 ; */
/*description: N/A*/
#define RTC_CNTL_WDT_LEVEL_INT_EN  (BIT(17))
#define RTC_CNTL_WDT_LEVEL_INT_EN_M  (BIT(17))
#define RTC_CNTL_WDT_LEVEL_INT_EN_V  0x1
#define RTC_CNTL_WDT_LEVEL_INT_EN_S  17
/* RTC_CNTL_WDT_CPU_RESET_LENGTH : R/W ;bitpos:[16:14] ;default: 3'h1 ; */
/*description: CPU reset counter length*/
#define RTC_CNTL_WDT_CPU_RESET_LENGTH  0x00000007
#define RTC_CNTL_WDT_CPU_RESET_LENGTH_M  ((RTC_CNTL_WDT_CPU_RESET_LENGTH_V)<<(RTC_CNTL_WDT_CPU_RESET_LENGTH_S))
#define RTC_CNTL_WDT_CPU_RESET_LENGTH_V  0x7
#define RTC_CNTL_WDT_CPU_RESET_LENGTH_S  14
/* RTC_CNTL_WDT_SYS_RESET_LENGTH : R/W ;bitpos:[13:11] ;default: 3'h1 ; */
/*description: system reset counter length*/
#define RTC_CNTL_WDT_SYS_RESET_LENGTH  0x00000007
#define RTC_CNTL_WDT_SYS_RESET_LENGTH_M  ((RTC_CNTL_WDT_SYS_RESET_LENGTH_V)<<(RTC_CNTL_WDT_SYS_RESET_LENGTH_S))
#define RTC_CNTL_WDT_SYS_RESET_LENGTH_V  0x7
#define RTC_CNTL_WDT_SYS_RESET_LENGTH_S  11
/* RTC_CNTL_WDT_FLASHBOOT_MOD_EN : R/W ;bitpos:[10] ;default: 1'h1 ; */
/*description: enable WDT in flash boot*/
#define RTC_CNTL_WDT_FLASHBOOT_MOD_EN  (BIT(10))
#define RTC_CNTL_WDT_FLASHBOOT_MOD_EN_M  (BIT(10))
#define RTC_CNTL_WDT_FLASHBOOT_MOD_EN_V  0x1
#define RTC_CNTL_WDT_FLASHBOOT_MOD_EN_S  10
/* RTC_CNTL_WDT_PROCPU_RESET_EN : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: enable WDT reset PRO CPU*/
#define RTC_CNTL_WDT_PROCPU_RESET_EN  (BIT(9))
#define RTC_CNTL_WDT_PROCPU_RESET_EN_M  (BIT(9))
#define RTC_CNTL_WDT_PROCPU_RESET_EN_V  0x1
#define RTC_CNTL_WDT_PROCPU_RESET_EN_S  9
/* RTC_CNTL_WDT_APPCPU_RESET_EN : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: enable WDT reset APP CPU*/
#define RTC_CNTL_WDT_APPCPU_RESET_EN  (BIT(8))
#define RTC_CNTL_WDT_APPCPU_RESET_EN_M  (BIT(8))
#define RTC_CNTL_WDT_APPCPU_RESET_EN_V  0x1
#define RTC_CNTL_WDT_APPCPU_RESET_EN_S  8
/* RTC_CNTL_WDT_PAUSE_IN_SLP : R/W ;bitpos:[7] ;default: 1'd1 ; */
/*description: pause WDT in sleep*/
#define RTC_CNTL_WDT_PAUSE_IN_SLP  (BIT(7))
#define RTC_CNTL_WDT_PAUSE_IN_SLP_M  (BIT(7))
#define RTC_CNTL_WDT_PAUSE_IN_SLP_V  0x1
#define RTC_CNTL_WDT_PAUSE_IN_SLP_S  7
/* RTC_CNTL_WDT_STGX : */
/*description: stage action selection values */
#define RTC_WDT_STG_SEL_OFF             0
#define RTC_WDT_STG_SEL_INT             1
#define RTC_WDT_STG_SEL_RESET_CPU       2
#define RTC_WDT_STG_SEL_RESET_SYSTEM    3
#define RTC_WDT_STG_SEL_RESET_RTC       4

#define RTC_CNTL_WDTCONFIG1_REG          (DR_REG_RTCCNTL_BASE + 0x90)
/* RTC_CNTL_WDT_STG0_HOLD : R/W ;bitpos:[31:0] ;default: 32'd128000 ; */
/*description: */
#define RTC_CNTL_WDT_STG0_HOLD  0xFFFFFFFF
#define RTC_CNTL_WDT_STG0_HOLD_M  ((RTC_CNTL_WDT_STG0_HOLD_V)<<(RTC_CNTL_WDT_STG0_HOLD_S))
#define RTC_CNTL_WDT_STG0_HOLD_V  0xFFFFFFFF
#define RTC_CNTL_WDT_STG0_HOLD_S  0

#define RTC_CNTL_WDTCONFIG2_REG          (DR_REG_RTCCNTL_BASE + 0x94)
/* RTC_CNTL_WDT_STG1_HOLD : R/W ;bitpos:[31:0] ;default: 32'd80000 ; */
/*description: */
#define RTC_CNTL_WDT_STG1_HOLD  0xFFFFFFFF
#define RTC_CNTL_WDT_STG1_HOLD_M  ((RTC_CNTL_WDT_STG1_HOLD_V)<<(RTC_CNTL_WDT_STG1_HOLD_S))
#define RTC_CNTL_WDT_STG1_HOLD_V  0xFFFFFFFF
#define RTC_CNTL_WDT_STG1_HOLD_S  0

#define RTC_CNTL_WDTCONFIG3_REG          (DR_REG_RTCCNTL_BASE + 0x98)
/* RTC_CNTL_WDT_STG2_HOLD : R/W ;bitpos:[31:0] ;default: 32'hfff ; */
/*description: */
#define RTC_CNTL_WDT_STG2_HOLD  0xFFFFFFFF
#define RTC_CNTL_WDT_STG2_HOLD_M  ((RTC_CNTL_WDT_STG2_HOLD_V)<<(RTC_CNTL_WDT_STG2_HOLD_S))
#define RTC_CNTL_WDT_STG2_HOLD_V  0xFFFFFFFF
#define RTC_CNTL_WDT_STG2_HOLD_S  0

#define RTC_CNTL_WDTCONFIG4_REG          (DR_REG_RTCCNTL_BASE + 0x9c)
/* RTC_CNTL_WDT_STG3_HOLD : R/W ;bitpos:[31:0] ;default: 32'hfff ; */
/*description: */
#define RTC_CNTL_WDT_STG3_HOLD  0xFFFFFFFF
#define RTC_CNTL_WDT_STG3_HOLD_M  ((RTC_CNTL_WDT_STG3_HOLD_V)<<(RTC_CNTL_WDT_STG3_HOLD_S))
#define RTC_CNTL_WDT_STG3_HOLD_V  0xFFFFFFFF
#define RTC_CNTL_WDT_STG3_HOLD_S  0

#define RTC_CNTL_WDTFEED_REG          (DR_REG_RTCCNTL_BASE + 0xa0)
/* RTC_CNTL_WDT_FEED : WO ;bitpos:[31] ;default: 1'd0 ; */
/*description: */
#define RTC_CNTL_WDT_FEED  (BIT(31))
#define RTC_CNTL_WDT_FEED_M  (BIT(31))
#define RTC_CNTL_WDT_FEED_V  0x1
#define RTC_CNTL_WDT_FEED_S  31

#define RTC_CNTL_WDTWPROTECT_REG          (DR_REG_RTCCNTL_BASE + 0xa4)
/* RTC_CNTL_WDT_WKEY : R/W ;bitpos:[31:0] ;default: 32'h50d83aa1 ; */
/*description: */
#define RTC_CNTL_WDT_WKEY  0xFFFFFFFF
#define RTC_CNTL_WDT_WKEY_M  ((RTC_CNTL_WDT_WKEY_V)<<(RTC_CNTL_WDT_WKEY_S))
#define RTC_CNTL_WDT_WKEY_V  0xFFFFFFFF
#define RTC_CNTL_WDT_WKEY_S  0

#define RTC_CNTL_TEST_MUX_REG          (DR_REG_RTCCNTL_BASE + 0xa8)
/* RTC_CNTL_DTEST_RTC : R/W ;bitpos:[31:30] ;default: 2'd0 ; */
/*description: DTEST_RTC*/
#define RTC_CNTL_DTEST_RTC  0x00000003
#define RTC_CNTL_DTEST_RTC_M  ((RTC_CNTL_DTEST_RTC_V)<<(RTC_CNTL_DTEST_RTC_S))
#define RTC_CNTL_DTEST_RTC_V  0x3
#define RTC_CNTL_DTEST_RTC_S  30
/* RTC_CNTL_ENT_RTC : R/W ;bitpos:[29] ;default: 1'd0 ; */
/*description: ENT_RTC*/
#define RTC_CNTL_ENT_RTC  (BIT(29))
#define RTC_CNTL_ENT_RTC_M  (BIT(29))
#define RTC_CNTL_ENT_RTC_V  0x1
#define RTC_CNTL_ENT_RTC_S  29

#define RTC_CNTL_SW_CPU_STALL_REG          (DR_REG_RTCCNTL_BASE + 0xac)
/* RTC_CNTL_SW_STALL_PROCPU_C1 : R/W ;bitpos:[31:26] ;default: 6'b0 ; */
/*description: {reg_sw_stall_procpu_c1[5:0]   reg_sw_stall_procpu_c0[1:0]} ==
 0x86 will stall PRO CPU*/
#define RTC_CNTL_SW_STALL_PROCPU_C1  0x0000003F
#define RTC_CNTL_SW_STALL_PROCPU_C1_M  ((RTC_CNTL_SW_STALL_PROCPU_C1_V)<<(RTC_CNTL_SW_STALL_PROCPU_C1_S))
#define RTC_CNTL_SW_STALL_PROCPU_C1_V  0x3F
#define RTC_CNTL_SW_STALL_PROCPU_C1_S  26
/* RTC_CNTL_SW_STALL_APPCPU_C1 : R/W ;bitpos:[25:20] ;default: 6'b0 ; */
/*description: {reg_sw_stall_appcpu_c1[5:0]   reg_sw_stall_appcpu_c0[1:0]} ==
 0x86 will stall APP CPU*/
#define RTC_CNTL_SW_STALL_APPCPU_C1  0x0000003F
#define RTC_CNTL_SW_STALL_APPCPU_C1_M  ((RTC_CNTL_SW_STALL_APPCPU_C1_V)<<(RTC_CNTL_SW_STALL_APPCPU_C1_S))
#define RTC_CNTL_SW_STALL_APPCPU_C1_V  0x3F
#define RTC_CNTL_SW_STALL_APPCPU_C1_S  20

#define RTC_CNTL_STORE4_REG          (DR_REG_RTCCNTL_BASE + 0xb0)
/* RTC_CNTL_SCRATCH4 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: 32-bit general purpose retention register*/
#define RTC_CNTL_SCRATCH4  0xFFFFFFFF
#define RTC_CNTL_SCRATCH4_M  ((RTC_CNTL_SCRATCH4_V)<<(RTC_CNTL_SCRATCH4_S))
#define RTC_CNTL_SCRATCH4_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH4_S  0

#define RTC_CNTL_STORE5_REG          (DR_REG_RTCCNTL_BASE + 0xb4)
/* RTC_CNTL_SCRATCH5 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: 32-bit general purpose retention register*/
#define RTC_CNTL_SCRATCH5  0xFFFFFFFF
#define RTC_CNTL_SCRATCH5_M  ((RTC_CNTL_SCRATCH5_V)<<(RTC_CNTL_SCRATCH5_S))
#define RTC_CNTL_SCRATCH5_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH5_S  0

#define RTC_CNTL_STORE6_REG          (DR_REG_RTCCNTL_BASE + 0xb8)
/* RTC_CNTL_SCRATCH6 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: 32-bit general purpose retention register*/
#define RTC_CNTL_SCRATCH6  0xFFFFFFFF
#define RTC_CNTL_SCRATCH6_M  ((RTC_CNTL_SCRATCH6_V)<<(RTC_CNTL_SCRATCH6_S))
#define RTC_CNTL_SCRATCH6_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH6_S  0

#define RTC_CNTL_STORE7_REG          (DR_REG_RTCCNTL_BASE + 0xbc)
/* RTC_CNTL_SCRATCH7 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: 32-bit general purpose retention register*/
#define RTC_CNTL_SCRATCH7  0xFFFFFFFF
#define RTC_CNTL_SCRATCH7_M  ((RTC_CNTL_SCRATCH7_V)<<(RTC_CNTL_SCRATCH7_S))
#define RTC_CNTL_SCRATCH7_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH7_S  0

#define RTC_CNTL_LOW_POWER_ST_REG          (DR_REG_RTCCNTL_BASE + 0xc0)
/* RTC_CNTL_RDY_FOR_WAKEUP : R/0; bitpos:[19]; default: 0 */
/*description: 1 if RTC controller is ready to execute WAKE instruction, 0 otherwise */
#define RTC_CNTL_RDY_FOR_WAKEUP  (BIT(19))
#define RTC_CNTL_RDY_FOR_WAKEUP_M  (BIT(19))
#define RTC_CNTL_RDY_FOR_WAKEUP_V  0x1
#define RTC_CNTL_RDY_FOR_WAKEUP_S  19

/* Compatibility definition */
#define RTC_CNTL_DIAG0_REG RTC_CNTL_LOW_POWER_ST_REG
/* RTC_CNTL_LOW_POWER_DIAG0 : RO ;bitpos:[31:0] ;default: 0 ; */
/*description: */
#define RTC_CNTL_LOW_POWER_DIAG0  0xFFFFFFFF
#define RTC_CNTL_LOW_POWER_DIAG0_M  ((RTC_CNTL_LOW_POWER_DIAG0_V)<<(RTC_CNTL_LOW_POWER_DIAG0_S))
#define RTC_CNTL_LOW_POWER_DIAG0_V  0xFFFFFFFF
#define RTC_CNTL_LOW_POWER_DIAG0_S  0

#define RTC_CNTL_DIAG1_REG          (DR_REG_RTCCNTL_BASE + 0xc4)
/* RTC_CNTL_LOW_POWER_DIAG1 : RO ;bitpos:[31:0] ;default: 0 ; */
/*description: */
#define RTC_CNTL_LOW_POWER_DIAG1  0xFFFFFFFF
#define RTC_CNTL_LOW_POWER_DIAG1_M  ((RTC_CNTL_LOW_POWER_DIAG1_V)<<(RTC_CNTL_LOW_POWER_DIAG1_S))
#define RTC_CNTL_LOW_POWER_DIAG1_V  0xFFFFFFFF
#define RTC_CNTL_LOW_POWER_DIAG1_S  0

#define RTC_CNTL_HOLD_FORCE_REG          (DR_REG_RTCCNTL_BASE + 0xc8)
/* RTC_CNTL_X32N_HOLD_FORCE : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_X32N_HOLD_FORCE  (BIT(17))
#define RTC_CNTL_X32N_HOLD_FORCE_M  (BIT(17))
#define RTC_CNTL_X32N_HOLD_FORCE_V  0x1
#define RTC_CNTL_X32N_HOLD_FORCE_S  17
/* RTC_CNTL_X32P_HOLD_FORCE : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_X32P_HOLD_FORCE  (BIT(16))
#define RTC_CNTL_X32P_HOLD_FORCE_M  (BIT(16))
#define RTC_CNTL_X32P_HOLD_FORCE_V  0x1
#define RTC_CNTL_X32P_HOLD_FORCE_S  16
/* RTC_CNTL_TOUCH_PAD7_HOLD_FORCE : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_TOUCH_PAD7_HOLD_FORCE  (BIT(15))
#define RTC_CNTL_TOUCH_PAD7_HOLD_FORCE_M  (BIT(15))
#define RTC_CNTL_TOUCH_PAD7_HOLD_FORCE_V  0x1
#define RTC_CNTL_TOUCH_PAD7_HOLD_FORCE_S  15
/* RTC_CNTL_TOUCH_PAD6_HOLD_FORCE : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_TOUCH_PAD6_HOLD_FORCE  (BIT(14))
#define RTC_CNTL_TOUCH_PAD6_HOLD_FORCE_M  (BIT(14))
#define RTC_CNTL_TOUCH_PAD6_HOLD_FORCE_V  0x1
#define RTC_CNTL_TOUCH_PAD6_HOLD_FORCE_S  14
/* RTC_CNTL_TOUCH_PAD5_HOLD_FORCE : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_TOUCH_PAD5_HOLD_FORCE  (BIT(13))
#define RTC_CNTL_TOUCH_PAD5_HOLD_FORCE_M  (BIT(13))
#define RTC_CNTL_TOUCH_PAD5_HOLD_FORCE_V  0x1
#define RTC_CNTL_TOUCH_PAD5_HOLD_FORCE_S  13
/* RTC_CNTL_TOUCH_PAD4_HOLD_FORCE : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_TOUCH_PAD4_HOLD_FORCE  (BIT(12))
#define RTC_CNTL_TOUCH_PAD4_HOLD_FORCE_M  (BIT(12))
#define RTC_CNTL_TOUCH_PAD4_HOLD_FORCE_V  0x1
#define RTC_CNTL_TOUCH_PAD4_HOLD_FORCE_S  12
/* RTC_CNTL_TOUCH_PAD3_HOLD_FORCE : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_TOUCH_PAD3_HOLD_FORCE  (BIT(11))
#define RTC_CNTL_TOUCH_PAD3_HOLD_FORCE_M  (BIT(11))
#define RTC_CNTL_TOUCH_PAD3_HOLD_FORCE_V  0x1
#define RTC_CNTL_TOUCH_PAD3_HOLD_FORCE_S  11
/* RTC_CNTL_TOUCH_PAD2_HOLD_FORCE : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_TOUCH_PAD2_HOLD_FORCE  (BIT(10))
#define RTC_CNTL_TOUCH_PAD2_HOLD_FORCE_M  (BIT(10))
#define RTC_CNTL_TOUCH_PAD2_HOLD_FORCE_V  0x1
#define RTC_CNTL_TOUCH_PAD2_HOLD_FORCE_S  10
/* RTC_CNTL_TOUCH_PAD1_HOLD_FORCE : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_TOUCH_PAD1_HOLD_FORCE  (BIT(9))
#define RTC_CNTL_TOUCH_PAD1_HOLD_FORCE_M  (BIT(9))
#define RTC_CNTL_TOUCH_PAD1_HOLD_FORCE_V  0x1
#define RTC_CNTL_TOUCH_PAD1_HOLD_FORCE_S  9
/* RTC_CNTL_TOUCH_PAD0_HOLD_FORCE : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_TOUCH_PAD0_HOLD_FORCE  (BIT(8))
#define RTC_CNTL_TOUCH_PAD0_HOLD_FORCE_M  (BIT(8))
#define RTC_CNTL_TOUCH_PAD0_HOLD_FORCE_V  0x1
#define RTC_CNTL_TOUCH_PAD0_HOLD_FORCE_S  8
/* RTC_CNTL_SENSE4_HOLD_FORCE : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_SENSE4_HOLD_FORCE  (BIT(7))
#define RTC_CNTL_SENSE4_HOLD_FORCE_M  (BIT(7))
#define RTC_CNTL_SENSE4_HOLD_FORCE_V  0x1
#define RTC_CNTL_SENSE4_HOLD_FORCE_S  7
/* RTC_CNTL_SENSE3_HOLD_FORCE : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_SENSE3_HOLD_FORCE  (BIT(6))
#define RTC_CNTL_SENSE3_HOLD_FORCE_M  (BIT(6))
#define RTC_CNTL_SENSE3_HOLD_FORCE_V  0x1
#define RTC_CNTL_SENSE3_HOLD_FORCE_S  6
/* RTC_CNTL_SENSE2_HOLD_FORCE : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_SENSE2_HOLD_FORCE  (BIT(5))
#define RTC_CNTL_SENSE2_HOLD_FORCE_M  (BIT(5))
#define RTC_CNTL_SENSE2_HOLD_FORCE_V  0x1
#define RTC_CNTL_SENSE2_HOLD_FORCE_S  5
/* RTC_CNTL_SENSE1_HOLD_FORCE : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_SENSE1_HOLD_FORCE  (BIT(4))
#define RTC_CNTL_SENSE1_HOLD_FORCE_M  (BIT(4))
#define RTC_CNTL_SENSE1_HOLD_FORCE_V  0x1
#define RTC_CNTL_SENSE1_HOLD_FORCE_S  4
/* RTC_CNTL_PDAC2_HOLD_FORCE : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_PDAC2_HOLD_FORCE  (BIT(3))
#define RTC_CNTL_PDAC2_HOLD_FORCE_M  (BIT(3))
#define RTC_CNTL_PDAC2_HOLD_FORCE_V  0x1
#define RTC_CNTL_PDAC2_HOLD_FORCE_S  3
/* RTC_CNTL_PDAC1_HOLD_FORCE : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_PDAC1_HOLD_FORCE  (BIT(2))
#define RTC_CNTL_PDAC1_HOLD_FORCE_M  (BIT(2))
#define RTC_CNTL_PDAC1_HOLD_FORCE_V  0x1
#define RTC_CNTL_PDAC1_HOLD_FORCE_S  2
/* RTC_CNTL_ADC2_HOLD_FORCE : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_ADC2_HOLD_FORCE  (BIT(1))
#define RTC_CNTL_ADC2_HOLD_FORCE_M  (BIT(1))
#define RTC_CNTL_ADC2_HOLD_FORCE_V  0x1
#define RTC_CNTL_ADC2_HOLD_FORCE_S  1
/* RTC_CNTL_ADC1_HOLD_FORCE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_ADC1_HOLD_FORCE  (BIT(0))
#define RTC_CNTL_ADC1_HOLD_FORCE_M  (BIT(0))
#define RTC_CNTL_ADC1_HOLD_FORCE_V  0x1
#define RTC_CNTL_ADC1_HOLD_FORCE_S  0

#define RTC_CNTL_EXT_WAKEUP1_REG          (DR_REG_RTCCNTL_BASE + 0xcc)
/* RTC_CNTL_EXT_WAKEUP1_STATUS_CLR : WO ;bitpos:[18] ;default: 1'd0 ; */
/*description: clear ext wakeup1 status*/
#define RTC_CNTL_EXT_WAKEUP1_STATUS_CLR  (BIT(18))
#define RTC_CNTL_EXT_WAKEUP1_STATUS_CLR_M  (BIT(18))
#define RTC_CNTL_EXT_WAKEUP1_STATUS_CLR_V  0x1
#define RTC_CNTL_EXT_WAKEUP1_STATUS_CLR_S  18
/* RTC_CNTL_EXT_WAKEUP1_SEL : R/W ;bitpos:[17:0] ;default: 18'd0 ; */
/*description: Bitmap to select RTC pads for ext wakeup1*/
#define RTC_CNTL_EXT_WAKEUP1_SEL  0x0003FFFF
#define RTC_CNTL_EXT_WAKEUP1_SEL_M  ((RTC_CNTL_EXT_WAKEUP1_SEL_V)<<(RTC_CNTL_EXT_WAKEUP1_SEL_S))
#define RTC_CNTL_EXT_WAKEUP1_SEL_V  0x3FFFF
#define RTC_CNTL_EXT_WAKEUP1_SEL_S  0

#define RTC_CNTL_EXT_WAKEUP1_STATUS_REG          (DR_REG_RTCCNTL_BASE + 0xd0)
/* RTC_CNTL_EXT_WAKEUP1_STATUS : RO ;bitpos:[17:0] ;default: 18'd0 ; */
/*description: ext wakeup1 status*/
#define RTC_CNTL_EXT_WAKEUP1_STATUS  0x0003FFFF
#define RTC_CNTL_EXT_WAKEUP1_STATUS_M  ((RTC_CNTL_EXT_WAKEUP1_STATUS_V)<<(RTC_CNTL_EXT_WAKEUP1_STATUS_S))
#define RTC_CNTL_EXT_WAKEUP1_STATUS_V  0x3FFFF
#define RTC_CNTL_EXT_WAKEUP1_STATUS_S  0

#define RTC_CNTL_BROWN_OUT_REG          (DR_REG_RTCCNTL_BASE + 0xd4)
/* RTC_CNTL_BROWN_OUT_DET : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: brown out detect*/
#define RTC_CNTL_BROWN_OUT_DET  (BIT(31))
#define RTC_CNTL_BROWN_OUT_DET_M  (BIT(31))
#define RTC_CNTL_BROWN_OUT_DET_V  0x1
#define RTC_CNTL_BROWN_OUT_DET_S  31
/* RTC_CNTL_BROWN_OUT_ENA : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: enable brown out*/
#define RTC_CNTL_BROWN_OUT_ENA  (BIT(30))
#define RTC_CNTL_BROWN_OUT_ENA_M  (BIT(30))
#define RTC_CNTL_BROWN_OUT_ENA_V  0x1
#define RTC_CNTL_BROWN_OUT_ENA_S  30
/* RTC_CNTL_DBROWN_OUT_THRES : R/W ;bitpos:[29:27] ;default: 3'b010 ; */
/*description: brown out threshold*/
#define RTC_CNTL_DBROWN_OUT_THRES  0x00000007
#define RTC_CNTL_DBROWN_OUT_THRES_M  ((RTC_CNTL_DBROWN_OUT_THRES_V)<<(RTC_CNTL_DBROWN_OUT_THRES_S))
#define RTC_CNTL_DBROWN_OUT_THRES_V  0x7
#define RTC_CNTL_DBROWN_OUT_THRES_S  27
/* RTC_CNTL_BROWN_OUT_RST_ENA : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: enable brown out reset*/
#define RTC_CNTL_BROWN_OUT_RST_ENA  (BIT(26))
#define RTC_CNTL_BROWN_OUT_RST_ENA_M  (BIT(26))
#define RTC_CNTL_BROWN_OUT_RST_ENA_V  0x1
#define RTC_CNTL_BROWN_OUT_RST_ENA_S  26
/* RTC_CNTL_BROWN_OUT_RST_WAIT : R/W ;bitpos:[25:16] ;default: 10'h3ff ; */
/*description: brown out reset wait cycles*/
#define RTC_CNTL_BROWN_OUT_RST_WAIT  0x000003FF
#define RTC_CNTL_BROWN_OUT_RST_WAIT_M  ((RTC_CNTL_BROWN_OUT_RST_WAIT_V)<<(RTC_CNTL_BROWN_OUT_RST_WAIT_S))
#define RTC_CNTL_BROWN_OUT_RST_WAIT_V  0x3FF
#define RTC_CNTL_BROWN_OUT_RST_WAIT_S  16
/* RTC_CNTL_BROWN_OUT_PD_RF_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: enable power down RF when brown out happens*/
#define RTC_CNTL_BROWN_OUT_PD_RF_ENA  (BIT(15))
#define RTC_CNTL_BROWN_OUT_PD_RF_ENA_M  (BIT(15))
#define RTC_CNTL_BROWN_OUT_PD_RF_ENA_V  0x1
#define RTC_CNTL_BROWN_OUT_PD_RF_ENA_S  15
/* RTC_CNTL_BROWN_OUT_CLOSE_FLASH_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: enable close flash when brown out happens*/
#define RTC_CNTL_BROWN_OUT_CLOSE_FLASH_ENA  (BIT(14))
#define RTC_CNTL_BROWN_OUT_CLOSE_FLASH_ENA_M  (BIT(14))
#define RTC_CNTL_BROWN_OUT_CLOSE_FLASH_ENA_V  0x1
#define RTC_CNTL_BROWN_OUT_CLOSE_FLASH_ENA_S  14

#define RTC_MEM_CONF            (DR_REG_RTCCNTL_BASE + 0x40 * 4)
#define RTC_MEM_CRC_FINISH      (BIT(31))
#define RTC_MEM_CRC_FINISH_M    (BIT(31))
#define RTC_MEM_CRC_FINISH_V    0x1
#define RTC_MEM_CRC_FINISH_S    31
#define RTC_MEM_CRC_LEN         (0x7ff)
#define RTC_MEM_CRC_LEN_M       ((RTC_MEM_CRC_LEN_V)<<(RTC_MEM_CRC_LEN_S))
#define RTC_MEM_CRC_LEN_V       0x7ff
#define RTC_MEM_CRC_LEN_S       20
#define RTC_MEM_CRC_ADDR        0x7ff
#define RTC_MEM_CRC_ADDR_M      ((RTC_MEM_CRC_ADDR_V)<<(RTC_MEM_CRC_ADDR_S))
#define RTC_MEM_CRC_ADDR_V      0x7ff
#define RTC_MEM_CRC_ADDR_S      9
#define RTC_MEM_CRC_START       (BIT(8))
#define RTC_MEM_CRC_START_M     (BIT(8))
#define RTC_MEM_CRC_START_V     0x1
#define RTC_MEM_CRC_START_S     8
#define RTC_MEM_PID_CONF        0xff
#define RTC_MEM_PID_CONF_M      0xff
#define RTC_MEM_PID_CONF_V      0xff
#define RTC_MEM_PID_CONF_S      0

#define RTC_MEM_CRC_RES         (DR_REG_RTCCNTL_BASE + 0x41 * 4)

#define RTC_CNTL_DATE_REG          (DR_REG_RTCCNTL_BASE + 0x13c)
/* RTC_CNTL_CNTL_DATE : R/W ;bitpos:[27:0] ;default: 28'h1604280 ; */
/*description: */
#define RTC_CNTL_CNTL_DATE  0x0FFFFFFF
#define RTC_CNTL_CNTL_DATE_M  ((RTC_CNTL_CNTL_DATE_V)<<(RTC_CNTL_CNTL_DATE_S))
#define RTC_CNTL_CNTL_DATE_V  0xFFFFFFF
#define RTC_CNTL_CNTL_DATE_S  0
#define RTC_CNTL_RTC_CNTL_DATE_VERSION 0x1604280




#endif /*_SOC_RTC_CNTL_REG_H_ */
