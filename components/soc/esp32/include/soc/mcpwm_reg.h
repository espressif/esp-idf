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
#ifndef _SOC_MCPWM_REG_H_
#define _SOC_MCPWM_REG_H_
#include "soc.h"

#define REG_MCPWM_BASE(i)             (DR_REG_PWM0_BASE + i * (0xE000))

#define MCPWM_CLK_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x0000)
/* MCPWM_CLK_PRESCALE : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: Period of PWM_clk = 6.25ns * (PWM_CLK_PRESCALE + 1)*/
#define MCPWM_CLK_PRESCALE  0x000000FF
#define MCPWM_CLK_PRESCALE_M  ((MCPWM_CLK_PRESCALE_V)<<(MCPWM_CLK_PRESCALE_S))
#define MCPWM_CLK_PRESCALE_V  0xFF
#define MCPWM_CLK_PRESCALE_S  0

#define MCPWM_TIMER0_CFG0_REG(i)          (REG_MCPWM_BASE(i) + 0x0004)
/* MCPWM_TIMER0_PERIOD_UPMETHOD : R/W ;bitpos:[25:24] ;default: 2'd0 ; */
/*description: Update method for active reg of PWM timer0 period  0: immediate
  1: TEZ  2: sync  3: TEZ or sync. TEZ here and below means timer equal zero event*/
#define MCPWM_TIMER0_PERIOD_UPMETHOD  0x00000003
#define MCPWM_TIMER0_PERIOD_UPMETHOD_M  ((MCPWM_TIMER0_PERIOD_UPMETHOD_V)<<(MCPWM_TIMER0_PERIOD_UPMETHOD_S))
#define MCPWM_TIMER0_PERIOD_UPMETHOD_V  0x3
#define MCPWM_TIMER0_PERIOD_UPMETHOD_S  24
/* MCPWM_TIMER0_PERIOD : R/W ;bitpos:[23:8] ;default: 16'h00ff ; */
/*description: Period shadow reg of PWM timer0*/
#define MCPWM_TIMER0_PERIOD  0x0000FFFF
#define MCPWM_TIMER0_PERIOD_M  ((MCPWM_TIMER0_PERIOD_V)<<(MCPWM_TIMER0_PERIOD_S))
#define MCPWM_TIMER0_PERIOD_V  0xFFFF
#define MCPWM_TIMER0_PERIOD_S  8
/* MCPWM_TIMER0_PRESCALE : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: Period of PT0_clk = Period of PWM_clk * (PWM_TIMER0_PRESCALE + 1)*/
#define MCPWM_TIMER0_PRESCALE  0x000000FF
#define MCPWM_TIMER0_PRESCALE_M  ((MCPWM_TIMER0_PRESCALE_V)<<(MCPWM_TIMER0_PRESCALE_S))
#define MCPWM_TIMER0_PRESCALE_V  0xFF
#define MCPWM_TIMER0_PRESCALE_S  0

#define MCPWM_TIMER0_CFG1_REG(i)          (REG_MCPWM_BASE(i) + 0x0008)
/* MCPWM_TIMER0_MOD : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: PWM timer0 working mode  0: freeze  1: increase mod  2: decrease
 mod  3: up-down mod*/
#define MCPWM_TIMER0_MOD  0x00000003
#define MCPWM_TIMER0_MOD_M  ((MCPWM_TIMER0_MOD_V)<<(MCPWM_TIMER0_MOD_S))
#define MCPWM_TIMER0_MOD_V  0x3
#define MCPWM_TIMER0_MOD_S  3
/* MCPWM_TIMER0_START : R/W ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: PWM timer0 start and stop control. 0: stop @ TEZ  1: stop @ TEP
  2: free run  3: start and stop @ next TEZ  4: start and stop @ next TEP. TEP here and below means timer equal period event*/
#define MCPWM_TIMER0_START  0x00000007
#define MCPWM_TIMER0_START_M  ((MCPWM_TIMER0_START_V)<<(MCPWM_TIMER0_START_S))
#define MCPWM_TIMER0_START_V  0x7
#define MCPWM_TIMER0_START_S  0

#define MCPWM_TIMER0_SYNC_REG(i)          (REG_MCPWM_BASE(i) + 0x000c)
/* MCPWM_TIMER0_PHASE : R/W ;bitpos:[20:4] ;default: 17'd0 ; */
/*description: Phase for timer reload on sync event*/
#define MCPWM_TIMER0_PHASE  0x0001FFFF
#define MCPWM_TIMER0_PHASE_M  ((MCPWM_TIMER0_PHASE_V)<<(MCPWM_TIMER0_PHASE_S))
#define MCPWM_TIMER0_PHASE_V  0x1FFFF
#define MCPWM_TIMER0_PHASE_S  4
/* MCPWM_TIMER0_SYNCO_SEL : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: PWM timer0 synco selection  0: synci  1: TEZ  2: TEP  else 0*/
#define MCPWM_TIMER0_SYNCO_SEL  0x00000003
#define MCPWM_TIMER0_SYNCO_SEL_M  ((MCPWM_TIMER0_SYNCO_SEL_V)<<(MCPWM_TIMER0_SYNCO_SEL_S))
#define MCPWM_TIMER0_SYNCO_SEL_V  0x3
#define MCPWM_TIMER0_SYNCO_SEL_S  2
/* MCPWM_TIMER0_SYNC_SW : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: Toggling this bit will trigger a software sync*/
#define MCPWM_TIMER0_SYNC_SW  (BIT(1))
#define MCPWM_TIMER0_SYNC_SW_M  (BIT(1))
#define MCPWM_TIMER0_SYNC_SW_V  0x1
#define MCPWM_TIMER0_SYNC_SW_S  1
/* MCPWM_TIMER0_SYNCI_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: When set   timer reload with phase on sync input event is enabled*/
#define MCPWM_TIMER0_SYNCI_EN  (BIT(0))
#define MCPWM_TIMER0_SYNCI_EN_M  (BIT(0))
#define MCPWM_TIMER0_SYNCI_EN_V  0x1
#define MCPWM_TIMER0_SYNCI_EN_S  0

#define MCPWM_TIMER0_STATUS_REG(i)          (REG_MCPWM_BASE(i) + 0x0010)
/* MCPWM_TIMER0_DIRECTION : RO ;bitpos:[16] ;default: 1'd0 ; */
/*description: Current PWM timer0 counter direction  0: increment 1: decrement*/
#define MCPWM_TIMER0_DIRECTION  (BIT(16))
#define MCPWM_TIMER0_DIRECTION_M  (BIT(16))
#define MCPWM_TIMER0_DIRECTION_V  0x1
#define MCPWM_TIMER0_DIRECTION_S  16
/* MCPWM_TIMER0_VALUE : RO ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: Current PWM timer0 counter value*/
#define MCPWM_TIMER0_VALUE  0x0000FFFF
#define MCPWM_TIMER0_VALUE_M  ((MCPWM_TIMER0_VALUE_V)<<(MCPWM_TIMER0_VALUE_S))
#define MCPWM_TIMER0_VALUE_V  0xFFFF
#define MCPWM_TIMER0_VALUE_S  0

#define MCPWM_TIMER1_CFG0_REG(i)          (REG_MCPWM_BASE(i) + 0x0014)
/* MCPWM_TIMER1_PERIOD_UPMETHOD : R/W ;bitpos:[25:24] ;default: 2'd0 ; */
/*description: Update method for active reg of PWM timer1 period  0: immediate
  1: TEZ  2: sync  3: TEZ or sync*/
#define MCPWM_TIMER1_PERIOD_UPMETHOD  0x00000003
#define MCPWM_TIMER1_PERIOD_UPMETHOD_M  ((MCPWM_TIMER1_PERIOD_UPMETHOD_V)<<(MCPWM_TIMER1_PERIOD_UPMETHOD_S))
#define MCPWM_TIMER1_PERIOD_UPMETHOD_V  0x3
#define MCPWM_TIMER1_PERIOD_UPMETHOD_S  24
/* MCPWM_TIMER1_PERIOD : R/W ;bitpos:[23:8] ;default: 16'h00ff ; */
/*description: Period shadow reg of PWM timer1*/
#define MCPWM_TIMER1_PERIOD  0x0000FFFF
#define MCPWM_TIMER1_PERIOD_M  ((MCPWM_TIMER1_PERIOD_V)<<(MCPWM_TIMER1_PERIOD_S))
#define MCPWM_TIMER1_PERIOD_V  0xFFFF
#define MCPWM_TIMER1_PERIOD_S  8
/* MCPWM_TIMER1_PRESCALE : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: Period of PT1_clk = Period of PWM_clk * (PWM_TIMER1_PRESCALE + 1)*/
#define MCPWM_TIMER1_PRESCALE  0x000000FF
#define MCPWM_TIMER1_PRESCALE_M  ((MCPWM_TIMER1_PRESCALE_V)<<(MCPWM_TIMER1_PRESCALE_S))
#define MCPWM_TIMER1_PRESCALE_V  0xFF
#define MCPWM_TIMER1_PRESCALE_S  0

#define MCPWM_TIMER1_CFG1_REG(i)          (REG_MCPWM_BASE(i) + 0x0018)
/* MCPWM_TIMER1_MOD : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: PWM timer1 working mode  0: freeze  1: increase mod  2: decrease
 mod  3: up-down mod*/
#define MCPWM_TIMER1_MOD  0x00000003
#define MCPWM_TIMER1_MOD_M  ((MCPWM_TIMER1_MOD_V)<<(MCPWM_TIMER1_MOD_S))
#define MCPWM_TIMER1_MOD_V  0x3
#define MCPWM_TIMER1_MOD_S  3
/* MCPWM_TIMER1_START : R/W ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: PWM timer1 start and stop control. 0: stop @ TEZ  1: stop @ TEP
  2: free run  3: start and stop @ next TEZ  4: start and stop @ next TEP.*/
#define MCPWM_TIMER1_START  0x00000007
#define MCPWM_TIMER1_START_M  ((MCPWM_TIMER1_START_V)<<(MCPWM_TIMER1_START_S))
#define MCPWM_TIMER1_START_V  0x7
#define MCPWM_TIMER1_START_S  0

#define MCPWM_TIMER1_SYNC_REG(i)          (REG_MCPWM_BASE(i) + 0x001c)
/* MCPWM_TIMER1_PHASE : R/W ;bitpos:[20:4] ;default: 17'd0 ; */
/*description: Phase for timer reload on sync event*/
#define MCPWM_TIMER1_PHASE  0x0001FFFF
#define MCPWM_TIMER1_PHASE_M  ((MCPWM_TIMER1_PHASE_V)<<(MCPWM_TIMER1_PHASE_S))
#define MCPWM_TIMER1_PHASE_V  0x1FFFF
#define MCPWM_TIMER1_PHASE_S  4
/* MCPWM_TIMER1_SYNCO_SEL : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: PWM timer1 synco selection  0: synci  1: TEZ  2: TEP  else 0*/
#define MCPWM_TIMER1_SYNCO_SEL  0x00000003
#define MCPWM_TIMER1_SYNCO_SEL_M  ((MCPWM_TIMER1_SYNCO_SEL_V)<<(MCPWM_TIMER1_SYNCO_SEL_S))
#define MCPWM_TIMER1_SYNCO_SEL_V  0x3
#define MCPWM_TIMER1_SYNCO_SEL_S  2
/* MCPWM_TIMER1_SYNC_SW : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: Toggling this bit will trigger a software sync*/
#define MCPWM_TIMER1_SYNC_SW  (BIT(1))
#define MCPWM_TIMER1_SYNC_SW_M  (BIT(1))
#define MCPWM_TIMER1_SYNC_SW_V  0x1
#define MCPWM_TIMER1_SYNC_SW_S  1
/* MCPWM_TIMER1_SYNCI_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: When set   timer reload with phase on sync input event is enabled*/
#define MCPWM_TIMER1_SYNCI_EN  (BIT(0))
#define MCPWM_TIMER1_SYNCI_EN_M  (BIT(0))
#define MCPWM_TIMER1_SYNCI_EN_V  0x1
#define MCPWM_TIMER1_SYNCI_EN_S  0

#define MCPWM_TIMER1_STATUS_REG(i)          (REG_MCPWM_BASE(i) + 0x0020)
/* MCPWM_TIMER1_DIRECTION : RO ;bitpos:[16] ;default: 1'd0 ; */
/*description: Current PWM timer1 counter direction  0: increment 1: decrement*/
#define MCPWM_TIMER1_DIRECTION  (BIT(16))
#define MCPWM_TIMER1_DIRECTION_M  (BIT(16))
#define MCPWM_TIMER1_DIRECTION_V  0x1
#define MCPWM_TIMER1_DIRECTION_S  16
/* MCPWM_TIMER1_VALUE : RO ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: Current PWM timer1 counter value*/
#define MCPWM_TIMER1_VALUE  0x0000FFFF
#define MCPWM_TIMER1_VALUE_M  ((MCPWM_TIMER1_VALUE_V)<<(MCPWM_TIMER1_VALUE_S))
#define MCPWM_TIMER1_VALUE_V  0xFFFF
#define MCPWM_TIMER1_VALUE_S  0

#define MCPWM_TIMER2_CFG0_REG(i)          (REG_MCPWM_BASE(i) + 0x0024)
/* MCPWM_TIMER2_PERIOD_UPMETHOD : R/W ;bitpos:[25:24] ;default: 2'd0 ; */
/*description: Update method for active reg of PWM timer2 period  0: immediate
  1: TEZ  2: sync  3: TEZ or sync*/
#define MCPWM_TIMER2_PERIOD_UPMETHOD  0x00000003
#define MCPWM_TIMER2_PERIOD_UPMETHOD_M  ((MCPWM_TIMER2_PERIOD_UPMETHOD_V)<<(MCPWM_TIMER2_PERIOD_UPMETHOD_S))
#define MCPWM_TIMER2_PERIOD_UPMETHOD_V  0x3
#define MCPWM_TIMER2_PERIOD_UPMETHOD_S  24
/* MCPWM_TIMER2_PERIOD : R/W ;bitpos:[23:8] ;default: 16'h00ff ; */
/*description: Period shadow reg of PWM timer2*/
#define MCPWM_TIMER2_PERIOD  0x0000FFFF
#define MCPWM_TIMER2_PERIOD_M  ((MCPWM_TIMER2_PERIOD_V)<<(MCPWM_TIMER2_PERIOD_S))
#define MCPWM_TIMER2_PERIOD_V  0xFFFF
#define MCPWM_TIMER2_PERIOD_S  8
/* MCPWM_TIMER2_PRESCALE : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: Period of PT2_clk = Period of PWM_clk * (PWM_TIMER2_PRESCALE + 1)*/
#define MCPWM_TIMER2_PRESCALE  0x000000FF
#define MCPWM_TIMER2_PRESCALE_M  ((MCPWM_TIMER2_PRESCALE_V)<<(MCPWM_TIMER2_PRESCALE_S))
#define MCPWM_TIMER2_PRESCALE_V  0xFF
#define MCPWM_TIMER2_PRESCALE_S  0

#define MCPWM_TIMER2_CFG1_REG(i)          (REG_MCPWM_BASE(i) + 0x0028)
/* MCPWM_TIMER2_MOD : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: PWM timer2 working mode  0: freeze  1: increase mod  2: decrease
 mod  3: up-down mod*/
#define MCPWM_TIMER2_MOD  0x00000003
#define MCPWM_TIMER2_MOD_M  ((MCPWM_TIMER2_MOD_V)<<(MCPWM_TIMER2_MOD_S))
#define MCPWM_TIMER2_MOD_V  0x3
#define MCPWM_TIMER2_MOD_S  3
/* MCPWM_TIMER2_START : R/W ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: PWM timer2 start and stop control. 0: stop @ TEZ  1: stop @ TEP
  2: free run  3: start and stop @ next TEZ  4: start and stop @ next TEP.*/
#define MCPWM_TIMER2_START  0x00000007
#define MCPWM_TIMER2_START_M  ((MCPWM_TIMER2_START_V)<<(MCPWM_TIMER2_START_S))
#define MCPWM_TIMER2_START_V  0x7
#define MCPWM_TIMER2_START_S  0

#define MCPWM_TIMER2_SYNC_REG(i)          (REG_MCPWM_BASE(i) + 0x002c)
/* MCPWM_TIMER2_PHASE : R/W ;bitpos:[20:4] ;default: 17'd0 ; */
/*description: Phase for timer reload on sync event*/
#define MCPWM_TIMER2_PHASE  0x0001FFFF
#define MCPWM_TIMER2_PHASE_M  ((MCPWM_TIMER2_PHASE_V)<<(MCPWM_TIMER2_PHASE_S))
#define MCPWM_TIMER2_PHASE_V  0x1FFFF
#define MCPWM_TIMER2_PHASE_S  4
/* MCPWM_TIMER2_SYNCO_SEL : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: PWM timer2 synco selection  0: synci  1: TEZ  2: TEP  else 0*/
#define MCPWM_TIMER2_SYNCO_SEL  0x00000003
#define MCPWM_TIMER2_SYNCO_SEL_M  ((MCPWM_TIMER2_SYNCO_SEL_V)<<(MCPWM_TIMER2_SYNCO_SEL_S))
#define MCPWM_TIMER2_SYNCO_SEL_V  0x3
#define MCPWM_TIMER2_SYNCO_SEL_S  2
/* MCPWM_TIMER2_SYNC_SW : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: Toggling this bit will trigger a software sync*/
#define MCPWM_TIMER2_SYNC_SW  (BIT(1))
#define MCPWM_TIMER2_SYNC_SW_M  (BIT(1))
#define MCPWM_TIMER2_SYNC_SW_V  0x1
#define MCPWM_TIMER2_SYNC_SW_S  1
/* MCPWM_TIMER2_SYNCI_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: When set   timer reload with phase on sync input event is enabled*/
#define MCPWM_TIMER2_SYNCI_EN  (BIT(0))
#define MCPWM_TIMER2_SYNCI_EN_M  (BIT(0))
#define MCPWM_TIMER2_SYNCI_EN_V  0x1
#define MCPWM_TIMER2_SYNCI_EN_S  0

#define MCPWM_TIMER2_STATUS_REG(i)          (REG_MCPWM_BASE(i) + 0x0030)
/* MCPWM_TIMER2_DIRECTION : RO ;bitpos:[16] ;default: 1'd0 ; */
/*description: Current PWM timer2 counter direction  0: increment 1: decrement*/
#define MCPWM_TIMER2_DIRECTION  (BIT(16))
#define MCPWM_TIMER2_DIRECTION_M  (BIT(16))
#define MCPWM_TIMER2_DIRECTION_V  0x1
#define MCPWM_TIMER2_DIRECTION_S  16
/* MCPWM_TIMER2_VALUE : RO ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: Current PWM timer2 counter value*/
#define MCPWM_TIMER2_VALUE  0x0000FFFF
#define MCPWM_TIMER2_VALUE_M  ((MCPWM_TIMER2_VALUE_V)<<(MCPWM_TIMER2_VALUE_S))
#define MCPWM_TIMER2_VALUE_V  0xFFFF
#define MCPWM_TIMER2_VALUE_S  0

#define MCPWM_TIMER_SYNCI_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x0034)
/* MCPWM_EXTERNAL_SYNCI2_INVERT : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: Onvert SYNC2 from GPIO matrix*/
#define MCPWM_EXTERNAL_SYNCI2_INVERT  (BIT(11))
#define MCPWM_EXTERNAL_SYNCI2_INVERT_M  (BIT(11))
#define MCPWM_EXTERNAL_SYNCI2_INVERT_V  0x1
#define MCPWM_EXTERNAL_SYNCI2_INVERT_S  11
/* MCPWM_EXTERNAL_SYNCI1_INVERT : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: Invert SYNC1 from GPIO matrix*/
#define MCPWM_EXTERNAL_SYNCI1_INVERT  (BIT(10))
#define MCPWM_EXTERNAL_SYNCI1_INVERT_M  (BIT(10))
#define MCPWM_EXTERNAL_SYNCI1_INVERT_V  0x1
#define MCPWM_EXTERNAL_SYNCI1_INVERT_S  10
/* MCPWM_EXTERNAL_SYNCI0_INVERT : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: Invert SYNC0 from GPIO matrix*/
#define MCPWM_EXTERNAL_SYNCI0_INVERT  (BIT(9))
#define MCPWM_EXTERNAL_SYNCI0_INVERT_M  (BIT(9))
#define MCPWM_EXTERNAL_SYNCI0_INVERT_V  0x1
#define MCPWM_EXTERNAL_SYNCI0_INVERT_S  9
/* MCPWM_TIMER2_SYNCISEL : R/W ;bitpos:[8:6] ;default: 3'd0 ; */
/*description: Select sync input for PWM timer2  1: PWM timer0 synco  2: PWM
 timer1 synco  3: PWM timer2 synco  4: SYNC0 from GPIO matrix  5: SYNC1 from GPIO matrix  6: SYNC2 from GPIO matrix  other values: no sync input selected*/
#define MCPWM_TIMER2_SYNCISEL  0x00000007
#define MCPWM_TIMER2_SYNCISEL_M  ((MCPWM_TIMER2_SYNCISEL_V)<<(MCPWM_TIMER2_SYNCISEL_S))
#define MCPWM_TIMER2_SYNCISEL_V  0x7
#define MCPWM_TIMER2_SYNCISEL_S  6
/* MCPWM_TIMER1_SYNCISEL : R/W ;bitpos:[5:3] ;default: 3'd0 ; */
/*description: Select sync input for PWM timer1  1: PWM timer0 synco  2: PWM
 timer1 synco  3: PWM timer2 synco  4: SYNC0 from GPIO matrix  5: SYNC1 from GPIO matrix  6: SYNC2 from GPIO matrix  other values: no sync input selected*/
#define MCPWM_TIMER1_SYNCISEL  0x00000007
#define MCPWM_TIMER1_SYNCISEL_M  ((MCPWM_TIMER1_SYNCISEL_V)<<(MCPWM_TIMER1_SYNCISEL_S))
#define MCPWM_TIMER1_SYNCISEL_V  0x7
#define MCPWM_TIMER1_SYNCISEL_S  3
/* MCPWM_TIMER0_SYNCISEL : R/W ;bitpos:[2:0] ;default: 3'd0 ; */
/*description: Select sync input for PWM timer0  1: PWM timer0 synco  2: PWM
 timer1 synco  3: PWM timer2 synco  4: SYNC0 from GPIO matrix  5: SYNC1 from GPIO matrix  6: SYNC2 from GPIO matrix  other values: no sync input selected*/
#define MCPWM_TIMER0_SYNCISEL  0x00000007
#define MCPWM_TIMER0_SYNCISEL_M  ((MCPWM_TIMER0_SYNCISEL_V)<<(MCPWM_TIMER0_SYNCISEL_S))
#define MCPWM_TIMER0_SYNCISEL_V  0x7
#define MCPWM_TIMER0_SYNCISEL_S  0

#define MCPWM_OPERATOR_TIMERSEL_REG(i)          (REG_MCPWM_BASE(i) + 0x0038)
/* MCPWM_OPERATOR2_TIMERSEL : R/W ;bitpos:[5:4] ;default: 2'd0 ; */
/*description: Select which PWM timer's is the timing reference for PWM operator2
  0: timer0  1: timer1  2: timer2*/
#define MCPWM_OPERATOR2_TIMERSEL  0x00000003
#define MCPWM_OPERATOR2_TIMERSEL_M  ((MCPWM_OPERATOR2_TIMERSEL_V)<<(MCPWM_OPERATOR2_TIMERSEL_S))
#define MCPWM_OPERATOR2_TIMERSEL_V  0x3
#define MCPWM_OPERATOR2_TIMERSEL_S  4
/* MCPWM_OPERATOR1_TIMERSEL : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: Select which PWM timer's is the timing reference for PWM operator1
  0: timer0  1: timer1  2: timer2*/
#define MCPWM_OPERATOR1_TIMERSEL  0x00000003
#define MCPWM_OPERATOR1_TIMERSEL_M  ((MCPWM_OPERATOR1_TIMERSEL_V)<<(MCPWM_OPERATOR1_TIMERSEL_S))
#define MCPWM_OPERATOR1_TIMERSEL_V  0x3
#define MCPWM_OPERATOR1_TIMERSEL_S  2
/* MCPWM_OPERATOR0_TIMERSEL : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: Select which PWM timer's is the timing reference for PWM operator0
  0: timer0  1: timer1  2: timer2*/
#define MCPWM_OPERATOR0_TIMERSEL  0x00000003
#define MCPWM_OPERATOR0_TIMERSEL_M  ((MCPWM_OPERATOR0_TIMERSEL_V)<<(MCPWM_OPERATOR0_TIMERSEL_S))
#define MCPWM_OPERATOR0_TIMERSEL_V  0x3
#define MCPWM_OPERATOR0_TIMERSEL_S  0

#define MCPWM_GEN0_STMP_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x003c)
/* MCPWM_GEN0_B_SHDW_FULL : RO ;bitpos:[9] ;default: 1'd0 ; */
/*description: Set and reset by hardware. If set  PWM generator 0 time stamp
 B's shadow reg is filled and waiting to be transferred to B's active reg. If cleared  B's active reg has been updated with shadow reg latest value*/
#define MCPWM_GEN0_B_SHDW_FULL  (BIT(9))
#define MCPWM_GEN0_B_SHDW_FULL_M  (BIT(9))
#define MCPWM_GEN0_B_SHDW_FULL_V  0x1
#define MCPWM_GEN0_B_SHDW_FULL_S  9
/* MCPWM_GEN0_A_SHDW_FULL : RO ;bitpos:[8] ;default: 1'd0 ; */
/*description: Set and reset by hardware. If set  PWM generator 0 time stamp
 A's shadow reg is filled and waiting to be transferred to A's active reg. If cleared  A's active reg has been updated with shadow reg latest value*/
#define MCPWM_GEN0_A_SHDW_FULL  (BIT(8))
#define MCPWM_GEN0_A_SHDW_FULL_M  (BIT(8))
#define MCPWM_GEN0_A_SHDW_FULL_V  0x1
#define MCPWM_GEN0_A_SHDW_FULL_S  8
/* MCPWM_GEN0_B_UPMETHOD : R/W ;bitpos:[7:4] ;default: 4'd0 ; */
/*description: Update method for PWM generator 0 time stamp B's active reg.
 0: immediate  bit0: TEZ  bit1: TEP  bit2: sync  bit3: disable update*/
#define MCPWM_GEN0_B_UPMETHOD  0x0000000F
#define MCPWM_GEN0_B_UPMETHOD_M  ((MCPWM_GEN0_B_UPMETHOD_V)<<(MCPWM_GEN0_B_UPMETHOD_S))
#define MCPWM_GEN0_B_UPMETHOD_V  0xF
#define MCPWM_GEN0_B_UPMETHOD_S  4
/* MCPWM_GEN0_A_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: Update method for PWM generator 0 time stamp A's active reg.
 0: immediate  bit0: TEZ  bit1: TEP  bit2: sync  bit3: disable update*/
#define MCPWM_GEN0_A_UPMETHOD  0x0000000F
#define MCPWM_GEN0_A_UPMETHOD_M  ((MCPWM_GEN0_A_UPMETHOD_V)<<(MCPWM_GEN0_A_UPMETHOD_S))
#define MCPWM_GEN0_A_UPMETHOD_V  0xF
#define MCPWM_GEN0_A_UPMETHOD_S  0

