// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** SYSTIMER_CONF_REG register
 *  Configure system timer clock
 */
#define SYSTIMER_CONF_REG (DR_REG_SYSTIMER_BASE + 0x0)
/** SYSTIMER_CLK_FO : R/W; bitpos: [0]; default: 0;
 *  system timer force clock enable
 */
#define SYSTIMER_CLK_FO    (BIT(0))
#define SYSTIMER_CLK_FO_M  (SYSTIMER_CLK_FO_V << SYSTIMER_CLK_FO_S)
#define SYSTIMER_CLK_FO_V  0x00000001
#define SYSTIMER_CLK_FO_S  0
/** SYSTIMER_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  register clock enable
 */
#define SYSTIMER_CLK_EN    (BIT(31))
#define SYSTIMER_CLK_EN_M  (SYSTIMER_CLK_EN_V << SYSTIMER_CLK_EN_S)
#define SYSTIMER_CLK_EN_V  0x00000001
#define SYSTIMER_CLK_EN_S  31


/** SYSTIMER_LOAD_REG register
 *  load value to system timer
 */
#define SYSTIMER_LOAD_REG (DR_REG_SYSTIMER_BASE + 0x4)
/** SYSTIMER_TIMER_LOAD : WO; bitpos: [31]; default: 0;
 *  load value to system timer
 */
#define SYSTIMER_TIMER_LOAD    (BIT(31))
#define SYSTIMER_TIMER_LOAD_M  (SYSTIMER_TIMER_LOAD_V << SYSTIMER_TIMER_LOAD_S)
#define SYSTIMER_TIMER_LOAD_V  0x00000001
#define SYSTIMER_TIMER_LOAD_S  31


/** SYSTIMER_LOAD_HI_REG register
 *  High 32-bit load to system timer
 */
#define SYSTIMER_LOAD_HI_REG (DR_REG_SYSTIMER_BASE + 0x8)
/** SYSTIMER_TIMER_LOAD_HI : R/W; bitpos: [32:0]; default: 0;
 *  High 32-bit load to system timer
 */
#define SYSTIMER_TIMER_LOAD_HI    0xFFFFFFFF
#define SYSTIMER_TIMER_LOAD_HI_M  (SYSTIMER_TIMER_LOAD_HI_V << SYSTIMER_TIMER_LOAD_HI_S)
#define SYSTIMER_TIMER_LOAD_HI_V  0xFFFFFFFF
#define SYSTIMER_TIMER_LOAD_HI_S  0


/** SYSTIMER_LOAD_LO_REG register
 *  Low 32-bit load to system timer
 */
#define SYSTIMER_LOAD_LO_REG (DR_REG_SYSTIMER_BASE + 0xc)
/** SYSTIMER_TIMER_LOAD_LO : R/W; bitpos: [32:0]; default: 0;
 *  Low 32-bit load to system timer
 */
#define SYSTIMER_TIMER_LOAD_LO    0xFFFFFFFF
#define SYSTIMER_TIMER_LOAD_LO_M  (SYSTIMER_TIMER_LOAD_LO_V << SYSTIMER_TIMER_LOAD_LO_S)
#define SYSTIMER_TIMER_LOAD_LO_V  0xFFFFFFFF
#define SYSTIMER_TIMER_LOAD_LO_S  0


/** SYSTIMER_STEP_REG register
 *  system timer accumulation step
 */
#define SYSTIMER_STEP_REG (DR_REG_SYSTIMER_BASE + 0x10)
/** SYSTIMER_TIMER_XTAL_STEP : R/W; bitpos: [10:0]; default: 80;
 *  system timer accumulation step when using XTAL
 */
#define SYSTIMER_TIMER_XTAL_STEP    0x000003FF
#define SYSTIMER_TIMER_XTAL_STEP_M  (SYSTIMER_TIMER_XTAL_STEP_V << SYSTIMER_TIMER_XTAL_STEP_S)
#define SYSTIMER_TIMER_XTAL_STEP_V  0x000003FF
#define SYSTIMER_TIMER_XTAL_STEP_S  0
/** SYSTIMER_TIMER_PLL_STEP : R/W; bitpos: [20:10]; default: 1;
 *  system timer accumulation step when using PLL
 */
