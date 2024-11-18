/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_RTC_CNTL_REG_H_
#define _SOC_RTC_CNTL_REG_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "soc/soc.h"
#define RTC_CNTL_TIME0_REG		RTC_CNTL_TIME_LOW0_REG
#define RTC_CNTL_TIME1_REG		RTC_CNTL_TIME_HIGH0_REG

#define RTC_CNTL_OPTIONS0_REG          (DR_REG_RTCCNTL_BASE + 0x0)
/* RTC_CNTL_SW_SYS_RST :  ;bitpos:[31] ;default: 1'd0 ; */
/*description: SW system reset.*/
#define RTC_CNTL_SW_SYS_RST    (BIT(31))
#define RTC_CNTL_SW_SYS_RST_M  (BIT(31))
#define RTC_CNTL_SW_SYS_RST_V  0x1
#define RTC_CNTL_SW_SYS_RST_S  31
/* RTC_CNTL_DG_WRAP_FORCE_NORST :  ;bitpos:[30] ;default: 1'd0 ; */
/*description: digital core force no reset in deep sleep.*/
#define RTC_CNTL_DG_WRAP_FORCE_NORST    (BIT(30))
#define RTC_CNTL_DG_WRAP_FORCE_NORST_M  (BIT(30))
#define RTC_CNTL_DG_WRAP_FORCE_NORST_V  0x1
#define RTC_CNTL_DG_WRAP_FORCE_NORST_S  30
/* RTC_CNTL_DG_WRAP_FORCE_RST :  ;bitpos:[29] ;default: 1'd0 ; */
/*description: digital wrap force reset in deep sleep.*/
#define RTC_CNTL_DG_WRAP_FORCE_RST    (BIT(29))
#define RTC_CNTL_DG_WRAP_FORCE_RST_M  (BIT(29))
#define RTC_CNTL_DG_WRAP_FORCE_RST_V  0x1
#define RTC_CNTL_DG_WRAP_FORCE_RST_S  29
/* RTC_CNTL_ANALOG_FORCE_NOISO :  ;bitpos:[28] ;default: 1'd1 ; */
/*description: Need add desc.*/
#define RTC_CNTL_ANALOG_FORCE_NOISO    (BIT(28))
#define RTC_CNTL_ANALOG_FORCE_NOISO_M  (BIT(28))
#define RTC_CNTL_ANALOG_FORCE_NOISO_V  0x1
#define RTC_CNTL_ANALOG_FORCE_NOISO_S  28
/* RTC_CNTL_ANALOG_FORCE_ISO :  ;bitpos:[25] ;default: 1'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_ANALOG_FORCE_ISO    (BIT(25))
#define RTC_CNTL_ANALOG_FORCE_ISO_M  (BIT(25))
#define RTC_CNTL_ANALOG_FORCE_ISO_V  0x1
#define RTC_CNTL_ANALOG_FORCE_ISO_S  25
/* RTC_CNTL_XTL_EXT_CTR_SEL :  ;bitpos:[22:20] ;default: 3'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_XTL_EXT_CTR_SEL    0x00000007
#define RTC_CNTL_XTL_EXT_CTR_SEL_M  ((RTC_CNTL_XTL_EXT_CTR_SEL_V)<<(RTC_CNTL_XTL_EXT_CTR_SEL_S))
#define RTC_CNTL_XTL_EXT_CTR_SEL_V  0x7
#define RTC_CNTL_XTL_EXT_CTR_SEL_S  20
/* RTC_CNTL_XTL_EN_WAIT :  ;bitpos:[17:14] ;default: 4'd2 ; */
/*description: wait bias_sleep and current source wakeup.*/
#define RTC_CNTL_XTL_EN_WAIT    0x0000000F
#define RTC_CNTL_XTL_EN_WAIT_M  ((RTC_CNTL_XTL_EN_WAIT_V)<<(RTC_CNTL_XTL_EN_WAIT_S))
#define RTC_CNTL_XTL_EN_WAIT_V  0xF
#define RTC_CNTL_XTL_EN_WAIT_S  14
/* RTC_CNTL_XTL_FORCE_PU :  ;bitpos:[13] ;default: 1'd1 ; */
/*description: crystall force power up.*/
#define RTC_CNTL_XTL_FORCE_PU    (BIT(13))
#define RTC_CNTL_XTL_FORCE_PU_M  (BIT(13))
#define RTC_CNTL_XTL_FORCE_PU_V  0x1
#define RTC_CNTL_XTL_FORCE_PU_S  13
/* RTC_CNTL_XTL_FORCE_PD :  ;bitpos:[12] ;default: 1'b0 ; */
/*description: crystall force power down.*/
#define RTC_CNTL_XTL_FORCE_PD    (BIT(12))
#define RTC_CNTL_XTL_FORCE_PD_M  (BIT(12))
#define RTC_CNTL_XTL_FORCE_PD_V  0x1
#define RTC_CNTL_XTL_FORCE_PD_S  12
/* RTC_CNTL_BBPLL_FORCE_PU :  ;bitpos:[11] ;default: 1'd0 ; */
/*description: BB_PLL force power up.*/
#define RTC_CNTL_BBPLL_FORCE_PU    (BIT(11))
#define RTC_CNTL_BBPLL_FORCE_PU_M  (BIT(11))
#define RTC_CNTL_BBPLL_FORCE_PU_V  0x1
#define RTC_CNTL_BBPLL_FORCE_PU_S  11
/* RTC_CNTL_BBPLL_FORCE_PD :  ;bitpos:[10] ;default: 1'b0 ; */
/*description: BB_PLL force power down.*/
#define RTC_CNTL_BBPLL_FORCE_PD    (BIT(10))
#define RTC_CNTL_BBPLL_FORCE_PD_M  (BIT(10))
#define RTC_CNTL_BBPLL_FORCE_PD_V  0x1
#define RTC_CNTL_BBPLL_FORCE_PD_S  10
/* RTC_CNTL_BBPLL_I2C_FORCE_PU :  ;bitpos:[9] ;default: 1'd0 ; */
/*description: BB_PLL_I2C force power up.*/
#define RTC_CNTL_BBPLL_I2C_FORCE_PU    (BIT(9))
#define RTC_CNTL_BBPLL_I2C_FORCE_PU_M  (BIT(9))
#define RTC_CNTL_BBPLL_I2C_FORCE_PU_V  0x1
#define RTC_CNTL_BBPLL_I2C_FORCE_PU_S  9
/* RTC_CNTL_BBPLL_I2C_FORCE_PD :  ;bitpos:[8] ;default: 1'b0 ; */
/*description: BB_PLL _I2C force power down.*/
#define RTC_CNTL_BBPLL_I2C_FORCE_PD    (BIT(8))
#define RTC_CNTL_BBPLL_I2C_FORCE_PD_M  (BIT(8))
#define RTC_CNTL_BBPLL_I2C_FORCE_PD_V  0x1
#define RTC_CNTL_BBPLL_I2C_FORCE_PD_S  8
/* RTC_CNTL_BB_I2C_FORCE_PU :  ;bitpos:[7] ;default: 1'd0 ; */
/*description: BB_I2C force power up.*/
#define RTC_CNTL_BB_I2C_FORCE_PU    (BIT(7))
#define RTC_CNTL_BB_I2C_FORCE_PU_M  (BIT(7))
#define RTC_CNTL_BB_I2C_FORCE_PU_V  0x1
#define RTC_CNTL_BB_I2C_FORCE_PU_S  7
/* RTC_CNTL_BB_I2C_FORCE_PD :  ;bitpos:[6] ;default: 1'b0 ; */
/*description: BB_I2C force power down.*/
#define RTC_CNTL_BB_I2C_FORCE_PD    (BIT(6))
#define RTC_CNTL_BB_I2C_FORCE_PD_M  (BIT(6))
#define RTC_CNTL_BB_I2C_FORCE_PD_V  0x1
#define RTC_CNTL_BB_I2C_FORCE_PD_S  6
/* RTC_CNTL_SW_PROCPU_RST :  ;bitpos:[5] ;default: 1'b0 ; */
/*description: PRO CPU SW reset.*/
#define RTC_CNTL_SW_PROCPU_RST    (BIT(5))
#define RTC_CNTL_SW_PROCPU_RST_M  (BIT(5))
#define RTC_CNTL_SW_PROCPU_RST_V  0x1
#define RTC_CNTL_SW_PROCPU_RST_S  5
/* RTC_CNTL_SW_STALL_PROCPU_C0 :  ;bitpos:[3:2] ;default: 2'b0 ; */
/*description: {reg_sw_stall_procpu_c1[5:0],  reg_sw_stall_procpu_c0[1:0]} == 0x86 will stall P
RO CPU.*/
#define RTC_CNTL_SW_STALL_PROCPU_C0    0x00000003
#define RTC_CNTL_SW_STALL_PROCPU_C0_M  ((RTC_CNTL_SW_STALL_PROCPU_C0_V)<<(RTC_CNTL_SW_STALL_PROCPU_C0_S))
#define RTC_CNTL_SW_STALL_PROCPU_C0_V  0x3
#define RTC_CNTL_SW_STALL_PROCPU_C0_S  2

#define RTC_CNTL_SLP_TIMER0_REG          (DR_REG_RTCCNTL_BASE + 0x4)
/* RTC_CNTL_SLP_VAL_LO :  ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_SLP_VAL_LO    0xFFFFFFFF
#define RTC_CNTL_SLP_VAL_LO_M  ((RTC_CNTL_SLP_VAL_LO_V)<<(RTC_CNTL_SLP_VAL_LO_S))
#define RTC_CNTL_SLP_VAL_LO_V  0xFFFFFFFF
#define RTC_CNTL_SLP_VAL_LO_S  0

#define RTC_CNTL_SLP_TIMER1_REG          (DR_REG_RTCCNTL_BASE + 0x8)
/* RTC_CNTL_MAIN_TIMER_ALARM_EN :  ;bitpos:[16] ;default: 1'h0 ; */
/*description: timer alarm enable bit.*/
#define RTC_CNTL_MAIN_TIMER_ALARM_EN    (BIT(16))
#define RTC_CNTL_MAIN_TIMER_ALARM_EN_M  (BIT(16))
#define RTC_CNTL_MAIN_TIMER_ALARM_EN_V  0x1
#define RTC_CNTL_MAIN_TIMER_ALARM_EN_S  16
/* RTC_CNTL_SLP_VAL_HI :  ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: RTC sleep timer high 16 bits.*/
#define RTC_CNTL_SLP_VAL_HI    0x0000FFFF
#define RTC_CNTL_SLP_VAL_HI_M  ((RTC_CNTL_SLP_VAL_HI_V)<<(RTC_CNTL_SLP_VAL_HI_S))
#define RTC_CNTL_SLP_VAL_HI_V  0xFFFF
#define RTC_CNTL_SLP_VAL_HI_S  0

#define RTC_CNTL_TIME_UPDATE_REG          (DR_REG_RTCCNTL_BASE + 0xC)
/* RTC_CNTL_TIME_UPDATE :  ;bitpos:[31] ;default: 1'h0 ; */
/*description: Set 1: to update register with RTC timer.*/
#define RTC_CNTL_TIME_UPDATE    (BIT(31))
#define RTC_CNTL_TIME_UPDATE_M  (BIT(31))
#define RTC_CNTL_TIME_UPDATE_V  0x1
#define RTC_CNTL_TIME_UPDATE_S  31
/* RTC_CNTL_TIMER_SYS_RST :  ;bitpos:[29] ;default: 1'b0 ; */
/*description: enable to record system reset time.*/
#define RTC_CNTL_TIMER_SYS_RST    (BIT(29))
#define RTC_CNTL_TIMER_SYS_RST_M  (BIT(29))
#define RTC_CNTL_TIMER_SYS_RST_V  0x1
#define RTC_CNTL_TIMER_SYS_RST_S  29
/* RTC_CNTL_TIMER_XTL_OFF :  ;bitpos:[28] ;default: 1'b0 ; */
/*description: Enable to record 40M XTAL OFF time.*/
#define RTC_CNTL_TIMER_XTL_OFF    (BIT(28))
#define RTC_CNTL_TIMER_XTL_OFF_M  (BIT(28))
#define RTC_CNTL_TIMER_XTL_OFF_V  0x1
#define RTC_CNTL_TIMER_XTL_OFF_S  28
/* RTC_CNTL_TIMER_SYS_STALL :  ;bitpos:[27] ;default: 1'b0 ; */
/*description: Enable to record system stall time.*/
#define RTC_CNTL_TIMER_SYS_STALL    (BIT(27))
#define RTC_CNTL_TIMER_SYS_STALL_M  (BIT(27))
#define RTC_CNTL_TIMER_SYS_STALL_V  0x1
#define RTC_CNTL_TIMER_SYS_STALL_S  27

#define RTC_CNTL_TIME_LOW0_REG          (DR_REG_RTCCNTL_BASE + 0x10)
/* RTC_CNTL_TIMER_VALUE0_LOW :  ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: RTC timer low 32 bits.*/
#define RTC_CNTL_TIMER_VALUE0_LOW    0xFFFFFFFF
#define RTC_CNTL_TIMER_VALUE0_LOW_M  ((RTC_CNTL_TIMER_VALUE0_LOW_V)<<(RTC_CNTL_TIMER_VALUE0_LOW_S))
#define RTC_CNTL_TIMER_VALUE0_LOW_V  0xFFFFFFFF
#define RTC_CNTL_TIMER_VALUE0_LOW_S  0

#define RTC_CNTL_TIME_HIGH0_REG          (DR_REG_RTCCNTL_BASE + 0x14)
/* RTC_CNTL_TIMER_VALUE0_HIGH :  ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: RTC timer high 16 bits.*/
#define RTC_CNTL_TIMER_VALUE0_HIGH    0x0000FFFF
#define RTC_CNTL_TIMER_VALUE0_HIGH_M  ((RTC_CNTL_TIMER_VALUE0_HIGH_V)<<(RTC_CNTL_TIMER_VALUE0_HIGH_S))
#define RTC_CNTL_TIMER_VALUE0_HIGH_V  0xFFFF
#define RTC_CNTL_TIMER_VALUE0_HIGH_S  0

#define RTC_CNTL_STATE0_REG          (DR_REG_RTCCNTL_BASE + 0x18)
/* RTC_CNTL_SLEEP_EN :  ;bitpos:[31] ;default: 1'd0 ; */
/*description: sleep enable bit.*/
#define RTC_CNTL_SLEEP_EN    (BIT(31))
#define RTC_CNTL_SLEEP_EN_M  (BIT(31))
#define RTC_CNTL_SLEEP_EN_V  0x1
#define RTC_CNTL_SLEEP_EN_S  31
/* RTC_CNTL_SLP_REJECT :  ;bitpos:[30] ;default: 1'd0 ; */
/*description: leep reject bit.*/
#define RTC_CNTL_SLP_REJECT    (BIT(30))
#define RTC_CNTL_SLP_REJECT_M  (BIT(30))
#define RTC_CNTL_SLP_REJECT_V  0x1
#define RTC_CNTL_SLP_REJECT_S  30
/* RTC_CNTL_SLP_WAKEUP :  ;bitpos:[29] ;default: 1'd0 ; */
/*description: leep wakeup bit.*/
#define RTC_CNTL_SLP_WAKEUP    (BIT(29))
#define RTC_CNTL_SLP_WAKEUP_M  (BIT(29))
#define RTC_CNTL_SLP_WAKEUP_V  0x1
#define RTC_CNTL_SLP_WAKEUP_S  29
/* RTC_CNTL_SDIO_ACTIVE_IND :  ;bitpos:[28] ;default: 1'd0 ; */
/*description: SDIO active indication.*/
#define RTC_CNTL_SDIO_ACTIVE_IND    (BIT(28))
#define RTC_CNTL_SDIO_ACTIVE_IND_M  (BIT(28))
#define RTC_CNTL_SDIO_ACTIVE_IND_V  0x1
#define RTC_CNTL_SDIO_ACTIVE_IND_S  28
/* RTC_CNTL_APB2RTC_BRIDGE_SEL :  ;bitpos:[22] ;default: 1'd0 ; */
/*description: 1: APB to RTC using bridge.*/
#define RTC_CNTL_APB2RTC_BRIDGE_SEL    (BIT(22))
#define RTC_CNTL_APB2RTC_BRIDGE_SEL_M  (BIT(22))
#define RTC_CNTL_APB2RTC_BRIDGE_SEL_V  0x1
#define RTC_CNTL_APB2RTC_BRIDGE_SEL_S  22
/* RTC_CNTL_SLP_REJECT_CAUSE_CLR :  ;bitpos:[1] ;default: 1'b0 ; */
/*description: clear rtc sleep reject cause.*/
#define RTC_CNTL_SLP_REJECT_CAUSE_CLR    (BIT(1))
#define RTC_CNTL_SLP_REJECT_CAUSE_CLR_M  (BIT(1))
#define RTC_CNTL_SLP_REJECT_CAUSE_CLR_V  0x1
#define RTC_CNTL_SLP_REJECT_CAUSE_CLR_S  1
/* RTC_CNTL_SW_CPU_INT :  ;bitpos:[0] ;default: 1'b0 ; */
/*description: rtc software interrupt to main cpu.*/
#define RTC_CNTL_SW_CPU_INT    (BIT(0))
#define RTC_CNTL_SW_CPU_INT_M  (BIT(0))
#define RTC_CNTL_SW_CPU_INT_V  0x1
#define RTC_CNTL_SW_CPU_INT_S  0

