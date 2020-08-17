// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "soc.h"

#define MCPWM_CLK_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x0000)
/* MCPWM_CLK_PRESCALE : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define MCPWM_CLK_PRESCALE 0x000000FF
#define MCPWM_CLK_PRESCALE_M ((MCPWM_CLK_PRESCALE_V) << (MCPWM_CLK_PRESCALE_S))
#define MCPWM_CLK_PRESCALE_V 0xFF
#define MCPWM_CLK_PRESCALE_S 0

#define MCPWM_TIMER0_CFG0_REG(i) (REG_MCPWM_BASE(i) + 0x0004)
/* MCPWM_TIMER0_PERIOD_UPMETHOD : R/W ;bitpos:[25:24] ;default: 2'd0 ; */
/*description: 0: immediate  1: eqz  2: sync  3: eqz | sync*/
#define MCPWM_TIMER0_PERIOD_UPMETHOD 0x00000003
#define MCPWM_TIMER0_PERIOD_UPMETHOD_M ((MCPWM_TIMER0_PERIOD_UPMETHOD_V) << (MCPWM_TIMER0_PERIOD_UPMETHOD_S))
#define MCPWM_TIMER0_PERIOD_UPMETHOD_V 0x3
#define MCPWM_TIMER0_PERIOD_UPMETHOD_S 24
/* MCPWM_TIMER0_PERIOD : R/W ;bitpos:[23:8] ;default: 16'h00ff ; */
/*description: */
#define MCPWM_TIMER0_PERIOD 0x0000FFFF
#define MCPWM_TIMER0_PERIOD_M ((MCPWM_TIMER0_PERIOD_V) << (MCPWM_TIMER0_PERIOD_S))
#define MCPWM_TIMER0_PERIOD_V 0xFFFF
#define MCPWM_TIMER0_PERIOD_S 8
/* MCPWM_TIMER0_PRESCALE : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define MCPWM_TIMER0_PRESCALE 0x000000FF
#define MCPWM_TIMER0_PRESCALE_M ((MCPWM_TIMER0_PRESCALE_V) << (MCPWM_TIMER0_PRESCALE_S))
#define MCPWM_TIMER0_PRESCALE_V 0xFF
#define MCPWM_TIMER0_PRESCALE_S 0

#define MCPWM_TIMER0_CFG1_REG(i) (REG_MCPWM_BASE(i) + 0x0008)
/* MCPWM_TIMER0_MOD : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: 0: freeze  1: inc  2: dec  3: up-down*/
#define MCPWM_TIMER0_MOD 0x00000003
#define MCPWM_TIMER0_MOD_M ((MCPWM_TIMER0_MOD_V) << (MCPWM_TIMER0_MOD_S))
#define MCPWM_TIMER0_MOD_V 0x3
#define MCPWM_TIMER0_MOD_S 3
/* MCPWM_TIMER0_START : R/W ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: 0: stop @ eqz  1: stop @ eqp  2: free run  3: start and stop
 @ next eqz  4: start and stop @ next eqp*/
#define MCPWM_TIMER0_START 0x00000007
#define MCPWM_TIMER0_START_M ((MCPWM_TIMER0_START_V) << (MCPWM_TIMER0_START_S))
#define MCPWM_TIMER0_START_V 0x7
#define MCPWM_TIMER0_START_S 0

#define MCPWM_TIMER0_SYNC_REG(i) (REG_MCPWM_BASE(i) + 0x000c)
/* MCPWM_TIMER0_PHASE : R/W ;bitpos:[20:4] ;default: 17'd0 ; */
/*description: */
#define MCPWM_TIMER0_PHASE 0x0001FFFF
#define MCPWM_TIMER0_PHASE_M ((MCPWM_TIMER0_PHASE_V) << (MCPWM_TIMER0_PHASE_S))
#define MCPWM_TIMER0_PHASE_V 0x1FFFF
#define MCPWM_TIMER0_PHASE_S 4
/* MCPWM_TIMER0_SYNCO_SEL : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: */
#define MCPWM_TIMER0_SYNCO_SEL 0x00000003
#define MCPWM_TIMER0_SYNCO_SEL_M ((MCPWM_TIMER0_SYNCO_SEL_V) << (MCPWM_TIMER0_SYNCO_SEL_S))
#define MCPWM_TIMER0_SYNCO_SEL_V 0x3
#define MCPWM_TIMER0_SYNCO_SEL_S 2
/* MCPWM_TIMER0_SYNC_SW : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: write the negate value will trigger a sw sync*/
#define MCPWM_TIMER0_SYNC_SW (BIT(1))
#define MCPWM_TIMER0_SYNC_SW_M (BIT(1))
#define MCPWM_TIMER0_SYNC_SW_V 0x1
#define MCPWM_TIMER0_SYNC_SW_S 1
/* MCPWM_TIMER0_SYNCI_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER0_SYNCI_EN (BIT(0))
#define MCPWM_TIMER0_SYNCI_EN_M (BIT(0))
#define MCPWM_TIMER0_SYNCI_EN_V 0x1
#define MCPWM_TIMER0_SYNCI_EN_S 0

#define MCPWM_TIMER0_STATUS_REG(i) (REG_MCPWM_BASE(i) + 0x0010)
/* MCPWM_TIMER0_DIRECTION : RO ;bitpos:[16] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TIMER0_DIRECTION (BIT(16))
#define MCPWM_TIMER0_DIRECTION_M (BIT(16))
#define MCPWM_TIMER0_DIRECTION_V 0x1
#define MCPWM_TIMER0_DIRECTION_S 16
/* MCPWM_TIMER0_VALUE : RO ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define MCPWM_TIMER0_VALUE 0x0000FFFF
#define MCPWM_TIMER0_VALUE_M ((MCPWM_TIMER0_VALUE_V) << (MCPWM_TIMER0_VALUE_S))
#define MCPWM_TIMER0_VALUE_V 0xFFFF
#define MCPWM_TIMER0_VALUE_S 0

#define MCPWM_TIMER1_CFG0_REG(i) (REG_MCPWM_BASE(i) + 0x0014)
/* MCPWM_TIMER1_PERIOD_UPMETHOD : R/W ;bitpos:[25:24] ;default: 2'd0 ; */
/*description: */
#define MCPWM_TIMER1_PERIOD_UPMETHOD 0x00000003
#define MCPWM_TIMER1_PERIOD_UPMETHOD_M ((MCPWM_TIMER1_PERIOD_UPMETHOD_V) << (MCPWM_TIMER1_PERIOD_UPMETHOD_S))
#define MCPWM_TIMER1_PERIOD_UPMETHOD_V 0x3
#define MCPWM_TIMER1_PERIOD_UPMETHOD_S 24
/* MCPWM_TIMER1_PERIOD : R/W ;bitpos:[23:8] ;default: 16'h00ff ; */
/*description: */
#define MCPWM_TIMER1_PERIOD 0x0000FFFF
#define MCPWM_TIMER1_PERIOD_M ((MCPWM_TIMER1_PERIOD_V) << (MCPWM_TIMER1_PERIOD_S))
#define MCPWM_TIMER1_PERIOD_V 0xFFFF
#define MCPWM_TIMER1_PERIOD_S 8
/* MCPWM_TIMER1_PRESCALE : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define MCPWM_TIMER1_PRESCALE 0x000000FF
#define MCPWM_TIMER1_PRESCALE_M ((MCPWM_TIMER1_PRESCALE_V) << (MCPWM_TIMER1_PRESCALE_S))
#define MCPWM_TIMER1_PRESCALE_V 0xFF
#define MCPWM_TIMER1_PRESCALE_S 0

#define MCPWM_TIMER1_CFG1_REG(i) (REG_MCPWM_BASE(i) + 0x0018)
/* MCPWM_TIMER1_MOD : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: 0: freeze  1: inc  2: dec  3: up-down*/
#define MCPWM_TIMER1_MOD 0x00000003
#define MCPWM_TIMER1_MOD_M ((MCPWM_TIMER1_MOD_V) << (MCPWM_TIMER1_MOD_S))
#define MCPWM_TIMER1_MOD_V 0x3
#define MCPWM_TIMER1_MOD_S 3
/* MCPWM_TIMER1_START : R/W ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: */
#define MCPWM_TIMER1_START 0x00000007
#define MCPWM_TIMER1_START_M ((MCPWM_TIMER1_START_V) << (MCPWM_TIMER1_START_S))
#define MCPWM_TIMER1_START_V 0x7
#define MCPWM_TIMER1_START_S 0

#define MCPWM_TIMER1_SYNC_REG(i) (REG_MCPWM_BASE(i) + 0x001c)
/* MCPWM_TIMER1_PHASE : R/W ;bitpos:[20:4] ;default: 17'd0 ; */
/*description: */
#define MCPWM_TIMER1_PHASE 0x0001FFFF
#define MCPWM_TIMER1_PHASE_M ((MCPWM_TIMER1_PHASE_V) << (MCPWM_TIMER1_PHASE_S))
#define MCPWM_TIMER1_PHASE_V 0x1FFFF
#define MCPWM_TIMER1_PHASE_S 4
/* MCPWM_TIMER1_SYNCO_SEL : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: 0: synci  1: tez  2: tep  else 0*/
#define MCPWM_TIMER1_SYNCO_SEL 0x00000003
#define MCPWM_TIMER1_SYNCO_SEL_M ((MCPWM_TIMER1_SYNCO_SEL_V) << (MCPWM_TIMER1_SYNCO_SEL_S))
#define MCPWM_TIMER1_SYNCO_SEL_V 0x3
#define MCPWM_TIMER1_SYNCO_SEL_S 2
/* MCPWM_TIMER1_SYNC_SW : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: write the negate value will trigger a sw sync*/
#define MCPWM_TIMER1_SYNC_SW (BIT(1))
#define MCPWM_TIMER1_SYNC_SW_M (BIT(1))
#define MCPWM_TIMER1_SYNC_SW_V 0x1
#define MCPWM_TIMER1_SYNC_SW_S 1
/* MCPWM_TIMER1_SYNCI_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER1_SYNCI_EN (BIT(0))
#define MCPWM_TIMER1_SYNCI_EN_M (BIT(0))
#define MCPWM_TIMER1_SYNCI_EN_V 0x1
#define MCPWM_TIMER1_SYNCI_EN_S 0

#define MCPWM_TIMER1_STATUS_REG(i) (REG_MCPWM_BASE(i) + 0x0020)
/* MCPWM_TIMER1_DIRECTION : RO ;bitpos:[16] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TIMER1_DIRECTION (BIT(16))
#define MCPWM_TIMER1_DIRECTION_M (BIT(16))
#define MCPWM_TIMER1_DIRECTION_V 0x1
#define MCPWM_TIMER1_DIRECTION_S 16
/* MCPWM_TIMER1_VALUE : RO ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define MCPWM_TIMER1_VALUE 0x0000FFFF
#define MCPWM_TIMER1_VALUE_M ((MCPWM_TIMER1_VALUE_V) << (MCPWM_TIMER1_VALUE_S))
#define MCPWM_TIMER1_VALUE_V 0xFFFF
#define MCPWM_TIMER1_VALUE_S 0

#define MCPWM_TIMER2_CFG0_REG(i) (REG_MCPWM_BASE(i) + 0x0024)
/* MCPWM_TIMER2_PERIOD_UPMETHOD : R/W ;bitpos:[25:24] ;default: 2'd0 ; */
/*description: */
#define MCPWM_TIMER2_PERIOD_UPMETHOD 0x00000003
#define MCPWM_TIMER2_PERIOD_UPMETHOD_M ((MCPWM_TIMER2_PERIOD_UPMETHOD_V) << (MCPWM_TIMER2_PERIOD_UPMETHOD_S))
#define MCPWM_TIMER2_PERIOD_UPMETHOD_V 0x3
#define MCPWM_TIMER2_PERIOD_UPMETHOD_S 24
/* MCPWM_TIMER2_PERIOD : R/W ;bitpos:[23:8] ;default: 16'h00ff ; */
/*description: */
#define MCPWM_TIMER2_PERIOD 0x0000FFFF
#define MCPWM_TIMER2_PERIOD_M ((MCPWM_TIMER2_PERIOD_V) << (MCPWM_TIMER2_PERIOD_S))
#define MCPWM_TIMER2_PERIOD_V 0xFFFF
#define MCPWM_TIMER2_PERIOD_S 8
/* MCPWM_TIMER2_PRESCALE : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define MCPWM_TIMER2_PRESCALE 0x000000FF
#define MCPWM_TIMER2_PRESCALE_M ((MCPWM_TIMER2_PRESCALE_V) << (MCPWM_TIMER2_PRESCALE_S))
#define MCPWM_TIMER2_PRESCALE_V 0xFF
#define MCPWM_TIMER2_PRESCALE_S 0

#define MCPWM_TIMER2_CFG1_REG(i) (REG_MCPWM_BASE(i) + 0x0028)
/* MCPWM_TIMER2_MOD : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define MCPWM_TIMER2_MOD 0x00000003
#define MCPWM_TIMER2_MOD_M ((MCPWM_TIMER2_MOD_V) << (MCPWM_TIMER2_MOD_S))
#define MCPWM_TIMER2_MOD_V 0x3
#define MCPWM_TIMER2_MOD_S 3
/* MCPWM_TIMER2_START : R/W ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: */
#define MCPWM_TIMER2_START 0x00000007
#define MCPWM_TIMER2_START_M ((MCPWM_TIMER2_START_V) << (MCPWM_TIMER2_START_S))
#define MCPWM_TIMER2_START_V 0x7
#define MCPWM_TIMER2_START_S 0

#define MCPWM_TIMER2_SYNC_REG(i) (REG_MCPWM_BASE(i) + 0x002c)
/* MCPWM_TIMER2_PHASE : R/W ;bitpos:[20:4] ;default: 17'd0 ; */
/*description: */
#define MCPWM_TIMER2_PHASE 0x0001FFFF
#define MCPWM_TIMER2_PHASE_M ((MCPWM_TIMER2_PHASE_V) << (MCPWM_TIMER2_PHASE_S))
#define MCPWM_TIMER2_PHASE_V 0x1FFFF
#define MCPWM_TIMER2_PHASE_S 4
/* MCPWM_TIMER2_SYNCO_SEL : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: */
#define MCPWM_TIMER2_SYNCO_SEL 0x00000003
#define MCPWM_TIMER2_SYNCO_SEL_M ((MCPWM_TIMER2_SYNCO_SEL_V) << (MCPWM_TIMER2_SYNCO_SEL_S))
#define MCPWM_TIMER2_SYNCO_SEL_V 0x3
#define MCPWM_TIMER2_SYNCO_SEL_S 2
/* MCPWM_TIMER2_SYNC_SW : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: write the negate value will trigger a sw sync*/
#define MCPWM_TIMER2_SYNC_SW (BIT(1))
#define MCPWM_TIMER2_SYNC_SW_M (BIT(1))
#define MCPWM_TIMER2_SYNC_SW_V 0x1
#define MCPWM_TIMER2_SYNC_SW_S 1
/* MCPWM_TIMER2_SYNCI_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER2_SYNCI_EN (BIT(0))
#define MCPWM_TIMER2_SYNCI_EN_M (BIT(0))
#define MCPWM_TIMER2_SYNCI_EN_V 0x1
#define MCPWM_TIMER2_SYNCI_EN_S 0

#define MCPWM_TIMER2_STATUS_REG(i) (REG_MCPWM_BASE(i) + 0x0030)
/* MCPWM_TIMER2_DIRECTION : RO ;bitpos:[16] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TIMER2_DIRECTION (BIT(16))
#define MCPWM_TIMER2_DIRECTION_M (BIT(16))
#define MCPWM_TIMER2_DIRECTION_V 0x1
#define MCPWM_TIMER2_DIRECTION_S 16
/* MCPWM_TIMER2_VALUE : RO ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define MCPWM_TIMER2_VALUE 0x0000FFFF
#define MCPWM_TIMER2_VALUE_M ((MCPWM_TIMER2_VALUE_V) << (MCPWM_TIMER2_VALUE_S))
#define MCPWM_TIMER2_VALUE_V 0xFFFF
#define MCPWM_TIMER2_VALUE_S 0

#define MCPWM_TIMER_SYNCI_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x0034)
/* MCPWM_EXTERNAL_SYNCI2_INVERT : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: */
#define MCPWM_EXTERNAL_SYNCI2_INVERT (BIT(11))
#define MCPWM_EXTERNAL_SYNCI2_INVERT_M (BIT(11))
#define MCPWM_EXTERNAL_SYNCI2_INVERT_V 0x1
#define MCPWM_EXTERNAL_SYNCI2_INVERT_S 11
/* MCPWM_EXTERNAL_SYNCI1_INVERT : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: */
#define MCPWM_EXTERNAL_SYNCI1_INVERT (BIT(10))
#define MCPWM_EXTERNAL_SYNCI1_INVERT_M (BIT(10))
#define MCPWM_EXTERNAL_SYNCI1_INVERT_V 0x1
#define MCPWM_EXTERNAL_SYNCI1_INVERT_S 10
/* MCPWM_EXTERNAL_SYNCI0_INVERT : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: */
#define MCPWM_EXTERNAL_SYNCI0_INVERT (BIT(9))
#define MCPWM_EXTERNAL_SYNCI0_INVERT_M (BIT(9))
#define MCPWM_EXTERNAL_SYNCI0_INVERT_V 0x1
#define MCPWM_EXTERNAL_SYNCI0_INVERT_S 9
/* MCPWM_TIMER2_SYNCISEL : R/W ;bitpos:[8:6] ;default: 3'd0 ; */
/*description: */
#define MCPWM_TIMER2_SYNCISEL 0x00000007
#define MCPWM_TIMER2_SYNCISEL_M ((MCPWM_TIMER2_SYNCISEL_V) << (MCPWM_TIMER2_SYNCISEL_S))
#define MCPWM_TIMER2_SYNCISEL_V 0x7
#define MCPWM_TIMER2_SYNCISEL_S 6
/* MCPWM_TIMER1_SYNCISEL : R/W ;bitpos:[5:3] ;default: 3'd0 ; */
/*description: */
#define MCPWM_TIMER1_SYNCISEL 0x00000007
#define MCPWM_TIMER1_SYNCISEL_M ((MCPWM_TIMER1_SYNCISEL_V) << (MCPWM_TIMER1_SYNCISEL_S))
#define MCPWM_TIMER1_SYNCISEL_V 0x7
#define MCPWM_TIMER1_SYNCISEL_S 3
/* MCPWM_TIMER0_SYNCISEL : R/W ;bitpos:[2:0] ;default: 3'd0 ; */
/*description: */
#define MCPWM_TIMER0_SYNCISEL 0x00000007
#define MCPWM_TIMER0_SYNCISEL_M ((MCPWM_TIMER0_SYNCISEL_V) << (MCPWM_TIMER0_SYNCISEL_S))
#define MCPWM_TIMER0_SYNCISEL_V 0x7
#define MCPWM_TIMER0_SYNCISEL_S 0

#define MCPWM_OPERATOR_TIMERSEL_REG(i) (REG_MCPWM_BASE(i) + 0x0038)
/* MCPWM_OPERATOR2_TIMERSEL : R/W ;bitpos:[5:4] ;default: 2'd0 ; */
/*description: 0: timer0  1: timer1  2: timer2*/
#define MCPWM_OPERATOR2_TIMERSEL 0x00000003
#define MCPWM_OPERATOR2_TIMERSEL_M ((MCPWM_OPERATOR2_TIMERSEL_V) << (MCPWM_OPERATOR2_TIMERSEL_S))
#define MCPWM_OPERATOR2_TIMERSEL_V 0x3
#define MCPWM_OPERATOR2_TIMERSEL_S 4
/* MCPWM_OPERATOR1_TIMERSEL : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: 0: timer0  1: timer1  2: timer2*/
#define MCPWM_OPERATOR1_TIMERSEL 0x00000003
#define MCPWM_OPERATOR1_TIMERSEL_M ((MCPWM_OPERATOR1_TIMERSEL_V) << (MCPWM_OPERATOR1_TIMERSEL_S))
#define MCPWM_OPERATOR1_TIMERSEL_V 0x3
#define MCPWM_OPERATOR1_TIMERSEL_S 2
/* MCPWM_OPERATOR0_TIMERSEL : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: 0: timer0  1: timer1  2: timer2*/
#define MCPWM_OPERATOR0_TIMERSEL 0x00000003
#define MCPWM_OPERATOR0_TIMERSEL_M ((MCPWM_OPERATOR0_TIMERSEL_V) << (MCPWM_OPERATOR0_TIMERSEL_S))
#define MCPWM_OPERATOR0_TIMERSEL_V 0x3
#define MCPWM_OPERATOR0_TIMERSEL_S 0

#define MCPWM_CMPR0_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x003c)
/* MCPWM_CMPR0_B_SHDW_FULL : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR0_B_SHDW_FULL (BIT(9))
#define MCPWM_CMPR0_B_SHDW_FULL_M (BIT(9))
#define MCPWM_CMPR0_B_SHDW_FULL_V 0x1
#define MCPWM_CMPR0_B_SHDW_FULL_S 9
/* MCPWM_CMPR0_A_SHDW_FULL : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR0_A_SHDW_FULL (BIT(8))
#define MCPWM_CMPR0_A_SHDW_FULL_M (BIT(8))
#define MCPWM_CMPR0_A_SHDW_FULL_V 0x1
#define MCPWM_CMPR0_A_SHDW_FULL_S 8
/* MCPWM_CMPR0_B_UPMETHOD : R/W ;bitpos:[7:4] ;default: 4'd0 ; */
/*description: 0: immediate  bit0: tez  bit1: tep  bit2: sync  bit3: freeze*/
#define MCPWM_CMPR0_B_UPMETHOD 0x0000000F
#define MCPWM_CMPR0_B_UPMETHOD_M ((MCPWM_CMPR0_B_UPMETHOD_V) << (MCPWM_CMPR0_B_UPMETHOD_S))
#define MCPWM_CMPR0_B_UPMETHOD_V 0xF
#define MCPWM_CMPR0_B_UPMETHOD_S 4
/* MCPWM_CMPR0_A_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: 0: immediate  bit0: tez  bit1: tep  bit2: sync  bit3: freeze*/
#define MCPWM_CMPR0_A_UPMETHOD 0x0000000F
#define MCPWM_CMPR0_A_UPMETHOD_M ((MCPWM_CMPR0_A_UPMETHOD_V) << (MCPWM_CMPR0_A_UPMETHOD_S))
#define MCPWM_CMPR0_A_UPMETHOD_V 0xF
#define MCPWM_CMPR0_A_UPMETHOD_S 0