#define SYSTIMER_TIMER_PLL_STEP    0x000003FF
#define SYSTIMER_TIMER_PLL_STEP_M  (SYSTIMER_TIMER_PLL_STEP_V << SYSTIMER_TIMER_PLL_STEP_S)
#define SYSTIMER_TIMER_PLL_STEP_V  0x000003FF
#define SYSTIMER_TIMER_PLL_STEP_S  10


/** SYSTIMER_TARGET0_HI_REG register
 *  System timer target0 high 32-bit
 */
#define SYSTIMER_TARGET0_HI_REG (DR_REG_SYSTIMER_BASE + 0x14)
/** SYSTIMER_TIMER_TARGET0_HI : R/W; bitpos: [32:0]; default: 0;
 *  System timer target0 high 32-bit
 */
#define SYSTIMER_TIMER_TARGET0_HI    0xFFFFFFFF
#define SYSTIMER_TIMER_TARGET0_HI_M  (SYSTIMER_TIMER_TARGET0_HI_V << SYSTIMER_TIMER_TARGET0_HI_S)
#define SYSTIMER_TIMER_TARGET0_HI_V  0xFFFFFFFF
#define SYSTIMER_TIMER_TARGET0_HI_S  0


/** SYSTIMER_TARGET0_LO_REG register
 *  System timer target0 low 32-bit
 */
#define SYSTIMER_TARGET0_LO_REG (DR_REG_SYSTIMER_BASE + 0x18)
/** SYSTIMER_TIMER_TARGET0_LO : R/W; bitpos: [32:0]; default: 0;
 *  System timer target0 low 32-bit
 */
#define SYSTIMER_TIMER_TARGET0_LO    0xFFFFFFFF
#define SYSTIMER_TIMER_TARGET0_LO_M  (SYSTIMER_TIMER_TARGET0_LO_V << SYSTIMER_TIMER_TARGET0_LO_S)
#define SYSTIMER_TIMER_TARGET0_LO_V  0xFFFFFFFF
#define SYSTIMER_TIMER_TARGET0_LO_S  0


/** SYSTIMER_TARGET1_HI_REG register
 *  System timer target1 high 32-bit
 */
#define SYSTIMER_TARGET1_HI_REG (DR_REG_SYSTIMER_BASE + 0x1c)
/** SYSTIMER_TIMER_TARGET1_HI : R/W; bitpos: [32:0]; default: 0;
 *  System timer target1 high 32-bit
 */
#define SYSTIMER_TIMER_TARGET1_HI    0xFFFFFFFF
#define SYSTIMER_TIMER_TARGET1_HI_M  (SYSTIMER_TIMER_TARGET1_HI_V << SYSTIMER_TIMER_TARGET1_HI_S)
#define SYSTIMER_TIMER_TARGET1_HI_V  0xFFFFFFFF
#define SYSTIMER_TIMER_TARGET1_HI_S  0


/** SYSTIMER_TARGET1_LO_REG register
 *  System timer target1 low 32-bit
 */
#define SYSTIMER_TARGET1_LO_REG (DR_REG_SYSTIMER_BASE + 0x20)
/** SYSTIMER_TIMER_TARGET1_LO : R/W; bitpos: [32:0]; default: 0;
 *  System timer target1 low 32-bit
 */
#define SYSTIMER_TIMER_TARGET1_LO    0xFFFFFFFF
#define SYSTIMER_TIMER_TARGET1_LO_M  (SYSTIMER_TIMER_TARGET1_LO_V << SYSTIMER_TIMER_TARGET1_LO_S)
#define SYSTIMER_TIMER_TARGET1_LO_V  0xFFFFFFFF
#define SYSTIMER_TIMER_TARGET1_LO_S  0