#define MCPWM_GEN0_TSTMP_A_REG(i)          (REG_MCPWM_BASE(i) + 0x0040)
/* MCPWM_GEN0_A : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: PWM generator 0 time stamp A's shadow reg*/
#define MCPWM_GEN0_A  0x0000FFFF
#define MCPWM_GEN0_A_M  ((MCPWM_GEN0_A_V)<<(MCPWM_GEN0_A_S))
#define MCPWM_GEN0_A_V  0xFFFF
#define MCPWM_GEN0_A_S  0

#define MCPWM_GEN0_TSTMP_B_REG(i)          (REG_MCPWM_BASE(i) + 0x0044)
/* MCPWM_GEN0_B : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: PWM generator 0 time stamp B's shadow reg*/
#define MCPWM_GEN0_B  0x0000FFFF
#define MCPWM_GEN0_B_M  ((MCPWM_GEN0_B_V)<<(MCPWM_GEN0_B_S))
#define MCPWM_GEN0_B_V  0xFFFF
#define MCPWM_GEN0_B_S  0

#define MCPWM_GEN0_CFG0_REG(i)          (REG_MCPWM_BASE(i) + 0x0048)
/* MCPWM_GEN0_T1_SEL : R/W ;bitpos:[9:7] ;default: 3'd0 ; */
/*description: Source selection for PWM generator 0 event_t1  take effect immediately
  0: fault_event0  1: fault_event1  2: fault_event2  3: sync_taken  4: none*/
#define MCPWM_GEN0_T1_SEL  0x00000007
#define MCPWM_GEN0_T1_SEL_M  ((MCPWM_GEN0_T1_SEL_V)<<(MCPWM_GEN0_T1_SEL_S))
#define MCPWM_GEN0_T1_SEL_V  0x7
#define MCPWM_GEN0_T1_SEL_S  7
/* MCPWM_GEN0_T0_SEL : R/W ;bitpos:[6:4] ;default: 3'd0 ; */
/*description: Source selection for PWM generator 0 event_t0  take effect immediately
  0: fault_event0  1: fault_event1  2: fault_event2  3: sync_taken  4: none*/
#define MCPWM_GEN0_T0_SEL  0x00000007
#define MCPWM_GEN0_T0_SEL_M  ((MCPWM_GEN0_T0_SEL_V)<<(MCPWM_GEN0_T0_SEL_S))
#define MCPWM_GEN0_T0_SEL_V  0x7
#define MCPWM_GEN0_T0_SEL_S  4
/* MCPWM_GEN0_CFG_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: Update method for PWM generator 0's active reg of configuration.
 0: immediate  bit0: TEZ  bit1: TEP  bit2: sync. bit3: disable update*/
#define MCPWM_GEN0_CFG_UPMETHOD  0x0000000F
#define MCPWM_GEN0_CFG_UPMETHOD_M  ((MCPWM_GEN0_CFG_UPMETHOD_V)<<(MCPWM_GEN0_CFG_UPMETHOD_S))
#define MCPWM_GEN0_CFG_UPMETHOD_V  0xF
#define MCPWM_GEN0_CFG_UPMETHOD_S  0

#define MCPWM_GEN0_FORCE_REG(i)          (REG_MCPWM_BASE(i) + 0x004c)
/* MCPWM_GEN0_B_NCIFORCE_MODE : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: Non-continuous immediate software force mode for PWM0B  0: disabled
  1: low  2: high  3: disabled*/
#define MCPWM_GEN0_B_NCIFORCE_MODE  0x00000003
#define MCPWM_GEN0_B_NCIFORCE_MODE_M  ((MCPWM_GEN0_B_NCIFORCE_MODE_V)<<(MCPWM_GEN0_B_NCIFORCE_MODE_S))
#define MCPWM_GEN0_B_NCIFORCE_MODE_V  0x3
#define MCPWM_GEN0_B_NCIFORCE_MODE_S  14
/* MCPWM_GEN0_B_NCIFORCE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: Non-continuous immediate software force trigger for PWM0B  a
 toggle will trigger a force event*/
#define MCPWM_GEN0_B_NCIFORCE  (BIT(13))
#define MCPWM_GEN0_B_NCIFORCE_M  (BIT(13))
#define MCPWM_GEN0_B_NCIFORCE_V  0x1
#define MCPWM_GEN0_B_NCIFORCE_S  13
/* MCPWM_GEN0_A_NCIFORCE_MODE : R/W ;bitpos:[12:11] ;default: 2'd0 ; */
/*description: Non-continuous immediate software force mode for PWM0A  0: disabled
  1: low  2: high  3: disabled*/
#define MCPWM_GEN0_A_NCIFORCE_MODE  0x00000003
#define MCPWM_GEN0_A_NCIFORCE_MODE_M  ((MCPWM_GEN0_A_NCIFORCE_MODE_V)<<(MCPWM_GEN0_A_NCIFORCE_MODE_S))
#define MCPWM_GEN0_A_NCIFORCE_MODE_V  0x3
#define MCPWM_GEN0_A_NCIFORCE_MODE_S  11
/* MCPWM_GEN0_A_NCIFORCE : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: Non-continuous immediate software force trigger for PWM0A  a
 toggle will trigger a force event*/
#define MCPWM_GEN0_A_NCIFORCE  (BIT(10))
#define MCPWM_GEN0_A_NCIFORCE_M  (BIT(10))
#define MCPWM_GEN0_A_NCIFORCE_V  0x1
#define MCPWM_GEN0_A_NCIFORCE_S  10
/* MCPWM_GEN0_B_CNTUFORCE_MODE : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: Continuous software force mode for PWM0B. 0: disabled  1: low
  2: high  3: disabled*/
#define MCPWM_GEN0_B_CNTUFORCE_MODE  0x00000003
#define MCPWM_GEN0_B_CNTUFORCE_MODE_M  ((MCPWM_GEN0_B_CNTUFORCE_MODE_V)<<(MCPWM_GEN0_B_CNTUFORCE_MODE_S))
#define MCPWM_GEN0_B_CNTUFORCE_MODE_V  0x3
#define MCPWM_GEN0_B_CNTUFORCE_MODE_S  8
/* MCPWM_GEN0_A_CNTUFORCE_MODE : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: Continuous software force mode for PWM0A. 0: disabled  1: low
  2: high  3: disabled*/
#define MCPWM_GEN0_A_CNTUFORCE_MODE  0x00000003
#define MCPWM_GEN0_A_CNTUFORCE_MODE_M  ((MCPWM_GEN0_A_CNTUFORCE_MODE_V)<<(MCPWM_GEN0_A_CNTUFORCE_MODE_S))
#define MCPWM_GEN0_A_CNTUFORCE_MODE_V  0x3
#define MCPWM_GEN0_A_CNTUFORCE_MODE_S  6
/* MCPWM_GEN0_CNTUFORCE_UPMETHOD : R/W ;bitpos:[5:0] ;default: 6'h20 ; */
/*description: Update method for continuous software force of PWM generator0.
 0: immediate  bit0: TEZ  bit1: TEP  bit2: TEA  bit3: TEB  bit4: sync  bit5: disable update. (TEA/B here and below means an event generated when timer value equals A/B register)*/
#define MCPWM_GEN0_CNTUFORCE_UPMETHOD  0x0000003F
#define MCPWM_GEN0_CNTUFORCE_UPMETHOD_M  ((MCPWM_GEN0_CNTUFORCE_UPMETHOD_V)<<(MCPWM_GEN0_CNTUFORCE_UPMETHOD_S))
#define MCPWM_GEN0_CNTUFORCE_UPMETHOD_V  0x3F
#define MCPWM_GEN0_CNTUFORCE_UPMETHOD_S  0

#define MCPWM_GEN0_A_REG(i)          (REG_MCPWM_BASE(i) + 0x0050)
/* MCPWM_GEN0_A_DT1 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: Action on PWM0A triggered by event_t1 when timer decreasing.
 0: no change  1: low  2: high  3: toggle*/
#define MCPWM_GEN0_A_DT1  0x00000003
#define MCPWM_GEN0_A_DT1_M  ((MCPWM_GEN0_A_DT1_V)<<(MCPWM_GEN0_A_DT1_S))
#define MCPWM_GEN0_A_DT1_V  0x3
#define MCPWM_GEN0_A_DT1_S  22
/* MCPWM_GEN0_A_DT0 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: Action on PWM0A triggered by event_t0 when timer decreasing*/
#define MCPWM_GEN0_A_DT0  0x00000003
#define MCPWM_GEN0_A_DT0_M  ((MCPWM_GEN0_A_DT0_V)<<(MCPWM_GEN0_A_DT0_S))
#define MCPWM_GEN0_A_DT0_V  0x3
#define MCPWM_GEN0_A_DT0_S  20
/* MCPWM_GEN0_A_DTEB : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: Action on PWM0A triggered by event TEB when timer decreasing*/
#define MCPWM_GEN0_A_DTEB  0x00000003
#define MCPWM_GEN0_A_DTEB_M  ((MCPWM_GEN0_A_DTEB_V)<<(MCPWM_GEN0_A_DTEB_S))
#define MCPWM_GEN0_A_DTEB_V  0x3
#define MCPWM_GEN0_A_DTEB_S  18
/* MCPWM_GEN0_A_DTEA : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: Action on PWM0A triggered by event TEA when timer decreasing*/
#define MCPWM_GEN0_A_DTEA  0x00000003
#define MCPWM_GEN0_A_DTEA_M  ((MCPWM_GEN0_A_DTEA_V)<<(MCPWM_GEN0_A_DTEA_S))
#define MCPWM_GEN0_A_DTEA_V  0x3
#define MCPWM_GEN0_A_DTEA_S  16
/* MCPWM_GEN0_A_DTEP : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: Action on PWM0A triggered by event TEP when timer decreasing*/
#define MCPWM_GEN0_A_DTEP  0x00000003
#define MCPWM_GEN0_A_DTEP_M  ((MCPWM_GEN0_A_DTEP_V)<<(MCPWM_GEN0_A_DTEP_S))
#define MCPWM_GEN0_A_DTEP_V  0x3
#define MCPWM_GEN0_A_DTEP_S  14
/* MCPWM_GEN0_A_DTEZ : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: Action on PWM0A triggered by event TEZ when timer decreasing*/
#define MCPWM_GEN0_A_DTEZ  0x00000003
#define MCPWM_GEN0_A_DTEZ_M  ((MCPWM_GEN0_A_DTEZ_V)<<(MCPWM_GEN0_A_DTEZ_S))
#define MCPWM_GEN0_A_DTEZ_V  0x3
#define MCPWM_GEN0_A_DTEZ_S  12
/* MCPWM_GEN0_A_UT1 : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: Action on PWM0A triggered by event_t1 when timer increasing*/
#define MCPWM_GEN0_A_UT1  0x00000003
#define MCPWM_GEN0_A_UT1_M  ((MCPWM_GEN0_A_UT1_V)<<(MCPWM_GEN0_A_UT1_S))
#define MCPWM_GEN0_A_UT1_V  0x3
#define MCPWM_GEN0_A_UT1_S  10
/* MCPWM_GEN0_A_UT0 : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: Action on PWM0A triggered by event_t0 when timer increasing*/
#define MCPWM_GEN0_A_UT0  0x00000003
#define MCPWM_GEN0_A_UT0_M  ((MCPWM_GEN0_A_UT0_V)<<(MCPWM_GEN0_A_UT0_S))
#define MCPWM_GEN0_A_UT0_V  0x3
#define MCPWM_GEN0_A_UT0_S  8
/* MCPWM_GEN0_A_UTEB : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: Action on PWM0A triggered by event TEB when timer increasing*/
#define MCPWM_GEN0_A_UTEB  0x00000003
#define MCPWM_GEN0_A_UTEB_M  ((MCPWM_GEN0_A_UTEB_V)<<(MCPWM_GEN0_A_UTEB_S))
#define MCPWM_GEN0_A_UTEB_V  0x3
#define MCPWM_GEN0_A_UTEB_S  6
/* MCPWM_GEN0_A_UTEA : R/W ;bitpos:[5:4] ;default: 2'd0 ; */
/*description: Action on PWM0A triggered by event TEA when timer increasing*/
#define MCPWM_GEN0_A_UTEA  0x00000003
#define MCPWM_GEN0_A_UTEA_M  ((MCPWM_GEN0_A_UTEA_V)<<(MCPWM_GEN0_A_UTEA_S))
#define MCPWM_GEN0_A_UTEA_V  0x3
#define MCPWM_GEN0_A_UTEA_S  4
/* MCPWM_GEN0_A_UTEP : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: Action on PWM0A triggered by event TEP when timer increasing*/
#define MCPWM_GEN0_A_UTEP  0x00000003
#define MCPWM_GEN0_A_UTEP_M  ((MCPWM_GEN0_A_UTEP_V)<<(MCPWM_GEN0_A_UTEP_S))
#define MCPWM_GEN0_A_UTEP_V  0x3
#define MCPWM_GEN0_A_UTEP_S  2
/* MCPWM_GEN0_A_UTEZ : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: Action on PWM0A triggered by event TEZ when timer increasing*/
#define MCPWM_GEN0_A_UTEZ  0x00000003
#define MCPWM_GEN0_A_UTEZ_M  ((MCPWM_GEN0_A_UTEZ_V)<<(MCPWM_GEN0_A_UTEZ_S))
#define MCPWM_GEN0_A_UTEZ_V  0x3
#define MCPWM_GEN0_A_UTEZ_S  0

#define MCPWM_GEN0_B_REG(i)          (REG_MCPWM_BASE(i) + 0x0054)
/* MCPWM_GEN0_B_DT1 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: Action on PWM0B triggered by event_t1 when timer decreasing.
 0: no change  1: low  2: high  3: toggle*/
#define MCPWM_GEN0_B_DT1  0x00000003
#define MCPWM_GEN0_B_DT1_M  ((MCPWM_GEN0_B_DT1_V)<<(MCPWM_GEN0_B_DT1_S))
#define MCPWM_GEN0_B_DT1_V  0x3
#define MCPWM_GEN0_B_DT1_S  22
/* MCPWM_GEN0_B_DT0 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: Action on PWM0B triggered by event_t0 when timer decreasing*/
#define MCPWM_GEN0_B_DT0  0x00000003
#define MCPWM_GEN0_B_DT0_M  ((MCPWM_GEN0_B_DT0_V)<<(MCPWM_GEN0_B_DT0_S))
#define MCPWM_GEN0_B_DT0_V  0x3
#define MCPWM_GEN0_B_DT0_S  20
/* MCPWM_GEN0_B_DTEB : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: Action on PWM0B triggered by event TEB when timer decreasing*/
#define MCPWM_GEN0_B_DTEB  0x00000003
#define MCPWM_GEN0_B_DTEB_M  ((MCPWM_GEN0_B_DTEB_V)<<(MCPWM_GEN0_B_DTEB_S))
#define MCPWM_GEN0_B_DTEB_V  0x3
#define MCPWM_GEN0_B_DTEB_S  18
/* MCPWM_GEN0_B_DTEA : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: Action on PWM0B triggered by event TEA when timer decreasing*/
#define MCPWM_GEN0_B_DTEA  0x00000003
#define MCPWM_GEN0_B_DTEA_M  ((MCPWM_GEN0_B_DTEA_V)<<(MCPWM_GEN0_B_DTEA_S))
#define MCPWM_GEN0_B_DTEA_V  0x3
#define MCPWM_GEN0_B_DTEA_S  16
/* MCPWM_GEN0_B_DTEP : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: Action on PWM0B triggered by event TEP when timer decreasing*/
#define MCPWM_GEN0_B_DTEP  0x00000003
#define MCPWM_GEN0_B_DTEP_M  ((MCPWM_GEN0_B_DTEP_V)<<(MCPWM_GEN0_B_DTEP_S))
#define MCPWM_GEN0_B_DTEP_V  0x3
#define MCPWM_GEN0_B_DTEP_S  14
/* MCPWM_GEN0_B_DTEZ : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: Action on PWM0B triggered by event TEZ when timer decreasing*/
#define MCPWM_GEN0_B_DTEZ  0x00000003
#define MCPWM_GEN0_B_DTEZ_M  ((MCPWM_GEN0_B_DTEZ_V)<<(MCPWM_GEN0_B_DTEZ_S))
#define MCPWM_GEN0_B_DTEZ_V  0x3
#define MCPWM_GEN0_B_DTEZ_S  12
/* MCPWM_GEN0_B_UT1 : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: Action on PWM0B triggered by event_t1 when timer increasing*/
#define MCPWM_GEN0_B_UT1  0x00000003
#define MCPWM_GEN0_B_UT1_M  ((MCPWM_GEN0_B_UT1_V)<<(MCPWM_GEN0_B_UT1_S))
#define MCPWM_GEN0_B_UT1_V  0x3
#define MCPWM_GEN0_B_UT1_S  10
/* MCPWM_GEN0_B_UT0 : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: Action on PWM0B triggered by event_t0 when timer increasing*/
#define MCPWM_GEN0_B_UT0  0x00000003
#define MCPWM_GEN0_B_UT0_M  ((MCPWM_GEN0_B_UT0_V)<<(MCPWM_GEN0_B_UT0_S))
#define MCPWM_GEN0_B_UT0_V  0x3
#define MCPWM_GEN0_B_UT0_S  8
/* MCPWM_GEN0_B_UTEB : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: Action on PWM0B triggered by event TEB when timer increasing*/
#define MCPWM_GEN0_B_UTEB  0x00000003
#define MCPWM_GEN0_B_UTEB_M  ((MCPWM_GEN0_B_UTEB_V)<<(MCPWM_GEN0_B_UTEB_S))
#define MCPWM_GEN0_B_UTEB_V  0x3
#define MCPWM_GEN0_B_UTEB_S  6
/* MCPWM_GEN0_B_UTEA : R/W ;bitpos:[5:4] ;default: 2'd0 ; */
/*description: Action on PWM0B triggered by event TEA when timer increasing*/
#define MCPWM_GEN0_B_UTEA  0x00000003
#define MCPWM_GEN0_B_UTEA_M  ((MCPWM_GEN0_B_UTEA_V)<<(MCPWM_GEN0_B_UTEA_S))
#define MCPWM_GEN0_B_UTEA_V  0x3
#define MCPWM_GEN0_B_UTEA_S  4
/* MCPWM_GEN0_B_UTEP : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: Action on PWM0B triggered by event TEP when timer increasing*/
#define MCPWM_GEN0_B_UTEP  0x00000003
#define MCPWM_GEN0_B_UTEP_M  ((MCPWM_GEN0_B_UTEP_V)<<(MCPWM_GEN0_B_UTEP_S))
#define MCPWM_GEN0_B_UTEP_V  0x3
#define MCPWM_GEN0_B_UTEP_S  2
/* MCPWM_GEN0_B_UTEZ : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: Action on PWM0B triggered by event TEZ when timer increasing*/
#define MCPWM_GEN0_B_UTEZ  0x00000003
#define MCPWM_GEN0_B_UTEZ_M  ((MCPWM_GEN0_B_UTEZ_V)<<(MCPWM_GEN0_B_UTEZ_S))
#define MCPWM_GEN0_B_UTEZ_V  0x3
#define MCPWM_GEN0_B_UTEZ_S  0

#define MCPWM_DT0_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x0058)
/* MCPWM_DT0_CLK_SEL : R/W ;bitpos:[17] ;default: 1'd0 ; */
/*description: Dead time generator 0 clock selection. 0: PWM_clk  1: PT_clk*/
#define MCPWM_DT0_CLK_SEL  (BIT(17))
#define MCPWM_DT0_CLK_SEL_M  (BIT(17))
#define MCPWM_DT0_CLK_SEL_V  0x1
#define MCPWM_DT0_CLK_SEL_S  17
/* MCPWM_DT0_B_OUTBYPASS : R/W ;bitpos:[16] ;default: 1'd1 ; */
/*description: S0 in documentation*/
#define MCPWM_DT0_B_OUTBYPASS  (BIT(16))
#define MCPWM_DT0_B_OUTBYPASS_M  (BIT(16))
#define MCPWM_DT0_B_OUTBYPASS_V  0x1
#define MCPWM_DT0_B_OUTBYPASS_S  16
/* MCPWM_DT0_A_OUTBYPASS : R/W ;bitpos:[15] ;default: 1'd1 ; */
/*description: S1 in documentation*/
#define MCPWM_DT0_A_OUTBYPASS  (BIT(15))
#define MCPWM_DT0_A_OUTBYPASS_M  (BIT(15))
#define MCPWM_DT0_A_OUTBYPASS_V  0x1
#define MCPWM_DT0_A_OUTBYPASS_S  15
/* MCPWM_DT0_FED_OUTINVERT : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: S3 in documentation*/
#define MCPWM_DT0_FED_OUTINVERT  (BIT(14))
#define MCPWM_DT0_FED_OUTINVERT_M  (BIT(14))
#define MCPWM_DT0_FED_OUTINVERT_V  0x1
#define MCPWM_DT0_FED_OUTINVERT_S  14
/* MCPWM_DT0_RED_OUTINVERT : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: S2 in documentation*/
#define MCPWM_DT0_RED_OUTINVERT  (BIT(13))
#define MCPWM_DT0_RED_OUTINVERT_M  (BIT(13))
#define MCPWM_DT0_RED_OUTINVERT_V  0x1
#define MCPWM_DT0_RED_OUTINVERT_S  13
/* MCPWM_DT0_FED_INSEL : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: S5 in documentation*/
#define MCPWM_DT0_FED_INSEL  (BIT(12))
#define MCPWM_DT0_FED_INSEL_M  (BIT(12))
#define MCPWM_DT0_FED_INSEL_V  0x1
#define MCPWM_DT0_FED_INSEL_S  12
/* MCPWM_DT0_RED_INSEL : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: S4 in documentation*/
#define MCPWM_DT0_RED_INSEL  (BIT(11))
#define MCPWM_DT0_RED_INSEL_M  (BIT(11))
#define MCPWM_DT0_RED_INSEL_V  0x1
#define MCPWM_DT0_RED_INSEL_S  11
/* MCPWM_DT0_B_OUTSWAP : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: S7 in documentation*/
#define MCPWM_DT0_B_OUTSWAP  (BIT(10))
#define MCPWM_DT0_B_OUTSWAP_M  (BIT(10))
#define MCPWM_DT0_B_OUTSWAP_V  0x1
#define MCPWM_DT0_B_OUTSWAP_S  10
/* MCPWM_DT0_A_OUTSWAP : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: S6 in documentation*/
#define MCPWM_DT0_A_OUTSWAP  (BIT(9))
#define MCPWM_DT0_A_OUTSWAP_M  (BIT(9))
#define MCPWM_DT0_A_OUTSWAP_V  0x1
#define MCPWM_DT0_A_OUTSWAP_S  9
/* MCPWM_DT0_DEB_MODE : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: S8 in documentation  dual-edge B mode  0: FED/RED take effect
 on different path separately  1: FED/RED take effect on B path  A out is in bypass or normal operation mode*/
#define MCPWM_DT0_DEB_MODE  (BIT(8))
#define MCPWM_DT0_DEB_MODE_M  (BIT(8))
#define MCPWM_DT0_DEB_MODE_V  0x1
#define MCPWM_DT0_DEB_MODE_S  8
/* MCPWM_DT0_RED_UPMETHOD : R/W ;bitpos:[7:4] ;default: 4'd0 ; */
/*description: Update method for RED (rising edge delay) active reg. 0: immediate
  bit0: TEZ  bit1: TEP  bit2: sync  bit3: disable update*/
#define MCPWM_DT0_RED_UPMETHOD  0x0000000F
#define MCPWM_DT0_RED_UPMETHOD_M  ((MCPWM_DT0_RED_UPMETHOD_V)<<(MCPWM_DT0_RED_UPMETHOD_S))
#define MCPWM_DT0_RED_UPMETHOD_V  0xF
#define MCPWM_DT0_RED_UPMETHOD_S  4
/* MCPWM_DT0_FED_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: Update method for FED (falling edge delay) active reg. 0: immediate
  bit0: TEZ  bit1: TEP  bit2: sync  bit3: disable update*/
#define MCPWM_DT0_FED_UPMETHOD  0x0000000F
#define MCPWM_DT0_FED_UPMETHOD_M  ((MCPWM_DT0_FED_UPMETHOD_V)<<(MCPWM_DT0_FED_UPMETHOD_S))
#define MCPWM_DT0_FED_UPMETHOD_V  0xF
#define MCPWM_DT0_FED_UPMETHOD_S  0

#define MCPWM_DT0_FED_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x005c)
/* MCPWM_DT0_FED : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: Shadow reg for FED*/
#define MCPWM_DT0_FED  0x0000FFFF
#define MCPWM_DT0_FED_M  ((MCPWM_DT0_FED_V)<<(MCPWM_DT0_FED_S))
#define MCPWM_DT0_FED_V  0xFFFF
#define MCPWM_DT0_FED_S  0

#define MCPWM_DT0_RED_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x0060)
/* MCPWM_DT0_RED : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: Shadow reg for RED*/
#define MCPWM_DT0_RED  0x0000FFFF
#define MCPWM_DT0_RED_M  ((MCPWM_DT0_RED_V)<<(MCPWM_DT0_RED_S))
#define MCPWM_DT0_RED_V  0xFFFF
#define MCPWM_DT0_RED_S  0

#define MCPWM_CARRIER0_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x0064)
/* MCPWM_CARRIER0_IN_INVERT : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: When set  invert the input of PWM0A and PWM0B for this submodule*/
#define MCPWM_CARRIER0_IN_INVERT  (BIT(13))
#define MCPWM_CARRIER0_IN_INVERT_M  (BIT(13))
#define MCPWM_CARRIER0_IN_INVERT_V  0x1
#define MCPWM_CARRIER0_IN_INVERT_S  13
/* MCPWM_CARRIER0_OUT_INVERT : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: When set  invert the output of PWM0A and PWM0B for this submodule*/
#define MCPWM_CARRIER0_OUT_INVERT  (BIT(12))
#define MCPWM_CARRIER0_OUT_INVERT_M  (BIT(12))
#define MCPWM_CARRIER0_OUT_INVERT_V  0x1
#define MCPWM_CARRIER0_OUT_INVERT_S  12
/* MCPWM_CARRIER0_OSHWTH : R/W ;bitpos:[11:8] ;default: 4'd0 ; */
/*description: Width of the fist pulse in number of periods of the carrier*/
#define MCPWM_CARRIER0_OSHWTH  0x0000000F
#define MCPWM_CARRIER0_OSHWTH_M  ((MCPWM_CARRIER0_OSHWTH_V)<<(MCPWM_CARRIER0_OSHWTH_S))
#define MCPWM_CARRIER0_OSHWTH_V  0xF
#define MCPWM_CARRIER0_OSHWTH_S  8
/* MCPWM_CARRIER0_DUTY : R/W ;bitpos:[7:5] ;default: 3'd0 ; */
/*description: Carrier duty selection. Duty = PWM_CARRIER0_DUTY / 8*/
#define MCPWM_CARRIER0_DUTY  0x00000007
#define MCPWM_CARRIER0_DUTY_M  ((MCPWM_CARRIER0_DUTY_V)<<(MCPWM_CARRIER0_DUTY_S))
#define MCPWM_CARRIER0_DUTY_V  0x7
#define MCPWM_CARRIER0_DUTY_S  5
/* MCPWM_CARRIER0_PRESCALE : R/W ;bitpos:[4:1] ;default: 4'd0 ; */
/*description: PWM carrier0 clock (PC_clk) prescale value. Period of PC_clk
 = period of PWM_clk * (PWM_CARRIER0_PRESCALE + 1)*/
