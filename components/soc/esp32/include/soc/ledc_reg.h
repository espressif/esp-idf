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
#ifndef _SOC_LEDC_REG_H_
#define _SOC_LEDC_REG_H_


#include "soc.h"
#define LEDC_HSCH0_CONF0_REG          (DR_REG_LEDC_BASE + 0x0000)
/* LEDC_CLK_EN : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: This bit is clock gating control signal. when software config
 LED_PWM internal registers  it controls the register clock.*/
#define LEDC_CLK_EN  (BIT(31))
#define LEDC_CLK_EN_M  (BIT(31))
#define LEDC_CLK_EN_V  0x1
#define LEDC_CLK_EN_S  31
/* LEDC_IDLE_LV_HSCH0 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when high speed channel0 is off.*/
#define LEDC_IDLE_LV_HSCH0  (BIT(3))
#define LEDC_IDLE_LV_HSCH0_M  (BIT(3))
#define LEDC_IDLE_LV_HSCH0_V  0x1
#define LEDC_IDLE_LV_HSCH0_S  3
/* LEDC_SIG_OUT_EN_HSCH0 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for high speed channel0*/
#define LEDC_SIG_OUT_EN_HSCH0  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH0_M  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH0_V  0x1
#define LEDC_SIG_OUT_EN_HSCH0_S  2
/* LEDC_TIMER_SEL_HSCH0 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four high speed timers  the two bits are used to select
 one of them for high speed channel0.  2'b00: seletc hstimer0.   2'b01: select hstimer1.  2'b10: select hstimer2.    2'b11: select hstimer3.*/
#define LEDC_TIMER_SEL_HSCH0  0x00000003
#define LEDC_TIMER_SEL_HSCH0_M  ((LEDC_TIMER_SEL_HSCH0_V)<<(LEDC_TIMER_SEL_HSCH0_S))
#define LEDC_TIMER_SEL_HSCH0_V  0x3
#define LEDC_TIMER_SEL_HSCH0_S  0

#define LEDC_HSCH0_HPOINT_REG          (DR_REG_LEDC_BASE + 0x0004)
/* LEDC_HPOINT_HSCH0 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when htimerx(x=[0 3]) selected
 by high speed channel0 has reached reg_hpoint_hsch0[19:0]*/
#define LEDC_HPOINT_HSCH0  0x000FFFFF
#define LEDC_HPOINT_HSCH0_M  ((LEDC_HPOINT_HSCH0_V)<<(LEDC_HPOINT_HSCH0_S))
#define LEDC_HPOINT_HSCH0_V  0xFFFFF
#define LEDC_HPOINT_HSCH0_S  0

#define LEDC_HSCH0_DUTY_REG          (DR_REG_LEDC_BASE + 0x0008)
/* LEDC_DUTY_HSCH0 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When hstimerx(x=[0
 3]) choosed by high speed channel0  has reached reg_lpoint_hsch0 the output signal changes to low. reg_lpoint_hsch0=(reg_hpoint_hsch0[19:0]+reg_duty_hsch0[24:4])          (1)  reg_lpoint_hsch0=(reg_hpoint_hsch0[19:0]+reg_duty_hsch0[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_HSCH0  0x01FFFFFF
#define LEDC_DUTY_HSCH0_M  ((LEDC_DUTY_HSCH0_V)<<(LEDC_DUTY_HSCH0_S))
#define LEDC_DUTY_HSCH0_V  0x1FFFFFF
#define LEDC_DUTY_HSCH0_S  0

#define LEDC_HSCH0_CONF1_REG          (DR_REG_LEDC_BASE + 0x000C)
/* LEDC_DUTY_START_HSCH0 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch0 reg_duty_cycle_hsch0 and reg_duty_scale_hsch0
 has been configured. these register won't take effect until set reg_duty_start_hsch0. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_HSCH0  (BIT(31))
#define LEDC_DUTY_START_HSCH0_M  (BIT(31))
#define LEDC_DUTY_START_HSCH0_V  0x1
#define LEDC_DUTY_START_HSCH0_S  31
/* LEDC_DUTY_INC_HSCH0 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for high speed channel0.*/
#define LEDC_DUTY_INC_HSCH0  (BIT(30))
#define LEDC_DUTY_INC_HSCH0_M  (BIT(30))
#define LEDC_DUTY_INC_HSCH0_V  0x1
#define LEDC_DUTY_INC_HSCH0_S  30
/* LEDC_DUTY_NUM_HSCH0 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for high speed channel0.*/
#define LEDC_DUTY_NUM_HSCH0  0x000003FF
#define LEDC_DUTY_NUM_HSCH0_M  ((LEDC_DUTY_NUM_HSCH0_V)<<(LEDC_DUTY_NUM_HSCH0_S))
#define LEDC_DUTY_NUM_HSCH0_V  0x3FF
#define LEDC_DUTY_NUM_HSCH0_S  20
/* LEDC_DUTY_CYCLE_HSCH0 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_hsch0 cycles for high speed channel0.*/
#define LEDC_DUTY_CYCLE_HSCH0  0x000003FF
#define LEDC_DUTY_CYCLE_HSCH0_M  ((LEDC_DUTY_CYCLE_HSCH0_V)<<(LEDC_DUTY_CYCLE_HSCH0_S))
#define LEDC_DUTY_CYCLE_HSCH0_V  0x3FF
#define LEDC_DUTY_CYCLE_HSCH0_S  10
/* LEDC_DUTY_SCALE_HSCH0 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 high speed channel0.*/
#define LEDC_DUTY_SCALE_HSCH0  0x000003FF
#define LEDC_DUTY_SCALE_HSCH0_M  ((LEDC_DUTY_SCALE_HSCH0_V)<<(LEDC_DUTY_SCALE_HSCH0_S))
#define LEDC_DUTY_SCALE_HSCH0_V  0x3FF
#define LEDC_DUTY_SCALE_HSCH0_S  0

#define LEDC_HSCH0_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x0010)
/* LEDC_DUTY_HSCH0 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for high speed channel0.*/
#define LEDC_DUTY_HSCH0  0x01FFFFFF
#define LEDC_DUTY_HSCH0_M  ((LEDC_DUTY_HSCH0_V)<<(LEDC_DUTY_HSCH0_S))
#define LEDC_DUTY_HSCH0_V  0x1FFFFFF
#define LEDC_DUTY_HSCH0_S  0

#define LEDC_HSCH1_CONF0_REG          (DR_REG_LEDC_BASE + 0x0014)
/* LEDC_IDLE_LV_HSCH1 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when high speed channel1 is off.*/
#define LEDC_IDLE_LV_HSCH1  (BIT(3))
#define LEDC_IDLE_LV_HSCH1_M  (BIT(3))
#define LEDC_IDLE_LV_HSCH1_V  0x1
#define LEDC_IDLE_LV_HSCH1_S  3
/* LEDC_SIG_OUT_EN_HSCH1 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for high speed channel1*/
#define LEDC_SIG_OUT_EN_HSCH1  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH1_M  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH1_V  0x1
#define LEDC_SIG_OUT_EN_HSCH1_S  2
/* LEDC_TIMER_SEL_HSCH1 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four high speed timers  the two bits are used to select
 one of them for high speed channel1.  2'b00: seletc hstimer0.   2'b01: select hstimer1.  2'b10: select hstimer2.    2'b11: select hstimer3.*/
#define LEDC_TIMER_SEL_HSCH1  0x00000003
#define LEDC_TIMER_SEL_HSCH1_M  ((LEDC_TIMER_SEL_HSCH1_V)<<(LEDC_TIMER_SEL_HSCH1_S))
#define LEDC_TIMER_SEL_HSCH1_V  0x3
#define LEDC_TIMER_SEL_HSCH1_S  0

#define LEDC_HSCH1_HPOINT_REG          (DR_REG_LEDC_BASE + 0x0018)
/* LEDC_HPOINT_HSCH1 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when htimerx(x=[0 3]) selected
 by high speed channel1 has reached reg_hpoint_hsch1[19:0]*/
#define LEDC_HPOINT_HSCH1  0x000FFFFF
#define LEDC_HPOINT_HSCH1_M  ((LEDC_HPOINT_HSCH1_V)<<(LEDC_HPOINT_HSCH1_S))
#define LEDC_HPOINT_HSCH1_V  0xFFFFF
#define LEDC_HPOINT_HSCH1_S  0

#define LEDC_HSCH1_DUTY_REG          (DR_REG_LEDC_BASE + 0x001C)
/* LEDC_DUTY_HSCH1 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When hstimerx(x=[0
 3]) choosed by high speed channel1 has reached reg_lpoint_hsch1 the output signal changes to low. reg_lpoint_hsch1=(reg_hpoint_hsch1[19:0]+reg_duty_hsch1[24:4])          (1)  reg_lpoint_hsch1=(reg_hpoint_hsch1[19:0]+reg_duty_hsch1[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_HSCH1  0x01FFFFFF
#define LEDC_DUTY_HSCH1_M  ((LEDC_DUTY_HSCH1_V)<<(LEDC_DUTY_HSCH1_S))
#define LEDC_DUTY_HSCH1_V  0x1FFFFFF
#define LEDC_DUTY_HSCH1_S  0

#define LEDC_HSCH1_CONF1_REG          (DR_REG_LEDC_BASE + 0x0020)
/* LEDC_DUTY_START_HSCH1 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch1 reg_duty_cycle_hsch1 and reg_duty_scale_hsch1
 has been configured. these register won't take effect until set reg_duty_start_hsch1. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_HSCH1  (BIT(31))
#define LEDC_DUTY_START_HSCH1_M  (BIT(31))
#define LEDC_DUTY_START_HSCH1_V  0x1
#define LEDC_DUTY_START_HSCH1_S  31
/* LEDC_DUTY_INC_HSCH1 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for high speed channel1.*/
#define LEDC_DUTY_INC_HSCH1  (BIT(30))
#define LEDC_DUTY_INC_HSCH1_M  (BIT(30))
#define LEDC_DUTY_INC_HSCH1_V  0x1
#define LEDC_DUTY_INC_HSCH1_S  30
/* LEDC_DUTY_NUM_HSCH1 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for high speed channel1.*/
#define LEDC_DUTY_NUM_HSCH1  0x000003FF
#define LEDC_DUTY_NUM_HSCH1_M  ((LEDC_DUTY_NUM_HSCH1_V)<<(LEDC_DUTY_NUM_HSCH1_S))
#define LEDC_DUTY_NUM_HSCH1_V  0x3FF
#define LEDC_DUTY_NUM_HSCH1_S  20
/* LEDC_DUTY_CYCLE_HSCH1 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_hsch1 cycles for high speed channel1.*/
#define LEDC_DUTY_CYCLE_HSCH1  0x000003FF
#define LEDC_DUTY_CYCLE_HSCH1_M  ((LEDC_DUTY_CYCLE_HSCH1_V)<<(LEDC_DUTY_CYCLE_HSCH1_S))
#define LEDC_DUTY_CYCLE_HSCH1_V  0x3FF
#define LEDC_DUTY_CYCLE_HSCH1_S  10
/* LEDC_DUTY_SCALE_HSCH1 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 high speed channel1.*/
#define LEDC_DUTY_SCALE_HSCH1  0x000003FF
#define LEDC_DUTY_SCALE_HSCH1_M  ((LEDC_DUTY_SCALE_HSCH1_V)<<(LEDC_DUTY_SCALE_HSCH1_S))
#define LEDC_DUTY_SCALE_HSCH1_V  0x3FF
#define LEDC_DUTY_SCALE_HSCH1_S  0

#define LEDC_HSCH1_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x0024)
/* LEDC_DUTY_HSCH1 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for high speed channel1.*/
#define LEDC_DUTY_HSCH1  0x01FFFFFF
#define LEDC_DUTY_HSCH1_M  ((LEDC_DUTY_HSCH1_V)<<(LEDC_DUTY_HSCH1_S))
#define LEDC_DUTY_HSCH1_V  0x1FFFFFF
#define LEDC_DUTY_HSCH1_S  0

#define LEDC_HSCH2_CONF0_REG          (DR_REG_LEDC_BASE + 0x0028)
/* LEDC_IDLE_LV_HSCH2 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when high speed channel2 is off.*/
#define LEDC_IDLE_LV_HSCH2  (BIT(3))
#define LEDC_IDLE_LV_HSCH2_M  (BIT(3))
#define LEDC_IDLE_LV_HSCH2_V  0x1
#define LEDC_IDLE_LV_HSCH2_S  3
/* LEDC_SIG_OUT_EN_HSCH2 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for high speed channel2*/
#define LEDC_SIG_OUT_EN_HSCH2  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH2_M  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH2_V  0x1
#define LEDC_SIG_OUT_EN_HSCH2_S  2
/* LEDC_TIMER_SEL_HSCH2 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four high speed timers  the two bits are used to select
 one of them for high speed channel2.  2'b00: seletc hstimer0.   2'b01: select hstimer1.  2'b10: select hstimer2.    2'b11: select hstimer3.*/
#define LEDC_TIMER_SEL_HSCH2  0x00000003
#define LEDC_TIMER_SEL_HSCH2_M  ((LEDC_TIMER_SEL_HSCH2_V)<<(LEDC_TIMER_SEL_HSCH2_S))
#define LEDC_TIMER_SEL_HSCH2_V  0x3
#define LEDC_TIMER_SEL_HSCH2_S  0

#define LEDC_HSCH2_HPOINT_REG          (DR_REG_LEDC_BASE + 0x002C)
/* LEDC_HPOINT_HSCH2 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when htimerx(x=[0 3]) selected
 by high speed channel2 has reached reg_hpoint_hsch2[19:0]*/
#define LEDC_HPOINT_HSCH2  0x000FFFFF
#define LEDC_HPOINT_HSCH2_M  ((LEDC_HPOINT_HSCH2_V)<<(LEDC_HPOINT_HSCH2_S))
#define LEDC_HPOINT_HSCH2_V  0xFFFFF
#define LEDC_HPOINT_HSCH2_S  0

#define LEDC_HSCH2_DUTY_REG          (DR_REG_LEDC_BASE + 0x0030)
/* LEDC_DUTY_HSCH2 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When hstimerx(x=[0
 3]) choosed by high speed channel2 has reached reg_lpoint_hsch2 the output signal changes to low. reg_lpoint_hsch2=(reg_hpoint_hsch2[19:0]+reg_duty_hsch2[24:4])          (1)  reg_lpoint_hsch2=(reg_hpoint_hsch2[19:0]+reg_duty_hsch2[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_HSCH2  0x01FFFFFF
#define LEDC_DUTY_HSCH2_M  ((LEDC_DUTY_HSCH2_V)<<(LEDC_DUTY_HSCH2_S))
#define LEDC_DUTY_HSCH2_V  0x1FFFFFF
#define LEDC_DUTY_HSCH2_S  0

#define LEDC_HSCH2_CONF1_REG          (DR_REG_LEDC_BASE + 0x0034)
/* LEDC_DUTY_START_HSCH2 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch2 reg_duty_cycle_hsch2 and reg_duty_scale_hsch2
 has been configured. these register won't take effect until set reg_duty_start_hsch2. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_HSCH2  (BIT(31))
#define LEDC_DUTY_START_HSCH2_M  (BIT(31))
#define LEDC_DUTY_START_HSCH2_V  0x1
#define LEDC_DUTY_START_HSCH2_S  31
/* LEDC_DUTY_INC_HSCH2 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for high speed channel2.*/
#define LEDC_DUTY_INC_HSCH2  (BIT(30))
#define LEDC_DUTY_INC_HSCH2_M  (BIT(30))
#define LEDC_DUTY_INC_HSCH2_V  0x1
#define LEDC_DUTY_INC_HSCH2_S  30
/* LEDC_DUTY_NUM_HSCH2 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for high speed channel2.*/
#define LEDC_DUTY_NUM_HSCH2  0x000003FF
#define LEDC_DUTY_NUM_HSCH2_M  ((LEDC_DUTY_NUM_HSCH2_V)<<(LEDC_DUTY_NUM_HSCH2_S))
#define LEDC_DUTY_NUM_HSCH2_V  0x3FF
#define LEDC_DUTY_NUM_HSCH2_S  20
/* LEDC_DUTY_CYCLE_HSCH2 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_hsch2 cycles for high speed channel2.*/
#define LEDC_DUTY_CYCLE_HSCH2  0x000003FF
#define LEDC_DUTY_CYCLE_HSCH2_M  ((LEDC_DUTY_CYCLE_HSCH2_V)<<(LEDC_DUTY_CYCLE_HSCH2_S))
#define LEDC_DUTY_CYCLE_HSCH2_V  0x3FF
#define LEDC_DUTY_CYCLE_HSCH2_S  10
/* LEDC_DUTY_SCALE_HSCH2 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 high speed channel2.*/
#define LEDC_DUTY_SCALE_HSCH2  0x000003FF
#define LEDC_DUTY_SCALE_HSCH2_M  ((LEDC_DUTY_SCALE_HSCH2_V)<<(LEDC_DUTY_SCALE_HSCH2_S))
#define LEDC_DUTY_SCALE_HSCH2_V  0x3FF
#define LEDC_DUTY_SCALE_HSCH2_S  0

#define LEDC_HSCH2_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x0038)
/* LEDC_DUTY_HSCH2 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for high speed channel2.*/
#define LEDC_DUTY_HSCH2  0x01FFFFFF
#define LEDC_DUTY_HSCH2_M  ((LEDC_DUTY_HSCH2_V)<<(LEDC_DUTY_HSCH2_S))
#define LEDC_DUTY_HSCH2_V  0x1FFFFFF
#define LEDC_DUTY_HSCH2_S  0

#define LEDC_HSCH3_CONF0_REG          (DR_REG_LEDC_BASE + 0x003C)
/* LEDC_IDLE_LV_HSCH3 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when high speed channel3 is off.*/
#define LEDC_IDLE_LV_HSCH3  (BIT(3))
#define LEDC_IDLE_LV_HSCH3_M  (BIT(3))
#define LEDC_IDLE_LV_HSCH3_V  0x1
#define LEDC_IDLE_LV_HSCH3_S  3
/* LEDC_SIG_OUT_EN_HSCH3 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for high speed channel3*/
#define LEDC_SIG_OUT_EN_HSCH3  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH3_M  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH3_V  0x1
#define LEDC_SIG_OUT_EN_HSCH3_S  2
/* LEDC_TIMER_SEL_HSCH3 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four high speed timers  the two bits are used to select
 one of them for high speed channel3.  2'b00: seletc hstimer0.   2'b01: select hstimer1.  2'b10: select hstimer2.    2'b11: select hstimer3.*/
#define LEDC_TIMER_SEL_HSCH3  0x00000003
#define LEDC_TIMER_SEL_HSCH3_M  ((LEDC_TIMER_SEL_HSCH3_V)<<(LEDC_TIMER_SEL_HSCH3_S))
#define LEDC_TIMER_SEL_HSCH3_V  0x3
#define LEDC_TIMER_SEL_HSCH3_S  0

#define LEDC_HSCH3_HPOINT_REG          (DR_REG_LEDC_BASE + 0x0040)
/* LEDC_HPOINT_HSCH3 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when htimerx(x=[0 3]) selected
 by high speed channel3 has reached reg_hpoint_hsch3[19:0]*/
#define LEDC_HPOINT_HSCH3  0x000FFFFF
#define LEDC_HPOINT_HSCH3_M  ((LEDC_HPOINT_HSCH3_V)<<(LEDC_HPOINT_HSCH3_S))
#define LEDC_HPOINT_HSCH3_V  0xFFFFF
#define LEDC_HPOINT_HSCH3_S  0

