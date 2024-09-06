/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_SENS_REG_H_
#define _SOC_SENS_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc/soc.h"
#define SENS_SAR_READER1_CTRL_REG          (DR_REG_SENS_BASE + 0x0000)
/* SENS_SAR1_INT_EN : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: enable saradc1 to send out interrupt*/
#define SENS_SAR1_INT_EN  (BIT(29))
#define SENS_SAR1_INT_EN_M  (BIT(29))
#define SENS_SAR1_INT_EN_V  0x1
#define SENS_SAR1_INT_EN_S  29
/* SENS_SAR1_DATA_INV : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: Invert SAR ADC1 data*/
#define SENS_SAR1_DATA_INV  (BIT(28))
#define SENS_SAR1_DATA_INV_M  (BIT(28))
#define SENS_SAR1_DATA_INV_V  0x1
#define SENS_SAR1_DATA_INV_S  28
/* SENS_SAR1_SAMPLE_NUM : R/W ;bitpos:[26:19] ;default: 8'd0 ; */
/*description: */
#define SENS_SAR1_SAMPLE_NUM  0x000000FF
#define SENS_SAR1_SAMPLE_NUM_M  ((SENS_SAR1_SAMPLE_NUM_V)<<(SENS_SAR1_SAMPLE_NUM_S))
#define SENS_SAR1_SAMPLE_NUM_V  0xFF
#define SENS_SAR1_SAMPLE_NUM_S  19
/* SENS_SAR1_CLK_GATED : R/W ;bitpos:[18] ;default: 1'b1 ; */
/*description: */
#define SENS_SAR1_CLK_GATED  (BIT(18))
#define SENS_SAR1_CLK_GATED_M  (BIT(18))
#define SENS_SAR1_CLK_GATED_V  0x1
#define SENS_SAR1_CLK_GATED_S  18
/* SENS_SAR1_CLK_DIV : R/W ;bitpos:[7:0] ;default: 8'd2 ; */
/*description: clock divider*/
#define SENS_SAR1_CLK_DIV  0x000000FF
#define SENS_SAR1_CLK_DIV_M  ((SENS_SAR1_CLK_DIV_V)<<(SENS_SAR1_CLK_DIV_S))
#define SENS_SAR1_CLK_DIV_V  0xFF
#define SENS_SAR1_CLK_DIV_S  0

#define SENS_SAR_READER1_STATUS_REG          (DR_REG_SENS_BASE + 0x0004)
/* SENS_SAR1_READER_STATUS : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SENS_SAR1_READER_STATUS  0xFFFFFFFF
#define SENS_SAR1_READER_STATUS_M  ((SENS_SAR1_READER_STATUS_V)<<(SENS_SAR1_READER_STATUS_S))
#define SENS_SAR1_READER_STATUS_V  0xFFFFFFFF
#define SENS_SAR1_READER_STATUS_S  0

#define SENS_SAR_MEAS1_CTRL1_REG          (DR_REG_SENS_BASE + 0x0008)
/* SENS_AMP_SHORT_REF_GND_FORCE : R/W ;bitpos:[31:30] ;default: 2'b0 ; */
/*description: */
#define SENS_AMP_SHORT_REF_GND_FORCE  0x00000003
#define SENS_AMP_SHORT_REF_GND_FORCE_M  ((SENS_AMP_SHORT_REF_GND_FORCE_V)<<(SENS_AMP_SHORT_REF_GND_FORCE_S))
#define SENS_AMP_SHORT_REF_GND_FORCE_V  0x3
#define SENS_AMP_SHORT_REF_GND_FORCE_S  30
/* SENS_AMP_SHORT_REF_FORCE : R/W ;bitpos:[29:28] ;default: 2'b0 ; */
/*description: */
#define SENS_AMP_SHORT_REF_FORCE  0x00000003
#define SENS_AMP_SHORT_REF_FORCE_M  ((SENS_AMP_SHORT_REF_FORCE_V)<<(SENS_AMP_SHORT_REF_FORCE_S))
#define SENS_AMP_SHORT_REF_FORCE_V  0x3
#define SENS_AMP_SHORT_REF_FORCE_S  28
/* SENS_AMP_RST_FB_FORCE : R/W ;bitpos:[27:26] ;default: 2'b0 ; */
/*description: */
#define SENS_AMP_RST_FB_FORCE  0x00000003
#define SENS_AMP_RST_FB_FORCE_M  ((SENS_AMP_RST_FB_FORCE_V)<<(SENS_AMP_RST_FB_FORCE_S))
#define SENS_AMP_RST_FB_FORCE_V  0x3
#define SENS_AMP_RST_FB_FORCE_S  26
/* SENS_FORCE_XPD_AMP : R/W ;bitpos:[25:24] ;default: 2'd0 ; */
/*description: */
#define SENS_FORCE_XPD_AMP  0x00000003
#define SENS_FORCE_XPD_AMP_M  ((SENS_FORCE_XPD_AMP_V)<<(SENS_FORCE_XPD_AMP_S))
#define SENS_FORCE_XPD_AMP_V  0x3
#define SENS_FORCE_XPD_AMP_S  24
#define SENS_FORCE_XPD_AMP_FSM 0 // Use FSM to control power down
#define SENS_FORCE_XPD_AMP_PD  2 // Force power down
#define SENS_FORCE_XPD_AMP_PU  3 // Force power up
/* SENS_RTC_CLKGATE_EN : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define SENS_RTC_CLKGATE_EN  (BIT(23))
#define SENS_RTC_CLKGATE_EN_M  (BIT(23))
#define SENS_RTC_CLKGATE_EN_V  0x1
#define SENS_RTC_CLKGATE_EN_S  23
/* SENS_RTC_RESET : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define SENS_RTC_RESET  (BIT(22))
#define SENS_RTC_RESET_M  (BIT(22))
#define SENS_RTC_RESET_V  0x1
#define SENS_RTC_RESET_S  22

#define SENS_SAR_MEAS1_CTRL2_REG          (DR_REG_SENS_BASE + 0x000c)
/* SENS_SAR1_EN_PAD_FORCE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: 1: SAR ADC1 pad enable bitmap is controlled by SW*/
#define SENS_SAR1_EN_PAD_FORCE  (BIT(31))
#define SENS_SAR1_EN_PAD_FORCE_M  (BIT(31))
#define SENS_SAR1_EN_PAD_FORCE_V  0x1
#define SENS_SAR1_EN_PAD_FORCE_S  31
/* SENS_SAR1_EN_PAD : R/W ;bitpos:[30:19] ;default: 12'b0 ; */
/*description: SAR ADC1 pad enable bitmap*/
#define SENS_SAR1_EN_PAD  0x00000FFF
#define SENS_SAR1_EN_PAD_M  ((SENS_SAR1_EN_PAD_V)<<(SENS_SAR1_EN_PAD_S))
#define SENS_SAR1_EN_PAD_V  0xFFF
#define SENS_SAR1_EN_PAD_S  19
/* SENS_MEAS1_START_FORCE : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: 1: SAR ADC1 controller (in RTC) is started by SW*/
#define SENS_MEAS1_START_FORCE  (BIT(18))
#define SENS_MEAS1_START_FORCE_M  (BIT(18))
#define SENS_MEAS1_START_FORCE_V  0x1
#define SENS_MEAS1_START_FORCE_S  18
/* SENS_MEAS1_START_SAR : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: SAR ADC1 controller (in RTC) starts conversion*/
#define SENS_MEAS1_START_SAR  (BIT(17))
#define SENS_MEAS1_START_SAR_M  (BIT(17))
#define SENS_MEAS1_START_SAR_V  0x1
#define SENS_MEAS1_START_SAR_S  17
/* SENS_MEAS1_DONE_SAR : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: SAR ADC1 conversion done indication*/
#define SENS_MEAS1_DONE_SAR  (BIT(16))
#define SENS_MEAS1_DONE_SAR_M  (BIT(16))
#define SENS_MEAS1_DONE_SAR_V  0x1
#define SENS_MEAS1_DONE_SAR_S  16
/* SENS_MEAS1_DATA_SAR : RO ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: SAR ADC1 data*/
#define SENS_MEAS1_DATA_SAR  0x0000FFFF
#define SENS_MEAS1_DATA_SAR_M  ((SENS_MEAS1_DATA_SAR_V)<<(SENS_MEAS1_DATA_SAR_S))
#define SENS_MEAS1_DATA_SAR_V  0xFFFF
#define SENS_MEAS1_DATA_SAR_S  0

#define SENS_SAR_MEAS1_MUX_REG          (DR_REG_SENS_BASE + 0x0010)
/* SENS_SAR1_DIG_FORCE : R/W ;bitpos:[31] ;default: 1'd0 ; */
/*description: 1: SAR ADC1 controlled by DIG ADC1 CTRL*/
#define SENS_SAR1_DIG_FORCE  (BIT(31))
#define SENS_SAR1_DIG_FORCE_M  (BIT(31))
#define SENS_SAR1_DIG_FORCE_V  0x1
#define SENS_SAR1_DIG_FORCE_S  31

#define SENS_SAR_ATTEN1_REG          (DR_REG_SENS_BASE + 0x0014)
/* SENS_SAR1_ATTEN : R/W ;bitpos:[31:0] ;default: 32'hffffffff ; */
/*description: 2-bit attenuation for each pad*/
#define SENS_SAR1_ATTEN  0xFFFFFFFF
#define SENS_SAR1_ATTEN_M  ((SENS_SAR1_ATTEN_V)<<(SENS_SAR1_ATTEN_S))
#define SENS_SAR1_ATTEN_V  0xFFFFFFFF
#define SENS_SAR1_ATTEN_S  0

#define SENS_SAR_AMP_CTRL1_REG          (DR_REG_SENS_BASE + 0x0018)
/* SENS_SAR_AMP_WAIT2 : R/W ;bitpos:[31:16] ;default: 16'd10 ; */
/*description: */
#define SENS_SAR_AMP_WAIT2  0x0000FFFF
#define SENS_SAR_AMP_WAIT2_M  ((SENS_SAR_AMP_WAIT2_V)<<(SENS_SAR_AMP_WAIT2_S))
#define SENS_SAR_AMP_WAIT2_V  0xFFFF
#define SENS_SAR_AMP_WAIT2_S  16
/* SENS_SAR_AMP_WAIT1 : R/W ;bitpos:[15:0] ;default: 16'd10 ; */
/*description: */
#define SENS_SAR_AMP_WAIT1  0x0000FFFF
#define SENS_SAR_AMP_WAIT1_M  ((SENS_SAR_AMP_WAIT1_V)<<(SENS_SAR_AMP_WAIT1_S))
#define SENS_SAR_AMP_WAIT1_V  0xFFFF
#define SENS_SAR_AMP_WAIT1_S  0