#define MCPWM_CARRIER0_PRESCALE  0x0000000F
#define MCPWM_CARRIER0_PRESCALE_M  ((MCPWM_CARRIER0_PRESCALE_V)<<(MCPWM_CARRIER0_PRESCALE_S))
#define MCPWM_CARRIER0_PRESCALE_V  0xF
#define MCPWM_CARRIER0_PRESCALE_S  1
/* MCPWM_CARRIER0_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: When set  carrier0 function is enabled. When cleared  carrier0 is bypassed*/
#define MCPWM_CARRIER0_EN  (BIT(0))
#define MCPWM_CARRIER0_EN_M  (BIT(0))
#define MCPWM_CARRIER0_EN_V  0x1
#define MCPWM_CARRIER0_EN_S  0

#define MCPWM_FH0_CFG0_REG(i)          (REG_MCPWM_BASE(i) + 0x0068)
/* MCPWM_FH0_B_OST_U : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: One-shot mode action on PWM0B when fault event occurs and timer
 is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH0_B_OST_U  0x00000003
#define MCPWM_FH0_B_OST_U_M  ((MCPWM_FH0_B_OST_U_V)<<(MCPWM_FH0_B_OST_U_S))
#define MCPWM_FH0_B_OST_U_V  0x3
#define MCPWM_FH0_B_OST_U_S  22
/* MCPWM_FH0_B_OST_D : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: One-shot mode action on PWM0B when fault event occurs and timer
 is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH0_B_OST_D  0x00000003
#define MCPWM_FH0_B_OST_D_M  ((MCPWM_FH0_B_OST_D_V)<<(MCPWM_FH0_B_OST_D_S))
#define MCPWM_FH0_B_OST_D_V  0x3
#define MCPWM_FH0_B_OST_D_S  20
/* MCPWM_FH0_B_CBC_U : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: Cycle-by-cycle mode action on PWM0B when fault event occurs and
 timer is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH0_B_CBC_U  0x00000003
#define MCPWM_FH0_B_CBC_U_M  ((MCPWM_FH0_B_CBC_U_V)<<(MCPWM_FH0_B_CBC_U_S))
#define MCPWM_FH0_B_CBC_U_V  0x3
#define MCPWM_FH0_B_CBC_U_S  18
/* MCPWM_FH0_B_CBC_D : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: Cycle-by-cycle mode action on PWM0B when fault event occurs and
 timer is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH0_B_CBC_D  0x00000003
#define MCPWM_FH0_B_CBC_D_M  ((MCPWM_FH0_B_CBC_D_V)<<(MCPWM_FH0_B_CBC_D_S))
#define MCPWM_FH0_B_CBC_D_V  0x3
#define MCPWM_FH0_B_CBC_D_S  16
/* MCPWM_FH0_A_OST_U : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: One-shot mode action on PWM0A when fault event occurs and timer
 is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH0_A_OST_U  0x00000003
#define MCPWM_FH0_A_OST_U_M  ((MCPWM_FH0_A_OST_U_V)<<(MCPWM_FH0_A_OST_U_S))
#define MCPWM_FH0_A_OST_U_V  0x3
#define MCPWM_FH0_A_OST_U_S  14
/* MCPWM_FH0_A_OST_D : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: One-shot mode action on PWM0A when fault event occurs and timer
 is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH0_A_OST_D  0x00000003
#define MCPWM_FH0_A_OST_D_M  ((MCPWM_FH0_A_OST_D_V)<<(MCPWM_FH0_A_OST_D_S))
#define MCPWM_FH0_A_OST_D_V  0x3
#define MCPWM_FH0_A_OST_D_S  12
/* MCPWM_FH0_A_CBC_U : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: Cycle-by-cycle mode action on PWM0A when fault event occurs and
 timer is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH0_A_CBC_U  0x00000003
#define MCPWM_FH0_A_CBC_U_M  ((MCPWM_FH0_A_CBC_U_V)<<(MCPWM_FH0_A_CBC_U_S))
#define MCPWM_FH0_A_CBC_U_V  0x3
#define MCPWM_FH0_A_CBC_U_S  10
/* MCPWM_FH0_A_CBC_D : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: Cycle-by-cycle mode action on PWM0A when fault event occurs and
 timer is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH0_A_CBC_D  0x00000003
#define MCPWM_FH0_A_CBC_D_M  ((MCPWM_FH0_A_CBC_D_V)<<(MCPWM_FH0_A_CBC_D_S))
#define MCPWM_FH0_A_CBC_D_V  0x3
#define MCPWM_FH0_A_CBC_D_S  8
/* MCPWM_FH0_F0_OST : R/W ;bitpos:[7] ;default: 1'd0 ; */
/*description: event_f0 will trigger one-shot mode action. 0: disable  1: enable*/
#define MCPWM_FH0_F0_OST  (BIT(7))
#define MCPWM_FH0_F0_OST_M  (BIT(7))
#define MCPWM_FH0_F0_OST_V  0x1
#define MCPWM_FH0_F0_OST_S  7
/* MCPWM_FH0_F1_OST : R/W ;bitpos:[6] ;default: 1'd0 ; */
/*description: event_f1 will trigger one-shot mode action. 0: disable  1: enable*/
#define MCPWM_FH0_F1_OST  (BIT(6))
#define MCPWM_FH0_F1_OST_M  (BIT(6))
#define MCPWM_FH0_F1_OST_V  0x1
#define MCPWM_FH0_F1_OST_S  6
/* MCPWM_FH0_F2_OST : R/W ;bitpos:[5] ;default: 1'd0 ; */
/*description: event_f2 will trigger one-shot mode action. 0: disable  1: enable*/
#define MCPWM_FH0_F2_OST  (BIT(5))
#define MCPWM_FH0_F2_OST_M  (BIT(5))
#define MCPWM_FH0_F2_OST_V  0x1
#define MCPWM_FH0_F2_OST_S  5
/* MCPWM_FH0_SW_OST : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: Enable register for software force one-shot mode action. 0: disable  1: enable*/
#define MCPWM_FH0_SW_OST  (BIT(4))
#define MCPWM_FH0_SW_OST_M  (BIT(4))
#define MCPWM_FH0_SW_OST_V  0x1
#define MCPWM_FH0_SW_OST_S  4
/* MCPWM_FH0_F0_CBC : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: event_f0 will trigger cycle-by-cycle mode action. 0: disable  1: enable*/
#define MCPWM_FH0_F0_CBC  (BIT(3))
#define MCPWM_FH0_F0_CBC_M  (BIT(3))
#define MCPWM_FH0_F0_CBC_V  0x1
#define MCPWM_FH0_F0_CBC_S  3
/* MCPWM_FH0_F1_CBC : R/W ;bitpos:[2] ;default: 1'd0 ; */
/*description: event_f1 will trigger cycle-by-cycle mode action. 0: disable  1: enable*/
#define MCPWM_FH0_F1_CBC  (BIT(2))
#define MCPWM_FH0_F1_CBC_M  (BIT(2))
#define MCPWM_FH0_F1_CBC_V  0x1
#define MCPWM_FH0_F1_CBC_S  2
/* MCPWM_FH0_F2_CBC : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: event_f2 will trigger cycle-by-cycle mode action. 0: disable  1: enable*/
#define MCPWM_FH0_F2_CBC  (BIT(1))
#define MCPWM_FH0_F2_CBC_M  (BIT(1))
#define MCPWM_FH0_F2_CBC_V  0x1
#define MCPWM_FH0_F2_CBC_S  1
/* MCPWM_FH0_SW_CBC : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: Enable register for software force cycle-by-cycle mode action.
 0: disable  1: enable*/
#define MCPWM_FH0_SW_CBC  (BIT(0))
#define MCPWM_FH0_SW_CBC_M  (BIT(0))
#define MCPWM_FH0_SW_CBC_V  0x1
#define MCPWM_FH0_SW_CBC_S  0

#define MCPWM_FH0_CFG1_REG(i)          (REG_MCPWM_BASE(i) + 0x006c)
/* MCPWM_FH0_FORCE_OST : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: A toggle (software negation of value of this bit) triggers a
 one-shot mode action*/
#define MCPWM_FH0_FORCE_OST  (BIT(4))
#define MCPWM_FH0_FORCE_OST_M  (BIT(4))
#define MCPWM_FH0_FORCE_OST_V  0x1
#define MCPWM_FH0_FORCE_OST_S  4
/* MCPWM_FH0_FORCE_CBC : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: A toggle triggers a cycle-by-cycle mode action*/
#define MCPWM_FH0_FORCE_CBC  (BIT(3))
#define MCPWM_FH0_FORCE_CBC_M  (BIT(3))
#define MCPWM_FH0_FORCE_CBC_V  0x1
#define MCPWM_FH0_FORCE_CBC_S  3
/* MCPWM_FH0_CBCPULSE : R/W ;bitpos:[2:1] ;default: 2'd0 ; */
/*description: The cycle-by-cycle mode action refresh moment selection. Bit0: TEZ  bit1:TEP*/
#define MCPWM_FH0_CBCPULSE  0x00000003
#define MCPWM_FH0_CBCPULSE_M  ((MCPWM_FH0_CBCPULSE_V)<<(MCPWM_FH0_CBCPULSE_S))
#define MCPWM_FH0_CBCPULSE_V  0x3
#define MCPWM_FH0_CBCPULSE_S  1
/* MCPWM_FH0_CLR_OST : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: A toggle will clear on going one-shot mode action*/
#define MCPWM_FH0_CLR_OST  (BIT(0))
#define MCPWM_FH0_CLR_OST_M  (BIT(0))
#define MCPWM_FH0_CLR_OST_V  0x1
#define MCPWM_FH0_CLR_OST_S  0

#define MCPWM_FH0_STATUS_REG(i)          (REG_MCPWM_BASE(i) + 0x0070)
/* MCPWM_FH0_OST_ON : RO ;bitpos:[1] ;default: 1'd0 ; */
/*description: Set and reset by hardware. If set  an one-shot mode action is on going*/
#define MCPWM_FH0_OST_ON  (BIT(1))
#define MCPWM_FH0_OST_ON_M  (BIT(1))
#define MCPWM_FH0_OST_ON_V  0x1
#define MCPWM_FH0_OST_ON_S  1
/* MCPWM_FH0_CBC_ON : RO ;bitpos:[0] ;default: 1'd0 ; */
/*description: Set and reset by hardware. If set  an cycle-by-cycle mode action is on going*/
#define MCPWM_FH0_CBC_ON  (BIT(0))
#define MCPWM_FH0_CBC_ON_M  (BIT(0))
#define MCPWM_FH0_CBC_ON_V  0x1
#define MCPWM_FH0_CBC_ON_S  0

#define MCPWM_GEN1_STMP_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x0074)
/* MCPWM_GEN1_B_SHDW_FULL : RO ;bitpos:[9] ;default: 1'd0 ; */
/*description: Set and reset by hardware. If set  PWM generator 1 time stamp
 B's shadow reg is filled and waiting to be transferred to B's active reg. If cleared  B's active reg has been updated with shadow reg latest value*/
#define MCPWM_GEN1_B_SHDW_FULL  (BIT(9))
#define MCPWM_GEN1_B_SHDW_FULL_M  (BIT(9))
#define MCPWM_GEN1_B_SHDW_FULL_V  0x1
#define MCPWM_GEN1_B_SHDW_FULL_S  9
/* MCPWM_GEN1_A_SHDW_FULL : RO ;bitpos:[8] ;default: 1'd0 ; */
/*description: Set and reset by hardware. If set  PWM generator 1 time stamp
 A's shadow reg is filled and waiting to be transferred to A's active reg. If cleared  A's active reg has been updated with shadow reg latest value*/
#define MCPWM_GEN1_A_SHDW_FULL  (BIT(8))
#define MCPWM_GEN1_A_SHDW_FULL_M  (BIT(8))
#define MCPWM_GEN1_A_SHDW_FULL_V  0x1
#define MCPWM_GEN1_A_SHDW_FULL_S  8
/* MCPWM_GEN1_B_UPMETHOD : R/W ;bitpos:[7:4] ;default: 4'd0 ; */
/*description: Update method for PWM generator 1 time stamp B's active reg.
 0: immediate  bit0: TEZ  bit1: TEP  bit2: sync  bit3: disable update*/
#define MCPWM_GEN1_B_UPMETHOD  0x0000000F
#define MCPWM_GEN1_B_UPMETHOD_M  ((MCPWM_GEN1_B_UPMETHOD_V)<<(MCPWM_GEN1_B_UPMETHOD_S))
#define MCPWM_GEN1_B_UPMETHOD_V  0xF
#define MCPWM_GEN1_B_UPMETHOD_S  4
/* MCPWM_GEN1_A_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: Update method for PWM generator 1 time stamp A's active reg.
 0: immediate  bit0: TEZ  bit1: TEP  bit2: sync  bit3: disable update*/
#define MCPWM_GEN1_A_UPMETHOD  0x0000000F
#define MCPWM_GEN1_A_UPMETHOD_M  ((MCPWM_GEN1_A_UPMETHOD_V)<<(MCPWM_GEN1_A_UPMETHOD_S))
#define MCPWM_GEN1_A_UPMETHOD_V  0xF
#define MCPWM_GEN1_A_UPMETHOD_S  0

#define MCPWM_GEN1_TSTMP_A_REG(i)          (REG_MCPWM_BASE(i) + 0x0078)
/* MCPWM_GEN1_A : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: PWM generator 1 time stamp A's shadow reg*/
#define MCPWM_GEN1_A  0x0000FFFF
#define MCPWM_GEN1_A_M  ((MCPWM_GEN1_A_V)<<(MCPWM_GEN1_A_S))
#define MCPWM_GEN1_A_V  0xFFFF
#define MCPWM_GEN1_A_S  0

#define MCPWM_GEN1_TSTMP_B_REG(i)          (REG_MCPWM_BASE(i) + 0x007c)
/* MCPWM_GEN1_B : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: PWM generator 1 time stamp B's shadow reg*/
#define MCPWM_GEN1_B  0x0000FFFF
#define MCPWM_GEN1_B_M  ((MCPWM_GEN1_B_V)<<(MCPWM_GEN1_B_S))
#define MCPWM_GEN1_B_V  0xFFFF
#define MCPWM_GEN1_B_S  0

#define MCPWM_GEN1_CFG0_REG(i)          (REG_MCPWM_BASE(i) + 0x0080)
/* MCPWM_GEN1_T1_SEL : R/W ;bitpos:[9:7] ;default: 3'd0 ; */
/*description: Source selection for PWM generate1 event_t1  take effect immediately
  0: fault_event0  1: fault_event1  2: fault_event2  3: sync_taken  4: none*/
#define MCPWM_GEN1_T1_SEL  0x00000007
#define MCPWM_GEN1_T1_SEL_M  ((MCPWM_GEN1_T1_SEL_V)<<(MCPWM_GEN1_T1_SEL_S))
#define MCPWM_GEN1_T1_SEL_V  0x7
#define MCPWM_GEN1_T1_SEL_S  7
/* MCPWM_GEN1_T0_SEL : R/W ;bitpos:[6:4] ;default: 3'd0 ; */
/*description: Source selection for PWM generate1 event_t0  take effect immediately
  0: fault_event0  1: fault_event1  2: fault_event2  3: sync_taken  4: none*/
#define MCPWM_GEN1_T0_SEL  0x00000007
#define MCPWM_GEN1_T0_SEL_M  ((MCPWM_GEN1_T0_SEL_V)<<(MCPWM_GEN1_T0_SEL_S))
#define MCPWM_GEN1_T0_SEL_V  0x7
#define MCPWM_GEN1_T0_SEL_S  4
/* MCPWM_GEN1_CFG_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: Update method for PWM generate1's active reg of configuration.
 0: immediate  bit0: TEZ  bit1: TEP  bit2: sync. bit3: disable update*/
#define MCPWM_GEN1_CFG_UPMETHOD  0x0000000F
#define MCPWM_GEN1_CFG_UPMETHOD_M  ((MCPWM_GEN1_CFG_UPMETHOD_V)<<(MCPWM_GEN1_CFG_UPMETHOD_S))
#define MCPWM_GEN1_CFG_UPMETHOD_V  0xF
#define MCPWM_GEN1_CFG_UPMETHOD_S  0

#define MCPWM_GEN1_FORCE_REG(i)          (REG_MCPWM_BASE(i) + 0x0084)
/* MCPWM_GEN1_B_NCIFORCE_MODE : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: Non-continuous immediate software force mode for PWM1B  0: disabled
  1: low  2: high  3: disabled*/
#define MCPWM_GEN1_B_NCIFORCE_MODE  0x00000003
#define MCPWM_GEN1_B_NCIFORCE_MODE_M  ((MCPWM_GEN1_B_NCIFORCE_MODE_V)<<(MCPWM_GEN1_B_NCIFORCE_MODE_S))
#define MCPWM_GEN1_B_NCIFORCE_MODE_V  0x3
#define MCPWM_GEN1_B_NCIFORCE_MODE_S  14
/* MCPWM_GEN1_B_NCIFORCE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: Non-continuous immediate software force trigger for PWM1B  a
 toggle will trigger a force event*/
#define MCPWM_GEN1_B_NCIFORCE  (BIT(13))
#define MCPWM_GEN1_B_NCIFORCE_M  (BIT(13))
#define MCPWM_GEN1_B_NCIFORCE_V  0x1
#define MCPWM_GEN1_B_NCIFORCE_S  13
/* MCPWM_GEN1_A_NCIFORCE_MODE : R/W ;bitpos:[12:11] ;default: 2'd0 ; */
/*description: Non-continuous immediate software force mode for PWM1A  0: disabled
  1: low  2: high  3: disabled*/
#define MCPWM_GEN1_A_NCIFORCE_MODE  0x00000003
#define MCPWM_GEN1_A_NCIFORCE_MODE_M  ((MCPWM_GEN1_A_NCIFORCE_MODE_V)<<(MCPWM_GEN1_A_NCIFORCE_MODE_S))
#define MCPWM_GEN1_A_NCIFORCE_MODE_V  0x3
#define MCPWM_GEN1_A_NCIFORCE_MODE_S  11
/* MCPWM_GEN1_A_NCIFORCE : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: Non-continuous immediate software force trigger for PWM1A  a
 toggle will trigger a force event*/
#define MCPWM_GEN1_A_NCIFORCE  (BIT(10))
#define MCPWM_GEN1_A_NCIFORCE_M  (BIT(10))
#define MCPWM_GEN1_A_NCIFORCE_V  0x1
#define MCPWM_GEN1_A_NCIFORCE_S  10
/* MCPWM_GEN1_B_CNTUFORCE_MODE : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: Continuous software force mode for PWM1B. 0: disabled  1: low
  2: high  3: disabled*/
#define MCPWM_GEN1_B_CNTUFORCE_MODE  0x00000003
#define MCPWM_GEN1_B_CNTUFORCE_MODE_M  ((MCPWM_GEN1_B_CNTUFORCE_MODE_V)<<(MCPWM_GEN1_B_CNTUFORCE_MODE_S))
#define MCPWM_GEN1_B_CNTUFORCE_MODE_V  0x3
#define MCPWM_GEN1_B_CNTUFORCE_MODE_S  8
/* MCPWM_GEN1_A_CNTUFORCE_MODE : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: Continuous software force mode for PWM1A. 0: disabled  1: low
  2: high  3: disabled*/
#define MCPWM_GEN1_A_CNTUFORCE_MODE  0x00000003
#define MCPWM_GEN1_A_CNTUFORCE_MODE_M  ((MCPWM_GEN1_A_CNTUFORCE_MODE_V)<<(MCPWM_GEN1_A_CNTUFORCE_MODE_S))
#define MCPWM_GEN1_A_CNTUFORCE_MODE_V  0x3
#define MCPWM_GEN1_A_CNTUFORCE_MODE_S  6
/* MCPWM_GEN1_CNTUFORCE_UPMETHOD : R/W ;bitpos:[5:0] ;default: 6'h20 ; */
/*description: Update method for continuous software force of PWM generator1.
 0: immediate  bit0: TEZ  bit1: TEP  bit2: TEA  bit3: TEB  bit4: sync  bit5: disable update. (TEA/B here and below means an event generated when timer value equals A/B register)*/
#define MCPWM_GEN1_CNTUFORCE_UPMETHOD  0x0000003F
#define MCPWM_GEN1_CNTUFORCE_UPMETHOD_M  ((MCPWM_GEN1_CNTUFORCE_UPMETHOD_V)<<(MCPWM_GEN1_CNTUFORCE_UPMETHOD_S))
#define MCPWM_GEN1_CNTUFORCE_UPMETHOD_V  0x3F
#define MCPWM_GEN1_CNTUFORCE_UPMETHOD_S  0

#define MCPWM_GEN1_A_REG(i)          (REG_MCPWM_BASE(i) + 0x0088)
/* MCPWM_GEN1_A_DT1 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: Action on PWM1A triggered by event_t1 when timer decreasing.
 0: no change  1: low  2: high  3: toggle*/
#define MCPWM_GEN1_A_DT1  0x00000003
#define MCPWM_GEN1_A_DT1_M  ((MCPWM_GEN1_A_DT1_V)<<(MCPWM_GEN1_A_DT1_S))
#define MCPWM_GEN1_A_DT1_V  0x3
#define MCPWM_GEN1_A_DT1_S  22
/* MCPWM_GEN1_A_DT0 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: Action on PWM1A triggered by event_t0 when timer decreasing*/
#define MCPWM_GEN1_A_DT0  0x00000003
#define MCPWM_GEN1_A_DT0_M  ((MCPWM_GEN1_A_DT0_V)<<(MCPWM_GEN1_A_DT0_S))
#define MCPWM_GEN1_A_DT0_V  0x3
#define MCPWM_GEN1_A_DT0_S  20
/* MCPWM_GEN1_A_DTEB : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: Action on PWM1A triggered by event TEB when timer decreasing*/
#define MCPWM_GEN1_A_DTEB  0x00000003
#define MCPWM_GEN1_A_DTEB_M  ((MCPWM_GEN1_A_DTEB_V)<<(MCPWM_GEN1_A_DTEB_S))
#define MCPWM_GEN1_A_DTEB_V  0x3
#define MCPWM_GEN1_A_DTEB_S  18
/* MCPWM_GEN1_A_DTEA : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: Action on PWM1A triggered by event TEA when timer decreasing*/
#define MCPWM_GEN1_A_DTEA  0x00000003
#define MCPWM_GEN1_A_DTEA_M  ((MCPWM_GEN1_A_DTEA_V)<<(MCPWM_GEN1_A_DTEA_S))
#define MCPWM_GEN1_A_DTEA_V  0x3
#define MCPWM_GEN1_A_DTEA_S  16
/* MCPWM_GEN1_A_DTEP : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: Action on PWM1A triggered by event TEP when timer decreasing*/
#define MCPWM_GEN1_A_DTEP  0x00000003
#define MCPWM_GEN1_A_DTEP_M  ((MCPWM_GEN1_A_DTEP_V)<<(MCPWM_GEN1_A_DTEP_S))
#define MCPWM_GEN1_A_DTEP_V  0x3
#define MCPWM_GEN1_A_DTEP_S  14
/* MCPWM_GEN1_A_DTEZ : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: Action on PWM1A triggered by event TEZ when timer decreasing*/
#define MCPWM_GEN1_A_DTEZ  0x00000003
#define MCPWM_GEN1_A_DTEZ_M  ((MCPWM_GEN1_A_DTEZ_V)<<(MCPWM_GEN1_A_DTEZ_S))
#define MCPWM_GEN1_A_DTEZ_V  0x3
#define MCPWM_GEN1_A_DTEZ_S  12
/* MCPWM_GEN1_A_UT1 : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: Action on PWM1A triggered by event_t1 when timer increasing*/
#define MCPWM_GEN1_A_UT1  0x00000003
#define MCPWM_GEN1_A_UT1_M  ((MCPWM_GEN1_A_UT1_V)<<(MCPWM_GEN1_A_UT1_S))
#define MCPWM_GEN1_A_UT1_V  0x3
#define MCPWM_GEN1_A_UT1_S  10
/* MCPWM_GEN1_A_UT0 : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: Action on PWM1A triggered by event_t0 when timer increasing*/
#define MCPWM_GEN1_A_UT0  0x00000003
#define MCPWM_GEN1_A_UT0_M  ((MCPWM_GEN1_A_UT0_V)<<(MCPWM_GEN1_A_UT0_S))
#define MCPWM_GEN1_A_UT0_V  0x3
#define MCPWM_GEN1_A_UT0_S  8
/* MCPWM_GEN1_A_UTEB : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: Action on PWM1A triggered by event TEB when timer increasing*/
#define MCPWM_GEN1_A_UTEB  0x00000003
#define MCPWM_GEN1_A_UTEB_M  ((MCPWM_GEN1_A_UTEB_V)<<(MCPWM_GEN1_A_UTEB_S))
#define MCPWM_GEN1_A_UTEB_V  0x3
#define MCPWM_GEN1_A_UTEB_S  6
/* MCPWM_GEN1_A_UTEA : R/W ;bitpos:[5:4] ;default: 2'd0 ; */
/*description: Action on PWM1A triggered by event TEA when timer increasing*/
#define MCPWM_GEN1_A_UTEA  0x00000003
#define MCPWM_GEN1_A_UTEA_M  ((MCPWM_GEN1_A_UTEA_V)<<(MCPWM_GEN1_A_UTEA_S))
#define MCPWM_GEN1_A_UTEA_V  0x3
#define MCPWM_GEN1_A_UTEA_S  4
/* MCPWM_GEN1_A_UTEP : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: Action on PWM1A triggered by event TEP when timer increasing*/
#define MCPWM_GEN1_A_UTEP  0x00000003
#define MCPWM_GEN1_A_UTEP_M  ((MCPWM_GEN1_A_UTEP_V)<<(MCPWM_GEN1_A_UTEP_S))
#define MCPWM_GEN1_A_UTEP_V  0x3
#define MCPWM_GEN1_A_UTEP_S  2
/* MCPWM_GEN1_A_UTEZ : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: Action on PWM1A triggered by event TEZ when timer increasing*/
#define MCPWM_GEN1_A_UTEZ  0x00000003
#define MCPWM_GEN1_A_UTEZ_M  ((MCPWM_GEN1_A_UTEZ_V)<<(MCPWM_GEN1_A_UTEZ_S))
#define MCPWM_GEN1_A_UTEZ_V  0x3
#define MCPWM_GEN1_A_UTEZ_S  0

#define MCPWM_GEN1_B_REG(i)          (REG_MCPWM_BASE(i) + 0x008c)
/* MCPWM_GEN1_B_DT1 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: Action on PWM1B triggered by event_t1 when timer decreasing.
 0: no change  1: low  2: high  3: toggle*/