#define LEDC_HSCH3_DUTY_REG          (DR_REG_LEDC_BASE + 0x0044)
/* LEDC_DUTY_HSCH3 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When hstimerx(x=[0
 3]) choosed by high speed channel3 has reached reg_lpoint_hsch3 the output signal changes to low. reg_lpoint_hsch3=(reg_hpoint_hsch3[19:0]+reg_duty_hsch3[24:4])          (1)  reg_lpoint_hsch3=(reg_hpoint_hsch3[19:0]+reg_duty_hsch3[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_HSCH3  0x01FFFFFF
#define LEDC_DUTY_HSCH3_M  ((LEDC_DUTY_HSCH3_V)<<(LEDC_DUTY_HSCH3_S))
#define LEDC_DUTY_HSCH3_V  0x1FFFFFF
#define LEDC_DUTY_HSCH3_S  0

#define LEDC_HSCH3_CONF1_REG          (DR_REG_LEDC_BASE + 0x0048)
/* LEDC_DUTY_START_HSCH3 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch3 reg_duty_cycle_hsch3 and reg_duty_scale_hsch3
 has been configured. these register won't take effect until set reg_duty_start_hsch3. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_HSCH3  (BIT(31))
#define LEDC_DUTY_START_HSCH3_M  (BIT(31))
#define LEDC_DUTY_START_HSCH3_V  0x1
#define LEDC_DUTY_START_HSCH3_S  31
/* LEDC_DUTY_INC_HSCH3 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for high speed channel3.*/
#define LEDC_DUTY_INC_HSCH3  (BIT(30))
#define LEDC_DUTY_INC_HSCH3_M  (BIT(30))
#define LEDC_DUTY_INC_HSCH3_V  0x1
#define LEDC_DUTY_INC_HSCH3_S  30
/* LEDC_DUTY_NUM_HSCH3 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for high speed channel3.*/
#define LEDC_DUTY_NUM_HSCH3  0x000003FF
#define LEDC_DUTY_NUM_HSCH3_M  ((LEDC_DUTY_NUM_HSCH3_V)<<(LEDC_DUTY_NUM_HSCH3_S))
#define LEDC_DUTY_NUM_HSCH3_V  0x3FF
#define LEDC_DUTY_NUM_HSCH3_S  20
/* LEDC_DUTY_CYCLE_HSCH3 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_hsch3 cycles for high speed channel3.*/
#define LEDC_DUTY_CYCLE_HSCH3  0x000003FF
#define LEDC_DUTY_CYCLE_HSCH3_M  ((LEDC_DUTY_CYCLE_HSCH3_V)<<(LEDC_DUTY_CYCLE_HSCH3_S))
#define LEDC_DUTY_CYCLE_HSCH3_V  0x3FF
#define LEDC_DUTY_CYCLE_HSCH3_S  10
/* LEDC_DUTY_SCALE_HSCH3 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 high speed channel3.*/
#define LEDC_DUTY_SCALE_HSCH3  0x000003FF
#define LEDC_DUTY_SCALE_HSCH3_M  ((LEDC_DUTY_SCALE_HSCH3_V)<<(LEDC_DUTY_SCALE_HSCH3_S))
#define LEDC_DUTY_SCALE_HSCH3_V  0x3FF
#define LEDC_DUTY_SCALE_HSCH3_S  0

#define LEDC_HSCH3_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x004C)
/* LEDC_DUTY_HSCH3 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for high speed channel3.*/
#define LEDC_DUTY_HSCH3  0x01FFFFFF
#define LEDC_DUTY_HSCH3_M  ((LEDC_DUTY_HSCH3_V)<<(LEDC_DUTY_HSCH3_S))
#define LEDC_DUTY_HSCH3_V  0x1FFFFFF
#define LEDC_DUTY_HSCH3_S  0

#define LEDC_HSCH4_CONF0_REG          (DR_REG_LEDC_BASE + 0x0050)
/* LEDC_IDLE_LV_HSCH4 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when high speed channel4 is off.*/
#define LEDC_IDLE_LV_HSCH4  (BIT(3))
#define LEDC_IDLE_LV_HSCH4_M  (BIT(3))
#define LEDC_IDLE_LV_HSCH4_V  0x1
#define LEDC_IDLE_LV_HSCH4_S  3
/* LEDC_SIG_OUT_EN_HSCH4 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for high speed channel4*/
#define LEDC_SIG_OUT_EN_HSCH4  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH4_M  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH4_V  0x1
#define LEDC_SIG_OUT_EN_HSCH4_S  2
/* LEDC_TIMER_SEL_HSCH4 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four high speed timers  the two bits are used to select
 one of them for high speed channel4.  2'b00: seletc hstimer0.   2'b01: select hstimer1.  2'b10: select hstimer2.    2'b11: select hstimer3.*/
#define LEDC_TIMER_SEL_HSCH4  0x00000003
#define LEDC_TIMER_SEL_HSCH4_M  ((LEDC_TIMER_SEL_HSCH4_V)<<(LEDC_TIMER_SEL_HSCH4_S))
#define LEDC_TIMER_SEL_HSCH4_V  0x3
#define LEDC_TIMER_SEL_HSCH4_S  0

#define LEDC_HSCH4_HPOINT_REG          (DR_REG_LEDC_BASE + 0x0054)
/* LEDC_HPOINT_HSCH4 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when htimerx(x=[0 3]) selected
 by high speed channel4 has reached reg_hpoint_hsch4[19:0]*/
#define LEDC_HPOINT_HSCH4  0x000FFFFF
#define LEDC_HPOINT_HSCH4_M  ((LEDC_HPOINT_HSCH4_V)<<(LEDC_HPOINT_HSCH4_S))
#define LEDC_HPOINT_HSCH4_V  0xFFFFF
#define LEDC_HPOINT_HSCH4_S  0

#define LEDC_HSCH4_DUTY_REG          (DR_REG_LEDC_BASE + 0x0058)
/* LEDC_DUTY_HSCH4 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When hstimerx(x=[0
 3]) choosed by high speed channel4 has reached reg_lpoint_hsch4 the output signal changes to low. reg_lpoint_hsch4=(reg_hpoint_hsch4[19:0]+reg_duty_hsch4[24:4])          (1)  reg_lpoint_hsch4=(reg_hpoint_hsch4[19:0]+reg_duty_hsch4[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_HSCH4  0x01FFFFFF
#define LEDC_DUTY_HSCH4_M  ((LEDC_DUTY_HSCH4_V)<<(LEDC_DUTY_HSCH4_S))
#define LEDC_DUTY_HSCH4_V  0x1FFFFFF
#define LEDC_DUTY_HSCH4_S  0

#define LEDC_HSCH4_CONF1_REG          (DR_REG_LEDC_BASE + 0x005C)
/* LEDC_DUTY_START_HSCH4 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch1 reg_duty_cycle_hsch1 and reg_duty_scale_hsch1
 has been configured. these register won't take effect until set reg_duty_start_hsch1. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_HSCH4  (BIT(31))
#define LEDC_DUTY_START_HSCH4_M  (BIT(31))
#define LEDC_DUTY_START_HSCH4_V  0x1
#define LEDC_DUTY_START_HSCH4_S  31
/* LEDC_DUTY_INC_HSCH4 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for high speed channel4.*/
#define LEDC_DUTY_INC_HSCH4  (BIT(30))
#define LEDC_DUTY_INC_HSCH4_M  (BIT(30))
#define LEDC_DUTY_INC_HSCH4_V  0x1
#define LEDC_DUTY_INC_HSCH4_S  30
/* LEDC_DUTY_NUM_HSCH4 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for high speed channel1.*/
#define LEDC_DUTY_NUM_HSCH4  0x000003FF
#define LEDC_DUTY_NUM_HSCH4_M  ((LEDC_DUTY_NUM_HSCH4_V)<<(LEDC_DUTY_NUM_HSCH4_S))
#define LEDC_DUTY_NUM_HSCH4_V  0x3FF
#define LEDC_DUTY_NUM_HSCH4_S  20
/* LEDC_DUTY_CYCLE_HSCH4 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_hsch4 cycles for high speed channel4.*/
#define LEDC_DUTY_CYCLE_HSCH4  0x000003FF
#define LEDC_DUTY_CYCLE_HSCH4_M  ((LEDC_DUTY_CYCLE_HSCH4_V)<<(LEDC_DUTY_CYCLE_HSCH4_S))
#define LEDC_DUTY_CYCLE_HSCH4_V  0x3FF
#define LEDC_DUTY_CYCLE_HSCH4_S  10
/* LEDC_DUTY_SCALE_HSCH4 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 high speed channel4.*/
#define LEDC_DUTY_SCALE_HSCH4  0x000003FF
#define LEDC_DUTY_SCALE_HSCH4_M  ((LEDC_DUTY_SCALE_HSCH4_V)<<(LEDC_DUTY_SCALE_HSCH4_S))
#define LEDC_DUTY_SCALE_HSCH4_V  0x3FF
#define LEDC_DUTY_SCALE_HSCH4_S  0

#define LEDC_HSCH4_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x0060)
/* LEDC_DUTY_HSCH4 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for high speed channel4.*/
#define LEDC_DUTY_HSCH4  0x01FFFFFF
#define LEDC_DUTY_HSCH4_M  ((LEDC_DUTY_HSCH4_V)<<(LEDC_DUTY_HSCH4_S))
#define LEDC_DUTY_HSCH4_V  0x1FFFFFF
#define LEDC_DUTY_HSCH4_S  0

#define LEDC_HSCH5_CONF0_REG          (DR_REG_LEDC_BASE + 0x0064)
/* LEDC_IDLE_LV_HSCH5 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when high speed channel5 is off.*/
#define LEDC_IDLE_LV_HSCH5  (BIT(3))
#define LEDC_IDLE_LV_HSCH5_M  (BIT(3))
#define LEDC_IDLE_LV_HSCH5_V  0x1
#define LEDC_IDLE_LV_HSCH5_S  3
/* LEDC_SIG_OUT_EN_HSCH5 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for high speed channel5.*/
#define LEDC_SIG_OUT_EN_HSCH5  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH5_M  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH5_V  0x1
#define LEDC_SIG_OUT_EN_HSCH5_S  2
/* LEDC_TIMER_SEL_HSCH5 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four high speed timers  the two bits are used to select
 one of them for high speed channel5.  2'b00: seletc hstimer0.   2'b01: select hstimer1.  2'b10: select hstimer2.    2'b11: select hstimer3.*/
#define LEDC_TIMER_SEL_HSCH5  0x00000003
#define LEDC_TIMER_SEL_HSCH5_M  ((LEDC_TIMER_SEL_HSCH5_V)<<(LEDC_TIMER_SEL_HSCH5_S))
#define LEDC_TIMER_SEL_HSCH5_V  0x3
#define LEDC_TIMER_SEL_HSCH5_S  0

#define LEDC_HSCH5_HPOINT_REG          (DR_REG_LEDC_BASE + 0x0068)
/* LEDC_HPOINT_HSCH5 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when htimerx(x=[0 3]) selected
 by high speed channel5 has reached reg_hpoint_hsch5[19:0]*/
#define LEDC_HPOINT_HSCH5  0x000FFFFF
#define LEDC_HPOINT_HSCH5_M  ((LEDC_HPOINT_HSCH5_V)<<(LEDC_HPOINT_HSCH5_S))
#define LEDC_HPOINT_HSCH5_V  0xFFFFF
#define LEDC_HPOINT_HSCH5_S  0

#define LEDC_HSCH5_DUTY_REG          (DR_REG_LEDC_BASE + 0x006C)
/* LEDC_DUTY_HSCH5 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When hstimerx(x=[0
 3]) choosed by high speed channel5 has reached reg_lpoint_hsch5 the output signal changes to low. reg_lpoint_hsch5=(reg_hpoint_hsch5[19:0]+reg_duty_hsch5[24:4])          (1)  reg_lpoint_hsch5=(reg_hpoint_hsch5[19:0]+reg_duty_hsch5[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_HSCH5  0x01FFFFFF
#define LEDC_DUTY_HSCH5_M  ((LEDC_DUTY_HSCH5_V)<<(LEDC_DUTY_HSCH5_S))
#define LEDC_DUTY_HSCH5_V  0x1FFFFFF
#define LEDC_DUTY_HSCH5_S  0

#define LEDC_HSCH5_CONF1_REG          (DR_REG_LEDC_BASE + 0x0070)
/* LEDC_DUTY_START_HSCH5 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch5 reg_duty_cycle_hsch5 and reg_duty_scale_hsch5
 has been configured. these register won't take effect until set reg_duty_start_hsch5. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_HSCH5  (BIT(31))
#define LEDC_DUTY_START_HSCH5_M  (BIT(31))
#define LEDC_DUTY_START_HSCH5_V  0x1
#define LEDC_DUTY_START_HSCH5_S  31
/* LEDC_DUTY_INC_HSCH5 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for high speed channel5.*/
#define LEDC_DUTY_INC_HSCH5  (BIT(30))
#define LEDC_DUTY_INC_HSCH5_M  (BIT(30))
#define LEDC_DUTY_INC_HSCH5_V  0x1
#define LEDC_DUTY_INC_HSCH5_S  30
/* LEDC_DUTY_NUM_HSCH5 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for high speed channel5.*/
#define LEDC_DUTY_NUM_HSCH5  0x000003FF
#define LEDC_DUTY_NUM_HSCH5_M  ((LEDC_DUTY_NUM_HSCH5_V)<<(LEDC_DUTY_NUM_HSCH5_S))
#define LEDC_DUTY_NUM_HSCH5_V  0x3FF
#define LEDC_DUTY_NUM_HSCH5_S  20
/* LEDC_DUTY_CYCLE_HSCH5 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_hsch5 cycles for high speed channel5.*/
#define LEDC_DUTY_CYCLE_HSCH5  0x000003FF
#define LEDC_DUTY_CYCLE_HSCH5_M  ((LEDC_DUTY_CYCLE_HSCH5_V)<<(LEDC_DUTY_CYCLE_HSCH5_S))
#define LEDC_DUTY_CYCLE_HSCH5_V  0x3FF
#define LEDC_DUTY_CYCLE_HSCH5_S  10
/* LEDC_DUTY_SCALE_HSCH5 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 high speed channel5.*/
#define LEDC_DUTY_SCALE_HSCH5  0x000003FF
#define LEDC_DUTY_SCALE_HSCH5_M  ((LEDC_DUTY_SCALE_HSCH5_V)<<(LEDC_DUTY_SCALE_HSCH5_S))
#define LEDC_DUTY_SCALE_HSCH5_V  0x3FF
#define LEDC_DUTY_SCALE_HSCH5_S  0

#define LEDC_HSCH5_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x0074)
/* LEDC_DUTY_HSCH5 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for high speed channel5.*/
#define LEDC_DUTY_HSCH5  0x01FFFFFF
#define LEDC_DUTY_HSCH5_M  ((LEDC_DUTY_HSCH5_V)<<(LEDC_DUTY_HSCH5_S))
#define LEDC_DUTY_HSCH5_V  0x1FFFFFF
#define LEDC_DUTY_HSCH5_S  0

#define LEDC_HSCH6_CONF0_REG          (DR_REG_LEDC_BASE + 0x0078)
/* LEDC_IDLE_LV_HSCH6 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when high speed channel6 is off.*/
#define LEDC_IDLE_LV_HSCH6  (BIT(3))
#define LEDC_IDLE_LV_HSCH6_M  (BIT(3))
#define LEDC_IDLE_LV_HSCH6_V  0x1
#define LEDC_IDLE_LV_HSCH6_S  3
/* LEDC_SIG_OUT_EN_HSCH6 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for high speed channel6*/
#define LEDC_SIG_OUT_EN_HSCH6  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH6_M  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH6_V  0x1
#define LEDC_SIG_OUT_EN_HSCH6_S  2
/* LEDC_TIMER_SEL_HSCH6 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four high speed timers  the two bits are used to select
 one of them for high speed channel6.  2'b00: seletc hstimer0.   2'b01: select hstimer1.  2'b10: select hstimer2.    2'b11: select hstimer3.*/
#define LEDC_TIMER_SEL_HSCH6  0x00000003
#define LEDC_TIMER_SEL_HSCH6_M  ((LEDC_TIMER_SEL_HSCH6_V)<<(LEDC_TIMER_SEL_HSCH6_S))
#define LEDC_TIMER_SEL_HSCH6_V  0x3
#define LEDC_TIMER_SEL_HSCH6_S  0

#define LEDC_HSCH6_HPOINT_REG          (DR_REG_LEDC_BASE + 0x007C)
/* LEDC_HPOINT_HSCH6 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when htimerx(x=[0 3]) selected
 by high speed channel6 has reached reg_hpoint_hsch6[19:0]*/
#define LEDC_HPOINT_HSCH6  0x000FFFFF
#define LEDC_HPOINT_HSCH6_M  ((LEDC_HPOINT_HSCH6_V)<<(LEDC_HPOINT_HSCH6_S))
#define LEDC_HPOINT_HSCH6_V  0xFFFFF
#define LEDC_HPOINT_HSCH6_S  0

#define LEDC_HSCH6_DUTY_REG          (DR_REG_LEDC_BASE + 0x0080)
/* LEDC_DUTY_HSCH6 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When hstimerx(x=[0
 3]) choosed by high speed channel6 has reached reg_lpoint_hsch6 the output signal changes to low. reg_lpoint_hsch6=(reg_hpoint_hsch6[19:0]+reg_duty_hsch6[24:4])          (1)  reg_lpoint_hsch6=(reg_hpoint_hsch6[19:0]+reg_duty_hsch6[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_HSCH6  0x01FFFFFF
#define LEDC_DUTY_HSCH6_M  ((LEDC_DUTY_HSCH6_V)<<(LEDC_DUTY_HSCH6_S))
#define LEDC_DUTY_HSCH6_V  0x1FFFFFF
#define LEDC_DUTY_HSCH6_S  0

#define LEDC_HSCH6_CONF1_REG          (DR_REG_LEDC_BASE + 0x0084)
/* LEDC_DUTY_START_HSCH6 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch1 reg_duty_cycle_hsch1 and reg_duty_scale_hsch1
 has been configured. these register won't take effect until set reg_duty_start_hsch1. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_HSCH6  (BIT(31))
#define LEDC_DUTY_START_HSCH6_M  (BIT(31))
#define LEDC_DUTY_START_HSCH6_V  0x1
#define LEDC_DUTY_START_HSCH6_S  31
/* LEDC_DUTY_INC_HSCH6 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for high speed channel6.*/
#define LEDC_DUTY_INC_HSCH6  (BIT(30))
#define LEDC_DUTY_INC_HSCH6_M  (BIT(30))
#define LEDC_DUTY_INC_HSCH6_V  0x1
#define LEDC_DUTY_INC_HSCH6_S  30
/* LEDC_DUTY_NUM_HSCH6 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for high speed channel6.*/
#define LEDC_DUTY_NUM_HSCH6  0x000003FF
#define LEDC_DUTY_NUM_HSCH6_M  ((LEDC_DUTY_NUM_HSCH6_V)<<(LEDC_DUTY_NUM_HSCH6_S))
#define LEDC_DUTY_NUM_HSCH6_V  0x3FF
#define LEDC_DUTY_NUM_HSCH6_S  20
/* LEDC_DUTY_CYCLE_HSCH6 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_hsch6 cycles for high speed channel6.*/
#define LEDC_DUTY_CYCLE_HSCH6  0x000003FF
#define LEDC_DUTY_CYCLE_HSCH6_M  ((LEDC_DUTY_CYCLE_HSCH6_V)<<(LEDC_DUTY_CYCLE_HSCH6_S))
#define LEDC_DUTY_CYCLE_HSCH6_V  0x3FF
#define LEDC_DUTY_CYCLE_HSCH6_S  10
/* LEDC_DUTY_SCALE_HSCH6 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 high speed channel6.*/
#define LEDC_DUTY_SCALE_HSCH6  0x000003FF
#define LEDC_DUTY_SCALE_HSCH6_M  ((LEDC_DUTY_SCALE_HSCH6_V)<<(LEDC_DUTY_SCALE_HSCH6_S))
#define LEDC_DUTY_SCALE_HSCH6_V  0x3FF
#define LEDC_DUTY_SCALE_HSCH6_S  0

#define LEDC_HSCH6_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x0088)
/* LEDC_DUTY_HSCH6 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for high speed channel6.*/
#define LEDC_DUTY_HSCH6  0x01FFFFFF
#define LEDC_DUTY_HSCH6_M  ((LEDC_DUTY_HSCH6_V)<<(LEDC_DUTY_HSCH6_S))
#define LEDC_DUTY_HSCH6_V  0x1FFFFFF
#define LEDC_DUTY_HSCH6_S  0

#define LEDC_HSCH7_CONF0_REG          (DR_REG_LEDC_BASE + 0x008C)
/* LEDC_IDLE_LV_HSCH7 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when high speed channel7 is off.*/
#define LEDC_IDLE_LV_HSCH7  (BIT(3))
#define LEDC_IDLE_LV_HSCH7_M  (BIT(3))
#define LEDC_IDLE_LV_HSCH7_V  0x1
#define LEDC_IDLE_LV_HSCH7_S  3
/* LEDC_SIG_OUT_EN_HSCH7 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for high speed channel7.*/
#define LEDC_SIG_OUT_EN_HSCH7  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH7_M  (BIT(2))
#define LEDC_SIG_OUT_EN_HSCH7_V  0x1
#define LEDC_SIG_OUT_EN_HSCH7_S  2
/* LEDC_TIMER_SEL_HSCH7 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four high speed timers  the two bits are used to select
 one of them for high speed channel7.  2'b00: seletc hstimer0.   2'b01: select hstimer1.  2'b10: select hstimer2.    2'b11: select hstimer3.*/
