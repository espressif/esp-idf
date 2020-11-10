/** Copyright 2020 Espressif Systems (Shanghai) PTE LTD
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif
/** Configuration registers */

/** DEDIC_GPIO_OUT_DRT_REG register
 *  Dedicated GPIO Directive Output
 *  register
 */
#define DEDIC_GPIO_OUT_DRT_REG (DR_REG_DEDICATED_GPIO_BASE + 0x0)
/* DEDIC_GPIO_OUT_DRT_VLAUE : WO; bitpos: [8:0]; default: 0;
 * This register is used to configure directive output value of 8-channel
 * dedicated
 * gpio.
 */
#define DEDIC_GPIO_OUT_DRT_VLAUE    0x000000FF
#define DEDIC_GPIO_OUT_DRT_VLAUE_M  (DEDIC_GPIO_OUT_DRT_VLAUE_V << DEDIC_GPIO_OUT_DRT_VLAUE_S)
#define DEDIC_GPIO_OUT_DRT_VLAUE_V  0x000000FF
#define DEDIC_GPIO_OUT_DRT_VLAUE_S  0

/** DEDIC_GPIO_OUT_MSK_REG register
 *  Dedicated GPIO Mask Output
 *  register
 */
#define DEDIC_GPIO_OUT_MSK_REG (DR_REG_DEDICATED_GPIO_BASE + 0x4)
/* DEDIC_GPIO_OUT_VALUE : WO; bitpos: [8:0]; default: 0;
 * This register is used to configure masked output value of 8-channel
 * dedicated
 * gpio.
 */
#define DEDIC_GPIO_OUT_VALUE    0x000000FF
#define DEDIC_GPIO_OUT_VALUE_M  (DEDIC_GPIO_OUT_VALUE_V << DEDIC_GPIO_OUT_VALUE_S)
#define DEDIC_GPIO_OUT_VALUE_V  0x000000FF
#define DEDIC_GPIO_OUT_VALUE_S  0
/* DEDIC_GPIO_OUT_MSK : WO; bitpos: [16:8]; default: 0;
 * This register is used to configure channels which would be masked. 1:
 * corresponding channel's output would be
 * masked.
 */
#define DEDIC_GPIO_OUT_MSK    0x000000FF
#define DEDIC_GPIO_OUT_MSK_M  (DEDIC_GPIO_OUT_MSK_V << DEDIC_GPIO_OUT_MSK_S)
#define DEDIC_GPIO_OUT_MSK_V  0x000000FF
#define DEDIC_GPIO_OUT_MSK_S  8

/** DEDIC_GPIO_OUT_IDV_REG register
 *  Dedicated GPIO Individual Output
 *  register
 */
#define DEDIC_GPIO_OUT_IDV_REG (DR_REG_DEDICATED_GPIO_BASE + 0x8)
/* DEDIC_GPIO_OUT_IDV_CH0 : WO; bitpos: [2:0]; default: 0;
 * Configure channel 0 output value. 0: hold output value; 1: set output
 * value; 2: clear output value; 3: inverse output
 * value.
 */
#define DEDIC_GPIO_OUT_IDV_CH0    0x00000003
#define DEDIC_GPIO_OUT_IDV_CH0_M  (DEDIC_GPIO_OUT_IDV_CH0_V << DEDIC_GPIO_OUT_IDV_CH0_S)
#define DEDIC_GPIO_OUT_IDV_CH0_V  0x00000003
#define DEDIC_GPIO_OUT_IDV_CH0_S  0
/* DEDIC_GPIO_OUT_IDV_CH1 : WO; bitpos: [4:2]; default: 0;
 * Configure channel 1 output value. 0: hold output value; 1: set output
 * value; 2: clear output value; 3: inverse output
 * value.
 */
#define DEDIC_GPIO_OUT_IDV_CH1    0x00000003
#define DEDIC_GPIO_OUT_IDV_CH1_M  (DEDIC_GPIO_OUT_IDV_CH1_V << DEDIC_GPIO_OUT_IDV_CH1_S)
#define DEDIC_GPIO_OUT_IDV_CH1_V  0x00000003
#define DEDIC_GPIO_OUT_IDV_CH1_S  2
/* DEDIC_GPIO_OUT_IDV_CH2 : WO; bitpos: [6:4]; default: 0;
 * Configure channel 2 output value. 0: hold output value; 1: set output
 * value; 2: clear output value; 3: inverse output
 * value.
 */
#define DEDIC_GPIO_OUT_IDV_CH2    0x00000003
#define DEDIC_GPIO_OUT_IDV_CH2_M  (DEDIC_GPIO_OUT_IDV_CH2_V << DEDIC_GPIO_OUT_IDV_CH2_S)
#define DEDIC_GPIO_OUT_IDV_CH2_V  0x00000003
#define DEDIC_GPIO_OUT_IDV_CH2_S  4
/* DEDIC_GPIO_OUT_IDV_CH3 : WO; bitpos: [8:6]; default: 0;
 * Configure channel 3 output value. 0: hold output value; 1: set output
 * value; 2: clear output value; 3: inverse output
 * value.
 */
#define DEDIC_GPIO_OUT_IDV_CH3    0x00000003
#define DEDIC_GPIO_OUT_IDV_CH3_M  (DEDIC_GPIO_OUT_IDV_CH3_V << DEDIC_GPIO_OUT_IDV_CH3_S)
#define DEDIC_GPIO_OUT_IDV_CH3_V  0x00000003
#define DEDIC_GPIO_OUT_IDV_CH3_S  6
/* DEDIC_GPIO_OUT_IDV_CH4 : WO; bitpos: [10:8]; default: 0;
 * Configure channel 4 output value. 0: hold output value; 1: set output
 * value; 2: clear output value; 3: inverse output
 * value.
 */