#define MCPWM_GEN1_B_DT1  0x00000003
#define MCPWM_GEN1_B_DT1_M  ((MCPWM_GEN1_B_DT1_V)<<(MCPWM_GEN1_B_DT1_S))
#define MCPWM_GEN1_B_DT1_V  0x3
#define MCPWM_GEN1_B_DT1_S  22
/* MCPWM_GEN1_B_DT0 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: Action on PWM1B triggered by event_t0 when timer decreasing*/
#define MCPWM_GEN1_B_DT0  0x00000003
#define MCPWM_GEN1_B_DT0_M  ((MCPWM_GEN1_B_DT0_V)<<(MCPWM_GEN1_B_DT0_S))
#define MCPWM_GEN1_B_DT0_V  0x3
#define MCPWM_GEN1_B_DT0_S  20
/* MCPWM_GEN1_B_DTEB : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: Action on PWM1B triggered by event TEB when timer decreasing*/
#define MCPWM_GEN1_B_DTEB  0x00000003
#define MCPWM_GEN1_B_DTEB_M  ((MCPWM_GEN1_B_DTEB_V)<<(MCPWM_GEN1_B_DTEB_S))
#define MCPWM_GEN1_B_DTEB_V  0x3
#define MCPWM_GEN1_B_DTEB_S  18
/* MCPWM_GEN1_B_DTEA : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: Action on PWM1B triggered by event TEA when timer decreasing*/
#define MCPWM_GEN1_B_DTEA  0x00000003
#define MCPWM_GEN1_B_DTEA_M  ((MCPWM_GEN1_B_DTEA_V)<<(MCPWM_GEN1_B_DTEA_S))
#define MCPWM_GEN1_B_DTEA_V  0x3
#define MCPWM_GEN1_B_DTEA_S  16
/* MCPWM_GEN1_B_DTEP : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: Action on PWM1B triggered by event TEP when timer decreasing*/
#define MCPWM_GEN1_B_DTEP  0x00000003
#define MCPWM_GEN1_B_DTEP_M  ((MCPWM_GEN1_B_DTEP_V)<<(MCPWM_GEN1_B_DTEP_S))
#define MCPWM_GEN1_B_DTEP_V  0x3
#define MCPWM_GEN1_B_DTEP_S  14
/* MCPWM_GEN1_B_DTEZ : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: Action on PWM1B triggered by event TEZ when timer decreasing*/
#define MCPWM_GEN1_B_DTEZ  0x00000003
#define MCPWM_GEN1_B_DTEZ_M  ((MCPWM_GEN1_B_DTEZ_V)<<(MCPWM_GEN1_B_DTEZ_S))
#define MCPWM_GEN1_B_DTEZ_V  0x3
#define MCPWM_GEN1_B_DTEZ_S  12
/* MCPWM_GEN1_B_UT1 : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: Action on PWM1B triggered by event_t1 when timer increasing*/
#define MCPWM_GEN1_B_UT1  0x00000003
#define MCPWM_GEN1_B_UT1_M  ((MCPWM_GEN1_B_UT1_V)<<(MCPWM_GEN1_B_UT1_S))
#define MCPWM_GEN1_B_UT1_V  0x3
#define MCPWM_GEN1_B_UT1_S  10
/* MCPWM_GEN1_B_UT0 : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: Action on PWM1B triggered by event_t0 when timer increasing*/
#define MCPWM_GEN1_B_UT0  0x00000003
#define MCPWM_GEN1_B_UT0_M  ((MCPWM_GEN1_B_UT0_V)<<(MCPWM_GEN1_B_UT0_S))
#define MCPWM_GEN1_B_UT0_V  0x3
#define MCPWM_GEN1_B_UT0_S  8
/* MCPWM_GEN1_B_UTEB : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: Action on PWM1B triggered by event TEB when timer increasing*/
#define MCPWM_GEN1_B_UTEB  0x00000003
#define MCPWM_GEN1_B_UTEB_M  ((MCPWM_GEN1_B_UTEB_V)<<(MCPWM_GEN1_B_UTEB_S))
#define MCPWM_GEN1_B_UTEB_V  0x3
#define MCPWM_GEN1_B_UTEB_S  6
/* MCPWM_GEN1_B_UTEA : R/W ;bitpos:[5:4] ;default: 2'd0 ; */
/*description: Action on PWM1B triggered by event TEA when timer increasing*/
#define MCPWM_GEN1_B_UTEA  0x00000003
#define MCPWM_GEN1_B_UTEA_M  ((MCPWM_GEN1_B_UTEA_V)<<(MCPWM_GEN1_B_UTEA_S))
#define MCPWM_GEN1_B_UTEA_V  0x3
#define MCPWM_GEN1_B_UTEA_S  4
/* MCPWM_GEN1_B_UTEP : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: Action on PWM1B triggered by event TEP when timer increasing*/
#define MCPWM_GEN1_B_UTEP  0x00000003
#define MCPWM_GEN1_B_UTEP_M  ((MCPWM_GEN1_B_UTEP_V)<<(MCPWM_GEN1_B_UTEP_S))
#define MCPWM_GEN1_B_UTEP_V  0x3
#define MCPWM_GEN1_B_UTEP_S  2
/* MCPWM_GEN1_B_UTEZ : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: Action on PWM1B triggered by event TEZ when timer increasing*/
#define MCPWM_GEN1_B_UTEZ  0x00000003
#define MCPWM_GEN1_B_UTEZ_M  ((MCPWM_GEN1_B_UTEZ_V)<<(MCPWM_GEN1_B_UTEZ_S))
#define MCPWM_GEN1_B_UTEZ_V  0x3
#define MCPWM_GEN1_B_UTEZ_S  0

#define MCPWM_DT1_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x0090)
/* MCPWM_DT1_CLK_SEL : R/W ;bitpos:[17] ;default: 1'd0 ; */
/*description: Dead time generator 1 clock selection. 0: PWM_clk  1: PT_clk*/
#define MCPWM_DT1_CLK_SEL  (BIT(17))
#define MCPWM_DT1_CLK_SEL_M  (BIT(17))
#define MCPWM_DT1_CLK_SEL_V  0x1
#define MCPWM_DT1_CLK_SEL_S  17
/* MCPWM_DT1_B_OUTBYPASS : R/W ;bitpos:[16] ;default: 1'd1 ; */
/*description: S0 in documentation*/
#define MCPWM_DT1_B_OUTBYPASS  (BIT(16))
#define MCPWM_DT1_B_OUTBYPASS_M  (BIT(16))
#define MCPWM_DT1_B_OUTBYPASS_V  0x1
#define MCPWM_DT1_B_OUTBYPASS_S  16
/* MCPWM_DT1_A_OUTBYPASS : R/W ;bitpos:[15] ;default: 1'd1 ; */
/*description: S1 in documentation*/
#define MCPWM_DT1_A_OUTBYPASS  (BIT(15))
#define MCPWM_DT1_A_OUTBYPASS_M  (BIT(15))
#define MCPWM_DT1_A_OUTBYPASS_V  0x1
#define MCPWM_DT1_A_OUTBYPASS_S  15
/* MCPWM_DT1_FED_OUTINVERT : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: S3 in documentation*/
#define MCPWM_DT1_FED_OUTINVERT  (BIT(14))
#define MCPWM_DT1_FED_OUTINVERT_M  (BIT(14))
#define MCPWM_DT1_FED_OUTINVERT_V  0x1
#define MCPWM_DT1_FED_OUTINVERT_S  14
/* MCPWM_DT1_RED_OUTINVERT : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: S2 in documentation*/
#define MCPWM_DT1_RED_OUTINVERT  (BIT(13))
#define MCPWM_DT1_RED_OUTINVERT_M  (BIT(13))
#define MCPWM_DT1_RED_OUTINVERT_V  0x1
#define MCPWM_DT1_RED_OUTINVERT_S  13
/* MCPWM_DT1_FED_INSEL : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: S5 in documentation*/
#define MCPWM_DT1_FED_INSEL  (BIT(12))
#define MCPWM_DT1_FED_INSEL_M  (BIT(12))
#define MCPWM_DT1_FED_INSEL_V  0x1
#define MCPWM_DT1_FED_INSEL_S  12
/* MCPWM_DT1_RED_INSEL : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: S4 in documentation*/
#define MCPWM_DT1_RED_INSEL  (BIT(11))
#define MCPWM_DT1_RED_INSEL_M  (BIT(11))
#define MCPWM_DT1_RED_INSEL_V  0x1
#define MCPWM_DT1_RED_INSEL_S  11
/* MCPWM_DT1_B_OUTSWAP : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: S7 in documentation*/
#define MCPWM_DT1_B_OUTSWAP  (BIT(10))
#define MCPWM_DT1_B_OUTSWAP_M  (BIT(10))
#define MCPWM_DT1_B_OUTSWAP_V  0x1
#define MCPWM_DT1_B_OUTSWAP_S  10
/* MCPWM_DT1_A_OUTSWAP : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: S6 in documentation*/
#define MCPWM_DT1_A_OUTSWAP  (BIT(9))
#define MCPWM_DT1_A_OUTSWAP_M  (BIT(9))
#define MCPWM_DT1_A_OUTSWAP_V  0x1
#define MCPWM_DT1_A_OUTSWAP_S  9
/* MCPWM_DT1_DEB_MODE : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: S8 in documentation  dual-edge B mode  0: FED/RED take effect
 on different path separately  1: FED/RED take effect on B path  A out is in bypass or normal operation mode*/
#define MCPWM_DT1_DEB_MODE  (BIT(8))
#define MCPWM_DT1_DEB_MODE_M  (BIT(8))
#define MCPWM_DT1_DEB_MODE_V  0x1
#define MCPWM_DT1_DEB_MODE_S  8
/* MCPWM_DT1_RED_UPMETHOD : R/W ;bitpos:[7:4] ;default: 4'd0 ; */
/*description: Update method for RED (rising edge delay) active reg. 0: immediate
  bit0: TEZ  bit1: TEP  bit2: sync  bit3: disable update*/
#define MCPWM_DT1_RED_UPMETHOD  0x0000000F
#define MCPWM_DT1_RED_UPMETHOD_M  ((MCPWM_DT1_RED_UPMETHOD_V)<<(MCPWM_DT1_RED_UPMETHOD_S))
#define MCPWM_DT1_RED_UPMETHOD_V  0xF
#define MCPWM_DT1_RED_UPMETHOD_S  4
/* MCPWM_DT1_FED_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: Update method for FED (falling edge delay) active reg. 0: immediate
  bit0: TEZ  bit1: TEP  bit2: sync  bit3: disable update*/
#define MCPWM_DT1_FED_UPMETHOD  0x0000000F
#define MCPWM_DT1_FED_UPMETHOD_M  ((MCPWM_DT1_FED_UPMETHOD_V)<<(MCPWM_DT1_FED_UPMETHOD_S))
#define MCPWM_DT1_FED_UPMETHOD_V  0xF
#define MCPWM_DT1_FED_UPMETHOD_S  0

#define MCPWM_DT1_FED_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x0094)
/* MCPWM_DT1_FED : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: Shadow reg for FED*/
#define MCPWM_DT1_FED  0x0000FFFF
#define MCPWM_DT1_FED_M  ((MCPWM_DT1_FED_V)<<(MCPWM_DT1_FED_S))
#define MCPWM_DT1_FED_V  0xFFFF
#define MCPWM_DT1_FED_S  0

#define MCPWM_DT1_RED_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x0098)
/* MCPWM_DT1_RED : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: Shadow reg for RED*/
#define MCPWM_DT1_RED  0x0000FFFF
#define MCPWM_DT1_RED_M  ((MCPWM_DT1_RED_V)<<(MCPWM_DT1_RED_S))
#define MCPWM_DT1_RED_V  0xFFFF
#define MCPWM_DT1_RED_S  0

#define MCPWM_CARRIER1_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x009c)
/* MCPWM_CARRIER1_IN_INVERT : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: When set  invert the input of PWM1A and PWM1B for this submodule*/
#define MCPWM_CARRIER1_IN_INVERT  (BIT(13))
#define MCPWM_CARRIER1_IN_INVERT_M  (BIT(13))
#define MCPWM_CARRIER1_IN_INVERT_V  0x1
#define MCPWM_CARRIER1_IN_INVERT_S  13
/* MCPWM_CARRIER1_OUT_INVERT : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: When set  invert the output of PWM1A and PWM1B for this submodule*/
#define MCPWM_CARRIER1_OUT_INVERT  (BIT(12))
#define MCPWM_CARRIER1_OUT_INVERT_M  (BIT(12))
#define MCPWM_CARRIER1_OUT_INVERT_V  0x1
#define MCPWM_CARRIER1_OUT_INVERT_S  12
/* MCPWM_CARRIER1_OSHWTH : R/W ;bitpos:[11:8] ;default: 4'd0 ; */
/*description: Width of the fist pulse in number of periods of the carrier*/
#define MCPWM_CARRIER1_OSHWTH  0x0000000F
#define MCPWM_CARRIER1_OSHWTH_M  ((MCPWM_CARRIER1_OSHWTH_V)<<(MCPWM_CARRIER1_OSHWTH_S))
#define MCPWM_CARRIER1_OSHWTH_V  0xF
#define MCPWM_CARRIER1_OSHWTH_S  8
/* MCPWM_CARRIER1_DUTY : R/W ;bitpos:[7:5] ;default: 3'd0 ; */
/*description: Carrier duty selection. Duty = PWM_CARRIER1_DUTY / 8*/
#define MCPWM_CARRIER1_DUTY  0x00000007
#define MCPWM_CARRIER1_DUTY_M  ((MCPWM_CARRIER1_DUTY_V)<<(MCPWM_CARRIER1_DUTY_S))
#define MCPWM_CARRIER1_DUTY_V  0x7
#define MCPWM_CARRIER1_DUTY_S  5
/* MCPWM_CARRIER1_PRESCALE : R/W ;bitpos:[4:1] ;default: 4'd0 ; */
/*description: PWM carrier1 clock (PC_clk) prescale value. Period of PC_clk
 = period of PWM_clk * (PWM_CARRIER1_PRESCALE + 1)*/
#define MCPWM_CARRIER1_PRESCALE  0x0000000F
#define MCPWM_CARRIER1_PRESCALE_M  ((MCPWM_CARRIER1_PRESCALE_V)<<(MCPWM_CARRIER1_PRESCALE_S))
#define MCPWM_CARRIER1_PRESCALE_V  0xF
#define MCPWM_CARRIER1_PRESCALE_S  1
/* MCPWM_CARRIER1_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: When set  carrier1 function is enabled. When cleared  carrier1 is bypassed*/
#define MCPWM_CARRIER1_EN  (BIT(0))
#define MCPWM_CARRIER1_EN_M  (BIT(0))
#define MCPWM_CARRIER1_EN_V  0x1
#define MCPWM_CARRIER1_EN_S  0

#define MCPWM_FH1_CFG0_REG(i)          (REG_MCPWM_BASE(i) + 0x00a0)
/* MCPWM_FH1_B_OST_U : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: One-shot mode action on PWM1B when fault event occurs and timer
 is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH1_B_OST_U  0x00000003
#define MCPWM_FH1_B_OST_U_M  ((MCPWM_FH1_B_OST_U_V)<<(MCPWM_FH1_B_OST_U_S))
#define MCPWM_FH1_B_OST_U_V  0x3
#define MCPWM_FH1_B_OST_U_S  22
/* MCPWM_FH1_B_OST_D : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: One-shot mode action on PWM1B when fault event occurs and timer
 is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH1_B_OST_D  0x00000003
#define MCPWM_FH1_B_OST_D_M  ((MCPWM_FH1_B_OST_D_V)<<(MCPWM_FH1_B_OST_D_S))
#define MCPWM_FH1_B_OST_D_V  0x3
#define MCPWM_FH1_B_OST_D_S  20
/* MCPWM_FH1_B_CBC_U : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: Cycle-by-cycle mode action on PWM1B when fault event occurs and
 timer is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH1_B_CBC_U  0x00000003
#define MCPWM_FH1_B_CBC_U_M  ((MCPWM_FH1_B_CBC_U_V)<<(MCPWM_FH1_B_CBC_U_S))
#define MCPWM_FH1_B_CBC_U_V  0x3
#define MCPWM_FH1_B_CBC_U_S  18
/* MCPWM_FH1_B_CBC_D : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: Cycle-by-cycle mode action on PWM1B when fault event occurs and
 timer is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH1_B_CBC_D  0x00000003
#define MCPWM_FH1_B_CBC_D_M  ((MCPWM_FH1_B_CBC_D_V)<<(MCPWM_FH1_B_CBC_D_S))
#define MCPWM_FH1_B_CBC_D_V  0x3
#define MCPWM_FH1_B_CBC_D_S  16
/* MCPWM_FH1_A_OST_U : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: One-shot mode action on PWM1A when fault event occurs and timer
 is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH1_A_OST_U  0x00000003
#define MCPWM_FH1_A_OST_U_M  ((MCPWM_FH1_A_OST_U_V)<<(MCPWM_FH1_A_OST_U_S))
#define MCPWM_FH1_A_OST_U_V  0x3
#define MCPWM_FH1_A_OST_U_S  14
/* MCPWM_FH1_A_OST_D : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: One-shot mode action on PWM1A when fault event occurs and timer
 is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH1_A_OST_D  0x00000003
#define MCPWM_FH1_A_OST_D_M  ((MCPWM_FH1_A_OST_D_V)<<(MCPWM_FH1_A_OST_D_S))
#define MCPWM_FH1_A_OST_D_V  0x3
#define MCPWM_FH1_A_OST_D_S  12
/* MCPWM_FH1_A_CBC_U : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: Cycle-by-cycle mode action on PWM1A when fault event occurs and
 timer is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH1_A_CBC_U  0x00000003
#define MCPWM_FH1_A_CBC_U_M  ((MCPWM_FH1_A_CBC_U_V)<<(MCPWM_FH1_A_CBC_U_S))
#define MCPWM_FH1_A_CBC_U_V  0x3
#define MCPWM_FH1_A_CBC_U_S  10
/* MCPWM_FH1_A_CBC_D : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: Cycle-by-cycle mode action on PWM1A when fault event occurs and
 timer is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH1_A_CBC_D  0x00000003
#define MCPWM_FH1_A_CBC_D_M  ((MCPWM_FH1_A_CBC_D_V)<<(MCPWM_FH1_A_CBC_D_S))
#define MCPWM_FH1_A_CBC_D_V  0x3
#define MCPWM_FH1_A_CBC_D_S  8
/* MCPWM_FH1_F0_OST : R/W ;bitpos:[7] ;default: 1'd0 ; */
/*description: event_f0 will trigger one-shot mode action. 0: disable  1: enable*/
#define MCPWM_FH1_F0_OST  (BIT(7))
#define MCPWM_FH1_F0_OST_M  (BIT(7))
#define MCPWM_FH1_F0_OST_V  0x1
#define MCPWM_FH1_F0_OST_S  7
/* MCPWM_FH1_F1_OST : R/W ;bitpos:[6] ;default: 1'd0 ; */
/*description: event_f1 will trigger one-shot mode action. 0: disable  1: enable*/
#define MCPWM_FH1_F1_OST  (BIT(6))
#define MCPWM_FH1_F1_OST_M  (BIT(6))
#define MCPWM_FH1_F1_OST_V  0x1
#define MCPWM_FH1_F1_OST_S  6
/* MCPWM_FH1_F2_OST : R/W ;bitpos:[5] ;default: 1'd0 ; */
/*description: event_f2 will trigger one-shot mode action. 0: disable  1: enable*/
#define MCPWM_FH1_F2_OST  (BIT(5))
#define MCPWM_FH1_F2_OST_M  (BIT(5))
#define MCPWM_FH1_F2_OST_V  0x1
#define MCPWM_FH1_F2_OST_S  5
/* MCPWM_FH1_SW_OST : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: Enable register for software force one-shot mode action. 0: disable  1: enable*/
#define MCPWM_FH1_SW_OST  (BIT(4))
#define MCPWM_FH1_SW_OST_M  (BIT(4))
#define MCPWM_FH1_SW_OST_V  0x1
#define MCPWM_FH1_SW_OST_S  4
/* MCPWM_FH1_F0_CBC : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: event_f0 will trigger cycle-by-cycle mode action. 0: disable  1: enable*/
#define MCPWM_FH1_F0_CBC  (BIT(3))
#define MCPWM_FH1_F0_CBC_M  (BIT(3))
#define MCPWM_FH1_F0_CBC_V  0x1
#define MCPWM_FH1_F0_CBC_S  3
/* MCPWM_FH1_F1_CBC : R/W ;bitpos:[2] ;default: 1'd0 ; */
/*description: event_f1 will trigger cycle-by-cycle mode action. 0: disable  1: enable*/
#define MCPWM_FH1_F1_CBC  (BIT(2))
#define MCPWM_FH1_F1_CBC_M  (BIT(2))
#define MCPWM_FH1_F1_CBC_V  0x1
#define MCPWM_FH1_F1_CBC_S  2
/* MCPWM_FH1_F2_CBC : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: event_f2 will trigger cycle-by-cycle mode action. 0: disable  1: enable*/
#define MCPWM_FH1_F2_CBC  (BIT(1))
#define MCPWM_FH1_F2_CBC_M  (BIT(1))
#define MCPWM_FH1_F2_CBC_V  0x1
#define MCPWM_FH1_F2_CBC_S  1
/* MCPWM_FH1_SW_CBC : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: Enable register for software force cycle-by-cycle mode action.
 0: disable  1: enable*/
#define MCPWM_FH1_SW_CBC  (BIT(0))
#define MCPWM_FH1_SW_CBC_M  (BIT(0))
#define MCPWM_FH1_SW_CBC_V  0x1
#define MCPWM_FH1_SW_CBC_S  0

#define MCPWM_FH1_CFG1_REG(i)          (REG_MCPWM_BASE(i) + 0x00a4)
/* MCPWM_FH1_FORCE_OST : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: A toggle (software negation of value of this bit) triggers a
 one-shot mode action*/
#define MCPWM_FH1_FORCE_OST  (BIT(4))
#define MCPWM_FH1_FORCE_OST_M  (BIT(4))
#define MCPWM_FH1_FORCE_OST_V  0x1
#define MCPWM_FH1_FORCE_OST_S  4
/* MCPWM_FH1_FORCE_CBC : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: A toggle triggers a cycle-by-cycle mode action*/
#define MCPWM_FH1_FORCE_CBC  (BIT(3))
#define MCPWM_FH1_FORCE_CBC_M  (BIT(3))
#define MCPWM_FH1_FORCE_CBC_V  0x1
#define MCPWM_FH1_FORCE_CBC_S  3
/* MCPWM_FH1_CBCPULSE : R/W ;bitpos:[2:1] ;default: 2'd0 ; */
/*description: The cycle-by-cycle mode action refresh moment selection. Bit0: TEZ  bit1:TEP*/
#define MCPWM_FH1_CBCPULSE  0x00000003
#define MCPWM_FH1_CBCPULSE_M  ((MCPWM_FH1_CBCPULSE_V)<<(MCPWM_FH1_CBCPULSE_S))
#define MCPWM_FH1_CBCPULSE_V  0x3
#define MCPWM_FH1_CBCPULSE_S  1
/* MCPWM_FH1_CLR_OST : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: A toggle will clear on going one-shot mode action*/
#define MCPWM_FH1_CLR_OST  (BIT(0))
#define MCPWM_FH1_CLR_OST_M  (BIT(0))
#define MCPWM_FH1_CLR_OST_V  0x1
#define MCPWM_FH1_CLR_OST_S  0

#define MCPWM_FH1_STATUS_REG(i)          (REG_MCPWM_BASE(i) + 0x00a8)
/* MCPWM_FH1_OST_ON : RO ;bitpos:[1] ;default: 1'd0 ; */
/*description: Set and reset by hardware. If set  an one-shot mode action is on going*/
#define MCPWM_FH1_OST_ON  (BIT(1))
#define MCPWM_FH1_OST_ON_M  (BIT(1))
#define MCPWM_FH1_OST_ON_V  0x1
#define MCPWM_FH1_OST_ON_S  1
/* MCPWM_FH1_CBC_ON : RO ;bitpos:[0] ;default: 1'd0 ; */
/*description: Set and reset by hardware. If set  an cycle-by-cycle mode action is on going*/
#define MCPWM_FH1_CBC_ON  (BIT(0))
#define MCPWM_FH1_CBC_ON_M  (BIT(0))
#define MCPWM_FH1_CBC_ON_V  0x1
#define MCPWM_FH1_CBC_ON_S  0

#define MCPWM_GEN2_STMP_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x00ac)
/* MCPWM_GEN2_B_SHDW_FULL : RO ;bitpos:[9] ;default: 1'd0 ; */
/*description: Set and reset by hardware. If set  PWM generator 2 time stamp
 B's shadow reg is filled and waiting to be transferred to B's active reg. If cleared  B's active reg has been updated with shadow reg latest value*/
#define MCPWM_GEN2_B_SHDW_FULL  (BIT(9))
#define MCPWM_GEN2_B_SHDW_FULL_M  (BIT(9))
#define MCPWM_GEN2_B_SHDW_FULL_V  0x1
#define MCPWM_GEN2_B_SHDW_FULL_S  9
/* MCPWM_GEN2_A_SHDW_FULL : RO ;bitpos:[8] ;default: 1'd0 ; */
/*description: Set and reset by hardware. If set  PWM generator 2 time stamp
 A's shadow reg is filled and waiting to be transferred to A's active reg. If cleared  A's active reg has been updated with shadow reg latest value*/
#define MCPWM_GEN2_A_SHDW_FULL  (BIT(8))
#define MCPWM_GEN2_A_SHDW_FULL_M  (BIT(8))
#define MCPWM_GEN2_A_SHDW_FULL_V  0x1
#define MCPWM_GEN2_A_SHDW_FULL_S  8
/* MCPWM_GEN2_B_UPMETHOD : R/W ;bitpos:[7:4] ;default: 4'd0 ; */
/*description: Update method for PWM generator 2 time stamp B's active reg.
 0: immediate  bit0: TEZ  bit1: TEP  bit2: sync  bit3: disable update*/
#define MCPWM_GEN2_B_UPMETHOD  0x0000000F
#define MCPWM_GEN2_B_UPMETHOD_M  ((MCPWM_GEN2_B_UPMETHOD_V)<<(MCPWM_GEN2_B_UPMETHOD_S))
#define MCPWM_GEN2_B_UPMETHOD_V  0xF
#define MCPWM_GEN2_B_UPMETHOD_S  4
/* MCPWM_GEN2_A_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: Update method for PWM generator 2 time stamp A's active reg.
 0: immediate  bit0: TEZ  bit1: TEP  bit2: sync  bit3: disable update*/
#define MCPWM_GEN2_A_UPMETHOD  0x0000000F
#define MCPWM_GEN2_A_UPMETHOD_M  ((MCPWM_GEN2_A_UPMETHOD_V)<<(MCPWM_GEN2_A_UPMETHOD_S))
#define MCPWM_GEN2_A_UPMETHOD_V  0xF
#define MCPWM_GEN2_A_UPMETHOD_S  0

#define MCPWM_GEN2_TSTMP_A_REG(i)          (REG_MCPWM_BASE(i) + 0x00b0)
/* MCPWM_GEN2_A : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: PWM generator 2 time stamp A's shadow reg*/
#define MCPWM_GEN2_A  0x0000FFFF
#define MCPWM_GEN2_A_M  ((MCPWM_GEN2_A_V)<<(MCPWM_GEN2_A_S))
#define MCPWM_GEN2_A_V  0xFFFF
#define MCPWM_GEN2_A_S  0