#define SENS_SAR_AMP_CTRL2_REG          (DR_REG_SENS_BASE + 0x001c)
/* SENS_SAR_AMP_WAIT3 : R/W ;bitpos:[31:16] ;default: 16'd10 ; */
/*description: */
#define SENS_SAR_AMP_WAIT3  0x0000FFFF
#define SENS_SAR_AMP_WAIT3_M  ((SENS_SAR_AMP_WAIT3_V)<<(SENS_SAR_AMP_WAIT3_S))
#define SENS_SAR_AMP_WAIT3_V  0xFFFF
#define SENS_SAR_AMP_WAIT3_S  16
/* SENS_SAR_RSTB_FSM_IDLE : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SENS_SAR_RSTB_FSM_IDLE  (BIT(6))
#define SENS_SAR_RSTB_FSM_IDLE_M  (BIT(6))
#define SENS_SAR_RSTB_FSM_IDLE_V  0x1
#define SENS_SAR_RSTB_FSM_IDLE_S  6
/* SENS_XPD_SAR_FSM_IDLE : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SENS_XPD_SAR_FSM_IDLE  (BIT(5))
#define SENS_XPD_SAR_FSM_IDLE_M  (BIT(5))
#define SENS_XPD_SAR_FSM_IDLE_V  0x1
#define SENS_XPD_SAR_FSM_IDLE_S  5
/* SENS_AMP_SHORT_REF_GND_FSM_IDLE : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SENS_AMP_SHORT_REF_GND_FSM_IDLE  (BIT(4))
#define SENS_AMP_SHORT_REF_GND_FSM_IDLE_M  (BIT(4))
#define SENS_AMP_SHORT_REF_GND_FSM_IDLE_V  0x1
#define SENS_AMP_SHORT_REF_GND_FSM_IDLE_S  4
/* SENS_AMP_SHORT_REF_FSM_IDLE : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SENS_AMP_SHORT_REF_FSM_IDLE  (BIT(3))
#define SENS_AMP_SHORT_REF_FSM_IDLE_M  (BIT(3))
#define SENS_AMP_SHORT_REF_FSM_IDLE_V  0x1
#define SENS_AMP_SHORT_REF_FSM_IDLE_S  3
/* SENS_AMP_RST_FB_FSM_IDLE : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SENS_AMP_RST_FB_FSM_IDLE  (BIT(2))
#define SENS_AMP_RST_FB_FSM_IDLE_M  (BIT(2))
#define SENS_AMP_RST_FB_FSM_IDLE_V  0x1
#define SENS_AMP_RST_FB_FSM_IDLE_S  2
/* SENS_XPD_SAR_AMP_FSM_IDLE : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SENS_XPD_SAR_AMP_FSM_IDLE  (BIT(1))
#define SENS_XPD_SAR_AMP_FSM_IDLE_M  (BIT(1))
#define SENS_XPD_SAR_AMP_FSM_IDLE_V  0x1
#define SENS_XPD_SAR_AMP_FSM_IDLE_S  1
/* SENS_SAR1_DAC_XPD_FSM_IDLE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SENS_SAR1_DAC_XPD_FSM_IDLE  (BIT(0))
#define SENS_SAR1_DAC_XPD_FSM_IDLE_M  (BIT(0))
#define SENS_SAR1_DAC_XPD_FSM_IDLE_V  0x1
#define SENS_SAR1_DAC_XPD_FSM_IDLE_S  0

#define SENS_SAR_AMP_CTRL3_REG          (DR_REG_SENS_BASE + 0x0020)
/* SENS_SAR_RSTB_FSM : R/W ;bitpos:[27:24] ;default: 4'b0000 ; */
/*description: */
#define SENS_SAR_RSTB_FSM  0x0000000F
#define SENS_SAR_RSTB_FSM_M  ((SENS_SAR_RSTB_FSM_V)<<(SENS_SAR_RSTB_FSM_S))
#define SENS_SAR_RSTB_FSM_V  0xF
#define SENS_SAR_RSTB_FSM_S  24
/* SENS_XPD_SAR_FSM : R/W ;bitpos:[23:20] ;default: 4'b0111 ; */
/*description: */
#define SENS_XPD_SAR_FSM  0x0000000F
#define SENS_XPD_SAR_FSM_M  ((SENS_XPD_SAR_FSM_V)<<(SENS_XPD_SAR_FSM_S))
#define SENS_XPD_SAR_FSM_V  0xF
#define SENS_XPD_SAR_FSM_S  20
/* SENS_AMP_SHORT_REF_GND_FSM : R/W ;bitpos:[19:16] ;default: 4'b0011 ; */
/*description: */
#define SENS_AMP_SHORT_REF_GND_FSM  0x0000000F
#define SENS_AMP_SHORT_REF_GND_FSM_M  ((SENS_AMP_SHORT_REF_GND_FSM_V)<<(SENS_AMP_SHORT_REF_GND_FSM_S))
#define SENS_AMP_SHORT_REF_GND_FSM_V  0xF
#define SENS_AMP_SHORT_REF_GND_FSM_S  16
/* SENS_AMP_SHORT_REF_FSM : R/W ;bitpos:[15:12] ;default: 4'b0011 ; */
/*description: */
#define SENS_AMP_SHORT_REF_FSM  0x0000000F
#define SENS_AMP_SHORT_REF_FSM_M  ((SENS_AMP_SHORT_REF_FSM_V)<<(SENS_AMP_SHORT_REF_FSM_S))
#define SENS_AMP_SHORT_REF_FSM_V  0xF
#define SENS_AMP_SHORT_REF_FSM_S  12
/* SENS_AMP_RST_FB_FSM : R/W ;bitpos:[11:8] ;default: 4'b1000 ; */
/*description: */
#define SENS_AMP_RST_FB_FSM  0x0000000F
#define SENS_AMP_RST_FB_FSM_M  ((SENS_AMP_RST_FB_FSM_V)<<(SENS_AMP_RST_FB_FSM_S))
#define SENS_AMP_RST_FB_FSM_V  0xF
#define SENS_AMP_RST_FB_FSM_S  8
/* SENS_XPD_SAR_AMP_FSM : R/W ;bitpos:[7:4] ;default: 4'b1111 ; */
/*description: */
#define SENS_XPD_SAR_AMP_FSM  0x0000000F
#define SENS_XPD_SAR_AMP_FSM_M  ((SENS_XPD_SAR_AMP_FSM_V)<<(SENS_XPD_SAR_AMP_FSM_S))
#define SENS_XPD_SAR_AMP_FSM_V  0xF
#define SENS_XPD_SAR_AMP_FSM_S  4
/* SENS_SAR1_DAC_XPD_FSM : R/W ;bitpos:[3:0] ;default: 4'b0011 ; */
/*description: */
#define SENS_SAR1_DAC_XPD_FSM  0x0000000F
#define SENS_SAR1_DAC_XPD_FSM_M  ((SENS_SAR1_DAC_XPD_FSM_V)<<(SENS_SAR1_DAC_XPD_FSM_S))
#define SENS_SAR1_DAC_XPD_FSM_V  0xF
#define SENS_SAR1_DAC_XPD_FSM_S  0

#define SENS_SAR_READER2_CTRL_REG          (DR_REG_SENS_BASE + 0x0024)
/* SENS_SAR2_INT_EN : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: enable saradc2 to send out interrupt*/
#define SENS_SAR2_INT_EN  (BIT(30))
#define SENS_SAR2_INT_EN_M  (BIT(30))
#define SENS_SAR2_INT_EN_V  0x1
#define SENS_SAR2_INT_EN_S  30
/* SENS_SAR2_DATA_INV : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: Invert SAR ADC2 data*/
#define SENS_SAR2_DATA_INV  (BIT(29))
#define SENS_SAR2_DATA_INV_M  (BIT(29))
#define SENS_SAR2_DATA_INV_V  0x1
#define SENS_SAR2_DATA_INV_S  29
/* SENS_SAR2_SAMPLE_NUM : R/W ;bitpos:[26:19] ;default: 8'd0 ; */
/*description: */
#define SENS_SAR2_SAMPLE_NUM  0x000000FF
#define SENS_SAR2_SAMPLE_NUM_M  ((SENS_SAR2_SAMPLE_NUM_V)<<(SENS_SAR2_SAMPLE_NUM_S))
#define SENS_SAR2_SAMPLE_NUM_V  0xFF
#define SENS_SAR2_SAMPLE_NUM_S  19
/* SENS_SAR2_CLK_GATED : R/W ;bitpos:[18] ;default: 1'b1 ; */
/*description: */
#define SENS_SAR2_CLK_GATED  (BIT(18))
#define SENS_SAR2_CLK_GATED_M  (BIT(18))
#define SENS_SAR2_CLK_GATED_V  0x1
#define SENS_SAR2_CLK_GATED_S  18
/* SENS_SAR2_WAIT_ARB_CYCLE : R/W ;bitpos:[17:16] ;default: 2'b1 ; */
/*description: wait arbit stable after sar_done*/
#define SENS_SAR2_WAIT_ARB_CYCLE  0x00000003
#define SENS_SAR2_WAIT_ARB_CYCLE_M  ((SENS_SAR2_WAIT_ARB_CYCLE_V)<<(SENS_SAR2_WAIT_ARB_CYCLE_S))
#define SENS_SAR2_WAIT_ARB_CYCLE_V  0x3
#define SENS_SAR2_WAIT_ARB_CYCLE_S  16
/* SENS_SAR2_CLK_DIV : R/W ;bitpos:[7:0] ;default: 8'd2 ; */
/*description: clock divider*/
#define SENS_SAR2_CLK_DIV  0x000000FF
#define SENS_SAR2_CLK_DIV_M  ((SENS_SAR2_CLK_DIV_V)<<(SENS_SAR2_CLK_DIV_S))
#define SENS_SAR2_CLK_DIV_V  0xFF
#define SENS_SAR2_CLK_DIV_S  0

#define SENS_SAR_READER2_STATUS_REG          (DR_REG_SENS_BASE + 0x0028)
/* SENS_SAR2_READER_STATUS : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SENS_SAR2_READER_STATUS  0xFFFFFFFF
#define SENS_SAR2_READER_STATUS_M  ((SENS_SAR2_READER_STATUS_V)<<(SENS_SAR2_READER_STATUS_S))
#define SENS_SAR2_READER_STATUS_V  0xFFFFFFFF
#define SENS_SAR2_READER_STATUS_S  0

#define SENS_SAR_MEAS2_CTRL1_REG          (DR_REG_SENS_BASE + 0x002c)
/* SENS_SAR2_XPD_WAIT : R/W ;bitpos:[31:24] ;default: 8'h7 ; */
/*description: */
#define SENS_SAR2_XPD_WAIT  0x000000FF
#define SENS_SAR2_XPD_WAIT_M  ((SENS_SAR2_XPD_WAIT_V)<<(SENS_SAR2_XPD_WAIT_S))
#define SENS_SAR2_XPD_WAIT_V  0xFF
#define SENS_SAR2_XPD_WAIT_S  24
/* SENS_SAR2_RSTB_WAIT : R/W ;bitpos:[23:16] ;default: 8'd2 ; */
/*description: */
#define SENS_SAR2_RSTB_WAIT  0x000000FF
#define SENS_SAR2_RSTB_WAIT_M  ((SENS_SAR2_RSTB_WAIT_V)<<(SENS_SAR2_RSTB_WAIT_S))
#define SENS_SAR2_RSTB_WAIT_V  0xFF
#define SENS_SAR2_RSTB_WAIT_S  16
/* SENS_SAR2_STANDBY_WAIT : R/W ;bitpos:[15:8] ;default: 8'd2 ; */
/*description: */
#define SENS_SAR2_STANDBY_WAIT  0x000000FF
#define SENS_SAR2_STANDBY_WAIT_M  ((SENS_SAR2_STANDBY_WAIT_V)<<(SENS_SAR2_STANDBY_WAIT_S))
#define SENS_SAR2_STANDBY_WAIT_V  0xFF
#define SENS_SAR2_STANDBY_WAIT_S  8
/* SENS_SAR2_RSTB_FORCE : R/W ;bitpos:[7:6] ;default: 2'b0 ; */
/*description: */
#define SENS_SAR2_RSTB_FORCE  0x00000003
#define SENS_SAR2_RSTB_FORCE_M  ((SENS_SAR2_RSTB_FORCE_V)<<(SENS_SAR2_RSTB_FORCE_S))
#define SENS_SAR2_RSTB_FORCE_V  0x3
#define SENS_SAR2_RSTB_FORCE_S  6
/* SENS_SAR2_EN_TEST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: SAR2_EN_TEST*/
#define SENS_SAR2_EN_TEST  (BIT(5))
#define SENS_SAR2_EN_TEST_M  (BIT(5))
#define SENS_SAR2_EN_TEST_V  0x1
#define SENS_SAR2_EN_TEST_S  5
/* SENS_SAR2_PKDET_CAL_EN : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: rtc control pkdet enable*/
#define SENS_SAR2_PKDET_CAL_EN  (BIT(4))
#define SENS_SAR2_PKDET_CAL_EN_M  (BIT(4))
#define SENS_SAR2_PKDET_CAL_EN_V  0x1
#define SENS_SAR2_PKDET_CAL_EN_S  4
/* SENS_SAR2_PWDET_CAL_EN : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: rtc control pwdet enable*/
#define SENS_SAR2_PWDET_CAL_EN  (BIT(3))
#define SENS_SAR2_PWDET_CAL_EN_M  (BIT(3))
#define SENS_SAR2_PWDET_CAL_EN_V  0x1
#define SENS_SAR2_PWDET_CAL_EN_S  3
/* SENS_SAR2_CNTL_STATE : RO ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: saradc2_cntl_fsm*/
#define SENS_SAR2_CNTL_STATE  0x00000007
#define SENS_SAR2_CNTL_STATE_M  ((SENS_SAR2_CNTL_STATE_V)<<(SENS_SAR2_CNTL_STATE_S))
#define SENS_SAR2_CNTL_STATE_V  0x7
#define SENS_SAR2_CNTL_STATE_S  0