#define DEDIC_GPIO_OUT_IDV_CH4    0x00000003
#define DEDIC_GPIO_OUT_IDV_CH4_M  (DEDIC_GPIO_OUT_IDV_CH4_V << DEDIC_GPIO_OUT_IDV_CH4_S)
#define DEDIC_GPIO_OUT_IDV_CH4_V  0x00000003
#define DEDIC_GPIO_OUT_IDV_CH4_S  8
/* DEDIC_GPIO_OUT_IDV_CH5 : WO; bitpos: [12:10]; default: 0;
 * Configure channel 5 output value. 0: hold output value; 1: set output
 * value; 2: clear output value; 3: inverse output
 * value.
 */
#define DEDIC_GPIO_OUT_IDV_CH5    0x00000003
#define DEDIC_GPIO_OUT_IDV_CH5_M  (DEDIC_GPIO_OUT_IDV_CH5_V << DEDIC_GPIO_OUT_IDV_CH5_S)
#define DEDIC_GPIO_OUT_IDV_CH5_V  0x00000003
#define DEDIC_GPIO_OUT_IDV_CH5_S  10
/* DEDIC_GPIO_OUT_IDV_CH6 : WO; bitpos: [14:12]; default: 0;
 * Configure channel 6 output value. 0: hold output value; 1: set output
 * value; 2: clear output value; 3: inverse output
 * value.
 */
#define DEDIC_GPIO_OUT_IDV_CH6    0x00000003
#define DEDIC_GPIO_OUT_IDV_CH6_M  (DEDIC_GPIO_OUT_IDV_CH6_V << DEDIC_GPIO_OUT_IDV_CH6_S)
#define DEDIC_GPIO_OUT_IDV_CH6_V  0x00000003
#define DEDIC_GPIO_OUT_IDV_CH6_S  12
/* DEDIC_GPIO_OUT_IDV_CH7 : WO; bitpos: [16:14]; default: 0;
 * Configure channel 7 output value. 0: hold output value; 1: set output
 * value; 2: clear output value; 3: inverse output
 * value.
 */
#define DEDIC_GPIO_OUT_IDV_CH7    0x00000003
#define DEDIC_GPIO_OUT_IDV_CH7_M  (DEDIC_GPIO_OUT_IDV_CH7_V << DEDIC_GPIO_OUT_IDV_CH7_S)
#define DEDIC_GPIO_OUT_IDV_CH7_V  0x00000003
#define DEDIC_GPIO_OUT_IDV_CH7_S  14

/** DEDIC_GPIO_OUT_CPU_REG register
 *  Dedicated GPIO Output Mode Select
 *  register
 */
#define DEDIC_GPIO_OUT_CPU_REG (DR_REG_DEDICATED_GPIO_BASE + 0x10)
/* DEDIC_GPIO_OUT_CPU_SEL0 : R/W; bitpos: [0]; default: 0;
 * Select gpio out value configured by registers or CPU instructors for
 * channel 0. 1: select CPU
 * instructors.
 */
#define DEDIC_GPIO_OUT_CPU_SEL0    (BIT(0))
#define DEDIC_GPIO_OUT_CPU_SEL0_M  (DEDIC_GPIO_OUT_CPU_SEL0_V << DEDIC_GPIO_OUT_CPU_SEL0_S)
#define DEDIC_GPIO_OUT_CPU_SEL0_V  0x00000001
#define DEDIC_GPIO_OUT_CPU_SEL0_S  0
/* DEDIC_GPIO_OUT_CPU_SEL1 : R/W; bitpos: [1]; default: 0;
 * Select gpio out value configured by registers or CPU instructors for
 * channel 1. 1: select CPU
 * instructors.
 */
#define DEDIC_GPIO_OUT_CPU_SEL1    (BIT(1))
#define DEDIC_GPIO_OUT_CPU_SEL1_M  (DEDIC_GPIO_OUT_CPU_SEL1_V << DEDIC_GPIO_OUT_CPU_SEL1_S)
#define DEDIC_GPIO_OUT_CPU_SEL1_V  0x00000001
#define DEDIC_GPIO_OUT_CPU_SEL1_S  1
/* DEDIC_GPIO_OUT_CPU_SEL2 : R/W; bitpos: [2]; default: 0;
 * Select gpio out value configured by registers or CPU instructors for
 * channel 2. 1: select CPU
 * instructors.
 */
#define DEDIC_GPIO_OUT_CPU_SEL2    (BIT(2))
#define DEDIC_GPIO_OUT_CPU_SEL2_M  (DEDIC_GPIO_OUT_CPU_SEL2_V << DEDIC_GPIO_OUT_CPU_SEL2_S)
#define DEDIC_GPIO_OUT_CPU_SEL2_V  0x00000001
#define DEDIC_GPIO_OUT_CPU_SEL2_S  2
/* DEDIC_GPIO_OUT_CPU_SEL3 : R/W; bitpos: [3]; default: 0;
 * Select gpio out value configured by registers or CPU instructors for
 * channel 3. 1: select CPU
 * instructors.
 */
#define DEDIC_GPIO_OUT_CPU_SEL3    (BIT(3))
#define DEDIC_GPIO_OUT_CPU_SEL3_M  (DEDIC_GPIO_OUT_CPU_SEL3_V << DEDIC_GPIO_OUT_CPU_SEL3_S)
#define DEDIC_GPIO_OUT_CPU_SEL3_V  0x00000001
#define DEDIC_GPIO_OUT_CPU_SEL3_S  3
/* DEDIC_GPIO_OUT_CPU_SEL4 : R/W; bitpos: [4]; default: 0;
 * Select gpio out value configured by registers or CPU instructors for
 * channel 4. 1: select CPU
 * instructors.
 */
#define DEDIC_GPIO_OUT_CPU_SEL4    (BIT(4))
#define DEDIC_GPIO_OUT_CPU_SEL4_M  (DEDIC_GPIO_OUT_CPU_SEL4_V << DEDIC_GPIO_OUT_CPU_SEL4_S)
#define DEDIC_GPIO_OUT_CPU_SEL4_V  0x00000001
#define DEDIC_GPIO_OUT_CPU_SEL4_S  4
/* DEDIC_GPIO_OUT_CPU_SEL5 : R/W; bitpos: [5]; default: 0;
 * Select gpio out value configured by registers or CPU instructors for
 * channel 5. 1: select CPU
 * instructors.
 */