#define MCPWM_GEN2_TSTMP_B_REG(i)          (REG_MCPWM_BASE(i) + 0x00b4)
/* MCPWM_GEN2_B : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: PWM generator 2 time stamp B's shadow reg*/
#define MCPWM_GEN2_B  0x0000FFFF
#define MCPWM_GEN2_B_M  ((MCPWM_GEN2_B_V)<<(MCPWM_GEN2_B_S))
#define MCPWM_GEN2_B_V  0xFFFF
#define MCPWM_GEN2_B_S  0

#define MCPWM_GEN2_CFG0_REG(i)          (REG_MCPWM_BASE(i) + 0x00b8)
/* MCPWM_GEN2_T1_SEL : R/W ;bitpos:[9:7] ;default: 3'd0 ; */
/*description: Source selection for PWM generate2 event_t1  take effect immediately
  0: fault_event0  1: fault_event1  2: fault_event2  3: sync_taken  4: none*/
#define MCPWM_GEN2_T1_SEL  0x00000007
#define MCPWM_GEN2_T1_SEL_M  ((MCPWM_GEN2_T1_SEL_V)<<(MCPWM_GEN2_T1_SEL_S))
#define MCPWM_GEN2_T1_SEL_V  0x7
#define MCPWM_GEN2_T1_SEL_S  7
/* MCPWM_GEN2_T0_SEL : R/W ;bitpos:[6:4] ;default: 3'd0 ; */
/*description: Source selection for PWM generate2 event_t0  take effect immediately
  0: fault_event0  1: fault_event1  2: fault_event2  3: sync_taken  4: none*/
#define MCPWM_GEN2_T0_SEL  0x00000007
#define MCPWM_GEN2_T0_SEL_M  ((MCPWM_GEN2_T0_SEL_V)<<(MCPWM_GEN2_T0_SEL_S))
#define MCPWM_GEN2_T0_SEL_V  0x7
#define MCPWM_GEN2_T0_SEL_S  4
/* MCPWM_GEN2_CFG_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: Update method for PWM generate2's active reg of configuration.
 0: immediate  bit0: TEZ  bit1: TEP  bit2: sync. bit3: disable update*/
#define MCPWM_GEN2_CFG_UPMETHOD  0x0000000F
#define MCPWM_GEN2_CFG_UPMETHOD_M  ((MCPWM_GEN2_CFG_UPMETHOD_V)<<(MCPWM_GEN2_CFG_UPMETHOD_S))
#define MCPWM_GEN2_CFG_UPMETHOD_V  0xF
#define MCPWM_GEN2_CFG_UPMETHOD_S  0

#define MCPWM_GEN2_FORCE_REG(i)          (REG_MCPWM_BASE(i) + 0x00bc)
/* MCPWM_GEN2_B_NCIFORCE_MODE : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: Non-continuous immediate software force mode for PWM2B  0: disabled
  1: low  2: high  3: disabled*/
#define MCPWM_GEN2_B_NCIFORCE_MODE  0x00000003
#define MCPWM_GEN2_B_NCIFORCE_MODE_M  ((MCPWM_GEN2_B_NCIFORCE_MODE_V)<<(MCPWM_GEN2_B_NCIFORCE_MODE_S))
#define MCPWM_GEN2_B_NCIFORCE_MODE_V  0x3
#define MCPWM_GEN2_B_NCIFORCE_MODE_S  14
/* MCPWM_GEN2_B_NCIFORCE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: Non-continuous immediate software force trigger for PWM2B  a
 toggle will trigger a force event*/
#define MCPWM_GEN2_B_NCIFORCE  (BIT(13))
#define MCPWM_GEN2_B_NCIFORCE_M  (BIT(13))
#define MCPWM_GEN2_B_NCIFORCE_V  0x1
#define MCPWM_GEN2_B_NCIFORCE_S  13
/* MCPWM_GEN2_A_NCIFORCE_MODE : R/W ;bitpos:[12:11] ;default: 2'd0 ; */
/*description: Non-continuous immediate software force mode for PWM2A  0: disabled
  1: low  2: high  3: disabled*/
#define MCPWM_GEN2_A_NCIFORCE_MODE  0x00000003
#define MCPWM_GEN2_A_NCIFORCE_MODE_M  ((MCPWM_GEN2_A_NCIFORCE_MODE_V)<<(MCPWM_GEN2_A_NCIFORCE_MODE_S))
#define MCPWM_GEN2_A_NCIFORCE_MODE_V  0x3
#define MCPWM_GEN2_A_NCIFORCE_MODE_S  11
/* MCPWM_GEN2_A_NCIFORCE : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: Non-continuous immediate software force trigger for PWM2A  a
 toggle will trigger a force event*/
#define MCPWM_GEN2_A_NCIFORCE  (BIT(10))
#define MCPWM_GEN2_A_NCIFORCE_M  (BIT(10))
#define MCPWM_GEN2_A_NCIFORCE_V  0x1
#define MCPWM_GEN2_A_NCIFORCE_S  10
/* MCPWM_GEN2_B_CNTUFORCE_MODE : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: Continuous software force mode for PWM2B. 0: disabled  1: low
  2: high  3: disabled*/
#define MCPWM_GEN2_B_CNTUFORCE_MODE  0x00000003
#define MCPWM_GEN2_B_CNTUFORCE_MODE_M  ((MCPWM_GEN2_B_CNTUFORCE_MODE_V)<<(MCPWM_GEN2_B_CNTUFORCE_MODE_S))
#define MCPWM_GEN2_B_CNTUFORCE_MODE_V  0x3
#define MCPWM_GEN2_B_CNTUFORCE_MODE_S  8
/* MCPWM_GEN2_A_CNTUFORCE_MODE : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: Continuous software force mode for PWM2A. 0: disabled  1: low
  2: high  3: disabled*/
#define MCPWM_GEN2_A_CNTUFORCE_MODE  0x00000003
#define MCPWM_GEN2_A_CNTUFORCE_MODE_M  ((MCPWM_GEN2_A_CNTUFORCE_MODE_V)<<(MCPWM_GEN2_A_CNTUFORCE_MODE_S))
#define MCPWM_GEN2_A_CNTUFORCE_MODE_V  0x3
#define MCPWM_GEN2_A_CNTUFORCE_MODE_S  6
/* MCPWM_GEN2_CNTUFORCE_UPMETHOD : R/W ;bitpos:[5:0] ;default: 6'h20 ; */
/*description: Update method for continuous software force of PWM generator2.
 0: immediate  bit0: TEZ  bit1: TEP  bit2: TEA  bit3: TEB  bit4: sync  bit5: disable update. (TEA/B here and below means an event generated when timer value equals A/B register)*/
#define MCPWM_GEN2_CNTUFORCE_UPMETHOD  0x0000003F
#define MCPWM_GEN2_CNTUFORCE_UPMETHOD_M  ((MCPWM_GEN2_CNTUFORCE_UPMETHOD_V)<<(MCPWM_GEN2_CNTUFORCE_UPMETHOD_S))
#define MCPWM_GEN2_CNTUFORCE_UPMETHOD_V  0x3F
#define MCPWM_GEN2_CNTUFORCE_UPMETHOD_S  0

#define MCPWM_GEN2_A_REG(i)          (REG_MCPWM_BASE(i) + 0x00c0)
/* MCPWM_GEN2_A_DT1 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: Action on PWM2A triggered by event_t1 when timer decreasing.
 0: no change  1: low  2: high  3: toggle*/
#define MCPWM_GEN2_A_DT1  0x00000003
#define MCPWM_GEN2_A_DT1_M  ((MCPWM_GEN2_A_DT1_V)<<(MCPWM_GEN2_A_DT1_S))
#define MCPWM_GEN2_A_DT1_V  0x3
#define MCPWM_GEN2_A_DT1_S  22
/* MCPWM_GEN2_A_DT0 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: Action on PWM2A triggered by event_t0 when timer decreasing*/
#define MCPWM_GEN2_A_DT0  0x00000003
#define MCPWM_GEN2_A_DT0_M  ((MCPWM_GEN2_A_DT0_V)<<(MCPWM_GEN2_A_DT0_S))
#define MCPWM_GEN2_A_DT0_V  0x3
#define MCPWM_GEN2_A_DT0_S  20
/* MCPWM_GEN2_A_DTEB : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: Action on PWM2A triggered by event TEB when timer decreasing*/
#define MCPWM_GEN2_A_DTEB  0x00000003
#define MCPWM_GEN2_A_DTEB_M  ((MCPWM_GEN2_A_DTEB_V)<<(MCPWM_GEN2_A_DTEB_S))
#define MCPWM_GEN2_A_DTEB_V  0x3
#define MCPWM_GEN2_A_DTEB_S  18
/* MCPWM_GEN2_A_DTEA : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: Action on PWM2A triggered by event TEA when timer decreasing*/
#define MCPWM_GEN2_A_DTEA  0x00000003
#define MCPWM_GEN2_A_DTEA_M  ((MCPWM_GEN2_A_DTEA_V)<<(MCPWM_GEN2_A_DTEA_S))
#define MCPWM_GEN2_A_DTEA_V  0x3
#define MCPWM_GEN2_A_DTEA_S  16
/* MCPWM_GEN2_A_DTEP : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: Action on PWM2A triggered by event TEP when timer decreasing*/
#define MCPWM_GEN2_A_DTEP  0x00000003
#define MCPWM_GEN2_A_DTEP_M  ((MCPWM_GEN2_A_DTEP_V)<<(MCPWM_GEN2_A_DTEP_S))
#define MCPWM_GEN2_A_DTEP_V  0x3
#define MCPWM_GEN2_A_DTEP_S  14
/* MCPWM_GEN2_A_DTEZ : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: Action on PWM2A triggered by event TEZ when timer decreasing*/
#define MCPWM_GEN2_A_DTEZ  0x00000003
#define MCPWM_GEN2_A_DTEZ_M  ((MCPWM_GEN2_A_DTEZ_V)<<(MCPWM_GEN2_A_DTEZ_S))
#define MCPWM_GEN2_A_DTEZ_V  0x3
#define MCPWM_GEN2_A_DTEZ_S  12
/* MCPWM_GEN2_A_UT1 : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: Action on PWM2A triggered by event_t1 when timer increasing*/
#define MCPWM_GEN2_A_UT1  0x00000003
#define MCPWM_GEN2_A_UT1_M  ((MCPWM_GEN2_A_UT1_V)<<(MCPWM_GEN2_A_UT1_S))
#define MCPWM_GEN2_A_UT1_V  0x3
#define MCPWM_GEN2_A_UT1_S  10
/* MCPWM_GEN2_A_UT0 : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: Action on PWM2A triggered by event_t0 when timer increasing*/
#define MCPWM_GEN2_A_UT0  0x00000003
#define MCPWM_GEN2_A_UT0_M  ((MCPWM_GEN2_A_UT0_V)<<(MCPWM_GEN2_A_UT0_S))
#define MCPWM_GEN2_A_UT0_V  0x3
#define MCPWM_GEN2_A_UT0_S  8
/* MCPWM_GEN2_A_UTEB : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: Action on PWM2A triggered by event TEB when timer increasing*/
#define MCPWM_GEN2_A_UTEB  0x00000003
#define MCPWM_GEN2_A_UTEB_M  ((MCPWM_GEN2_A_UTEB_V)<<(MCPWM_GEN2_A_UTEB_S))
#define MCPWM_GEN2_A_UTEB_V  0x3
#define MCPWM_GEN2_A_UTEB_S  6
/* MCPWM_GEN2_A_UTEA : R/W ;bitpos:[5:4] ;default: 2'd0 ; */
/*description: Action on PWM2A triggered by event TEA when timer increasing*/
#define MCPWM_GEN2_A_UTEA  0x00000003
#define MCPWM_GEN2_A_UTEA_M  ((MCPWM_GEN2_A_UTEA_V)<<(MCPWM_GEN2_A_UTEA_S))
#define MCPWM_GEN2_A_UTEA_V  0x3
#define MCPWM_GEN2_A_UTEA_S  4
/* MCPWM_GEN2_A_UTEP : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: Action on PWM2A triggered by event TEP when timer increasing*/
#define MCPWM_GEN2_A_UTEP  0x00000003
#define MCPWM_GEN2_A_UTEP_M  ((MCPWM_GEN2_A_UTEP_V)<<(MCPWM_GEN2_A_UTEP_S))
#define MCPWM_GEN2_A_UTEP_V  0x3
#define MCPWM_GEN2_A_UTEP_S  2
/* MCPWM_GEN2_A_UTEZ : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: Action on PWM2A triggered by event TEZ when timer increasing*/
#define MCPWM_GEN2_A_UTEZ  0x00000003
#define MCPWM_GEN2_A_UTEZ_M  ((MCPWM_GEN2_A_UTEZ_V)<<(MCPWM_GEN2_A_UTEZ_S))
#define MCPWM_GEN2_A_UTEZ_V  0x3
#define MCPWM_GEN2_A_UTEZ_S  0

#define MCPWM_GEN2_B_REG(i)          (REG_MCPWM_BASE(i) + 0x00c4)
/* MCPWM_GEN2_B_DT1 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: Action on PWM2B triggered by event_t1 when timer decreasing.
 0: no change  1: low  2: high  3: toggle*/
#define MCPWM_GEN2_B_DT1  0x00000003
#define MCPWM_GEN2_B_DT1_M  ((MCPWM_GEN2_B_DT1_V)<<(MCPWM_GEN2_B_DT1_S))
#define MCPWM_GEN2_B_DT1_V  0x3
#define MCPWM_GEN2_B_DT1_S  22
/* MCPWM_GEN2_B_DT0 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: Action on PWM2B triggered by event_t0 when timer decreasing*/
#define MCPWM_GEN2_B_DT0  0x00000003
#define MCPWM_GEN2_B_DT0_M  ((MCPWM_GEN2_B_DT0_V)<<(MCPWM_GEN2_B_DT0_S))
#define MCPWM_GEN2_B_DT0_V  0x3
#define MCPWM_GEN2_B_DT0_S  20
/* MCPWM_GEN2_B_DTEB : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: Action on PWM2B triggered by event TEB when timer decreasing*/
#define MCPWM_GEN2_B_DTEB  0x00000003
#define MCPWM_GEN2_B_DTEB_M  ((MCPWM_GEN2_B_DTEB_V)<<(MCPWM_GEN2_B_DTEB_S))
#define MCPWM_GEN2_B_DTEB_V  0x3
#define MCPWM_GEN2_B_DTEB_S  18
/* MCPWM_GEN2_B_DTEA : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: Action on PWM2B triggered by event TEA when timer decreasing*/
#define MCPWM_GEN2_B_DTEA  0x00000003
#define MCPWM_GEN2_B_DTEA_M  ((MCPWM_GEN2_B_DTEA_V)<<(MCPWM_GEN2_B_DTEA_S))
#define MCPWM_GEN2_B_DTEA_V  0x3
#define MCPWM_GEN2_B_DTEA_S  16
/* MCPWM_GEN2_B_DTEP : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: Action on PWM2B triggered by event TEP when timer decreasing*/
#define MCPWM_GEN2_B_DTEP  0x00000003
#define MCPWM_GEN2_B_DTEP_M  ((MCPWM_GEN2_B_DTEP_V)<<(MCPWM_GEN2_B_DTEP_S))
#define MCPWM_GEN2_B_DTEP_V  0x3
#define MCPWM_GEN2_B_DTEP_S  14
/* MCPWM_GEN2_B_DTEZ : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: Action on PWM2B triggered by event TEZ when timer decreasing*/
#define MCPWM_GEN2_B_DTEZ  0x00000003
#define MCPWM_GEN2_B_DTEZ_M  ((MCPWM_GEN2_B_DTEZ_V)<<(MCPWM_GEN2_B_DTEZ_S))
#define MCPWM_GEN2_B_DTEZ_V  0x3
#define MCPWM_GEN2_B_DTEZ_S  12
/* MCPWM_GEN2_B_UT1 : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: Action on PWM2B triggered by event_t1 when timer increasing*/
#define MCPWM_GEN2_B_UT1  0x00000003
#define MCPWM_GEN2_B_UT1_M  ((MCPWM_GEN2_B_UT1_V)<<(MCPWM_GEN2_B_UT1_S))
#define MCPWM_GEN2_B_UT1_V  0x3
#define MCPWM_GEN2_B_UT1_S  10
/* MCPWM_GEN2_B_UT0 : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: Action on PWM2B triggered by event_t0 when timer increasing*/
#define MCPWM_GEN2_B_UT0  0x00000003
#define MCPWM_GEN2_B_UT0_M  ((MCPWM_GEN2_B_UT0_V)<<(MCPWM_GEN2_B_UT0_S))
#define MCPWM_GEN2_B_UT0_V  0x3
#define MCPWM_GEN2_B_UT0_S  8
/* MCPWM_GEN2_B_UTEB : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: Action on PWM2B triggered by event TEB when timer increasing*/
#define MCPWM_GEN2_B_UTEB  0x00000003
#define MCPWM_GEN2_B_UTEB_M  ((MCPWM_GEN2_B_UTEB_V)<<(MCPWM_GEN2_B_UTEB_S))
#define MCPWM_GEN2_B_UTEB_V  0x3
#define MCPWM_GEN2_B_UTEB_S  6
/* MCPWM_GEN2_B_UTEA : R/W ;bitpos:[5:4] ;default: 2'd0 ; */
/*description: Action on PWM2B triggered by event TEA when timer increasing*/
#define MCPWM_GEN2_B_UTEA  0x00000003
#define MCPWM_GEN2_B_UTEA_M  ((MCPWM_GEN2_B_UTEA_V)<<(MCPWM_GEN2_B_UTEA_S))
#define MCPWM_GEN2_B_UTEA_V  0x3
#define MCPWM_GEN2_B_UTEA_S  4
/* MCPWM_GEN2_B_UTEP : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: Action on PWM2B triggered by event TEP when timer increasing*/
#define MCPWM_GEN2_B_UTEP  0x00000003
#define MCPWM_GEN2_B_UTEP_M  ((MCPWM_GEN2_B_UTEP_V)<<(MCPWM_GEN2_B_UTEP_S))
#define MCPWM_GEN2_B_UTEP_V  0x3
#define MCPWM_GEN2_B_UTEP_S  2
/* MCPWM_GEN2_B_UTEZ : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: Action on PWM2B triggered by event TEZ when timer increasing*/
#define MCPWM_GEN2_B_UTEZ  0x00000003
#define MCPWM_GEN2_B_UTEZ_M  ((MCPWM_GEN2_B_UTEZ_V)<<(MCPWM_GEN2_B_UTEZ_S))
#define MCPWM_GEN2_B_UTEZ_V  0x3
#define MCPWM_GEN2_B_UTEZ_S  0

#define MCPWM_DT2_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x00c8)
/* MCPWM_DT2_CLK_SEL : R/W ;bitpos:[17] ;default: 1'd0 ; */
/*description: Dead time generator 1 clock selection. 0: PWM_clk  1: PT_clk*/
#define MCPWM_DT2_CLK_SEL  (BIT(17))
#define MCPWM_DT2_CLK_SEL_M  (BIT(17))
#define MCPWM_DT2_CLK_SEL_V  0x1
#define MCPWM_DT2_CLK_SEL_S  17
/* MCPWM_DT2_B_OUTBYPASS : R/W ;bitpos:[16] ;default: 1'd1 ; */
/*description: S0 in documentation*/
#define MCPWM_DT2_B_OUTBYPASS  (BIT(16))
#define MCPWM_DT2_B_OUTBYPASS_M  (BIT(16))
#define MCPWM_DT2_B_OUTBYPASS_V  0x1
#define MCPWM_DT2_B_OUTBYPASS_S  16
/* MCPWM_DT2_A_OUTBYPASS : R/W ;bitpos:[15] ;default: 1'd1 ; */
/*description: S1 in documentation*/
#define MCPWM_DT2_A_OUTBYPASS  (BIT(15))
#define MCPWM_DT2_A_OUTBYPASS_M  (BIT(15))
#define MCPWM_DT2_A_OUTBYPASS_V  0x1
#define MCPWM_DT2_A_OUTBYPASS_S  15
/* MCPWM_DT2_FED_OUTINVERT : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: S3 in documentation*/
#define MCPWM_DT2_FED_OUTINVERT  (BIT(14))
#define MCPWM_DT2_FED_OUTINVERT_M  (BIT(14))
#define MCPWM_DT2_FED_OUTINVERT_V  0x1
#define MCPWM_DT2_FED_OUTINVERT_S  14
/* MCPWM_DT2_RED_OUTINVERT : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: S2 in documentation*/
#define MCPWM_DT2_RED_OUTINVERT  (BIT(13))
#define MCPWM_DT2_RED_OUTINVERT_M  (BIT(13))
#define MCPWM_DT2_RED_OUTINVERT_V  0x1
#define MCPWM_DT2_RED_OUTINVERT_S  13
/* MCPWM_DT2_FED_INSEL : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: S5 in documentation*/
#define MCPWM_DT2_FED_INSEL  (BIT(12))
#define MCPWM_DT2_FED_INSEL_M  (BIT(12))
#define MCPWM_DT2_FED_INSEL_V  0x1
#define MCPWM_DT2_FED_INSEL_S  12
/* MCPWM_DT2_RED_INSEL : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: S4 in documentation*/
#define MCPWM_DT2_RED_INSEL  (BIT(11))
#define MCPWM_DT2_RED_INSEL_M  (BIT(11))
#define MCPWM_DT2_RED_INSEL_V  0x1
#define MCPWM_DT2_RED_INSEL_S  11
/* MCPWM_DT2_B_OUTSWAP : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: S7 in documentation*/
#define MCPWM_DT2_B_OUTSWAP  (BIT(10))
#define MCPWM_DT2_B_OUTSWAP_M  (BIT(10))
#define MCPWM_DT2_B_OUTSWAP_V  0x1
#define MCPWM_DT2_B_OUTSWAP_S  10
/* MCPWM_DT2_A_OUTSWAP : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: S6 in documentation*/
#define MCPWM_DT2_A_OUTSWAP  (BIT(9))
#define MCPWM_DT2_A_OUTSWAP_M  (BIT(9))
#define MCPWM_DT2_A_OUTSWAP_V  0x1
#define MCPWM_DT2_A_OUTSWAP_S  9
/* MCPWM_DT2_DEB_MODE : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: S8 in documentation  dual-edge B mode  0: FED/RED take effect
 on different path separately  1: FED/RED take effect on B path  A out is in bypass or normal operation mode*/
#define MCPWM_DT2_DEB_MODE  (BIT(8))
#define MCPWM_DT2_DEB_MODE_M  (BIT(8))
#define MCPWM_DT2_DEB_MODE_V  0x1
#define MCPWM_DT2_DEB_MODE_S  8
/* MCPWM_DT2_RED_UPMETHOD : R/W ;bitpos:[7:4] ;default: 4'd0 ; */
/*description: Update method for RED (rising edge delay) active reg. 0: immediate
  bit0: TEZ  bit1: TEP  bit2: sync  bit3: disable update*/
#define MCPWM_DT2_RED_UPMETHOD  0x0000000F
#define MCPWM_DT2_RED_UPMETHOD_M  ((MCPWM_DT2_RED_UPMETHOD_V)<<(MCPWM_DT2_RED_UPMETHOD_S))
#define MCPWM_DT2_RED_UPMETHOD_V  0xF
#define MCPWM_DT2_RED_UPMETHOD_S  4
/* MCPWM_DT2_FED_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: Update method for FED (falling edge delay) active reg. 0: immediate
  bit0: TEZ  bit1: TEP  bit2: sync  bit3: disable update*/
#define MCPWM_DT2_FED_UPMETHOD  0x0000000F
#define MCPWM_DT2_FED_UPMETHOD_M  ((MCPWM_DT2_FED_UPMETHOD_V)<<(MCPWM_DT2_FED_UPMETHOD_S))
#define MCPWM_DT2_FED_UPMETHOD_V  0xF
#define MCPWM_DT2_FED_UPMETHOD_S  0

#define MCPWM_DT2_FED_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x00cc)
/* MCPWM_DT2_FED : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: Shadow reg for FED*/
#define MCPWM_DT2_FED  0x0000FFFF
#define MCPWM_DT2_FED_M  ((MCPWM_DT2_FED_V)<<(MCPWM_DT2_FED_S))
#define MCPWM_DT2_FED_V  0xFFFF
#define MCPWM_DT2_FED_S  0

#define MCPWM_DT2_RED_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x00d0)
/* MCPWM_DT2_RED : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: Shadow reg for RED*/
#define MCPWM_DT2_RED  0x0000FFFF
#define MCPWM_DT2_RED_M  ((MCPWM_DT2_RED_V)<<(MCPWM_DT2_RED_S))
#define MCPWM_DT2_RED_V  0xFFFF
#define MCPWM_DT2_RED_S  0

#define MCPWM_CARRIER2_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x00d4)
/* MCPWM_CARRIER2_IN_INVERT : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: When set  invert the input of PWM2A and PWM2B for this submodule*/
#define MCPWM_CARRIER2_IN_INVERT  (BIT(13))
#define MCPWM_CARRIER2_IN_INVERT_M  (BIT(13))
#define MCPWM_CARRIER2_IN_INVERT_V  0x1
#define MCPWM_CARRIER2_IN_INVERT_S  13
/* MCPWM_CARRIER2_OUT_INVERT : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: When set  invert the output of PWM2A and PWM2B for this submodule*/
#define MCPWM_CARRIER2_OUT_INVERT  (BIT(12))
#define MCPWM_CARRIER2_OUT_INVERT_M  (BIT(12))
#define MCPWM_CARRIER2_OUT_INVERT_V  0x1
#define MCPWM_CARRIER2_OUT_INVERT_S  12
/* MCPWM_CARRIER2_OSHWTH : R/W ;bitpos:[11:8] ;default: 4'd0 ; */
/*description: Width of the fist pulse in number of periods of the carrier*/
#define MCPWM_CARRIER2_OSHWTH  0x0000000F
#define MCPWM_CARRIER2_OSHWTH_M  ((MCPWM_CARRIER2_OSHWTH_V)<<(MCPWM_CARRIER2_OSHWTH_S))
#define MCPWM_CARRIER2_OSHWTH_V  0xF
#define MCPWM_CARRIER2_OSHWTH_S  8
/* MCPWM_CARRIER2_DUTY : R/W ;bitpos:[7:5] ;default: 3'd0 ; */
/*description: Carrier duty selection. Duty = PWM_CARRIER2_DUTY / 8*/
#define MCPWM_CARRIER2_DUTY  0x00000007
#define MCPWM_CARRIER2_DUTY_M  ((MCPWM_CARRIER2_DUTY_V)<<(MCPWM_CARRIER2_DUTY_S))
#define MCPWM_CARRIER2_DUTY_V  0x7
#define MCPWM_CARRIER2_DUTY_S  5
/* MCPWM_CARRIER2_PRESCALE : R/W ;bitpos:[4:1] ;default: 4'd0 ; */
/*description: PWM carrier2 clock (PC_clk) prescale value. Period of PC_clk
 = period of PWM_clk * (PWM_CARRIER2_PRESCALE + 1)*/
#define MCPWM_CARRIER2_PRESCALE  0x0000000F
#define MCPWM_CARRIER2_PRESCALE_M  ((MCPWM_CARRIER2_PRESCALE_V)<<(MCPWM_CARRIER2_PRESCALE_S))
#define MCPWM_CARRIER2_PRESCALE_V  0xF
#define MCPWM_CARRIER2_PRESCALE_S  1
/* MCPWM_CARRIER2_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: When set  carrier2 function is enabled. When cleared  carrier2 is bypassed*/
#define MCPWM_CARRIER2_EN  (BIT(0))
#define MCPWM_CARRIER2_EN_M  (BIT(0))
#define MCPWM_CARRIER2_EN_V  0x1
#define MCPWM_CARRIER2_EN_S  0

