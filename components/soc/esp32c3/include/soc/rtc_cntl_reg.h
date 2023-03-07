/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_RTC_CNTL_REG_H_
#define _SOC_RTC_CNTL_REG_H_

/* The value that needs to be written to RTC_CNTL_WDT_WKEY to write-enable the wdt registers */
#define RTC_CNTL_WDT_WKEY_VALUE 0x50D83AA1
/* The value that needs to be written to RTC_CNTL_SWD_WPROTECT_REG to write-enable the wdt registers */
#define RTC_CNTL_SWD_WKEY_VALUE 0x8F1D312A

/* Possible values for RTC_CNTL_WDT_CPU_RESET_LENGTH and RTC_CNTL_WDT_SYS_RESET_LENGTH */
#define RTC_WDT_RESET_LENGTH_100_NS    0
#define RTC_WDT_RESET_LENGTH_200_NS    1
#define RTC_WDT_RESET_LENGTH_300_NS    2
#define RTC_WDT_RESET_LENGTH_400_NS    3
#define RTC_WDT_RESET_LENGTH_500_NS    4
#define RTC_WDT_RESET_LENGTH_800_NS    5
#define RTC_WDT_RESET_LENGTH_1600_NS   6
#define RTC_WDT_RESET_LENGTH_3200_NS   7


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define RTC_CNTL_TIME0_REG		RTC_CNTL_TIME_LOW0_REG
#define RTC_CNTL_TIME1_REG		RTC_CNTL_TIME_HIGH0_REG

#define RTC_CNTL_OPTIONS0_REG          (DR_REG_RTCCNTL_BASE + 0x0000)
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
/* RTC_CNTL_XTL_EXT_CTR_SEL : R/W ;bitpos:[22:20] ;default: 3'd0 ; */
/*description: */
#define RTC_CNTL_XTL_EXT_CTR_SEL  0x00000007
#define RTC_CNTL_XTL_EXT_CTR_SEL_M  ((RTC_CNTL_XTL_EXT_CTR_SEL_V)<<(RTC_CNTL_XTL_EXT_CTR_SEL_S))
#define RTC_CNTL_XTL_EXT_CTR_SEL_V  0x7
#define RTC_CNTL_XTL_EXT_CTR_SEL_S  20
/* RTC_CNTL_XTL_EN_WAIT : R/W ;bitpos:[17:14] ;default: 4'd2 ; */
/*description: wait bias_sleep and current source wakeup*/
#define RTC_CNTL_XTL_EN_WAIT  0x0000000F
#define RTC_CNTL_XTL_EN_WAIT_M  ((RTC_CNTL_XTL_EN_WAIT_V)<<(RTC_CNTL_XTL_EN_WAIT_S))
#define RTC_CNTL_XTL_EN_WAIT_V  0xF
#define RTC_CNTL_XTL_EN_WAIT_S  14
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

#define RTC_CNTL_SLP_TIMER0_REG          (DR_REG_RTCCNTL_BASE + 0x0004)
/* RTC_CNTL_SLP_VAL_LO : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define RTC_CNTL_SLP_VAL_LO  0xFFFFFFFF
#define RTC_CNTL_SLP_VAL_LO_M  ((RTC_CNTL_SLP_VAL_LO_V)<<(RTC_CNTL_SLP_VAL_LO_S))
#define RTC_CNTL_SLP_VAL_LO_V  0xFFFFFFFF
#define RTC_CNTL_SLP_VAL_LO_S  0

#define RTC_CNTL_SLP_TIMER1_REG          (DR_REG_RTCCNTL_BASE + 0x0008)
/* RTC_CNTL_MAIN_TIMER_ALARM_EN : WO ;bitpos:[16] ;default: 1'h0 ; */
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

#define RTC_CNTL_TIME_UPDATE_REG          (DR_REG_RTCCNTL_BASE + 0x000C)
/* RTC_CNTL_TIME_UPDATE : WO ;bitpos:[31] ;default: 1'h0 ; */
/*description: Set 1: to update register with RTC timer*/
#define RTC_CNTL_TIME_UPDATE  (BIT(31))
#define RTC_CNTL_TIME_UPDATE_M  (BIT(31))
#define RTC_CNTL_TIME_UPDATE_V  0x1
#define RTC_CNTL_TIME_UPDATE_S  31
/* RTC_CNTL_TIMER_SYS_RST : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: enable to record system reset time*/
#define RTC_CNTL_TIMER_SYS_RST  (BIT(29))
#define RTC_CNTL_TIMER_SYS_RST_M  (BIT(29))
#define RTC_CNTL_TIMER_SYS_RST_V  0x1
#define RTC_CNTL_TIMER_SYS_RST_S  29
/* RTC_CNTL_TIMER_XTL_OFF : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: Enable to record 40M XTAL OFF time*/
#define RTC_CNTL_TIMER_XTL_OFF  (BIT(28))
#define RTC_CNTL_TIMER_XTL_OFF_M  (BIT(28))
#define RTC_CNTL_TIMER_XTL_OFF_V  0x1
#define RTC_CNTL_TIMER_XTL_OFF_S  28
/* RTC_CNTL_TIMER_SYS_STALL : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: Enable to record system stall time*/
#define RTC_CNTL_TIMER_SYS_STALL  (BIT(27))
#define RTC_CNTL_TIMER_SYS_STALL_M  (BIT(27))
#define RTC_CNTL_TIMER_SYS_STALL_V  0x1
#define RTC_CNTL_TIMER_SYS_STALL_S  27

#define RTC_CNTL_TIME_LOW0_REG          (DR_REG_RTCCNTL_BASE + 0x0010)
/* RTC_CNTL_TIMER_VALUE0_LOW : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: RTC timer low 32 bits*/
#define RTC_CNTL_TIMER_VALUE0_LOW  0xFFFFFFFF
#define RTC_CNTL_TIMER_VALUE0_LOW_M  ((RTC_CNTL_TIMER_VALUE0_LOW_V)<<(RTC_CNTL_TIMER_VALUE0_LOW_S))
#define RTC_CNTL_TIMER_VALUE0_LOW_V  0xFFFFFFFF
#define RTC_CNTL_TIMER_VALUE0_LOW_S  0

#define RTC_CNTL_TIME_HIGH0_REG          (DR_REG_RTCCNTL_BASE + 0x0014)
/* RTC_CNTL_TIMER_VALUE0_HIGH : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: RTC timer high 16 bits*/
#define RTC_CNTL_TIMER_VALUE0_HIGH  0x0000FFFF
#define RTC_CNTL_TIMER_VALUE0_HIGH_M  ((RTC_CNTL_TIMER_VALUE0_HIGH_V)<<(RTC_CNTL_TIMER_VALUE0_HIGH_S))
#define RTC_CNTL_TIMER_VALUE0_HIGH_V  0xFFFF
#define RTC_CNTL_TIMER_VALUE0_HIGH_S  0

#define RTC_CNTL_STATE0_REG          (DR_REG_RTCCNTL_BASE + 0x0018)
/* RTC_CNTL_SLEEP_EN : R/W ;bitpos:[31] ;default: 1'd0 ; */
/*description: sleep enable bit*/
#define RTC_CNTL_SLEEP_EN  (BIT(31))
#define RTC_CNTL_SLEEP_EN_M  (BIT(31))
#define RTC_CNTL_SLEEP_EN_V  0x1
#define RTC_CNTL_SLEEP_EN_S  31
/* RTC_CNTL_SLP_REJECT : R/W ;bitpos:[30] ;default: 1'd0 ; */
/*description: leep reject bit*/
#define RTC_CNTL_SLP_REJECT  (BIT(30))
#define RTC_CNTL_SLP_REJECT_M  (BIT(30))
#define RTC_CNTL_SLP_REJECT_V  0x1
#define RTC_CNTL_SLP_REJECT_S  30
/* RTC_CNTL_SLP_WAKEUP : R/W ;bitpos:[29] ;default: 1'd0 ; */
/*description: leep wakeup bit*/
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
/* RTC_CNTL_APB2RTC_BRIDGE_SEL : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: 1: APB to RTC using bridge*/
#define RTC_CNTL_APB2RTC_BRIDGE_SEL  (BIT(22))
#define RTC_CNTL_APB2RTC_BRIDGE_SEL_M  (BIT(22))
#define RTC_CNTL_APB2RTC_BRIDGE_SEL_V  0x1
#define RTC_CNTL_APB2RTC_BRIDGE_SEL_S  22
/* RTC_CNTL_SLP_REJECT_CAUSE_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: clear rtc sleep reject cause*/
#define RTC_CNTL_SLP_REJECT_CAUSE_CLR  (BIT(1))
#define RTC_CNTL_SLP_REJECT_CAUSE_CLR_M  (BIT(1))
#define RTC_CNTL_SLP_REJECT_CAUSE_CLR_V  0x1
#define RTC_CNTL_SLP_REJECT_CAUSE_CLR_S  1
/* RTC_CNTL_SW_CPU_INT : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: rtc software interrupt to main cpu*/
#define RTC_CNTL_SW_CPU_INT  (BIT(0))
#define RTC_CNTL_SW_CPU_INT_M  (BIT(0))
#define RTC_CNTL_SW_CPU_INT_V  0x1
#define RTC_CNTL_SW_CPU_INT_S  0

#define RTC_CNTL_TIMER1_REG          (DR_REG_RTCCNTL_BASE + 0x001C)
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
#define RTC_CNTL_XTL_BUF_WAIT_DEFAULT  100
/* RTC_CNTL_CK8M_WAIT : R/W ;bitpos:[13:6] ;default: 8'h10 ; */
/*description: CK8M wait cycles in slow_clk_rtc*/
#define RTC_CNTL_CK8M_WAIT  0x000000FF
#define RTC_CNTL_CK8M_WAIT_M  ((RTC_CNTL_CK8M_WAIT_V)<<(RTC_CNTL_CK8M_WAIT_S))
#define RTC_CNTL_CK8M_WAIT_V  0xFF
#define RTC_CNTL_CK8M_WAIT_S  6
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

#define RTC_CNTL_TIMER2_REG          (DR_REG_RTCCNTL_BASE + 0x0020)
/* RTC_CNTL_MIN_TIME_CK8M_OFF : R/W ;bitpos:[31:24] ;default: 8'h1 ; */
/*description: minimal cycles in slow_clk_rtc for CK8M in power down state*/
#define RTC_CNTL_MIN_TIME_CK8M_OFF  0x000000FF
#define RTC_CNTL_MIN_TIME_CK8M_OFF_M  ((RTC_CNTL_MIN_TIME_CK8M_OFF_V)<<(RTC_CNTL_MIN_TIME_CK8M_OFF_S))
#define RTC_CNTL_MIN_TIME_CK8M_OFF_V  0xFF
#define RTC_CNTL_MIN_TIME_CK8M_OFF_S  24

#define RTC_CNTL_TIMER3_REG          (DR_REG_RTCCNTL_BASE + 0x0024)
/* RTC_CNTL_BT_POWERUP_TIMER : R/W ;bitpos:[31:25] ;default: 7'h5 ; */
/*description: */
#define RTC_CNTL_BT_POWERUP_TIMER  0x0000007F
#define RTC_CNTL_BT_POWERUP_TIMER_M  ((RTC_CNTL_BT_POWERUP_TIMER_V)<<(RTC_CNTL_BT_POWERUP_TIMER_S))
#define RTC_CNTL_BT_POWERUP_TIMER_V  0x7F
#define RTC_CNTL_BT_POWERUP_TIMER_S  25
/* RTC_CNTL_BT_WAIT_TIMER : R/W ;bitpos:[24:16] ;default: 9'h8 ; */
/*description: */
#define RTC_CNTL_BT_WAIT_TIMER  0x000001FF
#define RTC_CNTL_BT_WAIT_TIMER_M  ((RTC_CNTL_BT_WAIT_TIMER_V)<<(RTC_CNTL_BT_WAIT_TIMER_S))
#define RTC_CNTL_BT_WAIT_TIMER_V  0x1FF
#define RTC_CNTL_BT_WAIT_TIMER_S  16
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

#define RTC_CNTL_TIMER4_REG          (DR_REG_RTCCNTL_BASE + 0x0028)
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
/* RTC_CNTL_CPU_TOP_POWERUP_TIMER : R/W ;bitpos:[15:9] ;default: 7'h5 ; */
/*description: */
#define RTC_CNTL_CPU_TOP_POWERUP_TIMER  0x0000007F
#define RTC_CNTL_CPU_TOP_POWERUP_TIMER_M  ((RTC_CNTL_CPU_TOP_POWERUP_TIMER_V)<<(RTC_CNTL_CPU_TOP_POWERUP_TIMER_S))
#define RTC_CNTL_CPU_TOP_POWERUP_TIMER_V  0x7F
#define RTC_CNTL_CPU_TOP_POWERUP_TIMER_S  9
/* RTC_CNTL_CPU_TOP_WAIT_TIMER : R/W ;bitpos:[8:0] ;default: 9'h8 ; */
/*description: */
#define RTC_CNTL_CPU_TOP_WAIT_TIMER  0x000001FF
#define RTC_CNTL_CPU_TOP_WAIT_TIMER_M  ((RTC_CNTL_CPU_TOP_WAIT_TIMER_V)<<(RTC_CNTL_CPU_TOP_WAIT_TIMER_S))
#define RTC_CNTL_CPU_TOP_WAIT_TIMER_V  0x1FF
#define RTC_CNTL_CPU_TOP_WAIT_TIMER_S  0

#define RTC_CNTL_TIMER5_REG          (DR_REG_RTCCNTL_BASE + 0x002C)
/* RTC_CNTL_MIN_SLP_VAL : R/W ;bitpos:[15:8] ;default: 8'h80 ; */
/*description: minimal sleep cycles in slow_clk_rtc*/
#define RTC_CNTL_MIN_SLP_VAL  0x000000FF
#define RTC_CNTL_MIN_SLP_VAL_M  ((RTC_CNTL_MIN_SLP_VAL_V)<<(RTC_CNTL_MIN_SLP_VAL_S))
#define RTC_CNTL_MIN_SLP_VAL_V  0xFF
#define RTC_CNTL_MIN_SLP_VAL_S  8

#define RTC_CNTL_TIMER6_REG          (DR_REG_RTCCNTL_BASE + 0x0030)
/* RTC_CNTL_DG_PERI_POWERUP_TIMER : R/W ;bitpos:[31:25] ;default: 7'h5 ; */
/*description: */
#define RTC_CNTL_DG_PERI_POWERUP_TIMER  0x0000007F
#define RTC_CNTL_DG_PERI_POWERUP_TIMER_M  ((RTC_CNTL_DG_PERI_POWERUP_TIMER_V)<<(RTC_CNTL_DG_PERI_POWERUP_TIMER_S))
#define RTC_CNTL_DG_PERI_POWERUP_TIMER_V  0x7F
#define RTC_CNTL_DG_PERI_POWERUP_TIMER_S  25
/* RTC_CNTL_DG_PERI_WAIT_TIMER : R/W ;bitpos:[24:16] ;default: 9'h8 ; */
/*description: */
#define RTC_CNTL_DG_PERI_WAIT_TIMER  0x000001FF
#define RTC_CNTL_DG_PERI_WAIT_TIMER_M  ((RTC_CNTL_DG_PERI_WAIT_TIMER_V)<<(RTC_CNTL_DG_PERI_WAIT_TIMER_S))
#define RTC_CNTL_DG_PERI_WAIT_TIMER_V  0x1FF
#define RTC_CNTL_DG_PERI_WAIT_TIMER_S  16

#define RTC_CNTL_ANA_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x0034)
/* RTC_CNTL_PLL_I2C_PU : R/W ;bitpos:[31] ;default: 1'd0 ; */
/*description: */
#define RTC_CNTL_PLL_I2C_PU  (BIT(31))
#define RTC_CNTL_PLL_I2C_PU_M  (BIT(31))
#define RTC_CNTL_PLL_I2C_PU_V  0x1
#define RTC_CNTL_PLL_I2C_PU_S  31
/* RTC_CNTL_CKGEN_I2C_PU : R/W ;bitpos:[30] ;default: 1'd0 ; */
/*description: 1: CKGEN_I2C power up*/
#define RTC_CNTL_CKGEN_I2C_PU  (BIT(30))
#define RTC_CNTL_CKGEN_I2C_PU_M  (BIT(30))
#define RTC_CNTL_CKGEN_I2C_PU_V  0x1
#define RTC_CNTL_CKGEN_I2C_PU_S  30
/* RTC_CNTL_RFRX_PBUS_PU : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: 1: RFRX_PBUS power up*/
#define RTC_CNTL_RFRX_PBUS_PU  (BIT(28))
#define RTC_CNTL_RFRX_PBUS_PU_M  (BIT(28))
#define RTC_CNTL_RFRX_PBUS_PU_V  0x1
#define RTC_CNTL_RFRX_PBUS_PU_S  28
/* RTC_CNTL_TXRF_I2C_PU : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: 1: TXRF_I2C power up*/
#define RTC_CNTL_TXRF_I2C_PU  (BIT(27))
#define RTC_CNTL_TXRF_I2C_PU_M  (BIT(27))
#define RTC_CNTL_TXRF_I2C_PU_V  0x1
#define RTC_CNTL_TXRF_I2C_PU_S  27
/* RTC_CNTL_PVTMON_PU : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: 1: PVTMON power up*/
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
/* RTC_CNTL_SAR_I2C_PU : R/W ;bitpos:[22] ;default: 1'b1 ; */
/*description: PLLA force power up*/
#define RTC_CNTL_SAR_I2C_PU  (BIT(22))
#define RTC_CNTL_SAR_I2C_PU_M  (BIT(22))
#define RTC_CNTL_SAR_I2C_PU_V  0x1
#define RTC_CNTL_SAR_I2C_PU_S  22
/* RTC_CNTL_GLITCH_RST_EN : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GLITCH_RST_EN  (BIT(20))
#define RTC_CNTL_GLITCH_RST_EN_M  (BIT(20))
#define RTC_CNTL_GLITCH_RST_EN_V  0x1
#define RTC_CNTL_GLITCH_RST_EN_S  20
/* RTC_CNTL_I2C_RESET_POR_FORCE_PU : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_I2C_RESET_POR_FORCE_PU  (BIT(19))
#define RTC_CNTL_I2C_RESET_POR_FORCE_PU_M  (BIT(19))
#define RTC_CNTL_I2C_RESET_POR_FORCE_PU_V  0x1
#define RTC_CNTL_I2C_RESET_POR_FORCE_PU_S  19
/* RTC_CNTL_I2C_RESET_POR_FORCE_PD : R/W ;bitpos:[18] ;default: 1'b1 ; */
/*description: */
#define RTC_CNTL_I2C_RESET_POR_FORCE_PD  (BIT(18))
#define RTC_CNTL_I2C_RESET_POR_FORCE_PD_M  (BIT(18))
#define RTC_CNTL_I2C_RESET_POR_FORCE_PD_V  0x1
#define RTC_CNTL_I2C_RESET_POR_FORCE_PD_S  18