#define SENS_SAR_MEAS2_CTRL2_REG          (DR_REG_SENS_BASE + 0x0030)
/* SENS_SAR2_EN_PAD_FORCE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: 1: SAR ADC2 pad enable bitmap is controlled by SW*/
#define SENS_SAR2_EN_PAD_FORCE  (BIT(31))
#define SENS_SAR2_EN_PAD_FORCE_M  (BIT(31))
#define SENS_SAR2_EN_PAD_FORCE_V  0x1
#define SENS_SAR2_EN_PAD_FORCE_S  31
/* SENS_SAR2_EN_PAD : R/W ;bitpos:[30:19] ;default: 12'b0 ; */
/*description: SAR ADC2 pad enable bitmap*/
#define SENS_SAR2_EN_PAD  0x00000FFF
#define SENS_SAR2_EN_PAD_M  ((SENS_SAR2_EN_PAD_V)<<(SENS_SAR2_EN_PAD_S))
#define SENS_SAR2_EN_PAD_V  0xFFF
#define SENS_SAR2_EN_PAD_S  19
/* SENS_MEAS2_START_FORCE : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: 1: SAR ADC2 controller (in RTC) is started by SW*/
#define SENS_MEAS2_START_FORCE  (BIT(18))
#define SENS_MEAS2_START_FORCE_M  (BIT(18))
#define SENS_MEAS2_START_FORCE_V  0x1
#define SENS_MEAS2_START_FORCE_S  18
/* SENS_MEAS2_START_SAR : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: SAR ADC2 controller (in RTC) starts conversion*/
#define SENS_MEAS2_START_SAR  (BIT(17))
#define SENS_MEAS2_START_SAR_M  (BIT(17))
#define SENS_MEAS2_START_SAR_V  0x1
#define SENS_MEAS2_START_SAR_S  17
/* SENS_MEAS2_DONE_SAR : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: SAR ADC2 conversion done indication*/
#define SENS_MEAS2_DONE_SAR  (BIT(16))
#define SENS_MEAS2_DONE_SAR_M  (BIT(16))
#define SENS_MEAS2_DONE_SAR_V  0x1
#define SENS_MEAS2_DONE_SAR_S  16
/* SENS_MEAS2_DATA_SAR : RO ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: SAR ADC2 data*/
#define SENS_MEAS2_DATA_SAR  0x0000FFFF
#define SENS_MEAS2_DATA_SAR_M  ((SENS_MEAS2_DATA_SAR_V)<<(SENS_MEAS2_DATA_SAR_S))
#define SENS_MEAS2_DATA_SAR_V  0xFFFF
#define SENS_MEAS2_DATA_SAR_S  0

#define SENS_SAR_MEAS2_MUX_REG          (DR_REG_SENS_BASE + 0x0034)
/* SENS_SAR2_RTC_FORCE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: in sleep  force to use rtc to control ADC*/
#define SENS_SAR2_RTC_FORCE  (BIT(31))
#define SENS_SAR2_RTC_FORCE_M  (BIT(31))
#define SENS_SAR2_RTC_FORCE_V  0x1
#define SENS_SAR2_RTC_FORCE_S  31
/* SENS_SAR2_PWDET_CCT : R/W ;bitpos:[30:28] ;default: 3'b0 ; */
/*description: SAR2_PWDET_CCT*/
#define SENS_SAR2_PWDET_CCT  0x00000007
#define SENS_SAR2_PWDET_CCT_M  ((SENS_SAR2_PWDET_CCT_V)<<(SENS_SAR2_PWDET_CCT_S))
#define SENS_SAR2_PWDET_CCT_V  0x7
#define SENS_SAR2_PWDET_CCT_S  28

#define SENS_SAR_ATTEN2_REG          (DR_REG_SENS_BASE + 0x0038)
/* SENS_SAR2_ATTEN : R/W ;bitpos:[31:0] ;default: 32'hffffffff ; */
/*description: 2-bit attenuation for each pad*/
#define SENS_SAR2_ATTEN  0xFFFFFFFF
#define SENS_SAR2_ATTEN_M  ((SENS_SAR2_ATTEN_V)<<(SENS_SAR2_ATTEN_S))
#define SENS_SAR2_ATTEN_V  0xFFFFFFFF
#define SENS_SAR2_ATTEN_S  0

#define SENS_SAR_POWER_XPD_SAR_REG          (DR_REG_SENS_BASE + 0x003c)
/* SENS_SARCLK_EN : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define SENS_SARCLK_EN  (BIT(31))
#define SENS_SARCLK_EN_M  (BIT(31))
#define SENS_SARCLK_EN_V  0x1
#define SENS_SARCLK_EN_S  31
/* SENS_FORCE_XPD_SAR : R/W ;bitpos:[30:29] ;default: 2'd0 ; */
/*description: */
#define SENS_FORCE_XPD_SAR  0x00000003
#define SENS_FORCE_XPD_SAR_M  ((SENS_FORCE_XPD_SAR_V)<<(SENS_FORCE_XPD_SAR_S))
#define SENS_FORCE_XPD_SAR_V  0x3
#define SENS_FORCE_XPD_SAR_S  29
#define SENS_FORCE_XPD_SAR_SW_M (BIT1)
#define SENS_FORCE_XPD_SAR_FSM 0 // Use FSM to control power down
#define SENS_FORCE_XPD_SAR_PD  2 // Force power down
#define SENS_FORCE_XPD_SAR_PU  3 // Force power up
#define SENS_SAR_SLAVE_ADDR1_REG          (DR_REG_SENS_BASE + 0x0040)
/* SENS_MEAS_STATUS : RO ;bitpos:[29:22] ;default: 8'h0 ; */
/*description: */
#define SENS_MEAS_STATUS  0x000000FF
#define SENS_MEAS_STATUS_M  ((SENS_MEAS_STATUS_V)<<(SENS_MEAS_STATUS_S))
#define SENS_MEAS_STATUS_V  0xFF
#define SENS_MEAS_STATUS_S  22
/* SENS_I2C_SLAVE_ADDR0 : R/W ;bitpos:[21:11] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR0  0x000007FF
#define SENS_I2C_SLAVE_ADDR0_M  ((SENS_I2C_SLAVE_ADDR0_V)<<(SENS_I2C_SLAVE_ADDR0_S))
#define SENS_I2C_SLAVE_ADDR0_V  0x7FF
#define SENS_I2C_SLAVE_ADDR0_S  11
/* SENS_I2C_SLAVE_ADDR1 : R/W ;bitpos:[10:0] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR1  0x000007FF
#define SENS_I2C_SLAVE_ADDR1_M  ((SENS_I2C_SLAVE_ADDR1_V)<<(SENS_I2C_SLAVE_ADDR1_S))
#define SENS_I2C_SLAVE_ADDR1_V  0x7FF
#define SENS_I2C_SLAVE_ADDR1_S  0

#define SENS_SAR_SLAVE_ADDR2_REG          (DR_REG_SENS_BASE + 0x0044)
/* SENS_I2C_SLAVE_ADDR2 : R/W ;bitpos:[21:11] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR2  0x000007FF
#define SENS_I2C_SLAVE_ADDR2_M  ((SENS_I2C_SLAVE_ADDR2_V)<<(SENS_I2C_SLAVE_ADDR2_S))
#define SENS_I2C_SLAVE_ADDR2_V  0x7FF
#define SENS_I2C_SLAVE_ADDR2_S  11
/* SENS_I2C_SLAVE_ADDR3 : R/W ;bitpos:[10:0] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR3  0x000007FF
#define SENS_I2C_SLAVE_ADDR3_M  ((SENS_I2C_SLAVE_ADDR3_V)<<(SENS_I2C_SLAVE_ADDR3_S))
#define SENS_I2C_SLAVE_ADDR3_V  0x7FF
#define SENS_I2C_SLAVE_ADDR3_S  0

#define SENS_SAR_SLAVE_ADDR3_REG          (DR_REG_SENS_BASE + 0x0048)
/* SENS_I2C_SLAVE_ADDR4 : R/W ;bitpos:[21:11] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR4  0x000007FF
#define SENS_I2C_SLAVE_ADDR4_M  ((SENS_I2C_SLAVE_ADDR4_V)<<(SENS_I2C_SLAVE_ADDR4_S))
#define SENS_I2C_SLAVE_ADDR4_V  0x7FF
#define SENS_I2C_SLAVE_ADDR4_S  11
/* SENS_I2C_SLAVE_ADDR5 : R/W ;bitpos:[10:0] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR5  0x000007FF
#define SENS_I2C_SLAVE_ADDR5_M  ((SENS_I2C_SLAVE_ADDR5_V)<<(SENS_I2C_SLAVE_ADDR5_S))
#define SENS_I2C_SLAVE_ADDR5_V  0x7FF
#define SENS_I2C_SLAVE_ADDR5_S  0

#define SENS_SAR_SLAVE_ADDR4_REG          (DR_REG_SENS_BASE + 0x004c)
/* SENS_I2C_SLAVE_ADDR6 : R/W ;bitpos:[21:11] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR6  0x000007FF
#define SENS_I2C_SLAVE_ADDR6_M  ((SENS_I2C_SLAVE_ADDR6_V)<<(SENS_I2C_SLAVE_ADDR6_S))
#define SENS_I2C_SLAVE_ADDR6_V  0x7FF
#define SENS_I2C_SLAVE_ADDR6_S  11
/* SENS_I2C_SLAVE_ADDR7 : R/W ;bitpos:[10:0] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR7  0x000007FF
#define SENS_I2C_SLAVE_ADDR7_M  ((SENS_I2C_SLAVE_ADDR7_V)<<(SENS_I2C_SLAVE_ADDR7_S))
#define SENS_I2C_SLAVE_ADDR7_V  0x7FF
#define SENS_I2C_SLAVE_ADDR7_S  0

#define SENS_SAR_TSENS_CTRL_REG          (DR_REG_SENS_BASE + 0x0050)
/* SENS_TSENS_DUMP_OUT : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: temperature sensor dump out*/
#define SENS_TSENS_DUMP_OUT  (BIT(24))
#define SENS_TSENS_DUMP_OUT_M  (BIT(24))
#define SENS_TSENS_DUMP_OUT_V  0x1
#define SENS_TSENS_DUMP_OUT_S  24
/* SENS_TSENS_POWER_UP_FORCE : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: 1: dump out & power up controlled by SW*/
#define SENS_TSENS_POWER_UP_FORCE  (BIT(23))
#define SENS_TSENS_POWER_UP_FORCE_M  (BIT(23))
#define SENS_TSENS_POWER_UP_FORCE_V  0x1
#define SENS_TSENS_POWER_UP_FORCE_S  23
/* SENS_TSENS_POWER_UP : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: temperature sensor power up*/
#define SENS_TSENS_POWER_UP  (BIT(22))
#define SENS_TSENS_POWER_UP_M  (BIT(22))
#define SENS_TSENS_POWER_UP_V  0x1
#define SENS_TSENS_POWER_UP_S  22
/* SENS_TSENS_CLK_DIV : R/W ;bitpos:[21:14] ;default: 8'd6 ; */
/*description: temperature sensor clock divider*/
#define SENS_TSENS_CLK_DIV  0x000000FF
#define SENS_TSENS_CLK_DIV_M  ((SENS_TSENS_CLK_DIV_V)<<(SENS_TSENS_CLK_DIV_S))
#define SENS_TSENS_CLK_DIV_V  0xFF
#define SENS_TSENS_CLK_DIV_S  14
/* SENS_TSENS_IN_INV : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: invert temperature sensor data*/
#define SENS_TSENS_IN_INV  (BIT(13))
#define SENS_TSENS_IN_INV_M  (BIT(13))
#define SENS_TSENS_IN_INV_V  0x1
#define SENS_TSENS_IN_INV_S  13
/* SENS_TSENS_INT_EN : R/W ;bitpos:[12] ;default: 1'b1 ; */
/*description: enable temperature sensor to send out interrupt*/
#define SENS_TSENS_INT_EN  (BIT(12))
#define SENS_TSENS_INT_EN_M  (BIT(12))
#define SENS_TSENS_INT_EN_V  0x1
#define SENS_TSENS_INT_EN_S  12
/* SENS_TSENS_READY : RO ;bitpos:[8] ;default: 1'h0 ; */
/*description: indicate temperature sensor out ready*/
#define SENS_TSENS_READY  (BIT(8))
#define SENS_TSENS_READY_M  (BIT(8))
#define SENS_TSENS_READY_V  0x1
#define SENS_TSENS_READY_S  8
/* SENS_TSENS_OUT : RO ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: temperature sensor data out*/
#define SENS_TSENS_OUT  0x000000FF
#define SENS_TSENS_OUT_M  ((SENS_TSENS_OUT_V)<<(SENS_TSENS_OUT_S))
#define SENS_TSENS_OUT_V  0xFF
#define SENS_TSENS_OUT_S  0

#define SENS_SAR_TSENS_CTRL2_REG          (DR_REG_SENS_BASE + 0x0054)
/* SENS_TSENS_RESET : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: temperature sensor reset*/
#define SENS_TSENS_RESET  (BIT(16))
#define SENS_TSENS_RESET_M  (BIT(16))
#define SENS_TSENS_RESET_V  0x1
#define SENS_TSENS_RESET_S  16
/* SENS_TSENS_CLKGATE_EN : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: temperature sensor clock enable*/
#define SENS_TSENS_CLKGATE_EN  (BIT(15))
#define SENS_TSENS_CLKGATE_EN_M  (BIT(15))
#define SENS_TSENS_CLKGATE_EN_V  0x1
#define SENS_TSENS_CLKGATE_EN_S  15
/* SENS_TSENS_CLK_INV : R/W ;bitpos:[14] ;default: 1'b1 ; */
/*description: */
#define SENS_TSENS_CLK_INV  (BIT(14))
#define SENS_TSENS_CLK_INV_M  (BIT(14))
#define SENS_TSENS_CLK_INV_V  0x1
#define SENS_TSENS_CLK_INV_S  14
/* SENS_TSENS_XPD_FORCE : R/W ;bitpos:[13:12] ;default: 2'b0 ; */
/*description: */
#define SENS_TSENS_XPD_FORCE  0x00000003
#define SENS_TSENS_XPD_FORCE_M  ((SENS_TSENS_XPD_FORCE_V)<<(SENS_TSENS_XPD_FORCE_S))
#define SENS_TSENS_XPD_FORCE_V  0x3
#define SENS_TSENS_XPD_FORCE_S  12
/* SENS_TSENS_XPD_WAIT : R/W ;bitpos:[11:0] ;default: 12'h2 ; */
/*description: */
#define SENS_TSENS_XPD_WAIT  0x00000FFF
#define SENS_TSENS_XPD_WAIT_M  ((SENS_TSENS_XPD_WAIT_V)<<(SENS_TSENS_XPD_WAIT_S))
#define SENS_TSENS_XPD_WAIT_V  0xFFF
#define SENS_TSENS_XPD_WAIT_S  0