#define RTC_CNTL_TIMER1_REG          (DR_REG_RTCCNTL_BASE + 0x1C)
/* RTC_CNTL_PLL_BUF_WAIT :  ;bitpos:[31:24] ;default: 8'd40 ; */
/*description: PLL wait cycles in slow_clk_rtc.*/
#define RTC_CNTL_PLL_BUF_WAIT    0x000000FF
#define RTC_CNTL_PLL_BUF_WAIT_M  ((RTC_CNTL_PLL_BUF_WAIT_V)<<(RTC_CNTL_PLL_BUF_WAIT_S))
#define RTC_CNTL_PLL_BUF_WAIT_V  0xFF
#define RTC_CNTL_PLL_BUF_WAIT_S  24
/* RTC_CNTL_XTL_BUF_WAIT :  ;bitpos:[23:14] ;default: 10'd80 ; */
/*description: XTAL wait cycles in slow_clk_rtc.*/
#define RTC_CNTL_XTL_BUF_WAIT    0x000003FF
#define RTC_CNTL_XTL_BUF_WAIT_M  ((RTC_CNTL_XTL_BUF_WAIT_V)<<(RTC_CNTL_XTL_BUF_WAIT_S))
#define RTC_CNTL_XTL_BUF_WAIT_V  0x3FF
#define RTC_CNTL_XTL_BUF_WAIT_S  14
/* RTC_CNTL_CK8M_WAIT :  ;bitpos:[13:6] ;default: 8'h10 ; */
/*description: CK8M wait cycles in slow_clk_rtc.*/
#define RTC_CNTL_CK8M_WAIT    0x000000FF
#define RTC_CNTL_CK8M_WAIT_M  ((RTC_CNTL_CK8M_WAIT_V)<<(RTC_CNTL_CK8M_WAIT_S))
#define RTC_CNTL_CK8M_WAIT_V  0xFF
#define RTC_CNTL_CK8M_WAIT_S  6
/* RTC_CNTL_CPU_STALL_WAIT :  ;bitpos:[5:1] ;default: 5'd1 ; */
/*description: CPU stall wait cycles in fast_clk_rtc.*/
#define RTC_CNTL_CPU_STALL_WAIT    0x0000001F
#define RTC_CNTL_CPU_STALL_WAIT_M  ((RTC_CNTL_CPU_STALL_WAIT_V)<<(RTC_CNTL_CPU_STALL_WAIT_S))
#define RTC_CNTL_CPU_STALL_WAIT_V  0x1F
#define RTC_CNTL_CPU_STALL_WAIT_S  1
/* RTC_CNTL_CPU_STALL_EN :  ;bitpos:[0] ;default: 1'd1 ; */
/*description: CPU stall enable bit.*/
#define RTC_CNTL_CPU_STALL_EN    (BIT(0))
#define RTC_CNTL_CPU_STALL_EN_M  (BIT(0))
#define RTC_CNTL_CPU_STALL_EN_V  0x1
#define RTC_CNTL_CPU_STALL_EN_S  0

#define RTC_CNTL_TIMER2_REG          (DR_REG_RTCCNTL_BASE + 0x20)
/* RTC_CNTL_MIN_TIME_CK8M_OFF :  ;bitpos:[31:24] ;default: 8'h1 ; */
/*description: minimal cycles in slow_clk_rtc for CK8M in power down state.*/
#define RTC_CNTL_MIN_TIME_CK8M_OFF    0x000000FF
#define RTC_CNTL_MIN_TIME_CK8M_OFF_M  ((RTC_CNTL_MIN_TIME_CK8M_OFF_V)<<(RTC_CNTL_MIN_TIME_CK8M_OFF_S))
#define RTC_CNTL_MIN_TIME_CK8M_OFF_V  0xFF
#define RTC_CNTL_MIN_TIME_CK8M_OFF_S  24

#define RTC_CNTL_TIMER4_REG          (DR_REG_RTCCNTL_BASE + 0x24)
/* RTC_CNTL_DG_WRAP_POWERUP_TIMER :  ;bitpos:[31:25] ;default: 7'h8 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DG_WRAP_POWERUP_TIMER    0x0000007F
#define RTC_CNTL_DG_WRAP_POWERUP_TIMER_M  ((RTC_CNTL_DG_WRAP_POWERUP_TIMER_V)<<(RTC_CNTL_DG_WRAP_POWERUP_TIMER_S))
#define RTC_CNTL_DG_WRAP_POWERUP_TIMER_V  0x7F
#define RTC_CNTL_DG_WRAP_POWERUP_TIMER_S  25
/* RTC_CNTL_DG_WRAP_WAIT_TIMER :  ;bitpos:[24:16] ;default: 9'h20 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DG_WRAP_WAIT_TIMER    0x000001FF
#define RTC_CNTL_DG_WRAP_WAIT_TIMER_M  ((RTC_CNTL_DG_WRAP_WAIT_TIMER_V)<<(RTC_CNTL_DG_WRAP_WAIT_TIMER_S))
#define RTC_CNTL_DG_WRAP_WAIT_TIMER_V  0x1FF
#define RTC_CNTL_DG_WRAP_WAIT_TIMER_S  16

#define RTC_CNTL_TIMER5_REG          (DR_REG_RTCCNTL_BASE + 0x28)
/* RTC_CNTL_MIN_SLP_VAL :  ;bitpos:[15:8] ;default: 8'h80 ; */
/*description: minimal sleep cycles in slow_clk_rtc.*/
#define RTC_CNTL_MIN_SLP_VAL    0x000000FF
#define RTC_CNTL_MIN_SLP_VAL_M  ((RTC_CNTL_MIN_SLP_VAL_V)<<(RTC_CNTL_MIN_SLP_VAL_S))
#define RTC_CNTL_MIN_SLP_VAL_V  0xFF
#define RTC_CNTL_MIN_SLP_VAL_S  8

#define RTC_CNTL_ANA_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x2C)
/* RTC_CNTL_PLL_I2C_PU :  ;bitpos:[31] ;default: 1'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_PLL_I2C_PU    (BIT(31))
#define RTC_CNTL_PLL_I2C_PU_M  (BIT(31))
#define RTC_CNTL_PLL_I2C_PU_V  0x1
#define RTC_CNTL_PLL_I2C_PU_S  31
/* RTC_CNTL_CKGEN_I2C_PU :  ;bitpos:[30] ;default: 1'd0 ; */
/*description: 1: CKGEN_I2C power up.*/
#define RTC_CNTL_CKGEN_I2C_PU    (BIT(30))
#define RTC_CNTL_CKGEN_I2C_PU_M  (BIT(30))
#define RTC_CNTL_CKGEN_I2C_PU_V  0x1
#define RTC_CNTL_CKGEN_I2C_PU_S  30
/* RTC_CNTL_RFRX_PBUS_PU :  ;bitpos:[28] ;default: 1'd0 ; */
/*description: 1: RFRX_PBUS power up.*/
#define RTC_CNTL_RFRX_PBUS_PU    (BIT(28))
#define RTC_CNTL_RFRX_PBUS_PU_M  (BIT(28))
#define RTC_CNTL_RFRX_PBUS_PU_V  0x1
#define RTC_CNTL_RFRX_PBUS_PU_S  28
/* RTC_CNTL_TXRF_I2C_PU :  ;bitpos:[27] ;default: 1'd0 ; */
/*description: 1: TXRF_I2C power up.*/
#define RTC_CNTL_TXRF_I2C_PU    (BIT(27))
#define RTC_CNTL_TXRF_I2C_PU_M  (BIT(27))
#define RTC_CNTL_TXRF_I2C_PU_V  0x1
#define RTC_CNTL_TXRF_I2C_PU_S  27
/* RTC_CNTL_BBPLL_CAL_SLP_START :  ;bitpos:[25] ;default: 1'b0 ; */
/*description: start BBPLL calibration during sleep.*/
#define RTC_CNTL_BBPLL_CAL_SLP_START    (BIT(25))
#define RTC_CNTL_BBPLL_CAL_SLP_START_M  (BIT(25))
#define RTC_CNTL_BBPLL_CAL_SLP_START_V  0x1
#define RTC_CNTL_BBPLL_CAL_SLP_START_S  25
/* RTC_CNTL_SAR_I2C_PU :  ;bitpos:[22] ;default: 1'b1 ; */
/*description: PLLA force power up.*/
#define RTC_CNTL_SAR_I2C_PU    (BIT(22))
#define RTC_CNTL_SAR_I2C_PU_M  (BIT(22))
#define RTC_CNTL_SAR_I2C_PU_V  0x1
#define RTC_CNTL_SAR_I2C_PU_S  22
/* RTC_CNTL_I2C_RESET_POR_FORCE_PU :  ;bitpos:[19] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_I2C_RESET_POR_FORCE_PU    (BIT(19))
#define RTC_CNTL_I2C_RESET_POR_FORCE_PU_M  (BIT(19))
#define RTC_CNTL_I2C_RESET_POR_FORCE_PU_V  0x1
#define RTC_CNTL_I2C_RESET_POR_FORCE_PU_S  19
/* RTC_CNTL_I2C_RESET_POR_FORCE_PD :  ;bitpos:[18] ;default: 1'b1 ; */
/*description: Need add desc.*/
#define RTC_CNTL_I2C_RESET_POR_FORCE_PD    (BIT(18))
#define RTC_CNTL_I2C_RESET_POR_FORCE_PD_M  (BIT(18))
#define RTC_CNTL_I2C_RESET_POR_FORCE_PD_V  0x1
#define RTC_CNTL_I2C_RESET_POR_FORCE_PD_S  18

#define RTC_CNTL_RESET_STATE_REG          (DR_REG_RTCCNTL_BASE + 0x30)
/* RTC_CNTL_DRESET_MASK_PROCPU :  ;bitpos:[20] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DRESET_MASK_PROCPU    (BIT(20))
#define RTC_CNTL_DRESET_MASK_PROCPU_M  (BIT(20))
#define RTC_CNTL_DRESET_MASK_PROCPU_V  0x1
#define RTC_CNTL_DRESET_MASK_PROCPU_S  20
/* RTC_CNTL_OCD_HALT_ON_RESET_PROCPU :  ;bitpos:[19] ;default: 1'b0 ; */
/*description: PROCPU OcdHaltOnReset.*/
#define RTC_CNTL_OCD_HALT_ON_RESET_PROCPU    (BIT(19))
#define RTC_CNTL_OCD_HALT_ON_RESET_PROCPU_M  (BIT(19))
#define RTC_CNTL_OCD_HALT_ON_RESET_PROCPU_V  0x1
#define RTC_CNTL_OCD_HALT_ON_RESET_PROCPU_S  19
/* RTC_CNTL_STAT_VECTOR_SEL_PROCPU :  ;bitpos:[13] ;default: 1'b1 ; */
/*description: PRO CPU state vector sel.*/
#define RTC_CNTL_STAT_VECTOR_SEL_PROCPU    (BIT(13))
#define RTC_CNTL_STAT_VECTOR_SEL_PROCPU_M  (BIT(13))
#define RTC_CNTL_STAT_VECTOR_SEL_PROCPU_V  0x1
#define RTC_CNTL_STAT_VECTOR_SEL_PROCPU_S  13
/* RTC_CNTL_RESET_CAUSE_PROCPU :  ;bitpos:[5:0] ;default: 0 ; */
/*description: reset cause of PRO CPU.*/
#define RTC_CNTL_RESET_CAUSE_PROCPU    0x0000003F
#define RTC_CNTL_RESET_CAUSE_PROCPU_M  ((RTC_CNTL_RESET_CAUSE_PROCPU_V)<<(RTC_CNTL_RESET_CAUSE_PROCPU_S))
#define RTC_CNTL_RESET_CAUSE_PROCPU_V  0x3F
#define RTC_CNTL_RESET_CAUSE_PROCPU_S  0

#define RTC_CNTL_WAKEUP_STATE_REG          (DR_REG_RTCCNTL_BASE + 0x34)
/* RTC_CNTL_WAKEUP_ENA :  ;bitpos:[31:15] ;default: 17'b1100 ; */
/*description: wakeup enable bitmap.*/
#define RTC_CNTL_WAKEUP_ENA    0x0001FFFF
#define RTC_CNTL_WAKEUP_ENA_M  ((RTC_CNTL_WAKEUP_ENA_V)<<(RTC_CNTL_WAKEUP_ENA_S))
#define RTC_CNTL_WAKEUP_ENA_V  0x1FFFF
#define RTC_CNTL_WAKEUP_ENA_S  15

#define RTC_CNTL_INT_ENA_REG          (DR_REG_RTCCNTL_BASE + 0x38)
/* RTC_CNTL_BBPLL_CAL_INT_ENA : BIT ;bitpos:[20] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_BBPLL_CAL_INT_ENA    (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ENA_M  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ENA_V  0x1
#define RTC_CNTL_BBPLL_CAL_INT_ENA_S  20
/* RTC_CNTL_SWD_INT_ENA : BIT ;bitpos:[15] ;default: 1'b0 ; */
/*description: enable super watch dog interrupt.*/
#define RTC_CNTL_SWD_INT_ENA    (BIT(15))
#define RTC_CNTL_SWD_INT_ENA_M  (BIT(15))
#define RTC_CNTL_SWD_INT_ENA_V  0x1
#define RTC_CNTL_SWD_INT_ENA_S  15
/* RTC_CNTL_MAIN_TIMER_INT_ENA : BIT ;bitpos:[10] ;default: 1'b0 ; */
/*description: enable RTC main timer interrupt.*/
#define RTC_CNTL_MAIN_TIMER_INT_ENA    (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ENA_M  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ENA_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_ENA_S  10
/* RTC_CNTL_BROWN_OUT_INT_ENA : BIT ;bitpos:[9] ;default: 1'b0 ; */
/*description: enable brown out interrupt.*/
#define RTC_CNTL_BROWN_OUT_INT_ENA    (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ENA_M  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ENA_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_ENA_S  9
/* RTC_CNTL_WDT_INT_ENA : BIT ;bitpos:[3] ;default: 1'b0 ; */
/*description: enable RTC WDT interrupt.*/
#define RTC_CNTL_WDT_INT_ENA    (BIT(3))
#define RTC_CNTL_WDT_INT_ENA_M  (BIT(3))
#define RTC_CNTL_WDT_INT_ENA_V  0x1
#define RTC_CNTL_WDT_INT_ENA_S  3
/* RTC_CNTL_SLP_REJECT_INT_ENA : BIT ;bitpos:[1] ;default: 1'b0 ; */
/*description: enable sleep reject interrupt.*/
#define RTC_CNTL_SLP_REJECT_INT_ENA    (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ENA_M  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ENA_V  0x1
#define RTC_CNTL_SLP_REJECT_INT_ENA_S  1
/* RTC_CNTL_SLP_WAKEUP_INT_ENA : BIT ;bitpos:[0] ;default: 1'b0 ; */
/*description: enable sleep wakeup interrupt.*/
#define RTC_CNTL_SLP_WAKEUP_INT_ENA    (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_M  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_V  0x1
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_S  0

#define RTC_CNTL_INT_RAW_REG          (DR_REG_RTCCNTL_BASE + 0x3C)
/* RTC_CNTL_BBPLL_CAL_INT_RAW :  ;bitpos:[20] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_BBPLL_CAL_INT_RAW    (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_RAW_M  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_RAW_V  0x1
#define RTC_CNTL_BBPLL_CAL_INT_RAW_S  20
/* RTC_CNTL_SWD_INT_RAW :  ;bitpos:[15] ;default: 1'b0 ; */
/*description: super watch dog interrupt raw.*/
#define RTC_CNTL_SWD_INT_RAW    (BIT(15))
#define RTC_CNTL_SWD_INT_RAW_M  (BIT(15))
#define RTC_CNTL_SWD_INT_RAW_V  0x1
#define RTC_CNTL_SWD_INT_RAW_S  15
/* RTC_CNTL_MAIN_TIMER_INT_RAW :  ;bitpos:[10] ;default: 1'b0 ; */
/*description: RTC main timer interrupt raw.*/
#define RTC_CNTL_MAIN_TIMER_INT_RAW    (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_RAW_M  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_RAW_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_RAW_S  10
/* RTC_CNTL_BROWN_OUT_INT_RAW :  ;bitpos:[9] ;default: 1'b0 ; */
/*description: brown out interrupt raw.*/
#define RTC_CNTL_BROWN_OUT_INT_RAW    (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_RAW_M  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_RAW_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_RAW_S  9
/* RTC_CNTL_WDT_INT_RAW :  ;bitpos:[3] ;default: 1'b0 ; */
/*description: RTC WDT interrupt raw.*/
#define RTC_CNTL_WDT_INT_RAW    (BIT(3))
#define RTC_CNTL_WDT_INT_RAW_M  (BIT(3))
#define RTC_CNTL_WDT_INT_RAW_V  0x1
#define RTC_CNTL_WDT_INT_RAW_S  3
/* RTC_CNTL_SLP_REJECT_INT_RAW :  ;bitpos:[1] ;default: 1'b0 ; */
/*description: sleep reject interrupt raw.*/
#define RTC_CNTL_SLP_REJECT_INT_RAW    (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_RAW_M  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_RAW_V  0x1
#define RTC_CNTL_SLP_REJECT_INT_RAW_S  1
/* RTC_CNTL_SLP_WAKEUP_INT_RAW :  ;bitpos:[0] ;default: 1'b0 ; */
/*description: sleep wakeup interrupt raw.*/
#define RTC_CNTL_SLP_WAKEUP_INT_RAW    (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_RAW_M  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_RAW_V  0x1
#define RTC_CNTL_SLP_WAKEUP_INT_RAW_S  0

