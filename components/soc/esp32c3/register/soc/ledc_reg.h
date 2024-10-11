/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_LEDC_REG_H_
#define _SOC_LEDC_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc/soc.h"
#define LEDC_LSCH0_CONF0_REG          (DR_REG_LEDC_BASE + 0x0000)
/* LEDC_OVF_CNT_RESET_LSCH0 : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_RESET_LSCH0  (BIT(16))
#define LEDC_OVF_CNT_RESET_LSCH0_M  (BIT(16))
#define LEDC_OVF_CNT_RESET_LSCH0_V  0x1
#define LEDC_OVF_CNT_RESET_LSCH0_S  16
/* LEDC_OVF_CNT_EN_LSCH0 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_EN_LSCH0  (BIT(15))
#define LEDC_OVF_CNT_EN_LSCH0_M  (BIT(15))
#define LEDC_OVF_CNT_EN_LSCH0_V  0x1
#define LEDC_OVF_CNT_EN_LSCH0_S  15
/* LEDC_OVF_NUM_LSCH0 : R/W ;bitpos:[14:5] ;default: 10'b0 ; */
/*description: */
#define LEDC_OVF_NUM_LSCH0  0x000003FF
#define LEDC_OVF_NUM_LSCH0_M  ((LEDC_OVF_NUM_LSCH0_V)<<(LEDC_OVF_NUM_LSCH0_S))
#define LEDC_OVF_NUM_LSCH0_V  0x3FF
#define LEDC_OVF_NUM_LSCH0_S  5
/* LEDC_PARA_UP_LSCH0 : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define LEDC_PARA_UP_LSCH0  (BIT(4))
#define LEDC_PARA_UP_LSCH0_M  (BIT(4))
#define LEDC_PARA_UP_LSCH0_V  0x1
#define LEDC_PARA_UP_LSCH0_S  4
/* LEDC_IDLE_LV_LSCH0 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define LEDC_IDLE_LV_LSCH0  (BIT(3))
#define LEDC_IDLE_LV_LSCH0_M  (BIT(3))
#define LEDC_IDLE_LV_LSCH0_V  0x1
#define LEDC_IDLE_LV_LSCH0_S  3
/* LEDC_SIG_OUT_EN_LSCH0 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define LEDC_SIG_OUT_EN_LSCH0  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH0_M  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH0_V  0x1
#define LEDC_SIG_OUT_EN_LSCH0_S  2
/* LEDC_TIMER_SEL_LSCH0 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: */
#define LEDC_TIMER_SEL_LSCH0  0x00000003
#define LEDC_TIMER_SEL_LSCH0_M  ((LEDC_TIMER_SEL_LSCH0_V)<<(LEDC_TIMER_SEL_LSCH0_S))
#define LEDC_TIMER_SEL_LSCH0_V  0x3
#define LEDC_TIMER_SEL_LSCH0_S  0

#define LEDC_LSCH0_HPOINT_REG          (DR_REG_LEDC_BASE + 0x0004)
/* LEDC_HPOINT_LSCH0 : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: */
#define LEDC_HPOINT_LSCH0  0x00003FFF
#define LEDC_HPOINT_LSCH0_M  ((LEDC_HPOINT_LSCH0_V)<<(LEDC_HPOINT_LSCH0_S))
#define LEDC_HPOINT_LSCH0_V  0x3FFF
#define LEDC_HPOINT_LSCH0_S  0

#define LEDC_LSCH0_DUTY_REG          (DR_REG_LEDC_BASE + 0x0008)
/* LEDC_DUTY_LSCH0 : R/W ;bitpos:[18:0] ;default: 19'h0 ; */
/*description: */
#define LEDC_DUTY_LSCH0  0x0007FFFF
#define LEDC_DUTY_LSCH0_M  ((LEDC_DUTY_LSCH0_V)<<(LEDC_DUTY_LSCH0_S))
#define LEDC_DUTY_LSCH0_V  0x7FFFF
#define LEDC_DUTY_LSCH0_S  0

#define LEDC_LSCH0_CONF1_REG          (DR_REG_LEDC_BASE + 0x000C)
/* LEDC_DUTY_START_LSCH0 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_START_LSCH0  (BIT(31))
#define LEDC_DUTY_START_LSCH0_M  (BIT(31))
#define LEDC_DUTY_START_LSCH0_V  0x1
#define LEDC_DUTY_START_LSCH0_S  31
/* LEDC_DUTY_INC_LSCH0 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: */
#define LEDC_DUTY_INC_LSCH0  (BIT(30))
#define LEDC_DUTY_INC_LSCH0_M  (BIT(30))
#define LEDC_DUTY_INC_LSCH0_V  0x1
#define LEDC_DUTY_INC_LSCH0_S  30
/* LEDC_DUTY_NUM_LSCH0 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_NUM_LSCH0  0x000003FF
#define LEDC_DUTY_NUM_LSCH0_M  ((LEDC_DUTY_NUM_LSCH0_V)<<(LEDC_DUTY_NUM_LSCH0_S))
#define LEDC_DUTY_NUM_LSCH0_V  0x3FF
#define LEDC_DUTY_NUM_LSCH0_S  20
/* LEDC_DUTY_CYCLE_LSCH0 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_CYCLE_LSCH0  0x000003FF
#define LEDC_DUTY_CYCLE_LSCH0_M  ((LEDC_DUTY_CYCLE_LSCH0_V)<<(LEDC_DUTY_CYCLE_LSCH0_S))
#define LEDC_DUTY_CYCLE_LSCH0_V  0x3FF
#define LEDC_DUTY_CYCLE_LSCH0_S  10
/* LEDC_DUTY_SCALE_LSCH0 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_SCALE_LSCH0  0x000003FF
#define LEDC_DUTY_SCALE_LSCH0_M  ((LEDC_DUTY_SCALE_LSCH0_V)<<(LEDC_DUTY_SCALE_LSCH0_S))
#define LEDC_DUTY_SCALE_LSCH0_V  0x3FF
#define LEDC_DUTY_SCALE_LSCH0_S  0

#define LEDC_LSCH0_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x0010)
/* LEDC_DUTY_LSCH0 : RO ;bitpos:[18:0] ;default: 19'h0 ; */
/*description: */
#define LEDC_DUTY_LSCH0  0x0007FFFF
#define LEDC_DUTY_LSCH0_M  ((LEDC_DUTY_LSCH0_V)<<(LEDC_DUTY_LSCH0_S))
#define LEDC_DUTY_LSCH0_V  0x7FFFF
#define LEDC_DUTY_LSCH0_S  0

#define LEDC_LSCH1_CONF0_REG          (DR_REG_LEDC_BASE + 0x0014)
/* LEDC_OVF_CNT_RESET_LSCH1 : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_RESET_LSCH1  (BIT(16))
#define LEDC_OVF_CNT_RESET_LSCH1_M  (BIT(16))
#define LEDC_OVF_CNT_RESET_LSCH1_V  0x1
#define LEDC_OVF_CNT_RESET_LSCH1_S  16
/* LEDC_OVF_CNT_EN_LSCH1 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_EN_LSCH1  (BIT(15))
#define LEDC_OVF_CNT_EN_LSCH1_M  (BIT(15))
#define LEDC_OVF_CNT_EN_LSCH1_V  0x1
#define LEDC_OVF_CNT_EN_LSCH1_S  15
/* LEDC_OVF_NUM_LSCH1 : R/W ;bitpos:[14:5] ;default: 10'b0 ; */
/*description: */
#define LEDC_OVF_NUM_LSCH1  0x000003FF
#define LEDC_OVF_NUM_LSCH1_M  ((LEDC_OVF_NUM_LSCH1_V)<<(LEDC_OVF_NUM_LSCH1_S))
#define LEDC_OVF_NUM_LSCH1_V  0x3FF
#define LEDC_OVF_NUM_LSCH1_S  5
/* LEDC_PARA_UP_LSCH1 : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define LEDC_PARA_UP_LSCH1  (BIT(4))
#define LEDC_PARA_UP_LSCH1_M  (BIT(4))
#define LEDC_PARA_UP_LSCH1_V  0x1
#define LEDC_PARA_UP_LSCH1_S  4
/* LEDC_IDLE_LV_LSCH1 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define LEDC_IDLE_LV_LSCH1  (BIT(3))
#define LEDC_IDLE_LV_LSCH1_M  (BIT(3))
#define LEDC_IDLE_LV_LSCH1_V  0x1
#define LEDC_IDLE_LV_LSCH1_S  3
/* LEDC_SIG_OUT_EN_LSCH1 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define LEDC_SIG_OUT_EN_LSCH1  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH1_M  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH1_V  0x1
#define LEDC_SIG_OUT_EN_LSCH1_S  2
/* LEDC_TIMER_SEL_LSCH1 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: */
#define LEDC_TIMER_SEL_LSCH1  0x00000003
#define LEDC_TIMER_SEL_LSCH1_M  ((LEDC_TIMER_SEL_LSCH1_V)<<(LEDC_TIMER_SEL_LSCH1_S))
#define LEDC_TIMER_SEL_LSCH1_V  0x3
#define LEDC_TIMER_SEL_LSCH1_S  0