#define MCPWM_CMPR0_VALUE0_REG(i) (REG_MCPWM_BASE(i) + 0x0040)
/* MCPWM_CMPR0_A : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define MCPWM_CMPR0_A 0x0000FFFF
#define MCPWM_CMPR0_A_M ((MCPWM_CMPR0_A_V) << (MCPWM_CMPR0_A_S))
#define MCPWM_CMPR0_A_V 0xFFFF
#define MCPWM_CMPR0_A_S 0

#define MCPWM_CMPR0_VALUE1_REG(i) (REG_MCPWM_BASE(i) + 0x0044)
/* MCPWM_CMPR0_B : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define MCPWM_CMPR0_B 0x0000FFFF
#define MCPWM_CMPR0_B_M ((MCPWM_CMPR0_B_V) << (MCPWM_CMPR0_B_S))
#define MCPWM_CMPR0_B_V 0xFFFF
#define MCPWM_CMPR0_B_S 0

#define MCPWM_GEN0_CFG0_REG(i) (REG_MCPWM_BASE(i) + 0x0048)
/* MCPWM_GEN0_T1_SEL : R/W ;bitpos:[9:7] ;default: 3'd0 ; */
/*description: take effect immediately  0: extra0  1: extra1  2: extra2  3: sync_taken  4: none*/
#define MCPWM_GEN0_T1_SEL 0x00000007
#define MCPWM_GEN0_T1_SEL_M ((MCPWM_GEN0_T1_SEL_V) << (MCPWM_GEN0_T1_SEL_S))
#define MCPWM_GEN0_T1_SEL_V 0x7
#define MCPWM_GEN0_T1_SEL_S 7
/* MCPWM_GEN0_T0_SEL : R/W ;bitpos:[6:4] ;default: 3'd0 ; */
/*description: take effect immediately  0: extra0  1: extra1  2: extra2  3: sync_taken  4: none*/
#define MCPWM_GEN0_T0_SEL 0x00000007
#define MCPWM_GEN0_T0_SEL_M ((MCPWM_GEN0_T0_SEL_V) << (MCPWM_GEN0_T0_SEL_S))
#define MCPWM_GEN0_T0_SEL_V 0x7
#define MCPWM_GEN0_T0_SEL_S 4
/* MCPWM_GEN0_CFG_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: 0: immediate  bit0: tez  bit1: tep  bit2: sync. bit3: freeze*/
#define MCPWM_GEN0_CFG_UPMETHOD 0x0000000F
#define MCPWM_GEN0_CFG_UPMETHOD_M ((MCPWM_GEN0_CFG_UPMETHOD_V) << (MCPWM_GEN0_CFG_UPMETHOD_S))
#define MCPWM_GEN0_CFG_UPMETHOD_V 0xF
#define MCPWM_GEN0_CFG_UPMETHOD_S 0

#define MCPWM_GEN0_FORCE_REG(i) (REG_MCPWM_BASE(i) + 0x004c)
/* MCPWM_GEN0_B_NCIFORCE_MODE : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: 0: disabled  1: low  2: high  3: disabled*/
#define MCPWM_GEN0_B_NCIFORCE_MODE 0x00000003
#define MCPWM_GEN0_B_NCIFORCE_MODE_M ((MCPWM_GEN0_B_NCIFORCE_MODE_V) << (MCPWM_GEN0_B_NCIFORCE_MODE_S))
#define MCPWM_GEN0_B_NCIFORCE_MODE_V 0x3
#define MCPWM_GEN0_B_NCIFORCE_MODE_S 14
/* MCPWM_GEN0_B_NCIFORCE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: non-continuous immediate sw force  a toggle will trigger a force event*/
#define MCPWM_GEN0_B_NCIFORCE (BIT(13))
#define MCPWM_GEN0_B_NCIFORCE_M (BIT(13))
#define MCPWM_GEN0_B_NCIFORCE_V 0x1
#define MCPWM_GEN0_B_NCIFORCE_S 13
/* MCPWM_GEN0_A_NCIFORCE_MODE : R/W ;bitpos:[12:11] ;default: 2'd0 ; */
/*description: 0: disabled  1: low  2: high  3: disabled*/
#define MCPWM_GEN0_A_NCIFORCE_MODE 0x00000003
#define MCPWM_GEN0_A_NCIFORCE_MODE_M ((MCPWM_GEN0_A_NCIFORCE_MODE_V) << (MCPWM_GEN0_A_NCIFORCE_MODE_S))
#define MCPWM_GEN0_A_NCIFORCE_MODE_V 0x3
#define MCPWM_GEN0_A_NCIFORCE_MODE_S 11
/* MCPWM_GEN0_A_NCIFORCE : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: non-continuous immediate sw force  a toggle will trigger a force event*/
#define MCPWM_GEN0_A_NCIFORCE (BIT(10))
#define MCPWM_GEN0_A_NCIFORCE_M (BIT(10))
#define MCPWM_GEN0_A_NCIFORCE_V 0x1
#define MCPWM_GEN0_A_NCIFORCE_S 10
/* MCPWM_GEN0_B_CNTUFORCE_MODE : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: 0: disabled  1: low  2: high  3: disabled*/
#define MCPWM_GEN0_B_CNTUFORCE_MODE 0x00000003
#define MCPWM_GEN0_B_CNTUFORCE_MODE_M ((MCPWM_GEN0_B_CNTUFORCE_MODE_V) << (MCPWM_GEN0_B_CNTUFORCE_MODE_S))
#define MCPWM_GEN0_B_CNTUFORCE_MODE_V 0x3
#define MCPWM_GEN0_B_CNTUFORCE_MODE_S 8
/* MCPWM_GEN0_A_CNTUFORCE_MODE : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: 0: disabled  1: low  2: high  3: disabled*/
#define MCPWM_GEN0_A_CNTUFORCE_MODE 0x00000003
#define MCPWM_GEN0_A_CNTUFORCE_MODE_M ((MCPWM_GEN0_A_CNTUFORCE_MODE_V) << (MCPWM_GEN0_A_CNTUFORCE_MODE_S))
#define MCPWM_GEN0_A_CNTUFORCE_MODE_V 0x3
#define MCPWM_GEN0_A_CNTUFORCE_MODE_S 6
/* MCPWM_GEN0_CNTUFORCE_UPMETHOD : R/W ;bitpos:[5:0] ;default: 6'h20 ; */
/*description: 0: immediate  bit0: tez  bit1: tep  bit2: tea  bit3: teb  bit4:
 sync  bit5: freeze*/
#define MCPWM_GEN0_CNTUFORCE_UPMETHOD 0x0000003F
#define MCPWM_GEN0_CNTUFORCE_UPMETHOD_M ((MCPWM_GEN0_CNTUFORCE_UPMETHOD_V) << (MCPWM_GEN0_CNTUFORCE_UPMETHOD_S))
#define MCPWM_GEN0_CNTUFORCE_UPMETHOD_V 0x3F
#define MCPWM_GEN0_CNTUFORCE_UPMETHOD_S 0

#define MCPWM_GEN0_A_REG(i) (REG_MCPWM_BASE(i) + 0x0050)
/* MCPWM_GEN0_A_DT1 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: 0: no change  1: low  2: high  3: toggle*/
#define MCPWM_GEN0_A_DT1 0x00000003
#define MCPWM_GEN0_A_DT1_M ((MCPWM_GEN0_A_DT1_V) << (MCPWM_GEN0_A_DT1_S))
#define MCPWM_GEN0_A_DT1_V 0x3
#define MCPWM_GEN0_A_DT1_S 22
/* MCPWM_GEN0_A_DT0 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_A_DT0 0x00000003
#define MCPWM_GEN0_A_DT0_M ((MCPWM_GEN0_A_DT0_V) << (MCPWM_GEN0_A_DT0_S))
#define MCPWM_GEN0_A_DT0_V 0x3
#define MCPWM_GEN0_A_DT0_S 20
/* MCPWM_GEN0_A_DTEB : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_A_DTEB 0x00000003
#define MCPWM_GEN0_A_DTEB_M ((MCPWM_GEN0_A_DTEB_V) << (MCPWM_GEN0_A_DTEB_S))
#define MCPWM_GEN0_A_DTEB_V 0x3
#define MCPWM_GEN0_A_DTEB_S 18
/* MCPWM_GEN0_A_DTEA : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_A_DTEA 0x00000003
#define MCPWM_GEN0_A_DTEA_M ((MCPWM_GEN0_A_DTEA_V) << (MCPWM_GEN0_A_DTEA_S))
#define MCPWM_GEN0_A_DTEA_V 0x3
#define MCPWM_GEN0_A_DTEA_S 16
/* MCPWM_GEN0_A_DTEP : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_A_DTEP 0x00000003
#define MCPWM_GEN0_A_DTEP_M ((MCPWM_GEN0_A_DTEP_V) << (MCPWM_GEN0_A_DTEP_S))
#define MCPWM_GEN0_A_DTEP_V 0x3
#define MCPWM_GEN0_A_DTEP_S 14
/* MCPWM_GEN0_A_DTEZ : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_A_DTEZ 0x00000003
#define MCPWM_GEN0_A_DTEZ_M ((MCPWM_GEN0_A_DTEZ_V) << (MCPWM_GEN0_A_DTEZ_S))
#define MCPWM_GEN0_A_DTEZ_V 0x3
#define MCPWM_GEN0_A_DTEZ_S 12
/* MCPWM_GEN0_A_UT1 : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_A_UT1 0x00000003
#define MCPWM_GEN0_A_UT1_M ((MCPWM_GEN0_A_UT1_V) << (MCPWM_GEN0_A_UT1_S))
#define MCPWM_GEN0_A_UT1_V 0x3
#define MCPWM_GEN0_A_UT1_S 10
/* MCPWM_GEN0_A_UT0 : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_A_UT0 0x00000003
#define MCPWM_GEN0_A_UT0_M ((MCPWM_GEN0_A_UT0_V) << (MCPWM_GEN0_A_UT0_S))
#define MCPWM_GEN0_A_UT0_V 0x3
#define MCPWM_GEN0_A_UT0_S 8
/* MCPWM_GEN0_A_UTEB : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_A_UTEB 0x00000003
#define MCPWM_GEN0_A_UTEB_M ((MCPWM_GEN0_A_UTEB_V) << (MCPWM_GEN0_A_UTEB_S))
#define MCPWM_GEN0_A_UTEB_V 0x3
#define MCPWM_GEN0_A_UTEB_S 6
/* MCPWM_GEN0_A_UTEA : R/W ;bitpos:[5:4] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_A_UTEA 0x00000003
#define MCPWM_GEN0_A_UTEA_M ((MCPWM_GEN0_A_UTEA_V) << (MCPWM_GEN0_A_UTEA_S))
#define MCPWM_GEN0_A_UTEA_V 0x3
#define MCPWM_GEN0_A_UTEA_S 4
/* MCPWM_GEN0_A_UTEP : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_A_UTEP 0x00000003
#define MCPWM_GEN0_A_UTEP_M ((MCPWM_GEN0_A_UTEP_V) << (MCPWM_GEN0_A_UTEP_S))
#define MCPWM_GEN0_A_UTEP_V 0x3
#define MCPWM_GEN0_A_UTEP_S 2
/* MCPWM_GEN0_A_UTEZ : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_A_UTEZ 0x00000003
#define MCPWM_GEN0_A_UTEZ_M ((MCPWM_GEN0_A_UTEZ_V) << (MCPWM_GEN0_A_UTEZ_S))
#define MCPWM_GEN0_A_UTEZ_V 0x3
#define MCPWM_GEN0_A_UTEZ_S 0

#define MCPWM_GEN0_B_REG(i) (REG_MCPWM_BASE(i) + 0x0054)
/* MCPWM_GEN0_B_DT1 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_B_DT1 0x00000003
#define MCPWM_GEN0_B_DT1_M ((MCPWM_GEN0_B_DT1_V) << (MCPWM_GEN0_B_DT1_S))
#define MCPWM_GEN0_B_DT1_V 0x3
#define MCPWM_GEN0_B_DT1_S 22
/* MCPWM_GEN0_B_DT0 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_B_DT0 0x00000003
#define MCPWM_GEN0_B_DT0_M ((MCPWM_GEN0_B_DT0_V) << (MCPWM_GEN0_B_DT0_S))
#define MCPWM_GEN0_B_DT0_V 0x3
#define MCPWM_GEN0_B_DT0_S 20
/* MCPWM_GEN0_B_DTEB : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_B_DTEB 0x00000003
#define MCPWM_GEN0_B_DTEB_M ((MCPWM_GEN0_B_DTEB_V) << (MCPWM_GEN0_B_DTEB_S))
#define MCPWM_GEN0_B_DTEB_V 0x3
#define MCPWM_GEN0_B_DTEB_S 18
/* MCPWM_GEN0_B_DTEA : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_B_DTEA 0x00000003
#define MCPWM_GEN0_B_DTEA_M ((MCPWM_GEN0_B_DTEA_V) << (MCPWM_GEN0_B_DTEA_S))
#define MCPWM_GEN0_B_DTEA_V 0x3
#define MCPWM_GEN0_B_DTEA_S 16
/* MCPWM_GEN0_B_DTEP : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_B_DTEP 0x00000003
#define MCPWM_GEN0_B_DTEP_M ((MCPWM_GEN0_B_DTEP_V) << (MCPWM_GEN0_B_DTEP_S))
#define MCPWM_GEN0_B_DTEP_V 0x3
#define MCPWM_GEN0_B_DTEP_S 14
/* MCPWM_GEN0_B_DTEZ : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_B_DTEZ 0x00000003
#define MCPWM_GEN0_B_DTEZ_M ((MCPWM_GEN0_B_DTEZ_V) << (MCPWM_GEN0_B_DTEZ_S))
#define MCPWM_GEN0_B_DTEZ_V 0x3
#define MCPWM_GEN0_B_DTEZ_S 12
/* MCPWM_GEN0_B_UT1 : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_B_UT1 0x00000003
#define MCPWM_GEN0_B_UT1_M ((MCPWM_GEN0_B_UT1_V) << (MCPWM_GEN0_B_UT1_S))
#define MCPWM_GEN0_B_UT1_V 0x3
#define MCPWM_GEN0_B_UT1_S 10
/* MCPWM_GEN0_B_UT0 : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_B_UT0 0x00000003
#define MCPWM_GEN0_B_UT0_M ((MCPWM_GEN0_B_UT0_V) << (MCPWM_GEN0_B_UT0_S))
#define MCPWM_GEN0_B_UT0_V 0x3
#define MCPWM_GEN0_B_UT0_S 8
/* MCPWM_GEN0_B_UTEB : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_B_UTEB 0x00000003
#define MCPWM_GEN0_B_UTEB_M ((MCPWM_GEN0_B_UTEB_V) << (MCPWM_GEN0_B_UTEB_S))
#define MCPWM_GEN0_B_UTEB_V 0x3
#define MCPWM_GEN0_B_UTEB_S 6
/* MCPWM_GEN0_B_UTEA : R/W ;bitpos:[5:4] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_B_UTEA 0x00000003
#define MCPWM_GEN0_B_UTEA_M ((MCPWM_GEN0_B_UTEA_V) << (MCPWM_GEN0_B_UTEA_S))
#define MCPWM_GEN0_B_UTEA_V 0x3
#define MCPWM_GEN0_B_UTEA_S 4
/* MCPWM_GEN0_B_UTEP : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_B_UTEP 0x00000003
#define MCPWM_GEN0_B_UTEP_M ((MCPWM_GEN0_B_UTEP_V) << (MCPWM_GEN0_B_UTEP_S))
#define MCPWM_GEN0_B_UTEP_V 0x3
#define MCPWM_GEN0_B_UTEP_S 2
/* MCPWM_GEN0_B_UTEZ : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN0_B_UTEZ 0x00000003
#define MCPWM_GEN0_B_UTEZ_M ((MCPWM_GEN0_B_UTEZ_V) << (MCPWM_GEN0_B_UTEZ_S))
#define MCPWM_GEN0_B_UTEZ_V 0x3
#define MCPWM_GEN0_B_UTEZ_S 0

#define MCPWM_DB0_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x0058)
/* MCPWM_DB0_CLK_SEL : R/W ;bitpos:[17] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB0_CLK_SEL (BIT(17))
#define MCPWM_DB0_CLK_SEL_M (BIT(17))
#define MCPWM_DB0_CLK_SEL_V 0x1
#define MCPWM_DB0_CLK_SEL_S 17
/* MCPWM_DB0_B_OUTBYPASS : R/W ;bitpos:[16] ;default: 1'd1 ; */
/*description: */
#define MCPWM_DB0_B_OUTBYPASS (BIT(16))
#define MCPWM_DB0_B_OUTBYPASS_M (BIT(16))
#define MCPWM_DB0_B_OUTBYPASS_V 0x1
#define MCPWM_DB0_B_OUTBYPASS_S 16
/* MCPWM_DB0_A_OUTBYPASS : R/W ;bitpos:[15] ;default: 1'd1 ; */
/*description: */
#define MCPWM_DB0_A_OUTBYPASS (BIT(15))
#define MCPWM_DB0_A_OUTBYPASS_M (BIT(15))
#define MCPWM_DB0_A_OUTBYPASS_V 0x1
#define MCPWM_DB0_A_OUTBYPASS_S 15
/* MCPWM_DB0_FED_OUTINVERT : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB0_FED_OUTINVERT (BIT(14))
#define MCPWM_DB0_FED_OUTINVERT_M (BIT(14))
#define MCPWM_DB0_FED_OUTINVERT_V 0x1
#define MCPWM_DB0_FED_OUTINVERT_S 14
/* MCPWM_DB0_RED_OUTINVERT : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB0_RED_OUTINVERT (BIT(13))
#define MCPWM_DB0_RED_OUTINVERT_M (BIT(13))
#define MCPWM_DB0_RED_OUTINVERT_V 0x1
#define MCPWM_DB0_RED_OUTINVERT_S 13
/* MCPWM_DB0_FED_INSEL : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB0_FED_INSEL (BIT(12))
#define MCPWM_DB0_FED_INSEL_M (BIT(12))
#define MCPWM_DB0_FED_INSEL_V 0x1
#define MCPWM_DB0_FED_INSEL_S 12
/* MCPWM_DB0_RED_INSEL : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB0_RED_INSEL (BIT(11))
#define MCPWM_DB0_RED_INSEL_M (BIT(11))
#define MCPWM_DB0_RED_INSEL_V 0x1
#define MCPWM_DB0_RED_INSEL_S 11
/* MCPWM_DB0_B_OUTSWAP : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB0_B_OUTSWAP (BIT(10))
#define MCPWM_DB0_B_OUTSWAP_M (BIT(10))
#define MCPWM_DB0_B_OUTSWAP_V 0x1
#define MCPWM_DB0_B_OUTSWAP_S 10
/* MCPWM_DB0_A_OUTSWAP : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB0_A_OUTSWAP (BIT(9))
#define MCPWM_DB0_A_OUTSWAP_M (BIT(9))
#define MCPWM_DB0_A_OUTSWAP_V 0x1
#define MCPWM_DB0_A_OUTSWAP_S 9
/* MCPWM_DB0_DEB_MODE : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: immediate  dual-edge B mode  0: fed/red take effect on different
 path separately  1: fed/red take effect on B path  A out is in bypass or dulpB mode*/
#define MCPWM_DB0_DEB_MODE (BIT(8))
#define MCPWM_DB0_DEB_MODE_M (BIT(8))
#define MCPWM_DB0_DEB_MODE_V 0x1
#define MCPWM_DB0_DEB_MODE_S 8
/* MCPWM_DB0_RED_UPMETHOD : R/W ;bitpos:[7:4] ;default: 4'd0 ; */
/*description: 0: immediate  bit0: tez  bit1: tep  bit2: sync  bit3: freeze*/
#define MCPWM_DB0_RED_UPMETHOD 0x0000000F
#define MCPWM_DB0_RED_UPMETHOD_M ((MCPWM_DB0_RED_UPMETHOD_V) << (MCPWM_DB0_RED_UPMETHOD_S))
#define MCPWM_DB0_RED_UPMETHOD_V 0xF
#define MCPWM_DB0_RED_UPMETHOD_S 4
/* MCPWM_DB0_FED_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: 0: immediate  bit0: tez  bit1: tep  bit2: sync  bit3: freeze*/
#define MCPWM_DB0_FED_UPMETHOD 0x0000000F
#define MCPWM_DB0_FED_UPMETHOD_M ((MCPWM_DB0_FED_UPMETHOD_V) << (MCPWM_DB0_FED_UPMETHOD_S))
#define MCPWM_DB0_FED_UPMETHOD_V 0xF
#define MCPWM_DB0_FED_UPMETHOD_S 0