#define RTC_CNTL_INT_ST_REG          (DR_REG_RTCCNTL_BASE + 0x40)
/* RTC_CNTL_BBPLL_CAL_INT_ST :  ;bitpos:[20] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_BBPLL_CAL_INT_ST    (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ST_M  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ST_V  0x1
#define RTC_CNTL_BBPLL_CAL_INT_ST_S  20
/* RTC_CNTL_SWD_INT_ST :  ;bitpos:[15] ;default: 1'b0 ; */
/*description: super watch dog interrupt state.*/
#define RTC_CNTL_SWD_INT_ST    (BIT(15))
#define RTC_CNTL_SWD_INT_ST_M  (BIT(15))
#define RTC_CNTL_SWD_INT_ST_V  0x1
#define RTC_CNTL_SWD_INT_ST_S  15
/* RTC_CNTL_MAIN_TIMER_INT_ST :  ;bitpos:[10] ;default: 1'b0 ; */
/*description: RTC main timer interrupt state.*/
#define RTC_CNTL_MAIN_TIMER_INT_ST    (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ST_M  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ST_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_ST_S  10
/* RTC_CNTL_BROWN_OUT_INT_ST :  ;bitpos:[9] ;default: 1'b0 ; */
/*description: brown out interrupt state.*/
#define RTC_CNTL_BROWN_OUT_INT_ST    (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ST_M  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ST_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_ST_S  9
/* RTC_CNTL_WDT_INT_ST :  ;bitpos:[3] ;default: 1'b0 ; */
/*description: RTC WDT interrupt state.*/
#define RTC_CNTL_WDT_INT_ST    (BIT(3))
#define RTC_CNTL_WDT_INT_ST_M  (BIT(3))
#define RTC_CNTL_WDT_INT_ST_V  0x1
#define RTC_CNTL_WDT_INT_ST_S  3
/* RTC_CNTL_SLP_REJECT_INT_ST :  ;bitpos:[1] ;default: 1'b0 ; */
/*description: sleep reject interrupt state.*/
#define RTC_CNTL_SLP_REJECT_INT_ST    (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ST_M  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ST_V  0x1
#define RTC_CNTL_SLP_REJECT_INT_ST_S  1
/* RTC_CNTL_SLP_WAKEUP_INT_ST :  ;bitpos:[0] ;default: 1'b0 ; */
/*description: sleep wakeup interrupt state.*/
#define RTC_CNTL_SLP_WAKEUP_INT_ST    (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ST_M  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ST_V  0x1
#define RTC_CNTL_SLP_WAKEUP_INT_ST_S  0

#define RTC_CNTL_INT_CLR_REG          (DR_REG_RTCCNTL_BASE + 0x44)
/* RTC_CNTL_BBPLL_CAL_INT_CLR :  ;bitpos:[20] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_BBPLL_CAL_INT_CLR    (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_CLR_M  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_CLR_V  0x1
#define RTC_CNTL_BBPLL_CAL_INT_CLR_S  20
/* RTC_CNTL_SWD_INT_CLR :  ;bitpos:[15] ;default: 1'b0 ; */
/*description: Clear super watch dog interrupt state.*/
#define RTC_CNTL_SWD_INT_CLR    (BIT(15))
#define RTC_CNTL_SWD_INT_CLR_M  (BIT(15))
#define RTC_CNTL_SWD_INT_CLR_V  0x1
#define RTC_CNTL_SWD_INT_CLR_S  15
/* RTC_CNTL_MAIN_TIMER_INT_CLR :  ;bitpos:[10] ;default: 1'b0 ; */
/*description: Clear RTC main timer interrupt state.*/
#define RTC_CNTL_MAIN_TIMER_INT_CLR    (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_CLR_M  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_CLR_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_CLR_S  10
/* RTC_CNTL_BROWN_OUT_INT_CLR :  ;bitpos:[9] ;default: 1'b0 ; */
/*description: Clear brown out interrupt state.*/
#define RTC_CNTL_BROWN_OUT_INT_CLR    (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_CLR_M  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_CLR_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_CLR_S  9
/* RTC_CNTL_WDT_INT_CLR :  ;bitpos:[3] ;default: 1'b0 ; */
/*description: Clear RTC WDT interrupt state.*/
#define RTC_CNTL_WDT_INT_CLR    (BIT(3))
#define RTC_CNTL_WDT_INT_CLR_M  (BIT(3))
#define RTC_CNTL_WDT_INT_CLR_V  0x1
#define RTC_CNTL_WDT_INT_CLR_S  3
/* RTC_CNTL_SLP_REJECT_INT_CLR :  ;bitpos:[1] ;default: 1'b0 ; */
/*description: Clear sleep reject interrupt state.*/
#define RTC_CNTL_SLP_REJECT_INT_CLR    (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_CLR_M  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_CLR_V  0x1
#define RTC_CNTL_SLP_REJECT_INT_CLR_S  1
/* RTC_CNTL_SLP_WAKEUP_INT_CLR :  ;bitpos:[0] ;default: 1'b0 ; */
/*description: Clear sleep wakeup interrupt state.*/
#define RTC_CNTL_SLP_WAKEUP_INT_CLR    (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_CLR_M  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_CLR_V  0x1
#define RTC_CNTL_SLP_WAKEUP_INT_CLR_S  0

#define RTC_CNTL_STORE0_REG          (DR_REG_RTCCNTL_BASE + 0x48)
/* RTC_CNTL_SCRATCH0 :  ;bitpos:[31:0] ;default: 0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_SCRATCH0    0xFFFFFFFF
#define RTC_CNTL_SCRATCH0_M  ((RTC_CNTL_SCRATCH0_V)<<(RTC_CNTL_SCRATCH0_S))
#define RTC_CNTL_SCRATCH0_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH0_S  0

#define RTC_CNTL_STORE1_REG          (DR_REG_RTCCNTL_BASE + 0x4C)
/* RTC_CNTL_SCRATCH1 :  ;bitpos:[31:0] ;default: 0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_SCRATCH1    0xFFFFFFFF
#define RTC_CNTL_SCRATCH1_M  ((RTC_CNTL_SCRATCH1_V)<<(RTC_CNTL_SCRATCH1_S))
#define RTC_CNTL_SCRATCH1_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH1_S  0

#define RTC_CNTL_STORE2_REG          (DR_REG_RTCCNTL_BASE + 0x50)
/* RTC_CNTL_SCRATCH2 :  ;bitpos:[31:0] ;default: 0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_SCRATCH2    0xFFFFFFFF
#define RTC_CNTL_SCRATCH2_M  ((RTC_CNTL_SCRATCH2_V)<<(RTC_CNTL_SCRATCH2_S))
#define RTC_CNTL_SCRATCH2_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH2_S  0

#define RTC_CNTL_STORE3_REG          (DR_REG_RTCCNTL_BASE + 0x54)
/* RTC_CNTL_SCRATCH3 :  ;bitpos:[31:0] ;default: 0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_SCRATCH3    0xFFFFFFFF
#define RTC_CNTL_SCRATCH3_M  ((RTC_CNTL_SCRATCH3_V)<<(RTC_CNTL_SCRATCH3_S))
#define RTC_CNTL_SCRATCH3_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH3_S  0

#define RTC_CNTL_EXT_XTL_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x58)
/* RTC_CNTL_XTL_EXT_CTR_EN :  ;bitpos:[31] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_XTL_EXT_CTR_EN    (BIT(31))
#define RTC_CNTL_XTL_EXT_CTR_EN_M  (BIT(31))
#define RTC_CNTL_XTL_EXT_CTR_EN_V  0x1
#define RTC_CNTL_XTL_EXT_CTR_EN_S  31
/* RTC_CNTL_XTL_EXT_CTR_LV :  ;bitpos:[30] ;default: 1'b0 ; */
/*description: 0: power down XTAL at high level.*/
#define RTC_CNTL_XTL_EXT_CTR_LV    (BIT(30))
#define RTC_CNTL_XTL_EXT_CTR_LV_M  (BIT(30))
#define RTC_CNTL_XTL_EXT_CTR_LV_V  0x1
#define RTC_CNTL_XTL_EXT_CTR_LV_S  30

#define RTC_CNTL_EXT_WAKEUP_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x5C)
/* RTC_CNTL_GPIO_WAKEUP_FILTER :  ;bitpos:[31] ;default: 1'b0 ; */
/*description: enable filter for gpio wakeup event.*/
#define RTC_CNTL_GPIO_WAKEUP_FILTER    (BIT(31))
#define RTC_CNTL_GPIO_WAKEUP_FILTER_M  (BIT(31))
#define RTC_CNTL_GPIO_WAKEUP_FILTER_V  0x1
#define RTC_CNTL_GPIO_WAKEUP_FILTER_S  31

#define RTC_CNTL_SLP_REJECT_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x60)
/* RTC_CNTL_DEEP_SLP_REJECT_EN :  ;bitpos:[31] ;default: 1'b0 ; */
/*description: enable reject for deep sleep.*/
#define RTC_CNTL_DEEP_SLP_REJECT_EN    (BIT(31))
#define RTC_CNTL_DEEP_SLP_REJECT_EN_M  (BIT(31))
#define RTC_CNTL_DEEP_SLP_REJECT_EN_V  0x1
#define RTC_CNTL_DEEP_SLP_REJECT_EN_S  31
/* RTC_CNTL_LIGHT_SLP_REJECT_EN :  ;bitpos:[30] ;default: 1'b0 ; */
/*description: enable reject for light sleep.*/
#define RTC_CNTL_LIGHT_SLP_REJECT_EN    (BIT(30))
#define RTC_CNTL_LIGHT_SLP_REJECT_EN_M  (BIT(30))
#define RTC_CNTL_LIGHT_SLP_REJECT_EN_V  0x1
#define RTC_CNTL_LIGHT_SLP_REJECT_EN_S  30
/* RTC_CNTL_SLEEP_REJECT_ENA :  ;bitpos:[29:12] ;default: 17'd0 ; */
/*description: sleep reject enable.*/
#define RTC_CNTL_SLEEP_REJECT_ENA    0x0003FFFF
#define RTC_CNTL_SLEEP_REJECT_ENA_M  ((RTC_CNTL_SLEEP_REJECT_ENA_V)<<(RTC_CNTL_SLEEP_REJECT_ENA_S))
#define RTC_CNTL_SLEEP_REJECT_ENA_V  0x3FFFF
#define RTC_CNTL_SLEEP_REJECT_ENA_S  12

#define RTC_CNTL_CPU_PERIOD_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x64)
/* RTC_CNTL_CPUPERIOD_SEL :  ;bitpos:[31:30] ;default: 2'b00 ; */
/*description: Need add desc.*/
#define RTC_CNTL_CPUPERIOD_SEL    0x00000003
#define RTC_CNTL_CPUPERIOD_SEL_M  ((RTC_CNTL_CPUPERIOD_SEL_V)<<(RTC_CNTL_CPUPERIOD_SEL_S))
#define RTC_CNTL_CPUPERIOD_SEL_V  0x3
#define RTC_CNTL_CPUPERIOD_SEL_S  30
/* RTC_CNTL_CPUSEL_CONF :  ;bitpos:[29] ;default: 1'b0 ; */
/*description: CPU sel option.*/
#define RTC_CNTL_CPUSEL_CONF    (BIT(29))
#define RTC_CNTL_CPUSEL_CONF_M  (BIT(29))
#define RTC_CNTL_CPUSEL_CONF_V  0x1
#define RTC_CNTL_CPUSEL_CONF_S  29

#define RTC_CNTL_CLK_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x68)
/* RTC_CNTL_ANA_CLK_RTC_SEL :  ;bitpos:[31:30] ;default: 2'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_ANA_CLK_RTC_SEL    0x00000003
#define RTC_CNTL_ANA_CLK_RTC_SEL_M  ((RTC_CNTL_ANA_CLK_RTC_SEL_V)<<(RTC_CNTL_ANA_CLK_RTC_SEL_S))
#define RTC_CNTL_ANA_CLK_RTC_SEL_V  0x3
#define RTC_CNTL_ANA_CLK_RTC_SEL_S  30
/* RTC_CNTL_FAST_CLK_RTC_SEL :  ;bitpos:[29] ;default: 1'b0 ; */
/*description: fast_clk_rtc sel. 0: XTAL div 2.*/
#define RTC_CNTL_FAST_CLK_RTC_SEL    (BIT(29))
#define RTC_CNTL_FAST_CLK_RTC_SEL_M  (BIT(29))
#define RTC_CNTL_FAST_CLK_RTC_SEL_V  0x1
#define RTC_CNTL_FAST_CLK_RTC_SEL_S  29
/* RTC_CNTL_XTAL_GLOBAL_FORCE_NOGATING :  ;bitpos:[28] ;default: 1'b1 ; */
/*description: Need add desc.*/
#define RTC_CNTL_XTAL_GLOBAL_FORCE_NOGATING    (BIT(28))
#define RTC_CNTL_XTAL_GLOBAL_FORCE_NOGATING_M  (BIT(28))
#define RTC_CNTL_XTAL_GLOBAL_FORCE_NOGATING_V  0x1
#define RTC_CNTL_XTAL_GLOBAL_FORCE_NOGATING_S  28
/* RTC_CNTL_XTAL_GLOBAL_FORCE_GATING :  ;bitpos:[27] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_XTAL_GLOBAL_FORCE_GATING    (BIT(27))
#define RTC_CNTL_XTAL_GLOBAL_FORCE_GATING_M  (BIT(27))
#define RTC_CNTL_XTAL_GLOBAL_FORCE_GATING_V  0x1
#define RTC_CNTL_XTAL_GLOBAL_FORCE_GATING_S  27
/* RTC_CNTL_CK8M_FORCE_PU :  ;bitpos:[26] ;default: 1'd0 ; */
/*description: CK8M force power up.*/
#define RTC_CNTL_CK8M_FORCE_PU    (BIT(26))
#define RTC_CNTL_CK8M_FORCE_PU_M  (BIT(26))
#define RTC_CNTL_CK8M_FORCE_PU_V  0x1
#define RTC_CNTL_CK8M_FORCE_PU_S  26
/* RTC_CNTL_CK8M_FORCE_PD :  ;bitpos:[25] ;default: 1'd0 ; */
/*description: CK8M force power down.*/
#define RTC_CNTL_CK8M_FORCE_PD    (BIT(25))
#define RTC_CNTL_CK8M_FORCE_PD_M  (BIT(25))
#define RTC_CNTL_CK8M_FORCE_PD_V  0x1
#define RTC_CNTL_CK8M_FORCE_PD_S  25
/* RTC_CNTL_CK8M_DFREQ :  ;bitpos:[24:17] ;default: 8'd172 ; */
/*description: CK8M_DFREQ.*/
#define RTC_CNTL_CK8M_DFREQ    0x000000FF
#define RTC_CNTL_CK8M_DFREQ_M  ((RTC_CNTL_CK8M_DFREQ_V)<<(RTC_CNTL_CK8M_DFREQ_S))
#define RTC_CNTL_CK8M_DFREQ_V  0xFF
#define RTC_CNTL_CK8M_DFREQ_S  17
/* RTC_CNTL_CK8M_FORCE_NOGATING :  ;bitpos:[16] ;default: 1'd0 ; */
/*description: CK8M force no gating during sleep.*/
#define RTC_CNTL_CK8M_FORCE_NOGATING    (BIT(16))
#define RTC_CNTL_CK8M_FORCE_NOGATING_M  (BIT(16))
#define RTC_CNTL_CK8M_FORCE_NOGATING_V  0x1
#define RTC_CNTL_CK8M_FORCE_NOGATING_S  16
/* RTC_CNTL_XTAL_FORCE_NOGATING :  ;bitpos:[15] ;default: 1'd0 ; */
/*description: XTAL force no gating during sleep.*/
#define RTC_CNTL_XTAL_FORCE_NOGATING    (BIT(15))
#define RTC_CNTL_XTAL_FORCE_NOGATING_M  (BIT(15))
#define RTC_CNTL_XTAL_FORCE_NOGATING_V  0x1
#define RTC_CNTL_XTAL_FORCE_NOGATING_S  15
/* RTC_CNTL_CK8M_DIV_SEL :  ;bitpos:[14:12] ;default: 3'd3 ; */
/*description: divider = reg_ck8m_div_sel + 1.*/
#define RTC_CNTL_CK8M_DIV_SEL    0x00000007
#define RTC_CNTL_CK8M_DIV_SEL_M  ((RTC_CNTL_CK8M_DIV_SEL_V)<<(RTC_CNTL_CK8M_DIV_SEL_S))
#define RTC_CNTL_CK8M_DIV_SEL_V  0x7
#define RTC_CNTL_CK8M_DIV_SEL_S  12
/* RTC_CNTL_DIG_CLK8M_EN :  ;bitpos:[10] ;default: 1'd0 ; */
/*description: enable CK8M for digital core (no relationship with RTC core).*/
#define RTC_CNTL_DIG_CLK8M_EN    (BIT(10))
#define RTC_CNTL_DIG_CLK8M_EN_M  (BIT(10))
#define RTC_CNTL_DIG_CLK8M_EN_V  0x1
#define RTC_CNTL_DIG_CLK8M_EN_S  10
/* RTC_CNTL_DIG_CLK8M_D256_EN :  ;bitpos:[9] ;default: 1'd1 ; */
/*description: enable CK8M_D256_OUT for digital core (no relationship with RTC core).*/
#define RTC_CNTL_DIG_CLK8M_D256_EN    (BIT(9))
#define RTC_CNTL_DIG_CLK8M_D256_EN_M  (BIT(9))
#define RTC_CNTL_DIG_CLK8M_D256_EN_V  0x1
#define RTC_CNTL_DIG_CLK8M_D256_EN_S  9
/* RTC_CNTL_DIG_XTAL32K_EN :  ;bitpos:[8] ;default: 1'd0 ; */
/*description: enable CK_XTAL_32K for digital core (no relationship with RTC core).*/
#define RTC_CNTL_DIG_XTAL32K_EN    (BIT(8))
#define RTC_CNTL_DIG_XTAL32K_EN_M  (BIT(8))
#define RTC_CNTL_DIG_XTAL32K_EN_V  0x1
#define RTC_CNTL_DIG_XTAL32K_EN_S  8
/* RTC_CNTL_ENB_CK8M_DIV :  ;bitpos:[7] ;default: 1'd0 ; */
/*description: 1: CK8M_D256_OUT is actually CK8M.*/
#define RTC_CNTL_ENB_CK8M_DIV    (BIT(7))
#define RTC_CNTL_ENB_CK8M_DIV_M  (BIT(7))
#define RTC_CNTL_ENB_CK8M_DIV_V  0x1
#define RTC_CNTL_ENB_CK8M_DIV_S  7
/* RTC_CNTL_ENB_CK8M :  ;bitpos:[6] ;default: 1'd0 ; */
/*description: disable CK8M and CK8M_D256_OUT.*/
#define RTC_CNTL_ENB_CK8M    (BIT(6))
#define RTC_CNTL_ENB_CK8M_M  (BIT(6))
#define RTC_CNTL_ENB_CK8M_V  0x1
#define RTC_CNTL_ENB_CK8M_S  6
/* RTC_CNTL_CK8M_DIV :  ;bitpos:[5:4] ;default: 2'b01 ; */
/*description: CK8M_D256_OUT divider. 00: div128.*/
#define RTC_CNTL_CK8M_DIV    0x00000003
#define RTC_CNTL_CK8M_DIV_M  ((RTC_CNTL_CK8M_DIV_V)<<(RTC_CNTL_CK8M_DIV_S))
#define RTC_CNTL_CK8M_DIV_V  0x3
#define RTC_CNTL_CK8M_DIV_S  4
/* RTC_CNTL_CK8M_DIV_SEL_VLD :  ;bitpos:[3] ;default: 1'b1 ; */
/*description: used to sync reg_ck8m_div_sel bus. Clear vld before set reg_ck8m_div_sel.*/
#define RTC_CNTL_CK8M_DIV_SEL_VLD    (BIT(3))
#define RTC_CNTL_CK8M_DIV_SEL_VLD_M  (BIT(3))
#define RTC_CNTL_CK8M_DIV_SEL_VLD_V  0x1
#define RTC_CNTL_CK8M_DIV_SEL_VLD_S  3
/* RTC_CNTL_EFUSE_CLK_FORCE_NOGATING :  ;bitpos:[2] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_EFUSE_CLK_FORCE_NOGATING    (BIT(2))
#define RTC_CNTL_EFUSE_CLK_FORCE_NOGATING_M  (BIT(2))
#define RTC_CNTL_EFUSE_CLK_FORCE_NOGATING_V  0x1
#define RTC_CNTL_EFUSE_CLK_FORCE_NOGATING_S  2
/* RTC_CNTL_EFUSE_CLK_FORCE_GATING :  ;bitpos:[1] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_EFUSE_CLK_FORCE_GATING    (BIT(1))
#define RTC_CNTL_EFUSE_CLK_FORCE_GATING_M  (BIT(1))
#define RTC_CNTL_EFUSE_CLK_FORCE_GATING_V  0x1
#define RTC_CNTL_EFUSE_CLK_FORCE_GATING_S  1

#define RTC_CNTL_SLOW_CLK_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x6C)
/* RTC_CNTL_SLOW_CLK_NEXT_EDGE :  ;bitpos:[31] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_SLOW_CLK_NEXT_EDGE    (BIT(31))
#define RTC_CNTL_SLOW_CLK_NEXT_EDGE_M  (BIT(31))
#define RTC_CNTL_SLOW_CLK_NEXT_EDGE_V  0x1
#define RTC_CNTL_SLOW_CLK_NEXT_EDGE_S  31
/* RTC_CNTL_ANA_CLK_DIV :  ;bitpos:[30:23] ;default: 8'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_ANA_CLK_DIV    0x000000FF
#define RTC_CNTL_ANA_CLK_DIV_M  ((RTC_CNTL_ANA_CLK_DIV_V)<<(RTC_CNTL_ANA_CLK_DIV_S))
#define RTC_CNTL_ANA_CLK_DIV_V  0xFF
#define RTC_CNTL_ANA_CLK_DIV_S  23
/* RTC_CNTL_ANA_CLK_DIV_VLD :  ;bitpos:[22] ;default: 1'b1 ; */
/*description: used to sync div bus. clear vld before set reg_rtc_ana_clk_div.*/
#define RTC_CNTL_ANA_CLK_DIV_VLD    (BIT(22))
#define RTC_CNTL_ANA_CLK_DIV_VLD_M  (BIT(22))
#define RTC_CNTL_ANA_CLK_DIV_VLD_V  0x1
#define RTC_CNTL_ANA_CLK_DIV_VLD_S  22