#define SENS_SAR_I2C_CTRL_REG          (DR_REG_SENS_BASE + 0x0058)
/* SENS_SAR_I2C_START_FORCE : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: 1: I2C started by SW*/
#define SENS_SAR_I2C_START_FORCE  (BIT(29))
#define SENS_SAR_I2C_START_FORCE_M  (BIT(29))
#define SENS_SAR_I2C_START_FORCE_V  0x1
#define SENS_SAR_I2C_START_FORCE_S  29
/* SENS_SAR_I2C_START : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: start I2C*/
#define SENS_SAR_I2C_START  (BIT(28))
#define SENS_SAR_I2C_START_M  (BIT(28))
#define SENS_SAR_I2C_START_V  0x1
#define SENS_SAR_I2C_START_S  28
/* SENS_SAR_I2C_CTRL : R/W ;bitpos:[27:0] ;default: 28'b0 ; */
/*description: I2C control data*/
#define SENS_SAR_I2C_CTRL  0x0FFFFFFF
#define SENS_SAR_I2C_CTRL_M  ((SENS_SAR_I2C_CTRL_V)<<(SENS_SAR_I2C_CTRL_S))
#define SENS_SAR_I2C_CTRL_V  0xFFFFFFF
#define SENS_SAR_I2C_CTRL_S  0

#define SENS_SAR_TOUCH_CONF_REG          (DR_REG_SENS_BASE + 0x005c)
/* SENS_TOUCH_APPROACH_PAD0 : R/W ;bitpos:[31:28] ;default: 4'hF ; */
/*description: indicate which pad is approach pad0*/
#define SENS_TOUCH_APPROACH_PAD0  0x0000000F
#define SENS_TOUCH_APPROACH_PAD0_M  ((SENS_TOUCH_APPROACH_PAD0_V)<<(SENS_TOUCH_APPROACH_PAD0_S))
#define SENS_TOUCH_APPROACH_PAD0_V  0xF
#define SENS_TOUCH_APPROACH_PAD0_S  28
/* SENS_TOUCH_APPROACH_PAD1 : R/W ;bitpos:[27:24] ;default: 4'hF ; */
/*description: indicate which pad is approach pad1*/
#define SENS_TOUCH_APPROACH_PAD1  0x0000000F
#define SENS_TOUCH_APPROACH_PAD1_M  ((SENS_TOUCH_APPROACH_PAD1_V)<<(SENS_TOUCH_APPROACH_PAD1_S))
#define SENS_TOUCH_APPROACH_PAD1_V  0xF
#define SENS_TOUCH_APPROACH_PAD1_S  24
/* SENS_TOUCH_APPROACH_PAD2 : R/W ;bitpos:[23:20] ;default: 4'hF ; */
/*description: indicate which pad is approach pad2*/
#define SENS_TOUCH_APPROACH_PAD2  0x0000000F
#define SENS_TOUCH_APPROACH_PAD2_M  ((SENS_TOUCH_APPROACH_PAD2_V)<<(SENS_TOUCH_APPROACH_PAD2_S))
#define SENS_TOUCH_APPROACH_PAD2_V  0xF
#define SENS_TOUCH_APPROACH_PAD2_S  20
/* SENS_TOUCH_UNIT_END : RO ;bitpos:[19] ;default: 1'd0 ; */
/*description: touch_unit_done*/
#define SENS_TOUCH_UNIT_END  (BIT(19))
#define SENS_TOUCH_UNIT_END_M  (BIT(19))
#define SENS_TOUCH_UNIT_END_V  0x1
#define SENS_TOUCH_UNIT_END_S  19
/* SENS_TOUCH_DENOISE_END : RO ;bitpos:[18] ;default: 1'd0 ; */
/*description: touch_denoise_done*/
#define SENS_TOUCH_DENOISE_END  (BIT(18))
#define SENS_TOUCH_DENOISE_END_M  (BIT(18))
#define SENS_TOUCH_DENOISE_END_V  0x1
#define SENS_TOUCH_DENOISE_END_S  18
/* SENS_TOUCH_DATA_SEL : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: 3: smooth data 2: benchmark 1 0: raw_data*/
#define SENS_TOUCH_DATA_SEL  0x00000003
#define SENS_TOUCH_DATA_SEL_M  ((SENS_TOUCH_DATA_SEL_V)<<(SENS_TOUCH_DATA_SEL_S))
#define SENS_TOUCH_DATA_SEL_V  0x3
#define SENS_TOUCH_DATA_SEL_S  16
/* SENS_TOUCH_STATUS_CLR : WO ;bitpos:[15] ;default: 1'd0 ; */
/*description: clear all touch active status*/
#define SENS_TOUCH_STATUS_CLR  (BIT(15))
#define SENS_TOUCH_STATUS_CLR_M  (BIT(15))
#define SENS_TOUCH_STATUS_CLR_V  0x1
#define SENS_TOUCH_STATUS_CLR_S  15
/* SENS_TOUCH_OUTEN : R/W ;bitpos:[14:0] ;default: 15'h7FFF ; */
/*description: touch controller output enable*/
#define SENS_TOUCH_OUTEN  0x00007FFF
#define SENS_TOUCH_OUTEN_M  ((SENS_TOUCH_OUTEN_V)<<(SENS_TOUCH_OUTEN_S))
#define SENS_TOUCH_OUTEN_V  0x7FFF
#define SENS_TOUCH_OUTEN_S  0

#define SENS_SAR_TOUCH_THRES1_REG          (DR_REG_SENS_BASE + 0x0060)
/* SENS_TOUCH_OUT_TH1 : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: Finger threshold for touch pad 1*/
#define SENS_TOUCH_OUT_TH1  0x003FFFFF
#define SENS_TOUCH_OUT_TH1_M  ((SENS_TOUCH_OUT_TH1_V)<<(SENS_TOUCH_OUT_TH1_S))
#define SENS_TOUCH_OUT_TH1_V  0x3FFFFF
#define SENS_TOUCH_OUT_TH1_S  0

#define SENS_SAR_TOUCH_THRES2_REG          (DR_REG_SENS_BASE + 0x0064)
/* SENS_TOUCH_OUT_TH2 : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: Finger threshold for touch pad 2*/
#define SENS_TOUCH_OUT_TH2  0x003FFFFF
#define SENS_TOUCH_OUT_TH2_M  ((SENS_TOUCH_OUT_TH2_V)<<(SENS_TOUCH_OUT_TH2_S))
#define SENS_TOUCH_OUT_TH2_V  0x3FFFFF
#define SENS_TOUCH_OUT_TH2_S  0

#define SENS_SAR_TOUCH_THRES3_REG          (DR_REG_SENS_BASE + 0x0068)
/* SENS_TOUCH_OUT_TH3 : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: Finger threshold for touch pad 3*/
#define SENS_TOUCH_OUT_TH3  0x003FFFFF
#define SENS_TOUCH_OUT_TH3_M  ((SENS_TOUCH_OUT_TH3_V)<<(SENS_TOUCH_OUT_TH3_S))
#define SENS_TOUCH_OUT_TH3_V  0x3FFFFF
#define SENS_TOUCH_OUT_TH3_S  0

#define SENS_SAR_TOUCH_THRES4_REG          (DR_REG_SENS_BASE + 0x006c)
/* SENS_TOUCH_OUT_TH4 : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: Finger threshold for touch pad 4*/
#define SENS_TOUCH_OUT_TH4  0x003FFFFF
#define SENS_TOUCH_OUT_TH4_M  ((SENS_TOUCH_OUT_TH4_V)<<(SENS_TOUCH_OUT_TH4_S))
#define SENS_TOUCH_OUT_TH4_V  0x3FFFFF
#define SENS_TOUCH_OUT_TH4_S  0

#define SENS_SAR_TOUCH_THRES5_REG          (DR_REG_SENS_BASE + 0x0070)
/* SENS_TOUCH_OUT_TH5 : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: Finger threshold for touch pad 5*/
#define SENS_TOUCH_OUT_TH5  0x003FFFFF
#define SENS_TOUCH_OUT_TH5_M  ((SENS_TOUCH_OUT_TH5_V)<<(SENS_TOUCH_OUT_TH5_S))
#define SENS_TOUCH_OUT_TH5_V  0x3FFFFF
#define SENS_TOUCH_OUT_TH5_S  0

#define SENS_SAR_TOUCH_THRES6_REG          (DR_REG_SENS_BASE + 0x0074)
/* SENS_TOUCH_OUT_TH6 : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: Finger threshold for touch pad 6*/
#define SENS_TOUCH_OUT_TH6  0x003FFFFF
#define SENS_TOUCH_OUT_TH6_M  ((SENS_TOUCH_OUT_TH6_V)<<(SENS_TOUCH_OUT_TH6_S))
#define SENS_TOUCH_OUT_TH6_V  0x3FFFFF
#define SENS_TOUCH_OUT_TH6_S  0

#define SENS_SAR_TOUCH_THRES7_REG          (DR_REG_SENS_BASE + 0x0078)
/* SENS_TOUCH_OUT_TH7 : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: Finger threshold for touch pad 7*/
#define SENS_TOUCH_OUT_TH7  0x003FFFFF
#define SENS_TOUCH_OUT_TH7_M  ((SENS_TOUCH_OUT_TH7_V)<<(SENS_TOUCH_OUT_TH7_S))
#define SENS_TOUCH_OUT_TH7_V  0x3FFFFF
#define SENS_TOUCH_OUT_TH7_S  0

#define SENS_SAR_TOUCH_THRES8_REG          (DR_REG_SENS_BASE + 0x007c)
/* SENS_TOUCH_OUT_TH8 : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: Finger threshold for touch pad 8*/
#define SENS_TOUCH_OUT_TH8  0x003FFFFF
#define SENS_TOUCH_OUT_TH8_M  ((SENS_TOUCH_OUT_TH8_V)<<(SENS_TOUCH_OUT_TH8_S))
#define SENS_TOUCH_OUT_TH8_V  0x3FFFFF
#define SENS_TOUCH_OUT_TH8_S  0

#define SENS_SAR_TOUCH_THRES9_REG          (DR_REG_SENS_BASE + 0x0080)
/* SENS_TOUCH_OUT_TH9 : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: Finger threshold for touch pad 9*/
#define SENS_TOUCH_OUT_TH9  0x003FFFFF
#define SENS_TOUCH_OUT_TH9_M  ((SENS_TOUCH_OUT_TH9_V)<<(SENS_TOUCH_OUT_TH9_S))
#define SENS_TOUCH_OUT_TH9_V  0x3FFFFF
#define SENS_TOUCH_OUT_TH9_S  0

#define SENS_SAR_TOUCH_THRES10_REG          (DR_REG_SENS_BASE + 0x0084)
/* SENS_TOUCH_OUT_TH10 : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: Finger threshold for touch pad 10*/
#define SENS_TOUCH_OUT_TH10  0x003FFFFF
#define SENS_TOUCH_OUT_TH10_M  ((SENS_TOUCH_OUT_TH10_V)<<(SENS_TOUCH_OUT_TH10_S))
#define SENS_TOUCH_OUT_TH10_V  0x3FFFFF
#define SENS_TOUCH_OUT_TH10_S  0

#define SENS_SAR_TOUCH_THRES11_REG          (DR_REG_SENS_BASE + 0x0088)
/* SENS_TOUCH_OUT_TH11 : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: Finger threshold for touch pad 11*/
#define SENS_TOUCH_OUT_TH11  0x003FFFFF
#define SENS_TOUCH_OUT_TH11_M  ((SENS_TOUCH_OUT_TH11_V)<<(SENS_TOUCH_OUT_TH11_S))
#define SENS_TOUCH_OUT_TH11_V  0x3FFFFF
#define SENS_TOUCH_OUT_TH11_S  0