/** SYSTIMER_TARGET2_HI_REG register
 *  System timer target2 high 32-bit
 */
#define SYSTIMER_TARGET2_HI_REG (DR_REG_SYSTIMER_BASE + 0x24)
/** SYSTIMER_TIMER_TARGET2_HI : R/W; bitpos: [32:0]; default: 0;
 *  System timer target2 high 32-bit
 */
#define SYSTIMER_TIMER_TARGET2_HI    0xFFFFFFFF
#define SYSTIMER_TIMER_TARGET2_HI_M  (SYSTIMER_TIMER_TARGET2_HI_V << SYSTIMER_TIMER_TARGET2_HI_S)
#define SYSTIMER_TIMER_TARGET2_HI_V  0xFFFFFFFF
#define SYSTIMER_TIMER_TARGET2_HI_S  0


/** SYSTIMER_TARGET2_LO_REG register
 *  System timer target2 low 32-bit
 */
#define SYSTIMER_TARGET2_LO_REG (DR_REG_SYSTIMER_BASE + 0x28)
/** SYSTIMER_TIMER_TARGET2_LO : R/W; bitpos: [32:0]; default: 0;
 *  System timer target2 low 32-bit
 */
#define SYSTIMER_TIMER_TARGET2_LO    0xFFFFFFFF
#define SYSTIMER_TIMER_TARGET2_LO_M  (SYSTIMER_TIMER_TARGET2_LO_V << SYSTIMER_TIMER_TARGET2_LO_S)
#define SYSTIMER_TIMER_TARGET2_LO_V  0xFFFFFFFF
#define SYSTIMER_TIMER_TARGET2_LO_S  0


/** SYSTIMER_TARGET0_CONF_REG register
 *  Configure system timer target0 work mode
 */
#define SYSTIMER_TARGET0_CONF_REG (DR_REG_SYSTIMER_BASE + 0x2c)
/** SYSTIMER_TARGET0_PERIOD : R/W; bitpos: [30:0]; default: 0;
 *  System timer target0 alarm period
 */
#define SYSTIMER_TARGET0_PERIOD    0x3FFFFFFF
#define SYSTIMER_TARGET0_PERIOD_M  (SYSTIMER_TARGET0_PERIOD_V << SYSTIMER_TARGET0_PERIOD_S)
#define SYSTIMER_TARGET0_PERIOD_V  0x3FFFFFFF
#define SYSTIMER_TARGET0_PERIOD_S  0
/** SYSTIMER_TARGET0_PERIOD_MODE : R/W; bitpos: [30]; default: 0;
 *  Whether system timer target0 work in period mode
 */
#define SYSTIMER_TARGET0_PERIOD_MODE    (BIT(30))
#define SYSTIMER_TARGET0_PERIOD_MODE_M  (SYSTIMER_TARGET0_PERIOD_MODE_V << SYSTIMER_TARGET0_PERIOD_MODE_S)
#define SYSTIMER_TARGET0_PERIOD_MODE_V  0x00000001
#define SYSTIMER_TARGET0_PERIOD_MODE_S  30
/** SYSTIMER_TARGET0_WORK_EN : R/W; bitpos: [31]; default: 0;
 *  system timer target0 work enable
 */
#define SYSTIMER_TARGET0_WORK_EN    (BIT(31))
#define SYSTIMER_TARGET0_WORK_EN_M  (SYSTIMER_TARGET0_WORK_EN_V << SYSTIMER_TARGET0_WORK_EN_S)
#define SYSTIMER_TARGET0_WORK_EN_V  0x00000001
#define SYSTIMER_TARGET0_WORK_EN_S  31


/** SYSTIMER_TARGET1_CONF_REG register
 *  Configure system timer target1 work mode
 */
#define SYSTIMER_TARGET1_CONF_REG (DR_REG_SYSTIMER_BASE + 0x30)
/** SYSTIMER_TARGET1_PERIOD : R/W; bitpos: [30:0]; default: 0;
 *  System timer target1 alarm period
 */