#define RTC_CNTL_BIAS_CONF_REG          (DR_REG_RTCCNTL_BASE + 0x70)
/* RTC_CNTL_DBG_ATTEN_ACTIVE :  ;bitpos:[29:26] ;default: 4'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DBG_ATTEN_ACTIVE    0x0000000F
#define RTC_CNTL_DBG_ATTEN_ACTIVE_M  ((RTC_CNTL_DBG_ATTEN_ACTIVE_V)<<(RTC_CNTL_DBG_ATTEN_ACTIVE_S))
#define RTC_CNTL_DBG_ATTEN_ACTIVE_V  0xF
#define RTC_CNTL_DBG_ATTEN_ACTIVE_S  26
/* RTC_CNTL_DBG_ATTEN_MONITOR :  ;bitpos:[25:22] ;default: 4'd0 ; */
/*description: DBG_ATTEN when rtc in active state.*/
#define RTC_CNTL_DBG_ATTEN_MONITOR    0x0000000F
#define RTC_CNTL_DBG_ATTEN_MONITOR_M  ((RTC_CNTL_DBG_ATTEN_MONITOR_V)<<(RTC_CNTL_DBG_ATTEN_MONITOR_S))
#define RTC_CNTL_DBG_ATTEN_MONITOR_V  0xF
#define RTC_CNTL_DBG_ATTEN_MONITOR_S  22
/* RTC_CNTL_DBG_ATTEN_DEEP_SLP :  ;bitpos:[21:18] ;default: 4'd0 ; */
/*description: DBG_ATTEN when rtc in sleep state.*/
#define RTC_CNTL_DBG_ATTEN_DEEP_SLP    0x0000000F
#define RTC_CNTL_DBG_ATTEN_DEEP_SLP_M  ((RTC_CNTL_DBG_ATTEN_DEEP_SLP_V)<<(RTC_CNTL_DBG_ATTEN_DEEP_SLP_S))
#define RTC_CNTL_DBG_ATTEN_DEEP_SLP_V  0xF
#define RTC_CNTL_DBG_ATTEN_DEEP_SLP_S  18
/* RTC_CNTL_BIAS_SLEEP_MONITOR :  ;bitpos:[17] ;default: 1'b0 ; */
/*description: bias_sleep when rtc in monitor state.*/
#define RTC_CNTL_BIAS_SLEEP_MONITOR    (BIT(17))
#define RTC_CNTL_BIAS_SLEEP_MONITOR_M  (BIT(17))
#define RTC_CNTL_BIAS_SLEEP_MONITOR_V  0x1
#define RTC_CNTL_BIAS_SLEEP_MONITOR_S  17
/* RTC_CNTL_BIAS_SLEEP_DEEP_SLP :  ;bitpos:[16] ;default: 1'b1 ; */
/*description: bias_sleep when rtc in sleep_state.*/
#define RTC_CNTL_BIAS_SLEEP_DEEP_SLP    (BIT(16))
#define RTC_CNTL_BIAS_SLEEP_DEEP_SLP_M  (BIT(16))
#define RTC_CNTL_BIAS_SLEEP_DEEP_SLP_V  0x1
#define RTC_CNTL_BIAS_SLEEP_DEEP_SLP_S  16
/* RTC_CNTL_PD_CUR_MONITOR :  ;bitpos:[15] ;default: 1'b0 ; */
/*description: xpd cur when rtc in monitor state.*/
#define RTC_CNTL_PD_CUR_MONITOR    (BIT(15))
#define RTC_CNTL_PD_CUR_MONITOR_M  (BIT(15))
#define RTC_CNTL_PD_CUR_MONITOR_V  0x1
#define RTC_CNTL_PD_CUR_MONITOR_S  15
/* RTC_CNTL_PD_CUR_DEEP_SLP :  ;bitpos:[14] ;default: 1'b0 ; */
/*description: xpd cur when rtc in sleep_state.*/
#define RTC_CNTL_PD_CUR_DEEP_SLP    (BIT(14))
#define RTC_CNTL_PD_CUR_DEEP_SLP_M  (BIT(14))
#define RTC_CNTL_PD_CUR_DEEP_SLP_V  0x1
#define RTC_CNTL_PD_CUR_DEEP_SLP_S  14
/* RTC_CNTL_BIAS_BUF_MONITOR :  ;bitpos:[13] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_BIAS_BUF_MONITOR    (BIT(13))
#define RTC_CNTL_BIAS_BUF_MONITOR_M  (BIT(13))
#define RTC_CNTL_BIAS_BUF_MONITOR_V  0x1
#define RTC_CNTL_BIAS_BUF_MONITOR_S  13
/* RTC_CNTL_BIAS_BUF_DEEP_SLP :  ;bitpos:[12] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_BIAS_BUF_DEEP_SLP    (BIT(12))
#define RTC_CNTL_BIAS_BUF_DEEP_SLP_M  (BIT(12))
#define RTC_CNTL_BIAS_BUF_DEEP_SLP_V  0x1
#define RTC_CNTL_BIAS_BUF_DEEP_SLP_S  12
/* RTC_CNTL_BIAS_BUF_WAKE :  ;bitpos:[11] ;default: 1'b1 ; */
/*description: Need add desc.*/
#define RTC_CNTL_BIAS_BUF_WAKE    (BIT(11))
#define RTC_CNTL_BIAS_BUF_WAKE_M  (BIT(11))
#define RTC_CNTL_BIAS_BUF_WAKE_V  0x1
#define RTC_CNTL_BIAS_BUF_WAKE_S  11
/* RTC_CNTL_BIAS_BUF_IDLE :  ;bitpos:[10] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_BIAS_BUF_IDLE    (BIT(10))
#define RTC_CNTL_BIAS_BUF_IDLE_M  (BIT(10))
#define RTC_CNTL_BIAS_BUF_IDLE_V  0x1
#define RTC_CNTL_BIAS_BUF_IDLE_S  10
/* RTC_CNTL_DG_VDD_DRV_B_SLP_EN :  ;bitpos:[8] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DG_VDD_DRV_B_SLP_EN    (BIT(8))
#define RTC_CNTL_DG_VDD_DRV_B_SLP_EN_M  (BIT(8))
#define RTC_CNTL_DG_VDD_DRV_B_SLP_EN_V  0x1
#define RTC_CNTL_DG_VDD_DRV_B_SLP_EN_S  8
/* RTC_CNTL_DG_VDD_DRV_B_SLP :  ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DG_VDD_DRV_B_SLP    0x000000FF
#define RTC_CNTL_DG_VDD_DRV_B_SLP_M  ((RTC_CNTL_DG_VDD_DRV_B_SLP_V)<<(RTC_CNTL_DG_VDD_DRV_B_SLP_S))
#define RTC_CNTL_DG_VDD_DRV_B_SLP_V  0xFF
#define RTC_CNTL_DG_VDD_DRV_B_SLP_S  0

#define RTC_CNTL_REG          (DR_REG_RTCCNTL_BASE + 0x74)
/* RTC_CNTL_REGULATOR_FORCE_PU :  ;bitpos:[31] ;default: 1'd1 ; */
/*description: Need add desc.*/
#define RTC_CNTL_REGULATOR_FORCE_PU    (BIT(31))
#define RTC_CNTL_REGULATOR_FORCE_PU_M  (BIT(31))
#define RTC_CNTL_REGULATOR_FORCE_PU_V  0x1
#define RTC_CNTL_REGULATOR_FORCE_PU_S  31
/* RTC_CNTL_REGULATOR_FORCE_PD :  ;bitpos:[30] ;default: 1'd0 ; */
/*description: RTC_REG force power down (for RTC_REG power down means decrease the voltage to 0
.8v or lower ).*/
#define RTC_CNTL_REGULATOR_FORCE_PD    (BIT(30))
#define RTC_CNTL_REGULATOR_FORCE_PD_M  (BIT(30))
#define RTC_CNTL_REGULATOR_FORCE_PD_V  0x1
#define RTC_CNTL_REGULATOR_FORCE_PD_S  30
/* RTC_CNTL_SCK_DCAP :  ;bitpos:[21:14] ;default: 8'd0 ; */
/*description: SCK_DCAP.*/
#define RTC_CNTL_SCK_DCAP    0x000000FF
#define RTC_CNTL_SCK_DCAP_M  ((RTC_CNTL_SCK_DCAP_V)<<(RTC_CNTL_SCK_DCAP_S))
#define RTC_CNTL_SCK_DCAP_V  0xFF
#define RTC_CNTL_SCK_DCAP_S  14
/* RTC_CNTL_DIG_REG_CAL_EN :  ;bitpos:[7] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DIG_REG_CAL_EN    (BIT(7))
#define RTC_CNTL_DIG_REG_CAL_EN_M  (BIT(7))
#define RTC_CNTL_DIG_REG_CAL_EN_V  0x1
#define RTC_CNTL_DIG_REG_CAL_EN_S  7

#define RTC_CNTL_PWC_REG          (DR_REG_RTCCNTL_BASE + 0x78)
/* RTC_CNTL_PAD_FORCE_HOLD :  ;bitpos:[21] ;default: 1'd0 ; */
/*description: rtc pad force hold.*/
#define RTC_CNTL_PAD_FORCE_HOLD    (BIT(21))
#define RTC_CNTL_PAD_FORCE_HOLD_M  (BIT(21))
#define RTC_CNTL_PAD_FORCE_HOLD_V  0x1
#define RTC_CNTL_PAD_FORCE_HOLD_S  21