#define MCPWM_DB0_FED_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x005c)
/* MCPWM_DB0_FED : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define MCPWM_DB0_FED 0x0000FFFF
#define MCPWM_DB0_FED_M ((MCPWM_DB0_FED_V) << (MCPWM_DB0_FED_S))
#define MCPWM_DB0_FED_V 0xFFFF
#define MCPWM_DB0_FED_S 0

#define MCPWM_DB0_RED_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x0060)
/* MCPWM_DB0_RED : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define MCPWM_DB0_RED 0x0000FFFF
#define MCPWM_DB0_RED_M ((MCPWM_DB0_RED_V) << (MCPWM_DB0_RED_S))
#define MCPWM_DB0_RED_V 0xFFFF
#define MCPWM_DB0_RED_S 0

#define MCPWM_CHOPPER0_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x0064)
/* MCPWM_CHOPPER0_IN_INVERT : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CHOPPER0_IN_INVERT (BIT(13))
#define MCPWM_CHOPPER0_IN_INVERT_M (BIT(13))
#define MCPWM_CHOPPER0_IN_INVERT_V 0x1
#define MCPWM_CHOPPER0_IN_INVERT_S 13
/* MCPWM_CHOPPER0_OUT_INVERT : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CHOPPER0_OUT_INVERT (BIT(12))
#define MCPWM_CHOPPER0_OUT_INVERT_M (BIT(12))
#define MCPWM_CHOPPER0_OUT_INVERT_V 0x1
#define MCPWM_CHOPPER0_OUT_INVERT_S 12
/* MCPWM_CHOPPER0_OSHTWTH : R/W ;bitpos:[11:8] ;default: 4'd0 ; */
/*description: */
#define MCPWM_CHOPPER0_OSHTWTH 0x0000000F
#define MCPWM_CHOPPER0_OSHTWTH_M ((MCPWM_CHOPPER0_OSHTWTH_V) << (MCPWM_CHOPPER0_OSHTWTH_S))
#define MCPWM_CHOPPER0_OSHTWTH_V 0xF
#define MCPWM_CHOPPER0_OSHTWTH_S 8
/* MCPWM_CHOPPER0_DUTY : R/W ;bitpos:[7:5] ;default: 3'd0 ; */
/*description: */
#define MCPWM_CHOPPER0_DUTY 0x00000007
#define MCPWM_CHOPPER0_DUTY_M ((MCPWM_CHOPPER0_DUTY_V) << (MCPWM_CHOPPER0_DUTY_S))
#define MCPWM_CHOPPER0_DUTY_V 0x7
#define MCPWM_CHOPPER0_DUTY_S 5
/* MCPWM_CHOPPER0_PRESCALE : R/W ;bitpos:[4:1] ;default: 4'd0 ; */
/*description: */
#define MCPWM_CHOPPER0_PRESCALE 0x0000000F
#define MCPWM_CHOPPER0_PRESCALE_M ((MCPWM_CHOPPER0_PRESCALE_V) << (MCPWM_CHOPPER0_PRESCALE_S))
#define MCPWM_CHOPPER0_PRESCALE_V 0xF
#define MCPWM_CHOPPER0_PRESCALE_S 1
/* MCPWM_CHOPPER0_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CHOPPER0_EN (BIT(0))
#define MCPWM_CHOPPER0_EN_M (BIT(0))
#define MCPWM_CHOPPER0_EN_V 0x1
#define MCPWM_CHOPPER0_EN_S 0

#define MCPWM_TZ0_CFG0_REG(i) (REG_MCPWM_BASE(i) + 0x0068)
/* MCPWM_TZ0_B_OST_U : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ0_B_OST_U 0x00000003
#define MCPWM_TZ0_B_OST_U_M ((MCPWM_TZ0_B_OST_U_V) << (MCPWM_TZ0_B_OST_U_S))
#define MCPWM_TZ0_B_OST_U_V 0x3
#define MCPWM_TZ0_B_OST_U_S 22
/* MCPWM_TZ0_B_OST_D : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ0_B_OST_D 0x00000003
#define MCPWM_TZ0_B_OST_D_M ((MCPWM_TZ0_B_OST_D_V) << (MCPWM_TZ0_B_OST_D_S))
#define MCPWM_TZ0_B_OST_D_V 0x3
#define MCPWM_TZ0_B_OST_D_S 20
/* MCPWM_TZ0_B_CBC_U : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ0_B_CBC_U 0x00000003
#define MCPWM_TZ0_B_CBC_U_M ((MCPWM_TZ0_B_CBC_U_V) << (MCPWM_TZ0_B_CBC_U_S))
#define MCPWM_TZ0_B_CBC_U_V 0x3
#define MCPWM_TZ0_B_CBC_U_S 18
/* MCPWM_TZ0_B_CBC_D : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ0_B_CBC_D 0x00000003
#define MCPWM_TZ0_B_CBC_D_M ((MCPWM_TZ0_B_CBC_D_V) << (MCPWM_TZ0_B_CBC_D_S))
#define MCPWM_TZ0_B_CBC_D_V 0x3
#define MCPWM_TZ0_B_CBC_D_S 16
/* MCPWM_TZ0_A_OST_U : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ0_A_OST_U 0x00000003
#define MCPWM_TZ0_A_OST_U_M ((MCPWM_TZ0_A_OST_U_V) << (MCPWM_TZ0_A_OST_U_S))
#define MCPWM_TZ0_A_OST_U_V 0x3
#define MCPWM_TZ0_A_OST_U_S 14
/* MCPWM_TZ0_A_OST_D : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ0_A_OST_D 0x00000003
#define MCPWM_TZ0_A_OST_D_M ((MCPWM_TZ0_A_OST_D_V) << (MCPWM_TZ0_A_OST_D_S))
#define MCPWM_TZ0_A_OST_D_V 0x3
#define MCPWM_TZ0_A_OST_D_S 12
/* MCPWM_TZ0_A_CBC_U : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ0_A_CBC_U 0x00000003
#define MCPWM_TZ0_A_CBC_U_M ((MCPWM_TZ0_A_CBC_U_V) << (MCPWM_TZ0_A_CBC_U_S))
#define MCPWM_TZ0_A_CBC_U_V 0x3
#define MCPWM_TZ0_A_CBC_U_S 10
/* MCPWM_TZ0_A_CBC_D : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ0_A_CBC_D 0x00000003
#define MCPWM_TZ0_A_CBC_D_M ((MCPWM_TZ0_A_CBC_D_V) << (MCPWM_TZ0_A_CBC_D_S))
#define MCPWM_TZ0_A_CBC_D_V 0x3
#define MCPWM_TZ0_A_CBC_D_S 8
/* MCPWM_TZ0_F0_OST : R/W ;bitpos:[7] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ0_F0_OST (BIT(7))
#define MCPWM_TZ0_F0_OST_M (BIT(7))
#define MCPWM_TZ0_F0_OST_V 0x1
#define MCPWM_TZ0_F0_OST_S 7
/* MCPWM_TZ0_F1_OST : R/W ;bitpos:[6] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ0_F1_OST (BIT(6))
#define MCPWM_TZ0_F1_OST_M (BIT(6))
#define MCPWM_TZ0_F1_OST_V 0x1
#define MCPWM_TZ0_F1_OST_S 6
/* MCPWM_TZ0_F2_OST : R/W ;bitpos:[5] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ0_F2_OST (BIT(5))
#define MCPWM_TZ0_F2_OST_M (BIT(5))
#define MCPWM_TZ0_F2_OST_V 0x1
#define MCPWM_TZ0_F2_OST_S 5
/* MCPWM_TZ0_SW_OST : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ0_SW_OST (BIT(4))
#define MCPWM_TZ0_SW_OST_M (BIT(4))
#define MCPWM_TZ0_SW_OST_V 0x1
#define MCPWM_TZ0_SW_OST_S 4
/* MCPWM_TZ0_F0_CBC : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ0_F0_CBC (BIT(3))
#define MCPWM_TZ0_F0_CBC_M (BIT(3))
#define MCPWM_TZ0_F0_CBC_V 0x1
#define MCPWM_TZ0_F0_CBC_S 3
/* MCPWM_TZ0_F1_CBC : R/W ;bitpos:[2] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ0_F1_CBC (BIT(2))
#define MCPWM_TZ0_F1_CBC_M (BIT(2))
#define MCPWM_TZ0_F1_CBC_V 0x1
#define MCPWM_TZ0_F1_CBC_S 2
/* MCPWM_TZ0_F2_CBC : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ0_F2_CBC (BIT(1))
#define MCPWM_TZ0_F2_CBC_M (BIT(1))
#define MCPWM_TZ0_F2_CBC_V 0x1
#define MCPWM_TZ0_F2_CBC_S 1
/* MCPWM_TZ0_SW_CBC : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ0_SW_CBC (BIT(0))
#define MCPWM_TZ0_SW_CBC_M (BIT(0))
#define MCPWM_TZ0_SW_CBC_V 0x1
#define MCPWM_TZ0_SW_CBC_S 0

#define MCPWM_TZ0_CFG1_REG(i) (REG_MCPWM_BASE(i) + 0x006c)
/* MCPWM_TZ0_FORCE_OST : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: a toggle trigger a oneshot tripping*/
#define MCPWM_TZ0_FORCE_OST (BIT(4))
#define MCPWM_TZ0_FORCE_OST_M (BIT(4))
#define MCPWM_TZ0_FORCE_OST_V 0x1
#define MCPWM_TZ0_FORCE_OST_S 4
/* MCPWM_TZ0_FORCE_CBC : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: a toggle trigger a cycle-by-cycle tripping*/
#define MCPWM_TZ0_FORCE_CBC (BIT(3))
#define MCPWM_TZ0_FORCE_CBC_M (BIT(3))
#define MCPWM_TZ0_FORCE_CBC_V 0x1
#define MCPWM_TZ0_FORCE_CBC_S 3
/* MCPWM_TZ0_CBCPULSE : R/W ;bitpos:[2:1] ;default: 2'd0 ; */
/*description: bit0: tez  bit1: tep*/
#define MCPWM_TZ0_CBCPULSE 0x00000003
#define MCPWM_TZ0_CBCPULSE_M ((MCPWM_TZ0_CBCPULSE_V) << (MCPWM_TZ0_CBCPULSE_S))
#define MCPWM_TZ0_CBCPULSE_V 0x3
#define MCPWM_TZ0_CBCPULSE_S 1
/* MCPWM_TZ0_CLR_OST : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: a toggle will clear oneshot tripping*/
#define MCPWM_TZ0_CLR_OST (BIT(0))
#define MCPWM_TZ0_CLR_OST_M (BIT(0))
#define MCPWM_TZ0_CLR_OST_V 0x1
#define MCPWM_TZ0_CLR_OST_S 0

#define MCPWM_TZ0_STATUS_REG(i) (REG_MCPWM_BASE(i) + 0x0070)
/* MCPWM_TZ0_OST_ON : RO ;bitpos:[1] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ0_OST_ON (BIT(1))
#define MCPWM_TZ0_OST_ON_M (BIT(1))
#define MCPWM_TZ0_OST_ON_V 0x1
#define MCPWM_TZ0_OST_ON_S 1
/* MCPWM_TZ0_CBC_ON : RO ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ0_CBC_ON (BIT(0))
#define MCPWM_TZ0_CBC_ON_M (BIT(0))
#define MCPWM_TZ0_CBC_ON_V 0x1
#define MCPWM_TZ0_CBC_ON_S 0

#define MCPWM_CMPR1_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x0074)
/* MCPWM_CMPR1_B_SHDW_FULL : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR1_B_SHDW_FULL (BIT(9))
#define MCPWM_CMPR1_B_SHDW_FULL_M (BIT(9))
#define MCPWM_CMPR1_B_SHDW_FULL_V 0x1
#define MCPWM_CMPR1_B_SHDW_FULL_S 9
/* MCPWM_CMPR1_A_SHDW_FULL : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR1_A_SHDW_FULL (BIT(8))
#define MCPWM_CMPR1_A_SHDW_FULL_M (BIT(8))
#define MCPWM_CMPR1_A_SHDW_FULL_V 0x1
#define MCPWM_CMPR1_A_SHDW_FULL_S 8
/* MCPWM_CMPR1_B_UPMETHOD : R/W ;bitpos:[7:4] ;default: 4'd0 ; */
/*description: 0: immediate  bit0: tez  bit1: tep  bit2: sync  bit3: freeze*/
#define MCPWM_CMPR1_B_UPMETHOD 0x0000000F
#define MCPWM_CMPR1_B_UPMETHOD_M ((MCPWM_CMPR1_B_UPMETHOD_V) << (MCPWM_CMPR1_B_UPMETHOD_S))
#define MCPWM_CMPR1_B_UPMETHOD_V 0xF
#define MCPWM_CMPR1_B_UPMETHOD_S 4
/* MCPWM_CMPR1_A_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: 0: immediate  bit0: tez  bit1: tep  bit2: sync  bit3: freeze*/
#define MCPWM_CMPR1_A_UPMETHOD 0x0000000F
#define MCPWM_CMPR1_A_UPMETHOD_M ((MCPWM_CMPR1_A_UPMETHOD_V) << (MCPWM_CMPR1_A_UPMETHOD_S))
#define MCPWM_CMPR1_A_UPMETHOD_V 0xF
#define MCPWM_CMPR1_A_UPMETHOD_S 0

#define MCPWM_CMPR1_VALUE0_REG(i) (REG_MCPWM_BASE(i) + 0x0078)
/* MCPWM_CMPR1_A : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define MCPWM_CMPR1_A 0x0000FFFF
#define MCPWM_CMPR1_A_M ((MCPWM_CMPR1_A_V) << (MCPWM_CMPR1_A_S))
#define MCPWM_CMPR1_A_V 0xFFFF
#define MCPWM_CMPR1_A_S 0

#define MCPWM_CMPR1_VALUE1_REG(i) (REG_MCPWM_BASE(i) + 0x007c)
/* MCPWM_CMPR1_B : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define MCPWM_CMPR1_B 0x0000FFFF
#define MCPWM_CMPR1_B_M ((MCPWM_CMPR1_B_V) << (MCPWM_CMPR1_B_S))
#define MCPWM_CMPR1_B_V 0xFFFF
#define MCPWM_CMPR1_B_S 0

#define MCPWM_GEN1_CFG0_REG(i) (REG_MCPWM_BASE(i) + 0x0080)
/* MCPWM_GEN1_T1_SEL : R/W ;bitpos:[9:7] ;default: 3'd0 ; */
/*description: take effect immediately  0: extra0  1: extra1  2: extra2  3: sync_taken  4: none*/
#define MCPWM_GEN1_T1_SEL 0x00000007
#define MCPWM_GEN1_T1_SEL_M ((MCPWM_GEN1_T1_SEL_V) << (MCPWM_GEN1_T1_SEL_S))
#define MCPWM_GEN1_T1_SEL_V 0x7
#define MCPWM_GEN1_T1_SEL_S 7
/* MCPWM_GEN1_T0_SEL : R/W ;bitpos:[6:4] ;default: 3'd0 ; */
/*description: take effect immediately  0: extra0  1: extra1  2: extra2  3: sync_taken  4: none*/
#define MCPWM_GEN1_T0_SEL 0x00000007
#define MCPWM_GEN1_T0_SEL_M ((MCPWM_GEN1_T0_SEL_V) << (MCPWM_GEN1_T0_SEL_S))
#define MCPWM_GEN1_T0_SEL_V 0x7
#define MCPWM_GEN1_T0_SEL_S 4
/* MCPWM_GEN1_CFG_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: 0: immediate  bit0: tez  bit1: tep  bit2: sync. bit3: freeze*/
#define MCPWM_GEN1_CFG_UPMETHOD 0x0000000F
#define MCPWM_GEN1_CFG_UPMETHOD_M ((MCPWM_GEN1_CFG_UPMETHOD_V) << (MCPWM_GEN1_CFG_UPMETHOD_S))
#define MCPWM_GEN1_CFG_UPMETHOD_V 0xF
#define MCPWM_GEN1_CFG_UPMETHOD_S 0

#define MCPWM_GEN1_FORCE_REG(i) (REG_MCPWM_BASE(i) + 0x0084)
/* MCPWM_GEN1_B_NCIFORCE_MODE : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: 0: disabled  1: low  2: high  3: disabled*/
#define MCPWM_GEN1_B_NCIFORCE_MODE 0x00000003
#define MCPWM_GEN1_B_NCIFORCE_MODE_M ((MCPWM_GEN1_B_NCIFORCE_MODE_V) << (MCPWM_GEN1_B_NCIFORCE_MODE_S))
#define MCPWM_GEN1_B_NCIFORCE_MODE_V 0x3
#define MCPWM_GEN1_B_NCIFORCE_MODE_S 14
/* MCPWM_GEN1_B_NCIFORCE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: non-continuous immediate sw force  a toggle will trigger a force event*/
#define MCPWM_GEN1_B_NCIFORCE (BIT(13))
#define MCPWM_GEN1_B_NCIFORCE_M (BIT(13))
#define MCPWM_GEN1_B_NCIFORCE_V 0x1
#define MCPWM_GEN1_B_NCIFORCE_S 13
/* MCPWM_GEN1_A_NCIFORCE_MODE : R/W ;bitpos:[12:11] ;default: 2'd0 ; */
/*description: 0: disabled  1: low  2: high  3: disabled*/
#define MCPWM_GEN1_A_NCIFORCE_MODE 0x00000003
#define MCPWM_GEN1_A_NCIFORCE_MODE_M ((MCPWM_GEN1_A_NCIFORCE_MODE_V) << (MCPWM_GEN1_A_NCIFORCE_MODE_S))
#define MCPWM_GEN1_A_NCIFORCE_MODE_V 0x3
#define MCPWM_GEN1_A_NCIFORCE_MODE_S 11
/* MCPWM_GEN1_A_NCIFORCE : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: non-continuous immediate sw force  a toggle will trigger a force event*/
#define MCPWM_GEN1_A_NCIFORCE (BIT(10))
#define MCPWM_GEN1_A_NCIFORCE_M (BIT(10))
#define MCPWM_GEN1_A_NCIFORCE_V 0x1
#define MCPWM_GEN1_A_NCIFORCE_S 10
/* MCPWM_GEN1_B_CNTUFORCE_MODE : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: 0: disabled  1: low  2: high  3: disabled*/
#define MCPWM_GEN1_B_CNTUFORCE_MODE 0x00000003
#define MCPWM_GEN1_B_CNTUFORCE_MODE_M ((MCPWM_GEN1_B_CNTUFORCE_MODE_V) << (MCPWM_GEN1_B_CNTUFORCE_MODE_S))
#define MCPWM_GEN1_B_CNTUFORCE_MODE_V 0x3
#define MCPWM_GEN1_B_CNTUFORCE_MODE_S 8
/* MCPWM_GEN1_A_CNTUFORCE_MODE : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: 0: disabled  1: low  2: high  3: disabled*/
#define MCPWM_GEN1_A_CNTUFORCE_MODE 0x00000003
#define MCPWM_GEN1_A_CNTUFORCE_MODE_M ((MCPWM_GEN1_A_CNTUFORCE_MODE_V) << (MCPWM_GEN1_A_CNTUFORCE_MODE_S))
#define MCPWM_GEN1_A_CNTUFORCE_MODE_V 0x3
#define MCPWM_GEN1_A_CNTUFORCE_MODE_S 6
/* MCPWM_GEN1_CNTUFORCE_UPMETHOD : R/W ;bitpos:[5:0] ;default: 6'h20 ; */
/*description: 0: immediate  bit0: tez  bit1: tep  bit2: tea  bit3: teb  bit4:
 sync  bit5: freeze*/
#define MCPWM_GEN1_CNTUFORCE_UPMETHOD 0x0000003F
#define MCPWM_GEN1_CNTUFORCE_UPMETHOD_M ((MCPWM_GEN1_CNTUFORCE_UPMETHOD_V) << (MCPWM_GEN1_CNTUFORCE_UPMETHOD_S))
#define MCPWM_GEN1_CNTUFORCE_UPMETHOD_V 0x3F
#define MCPWM_GEN1_CNTUFORCE_UPMETHOD_S 0