#define LEDC_TIMER_SEL_HSCH7  0x00000003
#define LEDC_TIMER_SEL_HSCH7_M  ((LEDC_TIMER_SEL_HSCH7_V)<<(LEDC_TIMER_SEL_HSCH7_S))
#define LEDC_TIMER_SEL_HSCH7_V  0x3
#define LEDC_TIMER_SEL_HSCH7_S  0

#define LEDC_HSCH7_HPOINT_REG          (DR_REG_LEDC_BASE + 0x0090)
/* LEDC_HPOINT_HSCH7 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when htimerx(x=[0 3]) selected
 by high speed channel7 has reached reg_hpoint_hsch7[19:0]*/
#define LEDC_HPOINT_HSCH7  0x000FFFFF
#define LEDC_HPOINT_HSCH7_M  ((LEDC_HPOINT_HSCH7_V)<<(LEDC_HPOINT_HSCH7_S))
#define LEDC_HPOINT_HSCH7_V  0xFFFFF
#define LEDC_HPOINT_HSCH7_S  0

#define LEDC_HSCH7_DUTY_REG          (DR_REG_LEDC_BASE + 0x0094)
/* LEDC_DUTY_HSCH7 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When hstimerx(x=[0
 3]) choosed by high speed channel7 has reached reg_lpoint_hsch7 the output signal changes to low. reg_lpoint_hsch7=(reg_hpoint_hsch7[19:0]+reg_duty_hsch7[24:4])          (1)  reg_lpoint_hsch7=(reg_hpoint_hsch7[19:0]+reg_duty_hsch7[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_HSCH7  0x01FFFFFF
#define LEDC_DUTY_HSCH7_M  ((LEDC_DUTY_HSCH7_V)<<(LEDC_DUTY_HSCH7_S))
#define LEDC_DUTY_HSCH7_V  0x1FFFFFF
#define LEDC_DUTY_HSCH7_S  0

#define LEDC_HSCH7_CONF1_REG          (DR_REG_LEDC_BASE + 0x0098)
/* LEDC_DUTY_START_HSCH7 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch1 reg_duty_cycle_hsch1 and reg_duty_scale_hsch1
 has been configured. these register won't take effect until set reg_duty_start_hsch1. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_HSCH7  (BIT(31))
#define LEDC_DUTY_START_HSCH7_M  (BIT(31))
#define LEDC_DUTY_START_HSCH7_V  0x1
#define LEDC_DUTY_START_HSCH7_S  31
/* LEDC_DUTY_INC_HSCH7 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for high speed channel6.*/
#define LEDC_DUTY_INC_HSCH7  (BIT(30))
#define LEDC_DUTY_INC_HSCH7_M  (BIT(30))
#define LEDC_DUTY_INC_HSCH7_V  0x1
#define LEDC_DUTY_INC_HSCH7_S  30
/* LEDC_DUTY_NUM_HSCH7 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for high speed channel6.*/
#define LEDC_DUTY_NUM_HSCH7  0x000003FF
#define LEDC_DUTY_NUM_HSCH7_M  ((LEDC_DUTY_NUM_HSCH7_V)<<(LEDC_DUTY_NUM_HSCH7_S))
#define LEDC_DUTY_NUM_HSCH7_V  0x3FF
#define LEDC_DUTY_NUM_HSCH7_S  20
/* LEDC_DUTY_CYCLE_HSCH7 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_hsch7 cycles for high speed channel7.*/
#define LEDC_DUTY_CYCLE_HSCH7  0x000003FF
#define LEDC_DUTY_CYCLE_HSCH7_M  ((LEDC_DUTY_CYCLE_HSCH7_V)<<(LEDC_DUTY_CYCLE_HSCH7_S))
#define LEDC_DUTY_CYCLE_HSCH7_V  0x3FF
#define LEDC_DUTY_CYCLE_HSCH7_S  10
/* LEDC_DUTY_SCALE_HSCH7 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 high speed channel7.*/
#define LEDC_DUTY_SCALE_HSCH7  0x000003FF
#define LEDC_DUTY_SCALE_HSCH7_M  ((LEDC_DUTY_SCALE_HSCH7_V)<<(LEDC_DUTY_SCALE_HSCH7_S))
#define LEDC_DUTY_SCALE_HSCH7_V  0x3FF
#define LEDC_DUTY_SCALE_HSCH7_S  0

#define LEDC_HSCH7_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x009C)
/* LEDC_DUTY_HSCH7 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for high speed channel7.*/
#define LEDC_DUTY_HSCH7  0x01FFFFFF
#define LEDC_DUTY_HSCH7_M  ((LEDC_DUTY_HSCH7_V)<<(LEDC_DUTY_HSCH7_S))
#define LEDC_DUTY_HSCH7_V  0x1FFFFFF
#define LEDC_DUTY_HSCH7_S  0

#define LEDC_LSCH0_CONF0_REG          (DR_REG_LEDC_BASE + 0x00A0)
/* LEDC_PARA_UP_LSCH0 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: This bit is used to update register LEDC_LSCH0_HPOINT and LEDC_LSCH0_DUTY
 for low speed channel0.*/
#define LEDC_PARA_UP_LSCH0  (BIT(4))
#define LEDC_PARA_UP_LSCH0_M  (BIT(4))
#define LEDC_PARA_UP_LSCH0_V  0x1
#define LEDC_PARA_UP_LSCH0_S  4
/* LEDC_IDLE_LV_LSCH0 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when low speed channel0 is off.*/
#define LEDC_IDLE_LV_LSCH0  (BIT(3))
#define LEDC_IDLE_LV_LSCH0_M  (BIT(3))
#define LEDC_IDLE_LV_LSCH0_V  0x1
#define LEDC_IDLE_LV_LSCH0_S  3
/* LEDC_SIG_OUT_EN_LSCH0 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for low speed channel0.*/
#define LEDC_SIG_OUT_EN_LSCH0  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH0_M  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH0_V  0x1
#define LEDC_SIG_OUT_EN_LSCH0_S  2
/* LEDC_TIMER_SEL_LSCH0 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four low speed timers  the two bits are used to select
 one of them for low speed channel0.  2'b00: seletc lstimer0.   2'b01: select lstimer1.  2'b10: select lstimer2.    2'b11: select lstimer3.*/
#define LEDC_TIMER_SEL_LSCH0  0x00000003
#define LEDC_TIMER_SEL_LSCH0_M  ((LEDC_TIMER_SEL_LSCH0_V)<<(LEDC_TIMER_SEL_LSCH0_S))
#define LEDC_TIMER_SEL_LSCH0_V  0x3
#define LEDC_TIMER_SEL_LSCH0_S  0

#define LEDC_LSCH0_HPOINT_REG          (DR_REG_LEDC_BASE + 0x00A4)
/* LEDC_HPOINT_LSCH0 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when lstimerx(x=[0 3]) selected
 by low speed channel0 has reached reg_hpoint_lsch0[19:0]*/
#define LEDC_HPOINT_LSCH0  0x000FFFFF
#define LEDC_HPOINT_LSCH0_M  ((LEDC_HPOINT_LSCH0_V)<<(LEDC_HPOINT_LSCH0_S))
#define LEDC_HPOINT_LSCH0_V  0xFFFFF
#define LEDC_HPOINT_LSCH0_S  0

#define LEDC_LSCH0_DUTY_REG          (DR_REG_LEDC_BASE + 0x00A8)
/* LEDC_DUTY_LSCH0 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When lstimerx(x=[0
 3]) choosed by low speed channel0 has reached reg_lpoint_lsch0 the output signal changes to low. reg_lpoint_lsch0=(reg_hpoint_lsch0[19:0]+reg_duty_lsch0[24:4])          (1)  reg_lpoint_lsch0=(reg_hpoint_lsch0[19:0]+reg_duty_lsch0[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_LSCH0  0x01FFFFFF
#define LEDC_DUTY_LSCH0_M  ((LEDC_DUTY_LSCH0_V)<<(LEDC_DUTY_LSCH0_S))
#define LEDC_DUTY_LSCH0_V  0x1FFFFFF
#define LEDC_DUTY_LSCH0_S  0

#define LEDC_LSCH0_CONF1_REG          (DR_REG_LEDC_BASE + 0x00AC)
/* LEDC_DUTY_START_LSCH0 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch1 reg_duty_cycle_hsch1 and reg_duty_scale_hsch1
 has been configured. these register won't take effect until set reg_duty_start_hsch1. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_LSCH0  (BIT(31))
#define LEDC_DUTY_START_LSCH0_M  (BIT(31))
#define LEDC_DUTY_START_LSCH0_V  0x1
#define LEDC_DUTY_START_LSCH0_S  31
/* LEDC_DUTY_INC_LSCH0 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for low speed channel6.*/
#define LEDC_DUTY_INC_LSCH0  (BIT(30))
#define LEDC_DUTY_INC_LSCH0_M  (BIT(30))
#define LEDC_DUTY_INC_LSCH0_V  0x1
#define LEDC_DUTY_INC_LSCH0_S  30
/* LEDC_DUTY_NUM_LSCH0 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for low speed channel6.*/
#define LEDC_DUTY_NUM_LSCH0  0x000003FF
#define LEDC_DUTY_NUM_LSCH0_M  ((LEDC_DUTY_NUM_LSCH0_V)<<(LEDC_DUTY_NUM_LSCH0_S))
#define LEDC_DUTY_NUM_LSCH0_V  0x3FF
#define LEDC_DUTY_NUM_LSCH0_S  20
/* LEDC_DUTY_CYCLE_LSCH0 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_lsch0 cycles for low speed channel0.*/
#define LEDC_DUTY_CYCLE_LSCH0  0x000003FF
#define LEDC_DUTY_CYCLE_LSCH0_M  ((LEDC_DUTY_CYCLE_LSCH0_V)<<(LEDC_DUTY_CYCLE_LSCH0_S))
#define LEDC_DUTY_CYCLE_LSCH0_V  0x3FF
#define LEDC_DUTY_CYCLE_LSCH0_S  10
/* LEDC_DUTY_SCALE_LSCH0 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 low speed channel0.*/
#define LEDC_DUTY_SCALE_LSCH0  0x000003FF
#define LEDC_DUTY_SCALE_LSCH0_M  ((LEDC_DUTY_SCALE_LSCH0_V)<<(LEDC_DUTY_SCALE_LSCH0_S))
#define LEDC_DUTY_SCALE_LSCH0_V  0x3FF
#define LEDC_DUTY_SCALE_LSCH0_S  0

#define LEDC_LSCH0_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x00B0)
/* LEDC_DUTY_LSCH0 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for low speed channel0.*/
#define LEDC_DUTY_LSCH0  0x01FFFFFF
#define LEDC_DUTY_LSCH0_M  ((LEDC_DUTY_LSCH0_V)<<(LEDC_DUTY_LSCH0_S))
#define LEDC_DUTY_LSCH0_V  0x1FFFFFF
#define LEDC_DUTY_LSCH0_S  0

#define LEDC_LSCH1_CONF0_REG          (DR_REG_LEDC_BASE + 0x00B4)
/* LEDC_PARA_UP_LSCH1 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: This bit is used to update register LEDC_LSCH1_HPOINT and LEDC_LSCH1_DUTY
 for low speed channel1.*/
#define LEDC_PARA_UP_LSCH1  (BIT(4))
#define LEDC_PARA_UP_LSCH1_M  (BIT(4))
#define LEDC_PARA_UP_LSCH1_V  0x1
#define LEDC_PARA_UP_LSCH1_S  4
/* LEDC_IDLE_LV_LSCH1 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when low speed channel1 is off.*/
#define LEDC_IDLE_LV_LSCH1  (BIT(3))
#define LEDC_IDLE_LV_LSCH1_M  (BIT(3))
#define LEDC_IDLE_LV_LSCH1_V  0x1
#define LEDC_IDLE_LV_LSCH1_S  3
/* LEDC_SIG_OUT_EN_LSCH1 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for low speed channel1.*/
#define LEDC_SIG_OUT_EN_LSCH1  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH1_M  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH1_V  0x1
#define LEDC_SIG_OUT_EN_LSCH1_S  2
/* LEDC_TIMER_SEL_LSCH1 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four low speed timers  the two bits are used to select
 one of them for low speed channel1.  2'b00: seletc lstimer0.   2'b01: select lstimer1.  2'b10: select lstimer2.    2'b11: select lstimer3.*/
#define LEDC_TIMER_SEL_LSCH1  0x00000003
#define LEDC_TIMER_SEL_LSCH1_M  ((LEDC_TIMER_SEL_LSCH1_V)<<(LEDC_TIMER_SEL_LSCH1_S))
#define LEDC_TIMER_SEL_LSCH1_V  0x3
#define LEDC_TIMER_SEL_LSCH1_S  0

#define LEDC_LSCH1_HPOINT_REG          (DR_REG_LEDC_BASE + 0x00B8)
/* LEDC_HPOINT_LSCH1 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when lstimerx(x=[0 3]) selected
 by low speed channel1 has reached reg_hpoint_lsch1[19:0]*/
#define LEDC_HPOINT_LSCH1  0x000FFFFF
#define LEDC_HPOINT_LSCH1_M  ((LEDC_HPOINT_LSCH1_V)<<(LEDC_HPOINT_LSCH1_S))
#define LEDC_HPOINT_LSCH1_V  0xFFFFF
#define LEDC_HPOINT_LSCH1_S  0

#define LEDC_LSCH1_DUTY_REG          (DR_REG_LEDC_BASE + 0x00BC)
/* LEDC_DUTY_LSCH1 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When lstimerx(x=[0
 3]) choosed by low speed channel1 has reached reg_lpoint_lsch1 the output signal changes to low. reg_lpoint_lsch1=(reg_hpoint_lsch1[19:0]+reg_duty_lsch1[24:4])          (1)  reg_lpoint_lsch1=(reg_hpoint_lsch1[19:0]+reg_duty_lsch1[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_LSCH1  0x01FFFFFF
#define LEDC_DUTY_LSCH1_M  ((LEDC_DUTY_LSCH1_V)<<(LEDC_DUTY_LSCH1_S))
#define LEDC_DUTY_LSCH1_V  0x1FFFFFF
#define LEDC_DUTY_LSCH1_S  0

#define LEDC_LSCH1_CONF1_REG          (DR_REG_LEDC_BASE + 0x00C0)
/* LEDC_DUTY_START_LSCH1 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch1 reg_duty_cycle_hsch1 and reg_duty_scale_hsch1
 has been configured. these register won't take effect until set reg_duty_start_hsch1. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_LSCH1  (BIT(31))
#define LEDC_DUTY_START_LSCH1_M  (BIT(31))
#define LEDC_DUTY_START_LSCH1_V  0x1
#define LEDC_DUTY_START_LSCH1_S  31
/* LEDC_DUTY_INC_LSCH1 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for low speed channel1.*/
#define LEDC_DUTY_INC_LSCH1  (BIT(30))
#define LEDC_DUTY_INC_LSCH1_M  (BIT(30))
#define LEDC_DUTY_INC_LSCH1_V  0x1
#define LEDC_DUTY_INC_LSCH1_S  30
/* LEDC_DUTY_NUM_LSCH1 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for low speed channel1.*/
#define LEDC_DUTY_NUM_LSCH1  0x000003FF
#define LEDC_DUTY_NUM_LSCH1_M  ((LEDC_DUTY_NUM_LSCH1_V)<<(LEDC_DUTY_NUM_LSCH1_S))
#define LEDC_DUTY_NUM_LSCH1_V  0x3FF
#define LEDC_DUTY_NUM_LSCH1_S  20
/* LEDC_DUTY_CYCLE_LSCH1 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_lsch1 cycles for low speed channel1.*/
#define LEDC_DUTY_CYCLE_LSCH1  0x000003FF
#define LEDC_DUTY_CYCLE_LSCH1_M  ((LEDC_DUTY_CYCLE_LSCH1_V)<<(LEDC_DUTY_CYCLE_LSCH1_S))
#define LEDC_DUTY_CYCLE_LSCH1_V  0x3FF
#define LEDC_DUTY_CYCLE_LSCH1_S  10
/* LEDC_DUTY_SCALE_LSCH1 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 low speed channel1.*/
#define LEDC_DUTY_SCALE_LSCH1  0x000003FF
#define LEDC_DUTY_SCALE_LSCH1_M  ((LEDC_DUTY_SCALE_LSCH1_V)<<(LEDC_DUTY_SCALE_LSCH1_S))
#define LEDC_DUTY_SCALE_LSCH1_V  0x3FF
#define LEDC_DUTY_SCALE_LSCH1_S  0

#define LEDC_LSCH1_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x00C4)
/* LEDC_DUTY_LSCH1 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for low speed channel1.*/
#define LEDC_DUTY_LSCH1  0x01FFFFFF
#define LEDC_DUTY_LSCH1_M  ((LEDC_DUTY_LSCH1_V)<<(LEDC_DUTY_LSCH1_S))
#define LEDC_DUTY_LSCH1_V  0x1FFFFFF
#define LEDC_DUTY_LSCH1_S  0

#define LEDC_LSCH2_CONF0_REG          (DR_REG_LEDC_BASE + 0x00C8)
/* LEDC_PARA_UP_LSCH2 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: This bit is used to update register LEDC_LSCH2_HPOINT and LEDC_LSCH2_DUTY
 for low speed channel2.*/
#define LEDC_PARA_UP_LSCH2  (BIT(4))
#define LEDC_PARA_UP_LSCH2_M  (BIT(4))
#define LEDC_PARA_UP_LSCH2_V  0x1
#define LEDC_PARA_UP_LSCH2_S  4
/* LEDC_IDLE_LV_LSCH2 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when low speed channel2 is off.*/
#define LEDC_IDLE_LV_LSCH2  (BIT(3))
#define LEDC_IDLE_LV_LSCH2_M  (BIT(3))
#define LEDC_IDLE_LV_LSCH2_V  0x1
#define LEDC_IDLE_LV_LSCH2_S  3
/* LEDC_SIG_OUT_EN_LSCH2 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for low speed channel2.*/
#define LEDC_SIG_OUT_EN_LSCH2  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH2_M  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH2_V  0x1
#define LEDC_SIG_OUT_EN_LSCH2_S  2
/* LEDC_TIMER_SEL_LSCH2 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four low speed timers  the two bits are used to select
 one of them for low speed channel2.  2'b00: seletc lstimer0.   2'b01: select lstimer1.  2'b10: select lstimer2.    2'b11: select lstimer3.*/
#define LEDC_TIMER_SEL_LSCH2  0x00000003
#define LEDC_TIMER_SEL_LSCH2_M  ((LEDC_TIMER_SEL_LSCH2_V)<<(LEDC_TIMER_SEL_LSCH2_S))
#define LEDC_TIMER_SEL_LSCH2_V  0x3
#define LEDC_TIMER_SEL_LSCH2_S  0

#define LEDC_LSCH2_HPOINT_REG          (DR_REG_LEDC_BASE + 0x00CC)
/* LEDC_HPOINT_LSCH2 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when lstimerx(x=[0 3]) selected
 by low speed channel2 has reached reg_hpoint_lsch2[19:0]*/
#define LEDC_HPOINT_LSCH2  0x000FFFFF
#define LEDC_HPOINT_LSCH2_M  ((LEDC_HPOINT_LSCH2_V)<<(LEDC_HPOINT_LSCH2_S))
#define LEDC_HPOINT_LSCH2_V  0xFFFFF
#define LEDC_HPOINT_LSCH2_S  0