#define SENS_SAR_TOUCH_THRES12_REG          (DR_REG_SENS_BASE + 0x008c)
/* SENS_TOUCH_OUT_TH12 : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: Finger threshold for touch pad 12*/
#define SENS_TOUCH_OUT_TH12  0x003FFFFF
#define SENS_TOUCH_OUT_TH12_M  ((SENS_TOUCH_OUT_TH12_V)<<(SENS_TOUCH_OUT_TH12_S))
#define SENS_TOUCH_OUT_TH12_V  0x3FFFFF
#define SENS_TOUCH_OUT_TH12_S  0

#define SENS_SAR_TOUCH_THRES13_REG          (DR_REG_SENS_BASE + 0x0090)
/* SENS_TOUCH_OUT_TH13 : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: Finger threshold for touch pad 13*/
#define SENS_TOUCH_OUT_TH13  0x003FFFFF
#define SENS_TOUCH_OUT_TH13_M  ((SENS_TOUCH_OUT_TH13_V)<<(SENS_TOUCH_OUT_TH13_S))
#define SENS_TOUCH_OUT_TH13_V  0x3FFFFF
#define SENS_TOUCH_OUT_TH13_S  0

#define SENS_SAR_TOUCH_THRES14_REG          (DR_REG_SENS_BASE + 0x0094)
/* SENS_TOUCH_OUT_TH14 : R/W ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: Finger threshold for touch pad 14*/
#define SENS_TOUCH_OUT_TH14  0x003FFFFF
#define SENS_TOUCH_OUT_TH14_M  ((SENS_TOUCH_OUT_TH14_V)<<(SENS_TOUCH_OUT_TH14_S))
#define SENS_TOUCH_OUT_TH14_V  0x3FFFFF
#define SENS_TOUCH_OUT_TH14_S  0

#define SENS_SAR_TOUCH_CHN_ST_REG          (DR_REG_SENS_BASE + 0x00d4)
/* SENS_TOUCH_MEAS_DONE : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define SENS_TOUCH_MEAS_DONE  (BIT(31))
#define SENS_TOUCH_MEAS_DONE_M  (BIT(31))
#define SENS_TOUCH_MEAS_DONE_V  0x1
#define SENS_TOUCH_MEAS_DONE_S  31
/* SENS_TOUCH_CHANNEL_CLR : WO ;bitpos:[29:15] ;default: 15'd0 ; */
/*description: Clear touch channel*/
#define SENS_TOUCH_CHANNEL_CLR  0x00007FFF
#define SENS_TOUCH_CHANNEL_CLR_M  ((SENS_TOUCH_CHANNEL_CLR_V)<<(SENS_TOUCH_CHANNEL_CLR_S))
#define SENS_TOUCH_CHANNEL_CLR_V  0x7FFF
#define SENS_TOUCH_CHANNEL_CLR_S  15
/* SENS_TOUCH_PAD_ACTIVE : RO ;bitpos:[14:0] ;default: 15'd0 ; */
/*description: touch active status*/
#define SENS_TOUCH_PAD_ACTIVE  0x00007FFF
#define SENS_TOUCH_PAD_ACTIVE_M  ((SENS_TOUCH_PAD_ACTIVE_V)<<(SENS_TOUCH_PAD_ACTIVE_S))
#define SENS_TOUCH_PAD_ACTIVE_V  0x7FFF
#define SENS_TOUCH_PAD_ACTIVE_S  0

#define SENS_SAR_TOUCH_STATUS0_REG          (DR_REG_SENS_BASE + 0x00d8)
/* SENS_TOUCH_SCAN_CURR : RO ;bitpos:[25:22] ;default: 4'd0 ; */
/*description: */
#define SENS_TOUCH_SCAN_CURR  0x0000000F
#define SENS_TOUCH_SCAN_CURR_M  ((SENS_TOUCH_SCAN_CURR_V)<<(SENS_TOUCH_SCAN_CURR_S))
#define SENS_TOUCH_SCAN_CURR_V  0xF
#define SENS_TOUCH_SCAN_CURR_S  22
/* SENS_TOUCH_DENOISE_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: the counter for touch pad 0*/
#define SENS_TOUCH_DENOISE_DATA  0x003FFFFF
#define SENS_TOUCH_DENOISE_DATA_M  ((SENS_TOUCH_DENOISE_DATA_V)<<(SENS_TOUCH_DENOISE_DATA_S))
#define SENS_TOUCH_DENOISE_DATA_V  0x3FFFFF
#define SENS_TOUCH_DENOISE_DATA_S  0

#define SENS_SAR_TOUCH_STATUS1_REG          (DR_REG_SENS_BASE + 0x00dc)
/* SENS_TOUCH_PAD1_DEBOUNCE : RO ;bitpos:[31:29] ;default: 3'd0 ; */
/*description: */
#define SENS_TOUCH_PAD1_DEBOUNCE  0x00000007
#define SENS_TOUCH_PAD1_DEBOUNCE_M  ((SENS_TOUCH_PAD1_DEBOUNCE_V)<<(SENS_TOUCH_PAD1_DEBOUNCE_S))
#define SENS_TOUCH_PAD1_DEBOUNCE_V  0x7
#define SENS_TOUCH_PAD1_DEBOUNCE_S  29
/* SENS_TOUCH_PAD1_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define SENS_TOUCH_PAD1_DATA  0x003FFFFF
#define SENS_TOUCH_PAD1_DATA_M  ((SENS_TOUCH_PAD1_DATA_V)<<(SENS_TOUCH_PAD1_DATA_S))
#define SENS_TOUCH_PAD1_DATA_V  0x3FFFFF
#define SENS_TOUCH_PAD1_DATA_S  0

#define SENS_SAR_TOUCH_STATUS2_REG          (DR_REG_SENS_BASE + 0x00e0)
/* SENS_TOUCH_PAD2_DEBOUNCE : RO ;bitpos:[31:29] ;default: 3'd0 ; */
/*description: */
#define SENS_TOUCH_PAD2_DEBOUNCE  0x00000007
#define SENS_TOUCH_PAD2_DEBOUNCE_M  ((SENS_TOUCH_PAD2_DEBOUNCE_V)<<(SENS_TOUCH_PAD2_DEBOUNCE_S))
#define SENS_TOUCH_PAD2_DEBOUNCE_V  0x7
#define SENS_TOUCH_PAD2_DEBOUNCE_S  29
/* SENS_TOUCH_PAD2_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define SENS_TOUCH_PAD2_DATA  0x003FFFFF
#define SENS_TOUCH_PAD2_DATA_M  ((SENS_TOUCH_PAD2_DATA_V)<<(SENS_TOUCH_PAD2_DATA_S))
#define SENS_TOUCH_PAD2_DATA_V  0x3FFFFF
#define SENS_TOUCH_PAD2_DATA_S  0

#define SENS_SAR_TOUCH_STATUS3_REG          (DR_REG_SENS_BASE + 0x00e4)
/* SENS_TOUCH_PAD3_DEBOUNCE : RO ;bitpos:[31:29] ;default: 3'd0 ; */
/*description: */
#define SENS_TOUCH_PAD3_DEBOUNCE  0x00000007
#define SENS_TOUCH_PAD3_DEBOUNCE_M  ((SENS_TOUCH_PAD3_DEBOUNCE_V)<<(SENS_TOUCH_PAD3_DEBOUNCE_S))
#define SENS_TOUCH_PAD3_DEBOUNCE_V  0x7
#define SENS_TOUCH_PAD3_DEBOUNCE_S  29
/* SENS_TOUCH_PAD3_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define SENS_TOUCH_PAD3_DATA  0x003FFFFF
#define SENS_TOUCH_PAD3_DATA_M  ((SENS_TOUCH_PAD3_DATA_V)<<(SENS_TOUCH_PAD3_DATA_S))
#define SENS_TOUCH_PAD3_DATA_V  0x3FFFFF
#define SENS_TOUCH_PAD3_DATA_S  0

#define SENS_SAR_TOUCH_STATUS4_REG          (DR_REG_SENS_BASE + 0x00e8)
/* SENS_TOUCH_PAD4_DEBOUNCE : RO ;bitpos:[31:29] ;default: 3'd0 ; */
/*description: */
#define SENS_TOUCH_PAD4_DEBOUNCE  0x00000007
#define SENS_TOUCH_PAD4_DEBOUNCE_M  ((SENS_TOUCH_PAD4_DEBOUNCE_V)<<(SENS_TOUCH_PAD4_DEBOUNCE_S))
#define SENS_TOUCH_PAD4_DEBOUNCE_V  0x7
#define SENS_TOUCH_PAD4_DEBOUNCE_S  29
/* SENS_TOUCH_PAD4_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define SENS_TOUCH_PAD4_DATA  0x003FFFFF
#define SENS_TOUCH_PAD4_DATA_M  ((SENS_TOUCH_PAD4_DATA_V)<<(SENS_TOUCH_PAD4_DATA_S))
#define SENS_TOUCH_PAD4_DATA_V  0x3FFFFF
#define SENS_TOUCH_PAD4_DATA_S  0

#define SENS_SAR_TOUCH_STATUS5_REG          (DR_REG_SENS_BASE + 0x00ec)
/* SENS_TOUCH_PAD5_DEBOUNCE : RO ;bitpos:[31:29] ;default: 3'd0 ; */
/*description: */
#define SENS_TOUCH_PAD5_DEBOUNCE  0x00000007
#define SENS_TOUCH_PAD5_DEBOUNCE_M  ((SENS_TOUCH_PAD5_DEBOUNCE_V)<<(SENS_TOUCH_PAD5_DEBOUNCE_S))
#define SENS_TOUCH_PAD5_DEBOUNCE_V  0x7
#define SENS_TOUCH_PAD5_DEBOUNCE_S  29
/* SENS_TOUCH_PAD5_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define SENS_TOUCH_PAD5_DATA  0x003FFFFF
#define SENS_TOUCH_PAD5_DATA_M  ((SENS_TOUCH_PAD5_DATA_V)<<(SENS_TOUCH_PAD5_DATA_S))
#define SENS_TOUCH_PAD5_DATA_V  0x3FFFFF
#define SENS_TOUCH_PAD5_DATA_S  0

#define SENS_SAR_TOUCH_STATUS6_REG          (DR_REG_SENS_BASE + 0x00f0)
/* SENS_TOUCH_PAD6_DEBOUNCE : RO ;bitpos:[31:29] ;default: 3'd0 ; */
/*description: */
#define SENS_TOUCH_PAD6_DEBOUNCE  0x00000007
#define SENS_TOUCH_PAD6_DEBOUNCE_M  ((SENS_TOUCH_PAD6_DEBOUNCE_V)<<(SENS_TOUCH_PAD6_DEBOUNCE_S))
#define SENS_TOUCH_PAD6_DEBOUNCE_V  0x7
#define SENS_TOUCH_PAD6_DEBOUNCE_S  29
/* SENS_TOUCH_PAD6_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define SENS_TOUCH_PAD6_DATA  0x003FFFFF
#define SENS_TOUCH_PAD6_DATA_M  ((SENS_TOUCH_PAD6_DATA_V)<<(SENS_TOUCH_PAD6_DATA_S))
#define SENS_TOUCH_PAD6_DATA_V  0x3FFFFF
#define SENS_TOUCH_PAD6_DATA_S  0

#define SENS_SAR_TOUCH_STATUS7_REG          (DR_REG_SENS_BASE + 0x00f4)
/* SENS_TOUCH_PAD7_DEBOUNCE : RO ;bitpos:[31:29] ;default: 3'd0 ; */
/*description: */
#define SENS_TOUCH_PAD7_DEBOUNCE  0x00000007
#define SENS_TOUCH_PAD7_DEBOUNCE_M  ((SENS_TOUCH_PAD7_DEBOUNCE_V)<<(SENS_TOUCH_PAD7_DEBOUNCE_S))
#define SENS_TOUCH_PAD7_DEBOUNCE_V  0x7
#define SENS_TOUCH_PAD7_DEBOUNCE_S  29
/* SENS_TOUCH_PAD7_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define SENS_TOUCH_PAD7_DATA  0x003FFFFF
#define SENS_TOUCH_PAD7_DATA_M  ((SENS_TOUCH_PAD7_DATA_V)<<(SENS_TOUCH_PAD7_DATA_S))
#define SENS_TOUCH_PAD7_DATA_V  0x3FFFFF
#define SENS_TOUCH_PAD7_DATA_S  0

#define SENS_SAR_TOUCH_STATUS8_REG          (DR_REG_SENS_BASE + 0x00f8)
/* SENS_TOUCH_PAD8_DEBOUNCE : RO ;bitpos:[31:29] ;default: 3'd0 ; */
/*description: */
#define SENS_TOUCH_PAD8_DEBOUNCE  0x00000007
#define SENS_TOUCH_PAD8_DEBOUNCE_M  ((SENS_TOUCH_PAD8_DEBOUNCE_V)<<(SENS_TOUCH_PAD8_DEBOUNCE_S))
#define SENS_TOUCH_PAD8_DEBOUNCE_V  0x7
#define SENS_TOUCH_PAD8_DEBOUNCE_S  29
/* SENS_TOUCH_PAD8_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define SENS_TOUCH_PAD8_DATA  0x003FFFFF
#define SENS_TOUCH_PAD8_DATA_M  ((SENS_TOUCH_PAD8_DATA_V)<<(SENS_TOUCH_PAD8_DATA_S))
#define SENS_TOUCH_PAD8_DATA_V  0x3FFFFF
#define SENS_TOUCH_PAD8_DATA_S  0

#define SENS_SAR_TOUCH_STATUS9_REG          (DR_REG_SENS_BASE + 0x00fc)
/* SENS_TOUCH_PAD9_DEBOUNCE : RO ;bitpos:[31:29] ;default: 3'd0 ; */
/*description: */
#define SENS_TOUCH_PAD9_DEBOUNCE  0x00000007
#define SENS_TOUCH_PAD9_DEBOUNCE_M  ((SENS_TOUCH_PAD9_DEBOUNCE_V)<<(SENS_TOUCH_PAD9_DEBOUNCE_S))
#define SENS_TOUCH_PAD9_DEBOUNCE_V  0x7
#define SENS_TOUCH_PAD9_DEBOUNCE_S  29
/* SENS_TOUCH_PAD9_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define SENS_TOUCH_PAD9_DATA  0x003FFFFF
#define SENS_TOUCH_PAD9_DATA_M  ((SENS_TOUCH_PAD9_DATA_V)<<(SENS_TOUCH_PAD9_DATA_S))
#define SENS_TOUCH_PAD9_DATA_V  0x3FFFFF
#define SENS_TOUCH_PAD9_DATA_S  0

#define SENS_SAR_TOUCH_STATUS10_REG          (DR_REG_SENS_BASE + 0x0100)
/* SENS_TOUCH_PAD10_DEBOUNCE : RO ;bitpos:[31:29] ;default: 3'd0 ; */
/*description: */
#define SENS_TOUCH_PAD10_DEBOUNCE  0x00000007
#define SENS_TOUCH_PAD10_DEBOUNCE_M  ((SENS_TOUCH_PAD10_DEBOUNCE_V)<<(SENS_TOUCH_PAD10_DEBOUNCE_S))
#define SENS_TOUCH_PAD10_DEBOUNCE_V  0x7
#define SENS_TOUCH_PAD10_DEBOUNCE_S  29
/* SENS_TOUCH_PAD10_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define SENS_TOUCH_PAD10_DATA  0x003FFFFF
#define SENS_TOUCH_PAD10_DATA_M  ((SENS_TOUCH_PAD10_DATA_V)<<(SENS_TOUCH_PAD10_DATA_S))
#define SENS_TOUCH_PAD10_DATA_V  0x3FFFFF
#define SENS_TOUCH_PAD10_DATA_S  0

#define SENS_SAR_TOUCH_STATUS11_REG          (DR_REG_SENS_BASE + 0x0104)
/* SENS_TOUCH_PAD11_DEBOUNCE : RO ;bitpos:[31:29] ;default: 3'd0 ; */
/*description: */
#define SENS_TOUCH_PAD11_DEBOUNCE  0x00000007
#define SENS_TOUCH_PAD11_DEBOUNCE_M  ((SENS_TOUCH_PAD11_DEBOUNCE_V)<<(SENS_TOUCH_PAD11_DEBOUNCE_S))
#define SENS_TOUCH_PAD11_DEBOUNCE_V  0x7
#define SENS_TOUCH_PAD11_DEBOUNCE_S  29
/* SENS_TOUCH_PAD11_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define SENS_TOUCH_PAD11_DATA  0x003FFFFF
#define SENS_TOUCH_PAD11_DATA_M  ((SENS_TOUCH_PAD11_DATA_V)<<(SENS_TOUCH_PAD11_DATA_S))
#define SENS_TOUCH_PAD11_DATA_V  0x3FFFFF
#define SENS_TOUCH_PAD11_DATA_S  0

#define SENS_SAR_TOUCH_STATUS12_REG          (DR_REG_SENS_BASE + 0x0108)
/* SENS_TOUCH_PAD12_DEBOUNCE : RO ;bitpos:[31:29] ;default: 3'd0 ; */
/*description: */
#define SENS_TOUCH_PAD12_DEBOUNCE  0x00000007
#define SENS_TOUCH_PAD12_DEBOUNCE_M  ((SENS_TOUCH_PAD12_DEBOUNCE_V)<<(SENS_TOUCH_PAD12_DEBOUNCE_S))
#define SENS_TOUCH_PAD12_DEBOUNCE_V  0x7
#define SENS_TOUCH_PAD12_DEBOUNCE_S  29
/* SENS_TOUCH_PAD12_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define SENS_TOUCH_PAD12_DATA  0x003FFFFF
#define SENS_TOUCH_PAD12_DATA_M  ((SENS_TOUCH_PAD12_DATA_V)<<(SENS_TOUCH_PAD12_DATA_S))
#define SENS_TOUCH_PAD12_DATA_V  0x3FFFFF
#define SENS_TOUCH_PAD12_DATA_S  0

#define SENS_SAR_TOUCH_STATUS13_REG          (DR_REG_SENS_BASE + 0x010c)
/* SENS_TOUCH_PAD13_DEBOUNCE : RO ;bitpos:[31:29] ;default: 3'd0 ; */
/*description: */
#define SENS_TOUCH_PAD13_DEBOUNCE  0x00000007
#define SENS_TOUCH_PAD13_DEBOUNCE_M  ((SENS_TOUCH_PAD13_DEBOUNCE_V)<<(SENS_TOUCH_PAD13_DEBOUNCE_S))
#define SENS_TOUCH_PAD13_DEBOUNCE_V  0x7
#define SENS_TOUCH_PAD13_DEBOUNCE_S  29
/* SENS_TOUCH_PAD13_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define SENS_TOUCH_PAD13_DATA  0x003FFFFF
#define SENS_TOUCH_PAD13_DATA_M  ((SENS_TOUCH_PAD13_DATA_V)<<(SENS_TOUCH_PAD13_DATA_S))
#define SENS_TOUCH_PAD13_DATA_V  0x3FFFFF
#define SENS_TOUCH_PAD13_DATA_S  0

#define SENS_SAR_TOUCH_STATUS14_REG          (DR_REG_SENS_BASE + 0x0110)
/* SENS_TOUCH_PAD14_DEBOUNCE : RO ;bitpos:[31:29] ;default: 3'd0 ; */
/*description: */
#define SENS_TOUCH_PAD14_DEBOUNCE  0x00000007
#define SENS_TOUCH_PAD14_DEBOUNCE_M  ((SENS_TOUCH_PAD14_DEBOUNCE_V)<<(SENS_TOUCH_PAD14_DEBOUNCE_S))
#define SENS_TOUCH_PAD14_DEBOUNCE_V  0x7
#define SENS_TOUCH_PAD14_DEBOUNCE_S  29
/* SENS_TOUCH_PAD14_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define SENS_TOUCH_PAD14_DATA  0x003FFFFF
#define SENS_TOUCH_PAD14_DATA_M  ((SENS_TOUCH_PAD14_DATA_V)<<(SENS_TOUCH_PAD14_DATA_S))
#define SENS_TOUCH_PAD14_DATA_V  0x3FFFFF
#define SENS_TOUCH_PAD14_DATA_S  0

#define SENS_SAR_TOUCH_SLP_STATUS_REG          (DR_REG_SENS_BASE + 0x0114)
/* SENS_TOUCH_SLP_DEBOUNCE : RO ;bitpos:[31:29] ;default: 3'd0 ; */
/*description: */
#define SENS_TOUCH_SLP_DEBOUNCE  0x00000007
#define SENS_TOUCH_SLP_DEBOUNCE_M  ((SENS_TOUCH_SLP_DEBOUNCE_V)<<(SENS_TOUCH_SLP_DEBOUNCE_S))
#define SENS_TOUCH_SLP_DEBOUNCE_V  0x7
#define SENS_TOUCH_SLP_DEBOUNCE_S  29
/* SENS_TOUCH_SLP_DATA : RO ;bitpos:[21:0] ;default: 22'h0 ; */
/*description: */
#define SENS_TOUCH_SLP_DATA  0x003FFFFF
#define SENS_TOUCH_SLP_DATA_M  ((SENS_TOUCH_SLP_DATA_V)<<(SENS_TOUCH_SLP_DATA_S))
#define SENS_TOUCH_SLP_DATA_V  0x3FFFFF
#define SENS_TOUCH_SLP_DATA_S  0