#define DEDIC_GPIO_OUT_CPU_SEL5    (BIT(5))
#define DEDIC_GPIO_OUT_CPU_SEL5_M  (DEDIC_GPIO_OUT_CPU_SEL5_V << DEDIC_GPIO_OUT_CPU_SEL5_S)
#define DEDIC_GPIO_OUT_CPU_SEL5_V  0x00000001
#define DEDIC_GPIO_OUT_CPU_SEL5_S  5
/* DEDIC_GPIO_OUT_CPU_SEL6 : R/W; bitpos: [6]; default: 0;
 * Select gpio out value configured by registers or CPU instructors for
 * channel 6. 1: select CPU
 * instructors.
 */
#define DEDIC_GPIO_OUT_CPU_SEL6    (BIT(6))
#define DEDIC_GPIO_OUT_CPU_SEL6_M  (DEDIC_GPIO_OUT_CPU_SEL6_V << DEDIC_GPIO_OUT_CPU_SEL6_S)
#define DEDIC_GPIO_OUT_CPU_SEL6_V  0x00000001
#define DEDIC_GPIO_OUT_CPU_SEL6_S  6
/* DEDIC_GPIO_OUT_CPU_SEL7 : R/W; bitpos: [7]; default: 0;
 * Select gpio out value configured by registers or CPU instructors for
 * channel 7. 1: select CPU
 * instructors.
 */
#define DEDIC_GPIO_OUT_CPU_SEL7    (BIT(7))
#define DEDIC_GPIO_OUT_CPU_SEL7_M  (DEDIC_GPIO_OUT_CPU_SEL7_V << DEDIC_GPIO_OUT_CPU_SEL7_S)
#define DEDIC_GPIO_OUT_CPU_SEL7_V  0x00000001
#define DEDIC_GPIO_OUT_CPU_SEL7_S  7

/** DEDIC_GPIO_IN_DLY_REG register
 *  Dedicated GPIO Input Delay Configuration
 *  register
 */
#define DEDIC_GPIO_IN_DLY_REG (DR_REG_DEDICATED_GPIO_BASE + 0x14)
/* DEDIC_GPIO_IN_DLY_CH0 : R/W; bitpos: [2:0]; default: 0;
 * Configure gpio 0 input delay. 0: No delay; 1: one clock delay; 2: two
 * clock delay; 3: three clock
 * delay.
 */
#define DEDIC_GPIO_IN_DLY_CH0    0x00000003
#define DEDIC_GPIO_IN_DLY_CH0_M  (DEDIC_GPIO_IN_DLY_CH0_V << DEDIC_GPIO_IN_DLY_CH0_S)
#define DEDIC_GPIO_IN_DLY_CH0_V  0x00000003
#define DEDIC_GPIO_IN_DLY_CH0_S  0
/* DEDIC_GPIO_IN_DLY_CH1 : R/W; bitpos: [4:2]; default: 0;
 * Configure gpio 1 input delay. 0: No delay; 1: one clock delay; 2: two
 * clock delay; 3: three clock
 * delay.
 */
#define DEDIC_GPIO_IN_DLY_CH1    0x00000003
#define DEDIC_GPIO_IN_DLY_CH1_M  (DEDIC_GPIO_IN_DLY_CH1_V << DEDIC_GPIO_IN_DLY_CH1_S)
#define DEDIC_GPIO_IN_DLY_CH1_V  0x00000003
#define DEDIC_GPIO_IN_DLY_CH1_S  2
/* DEDIC_GPIO_IN_DLY_CH2 : R/W; bitpos: [6:4]; default: 0;
 * Configure gpio 2 input delay. 0: No delay; 1: one clock delay; 2: two
 * clock delay; 3: three clock
 * delay.
 */
#define DEDIC_GPIO_IN_DLY_CH2    0x00000003
#define DEDIC_GPIO_IN_DLY_CH2_M  (DEDIC_GPIO_IN_DLY_CH2_V << DEDIC_GPIO_IN_DLY_CH2_S)
#define DEDIC_GPIO_IN_DLY_CH2_V  0x00000003
#define DEDIC_GPIO_IN_DLY_CH2_S  4
/* DEDIC_GPIO_IN_DLY_CH3 : R/W; bitpos: [8:6]; default: 0;
 * Configure gpio 3 input delay. 0: No delay; 1: one clock delay; 2: two
 * clock delay; 3: three clock
 * delay.
 */
#define DEDIC_GPIO_IN_DLY_CH3    0x00000003
#define DEDIC_GPIO_IN_DLY_CH3_M  (DEDIC_GPIO_IN_DLY_CH3_V << DEDIC_GPIO_IN_DLY_CH3_S)
#define DEDIC_GPIO_IN_DLY_CH3_V  0x00000003
#define DEDIC_GPIO_IN_DLY_CH3_S  6
/* DEDIC_GPIO_IN_DLY_CH4 : R/W; bitpos: [10:8]; default: 0;
 * Configure gpio 4 input delay. 0: No delay; 1: one clock delay; 2: two
 * clock delay; 3: three clock
 * delay.
 */
#define DEDIC_GPIO_IN_DLY_CH4    0x00000003
#define DEDIC_GPIO_IN_DLY_CH4_M  (DEDIC_GPIO_IN_DLY_CH4_V << DEDIC_GPIO_IN_DLY_CH4_S)
#define DEDIC_GPIO_IN_DLY_CH4_V  0x00000003
#define DEDIC_GPIO_IN_DLY_CH4_S  8
/* DEDIC_GPIO_IN_DLY_CH5 : R/W; bitpos: [12:10]; default: 0;
 * Configure gpio 5 input delay. 0: No delay; 1: one clock delay; 2: two
 * clock delay; 3: three clock
 * delay.
 */