#define RTC_CNTL_RESET_STATE_REG          (DR_REG_RTCCNTL_BASE + 0x0038)
/* RTC_CNTL_DRESET_MASK_PROCPU : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_DRESET_MASK_PROCPU  (BIT(25))
#define RTC_CNTL_DRESET_MASK_PROCPU_M  (BIT(25))
#define RTC_CNTL_DRESET_MASK_PROCPU_V  0x1
#define RTC_CNTL_DRESET_MASK_PROCPU_S  25
/* RTC_CNTL_DRESET_MASK_APPCPU : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_DRESET_MASK_APPCPU  (BIT(24))
#define RTC_CNTL_DRESET_MASK_APPCPU_M  (BIT(24))
#define RTC_CNTL_DRESET_MASK_APPCPU_V  0x1
#define RTC_CNTL_DRESET_MASK_APPCPU_S  24
/* RTC_CNTL_JTAG_RESET_FLAG_CLR_APPCPU : WO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_JTAG_RESET_FLAG_CLR_APPCPU  (BIT(23))
#define RTC_CNTL_JTAG_RESET_FLAG_CLR_APPCPU_M  (BIT(23))
#define RTC_CNTL_JTAG_RESET_FLAG_CLR_APPCPU_V  0x1
#define RTC_CNTL_JTAG_RESET_FLAG_CLR_APPCPU_S  23
/* RTC_CNTL_JTAG_RESET_FLAG_CLR_PROCPU : WO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_JTAG_RESET_FLAG_CLR_PROCPU  (BIT(22))
#define RTC_CNTL_JTAG_RESET_FLAG_CLR_PROCPU_M  (BIT(22))
#define RTC_CNTL_JTAG_RESET_FLAG_CLR_PROCPU_V  0x1
#define RTC_CNTL_JTAG_RESET_FLAG_CLR_PROCPU_S  22
/* RTC_CNTL_JTAG_RESET_FLAG_APPCPU : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_JTAG_RESET_FLAG_APPCPU  (BIT(21))
#define RTC_CNTL_JTAG_RESET_FLAG_APPCPU_M  (BIT(21))
#define RTC_CNTL_JTAG_RESET_FLAG_APPCPU_V  0x1
#define RTC_CNTL_JTAG_RESET_FLAG_APPCPU_S  21
/* RTC_CNTL_JTAG_RESET_FLAG_PROCPU : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_JTAG_RESET_FLAG_PROCPU  (BIT(20))
#define RTC_CNTL_JTAG_RESET_FLAG_PROCPU_M  (BIT(20))
#define RTC_CNTL_JTAG_RESET_FLAG_PROCPU_V  0x1
#define RTC_CNTL_JTAG_RESET_FLAG_PROCPU_S  20
/* RTC_CNTL_OCD_HALT_ON_RESET_PROCPU : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: PROCPU OcdHaltOnReset*/
#define RTC_CNTL_OCD_HALT_ON_RESET_PROCPU  (BIT(19))
#define RTC_CNTL_OCD_HALT_ON_RESET_PROCPU_M  (BIT(19))
#define RTC_CNTL_OCD_HALT_ON_RESET_PROCPU_V  0x1
#define RTC_CNTL_OCD_HALT_ON_RESET_PROCPU_S  19
/* RTC_CNTL_OCD_HALT_ON_RESET_APPCPU : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: APPCPU OcdHaltOnReset*/
#define RTC_CNTL_OCD_HALT_ON_RESET_APPCPU  (BIT(18))
#define RTC_CNTL_OCD_HALT_ON_RESET_APPCPU_M  (BIT(18))
#define RTC_CNTL_OCD_HALT_ON_RESET_APPCPU_V  0x1
#define RTC_CNTL_OCD_HALT_ON_RESET_APPCPU_S  18
/* RTC_CNTL_ALL_RESET_FLAG_CLR_APPCPU : WO ;bitpos:[17] ;default: 1'b0 ; */
/*description: clear APP CPU reset flag*/
#define RTC_CNTL_ALL_RESET_FLAG_CLR_APPCPU  (BIT(17))
#define RTC_CNTL_ALL_RESET_FLAG_CLR_APPCPU_M  (BIT(17))
#define RTC_CNTL_ALL_RESET_FLAG_CLR_APPCPU_V  0x1
#define RTC_CNTL_ALL_RESET_FLAG_CLR_APPCPU_S  17
/* RTC_CNTL_ALL_RESET_FLAG_CLR_PROCPU : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: clear PRO CPU reset_flag*/
#define RTC_CNTL_ALL_RESET_FLAG_CLR_PROCPU  (BIT(16))
#define RTC_CNTL_ALL_RESET_FLAG_CLR_PROCPU_M  (BIT(16))
#define RTC_CNTL_ALL_RESET_FLAG_CLR_PROCPU_V  0x1
#define RTC_CNTL_ALL_RESET_FLAG_CLR_PROCPU_S  16
/* RTC_CNTL_ALL_RESET_FLAG_APPCPU : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: APP CPU reset flag*/
#define RTC_CNTL_ALL_RESET_FLAG_APPCPU  (BIT(15))
#define RTC_CNTL_ALL_RESET_FLAG_APPCPU_M  (BIT(15))
#define RTC_CNTL_ALL_RESET_FLAG_APPCPU_V  0x1
#define RTC_CNTL_ALL_RESET_FLAG_APPCPU_S  15
/* RTC_CNTL_ALL_RESET_FLAG_PROCPU : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: PRO CPU reset_flag*/
#define RTC_CNTL_ALL_RESET_FLAG_PROCPU  (BIT(14))
#define RTC_CNTL_ALL_RESET_FLAG_PROCPU_M  (BIT(14))
#define RTC_CNTL_ALL_RESET_FLAG_PROCPU_V  0x1
#define RTC_CNTL_ALL_RESET_FLAG_PROCPU_S  14
/* RTC_CNTL_STAT_VECTOR_SEL_PROCPU : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: PRO CPU state vector sel*/
#define RTC_CNTL_STAT_VECTOR_SEL_PROCPU  (BIT(13))
#define RTC_CNTL_STAT_VECTOR_SEL_PROCPU_M  (BIT(13))
#define RTC_CNTL_STAT_VECTOR_SEL_PROCPU_V  0x1
#define RTC_CNTL_STAT_VECTOR_SEL_PROCPU_S  13
/* RTC_CNTL_STAT_VECTOR_SEL_APPCPU : R/W ;bitpos:[12] ;default: 1'b1 ; */
/*description: APP CPU state vector sel*/
#define RTC_CNTL_STAT_VECTOR_SEL_APPCPU  (BIT(12))
#define RTC_CNTL_STAT_VECTOR_SEL_APPCPU_M  (BIT(12))
#define RTC_CNTL_STAT_VECTOR_SEL_APPCPU_V  0x1
#define RTC_CNTL_STAT_VECTOR_SEL_APPCPU_S  12
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

#define RTC_CNTL_WAKEUP_STATE_REG          (DR_REG_RTCCNTL_BASE + 0x003C)
/* RTC_CNTL_WAKEUP_ENA : R/W ;bitpos:[31:15] ;default: 17'b1100 ; */
/*description: wakeup enable bitmap*/
#define RTC_CNTL_WAKEUP_ENA  0x0001FFFF
#define RTC_CNTL_WAKEUP_ENA_M  ((RTC_CNTL_WAKEUP_ENA_V)<<(RTC_CNTL_WAKEUP_ENA_S))
#define RTC_CNTL_WAKEUP_ENA_V  0x1FFFF
#define RTC_CNTL_WAKEUP_ENA_S  15

#define RTC_CNTL_INT_ENA_REG          (DR_REG_RTCCNTL_BASE + 0x0040)
/* RTC_CNTL_BBPLL_CAL_INT_ENA : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_BBPLL_CAL_INT_ENA  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ENA_M  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ENA_V  0x1
#define RTC_CNTL_BBPLL_CAL_INT_ENA_S  20
/* RTC_CNTL_GLITCH_DET_INT_ENA : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: enbale gitch det interrupt*/
#define RTC_CNTL_GLITCH_DET_INT_ENA  (BIT(19))
#define RTC_CNTL_GLITCH_DET_INT_ENA_M  (BIT(19))
#define RTC_CNTL_GLITCH_DET_INT_ENA_V  0x1
#define RTC_CNTL_GLITCH_DET_INT_ENA_S  19
/* RTC_CNTL_XTAL32K_DEAD_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: enable xtal32k_dead  interrupt*/
#define RTC_CNTL_XTAL32K_DEAD_INT_ENA  (BIT(16))
#define RTC_CNTL_XTAL32K_DEAD_INT_ENA_M  (BIT(16))
#define RTC_CNTL_XTAL32K_DEAD_INT_ENA_V  0x1
#define RTC_CNTL_XTAL32K_DEAD_INT_ENA_S  16
/* RTC_CNTL_SWD_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: enable super watch dog interrupt*/
#define RTC_CNTL_SWD_INT_ENA  (BIT(15))
#define RTC_CNTL_SWD_INT_ENA_M  (BIT(15))
#define RTC_CNTL_SWD_INT_ENA_V  0x1
#define RTC_CNTL_SWD_INT_ENA_S  15
/* RTC_CNTL_MAIN_TIMER_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: enable RTC main timer interrupt*/
#define RTC_CNTL_MAIN_TIMER_INT_ENA  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ENA_M  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ENA_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_ENA_S  10
/* RTC_CNTL_BROWN_OUT_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: enable brown out interrupt*/
#define RTC_CNTL_BROWN_OUT_INT_ENA  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ENA_M  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ENA_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_ENA_S  9
/* RTC_CNTL_WDT_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: enable RTC WDT interrupt*/
#define RTC_CNTL_WDT_INT_ENA  (BIT(3))
#define RTC_CNTL_WDT_INT_ENA_M  (BIT(3))
#define RTC_CNTL_WDT_INT_ENA_V  0x1
#define RTC_CNTL_WDT_INT_ENA_S  3
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

#define RTC_CNTL_INT_RAW_REG          (DR_REG_RTCCNTL_BASE + 0x0044)
/* RTC_CNTL_BBPLL_CAL_INT_RAW : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_BBPLL_CAL_INT_RAW  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_RAW_M  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_RAW_V  0x1
#define RTC_CNTL_BBPLL_CAL_INT_RAW_S  20
/* RTC_CNTL_GLITCH_DET_INT_RAW : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: glitch_det_interrupt_raw*/
#define RTC_CNTL_GLITCH_DET_INT_RAW  (BIT(19))
#define RTC_CNTL_GLITCH_DET_INT_RAW_M  (BIT(19))
#define RTC_CNTL_GLITCH_DET_INT_RAW_V  0x1
#define RTC_CNTL_GLITCH_DET_INT_RAW_S  19
/* RTC_CNTL_XTAL32K_DEAD_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: xtal32k dead detection interrupt raw*/
#define RTC_CNTL_XTAL32K_DEAD_INT_RAW  (BIT(16))
#define RTC_CNTL_XTAL32K_DEAD_INT_RAW_M  (BIT(16))
#define RTC_CNTL_XTAL32K_DEAD_INT_RAW_V  0x1
#define RTC_CNTL_XTAL32K_DEAD_INT_RAW_S  16
/* RTC_CNTL_SWD_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: super watch dog interrupt raw*/
#define RTC_CNTL_SWD_INT_RAW  (BIT(15))
#define RTC_CNTL_SWD_INT_RAW_M  (BIT(15))
#define RTC_CNTL_SWD_INT_RAW_V  0x1
#define RTC_CNTL_SWD_INT_RAW_S  15
/* RTC_CNTL_MAIN_TIMER_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: RTC main timer interrupt raw*/
#define RTC_CNTL_MAIN_TIMER_INT_RAW  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_RAW_M  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_RAW_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_RAW_S  10
/* RTC_CNTL_BROWN_OUT_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: brown out interrupt raw*/
#define RTC_CNTL_BROWN_OUT_INT_RAW  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_RAW_M  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_RAW_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_RAW_S  9
/* RTC_CNTL_WDT_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: RTC WDT interrupt raw*/
#define RTC_CNTL_WDT_INT_RAW  (BIT(3))
#define RTC_CNTL_WDT_INT_RAW_M  (BIT(3))
#define RTC_CNTL_WDT_INT_RAW_V  0x1
#define RTC_CNTL_WDT_INT_RAW_S  3
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

#define RTC_CNTL_INT_ST_REG          (DR_REG_RTCCNTL_BASE + 0x0048)
/* RTC_CNTL_BBPLL_CAL_INT_ST : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_BBPLL_CAL_INT_ST  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ST_M  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ST_V  0x1
#define RTC_CNTL_BBPLL_CAL_INT_ST_S  20
/* RTC_CNTL_GLITCH_DET_INT_ST : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: glitch_det_interrupt state*/
#define RTC_CNTL_GLITCH_DET_INT_ST  (BIT(19))
#define RTC_CNTL_GLITCH_DET_INT_ST_M  (BIT(19))
#define RTC_CNTL_GLITCH_DET_INT_ST_V  0x1
#define RTC_CNTL_GLITCH_DET_INT_ST_S  19
/* RTC_CNTL_XTAL32K_DEAD_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: xtal32k dead detection interrupt state*/
#define RTC_CNTL_XTAL32K_DEAD_INT_ST  (BIT(16))
#define RTC_CNTL_XTAL32K_DEAD_INT_ST_M  (BIT(16))
#define RTC_CNTL_XTAL32K_DEAD_INT_ST_V  0x1
#define RTC_CNTL_XTAL32K_DEAD_INT_ST_S  16
/* RTC_CNTL_SWD_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: super watch dog interrupt state*/
#define RTC_CNTL_SWD_INT_ST  (BIT(15))
#define RTC_CNTL_SWD_INT_ST_M  (BIT(15))
#define RTC_CNTL_SWD_INT_ST_V  0x1
#define RTC_CNTL_SWD_INT_ST_S  15
/* RTC_CNTL_MAIN_TIMER_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: RTC main timer interrupt state*/
#define RTC_CNTL_MAIN_TIMER_INT_ST  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ST_M  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ST_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_ST_S  10
/* RTC_CNTL_BROWN_OUT_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: brown out interrupt state*/
#define RTC_CNTL_BROWN_OUT_INT_ST  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ST_M  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ST_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_ST_S  9
/* RTC_CNTL_WDT_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: RTC WDT interrupt state*/
#define RTC_CNTL_WDT_INT_ST  (BIT(3))
#define RTC_CNTL_WDT_INT_ST_M  (BIT(3))
#define RTC_CNTL_WDT_INT_ST_V  0x1
#define RTC_CNTL_WDT_INT_ST_S  3
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

#define RTC_CNTL_INT_CLR_REG          (DR_REG_RTCCNTL_BASE + 0x004C)
/* RTC_CNTL_BBPLL_CAL_INT_CLR : WO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_BBPLL_CAL_INT_CLR  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_CLR_M  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_CLR_V  0x1
#define RTC_CNTL_BBPLL_CAL_INT_CLR_S  20
/* RTC_CNTL_GLITCH_DET_INT_CLR : WO ;bitpos:[19] ;default: 1'b0 ; */
/*description: Clear glitch det interrupt state*/
#define RTC_CNTL_GLITCH_DET_INT_CLR  (BIT(19))
#define RTC_CNTL_GLITCH_DET_INT_CLR_M  (BIT(19))
#define RTC_CNTL_GLITCH_DET_INT_CLR_V  0x1
#define RTC_CNTL_GLITCH_DET_INT_CLR_S  19
/* RTC_CNTL_XTAL32K_DEAD_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: Clear RTC WDT interrupt state*/
#define RTC_CNTL_XTAL32K_DEAD_INT_CLR  (BIT(16))
#define RTC_CNTL_XTAL32K_DEAD_INT_CLR_M  (BIT(16))
#define RTC_CNTL_XTAL32K_DEAD_INT_CLR_V  0x1
#define RTC_CNTL_XTAL32K_DEAD_INT_CLR_S  16
/* RTC_CNTL_SWD_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: Clear super watch dog interrupt state*/
#define RTC_CNTL_SWD_INT_CLR  (BIT(15))
#define RTC_CNTL_SWD_INT_CLR_M  (BIT(15))
#define RTC_CNTL_SWD_INT_CLR_V  0x1
#define RTC_CNTL_SWD_INT_CLR_S  15
/* RTC_CNTL_MAIN_TIMER_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: Clear RTC main timer interrupt state*/
#define RTC_CNTL_MAIN_TIMER_INT_CLR  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_CLR_M  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_CLR_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_CLR_S  10
/* RTC_CNTL_BROWN_OUT_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: Clear brown out interrupt state*/
#define RTC_CNTL_BROWN_OUT_INT_CLR  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_CLR_M  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_CLR_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_CLR_S  9
/* RTC_CNTL_WDT_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Clear RTC WDT interrupt state*/
#define RTC_CNTL_WDT_INT_CLR  (BIT(3))
#define RTC_CNTL_WDT_INT_CLR_M  (BIT(3))
#define RTC_CNTL_WDT_INT_CLR_V  0x1
#define RTC_CNTL_WDT_INT_CLR_S  3
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

#define RTC_CNTL_STORE0_REG          (DR_REG_RTCCNTL_BASE + 0x0050)
/* RTC_CNTL_SCRATCH0 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: */
#define RTC_CNTL_SCRATCH0  0xFFFFFFFF
#define RTC_CNTL_SCRATCH0_M  ((RTC_CNTL_SCRATCH0_V)<<(RTC_CNTL_SCRATCH0_S))
#define RTC_CNTL_SCRATCH0_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH0_S  0

#define RTC_CNTL_STORE1_REG          (DR_REG_RTCCNTL_BASE + 0x0054)
/* RTC_CNTL_SCRATCH1 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: */
#define RTC_CNTL_SCRATCH1  0xFFFFFFFF
#define RTC_CNTL_SCRATCH1_M  ((RTC_CNTL_SCRATCH1_V)<<(RTC_CNTL_SCRATCH1_S))
#define RTC_CNTL_SCRATCH1_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH1_S  0

#define RTC_CNTL_STORE2_REG          (DR_REG_RTCCNTL_BASE + 0x0058)
/* RTC_CNTL_SCRATCH2 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: */
#define RTC_CNTL_SCRATCH2  0xFFFFFFFF
#define RTC_CNTL_SCRATCH2_M  ((RTC_CNTL_SCRATCH2_V)<<(RTC_CNTL_SCRATCH2_S))
#define RTC_CNTL_SCRATCH2_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH2_S  0

#define RTC_CNTL_STORE3_REG          (DR_REG_RTCCNTL_BASE + 0x005C)
/* RTC_CNTL_SCRATCH3 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: */
#define RTC_CNTL_SCRATCH3  0xFFFFFFFF
#define RTC_CNTL_SCRATCH3_M  ((RTC_CNTL_SCRATCH3_V)<<(RTC_CNTL_SCRATCH3_S))
#define RTC_CNTL_SCRATCH3_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH3_S  0