#define SENS_SAR_TOUCH_APPR_STATUS_REG          (DR_REG_SENS_BASE + 0x0118)
/* SENS_TOUCH_SLP_APPROACH_CNT : RO ;bitpos:[31:24] ;default: 8'd0 ; */
/*description: */
#define SENS_TOUCH_SLP_APPROACH_CNT  0x000000FF
#define SENS_TOUCH_SLP_APPROACH_CNT_M  ((SENS_TOUCH_SLP_APPROACH_CNT_V)<<(SENS_TOUCH_SLP_APPROACH_CNT_S))
#define SENS_TOUCH_SLP_APPROACH_CNT_V  0xFF
#define SENS_TOUCH_SLP_APPROACH_CNT_S  24
/* SENS_TOUCH_APPROACH_PAD0_CNT : RO ;bitpos:[23:16] ;default: 8'd0 ; */
/*description: */
#define SENS_TOUCH_APPROACH_PAD0_CNT  0x000000FF
#define SENS_TOUCH_APPROACH_PAD0_CNT_M  ((SENS_TOUCH_APPROACH_PAD0_CNT_V)<<(SENS_TOUCH_APPROACH_PAD0_CNT_S))
#define SENS_TOUCH_APPROACH_PAD0_CNT_V  0xFF
#define SENS_TOUCH_APPROACH_PAD0_CNT_S  16
/* SENS_TOUCH_APPROACH_PAD1_CNT : RO ;bitpos:[15:8] ;default: 8'd0 ; */
/*description: */
#define SENS_TOUCH_APPROACH_PAD1_CNT  0x000000FF
#define SENS_TOUCH_APPROACH_PAD1_CNT_M  ((SENS_TOUCH_APPROACH_PAD1_CNT_V)<<(SENS_TOUCH_APPROACH_PAD1_CNT_S))
#define SENS_TOUCH_APPROACH_PAD1_CNT_V  0xFF
#define SENS_TOUCH_APPROACH_PAD1_CNT_S  8
/* SENS_TOUCH_APPROACH_PAD2_CNT : RO ;bitpos:[7:0] ;default: 8'd0 ; */
/*description: */
#define SENS_TOUCH_APPROACH_PAD2_CNT  0x000000FF
#define SENS_TOUCH_APPROACH_PAD2_CNT_M  ((SENS_TOUCH_APPROACH_PAD2_CNT_V)<<(SENS_TOUCH_APPROACH_PAD2_CNT_S))
#define SENS_TOUCH_APPROACH_PAD2_CNT_V  0xFF
#define SENS_TOUCH_APPROACH_PAD2_CNT_S  0

#define SENS_SAR_DAC_CTRL1_REG          (DR_REG_SENS_BASE + 0x011c)
/* SENS_DAC_CLKGATE_EN : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define SENS_DAC_CLKGATE_EN  (BIT(27))
#define SENS_DAC_CLKGATE_EN_M  (BIT(27))
#define SENS_DAC_CLKGATE_EN_V  0x1
#define SENS_DAC_CLKGATE_EN_S  27
/* SENS_DAC_RESET : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define SENS_DAC_RESET  (BIT(26))
#define SENS_DAC_RESET_M  (BIT(26))
#define SENS_DAC_RESET_V  0x1
#define SENS_DAC_RESET_S  26
/* SENS_DAC_CLK_INV : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: 1: invert PDAC_CLK*/
#define SENS_DAC_CLK_INV  (BIT(25))
#define SENS_DAC_CLK_INV_M  (BIT(25))
#define SENS_DAC_CLK_INV_V  0x1
#define SENS_DAC_CLK_INV_S  25
/* SENS_DAC_CLK_FORCE_HIGH : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: 1: force PDAC_CLK to high*/
#define SENS_DAC_CLK_FORCE_HIGH  (BIT(24))
#define SENS_DAC_CLK_FORCE_HIGH_M  (BIT(24))
#define SENS_DAC_CLK_FORCE_HIGH_V  0x1
#define SENS_DAC_CLK_FORCE_HIGH_S  24
/* SENS_DAC_CLK_FORCE_LOW : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: 1: force PDAC_CLK to low*/
#define SENS_DAC_CLK_FORCE_LOW  (BIT(23))
#define SENS_DAC_CLK_FORCE_LOW_M  (BIT(23))
#define SENS_DAC_CLK_FORCE_LOW_V  0x1
#define SENS_DAC_CLK_FORCE_LOW_S  23
/* SENS_DAC_DIG_FORCE : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: 1: DAC1 & DAC2 use DMA*/
#define SENS_DAC_DIG_FORCE  (BIT(22))
#define SENS_DAC_DIG_FORCE_M  (BIT(22))
#define SENS_DAC_DIG_FORCE_V  0x1
#define SENS_DAC_DIG_FORCE_S  22
/* SENS_DEBUG_BIT_SEL : R/W ;bitpos:[21:17] ;default: 5'b0 ; */
/*description: */
#define SENS_DEBUG_BIT_SEL  0x0000001F
#define SENS_DEBUG_BIT_SEL_M  ((SENS_DEBUG_BIT_SEL_V)<<(SENS_DEBUG_BIT_SEL_S))
#define SENS_DEBUG_BIT_SEL_V  0x1F
#define SENS_DEBUG_BIT_SEL_S  17
/* SENS_SW_TONE_EN : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: 1: enable CW generator*/
#define SENS_SW_TONE_EN  (BIT(16))
#define SENS_SW_TONE_EN_M  (BIT(16))
#define SENS_SW_TONE_EN_V  0x1
#define SENS_SW_TONE_EN_S  16
/* SENS_SW_FSTEP : R/W ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: frequency step for CW generator*/
#define SENS_SW_FSTEP  0x0000FFFF
#define SENS_SW_FSTEP_M  ((SENS_SW_FSTEP_V)<<(SENS_SW_FSTEP_S))
#define SENS_SW_FSTEP_V  0xFFFF
#define SENS_SW_FSTEP_S  0