#define MCPWM_GEN1_A_REG(i) (REG_MCPWM_BASE(i) + 0x0088)
/* MCPWM_GEN1_A_DT1 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: 0: no change  1: low  2: high  3: toggle*/
#define MCPWM_GEN1_A_DT1 0x00000003
#define MCPWM_GEN1_A_DT1_M ((MCPWM_GEN1_A_DT1_V) << (MCPWM_GEN1_A_DT1_S))
#define MCPWM_GEN1_A_DT1_V 0x3
#define MCPWM_GEN1_A_DT1_S 22
/* MCPWM_GEN1_A_DT0 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_A_DT0 0x00000003
#define MCPWM_GEN1_A_DT0_M ((MCPWM_GEN1_A_DT0_V) << (MCPWM_GEN1_A_DT0_S))
#define MCPWM_GEN1_A_DT0_V 0x3
#define MCPWM_GEN1_A_DT0_S 20
/* MCPWM_GEN1_A_DTEB : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_A_DTEB 0x00000003
#define MCPWM_GEN1_A_DTEB_M ((MCPWM_GEN1_A_DTEB_V) << (MCPWM_GEN1_A_DTEB_S))
#define MCPWM_GEN1_A_DTEB_V 0x3
#define MCPWM_GEN1_A_DTEB_S 18
/* MCPWM_GEN1_A_DTEA : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_A_DTEA 0x00000003
#define MCPWM_GEN1_A_DTEA_M ((MCPWM_GEN1_A_DTEA_V) << (MCPWM_GEN1_A_DTEA_S))
#define MCPWM_GEN1_A_DTEA_V 0x3
#define MCPWM_GEN1_A_DTEA_S 16
/* MCPWM_GEN1_A_DTEP : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_A_DTEP 0x00000003
#define MCPWM_GEN1_A_DTEP_M ((MCPWM_GEN1_A_DTEP_V) << (MCPWM_GEN1_A_DTEP_S))
#define MCPWM_GEN1_A_DTEP_V 0x3
#define MCPWM_GEN1_A_DTEP_S 14
/* MCPWM_GEN1_A_DTEZ : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_A_DTEZ 0x00000003
#define MCPWM_GEN1_A_DTEZ_M ((MCPWM_GEN1_A_DTEZ_V) << (MCPWM_GEN1_A_DTEZ_S))
#define MCPWM_GEN1_A_DTEZ_V 0x3
#define MCPWM_GEN1_A_DTEZ_S 12
/* MCPWM_GEN1_A_UT1 : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_A_UT1 0x00000003
#define MCPWM_GEN1_A_UT1_M ((MCPWM_GEN1_A_UT1_V) << (MCPWM_GEN1_A_UT1_S))
#define MCPWM_GEN1_A_UT1_V 0x3
#define MCPWM_GEN1_A_UT1_S 10
/* MCPWM_GEN1_A_UT0 : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_A_UT0 0x00000003
#define MCPWM_GEN1_A_UT0_M ((MCPWM_GEN1_A_UT0_V) << (MCPWM_GEN1_A_UT0_S))
#define MCPWM_GEN1_A_UT0_V 0x3
#define MCPWM_GEN1_A_UT0_S 8
/* MCPWM_GEN1_A_UTEB : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_A_UTEB 0x00000003
#define MCPWM_GEN1_A_UTEB_M ((MCPWM_GEN1_A_UTEB_V) << (MCPWM_GEN1_A_UTEB_S))
#define MCPWM_GEN1_A_UTEB_V 0x3
#define MCPWM_GEN1_A_UTEB_S 6
/* MCPWM_GEN1_A_UTEA : R/W ;bitpos:[5:4] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_A_UTEA 0x00000003
#define MCPWM_GEN1_A_UTEA_M ((MCPWM_GEN1_A_UTEA_V) << (MCPWM_GEN1_A_UTEA_S))
#define MCPWM_GEN1_A_UTEA_V 0x3
#define MCPWM_GEN1_A_UTEA_S 4
/* MCPWM_GEN1_A_UTEP : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_A_UTEP 0x00000003
#define MCPWM_GEN1_A_UTEP_M ((MCPWM_GEN1_A_UTEP_V) << (MCPWM_GEN1_A_UTEP_S))
#define MCPWM_GEN1_A_UTEP_V 0x3
#define MCPWM_GEN1_A_UTEP_S 2
/* MCPWM_GEN1_A_UTEZ : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_A_UTEZ 0x00000003
#define MCPWM_GEN1_A_UTEZ_M ((MCPWM_GEN1_A_UTEZ_V) << (MCPWM_GEN1_A_UTEZ_S))
#define MCPWM_GEN1_A_UTEZ_V 0x3
#define MCPWM_GEN1_A_UTEZ_S 0

#define MCPWM_GEN1_B_REG(i) (REG_MCPWM_BASE(i) + 0x008c)
/* MCPWM_GEN1_B_DT1 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_B_DT1 0x00000003
#define MCPWM_GEN1_B_DT1_M ((MCPWM_GEN1_B_DT1_V) << (MCPWM_GEN1_B_DT1_S))
#define MCPWM_GEN1_B_DT1_V 0x3
#define MCPWM_GEN1_B_DT1_S 22
/* MCPWM_GEN1_B_DT0 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_B_DT0 0x00000003
#define MCPWM_GEN1_B_DT0_M ((MCPWM_GEN1_B_DT0_V) << (MCPWM_GEN1_B_DT0_S))
#define MCPWM_GEN1_B_DT0_V 0x3
#define MCPWM_GEN1_B_DT0_S 20
/* MCPWM_GEN1_B_DTEB : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_B_DTEB 0x00000003
#define MCPWM_GEN1_B_DTEB_M ((MCPWM_GEN1_B_DTEB_V) << (MCPWM_GEN1_B_DTEB_S))
#define MCPWM_GEN1_B_DTEB_V 0x3
#define MCPWM_GEN1_B_DTEB_S 18
/* MCPWM_GEN1_B_DTEA : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_B_DTEA 0x00000003
#define MCPWM_GEN1_B_DTEA_M ((MCPWM_GEN1_B_DTEA_V) << (MCPWM_GEN1_B_DTEA_S))
#define MCPWM_GEN1_B_DTEA_V 0x3
#define MCPWM_GEN1_B_DTEA_S 16
/* MCPWM_GEN1_B_DTEP : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_B_DTEP 0x00000003
#define MCPWM_GEN1_B_DTEP_M ((MCPWM_GEN1_B_DTEP_V) << (MCPWM_GEN1_B_DTEP_S))
#define MCPWM_GEN1_B_DTEP_V 0x3
#define MCPWM_GEN1_B_DTEP_S 14
/* MCPWM_GEN1_B_DTEZ : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_B_DTEZ 0x00000003
#define MCPWM_GEN1_B_DTEZ_M ((MCPWM_GEN1_B_DTEZ_V) << (MCPWM_GEN1_B_DTEZ_S))
#define MCPWM_GEN1_B_DTEZ_V 0x3
#define MCPWM_GEN1_B_DTEZ_S 12
/* MCPWM_GEN1_B_UT1 : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_B_UT1 0x00000003
#define MCPWM_GEN1_B_UT1_M ((MCPWM_GEN1_B_UT1_V) << (MCPWM_GEN1_B_UT1_S))
#define MCPWM_GEN1_B_UT1_V 0x3
#define MCPWM_GEN1_B_UT1_S 10
/* MCPWM_GEN1_B_UT0 : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_B_UT0 0x00000003
#define MCPWM_GEN1_B_UT0_M ((MCPWM_GEN1_B_UT0_V) << (MCPWM_GEN1_B_UT0_S))
#define MCPWM_GEN1_B_UT0_V 0x3
#define MCPWM_GEN1_B_UT0_S 8
/* MCPWM_GEN1_B_UTEB : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_B_UTEB 0x00000003
#define MCPWM_GEN1_B_UTEB_M ((MCPWM_GEN1_B_UTEB_V) << (MCPWM_GEN1_B_UTEB_S))
#define MCPWM_GEN1_B_UTEB_V 0x3
#define MCPWM_GEN1_B_UTEB_S 6
/* MCPWM_GEN1_B_UTEA : R/W ;bitpos:[5:4] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_B_UTEA 0x00000003
#define MCPWM_GEN1_B_UTEA_M ((MCPWM_GEN1_B_UTEA_V) << (MCPWM_GEN1_B_UTEA_S))
#define MCPWM_GEN1_B_UTEA_V 0x3
#define MCPWM_GEN1_B_UTEA_S 4
/* MCPWM_GEN1_B_UTEP : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_B_UTEP 0x00000003
#define MCPWM_GEN1_B_UTEP_M ((MCPWM_GEN1_B_UTEP_V) << (MCPWM_GEN1_B_UTEP_S))
#define MCPWM_GEN1_B_UTEP_V 0x3
#define MCPWM_GEN1_B_UTEP_S 2
/* MCPWM_GEN1_B_UTEZ : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN1_B_UTEZ 0x00000003
#define MCPWM_GEN1_B_UTEZ_M ((MCPWM_GEN1_B_UTEZ_V) << (MCPWM_GEN1_B_UTEZ_S))
#define MCPWM_GEN1_B_UTEZ_V 0x3
#define MCPWM_GEN1_B_UTEZ_S 0

#define MCPWM_DB1_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x0090)
/* MCPWM_DB1_CLK_SEL : R/W ;bitpos:[17] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB1_CLK_SEL (BIT(17))
#define MCPWM_DB1_CLK_SEL_M (BIT(17))
#define MCPWM_DB1_CLK_SEL_V 0x1
#define MCPWM_DB1_CLK_SEL_S 17
/* MCPWM_DB1_B_OUTBYPASS : R/W ;bitpos:[16] ;default: 1'd1 ; */
/*description: */
#define MCPWM_DB1_B_OUTBYPASS (BIT(16))
#define MCPWM_DB1_B_OUTBYPASS_M (BIT(16))
#define MCPWM_DB1_B_OUTBYPASS_V 0x1
#define MCPWM_DB1_B_OUTBYPASS_S 16
/* MCPWM_DB1_A_OUTBYPASS : R/W ;bitpos:[15] ;default: 1'd1 ; */
/*description: */
#define MCPWM_DB1_A_OUTBYPASS (BIT(15))
#define MCPWM_DB1_A_OUTBYPASS_M (BIT(15))
#define MCPWM_DB1_A_OUTBYPASS_V 0x1
#define MCPWM_DB1_A_OUTBYPASS_S 15
/* MCPWM_DB1_FED_OUTINVERT : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB1_FED_OUTINVERT (BIT(14))
#define MCPWM_DB1_FED_OUTINVERT_M (BIT(14))
#define MCPWM_DB1_FED_OUTINVERT_V 0x1
#define MCPWM_DB1_FED_OUTINVERT_S 14
/* MCPWM_DB1_RED_OUTINVERT : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB1_RED_OUTINVERT (BIT(13))
#define MCPWM_DB1_RED_OUTINVERT_M (BIT(13))
#define MCPWM_DB1_RED_OUTINVERT_V 0x1
#define MCPWM_DB1_RED_OUTINVERT_S 13
/* MCPWM_DB1_FED_INSEL : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB1_FED_INSEL (BIT(12))
#define MCPWM_DB1_FED_INSEL_M (BIT(12))
#define MCPWM_DB1_FED_INSEL_V 0x1
#define MCPWM_DB1_FED_INSEL_S 12
/* MCPWM_DB1_RED_INSEL : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB1_RED_INSEL (BIT(11))
#define MCPWM_DB1_RED_INSEL_M (BIT(11))
#define MCPWM_DB1_RED_INSEL_V 0x1
#define MCPWM_DB1_RED_INSEL_S 11
/* MCPWM_DB1_B_OUTSWAP : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB1_B_OUTSWAP (BIT(10))
#define MCPWM_DB1_B_OUTSWAP_M (BIT(10))
#define MCPWM_DB1_B_OUTSWAP_V 0x1
#define MCPWM_DB1_B_OUTSWAP_S 10
/* MCPWM_DB1_A_OUTSWAP : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB1_A_OUTSWAP (BIT(9))
#define MCPWM_DB1_A_OUTSWAP_M (BIT(9))
#define MCPWM_DB1_A_OUTSWAP_V 0x1
#define MCPWM_DB1_A_OUTSWAP_S 9
/* MCPWM_DB1_DEB_MODE : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB1_DEB_MODE (BIT(8))
#define MCPWM_DB1_DEB_MODE_M (BIT(8))
#define MCPWM_DB1_DEB_MODE_V 0x1
#define MCPWM_DB1_DEB_MODE_S 8
/* MCPWM_DB1_RED_UPMETHOD : R/W ;bitpos:[7:4] ;default: 4'd0 ; */
/*description: */
#define MCPWM_DB1_RED_UPMETHOD 0x0000000F
#define MCPWM_DB1_RED_UPMETHOD_M ((MCPWM_DB1_RED_UPMETHOD_V) << (MCPWM_DB1_RED_UPMETHOD_S))
#define MCPWM_DB1_RED_UPMETHOD_V 0xF
#define MCPWM_DB1_RED_UPMETHOD_S 4
/* MCPWM_DB1_FED_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: */
#define MCPWM_DB1_FED_UPMETHOD 0x0000000F
#define MCPWM_DB1_FED_UPMETHOD_M ((MCPWM_DB1_FED_UPMETHOD_V) << (MCPWM_DB1_FED_UPMETHOD_S))
#define MCPWM_DB1_FED_UPMETHOD_V 0xF
#define MCPWM_DB1_FED_UPMETHOD_S 0

#define MCPWM_DB1_FED_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x0094)
/* MCPWM_DB1_FED : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define MCPWM_DB1_FED 0x0000FFFF
#define MCPWM_DB1_FED_M ((MCPWM_DB1_FED_V) << (MCPWM_DB1_FED_S))
#define MCPWM_DB1_FED_V 0xFFFF
#define MCPWM_DB1_FED_S 0

#define MCPWM_DB1_RED_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x0098)
/* MCPWM_DB1_RED : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define MCPWM_DB1_RED 0x0000FFFF
#define MCPWM_DB1_RED_M ((MCPWM_DB1_RED_V) << (MCPWM_DB1_RED_S))
#define MCPWM_DB1_RED_V 0xFFFF
#define MCPWM_DB1_RED_S 0

#define MCPWM_CHOPPER1_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x009c)
/* MCPWM_CHOPPER1_IN_INVERT : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CHOPPER1_IN_INVERT (BIT(13))
#define MCPWM_CHOPPER1_IN_INVERT_M (BIT(13))
#define MCPWM_CHOPPER1_IN_INVERT_V 0x1
#define MCPWM_CHOPPER1_IN_INVERT_S 13
/* MCPWM_CHOPPER1_OUT_INVERT : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CHOPPER1_OUT_INVERT (BIT(12))
#define MCPWM_CHOPPER1_OUT_INVERT_M (BIT(12))
#define MCPWM_CHOPPER1_OUT_INVERT_V 0x1
#define MCPWM_CHOPPER1_OUT_INVERT_S 12
/* MCPWM_CHOPPER1_OSHTWTH : R/W ;bitpos:[11:8] ;default: 4'd0 ; */
/*description: */
#define MCPWM_CHOPPER1_OSHTWTH 0x0000000F
#define MCPWM_CHOPPER1_OSHTWTH_M ((MCPWM_CHOPPER1_OSHTWTH_V) << (MCPWM_CHOPPER1_OSHTWTH_S))
#define MCPWM_CHOPPER1_OSHTWTH_V 0xF
#define MCPWM_CHOPPER1_OSHTWTH_S 8
/* MCPWM_CHOPPER1_DUTY : R/W ;bitpos:[7:5] ;default: 3'd0 ; */
/*description: */
#define MCPWM_CHOPPER1_DUTY 0x00000007
#define MCPWM_CHOPPER1_DUTY_M ((MCPWM_CHOPPER1_DUTY_V) << (MCPWM_CHOPPER1_DUTY_S))
#define MCPWM_CHOPPER1_DUTY_V 0x7
#define MCPWM_CHOPPER1_DUTY_S 5
/* MCPWM_CHOPPER1_PRESCALE : R/W ;bitpos:[4:1] ;default: 4'd0 ; */
/*description: */
#define MCPWM_CHOPPER1_PRESCALE 0x0000000F
#define MCPWM_CHOPPER1_PRESCALE_M ((MCPWM_CHOPPER1_PRESCALE_V) << (MCPWM_CHOPPER1_PRESCALE_S))
#define MCPWM_CHOPPER1_PRESCALE_V 0xF
#define MCPWM_CHOPPER1_PRESCALE_S 1
/* MCPWM_CHOPPER1_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CHOPPER1_EN (BIT(0))
#define MCPWM_CHOPPER1_EN_M (BIT(0))
#define MCPWM_CHOPPER1_EN_V 0x1
#define MCPWM_CHOPPER1_EN_S 0

#define MCPWM_TZ1_CFG0_REG(i) (REG_MCPWM_BASE(i) + 0x00a0)
/* MCPWM_TZ1_B_OST_U : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ1_B_OST_U 0x00000003
#define MCPWM_TZ1_B_OST_U_M ((MCPWM_TZ1_B_OST_U_V) << (MCPWM_TZ1_B_OST_U_S))
#define MCPWM_TZ1_B_OST_U_V 0x3
#define MCPWM_TZ1_B_OST_U_S 22
/* MCPWM_TZ1_B_OST_D : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ1_B_OST_D 0x00000003
#define MCPWM_TZ1_B_OST_D_M ((MCPWM_TZ1_B_OST_D_V) << (MCPWM_TZ1_B_OST_D_S))
#define MCPWM_TZ1_B_OST_D_V 0x3
#define MCPWM_TZ1_B_OST_D_S 20
/* MCPWM_TZ1_B_CBC_U : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ1_B_CBC_U 0x00000003
#define MCPWM_TZ1_B_CBC_U_M ((MCPWM_TZ1_B_CBC_U_V) << (MCPWM_TZ1_B_CBC_U_S))
#define MCPWM_TZ1_B_CBC_U_V 0x3
#define MCPWM_TZ1_B_CBC_U_S 18
/* MCPWM_TZ1_B_CBC_D : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ1_B_CBC_D 0x00000003
#define MCPWM_TZ1_B_CBC_D_M ((MCPWM_TZ1_B_CBC_D_V) << (MCPWM_TZ1_B_CBC_D_S))
#define MCPWM_TZ1_B_CBC_D_V 0x3
#define MCPWM_TZ1_B_CBC_D_S 16
/* MCPWM_TZ1_A_OST_U : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ1_A_OST_U 0x00000003
#define MCPWM_TZ1_A_OST_U_M ((MCPWM_TZ1_A_OST_U_V) << (MCPWM_TZ1_A_OST_U_S))
#define MCPWM_TZ1_A_OST_U_V 0x3
#define MCPWM_TZ1_A_OST_U_S 14
/* MCPWM_TZ1_A_OST_D : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ1_A_OST_D 0x00000003
#define MCPWM_TZ1_A_OST_D_M ((MCPWM_TZ1_A_OST_D_V) << (MCPWM_TZ1_A_OST_D_S))
#define MCPWM_TZ1_A_OST_D_V 0x3
#define MCPWM_TZ1_A_OST_D_S 12
/* MCPWM_TZ1_A_CBC_U : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ1_A_CBC_U 0x00000003
#define MCPWM_TZ1_A_CBC_U_M ((MCPWM_TZ1_A_CBC_U_V) << (MCPWM_TZ1_A_CBC_U_S))
#define MCPWM_TZ1_A_CBC_U_V 0x3
#define MCPWM_TZ1_A_CBC_U_S 10
/* MCPWM_TZ1_A_CBC_D : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ1_A_CBC_D 0x00000003
#define MCPWM_TZ1_A_CBC_D_M ((MCPWM_TZ1_A_CBC_D_V) << (MCPWM_TZ1_A_CBC_D_S))
#define MCPWM_TZ1_A_CBC_D_V 0x3
#define MCPWM_TZ1_A_CBC_D_S 8
/* MCPWM_TZ1_F0_OST : R/W ;bitpos:[7] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ1_F0_OST (BIT(7))
#define MCPWM_TZ1_F0_OST_M (BIT(7))
#define MCPWM_TZ1_F0_OST_V 0x1
#define MCPWM_TZ1_F0_OST_S 7
/* MCPWM_TZ1_F1_OST : R/W ;bitpos:[6] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ1_F1_OST (BIT(6))
#define MCPWM_TZ1_F1_OST_M (BIT(6))
#define MCPWM_TZ1_F1_OST_V 0x1
#define MCPWM_TZ1_F1_OST_S 6
/* MCPWM_TZ1_F2_OST : R/W ;bitpos:[5] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ1_F2_OST (BIT(5))
#define MCPWM_TZ1_F2_OST_M (BIT(5))
#define MCPWM_TZ1_F2_OST_V 0x1
#define MCPWM_TZ1_F2_OST_S 5
/* MCPWM_TZ1_SW_OST : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ1_SW_OST (BIT(4))
#define MCPWM_TZ1_SW_OST_M (BIT(4))
#define MCPWM_TZ1_SW_OST_V 0x1
#define MCPWM_TZ1_SW_OST_S 4
/* MCPWM_TZ1_F0_CBC : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ1_F0_CBC (BIT(3))
#define MCPWM_TZ1_F0_CBC_M (BIT(3))
#define MCPWM_TZ1_F0_CBC_V 0x1
#define MCPWM_TZ1_F0_CBC_S 3
/* MCPWM_TZ1_F1_CBC : R/W ;bitpos:[2] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ1_F1_CBC (BIT(2))
#define MCPWM_TZ1_F1_CBC_M (BIT(2))
#define MCPWM_TZ1_F1_CBC_V 0x1
#define MCPWM_TZ1_F1_CBC_S 2
/* MCPWM_TZ1_F2_CBC : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ1_F2_CBC (BIT(1))
#define MCPWM_TZ1_F2_CBC_M (BIT(1))
#define MCPWM_TZ1_F2_CBC_V 0x1
#define MCPWM_TZ1_F2_CBC_S 1
/* MCPWM_TZ1_SW_CBC : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ1_SW_CBC (BIT(0))
#define MCPWM_TZ1_SW_CBC_M (BIT(0))
#define MCPWM_TZ1_SW_CBC_V 0x1
#define MCPWM_TZ1_SW_CBC_S 0

#define MCPWM_TZ1_CFG1_REG(i) (REG_MCPWM_BASE(i) + 0x00a4)
/* MCPWM_TZ1_FORCE_OST : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: a toggle trigger a oneshot tripping*/
#define MCPWM_TZ1_FORCE_OST (BIT(4))
#define MCPWM_TZ1_FORCE_OST_M (BIT(4))
#define MCPWM_TZ1_FORCE_OST_V 0x1
#define MCPWM_TZ1_FORCE_OST_S 4
/* MCPWM_TZ1_FORCE_CBC : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: a toggle trigger a cycle-by-cycle tripping*/
#define MCPWM_TZ1_FORCE_CBC (BIT(3))
#define MCPWM_TZ1_FORCE_CBC_M (BIT(3))
#define MCPWM_TZ1_FORCE_CBC_V 0x1
#define MCPWM_TZ1_FORCE_CBC_S 3
/* MCPWM_TZ1_CBCPULSE : R/W ;bitpos:[2:1] ;default: 2'd0 ; */
/*description: bit0: tez  bit1: tep*/
#define MCPWM_TZ1_CBCPULSE 0x00000003
#define MCPWM_TZ1_CBCPULSE_M ((MCPWM_TZ1_CBCPULSE_V) << (MCPWM_TZ1_CBCPULSE_S))
#define MCPWM_TZ1_CBCPULSE_V 0x3
#define MCPWM_TZ1_CBCPULSE_S 1
/* MCPWM_TZ1_CLR_OST : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: a toggle will clear oneshot tripping*/
#define MCPWM_TZ1_CLR_OST (BIT(0))
#define MCPWM_TZ1_CLR_OST_M (BIT(0))
#define MCPWM_TZ1_CLR_OST_V 0x1
#define MCPWM_TZ1_CLR_OST_S 0

#define MCPWM_TZ1_STATUS_REG(i) (REG_MCPWM_BASE(i) + 0x00a8)
/* MCPWM_TZ1_OST_ON : RO ;bitpos:[1] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ1_OST_ON (BIT(1))
#define MCPWM_TZ1_OST_ON_M (BIT(1))
#define MCPWM_TZ1_OST_ON_V 0x1
#define MCPWM_TZ1_OST_ON_S 1
/* MCPWM_TZ1_CBC_ON : RO ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ1_CBC_ON (BIT(0))
#define MCPWM_TZ1_CBC_ON_M (BIT(0))
#define MCPWM_TZ1_CBC_ON_V 0x1
#define MCPWM_TZ1_CBC_ON_S 0

#define MCPWM_CMPR2_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x00ac)
/* MCPWM_CMPR2_B_SHDW_FULL : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR2_B_SHDW_FULL (BIT(9))
#define MCPWM_CMPR2_B_SHDW_FULL_M (BIT(9))
#define MCPWM_CMPR2_B_SHDW_FULL_V 0x1
#define MCPWM_CMPR2_B_SHDW_FULL_S 9
/* MCPWM_CMPR2_A_SHDW_FULL : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR2_A_SHDW_FULL (BIT(8))
#define MCPWM_CMPR2_A_SHDW_FULL_M (BIT(8))
#define MCPWM_CMPR2_A_SHDW_FULL_V 0x1
#define MCPWM_CMPR2_A_SHDW_FULL_S 8
/* MCPWM_CMPR2_B_UPMETHOD : R/W ;bitpos:[7:4] ;default: 4'd0 ; */
/*description: 0: immediate  bit0: tez  bit1: tep  bit2: sync  bit3: freeze*/
#define MCPWM_CMPR2_B_UPMETHOD 0x0000000F
#define MCPWM_CMPR2_B_UPMETHOD_M ((MCPWM_CMPR2_B_UPMETHOD_V) << (MCPWM_CMPR2_B_UPMETHOD_S))
#define MCPWM_CMPR2_B_UPMETHOD_V 0xF
#define MCPWM_CMPR2_B_UPMETHOD_S 4
/* MCPWM_CMPR2_A_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: 0: immediate  bit0: tez  bit1: tep  bit2: sync  bit3: freeze*/
#define MCPWM_CMPR2_A_UPMETHOD 0x0000000F
#define MCPWM_CMPR2_A_UPMETHOD_M ((MCPWM_CMPR2_A_UPMETHOD_V) << (MCPWM_CMPR2_A_UPMETHOD_S))
#define MCPWM_CMPR2_A_UPMETHOD_V 0xF
#define MCPWM_CMPR2_A_UPMETHOD_S 0