#define MCPWM_FH2_CFG0_REG(i)          (REG_MCPWM_BASE(i) + 0x00d8)
/* MCPWM_FH2_B_OST_U : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: One-shot mode action on PWM2B when fault event occurs and timer
 is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH2_B_OST_U  0x00000003
#define MCPWM_FH2_B_OST_U_M  ((MCPWM_FH2_B_OST_U_V)<<(MCPWM_FH2_B_OST_U_S))
#define MCPWM_FH2_B_OST_U_V  0x3
#define MCPWM_FH2_B_OST_U_S  22
/* MCPWM_FH2_B_OST_D : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: One-shot mode action on PWM2B when fault event occurs and timer
 is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH2_B_OST_D  0x00000003
#define MCPWM_FH2_B_OST_D_M  ((MCPWM_FH2_B_OST_D_V)<<(MCPWM_FH2_B_OST_D_S))
#define MCPWM_FH2_B_OST_D_V  0x3
#define MCPWM_FH2_B_OST_D_S  20
/* MCPWM_FH2_B_CBC_U : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: Cycle-by-cycle mode action on PWM2B when fault event occurs and
 timer is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH2_B_CBC_U  0x00000003
#define MCPWM_FH2_B_CBC_U_M  ((MCPWM_FH2_B_CBC_U_V)<<(MCPWM_FH2_B_CBC_U_S))
#define MCPWM_FH2_B_CBC_U_V  0x3
#define MCPWM_FH2_B_CBC_U_S  18
/* MCPWM_FH2_B_CBC_D : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: Cycle-by-cycle mode action on PWM2B when fault event occurs and
 timer is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH2_B_CBC_D  0x00000003
#define MCPWM_FH2_B_CBC_D_M  ((MCPWM_FH2_B_CBC_D_V)<<(MCPWM_FH2_B_CBC_D_S))
#define MCPWM_FH2_B_CBC_D_V  0x3
#define MCPWM_FH2_B_CBC_D_S  16
/* MCPWM_FH2_A_OST_U : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: One-shot mode action on PWM2A when fault event occurs and timer
 is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH2_A_OST_U  0x00000003
#define MCPWM_FH2_A_OST_U_M  ((MCPWM_FH2_A_OST_U_V)<<(MCPWM_FH2_A_OST_U_S))
#define MCPWM_FH2_A_OST_U_V  0x3
#define MCPWM_FH2_A_OST_U_S  14
/* MCPWM_FH2_A_OST_D : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: One-shot mode action on PWM2A when fault event occurs and timer
 is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH2_A_OST_D  0x00000003
#define MCPWM_FH2_A_OST_D_M  ((MCPWM_FH2_A_OST_D_V)<<(MCPWM_FH2_A_OST_D_S))
#define MCPWM_FH2_A_OST_D_V  0x3
#define MCPWM_FH2_A_OST_D_S  12
/* MCPWM_FH2_A_CBC_U : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: Cycle-by-cycle mode action on PWM2A when fault event occurs and
 timer is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH2_A_CBC_U  0x00000003
#define MCPWM_FH2_A_CBC_U_M  ((MCPWM_FH2_A_CBC_U_V)<<(MCPWM_FH2_A_CBC_U_S))
#define MCPWM_FH2_A_CBC_U_V  0x3
#define MCPWM_FH2_A_CBC_U_S  10
/* MCPWM_FH2_A_CBC_D : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: Cycle-by-cycle mode action on PWM2A when fault event occurs and
 timer is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_FH2_A_CBC_D  0x00000003
#define MCPWM_FH2_A_CBC_D_M  ((MCPWM_FH2_A_CBC_D_V)<<(MCPWM_FH2_A_CBC_D_S))
#define MCPWM_FH2_A_CBC_D_V  0x3
#define MCPWM_FH2_A_CBC_D_S  8
/* MCPWM_FH2_F0_OST : R/W ;bitpos:[7] ;default: 1'd0 ; */
/*description: event_f0 will trigger one-shot mode action. 0: disable  1: enable*/
#define MCPWM_FH2_F0_OST  (BIT(7))
#define MCPWM_FH2_F0_OST_M  (BIT(7))
#define MCPWM_FH2_F0_OST_V  0x1
#define MCPWM_FH2_F0_OST_S  7
/* MCPWM_FH2_F1_OST : R/W ;bitpos:[6] ;default: 1'd0 ; */
/*description: event_f1 will trigger one-shot mode action. 0: disable  1: enable*/
#define MCPWM_FH2_F1_OST  (BIT(6))
#define MCPWM_FH2_F1_OST_M  (BIT(6))
#define MCPWM_FH2_F1_OST_V  0x1
#define MCPWM_FH2_F1_OST_S  6
/* MCPWM_FH2_F2_OST : R/W ;bitpos:[5] ;default: 1'd0 ; */
/*description: event_f2 will trigger one-shot mode action. 0: disable  1: enable*/
#define MCPWM_FH2_F2_OST  (BIT(5))
#define MCPWM_FH2_F2_OST_M  (BIT(5))
#define MCPWM_FH2_F2_OST_V  0x1
#define MCPWM_FH2_F2_OST_S  5
/* MCPWM_FH2_SW_OST : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: Enable register for software force one-shot mode action. 0: disable  1: enable*/
#define MCPWM_FH2_SW_OST  (BIT(4))
#define MCPWM_FH2_SW_OST_M  (BIT(4))
#define MCPWM_FH2_SW_OST_V  0x1
#define MCPWM_FH2_SW_OST_S  4
/* MCPWM_FH2_F0_CBC : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: event_f0 will trigger cycle-by-cycle mode action. 0: disable  1: enable*/
#define MCPWM_FH2_F0_CBC  (BIT(3))
#define MCPWM_FH2_F0_CBC_M  (BIT(3))
#define MCPWM_FH2_F0_CBC_V  0x1
#define MCPWM_FH2_F0_CBC_S  3
/* MCPWM_FH2_F1_CBC : R/W ;bitpos:[2] ;default: 1'd0 ; */
/*description: event_f1 will trigger cycle-by-cycle mode action. 0: disable  1: enable*/
#define MCPWM_FH2_F1_CBC  (BIT(2))
#define MCPWM_FH2_F1_CBC_M  (BIT(2))
#define MCPWM_FH2_F1_CBC_V  0x1
#define MCPWM_FH2_F1_CBC_S  2
/* MCPWM_FH2_F2_CBC : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: event_f2 will trigger cycle-by-cycle mode action. 0: disable  1: enable*/
#define MCPWM_FH2_F2_CBC  (BIT(1))
#define MCPWM_FH2_F2_CBC_M  (BIT(1))
#define MCPWM_FH2_F2_CBC_V  0x1
#define MCPWM_FH2_F2_CBC_S  1
/* MCPWM_FH2_SW_CBC : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: Enable register for software force cycle-by-cycle mode action.
 0: disable  1: enable*/
#define MCPWM_FH2_SW_CBC  (BIT(0))
#define MCPWM_FH2_SW_CBC_M  (BIT(0))
#define MCPWM_FH2_SW_CBC_V  0x1
#define MCPWM_FH2_SW_CBC_S  0

#define MCPWM_FH2_CFG1_REG(i)          (REG_MCPWM_BASE(i) + 0x00dc)
/* MCPWM_FH2_FORCE_OST : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: A toggle (software negation of value of this bit) triggers a
 one-shot mode action*/
#define MCPWM_FH2_FORCE_OST  (BIT(4))
#define MCPWM_FH2_FORCE_OST_M  (BIT(4))
#define MCPWM_FH2_FORCE_OST_V  0x1
#define MCPWM_FH2_FORCE_OST_S  4
/* MCPWM_FH2_FORCE_CBC : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: A toggle triggers a cycle-by-cycle mode action*/
#define MCPWM_FH2_FORCE_CBC  (BIT(3))
#define MCPWM_FH2_FORCE_CBC_M  (BIT(3))
#define MCPWM_FH2_FORCE_CBC_V  0x1
#define MCPWM_FH2_FORCE_CBC_S  3
/* MCPWM_FH2_CBCPULSE : R/W ;bitpos:[2:1] ;default: 2'd0 ; */
/*description: The cycle-by-cycle mode action refresh moment selection. Bit0: TEZ  bit1:TEP*/
#define MCPWM_FH2_CBCPULSE  0x00000003
#define MCPWM_FH2_CBCPULSE_M  ((MCPWM_FH2_CBCPULSE_V)<<(MCPWM_FH2_CBCPULSE_S))
#define MCPWM_FH2_CBCPULSE_V  0x3
#define MCPWM_FH2_CBCPULSE_S  1
/* MCPWM_FH2_CLR_OST : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: A toggle will clear on going one-shot mode action*/
#define MCPWM_FH2_CLR_OST  (BIT(0))
#define MCPWM_FH2_CLR_OST_M  (BIT(0))
#define MCPWM_FH2_CLR_OST_V  0x1
#define MCPWM_FH2_CLR_OST_S  0

#define MCPWM_FH2_STATUS_REG(i)          (REG_MCPWM_BASE(i) + 0x00e0)
/* MCPWM_FH2_OST_ON : RO ;bitpos:[1] ;default: 1'd0 ; */
/*description: Set and reset by hardware. If set  an one-shot mode action is on going*/
#define MCPWM_FH2_OST_ON  (BIT(1))
#define MCPWM_FH2_OST_ON_M  (BIT(1))
#define MCPWM_FH2_OST_ON_V  0x1
#define MCPWM_FH2_OST_ON_S  1
/* MCPWM_FH2_CBC_ON : RO ;bitpos:[0] ;default: 1'd0 ; */
/*description: Set and reset by hardware. If set  an cycle-by-cycle mode action is on going*/
#define MCPWM_FH2_CBC_ON  (BIT(0))
#define MCPWM_FH2_CBC_ON_M  (BIT(0))
#define MCPWM_FH2_CBC_ON_V  0x1
#define MCPWM_FH2_CBC_ON_S  0

#define MCPWM_FAULT_DETECT_REG(i)          (REG_MCPWM_BASE(i) + 0x00e4)
/* MCPWM_EVENT_F2 : RO ;bitpos:[8] ;default: 1'd0 ; */
/*description: Set and reset by hardware. If set  event_f2 is on going*/
#define MCPWM_EVENT_F2  (BIT(8))
#define MCPWM_EVENT_F2_M  (BIT(8))
#define MCPWM_EVENT_F2_V  0x1
#define MCPWM_EVENT_F2_S  8
/* MCPWM_EVENT_F1 : RO ;bitpos:[7] ;default: 1'd0 ; */
/*description: Set and reset by hardware. If set  event_f1 is on going*/
#define MCPWM_EVENT_F1  (BIT(7))
#define MCPWM_EVENT_F1_M  (BIT(7))
#define MCPWM_EVENT_F1_V  0x1
#define MCPWM_EVENT_F1_S  7
/* MCPWM_EVENT_F0 : RO ;bitpos:[6] ;default: 1'd0 ; */
/*description: Set and reset by hardware. If set  event_f0 is on going*/
#define MCPWM_EVENT_F0  (BIT(6))
#define MCPWM_EVENT_F0_M  (BIT(6))
#define MCPWM_EVENT_F0_V  0x1
#define MCPWM_EVENT_F0_S  6
/* MCPWM_F2_POLE : R/W ;bitpos:[5] ;default: 1'd0 ; */
/*description: Set event_f2 trigger polarity on FAULT2 source from GPIO matrix.
 0: level low  1: level high*/
#define MCPWM_F2_POLE  (BIT(5))
#define MCPWM_F2_POLE_M  (BIT(5))
#define MCPWM_F2_POLE_V  0x1
#define MCPWM_F2_POLE_S  5
/* MCPWM_F1_POLE : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: Set event_f1 trigger polarity on FAULT2 source from GPIO matrix.
 0: level low  1: level high*/
#define MCPWM_F1_POLE  (BIT(4))
#define MCPWM_F1_POLE_M  (BIT(4))
#define MCPWM_F1_POLE_V  0x1
#define MCPWM_F1_POLE_S  4
/* MCPWM_F0_POLE : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: Set event_f0 trigger polarity on FAULT2 source from GPIO matrix.
 0: level low  1: level high*/
#define MCPWM_F0_POLE  (BIT(3))
#define MCPWM_F0_POLE_M  (BIT(3))
#define MCPWM_F0_POLE_V  0x1
#define MCPWM_F0_POLE_S  3
/* MCPWM_F2_EN : R/W ;bitpos:[2] ;default: 1'd0 ; */
/*description: Set to enable generation of event_f2*/
#define MCPWM_F2_EN  (BIT(2))
#define MCPWM_F2_EN_M  (BIT(2))
#define MCPWM_F2_EN_V  0x1
#define MCPWM_F2_EN_S  2
/* MCPWM_F1_EN : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: Set to enable generation of event_f1*/
#define MCPWM_F1_EN  (BIT(1))
#define MCPWM_F1_EN_M  (BIT(1))
#define MCPWM_F1_EN_V  0x1
#define MCPWM_F1_EN_S  1
/* MCPWM_F0_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: Set to enable generation of event_f0*/
#define MCPWM_F0_EN  (BIT(0))
#define MCPWM_F0_EN_M  (BIT(0))
#define MCPWM_F0_EN_V  0x1
#define MCPWM_F0_EN_S  0

#define MCPWM_CAP_TIMER_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x00e8)
/* MCPWM_CAP_SYNC_SW : WO ;bitpos:[5] ;default: 1'd0 ; */
/*description: Set this bit to force a capture timer sync  capture timer is
 loaded with value in phase register.*/
#define MCPWM_CAP_SYNC_SW  (BIT(5))
#define MCPWM_CAP_SYNC_SW_M  (BIT(5))
#define MCPWM_CAP_SYNC_SW_V  0x1
#define MCPWM_CAP_SYNC_SW_S  5
/* MCPWM_CAP_SYNCI_SEL : R/W ;bitpos:[4:2] ;default: 3'd0 ; */
/*description: Capture module sync input selection. 0: none  1: timer0 synco
  2: timer1 synco  3: timer2 synco  4: SYNC0 from GPIO matrix  5: SYNC1 from GPIO matrix  6: SYNC2 from GPIO matrix*/
#define MCPWM_CAP_SYNCI_SEL  0x00000007
#define MCPWM_CAP_SYNCI_SEL_M  ((MCPWM_CAP_SYNCI_SEL_V)<<(MCPWM_CAP_SYNCI_SEL_S))
#define MCPWM_CAP_SYNCI_SEL_V  0x7
#define MCPWM_CAP_SYNCI_SEL_S  2
/* MCPWM_CAP_SYNCI_EN : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: When set  capture timer sync is enabled.*/
#define MCPWM_CAP_SYNCI_EN  (BIT(1))
#define MCPWM_CAP_SYNCI_EN_M  (BIT(1))
#define MCPWM_CAP_SYNCI_EN_V  0x1
#define MCPWM_CAP_SYNCI_EN_S  1
/* MCPWM_CAP_TIMER_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: When set  capture timer incrementing under APB_clk is enabled.*/
#define MCPWM_CAP_TIMER_EN  (BIT(0))
#define MCPWM_CAP_TIMER_EN_M  (BIT(0))
#define MCPWM_CAP_TIMER_EN_V  0x1
#define MCPWM_CAP_TIMER_EN_S  0

#define MCPWM_CAP_TIMER_PHASE_REG(i)          (REG_MCPWM_BASE(i) + 0x00ec)
/* MCPWM_CAP_PHASE : R/W ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: Phase value for capture timer sync operation.*/
#define MCPWM_CAP_PHASE  0xFFFFFFFF
#define MCPWM_CAP_PHASE_M  ((MCPWM_CAP_PHASE_V)<<(MCPWM_CAP_PHASE_S))
#define MCPWM_CAP_PHASE_V  0xFFFFFFFF
#define MCPWM_CAP_PHASE_S  0

#define MCPWM_CAP_CH0_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x00f0)
/* MCPWM_CAP0_SW : WO ;bitpos:[12] ;default: 1'd0 ; */
/*description: Write 1 will trigger a software forced capture on channel 0*/
#define MCPWM_CAP0_SW  (BIT(12))
#define MCPWM_CAP0_SW_M  (BIT(12))
#define MCPWM_CAP0_SW_V  0x1
#define MCPWM_CAP0_SW_S  12
/* MCPWM_CAP0_IN_INVERT : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: When set  CAP0 form GPIO matrix is inverted before prescale*/
#define MCPWM_CAP0_IN_INVERT  (BIT(11))
#define MCPWM_CAP0_IN_INVERT_M  (BIT(11))
#define MCPWM_CAP0_IN_INVERT_V  0x1
#define MCPWM_CAP0_IN_INVERT_S  11
/* MCPWM_CAP0_PRESCALE : R/W ;bitpos:[10:3] ;default: 8'd0 ; */
/*description: Value of prescale on possitive edge of CAP0. Prescale value =
 PWM_CAP0_PRESCALE + 1*/
#define MCPWM_CAP0_PRESCALE  0x000000FF
#define MCPWM_CAP0_PRESCALE_M  ((MCPWM_CAP0_PRESCALE_V)<<(MCPWM_CAP0_PRESCALE_S))
#define MCPWM_CAP0_PRESCALE_V  0xFF
#define MCPWM_CAP0_PRESCALE_S  3
/* MCPWM_CAP0_MODE : R/W ;bitpos:[2:1] ;default: 2'd0 ; */
/*description: Edge of capture on channel 0 after prescale.  bit0: negedge cap
 en  bit1: posedge cap en*/
#define MCPWM_CAP0_MODE  0x00000003
#define MCPWM_CAP0_MODE_M  ((MCPWM_CAP0_MODE_V)<<(MCPWM_CAP0_MODE_S))
#define MCPWM_CAP0_MODE_V  0x3
#define MCPWM_CAP0_MODE_S  1
/* MCPWM_CAP0_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: When set  capture on channel 0 is enabled*/
#define MCPWM_CAP0_EN  (BIT(0))
#define MCPWM_CAP0_EN_M  (BIT(0))
#define MCPWM_CAP0_EN_V  0x1
#define MCPWM_CAP0_EN_S  0

#define MCPWM_CAP_CH1_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x00f4)
/* MCPWM_CAP1_SW : WO ;bitpos:[12] ;default: 1'd0 ; */
/*description: Write 1 will trigger a software forced capture on channel 1*/
#define MCPWM_CAP1_SW  (BIT(12))
#define MCPWM_CAP1_SW_M  (BIT(12))
#define MCPWM_CAP1_SW_V  0x1
#define MCPWM_CAP1_SW_S  12
/* MCPWM_CAP1_IN_INVERT : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: When set  CAP1 form GPIO matrix is inverted before prescale*/
#define MCPWM_CAP1_IN_INVERT  (BIT(11))
#define MCPWM_CAP1_IN_INVERT_M  (BIT(11))
#define MCPWM_CAP1_IN_INVERT_V  0x1
#define MCPWM_CAP1_IN_INVERT_S  11
/* MCPWM_CAP1_PRESCALE : R/W ;bitpos:[10:3] ;default: 8'd0 ; */
/*description: Value of prescale on possitive edge of CAP1. Prescale value =
 PWM_CAP1_PRESCALE + 1*/
#define MCPWM_CAP1_PRESCALE  0x000000FF
#define MCPWM_CAP1_PRESCALE_M  ((MCPWM_CAP1_PRESCALE_V)<<(MCPWM_CAP1_PRESCALE_S))
#define MCPWM_CAP1_PRESCALE_V  0xFF
#define MCPWM_CAP1_PRESCALE_S  3
/* MCPWM_CAP1_MODE : R/W ;bitpos:[2:1] ;default: 2'd0 ; */
/*description: Edge of capture on channel 1 after prescale.  bit0: negedge cap
 en  bit1: posedge cap en*/
#define MCPWM_CAP1_MODE  0x00000003
#define MCPWM_CAP1_MODE_M  ((MCPWM_CAP1_MODE_V)<<(MCPWM_CAP1_MODE_S))
#define MCPWM_CAP1_MODE_V  0x3
#define MCPWM_CAP1_MODE_S  1
/* MCPWM_CAP1_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: When set  capture on channel 1 is enabled*/
#define MCPWM_CAP1_EN  (BIT(0))
#define MCPWM_CAP1_EN_M  (BIT(0))
#define MCPWM_CAP1_EN_V  0x1
#define MCPWM_CAP1_EN_S  0

#define MCPWM_CAP_CH2_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x00f8)
/* MCPWM_CAP2_SW : WO ;bitpos:[12] ;default: 1'd0 ; */
/*description: Write 1 will trigger a software forced capture on channel 2*/
#define MCPWM_CAP2_SW  (BIT(12))
#define MCPWM_CAP2_SW_M  (BIT(12))
#define MCPWM_CAP2_SW_V  0x1
#define MCPWM_CAP2_SW_S  12
/* MCPWM_CAP2_IN_INVERT : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: When set  CAP2 form GPIO matrix is inverted before prescale*/
#define MCPWM_CAP2_IN_INVERT  (BIT(11))
#define MCPWM_CAP2_IN_INVERT_M  (BIT(11))
#define MCPWM_CAP2_IN_INVERT_V  0x1
#define MCPWM_CAP2_IN_INVERT_S  11
/* MCPWM_CAP2_PRESCALE : R/W ;bitpos:[10:3] ;default: 8'd0 ; */
/*description: Value of prescale on possitive edge of CAP2. Prescale value =
 PWM_CAP2_PRESCALE + 1*/
#define MCPWM_CAP2_PRESCALE  0x000000FF
#define MCPWM_CAP2_PRESCALE_M  ((MCPWM_CAP2_PRESCALE_V)<<(MCPWM_CAP2_PRESCALE_S))
#define MCPWM_CAP2_PRESCALE_V  0xFF
#define MCPWM_CAP2_PRESCALE_S  3
/* MCPWM_CAP2_MODE : R/W ;bitpos:[2:1] ;default: 2'd0 ; */
/*description: Edge of capture on channel 2 after prescale.  bit0: negedge cap
 en  bit1: posedge cap en*/
#define MCPWM_CAP2_MODE  0x00000003
#define MCPWM_CAP2_MODE_M  ((MCPWM_CAP2_MODE_V)<<(MCPWM_CAP2_MODE_S))
#define MCPWM_CAP2_MODE_V  0x3
#define MCPWM_CAP2_MODE_S  1
/* MCPWM_CAP2_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: When set  capture on channel 2 is enabled*/
#define MCPWM_CAP2_EN  (BIT(0))
#define MCPWM_CAP2_EN_M  (BIT(0))
#define MCPWM_CAP2_EN_V  0x1
#define MCPWM_CAP2_EN_S  0

#define MCPWM_CAP_CH0_REG(i)          (REG_MCPWM_BASE(i) + 0x00fc)
/* MCPWM_CAP0_VALUE : RO ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: Value of last capture on channel 0*/
#define MCPWM_CAP0_VALUE  0xFFFFFFFF
#define MCPWM_CAP0_VALUE_M  ((MCPWM_CAP0_VALUE_V)<<(MCPWM_CAP0_VALUE_S))
#define MCPWM_CAP0_VALUE_V  0xFFFFFFFF
#define MCPWM_CAP0_VALUE_S  0

#define MCPWM_CAP_CH1_REG(i)          (REG_MCPWM_BASE(i) + 0x0100)
/* MCPWM_CAP1_VALUE : RO ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: Value of last capture on channel 1*/
#define MCPWM_CAP1_VALUE  0xFFFFFFFF
#define MCPWM_CAP1_VALUE_M  ((MCPWM_CAP1_VALUE_V)<<(MCPWM_CAP1_VALUE_S))
#define MCPWM_CAP1_VALUE_V  0xFFFFFFFF
#define MCPWM_CAP1_VALUE_S  0

#define MCPWM_CAP_CH2_REG(i)          (REG_MCPWM_BASE(i) + 0x0104)
/* MCPWM_CAP2_VALUE : RO ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: Value of last capture on channel 2*/
#define MCPWM_CAP2_VALUE  0xFFFFFFFF
#define MCPWM_CAP2_VALUE_M  ((MCPWM_CAP2_VALUE_V)<<(MCPWM_CAP2_VALUE_S))
#define MCPWM_CAP2_VALUE_V  0xFFFFFFFF
#define MCPWM_CAP2_VALUE_S  0

#define MCPWM_CAP_STATUS_REG(i)          (REG_MCPWM_BASE(i) + 0x0108)
/* MCPWM_CAP2_EDGE : RO ;bitpos:[2] ;default: 1'd0 ; */
/*description: Edge of last capture trigger on channel 2  0: posedge  1: negedge*/
#define MCPWM_CAP2_EDGE  (BIT(2))
#define MCPWM_CAP2_EDGE_M  (BIT(2))
#define MCPWM_CAP2_EDGE_V  0x1
#define MCPWM_CAP2_EDGE_S  2
/* MCPWM_CAP1_EDGE : RO ;bitpos:[1] ;default: 1'd0 ; */
/*description: Edge of last capture trigger on channel 1  0: posedge  1: negedge*/
#define MCPWM_CAP1_EDGE  (BIT(1))
#define MCPWM_CAP1_EDGE_M  (BIT(1))
#define MCPWM_CAP1_EDGE_V  0x1
#define MCPWM_CAP1_EDGE_S  1
/* MCPWM_CAP0_EDGE : RO ;bitpos:[0] ;default: 1'd0 ; */
/*description: Edge of last capture trigger on channel 0  0: posedge  1: negedge*/
#define MCPWM_CAP0_EDGE  (BIT(0))
#define MCPWM_CAP0_EDGE_M  (BIT(0))
#define MCPWM_CAP0_EDGE_V  0x1
#define MCPWM_CAP0_EDGE_S  0

#define MCPWM_UPDATE_CFG_REG(i)          (REG_MCPWM_BASE(i) + 0x010c)
/* MCPWM_OP2_FORCE_UP : R/W ;bitpos:[7] ;default: 1'd0 ; */
/*description: A toggle (software negation of value of this bit) will trigger
 a forced update of active registers in PWM operator 2*/
#define MCPWM_OP2_FORCE_UP  (BIT(7))
#define MCPWM_OP2_FORCE_UP_M  (BIT(7))
#define MCPWM_OP2_FORCE_UP_V  0x1
#define MCPWM_OP2_FORCE_UP_S  7
/* MCPWM_OP2_UP_EN : R/W ;bitpos:[6] ;default: 1'd1 ; */
/*description: When set and PWM_GLOBAL_UP_EN is set  update of active registers
 in PWM operator 2 are enabled*/
#define MCPWM_OP2_UP_EN  (BIT(6))
#define MCPWM_OP2_UP_EN_M  (BIT(6))
#define MCPWM_OP2_UP_EN_V  0x1
#define MCPWM_OP2_UP_EN_S  6
/* MCPWM_OP1_FORCE_UP : R/W ;bitpos:[5] ;default: 1'd0 ; */
/*description: A toggle (software negation of value of this bit) will trigger
 a forced update of active registers in PWM operator 1*/