#define LEDC_LSCH1_HPOINT_REG          (DR_REG_LEDC_BASE + 0x0018)
/* LEDC_HPOINT_LSCH1 : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: */
#define LEDC_HPOINT_LSCH1  0x00003FFF
#define LEDC_HPOINT_LSCH1_M  ((LEDC_HPOINT_LSCH1_V)<<(LEDC_HPOINT_LSCH1_S))
#define LEDC_HPOINT_LSCH1_V  0x3FFF
#define LEDC_HPOINT_LSCH1_S  0

#define LEDC_LSCH1_DUTY_REG          (DR_REG_LEDC_BASE + 0x001C)
/* LEDC_DUTY_LSCH1 : R/W ;bitpos:[18:0] ;default: 19'h0 ; */
/*description: */
#define LEDC_DUTY_LSCH1  0x0007FFFF
#define LEDC_DUTY_LSCH1_M  ((LEDC_DUTY_LSCH1_V)<<(LEDC_DUTY_LSCH1_S))
#define LEDC_DUTY_LSCH1_V  0x7FFFF
#define LEDC_DUTY_LSCH1_S  0

#define LEDC_LSCH1_CONF1_REG          (DR_REG_LEDC_BASE + 0x0020)
/* LEDC_DUTY_START_LSCH1 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_START_LSCH1  (BIT(31))
#define LEDC_DUTY_START_LSCH1_M  (BIT(31))
#define LEDC_DUTY_START_LSCH1_V  0x1
#define LEDC_DUTY_START_LSCH1_S  31
/* LEDC_DUTY_INC_LSCH1 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: */
#define LEDC_DUTY_INC_LSCH1  (BIT(30))
#define LEDC_DUTY_INC_LSCH1_M  (BIT(30))
#define LEDC_DUTY_INC_LSCH1_V  0x1
#define LEDC_DUTY_INC_LSCH1_S  30
/* LEDC_DUTY_NUM_LSCH1 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_NUM_LSCH1  0x000003FF
#define LEDC_DUTY_NUM_LSCH1_M  ((LEDC_DUTY_NUM_LSCH1_V)<<(LEDC_DUTY_NUM_LSCH1_S))
#define LEDC_DUTY_NUM_LSCH1_V  0x3FF
#define LEDC_DUTY_NUM_LSCH1_S  20
/* LEDC_DUTY_CYCLE_LSCH1 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_CYCLE_LSCH1  0x000003FF
#define LEDC_DUTY_CYCLE_LSCH1_M  ((LEDC_DUTY_CYCLE_LSCH1_V)<<(LEDC_DUTY_CYCLE_LSCH1_S))
#define LEDC_DUTY_CYCLE_LSCH1_V  0x3FF
#define LEDC_DUTY_CYCLE_LSCH1_S  10
/* LEDC_DUTY_SCALE_LSCH1 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_SCALE_LSCH1  0x000003FF
#define LEDC_DUTY_SCALE_LSCH1_M  ((LEDC_DUTY_SCALE_LSCH1_V)<<(LEDC_DUTY_SCALE_LSCH1_S))
#define LEDC_DUTY_SCALE_LSCH1_V  0x3FF
#define LEDC_DUTY_SCALE_LSCH1_S  0

#define LEDC_LSCH1_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x0024)
/* LEDC_DUTY_LSCH1 : RO ;bitpos:[18:0] ;default: 19'h0 ; */
/*description: */
#define LEDC_DUTY_LSCH1  0x0007FFFF
#define LEDC_DUTY_LSCH1_M  ((LEDC_DUTY_LSCH1_V)<<(LEDC_DUTY_LSCH1_S))
#define LEDC_DUTY_LSCH1_V  0x7FFFF
#define LEDC_DUTY_LSCH1_S  0

#define LEDC_LSCH2_CONF0_REG          (DR_REG_LEDC_BASE + 0x0028)
/* LEDC_OVF_CNT_RESET_LSCH2 : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_RESET_LSCH2  (BIT(16))
#define LEDC_OVF_CNT_RESET_LSCH2_M  (BIT(16))
#define LEDC_OVF_CNT_RESET_LSCH2_V  0x1
#define LEDC_OVF_CNT_RESET_LSCH2_S  16
/* LEDC_OVF_CNT_EN_LSCH2 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_EN_LSCH2  (BIT(15))
#define LEDC_OVF_CNT_EN_LSCH2_M  (BIT(15))
#define LEDC_OVF_CNT_EN_LSCH2_V  0x1
#define LEDC_OVF_CNT_EN_LSCH2_S  15
/* LEDC_OVF_NUM_LSCH2 : R/W ;bitpos:[14:5] ;default: 10'b0 ; */
/*description: */
#define LEDC_OVF_NUM_LSCH2  0x000003FF
#define LEDC_OVF_NUM_LSCH2_M  ((LEDC_OVF_NUM_LSCH2_V)<<(LEDC_OVF_NUM_LSCH2_S))
#define LEDC_OVF_NUM_LSCH2_V  0x3FF
#define LEDC_OVF_NUM_LSCH2_S  5
/* LEDC_PARA_UP_LSCH2 : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define LEDC_PARA_UP_LSCH2  (BIT(4))
#define LEDC_PARA_UP_LSCH2_M  (BIT(4))
#define LEDC_PARA_UP_LSCH2_V  0x1
#define LEDC_PARA_UP_LSCH2_S  4
/* LEDC_IDLE_LV_LSCH2 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define LEDC_IDLE_LV_LSCH2  (BIT(3))
#define LEDC_IDLE_LV_LSCH2_M  (BIT(3))
#define LEDC_IDLE_LV_LSCH2_V  0x1
#define LEDC_IDLE_LV_LSCH2_S  3
/* LEDC_SIG_OUT_EN_LSCH2 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define LEDC_SIG_OUT_EN_LSCH2  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH2_M  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH2_V  0x1
#define LEDC_SIG_OUT_EN_LSCH2_S  2
/* LEDC_TIMER_SEL_LSCH2 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: */
#define LEDC_TIMER_SEL_LSCH2  0x00000003
#define LEDC_TIMER_SEL_LSCH2_M  ((LEDC_TIMER_SEL_LSCH2_V)<<(LEDC_TIMER_SEL_LSCH2_S))
#define LEDC_TIMER_SEL_LSCH2_V  0x3
#define LEDC_TIMER_SEL_LSCH2_S  0

#define LEDC_LSCH2_HPOINT_REG          (DR_REG_LEDC_BASE + 0x002C)
/* LEDC_HPOINT_LSCH2 : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: */
#define LEDC_HPOINT_LSCH2  0x00003FFF
#define LEDC_HPOINT_LSCH2_M  ((LEDC_HPOINT_LSCH2_V)<<(LEDC_HPOINT_LSCH2_S))
#define LEDC_HPOINT_LSCH2_V  0x3FFF
#define LEDC_HPOINT_LSCH2_S  0

#define LEDC_LSCH2_DUTY_REG          (DR_REG_LEDC_BASE + 0x0030)
/* LEDC_DUTY_LSCH2 : R/W ;bitpos:[18:0] ;default: 19'h0 ; */
/*description: */
#define LEDC_DUTY_LSCH2  0x0007FFFF
#define LEDC_DUTY_LSCH2_M  ((LEDC_DUTY_LSCH2_V)<<(LEDC_DUTY_LSCH2_S))
#define LEDC_DUTY_LSCH2_V  0x7FFFF
#define LEDC_DUTY_LSCH2_S  0