#define DEDIC_GPIO_IN_DLY_CH5    0x00000003
#define DEDIC_GPIO_IN_DLY_CH5_M  (DEDIC_GPIO_IN_DLY_CH5_V << DEDIC_GPIO_IN_DLY_CH5_S)
#define DEDIC_GPIO_IN_DLY_CH5_V  0x00000003
#define DEDIC_GPIO_IN_DLY_CH5_S  10
/* DEDIC_GPIO_IN_DLY_CH6 : R/W; bitpos: [14:12]; default: 0;
 * Configure gpio 6 input delay. 0: No delay; 1: one clock delay; 2: two
 * clock delay; 3: three clock
 * delay.
 */
#define DEDIC_GPIO_IN_DLY_CH6    0x00000003
#define DEDIC_GPIO_IN_DLY_CH6_M  (DEDIC_GPIO_IN_DLY_CH6_V << DEDIC_GPIO_IN_DLY_CH6_S)
#define DEDIC_GPIO_IN_DLY_CH6_V  0x00000003
#define DEDIC_GPIO_IN_DLY_CH6_S  12
/* DEDIC_GPIO_IN_DLY_CH7 : R/W; bitpos: [16:14]; default: 0;
 * Configure gpio 7 input delay. 0: No delay; 1: one clock delay; 2: two
 * clock delay; 3: three clock
 * delay.
 */
#define DEDIC_GPIO_IN_DLY_CH7    0x00000003
#define DEDIC_GPIO_IN_DLY_CH7_M  (DEDIC_GPIO_IN_DLY_CH7_V << DEDIC_GPIO_IN_DLY_CH7_S)
#define DEDIC_GPIO_IN_DLY_CH7_V  0x00000003
#define DEDIC_GPIO_IN_DLY_CH7_S  14

/** DEDIC_GPIO_INTR_RCGN_REG register
 *  Dedicated GPIO Interrupts Generate Mode
 *  register
 */
#define DEDIC_GPIO_INTR_RCGN_REG (DR_REG_DEDICATED_GPIO_BASE + 0x1c)
/* DEDIC_GPIO_INTR_MODE_CH0 : R/W; bitpos: [3:0]; default: 0;
 * Configure channel 0 interrupt generate mode.
 * 0/1: do not generate interrupt;
 * 2: low level trigger;
 * 3: high level trigger;
 * 4: falling edge trigger;
 * 5: raising edge trigger;
 * 6/7: falling and raising edge
 * trigger.
 */
#define DEDIC_GPIO_INTR_MODE_CH0    0x00000007
#define DEDIC_GPIO_INTR_MODE_CH0_M  (DEDIC_GPIO_INTR_MODE_CH0_V << DEDIC_GPIO_INTR_MODE_CH0_S)
#define DEDIC_GPIO_INTR_MODE_CH0_V  0x00000007
#define DEDIC_GPIO_INTR_MODE_CH0_S  0
/* DEDIC_GPIO_INTR_MODE_CH1 : R/W; bitpos: [6:3]; default: 0;
 * Configure channel 1 interrupt generate mode.
 * 0/1: do not generate interrupt;
 * 2: low level trigger;
 * 3: high level trigger;
 * 4: falling edge trigger;
 * 5: raising edge trigger;
 * 6/7: falling and raising edge
 * trigger.
 */
#define DEDIC_GPIO_INTR_MODE_CH1    0x00000007
#define DEDIC_GPIO_INTR_MODE_CH1_M  (DEDIC_GPIO_INTR_MODE_CH1_V << DEDIC_GPIO_INTR_MODE_CH1_S)
#define DEDIC_GPIO_INTR_MODE_CH1_V  0x00000007
#define DEDIC_GPIO_INTR_MODE_CH1_S  3
/* DEDIC_GPIO_INTR_MODE_CH2 : R/W; bitpos: [9:6]; default: 0;
 * Configure channel 2 interrupt generate mode.
 * 0/1: do not generate interrupt;
 * 2: low level trigger;
 * 3: high level trigger;
 * 4: falling edge trigger;
 * 5: raising edge trigger;
 * 6/7: falling and raising edge
 * trigger.
 */
#define DEDIC_GPIO_INTR_MODE_CH2    0x00000007
#define DEDIC_GPIO_INTR_MODE_CH2_M  (DEDIC_GPIO_INTR_MODE_CH2_V << DEDIC_GPIO_INTR_MODE_CH2_S)
#define DEDIC_GPIO_INTR_MODE_CH2_V  0x00000007
#define DEDIC_GPIO_INTR_MODE_CH2_S  6
/* DEDIC_GPIO_INTR_MODE_CH3 : R/W; bitpos: [12:9]; default: 0;
 * Configure channel 3 interrupt generate mode.
 * 0/1: do not generate interrupt;
 * 2: low level trigger;
 * 3: high level trigger;
 * 4: falling edge trigger;
 * 5: raising edge trigger;
 * 6/7: falling and raising edge
 * trigger.
 */
#define DEDIC_GPIO_INTR_MODE_CH3    0x00000007
#define DEDIC_GPIO_INTR_MODE_CH3_M  (DEDIC_GPIO_INTR_MODE_CH3_V << DEDIC_GPIO_INTR_MODE_CH3_S)
#define DEDIC_GPIO_INTR_MODE_CH3_V  0x00000007
#define DEDIC_GPIO_INTR_MODE_CH3_S  9
/* DEDIC_GPIO_INTR_MODE_CH4 : R/W; bitpos: [15:12]; default: 0;
 * Configure channel 4 interrupt generate mode.
 * 0/1: do not generate interrupt;
 * 2: low level trigger;
 * 3: high level trigger;
 * 4: falling edge trigger;
 * 5: raising edge trigger;
 * 6/7: falling and raising edge
 * trigger.
 */