#define RTC_CNTL_EXT_XTL_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x0060)
/* RTC_CNTL_XTL_EXT_CTR_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_XTL_EXT_CTR_EN  (BIT(31))
#define RTC_CNTL_XTL_EXT_CTR_EN_M  (BIT(31))
#define RTC_CNTL_XTL_EXT_CTR_EN_V  0x1
#define RTC_CNTL_XTL_EXT_CTR_EN_S  31
/* RTC_CNTL_XTL_EXT_CTR_LV : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: 0: power down XTAL at high level*/
#define RTC_CNTL_XTL_EXT_CTR_LV  (BIT(30))
#define RTC_CNTL_XTL_EXT_CTR_LV_M  (BIT(30))
#define RTC_CNTL_XTL_EXT_CTR_LV_V  0x1
#define RTC_CNTL_XTL_EXT_CTR_LV_S  30
/* RTC_CNTL_XTAL32K_GPIO_SEL : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: XTAL_32K sel. 0: external XTAL_32K*/
#define RTC_CNTL_XTAL32K_GPIO_SEL  (BIT(23))
#define RTC_CNTL_XTAL32K_GPIO_SEL_M  (BIT(23))
#define RTC_CNTL_XTAL32K_GPIO_SEL_V  0x1
#define RTC_CNTL_XTAL32K_GPIO_SEL_S  23
/* RTC_CNTL_WDT_STATE : RO ;bitpos:[22:20] ;default: 3'h0 ; */
/*description: state of 32k_wdt*/
#define RTC_CNTL_WDT_STATE  0x00000007
#define RTC_CNTL_WDT_STATE_M  ((RTC_CNTL_WDT_STATE_V)<<(RTC_CNTL_WDT_STATE_S))
#define RTC_CNTL_WDT_STATE_V  0x7
#define RTC_CNTL_WDT_STATE_S  20
/* RTC_CNTL_DAC_XTAL_32K : R/W ;bitpos:[19:17] ;default: 3'd3 ; */
/*description: DAC_XTAL_32K*/
#define RTC_CNTL_DAC_XTAL_32K  0x00000007
#define RTC_CNTL_DAC_XTAL_32K_M  ((RTC_CNTL_DAC_XTAL_32K_V)<<(RTC_CNTL_DAC_XTAL_32K_S))
#define RTC_CNTL_DAC_XTAL_32K_V  0x7
#define RTC_CNTL_DAC_XTAL_32K_S  17
/* RTC_CNTL_XPD_XTAL_32K : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: XPD_XTAL_32K*/
#define RTC_CNTL_XPD_XTAL_32K  (BIT(16))
#define RTC_CNTL_XPD_XTAL_32K_M  (BIT(16))
#define RTC_CNTL_XPD_XTAL_32K_V  0x1
#define RTC_CNTL_XPD_XTAL_32K_S  16
/* RTC_CNTL_DRES_XTAL_32K : R/W ;bitpos:[15:13] ;default: 3'd3 ; */
/*description: DRES_XTAL_32K*/
#define RTC_CNTL_DRES_XTAL_32K  0x00000007
#define RTC_CNTL_DRES_XTAL_32K_M  ((RTC_CNTL_DRES_XTAL_32K_V)<<(RTC_CNTL_DRES_XTAL_32K_S))
#define RTC_CNTL_DRES_XTAL_32K_V  0x7
#define RTC_CNTL_DRES_XTAL_32K_S  13
/* RTC_CNTL_DGM_XTAL_32K : R/W ;bitpos:[12:10] ;default: 3'd3 ; */
/*description: xtal_32k gm control*/
#define RTC_CNTL_DGM_XTAL_32K  0x00000007
#define RTC_CNTL_DGM_XTAL_32K_M  ((RTC_CNTL_DGM_XTAL_32K_V)<<(RTC_CNTL_DGM_XTAL_32K_S))
#define RTC_CNTL_DGM_XTAL_32K_V  0x7
#define RTC_CNTL_DGM_XTAL_32K_S  10
/* RTC_CNTL_DBUF_XTAL_32K : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: 0: single-end buffer 1: differential buffer*/
#define RTC_CNTL_DBUF_XTAL_32K  (BIT(9))
#define RTC_CNTL_DBUF_XTAL_32K_M  (BIT(9))
#define RTC_CNTL_DBUF_XTAL_32K_V  0x1
#define RTC_CNTL_DBUF_XTAL_32K_S  9
/* RTC_CNTL_ENCKINIT_XTAL_32K : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: apply an internal clock to help xtal 32k to start*/
#define RTC_CNTL_ENCKINIT_XTAL_32K  (BIT(8))
#define RTC_CNTL_ENCKINIT_XTAL_32K_M  (BIT(8))
#define RTC_CNTL_ENCKINIT_XTAL_32K_V  0x1
#define RTC_CNTL_ENCKINIT_XTAL_32K_S  8
/* RTC_CNTL_XTAL32K_XPD_FORCE : R/W ;bitpos:[7] ;default: 1'b1 ; */
/*description: Xtal 32k xpd control by sw or fsm*/
#define RTC_CNTL_XTAL32K_XPD_FORCE  (BIT(7))
#define RTC_CNTL_XTAL32K_XPD_FORCE_M  (BIT(7))
#define RTC_CNTL_XTAL32K_XPD_FORCE_V  0x1
#define RTC_CNTL_XTAL32K_XPD_FORCE_S  7
/* RTC_CNTL_XTAL32K_AUTO_RETURN : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: xtal 32k switch back xtal when xtal is restarted*/
#define RTC_CNTL_XTAL32K_AUTO_RETURN  (BIT(6))
#define RTC_CNTL_XTAL32K_AUTO_RETURN_M  (BIT(6))
#define RTC_CNTL_XTAL32K_AUTO_RETURN_V  0x1
#define RTC_CNTL_XTAL32K_AUTO_RETURN_S  6
/* RTC_CNTL_XTAL32K_AUTO_RESTART : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: xtal 32k restart xtal when xtal is dead*/
#define RTC_CNTL_XTAL32K_AUTO_RESTART  (BIT(5))
#define RTC_CNTL_XTAL32K_AUTO_RESTART_M  (BIT(5))
#define RTC_CNTL_XTAL32K_AUTO_RESTART_V  0x1
#define RTC_CNTL_XTAL32K_AUTO_RESTART_S  5
/* RTC_CNTL_XTAL32K_AUTO_BACKUP : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: xtal 32k switch to back up clock when xtal is dead*/
#define RTC_CNTL_XTAL32K_AUTO_BACKUP  (BIT(4))
#define RTC_CNTL_XTAL32K_AUTO_BACKUP_M  (BIT(4))
#define RTC_CNTL_XTAL32K_AUTO_BACKUP_V  0x1
#define RTC_CNTL_XTAL32K_AUTO_BACKUP_S  4
/* RTC_CNTL_XTAL32K_EXT_CLK_FO : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: xtal 32k external xtal clock force on*/
#define RTC_CNTL_XTAL32K_EXT_CLK_FO  (BIT(3))
#define RTC_CNTL_XTAL32K_EXT_CLK_FO_M  (BIT(3))
#define RTC_CNTL_XTAL32K_EXT_CLK_FO_V  0x1
#define RTC_CNTL_XTAL32K_EXT_CLK_FO_S  3
/* RTC_CNTL_XTAL32K_WDT_RESET : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: xtal 32k watch dog sw reset*/
#define RTC_CNTL_XTAL32K_WDT_RESET  (BIT(2))
#define RTC_CNTL_XTAL32K_WDT_RESET_M  (BIT(2))
#define RTC_CNTL_XTAL32K_WDT_RESET_V  0x1
#define RTC_CNTL_XTAL32K_WDT_RESET_S  2
/* RTC_CNTL_XTAL32K_WDT_CLK_FO : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: xtal 32k watch dog clock force on*/
#define RTC_CNTL_XTAL32K_WDT_CLK_FO  (BIT(1))
#define RTC_CNTL_XTAL32K_WDT_CLK_FO_M  (BIT(1))
#define RTC_CNTL_XTAL32K_WDT_CLK_FO_V  0x1
#define RTC_CNTL_XTAL32K_WDT_CLK_FO_S  1
/* RTC_CNTL_XTAL32K_WDT_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: xtal 32k watch dog enable*/
#define RTC_CNTL_XTAL32K_WDT_EN  (BIT(0))
#define RTC_CNTL_XTAL32K_WDT_EN_M  (BIT(0))
#define RTC_CNTL_XTAL32K_WDT_EN_V  0x1
#define RTC_CNTL_XTAL32K_WDT_EN_S  0

#define RTC_CNTL_EXT_WAKEUP_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x0064)
/* RTC_CNTL_GPIO_WAKEUP_FILTER : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: enable filter for gpio wakeup event*/
#define RTC_CNTL_GPIO_WAKEUP_FILTER  (BIT(31))
#define RTC_CNTL_GPIO_WAKEUP_FILTER_M  (BIT(31))
#define RTC_CNTL_GPIO_WAKEUP_FILTER_V  0x1
#define RTC_CNTL_GPIO_WAKEUP_FILTER_S  31

#define RTC_CNTL_SLP_REJECT_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x0068)
/* RTC_CNTL_DEEP_SLP_REJECT_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: enable reject for deep sleep*/
#define RTC_CNTL_DEEP_SLP_REJECT_EN  (BIT(31))
#define RTC_CNTL_DEEP_SLP_REJECT_EN_M  (BIT(31))
#define RTC_CNTL_DEEP_SLP_REJECT_EN_V  0x1
#define RTC_CNTL_DEEP_SLP_REJECT_EN_S  31
/* RTC_CNTL_LIGHT_SLP_REJECT_EN : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: enable reject for light sleep*/
#define RTC_CNTL_LIGHT_SLP_REJECT_EN  (BIT(30))
#define RTC_CNTL_LIGHT_SLP_REJECT_EN_M  (BIT(30))
#define RTC_CNTL_LIGHT_SLP_REJECT_EN_V  0x1
#define RTC_CNTL_LIGHT_SLP_REJECT_EN_S  30
/* RTC_CNTL_SLEEP_REJECT_ENA : R/W ;bitpos:[29:12] ;default: 17'd0 ; */
/*description: sleep reject enable*/
#define RTC_CNTL_SLEEP_REJECT_ENA  0x0003FFFF
#define RTC_CNTL_SLEEP_REJECT_ENA_M  ((RTC_CNTL_SLEEP_REJECT_ENA_V)<<(RTC_CNTL_SLEEP_REJECT_ENA_S))
#define RTC_CNTL_SLEEP_REJECT_ENA_V  0x3FFFF
#define RTC_CNTL_SLEEP_REJECT_ENA_S  12

#define RTC_CNTL_CPU_PERIOD_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x006C)
/* RTC_CNTL_CPUPERIOD_SEL : R/W ;bitpos:[31:30] ;default: 2'b00 ; */
/*description: */
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

#define RTC_CNTL_CLK_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x0070)
/* RTC_CNTL_ANA_CLK_RTC_SEL : R/W ;bitpos:[31:30] ;default: 2'd0 ; */
/*description: */
#define RTC_CNTL_ANA_CLK_RTC_SEL  0x00000003
#define RTC_CNTL_ANA_CLK_RTC_SEL_M  ((RTC_CNTL_ANA_CLK_RTC_SEL_V)<<(RTC_CNTL_ANA_CLK_RTC_SEL_S))
#define RTC_CNTL_ANA_CLK_RTC_SEL_V  0x3
#define RTC_CNTL_ANA_CLK_RTC_SEL_S  30
/* RTC_CNTL_FAST_CLK_RTC_SEL : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: fast_clk_rtc sel. 0: XTAL div 2*/
#define RTC_CNTL_FAST_CLK_RTC_SEL  (BIT(29))
#define RTC_CNTL_FAST_CLK_RTC_SEL_M  (BIT(29))
#define RTC_CNTL_FAST_CLK_RTC_SEL_V  0x1
#define RTC_CNTL_FAST_CLK_RTC_SEL_S  29
/* RTC_CNTL_XTAL_GLOBAL_FORCE_NOGATING : R/W ;bitpos:[28] ;default: 1'b1 ; */
/*description: */
#define RTC_CNTL_XTAL_GLOBAL_FORCE_NOGATING  (BIT(28))
#define RTC_CNTL_XTAL_GLOBAL_FORCE_NOGATING_M  (BIT(28))
#define RTC_CNTL_XTAL_GLOBAL_FORCE_NOGATING_V  0x1
#define RTC_CNTL_XTAL_GLOBAL_FORCE_NOGATING_S  28
/* RTC_CNTL_XTAL_GLOBAL_FORCE_GATING : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_XTAL_GLOBAL_FORCE_GATING  (BIT(27))
#define RTC_CNTL_XTAL_GLOBAL_FORCE_GATING_M  (BIT(27))
#define RTC_CNTL_XTAL_GLOBAL_FORCE_GATING_V  0x1
#define RTC_CNTL_XTAL_GLOBAL_FORCE_GATING_S  27
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
/* RTC_CNTL_CK8M_DFREQ : R/W ;bitpos:[24:17] ;default: 8'd172 ; */
/*description: CK8M_DFREQ*/
#define RTC_CNTL_CK8M_DFREQ  0x000000FF
#define RTC_CNTL_CK8M_DFREQ_M  ((RTC_CNTL_CK8M_DFREQ_V)<<(RTC_CNTL_CK8M_DFREQ_S))
#define RTC_CNTL_CK8M_DFREQ_V  0xFF
#define RTC_CNTL_CK8M_DFREQ_S  17
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
/* RTC_CNTL_CK8M_DIV_SEL : R/W ;bitpos:[14:12] ;default: 3'd3 ; */
/*description: divider = reg_ck8m_div_sel + 1*/
#define RTC_CNTL_CK8M_DIV_SEL  0x00000007
#define RTC_CNTL_CK8M_DIV_SEL_M  ((RTC_CNTL_CK8M_DIV_SEL_V)<<(RTC_CNTL_CK8M_DIV_SEL_S))
#define RTC_CNTL_CK8M_DIV_SEL_V  0x7
#define RTC_CNTL_CK8M_DIV_SEL_S  12
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
/*description: 1: CK8M_D256_OUT is actually CK8M*/
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
/*description: CK8M_D256_OUT divider. 00: div128*/
#define RTC_CNTL_CK8M_DIV  0x00000003
#define RTC_CNTL_CK8M_DIV_M  ((RTC_CNTL_CK8M_DIV_V)<<(RTC_CNTL_CK8M_DIV_S))
#define RTC_CNTL_CK8M_DIV_V  0x3
#define RTC_CNTL_CK8M_DIV_S  4
/* RTC_CNTL_CK8M_DIV_SEL_VLD : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: used to sync reg_ck8m_div_sel bus. Clear vld before set reg_ck8m_div_sel*/
#define RTC_CNTL_CK8M_DIV_SEL_VLD  (BIT(3))
#define RTC_CNTL_CK8M_DIV_SEL_VLD_M  (BIT(3))
#define RTC_CNTL_CK8M_DIV_SEL_VLD_V  0x1
#define RTC_CNTL_CK8M_DIV_SEL_VLD_S  3
/* RTC_CNTL_EFUSE_CLK_FORCE_NOGATING : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_EFUSE_CLK_FORCE_NOGATING  (BIT(2))
#define RTC_CNTL_EFUSE_CLK_FORCE_NOGATING_M  (BIT(2))
#define RTC_CNTL_EFUSE_CLK_FORCE_NOGATING_V  0x1
#define RTC_CNTL_EFUSE_CLK_FORCE_NOGATING_S  2
/* RTC_CNTL_EFUSE_CLK_FORCE_GATING : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_EFUSE_CLK_FORCE_GATING  (BIT(1))
#define RTC_CNTL_EFUSE_CLK_FORCE_GATING_M  (BIT(1))
#define RTC_CNTL_EFUSE_CLK_FORCE_GATING_V  0x1
#define RTC_CNTL_EFUSE_CLK_FORCE_GATING_S  1

#define RTC_CNTL_SLOW_CLK_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x0074)
/* RTC_CNTL_SLOW_CLK_NEXT_EDGE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_SLOW_CLK_NEXT_EDGE  (BIT(31))
#define RTC_CNTL_SLOW_CLK_NEXT_EDGE_M  (BIT(31))
#define RTC_CNTL_SLOW_CLK_NEXT_EDGE_V  0x1
#define RTC_CNTL_SLOW_CLK_NEXT_EDGE_S  31
/* RTC_CNTL_ANA_CLK_DIV : R/W ;bitpos:[30:23] ;default: 8'd0 ; */
/*description: */
#define RTC_CNTL_ANA_CLK_DIV  0x000000FF
#define RTC_CNTL_ANA_CLK_DIV_M  ((RTC_CNTL_ANA_CLK_DIV_V)<<(RTC_CNTL_ANA_CLK_DIV_S))
#define RTC_CNTL_ANA_CLK_DIV_V  0xFF
#define RTC_CNTL_ANA_CLK_DIV_S  23
/* RTC_CNTL_ANA_CLK_DIV_VLD : R/W ;bitpos:[22] ;default: 1'b1 ; */
/*description: used to sync div bus. clear vld before set reg_rtc_ana_clk_div*/
#define RTC_CNTL_ANA_CLK_DIV_VLD  (BIT(22))
#define RTC_CNTL_ANA_CLK_DIV_VLD_M  (BIT(22))
#define RTC_CNTL_ANA_CLK_DIV_VLD_V  0x1
#define RTC_CNTL_ANA_CLK_DIV_VLD_S  22