#define LEDC_LSCH2_DUTY_REG          (DR_REG_LEDC_BASE + 0x00D0)
/* LEDC_DUTY_LSCH2 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When lstimerx(x=[0
 3]) choosed by low speed channel2 has reached reg_lpoint_lsch2 the output signal changes to low. reg_lpoint_lsch2=(reg_hpoint_lsch2[19:0]+reg_duty_lsch2[24:4])          (1)  reg_lpoint_lsch2=(reg_hpoint_lsch2[19:0]+reg_duty_lsch2[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_LSCH2  0x01FFFFFF
#define LEDC_DUTY_LSCH2_M  ((LEDC_DUTY_LSCH2_V)<<(LEDC_DUTY_LSCH2_S))
#define LEDC_DUTY_LSCH2_V  0x1FFFFFF
#define LEDC_DUTY_LSCH2_S  0

#define LEDC_LSCH2_CONF1_REG          (DR_REG_LEDC_BASE + 0x00D4)
/* LEDC_DUTY_START_LSCH2 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch2 reg_duty_cycle_hsch2 and reg_duty_scale_hsch2
 has been configured. these register won't take effect until set reg_duty_start_hsch2. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_LSCH2  (BIT(31))
#define LEDC_DUTY_START_LSCH2_M  (BIT(31))
#define LEDC_DUTY_START_LSCH2_V  0x1
#define LEDC_DUTY_START_LSCH2_S  31
/* LEDC_DUTY_INC_LSCH2 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for low speed channel2.*/
#define LEDC_DUTY_INC_LSCH2  (BIT(30))
#define LEDC_DUTY_INC_LSCH2_M  (BIT(30))
#define LEDC_DUTY_INC_LSCH2_V  0x1
#define LEDC_DUTY_INC_LSCH2_S  30
/* LEDC_DUTY_NUM_LSCH2 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for low speed channel2.*/
#define LEDC_DUTY_NUM_LSCH2  0x000003FF
#define LEDC_DUTY_NUM_LSCH2_M  ((LEDC_DUTY_NUM_LSCH2_V)<<(LEDC_DUTY_NUM_LSCH2_S))
#define LEDC_DUTY_NUM_LSCH2_V  0x3FF
#define LEDC_DUTY_NUM_LSCH2_S  20
/* LEDC_DUTY_CYCLE_LSCH2 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_lsch2 cycles for low speed channel2.*/
#define LEDC_DUTY_CYCLE_LSCH2  0x000003FF
#define LEDC_DUTY_CYCLE_LSCH2_M  ((LEDC_DUTY_CYCLE_LSCH2_V)<<(LEDC_DUTY_CYCLE_LSCH2_S))
#define LEDC_DUTY_CYCLE_LSCH2_V  0x3FF
#define LEDC_DUTY_CYCLE_LSCH2_S  10
/* LEDC_DUTY_SCALE_LSCH2 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 low speed channel2.*/
#define LEDC_DUTY_SCALE_LSCH2  0x000003FF
#define LEDC_DUTY_SCALE_LSCH2_M  ((LEDC_DUTY_SCALE_LSCH2_V)<<(LEDC_DUTY_SCALE_LSCH2_S))
#define LEDC_DUTY_SCALE_LSCH2_V  0x3FF
#define LEDC_DUTY_SCALE_LSCH2_S  0

#define LEDC_LSCH2_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x00D8)
/* LEDC_DUTY_LSCH2 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for low speed channel2.*/
#define LEDC_DUTY_LSCH2  0x01FFFFFF
#define LEDC_DUTY_LSCH2_M  ((LEDC_DUTY_LSCH2_V)<<(LEDC_DUTY_LSCH2_S))
#define LEDC_DUTY_LSCH2_V  0x1FFFFFF
#define LEDC_DUTY_LSCH2_S  0

#define LEDC_LSCH3_CONF0_REG          (DR_REG_LEDC_BASE + 0x00DC)
/* LEDC_PARA_UP_LSCH3 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: This bit is used to update register LEDC_LSCH3_HPOINT and LEDC_LSCH3_DUTY
 for low speed channel3.*/
#define LEDC_PARA_UP_LSCH3  (BIT(4))
#define LEDC_PARA_UP_LSCH3_M  (BIT(4))
#define LEDC_PARA_UP_LSCH3_V  0x1
#define LEDC_PARA_UP_LSCH3_S  4
/* LEDC_IDLE_LV_LSCH3 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when low speed channel3 is off.*/
#define LEDC_IDLE_LV_LSCH3  (BIT(3))
#define LEDC_IDLE_LV_LSCH3_M  (BIT(3))
#define LEDC_IDLE_LV_LSCH3_V  0x1
#define LEDC_IDLE_LV_LSCH3_S  3
/* LEDC_SIG_OUT_EN_LSCH3 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for low speed channel3.*/
#define LEDC_SIG_OUT_EN_LSCH3  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH3_M  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH3_V  0x1
#define LEDC_SIG_OUT_EN_LSCH3_S  2
/* LEDC_TIMER_SEL_LSCH3 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four low speed timers  the two bits are used to select
 one of them for low speed channel3.  2'b00: seletc lstimer0.   2'b01: select lstimer1.  2'b10: select lstimer2.    2'b11: select lstimer3.*/
#define LEDC_TIMER_SEL_LSCH3  0x00000003
#define LEDC_TIMER_SEL_LSCH3_M  ((LEDC_TIMER_SEL_LSCH3_V)<<(LEDC_TIMER_SEL_LSCH3_S))
#define LEDC_TIMER_SEL_LSCH3_V  0x3
#define LEDC_TIMER_SEL_LSCH3_S  0

#define LEDC_LSCH3_HPOINT_REG          (DR_REG_LEDC_BASE + 0x00E0)
/* LEDC_HPOINT_LSCH3 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when lstimerx(x=[0 3]) selected
 by low speed channel3 has reached reg_hpoint_lsch3[19:0]*/
#define LEDC_HPOINT_LSCH3  0x000FFFFF
#define LEDC_HPOINT_LSCH3_M  ((LEDC_HPOINT_LSCH3_V)<<(LEDC_HPOINT_LSCH3_S))
#define LEDC_HPOINT_LSCH3_V  0xFFFFF
#define LEDC_HPOINT_LSCH3_S  0

#define LEDC_LSCH3_DUTY_REG          (DR_REG_LEDC_BASE + 0x00E4)
/* LEDC_DUTY_LSCH3 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When lstimerx(x=[0
 3]) choosed by low speed channel3 has reached reg_lpoint_lsch3 the output signal changes to low. reg_lpoint_lsch3=(reg_hpoint_lsch3[19:0]+reg_duty_lsch3[24:4])          (1)  reg_lpoint_lsch3=(reg_hpoint_lsch3[19:0]+reg_duty_lsch3[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_LSCH3  0x01FFFFFF
#define LEDC_DUTY_LSCH3_M  ((LEDC_DUTY_LSCH3_V)<<(LEDC_DUTY_LSCH3_S))
#define LEDC_DUTY_LSCH3_V  0x1FFFFFF
#define LEDC_DUTY_LSCH3_S  0

#define LEDC_LSCH3_CONF1_REG          (DR_REG_LEDC_BASE + 0x00E8)
/* LEDC_DUTY_START_LSCH3 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch3 reg_duty_cycle_hsch3 and reg_duty_scale_hsch3
 has been configured. these register won't take effect until set reg_duty_start_hsch3. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_LSCH3  (BIT(31))
#define LEDC_DUTY_START_LSCH3_M  (BIT(31))
#define LEDC_DUTY_START_LSCH3_V  0x1
#define LEDC_DUTY_START_LSCH3_S  31
/* LEDC_DUTY_INC_LSCH3 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for low speed channel3.*/
#define LEDC_DUTY_INC_LSCH3  (BIT(30))
#define LEDC_DUTY_INC_LSCH3_M  (BIT(30))
#define LEDC_DUTY_INC_LSCH3_V  0x1
#define LEDC_DUTY_INC_LSCH3_S  30
/* LEDC_DUTY_NUM_LSCH3 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for low speed channel3.*/
#define LEDC_DUTY_NUM_LSCH3  0x000003FF
#define LEDC_DUTY_NUM_LSCH3_M  ((LEDC_DUTY_NUM_LSCH3_V)<<(LEDC_DUTY_NUM_LSCH3_S))
#define LEDC_DUTY_NUM_LSCH3_V  0x3FF
#define LEDC_DUTY_NUM_LSCH3_S  20
/* LEDC_DUTY_CYCLE_LSCH3 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_lsch3 cycles for low speed channel3.*/
#define LEDC_DUTY_CYCLE_LSCH3  0x000003FF
#define LEDC_DUTY_CYCLE_LSCH3_M  ((LEDC_DUTY_CYCLE_LSCH3_V)<<(LEDC_DUTY_CYCLE_LSCH3_S))
#define LEDC_DUTY_CYCLE_LSCH3_V  0x3FF
#define LEDC_DUTY_CYCLE_LSCH3_S  10
/* LEDC_DUTY_SCALE_LSCH3 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 low speed channel3.*/
#define LEDC_DUTY_SCALE_LSCH3  0x000003FF
#define LEDC_DUTY_SCALE_LSCH3_M  ((LEDC_DUTY_SCALE_LSCH3_V)<<(LEDC_DUTY_SCALE_LSCH3_S))
#define LEDC_DUTY_SCALE_LSCH3_V  0x3FF
#define LEDC_DUTY_SCALE_LSCH3_S  0

#define LEDC_LSCH3_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x00EC)
/* LEDC_DUTY_LSCH3 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for low speed channel3.*/
#define LEDC_DUTY_LSCH3  0x01FFFFFF
#define LEDC_DUTY_LSCH3_M  ((LEDC_DUTY_LSCH3_V)<<(LEDC_DUTY_LSCH3_S))
#define LEDC_DUTY_LSCH3_V  0x1FFFFFF
#define LEDC_DUTY_LSCH3_S  0

#define LEDC_LSCH4_CONF0_REG          (DR_REG_LEDC_BASE + 0x00F0)
/* LEDC_PARA_UP_LSCH4 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: This bit is used to update register LEDC_LSCH4_HPOINT and LEDC_LSCH4_DUTY
 for low speed channel4.*/
#define LEDC_PARA_UP_LSCH4  (BIT(4))
#define LEDC_PARA_UP_LSCH4_M  (BIT(4))
#define LEDC_PARA_UP_LSCH4_V  0x1
#define LEDC_PARA_UP_LSCH4_S  4
/* LEDC_IDLE_LV_LSCH4 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when low speed channel4 is off.*/
#define LEDC_IDLE_LV_LSCH4  (BIT(3))
#define LEDC_IDLE_LV_LSCH4_M  (BIT(3))
#define LEDC_IDLE_LV_LSCH4_V  0x1
#define LEDC_IDLE_LV_LSCH4_S  3
/* LEDC_SIG_OUT_EN_LSCH4 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for low speed channel4.*/
#define LEDC_SIG_OUT_EN_LSCH4  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH4_M  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH4_V  0x1
#define LEDC_SIG_OUT_EN_LSCH4_S  2
/* LEDC_TIMER_SEL_LSCH4 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four low speed timers  the two bits are used to select
 one of them for low speed channel4.  2'b00: seletc lstimer0.   2'b01: select lstimer1.  2'b10: select lstimer2.    2'b11: select lstimer3.*/
#define LEDC_TIMER_SEL_LSCH4  0x00000003
#define LEDC_TIMER_SEL_LSCH4_M  ((LEDC_TIMER_SEL_LSCH4_V)<<(LEDC_TIMER_SEL_LSCH4_S))
#define LEDC_TIMER_SEL_LSCH4_V  0x3
#define LEDC_TIMER_SEL_LSCH4_S  0

#define LEDC_LSCH4_HPOINT_REG          (DR_REG_LEDC_BASE + 0x00F4)
/* LEDC_HPOINT_LSCH4 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when lstimerx(x=[0 3]) selected
 by low speed channel4 has reached reg_hpoint_lsch4[19:0]*/
#define LEDC_HPOINT_LSCH4  0x000FFFFF
#define LEDC_HPOINT_LSCH4_M  ((LEDC_HPOINT_LSCH4_V)<<(LEDC_HPOINT_LSCH4_S))
#define LEDC_HPOINT_LSCH4_V  0xFFFFF
#define LEDC_HPOINT_LSCH4_S  0

#define LEDC_LSCH4_DUTY_REG          (DR_REG_LEDC_BASE + 0x00F8)
/* LEDC_DUTY_LSCH4 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When lstimerx(x=[0
 3]) choosed by low speed channel4 has reached reg_lpoint_lsch4 the output signal changes to low. reg_lpoint_lsch4=(reg_hpoint_lsch4[19:0]+reg_duty_lsch4[24:4])          (1)  reg_lpoint_lsch4=(reg_hpoint_lsch4[19:0]+reg_duty_lsch4[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_LSCH4  0x01FFFFFF
#define LEDC_DUTY_LSCH4_M  ((LEDC_DUTY_LSCH4_V)<<(LEDC_DUTY_LSCH4_S))
#define LEDC_DUTY_LSCH4_V  0x1FFFFFF
#define LEDC_DUTY_LSCH4_S  0

#define LEDC_LSCH4_CONF1_REG          (DR_REG_LEDC_BASE + 0x00FC)
/* LEDC_DUTY_START_LSCH4 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch4  reg_duty_cycle_hsch4 and reg_duty_scale_hsch4
 has been configured. these register won't take effect until set reg_duty_start_hsch4. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_LSCH4  (BIT(31))
#define LEDC_DUTY_START_LSCH4_M  (BIT(31))
#define LEDC_DUTY_START_LSCH4_V  0x1
#define LEDC_DUTY_START_LSCH4_S  31
/* LEDC_DUTY_INC_LSCH4 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for low speed channel4.*/
#define LEDC_DUTY_INC_LSCH4  (BIT(30))
#define LEDC_DUTY_INC_LSCH4_M  (BIT(30))
#define LEDC_DUTY_INC_LSCH4_V  0x1
#define LEDC_DUTY_INC_LSCH4_S  30
/* LEDC_DUTY_NUM_LSCH4 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for low speed channel4.*/
#define LEDC_DUTY_NUM_LSCH4  0x000003FF
#define LEDC_DUTY_NUM_LSCH4_M  ((LEDC_DUTY_NUM_LSCH4_V)<<(LEDC_DUTY_NUM_LSCH4_S))
#define LEDC_DUTY_NUM_LSCH4_V  0x3FF
#define LEDC_DUTY_NUM_LSCH4_S  20
/* LEDC_DUTY_CYCLE_LSCH4 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_lsch4 cycles for low speed channel4.*/
#define LEDC_DUTY_CYCLE_LSCH4  0x000003FF
#define LEDC_DUTY_CYCLE_LSCH4_M  ((LEDC_DUTY_CYCLE_LSCH4_V)<<(LEDC_DUTY_CYCLE_LSCH4_S))
#define LEDC_DUTY_CYCLE_LSCH4_V  0x3FF
#define LEDC_DUTY_CYCLE_LSCH4_S  10
/* LEDC_DUTY_SCALE_LSCH4 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 low speed channel4.*/
#define LEDC_DUTY_SCALE_LSCH4  0x000003FF
#define LEDC_DUTY_SCALE_LSCH4_M  ((LEDC_DUTY_SCALE_LSCH4_V)<<(LEDC_DUTY_SCALE_LSCH4_S))
#define LEDC_DUTY_SCALE_LSCH4_V  0x3FF
#define LEDC_DUTY_SCALE_LSCH4_S  0

#define LEDC_LSCH4_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x0100)
/* LEDC_DUTY_LSCH4 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for low speed channel4.*/
#define LEDC_DUTY_LSCH4  0x01FFFFFF
#define LEDC_DUTY_LSCH4_M  ((LEDC_DUTY_LSCH4_V)<<(LEDC_DUTY_LSCH4_S))
#define LEDC_DUTY_LSCH4_V  0x1FFFFFF
#define LEDC_DUTY_LSCH4_S  0

#define LEDC_LSCH5_CONF0_REG          (DR_REG_LEDC_BASE + 0x0104)
/* LEDC_PARA_UP_LSCH5 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: This bit is used to update register LEDC_LSCH5_HPOINT and LEDC_LSCH5_DUTY
 for low speed channel5.*/
#define LEDC_PARA_UP_LSCH5  (BIT(4))
#define LEDC_PARA_UP_LSCH5_M  (BIT(4))
#define LEDC_PARA_UP_LSCH5_V  0x1
#define LEDC_PARA_UP_LSCH5_S  4
/* LEDC_IDLE_LV_LSCH5 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when low speed channel5 is off.*/
#define LEDC_IDLE_LV_LSCH5  (BIT(3))
#define LEDC_IDLE_LV_LSCH5_M  (BIT(3))
#define LEDC_IDLE_LV_LSCH5_V  0x1
#define LEDC_IDLE_LV_LSCH5_S  3
/* LEDC_SIG_OUT_EN_LSCH5 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for low speed channel5.*/
#define LEDC_SIG_OUT_EN_LSCH5  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH5_M  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH5_V  0x1
#define LEDC_SIG_OUT_EN_LSCH5_S  2
/* LEDC_TIMER_SEL_LSCH5 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four low speed timers  the two bits are used to select
 one of them for low speed channel5.  2'b00: seletc lstimer0.   2'b01: select lstimer1.  2'b10: select lstimer2.    2'b11: select lstimer3.*/
#define LEDC_TIMER_SEL_LSCH5  0x00000003
#define LEDC_TIMER_SEL_LSCH5_M  ((LEDC_TIMER_SEL_LSCH5_V)<<(LEDC_TIMER_SEL_LSCH5_S))
#define LEDC_TIMER_SEL_LSCH5_V  0x3
#define LEDC_TIMER_SEL_LSCH5_S  0

#define LEDC_LSCH5_HPOINT_REG          (DR_REG_LEDC_BASE + 0x0108)
/* LEDC_HPOINT_LSCH5 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when lstimerx(x=[0 3]) selected
 by low speed channel5 has reached reg_hpoint_lsch5[19:0]*/
#define LEDC_HPOINT_LSCH5  0x000FFFFF
#define LEDC_HPOINT_LSCH5_M  ((LEDC_HPOINT_LSCH5_V)<<(LEDC_HPOINT_LSCH5_S))
#define LEDC_HPOINT_LSCH5_V  0xFFFFF
#define LEDC_HPOINT_LSCH5_S  0

#define LEDC_LSCH5_DUTY_REG          (DR_REG_LEDC_BASE + 0x010C)
/* LEDC_DUTY_LSCH5 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When lstimerx(x=[0
 3]) choosed by low speed channel5 has reached reg_lpoint_lsch5 the output signal changes to low. reg_lpoint_lsch5=(reg_hpoint_lsch5[19:0]+reg_duty_lsch5[24:4])          (1)  reg_lpoint_lsch5=(reg_hpoint_lsch5[19:0]+reg_duty_lsch5[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_LSCH5  0x01FFFFFF
#define LEDC_DUTY_LSCH5_M  ((LEDC_DUTY_LSCH5_V)<<(LEDC_DUTY_LSCH5_S))
#define LEDC_DUTY_LSCH5_V  0x1FFFFFF
#define LEDC_DUTY_LSCH5_S  0

#define LEDC_LSCH5_CONF1_REG          (DR_REG_LEDC_BASE + 0x0110)
/* LEDC_DUTY_START_LSCH5 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch4  reg_duty_cycle_hsch4 and reg_duty_scale_hsch4
 has been configured. these register won't take effect until set reg_duty_start_hsch4. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_LSCH5  (BIT(31))
#define LEDC_DUTY_START_LSCH5_M  (BIT(31))
#define LEDC_DUTY_START_LSCH5_V  0x1
#define LEDC_DUTY_START_LSCH5_S  31
/* LEDC_DUTY_INC_LSCH5 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for low speed channel5.*/
#define LEDC_DUTY_INC_LSCH5  (BIT(30))
#define LEDC_DUTY_INC_LSCH5_M  (BIT(30))
#define LEDC_DUTY_INC_LSCH5_V  0x1
#define LEDC_DUTY_INC_LSCH5_S  30
/* LEDC_DUTY_NUM_LSCH5 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for low speed channel5.*/
#define LEDC_DUTY_NUM_LSCH5  0x000003FF
#define LEDC_DUTY_NUM_LSCH5_M  ((LEDC_DUTY_NUM_LSCH5_V)<<(LEDC_DUTY_NUM_LSCH5_S))
#define LEDC_DUTY_NUM_LSCH5_V  0x3FF
#define LEDC_DUTY_NUM_LSCH5_S  20
/* LEDC_DUTY_CYCLE_LSCH5 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_lsch5 cycles for low speed channel4.*/
#define LEDC_DUTY_CYCLE_LSCH5  0x000003FF
#define LEDC_DUTY_CYCLE_LSCH5_M  ((LEDC_DUTY_CYCLE_LSCH5_V)<<(LEDC_DUTY_CYCLE_LSCH5_S))
#define LEDC_DUTY_CYCLE_LSCH5_V  0x3FF
#define LEDC_DUTY_CYCLE_LSCH5_S  10
/* LEDC_DUTY_SCALE_LSCH5 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 low speed channel5.*/
#define LEDC_DUTY_SCALE_LSCH5  0x000003FF
#define LEDC_DUTY_SCALE_LSCH5_M  ((LEDC_DUTY_SCALE_LSCH5_V)<<(LEDC_DUTY_SCALE_LSCH5_S))
#define LEDC_DUTY_SCALE_LSCH5_V  0x3FF
#define LEDC_DUTY_SCALE_LSCH5_S  0

#define LEDC_LSCH5_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x0114)
/* LEDC_DUTY_LSCH5 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for low speed channel5.*/
#define LEDC_DUTY_LSCH5  0x01FFFFFF
#define LEDC_DUTY_LSCH5_M  ((LEDC_DUTY_LSCH5_V)<<(LEDC_DUTY_LSCH5_S))
#define LEDC_DUTY_LSCH5_V  0x1FFFFFF
#define LEDC_DUTY_LSCH5_S  0