#define MCPWM_OP1_FORCE_UP  (BIT(5))
#define MCPWM_OP1_FORCE_UP_M  (BIT(5))
#define MCPWM_OP1_FORCE_UP_V  0x1
#define MCPWM_OP1_FORCE_UP_S  5
/* MCPWM_OP1_UP_EN : R/W ;bitpos:[4] ;default: 1'd1 ; */
/*description: When set and PWM_GLOBAL_UP_EN is set  update of active registers
 in PWM operator 1 are enabled*/
#define MCPWM_OP1_UP_EN  (BIT(4))
#define MCPWM_OP1_UP_EN_M  (BIT(4))
#define MCPWM_OP1_UP_EN_V  0x1
#define MCPWM_OP1_UP_EN_S  4
/* MCPWM_OP0_FORCE_UP : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: A toggle (software negation of value of this bit) will trigger
 a forced update of active registers in PWM operator 0*/
#define MCPWM_OP0_FORCE_UP  (BIT(3))
#define MCPWM_OP0_FORCE_UP_M  (BIT(3))
#define MCPWM_OP0_FORCE_UP_V  0x1
#define MCPWM_OP0_FORCE_UP_S  3
/* MCPWM_OP0_UP_EN : R/W ;bitpos:[2] ;default: 1'd1 ; */
/*description: When set and PWM_GLOBAL_UP_EN is set  update of active registers
 in PWM operator 0 are enabled*/
#define MCPWM_OP0_UP_EN  (BIT(2))
#define MCPWM_OP0_UP_EN_M  (BIT(2))
#define MCPWM_OP0_UP_EN_V  0x1
#define MCPWM_OP0_UP_EN_S  2
/* MCPWM_GLOBAL_FORCE_UP : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: A toggle (software negation of value of this bit) will trigger
 a forced update of all active registers in MCPWM module*/
#define MCPWM_GLOBAL_FORCE_UP  (BIT(1))
#define MCPWM_GLOBAL_FORCE_UP_M  (BIT(1))
#define MCPWM_GLOBAL_FORCE_UP_V  0x1
#define MCPWM_GLOBAL_FORCE_UP_S  1
/* MCPWM_GLOBAL_UP_EN : R/W ;bitpos:[0] ;default: 1'd1 ; */
/*description: The global enable of update of all active registers in MCPWM module*/
#define MCPWM_GLOBAL_UP_EN  (BIT(0))
#define MCPWM_GLOBAL_UP_EN_M  (BIT(0))
#define MCPWM_GLOBAL_UP_EN_V  0x1
#define MCPWM_GLOBAL_UP_EN_S  0

#define MCMCPWM_INT_ENA_MCPWM_REG(i)          (REG_MCPWM_BASE(i) + 0x0110)
/* MCPWM_CAP2_INT_ENA : R/W ;bitpos:[29] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered by captureon channel 2*/
#define MCPWM_CAP2_INT_ENA  (BIT(29))
#define MCPWM_CAP2_INT_ENA_M  (BIT(29))
#define MCPWM_CAP2_INT_ENA_V  0x1
#define MCPWM_CAP2_INT_ENA_S  29
/* MCPWM_CAP1_INT_ENA : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered by captureon channel 1*/
#define MCPWM_CAP1_INT_ENA  (BIT(28))
#define MCPWM_CAP1_INT_ENA_M  (BIT(28))
#define MCPWM_CAP1_INT_ENA_V  0x1
#define MCPWM_CAP1_INT_ENA_S  28
/* MCPWM_CAP0_INT_ENA : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered by captureon channel 0*/
#define MCPWM_CAP0_INT_ENA  (BIT(27))
#define MCPWM_CAP0_INT_ENA_M  (BIT(27))
#define MCPWM_CAP0_INT_ENA_V  0x1
#define MCPWM_CAP0_INT_ENA_S  27
/* MCPWM_FH2_OST_INT_ENA : R/W ;bitpos:[26] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered by an one-shot mode action on PWM2*/
#define MCPWM_FH2_OST_INT_ENA  (BIT(26))
#define MCPWM_FH2_OST_INT_ENA_M  (BIT(26))
#define MCPWM_FH2_OST_INT_ENA_V  0x1
#define MCPWM_FH2_OST_INT_ENA_S  26
/* MCPWM_FH1_OST_INT_ENA : R/W ;bitpos:[25] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered by an one-shot mode action on PWM0*/
#define MCPWM_FH1_OST_INT_ENA  (BIT(25))
#define MCPWM_FH1_OST_INT_ENA_M  (BIT(25))
#define MCPWM_FH1_OST_INT_ENA_V  0x1
#define MCPWM_FH1_OST_INT_ENA_S  25
/* MCPWM_FH0_OST_INT_ENA : R/W ;bitpos:[24] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered by an one-shot mode action on PWM0*/
#define MCPWM_FH0_OST_INT_ENA  (BIT(24))
#define MCPWM_FH0_OST_INT_ENA_M  (BIT(24))
#define MCPWM_FH0_OST_INT_ENA_V  0x1
#define MCPWM_FH0_OST_INT_ENA_S  24
/* MCPWM_FH2_CBC_INT_ENA : R/W ;bitpos:[23] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered by an cycle-by-cycle mode action on PWM2*/
#define MCPWM_FH2_CBC_INT_ENA  (BIT(23))
#define MCPWM_FH2_CBC_INT_ENA_M  (BIT(23))
#define MCPWM_FH2_CBC_INT_ENA_V  0x1
#define MCPWM_FH2_CBC_INT_ENA_S  23
/* MCPWM_FH1_CBC_INT_ENA : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered by an cycle-by-cycle mode action on PWM1*/
#define MCPWM_FH1_CBC_INT_ENA  (BIT(22))
#define MCPWM_FH1_CBC_INT_ENA_M  (BIT(22))
#define MCPWM_FH1_CBC_INT_ENA_V  0x1
#define MCPWM_FH1_CBC_INT_ENA_S  22
/* MCPWM_FH0_CBC_INT_ENA : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered by an cycle-by-cycle mode action on PWM0*/
#define MCPWM_FH0_CBC_INT_ENA  (BIT(21))
#define MCPWM_FH0_CBC_INT_ENA_M  (BIT(21))
#define MCPWM_FH0_CBC_INT_ENA_V  0x1
#define MCPWM_FH0_CBC_INT_ENA_S  21
/* MCPWM_OP2_TEB_INT_ENA : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered by a PWM operator 2 TEB event*/
#define MCPWM_OP2_TEB_INT_ENA  (BIT(20))
#define MCPWM_OP2_TEB_INT_ENA_M  (BIT(20))
#define MCPWM_OP2_TEB_INT_ENA_V  0x1
#define MCPWM_OP2_TEB_INT_ENA_S  20
/* MCPWM_OP1_TEB_INT_ENA : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered by a PWM operator 1 TEB event*/
#define MCPWM_OP1_TEB_INT_ENA  (BIT(19))
#define MCPWM_OP1_TEB_INT_ENA_M  (BIT(19))
#define MCPWM_OP1_TEB_INT_ENA_V  0x1
#define MCPWM_OP1_TEB_INT_ENA_S  19
/* MCPWM_OP0_TEB_INT_ENA : R/W ;bitpos:[18] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered by a PWM operator 0 TEB event*/
#define MCPWM_OP0_TEB_INT_ENA  (BIT(18))
#define MCPWM_OP0_TEB_INT_ENA_M  (BIT(18))
#define MCPWM_OP0_TEB_INT_ENA_V  0x1
#define MCPWM_OP0_TEB_INT_ENA_S  18
/* MCPWM_OP2_TEA_INT_ENA : R/W ;bitpos:[17] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered by a PWM operator 2 TEA event*/
#define MCPWM_OP2_TEA_INT_ENA  (BIT(17))
#define MCPWM_OP2_TEA_INT_ENA_M  (BIT(17))
#define MCPWM_OP2_TEA_INT_ENA_V  0x1
#define MCPWM_OP2_TEA_INT_ENA_S  17
/* MCPWM_OP1_TEA_INT_ENA : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered by a PWM operator 1 TEA event*/
#define MCPWM_OP1_TEA_INT_ENA  (BIT(16))
#define MCPWM_OP1_TEA_INT_ENA_M  (BIT(16))
#define MCPWM_OP1_TEA_INT_ENA_V  0x1
#define MCPWM_OP1_TEA_INT_ENA_S  16
/* MCPWM_OP0_TEA_INT_ENA : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered by a PWM operator 0 TEA event*/
#define MCPWM_OP0_TEA_INT_ENA  (BIT(15))
#define MCPWM_OP0_TEA_INT_ENA_M  (BIT(15))
#define MCPWM_OP0_TEA_INT_ENA_V  0x1
#define MCPWM_OP0_TEA_INT_ENA_S  15
/* MCPWM_FAULT2_CLR_INT_ENA : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered when event_f2 ends*/
#define MCPWM_FAULT2_CLR_INT_ENA  (BIT(14))
#define MCPWM_FAULT2_CLR_INT_ENA_M  (BIT(14))
#define MCPWM_FAULT2_CLR_INT_ENA_V  0x1
#define MCPWM_FAULT2_CLR_INT_ENA_S  14
/* MCPWM_FAULT1_CLR_INT_ENA : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered when event_f1 ends*/
#define MCPWM_FAULT1_CLR_INT_ENA  (BIT(13))
#define MCPWM_FAULT1_CLR_INT_ENA_M  (BIT(13))
#define MCPWM_FAULT1_CLR_INT_ENA_V  0x1
#define MCPWM_FAULT1_CLR_INT_ENA_S  13
/* MCPWM_FAULT0_CLR_INT_ENA : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered when event_f0 ends*/
#define MCPWM_FAULT0_CLR_INT_ENA  (BIT(12))
#define MCPWM_FAULT0_CLR_INT_ENA_M  (BIT(12))
#define MCPWM_FAULT0_CLR_INT_ENA_V  0x1
#define MCPWM_FAULT0_CLR_INT_ENA_S  12
/* MCPWM_FAULT2_INT_ENA : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered when event_f2 starts*/
#define MCPWM_FAULT2_INT_ENA  (BIT(11))
#define MCPWM_FAULT2_INT_ENA_M  (BIT(11))
#define MCPWM_FAULT2_INT_ENA_V  0x1
#define MCPWM_FAULT2_INT_ENA_S  11
/* MCPWM_FAULT1_INT_ENA : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered when event_f1 starts*/
#define MCPWM_FAULT1_INT_ENA  (BIT(10))
#define MCPWM_FAULT1_INT_ENA_M  (BIT(10))
#define MCPWM_FAULT1_INT_ENA_V  0x1
#define MCPWM_FAULT1_INT_ENA_S  10
/* MCPWM_FAULT0_INT_ENA : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: The enable bit for interrupt triggered when event_f0 starts*/
#define MCPWM_FAULT0_INT_ENA  (BIT(9))
#define MCPWM_FAULT0_INT_ENA_M  (BIT(9))
#define MCPWM_FAULT0_INT_ENA_V  0x1
#define MCPWM_FAULT0_INT_ENA_S  9
/* MCPWM_TIMER2_TEP_INT_ENA : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: The enable bit for interrupt triggered by a PWM timer 2 TEP event*/
#define MCPWM_TIMER2_TEP_INT_ENA  (BIT(8))
#define MCPWM_TIMER2_TEP_INT_ENA_M  (BIT(8))
#define MCPWM_TIMER2_TEP_INT_ENA_V  0x1
#define MCPWM_TIMER2_TEP_INT_ENA_S  8
/* MCPWM_TIMER1_TEP_INT_ENA : R/W ;bitpos:[7] ;default: 1'h0 ; */
/*description: The enable bit for interrupt triggered by a PWM timer 1 TEP event*/
#define MCPWM_TIMER1_TEP_INT_ENA  (BIT(7))
#define MCPWM_TIMER1_TEP_INT_ENA_M  (BIT(7))
#define MCPWM_TIMER1_TEP_INT_ENA_V  0x1
#define MCPWM_TIMER1_TEP_INT_ENA_S  7
/* MCPWM_TIMER0_TEP_INT_ENA : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: The enable bit for interrupt triggered by a PWM timer 0 TEP event*/
#define MCPWM_TIMER0_TEP_INT_ENA  (BIT(6))
#define MCPWM_TIMER0_TEP_INT_ENA_M  (BIT(6))
#define MCPWM_TIMER0_TEP_INT_ENA_V  0x1
#define MCPWM_TIMER0_TEP_INT_ENA_S  6
/* MCPWM_TIMER2_TEZ_INT_ENA : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: The enable bit for interrupt triggered by a PWM timer 2 TEZ event*/
#define MCPWM_TIMER2_TEZ_INT_ENA  (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_ENA_M  (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_ENA_V  0x1
#define MCPWM_TIMER2_TEZ_INT_ENA_S  5
/* MCPWM_TIMER1_TEZ_INT_ENA : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: The enable bit for interrupt triggered by a PWM timer 1 TEZ event*/
#define MCPWM_TIMER1_TEZ_INT_ENA  (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_ENA_M  (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_ENA_V  0x1
#define MCPWM_TIMER1_TEZ_INT_ENA_S  4
/* MCPWM_TIMER0_TEZ_INT_ENA : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: The enable bit for interrupt triggered by a PWM timer 0 TEZ event*/
#define MCPWM_TIMER0_TEZ_INT_ENA  (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_ENA_M  (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_ENA_V  0x1
#define MCPWM_TIMER0_TEZ_INT_ENA_S  3
/* MCPWM_TIMER2_STOP_INT_ENA : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: The enable bit for interrupt triggered when timer 2 stops*/
#define MCPWM_TIMER2_STOP_INT_ENA  (BIT(2))
#define MCPWM_TIMER2_STOP_INT_ENA_M  (BIT(2))
#define MCPWM_TIMER2_STOP_INT_ENA_V  0x1
#define MCPWM_TIMER2_STOP_INT_ENA_S  2
/* MCPWM_TIMER1_STOP_INT_ENA : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: The enable bit for interrupt triggered when timer 1 stops*/
#define MCPWM_TIMER1_STOP_INT_ENA  (BIT(1))
#define MCPWM_TIMER1_STOP_INT_ENA_M  (BIT(1))
#define MCPWM_TIMER1_STOP_INT_ENA_V  0x1
#define MCPWM_TIMER1_STOP_INT_ENA_S  1
/* MCPWM_TIMER0_STOP_INT_ENA : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: The enable bit for interrupt triggered when timer 0 stops*/
#define MCPWM_TIMER0_STOP_INT_ENA  (BIT(0))
#define MCPWM_TIMER0_STOP_INT_ENA_M  (BIT(0))
#define MCPWM_TIMER0_STOP_INT_ENA_V  0x1
#define MCPWM_TIMER0_STOP_INT_ENA_S  0

#define MCMCPWM_INT_RAW_MCPWM_REG(i)          (REG_MCPWM_BASE(i) + 0x0114)
/* MCPWM_CAP2_INT_RAW : RO ;bitpos:[29] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered by captureon channel 2*/
#define MCPWM_CAP2_INT_RAW  (BIT(29))
#define MCPWM_CAP2_INT_RAW_M  (BIT(29))
#define MCPWM_CAP2_INT_RAW_V  0x1
#define MCPWM_CAP2_INT_RAW_S  29
/* MCPWM_CAP1_INT_RAW : RO ;bitpos:[28] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered by captureon channel 1*/
#define MCPWM_CAP1_INT_RAW  (BIT(28))
#define MCPWM_CAP1_INT_RAW_M  (BIT(28))
#define MCPWM_CAP1_INT_RAW_V  0x1
#define MCPWM_CAP1_INT_RAW_S  28
/* MCPWM_CAP0_INT_RAW : RO ;bitpos:[27] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered by captureon channel 0*/
#define MCPWM_CAP0_INT_RAW  (BIT(27))
#define MCPWM_CAP0_INT_RAW_M  (BIT(27))
#define MCPWM_CAP0_INT_RAW_V  0x1
#define MCPWM_CAP0_INT_RAW_S  27
/* MCPWM_FH2_OST_INT_RAW : RO ;bitpos:[26] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered by an one-shot mode action on PWM2*/
#define MCPWM_FH2_OST_INT_RAW  (BIT(26))
#define MCPWM_FH2_OST_INT_RAW_M  (BIT(26))
#define MCPWM_FH2_OST_INT_RAW_V  0x1
#define MCPWM_FH2_OST_INT_RAW_S  26
/* MCPWM_FH1_OST_INT_RAW : RO ;bitpos:[25] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered by an one-shot mode action on PWM0*/
#define MCPWM_FH1_OST_INT_RAW  (BIT(25))
#define MCPWM_FH1_OST_INT_RAW_M  (BIT(25))
#define MCPWM_FH1_OST_INT_RAW_V  0x1
#define MCPWM_FH1_OST_INT_RAW_S  25
/* MCPWM_FH0_OST_INT_RAW : RO ;bitpos:[24] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered by an one-shot mode action on PWM0*/
#define MCPWM_FH0_OST_INT_RAW  (BIT(24))
#define MCPWM_FH0_OST_INT_RAW_M  (BIT(24))
#define MCPWM_FH0_OST_INT_RAW_V  0x1
#define MCPWM_FH0_OST_INT_RAW_S  24
/* MCPWM_FH2_CBC_INT_RAW : RO ;bitpos:[23] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered by an cycle-by-cycle
 mode action on PWM2*/
#define MCPWM_FH2_CBC_INT_RAW  (BIT(23))
#define MCPWM_FH2_CBC_INT_RAW_M  (BIT(23))
#define MCPWM_FH2_CBC_INT_RAW_V  0x1
#define MCPWM_FH2_CBC_INT_RAW_S  23
/* MCPWM_FH1_CBC_INT_RAW : RO ;bitpos:[22] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered by an cycle-by-cycle
 mode action on PWM1*/
#define MCPWM_FH1_CBC_INT_RAW  (BIT(22))
#define MCPWM_FH1_CBC_INT_RAW_M  (BIT(22))
#define MCPWM_FH1_CBC_INT_RAW_V  0x1
#define MCPWM_FH1_CBC_INT_RAW_S  22
/* MCPWM_FH0_CBC_INT_RAW : RO ;bitpos:[21] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered by an cycle-by-cycle
 mode action on PWM0*/
#define MCPWM_FH0_CBC_INT_RAW  (BIT(21))
#define MCPWM_FH0_CBC_INT_RAW_M  (BIT(21))
#define MCPWM_FH0_CBC_INT_RAW_V  0x1
#define MCPWM_FH0_CBC_INT_RAW_S  21
/* MCPWM_OP2_TEB_INT_RAW : RO ;bitpos:[20] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered by a PWM operator 2 TEB event*/
#define MCPWM_OP2_TEB_INT_RAW  (BIT(20))
#define MCPWM_OP2_TEB_INT_RAW_M  (BIT(20))
#define MCPWM_OP2_TEB_INT_RAW_V  0x1
#define MCPWM_OP2_TEB_INT_RAW_S  20
/* MCPWM_OP1_TEB_INT_RAW : RO ;bitpos:[19] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered by a PWM operator 1 TEB event*/
#define MCPWM_OP1_TEB_INT_RAW  (BIT(19))
#define MCPWM_OP1_TEB_INT_RAW_M  (BIT(19))
#define MCPWM_OP1_TEB_INT_RAW_V  0x1
#define MCPWM_OP1_TEB_INT_RAW_S  19
/* MCPWM_OP0_TEB_INT_RAW : RO ;bitpos:[18] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered by a PWM operator 0 TEB event*/
#define MCPWM_OP0_TEB_INT_RAW  (BIT(18))
#define MCPWM_OP0_TEB_INT_RAW_M  (BIT(18))
#define MCPWM_OP0_TEB_INT_RAW_V  0x1
#define MCPWM_OP0_TEB_INT_RAW_S  18
/* MCPWM_OP2_TEA_INT_RAW : RO ;bitpos:[17] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered by a PWM operator 2 TEA event*/
#define MCPWM_OP2_TEA_INT_RAW  (BIT(17))
#define MCPWM_OP2_TEA_INT_RAW_M  (BIT(17))
#define MCPWM_OP2_TEA_INT_RAW_V  0x1
#define MCPWM_OP2_TEA_INT_RAW_S  17
/* MCPWM_OP1_TEA_INT_RAW : RO ;bitpos:[16] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered by a PWM operator 1 TEA event*/
#define MCPWM_OP1_TEA_INT_RAW  (BIT(16))
#define MCPWM_OP1_TEA_INT_RAW_M  (BIT(16))
#define MCPWM_OP1_TEA_INT_RAW_V  0x1
#define MCPWM_OP1_TEA_INT_RAW_S  16
/* MCPWM_OP0_TEA_INT_RAW : RO ;bitpos:[15] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered by a PWM operator 0 TEA event*/
#define MCPWM_OP0_TEA_INT_RAW  (BIT(15))
#define MCPWM_OP0_TEA_INT_RAW_M  (BIT(15))
#define MCPWM_OP0_TEA_INT_RAW_V  0x1
#define MCPWM_OP0_TEA_INT_RAW_S  15
/* MCPWM_FAULT2_CLR_INT_RAW : RO ;bitpos:[14] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered when event_f2 ends*/
#define MCPWM_FAULT2_CLR_INT_RAW  (BIT(14))
#define MCPWM_FAULT2_CLR_INT_RAW_M  (BIT(14))
#define MCPWM_FAULT2_CLR_INT_RAW_V  0x1
#define MCPWM_FAULT2_CLR_INT_RAW_S  14
/* MCPWM_FAULT1_CLR_INT_RAW : RO ;bitpos:[13] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered when event_f1 ends*/
#define MCPWM_FAULT1_CLR_INT_RAW  (BIT(13))
#define MCPWM_FAULT1_CLR_INT_RAW_M  (BIT(13))
#define MCPWM_FAULT1_CLR_INT_RAW_V  0x1
#define MCPWM_FAULT1_CLR_INT_RAW_S  13
/* MCPWM_FAULT0_CLR_INT_RAW : RO ;bitpos:[12] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered when event_f0 ends*/
#define MCPWM_FAULT0_CLR_INT_RAW  (BIT(12))
#define MCPWM_FAULT0_CLR_INT_RAW_M  (BIT(12))
#define MCPWM_FAULT0_CLR_INT_RAW_V  0x1
#define MCPWM_FAULT0_CLR_INT_RAW_S  12
/* MCPWM_FAULT2_INT_RAW : RO ;bitpos:[11] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered when event_f2 starts*/
#define MCPWM_FAULT2_INT_RAW  (BIT(11))
#define MCPWM_FAULT2_INT_RAW_M  (BIT(11))
#define MCPWM_FAULT2_INT_RAW_V  0x1
#define MCPWM_FAULT2_INT_RAW_S  11
/* MCPWM_FAULT1_INT_RAW : RO ;bitpos:[10] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered when event_f1 starts*/
#define MCPWM_FAULT1_INT_RAW  (BIT(10))
#define MCPWM_FAULT1_INT_RAW_M  (BIT(10))
#define MCPWM_FAULT1_INT_RAW_V  0x1
#define MCPWM_FAULT1_INT_RAW_S  10
/* MCPWM_FAULT0_INT_RAW : RO ;bitpos:[9] ;default: 1'd0 ; */
/*description: The raw status bit for interrupt triggered when event_f0 starts*/
#define MCPWM_FAULT0_INT_RAW  (BIT(9))
#define MCPWM_FAULT0_INT_RAW_M  (BIT(9))
#define MCPWM_FAULT0_INT_RAW_V  0x1
#define MCPWM_FAULT0_INT_RAW_S  9
/* MCPWM_TIMER2_TEP_INT_RAW : RO ;bitpos:[8] ;default: 1'h0 ; */
/*description: The raw status bit for interrupt triggered by a PWM timer 2 TEP event*/
#define MCPWM_TIMER2_TEP_INT_RAW  (BIT(8))
#define MCPWM_TIMER2_TEP_INT_RAW_M  (BIT(8))
#define MCPWM_TIMER2_TEP_INT_RAW_V  0x1
#define MCPWM_TIMER2_TEP_INT_RAW_S  8
/* MCPWM_TIMER1_TEP_INT_RAW : RO ;bitpos:[7] ;default: 1'h0 ; */
/*description: The raw status bit for interrupt triggered by a PWM timer 1 TEP event*/
#define MCPWM_TIMER1_TEP_INT_RAW  (BIT(7))
#define MCPWM_TIMER1_TEP_INT_RAW_M  (BIT(7))
#define MCPWM_TIMER1_TEP_INT_RAW_V  0x1
#define MCPWM_TIMER1_TEP_INT_RAW_S  7
/* MCPWM_TIMER0_TEP_INT_RAW : RO ;bitpos:[6] ;default: 1'h0 ; */
/*description: The raw status bit for interrupt triggered by a PWM timer 0 TEP event*/
#define MCPWM_TIMER0_TEP_INT_RAW  (BIT(6))
#define MCPWM_TIMER0_TEP_INT_RAW_M  (BIT(6))
#define MCPWM_TIMER0_TEP_INT_RAW_V  0x1
#define MCPWM_TIMER0_TEP_INT_RAW_S  6
/* MCPWM_TIMER2_TEZ_INT_RAW : RO ;bitpos:[5] ;default: 1'h0 ; */
/*description: The raw status bit for interrupt triggered by a PWM timer 2 TEZ event*/
#define MCPWM_TIMER2_TEZ_INT_RAW  (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_RAW_M  (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_RAW_V  0x1
#define MCPWM_TIMER2_TEZ_INT_RAW_S  5
/* MCPWM_TIMER1_TEZ_INT_RAW : RO ;bitpos:[4] ;default: 1'h0 ; */
/*description: The raw status bit for interrupt triggered by a PWM timer 1 TEZ event*/
#define MCPWM_TIMER1_TEZ_INT_RAW  (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_RAW_M  (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_RAW_V  0x1
#define MCPWM_TIMER1_TEZ_INT_RAW_S  4
/* MCPWM_TIMER0_TEZ_INT_RAW : RO ;bitpos:[3] ;default: 1'h0 ; */
/*description: The raw status bit for interrupt triggered by a PWM timer 0 TEZ event*/
#define MCPWM_TIMER0_TEZ_INT_RAW  (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_RAW_M  (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_RAW_V  0x1
#define MCPWM_TIMER0_TEZ_INT_RAW_S  3
/* MCPWM_TIMER2_STOP_INT_RAW : RO ;bitpos:[2] ;default: 1'h0 ; */
/*description: The raw status bit for interrupt triggered when timer 2 stops*/
#define MCPWM_TIMER2_STOP_INT_RAW  (BIT(2))
#define MCPWM_TIMER2_STOP_INT_RAW_M  (BIT(2))
#define MCPWM_TIMER2_STOP_INT_RAW_V  0x1
#define MCPWM_TIMER2_STOP_INT_RAW_S  2
/* MCPWM_TIMER1_STOP_INT_RAW : RO ;bitpos:[1] ;default: 1'h0 ; */
/*description: The raw status bit for interrupt triggered when timer 1 stops*/
#define MCPWM_TIMER1_STOP_INT_RAW  (BIT(1))
#define MCPWM_TIMER1_STOP_INT_RAW_M  (BIT(1))
#define MCPWM_TIMER1_STOP_INT_RAW_V  0x1
#define MCPWM_TIMER1_STOP_INT_RAW_S  1
/* MCPWM_TIMER0_STOP_INT_RAW : RO ;bitpos:[0] ;default: 1'h0 ; */
/*description: The raw status bit for interrupt triggered when timer 0 stops*/
#define MCPWM_TIMER0_STOP_INT_RAW  (BIT(0))
#define MCPWM_TIMER0_STOP_INT_RAW_M  (BIT(0))
#define MCPWM_TIMER0_STOP_INT_RAW_V  0x1
#define MCPWM_TIMER0_STOP_INT_RAW_S  0