#define LEDC_LSCH2_CONF1_REG          (DR_REG_LEDC_BASE + 0x0034)
/* LEDC_DUTY_START_LSCH2 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_START_LSCH2  (BIT(31))
#define LEDC_DUTY_START_LSCH2_M  (BIT(31))
#define LEDC_DUTY_START_LSCH2_V  0x1
#define LEDC_DUTY_START_LSCH2_S  31
/* LEDC_DUTY_INC_LSCH2 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: */
#define LEDC_DUTY_INC_LSCH2  (BIT(30))
#define LEDC_DUTY_INC_LSCH2_M  (BIT(30))
#define LEDC_DUTY_INC_LSCH2_V  0x1
#define LEDC_DUTY_INC_LSCH2_S  30
/* LEDC_DUTY_NUM_LSCH2 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_NUM_LSCH2  0x000003FF
#define LEDC_DUTY_NUM_LSCH2_M  ((LEDC_DUTY_NUM_LSCH2_V)<<(LEDC_DUTY_NUM_LSCH2_S))
#define LEDC_DUTY_NUM_LSCH2_V  0x3FF
#define LEDC_DUTY_NUM_LSCH2_S  20
/* LEDC_DUTY_CYCLE_LSCH2 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_CYCLE_LSCH2  0x000003FF
#define LEDC_DUTY_CYCLE_LSCH2_M  ((LEDC_DUTY_CYCLE_LSCH2_V)<<(LEDC_DUTY_CYCLE_LSCH2_S))
#define LEDC_DUTY_CYCLE_LSCH2_V  0x3FF
#define LEDC_DUTY_CYCLE_LSCH2_S  10
/* LEDC_DUTY_SCALE_LSCH2 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_SCALE_LSCH2  0x000003FF
#define LEDC_DUTY_SCALE_LSCH2_M  ((LEDC_DUTY_SCALE_LSCH2_V)<<(LEDC_DUTY_SCALE_LSCH2_S))
#define LEDC_DUTY_SCALE_LSCH2_V  0x3FF
#define LEDC_DUTY_SCALE_LSCH2_S  0

#define LEDC_LSCH2_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x0038)
/* LEDC_DUTY_LSCH2 : RO ;bitpos:[18:0] ;default: 19'h0 ; */
/*description: */
#define LEDC_DUTY_LSCH2  0x0007FFFF
#define LEDC_DUTY_LSCH2_M  ((LEDC_DUTY_LSCH2_V)<<(LEDC_DUTY_LSCH2_S))
#define LEDC_DUTY_LSCH2_V  0x7FFFF
#define LEDC_DUTY_LSCH2_S  0

#define LEDC_LSCH3_CONF0_REG          (DR_REG_LEDC_BASE + 0x003C)
/* LEDC_OVF_CNT_RESET_LSCH3 : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_RESET_LSCH3  (BIT(16))
#define LEDC_OVF_CNT_RESET_LSCH3_M  (BIT(16))
#define LEDC_OVF_CNT_RESET_LSCH3_V  0x1
#define LEDC_OVF_CNT_RESET_LSCH3_S  16
/* LEDC_OVF_CNT_EN_LSCH3 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_EN_LSCH3  (BIT(15))
#define LEDC_OVF_CNT_EN_LSCH3_M  (BIT(15))
#define LEDC_OVF_CNT_EN_LSCH3_V  0x1
#define LEDC_OVF_CNT_EN_LSCH3_S  15
/* LEDC_OVF_NUM_LSCH3 : R/W ;bitpos:[14:5] ;default: 10'b0 ; */
/*description: */
#define LEDC_OVF_NUM_LSCH3  0x000003FF
#define LEDC_OVF_NUM_LSCH3_M  ((LEDC_OVF_NUM_LSCH3_V)<<(LEDC_OVF_NUM_LSCH3_S))
#define LEDC_OVF_NUM_LSCH3_V  0x3FF
#define LEDC_OVF_NUM_LSCH3_S  5
/* LEDC_PARA_UP_LSCH3 : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define LEDC_PARA_UP_LSCH3  (BIT(4))
#define LEDC_PARA_UP_LSCH3_M  (BIT(4))
#define LEDC_PARA_UP_LSCH3_V  0x1
#define LEDC_PARA_UP_LSCH3_S  4
/* LEDC_IDLE_LV_LSCH3 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define LEDC_IDLE_LV_LSCH3  (BIT(3))
#define LEDC_IDLE_LV_LSCH3_M  (BIT(3))
#define LEDC_IDLE_LV_LSCH3_V  0x1
#define LEDC_IDLE_LV_LSCH3_S  3
/* LEDC_SIG_OUT_EN_LSCH3 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define LEDC_SIG_OUT_EN_LSCH3  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH3_M  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH3_V  0x1
#define LEDC_SIG_OUT_EN_LSCH3_S  2
/* LEDC_TIMER_SEL_LSCH3 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: */
#define LEDC_TIMER_SEL_LSCH3  0x00000003
#define LEDC_TIMER_SEL_LSCH3_M  ((LEDC_TIMER_SEL_LSCH3_V)<<(LEDC_TIMER_SEL_LSCH3_S))
#define LEDC_TIMER_SEL_LSCH3_V  0x3
#define LEDC_TIMER_SEL_LSCH3_S  0

#define LEDC_LSCH3_HPOINT_REG          (DR_REG_LEDC_BASE + 0x0040)
/* LEDC_HPOINT_LSCH3 : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: */
#define LEDC_HPOINT_LSCH3  0x00003FFF
#define LEDC_HPOINT_LSCH3_M  ((LEDC_HPOINT_LSCH3_V)<<(LEDC_HPOINT_LSCH3_S))
#define LEDC_HPOINT_LSCH3_V  0x3FFF
#define LEDC_HPOINT_LSCH3_S  0

#define LEDC_LSCH3_DUTY_REG          (DR_REG_LEDC_BASE + 0x0044)
/* LEDC_DUTY_LSCH3 : R/W ;bitpos:[18:0] ;default: 19'h0 ; */
/*description: */
#define LEDC_DUTY_LSCH3  0x0007FFFF
#define LEDC_DUTY_LSCH3_M  ((LEDC_DUTY_LSCH3_V)<<(LEDC_DUTY_LSCH3_S))
#define LEDC_DUTY_LSCH3_V  0x7FFFF
#define LEDC_DUTY_LSCH3_S  0

#define LEDC_LSCH3_CONF1_REG          (DR_REG_LEDC_BASE + 0x0048)
/* LEDC_DUTY_START_LSCH3 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_START_LSCH3  (BIT(31))
#define LEDC_DUTY_START_LSCH3_M  (BIT(31))
#define LEDC_DUTY_START_LSCH3_V  0x1
#define LEDC_DUTY_START_LSCH3_S  31
/* LEDC_DUTY_INC_LSCH3 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: */
#define LEDC_DUTY_INC_LSCH3  (BIT(30))
#define LEDC_DUTY_INC_LSCH3_M  (BIT(30))
#define LEDC_DUTY_INC_LSCH3_V  0x1
#define LEDC_DUTY_INC_LSCH3_S  30
/* LEDC_DUTY_NUM_LSCH3 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_NUM_LSCH3  0x000003FF
#define LEDC_DUTY_NUM_LSCH3_M  ((LEDC_DUTY_NUM_LSCH3_V)<<(LEDC_DUTY_NUM_LSCH3_S))
#define LEDC_DUTY_NUM_LSCH3_V  0x3FF
#define LEDC_DUTY_NUM_LSCH3_S  20
/* LEDC_DUTY_CYCLE_LSCH3 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_CYCLE_LSCH3  0x000003FF
#define LEDC_DUTY_CYCLE_LSCH3_M  ((LEDC_DUTY_CYCLE_LSCH3_V)<<(LEDC_DUTY_CYCLE_LSCH3_S))
#define LEDC_DUTY_CYCLE_LSCH3_V  0x3FF
#define LEDC_DUTY_CYCLE_LSCH3_S  10
/* LEDC_DUTY_SCALE_LSCH3 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_SCALE_LSCH3  0x000003FF
#define LEDC_DUTY_SCALE_LSCH3_M  ((LEDC_DUTY_SCALE_LSCH3_V)<<(LEDC_DUTY_SCALE_LSCH3_S))
#define LEDC_DUTY_SCALE_LSCH3_V  0x3FF
#define LEDC_DUTY_SCALE_LSCH3_S  0

#define LEDC_LSCH3_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x004C)
/* LEDC_DUTY_LSCH3 : RO ;bitpos:[18:0] ;default: 19'h0 ; */
/*description: */
#define LEDC_DUTY_LSCH3  0x0007FFFF
#define LEDC_DUTY_LSCH3_M  ((LEDC_DUTY_LSCH3_V)<<(LEDC_DUTY_LSCH3_S))
#define LEDC_DUTY_LSCH3_V  0x7FFFF
#define LEDC_DUTY_LSCH3_S  0