#define LEDC_LSCH6_CONF0_REG          (DR_REG_LEDC_BASE + 0x0118)
/* LEDC_PARA_UP_LSCH6 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: This bit is used to update register LEDC_LSCH6_HPOINT and LEDC_LSCH6_DUTY
 for low speed channel6.*/
#define LEDC_PARA_UP_LSCH6  (BIT(4))
#define LEDC_PARA_UP_LSCH6_M  (BIT(4))
#define LEDC_PARA_UP_LSCH6_V  0x1
#define LEDC_PARA_UP_LSCH6_S  4
/* LEDC_IDLE_LV_LSCH6 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when low speed channel6 is off.*/
#define LEDC_IDLE_LV_LSCH6  (BIT(3))
#define LEDC_IDLE_LV_LSCH6_M  (BIT(3))
#define LEDC_IDLE_LV_LSCH6_V  0x1
#define LEDC_IDLE_LV_LSCH6_S  3
/* LEDC_SIG_OUT_EN_LSCH6 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for low speed channel6.*/
#define LEDC_SIG_OUT_EN_LSCH6  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH6_M  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH6_V  0x1
#define LEDC_SIG_OUT_EN_LSCH6_S  2
/* LEDC_TIMER_SEL_LSCH6 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four low speed timers  the two bits are used to select
 one of them for low speed channel6.  2'b00: seletc lstimer0.   2'b01: select lstimer1.  2'b10: select lstimer2.    2'b11: select lstimer3.*/
#define LEDC_TIMER_SEL_LSCH6  0x00000003
#define LEDC_TIMER_SEL_LSCH6_M  ((LEDC_TIMER_SEL_LSCH6_V)<<(LEDC_TIMER_SEL_LSCH6_S))
#define LEDC_TIMER_SEL_LSCH6_V  0x3
#define LEDC_TIMER_SEL_LSCH6_S  0

#define LEDC_LSCH6_HPOINT_REG          (DR_REG_LEDC_BASE + 0x011C)
/* LEDC_HPOINT_LSCH6 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when lstimerx(x=[0 3]) selected
 by low speed channel6 has reached reg_hpoint_lsch6[19:0]*/
#define LEDC_HPOINT_LSCH6  0x000FFFFF
#define LEDC_HPOINT_LSCH6_M  ((LEDC_HPOINT_LSCH6_V)<<(LEDC_HPOINT_LSCH6_S))
#define LEDC_HPOINT_LSCH6_V  0xFFFFF
#define LEDC_HPOINT_LSCH6_S  0

#define LEDC_LSCH6_DUTY_REG          (DR_REG_LEDC_BASE + 0x0120)
/* LEDC_DUTY_LSCH6 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When lstimerx(x=[0
 3]) choosed by low speed channel6 has reached reg_lpoint_lsch6 the output signal changes to low. reg_lpoint_lsch6=(reg_hpoint_lsch6[19:0]+reg_duty_lsch6[24:4])          (1)  reg_lpoint_lsch6=(reg_hpoint_lsch6[19:0]+reg_duty_lsch6[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_LSCH6  0x01FFFFFF
#define LEDC_DUTY_LSCH6_M  ((LEDC_DUTY_LSCH6_V)<<(LEDC_DUTY_LSCH6_S))
#define LEDC_DUTY_LSCH6_V  0x1FFFFFF
#define LEDC_DUTY_LSCH6_S  0

#define LEDC_LSCH6_CONF1_REG          (DR_REG_LEDC_BASE + 0x0124)
/* LEDC_DUTY_START_LSCH6 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch6  reg_duty_cycle_hsch6 and reg_duty_scale_hsch6
 has been configured. these register won't take effect until set reg_duty_start_hsch6. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_LSCH6  (BIT(31))
#define LEDC_DUTY_START_LSCH6_M  (BIT(31))
#define LEDC_DUTY_START_LSCH6_V  0x1
#define LEDC_DUTY_START_LSCH6_S  31
/* LEDC_DUTY_INC_LSCH6 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for low speed channel6.*/
#define LEDC_DUTY_INC_LSCH6  (BIT(30))
#define LEDC_DUTY_INC_LSCH6_M  (BIT(30))
#define LEDC_DUTY_INC_LSCH6_V  0x1
#define LEDC_DUTY_INC_LSCH6_S  30
/* LEDC_DUTY_NUM_LSCH6 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for low speed channel6.*/
#define LEDC_DUTY_NUM_LSCH6  0x000003FF
#define LEDC_DUTY_NUM_LSCH6_M  ((LEDC_DUTY_NUM_LSCH6_V)<<(LEDC_DUTY_NUM_LSCH6_S))
#define LEDC_DUTY_NUM_LSCH6_V  0x3FF
#define LEDC_DUTY_NUM_LSCH6_S  20
/* LEDC_DUTY_CYCLE_LSCH6 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_lsch6 cycles for low speed channel6.*/
#define LEDC_DUTY_CYCLE_LSCH6  0x000003FF
#define LEDC_DUTY_CYCLE_LSCH6_M  ((LEDC_DUTY_CYCLE_LSCH6_V)<<(LEDC_DUTY_CYCLE_LSCH6_S))
#define LEDC_DUTY_CYCLE_LSCH6_V  0x3FF
#define LEDC_DUTY_CYCLE_LSCH6_S  10
/* LEDC_DUTY_SCALE_LSCH6 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 low speed channel6.*/
#define LEDC_DUTY_SCALE_LSCH6  0x000003FF
#define LEDC_DUTY_SCALE_LSCH6_M  ((LEDC_DUTY_SCALE_LSCH6_V)<<(LEDC_DUTY_SCALE_LSCH6_S))
#define LEDC_DUTY_SCALE_LSCH6_V  0x3FF
#define LEDC_DUTY_SCALE_LSCH6_S  0

#define LEDC_LSCH6_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x0128)
/* LEDC_DUTY_LSCH6 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for low speed channel6.*/
#define LEDC_DUTY_LSCH6  0x01FFFFFF
#define LEDC_DUTY_LSCH6_M  ((LEDC_DUTY_LSCH6_V)<<(LEDC_DUTY_LSCH6_S))
#define LEDC_DUTY_LSCH6_V  0x1FFFFFF
#define LEDC_DUTY_LSCH6_S  0

#define LEDC_LSCH7_CONF0_REG          (DR_REG_LEDC_BASE + 0x012C)
/* LEDC_PARA_UP_LSCH7 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: This bit is used to update register LEDC_LSCH7_HPOINT and LEDC_LSCH7_DUTY
 for low speed channel7.*/
#define LEDC_PARA_UP_LSCH7  (BIT(4))
#define LEDC_PARA_UP_LSCH7_M  (BIT(4))
#define LEDC_PARA_UP_LSCH7_V  0x1
#define LEDC_PARA_UP_LSCH7_S  4
/* LEDC_IDLE_LV_LSCH7 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: This bit is used to control the output value when low speed channel7 is off.*/
#define LEDC_IDLE_LV_LSCH7  (BIT(3))
#define LEDC_IDLE_LV_LSCH7_M  (BIT(3))
#define LEDC_IDLE_LV_LSCH7_V  0x1
#define LEDC_IDLE_LV_LSCH7_S  3
/* LEDC_SIG_OUT_EN_LSCH7 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: This is the output enable control bit for low speed channel7.*/
#define LEDC_SIG_OUT_EN_LSCH7  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH7_M  (BIT(2))
#define LEDC_SIG_OUT_EN_LSCH7_V  0x1
#define LEDC_SIG_OUT_EN_LSCH7_S  2
/* LEDC_TIMER_SEL_LSCH7 : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: There are four low speed timers  the two bits are used to select
 one of them for low speed channel7.  2'b00: seletc lstimer0.   2'b01: select lstimer1.  2'b10: select lstimer2.    2'b11: select lstimer3.*/
#define LEDC_TIMER_SEL_LSCH7  0x00000003
#define LEDC_TIMER_SEL_LSCH7_M  ((LEDC_TIMER_SEL_LSCH7_V)<<(LEDC_TIMER_SEL_LSCH7_S))
#define LEDC_TIMER_SEL_LSCH7_V  0x3
#define LEDC_TIMER_SEL_LSCH7_S  0

#define LEDC_LSCH7_HPOINT_REG          (DR_REG_LEDC_BASE + 0x0130)
/* LEDC_HPOINT_LSCH7 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: The output value changes to high when lstimerx(x=[0 3]) selected
 by low speed channel7 has reached reg_hpoint_lsch7[19:0]*/
#define LEDC_HPOINT_LSCH7  0x000FFFFF
#define LEDC_HPOINT_LSCH7_M  ((LEDC_HPOINT_LSCH7_V)<<(LEDC_HPOINT_LSCH7_S))
#define LEDC_HPOINT_LSCH7_V  0xFFFFF
#define LEDC_HPOINT_LSCH7_S  0

#define LEDC_LSCH7_DUTY_REG          (DR_REG_LEDC_BASE + 0x0134)
/* LEDC_DUTY_LSCH7 : R/W ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: The register is used to control output duty. When lstimerx(x=[0
 3]) choosed by low speed channel7 has reached reg_lpoint_lsch7 the output signal changes to low. reg_lpoint_lsch7=(reg_hpoint_lsch7[19:0]+reg_duty_lsch7[24:4])          (1)  reg_lpoint_lsch7=(reg_hpoint_lsch7[19:0]+reg_duty_lsch7[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
#define LEDC_DUTY_LSCH7  0x01FFFFFF
#define LEDC_DUTY_LSCH7_M  ((LEDC_DUTY_LSCH7_V)<<(LEDC_DUTY_LSCH7_S))
#define LEDC_DUTY_LSCH7_V  0x1FFFFFF
#define LEDC_DUTY_LSCH7_S  0

#define LEDC_LSCH7_CONF1_REG          (DR_REG_LEDC_BASE + 0x0138)
/* LEDC_DUTY_START_LSCH7 : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: When reg_duty_num_hsch4  reg_duty_cycle_hsch4 and reg_duty_scale_hsch4
 has been configured. these register won't take effect until set reg_duty_start_hsch4. this bit is automatically cleared by hardware.*/
#define LEDC_DUTY_START_LSCH7  (BIT(31))
#define LEDC_DUTY_START_LSCH7_M  (BIT(31))
#define LEDC_DUTY_START_LSCH7_V  0x1
#define LEDC_DUTY_START_LSCH7_S  31
/* LEDC_DUTY_INC_LSCH7 : R/W ;bitpos:[30] ;default: 1'b1 ; */
/*description: This register is used to increase the duty of output signal or
 decrease the duty of output signal for low speed channel4.*/
#define LEDC_DUTY_INC_LSCH7  (BIT(30))
#define LEDC_DUTY_INC_LSCH7_M  (BIT(30))
#define LEDC_DUTY_INC_LSCH7_V  0x1
#define LEDC_DUTY_INC_LSCH7_S  30
/* LEDC_DUTY_NUM_LSCH7 : R/W ;bitpos:[29:20] ;default: 10'h0 ; */
/*description: This register is used to control the num of increased or decreased
 times for low speed channel4.*/
#define LEDC_DUTY_NUM_LSCH7  0x000003FF
#define LEDC_DUTY_NUM_LSCH7_M  ((LEDC_DUTY_NUM_LSCH7_V)<<(LEDC_DUTY_NUM_LSCH7_S))
#define LEDC_DUTY_NUM_LSCH7_V  0x3FF
#define LEDC_DUTY_NUM_LSCH7_S  20
/* LEDC_DUTY_CYCLE_LSCH7 : R/W ;bitpos:[19:10] ;default: 10'h0 ; */
/*description: This register is used to increase or decrease the duty every
 reg_duty_cycle_lsch7 cycles for low speed channel7.*/
#define LEDC_DUTY_CYCLE_LSCH7  0x000003FF
#define LEDC_DUTY_CYCLE_LSCH7_M  ((LEDC_DUTY_CYCLE_LSCH7_V)<<(LEDC_DUTY_CYCLE_LSCH7_S))
#define LEDC_DUTY_CYCLE_LSCH7_V  0x3FF
#define LEDC_DUTY_CYCLE_LSCH7_S  10
/* LEDC_DUTY_SCALE_LSCH7 : R/W ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: This register controls the increase or decrease step scale for
 low speed channel7.*/
#define LEDC_DUTY_SCALE_LSCH7  0x000003FF
#define LEDC_DUTY_SCALE_LSCH7_M  ((LEDC_DUTY_SCALE_LSCH7_V)<<(LEDC_DUTY_SCALE_LSCH7_S))
#define LEDC_DUTY_SCALE_LSCH7_V  0x3FF
#define LEDC_DUTY_SCALE_LSCH7_S  0

#define LEDC_LSCH7_DUTY_R_REG          (DR_REG_LEDC_BASE + 0x013C)
/* LEDC_DUTY_LSCH7 : RO ;bitpos:[24:0] ;default: 25'h0 ; */
/*description: This register represents the current duty of the output signal
 for low speed channel7.*/
#define LEDC_DUTY_LSCH7  0x01FFFFFF
#define LEDC_DUTY_LSCH7_M  ((LEDC_DUTY_LSCH7_V)<<(LEDC_DUTY_LSCH7_S))
#define LEDC_DUTY_LSCH7_V  0x1FFFFFF
#define LEDC_DUTY_LSCH7_S  0

#define LEDC_HSTIMER0_CONF_REG          (DR_REG_LEDC_BASE + 0x0140)
/* LEDC_TICK_SEL_HSTIMER0 : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: This bit is used to choose apb_clk or ref_tick for high speed
 timer0. 1'b1:apb_clk  0:ref_tick*/
#define LEDC_TICK_SEL_HSTIMER0  (BIT(25))
#define LEDC_TICK_SEL_HSTIMER0_M  (BIT(25))
#define LEDC_TICK_SEL_HSTIMER0_V  0x1
#define LEDC_TICK_SEL_HSTIMER0_S  25
/* LEDC_HSTIMER0_RST : R/W ;bitpos:[24] ;default: 1'b1 ; */
/*description: This bit is used to reset high speed timer0 the counter will be 0 after reset.*/
#define LEDC_HSTIMER0_RST  (BIT(24))
#define LEDC_HSTIMER0_RST_M  (BIT(24))
#define LEDC_HSTIMER0_RST_V  0x1
#define LEDC_HSTIMER0_RST_S  24
/* LEDC_HSTIMER0_PAUSE : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: This bit is used to pause the counter in high speed timer0*/
#define LEDC_HSTIMER0_PAUSE  (BIT(23))
#define LEDC_HSTIMER0_PAUSE_M  (BIT(23))
#define LEDC_HSTIMER0_PAUSE_V  0x1
#define LEDC_HSTIMER0_PAUSE_S  23
/* LEDC_DIV_NUM_HSTIMER0 : R/W ;bitpos:[22:5] ;default: 18'h0 ; */
/*description: This register is used to configure parameter for divider in high
 speed timer0 the least significant eight bits represent the decimal part.*/
#define LEDC_DIV_NUM_HSTIMER0  0x0003FFFF
#define LEDC_DIV_NUM_HSTIMER0_M  ((LEDC_DIV_NUM_HSTIMER0_V)<<(LEDC_DIV_NUM_HSTIMER0_S))
#define LEDC_DIV_NUM_HSTIMER0_V  0x3FFFF
#define LEDC_DIV_NUM_HSTIMER0_S  5
/* LEDC_HSTIMER0_LIM : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: This register controls the range of the counter in high speed
 timer0. the counter range is [0 2**reg_hstimer0_lim] the max bit width for counter is 20.*/
#define LEDC_HSTIMER0_DUTY_RES  0x0000001F
#define LEDC_HSTIMER0_DUTY_RES_M  ((LEDC_HSTIMER0_DUTY_RES_V)<<(LEDC_HSTIMER0_DUTY_RES_S))
#define LEDC_HSTIMER0_DUTY_RES_V  0x1F
#define LEDC_HSTIMER0_DUTY_RES_S  0
// Keep the definitions below to be compatible with previous version
#define LEDC_HSTIMER0_LIM         LEDC_HSTIMER0_DUTY_RES
#define LEDC_HSTIMER0_LIM_M       LEDC_HSTIMER0_DUTY_RES_M
#define LEDC_HSTIMER0_LIM_V       LEDC_HSTIMER0_DUTY_RES_V
#define LEDC_HSTIMER0_LIM_S       LEDC_HSTIMER0_DUTY_RES_S

#define LEDC_HSTIMER0_VALUE_REG          (DR_REG_LEDC_BASE + 0x0144)
/* LEDC_HSTIMER0_CNT : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: software can read this register to get the current counter value
 in high speed timer0*/
#define LEDC_HSTIMER0_CNT  0x000FFFFF
#define LEDC_HSTIMER0_CNT_M  ((LEDC_HSTIMER0_CNT_V)<<(LEDC_HSTIMER0_CNT_S))
#define LEDC_HSTIMER0_CNT_V  0xFFFFF
#define LEDC_HSTIMER0_CNT_S  0

#define LEDC_HSTIMER1_CONF_REG          (DR_REG_LEDC_BASE + 0x0148)
/* LEDC_TICK_SEL_HSTIMER1 : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: This bit is used to choose apb_clk or ref_tick for high speed
 timer1. 1'b1:apb_clk  0:ref_tick*/
#define LEDC_TICK_SEL_HSTIMER1  (BIT(25))
#define LEDC_TICK_SEL_HSTIMER1_M  (BIT(25))
#define LEDC_TICK_SEL_HSTIMER1_V  0x1
#define LEDC_TICK_SEL_HSTIMER1_S  25
/* LEDC_HSTIMER1_RST : R/W ;bitpos:[24] ;default: 1'b1 ; */
/*description: This bit is used to reset high speed timer1 the counter will be 0 after reset.*/
#define LEDC_HSTIMER1_RST  (BIT(24))
#define LEDC_HSTIMER1_RST_M  (BIT(24))
#define LEDC_HSTIMER1_RST_V  0x1
#define LEDC_HSTIMER1_RST_S  24
/* LEDC_HSTIMER1_PAUSE : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: This bit is used to pause the counter in high speed timer1*/
#define LEDC_HSTIMER1_PAUSE  (BIT(23))
#define LEDC_HSTIMER1_PAUSE_M  (BIT(23))
#define LEDC_HSTIMER1_PAUSE_V  0x1
#define LEDC_HSTIMER1_PAUSE_S  23
/* LEDC_DIV_NUM_HSTIMER1 : R/W ;bitpos:[22:5] ;default: 18'h0 ; */
/*description: This register is used to configure parameter for divider in high
 speed timer1 the least significant eight bits represent the decimal part.*/
#define LEDC_DIV_NUM_HSTIMER1  0x0003FFFF
#define LEDC_DIV_NUM_HSTIMER1_M  ((LEDC_DIV_NUM_HSTIMER1_V)<<(LEDC_DIV_NUM_HSTIMER1_S))
#define LEDC_DIV_NUM_HSTIMER1_V  0x3FFFF
#define LEDC_DIV_NUM_HSTIMER1_S  5
/* LEDC_HSTIMER1_LIM : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: This register controls the range of the counter in high speed
 timer1. the counter range is [0 2**reg_hstimer1_lim] the max bit width for counter is 20.*/
#define LEDC_HSTIMER1_DUTY_RES  0x0000001F
#define LEDC_HSTIMER1_DUTY_RES_M  ((LEDC_HSTIMER1_DUTY_RES_V)<<(LEDC_HSTIMER1_DUTY_RES_S))
#define LEDC_HSTIMER1_DUTY_RES_V  0x1F
#define LEDC_HSTIMER1_DUTY_RES_S  0
// Keep the definitions below to be compatible with previous version
#define LEDC_HSTIMER1_LIM       LEDC_HSTIMER1_DUTY_RES
#define LEDC_HSTIMER1_LIM_M     LEDC_HSTIMER1_DUTY_RES_M
#define LEDC_HSTIMER1_LIM_V     LEDC_HSTIMER1_DUTY_RES_V
#define LEDC_HSTIMER1_LIM_S     LEDC_HSTIMER1_DUTY_RES_S

#define LEDC_HSTIMER1_VALUE_REG          (DR_REG_LEDC_BASE + 0x014C)
/* LEDC_HSTIMER1_CNT : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: software can read this register to get the current counter value
 in high speed timer1.*/
#define LEDC_HSTIMER1_CNT  0x000FFFFF
#define LEDC_HSTIMER1_CNT_M  ((LEDC_HSTIMER1_CNT_V)<<(LEDC_HSTIMER1_CNT_S))
#define LEDC_HSTIMER1_CNT_V  0xFFFFF
#define LEDC_HSTIMER1_CNT_S  0