#define MCPWM_CMPR2_VALUE0_REG(i) (REG_MCPWM_BASE(i) + 0x00b0)
/* MCPWM_CMPR2_A : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define MCPWM_CMPR2_A 0x0000FFFF
#define MCPWM_CMPR2_A_M ((MCPWM_CMPR2_A_V) << (MCPWM_CMPR2_A_S))
#define MCPWM_CMPR2_A_V 0xFFFF
#define MCPWM_CMPR2_A_S 0

#define MCPWM_CMPR2_VALUE1_REG(i) (REG_MCPWM_BASE(i) + 0x00b4)
/* MCPWM_CMPR2_B : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define MCPWM_CMPR2_B 0x0000FFFF
#define MCPWM_CMPR2_B_M ((MCPWM_CMPR2_B_V) << (MCPWM_CMPR2_B_S))
#define MCPWM_CMPR2_B_V 0xFFFF
#define MCPWM_CMPR2_B_S 0

#define MCPWM_GEN2_CFG0_REG(i) (REG_MCPWM_BASE(i) + 0x00b8)
/* MCPWM_GEN2_T1_SEL : R/W ;bitpos:[9:7] ;default: 3'd0 ; */
/*description: take effect immediately  0: extra0  1: extra1  2: extra2  3: sync_taken  4: none*/
#define MCPWM_GEN2_T1_SEL 0x00000007
#define MCPWM_GEN2_T1_SEL_M ((MCPWM_GEN2_T1_SEL_V) << (MCPWM_GEN2_T1_SEL_S))
#define MCPWM_GEN2_T1_SEL_V 0x7
#define MCPWM_GEN2_T1_SEL_S 7
/* MCPWM_GEN2_T0_SEL : R/W ;bitpos:[6:4] ;default: 3'd0 ; */
/*description: take effect immediately  0: extra0  1: extra1  2: extra2  3: sync_taken  4: none*/
#define MCPWM_GEN2_T0_SEL 0x00000007
#define MCPWM_GEN2_T0_SEL_M ((MCPWM_GEN2_T0_SEL_V) << (MCPWM_GEN2_T0_SEL_S))
#define MCPWM_GEN2_T0_SEL_V 0x7
#define MCPWM_GEN2_T0_SEL_S 4
/* MCPWM_GEN2_CFG_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: 0: immediate  bit0: tez  bit1: tep  bit2: sync. bit3: freeze*/
#define MCPWM_GEN2_CFG_UPMETHOD 0x0000000F
#define MCPWM_GEN2_CFG_UPMETHOD_M ((MCPWM_GEN2_CFG_UPMETHOD_V) << (MCPWM_GEN2_CFG_UPMETHOD_S))
#define MCPWM_GEN2_CFG_UPMETHOD_V 0xF
#define MCPWM_GEN2_CFG_UPMETHOD_S 0

#define MCPWM_GEN2_FORCE_REG(i) (REG_MCPWM_BASE(i) + 0x00bc)
/* MCPWM_GEN2_B_NCIFORCE_MODE : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: 0: disabled  1: low  2: high  3: disabled*/
#define MCPWM_GEN2_B_NCIFORCE_MODE 0x00000003
#define MCPWM_GEN2_B_NCIFORCE_MODE_M ((MCPWM_GEN2_B_NCIFORCE_MODE_V) << (MCPWM_GEN2_B_NCIFORCE_MODE_S))
#define MCPWM_GEN2_B_NCIFORCE_MODE_V 0x3
#define MCPWM_GEN2_B_NCIFORCE_MODE_S 14
/* MCPWM_GEN2_B_NCIFORCE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: non-continuous immediate sw force  a toggle will trigger a force event*/
#define MCPWM_GEN2_B_NCIFORCE (BIT(13))
#define MCPWM_GEN2_B_NCIFORCE_M (BIT(13))
#define MCPWM_GEN2_B_NCIFORCE_V 0x1
#define MCPWM_GEN2_B_NCIFORCE_S 13
/* MCPWM_GEN2_A_NCIFORCE_MODE : R/W ;bitpos:[12:11] ;default: 2'd0 ; */
/*description: 0: disabled  1: low  2: high  3: disabled*/
#define MCPWM_GEN2_A_NCIFORCE_MODE 0x00000003
#define MCPWM_GEN2_A_NCIFORCE_MODE_M ((MCPWM_GEN2_A_NCIFORCE_MODE_V) << (MCPWM_GEN2_A_NCIFORCE_MODE_S))
#define MCPWM_GEN2_A_NCIFORCE_MODE_V 0x3
#define MCPWM_GEN2_A_NCIFORCE_MODE_S 11
/* MCPWM_GEN2_A_NCIFORCE : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: non-continuous immediate sw force  a toggle will trigger a force event*/
#define MCPWM_GEN2_A_NCIFORCE (BIT(10))
#define MCPWM_GEN2_A_NCIFORCE_M (BIT(10))
#define MCPWM_GEN2_A_NCIFORCE_V 0x1
#define MCPWM_GEN2_A_NCIFORCE_S 10
/* MCPWM_GEN2_B_CNTUFORCE_MODE : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: 0: disabled  1: low  2: high  3: disabled*/
#define MCPWM_GEN2_B_CNTUFORCE_MODE 0x00000003
#define MCPWM_GEN2_B_CNTUFORCE_MODE_M ((MCPWM_GEN2_B_CNTUFORCE_MODE_V) << (MCPWM_GEN2_B_CNTUFORCE_MODE_S))
#define MCPWM_GEN2_B_CNTUFORCE_MODE_V 0x3
#define MCPWM_GEN2_B_CNTUFORCE_MODE_S 8
/* MCPWM_GEN2_A_CNTUFORCE_MODE : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: 0: disabled  1: low  2: high  3: disabled*/
#define MCPWM_GEN2_A_CNTUFORCE_MODE 0x00000003
#define MCPWM_GEN2_A_CNTUFORCE_MODE_M ((MCPWM_GEN2_A_CNTUFORCE_MODE_V) << (MCPWM_GEN2_A_CNTUFORCE_MODE_S))
#define MCPWM_GEN2_A_CNTUFORCE_MODE_V 0x3
#define MCPWM_GEN2_A_CNTUFORCE_MODE_S 6
/* MCPWM_GEN2_CNTUFORCE_UPMETHOD : R/W ;bitpos:[5:0] ;default: 6'h20 ; */
/*description: 0: immediate  bit0: tez  bit1: tep  bit2: tea  bit3: teb  bit4:
 sync  bit5: freeze*/
#define MCPWM_GEN2_CNTUFORCE_UPMETHOD 0x0000003F
#define MCPWM_GEN2_CNTUFORCE_UPMETHOD_M ((MCPWM_GEN2_CNTUFORCE_UPMETHOD_V) << (MCPWM_GEN2_CNTUFORCE_UPMETHOD_S))
#define MCPWM_GEN2_CNTUFORCE_UPMETHOD_V 0x3F
#define MCPWM_GEN2_CNTUFORCE_UPMETHOD_S 0

#define MCPWM_GEN2_A_REG(i) (REG_MCPWM_BASE(i) + 0x00c0)
/* MCPWM_GEN2_A_DT1 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: 0: no change  1: low  2: high  3: toggle*/
#define MCPWM_GEN2_A_DT1 0x00000003
#define MCPWM_GEN2_A_DT1_M ((MCPWM_GEN2_A_DT1_V) << (MCPWM_GEN2_A_DT1_S))
#define MCPWM_GEN2_A_DT1_V 0x3
#define MCPWM_GEN2_A_DT1_S 22
/* MCPWM_GEN2_A_DT0 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_A_DT0 0x00000003
#define MCPWM_GEN2_A_DT0_M ((MCPWM_GEN2_A_DT0_V) << (MCPWM_GEN2_A_DT0_S))
#define MCPWM_GEN2_A_DT0_V 0x3
#define MCPWM_GEN2_A_DT0_S 20
/* MCPWM_GEN2_A_DTEB : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_A_DTEB 0x00000003
#define MCPWM_GEN2_A_DTEB_M ((MCPWM_GEN2_A_DTEB_V) << (MCPWM_GEN2_A_DTEB_S))
#define MCPWM_GEN2_A_DTEB_V 0x3
#define MCPWM_GEN2_A_DTEB_S 18
/* MCPWM_GEN2_A_DTEA : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_A_DTEA 0x00000003
#define MCPWM_GEN2_A_DTEA_M ((MCPWM_GEN2_A_DTEA_V) << (MCPWM_GEN2_A_DTEA_S))
#define MCPWM_GEN2_A_DTEA_V 0x3
#define MCPWM_GEN2_A_DTEA_S 16
/* MCPWM_GEN2_A_DTEP : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_A_DTEP 0x00000003
#define MCPWM_GEN2_A_DTEP_M ((MCPWM_GEN2_A_DTEP_V) << (MCPWM_GEN2_A_DTEP_S))
#define MCPWM_GEN2_A_DTEP_V 0x3
#define MCPWM_GEN2_A_DTEP_S 14
/* MCPWM_GEN2_A_DTEZ : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_A_DTEZ 0x00000003
#define MCPWM_GEN2_A_DTEZ_M ((MCPWM_GEN2_A_DTEZ_V) << (MCPWM_GEN2_A_DTEZ_S))
#define MCPWM_GEN2_A_DTEZ_V 0x3
#define MCPWM_GEN2_A_DTEZ_S 12
/* MCPWM_GEN2_A_UT1 : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_A_UT1 0x00000003
#define MCPWM_GEN2_A_UT1_M ((MCPWM_GEN2_A_UT1_V) << (MCPWM_GEN2_A_UT1_S))
#define MCPWM_GEN2_A_UT1_V 0x3
#define MCPWM_GEN2_A_UT1_S 10
/* MCPWM_GEN2_A_UT0 : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_A_UT0 0x00000003
#define MCPWM_GEN2_A_UT0_M ((MCPWM_GEN2_A_UT0_V) << (MCPWM_GEN2_A_UT0_S))
#define MCPWM_GEN2_A_UT0_V 0x3
#define MCPWM_GEN2_A_UT0_S 8
/* MCPWM_GEN2_A_UTEB : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_A_UTEB 0x00000003
#define MCPWM_GEN2_A_UTEB_M ((MCPWM_GEN2_A_UTEB_V) << (MCPWM_GEN2_A_UTEB_S))
#define MCPWM_GEN2_A_UTEB_V 0x3
#define MCPWM_GEN2_A_UTEB_S 6
/* MCPWM_GEN2_A_UTEA : R/W ;bitpos:[5:4] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_A_UTEA 0x00000003
#define MCPWM_GEN2_A_UTEA_M ((MCPWM_GEN2_A_UTEA_V) << (MCPWM_GEN2_A_UTEA_S))
#define MCPWM_GEN2_A_UTEA_V 0x3
#define MCPWM_GEN2_A_UTEA_S 4
/* MCPWM_GEN2_A_UTEP : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_A_UTEP 0x00000003
#define MCPWM_GEN2_A_UTEP_M ((MCPWM_GEN2_A_UTEP_V) << (MCPWM_GEN2_A_UTEP_S))
#define MCPWM_GEN2_A_UTEP_V 0x3
#define MCPWM_GEN2_A_UTEP_S 2
/* MCPWM_GEN2_A_UTEZ : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_A_UTEZ 0x00000003
#define MCPWM_GEN2_A_UTEZ_M ((MCPWM_GEN2_A_UTEZ_V) << (MCPWM_GEN2_A_UTEZ_S))
#define MCPWM_GEN2_A_UTEZ_V 0x3
#define MCPWM_GEN2_A_UTEZ_S 0

#define MCPWM_GEN2_B_REG(i) (REG_MCPWM_BASE(i) + 0x00c4)
/* MCPWM_GEN2_B_DT1 : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_B_DT1 0x00000003
#define MCPWM_GEN2_B_DT1_M ((MCPWM_GEN2_B_DT1_V) << (MCPWM_GEN2_B_DT1_S))
#define MCPWM_GEN2_B_DT1_V 0x3
#define MCPWM_GEN2_B_DT1_S 22
/* MCPWM_GEN2_B_DT0 : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_B_DT0 0x00000003
#define MCPWM_GEN2_B_DT0_M ((MCPWM_GEN2_B_DT0_V) << (MCPWM_GEN2_B_DT0_S))
#define MCPWM_GEN2_B_DT0_V 0x3
#define MCPWM_GEN2_B_DT0_S 20
/* MCPWM_GEN2_B_DTEB : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_B_DTEB 0x00000003
#define MCPWM_GEN2_B_DTEB_M ((MCPWM_GEN2_B_DTEB_V) << (MCPWM_GEN2_B_DTEB_S))
#define MCPWM_GEN2_B_DTEB_V 0x3
#define MCPWM_GEN2_B_DTEB_S 18
/* MCPWM_GEN2_B_DTEA : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_B_DTEA 0x00000003
#define MCPWM_GEN2_B_DTEA_M ((MCPWM_GEN2_B_DTEA_V) << (MCPWM_GEN2_B_DTEA_S))
#define MCPWM_GEN2_B_DTEA_V 0x3
#define MCPWM_GEN2_B_DTEA_S 16
/* MCPWM_GEN2_B_DTEP : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_B_DTEP 0x00000003
#define MCPWM_GEN2_B_DTEP_M ((MCPWM_GEN2_B_DTEP_V) << (MCPWM_GEN2_B_DTEP_S))
#define MCPWM_GEN2_B_DTEP_V 0x3
#define MCPWM_GEN2_B_DTEP_S 14
/* MCPWM_GEN2_B_DTEZ : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_B_DTEZ 0x00000003
#define MCPWM_GEN2_B_DTEZ_M ((MCPWM_GEN2_B_DTEZ_V) << (MCPWM_GEN2_B_DTEZ_S))
#define MCPWM_GEN2_B_DTEZ_V 0x3
#define MCPWM_GEN2_B_DTEZ_S 12
/* MCPWM_GEN2_B_UT1 : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_B_UT1 0x00000003
#define MCPWM_GEN2_B_UT1_M ((MCPWM_GEN2_B_UT1_V) << (MCPWM_GEN2_B_UT1_S))
#define MCPWM_GEN2_B_UT1_V 0x3
#define MCPWM_GEN2_B_UT1_S 10
/* MCPWM_GEN2_B_UT0 : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_B_UT0 0x00000003
#define MCPWM_GEN2_B_UT0_M ((MCPWM_GEN2_B_UT0_V) << (MCPWM_GEN2_B_UT0_S))
#define MCPWM_GEN2_B_UT0_V 0x3
#define MCPWM_GEN2_B_UT0_S 8
/* MCPWM_GEN2_B_UTEB : R/W ;bitpos:[7:6] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_B_UTEB 0x00000003
#define MCPWM_GEN2_B_UTEB_M ((MCPWM_GEN2_B_UTEB_V) << (MCPWM_GEN2_B_UTEB_S))
#define MCPWM_GEN2_B_UTEB_V 0x3
#define MCPWM_GEN2_B_UTEB_S 6
/* MCPWM_GEN2_B_UTEA : R/W ;bitpos:[5:4] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_B_UTEA 0x00000003
#define MCPWM_GEN2_B_UTEA_M ((MCPWM_GEN2_B_UTEA_V) << (MCPWM_GEN2_B_UTEA_S))
#define MCPWM_GEN2_B_UTEA_V 0x3
#define MCPWM_GEN2_B_UTEA_S 4
/* MCPWM_GEN2_B_UTEP : R/W ;bitpos:[3:2] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_B_UTEP 0x00000003
#define MCPWM_GEN2_B_UTEP_M ((MCPWM_GEN2_B_UTEP_V) << (MCPWM_GEN2_B_UTEP_S))
#define MCPWM_GEN2_B_UTEP_V 0x3
#define MCPWM_GEN2_B_UTEP_S 2
/* MCPWM_GEN2_B_UTEZ : R/W ;bitpos:[1:0] ;default: 2'd0 ; */
/*description: */
#define MCPWM_GEN2_B_UTEZ 0x00000003
#define MCPWM_GEN2_B_UTEZ_M ((MCPWM_GEN2_B_UTEZ_V) << (MCPWM_GEN2_B_UTEZ_S))
#define MCPWM_GEN2_B_UTEZ_V 0x3
#define MCPWM_GEN2_B_UTEZ_S 0

#define MCPWM_DB2_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x00c8)
/* MCPWM_DB2_CLK_SEL : R/W ;bitpos:[17] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB2_CLK_SEL (BIT(17))
#define MCPWM_DB2_CLK_SEL_M (BIT(17))
#define MCPWM_DB2_CLK_SEL_V 0x1
#define MCPWM_DB2_CLK_SEL_S 17
/* MCPWM_DB2_B_OUTBYPASS : R/W ;bitpos:[16] ;default: 1'd1 ; */
/*description: */
#define MCPWM_DB2_B_OUTBYPASS (BIT(16))
#define MCPWM_DB2_B_OUTBYPASS_M (BIT(16))
#define MCPWM_DB2_B_OUTBYPASS_V 0x1
#define MCPWM_DB2_B_OUTBYPASS_S 16
/* MCPWM_DB2_A_OUTBYPASS : R/W ;bitpos:[15] ;default: 1'd1 ; */
/*description: */
#define MCPWM_DB2_A_OUTBYPASS (BIT(15))
#define MCPWM_DB2_A_OUTBYPASS_M (BIT(15))
#define MCPWM_DB2_A_OUTBYPASS_V 0x1
#define MCPWM_DB2_A_OUTBYPASS_S 15
/* MCPWM_DB2_FED_OUTINVERT : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB2_FED_OUTINVERT (BIT(14))
#define MCPWM_DB2_FED_OUTINVERT_M (BIT(14))
#define MCPWM_DB2_FED_OUTINVERT_V 0x1
#define MCPWM_DB2_FED_OUTINVERT_S 14
/* MCPWM_DB2_RED_OUTINVERT : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB2_RED_OUTINVERT (BIT(13))
#define MCPWM_DB2_RED_OUTINVERT_M (BIT(13))
#define MCPWM_DB2_RED_OUTINVERT_V 0x1
#define MCPWM_DB2_RED_OUTINVERT_S 13
/* MCPWM_DB2_FED_INSEL : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB2_FED_INSEL (BIT(12))
#define MCPWM_DB2_FED_INSEL_M (BIT(12))
#define MCPWM_DB2_FED_INSEL_V 0x1
#define MCPWM_DB2_FED_INSEL_S 12
/* MCPWM_DB2_RED_INSEL : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB2_RED_INSEL (BIT(11))
#define MCPWM_DB2_RED_INSEL_M (BIT(11))
#define MCPWM_DB2_RED_INSEL_V 0x1
#define MCPWM_DB2_RED_INSEL_S 11
/* MCPWM_DB2_B_OUTSWAP : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB2_B_OUTSWAP (BIT(10))
#define MCPWM_DB2_B_OUTSWAP_M (BIT(10))
#define MCPWM_DB2_B_OUTSWAP_V 0x1
#define MCPWM_DB2_B_OUTSWAP_S 10
/* MCPWM_DB2_A_OUTSWAP : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB2_A_OUTSWAP (BIT(9))
#define MCPWM_DB2_A_OUTSWAP_M (BIT(9))
#define MCPWM_DB2_A_OUTSWAP_V 0x1
#define MCPWM_DB2_A_OUTSWAP_S 9
/* MCPWM_DB2_DEB_MODE : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: */
#define MCPWM_DB2_DEB_MODE (BIT(8))
#define MCPWM_DB2_DEB_MODE_M (BIT(8))
#define MCPWM_DB2_DEB_MODE_V 0x1
#define MCPWM_DB2_DEB_MODE_S 8
/* MCPWM_DB2_RED_UPMETHOD : R/W ;bitpos:[7:4] ;default: 4'd0 ; */
/*description: */
#define MCPWM_DB2_RED_UPMETHOD 0x0000000F
#define MCPWM_DB2_RED_UPMETHOD_M ((MCPWM_DB2_RED_UPMETHOD_V) << (MCPWM_DB2_RED_UPMETHOD_S))
#define MCPWM_DB2_RED_UPMETHOD_V 0xF
#define MCPWM_DB2_RED_UPMETHOD_S 4
/* MCPWM_DB2_FED_UPMETHOD : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: */
#define MCPWM_DB2_FED_UPMETHOD 0x0000000F
#define MCPWM_DB2_FED_UPMETHOD_M ((MCPWM_DB2_FED_UPMETHOD_V) << (MCPWM_DB2_FED_UPMETHOD_S))
#define MCPWM_DB2_FED_UPMETHOD_V 0xF
#define MCPWM_DB2_FED_UPMETHOD_S 0

#define MCPWM_DB2_FED_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x00cc)
/* MCPWM_DB2_FED : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define MCPWM_DB2_FED 0x0000FFFF
#define MCPWM_DB2_FED_M ((MCPWM_DB2_FED_V) << (MCPWM_DB2_FED_S))
#define MCPWM_DB2_FED_V 0xFFFF
#define MCPWM_DB2_FED_S 0

#define MCPWM_DB2_RED_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x00d0)
/* MCPWM_DB2_RED : R/W ;bitpos:[15:0] ;default: 16'd0 ; */
/*description: */
#define MCPWM_DB2_RED 0x0000FFFF
#define MCPWM_DB2_RED_M ((MCPWM_DB2_RED_V) << (MCPWM_DB2_RED_S))
#define MCPWM_DB2_RED_V 0xFFFF
#define MCPWM_DB2_RED_S 0