#define RTC_CNTL_DIG_PWC_REG          (DR_REG_RTCCNTL_BASE + 0x7C)
/* RTC_CNTL_DG_WRAP_PD_EN :  ;bitpos:[31] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DG_WRAP_PD_EN    (BIT(31))
#define RTC_CNTL_DG_WRAP_PD_EN_M  (BIT(31))
#define RTC_CNTL_DG_WRAP_PD_EN_V  0x1
#define RTC_CNTL_DG_WRAP_PD_EN_S  31
/* RTC_CNTL_DG_WRAP_FORCE_PU :  ;bitpos:[20] ;default: 1'd1 ; */
/*description: digital core force power up.*/
#define RTC_CNTL_DG_WRAP_FORCE_PU    (BIT(20))
#define RTC_CNTL_DG_WRAP_FORCE_PU_M  (BIT(20))
#define RTC_CNTL_DG_WRAP_FORCE_PU_V  0x1
#define RTC_CNTL_DG_WRAP_FORCE_PU_S  20
/* RTC_CNTL_DG_WRAP_FORCE_PD :  ;bitpos:[19] ;default: 1'b0 ; */
/*description: digital core force power down.*/
#define RTC_CNTL_DG_WRAP_FORCE_PD    (BIT(19))
#define RTC_CNTL_DG_WRAP_FORCE_PD_M  (BIT(19))
#define RTC_CNTL_DG_WRAP_FORCE_PD_V  0x1
#define RTC_CNTL_DG_WRAP_FORCE_PD_S  19
/* RTC_CNTL_LSLP_MEM_FORCE_PU :  ;bitpos:[5] ;default: 1'b1 ; */
/*description: memories in digital core force no PD in sleep.*/
#define RTC_CNTL_LSLP_MEM_FORCE_PU    (BIT(5))
#define RTC_CNTL_LSLP_MEM_FORCE_PU_M  (BIT(5))
#define RTC_CNTL_LSLP_MEM_FORCE_PU_V  0x1
#define RTC_CNTL_LSLP_MEM_FORCE_PU_S  5
/* RTC_CNTL_LSLP_MEM_FORCE_PD :  ;bitpos:[4] ;default: 1'b0 ; */
/*description: memories in digital core force PD in sleep.*/
#define RTC_CNTL_LSLP_MEM_FORCE_PD    (BIT(4))
#define RTC_CNTL_LSLP_MEM_FORCE_PD_M  (BIT(4))
#define RTC_CNTL_LSLP_MEM_FORCE_PD_V  0x1
#define RTC_CNTL_LSLP_MEM_FORCE_PD_S  4
/* RTC_CNTL_VDD_SPI_PD_EN :  ;bitpos:[3] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_VDD_SPI_PD_EN    (BIT(3))
#define RTC_CNTL_VDD_SPI_PD_EN_M  (BIT(3))
#define RTC_CNTL_VDD_SPI_PD_EN_V  0x1
#define RTC_CNTL_VDD_SPI_PD_EN_S  3
/* RTC_CNTL_VDD_SPI_PWR_FORCE :  ;bitpos:[2] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_VDD_SPI_PWR_FORCE    (BIT(2))
#define RTC_CNTL_VDD_SPI_PWR_FORCE_M  (BIT(2))
#define RTC_CNTL_VDD_SPI_PWR_FORCE_V  0x1
#define RTC_CNTL_VDD_SPI_PWR_FORCE_S  2
/* RTC_CNTL_VDD_SPI_PWR_DRV :  ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_VDD_SPI_PWR_DRV    0x00000003
#define RTC_CNTL_VDD_SPI_PWR_DRV_M  ((RTC_CNTL_VDD_SPI_PWR_DRV_V)<<(RTC_CNTL_VDD_SPI_PWR_DRV_S))
#define RTC_CNTL_VDD_SPI_PWR_DRV_V  0x3
#define RTC_CNTL_VDD_SPI_PWR_DRV_S  0

#define RTC_CNTL_DIG_ISO_REG          (DR_REG_RTCCNTL_BASE + 0x80)
/* RTC_CNTL_DG_WRAP_FORCE_NOISO :  ;bitpos:[31] ;default: 1'd1 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DG_WRAP_FORCE_NOISO    (BIT(31))
#define RTC_CNTL_DG_WRAP_FORCE_NOISO_M  (BIT(31))
#define RTC_CNTL_DG_WRAP_FORCE_NOISO_V  0x1
#define RTC_CNTL_DG_WRAP_FORCE_NOISO_S  31
/* RTC_CNTL_DG_WRAP_FORCE_ISO :  ;bitpos:[30] ;default: 1'd0 ; */
/*description: digital core force ISO.*/
#define RTC_CNTL_DG_WRAP_FORCE_ISO    (BIT(30))
#define RTC_CNTL_DG_WRAP_FORCE_ISO_M  (BIT(30))
#define RTC_CNTL_DG_WRAP_FORCE_ISO_V  0x1
#define RTC_CNTL_DG_WRAP_FORCE_ISO_S  30
/* RTC_CNTL_DG_PAD_FORCE_HOLD :  ;bitpos:[15] ;default: 1'd0 ; */
/*description: digital pad force hold.*/
#define RTC_CNTL_DG_PAD_FORCE_HOLD    (BIT(15))
#define RTC_CNTL_DG_PAD_FORCE_HOLD_M  (BIT(15))
#define RTC_CNTL_DG_PAD_FORCE_HOLD_V  0x1
#define RTC_CNTL_DG_PAD_FORCE_HOLD_S  15
/* RTC_CNTL_DG_PAD_FORCE_UNHOLD :  ;bitpos:[14] ;default: 1'd1 ; */
/*description: digital pad force un-hold.*/
#define RTC_CNTL_DG_PAD_FORCE_UNHOLD    (BIT(14))
#define RTC_CNTL_DG_PAD_FORCE_UNHOLD_M  (BIT(14))
#define RTC_CNTL_DG_PAD_FORCE_UNHOLD_V  0x1
#define RTC_CNTL_DG_PAD_FORCE_UNHOLD_S  14
/* RTC_CNTL_DG_PAD_FORCE_ISO :  ;bitpos:[13] ;default: 1'd0 ; */
/*description: digital pad force ISO.*/
#define RTC_CNTL_DG_PAD_FORCE_ISO    (BIT(13))
#define RTC_CNTL_DG_PAD_FORCE_ISO_M  (BIT(13))
#define RTC_CNTL_DG_PAD_FORCE_ISO_V  0x1
#define RTC_CNTL_DG_PAD_FORCE_ISO_S  13
/* RTC_CNTL_DG_PAD_FORCE_NOISO :  ;bitpos:[12] ;default: 1'd1 ; */
/*description: digital pad force no ISO.*/
#define RTC_CNTL_DG_PAD_FORCE_NOISO    (BIT(12))
#define RTC_CNTL_DG_PAD_FORCE_NOISO_M  (BIT(12))
#define RTC_CNTL_DG_PAD_FORCE_NOISO_V  0x1
#define RTC_CNTL_DG_PAD_FORCE_NOISO_S  12
/* RTC_CNTL_DG_PAD_AUTOHOLD_EN :  ;bitpos:[11] ;default: 1'd0 ; */
/*description: digital pad enable auto-hold.*/
#define RTC_CNTL_DG_PAD_AUTOHOLD_EN    (BIT(11))
#define RTC_CNTL_DG_PAD_AUTOHOLD_EN_M  (BIT(11))
#define RTC_CNTL_DG_PAD_AUTOHOLD_EN_V  0x1
#define RTC_CNTL_DG_PAD_AUTOHOLD_EN_S  11
/* RTC_CNTL_CLR_DG_PAD_AUTOHOLD :  ;bitpos:[10] ;default: 1'd0 ; */
/*description: wtite only register to clear digital pad auto-hold.*/
#define RTC_CNTL_CLR_DG_PAD_AUTOHOLD    (BIT(10))
#define RTC_CNTL_CLR_DG_PAD_AUTOHOLD_M  (BIT(10))
#define RTC_CNTL_CLR_DG_PAD_AUTOHOLD_V  0x1
#define RTC_CNTL_CLR_DG_PAD_AUTOHOLD_S  10
/* RTC_CNTL_DG_PAD_AUTOHOLD :  ;bitpos:[9] ;default: 1'd0 ; */
/*description: read only register to indicate digital pad auto-hold status.*/
#define RTC_CNTL_DG_PAD_AUTOHOLD    (BIT(9))
#define RTC_CNTL_DG_PAD_AUTOHOLD_M  (BIT(9))
#define RTC_CNTL_DG_PAD_AUTOHOLD_V  0x1
#define RTC_CNTL_DG_PAD_AUTOHOLD_S  9
/* RTC_CNTL_DIG_ISO_FORCE_ON :  ;bitpos:[8] ;default: 1'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DIG_ISO_FORCE_ON    (BIT(8))
#define RTC_CNTL_DIG_ISO_FORCE_ON_M  (BIT(8))
#define RTC_CNTL_DIG_ISO_FORCE_ON_V  0x1
#define RTC_CNTL_DIG_ISO_FORCE_ON_S  8
/* RTC_CNTL_DIG_ISO_FORCE_OFF :  ;bitpos:[7] ;default: 1'd1 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DIG_ISO_FORCE_OFF    (BIT(7))
#define RTC_CNTL_DIG_ISO_FORCE_OFF_M  (BIT(7))
#define RTC_CNTL_DIG_ISO_FORCE_OFF_V  0x1
#define RTC_CNTL_DIG_ISO_FORCE_OFF_S  7

#define RTC_CNTL_WDTCONFIG0_REG          (DR_REG_RTCCNTL_BASE + 0x84)
/* RTC_CNTL_WDT_EN :  ;bitpos:[31] ;default: 1'h0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_WDT_EN    (BIT(31))
#define RTC_CNTL_WDT_EN_M  (BIT(31))
#define RTC_CNTL_WDT_EN_V  0x1
#define RTC_CNTL_WDT_EN_S  31
/* RTC_CNTL_WDT_STG0 :  ;bitpos:[30:28] ;default: 3'h0 ; */
/*description: 1: interrupt stage en.*/
#define RTC_CNTL_WDT_STG0    0x00000007
#define RTC_CNTL_WDT_STG0_M  ((RTC_CNTL_WDT_STG0_V)<<(RTC_CNTL_WDT_STG0_S))
#define RTC_CNTL_WDT_STG0_V  0x7
#define RTC_CNTL_WDT_STG0_S  28
/* RTC_CNTL_WDT_STG1 :  ;bitpos:[27:25] ;default: 3'h0 ; */
/*description: 1: interrupt stage en.*/
#define RTC_CNTL_WDT_STG1    0x00000007
#define RTC_CNTL_WDT_STG1_M  ((RTC_CNTL_WDT_STG1_V)<<(RTC_CNTL_WDT_STG1_S))
#define RTC_CNTL_WDT_STG1_V  0x7
#define RTC_CNTL_WDT_STG1_S  25
/* RTC_CNTL_WDT_STG2 :  ;bitpos:[24:22] ;default: 3'h0 ; */
/*description: 1: interrupt stage en.*/
#define RTC_CNTL_WDT_STG2    0x00000007
#define RTC_CNTL_WDT_STG2_M  ((RTC_CNTL_WDT_STG2_V)<<(RTC_CNTL_WDT_STG2_S))
#define RTC_CNTL_WDT_STG2_V  0x7
#define RTC_CNTL_WDT_STG2_S  22
/* RTC_CNTL_WDT_STG3 :  ;bitpos:[21:19] ;default: 3'h0 ; */
/*description: 1: interrupt stage en.*/
#define RTC_CNTL_WDT_STG3    0x00000007
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

/* RTC_CNTL_WDT_CPU_RESET_LENGTH :  ;bitpos:[18:16] ;default: 3'h1 ; */
/*description: CPU reset counter length.*/
#define RTC_CNTL_WDT_CPU_RESET_LENGTH    0x00000007
#define RTC_CNTL_WDT_CPU_RESET_LENGTH_M  ((RTC_CNTL_WDT_CPU_RESET_LENGTH_V)<<(RTC_CNTL_WDT_CPU_RESET_LENGTH_S))
#define RTC_CNTL_WDT_CPU_RESET_LENGTH_V  0x7
#define RTC_CNTL_WDT_CPU_RESET_LENGTH_S  16
/* RTC_CNTL_WDT_SYS_RESET_LENGTH :  ;bitpos:[15:13] ;default: 3'h1 ; */
/*description: system reset counter length.*/
#define RTC_CNTL_WDT_SYS_RESET_LENGTH    0x00000007
#define RTC_CNTL_WDT_SYS_RESET_LENGTH_M  ((RTC_CNTL_WDT_SYS_RESET_LENGTH_V)<<(RTC_CNTL_WDT_SYS_RESET_LENGTH_S))
#define RTC_CNTL_WDT_SYS_RESET_LENGTH_V  0x7
#define RTC_CNTL_WDT_SYS_RESET_LENGTH_S  13
/* RTC_CNTL_WDT_FLASHBOOT_MOD_EN :  ;bitpos:[12] ;default: 1'h1 ; */
/*description: enable WDT in flash boot.*/
#define RTC_CNTL_WDT_FLASHBOOT_MOD_EN    (BIT(12))
#define RTC_CNTL_WDT_FLASHBOOT_MOD_EN_M  (BIT(12))
#define RTC_CNTL_WDT_FLASHBOOT_MOD_EN_V  0x1
#define RTC_CNTL_WDT_FLASHBOOT_MOD_EN_S  12
/* RTC_CNTL_WDT_PROCPU_RESET_EN :  ;bitpos:[11] ;default: 1'd0 ; */
/*description: enable WDT reset PRO CPU.*/
#define RTC_CNTL_WDT_PROCPU_RESET_EN    (BIT(11))
#define RTC_CNTL_WDT_PROCPU_RESET_EN_M  (BIT(11))
#define RTC_CNTL_WDT_PROCPU_RESET_EN_V  0x1
#define RTC_CNTL_WDT_PROCPU_RESET_EN_S  11
/* RTC_CNTL_WDT_PAUSE_IN_SLP :  ;bitpos:[9] ;default: 1'd1 ; */
/*description: pause WDT in sleep.*/
#define RTC_CNTL_WDT_PAUSE_IN_SLP    (BIT(9))
#define RTC_CNTL_WDT_PAUSE_IN_SLP_M  (BIT(9))
#define RTC_CNTL_WDT_PAUSE_IN_SLP_V  0x1
#define RTC_CNTL_WDT_PAUSE_IN_SLP_S  9
/* RTC_CNTL_WDT_CHIP_RESET_EN :  ;bitpos:[8] ;default: 1'b0 ; */
/*description: wdt reset whole chip enable.*/
#define RTC_CNTL_WDT_CHIP_RESET_EN    (BIT(8))
#define RTC_CNTL_WDT_CHIP_RESET_EN_M  (BIT(8))
#define RTC_CNTL_WDT_CHIP_RESET_EN_V  0x1
#define RTC_CNTL_WDT_CHIP_RESET_EN_S  8
/* RTC_CNTL_WDT_CHIP_RESET_WIDTH :  ;bitpos:[7:0] ;default: 8'd20 ; */
/*description: chip reset siginal pulse width.*/
#define RTC_CNTL_WDT_CHIP_RESET_WIDTH    0x000000FF
#define RTC_CNTL_WDT_CHIP_RESET_WIDTH_M  ((RTC_CNTL_WDT_CHIP_RESET_WIDTH_V)<<(RTC_CNTL_WDT_CHIP_RESET_WIDTH_S))
#define RTC_CNTL_WDT_CHIP_RESET_WIDTH_V  0xFF
#define RTC_CNTL_WDT_CHIP_RESET_WIDTH_S  0

#define RTC_CNTL_WDTCONFIG1_REG          (DR_REG_RTCCNTL_BASE + 0x88)
/* RTC_CNTL_WDT_STG0_HOLD :  ;bitpos:[31:0] ;default: 32'd200000 ; */
/*description: Need add desc.*/
#define RTC_CNTL_WDT_STG0_HOLD    0xFFFFFFFF
#define RTC_CNTL_WDT_STG0_HOLD_M  ((RTC_CNTL_WDT_STG0_HOLD_V)<<(RTC_CNTL_WDT_STG0_HOLD_S))
#define RTC_CNTL_WDT_STG0_HOLD_V  0xFFFFFFFF
#define RTC_CNTL_WDT_STG0_HOLD_S  0

#define RTC_CNTL_WDTCONFIG2_REG          (DR_REG_RTCCNTL_BASE + 0x8C)
/* RTC_CNTL_WDT_STG1_HOLD :  ;bitpos:[31:0] ;default: 32'd80000 ; */
/*description: Need add desc.*/
#define RTC_CNTL_WDT_STG1_HOLD    0xFFFFFFFF
#define RTC_CNTL_WDT_STG1_HOLD_M  ((RTC_CNTL_WDT_STG1_HOLD_V)<<(RTC_CNTL_WDT_STG1_HOLD_S))
#define RTC_CNTL_WDT_STG1_HOLD_V  0xFFFFFFFF
#define RTC_CNTL_WDT_STG1_HOLD_S  0

#define RTC_CNTL_WDTCONFIG3_REG          (DR_REG_RTCCNTL_BASE + 0x90)
/* RTC_CNTL_WDT_STG2_HOLD :  ;bitpos:[31:0] ;default: 32'hfff ; */
/*description: Need add desc.*/
#define RTC_CNTL_WDT_STG2_HOLD    0xFFFFFFFF
#define RTC_CNTL_WDT_STG2_HOLD_M  ((RTC_CNTL_WDT_STG2_HOLD_V)<<(RTC_CNTL_WDT_STG2_HOLD_S))
#define RTC_CNTL_WDT_STG2_HOLD_V  0xFFFFFFFF
#define RTC_CNTL_WDT_STG2_HOLD_S  0

#define RTC_CNTL_WDTCONFIG4_REG          (DR_REG_RTCCNTL_BASE + 0x94)
/* RTC_CNTL_WDT_STG3_HOLD :  ;bitpos:[31:0] ;default: 32'hfff ; */
/*description: Need add desc.*/
#define RTC_CNTL_WDT_STG3_HOLD    0xFFFFFFFF
#define RTC_CNTL_WDT_STG3_HOLD_M  ((RTC_CNTL_WDT_STG3_HOLD_V)<<(RTC_CNTL_WDT_STG3_HOLD_S))
#define RTC_CNTL_WDT_STG3_HOLD_V  0xFFFFFFFF
#define RTC_CNTL_WDT_STG3_HOLD_S  0

#define RTC_CNTL_WDTFEED_REG          (DR_REG_RTCCNTL_BASE + 0x98)
/* RTC_CNTL_WDT_FEED :  ;bitpos:[31] ;default: 1'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_WDT_FEED    (BIT(31))
#define RTC_CNTL_WDT_FEED_M  (BIT(31))
#define RTC_CNTL_WDT_FEED_V  0x1
#define RTC_CNTL_WDT_FEED_S  31

#define RTC_CNTL_WDTWPROTECT_REG          (DR_REG_RTCCNTL_BASE + 0x9C)
/* RTC_CNTL_WDT_WKEY :  ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_WDT_WKEY    0xFFFFFFFF
#define RTC_CNTL_WDT_WKEY_M  ((RTC_CNTL_WDT_WKEY_V)<<(RTC_CNTL_WDT_WKEY_S))
#define RTC_CNTL_WDT_WKEY_V  0xFFFFFFFF
#define RTC_CNTL_WDT_WKEY_S  0

#define RTC_CNTL_SWD_CONF_REG          (DR_REG_RTCCNTL_BASE + 0xA0)
/* RTC_CNTL_SWD_AUTO_FEED_EN :  ;bitpos:[31] ;default: 1'b0 ; */
/*description: automatically feed swd when int comes.*/
#define RTC_CNTL_SWD_AUTO_FEED_EN    (BIT(31))
#define RTC_CNTL_SWD_AUTO_FEED_EN_M  (BIT(31))
#define RTC_CNTL_SWD_AUTO_FEED_EN_V  0x1
#define RTC_CNTL_SWD_AUTO_FEED_EN_S  31
/* RTC_CNTL_SWD_DISABLE :  ;bitpos:[30] ;default: 1'b0 ; */
/*description: disable SWD.*/
#define RTC_CNTL_SWD_DISABLE    (BIT(30))
#define RTC_CNTL_SWD_DISABLE_M  (BIT(30))
#define RTC_CNTL_SWD_DISABLE_V  0x1
#define RTC_CNTL_SWD_DISABLE_S  30
/* RTC_CNTL_SWD_FEED :  ;bitpos:[29] ;default: 1'b0 ; */
/*description: Sw feed swd.*/
#define RTC_CNTL_SWD_FEED    (BIT(29))
#define RTC_CNTL_SWD_FEED_M  (BIT(29))
#define RTC_CNTL_SWD_FEED_V  0x1
#define RTC_CNTL_SWD_FEED_S  29
/* RTC_CNTL_SWD_RST_FLAG_CLR :  ;bitpos:[28] ;default: 1'b0 ; */
/*description: reset swd reset flag.*/
#define RTC_CNTL_SWD_RST_FLAG_CLR    (BIT(28))
#define RTC_CNTL_SWD_RST_FLAG_CLR_M  (BIT(28))
#define RTC_CNTL_SWD_RST_FLAG_CLR_V  0x1
#define RTC_CNTL_SWD_RST_FLAG_CLR_S  28
/* RTC_CNTL_SWD_SIGNAL_WIDTH :  ;bitpos:[27:18] ;default: 10'd300 ; */
/*description: adjust signal width send to swd.*/
#define RTC_CNTL_SWD_SIGNAL_WIDTH    0x000003FF
#define RTC_CNTL_SWD_SIGNAL_WIDTH_M  ((RTC_CNTL_SWD_SIGNAL_WIDTH_V)<<(RTC_CNTL_SWD_SIGNAL_WIDTH_S))
#define RTC_CNTL_SWD_SIGNAL_WIDTH_V  0x3FF
#define RTC_CNTL_SWD_SIGNAL_WIDTH_S  18
/* RTC_CNTL_SWD_BYPASS_RST :  ;bitpos:[17] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_SWD_BYPASS_RST    (BIT(17))
#define RTC_CNTL_SWD_BYPASS_RST_M  (BIT(17))
#define RTC_CNTL_SWD_BYPASS_RST_V  0x1
#define RTC_CNTL_SWD_BYPASS_RST_S  17
/* RTC_CNTL_SWD_FEED_INT :  ;bitpos:[1] ;default: 1'b0 ; */
/*description: swd interrupt for feeding.*/
#define RTC_CNTL_SWD_FEED_INT    (BIT(1))
#define RTC_CNTL_SWD_FEED_INT_M  (BIT(1))
#define RTC_CNTL_SWD_FEED_INT_V  0x1
#define RTC_CNTL_SWD_FEED_INT_S  1
/* RTC_CNTL_SWD_RESET_FLAG :  ;bitpos:[0] ;default: 1'b0 ; */
/*description: swd reset flag.*/
#define RTC_CNTL_SWD_RESET_FLAG    (BIT(0))
#define RTC_CNTL_SWD_RESET_FLAG_M  (BIT(0))
#define RTC_CNTL_SWD_RESET_FLAG_V  0x1
#define RTC_CNTL_SWD_RESET_FLAG_S  0