#define SYSTIMER_TARGET1_PERIOD    0x3FFFFFFF
#define SYSTIMER_TARGET1_PERIOD_M  (SYSTIMER_TARGET1_PERIOD_V << SYSTIMER_TARGET1_PERIOD_S)
#define SYSTIMER_TARGET1_PERIOD_V  0x3FFFFFFF
#define SYSTIMER_TARGET1_PERIOD_S  0
/** SYSTIMER_TARGET1_PERIOD_MODE : R/W; bitpos: [30]; default: 0;
 *  Whether system timer target1 work in period mode
 */
#define SYSTIMER_TARGET1_PERIOD_MODE    (BIT(30))
#define SYSTIMER_TARGET1_PERIOD_MODE_M  (SYSTIMER_TARGET1_PERIOD_MODE_V << SYSTIMER_TARGET1_PERIOD_MODE_S)
#define SYSTIMER_TARGET1_PERIOD_MODE_V  0x00000001
#define SYSTIMER_TARGET1_PERIOD_MODE_S  30
/** SYSTIMER_TARGET1_WORK_EN : R/W; bitpos: [31]; default: 0;
 *  system timer target1 work enable
 */
#define SYSTIMER_TARGET1_WORK_EN    (BIT(31))
#define SYSTIMER_TARGET1_WORK_EN_M  (SYSTIMER_TARGET1_WORK_EN_V << SYSTIMER_TARGET1_WORK_EN_S)
#define SYSTIMER_TARGET1_WORK_EN_V  0x00000001
#define SYSTIMER_TARGET1_WORK_EN_S  31


/** SYSTIMER_TARGET2_CONF_REG register
 *  Configure system timer target2 work mode
 */
#define SYSTIMER_TARGET2_CONF_REG (DR_REG_SYSTIMER_BASE + 0x34)
/** SYSTIMER_TARGET2_PERIOD : R/W; bitpos: [30:0]; default: 0;
 *  System timer target2 alarm period
 */
#define SYSTIMER_TARGET2_PERIOD    0x3FFFFFFF
#define SYSTIMER_TARGET2_PERIOD_M  (SYSTIMER_TARGET2_PERIOD_V << SYSTIMER_TARGET2_PERIOD_S)
#define SYSTIMER_TARGET2_PERIOD_V  0x3FFFFFFF
#define SYSTIMER_TARGET2_PERIOD_S  0
/** SYSTIMER_TARGET2_PERIOD_MODE : R/W; bitpos: [30]; default: 0;
 *  Whether system timer target2 work in period mode
 */
#define SYSTIMER_TARGET2_PERIOD_MODE    (BIT(30))
#define SYSTIMER_TARGET2_PERIOD_MODE_M  (SYSTIMER_TARGET2_PERIOD_MODE_V << SYSTIMER_TARGET2_PERIOD_MODE_S)
#define SYSTIMER_TARGET2_PERIOD_MODE_V  0x00000001
#define SYSTIMER_TARGET2_PERIOD_MODE_S  30
/** SYSTIMER_TARGET2_WORK_EN : R/W; bitpos: [31]; default: 0;
 *  system timer target2 work enable
 */
#define SYSTIMER_TARGET2_WORK_EN    (BIT(31))
#define SYSTIMER_TARGET2_WORK_EN_M  (SYSTIMER_TARGET2_WORK_EN_V << SYSTIMER_TARGET2_WORK_EN_S)
#define SYSTIMER_TARGET2_WORK_EN_V  0x00000001
#define SYSTIMER_TARGET2_WORK_EN_S  31


/** SYSTIMER_UPDATE_REG register
 *  Read out system timer value
 */
#define SYSTIMER_UPDATE_REG (DR_REG_SYSTIMER_BASE + 0x38)
/** SYSTIMER_TIMER_VALUE_VALID : RO; bitpos: [30]; default: 0;
 *  If it is valid to read out timer value from register
 */