#define LEDC_HSTIMER2_CONF_REG          (DR_REG_LEDC_BASE + 0x0150)
/* LEDC_TICK_SEL_HSTIMER2 : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: This bit is used to choose apb_clk or ref_tick for high speed
 timer2. 1'b1:apb_clk  0:ref_tick*/
#define LEDC_TICK_SEL_HSTIMER2  (BIT(25))
#define LEDC_TICK_SEL_HSTIMER2_M  (BIT(25))
#define LEDC_TICK_SEL_HSTIMER2_V  0x1
#define LEDC_TICK_SEL_HSTIMER2_S  25
/* LEDC_HSTIMER2_RST : R/W ;bitpos:[24] ;default: 1'b1 ; */
/*description: This bit is used to reset high speed timer2 the counter will be 0 after reset.*/
#define LEDC_HSTIMER2_RST  (BIT(24))
#define LEDC_HSTIMER2_RST_M  (BIT(24))
#define LEDC_HSTIMER2_RST_V  0x1
#define LEDC_HSTIMER2_RST_S  24
/* LEDC_HSTIMER2_PAUSE : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: This bit is used to pause the counter in high speed timer2*/
#define LEDC_HSTIMER2_PAUSE  (BIT(23))
#define LEDC_HSTIMER2_PAUSE_M  (BIT(23))
#define LEDC_HSTIMER2_PAUSE_V  0x1
#define LEDC_HSTIMER2_PAUSE_S  23
/* LEDC_DIV_NUM_HSTIMER2 : R/W ;bitpos:[22:5] ;default: 18'h0 ; */
/*description: This register is used to configure parameter for divider in high
 speed timer2 the least significant eight bits represent the decimal part.*/
#define LEDC_DIV_NUM_HSTIMER2  0x0003FFFF
#define LEDC_DIV_NUM_HSTIMER2_M  ((LEDC_DIV_NUM_HSTIMER2_V)<<(LEDC_DIV_NUM_HSTIMER2_S))
#define LEDC_DIV_NUM_HSTIMER2_V  0x3FFFF
#define LEDC_DIV_NUM_HSTIMER2_S  5
/* LEDC_HSTIMER2_LIM : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: This register controls the range of the counter in high speed
 timer2. the counter range is [0 2**reg_hstimer2_lim] the max bit width for counter is 20.*/
#define LEDC_HSTIMER2_DUTY_RES  0x0000001F
#define LEDC_HSTIMER2_DUTY_RES_M  ((LEDC_HSTIMER2_DUTY_RES_V)<<(LEDC_HSTIMER2_DUTY_RES_S))
#define LEDC_HSTIMER2_DUTY_RES_V  0x1F
#define LEDC_HSTIMER2_DUTY_RES_S  0
// Keep the definitions below to be compatible with previous version
#define LEDC_HSTIMER2_LIM       LEDC_HSTIMER2_DUTY_RES
#define LEDC_HSTIMER2_LIM_M     LEDC_HSTIMER2_DUTY_RES_M
#define LEDC_HSTIMER2_LIM_V     LEDC_HSTIMER2_DUTY_RES_V
#define LEDC_HSTIMER2_LIM_S     LEDC_HSTIMER2_DUTY_RES_S

#define LEDC_HSTIMER2_VALUE_REG          (DR_REG_LEDC_BASE + 0x0154)
/* LEDC_HSTIMER2_CNT : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: software can read this register to get the current counter value
 in high speed timer2*/
#define LEDC_HSTIMER2_CNT  0x000FFFFF
#define LEDC_HSTIMER2_CNT_M  ((LEDC_HSTIMER2_CNT_V)<<(LEDC_HSTIMER2_CNT_S))
#define LEDC_HSTIMER2_CNT_V  0xFFFFF
#define LEDC_HSTIMER2_CNT_S  0

#define LEDC_HSTIMER3_CONF_REG          (DR_REG_LEDC_BASE + 0x0158)
/* LEDC_TICK_SEL_HSTIMER3 : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: This bit is used to choose apb_clk or ref_tick for high speed
 timer3. 1'b1:apb_clk  0:ref_tick*/
#define LEDC_TICK_SEL_HSTIMER3  (BIT(25))
#define LEDC_TICK_SEL_HSTIMER3_M  (BIT(25))
#define LEDC_TICK_SEL_HSTIMER3_V  0x1
#define LEDC_TICK_SEL_HSTIMER3_S  25
/* LEDC_HSTIMER3_RST : R/W ;bitpos:[24] ;default: 1'b1 ; */
/*description: This bit is used to reset high speed timer3 the counter will be 0 after reset.*/
#define LEDC_HSTIMER3_RST  (BIT(24))
#define LEDC_HSTIMER3_RST_M  (BIT(24))
#define LEDC_HSTIMER3_RST_V  0x1
#define LEDC_HSTIMER3_RST_S  24
/* LEDC_HSTIMER3_PAUSE : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: This bit is used to pause the counter in high speed timer3*/
#define LEDC_HSTIMER3_PAUSE  (BIT(23))
#define LEDC_HSTIMER3_PAUSE_M  (BIT(23))
#define LEDC_HSTIMER3_PAUSE_V  0x1
#define LEDC_HSTIMER3_PAUSE_S  23
/* LEDC_DIV_NUM_HSTIMER3 : R/W ;bitpos:[22:5] ;default: 18'h0 ; */
/*description: This register is used to configure parameter for divider in high
 speed timer3 the least significant eight bits represent the decimal part.*/
#define LEDC_DIV_NUM_HSTIMER3  0x0003FFFF
#define LEDC_DIV_NUM_HSTIMER3_M  ((LEDC_DIV_NUM_HSTIMER3_V)<<(LEDC_DIV_NUM_HSTIMER3_S))
#define LEDC_DIV_NUM_HSTIMER3_V  0x3FFFF
#define LEDC_DIV_NUM_HSTIMER3_S  5
/* LEDC_HSTIMER3_LIM : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: This register controls the range of the counter in high speed
 timer3. the counter range is [0 2**reg_hstimer3_lim] the max bit width for counter is 20.*/
#define LEDC_HSTIMER3_DUTY_RES  0x0000001F
#define LEDC_HSTIMER3_DUTY_RES_M  ((LEDC_HSTIMER3_DUTY_RES_V)<<(LEDC_HSTIMER3_DUTY_RES_S))
#define LEDC_HSTIMER3_DUTY_RES_V  0x1F
#define LEDC_HSTIMER3_DUTY_RES_S  0
// Keep the definitions below to be compatible with previous version
#define LEDC_HSTIMER3_LIM       LEDC_HSTIMER3_DUTY_RES
#define LEDC_HSTIMER3_LIM_M     LEDC_HSTIMER3_DUTY_RES_M
#define LEDC_HSTIMER3_LIM_V     LEDC_HSTIMER3_DUTY_RES_V
#define LEDC_HSTIMER3_LIM_S     LEDC_HSTIMER3_DUTY_RES_S

#define LEDC_HSTIMER3_VALUE_REG          (DR_REG_LEDC_BASE + 0x015C)
/* LEDC_HSTIMER3_CNT : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: software can read this register to get the current counter value
 in high speed timer3*/
#define LEDC_HSTIMER3_CNT  0x000FFFFF
#define LEDC_HSTIMER3_CNT_M  ((LEDC_HSTIMER3_CNT_V)<<(LEDC_HSTIMER3_CNT_S))
#define LEDC_HSTIMER3_CNT_V  0xFFFFF
#define LEDC_HSTIMER3_CNT_S  0

#define LEDC_LSTIMER0_CONF_REG          (DR_REG_LEDC_BASE + 0x0160)
/* LEDC_LSTIMER0_PARA_UP : R/W ;bitpos:[26] ;default: 1'h0 ; */
/*description: Set this bit  to update  reg_div_num_lstime0 and  reg_lstimer0_lim.*/
#define LEDC_LSTIMER0_PARA_UP  (BIT(26))
#define LEDC_LSTIMER0_PARA_UP_M  (BIT(26))
#define LEDC_LSTIMER0_PARA_UP_V  0x1
#define LEDC_LSTIMER0_PARA_UP_S  26
/* LEDC_TICK_SEL_LSTIMER0 : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: This bit is used to choose slow_clk or ref_tick for low speed
 timer0. 1'b1:slow_clk  0:ref_tick*/
#define LEDC_TICK_SEL_LSTIMER0  (BIT(25))
#define LEDC_TICK_SEL_LSTIMER0_M  (BIT(25))
#define LEDC_TICK_SEL_LSTIMER0_V  0x1
#define LEDC_TICK_SEL_LSTIMER0_S  25
/* LEDC_LSTIMER0_RST : R/W ;bitpos:[24] ;default: 1'b1 ; */
/*description: This bit is used to reset low speed timer0 the counter will be 0 after reset.*/
#define LEDC_LSTIMER0_RST  (BIT(24))
#define LEDC_LSTIMER0_RST_M  (BIT(24))
#define LEDC_LSTIMER0_RST_V  0x1
#define LEDC_LSTIMER0_RST_S  24
/* LEDC_LSTIMER0_PAUSE : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: This bit is used to pause the counter in low speed timer0.*/
#define LEDC_LSTIMER0_PAUSE  (BIT(23))
#define LEDC_LSTIMER0_PAUSE_M  (BIT(23))
#define LEDC_LSTIMER0_PAUSE_V  0x1
#define LEDC_LSTIMER0_PAUSE_S  23
/* LEDC_DIV_NUM_LSTIMER0 : R/W ;bitpos:[22:5] ;default: 18'h0 ; */
/*description: This register is used to configure parameter for divider in low
 speed timer0 the least significant eight bits represent the decimal part.*/
#define LEDC_DIV_NUM_LSTIMER0  0x0003FFFF
#define LEDC_DIV_NUM_LSTIMER0_M  ((LEDC_DIV_NUM_LSTIMER0_V)<<(LEDC_DIV_NUM_LSTIMER0_S))
#define LEDC_DIV_NUM_LSTIMER0_V  0x3FFFF
#define LEDC_DIV_NUM_LSTIMER0_S  5
/* LEDC_LSTIMER0_LIM : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: This register controls the range of the counter in low speed
 timer0. the counter range is [0 2**reg_lstimer0_lim] the max bit width for counter is 20.*/
#define LEDC_LSTIMER0_DUTY_RES  0x0000001F
#define LEDC_LSTIMER0_DUTY_RES_M  ((LEDC_LSTIMER0_DUTY_RES_V)<<(LEDC_LSTIMER0_DUTY_RES_S))
#define LEDC_LSTIMER0_DUTY_RES_V  0x1F
#define LEDC_LSTIMER0_DUTY_RES_S  0
// Keep the definitions below to be compatible with previous version
#define LEDC_LSTIMER0_LIM       LEDC_LSTIMER0_DUTY_RES
#define LEDC_LSTIMER0_LIM_M     LEDC_LSTIMER0_DUTY_RES_M
#define LEDC_LSTIMER0_LIM_V     LEDC_LSTIMER0_DUTY_RES_V
#define LEDC_LSTIMER0_LIM_S     LEDC_LSTIMER0_DUTY_RES_S

#define LEDC_LSTIMER0_VALUE_REG          (DR_REG_LEDC_BASE + 0x0164)
/* LEDC_LSTIMER0_CNT : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: software can read this register to get the current counter value
 in low speed timer0.*/
#define LEDC_LSTIMER0_CNT  0x000FFFFF
#define LEDC_LSTIMER0_CNT_M  ((LEDC_LSTIMER0_CNT_V)<<(LEDC_LSTIMER0_CNT_S))
#define LEDC_LSTIMER0_CNT_V  0xFFFFF
#define LEDC_LSTIMER0_CNT_S  0

#define LEDC_LSTIMER1_CONF_REG          (DR_REG_LEDC_BASE + 0x0168)
/* LEDC_LSTIMER1_PARA_UP : R/W ;bitpos:[26] ;default: 1'h0 ; */
/*description: Set this bit  to update  reg_div_num_lstime1 and  reg_lstimer1_lim.*/
#define LEDC_LSTIMER1_PARA_UP  (BIT(26))
#define LEDC_LSTIMER1_PARA_UP_M  (BIT(26))
#define LEDC_LSTIMER1_PARA_UP_V  0x1
#define LEDC_LSTIMER1_PARA_UP_S  26
/* LEDC_TICK_SEL_LSTIMER1 : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: This bit is used to choose slow_clk or ref_tick for low speed
 timer1. 1'b1:slow_clk  0:ref_tick*/
#define LEDC_TICK_SEL_LSTIMER1  (BIT(25))
#define LEDC_TICK_SEL_LSTIMER1_M  (BIT(25))
#define LEDC_TICK_SEL_LSTIMER1_V  0x1
#define LEDC_TICK_SEL_LSTIMER1_S  25
/* LEDC_LSTIMER1_RST : R/W ;bitpos:[24] ;default: 1'b1 ; */
/*description: This bit is used to reset low speed timer1 the counter will be 0 after reset.*/
#define LEDC_LSTIMER1_RST  (BIT(24))
#define LEDC_LSTIMER1_RST_M  (BIT(24))
#define LEDC_LSTIMER1_RST_V  0x1
#define LEDC_LSTIMER1_RST_S  24
/* LEDC_LSTIMER1_PAUSE : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: This bit is used to pause the counter in low speed timer1.*/
#define LEDC_LSTIMER1_PAUSE  (BIT(23))
#define LEDC_LSTIMER1_PAUSE_M  (BIT(23))
#define LEDC_LSTIMER1_PAUSE_V  0x1
#define LEDC_LSTIMER1_PAUSE_S  23
/* LEDC_DIV_NUM_LSTIMER1 : R/W ;bitpos:[22:5] ;default: 18'h0 ; */
/*description: This register is used to configure parameter for divider in low
 speed timer1 the least significant eight bits represent the decimal part.*/
#define LEDC_DIV_NUM_LSTIMER1  0x0003FFFF
#define LEDC_DIV_NUM_LSTIMER1_M  ((LEDC_DIV_NUM_LSTIMER1_V)<<(LEDC_DIV_NUM_LSTIMER1_S))
#define LEDC_DIV_NUM_LSTIMER1_V  0x3FFFF
#define LEDC_DIV_NUM_LSTIMER1_S  5
/* LEDC_LSTIMER1_LIM : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: This register controls the range of the counter in low speed
 timer1. the counter range is [0 2**reg_lstimer1_lim] the max bit width for counter is 20.*/
#define LEDC_LSTIMER1_DUTY_RES  0x0000001F
#define LEDC_LSTIMER1_DUTY_RES_M  ((LEDC_LSTIMER1_DUTY_RES_V)<<(LEDC_LSTIMER1_DUTY_RES_S))
#define LEDC_LSTIMER1_DUTY_RES_V  0x1F
#define LEDC_LSTIMER1_DUTY_RES_S  0
// Keep the definitions below to be compatible with previous version
#define LEDC_LSTIMER1_LIM       LEDC_LSTIMER1_DUTY_RES
#define LEDC_LSTIMER1_LIM_M     LEDC_LSTIMER1_DUTY_RES_M
#define LEDC_LSTIMER1_LIM_V     LEDC_LSTIMER1_DUTY_RES_V
#define LEDC_LSTIMER1_LIM_S     LEDC_LSTIMER1_DUTY_RES_S

#define LEDC_LSTIMER1_VALUE_REG          (DR_REG_LEDC_BASE + 0x016C)
/* LEDC_LSTIMER1_CNT : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: software can read this register to get the current counter value
 in low speed timer1.*/
#define LEDC_LSTIMER1_CNT  0x000FFFFF
#define LEDC_LSTIMER1_CNT_M  ((LEDC_LSTIMER1_CNT_V)<<(LEDC_LSTIMER1_CNT_S))
#define LEDC_LSTIMER1_CNT_V  0xFFFFF
#define LEDC_LSTIMER1_CNT_S  0

#define LEDC_LSTIMER2_CONF_REG          (DR_REG_LEDC_BASE + 0x0170)
/* LEDC_LSTIMER2_PARA_UP : R/W ;bitpos:[26] ;default: 1'h0 ; */
/*description: Set this bit  to update  reg_div_num_lstime2 and  reg_lstimer2_lim.*/
#define LEDC_LSTIMER2_PARA_UP  (BIT(26))
#define LEDC_LSTIMER2_PARA_UP_M  (BIT(26))
#define LEDC_LSTIMER2_PARA_UP_V  0x1
#define LEDC_LSTIMER2_PARA_UP_S  26
/* LEDC_TICK_SEL_LSTIMER2 : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: This bit is used to choose slow_clk or ref_tick for low speed
 timer2. 1'b1:slow_clk  0:ref_tick*/
#define LEDC_TICK_SEL_LSTIMER2  (BIT(25))
#define LEDC_TICK_SEL_LSTIMER2_M  (BIT(25))
#define LEDC_TICK_SEL_LSTIMER2_V  0x1
#define LEDC_TICK_SEL_LSTIMER2_S  25
/* LEDC_LSTIMER2_RST : R/W ;bitpos:[24] ;default: 1'b1 ; */
/*description: This bit is used to reset low speed timer2 the counter will be 0 after reset.*/
#define LEDC_LSTIMER2_RST  (BIT(24))
#define LEDC_LSTIMER2_RST_M  (BIT(24))
#define LEDC_LSTIMER2_RST_V  0x1
#define LEDC_LSTIMER2_RST_S  24
/* LEDC_LSTIMER2_PAUSE : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: This bit is used to pause the counter in low speed timer2.*/
#define LEDC_LSTIMER2_PAUSE  (BIT(23))
#define LEDC_LSTIMER2_PAUSE_M  (BIT(23))
#define LEDC_LSTIMER2_PAUSE_V  0x1
#define LEDC_LSTIMER2_PAUSE_S  23
/* LEDC_DIV_NUM_LSTIMER2 : R/W ;bitpos:[22:5] ;default: 18'h0 ; */
/*description: This register is used to configure parameter for divider in low
 speed timer2 the least significant eight bits represent the decimal part.*/
#define LEDC_DIV_NUM_LSTIMER2  0x0003FFFF
#define LEDC_DIV_NUM_LSTIMER2_M  ((LEDC_DIV_NUM_LSTIMER2_V)<<(LEDC_DIV_NUM_LSTIMER2_S))
#define LEDC_DIV_NUM_LSTIMER2_V  0x3FFFF
#define LEDC_DIV_NUM_LSTIMER2_S  5
/* LEDC_LSTIMER2_LIM : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: This register controls the range of the counter in low speed
 timer2. the counter range is [0 2**reg_lstimer2_lim] the max bit width for counter is 20.*/
#define LEDC_LSTIMER2_DUTY_RES  0x0000001F
#define LEDC_LSTIMER2_DUTY_RES_M  ((LEDC_LSTIMER2_DUTY_RES_V)<<(LEDC_LSTIMER2_DUTY_RES_S))
#define LEDC_LSTIMER2_DUTY_RES_V  0x1F
#define LEDC_LSTIMER2_DUTY_RES_S  0
// Keep the definitions below to be compatible with previous version
#define LEDC_LSTIMER2_LIM       LEDC_LSTIMER2_DUTY_RES
#define LEDC_LSTIMER2_LIM_M     LEDC_LSTIMER2_DUTY_RES_M
#define LEDC_LSTIMER2_LIM_V     LEDC_LSTIMER2_DUTY_RES_V
#define LEDC_LSTIMER2_LIM_S     LEDC_LSTIMER2_DUTY_RES_S

#define LEDC_LSTIMER2_VALUE_REG          (DR_REG_LEDC_BASE + 0x0174)
/* LEDC_LSTIMER2_CNT : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: software can read this register to get the current counter value
 in low speed timer2.*/
#define LEDC_LSTIMER2_CNT  0x000FFFFF
#define LEDC_LSTIMER2_CNT_M  ((LEDC_LSTIMER2_CNT_V)<<(LEDC_LSTIMER2_CNT_S))
#define LEDC_LSTIMER2_CNT_V  0xFFFFF
#define LEDC_LSTIMER2_CNT_S  0

#define LEDC_LSTIMER3_CONF_REG          (DR_REG_LEDC_BASE + 0x0178)
/* LEDC_LSTIMER3_PARA_UP : R/W ;bitpos:[26] ;default: 1'h0 ; */
/*description: Set this bit  to update  reg_div_num_lstime3 and  reg_lstimer3_lim.*/
#define LEDC_LSTIMER3_PARA_UP  (BIT(26))
#define LEDC_LSTIMER3_PARA_UP_M  (BIT(26))
#define LEDC_LSTIMER3_PARA_UP_V  0x1
#define LEDC_LSTIMER3_PARA_UP_S  26
/* LEDC_TICK_SEL_LSTIMER3 : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: This bit is used to choose slow_clk or ref_tick for low speed
 timer3. 1'b1:slow_clk  0:ref_tick*/