#define RTC_CNTL_SWD_WPROTECT_REG          (DR_REG_RTCCNTL_BASE + 0xA4)
/* RTC_CNTL_SWD_WKEY :  ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_SWD_WKEY    0xFFFFFFFF
#define RTC_CNTL_SWD_WKEY_M  ((RTC_CNTL_SWD_WKEY_V)<<(RTC_CNTL_SWD_WKEY_S))
#define RTC_CNTL_SWD_WKEY_V  0xFFFFFFFF
#define RTC_CNTL_SWD_WKEY_S  0

#define RTC_CNTL_SW_CPU_STALL_REG          (DR_REG_RTCCNTL_BASE + 0xA8)
/* RTC_CNTL_SW_STALL_PROCPU_C1 :  ;bitpos:[31:26] ;default: 6'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_SW_STALL_PROCPU_C1    0x0000003F
#define RTC_CNTL_SW_STALL_PROCPU_C1_M  ((RTC_CNTL_SW_STALL_PROCPU_C1_V)<<(RTC_CNTL_SW_STALL_PROCPU_C1_S))
#define RTC_CNTL_SW_STALL_PROCPU_C1_V  0x3F
#define RTC_CNTL_SW_STALL_PROCPU_C1_S  26

#define RTC_CNTL_STORE4_REG          (DR_REG_RTCCNTL_BASE + 0xAC)
/* RTC_CNTL_SCRATCH4 :  ;bitpos:[31:0] ;default: 0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_SCRATCH4    0xFFFFFFFF
#define RTC_CNTL_SCRATCH4_M  ((RTC_CNTL_SCRATCH4_V)<<(RTC_CNTL_SCRATCH4_S))
#define RTC_CNTL_SCRATCH4_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH4_S  0

#define RTC_CNTL_STORE5_REG          (DR_REG_RTCCNTL_BASE + 0xB0)
/* RTC_CNTL_SCRATCH5 :  ;bitpos:[31:0] ;default: 0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_SCRATCH5    0xFFFFFFFF
#define RTC_CNTL_SCRATCH5_M  ((RTC_CNTL_SCRATCH5_V)<<(RTC_CNTL_SCRATCH5_S))
#define RTC_CNTL_SCRATCH5_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH5_S  0

#define RTC_CNTL_STORE6_REG          (DR_REG_RTCCNTL_BASE + 0xB4)
/* RTC_CNTL_SCRATCH6 :  ;bitpos:[31:0] ;default: 0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_SCRATCH6    0xFFFFFFFF
#define RTC_CNTL_SCRATCH6_M  ((RTC_CNTL_SCRATCH6_V)<<(RTC_CNTL_SCRATCH6_S))
#define RTC_CNTL_SCRATCH6_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH6_S  0

#define RTC_CNTL_STORE7_REG          (DR_REG_RTCCNTL_BASE + 0xB8)
/* RTC_CNTL_SCRATCH7 :  ;bitpos:[31:0] ;default: 0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_SCRATCH7    0xFFFFFFFF
#define RTC_CNTL_SCRATCH7_M  ((RTC_CNTL_SCRATCH7_V)<<(RTC_CNTL_SCRATCH7_S))
#define RTC_CNTL_SCRATCH7_V  0xFFFFFFFF
#define RTC_CNTL_SCRATCH7_S  0

#define RTC_CNTL_LOW_POWER_ST_REG          (DR_REG_RTCCNTL_BASE + 0xBC)
/* RTC_CNTL_MAIN_STATE :  ;bitpos:[31:28] ;default: 4'd0 ; */
/*description: rtc main state machine status.*/
#define RTC_CNTL_MAIN_STATE    0x0000000F
#define RTC_CNTL_MAIN_STATE_M  ((RTC_CNTL_MAIN_STATE_V)<<(RTC_CNTL_MAIN_STATE_S))
#define RTC_CNTL_MAIN_STATE_V  0xF
#define RTC_CNTL_MAIN_STATE_S  28
/* RTC_CNTL_MAIN_STATE_IN_IDLE :  ;bitpos:[27] ;default: 1'b0 ; */
/*description: rtc main state machine is in idle state.*/
#define RTC_CNTL_MAIN_STATE_IN_IDLE    (BIT(27))
#define RTC_CNTL_MAIN_STATE_IN_IDLE_M  (BIT(27))
#define RTC_CNTL_MAIN_STATE_IN_IDLE_V  0x1
#define RTC_CNTL_MAIN_STATE_IN_IDLE_S  27
/* RTC_CNTL_MAIN_STATE_IN_SLP :  ;bitpos:[26] ;default: 1'b0 ; */
/*description: rtc main state machine is in sleep state.*/
#define RTC_CNTL_MAIN_STATE_IN_SLP    (BIT(26))
#define RTC_CNTL_MAIN_STATE_IN_SLP_M  (BIT(26))
#define RTC_CNTL_MAIN_STATE_IN_SLP_V  0x1
#define RTC_CNTL_MAIN_STATE_IN_SLP_S  26
/* RTC_CNTL_MAIN_STATE_IN_WAIT_XTL :  ;bitpos:[25] ;default: 1'b0 ; */
/*description: rtc main state machine is in wait xtal state.*/
#define RTC_CNTL_MAIN_STATE_IN_WAIT_XTL    (BIT(25))
#define RTC_CNTL_MAIN_STATE_IN_WAIT_XTL_M  (BIT(25))
#define RTC_CNTL_MAIN_STATE_IN_WAIT_XTL_V  0x1
#define RTC_CNTL_MAIN_STATE_IN_WAIT_XTL_S  25
/* RTC_CNTL_MAIN_STATE_IN_WAIT_PLL :  ;bitpos:[24] ;default: 1'b0 ; */
/*description: rtc main state machine is in wait pll state.*/
#define RTC_CNTL_MAIN_STATE_IN_WAIT_PLL    (BIT(24))
#define RTC_CNTL_MAIN_STATE_IN_WAIT_PLL_M  (BIT(24))
#define RTC_CNTL_MAIN_STATE_IN_WAIT_PLL_V  0x1
#define RTC_CNTL_MAIN_STATE_IN_WAIT_PLL_S  24
/* RTC_CNTL_MAIN_STATE_IN_WAIT_8M :  ;bitpos:[23] ;default: 1'b0 ; */
/*description: rtc main state machine is in wait 8m state.*/
#define RTC_CNTL_MAIN_STATE_IN_WAIT_8M    (BIT(23))
#define RTC_CNTL_MAIN_STATE_IN_WAIT_8M_M  (BIT(23))
#define RTC_CNTL_MAIN_STATE_IN_WAIT_8M_V  0x1
#define RTC_CNTL_MAIN_STATE_IN_WAIT_8M_S  23
/* RTC_CNTL_IN_LOW_POWER_STATE :  ;bitpos:[22] ;default: 1'b0 ; */
/*description: rtc main state machine is in the states of low power.*/
#define RTC_CNTL_IN_LOW_POWER_STATE    (BIT(22))
#define RTC_CNTL_IN_LOW_POWER_STATE_M  (BIT(22))
#define RTC_CNTL_IN_LOW_POWER_STATE_V  0x1
#define RTC_CNTL_IN_LOW_POWER_STATE_S  22
/* RTC_CNTL_IN_WAKEUP_STATE :  ;bitpos:[21] ;default: 1'b0 ; */
/*description: rtc main state machine is in the states of wakeup process.*/
#define RTC_CNTL_IN_WAKEUP_STATE    (BIT(21))
#define RTC_CNTL_IN_WAKEUP_STATE_M  (BIT(21))
#define RTC_CNTL_IN_WAKEUP_STATE_V  0x1
#define RTC_CNTL_IN_WAKEUP_STATE_S  21
/* RTC_CNTL_MAIN_STATE_WAIT_END :  ;bitpos:[20] ;default: 1'b0 ; */
/*description: rtc main state machine has been waited for some cycles.*/
#define RTC_CNTL_MAIN_STATE_WAIT_END    (BIT(20))
#define RTC_CNTL_MAIN_STATE_WAIT_END_M  (BIT(20))
#define RTC_CNTL_MAIN_STATE_WAIT_END_V  0x1
#define RTC_CNTL_MAIN_STATE_WAIT_END_S  20
/* RTC_CNTL_RDY_FOR_WAKEUP :  ;bitpos:[19] ;default: 1'b0 ; */
/*description: rtc is ready to receive wake up trigger from wake up source.*/
#define RTC_CNTL_RDY_FOR_WAKEUP    (BIT(19))
#define RTC_CNTL_RDY_FOR_WAKEUP_M  (BIT(19))
#define RTC_CNTL_RDY_FOR_WAKEUP_V  0x1
#define RTC_CNTL_RDY_FOR_WAKEUP_S  19
/* RTC_CNTL_MAIN_STATE_PLL_ON :  ;bitpos:[18] ;default: 1'b0 ; */
/*description: rtc main state machine is in states that pll should be running.*/
#define RTC_CNTL_MAIN_STATE_PLL_ON    (BIT(18))
#define RTC_CNTL_MAIN_STATE_PLL_ON_M  (BIT(18))
#define RTC_CNTL_MAIN_STATE_PLL_ON_V  0x1
#define RTC_CNTL_MAIN_STATE_PLL_ON_S  18
/* RTC_CNTL_MAIN_STATE_XTAL_ISO :  ;bitpos:[17] ;default: 1'b0 ; */
/*description: no use any more.*/
#define RTC_CNTL_MAIN_STATE_XTAL_ISO    (BIT(17))
#define RTC_CNTL_MAIN_STATE_XTAL_ISO_M  (BIT(17))
#define RTC_CNTL_MAIN_STATE_XTAL_ISO_V  0x1
#define RTC_CNTL_MAIN_STATE_XTAL_ISO_S  17
/* RTC_CNTL_COCPU_STATE_DONE :  ;bitpos:[16] ;default: 1'b0 ; */
/*description: ulp/cocpu is done.*/
#define RTC_CNTL_COCPU_STATE_DONE    (BIT(16))
#define RTC_CNTL_COCPU_STATE_DONE_M  (BIT(16))
#define RTC_CNTL_COCPU_STATE_DONE_V  0x1
#define RTC_CNTL_COCPU_STATE_DONE_S  16
/* RTC_CNTL_COCPU_STATE_SLP :  ;bitpos:[15] ;default: 1'b0 ; */
/*description: ulp/cocpu is in sleep state.*/
#define RTC_CNTL_COCPU_STATE_SLP    (BIT(15))
#define RTC_CNTL_COCPU_STATE_SLP_M  (BIT(15))
#define RTC_CNTL_COCPU_STATE_SLP_V  0x1
#define RTC_CNTL_COCPU_STATE_SLP_S  15
/* RTC_CNTL_COCPU_STATE_SWITCH :  ;bitpos:[14] ;default: 1'b0 ; */
/*description: ulp/cocpu is about to working. Switch rtc main state.*/
#define RTC_CNTL_COCPU_STATE_SWITCH    (BIT(14))
#define RTC_CNTL_COCPU_STATE_SWITCH_M  (BIT(14))
#define RTC_CNTL_COCPU_STATE_SWITCH_V  0x1
#define RTC_CNTL_COCPU_STATE_SWITCH_S  14
/* RTC_CNTL_COCPU_STATE_START :  ;bitpos:[13] ;default: 1'b0 ; */
/*description: ulp/cocpu should start to work.*/
#define RTC_CNTL_COCPU_STATE_START    (BIT(13))
#define RTC_CNTL_COCPU_STATE_START_M  (BIT(13))
#define RTC_CNTL_COCPU_STATE_START_V  0x1
#define RTC_CNTL_COCPU_STATE_START_S  13
/* RTC_CNTL_TOUCH_STATE_DONE :  ;bitpos:[12] ;default: 1'b0 ; */
/*description: touch is done.*/
#define RTC_CNTL_TOUCH_STATE_DONE    (BIT(12))
#define RTC_CNTL_TOUCH_STATE_DONE_M  (BIT(12))
#define RTC_CNTL_TOUCH_STATE_DONE_V  0x1
#define RTC_CNTL_TOUCH_STATE_DONE_S  12
/* RTC_CNTL_TOUCH_STATE_SLP :  ;bitpos:[11] ;default: 1'b0 ; */
/*description: touch is in sleep state.*/
#define RTC_CNTL_TOUCH_STATE_SLP    (BIT(11))
#define RTC_CNTL_TOUCH_STATE_SLP_M  (BIT(11))
#define RTC_CNTL_TOUCH_STATE_SLP_V  0x1
#define RTC_CNTL_TOUCH_STATE_SLP_S  11
/* RTC_CNTL_TOUCH_STATE_SWITCH :  ;bitpos:[10] ;default: 1'b0 ; */
/*description: touch is about to working. Switch rtc main state.*/
#define RTC_CNTL_TOUCH_STATE_SWITCH    (BIT(10))
#define RTC_CNTL_TOUCH_STATE_SWITCH_M  (BIT(10))
#define RTC_CNTL_TOUCH_STATE_SWITCH_V  0x1
#define RTC_CNTL_TOUCH_STATE_SWITCH_S  10
/* RTC_CNTL_TOUCH_STATE_START :  ;bitpos:[9] ;default: 1'b0 ; */
/*description: touch should start to work.*/
#define RTC_CNTL_TOUCH_STATE_START    (BIT(9))
#define RTC_CNTL_TOUCH_STATE_START_M  (BIT(9))
#define RTC_CNTL_TOUCH_STATE_START_V  0x1
#define RTC_CNTL_TOUCH_STATE_START_S  9
/* RTC_CNTL_XPD_DIG :  ;bitpos:[8] ;default: 1'b0 ; */
/*description: digital wrap power down.*/
#define RTC_CNTL_XPD_DIG    (BIT(8))
#define RTC_CNTL_XPD_DIG_M  (BIT(8))
#define RTC_CNTL_XPD_DIG_V  0x1
#define RTC_CNTL_XPD_DIG_S  8

#define RTC_CNTL_DIAG0_REG          (DR_REG_RTCCNTL_BASE + 0xC0)
/* RTC_CNTL_LOW_POWER_DIAG1 :  ;bitpos:[31:0] ;default: 0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_LOW_POWER_DIAG1    0xFFFFFFFF
#define RTC_CNTL_LOW_POWER_DIAG1_M  ((RTC_CNTL_LOW_POWER_DIAG1_V)<<(RTC_CNTL_LOW_POWER_DIAG1_S))
#define RTC_CNTL_LOW_POWER_DIAG1_V  0xFFFFFFFF
#define RTC_CNTL_LOW_POWER_DIAG1_S  0

#define RTC_CNTL_PAD_HOLD_REG          (DR_REG_RTCCNTL_BASE + 0xC4)
/* RTC_CNTL_GPIO_PIN5_HOLD :  ;bitpos:[5] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN5_HOLD    (BIT(5))
#define RTC_CNTL_GPIO_PIN5_HOLD_M  (BIT(5))
#define RTC_CNTL_GPIO_PIN5_HOLD_V  0x1
#define RTC_CNTL_GPIO_PIN5_HOLD_S  5
/* RTC_CNTL_GPIO_PIN4_HOLD :  ;bitpos:[4] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN4_HOLD    (BIT(4))
#define RTC_CNTL_GPIO_PIN4_HOLD_M  (BIT(4))
#define RTC_CNTL_GPIO_PIN4_HOLD_V  0x1
#define RTC_CNTL_GPIO_PIN4_HOLD_S  4
/* RTC_CNTL_GPIO_PIN3_HOLD :  ;bitpos:[3] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN3_HOLD    (BIT(3))
#define RTC_CNTL_GPIO_PIN3_HOLD_M  (BIT(3))
#define RTC_CNTL_GPIO_PIN3_HOLD_V  0x1
#define RTC_CNTL_GPIO_PIN3_HOLD_S  3
/* RTC_CNTL_GPIO_PIN2_HOLD :  ;bitpos:[2] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN2_HOLD    (BIT(2))
#define RTC_CNTL_GPIO_PIN2_HOLD_M  (BIT(2))
#define RTC_CNTL_GPIO_PIN2_HOLD_V  0x1
#define RTC_CNTL_GPIO_PIN2_HOLD_S  2
/* RTC_CNTL_GPIO_PIN1_HOLD :  ;bitpos:[1] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN1_HOLD    (BIT(1))
#define RTC_CNTL_GPIO_PIN1_HOLD_M  (BIT(1))
#define RTC_CNTL_GPIO_PIN1_HOLD_V  0x1
#define RTC_CNTL_GPIO_PIN1_HOLD_S  1
/* RTC_CNTL_GPIO_PIN0_HOLD :  ;bitpos:[0] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN0_HOLD    (BIT(0))
#define RTC_CNTL_GPIO_PIN0_HOLD_M  (BIT(0))
#define RTC_CNTL_GPIO_PIN0_HOLD_V  0x1
#define RTC_CNTL_GPIO_PIN0_HOLD_S  0

#define RTC_CNTL_DIG_PAD_HOLD_REG          (DR_REG_RTCCNTL_BASE + 0xC8)
/* RTC_CNTL_DIG_PAD_HOLD :  ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DIG_PAD_HOLD    0xFFFFFFFF
#define RTC_CNTL_DIG_PAD_HOLD_M  ((RTC_CNTL_DIG_PAD_HOLD_V)<<(RTC_CNTL_DIG_PAD_HOLD_S))
#define RTC_CNTL_DIG_PAD_HOLD_V  0xFFFFFFFF
#define RTC_CNTL_DIG_PAD_HOLD_S  0

#define RTC_CNTL_BROWN_OUT_REG          (DR_REG_RTCCNTL_BASE + 0xCC)
/* RTC_CNTL_BROWN_OUT_DET :  ;bitpos:[31] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_BROWN_OUT_DET    (BIT(31))
#define RTC_CNTL_BROWN_OUT_DET_M  (BIT(31))
#define RTC_CNTL_BROWN_OUT_DET_V  0x1
#define RTC_CNTL_BROWN_OUT_DET_S  31
/* RTC_CNTL_BROWN_OUT_ENA :  ;bitpos:[30] ;default: 1'b1 ; */
/*description: enable brown out.*/
#define RTC_CNTL_BROWN_OUT_ENA    (BIT(30))
#define RTC_CNTL_BROWN_OUT_ENA_M  (BIT(30))
#define RTC_CNTL_BROWN_OUT_ENA_V  0x1
#define RTC_CNTL_BROWN_OUT_ENA_S  30
/* RTC_CNTL_BROWN_OUT_CNT_CLR :  ;bitpos:[29] ;default: 1'b0 ; */
/*description: clear brown out counter.*/
#define RTC_CNTL_BROWN_OUT_CNT_CLR    (BIT(29))
#define RTC_CNTL_BROWN_OUT_CNT_CLR_M  (BIT(29))
#define RTC_CNTL_BROWN_OUT_CNT_CLR_V  0x1
#define RTC_CNTL_BROWN_OUT_CNT_CLR_S  29
/* RTC_CNTL_BROWN_OUT_ANA_RST_EN :  ;bitpos:[28] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_BROWN_OUT_ANA_RST_EN    (BIT(28))
#define RTC_CNTL_BROWN_OUT_ANA_RST_EN_M  (BIT(28))
#define RTC_CNTL_BROWN_OUT_ANA_RST_EN_V  0x1
#define RTC_CNTL_BROWN_OUT_ANA_RST_EN_S  28
/* RTC_CNTL_BROWN_OUT_RST_SEL :  ;bitpos:[27] ;default: 1'b0 ; */
/*description: 1:  4-pos reset.*/
#define RTC_CNTL_BROWN_OUT_RST_SEL    (BIT(27))
#define RTC_CNTL_BROWN_OUT_RST_SEL_M  (BIT(27))
#define RTC_CNTL_BROWN_OUT_RST_SEL_V  0x1
#define RTC_CNTL_BROWN_OUT_RST_SEL_S  27
/* RTC_CNTL_BROWN_OUT_RST_ENA :  ;bitpos:[26] ;default: 1'b0 ; */
/*description: enable brown out reset.*/
#define RTC_CNTL_BROWN_OUT_RST_ENA    (BIT(26))
#define RTC_CNTL_BROWN_OUT_RST_ENA_M  (BIT(26))
#define RTC_CNTL_BROWN_OUT_RST_ENA_V  0x1
#define RTC_CNTL_BROWN_OUT_RST_ENA_S  26
/* RTC_CNTL_BROWN_OUT_RST_WAIT :  ;bitpos:[25:16] ;default: 10'h3ff ; */
/*description: brown out reset wait cycles.*/
#define RTC_CNTL_BROWN_OUT_RST_WAIT    0x000003FF
#define RTC_CNTL_BROWN_OUT_RST_WAIT_M  ((RTC_CNTL_BROWN_OUT_RST_WAIT_V)<<(RTC_CNTL_BROWN_OUT_RST_WAIT_S))
#define RTC_CNTL_BROWN_OUT_RST_WAIT_V  0x3FF
#define RTC_CNTL_BROWN_OUT_RST_WAIT_S  16
/* RTC_CNTL_BROWN_OUT_PD_RF_ENA :  ;bitpos:[15] ;default: 1'b0 ; */
/*description: enable power down RF when brown out happens.*/
#define RTC_CNTL_BROWN_OUT_PD_RF_ENA    (BIT(15))
#define RTC_CNTL_BROWN_OUT_PD_RF_ENA_M  (BIT(15))
#define RTC_CNTL_BROWN_OUT_PD_RF_ENA_V  0x1
#define RTC_CNTL_BROWN_OUT_PD_RF_ENA_S  15
/* RTC_CNTL_BROWN_OUT_CLOSE_FLASH_ENA :  ;bitpos:[14] ;default: 1'b0 ; */
/*description: enable close flash when brown out happens.*/
#define RTC_CNTL_BROWN_OUT_CLOSE_FLASH_ENA    (BIT(14))
#define RTC_CNTL_BROWN_OUT_CLOSE_FLASH_ENA_M  (BIT(14))
#define RTC_CNTL_BROWN_OUT_CLOSE_FLASH_ENA_V  0x1
#define RTC_CNTL_BROWN_OUT_CLOSE_FLASH_ENA_S  14
/* RTC_CNTL_BROWN_OUT_INT_WAIT :  ;bitpos:[13:4] ;default: 10'h1 ; */
/*description: brown out interrupt wait cycles.*/
#define RTC_CNTL_BROWN_OUT_INT_WAIT    0x000003FF
#define RTC_CNTL_BROWN_OUT_INT_WAIT_M  ((RTC_CNTL_BROWN_OUT_INT_WAIT_V)<<(RTC_CNTL_BROWN_OUT_INT_WAIT_S))
#define RTC_CNTL_BROWN_OUT_INT_WAIT_V  0x3FF
#define RTC_CNTL_BROWN_OUT_INT_WAIT_S  4