#define RTC_CNTL_SDIO_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x0078)
/* RTC_CNTL_XPD_SDIO_REG : R/W ;bitpos:[31] ;default: 1'd0 ; */
/*description: */
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
/* RTC_CNTL_DREFM_SDIO : R/W ;bitpos:[28:27] ;default: 2'b01 ; */
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
/*description: 1: use SW option to control SDIO_REG*/
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
/* RTC_CNTL_SDIO_ENCURLIM : R/W ;bitpos:[20] ;default: 1'd1 ; */
/*description: enable current limit*/
#define RTC_CNTL_SDIO_ENCURLIM  (BIT(20))
#define RTC_CNTL_SDIO_ENCURLIM_M  (BIT(20))
#define RTC_CNTL_SDIO_ENCURLIM_V  0x1
#define RTC_CNTL_SDIO_ENCURLIM_S  20
/* RTC_CNTL_SDIO_MODECURLIM : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: select current limit mode*/
#define RTC_CNTL_SDIO_MODECURLIM  (BIT(19))
#define RTC_CNTL_SDIO_MODECURLIM_M  (BIT(19))
#define RTC_CNTL_SDIO_MODECURLIM_V  0x1
#define RTC_CNTL_SDIO_MODECURLIM_S  19
/* RTC_CNTL_SDIO_DCURLIM : R/W ;bitpos:[18:16] ;default: 3'd0 ; */
/*description: tune current limit threshold when tieh = 0. About 800mA/(8+d)*/
#define RTC_CNTL_SDIO_DCURLIM  0x00000007
#define RTC_CNTL_SDIO_DCURLIM_M  ((RTC_CNTL_SDIO_DCURLIM_V)<<(RTC_CNTL_SDIO_DCURLIM_S))
#define RTC_CNTL_SDIO_DCURLIM_V  0x7
#define RTC_CNTL_SDIO_DCURLIM_S  16
/* RTC_CNTL_SDIO_EN_INITI : R/W ;bitpos:[15] ;default: 1'd1 ; */
/*description: 0 to set init[1:0]=0*/
#define RTC_CNTL_SDIO_EN_INITI  (BIT(15))
#define RTC_CNTL_SDIO_EN_INITI_M  (BIT(15))
#define RTC_CNTL_SDIO_EN_INITI_V  0x1
#define RTC_CNTL_SDIO_EN_INITI_S  15
/* RTC_CNTL_SDIO_INITI : R/W ;bitpos:[14:13] ;default: 2'd1 ; */
/*description: add resistor from ldo output to ground. 0: no res*/
#define RTC_CNTL_SDIO_INITI  0x00000003
#define RTC_CNTL_SDIO_INITI_M  ((RTC_CNTL_SDIO_INITI_V)<<(RTC_CNTL_SDIO_INITI_S))
#define RTC_CNTL_SDIO_INITI_V  0x3
#define RTC_CNTL_SDIO_INITI_S  13
/* RTC_CNTL_SDIO_DCAP : R/W ;bitpos:[12:11] ;default: 2'b11 ; */
/*description: ability to prevent LDO from overshoot*/
#define RTC_CNTL_SDIO_DCAP  0x00000003
#define RTC_CNTL_SDIO_DCAP_M  ((RTC_CNTL_SDIO_DCAP_V)<<(RTC_CNTL_SDIO_DCAP_S))
#define RTC_CNTL_SDIO_DCAP_V  0x3
#define RTC_CNTL_SDIO_DCAP_S  11
/* RTC_CNTL_SDIO_DTHDRV : R/W ;bitpos:[10:9] ;default: 2'b11 ; */
/*description: Tieh = 1 mode drive ability. Initially set to 0 to limit charge current*/
#define RTC_CNTL_SDIO_DTHDRV  0x00000003
#define RTC_CNTL_SDIO_DTHDRV_M  ((RTC_CNTL_SDIO_DTHDRV_V)<<(RTC_CNTL_SDIO_DTHDRV_S))
#define RTC_CNTL_SDIO_DTHDRV_V  0x3
#define RTC_CNTL_SDIO_DTHDRV_S  9
/* RTC_CNTL_SDIO_TIMER_TARGET : R/W ;bitpos:[7:0] ;default: 8'd10 ; */
/*description: timer count to apply reg_sdio_dcap after sdio power on*/
#define RTC_CNTL_SDIO_TIMER_TARGET  0x000000FF
#define RTC_CNTL_SDIO_TIMER_TARGET_M  ((RTC_CNTL_SDIO_TIMER_TARGET_V)<<(RTC_CNTL_SDIO_TIMER_TARGET_S))
#define RTC_CNTL_SDIO_TIMER_TARGET_V  0xFF
#define RTC_CNTL_SDIO_TIMER_TARGET_S  0

#define RTC_CNTL_BIAS_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x007C)
/* RTC_CNTL_DBG_ATTEN_MONITOR : R/W ;bitpos:[25:22] ;default: 4'd0 ; */
/*description: DBG_ATTEN when rtc in monitor state*/
#define RTC_CNTL_DBG_ATTEN_MONITOR  0x0000000F
#define RTC_CNTL_DBG_ATTEN_MONITOR_M  ((RTC_CNTL_DBG_ATTEN_MONITOR_V)<<(RTC_CNTL_DBG_ATTEN_MONITOR_S))
#define RTC_CNTL_DBG_ATTEN_MONITOR_V  0xF
#define RTC_CNTL_DBG_ATTEN_MONITOR_S  22
/* RTC_CNTL_DBG_ATTEN_DEEP_SLP : R/W ;bitpos:[21:18] ;default: 4'd0 ; */
/*description: DBG_ATTEN when rtc in sleep state*/
#define RTC_CNTL_DBG_ATTEN_DEEP_SLP  0x0000000F
#define RTC_CNTL_DBG_ATTEN_DEEP_SLP_M  ((RTC_CNTL_DBG_ATTEN_DEEP_SLP_V)<<(RTC_CNTL_DBG_ATTEN_DEEP_SLP_S))
#define RTC_CNTL_DBG_ATTEN_DEEP_SLP_V  0xF
#define RTC_CNTL_DBG_ATTEN_DEEP_SLP_S  18
/* RTC_CNTL_BIAS_SLEEP_MONITOR : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: bias_sleep when rtc in monitor state*/
#define RTC_CNTL_BIAS_SLEEP_MONITOR  (BIT(17))
#define RTC_CNTL_BIAS_SLEEP_MONITOR_M  (BIT(17))
#define RTC_CNTL_BIAS_SLEEP_MONITOR_V  0x1
#define RTC_CNTL_BIAS_SLEEP_MONITOR_S  17
/* RTC_CNTL_BIAS_SLEEP_DEEP_SLP : R/W ;bitpos:[16] ;default: 1'b1 ; */
/*description: bias_sleep when rtc in sleep_state*/
#define RTC_CNTL_BIAS_SLEEP_DEEP_SLP  (BIT(16))
#define RTC_CNTL_BIAS_SLEEP_DEEP_SLP_M  (BIT(16))
#define RTC_CNTL_BIAS_SLEEP_DEEP_SLP_V  0x1
#define RTC_CNTL_BIAS_SLEEP_DEEP_SLP_S  16
/* RTC_CNTL_PD_CUR_MONITOR : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: xpd cur when rtc in monitor state*/
#define RTC_CNTL_PD_CUR_MONITOR  (BIT(15))
#define RTC_CNTL_PD_CUR_MONITOR_M  (BIT(15))
#define RTC_CNTL_PD_CUR_MONITOR_V  0x1
#define RTC_CNTL_PD_CUR_MONITOR_S  15
/* RTC_CNTL_PD_CUR_DEEP_SLP : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: xpd cur when rtc in sleep_state*/
#define RTC_CNTL_PD_CUR_DEEP_SLP  (BIT(14))
#define RTC_CNTL_PD_CUR_DEEP_SLP_M  (BIT(14))
#define RTC_CNTL_PD_CUR_DEEP_SLP_V  0x1
#define RTC_CNTL_PD_CUR_DEEP_SLP_S  14
/* RTC_CNTL_BIAS_BUF_MONITOR : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_BIAS_BUF_MONITOR  (BIT(13))
#define RTC_CNTL_BIAS_BUF_MONITOR_M  (BIT(13))
#define RTC_CNTL_BIAS_BUF_MONITOR_V  0x1
#define RTC_CNTL_BIAS_BUF_MONITOR_S  13
/* RTC_CNTL_BIAS_BUF_DEEP_SLP : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_BIAS_BUF_DEEP_SLP  (BIT(12))
#define RTC_CNTL_BIAS_BUF_DEEP_SLP_M  (BIT(12))
#define RTC_CNTL_BIAS_BUF_DEEP_SLP_V  0x1
#define RTC_CNTL_BIAS_BUF_DEEP_SLP_S  12
/* RTC_CNTL_BIAS_BUF_WAKE : R/W ;bitpos:[11] ;default: 1'b1 ; */
/*description: */
#define RTC_CNTL_BIAS_BUF_WAKE  (BIT(11))
#define RTC_CNTL_BIAS_BUF_WAKE_M  (BIT(11))
#define RTC_CNTL_BIAS_BUF_WAKE_V  0x1
#define RTC_CNTL_BIAS_BUF_WAKE_S  11
/* RTC_CNTL_BIAS_BUF_IDLE : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_BIAS_BUF_IDLE  (BIT(10))
#define RTC_CNTL_BIAS_BUF_IDLE_M  (BIT(10))
#define RTC_CNTL_BIAS_BUF_IDLE_V  0x1
#define RTC_CNTL_BIAS_BUF_IDLE_S  10
/* RTC_CNTL_DG_VDD_DRV_B_SLP_EN : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_DG_VDD_DRV_B_SLP_EN  (BIT(8))
#define RTC_CNTL_DG_VDD_DRV_B_SLP_EN_M  (BIT(8))
#define RTC_CNTL_DG_VDD_DRV_B_SLP_EN_V  0x1
#define RTC_CNTL_DG_VDD_DRV_B_SLP_EN_S  8
/* RTC_CNTL_DG_VDD_DRV_B_SLP : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define RTC_CNTL_DG_VDD_DRV_B_SLP  0x000000FF
#define RTC_CNTL_DG_VDD_DRV_B_SLP_M  ((RTC_CNTL_DG_VDD_DRV_B_SLP_V)<<(RTC_CNTL_DG_VDD_DRV_B_SLP_S))
#define RTC_CNTL_DG_VDD_DRV_B_SLP_V  0xFF
#define RTC_CNTL_DG_VDD_DRV_B_SLP_S  0

#define RTC_CNTL_REG          (DR_REG_RTCCNTL_BASE + 0x0080)
/* RTC_CNTL_REGULATOR_FORCE_PU : R/W ;bitpos:[31] ;default: 1'd1 ; */
/*description: */
#define RTC_CNTL_REGULATOR_FORCE_PU  (BIT(31))
#define RTC_CNTL_REGULATOR_FORCE_PU_M  (BIT(31))
#define RTC_CNTL_REGULATOR_FORCE_PU_V  0x1
#define RTC_CNTL_REGULATOR_FORCE_PU_S  31
/* RTC_CNTL_REGULATOR_FORCE_PD : R/W ;bitpos:[30] ;default: 1'd0 ; */
/*description: RTC_REG force power down (for RTC_REG power down means decrease
 the voltage to 0.8v or lower )*/
#define RTC_CNTL_REGULATOR_FORCE_PD  (BIT(30))
#define RTC_CNTL_REGULATOR_FORCE_PD_M  (BIT(30))
#define RTC_CNTL_REGULATOR_FORCE_PD_V  0x1
#define RTC_CNTL_REGULATOR_FORCE_PD_S  30
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

/* RTC_CNTL_SCK_DCAP : R/W ;bitpos:[21:14] ;default: 8'd0 ; */
/*description: SCK_DCAP*/
#define RTC_CNTL_SCK_DCAP  0x000000FF
#define RTC_CNTL_SCK_DCAP_M  ((RTC_CNTL_SCK_DCAP_V)<<(RTC_CNTL_SCK_DCAP_S))
#define RTC_CNTL_SCK_DCAP_V  0xFF
#define RTC_CNTL_SCK_DCAP_S  14
#define RTC_CNTL_SCK_DCAP_DEFAULT   255
/* RTC_CNTL_DIG_CAL_EN : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_DIG_CAL_EN  (BIT(7))
#define RTC_CNTL_DIG_CAL_EN_M  (BIT(7))
#define RTC_CNTL_DIG_CAL_EN_V  0x1
#define RTC_CNTL_DIG_CAL_EN_S  7

#define RTC_CNTL_PWC_REG          (DR_REG_RTCCNTL_BASE + 0x0084)
/* RTC_CNTL_PAD_FORCE_HOLD : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: rtc pad force hold*/
#define RTC_CNTL_PAD_FORCE_HOLD  (BIT(21))
#define RTC_CNTL_PAD_FORCE_HOLD_M  (BIT(21))
#define RTC_CNTL_PAD_FORCE_HOLD_V  0x1
#define RTC_CNTL_PAD_FORCE_HOLD_S  21

#define RTC_CNTL_DIG_PWC_REG          (DR_REG_RTCCNTL_BASE + 0x0088)
/* RTC_CNTL_DG_WRAP_PD_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_DG_WRAP_PD_EN  (BIT(31))
#define RTC_CNTL_DG_WRAP_PD_EN_M  (BIT(31))
#define RTC_CNTL_DG_WRAP_PD_EN_V  0x1
#define RTC_CNTL_DG_WRAP_PD_EN_S  31
/* RTC_CNTL_WIFI_PD_EN : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: enable power down wifi in sleep*/
#define RTC_CNTL_WIFI_PD_EN  (BIT(30))
#define RTC_CNTL_WIFI_PD_EN_M  (BIT(30))
#define RTC_CNTL_WIFI_PD_EN_V  0x1
#define RTC_CNTL_WIFI_PD_EN_S  30
/* RTC_CNTL_CPU_TOP_PD_EN : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_CPU_TOP_PD_EN  (BIT(29))
#define RTC_CNTL_CPU_TOP_PD_EN_M  (BIT(29))
#define RTC_CNTL_CPU_TOP_PD_EN_V  0x1
#define RTC_CNTL_CPU_TOP_PD_EN_S  29
/* RTC_CNTL_DG_PERI_PD_EN : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_DG_PERI_PD_EN  (BIT(28))
#define RTC_CNTL_DG_PERI_PD_EN_M  (BIT(28))
#define RTC_CNTL_DG_PERI_PD_EN_V  0x1
#define RTC_CNTL_DG_PERI_PD_EN_S  28
/* RTC_CNTL_BT_PD_EN : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_BT_PD_EN  (BIT(27))
#define RTC_CNTL_BT_PD_EN_M  (BIT(27))
#define RTC_CNTL_BT_PD_EN_V  0x1
#define RTC_CNTL_BT_PD_EN_S  27
/* RTC_CNTL_CPU_TOP_FORCE_PU : R/W ;bitpos:[22] ;default: 1'b1 ; */
/*description: */
#define RTC_CNTL_CPU_TOP_FORCE_PU  (BIT(22))
#define RTC_CNTL_CPU_TOP_FORCE_PU_M  (BIT(22))
#define RTC_CNTL_CPU_TOP_FORCE_PU_V  0x1
#define RTC_CNTL_CPU_TOP_FORCE_PU_S  22
/* RTC_CNTL_CPU_TOP_FORCE_PD : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_CPU_TOP_FORCE_PD  (BIT(21))
#define RTC_CNTL_CPU_TOP_FORCE_PD_M  (BIT(21))
#define RTC_CNTL_CPU_TOP_FORCE_PD_V  0x1
#define RTC_CNTL_CPU_TOP_FORCE_PD_S  21
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
/* RTC_CNTL_FASTMEM_FORCE_LPU : R/W ;bitpos:[16] ;default: 1'b1 ; */
/*description: */
#define RTC_CNTL_FASTMEM_FORCE_LPU  (BIT(16))
#define RTC_CNTL_FASTMEM_FORCE_LPU_M  (BIT(16))
#define RTC_CNTL_FASTMEM_FORCE_LPU_V  0x1
#define RTC_CNTL_FASTMEM_FORCE_LPU_S  16
/* RTC_CNTL_FASTMEM_FORCE_LPD : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_FASTMEM_FORCE_LPD  (BIT(15))
#define RTC_CNTL_FASTMEM_FORCE_LPD_M  (BIT(15))
#define RTC_CNTL_FASTMEM_FORCE_LPD_V  0x1
#define RTC_CNTL_FASTMEM_FORCE_LPD_S  15
/* RTC_CNTL_DG_PERI_FORCE_PU : R/W ;bitpos:[14] ;default: 1'b1 ; */
/*description: */
#define RTC_CNTL_DG_PERI_FORCE_PU  (BIT(14))
#define RTC_CNTL_DG_PERI_FORCE_PU_M  (BIT(14))
#define RTC_CNTL_DG_PERI_FORCE_PU_V  0x1
#define RTC_CNTL_DG_PERI_FORCE_PU_S  14
/* RTC_CNTL_DG_PERI_FORCE_PD : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_DG_PERI_FORCE_PD  (BIT(13))
#define RTC_CNTL_DG_PERI_FORCE_PD_M  (BIT(13))
#define RTC_CNTL_DG_PERI_FORCE_PD_V  0x1
#define RTC_CNTL_DG_PERI_FORCE_PD_S  13
/* RTC_CNTL_BT_FORCE_PU : R/W ;bitpos:[12] ;default: 1'd1 ; */
/*description: */
#define RTC_CNTL_BT_FORCE_PU  (BIT(12))
#define RTC_CNTL_BT_FORCE_PU_M  (BIT(12))
#define RTC_CNTL_BT_FORCE_PU_V  0x1
#define RTC_CNTL_BT_FORCE_PU_S  12
/* RTC_CNTL_BT_FORCE_PD : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_BT_FORCE_PD  (BIT(11))
#define RTC_CNTL_BT_FORCE_PD_M  (BIT(11))
#define RTC_CNTL_BT_FORCE_PD_V  0x1
#define RTC_CNTL_BT_FORCE_PD_S  11
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
/* RTC_CNTL_VDD_SPI_PWR_FORCE : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_VDD_SPI_PWR_FORCE  (BIT(2))
#define RTC_CNTL_VDD_SPI_PWR_FORCE_M  (BIT(2))
#define RTC_CNTL_VDD_SPI_PWR_FORCE_V  0x1
#define RTC_CNTL_VDD_SPI_PWR_FORCE_S  2
/* RTC_CNTL_VDD_SPI_PWR_DRV : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define RTC_CNTL_VDD_SPI_PWR_DRV  0x00000003
#define RTC_CNTL_VDD_SPI_PWR_DRV_M  ((RTC_CNTL_VDD_SPI_PWR_DRV_V)<<(RTC_CNTL_VDD_SPI_PWR_DRV_S))
#define RTC_CNTL_VDD_SPI_PWR_DRV_V  0x3
#define RTC_CNTL_VDD_SPI_PWR_DRV_S  0

#define RTC_CNTL_DIG_ISO_REG          (DR_REG_RTCCNTL_BASE + 0x008C)
/* RTC_CNTL_DG_WRAP_FORCE_NOISO : R/W ;bitpos:[31] ;default: 1'd1 ; */
/*description: */
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
/* RTC_CNTL_CPU_TOP_FORCE_NOISO : R/W ;bitpos:[27] ;default: 1'd1 ; */
/*description: cpu force no ISO*/
#define RTC_CNTL_CPU_TOP_FORCE_NOISO  (BIT(27))
#define RTC_CNTL_CPU_TOP_FORCE_NOISO_M  (BIT(27))
#define RTC_CNTL_CPU_TOP_FORCE_NOISO_V  0x1
#define RTC_CNTL_CPU_TOP_FORCE_NOISO_S  27
/* RTC_CNTL_CPU_TOP_FORCE_ISO : R/W ;bitpos:[26] ;default: 1'd0 ; */
/*description: cpu force ISO*/
#define RTC_CNTL_CPU_TOP_FORCE_ISO  (BIT(26))
#define RTC_CNTL_CPU_TOP_FORCE_ISO_M  (BIT(26))
#define RTC_CNTL_CPU_TOP_FORCE_ISO_V  0x1
#define RTC_CNTL_CPU_TOP_FORCE_ISO_S  26
/* RTC_CNTL_DG_PERI_FORCE_NOISO : R/W ;bitpos:[25] ;default: 1'd1 ; */
/*description: */
#define RTC_CNTL_DG_PERI_FORCE_NOISO  (BIT(25))
#define RTC_CNTL_DG_PERI_FORCE_NOISO_M  (BIT(25))
#define RTC_CNTL_DG_PERI_FORCE_NOISO_V  0x1
#define RTC_CNTL_DG_PERI_FORCE_NOISO_S  25
/* RTC_CNTL_DG_PERI_FORCE_ISO : R/W ;bitpos:[24] ;default: 1'd0 ; */
/*description: */
#define RTC_CNTL_DG_PERI_FORCE_ISO  (BIT(24))
#define RTC_CNTL_DG_PERI_FORCE_ISO_M  (BIT(24))
#define RTC_CNTL_DG_PERI_FORCE_ISO_V  0x1
#define RTC_CNTL_DG_PERI_FORCE_ISO_S  24
/* RTC_CNTL_BT_FORCE_NOISO : R/W ;bitpos:[23] ;default: 1'd1 ; */
/*description: */
#define RTC_CNTL_BT_FORCE_NOISO  (BIT(23))
#define RTC_CNTL_BT_FORCE_NOISO_M  (BIT(23))
#define RTC_CNTL_BT_FORCE_NOISO_V  0x1
#define RTC_CNTL_BT_FORCE_NOISO_S  23
/* RTC_CNTL_BT_FORCE_ISO : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: */
#define RTC_CNTL_BT_FORCE_ISO  (BIT(22))
#define RTC_CNTL_BT_FORCE_ISO_M  (BIT(22))
#define RTC_CNTL_BT_FORCE_ISO_V  0x1
#define RTC_CNTL_BT_FORCE_ISO_S  22
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
/* RTC_CNTL_DIG_ISO_FORCE_OFF : R/W ;bitpos:[7] ;default: 1'd1 ; */
/*description: */
#define RTC_CNTL_DIG_ISO_FORCE_OFF  (BIT(7))
#define RTC_CNTL_DIG_ISO_FORCE_OFF_M  (BIT(7))
#define RTC_CNTL_DIG_ISO_FORCE_OFF_V  0x1
#define RTC_CNTL_DIG_ISO_FORCE_OFF_S  7