#define SYSTIMER_TIMER_VALUE_VALID    (BIT(30))
#define SYSTIMER_TIMER_VALUE_VALID_M  (SYSTIMER_TIMER_VALUE_VALID_V << SYSTIMER_TIMER_VALUE_VALID_S)
#define SYSTIMER_TIMER_VALUE_VALID_V  0x00000001
#define SYSTIMER_TIMER_VALUE_VALID_S  30
/** SYSTIMER_TIMER_UPDATE : WO; bitpos: [31]; default: 0;
 *  Update system timer value to register
 */
#define SYSTIMER_TIMER_UPDATE    (BIT(31))
#define SYSTIMER_TIMER_UPDATE_M  (SYSTIMER_TIMER_UPDATE_V << SYSTIMER_TIMER_UPDATE_S)
#define SYSTIMER_TIMER_UPDATE_V  0x00000001
#define SYSTIMER_TIMER_UPDATE_S  31


/** SYSTIMER_VALUE_HI_REG register
 *  system timer high 32-bit
 */
#define SYSTIMER_VALUE_HI_REG (DR_REG_SYSTIMER_BASE + 0x3c)
/** SYSTIMER_TIMER_VALUE_HI : RO; bitpos: [32:0]; default: 0;
 *  system timer high 32-bit
 */
#define SYSTIMER_TIMER_VALUE_HI    0xFFFFFFFF
#define SYSTIMER_TIMER_VALUE_HI_M  (SYSTIMER_TIMER_VALUE_HI_V << SYSTIMER_TIMER_VALUE_HI_S)
#define SYSTIMER_TIMER_VALUE_HI_V  0xFFFFFFFF
#define SYSTIMER_TIMER_VALUE_HI_S  0


/** SYSTIMER_VALUE_LO_REG register
 *  system timer low 32-bit
 */
#define SYSTIMER_VALUE_LO_REG (DR_REG_SYSTIMER_BASE + 0x40)
/** SYSTIMER_TIMER_VALUE_LO : RO; bitpos: [32:0]; default: 0;
 *  system timer low 32-bit
 */
#define SYSTIMER_TIMER_VALUE_LO    0xFFFFFFFF
#define SYSTIMER_TIMER_VALUE_LO_M  (SYSTIMER_TIMER_VALUE_LO_V << SYSTIMER_TIMER_VALUE_LO_S)
#define SYSTIMER_TIMER_VALUE_LO_V  0xFFFFFFFF
#define SYSTIMER_TIMER_VALUE_LO_S  0


/** SYSTIMER_INT_ENA_REG register
 *  system timer interrupt enable
 */
#define SYSTIMER_INT_ENA_REG (DR_REG_SYSTIMER_BASE + 0x44)
/** SYSTIMER_INT0_ENA : R/W; bitpos: [0]; default: 0;
 *  system timer target0 interrupt enable
 */
#define SYSTIMER_INT0_ENA    (BIT(0))
#define SYSTIMER_INT0_ENA_M  (SYSTIMER_INT0_ENA_V << SYSTIMER_INT0_ENA_S)
#define SYSTIMER_INT0_ENA_V  0x00000001
#define SYSTIMER_INT0_ENA_S  0
/** SYSTIMER_INT1_ENA : R/W; bitpos: [1]; default: 0;
 *  system timer target1 interrupt enable
 */
#define SYSTIMER_INT1_ENA    (BIT(1))
#define SYSTIMER_INT1_ENA_M  (SYSTIMER_INT1_ENA_V << SYSTIMER_INT1_ENA_S)
#define SYSTIMER_INT1_ENA_V  0x00000001
#define SYSTIMER_INT1_ENA_S  1
/** SYSTIMER_INT2_ENA : R/W; bitpos: [2]; default: 0;
 *  system timer target2 interrupt enable
 */
#define SYSTIMER_INT2_ENA    (BIT(2))
#define SYSTIMER_INT2_ENA_M  (SYSTIMER_INT2_ENA_V << SYSTIMER_INT2_ENA_S)
#define SYSTIMER_INT2_ENA_V  0x00000001
#define SYSTIMER_INT2_ENA_S  2