#define SENS_SAR_DAC_CTRL2_REG          (DR_REG_SENS_BASE + 0x0120)
/* SENS_DAC_CW_EN2 : R/W ;bitpos:[25] ;default: 1'b1 ; */
/*description: 1: to select CW generator as source to PDAC2_DAC[7:0]*/
#define SENS_DAC_CW_EN2  (BIT(25))
#define SENS_DAC_CW_EN2_M  (BIT(25))
#define SENS_DAC_CW_EN2_V  0x1
#define SENS_DAC_CW_EN2_S  25
/* SENS_DAC_CW_EN1 : R/W ;bitpos:[24] ;default: 1'b1 ; */
/*description: 1: to select CW generator as source to PDAC1_DAC[7:0]*/
#define SENS_DAC_CW_EN1  (BIT(24))
#define SENS_DAC_CW_EN1_M  (BIT(24))
#define SENS_DAC_CW_EN1_V  0x1
#define SENS_DAC_CW_EN1_S  24
/* SENS_DAC_INV2 : R/W ;bitpos:[23:22] ;default: 2'b0 ; */
/*description: 00: do not invert any bits*/
#define SENS_DAC_INV2  0x00000003
#define SENS_DAC_INV2_M  ((SENS_DAC_INV2_V)<<(SENS_DAC_INV2_S))
#define SENS_DAC_INV2_V  0x3
#define SENS_DAC_INV2_S  22
/* SENS_DAC_INV1 : R/W ;bitpos:[21:20] ;default: 2'b0 ; */
/*description: 00: do not invert any bits*/
#define SENS_DAC_INV1  0x00000003
#define SENS_DAC_INV1_M  ((SENS_DAC_INV1_V)<<(SENS_DAC_INV1_S))
#define SENS_DAC_INV1_V  0x3
#define SENS_DAC_INV1_S  20
/* SENS_DAC_SCALE2 : R/W ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: 00: no scale*/
#define SENS_DAC_SCALE2  0x00000003
#define SENS_DAC_SCALE2_M  ((SENS_DAC_SCALE2_V)<<(SENS_DAC_SCALE2_S))
#define SENS_DAC_SCALE2_V  0x3
#define SENS_DAC_SCALE2_S  18
/* SENS_DAC_SCALE1 : R/W ;bitpos:[17:16] ;default: 2'b0 ; */
/*description: 00: no scale*/
#define SENS_DAC_SCALE1  0x00000003
#define SENS_DAC_SCALE1_M  ((SENS_DAC_SCALE1_V)<<(SENS_DAC_SCALE1_S))
#define SENS_DAC_SCALE1_V  0x3
#define SENS_DAC_SCALE1_S  16
/* SENS_DAC_DC2 : R/W ;bitpos:[15:8] ;default: 8'b0 ; */
/*description: DC offset for DAC2 CW generator*/
#define SENS_DAC_DC2  0x000000FF
#define SENS_DAC_DC2_M  ((SENS_DAC_DC2_V)<<(SENS_DAC_DC2_S))
#define SENS_DAC_DC2_V  0xFF
#define SENS_DAC_DC2_S  8
/* SENS_DAC_DC1 : R/W ;bitpos:[7:0] ;default: 8'b0 ; */
/*description: DC offset for DAC1 CW generator*/
#define SENS_DAC_DC1  0x000000FF
#define SENS_DAC_DC1_M  ((SENS_DAC_DC1_V)<<(SENS_DAC_DC1_S))
#define SENS_DAC_DC1_V  0xFF
#define SENS_DAC_DC1_S  0

#define SENS_SAR_COCPU_STATE_REG          (DR_REG_SENS_BASE + 0x0124)
/* SENS_COCPU_EBREAK : RO ;bitpos:[30] ;default: 1'b0 ; */
/*description: check cocpu whether in ebreak*/
#define SENS_COCPU_EBREAK  (BIT(30))
#define SENS_COCPU_EBREAK_M  (BIT(30))
#define SENS_COCPU_EBREAK_V  0x1
#define SENS_COCPU_EBREAK_S  30
/* SENS_COCPU_TRAP : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: check cocpu whether in trap state*/
#define SENS_COCPU_TRAP  (BIT(29))
#define SENS_COCPU_TRAP_M  (BIT(29))
#define SENS_COCPU_TRAP_V  0x1
#define SENS_COCPU_TRAP_S  29
/* SENS_COCPU_EOI : RO ;bitpos:[28] ;default: 1'b0 ; */
/*description: check cocpu whether in interrupt state*/
#define SENS_COCPU_EOI  (BIT(28))
#define SENS_COCPU_EOI_M  (BIT(28))
#define SENS_COCPU_EOI_V  0x1
#define SENS_COCPU_EOI_S  28
/* SENS_COCPU_RESET_N : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: check cocpu whether in reset state*/
#define SENS_COCPU_RESET_N  (BIT(27))
#define SENS_COCPU_RESET_N_M  (BIT(27))
#define SENS_COCPU_RESET_N_V  0x1
#define SENS_COCPU_RESET_N_S  27
/* SENS_COCPU_CLK_EN : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: check cocpu whether clk on*/
#define SENS_COCPU_CLK_EN  (BIT(26))
#define SENS_COCPU_CLK_EN_M  (BIT(26))
#define SENS_COCPU_CLK_EN_V  0x1
#define SENS_COCPU_CLK_EN_S  26
/* SENS_COCPU_DBG_TRIGGER : WO ;bitpos:[25] ;default: 1'b0 ; */
/*description: trigger cocpu debug registers*/
#define SENS_COCPU_DBG_TRIGGER  (BIT(25))
#define SENS_COCPU_DBG_TRIGGER_M  (BIT(25))
#define SENS_COCPU_DBG_TRIGGER_V  0x1
#define SENS_COCPU_DBG_TRIGGER_S  25

#define SENS_SAR_COCPU_INT_RAW_REG          (DR_REG_SENS_BASE + 0x0128)
/* SENS_COCPU_SWD_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: int from super watch dog*/
#define SENS_COCPU_SWD_INT_RAW  (BIT(8))
#define SENS_COCPU_SWD_INT_RAW_M  (BIT(8))
#define SENS_COCPU_SWD_INT_RAW_V  0x1
#define SENS_COCPU_SWD_INT_RAW_S  8
/* SENS_COCPU_SW_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: int from software*/
#define SENS_COCPU_SW_INT_RAW  (BIT(7))
#define SENS_COCPU_SW_INT_RAW_M  (BIT(7))
#define SENS_COCPU_SW_INT_RAW_V  0x1
#define SENS_COCPU_SW_INT_RAW_S  7
/* SENS_COCPU_START_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: int from start*/
#define SENS_COCPU_START_INT_RAW  (BIT(6))
#define SENS_COCPU_START_INT_RAW_M  (BIT(6))
#define SENS_COCPU_START_INT_RAW_V  0x1
#define SENS_COCPU_START_INT_RAW_S  6
/* SENS_COCPU_TSENS_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: int from tsens*/
#define SENS_COCPU_TSENS_INT_RAW  (BIT(5))
#define SENS_COCPU_TSENS_INT_RAW_M  (BIT(5))
#define SENS_COCPU_TSENS_INT_RAW_V  0x1
#define SENS_COCPU_TSENS_INT_RAW_S  5
/* SENS_COCPU_SENS2_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: int from saradc2*/
#define SENS_COCPU_SENS2_INT_RAW  (BIT(4))
#define SENS_COCPU_SENS2_INT_RAW_M  (BIT(4))
#define SENS_COCPU_SENS2_INT_RAW_V  0x1
#define SENS_COCPU_SENS2_INT_RAW_S  4
/* SENS_COCPU_SENS1_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: int from saradc1*/
#define SENS_COCPU_SENS1_INT_RAW  (BIT(3))
#define SENS_COCPU_SENS1_INT_RAW_M  (BIT(3))
#define SENS_COCPU_SENS1_INT_RAW_V  0x1
#define SENS_COCPU_SENS1_INT_RAW_S  3
/* SENS_COCPU_TOUCH_ACTIVE_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: int from touch active*/
#define SENS_COCPU_TOUCH_ACTIVE_INT_RAW  (BIT(2))
#define SENS_COCPU_TOUCH_ACTIVE_INT_RAW_M  (BIT(2))
#define SENS_COCPU_TOUCH_ACTIVE_INT_RAW_V  0x1
#define SENS_COCPU_TOUCH_ACTIVE_INT_RAW_S  2
/* SENS_COCPU_TOUCH_INACTIVE_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: int from touch inactive*/
#define SENS_COCPU_TOUCH_INACTIVE_INT_RAW  (BIT(1))
#define SENS_COCPU_TOUCH_INACTIVE_INT_RAW_M  (BIT(1))
#define SENS_COCPU_TOUCH_INACTIVE_INT_RAW_V  0x1
#define SENS_COCPU_TOUCH_INACTIVE_INT_RAW_S  1
/* SENS_COCPU_TOUCH_DONE_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: int from touch done*/
#define SENS_COCPU_TOUCH_DONE_INT_RAW  (BIT(0))
#define SENS_COCPU_TOUCH_DONE_INT_RAW_M  (BIT(0))
#define SENS_COCPU_TOUCH_DONE_INT_RAW_V  0x1
#define SENS_COCPU_TOUCH_DONE_INT_RAW_S  0