#define MCPWM_CHOPPER2_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x00d4)
/* MCPWM_CHOPPER2_IN_INVERT : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CHOPPER2_IN_INVERT (BIT(13))
#define MCPWM_CHOPPER2_IN_INVERT_M (BIT(13))
#define MCPWM_CHOPPER2_IN_INVERT_V 0x1
#define MCPWM_CHOPPER2_IN_INVERT_S 13
/* MCPWM_CHOPPER2_OUT_INVERT : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CHOPPER2_OUT_INVERT (BIT(12))
#define MCPWM_CHOPPER2_OUT_INVERT_M (BIT(12))
#define MCPWM_CHOPPER2_OUT_INVERT_V 0x1
#define MCPWM_CHOPPER2_OUT_INVERT_S 12
/* MCPWM_CHOPPER2_OSHTWTH : R/W ;bitpos:[11:8] ;default: 4'd0 ; */
/*description: */
#define MCPWM_CHOPPER2_OSHTWTH 0x0000000F
#define MCPWM_CHOPPER2_OSHTWTH_M ((MCPWM_CHOPPER2_OSHTWTH_V) << (MCPWM_CHOPPER2_OSHTWTH_S))
#define MCPWM_CHOPPER2_OSHTWTH_V 0xF
#define MCPWM_CHOPPER2_OSHTWTH_S 8
/* MCPWM_CHOPPER2_DUTY : R/W ;bitpos:[7:5] ;default: 3'd0 ; */
/*description: */
#define MCPWM_CHOPPER2_DUTY 0x00000007
#define MCPWM_CHOPPER2_DUTY_M ((MCPWM_CHOPPER2_DUTY_V) << (MCPWM_CHOPPER2_DUTY_S))
#define MCPWM_CHOPPER2_DUTY_V 0x7
#define MCPWM_CHOPPER2_DUTY_S 5
/* MCPWM_CHOPPER2_PRESCALE : R/W ;bitpos:[4:1] ;default: 4'd0 ; */
/*description: */
#define MCPWM_CHOPPER2_PRESCALE 0x0000000F
#define MCPWM_CHOPPER2_PRESCALE_M ((MCPWM_CHOPPER2_PRESCALE_V) << (MCPWM_CHOPPER2_PRESCALE_S))
#define MCPWM_CHOPPER2_PRESCALE_V 0xF
#define MCPWM_CHOPPER2_PRESCALE_S 1
/* MCPWM_CHOPPER2_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CHOPPER2_EN (BIT(0))
#define MCPWM_CHOPPER2_EN_M (BIT(0))
#define MCPWM_CHOPPER2_EN_V 0x1
#define MCPWM_CHOPPER2_EN_S 0

#define MCPWM_TZ2_CFG0_REG(i) (REG_MCPWM_BASE(i) + 0x00d8)
/* MCPWM_TZ2_B_OST_U : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ2_B_OST_U 0x00000003
#define MCPWM_TZ2_B_OST_U_M ((MCPWM_TZ2_B_OST_U_V) << (MCPWM_TZ2_B_OST_U_S))
#define MCPWM_TZ2_B_OST_U_V 0x3
#define MCPWM_TZ2_B_OST_U_S 22
/* MCPWM_TZ2_B_OST_D : R/W ;bitpos:[21:20] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ2_B_OST_D 0x00000003
#define MCPWM_TZ2_B_OST_D_M ((MCPWM_TZ2_B_OST_D_V) << (MCPWM_TZ2_B_OST_D_S))
#define MCPWM_TZ2_B_OST_D_V 0x3
#define MCPWM_TZ2_B_OST_D_S 20
/* MCPWM_TZ2_B_CBC_U : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ2_B_CBC_U 0x00000003
#define MCPWM_TZ2_B_CBC_U_M ((MCPWM_TZ2_B_CBC_U_V) << (MCPWM_TZ2_B_CBC_U_S))
#define MCPWM_TZ2_B_CBC_U_V 0x3
#define MCPWM_TZ2_B_CBC_U_S 18
/* MCPWM_TZ2_B_CBC_D : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ2_B_CBC_D 0x00000003
#define MCPWM_TZ2_B_CBC_D_M ((MCPWM_TZ2_B_CBC_D_V) << (MCPWM_TZ2_B_CBC_D_S))
#define MCPWM_TZ2_B_CBC_D_V 0x3
#define MCPWM_TZ2_B_CBC_D_S 16
/* MCPWM_TZ2_A_OST_U : R/W ;bitpos:[15:14] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ2_A_OST_U 0x00000003
#define MCPWM_TZ2_A_OST_U_M ((MCPWM_TZ2_A_OST_U_V) << (MCPWM_TZ2_A_OST_U_S))
#define MCPWM_TZ2_A_OST_U_V 0x3
#define MCPWM_TZ2_A_OST_U_S 14
/* MCPWM_TZ2_A_OST_D : R/W ;bitpos:[13:12] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ2_A_OST_D 0x00000003
#define MCPWM_TZ2_A_OST_D_M ((MCPWM_TZ2_A_OST_D_V) << (MCPWM_TZ2_A_OST_D_S))
#define MCPWM_TZ2_A_OST_D_V 0x3
#define MCPWM_TZ2_A_OST_D_S 12
/* MCPWM_TZ2_A_CBC_U : R/W ;bitpos:[11:10] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ2_A_CBC_U 0x00000003
#define MCPWM_TZ2_A_CBC_U_M ((MCPWM_TZ2_A_CBC_U_V) << (MCPWM_TZ2_A_CBC_U_S))
#define MCPWM_TZ2_A_CBC_U_V 0x3
#define MCPWM_TZ2_A_CBC_U_S 10
/* MCPWM_TZ2_A_CBC_D : R/W ;bitpos:[9:8] ;default: 2'd0 ; */
/*description: 0: do nothing  1: force lo  2: force hi  3: toggle*/
#define MCPWM_TZ2_A_CBC_D 0x00000003
#define MCPWM_TZ2_A_CBC_D_M ((MCPWM_TZ2_A_CBC_D_V) << (MCPWM_TZ2_A_CBC_D_S))
#define MCPWM_TZ2_A_CBC_D_V 0x3
#define MCPWM_TZ2_A_CBC_D_S 8
/* MCPWM_TZ2_F0_OST : R/W ;bitpos:[7] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ2_F0_OST (BIT(7))
#define MCPWM_TZ2_F0_OST_M (BIT(7))
#define MCPWM_TZ2_F0_OST_V 0x1
#define MCPWM_TZ2_F0_OST_S 7
/* MCPWM_TZ2_F1_OST : R/W ;bitpos:[6] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ2_F1_OST (BIT(6))
#define MCPWM_TZ2_F1_OST_M (BIT(6))
#define MCPWM_TZ2_F1_OST_V 0x1
#define MCPWM_TZ2_F1_OST_S 6
/* MCPWM_TZ2_F2_OST : R/W ;bitpos:[5] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ2_F2_OST (BIT(5))
#define MCPWM_TZ2_F2_OST_M (BIT(5))
#define MCPWM_TZ2_F2_OST_V 0x1
#define MCPWM_TZ2_F2_OST_S 5
/* MCPWM_TZ2_SW_OST : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ2_SW_OST (BIT(4))
#define MCPWM_TZ2_SW_OST_M (BIT(4))
#define MCPWM_TZ2_SW_OST_V 0x1
#define MCPWM_TZ2_SW_OST_S 4
/* MCPWM_TZ2_F0_CBC : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ2_F0_CBC (BIT(3))
#define MCPWM_TZ2_F0_CBC_M (BIT(3))
#define MCPWM_TZ2_F0_CBC_V 0x1
#define MCPWM_TZ2_F0_CBC_S 3
/* MCPWM_TZ2_F1_CBC : R/W ;bitpos:[2] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ2_F1_CBC (BIT(2))
#define MCPWM_TZ2_F1_CBC_M (BIT(2))
#define MCPWM_TZ2_F1_CBC_V 0x1
#define MCPWM_TZ2_F1_CBC_S 2
/* MCPWM_TZ2_F2_CBC : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ2_F2_CBC (BIT(1))
#define MCPWM_TZ2_F2_CBC_M (BIT(1))
#define MCPWM_TZ2_F2_CBC_V 0x1
#define MCPWM_TZ2_F2_CBC_S 1
/* MCPWM_TZ2_SW_CBC : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: 0: disable  1: enable*/
#define MCPWM_TZ2_SW_CBC (BIT(0))
#define MCPWM_TZ2_SW_CBC_M (BIT(0))
#define MCPWM_TZ2_SW_CBC_V 0x1
#define MCPWM_TZ2_SW_CBC_S 0

#define MCPWM_TZ2_CFG1_REG(i) (REG_MCPWM_BASE(i) + 0x00dc)
/* MCPWM_TZ2_FORCE_OST : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: a toggle trigger a oneshot tripping*/
#define MCPWM_TZ2_FORCE_OST (BIT(4))
#define MCPWM_TZ2_FORCE_OST_M (BIT(4))
#define MCPWM_TZ2_FORCE_OST_V 0x1
#define MCPWM_TZ2_FORCE_OST_S 4
/* MCPWM_TZ2_FORCE_CBC : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: a toggle trigger a cycle-by-cycle tripping*/
#define MCPWM_TZ2_FORCE_CBC (BIT(3))
#define MCPWM_TZ2_FORCE_CBC_M (BIT(3))
#define MCPWM_TZ2_FORCE_CBC_V 0x1
#define MCPWM_TZ2_FORCE_CBC_S 3
/* MCPWM_TZ2_CBCPULSE : R/W ;bitpos:[2:1] ;default: 2'd0 ; */
/*description: bit0: tez  bit1: tep*/
#define MCPWM_TZ2_CBCPULSE 0x00000003
#define MCPWM_TZ2_CBCPULSE_M ((MCPWM_TZ2_CBCPULSE_V) << (MCPWM_TZ2_CBCPULSE_S))
#define MCPWM_TZ2_CBCPULSE_V 0x3
#define MCPWM_TZ2_CBCPULSE_S 1
/* MCPWM_TZ2_CLR_OST : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: a toggle will clear oneshot tripping*/
#define MCPWM_TZ2_CLR_OST (BIT(0))
#define MCPWM_TZ2_CLR_OST_M (BIT(0))
#define MCPWM_TZ2_CLR_OST_V 0x1
#define MCPWM_TZ2_CLR_OST_S 0

#define MCPWM_TZ2_STATUS_REG(i) (REG_MCPWM_BASE(i) + 0x00e0)
/* MCPWM_TZ2_OST_ON : RO ;bitpos:[1] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ2_OST_ON (BIT(1))
#define MCPWM_TZ2_OST_ON_M (BIT(1))
#define MCPWM_TZ2_OST_ON_V 0x1
#define MCPWM_TZ2_OST_ON_S 1
/* MCPWM_TZ2_CBC_ON : RO ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ2_CBC_ON (BIT(0))
#define MCPWM_TZ2_CBC_ON_M (BIT(0))
#define MCPWM_TZ2_CBC_ON_V 0x1
#define MCPWM_TZ2_CBC_ON_S 0

#define MCPWM_FAULT_DETECT_REG(i) (REG_MCPWM_BASE(i) + 0x00e4)
/* MCPWM_EVENT_F2 : RO ;bitpos:[8] ;default: 1'd0 ; */
/*description: */
#define MCPWM_EVENT_F2 (BIT(8))
#define MCPWM_EVENT_F2_M (BIT(8))
#define MCPWM_EVENT_F2_V 0x1
#define MCPWM_EVENT_F2_S 8
/* MCPWM_EVENT_F1 : RO ;bitpos:[7] ;default: 1'd0 ; */
/*description: */
#define MCPWM_EVENT_F1 (BIT(7))
#define MCPWM_EVENT_F1_M (BIT(7))
#define MCPWM_EVENT_F1_V 0x1
#define MCPWM_EVENT_F1_S 7
/* MCPWM_EVENT_F0 : RO ;bitpos:[6] ;default: 1'd0 ; */
/*description: */
#define MCPWM_EVENT_F0 (BIT(6))
#define MCPWM_EVENT_F0_M (BIT(6))
#define MCPWM_EVENT_F0_V 0x1
#define MCPWM_EVENT_F0_S 6
/* MCPWM_F2_POLE : R/W ;bitpos:[5] ;default: 1'd0 ; */
/*description: */
#define MCPWM_F2_POLE (BIT(5))
#define MCPWM_F2_POLE_M (BIT(5))
#define MCPWM_F2_POLE_V 0x1
#define MCPWM_F2_POLE_S 5
/* MCPWM_F1_POLE : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: */
#define MCPWM_F1_POLE (BIT(4))
#define MCPWM_F1_POLE_M (BIT(4))
#define MCPWM_F1_POLE_V 0x1
#define MCPWM_F1_POLE_S 4
/* MCPWM_F0_POLE : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: */
#define MCPWM_F0_POLE (BIT(3))
#define MCPWM_F0_POLE_M (BIT(3))
#define MCPWM_F0_POLE_V 0x1
#define MCPWM_F0_POLE_S 3
/* MCPWM_F2_EN : R/W ;bitpos:[2] ;default: 1'd0 ; */
/*description: */
#define MCPWM_F2_EN (BIT(2))
#define MCPWM_F2_EN_M (BIT(2))
#define MCPWM_F2_EN_V 0x1
#define MCPWM_F2_EN_S 2
/* MCPWM_F1_EN : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: */
#define MCPWM_F1_EN (BIT(1))
#define MCPWM_F1_EN_M (BIT(1))
#define MCPWM_F1_EN_V 0x1
#define MCPWM_F1_EN_S 1
/* MCPWM_F0_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define MCPWM_F0_EN (BIT(0))
#define MCPWM_F0_EN_M (BIT(0))
#define MCPWM_F0_EN_V 0x1
#define MCPWM_F0_EN_S 0

#define MCPWM_CAP_TIMER_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x00e8)
/* MCPWM_CAP_SYNC_SW : WO ;bitpos:[5] ;default: 1'd0 ; */
/*description: Write 1 will force a timer sync*/
#define MCPWM_CAP_SYNC_SW (BIT(5))
#define MCPWM_CAP_SYNC_SW_M (BIT(5))
#define MCPWM_CAP_SYNC_SW_V 0x1
#define MCPWM_CAP_SYNC_SW_S 5
/* MCPWM_CAP_SYNCI_SEL : R/W ;bitpos:[4:2] ;default: 3'd0 ; */
/*description: */
#define MCPWM_CAP_SYNCI_SEL 0x00000007
#define MCPWM_CAP_SYNCI_SEL_M ((MCPWM_CAP_SYNCI_SEL_V) << (MCPWM_CAP_SYNCI_SEL_S))
#define MCPWM_CAP_SYNCI_SEL_V 0x7
#define MCPWM_CAP_SYNCI_SEL_S 2
/* MCPWM_CAP_SYNCI_EN : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP_SYNCI_EN (BIT(1))
#define MCPWM_CAP_SYNCI_EN_M (BIT(1))
#define MCPWM_CAP_SYNCI_EN_V 0x1
#define MCPWM_CAP_SYNCI_EN_S 1
/* MCPWM_CAP_TIMER_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP_TIMER_EN (BIT(0))
#define MCPWM_CAP_TIMER_EN_M (BIT(0))
#define MCPWM_CAP_TIMER_EN_V 0x1
#define MCPWM_CAP_TIMER_EN_S 0

#define MCPWM_CAP_TIMER_PHASE_REG(i) (REG_MCPWM_BASE(i) + 0x00ec)
/* MCPWM_CAP_PHASE : R/W ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: */
#define MCPWM_CAP_PHASE 0xFFFFFFFF
#define MCPWM_CAP_PHASE_M ((MCPWM_CAP_PHASE_V) << (MCPWM_CAP_PHASE_S))
#define MCPWM_CAP_PHASE_V 0xFFFFFFFF
#define MCPWM_CAP_PHASE_S 0

#define MCPWM_CAP_CH0_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x00f0)
/* MCPWM_CAP0_SW : WO ;bitpos:[12] ;default: 1'd0 ; */
/*description: Write 1 will trigger a sw capture*/
#define MCPWM_CAP0_SW (BIT(12))
#define MCPWM_CAP0_SW_M (BIT(12))
#define MCPWM_CAP0_SW_V 0x1
#define MCPWM_CAP0_SW_S 12
/* MCPWM_CAP0_IN_INVERT : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP0_IN_INVERT (BIT(11))
#define MCPWM_CAP0_IN_INVERT_M (BIT(11))
#define MCPWM_CAP0_IN_INVERT_V 0x1
#define MCPWM_CAP0_IN_INVERT_S 11
/* MCPWM_CAP0_PRESCALE : R/W ;bitpos:[10:3] ;default: 8'd0 ; */
/*description: */
#define MCPWM_CAP0_PRESCALE 0x000000FF
#define MCPWM_CAP0_PRESCALE_M ((MCPWM_CAP0_PRESCALE_V) << (MCPWM_CAP0_PRESCALE_S))
#define MCPWM_CAP0_PRESCALE_V 0xFF
#define MCPWM_CAP0_PRESCALE_S 3
/* MCPWM_CAP0_MODE : R/W ;bitpos:[2:1] ;default: 2'd0 ; */
/*description: bit0: negedge cap en  bit1: posedge cap en*/
#define MCPWM_CAP0_MODE 0x00000003
#define MCPWM_CAP0_MODE_M ((MCPWM_CAP0_MODE_V) << (MCPWM_CAP0_MODE_S))
#define MCPWM_CAP0_MODE_V 0x3
#define MCPWM_CAP0_MODE_S 1
/* MCPWM_CAP0_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP0_EN (BIT(0))
#define MCPWM_CAP0_EN_M (BIT(0))
#define MCPWM_CAP0_EN_V 0x1
#define MCPWM_CAP0_EN_S 0

#define MCPWM_CAP_CH1_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x00f4)
/* MCPWM_CAP1_SW : WO ;bitpos:[12] ;default: 1'd0 ; */
/*description: Write 1 will trigger a sw capture*/
#define MCPWM_CAP1_SW (BIT(12))
#define MCPWM_CAP1_SW_M (BIT(12))
#define MCPWM_CAP1_SW_V 0x1
#define MCPWM_CAP1_SW_S 12
/* MCPWM_CAP1_IN_INVERT : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP1_IN_INVERT (BIT(11))
#define MCPWM_CAP1_IN_INVERT_M (BIT(11))
#define MCPWM_CAP1_IN_INVERT_V 0x1
#define MCPWM_CAP1_IN_INVERT_S 11
/* MCPWM_CAP1_PRESCALE : R/W ;bitpos:[10:3] ;default: 8'd0 ; */
/*description: */
#define MCPWM_CAP1_PRESCALE 0x000000FF
#define MCPWM_CAP1_PRESCALE_M ((MCPWM_CAP1_PRESCALE_V) << (MCPWM_CAP1_PRESCALE_S))
#define MCPWM_CAP1_PRESCALE_V 0xFF
#define MCPWM_CAP1_PRESCALE_S 3
/* MCPWM_CAP1_MODE : R/W ;bitpos:[2:1] ;default: 2'd0 ; */
/*description: */
#define MCPWM_CAP1_MODE 0x00000003
#define MCPWM_CAP1_MODE_M ((MCPWM_CAP1_MODE_V) << (MCPWM_CAP1_MODE_S))
#define MCPWM_CAP1_MODE_V 0x3
#define MCPWM_CAP1_MODE_S 1
/* MCPWM_CAP1_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP1_EN (BIT(0))
#define MCPWM_CAP1_EN_M (BIT(0))
#define MCPWM_CAP1_EN_V 0x1
#define MCPWM_CAP1_EN_S 0

#define MCPWM_CAP_CH2_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x00f8)
/* MCPWM_CAP2_SW : WO ;bitpos:[12] ;default: 1'd0 ; */
/*description: Write 1 will trigger a sw capture*/
#define MCPWM_CAP2_SW (BIT(12))
#define MCPWM_CAP2_SW_M (BIT(12))
#define MCPWM_CAP2_SW_V 0x1
#define MCPWM_CAP2_SW_S 12
/* MCPWM_CAP2_IN_INVERT : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP2_IN_INVERT (BIT(11))
#define MCPWM_CAP2_IN_INVERT_M (BIT(11))
#define MCPWM_CAP2_IN_INVERT_V 0x1
#define MCPWM_CAP2_IN_INVERT_S 11
/* MCPWM_CAP2_PRESCALE : R/W ;bitpos:[10:3] ;default: 8'd0 ; */
/*description: */
#define MCPWM_CAP2_PRESCALE 0x000000FF
#define MCPWM_CAP2_PRESCALE_M ((MCPWM_CAP2_PRESCALE_V) << (MCPWM_CAP2_PRESCALE_S))
#define MCPWM_CAP2_PRESCALE_V 0xFF
#define MCPWM_CAP2_PRESCALE_S 3
/* MCPWM_CAP2_MODE : R/W ;bitpos:[2:1] ;default: 2'd0 ; */
/*description: */
#define MCPWM_CAP2_MODE 0x00000003
#define MCPWM_CAP2_MODE_M ((MCPWM_CAP2_MODE_V) << (MCPWM_CAP2_MODE_S))
#define MCPWM_CAP2_MODE_V 0x3
#define MCPWM_CAP2_MODE_S 1
/* MCPWM_CAP2_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP2_EN (BIT(0))
#define MCPWM_CAP2_EN_M (BIT(0))
#define MCPWM_CAP2_EN_V 0x1
#define MCPWM_CAP2_EN_S 0

#define MCPWM_CAP_CH0_REG(i) (REG_MCPWM_BASE(i) + 0x00fc)
/* MCPWM_CAP0_VALUE : RO ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: */
#define MCPWM_CAP0_VALUE 0xFFFFFFFF
#define MCPWM_CAP0_VALUE_M ((MCPWM_CAP0_VALUE_V) << (MCPWM_CAP0_VALUE_S))
#define MCPWM_CAP0_VALUE_V 0xFFFFFFFF
#define MCPWM_CAP0_VALUE_S 0

#define MCPWM_CAP_CH1_REG(i) (REG_MCPWM_BASE(i) + 0x0100)
/* MCPWM_CAP1_VALUE : RO ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: */
#define MCPWM_CAP1_VALUE 0xFFFFFFFF
#define MCPWM_CAP1_VALUE_M ((MCPWM_CAP1_VALUE_V) << (MCPWM_CAP1_VALUE_S))
#define MCPWM_CAP1_VALUE_V 0xFFFFFFFF
#define MCPWM_CAP1_VALUE_S 0

#define MCPWM_CAP_CH2_REG(i) (REG_MCPWM_BASE(i) + 0x0104)
/* MCPWM_CAP2_VALUE : RO ;bitpos:[31:0] ;default: 32'd0 ; */
/*description: */
#define MCPWM_CAP2_VALUE 0xFFFFFFFF
#define MCPWM_CAP2_VALUE_M ((MCPWM_CAP2_VALUE_V) << (MCPWM_CAP2_VALUE_S))
#define MCPWM_CAP2_VALUE_V 0xFFFFFFFF
#define MCPWM_CAP2_VALUE_S 0