/** SYSTIMER_INT_RAW_REG register
 *  system timer interrupt raw
 */
#define SYSTIMER_INT_RAW_REG (DR_REG_SYSTIMER_BASE + 0x48)
/** SYSTIMER_INT0_RAW : RO; bitpos: [0]; default: 0;
 *  system timer target0 interrupt raw
 */
#define SYSTIMER_INT0_RAW    (BIT(0))
#define SYSTIMER_INT0_RAW_M  (SYSTIMER_INT0_RAW_V << SYSTIMER_INT0_RAW_S)
#define SYSTIMER_INT0_RAW_V  0x00000001
#define SYSTIMER_INT0_RAW_S  0
/** SYSTIMER_INT1_RAW : RO; bitpos: [1]; default: 0;
 *  system timer target1 interrupt raw
 */
#define SYSTIMER_INT1_RAW    (BIT(1))
#define SYSTIMER_INT1_RAW_M  (SYSTIMER_INT1_RAW_V << SYSTIMER_INT1_RAW_S)
#define SYSTIMER_INT1_RAW_V  0x00000001
#define SYSTIMER_INT1_RAW_S  1
/** SYSTIMER_INT2_RAW : RO; bitpos: [2]; default: 0;
 *  system timer target2 interrupt raw
 */
#define SYSTIMER_INT2_RAW    (BIT(2))
#define SYSTIMER_INT2_RAW_M  (SYSTIMER_INT2_RAW_V << SYSTIMER_INT2_RAW_S)
#define SYSTIMER_INT2_RAW_V  0x00000001
#define SYSTIMER_INT2_RAW_S  2


/** SYSTIMER_INT_CLR_REG register
 *  system timer interrupt clear
 */
#define SYSTIMER_INT_CLR_REG (DR_REG_SYSTIMER_BASE + 0x4c)
/** SYSTIMER_INT0_CLR : WO; bitpos: [0]; default: 0;
 *  system timer target0 interrupt clear
 */
#define SYSTIMER_INT0_CLR    (BIT(0))
#define SYSTIMER_INT0_CLR_M  (SYSTIMER_INT0_CLR_V << SYSTIMER_INT0_CLR_S)
#define SYSTIMER_INT0_CLR_V  0x00000001
#define SYSTIMER_INT0_CLR_S  0
/** SYSTIMER_INT1_CLR : WO; bitpos: [1]; default: 0;
 *  system timer target1 interrupt clear
 */
#define SYSTIMER_INT1_CLR    (BIT(1))
#define SYSTIMER_INT1_CLR_M  (SYSTIMER_INT1_CLR_V << SYSTIMER_INT1_CLR_S)
#define SYSTIMER_INT1_CLR_V  0x00000001
#define SYSTIMER_INT1_CLR_S  1
/** SYSTIMER_INT2_CLR : WO; bitpos: [2]; default: 0;
 *  system timer target2 interrupt clear
 */
#define SYSTIMER_INT2_CLR    (BIT(2))
#define SYSTIMER_INT2_CLR_M  (SYSTIMER_INT2_CLR_V << SYSTIMER_INT2_CLR_S)
#define SYSTIMER_INT2_CLR_V  0x00000001
#define SYSTIMER_INT2_CLR_S  2


/** SYSTIMER_DATE_REG register
 *  system timer register version
 */
#define SYSTIMER_DATE_REG (DR_REG_SYSTIMER_BASE + 0xfc)
/** SYSTIMER_DATE : R/W; bitpos: [32:0]; default: 25194848;
 *  system timer register version
 */
#define SYSTIMER_DATE    0xFFFFFFFF
#define SYSTIMER_DATE_M  (SYSTIMER_DATE_V << SYSTIMER_DATE_S)
#define SYSTIMER_DATE_V  0xFFFFFFFF
#define SYSTIMER_DATE_S  0


#ifdef __cplusplus
}
#endif