#define LEDC_TICK_SEL_LSTIMER3  (BIT(25))
#define LEDC_TICK_SEL_LSTIMER3_M  (BIT(25))
#define LEDC_TICK_SEL_LSTIMER3_V  0x1
#define LEDC_TICK_SEL_LSTIMER3_S  25
/* LEDC_LSTIMER3_RST : R/W ;bitpos:[24] ;default: 1'b1 ; */
/*description: This bit is used to reset low speed timer3 the counter will be 0 after reset.*/
#define LEDC_LSTIMER3_RST  (BIT(24))
#define LEDC_LSTIMER3_RST_M  (BIT(24))
#define LEDC_LSTIMER3_RST_V  0x1
#define LEDC_LSTIMER3_RST_S  24
/* LEDC_LSTIMER3_PAUSE : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: This bit is used to pause the counter in low speed timer3.*/
#define LEDC_LSTIMER3_PAUSE  (BIT(23))
#define LEDC_LSTIMER3_PAUSE_M  (BIT(23))
#define LEDC_LSTIMER3_PAUSE_V  0x1
#define LEDC_LSTIMER3_PAUSE_S  23
/* LEDC_DIV_NUM_LSTIMER3 : R/W ;bitpos:[22:5] ;default: 18'h0 ; */
/*description: This register is used to configure parameter for divider in low
 speed timer3 the least significant eight bits represent the decimal part.*/
#define LEDC_DIV_NUM_LSTIMER3  0x0003FFFF
#define LEDC_DIV_NUM_LSTIMER3_M  ((LEDC_DIV_NUM_LSTIMER3_V)<<(LEDC_DIV_NUM_LSTIMER3_S))
#define LEDC_DIV_NUM_LSTIMER3_V  0x3FFFF
#define LEDC_DIV_NUM_LSTIMER3_S  5
/* LEDC_LSTIMER3_LIM : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: This register controls the range of the counter in low speed
 timer3. the counter range is [0 2**reg_lstimer3_lim] the max bit width for counter is 20.*/
#define LEDC_LSTIMER3_DUTY_RES  0x0000001F
#define LEDC_LSTIMER3_DUTY_RES_M  ((LEDC_LSTIMER3_DUTY_RES_V)<<(LEDC_LSTIMER3_DUTY_RES_S))
#define LEDC_LSTIMER3_DUTY_RES_V  0x1F
#define LEDC_LSTIMER3_DUTY_RES_S  0
// Keep the definitions below to be compatible with previous version
#define LEDC_LSTIMER3_LIM       LEDC_LSTIMER3_DUTY_RES
#define LEDC_LSTIMER3_LIM_M     LEDC_LSTIMER3_DUTY_RES_M
#define LEDC_LSTIMER3_LIM_V     LEDC_LSTIMER3_DUTY_RES_V
#define LEDC_LSTIMER3_LIM_S     LEDC_LSTIMER3_DUTY_RES_S

#define LEDC_LSTIMER3_VALUE_REG          (DR_REG_LEDC_BASE + 0x017C)
/* LEDC_LSTIMER3_CNT : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: software can read this register to get the current counter value
 in low speed timer3.*/
#define LEDC_LSTIMER3_CNT  0x000FFFFF
#define LEDC_LSTIMER3_CNT_M  ((LEDC_LSTIMER3_CNT_V)<<(LEDC_LSTIMER3_CNT_S))
#define LEDC_LSTIMER3_CNT_V  0xFFFFF
#define LEDC_LSTIMER3_CNT_S  0

#define LEDC_INT_RAW_REG          (DR_REG_LEDC_BASE + 0x0180)
/* LEDC_DUTY_CHNG_END_LSCH7_INT_RAW : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: The interrupt raw bit for low speed channel 7 duty change done.*/
#define LEDC_DUTY_CHNG_END_LSCH7_INT_RAW  (BIT(23))
#define LEDC_DUTY_CHNG_END_LSCH7_INT_RAW_M  (BIT(23))
#define LEDC_DUTY_CHNG_END_LSCH7_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH7_INT_RAW_S  23
/* LEDC_DUTY_CHNG_END_LSCH6_INT_RAW : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: The interrupt raw bit for low speed channel 6 duty change done.*/
#define LEDC_DUTY_CHNG_END_LSCH6_INT_RAW  (BIT(22))
#define LEDC_DUTY_CHNG_END_LSCH6_INT_RAW_M  (BIT(22))
#define LEDC_DUTY_CHNG_END_LSCH6_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH6_INT_RAW_S  22
/* LEDC_DUTY_CHNG_END_LSCH5_INT_RAW : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: The interrupt raw bit for low speed channel 5 duty change done.*/
#define LEDC_DUTY_CHNG_END_LSCH5_INT_RAW  (BIT(21))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_RAW_M  (BIT(21))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH5_INT_RAW_S  21
/* LEDC_DUTY_CHNG_END_LSCH4_INT_RAW : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: The interrupt raw bit for low speed channel 4 duty change done.*/
#define LEDC_DUTY_CHNG_END_LSCH4_INT_RAW  (BIT(20))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_RAW_M  (BIT(20))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH4_INT_RAW_S  20
/* LEDC_DUTY_CHNG_END_LSCH3_INT_RAW : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: The interrupt raw bit for low speed channel 3 duty change done.*/
#define LEDC_DUTY_CHNG_END_LSCH3_INT_RAW  (BIT(19))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_RAW_M  (BIT(19))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH3_INT_RAW_S  19
/* LEDC_DUTY_CHNG_END_LSCH2_INT_RAW : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: The interrupt raw bit for low speed channel 2 duty change done.*/
#define LEDC_DUTY_CHNG_END_LSCH2_INT_RAW  (BIT(18))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_RAW_M  (BIT(18))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH2_INT_RAW_S  18
/* LEDC_DUTY_CHNG_END_LSCH1_INT_RAW : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: The interrupt raw bit for low speed channel 1 duty change done.*/
#define LEDC_DUTY_CHNG_END_LSCH1_INT_RAW  (BIT(17))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_RAW_M  (BIT(17))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH1_INT_RAW_S  17
/* LEDC_DUTY_CHNG_END_LSCH0_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: The interrupt raw bit for low speed channel 0 duty change done.*/
#define LEDC_DUTY_CHNG_END_LSCH0_INT_RAW  (BIT(16))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_RAW_M  (BIT(16))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH0_INT_RAW_S  16
/* LEDC_DUTY_CHNG_END_HSCH7_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: The interrupt raw bit for high speed channel 7 duty change done.*/
#define LEDC_DUTY_CHNG_END_HSCH7_INT_RAW  (BIT(15))
#define LEDC_DUTY_CHNG_END_HSCH7_INT_RAW_M  (BIT(15))
#define LEDC_DUTY_CHNG_END_HSCH7_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH7_INT_RAW_S  15
/* LEDC_DUTY_CHNG_END_HSCH6_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: The interrupt raw bit for high speed channel 6 duty change done.*/
#define LEDC_DUTY_CHNG_END_HSCH6_INT_RAW  (BIT(14))
#define LEDC_DUTY_CHNG_END_HSCH6_INT_RAW_M  (BIT(14))
#define LEDC_DUTY_CHNG_END_HSCH6_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH6_INT_RAW_S  14
/* LEDC_DUTY_CHNG_END_HSCH5_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: The interrupt raw bit for high speed channel 5 duty change done.*/
#define LEDC_DUTY_CHNG_END_HSCH5_INT_RAW  (BIT(13))
#define LEDC_DUTY_CHNG_END_HSCH5_INT_RAW_M  (BIT(13))
#define LEDC_DUTY_CHNG_END_HSCH5_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH5_INT_RAW_S  13
/* LEDC_DUTY_CHNG_END_HSCH4_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: The interrupt raw bit for high speed channel 4 duty change done.*/
#define LEDC_DUTY_CHNG_END_HSCH4_INT_RAW  (BIT(12))
#define LEDC_DUTY_CHNG_END_HSCH4_INT_RAW_M  (BIT(12))
#define LEDC_DUTY_CHNG_END_HSCH4_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH4_INT_RAW_S  12
/* LEDC_DUTY_CHNG_END_HSCH3_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The interrupt raw bit for high speed channel 3 duty change done.*/
#define LEDC_DUTY_CHNG_END_HSCH3_INT_RAW  (BIT(11))
#define LEDC_DUTY_CHNG_END_HSCH3_INT_RAW_M  (BIT(11))
#define LEDC_DUTY_CHNG_END_HSCH3_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH3_INT_RAW_S  11
/* LEDC_DUTY_CHNG_END_HSCH2_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The interrupt raw bit for high speed channel 2 duty change done.*/
#define LEDC_DUTY_CHNG_END_HSCH2_INT_RAW  (BIT(10))
#define LEDC_DUTY_CHNG_END_HSCH2_INT_RAW_M  (BIT(10))
#define LEDC_DUTY_CHNG_END_HSCH2_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH2_INT_RAW_S  10
/* LEDC_DUTY_CHNG_END_HSCH1_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt raw bit for high speed channel 1 duty change done.*/
#define LEDC_DUTY_CHNG_END_HSCH1_INT_RAW  (BIT(9))
#define LEDC_DUTY_CHNG_END_HSCH1_INT_RAW_M  (BIT(9))
#define LEDC_DUTY_CHNG_END_HSCH1_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH1_INT_RAW_S  9
/* LEDC_DUTY_CHNG_END_HSCH0_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt raw bit for high speed channel 0 duty change done.*/
#define LEDC_DUTY_CHNG_END_HSCH0_INT_RAW  (BIT(8))
#define LEDC_DUTY_CHNG_END_HSCH0_INT_RAW_M  (BIT(8))
#define LEDC_DUTY_CHNG_END_HSCH0_INT_RAW_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH0_INT_RAW_S  8
/* LEDC_LSTIMER3_OVF_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt raw bit for low speed channel3  counter overflow.*/
#define LEDC_LSTIMER3_OVF_INT_RAW  (BIT(7))
#define LEDC_LSTIMER3_OVF_INT_RAW_M  (BIT(7))
#define LEDC_LSTIMER3_OVF_INT_RAW_V  0x1
#define LEDC_LSTIMER3_OVF_INT_RAW_S  7
/* LEDC_LSTIMER2_OVF_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt raw bit for low speed channel2  counter overflow.*/
#define LEDC_LSTIMER2_OVF_INT_RAW  (BIT(6))
#define LEDC_LSTIMER2_OVF_INT_RAW_M  (BIT(6))
#define LEDC_LSTIMER2_OVF_INT_RAW_V  0x1
#define LEDC_LSTIMER2_OVF_INT_RAW_S  6
/* LEDC_LSTIMER1_OVF_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt raw bit for low speed channel1  counter overflow.*/
#define LEDC_LSTIMER1_OVF_INT_RAW  (BIT(5))
#define LEDC_LSTIMER1_OVF_INT_RAW_M  (BIT(5))
#define LEDC_LSTIMER1_OVF_INT_RAW_V  0x1
#define LEDC_LSTIMER1_OVF_INT_RAW_S  5
/* LEDC_LSTIMER0_OVF_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt raw bit for low speed channel0  counter overflow.*/
#define LEDC_LSTIMER0_OVF_INT_RAW  (BIT(4))
#define LEDC_LSTIMER0_OVF_INT_RAW_M  (BIT(4))
#define LEDC_LSTIMER0_OVF_INT_RAW_V  0x1
#define LEDC_LSTIMER0_OVF_INT_RAW_S  4
/* LEDC_HSTIMER3_OVF_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt raw bit for high speed channel3  counter overflow.*/
#define LEDC_HSTIMER3_OVF_INT_RAW  (BIT(3))
#define LEDC_HSTIMER3_OVF_INT_RAW_M  (BIT(3))
#define LEDC_HSTIMER3_OVF_INT_RAW_V  0x1
#define LEDC_HSTIMER3_OVF_INT_RAW_S  3
/* LEDC_HSTIMER2_OVF_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt raw bit for high speed channel2  counter overflow.*/
#define LEDC_HSTIMER2_OVF_INT_RAW  (BIT(2))
#define LEDC_HSTIMER2_OVF_INT_RAW_M  (BIT(2))
#define LEDC_HSTIMER2_OVF_INT_RAW_V  0x1
#define LEDC_HSTIMER2_OVF_INT_RAW_S  2
/* LEDC_HSTIMER1_OVF_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt raw bit for high speed channel1  counter overflow.*/
#define LEDC_HSTIMER1_OVF_INT_RAW  (BIT(1))
#define LEDC_HSTIMER1_OVF_INT_RAW_M  (BIT(1))
#define LEDC_HSTIMER1_OVF_INT_RAW_V  0x1
#define LEDC_HSTIMER1_OVF_INT_RAW_S  1
/* LEDC_HSTIMER0_OVF_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt raw bit for high speed channel0  counter overflow.*/
#define LEDC_HSTIMER0_OVF_INT_RAW  (BIT(0))
#define LEDC_HSTIMER0_OVF_INT_RAW_M  (BIT(0))
#define LEDC_HSTIMER0_OVF_INT_RAW_V  0x1
#define LEDC_HSTIMER0_OVF_INT_RAW_S  0

#define LEDC_INT_ST_REG          (DR_REG_LEDC_BASE + 0x0184)
/* LEDC_DUTY_CHNG_END_LSCH7_INT_ST : RO ;bitpos:[23] ;default: 1'h0 ; */
/*description: The interrupt status bit for low speed channel 7 duty change done event*/
#define LEDC_DUTY_CHNG_END_LSCH7_INT_ST  (BIT(23))
#define LEDC_DUTY_CHNG_END_LSCH7_INT_ST_M  (BIT(23))
#define LEDC_DUTY_CHNG_END_LSCH7_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH7_INT_ST_S  23
/* LEDC_DUTY_CHNG_END_LSCH6_INT_ST : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: The interrupt status bit for low speed channel 6 duty change done event.*/
#define LEDC_DUTY_CHNG_END_LSCH6_INT_ST  (BIT(22))
#define LEDC_DUTY_CHNG_END_LSCH6_INT_ST_M  (BIT(22))
#define LEDC_DUTY_CHNG_END_LSCH6_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH6_INT_ST_S  22
/* LEDC_DUTY_CHNG_END_LSCH5_INT_ST : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: The interrupt status bit for low speed channel 5 duty change done event.*/
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ST  (BIT(21))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ST_M  (BIT(21))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ST_S  21
/* LEDC_DUTY_CHNG_END_LSCH4_INT_ST : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: The interrupt status bit for low speed channel 4 duty change done event.*/
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ST  (BIT(20))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ST_M  (BIT(20))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ST_S  20
/* LEDC_DUTY_CHNG_END_LSCH3_INT_ST : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: The interrupt status bit for low speed channel 3 duty change done event.*/
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ST  (BIT(19))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ST_M  (BIT(19))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ST_S  19
/* LEDC_DUTY_CHNG_END_LSCH2_INT_ST : RO ;bitpos:[18] ;default: 1'b0 ; */
/*description: The interrupt status bit for low speed channel 2 duty change done event.*/
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ST  (BIT(18))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ST_M  (BIT(18))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ST_S  18
/* LEDC_DUTY_CHNG_END_LSCH1_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: The interrupt status bit for low speed channel 1 duty change done event.*/
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ST  (BIT(17))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ST_M  (BIT(17))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ST_S  17
/* LEDC_DUTY_CHNG_END_LSCH0_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: The interrupt status bit for low speed channel 0 duty change done event.*/
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ST  (BIT(16))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ST_M  (BIT(16))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ST_S  16
/* LEDC_DUTY_CHNG_END_HSCH7_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: The interrupt status bit for high speed channel 7 duty change done event.*/
#define LEDC_DUTY_CHNG_END_HSCH7_INT_ST  (BIT(15))
#define LEDC_DUTY_CHNG_END_HSCH7_INT_ST_M  (BIT(15))
#define LEDC_DUTY_CHNG_END_HSCH7_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH7_INT_ST_S  15
/* LEDC_DUTY_CHNG_END_HSCH6_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: The interrupt status bit for high speed channel 6 duty change done event.*/
#define LEDC_DUTY_CHNG_END_HSCH6_INT_ST  (BIT(14))
#define LEDC_DUTY_CHNG_END_HSCH6_INT_ST_M  (BIT(14))
#define LEDC_DUTY_CHNG_END_HSCH6_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH6_INT_ST_S  14
/* LEDC_DUTY_CHNG_END_HSCH5_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: The interrupt status bit for high speed channel 5 duty change done event.*/
#define LEDC_DUTY_CHNG_END_HSCH5_INT_ST  (BIT(13))
#define LEDC_DUTY_CHNG_END_HSCH5_INT_ST_M  (BIT(13))
#define LEDC_DUTY_CHNG_END_HSCH5_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH5_INT_ST_S  13
/* LEDC_DUTY_CHNG_END_HSCH4_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: The interrupt status bit for high speed channel 4 duty change done event.*/
#define LEDC_DUTY_CHNG_END_HSCH4_INT_ST  (BIT(12))
#define LEDC_DUTY_CHNG_END_HSCH4_INT_ST_M  (BIT(12))
#define LEDC_DUTY_CHNG_END_HSCH4_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH4_INT_ST_S  12
/* LEDC_DUTY_CHNG_END_HSCH3_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The interrupt status bit for high speed channel 3 duty change done event.*/
#define LEDC_DUTY_CHNG_END_HSCH3_INT_ST  (BIT(11))
#define LEDC_DUTY_CHNG_END_HSCH3_INT_ST_M  (BIT(11))
#define LEDC_DUTY_CHNG_END_HSCH3_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH3_INT_ST_S  11
/* LEDC_DUTY_CHNG_END_HSCH2_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The interrupt status bit for high speed channel 2 duty change done event.*/
#define LEDC_DUTY_CHNG_END_HSCH2_INT_ST  (BIT(10))
#define LEDC_DUTY_CHNG_END_HSCH2_INT_ST_M  (BIT(10))
#define LEDC_DUTY_CHNG_END_HSCH2_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH2_INT_ST_S  10
/* LEDC_DUTY_CHNG_END_HSCH1_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt status bit for high speed channel 1 duty change done event.*/
#define LEDC_DUTY_CHNG_END_HSCH1_INT_ST  (BIT(9))
#define LEDC_DUTY_CHNG_END_HSCH1_INT_ST_M  (BIT(9))
#define LEDC_DUTY_CHNG_END_HSCH1_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH1_INT_ST_S  9
/* LEDC_DUTY_CHNG_END_HSCH0_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt status bit for high speed channel 0 duty change done event.*/
#define LEDC_DUTY_CHNG_END_HSCH0_INT_ST  (BIT(8))
#define LEDC_DUTY_CHNG_END_HSCH0_INT_ST_M  (BIT(8))
#define LEDC_DUTY_CHNG_END_HSCH0_INT_ST_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH0_INT_ST_S  8
/* LEDC_LSTIMER3_OVF_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt status bit for low speed channel3  counter overflow event.*/
#define LEDC_LSTIMER3_OVF_INT_ST  (BIT(7))
#define LEDC_LSTIMER3_OVF_INT_ST_M  (BIT(7))
#define LEDC_LSTIMER3_OVF_INT_ST_V  0x1
#define LEDC_LSTIMER3_OVF_INT_ST_S  7
/* LEDC_LSTIMER2_OVF_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt status bit for low speed channel2  counter overflow event.*/
#define LEDC_LSTIMER2_OVF_INT_ST  (BIT(6))
#define LEDC_LSTIMER2_OVF_INT_ST_M  (BIT(6))
#define LEDC_LSTIMER2_OVF_INT_ST_V  0x1
#define LEDC_LSTIMER2_OVF_INT_ST_S  6
/* LEDC_LSTIMER1_OVF_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt status bit for low speed channel1  counter overflow event.*/
#define LEDC_LSTIMER1_OVF_INT_ST  (BIT(5))
#define LEDC_LSTIMER1_OVF_INT_ST_M  (BIT(5))
#define LEDC_LSTIMER1_OVF_INT_ST_V  0x1
#define LEDC_LSTIMER1_OVF_INT_ST_S  5
/* LEDC_LSTIMER0_OVF_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt status bit for low speed channel0  counter overflow event.*/
#define LEDC_LSTIMER0_OVF_INT_ST  (BIT(4))
#define LEDC_LSTIMER0_OVF_INT_ST_M  (BIT(4))
#define LEDC_LSTIMER0_OVF_INT_ST_V  0x1
#define LEDC_LSTIMER0_OVF_INT_ST_S  4
/* LEDC_HSTIMER3_OVF_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt status bit for high speed channel3  counter overflow event.*/
#define LEDC_HSTIMER3_OVF_INT_ST  (BIT(3))
#define LEDC_HSTIMER3_OVF_INT_ST_M  (BIT(3))
#define LEDC_HSTIMER3_OVF_INT_ST_V  0x1
#define LEDC_HSTIMER3_OVF_INT_ST_S  3
/* LEDC_HSTIMER2_OVF_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt status bit for high speed channel2  counter overflow event.*/
#define LEDC_HSTIMER2_OVF_INT_ST  (BIT(2))
#define LEDC_HSTIMER2_OVF_INT_ST_M  (BIT(2))
#define LEDC_HSTIMER2_OVF_INT_ST_V  0x1
#define LEDC_HSTIMER2_OVF_INT_ST_S  2
/* LEDC_HSTIMER1_OVF_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt status bit for high speed channel1  counter overflow event.*/
#define LEDC_HSTIMER1_OVF_INT_ST  (BIT(1))
#define LEDC_HSTIMER1_OVF_INT_ST_M  (BIT(1))
#define LEDC_HSTIMER1_OVF_INT_ST_V  0x1
#define LEDC_HSTIMER1_OVF_INT_ST_S  1
/* LEDC_HSTIMER0_OVF_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt status bit for high speed channel0  counter overflow event.*/
#define LEDC_HSTIMER0_OVF_INT_ST  (BIT(0))
#define LEDC_HSTIMER0_OVF_INT_ST_M  (BIT(0))
#define LEDC_HSTIMER0_OVF_INT_ST_V  0x1
#define LEDC_HSTIMER0_OVF_INT_ST_S  0