#define LEDC_LSCH4_CONF0_REG          (DR_REG_LEDC_BASE + 0x0050)
/* LEDC_OVF_CNT_RESET_LSCH4 : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_RESET_LSCH4  (BIT(16))
#define LEDC_OVF_CNT_RESET_LSCH4_M  (BIT(16))
#define LEDC_OVF_CNT_RESET_LSCH4_V  0x1
#define LEDC_OVF_CNT_RESET_LSCH4_S  16
/* LEDC_OVF_CNT_EN_LSCH4 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_EN_LSCH4  (BIT(15))
#define LEDC_OVF_CNT_EN_LSCH4_M  (BIT(15))
#define LEDC_OVF_CNT_EN_LSCH4_V  0x1
#define LEDC_OVF_CNT_EN_LSCH4_S  15
/* LEDC_OVF_NUM_LSCH4 : R/W ;bitpos:[14:5] ;default: 10'b0 ; */
/*description: */
#define LEDC_OVF_NUM_LSCH4  0x000003FF
#define LEDC_OVF_NUM_LSCH4_M  ((LEDC_OVF_NUM_LSCH4_V)<<(LEDC_OVF_NUM_LSCH4_S))
#define LEDC_OVF_NUM_LSCH4_V  0x3FF
#define LEDC_OVF_NUM_LSCH4_S  5
/* LEDC_PARA_UP_LSCH4 : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define LEDC_PARA_UP_LSCH4  (BIT(4))
#define LEDC_PARA_UP_LSCH4_M  (BIT(4))
#define LEDC_PARA_UP_LSCH4_V  0x1
#define LEDC_PARA_UP_LSCH4_S  4
/* LEDC_IDLE_LV_LSCH4 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define LEDC_IDLE_LV_LSCH4  (BIT(3))
#define LEDC_IDLE_LV_LSCH4_M  (BIT(3))
#define LEDC_IDLE_LV_LSCH4_V  0x1
#define LEDC_IDLE_LV_LSCH4_S  3
/* LEDC_SIG_OUT_EN_LSCH4 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define LEDC_SIG_OUT_EN_LSCH4  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH4_M  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH4_V  0x1
#define LEDC_SIG_OUT_EN_LSCH4_S  2
/* LEDC_TIMER_SEL_LSCH4 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: */
#define LEDC_TIMER_SEL_LSCH4  0x00000003
#define LEDC_TIMER_SEL_LSCH4_M  ((LEDC_TIMER_SEL_LSCH4_V)<<(LEDC_TIMER_SEL_LSCH4_S))
#define LEDC_TIMER_SEL_LSCH4_V  0x3
#define LEDC_TIMER_SEL_LSCH4_S  0

#define LEDC_LSCH4_HPOINT_REG          (DR_REG_LEDC_BASE + 0x0054)
/* LEDC_HPOINT_LSCH4 : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: */
#define LEDC_HPOINT_LSCH4  0x00003FFF
#define LEDC_HPOINT_LSCH4_M  ((LEDC_HPOINT_LSCH4_V)<<(LEDC_HPOINT_LSCH4_S))
#define LEDC_HPOINT_LSCH4_V  0x3FFF
#define LEDC_HPOINT_LSCH4_S  0

#define LEDC_LSCH4_DUTY_REG          (DR_REG_LEDC_BASE + 0x0058)
/* LEDC_DUTY_LSCH4 : R/W ;bitpos:[18:0] ;default: 19'h0 ; */
/*description: */
#define LEDC_DUTY_LSCH4  0x0007FFFF
#define LEDC_DUTY_LSCH4_M  ((LEDC_DUTY_LSCH4_V)<<(LEDC_DUTY_LSCH4_S))
#define LEDC_DUTY_LSCH4_V  0x7FFFF
#define LEDC_DUTY_LSCH4_S  0

#define LEDC_LSCH4_CONF1_REG          (DR_REG_LEDC_BASE + 0x005C)
/* LEDC_DUTY_START_LSCH4 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_START_LSCH4  (BIT(31))
#define LEDC_DUTY_START_LSCH4_M  (BIT(31))
#define LEDC_DUTY_START_LSCH4_V  0x1
#define LEDC_DUTY_START_LSCH4_S  31
/* LEDC_DUTY_INC_LSCH4 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: */
#define LEDC_DUTY_INC_LSCH4  (BIT(30))
#define LEDC_DUTY_INC_LSCH4_M  (BIT(30))
#define LEDC_DUTY_INC_LSCH4_V  0x1
#define LEDC_DUTY_INC_LSCH4_S  30
/* LEDC_DUTY_NUM_LSCH4 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_NUM_LSCH4  0x000003FF
#define LEDC_DUTY_NUM_LSCH4_M  ((LEDC_DUTY_NUM_LSCH4_V)<<(LEDC_DUTY_NUM_LSCH4_S))
#define LEDC_DUTY_NUM_LSCH4_V  0x3FF
#define LEDC_DUTY_NUM_LSCH4_S  20
/* LEDC_DUTY_CYCLE_LSCH4 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_CYCLE_LSCH4  0x000003FF
#define LEDC_DUTY_CYCLE_LSCH4_M  ((LEDC_DUTY_CYCLE_LSCH4_V)<<(LEDC_DUTY_CYCLE_LSCH4_S))
#define LEDC_DUTY_CYCLE_LSCH4_V  0x3FF
#define LEDC_DUTY_CYCLE_LSCH4_S  10
/* LEDC_DUTY_SCALE_LSCH4 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_SCALE_LSCH4  0x000003FF
#define LEDC_DUTY_SCALE_LSCH4_M  ((LEDC_DUTY_SCALE_LSCH4_V)<<(LEDC_DUTY_SCALE_LSCH4_S))
#define LEDC_DUTY_SCALE_LSCH4_V  0x3FF
#define LEDC_DUTY_SCALE_LSCH4_S  0

#define LEDC_LSCH4_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x0060)
/* LEDC_DUTY_LSCH4 : RO ;bitpos:[18:0] ;default: 19'h0 ; */
/*description: */
#define LEDC_DUTY_LSCH4  0x0007FFFF
#define LEDC_DUTY_LSCH4_M  ((LEDC_DUTY_LSCH4_V)<<(LEDC_DUTY_LSCH4_S))
#define LEDC_DUTY_LSCH4_V  0x7FFFF
#define LEDC_DUTY_LSCH4_S  0

#define LEDC_LSCH5_CONF0_REG          (DR_REG_LEDC_BASE + 0x0064)
/* LEDC_OVF_CNT_RESET_LSCH5 : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_RESET_LSCH5  (BIT(16))
#define LEDC_OVF_CNT_RESET_LSCH5_M  (BIT(16))
#define LEDC_OVF_CNT_RESET_LSCH5_V  0x1
#define LEDC_OVF_CNT_RESET_LSCH5_S  16
/* LEDC_OVF_CNT_EN_LSCH5 : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_EN_LSCH5  (BIT(15))
#define LEDC_OVF_CNT_EN_LSCH5_M  (BIT(15))
#define LEDC_OVF_CNT_EN_LSCH5_V  0x1
#define LEDC_OVF_CNT_EN_LSCH5_S  15
/* LEDC_OVF_NUM_LSCH5 : R/W ;bitpos:[14:5] ;default: 10'b0 ; */
/*description: */
#define LEDC_OVF_NUM_LSCH5  0x000003FF
#define LEDC_OVF_NUM_LSCH5_M  ((LEDC_OVF_NUM_LSCH5_V)<<(LEDC_OVF_NUM_LSCH5_S))
#define LEDC_OVF_NUM_LSCH5_V  0x3FF
#define LEDC_OVF_NUM_LSCH5_S  5
/* LEDC_PARA_UP_LSCH5 : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define LEDC_PARA_UP_LSCH5  (BIT(4))
#define LEDC_PARA_UP_LSCH5_M  (BIT(4))
#define LEDC_PARA_UP_LSCH5_V  0x1
#define LEDC_PARA_UP_LSCH5_S  4
/* LEDC_IDLE_LV_LSCH5 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define LEDC_IDLE_LV_LSCH5  (BIT(3))
#define LEDC_IDLE_LV_LSCH5_M  (BIT(3))
#define LEDC_IDLE_LV_LSCH5_V  0x1
#define LEDC_IDLE_LV_LSCH5_S  3
/* LEDC_SIG_OUT_EN_LSCH5 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define LEDC_SIG_OUT_EN_LSCH5  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH5_M  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH5_V  0x1
#define LEDC_SIG_OUT_EN_LSCH5_S  2
/* LEDC_TIMER_SEL_LSCH5 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: */
#define LEDC_TIMER_SEL_LSCH5  0x00000003
#define LEDC_TIMER_SEL_LSCH5_M  ((LEDC_TIMER_SEL_LSCH5_V)<<(LEDC_TIMER_SEL_LSCH5_S))
#define LEDC_TIMER_SEL_LSCH5_V  0x3
#define LEDC_TIMER_SEL_LSCH5_S  0

#define LEDC_LSCH5_HPOINT_REG          (DR_REG_LEDC_BASE + 0x0068)
/* LEDC_HPOINT_LSCH5 : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: */
#define LEDC_HPOINT_LSCH5  0x00003FFF
#define LEDC_HPOINT_LSCH5_M  ((LEDC_HPOINT_LSCH5_V)<<(LEDC_HPOINT_LSCH5_S))
#define LEDC_HPOINT_LSCH5_V  0x3FFF
#define LEDC_HPOINT_LSCH5_S  0

#define LEDC_LSCH5_DUTY_REG          (DR_REG_LEDC_BASE + 0x006C)
/* LEDC_DUTY_LSCH5 : R/W ;bitpos:[18:0] ;default: 19'h0 ; */
/*description: */
#define LEDC_DUTY_LSCH5  0x0007FFFF
#define LEDC_DUTY_LSCH5_M  ((LEDC_DUTY_LSCH5_V)<<(LEDC_DUTY_LSCH5_S))
#define LEDC_DUTY_LSCH5_V  0x7FFFF
#define LEDC_DUTY_LSCH5_S  0