#define RTC_CNTL_WDTCONFIG0_REG          (DR_REG_RTCCNTL_BASE + 0x0090)
/* RTC_CNTL_WDT_EN : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define RTC_CNTL_WDT_EN  (BIT(31))
#define RTC_CNTL_WDT_EN_M  (BIT(31))
#define RTC_CNTL_WDT_EN_V  0x1
#define RTC_CNTL_WDT_EN_S  31
/* RTC_CNTL_WDT_STG0 : R/W ;bitpos:[30:28] ;default: 3'h0 ; */
/*description: 1: interrupt stage en*/
#define RTC_CNTL_WDT_STG0  0x00000007
#define RTC_CNTL_WDT_STG0_M  ((RTC_CNTL_WDT_STG0_V)<<(RTC_CNTL_WDT_STG0_S))
#define RTC_CNTL_WDT_STG0_V  0x7
#define RTC_CNTL_WDT_STG0_S  28
/* RTC_CNTL_WDT_STG1 : R/W ;bitpos:[27:25] ;default: 3'h0 ; */
/*description: 1: interrupt stage en*/
#define RTC_CNTL_WDT_STG1  0x00000007
#define RTC_CNTL_WDT_STG1_M  ((RTC_CNTL_WDT_STG1_V)<<(RTC_CNTL_WDT_STG1_S))
#define RTC_CNTL_WDT_STG1_V  0x7
#define RTC_CNTL_WDT_STG1_S  25
/* RTC_CNTL_WDT_STG2 : R/W ;bitpos:[24:22] ;default: 3'h0 ; */
/*description: 1: interrupt stage en*/
#define RTC_CNTL_WDT_STG2  0x00000007
#define RTC_CNTL_WDT_STG2_M  ((RTC_CNTL_WDT_STG2_V)<<(RTC_CNTL_WDT_STG2_S))
#define RTC_CNTL_WDT_STG2_V  0x7
#define RTC_CNTL_WDT_STG2_S  22
/* RTC_CNTL_WDT_STG3 : R/W ;bitpos:[21:19] ;default: 3'h0 ; */
/*description: 1: interrupt stage en*/
#define RTC_CNTL_WDT_STG3  0x00000007
#define RTC_CNTL_WDT_STG3_M  ((RTC_CNTL_WDT_STG3_V)<<(RTC_CNTL_WDT_STG3_S))
#define RTC_CNTL_WDT_STG3_V  0x7
#define RTC_CNTL_WDT_STG3_S  19
/* RTC_CNTL_WDT_STGX : */
/*description: stage action selection values */
#define RTC_WDT_STG_SEL_OFF             0
#define RTC_WDT_STG_SEL_INT             1
#define RTC_WDT_STG_SEL_RESET_CPU       2
#define RTC_WDT_STG_SEL_RESET_SYSTEM    3
#define RTC_WDT_STG_SEL_RESET_RTC       4

/* RTC_CNTL_WDT_CPU_RESET_LENGTH : R/W ;bitpos:[18:16] ;default: 3'h1 ; */
/*description: CPU reset counter length*/
#define RTC_CNTL_WDT_CPU_RESET_LENGTH  0x00000007
#define RTC_CNTL_WDT_CPU_RESET_LENGTH_M  ((RTC_CNTL_WDT_CPU_RESET_LENGTH_V)<<(RTC_CNTL_WDT_CPU_RESET_LENGTH_S))
#define RTC_CNTL_WDT_CPU_RESET_LENGTH_V  0x7
#define RTC_CNTL_WDT_CPU_RESET_LENGTH_S  16
/* RTC_CNTL_WDT_SYS_RESET_LENGTH : R/W ;bitpos:[15:13] ;default: 3'h1 ; */
/*description: system reset counter length*/
#define RTC_CNTL_WDT_SYS_RESET_LENGTH  0x00000007
#define RTC_CNTL_WDT_SYS_RESET_LENGTH_M  ((RTC_CNTL_WDT_SYS_RESET_LENGTH_V)<<(RTC_CNTL_WDT_SYS_RESET_LENGTH_S))
#define RTC_CNTL_WDT_SYS_RESET_LENGTH_V  0x7
#define RTC_CNTL_WDT_SYS_RESET_LENGTH_S  13
/* RTC_CNTL_WDT_FLASHBOOT_MOD_EN : R/W ;bitpos:[12] ;default: 1'h1 ; */
/*description: enable WDT in flash boot*/
#define RTC_CNTL_WDT_FLASHBOOT_MOD_EN  (BIT(12))
#define RTC_CNTL_WDT_FLASHBOOT_MOD_EN_M  (BIT(12))
#define RTC_CNTL_WDT_FLASHBOOT_MOD_EN_V  0x1
#define RTC_CNTL_WDT_FLASHBOOT_MOD_EN_S  12
/* RTC_CNTL_WDT_PROCPU_RESET_EN : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: enable WDT reset PRO CPU*/
#define RTC_CNTL_WDT_PROCPU_RESET_EN  (BIT(11))
#define RTC_CNTL_WDT_PROCPU_RESET_EN_M  (BIT(11))
#define RTC_CNTL_WDT_PROCPU_RESET_EN_V  0x1
#define RTC_CNTL_WDT_PROCPU_RESET_EN_S  11
/* RTC_CNTL_WDT_APPCPU_RESET_EN : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: enable WDT reset APP CPU*/
#define RTC_CNTL_WDT_APPCPU_RESET_EN  (BIT(10))
#define RTC_CNTL_WDT_APPCPU_RESET_EN_M  (BIT(10))
#define RTC_CNTL_WDT_APPCPU_RESET_EN_V  0x1
#define RTC_CNTL_WDT_APPCPU_RESET_EN_S  10
/* RTC_CNTL_WDT_PAUSE_IN_SLP : R/W ;bitpos:[9] ;default: 1'd1 ; */
/*description: pause WDT in sleep*/
#define RTC_CNTL_WDT_PAUSE_IN_SLP  (BIT(9))
#define RTC_CNTL_WDT_PAUSE_IN_SLP_M  (BIT(9))
#define RTC_CNTL_WDT_PAUSE_IN_SLP_V  0x1
#define RTC_CNTL_WDT_PAUSE_IN_SLP_S  9
/* RTC_CNTL_WDT_CHIP_RESET_EN : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: wdt reset whole chip enable*/
#define RTC_CNTL_WDT_CHIP_RESET_EN  (BIT(8))
#define RTC_CNTL_WDT_CHIP_RESET_EN_M  (BIT(8))
#define RTC_CNTL_WDT_CHIP_RESET_EN_V  0x1
#define RTC_CNTL_WDT_CHIP_RESET_EN_S  8
/* RTC_CNTL_WDT_CHIP_RESET_WIDTH : R/W ;bitpos:[7:0] ;default: 8'd20 ; */
/*description: chip reset siginal pulse width*/
#define RTC_CNTL_WDT_CHIP_RESET_WIDTH  0x000000FF
#define RTC_CNTL_WDT_CHIP_RESET_WIDTH_M  ((RTC_CNTL_WDT_CHIP_RESET_WIDTH_V)<<(RTC_CNTL_WDT_CHIP_RESET_WIDTH_S))
#define RTC_CNTL_WDT_CHIP_RESET_WIDTH_V  0xFF
#define RTC_CNTL_WDT_CHIP_RESET_WIDTH_S  0

#define RTC_CNTL_WDTCONFIG1_REG          (DR_REG_RTCCNTL_BASE + 0x0094)
/* RTC_CNTL_WDT_STG0_HOLD : R/W ;bitpos:[31:0] ;default: 32'd200000 ; */
/*description: */
#define RTC_CNTL_WDT_STG0_HOLD  0xFFFFFFFF
#define RTC_CNTL_WDT_STG0_HOLD_M  ((RTC_CNTL_WDT_STG0_HOLD_V)<<(RTC_CNTL_WDT_STG0_HOLD_S))
#define RTC_CNTL_WDT_STG0_HOLD_V  0xFFFFFFFF
#define RTC_CNTL_WDT_STG0_HOLD_S  0

#define RTC_CNTL_WDTCONFIG2_REG          (DR_REG_RTCCNTL_BASE + 0x0098)
/* RTC_CNTL_WDT_STG1_HOLD : R/W ;bitpos:[31:0] ;default: 32'd80000 ; */
/*description: */
#define RTC_CNTL_WDT_STG1_HOLD  0xFFFFFFFF
#define RTC_CNTL_WDT_STG1_HOLD_M  ((RTC_CNTL_WDT_STG1_HOLD_V)<<(RTC_CNTL_WDT_STG1_HOLD_S))
#define RTC_CNTL_WDT_STG1_HOLD_V  0xFFFFFFFF
#define RTC_CNTL_WDT_STG1_HOLD_S  0

#define RTC_CNTL_WDTCONFIG3_REG          (DR_REG_RTCCNTL_BASE + 0x009C)
/* RTC_CNTL_WDT_STG2_HOLD : R/W ;bitpos:[31:0] ;default: 32'hfff ; */
/*description: */
#define RTC_CNTL_WDT_STG2_HOLD  0xFFFFFFFF
#define RTC_CNTL_WDT_STG2_HOLD_M  ((RTC_CNTL_WDT_STG2_HOLD_V)<<(RTC_CNTL_WDT_STG2_HOLD_S))
#define RTC_CNTL_WDT_STG2_HOLD_V  0xFFFFFFFF
#define RTC_CNTL_WDT_STG2_HOLD_S  0

#define RTC_CNTL_WDTCONFIG4_REG          (DR_REG_RTCCNTL_BASE + 0x00A0)
/* RTC_CNTL_WDT_STG3_HOLD : R/W ;bitpos:[31:0] ;default: 32'hfff ; */
/*description: */
#define RTC_CNTL_WDT_STG3_HOLD  0xFFFFFFFF
#define RTC_CNTL_WDT_STG3_HOLD_M  ((RTC_CNTL_WDT_STG3_HOLD_V)<<(RTC_CNTL_WDT_STG3_HOLD_S))
#define RTC_CNTL_WDT_STG3_HOLD_V  0xFFFFFFFF
#define RTC_CNTL_WDT_STG3_HOLD_S  0

#define RTC_CNTL_WDTFEED_REG          (DR_REG_RTCCNTL_BASE + 0x00A4)
/* RTC_CNTL_WDT_FEED : WO ;bitpos:[31] ;default: 1'd0 ; */
/*description: */
#define RTC_CNTL_WDT_FEED  (BIT(31))
#define RTC_CNTL_WDT_FEED_M  (BIT(31))
#define RTC_CNTL_WDT_FEED_V  0x1
#define RTC_CNTL_WDT_FEED_S  31

#define RTC_CNTL_WDTWPROTECT_REG          (DR_REG_RTCCNTL_BASE + 0x00A8)
/* RTC_CNTL_WDT_WKEY : R/W ;bitpos:[31:0] ;default: 32'h50d83aa1 ; */
/*description: */
#define RTC_CNTL_WDT_WKEY  0xFFFFFFFF
#define RTC_CNTL_WDT_WKEY_M  ((RTC_CNTL_WDT_WKEY_V)<<(RTC_CNTL_WDT_WKEY_S))
#define RTC_CNTL_WDT_WKEY_V  0xFFFFFFFF
#define RTC_CNTL_WDT_WKEY_S  0

#define RTC_CNTL_SWD_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x00AC)
/* RTC_CNTL_SWD_AUTO_FEED_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: automatically feed swd when int comes*/
#define RTC_CNTL_SWD_AUTO_FEED_EN  (BIT(31))
#define RTC_CNTL_SWD_AUTO_FEED_EN_M  (BIT(31))
#define RTC_CNTL_SWD_AUTO_FEED_EN_V  0x1
#define RTC_CNTL_SWD_AUTO_FEED_EN_S  31
/* RTC_CNTL_SWD_DISABLE : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: disabel SWD*/
#define RTC_CNTL_SWD_DISABLE  (BIT(30))
#define RTC_CNTL_SWD_DISABLE_M  (BIT(30))
#define RTC_CNTL_SWD_DISABLE_V  0x1
#define RTC_CNTL_SWD_DISABLE_S  30
/* RTC_CNTL_SWD_FEED : WO ;bitpos:[29] ;default: 1'b0 ; */
/*description: Sw feed swd*/
#define RTC_CNTL_SWD_FEED  (BIT(29))
#define RTC_CNTL_SWD_FEED_M  (BIT(29))
#define RTC_CNTL_SWD_FEED_V  0x1
#define RTC_CNTL_SWD_FEED_S  29
/* RTC_CNTL_SWD_RST_FLAG_CLR : WO ;bitpos:[28] ;default: 1'b0 ; */
/*description: reset swd reset flag*/
#define RTC_CNTL_SWD_RST_FLAG_CLR  (BIT(28))
#define RTC_CNTL_SWD_RST_FLAG_CLR_M  (BIT(28))
#define RTC_CNTL_SWD_RST_FLAG_CLR_V  0x1
#define RTC_CNTL_SWD_RST_FLAG_CLR_S  28
/* RTC_CNTL_SWD_SIGNAL_WIDTH : R/W ;bitpos:[27:18] ;default: 10'd300 ; */
/*description: adjust signal width send to swd*/
#define RTC_CNTL_SWD_SIGNAL_WIDTH  0x000003FF
#define RTC_CNTL_SWD_SIGNAL_WIDTH_M  ((RTC_CNTL_SWD_SIGNAL_WIDTH_V)<<(RTC_CNTL_SWD_SIGNAL_WIDTH_S))
#define RTC_CNTL_SWD_SIGNAL_WIDTH_V  0x3FF
#define RTC_CNTL_SWD_SIGNAL_WIDTH_S  18
/* RTC_CNTL_SWD_BYPASS_RST : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_SWD_BYPASS_RST  (BIT(17))
#define RTC_CNTL_SWD_BYPASS_RST_M  (BIT(17))
#define RTC_CNTL_SWD_BYPASS_RST_V  0x1
#define RTC_CNTL_SWD_BYPASS_RST_S  17
/* RTC_CNTL_SWD_FEED_INT : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: swd interrupt for feeding*/
#define RTC_CNTL_SWD_FEED_INT  (BIT(1))
#define RTC_CNTL_SWD_FEED_INT_M  (BIT(1))
#define RTC_CNTL_SWD_FEED_INT_V  0x1
#define RTC_CNTL_SWD_FEED_INT_S  1
/* RTC_CNTL_SWD_RESET_FLAG : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: swd reset flag*/
#define RTC_CNTL_SWD_RESET_FLAG  (BIT(0))
#define RTC_CNTL_SWD_RESET_FLAG_M  (BIT(0))
#define RTC_CNTL_SWD_RESET_FLAG_V  0x1
#define RTC_CNTL_SWD_RESET_FLAG_S  0

#define RTC_CNTL_SWD_WPROTECT_REG          (DR_REG_RTCCNTL_BASE + 0x00B0)
/* RTC_CNTL_SWD_WKEY : R/W ;bitpos:[31:0] ;default: 32'h8f1d312a ; */
/*description: */
#define RTC_CNTL_SWD_WKEY  0xFFFFFFFF
#define RTC_CNTL_SWD_WKEY_M  ((RTC_CNTL_SWD_WKEY_V)<<(RTC_CNTL_SWD_WKEY_S))
#define RTC_CNTL_SWD_WKEY_V  0xFFFFFFFF
#define RTC_CNTL_SWD_WKEY_S  0

#define RTC_CNTL_SW_CPU_STALL_REG          (DR_REG_RTCCNTL_BASE + 0x00B4)
/* RTC_CNTL_SW_STALL_PROCPU_C1 : R/W ;bitpos:[31:26] ;default: 6'b0 ; */
/*description: */
#define RTC_CNTL_SW_STALL_PROCPU_C1  0x0000003F
#define RTC_CNTL_SW_STALL_PROCPU_C1_M  ((RTC_CNTL_SW_STALL_PROCPU_C1_V)<<(RTC_CNTL_SW_STALL_PROCPU_C1_S))
#define RTC_CNTL_SW_STALL_PROCPU_C1_V  0x3F
#define RTC_CNTL_SW_STALL_PROCPU_C1_S  26
/* RTC_CNTL_SW_STALL_APPCPU_C1 : R/W ;bitpos:[25:20] ;default: 6'b0 ; */
/*description: {reg_sw_stall_appcpu_c1[5:0]*/
#define RTC_CNTL_SW_STALL_APPCPU_C1  0x0000003F
#define RTC_CNTL_SW_STALL_APPCPU_C1_M  ((RTC_CNTL_SW_STALL_APPCPU_C1_V)<<(RTC_CNTL_SW_STALL_APPCPU_C1_S))
#define RTC_CNTL_SW_STALL_APPCPU_C1_V  0x3F
#define RTC_CNTL_SW_STALL_APPCPU_C1_S  20

#define RTC_CNTL_STORE4_REG          (DR_REG_RTCCNTL_BASE + 0x00B8)
/* RTC_CNTL_SCRATCH4 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: */
#define RTC_CNTL_SCRATCH4  0xFFFFFFFF
#define RTC_CNTL_SCRATCH4_M  ((RTC_CNTL_SCRATCH4_V)<<(RTC_CNTL_SCRATCH4_S))
#define RTC_CNTL_SCRATCH4_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH4_S  0

#define RTC_CNTL_STORE5_REG          (DR_REG_RTCCNTL_BASE + 0x00BC)
/* RTC_CNTL_SCRATCH5 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: */
#define RTC_CNTL_SCRATCH5  0xFFFFFFFF
#define RTC_CNTL_SCRATCH5_M  ((RTC_CNTL_SCRATCH5_V)<<(RTC_CNTL_SCRATCH5_S))
#define RTC_CNTL_SCRATCH5_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH5_S  0