#define DEDIC_GPIO_INTR_MODE_CH4    0x00000007
#define DEDIC_GPIO_INTR_MODE_CH4_M  (DEDIC_GPIO_INTR_MODE_CH4_V << DEDIC_GPIO_INTR_MODE_CH4_S)
#define DEDIC_GPIO_INTR_MODE_CH4_V  0x00000007
#define DEDIC_GPIO_INTR_MODE_CH4_S  12
/* DEDIC_GPIO_INTR_MODE_CH5 : R/W; bitpos: [18:15]; default: 0;
 * Configure channel 5 interrupt generate mode.
 * 0/1: do not generate interrupt;
 * 2: low level trigger;
 * 3: high level trigger;
 * 4: falling edge trigger;
 * 5: raising edge trigger;
 * 6/7: falling and raising edge
 * trigger.
 */
#define DEDIC_GPIO_INTR_MODE_CH5    0x00000007
#define DEDIC_GPIO_INTR_MODE_CH5_M  (DEDIC_GPIO_INTR_MODE_CH5_V << DEDIC_GPIO_INTR_MODE_CH5_S)
#define DEDIC_GPIO_INTR_MODE_CH5_V  0x00000007
#define DEDIC_GPIO_INTR_MODE_CH5_S  15
/* DEDIC_GPIO_INTR_MODE_CH6 : R/W; bitpos: [21:18]; default: 0;
 * Configure channel 6 interrupt generate mode.
 * 0/1: do not generate interrupt;
 * 2: low level trigger;
 * 3: high level trigger;
 * 4: falling edge trigger;
 * 5: raising edge trigger;
 * 6/7: falling and raising edge
 * trigger.
 */
#define DEDIC_GPIO_INTR_MODE_CH6    0x00000007
#define DEDIC_GPIO_INTR_MODE_CH6_M  (DEDIC_GPIO_INTR_MODE_CH6_V << DEDIC_GPIO_INTR_MODE_CH6_S)
#define DEDIC_GPIO_INTR_MODE_CH6_V  0x00000007
#define DEDIC_GPIO_INTR_MODE_CH6_S  18
/* DEDIC_GPIO_INTR_MODE_CH7 : R/W; bitpos: [24:21]; default: 0;
 * Configure channel 7 interrupt generate mode.
 * 0/1: do not generate interrupt;
 * 2: low level trigger;
 * 3: high level trigger;
 * 4: falling edge trigger;
 * 5: raising edge trigger;
 * 6/7: falling and raising edge
 * trigger.
 */
#define DEDIC_GPIO_INTR_MODE_CH7    0x00000007
#define DEDIC_GPIO_INTR_MODE_CH7_M  (DEDIC_GPIO_INTR_MODE_CH7_V << DEDIC_GPIO_INTR_MODE_CH7_S)
#define DEDIC_GPIO_INTR_MODE_CH7_V  0x00000007
#define DEDIC_GPIO_INTR_MODE_CH7_S  21


/** Status registers */

/** DEDIC_GPIO_OUT_SCAN_REG register
 *  Dedicated GPIO Output Status
 *  register
 */
#define DEDIC_GPIO_OUT_SCAN_REG (DR_REG_DEDICATED_GPIO_BASE + 0xc)
/* DEDIC_GPIO_OUT_STATUS : RO; bitpos: [8:0]; default: 0;
 * gpio out value configured by DEDIC_GPIO_OUT_DRT_REG,
 * DEDIC_GPIO_OUT_MSK_REG,
 * DEDIC_GPIO_OUT_IDV_REG.
 */
#define DEDIC_GPIO_OUT_STATUS    0x000000FF
#define DEDIC_GPIO_OUT_STATUS_M  (DEDIC_GPIO_OUT_STATUS_V << DEDIC_GPIO_OUT_STATUS_S)
#define DEDIC_GPIO_OUT_STATUS_V  0x000000FF
#define DEDIC_GPIO_OUT_STATUS_S  0

/** DEDIC_GPIO_IN_SCAN_REG register
 *  Dedicated GPIO Input Status
 *  register
 */
#define DEDIC_GPIO_IN_SCAN_REG (DR_REG_DEDICATED_GPIO_BASE + 0x18)
/* DEDIC_GPIO_IN_STATUS : RO; bitpos: [8:0]; default: 0;
 * gpio in value after configured by
 * DEDIC_GPIO_IN_DLY_REG.
 */
#define DEDIC_GPIO_IN_STATUS    0x000000FF
#define DEDIC_GPIO_IN_STATUS_M  (DEDIC_GPIO_IN_STATUS_V << DEDIC_GPIO_IN_STATUS_S)
#define DEDIC_GPIO_IN_STATUS_V  0x000000FF
#define DEDIC_GPIO_IN_STATUS_S  0


/** Interrupt registers */

/** DEDIC_GPIO_INTR_RAW_REG register
 *  Raw interrupt
 *  status
 */
#define DEDIC_GPIO_INTR_RAW_REG (DR_REG_DEDICATED_GPIO_BASE + 0x20)
/* DEDIC_GPIO0_INT_RAW : RO; bitpos: [0]; default: 0;
 * This interrupt raw bit turns to high level when dedicated GPIO 0 has
 * level/edge change configured by
 * DEDIC_GPIO_INTR_RCGN_REG.
 */
#define DEDIC_GPIO0_INT_RAW    (BIT(0))
#define DEDIC_GPIO0_INT_RAW_M  (DEDIC_GPIO0_INT_RAW_V << DEDIC_GPIO0_INT_RAW_S)
#define DEDIC_GPIO0_INT_RAW_V  0x00000001
#define DEDIC_GPIO0_INT_RAW_S  0
/* DEDIC_GPIO1_INT_RAW : RO; bitpos: [1]; default: 0;
 * This interrupt raw bit turns to high level when dedicated GPIO 1 has
 * level/edge change configured by
 * DEDIC_GPIO_INTR_RCGN_REG.
 */
#define DEDIC_GPIO1_INT_RAW    (BIT(1))
#define DEDIC_GPIO1_INT_RAW_M  (DEDIC_GPIO1_INT_RAW_V << DEDIC_GPIO1_INT_RAW_S)
#define DEDIC_GPIO1_INT_RAW_V  0x00000001
#define DEDIC_GPIO1_INT_RAW_S  1
/* DEDIC_GPIO2_INT_RAW : RO; bitpos: [2]; default: 0;
 * This interrupt raw bit turns to high level when dedicated GPIO 2 has
 * level/edge change configured by
 * DEDIC_GPIO_INTR_RCGN_REG.
 */