#define LEDC_INT_ENA_REG          (DR_REG_LEDC_BASE + 0x0188)
/* LEDC_DUTY_CHNG_END_LSCH7_INT_ENA : R/W ;bitpos:[23] ;default: 1'h0 ; */
/*description: The interrupt enable bit for low speed channel 7 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH7_INT_ENA  (BIT(23))
#define LEDC_DUTY_CHNG_END_LSCH7_INT_ENA_M  (BIT(23))
#define LEDC_DUTY_CHNG_END_LSCH7_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH7_INT_ENA_S  23
/* LEDC_DUTY_CHNG_END_LSCH6_INT_ENA : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: The interrupt enable bit for low speed channel 6 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH6_INT_ENA  (BIT(22))
#define LEDC_DUTY_CHNG_END_LSCH6_INT_ENA_M  (BIT(22))
#define LEDC_DUTY_CHNG_END_LSCH6_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH6_INT_ENA_S  22
/* LEDC_DUTY_CHNG_END_LSCH5_INT_ENA : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: The interrupt enable bit for low speed channel 5 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ENA  (BIT(21))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ENA_M  (BIT(21))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH5_INT_ENA_S  21
/* LEDC_DUTY_CHNG_END_LSCH4_INT_ENA : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: The interrupt enable bit for low speed channel 4 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ENA  (BIT(20))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ENA_M  (BIT(20))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH4_INT_ENA_S  20
/* LEDC_DUTY_CHNG_END_LSCH3_INT_ENA : R/W ;bitpos:[19] ;default: 1'b0 ; */
/*description: The interrupt enable bit for low speed channel 3 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ENA  (BIT(19))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ENA_M  (BIT(19))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH3_INT_ENA_S  19
/* LEDC_DUTY_CHNG_END_LSCH2_INT_ENA : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: The interrupt enable bit for low speed channel 2 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ENA  (BIT(18))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ENA_M  (BIT(18))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH2_INT_ENA_S  18
/* LEDC_DUTY_CHNG_END_LSCH1_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: The interrupt enable bit for low speed channel 1 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ENA  (BIT(17))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ENA_M  (BIT(17))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH1_INT_ENA_S  17
/* LEDC_DUTY_CHNG_END_LSCH0_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: The interrupt enable bit for low speed channel 0 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ENA  (BIT(16))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ENA_M  (BIT(16))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH0_INT_ENA_S  16
/* LEDC_DUTY_CHNG_END_HSCH7_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: The interrupt enable bit for high speed channel 7 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH7_INT_ENA  (BIT(15))
#define LEDC_DUTY_CHNG_END_HSCH7_INT_ENA_M  (BIT(15))
#define LEDC_DUTY_CHNG_END_HSCH7_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH7_INT_ENA_S  15
/* LEDC_DUTY_CHNG_END_HSCH6_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: The interrupt enable bit for high speed channel 6 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH6_INT_ENA  (BIT(14))
#define LEDC_DUTY_CHNG_END_HSCH6_INT_ENA_M  (BIT(14))
#define LEDC_DUTY_CHNG_END_HSCH6_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH6_INT_ENA_S  14
/* LEDC_DUTY_CHNG_END_HSCH5_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: The interrupt enable bit for high speed channel 5 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH5_INT_ENA  (BIT(13))
#define LEDC_DUTY_CHNG_END_HSCH5_INT_ENA_M  (BIT(13))
#define LEDC_DUTY_CHNG_END_HSCH5_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH5_INT_ENA_S  13
/* LEDC_DUTY_CHNG_END_HSCH4_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: The interrupt enable bit for high speed channel 4 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH4_INT_ENA  (BIT(12))
#define LEDC_DUTY_CHNG_END_HSCH4_INT_ENA_M  (BIT(12))
#define LEDC_DUTY_CHNG_END_HSCH4_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH4_INT_ENA_S  12
/* LEDC_DUTY_CHNG_END_HSCH3_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: The interrupt enable bit for high speed channel 3 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH3_INT_ENA  (BIT(11))
#define LEDC_DUTY_CHNG_END_HSCH3_INT_ENA_M  (BIT(11))
#define LEDC_DUTY_CHNG_END_HSCH3_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH3_INT_ENA_S  11
/* LEDC_DUTY_CHNG_END_HSCH2_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: The interrupt enable bit for high speed channel 2 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH2_INT_ENA  (BIT(10))
#define LEDC_DUTY_CHNG_END_HSCH2_INT_ENA_M  (BIT(10))
#define LEDC_DUTY_CHNG_END_HSCH2_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH2_INT_ENA_S  10
/* LEDC_DUTY_CHNG_END_HSCH1_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt enable bit for high speed channel 1 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH1_INT_ENA  (BIT(9))
#define LEDC_DUTY_CHNG_END_HSCH1_INT_ENA_M  (BIT(9))
#define LEDC_DUTY_CHNG_END_HSCH1_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH1_INT_ENA_S  9
/* LEDC_DUTY_CHNG_END_HSCH0_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt enable bit for high speed channel 0 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH0_INT_ENA  (BIT(8))
#define LEDC_DUTY_CHNG_END_HSCH0_INT_ENA_M  (BIT(8))
#define LEDC_DUTY_CHNG_END_HSCH0_INT_ENA_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH0_INT_ENA_S  8
/* LEDC_LSTIMER3_OVF_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for low speed channel3  counter overflow interrupt.*/
#define LEDC_LSTIMER3_OVF_INT_ENA  (BIT(7))
#define LEDC_LSTIMER3_OVF_INT_ENA_M  (BIT(7))
#define LEDC_LSTIMER3_OVF_INT_ENA_V  0x1
#define LEDC_LSTIMER3_OVF_INT_ENA_S  7
/* LEDC_LSTIMER2_OVF_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for low speed channel2  counter overflow interrupt.*/
#define LEDC_LSTIMER2_OVF_INT_ENA  (BIT(6))
#define LEDC_LSTIMER2_OVF_INT_ENA_M  (BIT(6))
#define LEDC_LSTIMER2_OVF_INT_ENA_V  0x1
#define LEDC_LSTIMER2_OVF_INT_ENA_S  6
/* LEDC_LSTIMER1_OVF_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for low speed channel1  counter overflow interrupt.*/
#define LEDC_LSTIMER1_OVF_INT_ENA  (BIT(5))
#define LEDC_LSTIMER1_OVF_INT_ENA_M  (BIT(5))
#define LEDC_LSTIMER1_OVF_INT_ENA_V  0x1
#define LEDC_LSTIMER1_OVF_INT_ENA_S  5
/* LEDC_LSTIMER0_OVF_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for low speed channel0  counter overflow interrupt.*/
#define LEDC_LSTIMER0_OVF_INT_ENA  (BIT(4))
#define LEDC_LSTIMER0_OVF_INT_ENA_M  (BIT(4))
#define LEDC_LSTIMER0_OVF_INT_ENA_V  0x1
#define LEDC_LSTIMER0_OVF_INT_ENA_S  4
/* LEDC_HSTIMER3_OVF_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for high speed channel3  counter overflow interrupt.*/
#define LEDC_HSTIMER3_OVF_INT_ENA  (BIT(3))
#define LEDC_HSTIMER3_OVF_INT_ENA_M  (BIT(3))
#define LEDC_HSTIMER3_OVF_INT_ENA_V  0x1
#define LEDC_HSTIMER3_OVF_INT_ENA_S  3
/* LEDC_HSTIMER2_OVF_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for high speed channel2  counter overflow interrupt.*/
#define LEDC_HSTIMER2_OVF_INT_ENA  (BIT(2))
#define LEDC_HSTIMER2_OVF_INT_ENA_M  (BIT(2))
#define LEDC_HSTIMER2_OVF_INT_ENA_V  0x1
#define LEDC_HSTIMER2_OVF_INT_ENA_S  2
/* LEDC_HSTIMER1_OVF_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for high speed channel1  counter overflow interrupt.*/
#define LEDC_HSTIMER1_OVF_INT_ENA  (BIT(1))
#define LEDC_HSTIMER1_OVF_INT_ENA_M  (BIT(1))
#define LEDC_HSTIMER1_OVF_INT_ENA_V  0x1
#define LEDC_HSTIMER1_OVF_INT_ENA_S  1
/* LEDC_HSTIMER0_OVF_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for high speed channel0  counter overflow interrupt.*/
#define LEDC_HSTIMER0_OVF_INT_ENA  (BIT(0))
#define LEDC_HSTIMER0_OVF_INT_ENA_M  (BIT(0))
#define LEDC_HSTIMER0_OVF_INT_ENA_V  0x1
#define LEDC_HSTIMER0_OVF_INT_ENA_S  0

#define LEDC_INT_CLR_REG          (DR_REG_LEDC_BASE + 0x018C)
/* LEDC_DUTY_CHNG_END_LSCH7_INT_CLR : WO ;bitpos:[23] ;default: 1'h0 ; */
/*description: Set this  bit to clear  low speed channel 7 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH7_INT_CLR  (BIT(23))
#define LEDC_DUTY_CHNG_END_LSCH7_INT_CLR_M  (BIT(23))
#define LEDC_DUTY_CHNG_END_LSCH7_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH7_INT_CLR_S  23
/* LEDC_DUTY_CHNG_END_LSCH6_INT_CLR : WO ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this  bit to clear  low speed channel 6 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH6_INT_CLR  (BIT(22))
#define LEDC_DUTY_CHNG_END_LSCH6_INT_CLR_M  (BIT(22))
#define LEDC_DUTY_CHNG_END_LSCH6_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH6_INT_CLR_S  22
/* LEDC_DUTY_CHNG_END_LSCH5_INT_CLR : WO ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this  bit to clear  low speed channel 5 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH5_INT_CLR  (BIT(21))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_CLR_M  (BIT(21))
#define LEDC_DUTY_CHNG_END_LSCH5_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH5_INT_CLR_S  21
/* LEDC_DUTY_CHNG_END_LSCH4_INT_CLR : WO ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this  bit to clear  low speed channel 4 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH4_INT_CLR  (BIT(20))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_CLR_M  (BIT(20))
#define LEDC_DUTY_CHNG_END_LSCH4_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH4_INT_CLR_S  20
/* LEDC_DUTY_CHNG_END_LSCH3_INT_CLR : WO ;bitpos:[19] ;default: 1'b0 ; */
/*description: Set this  bit to clear  low speed channel 3 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH3_INT_CLR  (BIT(19))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_CLR_M  (BIT(19))
#define LEDC_DUTY_CHNG_END_LSCH3_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH3_INT_CLR_S  19
/* LEDC_DUTY_CHNG_END_LSCH2_INT_CLR : WO ;bitpos:[18] ;default: 1'b0 ; */
/*description: Set this  bit to clear  low speed channel 2 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH2_INT_CLR  (BIT(18))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_CLR_M  (BIT(18))
#define LEDC_DUTY_CHNG_END_LSCH2_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH2_INT_CLR_S  18
/* LEDC_DUTY_CHNG_END_LSCH1_INT_CLR : WO ;bitpos:[17] ;default: 1'b0 ; */
/*description: Set this  bit to clear  low speed channel 1 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH1_INT_CLR  (BIT(17))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_CLR_M  (BIT(17))
#define LEDC_DUTY_CHNG_END_LSCH1_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH1_INT_CLR_S  17
/* LEDC_DUTY_CHNG_END_LSCH0_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this  bit to clear  low speed channel 0 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_LSCH0_INT_CLR  (BIT(16))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_CLR_M  (BIT(16))
#define LEDC_DUTY_CHNG_END_LSCH0_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_LSCH0_INT_CLR_S  16
/* LEDC_DUTY_CHNG_END_HSCH7_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: Set this  bit to clear  high speed channel 7 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH7_INT_CLR  (BIT(15))
#define LEDC_DUTY_CHNG_END_HSCH7_INT_CLR_M  (BIT(15))
#define LEDC_DUTY_CHNG_END_HSCH7_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH7_INT_CLR_S  15
/* LEDC_DUTY_CHNG_END_HSCH6_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: Set this  bit to clear  high speed channel 6 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH6_INT_CLR  (BIT(14))
#define LEDC_DUTY_CHNG_END_HSCH6_INT_CLR_M  (BIT(14))
#define LEDC_DUTY_CHNG_END_HSCH6_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH6_INT_CLR_S  14
/* LEDC_DUTY_CHNG_END_HSCH5_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: Set this  bit to clear  high speed channel 5 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH5_INT_CLR  (BIT(13))
#define LEDC_DUTY_CHNG_END_HSCH5_INT_CLR_M  (BIT(13))
#define LEDC_DUTY_CHNG_END_HSCH5_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH5_INT_CLR_S  13
/* LEDC_DUTY_CHNG_END_HSCH4_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this  bit to clear  high speed channel 4 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH4_INT_CLR  (BIT(12))
#define LEDC_DUTY_CHNG_END_HSCH4_INT_CLR_M  (BIT(12))
#define LEDC_DUTY_CHNG_END_HSCH4_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH4_INT_CLR_S  12
/* LEDC_DUTY_CHNG_END_HSCH3_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: Set this  bit to clear  high speed channel 3 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH3_INT_CLR  (BIT(11))
#define LEDC_DUTY_CHNG_END_HSCH3_INT_CLR_M  (BIT(11))
#define LEDC_DUTY_CHNG_END_HSCH3_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH3_INT_CLR_S  11
/* LEDC_DUTY_CHNG_END_HSCH2_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this  bit to clear  high speed channel 2 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH2_INT_CLR  (BIT(10))
#define LEDC_DUTY_CHNG_END_HSCH2_INT_CLR_M  (BIT(10))
#define LEDC_DUTY_CHNG_END_HSCH2_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH2_INT_CLR_S  10
/* LEDC_DUTY_CHNG_END_HSCH1_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this  bit to clear  high speed channel 1 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH1_INT_CLR  (BIT(9))
#define LEDC_DUTY_CHNG_END_HSCH1_INT_CLR_M  (BIT(9))
#define LEDC_DUTY_CHNG_END_HSCH1_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH1_INT_CLR_S  9
/* LEDC_DUTY_CHNG_END_HSCH0_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this  bit to clear  high speed channel 0 duty change done interrupt.*/
#define LEDC_DUTY_CHNG_END_HSCH0_INT_CLR  (BIT(8))
#define LEDC_DUTY_CHNG_END_HSCH0_INT_CLR_M  (BIT(8))
#define LEDC_DUTY_CHNG_END_HSCH0_INT_CLR_V  0x1
#define LEDC_DUTY_CHNG_END_HSCH0_INT_CLR_S  8
/* LEDC_LSTIMER3_OVF_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this  bit to clear  low speed channel3  counter overflow interrupt.*/
#define LEDC_LSTIMER3_OVF_INT_CLR  (BIT(7))
#define LEDC_LSTIMER3_OVF_INT_CLR_M  (BIT(7))
#define LEDC_LSTIMER3_OVF_INT_CLR_V  0x1
#define LEDC_LSTIMER3_OVF_INT_CLR_S  7
/* LEDC_LSTIMER2_OVF_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this  bit to clear  low speed channel2  counter overflow interrupt.*/
#define LEDC_LSTIMER2_OVF_INT_CLR  (BIT(6))
#define LEDC_LSTIMER2_OVF_INT_CLR_M  (BIT(6))
#define LEDC_LSTIMER2_OVF_INT_CLR_V  0x1
#define LEDC_LSTIMER2_OVF_INT_CLR_S  6
/* LEDC_LSTIMER1_OVF_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this  bit to clear  low speed channel1  counter overflow interrupt.*/
#define LEDC_LSTIMER1_OVF_INT_CLR  (BIT(5))
#define LEDC_LSTIMER1_OVF_INT_CLR_M  (BIT(5))
#define LEDC_LSTIMER1_OVF_INT_CLR_V  0x1
#define LEDC_LSTIMER1_OVF_INT_CLR_S  5
/* LEDC_LSTIMER0_OVF_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this  bit to clear  low speed channel0  counter overflow interrupt.*/
#define LEDC_LSTIMER0_OVF_INT_CLR  (BIT(4))
#define LEDC_LSTIMER0_OVF_INT_CLR_M  (BIT(4))
#define LEDC_LSTIMER0_OVF_INT_CLR_V  0x1
#define LEDC_LSTIMER0_OVF_INT_CLR_S  4
/* LEDC_HSTIMER3_OVF_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this  bit to clear  high speed channel3  counter overflow interrupt.*/
#define LEDC_HSTIMER3_OVF_INT_CLR  (BIT(3))
#define LEDC_HSTIMER3_OVF_INT_CLR_M  (BIT(3))
#define LEDC_HSTIMER3_OVF_INT_CLR_V  0x1
#define LEDC_HSTIMER3_OVF_INT_CLR_S  3
/* LEDC_HSTIMER2_OVF_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this  bit to clear  high speed channel2  counter overflow interrupt.*/
#define LEDC_HSTIMER2_OVF_INT_CLR  (BIT(2))
#define LEDC_HSTIMER2_OVF_INT_CLR_M  (BIT(2))
#define LEDC_HSTIMER2_OVF_INT_CLR_V  0x1
#define LEDC_HSTIMER2_OVF_INT_CLR_S  2
/* LEDC_HSTIMER1_OVF_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this  bit to clear  high speed channel1  counter overflow interrupt.*/
#define LEDC_HSTIMER1_OVF_INT_CLR  (BIT(1))
#define LEDC_HSTIMER1_OVF_INT_CLR_M  (BIT(1))
#define LEDC_HSTIMER1_OVF_INT_CLR_V  0x1
#define LEDC_HSTIMER1_OVF_INT_CLR_S  1
/* LEDC_HSTIMER0_OVF_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this  bit to clear  high speed channel0  counter overflow interrupt.*/
#define LEDC_HSTIMER0_OVF_INT_CLR  (BIT(0))
#define LEDC_HSTIMER0_OVF_INT_CLR_M  (BIT(0))
#define LEDC_HSTIMER0_OVF_INT_CLR_V  0x1
#define LEDC_HSTIMER0_OVF_INT_CLR_S  0

#define LEDC_CONF_REG          (DR_REG_LEDC_BASE + 0x0190)
/* LEDC_APB_CLK_SEL : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: This bit is used to set the frequency of slow_clk. 1'b1:80mhz  1'b0:8mhz*/
#define LEDC_APB_CLK_SEL  (BIT(0))
#define LEDC_APB_CLK_SEL_M  (BIT(0))
#define LEDC_APB_CLK_SEL_V  0x1
#define LEDC_APB_CLK_SEL_S  0

#define LEDC_DATE_REG          (DR_REG_LEDC_BASE + 0x01FC)
/* LEDC_DATE : R/W ;bitpos:[31:0] ;default: 32'h16031700 ; */
/*description: This register represents the version .*/
#define LEDC_DATE  0xFFFFFFFF
#define LEDC_DATE_M  ((LEDC_DATE_V)<<(LEDC_DATE_S))
#define LEDC_DATE_V  0xFFFFFFFF
#define LEDC_DATE_S  0




#endif /*_SOC_LEDC_REG_H_ */