#define LEDC_LSCH5_CONF1_REG          (DR_REG_LEDC_BASE + 0x0070)
/* LEDC_DUTY_START_LSCH5 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_START_LSCH5  (BIT(31))
#define LEDC_DUTY_START_LSCH5_M  (BIT(31))
#define LEDC_DUTY_START_LSCH5_V  0x1
#define LEDC_DUTY_START_LSCH5_S  31
/* LEDC_DUTY_INC_LSCH5 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: */
#define LEDC_DUTY_INC_LSCH5  (BIT(30))
#define LEDC_DUTY_INC_LSCH5_M  (BIT(30))
#define LEDC_DUTY_INC_LSCH5_V  0x1
#define LEDC_DUTY_INC_LSCH5_S  30
/* LEDC_DUTY_NUM_LSCH5 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_NUM_LSCH5  0x000003FF
#define LEDC_DUTY_NUM_LSCH5_M  ((LEDC_DUTY_NUM_LSCH5_V)<<(LEDC_DUTY_NUM_LSCH5_S))
#define LEDC_DUTY_NUM_LSCH5_V  0x3FF
#define LEDC_DUTY_NUM_LSCH5_S  20
/* LEDC_DUTY_CYCLE_LSCH5 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_CYCLE_LSCH5  0x000003FF
#define LEDC_DUTY_CYCLE_LSCH5_M  ((LEDC_DUTY_CYCLE_LSCH5_V)<<(LEDC_DUTY_CYCLE_LSCH5_S))
#define LEDC_DUTY_CYCLE_LSCH5_V  0x3FF
#define LEDC_DUTY_CYCLE_LSCH5_S  10
/* LEDC_DUTY_SCALE_LSCH5 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: */
#define LEDC_DUTY_SCALE_LSCH5  0x000003FF
#define LEDC_DUTY_SCALE_LSCH5_M  ((LEDC_DUTY_SCALE_LSCH5_V)<<(LEDC_DUTY_SCALE_LSCH5_S))
#define LEDC_DUTY_SCALE_LSCH5_V  0x3FF
#define LEDC_DUTY_SCALE_LSCH5_S  0

#define LEDC_LSCH5_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x0074)
/* LEDC_DUTY_LSCH5 : RO ;bitpos:[18:0] ;default: 19'h0 ; */
/*description: */
#define LEDC_DUTY_LSCH5  0x0007FFFF
#define LEDC_DUTY_LSCH5_M  ((LEDC_DUTY_LSCH5_V)<<(LEDC_DUTY_LSCH5_S))
#define LEDC_DUTY_LSCH5_V  0x7FFFF
#define LEDC_DUTY_LSCH5_S  0

#define LEDC_LSTIMER0_CONF_REG          (DR_REG_LEDC_BASE + 0x00a0)
/* LEDC_LSTIMER0_PARA_UP : WO ;bitpos:[25] ;default: 1'h0 ; */
/*description: */
#define LEDC_LSTIMER0_PARA_UP  (BIT(25))
#define LEDC_LSTIMER0_PARA_UP_M  (BIT(25))
#define LEDC_LSTIMER0_PARA_UP_V  0x1
#define LEDC_LSTIMER0_PARA_UP_S  25
/* LEDC_TICK_SEL_LSTIMER0 : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define LEDC_TICK_SEL_LSTIMER0  (BIT(24))
#define LEDC_TICK_SEL_LSTIMER0_M  (BIT(24))
#define LEDC_TICK_SEL_LSTIMER0_V  0x1
#define LEDC_TICK_SEL_LSTIMER0_S  24
/* LEDC_LSTIMER0_RST : R/W ;bitpos:[23] ;default: 1'b1 ; */
/*description: */
#define LEDC_LSTIMER0_RST  (BIT(23))
#define LEDC_LSTIMER0_RST_M  (BIT(23))
#define LEDC_LSTIMER0_RST_V  0x1
#define LEDC_LSTIMER0_RST_S  23
/* LEDC_LSTIMER0_PAUSE : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER0_PAUSE  (BIT(22))
#define LEDC_LSTIMER0_PAUSE_M  (BIT(22))
#define LEDC_LSTIMER0_PAUSE_V  0x1
#define LEDC_LSTIMER0_PAUSE_S  22
/* LEDC_CLK_DIV_LSTIMER0 : R/W ;bitpos:[21:4] ;default: 18'h0 ; */
/*description: */
#define LEDC_CLK_DIV_LSTIMER0  0x0003FFFF
#define LEDC_CLK_DIV_LSTIMER0_M  ((LEDC_CLK_DIV_LSTIMER0_V)<<(LEDC_CLK_DIV_LSTIMER0_S))
#define LEDC_CLK_DIV_LSTIMER0_V  0x3FFFF
#define LEDC_CLK_DIV_LSTIMER0_S  4
/* LEDC_LSTIMER0_DUTY_RES : R/W ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: */
#define LEDC_LSTIMER0_DUTY_RES  0x0000000F
#define LEDC_LSTIMER0_DUTY_RES_M  ((LEDC_LSTIMER0_DUTY_RES_V)<<(LEDC_LSTIMER0_DUTY_RES_S))
#define LEDC_LSTIMER0_DUTY_RES_V  0xF
#define LEDC_LSTIMER0_DUTY_RES_S  0

#define LEDC_LSTIMER0_VALUE_REG          (DR_REG_LEDC_BASE + 0x00a4)
/* LEDC_LSTIMER0_CNT : RO ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: */
#define LEDC_LSTIMER0_CNT  0x00003FFF
#define LEDC_LSTIMER0_CNT_M  ((LEDC_LSTIMER0_CNT_V)<<(LEDC_LSTIMER0_CNT_S))
#define LEDC_LSTIMER0_CNT_V  0x3FFF
#define LEDC_LSTIMER0_CNT_S  0

#define LEDC_LSTIMER1_CONF_REG          (DR_REG_LEDC_BASE + 0x00a8)
/* LEDC_LSTIMER1_PARA_UP : WO ;bitpos:[25] ;default: 1'h0 ; */
/*description: */
#define LEDC_LSTIMER1_PARA_UP  (BIT(25))
#define LEDC_LSTIMER1_PARA_UP_M  (BIT(25))
#define LEDC_LSTIMER1_PARA_UP_V  0x1
#define LEDC_LSTIMER1_PARA_UP_S  25
/* LEDC_TICK_SEL_LSTIMER1 : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define LEDC_TICK_SEL_LSTIMER1  (BIT(24))
#define LEDC_TICK_SEL_LSTIMER1_M  (BIT(24))
#define LEDC_TICK_SEL_LSTIMER1_V  0x1
#define LEDC_TICK_SEL_LSTIMER1_S  24
/* LEDC_LSTIMER1_RST : R/W ;bitpos:[23] ;default: 1'b1 ; */
/*description: */
#define LEDC_LSTIMER1_RST  (BIT(23))
#define LEDC_LSTIMER1_RST_M  (BIT(23))
#define LEDC_LSTIMER1_RST_V  0x1
#define LEDC_LSTIMER1_RST_S  23
/* LEDC_LSTIMER1_PAUSE : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER1_PAUSE  (BIT(22))
#define LEDC_LSTIMER1_PAUSE_M  (BIT(22))
#define LEDC_LSTIMER1_PAUSE_V  0x1
#define LEDC_LSTIMER1_PAUSE_S  22
/* LEDC_CLK_DIV_LSTIMER1 : R/W ;bitpos:[21:4] ;default: 18'h0 ; */
/*description: */
#define LEDC_CLK_DIV_LSTIMER1  0x0003FFFF
#define LEDC_CLK_DIV_LSTIMER1_M  ((LEDC_CLK_DIV_LSTIMER1_V)<<(LEDC_CLK_DIV_LSTIMER1_S))
#define LEDC_CLK_DIV_LSTIMER1_V  0x3FFFF
#define LEDC_CLK_DIV_LSTIMER1_S  4
/* LEDC_LSTIMER1_DUTY_RES : R/W ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: */
#define LEDC_LSTIMER1_DUTY_RES  0x0000000F
#define LEDC_LSTIMER1_DUTY_RES_M  ((LEDC_LSTIMER1_DUTY_RES_V)<<(LEDC_LSTIMER1_DUTY_RES_S))
#define LEDC_LSTIMER1_DUTY_RES_V  0xF
#define LEDC_LSTIMER1_DUTY_RES_S  0