#define RTC_CNTL_STORE6_REG          (DR_REG_RTCCNTL_BASE + 0x00C0)
/* RTC_CNTL_SCRATCH6 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: */
#define RTC_CNTL_SCRATCH6  0xFFFFFFFF
#define RTC_CNTL_SCRATCH6_M  ((RTC_CNTL_SCRATCH6_V)<<(RTC_CNTL_SCRATCH6_S))
#define RTC_CNTL_SCRATCH6_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH6_S  0

#define RTC_CNTL_STORE7_REG          (DR_REG_RTCCNTL_BASE + 0x00C4)
/* RTC_CNTL_SCRATCH7 : R/W ;bitpos:[31:0] ;default: 0 ; */
/*description: */
#define RTC_CNTL_SCRATCH7  0xFFFFFFFF
#define RTC_CNTL_SCRATCH7_M  ((RTC_CNTL_SCRATCH7_V)<<(RTC_CNTL_SCRATCH7_S))
#define RTC_CNTL_SCRATCH7_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH7_S  0

#define RTC_CNTL_LOW_POWER_ST_REG          (DR_REG_RTCCNTL_BASE + 0x00C8)
/* RTC_CNTL_MAIN_STATE : RO ;bitpos:[31:28] ;default: 4'd0 ; */
/*description: rtc main state machine status*/
#define RTC_CNTL_MAIN_STATE  0x0000000F
#define RTC_CNTL_MAIN_STATE_M  ((RTC_CNTL_MAIN_STATE_V)<<(RTC_CNTL_MAIN_STATE_S))
#define RTC_CNTL_MAIN_STATE_V  0xF
#define RTC_CNTL_MAIN_STATE_S  28
/* RTC_CNTL_MAIN_STATE_IN_IDLE : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: rtc main state machine is in idle state*/
#define RTC_CNTL_MAIN_STATE_IN_IDLE  (BIT(27))
#define RTC_CNTL_MAIN_STATE_IN_IDLE_M  (BIT(27))
#define RTC_CNTL_MAIN_STATE_IN_IDLE_V  0x1
#define RTC_CNTL_MAIN_STATE_IN_IDLE_S  27
/* RTC_CNTL_MAIN_STATE_IN_SLP : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: rtc main state machine is in sleep state*/
#define RTC_CNTL_MAIN_STATE_IN_SLP  (BIT(26))
#define RTC_CNTL_MAIN_STATE_IN_SLP_M  (BIT(26))
#define RTC_CNTL_MAIN_STATE_IN_SLP_V  0x1
#define RTC_CNTL_MAIN_STATE_IN_SLP_S  26
/* RTC_CNTL_MAIN_STATE_IN_WAIT_XTL : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: rtc main state machine is in wait xtal state*/
#define RTC_CNTL_MAIN_STATE_IN_WAIT_XTL  (BIT(25))
#define RTC_CNTL_MAIN_STATE_IN_WAIT_XTL_M  (BIT(25))
#define RTC_CNTL_MAIN_STATE_IN_WAIT_XTL_V  0x1
#define RTC_CNTL_MAIN_STATE_IN_WAIT_XTL_S  25
/* RTC_CNTL_MAIN_STATE_IN_WAIT_PLL : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: rtc main state machine is in wait pll state*/
#define RTC_CNTL_MAIN_STATE_IN_WAIT_PLL  (BIT(24))
#define RTC_CNTL_MAIN_STATE_IN_WAIT_PLL_M  (BIT(24))
#define RTC_CNTL_MAIN_STATE_IN_WAIT_PLL_V  0x1
#define RTC_CNTL_MAIN_STATE_IN_WAIT_PLL_S  24
/* RTC_CNTL_MAIN_STATE_IN_WAIT_8M : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: rtc main state machine is in wait 8m state*/
#define RTC_CNTL_MAIN_STATE_IN_WAIT_8M  (BIT(23))
#define RTC_CNTL_MAIN_STATE_IN_WAIT_8M_M  (BIT(23))
#define RTC_CNTL_MAIN_STATE_IN_WAIT_8M_V  0x1
#define RTC_CNTL_MAIN_STATE_IN_WAIT_8M_S  23
/* RTC_CNTL_IN_LOW_POWER_STATE : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: rtc main state machine is in the states of low power*/
#define RTC_CNTL_IN_LOW_POWER_STATE  (BIT(22))
#define RTC_CNTL_IN_LOW_POWER_STATE_M  (BIT(22))
#define RTC_CNTL_IN_LOW_POWER_STATE_V  0x1
#define RTC_CNTL_IN_LOW_POWER_STATE_S  22
/* RTC_CNTL_IN_WAKEUP_STATE : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: rtc main state machine is in the states of wakeup process*/
#define RTC_CNTL_IN_WAKEUP_STATE  (BIT(21))
#define RTC_CNTL_IN_WAKEUP_STATE_M  (BIT(21))
#define RTC_CNTL_IN_WAKEUP_STATE_V  0x1
#define RTC_CNTL_IN_WAKEUP_STATE_S  21
/* RTC_CNTL_MAIN_STATE_WAIT_END : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: rtc main state machine has been waited for some cycles*/
#define RTC_CNTL_MAIN_STATE_WAIT_END  (BIT(20))
#define RTC_CNTL_MAIN_STATE_WAIT_END_M  (BIT(20))
#define RTC_CNTL_MAIN_STATE_WAIT_END_V  0x1
#define RTC_CNTL_MAIN_STATE_WAIT_END_S  20
/* RTC_CNTL_RDY_FOR_WAKEUP : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: rtc is ready to receive wake up trigger from wake up source*/
#define RTC_CNTL_RDY_FOR_WAKEUP  (BIT(19))
#define RTC_CNTL_RDY_FOR_WAKEUP_M  (BIT(19))
#define RTC_CNTL_RDY_FOR_WAKEUP_V  0x1
#define RTC_CNTL_RDY_FOR_WAKEUP_S  19
/* RTC_CNTL_MAIN_STATE_PLL_ON : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: rtc main state machine is in states that pll should be running*/
#define RTC_CNTL_MAIN_STATE_PLL_ON  (BIT(18))
#define RTC_CNTL_MAIN_STATE_PLL_ON_M  (BIT(18))
#define RTC_CNTL_MAIN_STATE_PLL_ON_V  0x1
#define RTC_CNTL_MAIN_STATE_PLL_ON_S  18
/* RTC_CNTL_MAIN_STATE_XTAL_ISO : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: no use any more*/
#define RTC_CNTL_MAIN_STATE_XTAL_ISO  (BIT(17))
#define RTC_CNTL_MAIN_STATE_XTAL_ISO_M  (BIT(17))
#define RTC_CNTL_MAIN_STATE_XTAL_ISO_V  0x1
#define RTC_CNTL_MAIN_STATE_XTAL_ISO_S  17
/* RTC_CNTL_COCPU_STATE_DONE : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: ulp/cocpu is done*/
#define RTC_CNTL_COCPU_STATE_DONE  (BIT(16))
#define RTC_CNTL_COCPU_STATE_DONE_M  (BIT(16))
#define RTC_CNTL_COCPU_STATE_DONE_V  0x1
#define RTC_CNTL_COCPU_STATE_DONE_S  16
/* RTC_CNTL_COCPU_STATE_SLP : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: ulp/cocpu is in sleep state*/
#define RTC_CNTL_COCPU_STATE_SLP  (BIT(15))
#define RTC_CNTL_COCPU_STATE_SLP_M  (BIT(15))
#define RTC_CNTL_COCPU_STATE_SLP_V  0x1
#define RTC_CNTL_COCPU_STATE_SLP_S  15
/* RTC_CNTL_COCPU_STATE_SWITCH : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: ulp/cocpu is about to working. Switch rtc main state*/
#define RTC_CNTL_COCPU_STATE_SWITCH  (BIT(14))
#define RTC_CNTL_COCPU_STATE_SWITCH_M  (BIT(14))
#define RTC_CNTL_COCPU_STATE_SWITCH_V  0x1
#define RTC_CNTL_COCPU_STATE_SWITCH_S  14
/* RTC_CNTL_COCPU_STATE_START : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: ulp/cocpu should start to work*/
#define RTC_CNTL_COCPU_STATE_START  (BIT(13))
#define RTC_CNTL_COCPU_STATE_START_M  (BIT(13))
#define RTC_CNTL_COCPU_STATE_START_V  0x1
#define RTC_CNTL_COCPU_STATE_START_S  13
/* RTC_CNTL_TOUCH_STATE_DONE : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: touch is done*/
#define RTC_CNTL_TOUCH_STATE_DONE  (BIT(12))
#define RTC_CNTL_TOUCH_STATE_DONE_M  (BIT(12))
#define RTC_CNTL_TOUCH_STATE_DONE_V  0x1
#define RTC_CNTL_TOUCH_STATE_DONE_S  12
/* RTC_CNTL_TOUCH_STATE_SLP : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: touch is in sleep state*/
#define RTC_CNTL_TOUCH_STATE_SLP  (BIT(11))
#define RTC_CNTL_TOUCH_STATE_SLP_M  (BIT(11))
#define RTC_CNTL_TOUCH_STATE_SLP_V  0x1
#define RTC_CNTL_TOUCH_STATE_SLP_S  11
/* RTC_CNTL_TOUCH_STATE_SWITCH : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: touch is about to working. Switch rtc main state*/
#define RTC_CNTL_TOUCH_STATE_SWITCH  (BIT(10))
#define RTC_CNTL_TOUCH_STATE_SWITCH_M  (BIT(10))
#define RTC_CNTL_TOUCH_STATE_SWITCH_V  0x1
#define RTC_CNTL_TOUCH_STATE_SWITCH_S  10
/* RTC_CNTL_TOUCH_STATE_START : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: touch should start to work*/
#define RTC_CNTL_TOUCH_STATE_START  (BIT(9))
#define RTC_CNTL_TOUCH_STATE_START_M  (BIT(9))
#define RTC_CNTL_TOUCH_STATE_START_V  0x1
#define RTC_CNTL_TOUCH_STATE_START_S  9
/* RTC_CNTL_XPD_DIG : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: digital wrap power down*/
#define RTC_CNTL_XPD_DIG  (BIT(8))
#define RTC_CNTL_XPD_DIG_M  (BIT(8))
#define RTC_CNTL_XPD_DIG_V  0x1
#define RTC_CNTL_XPD_DIG_S  8
/* RTC_CNTL_DIG_ISO : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: digital wrap iso*/
#define RTC_CNTL_DIG_ISO  (BIT(7))
#define RTC_CNTL_DIG_ISO_M  (BIT(7))
#define RTC_CNTL_DIG_ISO_V  0x1
#define RTC_CNTL_DIG_ISO_S  7
/* RTC_CNTL_XPD_WIFI : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: wifi wrap power down*/
#define RTC_CNTL_XPD_WIFI  (BIT(6))
#define RTC_CNTL_XPD_WIFI_M  (BIT(6))
#define RTC_CNTL_XPD_WIFI_V  0x1
#define RTC_CNTL_XPD_WIFI_S  6
/* RTC_CNTL_WIFI_ISO : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: wifi iso*/
#define RTC_CNTL_WIFI_ISO  (BIT(5))
#define RTC_CNTL_WIFI_ISO_M  (BIT(5))
#define RTC_CNTL_WIFI_ISO_V  0x1
#define RTC_CNTL_WIFI_ISO_S  5
/* RTC_CNTL_XPD_RTC_PERI : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: rtc peripheral power down*/
#define RTC_CNTL_XPD_RTC_PERI  (BIT(4))
#define RTC_CNTL_XPD_RTC_PERI_M  (BIT(4))
#define RTC_CNTL_XPD_RTC_PERI_V  0x1
#define RTC_CNTL_XPD_RTC_PERI_S  4
/* RTC_CNTL_PERI_ISO : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: rtc peripheral iso*/
#define RTC_CNTL_PERI_ISO  (BIT(3))
#define RTC_CNTL_PERI_ISO_M  (BIT(3))
#define RTC_CNTL_PERI_ISO_V  0x1
#define RTC_CNTL_PERI_ISO_S  3
/* RTC_CNTL_XPD_DIG_DCDC : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: External DCDC power down*/
#define RTC_CNTL_XPD_DIG_DCDC  (BIT(2))
#define RTC_CNTL_XPD_DIG_DCDC_M  (BIT(2))
#define RTC_CNTL_XPD_DIG_DCDC_V  0x1
#define RTC_CNTL_XPD_DIG_DCDC_S  2
/* RTC_CNTL_XPD_ROM0 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: rom0 power down*/
#define RTC_CNTL_XPD_ROM0  (BIT(0))
#define RTC_CNTL_XPD_ROM0_M  (BIT(0))
#define RTC_CNTL_XPD_ROM0_V  0x1
#define RTC_CNTL_XPD_ROM0_S  0

#define RTC_CNTL_DIAG0_REG          (DR_REG_RTCCNTL_BASE + 0x00CC)
/* RTC_CNTL_LOW_POWER_DIAG1 : RO ;bitpos:[31:0] ;default: 0 ; */
/*description: */
#define RTC_CNTL_LOW_POWER_DIAG1  0xFFFFFFFF
#define RTC_CNTL_LOW_POWER_DIAG1_M  ((RTC_CNTL_LOW_POWER_DIAG1_V)<<(RTC_CNTL_LOW_POWER_DIAG1_S))
#define RTC_CNTL_LOW_POWER_DIAG1_V  0xFFFFFFFF
#define RTC_CNTL_LOW_POWER_DIAG1_S  0

#define RTC_CNTL_PAD_HOLD_REG          (DR_REG_RTCCNTL_BASE + 0x00D0)
/* RTC_CNTL_GPIO_PIN5_HOLD : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN5_HOLD  (BIT(5))
#define RTC_CNTL_GPIO_PIN5_HOLD_M  (BIT(5))
#define RTC_CNTL_GPIO_PIN5_HOLD_V  0x1
#define RTC_CNTL_GPIO_PIN5_HOLD_S  5
/* RTC_CNTL_GPIO_PIN4_HOLD : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN4_HOLD  (BIT(4))
#define RTC_CNTL_GPIO_PIN4_HOLD_M  (BIT(4))
#define RTC_CNTL_GPIO_PIN4_HOLD_V  0x1
#define RTC_CNTL_GPIO_PIN4_HOLD_S  4
/* RTC_CNTL_GPIO_PIN3_HOLD : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN3_HOLD  (BIT(3))
#define RTC_CNTL_GPIO_PIN3_HOLD_M  (BIT(3))
#define RTC_CNTL_GPIO_PIN3_HOLD_V  0x1
#define RTC_CNTL_GPIO_PIN3_HOLD_S  3
/* RTC_CNTL_GPIO_PIN2_HOLD : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN2_HOLD  (BIT(2))
#define RTC_CNTL_GPIO_PIN2_HOLD_M  (BIT(2))
#define RTC_CNTL_GPIO_PIN2_HOLD_V  0x1
#define RTC_CNTL_GPIO_PIN2_HOLD_S  2
/* RTC_CNTL_GPIO_PIN1_HOLD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN1_HOLD  (BIT(1))
#define RTC_CNTL_GPIO_PIN1_HOLD_M  (BIT(1))
#define RTC_CNTL_GPIO_PIN1_HOLD_V  0x1
#define RTC_CNTL_GPIO_PIN1_HOLD_S  1
/* RTC_CNTL_GPIO_PIN0_HOLD : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN0_HOLD  (BIT(0))
#define RTC_CNTL_GPIO_PIN0_HOLD_M  (BIT(0))
#define RTC_CNTL_GPIO_PIN0_HOLD_V  0x1
#define RTC_CNTL_GPIO_PIN0_HOLD_S  0

#define RTC_CNTL_DIG_PAD_HOLD_REG          (DR_REG_RTCCNTL_BASE + 0x00D4)
/* RTC_CNTL_DIG_PAD_HOLD : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define RTC_CNTL_DIG_PAD_HOLD  0xFFFFFFFF
#define RTC_CNTL_DIG_PAD_HOLD_M  ((RTC_CNTL_DIG_PAD_HOLD_V)<<(RTC_CNTL_DIG_PAD_HOLD_S))
#define RTC_CNTL_DIG_PAD_HOLD_V  0xFFFFFFFF
#define RTC_CNTL_DIG_PAD_HOLD_S  0

#define RTC_CNTL_BROWN_OUT_REG          (DR_REG_RTCCNTL_BASE + 0x00D8)
/* RTC_CNTL_BROWN_OUT_DET : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_BROWN_OUT_DET  (BIT(31))
#define RTC_CNTL_BROWN_OUT_DET_M  (BIT(31))
#define RTC_CNTL_BROWN_OUT_DET_V  0x1
#define RTC_CNTL_BROWN_OUT_DET_S  31
/* RTC_CNTL_BROWN_OUT_ENA : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: enable brown out*/
#define RTC_CNTL_BROWN_OUT_ENA  (BIT(30))
#define RTC_CNTL_BROWN_OUT_ENA_M  (BIT(30))
#define RTC_CNTL_BROWN_OUT_ENA_V  0x1
#define RTC_CNTL_BROWN_OUT_ENA_S  30
/* RTC_CNTL_BROWN_OUT_CNT_CLR : WO ;bitpos:[29] ;default: 1'b0 ; */
/*description: clear brown out counter*/
#define RTC_CNTL_BROWN_OUT_CNT_CLR  (BIT(29))
#define RTC_CNTL_BROWN_OUT_CNT_CLR_M  (BIT(29))
#define RTC_CNTL_BROWN_OUT_CNT_CLR_V  0x1
#define RTC_CNTL_BROWN_OUT_CNT_CLR_S  29
/* RTC_CNTL_BROWN_OUT_ANA_RST_EN : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_BROWN_OUT_ANA_RST_EN  (BIT(28))
#define RTC_CNTL_BROWN_OUT_ANA_RST_EN_M  (BIT(28))
#define RTC_CNTL_BROWN_OUT_ANA_RST_EN_V  0x1
#define RTC_CNTL_BROWN_OUT_ANA_RST_EN_S  28
/* RTC_CNTL_BROWN_OUT_RST_SEL : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: 1:  4-pos reset*/
#define RTC_CNTL_BROWN_OUT_RST_SEL  (BIT(27))
#define RTC_CNTL_BROWN_OUT_RST_SEL_M  (BIT(27))
#define RTC_CNTL_BROWN_OUT_RST_SEL_V  0x1
#define RTC_CNTL_BROWN_OUT_RST_SEL_S  27
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
/* RTC_CNTL_BROWN_OUT_INT_WAIT : R/W ;bitpos:[13:4] ;default: 10'h1 ; */
/*description: brown out interrupt wait cycles*/
#define RTC_CNTL_BROWN_OUT_INT_WAIT  0x000003FF
#define RTC_CNTL_BROWN_OUT_INT_WAIT_M  ((RTC_CNTL_BROWN_OUT_INT_WAIT_V)<<(RTC_CNTL_BROWN_OUT_INT_WAIT_S))
#define RTC_CNTL_BROWN_OUT_INT_WAIT_V  0x3FF
#define RTC_CNTL_BROWN_OUT_INT_WAIT_S  4