#define DEDIC_GPIO2_INT_RAW    (BIT(2))
#define DEDIC_GPIO2_INT_RAW_M  (DEDIC_GPIO2_INT_RAW_V << DEDIC_GPIO2_INT_RAW_S)
#define DEDIC_GPIO2_INT_RAW_V  0x00000001
#define DEDIC_GPIO2_INT_RAW_S  2
/* DEDIC_GPIO3_INT_RAW : RO; bitpos: [3]; default: 0;
 * This interrupt raw bit turns to high level when dedicated GPIO 3 has
 * level/edge change configured by
 * DEDIC_GPIO_INTR_RCGN_REG.
 */
#define DEDIC_GPIO3_INT_RAW    (BIT(3))
#define DEDIC_GPIO3_INT_RAW_M  (DEDIC_GPIO3_INT_RAW_V << DEDIC_GPIO3_INT_RAW_S)
#define DEDIC_GPIO3_INT_RAW_V  0x00000001
#define DEDIC_GPIO3_INT_RAW_S  3
/* DEDIC_GPIO4_INT_RAW : RO; bitpos: [4]; default: 0;
 * This interrupt raw bit turns to high level when dedicated GPIO 4 has
 * level/edge change configured by
 * DEDIC_GPIO_INTR_RCGN_REG.
 */
#define DEDIC_GPIO4_INT_RAW    (BIT(4))
#define DEDIC_GPIO4_INT_RAW_M  (DEDIC_GPIO4_INT_RAW_V << DEDIC_GPIO4_INT_RAW_S)
#define DEDIC_GPIO4_INT_RAW_V  0x00000001
#define DEDIC_GPIO4_INT_RAW_S  4
/* DEDIC_GPIO5_INT_RAW : RO; bitpos: [5]; default: 0;
 * This interrupt raw bit turns to high level when dedicated GPIO 5 has
 * level/edge change configured by
 * DEDIC_GPIO_INTR_RCGN_REG.
 */
#define DEDIC_GPIO5_INT_RAW    (BIT(5))
#define DEDIC_GPIO5_INT_RAW_M  (DEDIC_GPIO5_INT_RAW_V << DEDIC_GPIO5_INT_RAW_S)
#define DEDIC_GPIO5_INT_RAW_V  0x00000001
#define DEDIC_GPIO5_INT_RAW_S  5
/* DEDIC_GPIO6_INT_RAW : RO; bitpos: [6]; default: 0;
 * This interrupt raw bit turns to high level when dedicated GPIO 6 has
 * level/edge change configured by
 * DEDIC_GPIO_INTR_RCGN_REG.
 */
#define DEDIC_GPIO6_INT_RAW    (BIT(6))
#define DEDIC_GPIO6_INT_RAW_M  (DEDIC_GPIO6_INT_RAW_V << DEDIC_GPIO6_INT_RAW_S)
#define DEDIC_GPIO6_INT_RAW_V  0x00000001
#define DEDIC_GPIO6_INT_RAW_S  6
/* DEDIC_GPIO7_INT_RAW : RO; bitpos: [7]; default: 0;
 * This interrupt raw bit turns to high level when dedicated GPIO 7 has
 * level/edge change configured by
 * DEDIC_GPIO_INTR_RCGN_REG.
 */
#define DEDIC_GPIO7_INT_RAW    (BIT(7))
#define DEDIC_GPIO7_INT_RAW_M  (DEDIC_GPIO7_INT_RAW_V << DEDIC_GPIO7_INT_RAW_S)
#define DEDIC_GPIO7_INT_RAW_V  0x00000001
#define DEDIC_GPIO7_INT_RAW_S  7

/** DEDIC_GPIO_INTR_RLS_REG register
 *  Interrupt enable
 *  bits
 */
#define DEDIC_GPIO_INTR_RLS_REG (DR_REG_DEDICATED_GPIO_BASE + 0x24)
/* DEDIC_GPIO0_INT_ENA : R/W; bitpos: [0]; default: 0;
 * This enable bit for reg_gpio0_int_st
 * register.
 */
#define DEDIC_GPIO0_INT_ENA    (BIT(0))
#define DEDIC_GPIO0_INT_ENA_M  (DEDIC_GPIO0_INT_ENA_V << DEDIC_GPIO0_INT_ENA_S)
#define DEDIC_GPIO0_INT_ENA_V  0x00000001
#define DEDIC_GPIO0_INT_ENA_S  0
/* DEDIC_GPIO1_INT_ENA : R/W; bitpos: [1]; default: 0;
 * This enable bit for reg_gpio1_int_st
 * register.
 */
#define DEDIC_GPIO1_INT_ENA    (BIT(1))
#define DEDIC_GPIO1_INT_ENA_M  (DEDIC_GPIO1_INT_ENA_V << DEDIC_GPIO1_INT_ENA_S)
#define DEDIC_GPIO1_INT_ENA_V  0x00000001
#define DEDIC_GPIO1_INT_ENA_S  1
/* DEDIC_GPIO2_INT_ENA : R/W; bitpos: [2]; default: 0;
 * This enable bit for reg_gpio2_int_st
 * register.
 */
#define DEDIC_GPIO2_INT_ENA    (BIT(2))
#define DEDIC_GPIO2_INT_ENA_M  (DEDIC_GPIO2_INT_ENA_V << DEDIC_GPIO2_INT_ENA_S)
#define DEDIC_GPIO2_INT_ENA_V  0x00000001
#define DEDIC_GPIO2_INT_ENA_S  2
/* DEDIC_GPIO3_INT_ENA : R/W; bitpos: [3]; default: 0;
 * This enable bit for reg_gpio3_int_st
 * register.
 */