#define RTC_CNTL_TIME_LOW1_REG          (DR_REG_RTCCNTL_BASE + 0xD0)
/* RTC_CNTL_TIMER_VALUE1_LOW :  ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: RTC timer low 32 bits.*/
#define RTC_CNTL_TIMER_VALUE1_LOW    0xFFFFFFFF
#define RTC_CNTL_TIMER_VALUE1_LOW_M  ((RTC_CNTL_TIMER_VALUE1_LOW_V)<<(RTC_CNTL_TIMER_VALUE1_LOW_S))
#define RTC_CNTL_TIMER_VALUE1_LOW_V  0xFFFFFFFF
#define RTC_CNTL_TIMER_VALUE1_LOW_S  0

#define RTC_CNTL_TIME_HIGH1_REG          (DR_REG_RTCCNTL_BASE + 0xD4)
/* RTC_CNTL_TIMER_VALUE1_HIGH :  ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: RTC timer high 16 bits.*/
#define RTC_CNTL_TIMER_VALUE1_HIGH    0x0000FFFF
#define RTC_CNTL_TIMER_VALUE1_HIGH_M  ((RTC_CNTL_TIMER_VALUE1_HIGH_V)<<(RTC_CNTL_TIMER_VALUE1_HIGH_S))
#define RTC_CNTL_TIMER_VALUE1_HIGH_V  0xFFFF
#define RTC_CNTL_TIMER_VALUE1_HIGH_S  0

#define RTC_CNTL_USB_CONF_REG          (DR_REG_RTCCNTL_BASE + 0xD8)
/* RTC_CNTL_IO_MUX_RESET_DISABLE :  ;bitpos:[18] ;default: 1'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_IO_MUX_RESET_DISABLE    (BIT(18))
#define RTC_CNTL_IO_MUX_RESET_DISABLE_M  (BIT(18))
#define RTC_CNTL_IO_MUX_RESET_DISABLE_V  0x1
#define RTC_CNTL_IO_MUX_RESET_DISABLE_S  18

#define RTC_CNTL_SLP_REJECT_CAUSE_REG          (DR_REG_RTCCNTL_BASE + 0xDC)
/* RTC_CNTL_REJECT_CAUSE :  ;bitpos:[17:0] ;default: 18'd0 ; */
/*description: sleep reject cause.*/
#define RTC_CNTL_REJECT_CAUSE    0x0003FFFF
#define RTC_CNTL_REJECT_CAUSE_M  ((RTC_CNTL_REJECT_CAUSE_V)<<(RTC_CNTL_REJECT_CAUSE_S))
#define RTC_CNTL_REJECT_CAUSE_V  0x3FFFF
#define RTC_CNTL_REJECT_CAUSE_S  0

#define RTC_CNTL_OPTION1_REG          (DR_REG_RTCCNTL_BASE + 0xE0)
/* RTC_CNTL_FORCE_DOWNLOAD_BOOT :  ;bitpos:[0] ;default: 1'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_FORCE_DOWNLOAD_BOOT    (BIT(0))
#define RTC_CNTL_FORCE_DOWNLOAD_BOOT_M  (BIT(0))
#define RTC_CNTL_FORCE_DOWNLOAD_BOOT_V  0x1
#define RTC_CNTL_FORCE_DOWNLOAD_BOOT_S  0

#define RTC_CNTL_SLP_WAKEUP_CAUSE_REG          (DR_REG_RTCCNTL_BASE + 0xE4)
/* RTC_CNTL_WAKEUP_CAUSE :  ;bitpos:[16:0] ;default: 17'd0 ; */
/*description: sleep wakeup cause.*/
#define RTC_CNTL_WAKEUP_CAUSE    0x0001FFFF
#define RTC_CNTL_WAKEUP_CAUSE_M  ((RTC_CNTL_WAKEUP_CAUSE_V)<<(RTC_CNTL_WAKEUP_CAUSE_S))
#define RTC_CNTL_WAKEUP_CAUSE_V  0x1FFFF
#define RTC_CNTL_WAKEUP_CAUSE_S  0

#define RTC_CNTL_ULP_CP_TIMER_1_REG          (DR_REG_RTCCNTL_BASE + 0xE8)
/* RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE :  ;bitpos:[31:8] ;default: 24'd200 ; */
/*description: sleep cycles for ULP-coprocessor timer.*/
#define RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE    0x00FFFFFF
#define RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE_M  ((RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE_V)<<(RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE_S))
#define RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE_V  0xFFFFFF
#define RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE_S  8