#define RTC_CNTL_TIME_LOW1_REG          (DR_REG_RTCCNTL_BASE + 0x00DC)
/* RTC_CNTL_TIMER_VALUE1_LOW : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: RTC timer low 32 bits*/
#define RTC_CNTL_TIMER_VALUE1_LOW  0xFFFFFFFF
#define RTC_CNTL_TIMER_VALUE1_LOW_M  ((RTC_CNTL_TIMER_VALUE1_LOW_V)<<(RTC_CNTL_TIMER_VALUE1_LOW_S))
#define RTC_CNTL_TIMER_VALUE1_LOW_V  0xFFFFFFFF
#define RTC_CNTL_TIMER_VALUE1_LOW_S  0

#define RTC_CNTL_TIME_HIGH1_REG          (DR_REG_RTCCNTL_BASE + 0x00E0)
/* RTC_CNTL_TIMER_VALUE1_HIGH : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: RTC timer high 16 bits*/
#define RTC_CNTL_TIMER_VALUE1_HIGH  0x0000FFFF
#define RTC_CNTL_TIMER_VALUE1_HIGH_M  ((RTC_CNTL_TIMER_VALUE1_HIGH_V)<<(RTC_CNTL_TIMER_VALUE1_HIGH_S))
#define RTC_CNTL_TIMER_VALUE1_HIGH_V  0xFFFF
#define RTC_CNTL_TIMER_VALUE1_HIGH_S  0

#define RTC_CNTL_XTAL32K_CLK_FACTOR_REG          (DR_REG_RTCCNTL_BASE + 0x00E4)
/* RTC_CNTL_XTAL32K_CLK_FACTOR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: xtal 32k watch dog backup clock factor*/
#define RTC_CNTL_XTAL32K_CLK_FACTOR  0xFFFFFFFF
#define RTC_CNTL_XTAL32K_CLK_FACTOR_M  ((RTC_CNTL_XTAL32K_CLK_FACTOR_V)<<(RTC_CNTL_XTAL32K_CLK_FACTOR_S))
#define RTC_CNTL_XTAL32K_CLK_FACTOR_V  0xFFFFFFFF
#define RTC_CNTL_XTAL32K_CLK_FACTOR_S  0

#define RTC_CNTL_XTAL32K_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x00E8)
/* RTC_CNTL_XTAL32K_STABLE_THRES : R/W ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: if restarted xtal32k period is smaller than this*/
#define RTC_CNTL_XTAL32K_STABLE_THRES  0x0000000F
#define RTC_CNTL_XTAL32K_STABLE_THRES_M  ((RTC_CNTL_XTAL32K_STABLE_THRES_V)<<(RTC_CNTL_XTAL32K_STABLE_THRES_S))
#define RTC_CNTL_XTAL32K_STABLE_THRES_V  0xF
#define RTC_CNTL_XTAL32K_STABLE_THRES_S  28
/* RTC_CNTL_XTAL32K_WDT_TIMEOUT : R/W ;bitpos:[27:20] ;default: 8'hff ; */
/*description: If no clock detected for this amount of time*/
#define RTC_CNTL_XTAL32K_WDT_TIMEOUT  0x000000FF
#define RTC_CNTL_XTAL32K_WDT_TIMEOUT_M  ((RTC_CNTL_XTAL32K_WDT_TIMEOUT_V)<<(RTC_CNTL_XTAL32K_WDT_TIMEOUT_S))
#define RTC_CNTL_XTAL32K_WDT_TIMEOUT_V  0xFF
#define RTC_CNTL_XTAL32K_WDT_TIMEOUT_S  20
/* RTC_CNTL_XTAL32K_RESTART_WAIT : R/W ;bitpos:[19:4] ;default: 16'h0 ; */
/*description: cycles to wait to repower on xtal 32k*/
#define RTC_CNTL_XTAL32K_RESTART_WAIT  0x0000FFFF
#define RTC_CNTL_XTAL32K_RESTART_WAIT_M  ((RTC_CNTL_XTAL32K_RESTART_WAIT_V)<<(RTC_CNTL_XTAL32K_RESTART_WAIT_S))
#define RTC_CNTL_XTAL32K_RESTART_WAIT_V  0xFFFF
#define RTC_CNTL_XTAL32K_RESTART_WAIT_S  4
/* RTC_CNTL_XTAL32K_RETURN_WAIT : R/W ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: cycles to wait to return noral xtal 32k*/
#define RTC_CNTL_XTAL32K_RETURN_WAIT  0x0000000F
#define RTC_CNTL_XTAL32K_RETURN_WAIT_M  ((RTC_CNTL_XTAL32K_RETURN_WAIT_V)<<(RTC_CNTL_XTAL32K_RETURN_WAIT_S))
#define RTC_CNTL_XTAL32K_RETURN_WAIT_V  0xF
#define RTC_CNTL_XTAL32K_RETURN_WAIT_S  0

#define RTC_CNTL_USB_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x00EC)
/* RTC_CNTL_IO_MUX_RESET_DISABLE : R/W ;bitpos:[18] ;default: 1'd0 ; */
/*description: */
#define RTC_CNTL_IO_MUX_RESET_DISABLE  (BIT(18))
#define RTC_CNTL_IO_MUX_RESET_DISABLE_M  (BIT(18))
#define RTC_CNTL_IO_MUX_RESET_DISABLE_V  0x1
#define RTC_CNTL_IO_MUX_RESET_DISABLE_S  18

#define RTC_CNTL_SLP_REJECT_CAUSE_REG          (DR_REG_RTCCNTL_BASE + 0x00F0)
/* RTC_CNTL_REJECT_CAUSE : RO ;bitpos:[17:0] ;default: 18'd0 ; */
/*description: sleep reject cause*/
#define RTC_CNTL_REJECT_CAUSE  0x0003FFFF
#define RTC_CNTL_REJECT_CAUSE_M  ((RTC_CNTL_REJECT_CAUSE_V)<<(RTC_CNTL_REJECT_CAUSE_S))
#define RTC_CNTL_REJECT_CAUSE_V  0x3FFFF
#define RTC_CNTL_REJECT_CAUSE_S  0

#define RTC_CNTL_OPTION1_REG          (DR_REG_RTCCNTL_BASE + 0x00F4)
/* RTC_CNTL_FORCE_DOWNLOAD_BOOT : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define RTC_CNTL_FORCE_DOWNLOAD_BOOT  (BIT(0))
#define RTC_CNTL_FORCE_DOWNLOAD_BOOT_M  (BIT(0))
#define RTC_CNTL_FORCE_DOWNLOAD_BOOT_V  0x1
#define RTC_CNTL_FORCE_DOWNLOAD_BOOT_S  0

#define RTC_CNTL_SLP_WAKEUP_CAUSE_REG          (DR_REG_RTCCNTL_BASE + 0x00F8)
/* RTC_CNTL_WAKEUP_CAUSE : RO ;bitpos:[16:0] ;default: 17'd0 ; */
/*description: sleep wakeup cause*/
#define RTC_CNTL_WAKEUP_CAUSE  0x0001FFFF
#define RTC_CNTL_WAKEUP_CAUSE_M  ((RTC_CNTL_WAKEUP_CAUSE_V)<<(RTC_CNTL_WAKEUP_CAUSE_S))
#define RTC_CNTL_WAKEUP_CAUSE_V  0x1FFFF
#define RTC_CNTL_WAKEUP_CAUSE_S  0

#define RTC_CNTL_ULP_CP_TIMER_1_REG          (DR_REG_RTCCNTL_BASE + 0x00FC)
/* RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE : R/W ;bitpos:[31:8] ;default: 24'd200 ; */
/*description: sleep cycles for ULP-coprocessor timer*/
#define RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE  0x00FFFFFF
#define RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE_M  ((RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE_V)<<(RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE_S))
#define RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE_V  0xFFFFFF
#define RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE_S  8

#define RTC_CNTL_INT_ENA_W1TS_REG          (DR_REG_RTCCNTL_BASE + 0x0100)
/* RTC_CNTL_BBPLL_CAL_INT_ENA_W1TS : WO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TS  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TS_M  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TS_S  20
/* RTC_CNTL_GLITCH_DET_INT_ENA_W1TS : WO ;bitpos:[19] ;default: 1'b0 ; */
/*description: enbale gitch det interrupt*/
#define RTC_CNTL_GLITCH_DET_INT_ENA_W1TS  (BIT(19))
#define RTC_CNTL_GLITCH_DET_INT_ENA_W1TS_M  (BIT(19))
#define RTC_CNTL_GLITCH_DET_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_GLITCH_DET_INT_ENA_W1TS_S  19
/* RTC_CNTL_XTAL32K_DEAD_INT_ENA_W1TS : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: enable xtal32k_dead  interrupt*/
#define RTC_CNTL_XTAL32K_DEAD_INT_ENA_W1TS  (BIT(16))
#define RTC_CNTL_XTAL32K_DEAD_INT_ENA_W1TS_M  (BIT(16))
#define RTC_CNTL_XTAL32K_DEAD_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_XTAL32K_DEAD_INT_ENA_W1TS_S  16
/* RTC_CNTL_SWD_INT_ENA_W1TS : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: enable super watch dog interrupt*/
#define RTC_CNTL_SWD_INT_ENA_W1TS  (BIT(15))
#define RTC_CNTL_SWD_INT_ENA_W1TS_M  (BIT(15))
#define RTC_CNTL_SWD_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_SWD_INT_ENA_W1TS_S  15
/* RTC_CNTL_MAIN_TIMER_INT_ENA_W1TS : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: enable RTC main timer interrupt*/
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TS  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TS_M  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TS_S  10
/* RTC_CNTL_BROWN_OUT_INT_ENA_W1TS : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: enable brown out interrupt*/
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TS  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TS_M  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TS_S  9
/* RTC_CNTL_WDT_INT_ENA_W1TS : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: enable RTC WDT interrupt*/
#define RTC_CNTL_WDT_INT_ENA_W1TS  (BIT(3))
#define RTC_CNTL_WDT_INT_ENA_W1TS_M  (BIT(3))
#define RTC_CNTL_WDT_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_WDT_INT_ENA_W1TS_S  3
/* RTC_CNTL_SLP_REJECT_INT_ENA_W1TS : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: enable sleep reject interrupt*/
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TS  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TS_M  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TS_S  1
/* RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TS : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: enable sleep wakeup interrupt*/
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TS  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TS_M  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TS_S  0

#define RTC_CNTL_INT_ENA_W1TC_REG          (DR_REG_RTCCNTL_BASE + 0x0104)
/* RTC_CNTL_BBPLL_CAL_INT_ENA_W1TC : WO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TC  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TC_M  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TC_S  20
/* RTC_CNTL_GLITCH_DET_INT_ENA_W1TC : WO ;bitpos:[19] ;default: 1'b0 ; */
/*description: enbale gitch det interrupt*/
#define RTC_CNTL_GLITCH_DET_INT_ENA_W1TC  (BIT(19))
#define RTC_CNTL_GLITCH_DET_INT_ENA_W1TC_M  (BIT(19))
#define RTC_CNTL_GLITCH_DET_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_GLITCH_DET_INT_ENA_W1TC_S  19
/* RTC_CNTL_XTAL32K_DEAD_INT_ENA_W1TC : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: enable xtal32k_dead  interrupt*/
#define RTC_CNTL_XTAL32K_DEAD_INT_ENA_W1TC  (BIT(16))
#define RTC_CNTL_XTAL32K_DEAD_INT_ENA_W1TC_M  (BIT(16))
#define RTC_CNTL_XTAL32K_DEAD_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_XTAL32K_DEAD_INT_ENA_W1TC_S  16
/* RTC_CNTL_SWD_INT_ENA_W1TC : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: enable super watch dog interrupt*/
#define RTC_CNTL_SWD_INT_ENA_W1TC  (BIT(15))
#define RTC_CNTL_SWD_INT_ENA_W1TC_M  (BIT(15))
#define RTC_CNTL_SWD_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_SWD_INT_ENA_W1TC_S  15
/* RTC_CNTL_MAIN_TIMER_INT_ENA_W1TC : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: enable RTC main timer interrupt*/
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TC  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TC_M  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TC_S  10
/* RTC_CNTL_BROWN_OUT_INT_ENA_W1TC : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: enable brown out interrupt*/
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TC  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TC_M  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TC_S  9
/* RTC_CNTL_WDT_INT_ENA_W1TC : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: enable RTC WDT interrupt*/
#define RTC_CNTL_WDT_INT_ENA_W1TC  (BIT(3))
#define RTC_CNTL_WDT_INT_ENA_W1TC_M  (BIT(3))
#define RTC_CNTL_WDT_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_WDT_INT_ENA_W1TC_S  3
/* RTC_CNTL_SLP_REJECT_INT_ENA_W1TC : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: enable sleep reject interrupt*/
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TC  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TC_M  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TC_S  1
/* RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TC : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: enable sleep wakeup interrupt*/
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TC  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TC_M  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TC_S  0

#define RTC_CNTL_RETENTION_CTRL_REG          (DR_REG_RTCCNTL_BASE + 0x0108)
/* RTC_CNTL_RETENTION_WAIT : R/W ;bitpos:[31:27] ;default: 5'd20 ; */
/*description: wait cycles for rention operation*/
#define RTC_CNTL_RETENTION_WAIT  0x0000001F
#define RTC_CNTL_RETENTION_WAIT_M  ((RTC_CNTL_RETENTION_WAIT_V)<<(RTC_CNTL_RETENTION_WAIT_S))
#define RTC_CNTL_RETENTION_WAIT_V  0x1F
#define RTC_CNTL_RETENTION_WAIT_S  27
/* RTC_CNTL_RETENTION_EN : R/W ;bitpos:[26] ;default: 1'd0 ; */
/*description: */
#define RTC_CNTL_RETENTION_EN  (BIT(26))
#define RTC_CNTL_RETENTION_EN_M  (BIT(26))
#define RTC_CNTL_RETENTION_EN_V  0x1
#define RTC_CNTL_RETENTION_EN_S  26
/* RTC_CNTL_RETENTION_CLKOFF_WAIT : R/W ;bitpos:[25:22] ;default: 4'd3 ; */
/*description: */
#define RTC_CNTL_RETENTION_CLKOFF_WAIT  0x0000000F
#define RTC_CNTL_RETENTION_CLKOFF_WAIT_M  ((RTC_CNTL_RETENTION_CLKOFF_WAIT_V)<<(RTC_CNTL_RETENTION_CLKOFF_WAIT_S))
#define RTC_CNTL_RETENTION_CLKOFF_WAIT_V  0xF
#define RTC_CNTL_RETENTION_CLKOFF_WAIT_S  22
/* RTC_CNTL_RETENTION_DONE_WAIT : R/W ;bitpos:[21:19] ;default: 3'd2 ; */
/*description: */
#define RTC_CNTL_RETENTION_DONE_WAIT  0x00000007
#define RTC_CNTL_RETENTION_DONE_WAIT_M  ((RTC_CNTL_RETENTION_DONE_WAIT_V)<<(RTC_CNTL_RETENTION_DONE_WAIT_S))
#define RTC_CNTL_RETENTION_DONE_WAIT_V  0x7
#define RTC_CNTL_RETENTION_DONE_WAIT_S  19
/* RTC_CNTL_RETENTION_CLK_SEL : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_RETENTION_CLK_SEL  (BIT(18))
#define RTC_CNTL_RETENTION_CLK_SEL_M  (BIT(18))
#define RTC_CNTL_RETENTION_CLK_SEL_V  0x1
#define RTC_CNTL_RETENTION_CLK_SEL_S  18

#define RTC_CNTL_FIB_SEL_REG          (DR_REG_RTCCNTL_BASE + 0x010C)
/* RTC_CNTL_FIB_SEL : R/W ;bitpos:[2:0] ;default: 3'd7 ; */
/*description: select use analog fib signal*/
#define RTC_CNTL_FIB_SEL  0x00000007
#define RTC_CNTL_FIB_SEL_M  ((RTC_CNTL_FIB_SEL_V)<<(RTC_CNTL_FIB_SEL_S))
#define RTC_CNTL_FIB_SEL_V  0x7
#define RTC_CNTL_FIB_SEL_S  0

#define RTC_CNTL_FIB_GLITCH_RST BIT(0)
#define RTC_CNTL_FIB_BOD_RST BIT(1)
#define RTC_CNTL_FIB_SUPER_WDT_RST BIT(2)