#define MCPWM_CAP_STATUS_REG(i) (REG_MCPWM_BASE(i) + 0x0108)
/* MCPWM_CAP2_EDGE : RO ;bitpos:[2] ;default: 1'd0 ; */
/*description: cap trigger's edge  0: posedge  1: negedge*/
#define MCPWM_CAP2_EDGE (BIT(2))
#define MCPWM_CAP2_EDGE_M (BIT(2))
#define MCPWM_CAP2_EDGE_V 0x1
#define MCPWM_CAP2_EDGE_S 2
/* MCPWM_CAP1_EDGE : RO ;bitpos:[1] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP1_EDGE (BIT(1))
#define MCPWM_CAP1_EDGE_M (BIT(1))
#define MCPWM_CAP1_EDGE_V 0x1
#define MCPWM_CAP1_EDGE_S 1
/* MCPWM_CAP0_EDGE : RO ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP0_EDGE (BIT(0))
#define MCPWM_CAP0_EDGE_M (BIT(0))
#define MCPWM_CAP0_EDGE_V 0x1
#define MCPWM_CAP0_EDGE_S 0

#define MCPWM_UPDATE_CFG_REG(i) (REG_MCPWM_BASE(i) + 0x010c)
/* MCPWM_OP2_FORCE_UP : R/W ;bitpos:[7] ;default: 1'd0 ; */
/*description: a toggle will trigger a force update*/
#define MCPWM_OP2_FORCE_UP (BIT(7))
#define MCPWM_OP2_FORCE_UP_M (BIT(7))
#define MCPWM_OP2_FORCE_UP_V 0x1
#define MCPWM_OP2_FORCE_UP_S 7
/* MCPWM_OP2_UP_EN : R/W ;bitpos:[6] ;default: 1'd1 ; */
/*description: reg update local enable*/
#define MCPWM_OP2_UP_EN (BIT(6))
#define MCPWM_OP2_UP_EN_M (BIT(6))
#define MCPWM_OP2_UP_EN_V 0x1
#define MCPWM_OP2_UP_EN_S 6
/* MCPWM_OP1_FORCE_UP : R/W ;bitpos:[5] ;default: 1'd0 ; */
/*description: a toggle will trigger a force update*/
#define MCPWM_OP1_FORCE_UP (BIT(5))
#define MCPWM_OP1_FORCE_UP_M (BIT(5))
#define MCPWM_OP1_FORCE_UP_V 0x1
#define MCPWM_OP1_FORCE_UP_S 5
/* MCPWM_OP1_UP_EN : R/W ;bitpos:[4] ;default: 1'd1 ; */
/*description: */
#define MCPWM_OP1_UP_EN (BIT(4))
#define MCPWM_OP1_UP_EN_M (BIT(4))
#define MCPWM_OP1_UP_EN_V 0x1
#define MCPWM_OP1_UP_EN_S 4
/* MCPWM_OP0_FORCE_UP : R/W ;bitpos:[3] ;default: 1'd0 ; */
/*description: a toggle will trigger a force update*/
#define MCPWM_OP0_FORCE_UP (BIT(3))
#define MCPWM_OP0_FORCE_UP_M (BIT(3))
#define MCPWM_OP0_FORCE_UP_V 0x1
#define MCPWM_OP0_FORCE_UP_S 3
/* MCPWM_OP0_UP_EN : R/W ;bitpos:[2] ;default: 1'd1 ; */
/*description: */
#define MCPWM_OP0_UP_EN (BIT(2))
#define MCPWM_OP0_UP_EN_M (BIT(2))
#define MCPWM_OP0_UP_EN_V 0x1
#define MCPWM_OP0_UP_EN_S 2
/* MCPWM_GLOBAL_FORCE_UP : R/W ;bitpos:[1] ;default: 1'd0 ; */
/*description: a toggle will trigger a force update  all timers and operators
 will update their active regs*/
#define MCPWM_GLOBAL_FORCE_UP (BIT(1))
#define MCPWM_GLOBAL_FORCE_UP_M (BIT(1))
#define MCPWM_GLOBAL_FORCE_UP_V 0x1
#define MCPWM_GLOBAL_FORCE_UP_S 1
/* MCPWM_GLOBAL_UP_EN : R/W ;bitpos:[0] ;default: 1'd1 ; */
/*description: */
#define MCPWM_GLOBAL_UP_EN (BIT(0))
#define MCPWM_GLOBAL_UP_EN_M (BIT(0))
#define MCPWM_GLOBAL_UP_EN_V 0x1
#define MCPWM_GLOBAL_UP_EN_S 0

