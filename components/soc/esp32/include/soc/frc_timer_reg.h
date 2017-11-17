// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _SOC_FRC_TIMER_REG_H_
#define _SOC_FRC_TIMER_REG_H_

#include "soc.h"

/**
 * These are the register definitions for "legacy" timers
 */

#define REG_FRC_TIMER_BASE(i)           (DR_REG_FRC_TIMER_BASE + i*0x20)

#define FRC_TIMER_LOAD_REG(i)           (REG_FRC_TIMER_BASE(i) + 0x0)   // timer load value (23 bit for i==0, 32 bit for i==1)
#define FRC_TIMER_LOAD_VALUE(i)         ((i == 0)?0x007FFFFF:0xffffffff)
#define FRC_TIMER_LOAD_VALUE_S          0

#define FRC_TIMER_COUNT_REG(i)          (REG_FRC_TIMER_BASE(i) + 0x4)   // timer count value (23 bit for i==0, 32 bit for i==1)
#define FRC_TIMER_COUNT                 ((i == 0)?0x007FFFFF:0xffffffff)
#define FRC_TIMER_COUNT_S               0

#define FRC_TIMER_CTRL_REG(i)           (REG_FRC_TIMER_BASE(i) + 0x8)
#define FRC_TIMER_INT_STATUS            (BIT(8))        // interrupt status (RO)
#define FRC_TIMER_ENABLE                (BIT(7))        // enable timer
#define FRC_TIMER_AUTOLOAD              (BIT(6))        // enable autoload
#define FRC_TIMER_PRESCALER             0x00000007
#define FRC_TIMER_PRESCALER_S           1
#define FRC_TIMER_PRESCALER_1           (0 << FRC_TIMER_PRESCALER_S)
#define FRC_TIMER_PRESCALER_16          (2 << FRC_TIMER_PRESCALER_S)
#define FRC_TIMER_PRESCALER_256         (4 << FRC_TIMER_PRESCALER_S)
#define FRC_TIMER_LEVEL_INT             (BIT(0))        // 1: level, 0: edge

#define FRC_TIMER_INT_REG(i)            (REG_FRC_TIMER_BASE(i) + 0xC)
#define FRC_TIMER_INT_CLR               (BIT(0))        // clear interrupt

#define FRC_TIMER_ALARM_REG(i)          (REG_FRC_TIMER_BASE(i) + 0x10)  // timer alarm value; register only present for i == 1
#define FRC_TIMER_ALARM                 0xFFFFFFFF
#define FRC_TIMER_ALARM_S               0

#endif //_SOC_FRC_TIMER_REG_H_