#define RTC_CNTL_GPIO_WAKEUP_REG          (DR_REG_RTCCNTL_BASE + 0x0110)
/* RTC_CNTL_GPIO_PIN0_WAKEUP_ENABLE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN0_WAKEUP_ENABLE  (BIT(31))
#define RTC_CNTL_GPIO_PIN0_WAKEUP_ENABLE_M  (BIT(31))
#define RTC_CNTL_GPIO_PIN0_WAKEUP_ENABLE_V  0x1
#define RTC_CNTL_GPIO_PIN0_WAKEUP_ENABLE_S  31
/* RTC_CNTL_GPIO_PIN1_WAKEUP_ENABLE : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN1_WAKEUP_ENABLE  (BIT(30))
#define RTC_CNTL_GPIO_PIN1_WAKEUP_ENABLE_M  (BIT(30))
#define RTC_CNTL_GPIO_PIN1_WAKEUP_ENABLE_V  0x1
#define RTC_CNTL_GPIO_PIN1_WAKEUP_ENABLE_S  30
/* RTC_CNTL_GPIO_PIN2_WAKEUP_ENABLE : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN2_WAKEUP_ENABLE  (BIT(29))
#define RTC_CNTL_GPIO_PIN2_WAKEUP_ENABLE_M  (BIT(29))
#define RTC_CNTL_GPIO_PIN2_WAKEUP_ENABLE_V  0x1
#define RTC_CNTL_GPIO_PIN2_WAKEUP_ENABLE_S  29
/* RTC_CNTL_GPIO_PIN3_WAKEUP_ENABLE : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN3_WAKEUP_ENABLE  (BIT(28))
#define RTC_CNTL_GPIO_PIN3_WAKEUP_ENABLE_M  (BIT(28))
#define RTC_CNTL_GPIO_PIN3_WAKEUP_ENABLE_V  0x1
#define RTC_CNTL_GPIO_PIN3_WAKEUP_ENABLE_S  28
/* RTC_CNTL_GPIO_PIN4_WAKEUP_ENABLE : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN4_WAKEUP_ENABLE  (BIT(27))
#define RTC_CNTL_GPIO_PIN4_WAKEUP_ENABLE_M  (BIT(27))
#define RTC_CNTL_GPIO_PIN4_WAKEUP_ENABLE_V  0x1
#define RTC_CNTL_GPIO_PIN4_WAKEUP_ENABLE_S  27
/* RTC_CNTL_GPIO_PIN5_WAKEUP_ENABLE : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN5_WAKEUP_ENABLE  (BIT(26))
#define RTC_CNTL_GPIO_PIN5_WAKEUP_ENABLE_M  (BIT(26))
#define RTC_CNTL_GPIO_PIN5_WAKEUP_ENABLE_V  0x1
#define RTC_CNTL_GPIO_PIN5_WAKEUP_ENABLE_S  26
/* RTC_CNTL_GPIO_PIN0_INT_TYPE : R/W ;bitpos:[25:23] ;default: 3'd0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN0_INT_TYPE  0x00000007
#define RTC_CNTL_GPIO_PIN0_INT_TYPE_M  ((RTC_CNTL_GPIO_PIN0_INT_TYPE_V)<<(RTC_CNTL_GPIO_PIN0_INT_TYPE_S))
#define RTC_CNTL_GPIO_PIN0_INT_TYPE_V  0x7
#define RTC_CNTL_GPIO_PIN0_INT_TYPE_S  23
/* RTC_CNTL_GPIO_PIN1_INT_TYPE : R/W ;bitpos:[22:20] ;default: 3'd0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN1_INT_TYPE  0x00000007
#define RTC_CNTL_GPIO_PIN1_INT_TYPE_M  ((RTC_CNTL_GPIO_PIN1_INT_TYPE_V)<<(RTC_CNTL_GPIO_PIN1_INT_TYPE_S))
#define RTC_CNTL_GPIO_PIN1_INT_TYPE_V  0x7
#define RTC_CNTL_GPIO_PIN1_INT_TYPE_S  20
/* RTC_CNTL_GPIO_PIN2_INT_TYPE : R/W ;bitpos:[19:17] ;default: 3'd0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN2_INT_TYPE  0x00000007
#define RTC_CNTL_GPIO_PIN2_INT_TYPE_M  ((RTC_CNTL_GPIO_PIN2_INT_TYPE_V)<<(RTC_CNTL_GPIO_PIN2_INT_TYPE_S))
#define RTC_CNTL_GPIO_PIN2_INT_TYPE_V  0x7
#define RTC_CNTL_GPIO_PIN2_INT_TYPE_S  17
/* RTC_CNTL_GPIO_PIN3_INT_TYPE : R/W ;bitpos:[16:14] ;default: 3'd0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN3_INT_TYPE  0x00000007
#define RTC_CNTL_GPIO_PIN3_INT_TYPE_M  ((RTC_CNTL_GPIO_PIN3_INT_TYPE_V)<<(RTC_CNTL_GPIO_PIN3_INT_TYPE_S))
#define RTC_CNTL_GPIO_PIN3_INT_TYPE_V  0x7
#define RTC_CNTL_GPIO_PIN3_INT_TYPE_S  14
/* RTC_CNTL_GPIO_PIN4_INT_TYPE : R/W ;bitpos:[13:11] ;default: 3'd0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN4_INT_TYPE  0x00000007
#define RTC_CNTL_GPIO_PIN4_INT_TYPE_M  ((RTC_CNTL_GPIO_PIN4_INT_TYPE_V)<<(RTC_CNTL_GPIO_PIN4_INT_TYPE_S))
#define RTC_CNTL_GPIO_PIN4_INT_TYPE_V  0x7
#define RTC_CNTL_GPIO_PIN4_INT_TYPE_S  11
/* RTC_CNTL_GPIO_PIN5_INT_TYPE : R/W ;bitpos:[10:8] ;default: 3'd0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN5_INT_TYPE  0x00000007
#define RTC_CNTL_GPIO_PIN5_INT_TYPE_M  ((RTC_CNTL_GPIO_PIN5_INT_TYPE_V)<<(RTC_CNTL_GPIO_PIN5_INT_TYPE_S))
#define RTC_CNTL_GPIO_PIN5_INT_TYPE_V  0x7
#define RTC_CNTL_GPIO_PIN5_INT_TYPE_S  8
/* RTC_CNTL_GPIO_PIN_CLK_GATE : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN_CLK_GATE  (BIT(7))
#define RTC_CNTL_GPIO_PIN_CLK_GATE_M  (BIT(7))
#define RTC_CNTL_GPIO_PIN_CLK_GATE_V  0x1
#define RTC_CNTL_GPIO_PIN_CLK_GATE_S  7
/* RTC_CNTL_GPIO_WAKEUP_STATUS_CLR : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_WAKEUP_STATUS_CLR  (BIT(6))
#define RTC_CNTL_GPIO_WAKEUP_STATUS_CLR_M  (BIT(6))
#define RTC_CNTL_GPIO_WAKEUP_STATUS_CLR_V  0x1
#define RTC_CNTL_GPIO_WAKEUP_STATUS_CLR_S  6
/* RTC_CNTL_GPIO_WAKEUP_STATUS : RO ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_WAKEUP_STATUS  0x0000003F
#define RTC_CNTL_GPIO_WAKEUP_STATUS_M  ((RTC_CNTL_GPIO_WAKEUP_STATUS_V)<<(RTC_CNTL_GPIO_WAKEUP_STATUS_S))
#define RTC_CNTL_GPIO_WAKEUP_STATUS_V  0x3F
#define RTC_CNTL_GPIO_WAKEUP_STATUS_S  0

#define RTC_CNTL_DBG_SEL_REG          (DR_REG_RTCCNTL_BASE + 0x0114)
/* RTC_CNTL_DEBUG_SEL4 : R/W ;bitpos:[31:27] ;default: 5'd0 ; */
/*description: */
#define RTC_CNTL_DEBUG_SEL4  0x0000001F
#define RTC_CNTL_DEBUG_SEL4_M  ((RTC_CNTL_DEBUG_SEL4_V)<<(RTC_CNTL_DEBUG_SEL4_S))
#define RTC_CNTL_DEBUG_SEL4_V  0x1F
#define RTC_CNTL_DEBUG_SEL4_S  27
/* RTC_CNTL_DEBUG_SEL3 : R/W ;bitpos:[26:22] ;default: 5'd0 ; */
/*description: */
#define RTC_CNTL_DEBUG_SEL3  0x0000001F
#define RTC_CNTL_DEBUG_SEL3_M  ((RTC_CNTL_DEBUG_SEL3_V)<<(RTC_CNTL_DEBUG_SEL3_S))
#define RTC_CNTL_DEBUG_SEL3_V  0x1F
#define RTC_CNTL_DEBUG_SEL3_S  22
/* RTC_CNTL_DEBUG_SEL2 : R/W ;bitpos:[21:17] ;default: 5'd0 ; */
/*description: */
#define RTC_CNTL_DEBUG_SEL2  0x0000001F
#define RTC_CNTL_DEBUG_SEL2_M  ((RTC_CNTL_DEBUG_SEL2_V)<<(RTC_CNTL_DEBUG_SEL2_S))
#define RTC_CNTL_DEBUG_SEL2_V  0x1F
#define RTC_CNTL_DEBUG_SEL2_S  17
/* RTC_CNTL_DEBUG_SEL1 : R/W ;bitpos:[16:12] ;default: 5'd0 ; */
/*description: */
#define RTC_CNTL_DEBUG_SEL1  0x0000001F
#define RTC_CNTL_DEBUG_SEL1_M  ((RTC_CNTL_DEBUG_SEL1_V)<<(RTC_CNTL_DEBUG_SEL1_S))
#define RTC_CNTL_DEBUG_SEL1_V  0x1F
#define RTC_CNTL_DEBUG_SEL1_S  12
/* RTC_CNTL_DEBUG_SEL0 : R/W ;bitpos:[11:7] ;default: 5'd0 ; */
/*description: */
#define RTC_CNTL_DEBUG_SEL0  0x0000001F
#define RTC_CNTL_DEBUG_SEL0_M  ((RTC_CNTL_DEBUG_SEL0_V)<<(RTC_CNTL_DEBUG_SEL0_S))
#define RTC_CNTL_DEBUG_SEL0_V  0x1F
#define RTC_CNTL_DEBUG_SEL0_S  7
/* RTC_CNTL_DEBUG_BIT_SEL : R/W ;bitpos:[6:2] ;default: 5'd0 ; */
/*description: */
#define RTC_CNTL_DEBUG_BIT_SEL  0x0000001F
#define RTC_CNTL_DEBUG_BIT_SEL_M  ((RTC_CNTL_DEBUG_BIT_SEL_V)<<(RTC_CNTL_DEBUG_BIT_SEL_S))
#define RTC_CNTL_DEBUG_BIT_SEL_V  0x1F
#define RTC_CNTL_DEBUG_BIT_SEL_S  2
/* RTC_CNTL_DEBUG_12M_NO_GATING : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_DEBUG_12M_NO_GATING  (BIT(1))
#define RTC_CNTL_DEBUG_12M_NO_GATING_M  (BIT(1))
#define RTC_CNTL_DEBUG_12M_NO_GATING_V  0x1
#define RTC_CNTL_DEBUG_12M_NO_GATING_S  1

#define RTC_CNTL_DBG_MAP_REG          (DR_REG_RTCCNTL_BASE + 0x0118)
/* RTC_CNTL_GPIO_PIN0_FUN_SEL : R/W ;bitpos:[31:28] ;default: 4'd0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN0_FUN_SEL  0x0000000F
#define RTC_CNTL_GPIO_PIN0_FUN_SEL_M  ((RTC_CNTL_GPIO_PIN0_FUN_SEL_V)<<(RTC_CNTL_GPIO_PIN0_FUN_SEL_S))
#define RTC_CNTL_GPIO_PIN0_FUN_SEL_V  0xF
#define RTC_CNTL_GPIO_PIN0_FUN_SEL_S  28
/* RTC_CNTL_GPIO_PIN1_FUN_SEL : R/W ;bitpos:[27:24] ;default: 4'd0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN1_FUN_SEL  0x0000000F
#define RTC_CNTL_GPIO_PIN1_FUN_SEL_M  ((RTC_CNTL_GPIO_PIN1_FUN_SEL_V)<<(RTC_CNTL_GPIO_PIN1_FUN_SEL_S))
#define RTC_CNTL_GPIO_PIN1_FUN_SEL_V  0xF
#define RTC_CNTL_GPIO_PIN1_FUN_SEL_S  24
/* RTC_CNTL_GPIO_PIN2_FUN_SEL : R/W ;bitpos:[23:20] ;default: 4'd0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN2_FUN_SEL  0x0000000F
#define RTC_CNTL_GPIO_PIN2_FUN_SEL_M  ((RTC_CNTL_GPIO_PIN2_FUN_SEL_V)<<(RTC_CNTL_GPIO_PIN2_FUN_SEL_S))
#define RTC_CNTL_GPIO_PIN2_FUN_SEL_V  0xF
#define RTC_CNTL_GPIO_PIN2_FUN_SEL_S  20
/* RTC_CNTL_GPIO_PIN3_FUN_SEL : R/W ;bitpos:[19:16] ;default: 4'd0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN3_FUN_SEL  0x0000000F
#define RTC_CNTL_GPIO_PIN3_FUN_SEL_M  ((RTC_CNTL_GPIO_PIN3_FUN_SEL_V)<<(RTC_CNTL_GPIO_PIN3_FUN_SEL_S))
#define RTC_CNTL_GPIO_PIN3_FUN_SEL_V  0xF
#define RTC_CNTL_GPIO_PIN3_FUN_SEL_S  16
/* RTC_CNTL_GPIO_PIN4_FUN_SEL : R/W ;bitpos:[15:12] ;default: 4'd0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN4_FUN_SEL  0x0000000F
#define RTC_CNTL_GPIO_PIN4_FUN_SEL_M  ((RTC_CNTL_GPIO_PIN4_FUN_SEL_V)<<(RTC_CNTL_GPIO_PIN4_FUN_SEL_S))
#define RTC_CNTL_GPIO_PIN4_FUN_SEL_V  0xF
#define RTC_CNTL_GPIO_PIN4_FUN_SEL_S  12
/* RTC_CNTL_GPIO_PIN5_FUN_SEL : R/W ;bitpos:[11:8] ;default: 4'd0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN5_FUN_SEL  0x0000000F
#define RTC_CNTL_GPIO_PIN5_FUN_SEL_M  ((RTC_CNTL_GPIO_PIN5_FUN_SEL_V)<<(RTC_CNTL_GPIO_PIN5_FUN_SEL_S))
#define RTC_CNTL_GPIO_PIN5_FUN_SEL_V  0xF
#define RTC_CNTL_GPIO_PIN5_FUN_SEL_S  8
/* RTC_CNTL_GPIO_PIN0_MUX_SEL : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN0_MUX_SEL  (BIT(7))
#define RTC_CNTL_GPIO_PIN0_MUX_SEL_M  (BIT(7))
#define RTC_CNTL_GPIO_PIN0_MUX_SEL_V  0x1
#define RTC_CNTL_GPIO_PIN0_MUX_SEL_S  7
/* RTC_CNTL_GPIO_PIN1_MUX_SEL : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN1_MUX_SEL  (BIT(6))
#define RTC_CNTL_GPIO_PIN1_MUX_SEL_M  (BIT(6))
#define RTC_CNTL_GPIO_PIN1_MUX_SEL_V  0x1
#define RTC_CNTL_GPIO_PIN1_MUX_SEL_S  6
/* RTC_CNTL_GPIO_PIN2_MUX_SEL : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN2_MUX_SEL  (BIT(5))
#define RTC_CNTL_GPIO_PIN2_MUX_SEL_M  (BIT(5))
#define RTC_CNTL_GPIO_PIN2_MUX_SEL_V  0x1
#define RTC_CNTL_GPIO_PIN2_MUX_SEL_S  5
/* RTC_CNTL_GPIO_PIN3_MUX_SEL : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN3_MUX_SEL  (BIT(4))
#define RTC_CNTL_GPIO_PIN3_MUX_SEL_M  (BIT(4))
#define RTC_CNTL_GPIO_PIN3_MUX_SEL_V  0x1
#define RTC_CNTL_GPIO_PIN3_MUX_SEL_S  4
/* RTC_CNTL_GPIO_PIN4_MUX_SEL : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN4_MUX_SEL  (BIT(3))
#define RTC_CNTL_GPIO_PIN4_MUX_SEL_M  (BIT(3))
#define RTC_CNTL_GPIO_PIN4_MUX_SEL_V  0x1
#define RTC_CNTL_GPIO_PIN4_MUX_SEL_S  3
/* RTC_CNTL_GPIO_PIN5_MUX_SEL : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_GPIO_PIN5_MUX_SEL  (BIT(2))
#define RTC_CNTL_GPIO_PIN5_MUX_SEL_M  (BIT(2))
#define RTC_CNTL_GPIO_PIN5_MUX_SEL_V  0x1
#define RTC_CNTL_GPIO_PIN5_MUX_SEL_S  2

#define RTC_CNTL_SENSOR_CTRL_REG          (DR_REG_RTCCNTL_BASE + 0x011C)
/* RTC_CNTL_FORCE_XPD_SAR : R/W ;bitpos:[31:30] ;default: 2'b0 ; */
/*description: */
#define RTC_CNTL_FORCE_XPD_SAR  0x00000003
#define RTC_CNTL_FORCE_XPD_SAR_M  ((RTC_CNTL_FORCE_XPD_SAR_V)<<(RTC_CNTL_FORCE_XPD_SAR_S))
#define RTC_CNTL_FORCE_XPD_SAR_V  0x3
#define RTC_CNTL_FORCE_XPD_SAR_S  30
/* RTC_CNTL_SAR2_PWDET_CCT : R/W ;bitpos:[29:27] ;default: 3'd0 ; */
/*description: */
#define RTC_CNTL_SAR2_PWDET_CCT  0x00000007
#define RTC_CNTL_SAR2_PWDET_CCT_M  ((RTC_CNTL_SAR2_PWDET_CCT_V)<<(RTC_CNTL_SAR2_PWDET_CCT_S))
#define RTC_CNTL_SAR2_PWDET_CCT_V  0x7
#define RTC_CNTL_SAR2_PWDET_CCT_S  27

#define RTC_CNTL_DBG_SAR_SEL_REG          (DR_REG_RTCCNTL_BASE + 0x0120)
/* RTC_CNTL_SAR_DEBUG_SEL : R/W ;bitpos:[31:27] ;default: 5'd0 ; */
/*description: */
#define RTC_CNTL_SAR_DEBUG_SEL  0x0000001F
#define RTC_CNTL_SAR_DEBUG_SEL_M  ((RTC_CNTL_SAR_DEBUG_SEL_V)<<(RTC_CNTL_SAR_DEBUG_SEL_S))
#define RTC_CNTL_SAR_DEBUG_SEL_V  0x1F
#define RTC_CNTL_SAR_DEBUG_SEL_S  27

#define RTC_CNTL_PG_CTRL_REG          (DR_REG_RTCCNTL_BASE + 0x0124)
/* RTC_CNTL_POWER_GLITCH_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_POWER_GLITCH_EN  (BIT(31))
#define RTC_CNTL_POWER_GLITCH_EN_M  (BIT(31))
#define RTC_CNTL_POWER_GLITCH_EN_V  0x1
#define RTC_CNTL_POWER_GLITCH_EN_S  31
/* RTC_CNTL_POWER_GLITCH_EFUSE_SEL : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_POWER_GLITCH_EFUSE_SEL  (BIT(30))
#define RTC_CNTL_POWER_GLITCH_EFUSE_SEL_M  (BIT(30))
#define RTC_CNTL_POWER_GLITCH_EFUSE_SEL_V  0x1
#define RTC_CNTL_POWER_GLITCH_EFUSE_SEL_S  30
/* RTC_CNTL_POWER_GLITCH_FORCE_PU : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_POWER_GLITCH_FORCE_PU  (BIT(29))
#define RTC_CNTL_POWER_GLITCH_FORCE_PU_M  (BIT(29))
#define RTC_CNTL_POWER_GLITCH_FORCE_PU_V  0x1
#define RTC_CNTL_POWER_GLITCH_FORCE_PU_S  29
/* RTC_CNTL_POWER_GLITCH_FORCE_PD : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: */
#define RTC_CNTL_POWER_GLITCH_FORCE_PD  (BIT(28))
#define RTC_CNTL_POWER_GLITCH_FORCE_PD_M  (BIT(28))
#define RTC_CNTL_POWER_GLITCH_FORCE_PD_V  0x1
#define RTC_CNTL_POWER_GLITCH_FORCE_PD_S  28
/* RTC_CNTL_POWER_GLITCH_DSENSE : R/W ;bitpos:[27:26] ;default: 2'b0 ; */
/*description: */
#define RTC_CNTL_POWER_GLITCH_DSENSE  0x00000003
#define RTC_CNTL_POWER_GLITCH_DSENSE_M  ((RTC_CNTL_POWER_GLITCH_DSENSE_V)<<(RTC_CNTL_POWER_GLITCH_DSENSE_S))
#define RTC_CNTL_POWER_GLITCH_DSENSE_V  0x3
#define RTC_CNTL_POWER_GLITCH_DSENSE_S  26

#define RTC_CNTL_DATE_REG          (DR_REG_RTCCNTL_BASE + 0x01fc)
/* RTC_CNTL_CNTL_DATE : R/W ;bitpos:[27:0] ;default: 28'h2007270 ; */
/*description: */
#define RTC_CNTL_CNTL_DATE  0x0FFFFFFF
#define RTC_CNTL_CNTL_DATE_M  ((RTC_CNTL_CNTL_DATE_V)<<(RTC_CNTL_CNTL_DATE_S))
#define RTC_CNTL_CNTL_DATE_V  0xFFFFFFF
#define RTC_CNTL_CNTL_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_RTC_CNTL_REG_H_ */