#define DEDIC_GPIO3_INT_ENA    (BIT(3))
#define DEDIC_GPIO3_INT_ENA_M  (DEDIC_GPIO3_INT_ENA_V << DEDIC_GPIO3_INT_ENA_S)
#define DEDIC_GPIO3_INT_ENA_V  0x00000001
#define DEDIC_GPIO3_INT_ENA_S  3
/* DEDIC_GPIO4_INT_ENA : R/W; bitpos: [4]; default: 0;
 * This enable bit for reg_gpio4_int_st
 * register.
 */
#define DEDIC_GPIO4_INT_ENA    (BIT(4))
#define DEDIC_GPIO4_INT_ENA_M  (DEDIC_GPIO4_INT_ENA_V << DEDIC_GPIO4_INT_ENA_S)
#define DEDIC_GPIO4_INT_ENA_V  0x00000001
#define DEDIC_GPIO4_INT_ENA_S  4
/* DEDIC_GPIO5_INT_ENA : R/W; bitpos: [5]; default: 0;
 * This enable bit for reg_gpio5_int_st
 * register.
 */
#define DEDIC_GPIO5_INT_ENA    (BIT(5))
#define DEDIC_GPIO5_INT_ENA_M  (DEDIC_GPIO5_INT_ENA_V << DEDIC_GPIO5_INT_ENA_S)
#define DEDIC_GPIO5_INT_ENA_V  0x00000001
#define DEDIC_GPIO5_INT_ENA_S  5
/* DEDIC_GPIO6_INT_ENA : R/W; bitpos: [6]; default: 0;
 * This enable bit for reg_gpio6_int_st
 * register.
 */
#define DEDIC_GPIO6_INT_ENA    (BIT(6))
#define DEDIC_GPIO6_INT_ENA_M  (DEDIC_GPIO6_INT_ENA_V << DEDIC_GPIO6_INT_ENA_S)
#define DEDIC_GPIO6_INT_ENA_V  0x00000001
#define DEDIC_GPIO6_INT_ENA_S  6
/* DEDIC_GPIO7_INT_ENA : R/W; bitpos: [7]; default: 0;
 * This enable bit for reg_gpio7_int_st
 * register.
 */
#define DEDIC_GPIO7_INT_ENA    (BIT(7))
#define DEDIC_GPIO7_INT_ENA_M  (DEDIC_GPIO7_INT_ENA_V << DEDIC_GPIO7_INT_ENA_S)
#define DEDIC_GPIO7_INT_ENA_V  0x00000001
#define DEDIC_GPIO7_INT_ENA_S  7

/** DEDIC_GPIO_INTR_ST_REG register
 *  Masked interrupt
 *  status
 */
#define DEDIC_GPIO_INTR_ST_REG (DR_REG_DEDICATED_GPIO_BASE + 0x28)
/* DEDIC_GPIO0_INT_ST : RO; bitpos: [0]; default: 0;
 * This is the status bit for reg_gpio0_int_raw when reg_gpio0_int_ena is
 * set to
 * 1.
 */
#define DEDIC_GPIO0_INT_ST    (BIT(0))
#define DEDIC_GPIO0_INT_ST_M  (DEDIC_GPIO0_INT_ST_V << DEDIC_GPIO0_INT_ST_S)
#define DEDIC_GPIO0_INT_ST_V  0x00000001
#define DEDIC_GPIO0_INT_ST_S  0
/* DEDIC_GPIO1_INT_ST : RO; bitpos: [1]; default: 0;
 * This is the status bit for reg_gpio1_int_raw when reg_gpio1_int_ena is
 * set to
 * 1.
 */
#define DEDIC_GPIO1_INT_ST    (BIT(1))
#define DEDIC_GPIO1_INT_ST_M  (DEDIC_GPIO1_INT_ST_V << DEDIC_GPIO1_INT_ST_S)
#define DEDIC_GPIO1_INT_ST_V  0x00000001
#define DEDIC_GPIO1_INT_ST_S  1
/* DEDIC_GPIO2_INT_ST : RO; bitpos: [2]; default: 0;
 * This is the status bit for reg_gpio2_int_raw when reg_gpio2_int_ena is
 * set to
 * 1.
 */
#define DEDIC_GPIO2_INT_ST    (BIT(2))
#define DEDIC_GPIO2_INT_ST_M  (DEDIC_GPIO2_INT_ST_V << DEDIC_GPIO2_INT_ST_S)
#define DEDIC_GPIO2_INT_ST_V  0x00000001
#define DEDIC_GPIO2_INT_ST_S  2
/* DEDIC_GPIO3_INT_ST : RO; bitpos: [3]; default: 0;
 * This is the status bit for reg_gpio3_int_raw when reg_gpio3_int_ena is
 * set to
 * 1.
 */
#define DEDIC_GPIO3_INT_ST    (BIT(3))
#define DEDIC_GPIO3_INT_ST_M  (DEDIC_GPIO3_INT_ST_V << DEDIC_GPIO3_INT_ST_S)
#define DEDIC_GPIO3_INT_ST_V  0x00000001
#define DEDIC_GPIO3_INT_ST_S  3
/* DEDIC_GPIO4_INT_ST : RO; bitpos: [4]; default: 0;
 * This is the status bit for reg_gpio4_int_raw when reg_gpio4_int_ena is
 * set to
 * 1.
 */
#define DEDIC_GPIO4_INT_ST    (BIT(4))
#define DEDIC_GPIO4_INT_ST_M  (DEDIC_GPIO4_INT_ST_V << DEDIC_GPIO4_INT_ST_S)
#define DEDIC_GPIO4_INT_ST_V  0x00000001
#define DEDIC_GPIO4_INT_ST_S  4
/* DEDIC_GPIO5_INT_ST : RO; bitpos: [5]; default: 0;
 * This is the status bit for reg_gpio5_int_raw when reg_gpio5_int_ena is
 * set to
 * 1.
 */