#define MCMCPWM_INT_ENA_MCPWM_REG(i) (REG_MCPWM_BASE(i) + 0x0110)
/* MCPWM_CAP2_INT_ENA : R/W ;bitpos:[29] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP2_INT_ENA (BIT(29))
#define MCPWM_CAP2_INT_ENA_M (BIT(29))
#define MCPWM_CAP2_INT_ENA_V 0x1
#define MCPWM_CAP2_INT_ENA_S 29
/* MCPWM_CAP1_INT_ENA : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP1_INT_ENA (BIT(28))
#define MCPWM_CAP1_INT_ENA_M (BIT(28))
#define MCPWM_CAP1_INT_ENA_V 0x1
#define MCPWM_CAP1_INT_ENA_S 28
/* MCPWM_CAP0_INT_ENA : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP0_INT_ENA (BIT(27))
#define MCPWM_CAP0_INT_ENA_M (BIT(27))
#define MCPWM_CAP0_INT_ENA_V 0x1
#define MCPWM_CAP0_INT_ENA_S 27
/* MCPWM_TZ2_OST_INT_ENA : R/W ;bitpos:[26] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ2_OST_INT_ENA (BIT(26))
#define MCPWM_TZ2_OST_INT_ENA_M (BIT(26))
#define MCPWM_TZ2_OST_INT_ENA_V 0x1
#define MCPWM_TZ2_OST_INT_ENA_S 26
/* MCPWM_TZ1_OST_INT_ENA : R/W ;bitpos:[25] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ1_OST_INT_ENA (BIT(25))
#define MCPWM_TZ1_OST_INT_ENA_M (BIT(25))
#define MCPWM_TZ1_OST_INT_ENA_V 0x1
#define MCPWM_TZ1_OST_INT_ENA_S 25
/* MCPWM_TZ0_OST_INT_ENA : R/W ;bitpos:[24] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ0_OST_INT_ENA (BIT(24))
#define MCPWM_TZ0_OST_INT_ENA_M (BIT(24))
#define MCPWM_TZ0_OST_INT_ENA_V 0x1
#define MCPWM_TZ0_OST_INT_ENA_S 24
/* MCPWM_TZ2_CBC_INT_ENA : R/W ;bitpos:[23] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ2_CBC_INT_ENA (BIT(23))
#define MCPWM_TZ2_CBC_INT_ENA_M (BIT(23))
#define MCPWM_TZ2_CBC_INT_ENA_V 0x1
#define MCPWM_TZ2_CBC_INT_ENA_S 23
/* MCPWM_TZ1_CBC_INT_ENA : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ1_CBC_INT_ENA (BIT(22))
#define MCPWM_TZ1_CBC_INT_ENA_M (BIT(22))
#define MCPWM_TZ1_CBC_INT_ENA_V 0x1
#define MCPWM_TZ1_CBC_INT_ENA_S 22
/* MCPWM_TZ0_CBC_INT_ENA : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ0_CBC_INT_ENA (BIT(21))
#define MCPWM_TZ0_CBC_INT_ENA_M (BIT(21))
#define MCPWM_TZ0_CBC_INT_ENA_V 0x1
#define MCPWM_TZ0_CBC_INT_ENA_S 21
/* MCPWM_CMPR2_TEB_INT_ENA : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR2_TEB_INT_ENA (BIT(20))
#define MCPWM_CMPR2_TEB_INT_ENA_M (BIT(20))
#define MCPWM_CMPR2_TEB_INT_ENA_V 0x1
#define MCPWM_CMPR2_TEB_INT_ENA_S 20
/* MCPWM_CMPR1_TEB_INT_ENA : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR1_TEB_INT_ENA (BIT(19))
#define MCPWM_CMPR1_TEB_INT_ENA_M (BIT(19))
#define MCPWM_CMPR1_TEB_INT_ENA_V 0x1
#define MCPWM_CMPR1_TEB_INT_ENA_S 19
/* MCPWM_CMPR0_TEB_INT_ENA : R/W ;bitpos:[18] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR0_TEB_INT_ENA (BIT(18))
#define MCPWM_CMPR0_TEB_INT_ENA_M (BIT(18))
#define MCPWM_CMPR0_TEB_INT_ENA_V 0x1
#define MCPWM_CMPR0_TEB_INT_ENA_S 18
/* MCPWM_CMPR2_TEA_INT_ENA : R/W ;bitpos:[17] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR2_TEA_INT_ENA (BIT(17))
#define MCPWM_CMPR2_TEA_INT_ENA_M (BIT(17))
#define MCPWM_CMPR2_TEA_INT_ENA_V 0x1
#define MCPWM_CMPR2_TEA_INT_ENA_S 17
/* MCPWM_CMPR1_TEA_INT_ENA : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR1_TEA_INT_ENA (BIT(16))
#define MCPWM_CMPR1_TEA_INT_ENA_M (BIT(16))
#define MCPWM_CMPR1_TEA_INT_ENA_V 0x1
#define MCPWM_CMPR1_TEA_INT_ENA_S 16
/* MCPWM_CMPR0_TEA_INT_ENA : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR0_TEA_INT_ENA (BIT(15))
#define MCPWM_CMPR0_TEA_INT_ENA_M (BIT(15))
#define MCPWM_CMPR0_TEA_INT_ENA_V 0x1
#define MCPWM_CMPR0_TEA_INT_ENA_S 15
/* MCPWM_FAULT2_CLR_INT_ENA : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT2_CLR_INT_ENA (BIT(14))
#define MCPWM_FAULT2_CLR_INT_ENA_M (BIT(14))
#define MCPWM_FAULT2_CLR_INT_ENA_V 0x1
#define MCPWM_FAULT2_CLR_INT_ENA_S 14
/* MCPWM_FAULT1_CLR_INT_ENA : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT1_CLR_INT_ENA (BIT(13))
#define MCPWM_FAULT1_CLR_INT_ENA_M (BIT(13))
#define MCPWM_FAULT1_CLR_INT_ENA_V 0x1
#define MCPWM_FAULT1_CLR_INT_ENA_S 13
/* MCPWM_FAULT0_CLR_INT_ENA : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT0_CLR_INT_ENA (BIT(12))
#define MCPWM_FAULT0_CLR_INT_ENA_M (BIT(12))
#define MCPWM_FAULT0_CLR_INT_ENA_V 0x1
#define MCPWM_FAULT0_CLR_INT_ENA_S 12
/* MCPWM_FAULT2_INT_ENA : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT2_INT_ENA (BIT(11))
#define MCPWM_FAULT2_INT_ENA_M (BIT(11))
#define MCPWM_FAULT2_INT_ENA_V 0x1
#define MCPWM_FAULT2_INT_ENA_S 11
/* MCPWM_FAULT1_INT_ENA : R/W ;bitpos:[10] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT1_INT_ENA (BIT(10))
#define MCPWM_FAULT1_INT_ENA_M (BIT(10))
#define MCPWM_FAULT1_INT_ENA_V 0x1
#define MCPWM_FAULT1_INT_ENA_S 10
/* MCPWM_FAULT0_INT_ENA : R/W ;bitpos:[9] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT0_INT_ENA (BIT(9))
#define MCPWM_FAULT0_INT_ENA_M (BIT(9))
#define MCPWM_FAULT0_INT_ENA_V 0x1
#define MCPWM_FAULT0_INT_ENA_S 9
/* MCPWM_TIMER2_TEP_INT_ENA : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER2_TEP_INT_ENA (BIT(8))
#define MCPWM_TIMER2_TEP_INT_ENA_M (BIT(8))
#define MCPWM_TIMER2_TEP_INT_ENA_V 0x1
#define MCPWM_TIMER2_TEP_INT_ENA_S 8
/* MCPWM_TIMER1_TEP_INT_ENA : R/W ;bitpos:[7] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER1_TEP_INT_ENA (BIT(7))
#define MCPWM_TIMER1_TEP_INT_ENA_M (BIT(7))
#define MCPWM_TIMER1_TEP_INT_ENA_V 0x1
#define MCPWM_TIMER1_TEP_INT_ENA_S 7
/* MCPWM_TIMER0_TEP_INT_ENA : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER0_TEP_INT_ENA (BIT(6))
#define MCPWM_TIMER0_TEP_INT_ENA_M (BIT(6))
#define MCPWM_TIMER0_TEP_INT_ENA_V 0x1
#define MCPWM_TIMER0_TEP_INT_ENA_S 6
/* MCPWM_TIMER2_TEZ_INT_ENA : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER2_TEZ_INT_ENA (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_ENA_M (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_ENA_V 0x1
#define MCPWM_TIMER2_TEZ_INT_ENA_S 5
/* MCPWM_TIMER1_TEZ_INT_ENA : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER1_TEZ_INT_ENA (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_ENA_M (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_ENA_V 0x1
#define MCPWM_TIMER1_TEZ_INT_ENA_S 4
/* MCPWM_TIMER0_TEZ_INT_ENA : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER0_TEZ_INT_ENA (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_ENA_M (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_ENA_V 0x1
#define MCPWM_TIMER0_TEZ_INT_ENA_S 3
/* MCPWM_TIMER2_STOP_INT_ENA : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER2_STOP_INT_ENA (BIT(2))
#define MCPWM_TIMER2_STOP_INT_ENA_M (BIT(2))
#define MCPWM_TIMER2_STOP_INT_ENA_V 0x1
#define MCPWM_TIMER2_STOP_INT_ENA_S 2
/* MCPWM_TIMER1_STOP_INT_ENA : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER1_STOP_INT_ENA (BIT(1))
#define MCPWM_TIMER1_STOP_INT_ENA_M (BIT(1))
#define MCPWM_TIMER1_STOP_INT_ENA_V 0x1
#define MCPWM_TIMER1_STOP_INT_ENA_S 1
/* MCPWM_TIMER0_STOP_INT_ENA : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER0_STOP_INT_ENA (BIT(0))
#define MCPWM_TIMER0_STOP_INT_ENA_M (BIT(0))
#define MCPWM_TIMER0_STOP_INT_ENA_V 0x1
#define MCPWM_TIMER0_STOP_INT_ENA_S 0

#define MCMCPWM_INT_RAW_MCPWM_REG(i) (REG_MCPWM_BASE(i) + 0x0114)
/* MCPWM_CAP2_INT_RAW : RO ;bitpos:[29] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP2_INT_RAW (BIT(29))
#define MCPWM_CAP2_INT_RAW_M (BIT(29))
#define MCPWM_CAP2_INT_RAW_V 0x1
#define MCPWM_CAP2_INT_RAW_S 29
/* MCPWM_CAP1_INT_RAW : RO ;bitpos:[28] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP1_INT_RAW (BIT(28))
#define MCPWM_CAP1_INT_RAW_M (BIT(28))
#define MCPWM_CAP1_INT_RAW_V 0x1
#define MCPWM_CAP1_INT_RAW_S 28
/* MCPWM_CAP0_INT_RAW : RO ;bitpos:[27] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP0_INT_RAW (BIT(27))
#define MCPWM_CAP0_INT_RAW_M (BIT(27))
#define MCPWM_CAP0_INT_RAW_V 0x1
#define MCPWM_CAP0_INT_RAW_S 27
/* MCPWM_TZ2_OST_INT_RAW : RO ;bitpos:[26] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ2_OST_INT_RAW (BIT(26))
#define MCPWM_TZ2_OST_INT_RAW_M (BIT(26))
#define MCPWM_TZ2_OST_INT_RAW_V 0x1
#define MCPWM_TZ2_OST_INT_RAW_S 26
/* MCPWM_TZ1_OST_INT_RAW : RO ;bitpos:[25] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ1_OST_INT_RAW (BIT(25))
#define MCPWM_TZ1_OST_INT_RAW_M (BIT(25))
#define MCPWM_TZ1_OST_INT_RAW_V 0x1
#define MCPWM_TZ1_OST_INT_RAW_S 25
/* MCPWM_TZ0_OST_INT_RAW : RO ;bitpos:[24] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ0_OST_INT_RAW (BIT(24))
#define MCPWM_TZ0_OST_INT_RAW_M (BIT(24))
#define MCPWM_TZ0_OST_INT_RAW_V 0x1
#define MCPWM_TZ0_OST_INT_RAW_S 24
/* MCPWM_TZ2_CBC_INT_RAW : RO ;bitpos:[23] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ2_CBC_INT_RAW (BIT(23))
#define MCPWM_TZ2_CBC_INT_RAW_M (BIT(23))
#define MCPWM_TZ2_CBC_INT_RAW_V 0x1
#define MCPWM_TZ2_CBC_INT_RAW_S 23
/* MCPWM_TZ1_CBC_INT_RAW : RO ;bitpos:[22] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ1_CBC_INT_RAW (BIT(22))
#define MCPWM_TZ1_CBC_INT_RAW_M (BIT(22))
#define MCPWM_TZ1_CBC_INT_RAW_V 0x1
#define MCPWM_TZ1_CBC_INT_RAW_S 22
/* MCPWM_TZ0_CBC_INT_RAW : RO ;bitpos:[21] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ0_CBC_INT_RAW (BIT(21))
#define MCPWM_TZ0_CBC_INT_RAW_M (BIT(21))
#define MCPWM_TZ0_CBC_INT_RAW_V 0x1
#define MCPWM_TZ0_CBC_INT_RAW_S 21
/* MCPWM_CMPR2_TEB_INT_RAW : RO ;bitpos:[20] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR2_TEB_INT_RAW (BIT(20))
#define MCPWM_CMPR2_TEB_INT_RAW_M (BIT(20))
#define MCPWM_CMPR2_TEB_INT_RAW_V 0x1
#define MCPWM_CMPR2_TEB_INT_RAW_S 20
/* MCPWM_CMPR1_TEB_INT_RAW : RO ;bitpos:[19] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR1_TEB_INT_RAW (BIT(19))
#define MCPWM_CMPR1_TEB_INT_RAW_M (BIT(19))
#define MCPWM_CMPR1_TEB_INT_RAW_V 0x1
#define MCPWM_CMPR1_TEB_INT_RAW_S 19
/* MCPWM_CMPR0_TEB_INT_RAW : RO ;bitpos:[18] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR0_TEB_INT_RAW (BIT(18))
#define MCPWM_CMPR0_TEB_INT_RAW_M (BIT(18))
#define MCPWM_CMPR0_TEB_INT_RAW_V 0x1
#define MCPWM_CMPR0_TEB_INT_RAW_S 18
/* MCPWM_CMPR2_TEA_INT_RAW : RO ;bitpos:[17] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR2_TEA_INT_RAW (BIT(17))
#define MCPWM_CMPR2_TEA_INT_RAW_M (BIT(17))
#define MCPWM_CMPR2_TEA_INT_RAW_V 0x1
#define MCPWM_CMPR2_TEA_INT_RAW_S 17
/* MCPWM_CMPR1_TEA_INT_RAW : RO ;bitpos:[16] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR1_TEA_INT_RAW (BIT(16))
#define MCPWM_CMPR1_TEA_INT_RAW_M (BIT(16))
#define MCPWM_CMPR1_TEA_INT_RAW_V 0x1
#define MCPWM_CMPR1_TEA_INT_RAW_S 16
/* MCPWM_CMPR0_TEA_INT_RAW : RO ;bitpos:[15] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR0_TEA_INT_RAW (BIT(15))
#define MCPWM_CMPR0_TEA_INT_RAW_M (BIT(15))
#define MCPWM_CMPR0_TEA_INT_RAW_V 0x1
#define MCPWM_CMPR0_TEA_INT_RAW_S 15
/* MCPWM_FAULT2_CLR_INT_RAW : RO ;bitpos:[14] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT2_CLR_INT_RAW (BIT(14))
#define MCPWM_FAULT2_CLR_INT_RAW_M (BIT(14))
#define MCPWM_FAULT2_CLR_INT_RAW_V 0x1
#define MCPWM_FAULT2_CLR_INT_RAW_S 14
/* MCPWM_FAULT1_CLR_INT_RAW : RO ;bitpos:[13] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT1_CLR_INT_RAW (BIT(13))
#define MCPWM_FAULT1_CLR_INT_RAW_M (BIT(13))
#define MCPWM_FAULT1_CLR_INT_RAW_V 0x1
#define MCPWM_FAULT1_CLR_INT_RAW_S 13
/* MCPWM_FAULT0_CLR_INT_RAW : RO ;bitpos:[12] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT0_CLR_INT_RAW (BIT(12))
#define MCPWM_FAULT0_CLR_INT_RAW_M (BIT(12))
#define MCPWM_FAULT0_CLR_INT_RAW_V 0x1
#define MCPWM_FAULT0_CLR_INT_RAW_S 12
/* MCPWM_FAULT2_INT_RAW : RO ;bitpos:[11] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT2_INT_RAW (BIT(11))
#define MCPWM_FAULT2_INT_RAW_M (BIT(11))
#define MCPWM_FAULT2_INT_RAW_V 0x1
#define MCPWM_FAULT2_INT_RAW_S 11
/* MCPWM_FAULT1_INT_RAW : RO ;bitpos:[10] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT1_INT_RAW (BIT(10))
#define MCPWM_FAULT1_INT_RAW_M (BIT(10))
#define MCPWM_FAULT1_INT_RAW_V 0x1
#define MCPWM_FAULT1_INT_RAW_S 10
/* MCPWM_FAULT0_INT_RAW : RO ;bitpos:[9] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT0_INT_RAW (BIT(9))
#define MCPWM_FAULT0_INT_RAW_M (BIT(9))
#define MCPWM_FAULT0_INT_RAW_V 0x1
#define MCPWM_FAULT0_INT_RAW_S 9
/* MCPWM_TIMER2_TEP_INT_RAW : RO ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER2_TEP_INT_RAW (BIT(8))
#define MCPWM_TIMER2_TEP_INT_RAW_M (BIT(8))
#define MCPWM_TIMER2_TEP_INT_RAW_V 0x1
#define MCPWM_TIMER2_TEP_INT_RAW_S 8
/* MCPWM_TIMER1_TEP_INT_RAW : RO ;bitpos:[7] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER1_TEP_INT_RAW (BIT(7))
#define MCPWM_TIMER1_TEP_INT_RAW_M (BIT(7))
#define MCPWM_TIMER1_TEP_INT_RAW_V 0x1
#define MCPWM_TIMER1_TEP_INT_RAW_S 7
/* MCPWM_TIMER0_TEP_INT_RAW : RO ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER0_TEP_INT_RAW (BIT(6))
#define MCPWM_TIMER0_TEP_INT_RAW_M (BIT(6))
#define MCPWM_TIMER0_TEP_INT_RAW_V 0x1
#define MCPWM_TIMER0_TEP_INT_RAW_S 6
/* MCPWM_TIMER2_TEZ_INT_RAW : RO ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER2_TEZ_INT_RAW (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_RAW_M (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_RAW_V 0x1
#define MCPWM_TIMER2_TEZ_INT_RAW_S 5
/* MCPWM_TIMER1_TEZ_INT_RAW : RO ;bitpos:[4] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER1_TEZ_INT_RAW (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_RAW_M (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_RAW_V 0x1
#define MCPWM_TIMER1_TEZ_INT_RAW_S 4
/* MCPWM_TIMER0_TEZ_INT_RAW : RO ;bitpos:[3] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER0_TEZ_INT_RAW (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_RAW_M (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_RAW_V 0x1
#define MCPWM_TIMER0_TEZ_INT_RAW_S 3
/* MCPWM_TIMER2_STOP_INT_RAW : RO ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER2_STOP_INT_RAW (BIT(2))
#define MCPWM_TIMER2_STOP_INT_RAW_M (BIT(2))
#define MCPWM_TIMER2_STOP_INT_RAW_V 0x1
#define MCPWM_TIMER2_STOP_INT_RAW_S 2
/* MCPWM_TIMER1_STOP_INT_RAW : RO ;bitpos:[1] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER1_STOP_INT_RAW (BIT(1))
#define MCPWM_TIMER1_STOP_INT_RAW_M (BIT(1))
#define MCPWM_TIMER1_STOP_INT_RAW_V 0x1
#define MCPWM_TIMER1_STOP_INT_RAW_S 1
/* MCPWM_TIMER0_STOP_INT_RAW : RO ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER0_STOP_INT_RAW (BIT(0))
#define MCPWM_TIMER0_STOP_INT_RAW_M (BIT(0))
#define MCPWM_TIMER0_STOP_INT_RAW_V 0x1
#define MCPWM_TIMER0_STOP_INT_RAW_S 0

#define MCMCPWM_INT_ST_MCPWM_REG(i) (REG_MCPWM_BASE(i) + 0x0118)
/* MCPWM_CAP2_INT_ST : RO ;bitpos:[29] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP2_INT_ST (BIT(29))
#define MCPWM_CAP2_INT_ST_M (BIT(29))
#define MCPWM_CAP2_INT_ST_V 0x1
#define MCPWM_CAP2_INT_ST_S 29
/* MCPWM_CAP1_INT_ST : RO ;bitpos:[28] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP1_INT_ST (BIT(28))
#define MCPWM_CAP1_INT_ST_M (BIT(28))
#define MCPWM_CAP1_INT_ST_V 0x1
#define MCPWM_CAP1_INT_ST_S 28
/* MCPWM_CAP0_INT_ST : RO ;bitpos:[27] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP0_INT_ST (BIT(27))
#define MCPWM_CAP0_INT_ST_M (BIT(27))
#define MCPWM_CAP0_INT_ST_V 0x1
#define MCPWM_CAP0_INT_ST_S 27
/* MCPWM_TZ2_OST_INT_ST : RO ;bitpos:[26] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ2_OST_INT_ST (BIT(26))
#define MCPWM_TZ2_OST_INT_ST_M (BIT(26))
#define MCPWM_TZ2_OST_INT_ST_V 0x1
#define MCPWM_TZ2_OST_INT_ST_S 26
/* MCPWM_TZ1_OST_INT_ST : RO ;bitpos:[25] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ1_OST_INT_ST (BIT(25))
#define MCPWM_TZ1_OST_INT_ST_M (BIT(25))
#define MCPWM_TZ1_OST_INT_ST_V 0x1
#define MCPWM_TZ1_OST_INT_ST_S 25
/* MCPWM_TZ0_OST_INT_ST : RO ;bitpos:[24] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ0_OST_INT_ST (BIT(24))
#define MCPWM_TZ0_OST_INT_ST_M (BIT(24))
#define MCPWM_TZ0_OST_INT_ST_V 0x1
#define MCPWM_TZ0_OST_INT_ST_S 24
/* MCPWM_TZ2_CBC_INT_ST : RO ;bitpos:[23] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ2_CBC_INT_ST (BIT(23))
#define MCPWM_TZ2_CBC_INT_ST_M (BIT(23))
#define MCPWM_TZ2_CBC_INT_ST_V 0x1
#define MCPWM_TZ2_CBC_INT_ST_S 23
/* MCPWM_TZ1_CBC_INT_ST : RO ;bitpos:[22] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ1_CBC_INT_ST (BIT(22))
#define MCPWM_TZ1_CBC_INT_ST_M (BIT(22))
#define MCPWM_TZ1_CBC_INT_ST_V 0x1
#define MCPWM_TZ1_CBC_INT_ST_S 22
/* MCPWM_TZ0_CBC_INT_ST : RO ;bitpos:[21] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ0_CBC_INT_ST (BIT(21))
#define MCPWM_TZ0_CBC_INT_ST_M (BIT(21))
#define MCPWM_TZ0_CBC_INT_ST_V 0x1
#define MCPWM_TZ0_CBC_INT_ST_S 21
/* MCPWM_CMPR2_TEB_INT_ST : RO ;bitpos:[20] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR2_TEB_INT_ST (BIT(20))
#define MCPWM_CMPR2_TEB_INT_ST_M (BIT(20))
#define MCPWM_CMPR2_TEB_INT_ST_V 0x1
#define MCPWM_CMPR2_TEB_INT_ST_S 20
/* MCPWM_CMPR1_TEB_INT_ST : RO ;bitpos:[19] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR1_TEB_INT_ST (BIT(19))
#define MCPWM_CMPR1_TEB_INT_ST_M (BIT(19))
#define MCPWM_CMPR1_TEB_INT_ST_V 0x1
#define MCPWM_CMPR1_TEB_INT_ST_S 19
/* MCPWM_CMPR0_TEB_INT_ST : RO ;bitpos:[18] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR0_TEB_INT_ST (BIT(18))
#define MCPWM_CMPR0_TEB_INT_ST_M (BIT(18))
#define MCPWM_CMPR0_TEB_INT_ST_V 0x1
#define MCPWM_CMPR0_TEB_INT_ST_S 18
/* MCPWM_CMPR2_TEA_INT_ST : RO ;bitpos:[17] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR2_TEA_INT_ST (BIT(17))
#define MCPWM_CMPR2_TEA_INT_ST_M (BIT(17))
#define MCPWM_CMPR2_TEA_INT_ST_V 0x1
#define MCPWM_CMPR2_TEA_INT_ST_S 17
/* MCPWM_CMPR1_TEA_INT_ST : RO ;bitpos:[16] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR1_TEA_INT_ST (BIT(16))
#define MCPWM_CMPR1_TEA_INT_ST_M (BIT(16))
#define MCPWM_CMPR1_TEA_INT_ST_V 0x1
#define MCPWM_CMPR1_TEA_INT_ST_S 16
/* MCPWM_CMPR0_TEA_INT_ST : RO ;bitpos:[15] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR0_TEA_INT_ST (BIT(15))
#define MCPWM_CMPR0_TEA_INT_ST_M (BIT(15))
#define MCPWM_CMPR0_TEA_INT_ST_V 0x1
#define MCPWM_CMPR0_TEA_INT_ST_S 15
/* MCPWM_FAULT2_CLR_INT_ST : RO ;bitpos:[14] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT2_CLR_INT_ST (BIT(14))
#define MCPWM_FAULT2_CLR_INT_ST_M (BIT(14))
#define MCPWM_FAULT2_CLR_INT_ST_V 0x1
#define MCPWM_FAULT2_CLR_INT_ST_S 14
/* MCPWM_FAULT1_CLR_INT_ST : RO ;bitpos:[13] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT1_CLR_INT_ST (BIT(13))
#define MCPWM_FAULT1_CLR_INT_ST_M (BIT(13))
#define MCPWM_FAULT1_CLR_INT_ST_V 0x1
#define MCPWM_FAULT1_CLR_INT_ST_S 13
/* MCPWM_FAULT0_CLR_INT_ST : RO ;bitpos:[12] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT0_CLR_INT_ST (BIT(12))
#define MCPWM_FAULT0_CLR_INT_ST_M (BIT(12))
#define MCPWM_FAULT0_CLR_INT_ST_V 0x1
#define MCPWM_FAULT0_CLR_INT_ST_S 12
/* MCPWM_FAULT2_INT_ST : RO ;bitpos:[11] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT2_INT_ST (BIT(11))
#define MCPWM_FAULT2_INT_ST_M (BIT(11))
#define MCPWM_FAULT2_INT_ST_V 0x1
#define MCPWM_FAULT2_INT_ST_S 11
/* MCPWM_FAULT1_INT_ST : RO ;bitpos:[10] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT1_INT_ST (BIT(10))
#define MCPWM_FAULT1_INT_ST_M (BIT(10))
#define MCPWM_FAULT1_INT_ST_V 0x1
#define MCPWM_FAULT1_INT_ST_S 10
/* MCPWM_FAULT0_INT_ST : RO ;bitpos:[9] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT0_INT_ST (BIT(9))
#define MCPWM_FAULT0_INT_ST_M (BIT(9))
#define MCPWM_FAULT0_INT_ST_V 0x1
#define MCPWM_FAULT0_INT_ST_S 9
/* MCPWM_TIMER2_TEP_INT_ST : RO ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER2_TEP_INT_ST (BIT(8))
#define MCPWM_TIMER2_TEP_INT_ST_M (BIT(8))
#define MCPWM_TIMER2_TEP_INT_ST_V 0x1
#define MCPWM_TIMER2_TEP_INT_ST_S 8
/* MCPWM_TIMER1_TEP_INT_ST : RO ;bitpos:[7] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER1_TEP_INT_ST (BIT(7))
#define MCPWM_TIMER1_TEP_INT_ST_M (BIT(7))
#define MCPWM_TIMER1_TEP_INT_ST_V 0x1
#define MCPWM_TIMER1_TEP_INT_ST_S 7
/* MCPWM_TIMER0_TEP_INT_ST : RO ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER0_TEP_INT_ST (BIT(6))
#define MCPWM_TIMER0_TEP_INT_ST_M (BIT(6))
#define MCPWM_TIMER0_TEP_INT_ST_V 0x1
#define MCPWM_TIMER0_TEP_INT_ST_S 6
/* MCPWM_TIMER2_TEZ_INT_ST : RO ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER2_TEZ_INT_ST (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_ST_M (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_ST_V 0x1
#define MCPWM_TIMER2_TEZ_INT_ST_S 5
/* MCPWM_TIMER1_TEZ_INT_ST : RO ;bitpos:[4] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER1_TEZ_INT_ST (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_ST_M (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_ST_V 0x1
#define MCPWM_TIMER1_TEZ_INT_ST_S 4
/* MCPWM_TIMER0_TEZ_INT_ST : RO ;bitpos:[3] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER0_TEZ_INT_ST (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_ST_M (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_ST_V 0x1
#define MCPWM_TIMER0_TEZ_INT_ST_S 3
/* MCPWM_TIMER2_STOP_INT_ST : RO ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER2_STOP_INT_ST (BIT(2))
#define MCPWM_TIMER2_STOP_INT_ST_M (BIT(2))
#define MCPWM_TIMER2_STOP_INT_ST_V 0x1
#define MCPWM_TIMER2_STOP_INT_ST_S 2
/* MCPWM_TIMER1_STOP_INT_ST : RO ;bitpos:[1] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER1_STOP_INT_ST (BIT(1))
#define MCPWM_TIMER1_STOP_INT_ST_M (BIT(1))
#define MCPWM_TIMER1_STOP_INT_ST_V 0x1
#define MCPWM_TIMER1_STOP_INT_ST_S 1
/* MCPWM_TIMER0_STOP_INT_ST : RO ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER0_STOP_INT_ST (BIT(0))
#define MCPWM_TIMER0_STOP_INT_ST_M (BIT(0))
#define MCPWM_TIMER0_STOP_INT_ST_V 0x1
#define MCPWM_TIMER0_STOP_INT_ST_S 0

#define MCMCPWM_INT_CLR_MCPWM_REG(i) (REG_MCPWM_BASE(i) + 0x011c)
/* MCPWM_CAP2_INT_CLR : WO ;bitpos:[29] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP2_INT_CLR (BIT(29))
#define MCPWM_CAP2_INT_CLR_M (BIT(29))
#define MCPWM_CAP2_INT_CLR_V 0x1
#define MCPWM_CAP2_INT_CLR_S 29
/* MCPWM_CAP1_INT_CLR : WO ;bitpos:[28] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP1_INT_CLR (BIT(28))
#define MCPWM_CAP1_INT_CLR_M (BIT(28))
#define MCPWM_CAP1_INT_CLR_V 0x1
#define MCPWM_CAP1_INT_CLR_S 28
/* MCPWM_CAP0_INT_CLR : WO ;bitpos:[27] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CAP0_INT_CLR (BIT(27))
#define MCPWM_CAP0_INT_CLR_M (BIT(27))
#define MCPWM_CAP0_INT_CLR_V 0x1
#define MCPWM_CAP0_INT_CLR_S 27
/* MCPWM_TZ2_OST_INT_CLR : WO ;bitpos:[26] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ2_OST_INT_CLR (BIT(26))
#define MCPWM_TZ2_OST_INT_CLR_M (BIT(26))
#define MCPWM_TZ2_OST_INT_CLR_V 0x1
#define MCPWM_TZ2_OST_INT_CLR_S 26
/* MCPWM_TZ1_OST_INT_CLR : WO ;bitpos:[25] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ1_OST_INT_CLR (BIT(25))
#define MCPWM_TZ1_OST_INT_CLR_M (BIT(25))
#define MCPWM_TZ1_OST_INT_CLR_V 0x1
#define MCPWM_TZ1_OST_INT_CLR_S 25
/* MCPWM_TZ0_OST_INT_CLR : WO ;bitpos:[24] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ0_OST_INT_CLR (BIT(24))
#define MCPWM_TZ0_OST_INT_CLR_M (BIT(24))
#define MCPWM_TZ0_OST_INT_CLR_V 0x1
#define MCPWM_TZ0_OST_INT_CLR_S 24
/* MCPWM_TZ2_CBC_INT_CLR : WO ;bitpos:[23] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ2_CBC_INT_CLR (BIT(23))
#define MCPWM_TZ2_CBC_INT_CLR_M (BIT(23))
#define MCPWM_TZ2_CBC_INT_CLR_V 0x1
#define MCPWM_TZ2_CBC_INT_CLR_S 23
/* MCPWM_TZ1_CBC_INT_CLR : WO ;bitpos:[22] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ1_CBC_INT_CLR (BIT(22))
#define MCPWM_TZ1_CBC_INT_CLR_M (BIT(22))
#define MCPWM_TZ1_CBC_INT_CLR_V 0x1
#define MCPWM_TZ1_CBC_INT_CLR_S 22
/* MCPWM_TZ0_CBC_INT_CLR : WO ;bitpos:[21] ;default: 1'd0 ; */
/*description: */
#define MCPWM_TZ0_CBC_INT_CLR (BIT(21))
#define MCPWM_TZ0_CBC_INT_CLR_M (BIT(21))
#define MCPWM_TZ0_CBC_INT_CLR_V 0x1
#define MCPWM_TZ0_CBC_INT_CLR_S 21
/* MCPWM_CMPR2_TEB_INT_CLR : WO ;bitpos:[20] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR2_TEB_INT_CLR (BIT(20))
#define MCPWM_CMPR2_TEB_INT_CLR_M (BIT(20))
#define MCPWM_CMPR2_TEB_INT_CLR_V 0x1
#define MCPWM_CMPR2_TEB_INT_CLR_S 20
/* MCPWM_CMPR1_TEB_INT_CLR : WO ;bitpos:[19] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR1_TEB_INT_CLR (BIT(19))
#define MCPWM_CMPR1_TEB_INT_CLR_M (BIT(19))
#define MCPWM_CMPR1_TEB_INT_CLR_V 0x1
#define MCPWM_CMPR1_TEB_INT_CLR_S 19
/* MCPWM_CMPR0_TEB_INT_CLR : WO ;bitpos:[18] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR0_TEB_INT_CLR (BIT(18))
#define MCPWM_CMPR0_TEB_INT_CLR_M (BIT(18))
#define MCPWM_CMPR0_TEB_INT_CLR_V 0x1
#define MCPWM_CMPR0_TEB_INT_CLR_S 18
/* MCPWM_CMPR2_TEA_INT_CLR : WO ;bitpos:[17] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR2_TEA_INT_CLR (BIT(17))
#define MCPWM_CMPR2_TEA_INT_CLR_M (BIT(17))
#define MCPWM_CMPR2_TEA_INT_CLR_V 0x1
#define MCPWM_CMPR2_TEA_INT_CLR_S 17
/* MCPWM_CMPR1_TEA_INT_CLR : WO ;bitpos:[16] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR1_TEA_INT_CLR (BIT(16))
#define MCPWM_CMPR1_TEA_INT_CLR_M (BIT(16))
#define MCPWM_CMPR1_TEA_INT_CLR_V 0x1
#define MCPWM_CMPR1_TEA_INT_CLR_S 16
/* MCPWM_CMPR0_TEA_INT_CLR : WO ;bitpos:[15] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CMPR0_TEA_INT_CLR (BIT(15))
#define MCPWM_CMPR0_TEA_INT_CLR_M (BIT(15))
#define MCPWM_CMPR0_TEA_INT_CLR_V 0x1
#define MCPWM_CMPR0_TEA_INT_CLR_S 15
/* MCPWM_FAULT2_CLR_INT_CLR : WO ;bitpos:[14] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT2_CLR_INT_CLR (BIT(14))
#define MCPWM_FAULT2_CLR_INT_CLR_M (BIT(14))
#define MCPWM_FAULT2_CLR_INT_CLR_V 0x1
#define MCPWM_FAULT2_CLR_INT_CLR_S 14
/* MCPWM_FAULT1_CLR_INT_CLR : WO ;bitpos:[13] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT1_CLR_INT_CLR (BIT(13))
#define MCPWM_FAULT1_CLR_INT_CLR_M (BIT(13))
#define MCPWM_FAULT1_CLR_INT_CLR_V 0x1
#define MCPWM_FAULT1_CLR_INT_CLR_S 13
/* MCPWM_FAULT0_CLR_INT_CLR : WO ;bitpos:[12] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT0_CLR_INT_CLR (BIT(12))
#define MCPWM_FAULT0_CLR_INT_CLR_M (BIT(12))
#define MCPWM_FAULT0_CLR_INT_CLR_V 0x1
#define MCPWM_FAULT0_CLR_INT_CLR_S 12
/* MCPWM_FAULT2_INT_CLR : WO ;bitpos:[11] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT2_INT_CLR (BIT(11))
#define MCPWM_FAULT2_INT_CLR_M (BIT(11))
#define MCPWM_FAULT2_INT_CLR_V 0x1
#define MCPWM_FAULT2_INT_CLR_S 11
/* MCPWM_FAULT1_INT_CLR : WO ;bitpos:[10] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT1_INT_CLR (BIT(10))
#define MCPWM_FAULT1_INT_CLR_M (BIT(10))
#define MCPWM_FAULT1_INT_CLR_V 0x1
#define MCPWM_FAULT1_INT_CLR_S 10
/* MCPWM_FAULT0_INT_CLR : WO ;bitpos:[9] ;default: 1'd0 ; */
/*description: */
#define MCPWM_FAULT0_INT_CLR (BIT(9))
#define MCPWM_FAULT0_INT_CLR_M (BIT(9))
#define MCPWM_FAULT0_INT_CLR_V 0x1
#define MCPWM_FAULT0_INT_CLR_S 9
/* MCPWM_TIMER2_TEP_INT_CLR : WO ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER2_TEP_INT_CLR (BIT(8))
#define MCPWM_TIMER2_TEP_INT_CLR_M (BIT(8))
#define MCPWM_TIMER2_TEP_INT_CLR_V 0x1
#define MCPWM_TIMER2_TEP_INT_CLR_S 8
/* MCPWM_TIMER1_TEP_INT_CLR : WO ;bitpos:[7] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER1_TEP_INT_CLR (BIT(7))
#define MCPWM_TIMER1_TEP_INT_CLR_M (BIT(7))
#define MCPWM_TIMER1_TEP_INT_CLR_V 0x1
#define MCPWM_TIMER1_TEP_INT_CLR_S 7
/* MCPWM_TIMER0_TEP_INT_CLR : WO ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER0_TEP_INT_CLR (BIT(6))
#define MCPWM_TIMER0_TEP_INT_CLR_M (BIT(6))
#define MCPWM_TIMER0_TEP_INT_CLR_V 0x1
#define MCPWM_TIMER0_TEP_INT_CLR_S 6
/* MCPWM_TIMER2_TEZ_INT_CLR : WO ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER2_TEZ_INT_CLR (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_CLR_M (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_CLR_V 0x1
#define MCPWM_TIMER2_TEZ_INT_CLR_S 5
/* MCPWM_TIMER1_TEZ_INT_CLR : WO ;bitpos:[4] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER1_TEZ_INT_CLR (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_CLR_M (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_CLR_V 0x1
#define MCPWM_TIMER1_TEZ_INT_CLR_S 4
/* MCPWM_TIMER0_TEZ_INT_CLR : WO ;bitpos:[3] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER0_TEZ_INT_CLR (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_CLR_M (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_CLR_V 0x1
#define MCPWM_TIMER0_TEZ_INT_CLR_S 3
/* MCPWM_TIMER2_STOP_INT_CLR : WO ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER2_STOP_INT_CLR (BIT(2))
#define MCPWM_TIMER2_STOP_INT_CLR_M (BIT(2))
#define MCPWM_TIMER2_STOP_INT_CLR_V 0x1
#define MCPWM_TIMER2_STOP_INT_CLR_S 2
/* MCPWM_TIMER1_STOP_INT_CLR : WO ;bitpos:[1] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER1_STOP_INT_CLR (BIT(1))
#define MCPWM_TIMER1_STOP_INT_CLR_M (BIT(1))
#define MCPWM_TIMER1_STOP_INT_CLR_V 0x1
#define MCPWM_TIMER1_STOP_INT_CLR_S 1
/* MCPWM_TIMER0_STOP_INT_CLR : WO ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define MCPWM_TIMER0_STOP_INT_CLR (BIT(0))
#define MCPWM_TIMER0_STOP_INT_CLR_M (BIT(0))
#define MCPWM_TIMER0_STOP_INT_CLR_V 0x1
#define MCPWM_TIMER0_STOP_INT_CLR_S 0

#define MCPWM_CLK_REG(i) (REG_MCPWM_BASE(i) + 0x0120)
/* MCPWM_CLK_EN : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: */
#define MCPWM_CLK_EN (BIT(0))
#define MCPWM_CLK_EN_M (BIT(0))
#define MCPWM_CLK_EN_V 0x1
#define MCPWM_CLK_EN_S 0

#define MCPWM_VERSION_REG(i) (REG_MCPWM_BASE(i) + 0x0124)
/* MCPWM_DATE : R/W ;bitpos:[27:0] ;default: 28'h1509110 ; */
/*description: */
#define MCPWM_DATE 0x0FFFFFFF
#define MCPWM_DATE_M ((MCPWM_DATE_V) << (MCPWM_DATE_S))
#define MCPWM_DATE_V 0xFFFFFFF
#define MCPWM_DATE_S 0

#ifdef __cplusplus
}
#endif