#define LEDC_LSTIMER1_VALUE_REG          (DR_REG_LEDC_BASE + 0x00aC)
/* LEDC_LSTIMER1_CNT : RO ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: */
#define LEDC_LSTIMER1_CNT  0x00003FFF
#define LEDC_LSTIMER1_CNT_M  ((LEDC_LSTIMER1_CNT_V)<<(LEDC_LSTIMER1_CNT_S))
#define LEDC_LSTIMER1_CNT_V  0x3FFF
#define LEDC_LSTIMER1_CNT_S  0

#define LEDC_LSTIMER2_CONF_REG          (DR_REG_LEDC_BASE + 0x00b0)
/* LEDC_LSTIMER2_PARA_UP : WO ;bitpos:[25] ;default: 1'h0 ; */
/*description: */
#define LEDC_LSTIMER2_PARA_UP  (BIT(25))
#define LEDC_LSTIMER2_PARA_UP_M  (BIT(25))
#define LEDC_LSTIMER2_PARA_UP_V  0x1
#define LEDC_LSTIMER2_PARA_UP_S  25
/* LEDC_TICK_SEL_LSTIMER2 : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define LEDC_TICK_SEL_LSTIMER2  (BIT(24))
#define LEDC_TICK_SEL_LSTIMER2_M  (BIT(24))
#define LEDC_TICK_SEL_LSTIMER2_V  0x1
#define LEDC_TICK_SEL_LSTIMER2_S  24
/* LEDC_LSTIMER2_RST : R/W ;bitpos:[23] ;default: 1'b1 ; */
/*description: */
#define LEDC_LSTIMER2_RST  (BIT(23))
#define LEDC_LSTIMER2_RST_M  (BIT(23))
#define LEDC_LSTIMER2_RST_V  0x1
#define LEDC_LSTIMER2_RST_S  23
/* LEDC_LSTIMER2_PAUSE : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER2_PAUSE  (BIT(22))
#define LEDC_LSTIMER2_PAUSE_M  (BIT(22))
#define LEDC_LSTIMER2_PAUSE_V  0x1
#define LEDC_LSTIMER2_PAUSE_S  22
/* LEDC_CLK_DIV_LSTIMER2 : R/W ;bitpos:[21:4] ;default: 18'h0 ; */
/*description: */
#define LEDC_CLK_DIV_LSTIMER2  0x0003FFFF
#define LEDC_CLK_DIV_LSTIMER2_M  ((LEDC_CLK_DIV_LSTIMER2_V)<<(LEDC_CLK_DIV_LSTIMER2_S))
#define LEDC_CLK_DIV_LSTIMER2_V  0x3FFFF
#define LEDC_CLK_DIV_LSTIMER2_S  4
/* LEDC_LSTIMER2_DUTY_RES : R/W ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: */
#define LEDC_LSTIMER2_DUTY_RES  0x0000000F
#define LEDC_LSTIMER2_DUTY_RES_M  ((LEDC_LSTIMER2_DUTY_RES_V)<<(LEDC_LSTIMER2_DUTY_RES_S))
#define LEDC_LSTIMER2_DUTY_RES_V  0xF
#define LEDC_LSTIMER2_DUTY_RES_S  0

#define LEDC_LSTIMER2_VALUE_REG          (DR_REG_LEDC_BASE + 0x00b4)
/* LEDC_LSTIMER2_CNT : RO ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: */
#define LEDC_LSTIMER2_CNT  0x00003FFF
#define LEDC_LSTIMER2_CNT_M  ((LEDC_LSTIMER2_CNT_V)<<(LEDC_LSTIMER2_CNT_S))
#define LEDC_LSTIMER2_CNT_V  0x3FFF
#define LEDC_LSTIMER2_CNT_S  0

#define LEDC_LSTIMER3_CONF_REG          (DR_REG_LEDC_BASE + 0x00b8)
/* LEDC_LSTIMER3_PARA_UP : WO ;bitpos:[25] ;default: 1'h0 ; */
/*description: */
#define LEDC_LSTIMER3_PARA_UP  (BIT(25))
#define LEDC_LSTIMER3_PARA_UP_M  (BIT(25))
#define LEDC_LSTIMER3_PARA_UP_V  0x1
#define LEDC_LSTIMER3_PARA_UP_S  25
/* LEDC_TICK_SEL_LSTIMER3 : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define LEDC_TICK_SEL_LSTIMER3  (BIT(24))
#define LEDC_TICK_SEL_LSTIMER3_M  (BIT(24))
#define LEDC_TICK_SEL_LSTIMER3_V  0x1
#define LEDC_TICK_SEL_LSTIMER3_S  24
/* LEDC_LSTIMER3_RST : R/W ;bitpos:[23] ;default: 1'b1 ; */
/*description: */
#define LEDC_LSTIMER3_RST  (BIT(23))
#define LEDC_LSTIMER3_RST_M  (BIT(23))
#define LEDC_LSTIMER3_RST_V  0x1
#define LEDC_LSTIMER3_RST_S  23
/* LEDC_LSTIMER3_PAUSE : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER3_PAUSE  (BIT(22))
#define LEDC_LSTIMER3_PAUSE_M  (BIT(22))
#define LEDC_LSTIMER3_PAUSE_V  0x1
#define LEDC_LSTIMER3_PAUSE_S  22
/* LEDC_CLK_DIV_LSTIMER3 : R/W ;bitpos:[21:4] ;default: 18'h0 ; */
/*description: */
#define LEDC_CLK_DIV_LSTIMER3  0x0003FFFF
#define LEDC_CLK_DIV_LSTIMER3_M  ((LEDC_CLK_DIV_LSTIMER3_V)<<(LEDC_CLK_DIV_LSTIMER3_S))
#define LEDC_CLK_DIV_LSTIMER3_V  0x3FFFF
#define LEDC_CLK_DIV_LSTIMER3_S  4
/* LEDC_LSTIMER3_DUTY_RES : R/W ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: */
#define LEDC_LSTIMER3_DUTY_RES  0x0000000F
#define LEDC_LSTIMER3_DUTY_RES_M  ((LEDC_LSTIMER3_DUTY_RES_V)<<(LEDC_LSTIMER3_DUTY_RES_S))
#define LEDC_LSTIMER3_DUTY_RES_V  0xF
#define LEDC_LSTIMER3_DUTY_RES_S  0

#define LEDC_LSTIMER3_VALUE_REG          (DR_REG_LEDC_BASE + 0x00BC)
/* LEDC_LSTIMER3_CNT : RO ;bitpos:[13:0] ;default: 14'b0 ; */
/*description: */
#define LEDC_LSTIMER3_CNT  0x00003FFF
#define LEDC_LSTIMER3_CNT_M  ((LEDC_LSTIMER3_CNT_V)<<(LEDC_LSTIMER3_CNT_S))
#define LEDC_LSTIMER3_CNT_V  0x3FFF
#define LEDC_LSTIMER3_CNT_S  0