#define MCMCPWM_INT_ST_MCPWM_REG(i)          (REG_MCPWM_BASE(i) + 0x0118)
/* MCPWM_CAP2_INT_ST : RO ;bitpos:[29] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered by captureon channel 2*/
#define MCPWM_CAP2_INT_ST  (BIT(29))
#define MCPWM_CAP2_INT_ST_M  (BIT(29))
#define MCPWM_CAP2_INT_ST_V  0x1
#define MCPWM_CAP2_INT_ST_S  29
/* MCPWM_CAP1_INT_ST : RO ;bitpos:[28] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered by captureon channel 1*/
#define MCPWM_CAP1_INT_ST  (BIT(28))
#define MCPWM_CAP1_INT_ST_M  (BIT(28))
#define MCPWM_CAP1_INT_ST_V  0x1
#define MCPWM_CAP1_INT_ST_S  28
/* MCPWM_CAP0_INT_ST : RO ;bitpos:[27] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered by captureon channel 0*/
#define MCPWM_CAP0_INT_ST  (BIT(27))
#define MCPWM_CAP0_INT_ST_M  (BIT(27))
#define MCPWM_CAP0_INT_ST_V  0x1
#define MCPWM_CAP0_INT_ST_S  27
/* MCPWM_FH2_OST_INT_ST : RO ;bitpos:[26] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered by an one-shot mode action on PWM2*/
#define MCPWM_FH2_OST_INT_ST  (BIT(26))
#define MCPWM_FH2_OST_INT_ST_M  (BIT(26))
#define MCPWM_FH2_OST_INT_ST_V  0x1
#define MCPWM_FH2_OST_INT_ST_S  26
/* MCPWM_FH1_OST_INT_ST : RO ;bitpos:[25] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered by an one-shot mode action on PWM0*/
#define MCPWM_FH1_OST_INT_ST  (BIT(25))
#define MCPWM_FH1_OST_INT_ST_M  (BIT(25))
#define MCPWM_FH1_OST_INT_ST_V  0x1
#define MCPWM_FH1_OST_INT_ST_S  25
/* MCPWM_FH0_OST_INT_ST : RO ;bitpos:[24] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered by an one-shot mode action on PWM0*/
#define MCPWM_FH0_OST_INT_ST  (BIT(24))
#define MCPWM_FH0_OST_INT_ST_M  (BIT(24))
#define MCPWM_FH0_OST_INT_ST_V  0x1
#define MCPWM_FH0_OST_INT_ST_S  24
/* MCPWM_FH2_CBC_INT_ST : RO ;bitpos:[23] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered by an cycle-by-cycle
 mode action on PWM2*/
#define MCPWM_FH2_CBC_INT_ST  (BIT(23))
#define MCPWM_FH2_CBC_INT_ST_M  (BIT(23))
#define MCPWM_FH2_CBC_INT_ST_V  0x1
#define MCPWM_FH2_CBC_INT_ST_S  23
/* MCPWM_FH1_CBC_INT_ST : RO ;bitpos:[22] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered by an cycle-by-cycle
 mode action on PWM1*/
#define MCPWM_FH1_CBC_INT_ST  (BIT(22))
#define MCPWM_FH1_CBC_INT_ST_M  (BIT(22))
#define MCPWM_FH1_CBC_INT_ST_V  0x1
#define MCPWM_FH1_CBC_INT_ST_S  22
/* MCPWM_FH0_CBC_INT_ST : RO ;bitpos:[21] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered by an cycle-by-cycle
 mode action on PWM0*/
#define MCPWM_FH0_CBC_INT_ST  (BIT(21))
#define MCPWM_FH0_CBC_INT_ST_M  (BIT(21))
#define MCPWM_FH0_CBC_INT_ST_V  0x1
#define MCPWM_FH0_CBC_INT_ST_S  21
/* MCPWM_OP2_TEB_INT_ST : RO ;bitpos:[20] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered by a PWM operator 2 TEB event*/
#define MCPWM_OP2_TEB_INT_ST  (BIT(20))
#define MCPWM_OP2_TEB_INT_ST_M  (BIT(20))
#define MCPWM_OP2_TEB_INT_ST_V  0x1
#define MCPWM_OP2_TEB_INT_ST_S  20
/* MCPWM_OP1_TEB_INT_ST : RO ;bitpos:[19] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered by a PWM operator 1 TEB event*/
#define MCPWM_OP1_TEB_INT_ST  (BIT(19))
#define MCPWM_OP1_TEB_INT_ST_M  (BIT(19))
#define MCPWM_OP1_TEB_INT_ST_V  0x1
#define MCPWM_OP1_TEB_INT_ST_S  19
/* MCPWM_OP0_TEB_INT_ST : RO ;bitpos:[18] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered by a PWM operator 0 TEB event*/
#define MCPWM_OP0_TEB_INT_ST  (BIT(18))
#define MCPWM_OP0_TEB_INT_ST_M  (BIT(18))
#define MCPWM_OP0_TEB_INT_ST_V  0x1
#define MCPWM_OP0_TEB_INT_ST_S  18
/* MCPWM_OP2_TEA_INT_ST : RO ;bitpos:[17] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered by a PWM operator 2 TEA event*/
#define MCPWM_OP2_TEA_INT_ST  (BIT(17))
#define MCPWM_OP2_TEA_INT_ST_M  (BIT(17))
#define MCPWM_OP2_TEA_INT_ST_V  0x1
#define MCPWM_OP2_TEA_INT_ST_S  17
/* MCPWM_OP1_TEA_INT_ST : RO ;bitpos:[16] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered by a PWM operator 1 TEA event*/
#define MCPWM_OP1_TEA_INT_ST  (BIT(16))
#define MCPWM_OP1_TEA_INT_ST_M  (BIT(16))
#define MCPWM_OP1_TEA_INT_ST_V  0x1
#define MCPWM_OP1_TEA_INT_ST_S  16
/* MCPWM_OP0_TEA_INT_ST : RO ;bitpos:[15] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered by a PWM operator 0 TEA event*/
#define MCPWM_OP0_TEA_INT_ST  (BIT(15))
#define MCPWM_OP0_TEA_INT_ST_M  (BIT(15))
#define MCPWM_OP0_TEA_INT_ST_V  0x1
#define MCPWM_OP0_TEA_INT_ST_S  15
/* MCPWM_FAULT2_CLR_INT_ST : RO ;bitpos:[14] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered when event_f2 ends*/
#define MCPWM_FAULT2_CLR_INT_ST  (BIT(14))
#define MCPWM_FAULT2_CLR_INT_ST_M  (BIT(14))
#define MCPWM_FAULT2_CLR_INT_ST_V  0x1
#define MCPWM_FAULT2_CLR_INT_ST_S  14
/* MCPWM_FAULT1_CLR_INT_ST : RO ;bitpos:[13] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered when event_f1 ends*/
#define MCPWM_FAULT1_CLR_INT_ST  (BIT(13))
#define MCPWM_FAULT1_CLR_INT_ST_M  (BIT(13))
#define MCPWM_FAULT1_CLR_INT_ST_V  0x1
#define MCPWM_FAULT1_CLR_INT_ST_S  13
/* MCPWM_FAULT0_CLR_INT_ST : RO ;bitpos:[12] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered when event_f0 ends*/
#define MCPWM_FAULT0_CLR_INT_ST  (BIT(12))
#define MCPWM_FAULT0_CLR_INT_ST_M  (BIT(12))
#define MCPWM_FAULT0_CLR_INT_ST_V  0x1
#define MCPWM_FAULT0_CLR_INT_ST_S  12
/* MCPWM_FAULT2_INT_ST : RO ;bitpos:[11] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered when event_f2 starts*/
#define MCPWM_FAULT2_INT_ST  (BIT(11))
#define MCPWM_FAULT2_INT_ST_M  (BIT(11))
#define MCPWM_FAULT2_INT_ST_V  0x1
#define MCPWM_FAULT2_INT_ST_S  11
/* MCPWM_FAULT1_INT_ST : RO ;bitpos:[10] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered when event_f1 starts*/
#define MCPWM_FAULT1_INT_ST  (BIT(10))
#define MCPWM_FAULT1_INT_ST_M  (BIT(10))
#define MCPWM_FAULT1_INT_ST_V  0x1
#define MCPWM_FAULT1_INT_ST_S  10
/* MCPWM_FAULT0_INT_ST : RO ;bitpos:[9] ;default: 1'd0 ; */
/*description: The masked status bit for interrupt triggered when event_f0 starts*/
#define MCPWM_FAULT0_INT_ST  (BIT(9))
#define MCPWM_FAULT0_INT_ST_M  (BIT(9))
#define MCPWM_FAULT0_INT_ST_V  0x1
#define MCPWM_FAULT0_INT_ST_S  9
/* MCPWM_TIMER2_TEP_INT_ST : RO ;bitpos:[8] ;default: 1'h0 ; */
/*description: The masked status bit for interrupt triggered by a PWM timer 2 TEP event*/
#define MCPWM_TIMER2_TEP_INT_ST  (BIT(8))
#define MCPWM_TIMER2_TEP_INT_ST_M  (BIT(8))
#define MCPWM_TIMER2_TEP_INT_ST_V  0x1
#define MCPWM_TIMER2_TEP_INT_ST_S  8
/* MCPWM_TIMER1_TEP_INT_ST : RO ;bitpos:[7] ;default: 1'h0 ; */
/*description: The masked status bit for interrupt triggered by a PWM timer 1 TEP event*/
#define MCPWM_TIMER1_TEP_INT_ST  (BIT(7))
#define MCPWM_TIMER1_TEP_INT_ST_M  (BIT(7))
#define MCPWM_TIMER1_TEP_INT_ST_V  0x1
#define MCPWM_TIMER1_TEP_INT_ST_S  7
/* MCPWM_TIMER0_TEP_INT_ST : RO ;bitpos:[6] ;default: 1'h0 ; */
/*description: The masked status bit for interrupt triggered by a PWM timer 0 TEP event*/
#define MCPWM_TIMER0_TEP_INT_ST  (BIT(6))
#define MCPWM_TIMER0_TEP_INT_ST_M  (BIT(6))
#define MCPWM_TIMER0_TEP_INT_ST_V  0x1
#define MCPWM_TIMER0_TEP_INT_ST_S  6
/* MCPWM_TIMER2_TEZ_INT_ST : RO ;bitpos:[5] ;default: 1'h0 ; */
/*description: The masked status bit for interrupt triggered by a PWM timer 2 TEZ event*/
#define MCPWM_TIMER2_TEZ_INT_ST  (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_ST_M  (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_ST_V  0x1
#define MCPWM_TIMER2_TEZ_INT_ST_S  5
/* MCPWM_TIMER1_TEZ_INT_ST : RO ;bitpos:[4] ;default: 1'h0 ; */
/*description: The masked status bit for interrupt triggered by a PWM timer 1 TEZ event*/
#define MCPWM_TIMER1_TEZ_INT_ST  (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_ST_M  (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_ST_V  0x1
#define MCPWM_TIMER1_TEZ_INT_ST_S  4
/* MCPWM_TIMER0_TEZ_INT_ST : RO ;bitpos:[3] ;default: 1'h0 ; */
/*description: The masked status bit for interrupt triggered by a PWM timer 0 TEZ event*/
#define MCPWM_TIMER0_TEZ_INT_ST  (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_ST_M  (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_ST_V  0x1
#define MCPWM_TIMER0_TEZ_INT_ST_S  3
/* MCPWM_TIMER2_STOP_INT_ST : RO ;bitpos:[2] ;default: 1'h0 ; */
/*description: The masked status bit for interrupt triggered when timer 2 stops*/
#define MCPWM_TIMER2_STOP_INT_ST  (BIT(2))
#define MCPWM_TIMER2_STOP_INT_ST_M  (BIT(2))
#define MCPWM_TIMER2_STOP_INT_ST_V  0x1
#define MCPWM_TIMER2_STOP_INT_ST_S  2
/* MCPWM_TIMER1_STOP_INT_ST : RO ;bitpos:[1] ;default: 1'h0 ; */
/*description: The masked status bit for interrupt triggered when timer 1 stops*/
#define MCPWM_TIMER1_STOP_INT_ST  (BIT(1))
#define MCPWM_TIMER1_STOP_INT_ST_M  (BIT(1))
#define MCPWM_TIMER1_STOP_INT_ST_V  0x1
#define MCPWM_TIMER1_STOP_INT_ST_S  1
/* MCPWM_TIMER0_STOP_INT_ST : RO ;bitpos:[0] ;default: 1'h0 ; */
/*description: The masked status bit for interrupt triggered when timer 0 stops*/
#define MCPWM_TIMER0_STOP_INT_ST  (BIT(0))
#define MCPWM_TIMER0_STOP_INT_ST_M  (BIT(0))
#define MCPWM_TIMER0_STOP_INT_ST_V  0x1
#define MCPWM_TIMER0_STOP_INT_ST_S  0

#define MCMCPWM_INT_CLR_MCPWM_REG(i)          (REG_MCPWM_BASE(i) + 0x011c)
/* MCPWM_CAP2_INT_CLR : WO ;bitpos:[29] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered by captureon channel 2*/
#define MCPWM_CAP2_INT_CLR  (BIT(29))
#define MCPWM_CAP2_INT_CLR_M  (BIT(29))
#define MCPWM_CAP2_INT_CLR_V  0x1
#define MCPWM_CAP2_INT_CLR_S  29
/* MCPWM_CAP1_INT_CLR : WO ;bitpos:[28] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered by captureon channel 1*/
#define MCPWM_CAP1_INT_CLR  (BIT(28))
#define MCPWM_CAP1_INT_CLR_M  (BIT(28))
#define MCPWM_CAP1_INT_CLR_V  0x1
#define MCPWM_CAP1_INT_CLR_S  28
/* MCPWM_CAP0_INT_CLR : WO ;bitpos:[27] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered by captureon channel 0*/
#define MCPWM_CAP0_INT_CLR  (BIT(27))
#define MCPWM_CAP0_INT_CLR_M  (BIT(27))
#define MCPWM_CAP0_INT_CLR_V  0x1
#define MCPWM_CAP0_INT_CLR_S  27
/* MCPWM_FH2_OST_INT_CLR : WO ;bitpos:[26] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered by an one-shot mode action on PWM2*/
#define MCPWM_FH2_OST_INT_CLR  (BIT(26))
#define MCPWM_FH2_OST_INT_CLR_M  (BIT(26))
#define MCPWM_FH2_OST_INT_CLR_V  0x1
#define MCPWM_FH2_OST_INT_CLR_S  26
/* MCPWM_FH1_OST_INT_CLR : WO ;bitpos:[25] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered by an one-shot mode action on PWM0*/
#define MCPWM_FH1_OST_INT_CLR  (BIT(25))
#define MCPWM_FH1_OST_INT_CLR_M  (BIT(25))
#define MCPWM_FH1_OST_INT_CLR_V  0x1
#define MCPWM_FH1_OST_INT_CLR_S  25
/* MCPWM_FH0_OST_INT_CLR : WO ;bitpos:[24] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered by an one-shot mode action on PWM0*/
#define MCPWM_FH0_OST_INT_CLR  (BIT(24))
#define MCPWM_FH0_OST_INT_CLR_M  (BIT(24))
#define MCPWM_FH0_OST_INT_CLR_V  0x1
#define MCPWM_FH0_OST_INT_CLR_S  24
/* MCPWM_FH2_CBC_INT_CLR : WO ;bitpos:[23] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered by an cycle-by-cycle
 mode action on PWM2*/
#define MCPWM_FH2_CBC_INT_CLR  (BIT(23))
#define MCPWM_FH2_CBC_INT_CLR_M  (BIT(23))
#define MCPWM_FH2_CBC_INT_CLR_V  0x1
#define MCPWM_FH2_CBC_INT_CLR_S  23
/* MCPWM_FH1_CBC_INT_CLR : WO ;bitpos:[22] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered by an cycle-by-cycle
 mode action on PWM1*/
#define MCPWM_FH1_CBC_INT_CLR  (BIT(22))
#define MCPWM_FH1_CBC_INT_CLR_M  (BIT(22))
#define MCPWM_FH1_CBC_INT_CLR_V  0x1
#define MCPWM_FH1_CBC_INT_CLR_S  22
/* MCPWM_FH0_CBC_INT_CLR : WO ;bitpos:[21] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered by an cycle-by-cycle
 mode action on PWM0*/
#define MCPWM_FH0_CBC_INT_CLR  (BIT(21))
#define MCPWM_FH0_CBC_INT_CLR_M  (BIT(21))
#define MCPWM_FH0_CBC_INT_CLR_V  0x1
#define MCPWM_FH0_CBC_INT_CLR_S  21
/* MCPWM_OP2_TEB_INT_CLR : WO ;bitpos:[20] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered by a PWM operator 2 TEB event*/
#define MCPWM_OP2_TEB_INT_CLR  (BIT(20))
#define MCPWM_OP2_TEB_INT_CLR_M  (BIT(20))
#define MCPWM_OP2_TEB_INT_CLR_V  0x1
#define MCPWM_OP2_TEB_INT_CLR_S  20
/* MCPWM_OP1_TEB_INT_CLR : WO ;bitpos:[19] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered by a PWM operator 1 TEB event*/
#define MCPWM_OP1_TEB_INT_CLR  (BIT(19))
#define MCPWM_OP1_TEB_INT_CLR_M  (BIT(19))
#define MCPWM_OP1_TEB_INT_CLR_V  0x1
#define MCPWM_OP1_TEB_INT_CLR_S  19
/* MCPWM_OP0_TEB_INT_CLR : WO ;bitpos:[18] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered by a PWM operator 0 TEB event*/
#define MCPWM_OP0_TEB_INT_CLR  (BIT(18))
#define MCPWM_OP0_TEB_INT_CLR_M  (BIT(18))
#define MCPWM_OP0_TEB_INT_CLR_V  0x1
#define MCPWM_OP0_TEB_INT_CLR_S  18
/* MCPWM_OP2_TEA_INT_CLR : WO ;bitpos:[17] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered by a PWM operator 2 TEA event*/
#define MCPWM_OP2_TEA_INT_CLR  (BIT(17))
#define MCPWM_OP2_TEA_INT_CLR_M  (BIT(17))
#define MCPWM_OP2_TEA_INT_CLR_V  0x1
#define MCPWM_OP2_TEA_INT_CLR_S  17
/* MCPWM_OP1_TEA_INT_CLR : WO ;bitpos:[16] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered by a PWM operator 1 TEA event*/
#define MCPWM_OP1_TEA_INT_CLR  (BIT(16))
#define MCPWM_OP1_TEA_INT_CLR_M  (BIT(16))
#define MCPWM_OP1_TEA_INT_CLR_V  0x1
#define MCPWM_OP1_TEA_INT_CLR_S  16
/* MCPWM_OP0_TEA_INT_CLR : WO ;bitpos:[15] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered by a PWM operator 0 TEA event*/
#define MCPWM_OP0_TEA_INT_CLR  (BIT(15))
#define MCPWM_OP0_TEA_INT_CLR_M  (BIT(15))
#define MCPWM_OP0_TEA_INT_CLR_V  0x1
#define MCPWM_OP0_TEA_INT_CLR_S  15
/* MCPWM_FAULT2_CLR_INT_CLR : WO ;bitpos:[14] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered when event_f2 ends*/
#define MCPWM_FAULT2_CLR_INT_CLR  (BIT(14))
#define MCPWM_FAULT2_CLR_INT_CLR_M  (BIT(14))
#define MCPWM_FAULT2_CLR_INT_CLR_V  0x1
#define MCPWM_FAULT2_CLR_INT_CLR_S  14
/* MCPWM_FAULT1_CLR_INT_CLR : WO ;bitpos:[13] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered when event_f1 ends*/
#define MCPWM_FAULT1_CLR_INT_CLR  (BIT(13))
#define MCPWM_FAULT1_CLR_INT_CLR_M  (BIT(13))
#define MCPWM_FAULT1_CLR_INT_CLR_V  0x1
#define MCPWM_FAULT1_CLR_INT_CLR_S  13
/* MCPWM_FAULT0_CLR_INT_CLR : WO ;bitpos:[12] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered when event_f0 ends*/
#define MCPWM_FAULT0_CLR_INT_CLR  (BIT(12))
#define MCPWM_FAULT0_CLR_INT_CLR_M  (BIT(12))
#define MCPWM_FAULT0_CLR_INT_CLR_V  0x1
#define MCPWM_FAULT0_CLR_INT_CLR_S  12
/* MCPWM_FAULT2_INT_CLR : WO ;bitpos:[11] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered when event_f2 starts*/
#define MCPWM_FAULT2_INT_CLR  (BIT(11))
#define MCPWM_FAULT2_INT_CLR_M  (BIT(11))
#define MCPWM_FAULT2_INT_CLR_V  0x1
#define MCPWM_FAULT2_INT_CLR_S  11
/* MCPWM_FAULT1_INT_CLR : WO ;bitpos:[10] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered when event_f1 starts*/
#define MCPWM_FAULT1_INT_CLR  (BIT(10))
#define MCPWM_FAULT1_INT_CLR_M  (BIT(10))
#define MCPWM_FAULT1_INT_CLR_V  0x1
#define MCPWM_FAULT1_INT_CLR_S  10
/* MCPWM_FAULT0_INT_CLR : WO ;bitpos:[9] ;default: 1'd0 ; */
/*description: Set this bit to clear interrupt triggered when event_f0 starts*/
#define MCPWM_FAULT0_INT_CLR  (BIT(9))
#define MCPWM_FAULT0_INT_CLR_M  (BIT(9))
#define MCPWM_FAULT0_INT_CLR_V  0x1
#define MCPWM_FAULT0_INT_CLR_S  9
/* MCPWM_TIMER2_TEP_INT_CLR : WO ;bitpos:[8] ;default: 1'h0 ; */
/*description: Set this bit to clear interrupt triggered by a PWM timer 2 TEP event*/
#define MCPWM_TIMER2_TEP_INT_CLR  (BIT(8))
#define MCPWM_TIMER2_TEP_INT_CLR_M  (BIT(8))
#define MCPWM_TIMER2_TEP_INT_CLR_V  0x1
#define MCPWM_TIMER2_TEP_INT_CLR_S  8
/* MCPWM_TIMER1_TEP_INT_CLR : WO ;bitpos:[7] ;default: 1'h0 ; */
/*description: Set this bit to clear interrupt triggered by a PWM timer 1 TEP event*/
#define MCPWM_TIMER1_TEP_INT_CLR  (BIT(7))
#define MCPWM_TIMER1_TEP_INT_CLR_M  (BIT(7))
#define MCPWM_TIMER1_TEP_INT_CLR_V  0x1
#define MCPWM_TIMER1_TEP_INT_CLR_S  7
/* MCPWM_TIMER0_TEP_INT_CLR : WO ;bitpos:[6] ;default: 1'h0 ; */
/*description: Set this bit to clear interrupt triggered by a PWM timer 0 TEP event*/
#define MCPWM_TIMER0_TEP_INT_CLR  (BIT(6))
#define MCPWM_TIMER0_TEP_INT_CLR_M  (BIT(6))
#define MCPWM_TIMER0_TEP_INT_CLR_V  0x1
#define MCPWM_TIMER0_TEP_INT_CLR_S  6
/* MCPWM_TIMER2_TEZ_INT_CLR : WO ;bitpos:[5] ;default: 1'h0 ; */
/*description: Set this bit to clear interrupt triggered by a PWM timer 2 TEZ event*/
#define MCPWM_TIMER2_TEZ_INT_CLR  (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_CLR_M  (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_CLR_V  0x1
#define MCPWM_TIMER2_TEZ_INT_CLR_S  5
/* MCPWM_TIMER1_TEZ_INT_CLR : WO ;bitpos:[4] ;default: 1'h0 ; */
/*description: Set this bit to clear interrupt triggered by a PWM timer 1 TEZ event*/
#define MCPWM_TIMER1_TEZ_INT_CLR  (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_CLR_M  (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_CLR_V  0x1
#define MCPWM_TIMER1_TEZ_INT_CLR_S  4
/* MCPWM_TIMER0_TEZ_INT_CLR : WO ;bitpos:[3] ;default: 1'h0 ; */
/*description: Set this bit to clear interrupt triggered by a PWM timer 0 TEZ event*/
#define MCPWM_TIMER0_TEZ_INT_CLR  (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_CLR_M  (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_CLR_V  0x1
#define MCPWM_TIMER0_TEZ_INT_CLR_S  3
/* MCPWM_TIMER2_STOP_INT_CLR : WO ;bitpos:[2] ;default: 1'h0 ; */
/*description: Set this bit to clear interrupt triggered when timer 2 stops*/
#define MCPWM_TIMER2_STOP_INT_CLR  (BIT(2))
#define MCPWM_TIMER2_STOP_INT_CLR_M  (BIT(2))
#define MCPWM_TIMER2_STOP_INT_CLR_V  0x1
#define MCPWM_TIMER2_STOP_INT_CLR_S  2
/* MCPWM_TIMER1_STOP_INT_CLR : WO ;bitpos:[1] ;default: 1'h0 ; */
/*description: Set this bit to clear interrupt triggered when timer 1 stops*/
#define MCPWM_TIMER1_STOP_INT_CLR  (BIT(1))
#define MCPWM_TIMER1_STOP_INT_CLR_M  (BIT(1))
#define MCPWM_TIMER1_STOP_INT_CLR_V  0x1
#define MCPWM_TIMER1_STOP_INT_CLR_S  1
/* MCPWM_TIMER0_STOP_INT_CLR : WO ;bitpos:[0] ;default: 1'h0 ; */
/*description: Set this bit to clear interrupt triggered when timer 0 stops*/
#define MCPWM_TIMER0_STOP_INT_CLR  (BIT(0))
#define MCPWM_TIMER0_STOP_INT_CLR_M  (BIT(0))
#define MCPWM_TIMER0_STOP_INT_CLR_V  0x1
#define MCPWM_TIMER0_STOP_INT_CLR_S  0

#define MCPWM_CLK_REG(i)          (REG_MCPWM_BASE(i) + 0x0120)
/* MCPWM_CLK_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: Force clock on for this reg file*/
#define MCPWM_CLK_EN  (BIT(0))
#define MCPWM_CLK_EN_M  (BIT(0))
#define MCPWM_CLK_EN_V  0x1
#define MCPWM_CLK_EN_S  0

#define MCPWM_VERSION_REG(i)          (REG_MCPWM_BASE(i) + 0x0124)
/* MCPWM_DATE : R/W ;bitpos:[27:0] ;default: 28'h1509110 ; */
/*description: Version of this reg file*/
#define MCPWM_DATE  0x0FFFFFFF
#define MCPWM_DATE_M  ((MCPWM_DATE_V)<<(MCPWM_DATE_S))
#define MCPWM_DATE_V  0xFFFFFFF
#define MCPWM_DATE_S  0




#endif /*_SOC_MCPWM_REG_H_ */