#define RTC_CNTL_INT_ENA_RTC_W1TS_REG          (DR_REG_RTCCNTL_BASE + 0xEC)
/* RTC_CNTL_BBPLL_CAL_INT_ENA_W1TS :  ;bitpos:[20] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TS    (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TS_M  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TS_S  20
/* RTC_CNTL_SWD_INT_ENA_W1TS :  ;bitpos:[15] ;default: 1'b0 ; */
/*description: enable super watch dog interrupt.*/
#define RTC_CNTL_SWD_INT_ENA_W1TS    (BIT(15))
#define RTC_CNTL_SWD_INT_ENA_W1TS_M  (BIT(15))
#define RTC_CNTL_SWD_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_SWD_INT_ENA_W1TS_S  15
/* RTC_CNTL_MAIN_TIMER_INT_ENA_W1TS :  ;bitpos:[10] ;default: 1'b0 ; */
/*description: enable RTC main timer interrupt.*/
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TS    (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TS_M  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TS_S  10
/* RTC_CNTL_BROWN_OUT_INT_ENA_W1TS :  ;bitpos:[9] ;default: 1'b0 ; */
/*description: enable brown out interrupt.*/
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TS    (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TS_M  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TS_S  9
/* RTC_CNTL_WDT_INT_ENA_W1TS :  ;bitpos:[3] ;default: 1'b0 ; */
/*description: enable RTC WDT interrupt.*/
#define RTC_CNTL_WDT_INT_ENA_W1TS    (BIT(3))
#define RTC_CNTL_WDT_INT_ENA_W1TS_M  (BIT(3))
#define RTC_CNTL_WDT_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_WDT_INT_ENA_W1TS_S  3
/* RTC_CNTL_SLP_REJECT_INT_ENA_W1TS :  ;bitpos:[1] ;default: 1'b0 ; */
/*description: enable sleep reject interrupt.*/
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TS    (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TS_M  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TS_S  1
/* RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TS :  ;bitpos:[0] ;default: 1'b0 ; */
/*description: enable sleep wakeup interrupt.*/
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TS    (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TS_M  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TS_V  0x1
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TS_S  0

#define RTC_CNTL_INT_ENA_RTC_W1TC_REG          (DR_REG_RTCCNTL_BASE + 0xF0)
/* RTC_CNTL_BBPLL_CAL_INT_ENA_W1TC :  ;bitpos:[20] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TC    (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TC_M  (BIT(20))
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_BBPLL_CAL_INT_ENA_W1TC_S  20
/* RTC_CNTL_SWD_INT_ENA_W1TC :  ;bitpos:[15] ;default: 1'b0 ; */
/*description: enable super watch dog interrupt.*/
#define RTC_CNTL_SWD_INT_ENA_W1TC    (BIT(15))
#define RTC_CNTL_SWD_INT_ENA_W1TC_M  (BIT(15))
#define RTC_CNTL_SWD_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_SWD_INT_ENA_W1TC_S  15
/* RTC_CNTL_MAIN_TIMER_INT_ENA_W1TC :  ;bitpos:[10] ;default: 1'b0 ; */
/*description: enable RTC main timer interrupt.*/
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TC    (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TC_M  (BIT(10))
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_MAIN_TIMER_INT_ENA_W1TC_S  10
/* RTC_CNTL_BROWN_OUT_INT_ENA_W1TC :  ;bitpos:[9] ;default: 1'b0 ; */
/*description: enable brown out interrupt.*/
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TC    (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TC_M  (BIT(9))
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_BROWN_OUT_INT_ENA_W1TC_S  9
/* RTC_CNTL_WDT_INT_ENA_W1TC :  ;bitpos:[3] ;default: 1'b0 ; */
/*description: enable RTC WDT interrupt.*/
#define RTC_CNTL_WDT_INT_ENA_W1TC    (BIT(3))
#define RTC_CNTL_WDT_INT_ENA_W1TC_M  (BIT(3))
#define RTC_CNTL_WDT_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_WDT_INT_ENA_W1TC_S  3
/* RTC_CNTL_SLP_REJECT_INT_ENA_W1TC :  ;bitpos:[1] ;default: 1'b0 ; */
/*description: enable sleep reject interrupt.*/
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TC    (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TC_M  (BIT(1))
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_SLP_REJECT_INT_ENA_W1TC_S  1
/* RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TC :  ;bitpos:[0] ;default: 1'b0 ; */
/*description: enable sleep wakeup interrupt.*/
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TC    (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TC_M  (BIT(0))
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TC_V  0x1
#define RTC_CNTL_SLP_WAKEUP_INT_ENA_W1TC_S  0

#define RTC_CNTL_RETENTION_CTRL_REG          (DR_REG_RTCCNTL_BASE + 0xF4)
/* RTC_CNTL_RETENTION_WAIT :  ;bitpos:[31:27] ;default: 5'd20 ; */
/*description: wait cycles for rention operation.*/
#define RTC_CNTL_RETENTION_WAIT    0x0000001F
#define RTC_CNTL_RETENTION_WAIT_M  ((RTC_CNTL_RETENTION_WAIT_V)<<(RTC_CNTL_RETENTION_WAIT_S))
#define RTC_CNTL_RETENTION_WAIT_V  0x1F
#define RTC_CNTL_RETENTION_WAIT_S  27
/* RTC_CNTL_RETENTION_EN :  ;bitpos:[26] ;default: 1'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_RETENTION_EN    (BIT(26))
#define RTC_CNTL_RETENTION_EN_M  (BIT(26))
#define RTC_CNTL_RETENTION_EN_V  0x1
#define RTC_CNTL_RETENTION_EN_S  26
/* RTC_CNTL_RETENTION_CLKOFF_WAIT :  ;bitpos:[25:22] ;default: 4'd3 ; */
/*description: Need add desc.*/
#define RTC_CNTL_RETENTION_CLKOFF_WAIT    0x0000000F
#define RTC_CNTL_RETENTION_CLKOFF_WAIT_M  ((RTC_CNTL_RETENTION_CLKOFF_WAIT_V)<<(RTC_CNTL_RETENTION_CLKOFF_WAIT_S))
#define RTC_CNTL_RETENTION_CLKOFF_WAIT_V  0xF
#define RTC_CNTL_RETENTION_CLKOFF_WAIT_S  22
/* RTC_CNTL_RETENTION_DONE_WAIT :  ;bitpos:[21:19] ;default: 3'd2 ; */
/*description: Need add desc.*/
#define RTC_CNTL_RETENTION_DONE_WAIT    0x00000007
#define RTC_CNTL_RETENTION_DONE_WAIT_M  ((RTC_CNTL_RETENTION_DONE_WAIT_V)<<(RTC_CNTL_RETENTION_DONE_WAIT_S))
#define RTC_CNTL_RETENTION_DONE_WAIT_V  0x7
#define RTC_CNTL_RETENTION_DONE_WAIT_S  19
/* RTC_CNTL_RETENTION_CLK_SEL :  ;bitpos:[18] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_RETENTION_CLK_SEL    (BIT(18))
#define RTC_CNTL_RETENTION_CLK_SEL_M  (BIT(18))
#define RTC_CNTL_RETENTION_CLK_SEL_V  0x1
#define RTC_CNTL_RETENTION_CLK_SEL_S  18

#define RTC_CNTL_FIB_SEL_REG          (DR_REG_RTCCNTL_BASE + 0xF8)
/* RTC_CNTL_FIB_SEL :  ;bitpos:[2:0] ;default: 3'd7 ; */
/*description: select use analog fib signal.*/
#define RTC_CNTL_FIB_SEL    0x00000007
#define RTC_CNTL_FIB_SEL_M  ((RTC_CNTL_FIB_SEL_V)<<(RTC_CNTL_FIB_SEL_S))
#define RTC_CNTL_FIB_SEL_V  0x7
#define RTC_CNTL_FIB_SEL_S  0

#define RTC_CNTL_FIB_GLITCH_RST BIT(0)
#define RTC_CNTL_FIB_BOD_RST BIT(1)
#define RTC_CNTL_FIB_SUPER_WDT_RST BIT(2)

#define RTC_CNTL_GPIO_WAKEUP_REG          (DR_REG_RTCCNTL_BASE + 0xFC)
/* RTC_CNTL_GPIO_PIN0_WAKEUP_ENABLE :  ;bitpos:[31] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN0_WAKEUP_ENABLE    (BIT(31))
#define RTC_CNTL_GPIO_PIN0_WAKEUP_ENABLE_M  (BIT(31))
#define RTC_CNTL_GPIO_PIN0_WAKEUP_ENABLE_V  0x1
#define RTC_CNTL_GPIO_PIN0_WAKEUP_ENABLE_S  31
/* RTC_CNTL_GPIO_PIN1_WAKEUP_ENABLE :  ;bitpos:[30] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN1_WAKEUP_ENABLE    (BIT(30))
#define RTC_CNTL_GPIO_PIN1_WAKEUP_ENABLE_M  (BIT(30))
#define RTC_CNTL_GPIO_PIN1_WAKEUP_ENABLE_V  0x1
#define RTC_CNTL_GPIO_PIN1_WAKEUP_ENABLE_S  30
/* RTC_CNTL_GPIO_PIN2_WAKEUP_ENABLE :  ;bitpos:[29] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN2_WAKEUP_ENABLE    (BIT(29))
#define RTC_CNTL_GPIO_PIN2_WAKEUP_ENABLE_M  (BIT(29))
#define RTC_CNTL_GPIO_PIN2_WAKEUP_ENABLE_V  0x1
#define RTC_CNTL_GPIO_PIN2_WAKEUP_ENABLE_S  29
/* RTC_CNTL_GPIO_PIN3_WAKEUP_ENABLE :  ;bitpos:[28] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN3_WAKEUP_ENABLE    (BIT(28))
#define RTC_CNTL_GPIO_PIN3_WAKEUP_ENABLE_M  (BIT(28))
#define RTC_CNTL_GPIO_PIN3_WAKEUP_ENABLE_V  0x1
#define RTC_CNTL_GPIO_PIN3_WAKEUP_ENABLE_S  28
/* RTC_CNTL_GPIO_PIN4_WAKEUP_ENABLE :  ;bitpos:[27] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN4_WAKEUP_ENABLE    (BIT(27))
#define RTC_CNTL_GPIO_PIN4_WAKEUP_ENABLE_M  (BIT(27))
#define RTC_CNTL_GPIO_PIN4_WAKEUP_ENABLE_V  0x1
#define RTC_CNTL_GPIO_PIN4_WAKEUP_ENABLE_S  27
/* RTC_CNTL_GPIO_PIN5_WAKEUP_ENABLE :  ;bitpos:[26] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN5_WAKEUP_ENABLE    (BIT(26))
#define RTC_CNTL_GPIO_PIN5_WAKEUP_ENABLE_M  (BIT(26))
#define RTC_CNTL_GPIO_PIN5_WAKEUP_ENABLE_V  0x1
#define RTC_CNTL_GPIO_PIN5_WAKEUP_ENABLE_S  26
/* RTC_CNTL_GPIO_PIN0_INT_TYPE :  ;bitpos:[25:23] ;default: 3'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN0_INT_TYPE    0x00000007
#define RTC_CNTL_GPIO_PIN0_INT_TYPE_M  ((RTC_CNTL_GPIO_PIN0_INT_TYPE_V)<<(RTC_CNTL_GPIO_PIN0_INT_TYPE_S))
#define RTC_CNTL_GPIO_PIN0_INT_TYPE_V  0x7
#define RTC_CNTL_GPIO_PIN0_INT_TYPE_S  23
/* RTC_CNTL_GPIO_PIN1_INT_TYPE :  ;bitpos:[22:20] ;default: 3'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN1_INT_TYPE    0x00000007
#define RTC_CNTL_GPIO_PIN1_INT_TYPE_M  ((RTC_CNTL_GPIO_PIN1_INT_TYPE_V)<<(RTC_CNTL_GPIO_PIN1_INT_TYPE_S))
#define RTC_CNTL_GPIO_PIN1_INT_TYPE_V  0x7
#define RTC_CNTL_GPIO_PIN1_INT_TYPE_S  20
/* RTC_CNTL_GPIO_PIN2_INT_TYPE :  ;bitpos:[19:17] ;default: 3'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN2_INT_TYPE    0x00000007
#define RTC_CNTL_GPIO_PIN2_INT_TYPE_M  ((RTC_CNTL_GPIO_PIN2_INT_TYPE_V)<<(RTC_CNTL_GPIO_PIN2_INT_TYPE_S))
#define RTC_CNTL_GPIO_PIN2_INT_TYPE_V  0x7
#define RTC_CNTL_GPIO_PIN2_INT_TYPE_S  17
/* RTC_CNTL_GPIO_PIN3_INT_TYPE :  ;bitpos:[16:14] ;default: 3'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN3_INT_TYPE    0x00000007
#define RTC_CNTL_GPIO_PIN3_INT_TYPE_M  ((RTC_CNTL_GPIO_PIN3_INT_TYPE_V)<<(RTC_CNTL_GPIO_PIN3_INT_TYPE_S))
#define RTC_CNTL_GPIO_PIN3_INT_TYPE_V  0x7
#define RTC_CNTL_GPIO_PIN3_INT_TYPE_S  14
/* RTC_CNTL_GPIO_PIN4_INT_TYPE :  ;bitpos:[13:11] ;default: 3'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN4_INT_TYPE    0x00000007
#define RTC_CNTL_GPIO_PIN4_INT_TYPE_M  ((RTC_CNTL_GPIO_PIN4_INT_TYPE_V)<<(RTC_CNTL_GPIO_PIN4_INT_TYPE_S))
#define RTC_CNTL_GPIO_PIN4_INT_TYPE_V  0x7
#define RTC_CNTL_GPIO_PIN4_INT_TYPE_S  11
/* RTC_CNTL_GPIO_PIN5_INT_TYPE :  ;bitpos:[10:8] ;default: 3'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN5_INT_TYPE    0x00000007
#define RTC_CNTL_GPIO_PIN5_INT_TYPE_M  ((RTC_CNTL_GPIO_PIN5_INT_TYPE_V)<<(RTC_CNTL_GPIO_PIN5_INT_TYPE_S))
#define RTC_CNTL_GPIO_PIN5_INT_TYPE_V  0x7
#define RTC_CNTL_GPIO_PIN5_INT_TYPE_S  8
/* RTC_CNTL_GPIO_PIN_CLK_GATE :  ;bitpos:[7] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN_CLK_GATE    (BIT(7))
#define RTC_CNTL_GPIO_PIN_CLK_GATE_M  (BIT(7))
#define RTC_CNTL_GPIO_PIN_CLK_GATE_V  0x1
#define RTC_CNTL_GPIO_PIN_CLK_GATE_S  7
/* RTC_CNTL_GPIO_WAKEUP_STATUS_CLR :  ;bitpos:[6] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_WAKEUP_STATUS_CLR    (BIT(6))
#define RTC_CNTL_GPIO_WAKEUP_STATUS_CLR_M  (BIT(6))
#define RTC_CNTL_GPIO_WAKEUP_STATUS_CLR_V  0x1
#define RTC_CNTL_GPIO_WAKEUP_STATUS_CLR_S  6
/* RTC_CNTL_GPIO_WAKEUP_STATUS :  ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_WAKEUP_STATUS    0x0000003F
#define RTC_CNTL_GPIO_WAKEUP_STATUS_M  ((RTC_CNTL_GPIO_WAKEUP_STATUS_V)<<(RTC_CNTL_GPIO_WAKEUP_STATUS_S))
#define RTC_CNTL_GPIO_WAKEUP_STATUS_V  0x3F
#define RTC_CNTL_GPIO_WAKEUP_STATUS_S  0

#define RTC_CNTL_DBG_SEL_REG          (DR_REG_RTCCNTL_BASE + 0x100)
/* RTC_CNTL_DEBUG_SEL4 :  ;bitpos:[31:27] ;default: 5'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DEBUG_SEL4    0x0000001F
#define RTC_CNTL_DEBUG_SEL4_M  ((RTC_CNTL_DEBUG_SEL4_V)<<(RTC_CNTL_DEBUG_SEL4_S))
#define RTC_CNTL_DEBUG_SEL4_V  0x1F
#define RTC_CNTL_DEBUG_SEL4_S  27
/* RTC_CNTL_DEBUG_SEL3 :  ;bitpos:[26:22] ;default: 5'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DEBUG_SEL3    0x0000001F
#define RTC_CNTL_DEBUG_SEL3_M  ((RTC_CNTL_DEBUG_SEL3_V)<<(RTC_CNTL_DEBUG_SEL3_S))
#define RTC_CNTL_DEBUG_SEL3_V  0x1F
#define RTC_CNTL_DEBUG_SEL3_S  22
/* RTC_CNTL_DEBUG_SEL2 :  ;bitpos:[21:17] ;default: 5'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DEBUG_SEL2    0x0000001F
#define RTC_CNTL_DEBUG_SEL2_M  ((RTC_CNTL_DEBUG_SEL2_V)<<(RTC_CNTL_DEBUG_SEL2_S))
#define RTC_CNTL_DEBUG_SEL2_V  0x1F
#define RTC_CNTL_DEBUG_SEL2_S  17
/* RTC_CNTL_DEBUG_SEL1 :  ;bitpos:[16:12] ;default: 5'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DEBUG_SEL1    0x0000001F
#define RTC_CNTL_DEBUG_SEL1_M  ((RTC_CNTL_DEBUG_SEL1_V)<<(RTC_CNTL_DEBUG_SEL1_S))
#define RTC_CNTL_DEBUG_SEL1_V  0x1F
#define RTC_CNTL_DEBUG_SEL1_S  12
/* RTC_CNTL_DEBUG_SEL0 :  ;bitpos:[11:7] ;default: 5'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DEBUG_SEL0    0x0000001F
#define RTC_CNTL_DEBUG_SEL0_M  ((RTC_CNTL_DEBUG_SEL0_V)<<(RTC_CNTL_DEBUG_SEL0_S))
#define RTC_CNTL_DEBUG_SEL0_V  0x1F
#define RTC_CNTL_DEBUG_SEL0_S  7
/* RTC_CNTL_DEBUG_BIT_SEL :  ;bitpos:[6:2] ;default: 5'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DEBUG_BIT_SEL    0x0000001F
#define RTC_CNTL_DEBUG_BIT_SEL_M  ((RTC_CNTL_DEBUG_BIT_SEL_V)<<(RTC_CNTL_DEBUG_BIT_SEL_S))
#define RTC_CNTL_DEBUG_BIT_SEL_V  0x1F
#define RTC_CNTL_DEBUG_BIT_SEL_S  2
/* RTC_CNTL_DEBUG_12M_NO_GATING :  ;bitpos:[1] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DEBUG_12M_NO_GATING    (BIT(1))
#define RTC_CNTL_DEBUG_12M_NO_GATING_M  (BIT(1))
#define RTC_CNTL_DEBUG_12M_NO_GATING_V  0x1
#define RTC_CNTL_DEBUG_12M_NO_GATING_S  1

#define RTC_CNTL_DBG_MAP_REG          (DR_REG_RTCCNTL_BASE + 0x104)
/* RTC_CNTL_GPIO_PIN0_FUN_SEL :  ;bitpos:[31:28] ;default: 4'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN0_FUN_SEL    0x0000000F
#define RTC_CNTL_GPIO_PIN0_FUN_SEL_M  ((RTC_CNTL_GPIO_PIN0_FUN_SEL_V)<<(RTC_CNTL_GPIO_PIN0_FUN_SEL_S))
#define RTC_CNTL_GPIO_PIN0_FUN_SEL_V  0xF
#define RTC_CNTL_GPIO_PIN0_FUN_SEL_S  28
/* RTC_CNTL_GPIO_PIN1_FUN_SEL :  ;bitpos:[27:24] ;default: 4'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN1_FUN_SEL    0x0000000F
#define RTC_CNTL_GPIO_PIN1_FUN_SEL_M  ((RTC_CNTL_GPIO_PIN1_FUN_SEL_V)<<(RTC_CNTL_GPIO_PIN1_FUN_SEL_S))
#define RTC_CNTL_GPIO_PIN1_FUN_SEL_V  0xF
#define RTC_CNTL_GPIO_PIN1_FUN_SEL_S  24
/* RTC_CNTL_GPIO_PIN2_FUN_SEL :  ;bitpos:[23:20] ;default: 4'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN2_FUN_SEL    0x0000000F
#define RTC_CNTL_GPIO_PIN2_FUN_SEL_M  ((RTC_CNTL_GPIO_PIN2_FUN_SEL_V)<<(RTC_CNTL_GPIO_PIN2_FUN_SEL_S))
#define RTC_CNTL_GPIO_PIN2_FUN_SEL_V  0xF
#define RTC_CNTL_GPIO_PIN2_FUN_SEL_S  20
/* RTC_CNTL_GPIO_PIN3_FUN_SEL :  ;bitpos:[19:16] ;default: 4'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN3_FUN_SEL    0x0000000F
#define RTC_CNTL_GPIO_PIN3_FUN_SEL_M  ((RTC_CNTL_GPIO_PIN3_FUN_SEL_V)<<(RTC_CNTL_GPIO_PIN3_FUN_SEL_S))
#define RTC_CNTL_GPIO_PIN3_FUN_SEL_V  0xF
#define RTC_CNTL_GPIO_PIN3_FUN_SEL_S  16
/* RTC_CNTL_GPIO_PIN4_FUN_SEL :  ;bitpos:[15:12] ;default: 4'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN4_FUN_SEL    0x0000000F
#define RTC_CNTL_GPIO_PIN4_FUN_SEL_M  ((RTC_CNTL_GPIO_PIN4_FUN_SEL_V)<<(RTC_CNTL_GPIO_PIN4_FUN_SEL_S))
#define RTC_CNTL_GPIO_PIN4_FUN_SEL_V  0xF
#define RTC_CNTL_GPIO_PIN4_FUN_SEL_S  12
/* RTC_CNTL_GPIO_PIN5_FUN_SEL :  ;bitpos:[11:8] ;default: 4'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN5_FUN_SEL    0x0000000F
#define RTC_CNTL_GPIO_PIN5_FUN_SEL_M  ((RTC_CNTL_GPIO_PIN5_FUN_SEL_V)<<(RTC_CNTL_GPIO_PIN5_FUN_SEL_S))
#define RTC_CNTL_GPIO_PIN5_FUN_SEL_V  0xF
#define RTC_CNTL_GPIO_PIN5_FUN_SEL_S  8
/* RTC_CNTL_GPIO_PIN0_MUX_SEL :  ;bitpos:[7] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN0_MUX_SEL    (BIT(7))
#define RTC_CNTL_GPIO_PIN0_MUX_SEL_M  (BIT(7))
#define RTC_CNTL_GPIO_PIN0_MUX_SEL_V  0x1
#define RTC_CNTL_GPIO_PIN0_MUX_SEL_S  7
/* RTC_CNTL_GPIO_PIN1_MUX_SEL :  ;bitpos:[6] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN1_MUX_SEL    (BIT(6))
#define RTC_CNTL_GPIO_PIN1_MUX_SEL_M  (BIT(6))
#define RTC_CNTL_GPIO_PIN1_MUX_SEL_V  0x1
#define RTC_CNTL_GPIO_PIN1_MUX_SEL_S  6
/* RTC_CNTL_GPIO_PIN2_MUX_SEL :  ;bitpos:[5] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN2_MUX_SEL    (BIT(5))
#define RTC_CNTL_GPIO_PIN2_MUX_SEL_M  (BIT(5))
#define RTC_CNTL_GPIO_PIN2_MUX_SEL_V  0x1
#define RTC_CNTL_GPIO_PIN2_MUX_SEL_S  5
/* RTC_CNTL_GPIO_PIN3_MUX_SEL :  ;bitpos:[4] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN3_MUX_SEL    (BIT(4))
#define RTC_CNTL_GPIO_PIN3_MUX_SEL_M  (BIT(4))
#define RTC_CNTL_GPIO_PIN3_MUX_SEL_V  0x1
#define RTC_CNTL_GPIO_PIN3_MUX_SEL_S  4
/* RTC_CNTL_GPIO_PIN4_MUX_SEL :  ;bitpos:[3] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN4_MUX_SEL    (BIT(3))
#define RTC_CNTL_GPIO_PIN4_MUX_SEL_M  (BIT(3))
#define RTC_CNTL_GPIO_PIN4_MUX_SEL_V  0x1
#define RTC_CNTL_GPIO_PIN4_MUX_SEL_S  3
/* RTC_CNTL_GPIO_PIN5_MUX_SEL :  ;bitpos:[2] ;default: 1'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_GPIO_PIN5_MUX_SEL    (BIT(2))
#define RTC_CNTL_GPIO_PIN5_MUX_SEL_M  (BIT(2))
#define RTC_CNTL_GPIO_PIN5_MUX_SEL_V  0x1
#define RTC_CNTL_GPIO_PIN5_MUX_SEL_S  2

#define RTC_CNTL_SENSOR_CTRL_REG          (DR_REG_RTCCNTL_BASE + 0x108)
/* RTC_CNTL_FORCE_XPD_SAR :  ;bitpos:[31:30] ;default: 2'b0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_FORCE_XPD_SAR    0x00000003
#define RTC_CNTL_FORCE_XPD_SAR_M  ((RTC_CNTL_FORCE_XPD_SAR_V)<<(RTC_CNTL_FORCE_XPD_SAR_S))
#define RTC_CNTL_FORCE_XPD_SAR_V  0x3
#define RTC_CNTL_FORCE_XPD_SAR_S  30
/* RTC_CNTL_SAR2_PWDET_CCT :  ;bitpos:[29:27] ;default: 3'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_SAR2_PWDET_CCT    0x00000007
#define RTC_CNTL_SAR2_PWDET_CCT_M  ((RTC_CNTL_SAR2_PWDET_CCT_V)<<(RTC_CNTL_SAR2_PWDET_CCT_S))
#define RTC_CNTL_SAR2_PWDET_CCT_V  0x7
#define RTC_CNTL_SAR2_PWDET_CCT_S  27

#define RTC_CNTL_DBG_SAR_SEL_REG          (DR_REG_RTCCNTL_BASE + 0x10C)
/* RTC_CNTL_SAR_DEBUG_SEL :  ;bitpos:[31:27] ;default: 5'd0 ; */
/*description: Need add desc.*/
#define RTC_CNTL_SAR_DEBUG_SEL    0x0000001F
#define RTC_CNTL_SAR_DEBUG_SEL_M  ((RTC_CNTL_SAR_DEBUG_SEL_V)<<(RTC_CNTL_SAR_DEBUG_SEL_S))
#define RTC_CNTL_SAR_DEBUG_SEL_V  0x1F
#define RTC_CNTL_SAR_DEBUG_SEL_S  27

#define RTC_CNTL_DATE_REG          (DR_REG_RTCCNTL_BASE + 0x1FC)
/* RTC_CNTL_DATE :  ;bitpos:[27:0] ;default: 28'h2107190 ; */
/*description: Need add desc.*/
#define RTC_CNTL_DATE    0x0FFFFFFF
#define RTC_CNTL_DATE_M  ((RTC_CNTL_DATE_V)<<(RTC_CNTL_DATE_S))
#define RTC_CNTL_DATE_V  0xFFFFFFF
#define RTC_CNTL_DATE_S  0


#ifdef __cplusplus
}
#endif



#endif /*_SOC_RTC_CNTL_REG_H_ */