#define DEDIC_GPIO5_INT_ST    (BIT(5))
#define DEDIC_GPIO5_INT_ST_M  (DEDIC_GPIO5_INT_ST_V << DEDIC_GPIO5_INT_ST_S)
#define DEDIC_GPIO5_INT_ST_V  0x00000001
#define DEDIC_GPIO5_INT_ST_S  5
/* DEDIC_GPIO6_INT_ST : RO; bitpos: [6]; default: 0;
 * This is the status bit for reg_gpio6_int_raw when reg_gpio6_int_ena is
 * set to
 * 1.
 */
#define DEDIC_GPIO6_INT_ST    (BIT(6))
#define DEDIC_GPIO6_INT_ST_M  (DEDIC_GPIO6_INT_ST_V << DEDIC_GPIO6_INT_ST_S)
#define DEDIC_GPIO6_INT_ST_V  0x00000001
#define DEDIC_GPIO6_INT_ST_S  6
/* DEDIC_GPIO7_INT_ST : RO; bitpos: [7]; default: 0;
 * This is the status bit for reg_gpio7_int_raw when reg_gpio7_int_ena is
 * set to
 * 1.
 */
#define DEDIC_GPIO7_INT_ST    (BIT(7))
#define DEDIC_GPIO7_INT_ST_M  (DEDIC_GPIO7_INT_ST_V << DEDIC_GPIO7_INT_ST_S)
#define DEDIC_GPIO7_INT_ST_V  0x00000001
#define DEDIC_GPIO7_INT_ST_S  7

/** DEDIC_GPIO_INTR_CLR_REG register
 *  Interrupt clear
 *  bits
 */
#define DEDIC_GPIO_INTR_CLR_REG (DR_REG_DEDICATED_GPIO_BASE + 0x2c)
/* DEDIC_GPIO0_INT_CLR : WO; bitpos: [0]; default: 0;
 * Set this bit to clear the reg_gpio0_int_raw
 * interrupt.
 */
#define DEDIC_GPIO0_INT_CLR    (BIT(0))
#define DEDIC_GPIO0_INT_CLR_M  (DEDIC_GPIO0_INT_CLR_V << DEDIC_GPIO0_INT_CLR_S)
#define DEDIC_GPIO0_INT_CLR_V  0x00000001
#define DEDIC_GPIO0_INT_CLR_S  0
/* DEDIC_GPIO1_INT_CLR : WO; bitpos: [1]; default: 0;
 * Set this bit to clear the reg_gpio1_int_raw
 * interrupt.
 */
#define DEDIC_GPIO1_INT_CLR    (BIT(1))
#define DEDIC_GPIO1_INT_CLR_M  (DEDIC_GPIO1_INT_CLR_V << DEDIC_GPIO1_INT_CLR_S)
#define DEDIC_GPIO1_INT_CLR_V  0x00000001
#define DEDIC_GPIO1_INT_CLR_S  1
/* DEDIC_GPIO2_INT_CLR : WO; bitpos: [2]; default: 0;
 * Set this bit to clear the reg_gpio2_int_raw
 * interrupt.
 */
#define DEDIC_GPIO2_INT_CLR    (BIT(2))
#define DEDIC_GPIO2_INT_CLR_M  (DEDIC_GPIO2_INT_CLR_V << DEDIC_GPIO2_INT_CLR_S)
#define DEDIC_GPIO2_INT_CLR_V  0x00000001
#define DEDIC_GPIO2_INT_CLR_S  2
/* DEDIC_GPIO3_INT_CLR : WO; bitpos: [3]; default: 0;
 * Set this bit to clear the reg_gpio3_int_raw
 * interrupt.
 */
#define DEDIC_GPIO3_INT_CLR    (BIT(3))
#define DEDIC_GPIO3_INT_CLR_M  (DEDIC_GPIO3_INT_CLR_V << DEDIC_GPIO3_INT_CLR_S)
#define DEDIC_GPIO3_INT_CLR_V  0x00000001
#define DEDIC_GPIO3_INT_CLR_S  3
/* DEDIC_GPIO4_INT_CLR : WO; bitpos: [4]; default: 0;
 * Set this bit to clear the reg_gpio4_int_raw
 * interrupt.
 */
#define DEDIC_GPIO4_INT_CLR    (BIT(4))
#define DEDIC_GPIO4_INT_CLR_M  (DEDIC_GPIO4_INT_CLR_V << DEDIC_GPIO4_INT_CLR_S)
#define DEDIC_GPIO4_INT_CLR_V  0x00000001
#define DEDIC_GPIO4_INT_CLR_S  4
/* DEDIC_GPIO5_INT_CLR : WO; bitpos: [5]; default: 0;
 * Set this bit to clear the reg_gpio5_int_raw
 * interrupt.
 */
#define DEDIC_GPIO5_INT_CLR    (BIT(5))
#define DEDIC_GPIO5_INT_CLR_M  (DEDIC_GPIO5_INT_CLR_V << DEDIC_GPIO5_INT_CLR_S)
#define DEDIC_GPIO5_INT_CLR_V  0x00000001
#define DEDIC_GPIO5_INT_CLR_S  5
/* DEDIC_GPIO6_INT_CLR : WO; bitpos: [6]; default: 0;
 * Set this bit to clear the reg_gpio6_int_raw
 * interrupt.
 */
#define DEDIC_GPIO6_INT_CLR    (BIT(6))
#define DEDIC_GPIO6_INT_CLR_M  (DEDIC_GPIO6_INT_CLR_V << DEDIC_GPIO6_INT_CLR_S)
#define DEDIC_GPIO6_INT_CLR_V  0x00000001
#define DEDIC_GPIO6_INT_CLR_S  6
/* DEDIC_GPIO7_INT_CLR : WO; bitpos: [7]; default: 0;
 * Set this bit to clear the reg_gpio7_int_raw
 * interrupt.
 */
#define DEDIC_GPIO7_INT_CLR    (BIT(7))
#define DEDIC_GPIO7_INT_CLR_M  (DEDIC_GPIO7_INT_CLR_V << DEDIC_GPIO7_INT_CLR_S)
#define DEDIC_GPIO7_INT_CLR_V  0x00000001
#define DEDIC_GPIO7_INT_CLR_S  7

#ifdef __cplusplus
}
#endif