#define SENS_SAR_COCPU_INT_ENA_REG          (DR_REG_SENS_BASE + 0x012c)
/* SENS_COCPU_SWD_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_SWD_INT_ENA  (BIT(8))
#define SENS_COCPU_SWD_INT_ENA_M  (BIT(8))
#define SENS_COCPU_SWD_INT_ENA_V  0x1
#define SENS_COCPU_SWD_INT_ENA_S  8
/* SENS_COCPU_SW_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: cocpu int enable*/
#define SENS_COCPU_SW_INT_ENA  (BIT(7))
#define SENS_COCPU_SW_INT_ENA_M  (BIT(7))
#define SENS_COCPU_SW_INT_ENA_V  0x1
#define SENS_COCPU_SW_INT_ENA_S  7
/* SENS_COCPU_START_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_START_INT_ENA  (BIT(6))
#define SENS_COCPU_START_INT_ENA_M  (BIT(6))
#define SENS_COCPU_START_INT_ENA_V  0x1
#define SENS_COCPU_START_INT_ENA_S  6
/* SENS_COCPU_TSENS_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_TSENS_INT_ENA  (BIT(5))
#define SENS_COCPU_TSENS_INT_ENA_M  (BIT(5))
#define SENS_COCPU_TSENS_INT_ENA_V  0x1
#define SENS_COCPU_TSENS_INT_ENA_S  5
/* SENS_COCPU_SENS2_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_SENS2_INT_ENA  (BIT(4))
#define SENS_COCPU_SENS2_INT_ENA_M  (BIT(4))
#define SENS_COCPU_SENS2_INT_ENA_V  0x1
#define SENS_COCPU_SENS2_INT_ENA_S  4
/* SENS_COCPU_SENS1_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_SENS1_INT_ENA  (BIT(3))
#define SENS_COCPU_SENS1_INT_ENA_M  (BIT(3))
#define SENS_COCPU_SENS1_INT_ENA_V  0x1
#define SENS_COCPU_SENS1_INT_ENA_S  3
/* SENS_COCPU_TOUCH_ACTIVE_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_TOUCH_ACTIVE_INT_ENA  (BIT(2))
#define SENS_COCPU_TOUCH_ACTIVE_INT_ENA_M  (BIT(2))
#define SENS_COCPU_TOUCH_ACTIVE_INT_ENA_V  0x1
#define SENS_COCPU_TOUCH_ACTIVE_INT_ENA_S  2
/* SENS_COCPU_TOUCH_INACTIVE_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_TOUCH_INACTIVE_INT_ENA  (BIT(1))
#define SENS_COCPU_TOUCH_INACTIVE_INT_ENA_M  (BIT(1))
#define SENS_COCPU_TOUCH_INACTIVE_INT_ENA_V  0x1
#define SENS_COCPU_TOUCH_INACTIVE_INT_ENA_S  1
/* SENS_COCPU_TOUCH_DONE_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_TOUCH_DONE_INT_ENA  (BIT(0))
#define SENS_COCPU_TOUCH_DONE_INT_ENA_M  (BIT(0))
#define SENS_COCPU_TOUCH_DONE_INT_ENA_V  0x1
#define SENS_COCPU_TOUCH_DONE_INT_ENA_S  0

#define SENS_SAR_COCPU_INT_ST_REG          (DR_REG_SENS_BASE + 0x0130)
/* SENS_COCPU_SWD_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_SWD_INT_ST  (BIT(8))
#define SENS_COCPU_SWD_INT_ST_M  (BIT(8))
#define SENS_COCPU_SWD_INT_ST_V  0x1
#define SENS_COCPU_SWD_INT_ST_S  8
/* SENS_COCPU_SW_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: cocpu int status*/
#define SENS_COCPU_SW_INT_ST  (BIT(7))
#define SENS_COCPU_SW_INT_ST_M  (BIT(7))
#define SENS_COCPU_SW_INT_ST_V  0x1
#define SENS_COCPU_SW_INT_ST_S  7
/* SENS_COCPU_START_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_START_INT_ST  (BIT(6))
#define SENS_COCPU_START_INT_ST_M  (BIT(6))
#define SENS_COCPU_START_INT_ST_V  0x1
#define SENS_COCPU_START_INT_ST_S  6
/* SENS_COCPU_TSENS_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_TSENS_INT_ST  (BIT(5))
#define SENS_COCPU_TSENS_INT_ST_M  (BIT(5))
#define SENS_COCPU_TSENS_INT_ST_V  0x1
#define SENS_COCPU_TSENS_INT_ST_S  5
/* SENS_COCPU_SENS2_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_SENS2_INT_ST  (BIT(4))
#define SENS_COCPU_SENS2_INT_ST_M  (BIT(4))
#define SENS_COCPU_SENS2_INT_ST_V  0x1
#define SENS_COCPU_SENS2_INT_ST_S  4
/* SENS_COCPU_SENS1_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_SENS1_INT_ST  (BIT(3))
#define SENS_COCPU_SENS1_INT_ST_M  (BIT(3))
#define SENS_COCPU_SENS1_INT_ST_V  0x1
#define SENS_COCPU_SENS1_INT_ST_S  3
/* SENS_COCPU_TOUCH_ACTIVE_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_TOUCH_ACTIVE_INT_ST  (BIT(2))
#define SENS_COCPU_TOUCH_ACTIVE_INT_ST_M  (BIT(2))
#define SENS_COCPU_TOUCH_ACTIVE_INT_ST_V  0x1
#define SENS_COCPU_TOUCH_ACTIVE_INT_ST_S  2
/* SENS_COCPU_TOUCH_INACTIVE_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_TOUCH_INACTIVE_INT_ST  (BIT(1))
#define SENS_COCPU_TOUCH_INACTIVE_INT_ST_M  (BIT(1))
#define SENS_COCPU_TOUCH_INACTIVE_INT_ST_V  0x1
#define SENS_COCPU_TOUCH_INACTIVE_INT_ST_S  1
/* SENS_COCPU_TOUCH_DONE_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_TOUCH_DONE_INT_ST  (BIT(0))
#define SENS_COCPU_TOUCH_DONE_INT_ST_M  (BIT(0))
#define SENS_COCPU_TOUCH_DONE_INT_ST_V  0x1
#define SENS_COCPU_TOUCH_DONE_INT_ST_S  0

#define SENS_SAR_COCPU_INT_CLR_REG          (DR_REG_SENS_BASE + 0x0134)
/* SENS_COCPU_SWD_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_SWD_INT_CLR  (BIT(8))
#define SENS_COCPU_SWD_INT_CLR_M  (BIT(8))
#define SENS_COCPU_SWD_INT_CLR_V  0x1
#define SENS_COCPU_SWD_INT_CLR_S  8
/* SENS_COCPU_SW_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: cocpu int clear*/
#define SENS_COCPU_SW_INT_CLR  (BIT(7))
#define SENS_COCPU_SW_INT_CLR_M  (BIT(7))
#define SENS_COCPU_SW_INT_CLR_V  0x1
#define SENS_COCPU_SW_INT_CLR_S  7
/* SENS_COCPU_START_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_START_INT_CLR  (BIT(6))
#define SENS_COCPU_START_INT_CLR_M  (BIT(6))
#define SENS_COCPU_START_INT_CLR_V  0x1
#define SENS_COCPU_START_INT_CLR_S  6
/* SENS_COCPU_TSENS_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_TSENS_INT_CLR  (BIT(5))
#define SENS_COCPU_TSENS_INT_CLR_M  (BIT(5))
#define SENS_COCPU_TSENS_INT_CLR_V  0x1
#define SENS_COCPU_TSENS_INT_CLR_S  5
/* SENS_COCPU_SENS2_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_SENS2_INT_CLR  (BIT(4))
#define SENS_COCPU_SENS2_INT_CLR_M  (BIT(4))
#define SENS_COCPU_SENS2_INT_CLR_V  0x1
#define SENS_COCPU_SENS2_INT_CLR_S  4
/* SENS_COCPU_SENS1_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_SENS1_INT_CLR  (BIT(3))
#define SENS_COCPU_SENS1_INT_CLR_M  (BIT(3))
#define SENS_COCPU_SENS1_INT_CLR_V  0x1
#define SENS_COCPU_SENS1_INT_CLR_S  3
/* SENS_COCPU_TOUCH_ACTIVE_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_TOUCH_ACTIVE_INT_CLR  (BIT(2))
#define SENS_COCPU_TOUCH_ACTIVE_INT_CLR_M  (BIT(2))
#define SENS_COCPU_TOUCH_ACTIVE_INT_CLR_V  0x1
#define SENS_COCPU_TOUCH_ACTIVE_INT_CLR_S  2
/* SENS_COCPU_TOUCH_INACTIVE_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_TOUCH_INACTIVE_INT_CLR  (BIT(1))
#define SENS_COCPU_TOUCH_INACTIVE_INT_CLR_M  (BIT(1))
#define SENS_COCPU_TOUCH_INACTIVE_INT_CLR_V  0x1
#define SENS_COCPU_TOUCH_INACTIVE_INT_CLR_S  1
/* SENS_COCPU_TOUCH_DONE_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SENS_COCPU_TOUCH_DONE_INT_CLR  (BIT(0))
#define SENS_COCPU_TOUCH_DONE_INT_CLR_M  (BIT(0))
#define SENS_COCPU_TOUCH_DONE_INT_CLR_V  0x1
#define SENS_COCPU_TOUCH_DONE_INT_CLR_S  0

#define SENS_SAR_COCPU_DEBUG_REG          (DR_REG_SENS_BASE + 0x0138)
/* SENS_COCPU_MEM_ADDR : RO ;bitpos:[31:19] ;default: 13'd0 ; */
/*description: cocpu mem address output*/
#define SENS_COCPU_MEM_ADDR  0x00001FFF
#define SENS_COCPU_MEM_ADDR_M  ((SENS_COCPU_MEM_ADDR_V)<<(SENS_COCPU_MEM_ADDR_S))
#define SENS_COCPU_MEM_ADDR_V  0x1FFF
#define SENS_COCPU_MEM_ADDR_S  19
/* SENS_COCPU_MEM_WEN : RO ;bitpos:[18:15] ;default: 4'd0 ; */
/*description: cocpu mem write enable output*/
#define SENS_COCPU_MEM_WEN  0x0000000F
#define SENS_COCPU_MEM_WEN_M  ((SENS_COCPU_MEM_WEN_V)<<(SENS_COCPU_MEM_WEN_S))
#define SENS_COCPU_MEM_WEN_V  0xF
#define SENS_COCPU_MEM_WEN_S  15
/* SENS_COCPU_MEM_RDY : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: cocpu mem ready input*/
#define SENS_COCPU_MEM_RDY  (BIT(14))
#define SENS_COCPU_MEM_RDY_M  (BIT(14))
#define SENS_COCPU_MEM_RDY_V  0x1
#define SENS_COCPU_MEM_RDY_S  14
/* SENS_COCPU_MEM_VLD : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: cocpu mem valid output*/
#define SENS_COCPU_MEM_VLD  (BIT(13))
#define SENS_COCPU_MEM_VLD_M  (BIT(13))
#define SENS_COCPU_MEM_VLD_V  0x1
#define SENS_COCPU_MEM_VLD_S  13
/* SENS_COCPU_PC : RO ;bitpos:[12:0] ;default: 13'd0 ; */
/*description: cocpu Program counter*/
#define SENS_COCPU_PC  0x00001FFF
#define SENS_COCPU_PC_M  ((SENS_COCPU_PC_V)<<(SENS_COCPU_PC_S))
#define SENS_COCPU_PC_V  0x1FFF
#define SENS_COCPU_PC_S  0

#define SENS_SAR_HALL_CTRL_REG          (DR_REG_SENS_BASE + 0x013c)
/* SENS_HALL_PHASE_FORCE : R/W ;bitpos:[31] ;default: 1'b1 ; */
/*description: 1: HALL PHASE is controlled by SW  0: HALL PHASE is controlled
 by FSM in ULP-coprocessor*/
#define SENS_HALL_PHASE_FORCE  (BIT(31))
#define SENS_HALL_PHASE_FORCE_M  (BIT(31))
#define SENS_HALL_PHASE_FORCE_V  0x1
#define SENS_HALL_PHASE_FORCE_S  31
/* SENS_HALL_PHASE : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: Reverse phase of hall sensor*/
#define SENS_HALL_PHASE  (BIT(30))
#define SENS_HALL_PHASE_M  (BIT(30))
#define SENS_HALL_PHASE_V  0x1
#define SENS_HALL_PHASE_S  30
/* SENS_XPD_HALL_FORCE : R/W ;bitpos:[29] ;default: 1'b1 ; */
/*description: 1: XPD HALL is controlled by SW. 0: XPD HALL is controlled by
 FSM in ULP-coprocessor*/
#define SENS_XPD_HALL_FORCE  (BIT(29))
#define SENS_XPD_HALL_FORCE_M  (BIT(29))
#define SENS_XPD_HALL_FORCE_V  0x1
#define SENS_XPD_HALL_FORCE_S  29
/* SENS_XPD_HALL : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: Power on hall sensor and connect to VP and VN*/
#define SENS_XPD_HALL  (BIT(28))
#define SENS_XPD_HALL_M  (BIT(28))
#define SENS_XPD_HALL_V  0x1
#define SENS_XPD_HALL_S  28

#define SENS_SAR_NOUSE_REG          (DR_REG_SENS_BASE + 0x0140)
/* SENS_SAR_NOUSE : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SENS_SAR_NOUSE  0xFFFFFFFF
#define SENS_SAR_NOUSE_M  ((SENS_SAR_NOUSE_V)<<(SENS_SAR_NOUSE_S))
#define SENS_SAR_NOUSE_V  0xFFFFFFFF
#define SENS_SAR_NOUSE_S  0

#define SENS_SAR_IO_MUX_CONF_REG          (DR_REG_SENS_BASE + 0x0144)
/* SENS_IOMUX_CLK_GATE_EN : R/W ;bitpos:[31] ;default: 1'd0 ; */
/*description: */
#define SENS_IOMUX_CLK_GATE_EN  (BIT(31))
#define SENS_IOMUX_CLK_GATE_EN_M  (BIT(31))
#define SENS_IOMUX_CLK_GATE_EN_V  0x1
#define SENS_IOMUX_CLK_GATE_EN_S  31
/* SENS_IOMUX_RESET : R/W ;bitpos:[30] ;default: 1'b0 ; */
/*description: */
#define SENS_IOMUX_RESET  (BIT(30))
#define SENS_IOMUX_RESET_M  (BIT(30))
#define SENS_IOMUX_RESET_V  0x1
#define SENS_IOMUX_RESET_S  30

#define SENS_SARDATE_REG          (DR_REG_SENS_BASE + 0x0148)
/* SENS_SAR_DATE : R/W ;bitpos:[27:0] ;default: 28'h1906140 ; */
/*description: */
#define SENS_SAR_DATE  0x0FFFFFFF
#define SENS_SAR_DATE_M  ((SENS_SAR_DATE_V)<<(SENS_SAR_DATE_S))
#define SENS_SAR_DATE_V  0xFFFFFFF
#define SENS_SAR_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_SENS_REG_H_ */