#define LEDC_INT_RAW_REG          (DR_REG_LEDC_BASE + 0x00C0)
/* LEDC_OVF_CNT_LSCH5_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH5_INT_RAW    (BIT(15))
#define LEDC_OVF_CNT_LSCH5_INT_RAW_M  (BIT(15))
#define LEDC_OVF_CNT_LSCH5_INT_RAW_V  0x1
#define LEDC_OVF_CNT_LSCH5_INT_RAW_S  15
/* LEDC_OVF_CNT_LSCH4_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH4_INT_RAW    (BIT(14))
#define LEDC_OVF_CNT_LSCH4_INT_RAW_M  (BIT(14))
#define LEDC_OVF_CNT_LSCH4_INT_RAW_V  0x1
#define LEDC_OVF_CNT_LSCH4_INT_RAW_S  14
/* LEDC_OVF_CNT_LSCH3_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH3_INT_RAW    (BIT(13))
#define LEDC_OVF_CNT_LSCH3_INT_RAW_M  (BIT(13))
#define LEDC_OVF_CNT_LSCH3_INT_RAW_V  0x1
#define LEDC_OVF_CNT_LSCH3_INT_RAW_S  13
/* LEDC_OVF_CNT_LSCH2_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH2_INT_RAW    (BIT(12))
#define LEDC_OVF_CNT_LSCH2_INT_RAW_M  (BIT(12))
#define LEDC_OVF_CNT_LSCH2_INT_RAW_V  0x1
#define LEDC_OVF_CNT_LSCH2_INT_RAW_S  12
/* LEDC_OVF_CNT_LSCH1_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH1_INT_RAW    (BIT(11))
#define LEDC_OVF_CNT_LSCH1_INT_RAW_M  (BIT(11))
#define LEDC_OVF_CNT_LSCH1_INT_RAW_V  0x1
#define LEDC_OVF_CNT_LSCH1_INT_RAW_S  11
/* LEDC_OVF_CNT_LSCH0_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH0_INT_RAW    (BIT(10))
#define LEDC_OVF_CNT_LSCH0_INT_RAW_M  (BIT(10))
#define LEDC_OVF_CNT_LSCH0_INT_RAW_V  0x1
#define LEDC_OVF_CNT_LSCH0_INT_RAW_S  10
/* LEDC_DUTY_CHNG_END_LSCH5_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH5_INT_RAW  (BIT(9))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_RAW_M  (BIT(9))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH5_INT_RAW_S  9
/* LEDC_DUTY_CHNG_END_LSCH4_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH4_INT_RAW  (BIT(8))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_RAW_M  (BIT(8))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH4_INT_RAW_S  8
/* LEDC_DUTY_CHNG_END_LSCH3_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH3_INT_RAW  (BIT(7))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_RAW_M  (BIT(7))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH3_INT_RAW_S  7
/* LEDC_DUTY_CHNG_END_LSCH2_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH2_INT_RAW  (BIT(6))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_RAW_M  (BIT(6))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH2_INT_RAW_S  6
/* LEDC_DUTY_CHNG_END_LSCH1_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH1_INT_RAW  (BIT(5))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_RAW_M  (BIT(5))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH1_INT_RAW_S  5
/* LEDC_DUTY_CHNG_END_LSCH0_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH0_INT_RAW  (BIT(4))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_RAW_M  (BIT(4))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH0_INT_RAW_S  4
/* LEDC_LSTIMER3_OVF_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER3_OVF_INT_RAW  (BIT(3))
#define LEDC_LSTIMER3_OVF_INT_RAW_M  (BIT(3))
#define LEDC_LSTIMER3_OVF_INT_RAW_V  0x1
#define LEDC_LSTIMER3_OVF_INT_RAW_S  3
/* LEDC_LSTIMER2_OVF_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER2_OVF_INT_RAW  (BIT(2))
#define LEDC_LSTIMER2_OVF_INT_RAW_M  (BIT(2))
#define LEDC_LSTIMER2_OVF_INT_RAW_V  0x1
#define LEDC_LSTIMER2_OVF_INT_RAW_S  2
/* LEDC_LSTIMER1_OVF_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER1_OVF_INT_RAW  (BIT(1))
#define LEDC_LSTIMER1_OVF_INT_RAW_M  (BIT(1))
#define LEDC_LSTIMER1_OVF_INT_RAW_V  0x1
#define LEDC_LSTIMER1_OVF_INT_RAW_S  1
/* LEDC_LSTIMER0_OVF_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER0_OVF_INT_RAW  (BIT(0))
#define LEDC_LSTIMER0_OVF_INT_RAW_M  (BIT(0))
#define LEDC_LSTIMER0_OVF_INT_RAW_V  0x1
#define LEDC_LSTIMER0_OVF_INT_RAW_S  0

#define LEDC_INT_ST_REG          (DR_REG_LEDC_BASE + 0x00c4)
/* LEDC_OVF_CNT_LSCH5_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH5_INT_ST  (BIT(15))
#define LEDC_OVF_CNT_LSCH5_INT_ST_M  (BIT(15))
#define LEDC_OVF_CNT_LSCH5_INT_ST_V  0x1
#define LEDC_OVF_CNT_LSCH5_INT_ST_S  15
/* LEDC_OVF_CNT_LSCH4_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH4_INT_ST  (BIT(14))
#define LEDC_OVF_CNT_LSCH4_INT_ST_M  (BIT(14))
#define LEDC_OVF_CNT_LSCH4_INT_ST_V  0x1
#define LEDC_OVF_CNT_LSCH4_INT_ST_S  14
/* LEDC_OVF_CNT_LSCH3_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH3_INT_ST  (BIT(13))
#define LEDC_OVF_CNT_LSCH3_INT_ST_M  (BIT(13))
#define LEDC_OVF_CNT_LSCH3_INT_ST_V  0x1
#define LEDC_OVF_CNT_LSCH3_INT_ST_S  13
/* LEDC_OVF_CNT_LSCH2_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH2_INT_ST    (BIT(12))
#define LEDC_OVF_CNT_LSCH2_INT_ST_M  (BIT(12))
#define LEDC_OVF_CNT_LSCH2_INT_ST_V  0x1
#define LEDC_OVF_CNT_LSCH2_INT_ST_S  12
/* LEDC_OVF_CNT_LSCH1_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH1_INT_ST    (BIT(11))
#define LEDC_OVF_CNT_LSCH1_INT_ST_M  (BIT(11))
#define LEDC_OVF_CNT_LSCH1_INT_ST_V  0x1
#define LEDC_OVF_CNT_LSCH1_INT_ST_S  11
/* LEDC_OVF_CNT_LSCH0_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH0_INT_ST    (BIT(10))
#define LEDC_OVF_CNT_LSCH0_INT_ST_M  (BIT(10))
#define LEDC_OVF_CNT_LSCH0_INT_ST_V  0x1
#define LEDC_OVF_CNT_LSCH0_INT_ST_S  10
/* LEDC_DUTY_CHNG_END_LSCH5_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ST  (BIT(9))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ST_M  (BIT(9))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ST_S  9
/* LEDC_DUTY_CHNG_END_LSCH4_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ST  (BIT(8))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ST_M  (BIT(8))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ST_S  8
/* LEDC_DUTY_CHNG_END_LSCH3_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ST  (BIT(7))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ST_M  (BIT(7))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ST_S  7
/* LEDC_DUTY_CHNG_END_LSCH2_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ST  (BIT(6))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ST_M  (BIT(6))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ST_S  6
/* LEDC_DUTY_CHNG_END_LSCH1_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ST  (BIT(5))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ST_M  (BIT(5))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ST_S  5
/* LEDC_DUTY_CHNG_END_LSCH0_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ST  (BIT(4))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ST_M  (BIT(4))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ST_S  4
/* LEDC_LSTIMER3_OVF_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER3_OVF_INT_ST  (BIT(3))
#define LEDC_LSTIMER3_OVF_INT_ST_M  (BIT(3))
#define LEDC_LSTIMER3_OVF_INT_ST_V  0x1
#define LEDC_LSTIMER3_OVF_INT_ST_S  3
/* LEDC_LSTIMER2_OVF_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER2_OVF_INT_ST  (BIT(2))
#define LEDC_LSTIMER2_OVF_INT_ST_M  (BIT(2))
#define LEDC_LSTIMER2_OVF_INT_ST_V  0x1
#define LEDC_LSTIMER2_OVF_INT_ST_S  2
/* LEDC_LSTIMER1_OVF_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER1_OVF_INT_ST  (BIT(1))
#define LEDC_LSTIMER1_OVF_INT_ST_M  (BIT(1))
#define LEDC_LSTIMER1_OVF_INT_ST_V  0x1
#define LEDC_LSTIMER1_OVF_INT_ST_S  1
/* LEDC_LSTIMER0_OVF_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER0_OVF_INT_ST  (BIT(0))
#define LEDC_LSTIMER0_OVF_INT_ST_M  (BIT(0))
#define LEDC_LSTIMER0_OVF_INT_ST_V  0x1
#define LEDC_LSTIMER0_OVF_INT_ST_S  0

#define LEDC_INT_ENA_REG          (DR_REG_LEDC_BASE + 0xC8)
/* LEDC_OVF_CNT_LSCH5_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH5_INT_ENA    (BIT(15))
#define LEDC_OVF_CNT_LSCH5_INT_ENA_M  (BIT(15))
#define LEDC_OVF_CNT_LSCH5_INT_ENA_V  0x1
#define LEDC_OVF_CNT_LSCH5_INT_ENA_S  15
/* LEDC_OVF_CNT_LSCH4_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH4_INT_ENA    (BIT(14))
#define LEDC_OVF_CNT_LSCH4_INT_ENA_M  (BIT(14))
#define LEDC_OVF_CNT_LSCH4_INT_ENA_V  0x1
#define LEDC_OVF_CNT_LSCH4_INT_ENA_S  14
/* LEDC_OVF_CNT_LSCH3_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH3_INT_ENA    (BIT(13))
#define LEDC_OVF_CNT_LSCH3_INT_ENA_M  (BIT(13))
#define LEDC_OVF_CNT_LSCH3_INT_ENA_V  0x1
#define LEDC_OVF_CNT_LSCH3_INT_ENA_S  13
/* LEDC_OVF_CNT_LSCH2_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH2_INT_ENA    (BIT(12))
#define LEDC_OVF_CNT_LSCH2_INT_ENA_M  (BIT(12))
#define LEDC_OVF_CNT_LSCH2_INT_ENA_V  0x1
#define LEDC_OVF_CNT_LSCH2_INT_ENA_S  12
/* LEDC_OVF_CNT_LSCH1_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH1_INT_ENA    (BIT(11))
#define LEDC_OVF_CNT_LSCH1_INT_ENA_M  (BIT(11))
#define LEDC_OVF_CNT_LSCH1_INT_ENA_V  0x1
#define LEDC_OVF_CNT_LSCH1_INT_ENA_S  11
/* LEDC_OVF_CNT_LSCH0_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH0_INT_ENA    (BIT(10))
#define LEDC_OVF_CNT_LSCH0_INT_ENA_M  (BIT(10))
#define LEDC_OVF_CNT_LSCH0_INT_ENA_V  0x1
#define LEDC_OVF_CNT_LSCH0_INT_ENA_S  10
/* LEDC_DUTY_CHNG_END_LSCH5_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ENA  (BIT(9))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ENA_M  (BIT(9))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ENA_S  9
/* LEDC_DUTY_CHNG_END_LSCH4_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ENA  (BIT(8))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ENA_M  (BIT(8))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ENA_S  8
/* LEDC_DUTY_CHNG_END_LSCH3_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ENA  (BIT(7))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ENA_M  (BIT(7))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ENA_S  7
/* LEDC_DUTY_CHNG_END_LSCH2_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ENA  (BIT(6))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ENA_M  (BIT(6))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ENA_S  6
/* LEDC_DUTY_CHNG_END_LSCH1_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ENA  (BIT(5))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ENA_M  (BIT(5))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ENA_S  5
/* LEDC_DUTY_CHNG_END_LSCH0_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ENA  (BIT(4))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ENA_M  (BIT(4))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ENA_S  4
/* LEDC_LSTIMER3_OVF_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER3_OVF_INT_ENA  (BIT(3))
#define LEDC_LSTIMER3_OVF_INT_ENA_M  (BIT(3))
#define LEDC_LSTIMER3_OVF_INT_ENA_V  0x1
#define LEDC_LSTIMER3_OVF_INT_ENA_S  3
/* LEDC_LSTIMER2_OVF_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER2_OVF_INT_ENA  (BIT(2))
#define LEDC_LSTIMER2_OVF_INT_ENA_M  (BIT(2))
#define LEDC_LSTIMER2_OVF_INT_ENA_V  0x1
#define LEDC_LSTIMER2_OVF_INT_ENA_S  2
/* LEDC_LSTIMER1_OVF_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER1_OVF_INT_ENA  (BIT(1))
#define LEDC_LSTIMER1_OVF_INT_ENA_M  (BIT(1))
#define LEDC_LSTIMER1_OVF_INT_ENA_V  0x1
#define LEDC_LSTIMER1_OVF_INT_ENA_S  1
/* LEDC_LSTIMER0_OVF_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER0_OVF_INT_ENA  (BIT(0))
#define LEDC_LSTIMER0_OVF_INT_ENA_M  (BIT(0))
#define LEDC_LSTIMER0_OVF_INT_ENA_V  0x1
#define LEDC_LSTIMER0_OVF_INT_ENA_S  0

#define LEDC_INT_CLR_REG          (DR_REG_LEDC_BASE + 0xCC)
/* LEDC_OVF_CNT_LSCH5_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH5_INT_CLR    (BIT(15))
#define LEDC_OVF_CNT_LSCH5_INT_CLR_M  (BIT(15))
#define LEDC_OVF_CNT_LSCH5_INT_CLR_V  0x1
#define LEDC_OVF_CNT_LSCH5_INT_CLR_S  15
/* LEDC_OVF_CNT_LSCH4_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH4_INT_CLR    (BIT(14))
#define LEDC_OVF_CNT_LSCH4_INT_CLR_M  (BIT(14))
#define LEDC_OVF_CNT_LSCH4_INT_CLR_V  0x1
#define LEDC_OVF_CNT_LSCH4_INT_CLR_S  14
/* LEDC_OVF_CNT_LSCH3_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH3_INT_CLR    (BIT(13))
#define LEDC_OVF_CNT_LSCH3_INT_CLR_M  (BIT(13))
#define LEDC_OVF_CNT_LSCH3_INT_CLR_V  0x1
#define LEDC_OVF_CNT_LSCH3_INT_CLR_S  13
/* LEDC_OVF_CNT_LSCH2_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH2_INT_CLR    (BIT(12))
#define LEDC_OVF_CNT_LSCH2_INT_CLR_M  (BIT(12))
#define LEDC_OVF_CNT_LSCH2_INT_CLR_V  0x1
#define LEDC_OVF_CNT_LSCH2_INT_CLR_S  12
/* LEDC_OVF_CNT_LSCH1_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH1_INT_CLR    (BIT(11))
#define LEDC_OVF_CNT_LSCH1_INT_CLR_M  (BIT(11))
#define LEDC_OVF_CNT_LSCH1_INT_CLR_V  0x1
#define LEDC_OVF_CNT_LSCH1_INT_CLR_S  11
/* LEDC_OVF_CNT_LSCH0_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define LEDC_OVF_CNT_LSCH0_INT_CLR    (BIT(10))
#define LEDC_OVF_CNT_LSCH0_INT_CLR_M  (BIT(10))
#define LEDC_OVF_CNT_LSCH0_INT_CLR_V  0x1
#define LEDC_OVF_CNT_LSCH0_INT_CLR_S  10
/* LEDC_DUTY_CHNG_END_LSCH5_INT_CLR : WT ;bitpos:[9] ;default: 1'b0 ; */
/*description: reg_duty_chng_end_lsch5_int_clr..*/
#define LEDC_DUTY_CHNG_END_LSCH5_INT_CLR  (BIT(9))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_CLR_M  (BIT(9))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH5_INT_CLR_S  9
/* LEDC_DUTY_CHNG_END_LSCH4_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH4_INT_CLR  (BIT(8))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_CLR_M  (BIT(8))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH4_INT_CLR_S  8
/* LEDC_DUTY_CHNG_END_LSCH3_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH3_INT_CLR  (BIT(7))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_CLR_M  (BIT(7))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH3_INT_CLR_S  7
/* LEDC_DUTY_CHNG_END_LSCH2_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH2_INT_CLR  (BIT(6))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_CLR_M  (BIT(6))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH2_INT_CLR_S  6
/* LEDC_DUTY_CHNG_END_LSCH1_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH1_INT_CLR  (BIT(5))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_CLR_M  (BIT(5))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH1_INT_CLR_S  5
/* LEDC_DUTY_CHNG_END_LSCH0_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define LEDC_DUTY_CHNG_END_LSCH0_INT_CLR  (BIT(4))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_CLR_M  (BIT(4))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH0_INT_CLR_S  4
/* LEDC_LSTIMER3_OVF_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER3_OVF_INT_CLR  (BIT(3))
#define LEDC_LSTIMER3_OVF_INT_CLR_M  (BIT(3))
#define LEDC_LSTIMER3_OVF_INT_CLR_V  0x1
#define LEDC_LSTIMER3_OVF_INT_CLR_S  3
/* LEDC_LSTIMER2_OVF_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER2_OVF_INT_CLR  (BIT(2))
#define LEDC_LSTIMER2_OVF_INT_CLR_M  (BIT(2))
#define LEDC_LSTIMER2_OVF_INT_CLR_V  0x1
#define LEDC_LSTIMER2_OVF_INT_CLR_S  2
/* LEDC_LSTIMER1_OVF_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER1_OVF_INT_CLR  (BIT(1))
#define LEDC_LSTIMER1_OVF_INT_CLR_M  (BIT(1))
#define LEDC_LSTIMER1_OVF_INT_CLR_V  0x1
#define LEDC_LSTIMER1_OVF_INT_CLR_S  1
/* LEDC_LSTIMER0_OVF_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define LEDC_LSTIMER0_OVF_INT_CLR  (BIT(0))
#define LEDC_LSTIMER0_OVF_INT_CLR_M  (BIT(0))
#define LEDC_LSTIMER0_OVF_INT_CLR_V  0x1
#define LEDC_LSTIMER0_OVF_INT_CLR_S  0

#define LEDC_CONF_REG          (DR_REG_LEDC_BASE + 0x00d0)
/* LEDC_CLK_EN : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define LEDC_CLK_EN  (BIT(31))
#define LEDC_CLK_EN_M  (BIT(31))
#define LEDC_CLK_EN_V  0x1
#define LEDC_CLK_EN_S  31
/* LEDC_APB_CLK_SEL : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define LEDC_APB_CLK_SEL  0x00000003
#define LEDC_APB_CLK_SEL_M  ((LEDC_APB_CLK_SEL_V)<<(LEDC_APB_CLK_SEL_S))
#define LEDC_APB_CLK_SEL_V  0x3
#define LEDC_APB_CLK_SEL_S  0

#define LEDC_DATE_REG          (DR_REG_LEDC_BASE + 0x00FC)
/* LEDC_DATE : R/W ;bitpos:[31:0] ;default: 32'h19061700 ; */
/*description: */
#define LEDC_DATE  0xFFFFFFFF
#define LEDC_DATE_M  ((LEDC_DATE_V)<<(LEDC_DATE_S))
#define LEDC_DATE_V  0xFFFFFFFF
#define LEDC_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_LEDC_REG_H_ */
