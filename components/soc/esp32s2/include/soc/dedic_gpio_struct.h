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

#ifdef __cplusplus
extern "C" {
#endif

/** Configuration registers */
/** Type of gpio_out_drt register
 *  Dedicated GPIO Directive Output register
 */
typedef union {
    struct {
        /** gpio_out_drt_vlaue : WO; bitpos: [7:0]; default: 0;
         *  This register is used to configure directive output value of 8-channel dedicated
         *  gpio.
         */
        uint32_t gpio_out_drt_vlaue: 8;
        uint32_t reserved8: 24;
    };
    uint32_t val;
} dedic_gpio_out_drt_reg_t;

/** Type of gpio_out_msk register
 *  Dedicated GPIO Mask Output register
 */
typedef union {
    struct {
        /** gpio_out_value : WO; bitpos: [7:0]; default: 0;
         *  This register is used to configure masked output value of 8-channel dedicated gpio.
         */
        uint32_t gpio_out_value: 8;
        /** gpio_out_msk : WO; bitpos: [15:8]; default: 0;
         *  This register is used to configure channels which would be masked. 1: corresponding
         *  channel's output would be masked.
         */
        uint32_t gpio_out_msk: 8;
        uint32_t reserved16: 16;
    };
    uint32_t val;
} dedic_gpio_out_msk_reg_t;

/** Type of gpio_out_idv register
 *  Dedicated GPIO Individual Output register
 */
typedef union {
    struct {
        /** gpio_out_idv_ch0 : WO; bitpos: [1:0]; default: 0;
         *  Configure channel 0 output value. 0: hold output value; 1: set output value; 2:
         *  clear output value; 3: inverse output value.
         */
        uint32_t gpio_out_idv_ch0: 2;
        /** gpio_out_idv_ch1 : WO; bitpos: [3:2]; default: 0;
         *  Configure channel 1 output value. 0: hold output value; 1: set output value; 2:
         *  clear output value; 3: inverse output value.
         */
        uint32_t gpio_out_idv_ch1: 2;
        /** gpio_out_idv_ch2 : WO; bitpos: [5:4]; default: 0;
         *  Configure channel 2 output value. 0: hold output value; 1: set output value; 2:
         *  clear output value; 3: inverse output value.
         */
        uint32_t gpio_out_idv_ch2: 2;
        /** gpio_out_idv_ch3 : WO; bitpos: [7:6]; default: 0;
         *  Configure channel 3 output value. 0: hold output value; 1: set output value; 2:
         *  clear output value; 3: inverse output value.
         */
        uint32_t gpio_out_idv_ch3: 2;
        /** gpio_out_idv_ch4 : WO; bitpos: [9:8]; default: 0;
         *  Configure channel 4 output value. 0: hold output value; 1: set output value; 2:
         *  clear output value; 3: inverse output value.
         */
        uint32_t gpio_out_idv_ch4: 2;
        /** gpio_out_idv_ch5 : WO; bitpos: [11:10]; default: 0;
         *  Configure channel 5 output value. 0: hold output value; 1: set output value; 2:
         *  clear output value; 3: inverse output value.
         */
        uint32_t gpio_out_idv_ch5: 2;
        /** gpio_out_idv_ch6 : WO; bitpos: [13:12]; default: 0;
         *  Configure channel 6 output value. 0: hold output value; 1: set output value; 2:
         *  clear output value; 3: inverse output value.
         */
        uint32_t gpio_out_idv_ch6: 2;
        /** gpio_out_idv_ch7 : WO; bitpos: [15:14]; default: 0;
         *  Configure channel 7 output value. 0: hold output value; 1: set output value; 2:
         *  clear output value; 3: inverse output value.
         */
        uint32_t gpio_out_idv_ch7: 2;
        uint32_t reserved16: 16;
    };
    uint32_t val;
} dedic_gpio_out_idv_reg_t;

/** Type of gpio_out_cpu register
 *  Dedicated GPIO Output Mode Select register
 */
typedef union {
    struct {
        /** gpio_out_cpu_sel0 : R/W; bitpos: [0]; default: 0;
         *  Select gpio out value configured by registers or CPU instructors for channel 0. 1:
         *  select CPU instructors.
         */
        uint32_t gpio_out_cpu_sel0: 1;
        /** gpio_out_cpu_sel1 : R/W; bitpos: [1]; default: 0;
         *  Select gpio out value configured by registers or CPU instructors for channel 1. 1:
         *  select CPU instructors.
         */
        uint32_t gpio_out_cpu_sel1: 1;
        /** gpio_out_cpu_sel2 : R/W; bitpos: [2]; default: 0;
         *  Select gpio out value configured by registers or CPU instructors for channel 2. 1:
         *  select CPU instructors.
         */
        uint32_t gpio_out_cpu_sel2: 1;
        /** gpio_out_cpu_sel3 : R/W; bitpos: [3]; default: 0;
         *  Select gpio out value configured by registers or CPU instructors for channel 3. 1:
         *  select CPU instructors.
         */
        uint32_t gpio_out_cpu_sel3: 1;
        /** gpio_out_cpu_sel4 : R/W; bitpos: [4]; default: 0;
         *  Select gpio out value configured by registers or CPU instructors for channel 4. 1:
         *  select CPU instructors.
         */
        uint32_t gpio_out_cpu_sel4: 1;
        /** gpio_out_cpu_sel5 : R/W; bitpos: [5]; default: 0;
         *  Select gpio out value configured by registers or CPU instructors for channel 5. 1:
         *  select CPU instructors.
         */
        uint32_t gpio_out_cpu_sel5: 1;
        /** gpio_out_cpu_sel6 : R/W; bitpos: [6]; default: 0;
         *  Select gpio out value configured by registers or CPU instructors for channel 6. 1:
         *  select CPU instructors.
         */
        uint32_t gpio_out_cpu_sel6: 1;
        /** gpio_out_cpu_sel7 : R/W; bitpos: [7]; default: 0;
         *  Select gpio out value configured by registers or CPU instructors for channel 7. 1:
         *  select CPU instructors.
         */
        uint32_t gpio_out_cpu_sel7: 1;
        uint32_t reserved8: 24;
    };
    uint32_t val;
} dedic_gpio_out_cpu_reg_t;

/** Type of gpio_in_dly register
 *  Dedicated GPIO Input Delay Configuration register
 */
typedef union {
    struct {
        /** gpio_in_dly_ch0 : R/W; bitpos: [1:0]; default: 0;
         *  Configure gpio 0 input delay. 0: No delay; 1: one clock delay; 2: two clock delay;
         *  3: three clock delay.
         */
        uint32_t gpio_in_dly_ch0: 2;
        /** gpio_in_dly_ch1 : R/W; bitpos: [3:2]; default: 0;
         *  Configure gpio 1 input delay. 0: No delay; 1: one clock delay; 2: two clock delay;
         *  3: three clock delay.
         */
        uint32_t gpio_in_dly_ch1: 2;
        /** gpio_in_dly_ch2 : R/W; bitpos: [5:4]; default: 0;
         *  Configure gpio 2 input delay. 0: No delay; 1: one clock delay; 2: two clock delay;
         *  3: three clock delay.
         */
        uint32_t gpio_in_dly_ch2: 2;
        /** gpio_in_dly_ch3 : R/W; bitpos: [7:6]; default: 0;
         *  Configure gpio 3 input delay. 0: No delay; 1: one clock delay; 2: two clock delay;
         *  3: three clock delay.
         */
        uint32_t gpio_in_dly_ch3: 2;
        /** gpio_in_dly_ch4 : R/W; bitpos: [9:8]; default: 0;
         *  Configure gpio 4 input delay. 0: No delay; 1: one clock delay; 2: two clock delay;
         *  3: three clock delay.
         */
        uint32_t gpio_in_dly_ch4: 2;
        /** gpio_in_dly_ch5 : R/W; bitpos: [11:10]; default: 0;
         *  Configure gpio 5 input delay. 0: No delay; 1: one clock delay; 2: two clock delay;
         *  3: three clock delay.
         */
        uint32_t gpio_in_dly_ch5: 2;
        /** gpio_in_dly_ch6 : R/W; bitpos: [13:12]; default: 0;
         *  Configure gpio 6 input delay. 0: No delay; 1: one clock delay; 2: two clock delay;
         *  3: three clock delay.
         */
        uint32_t gpio_in_dly_ch6: 2;
        /** gpio_in_dly_ch7 : R/W; bitpos: [15:14]; default: 0;
         *  Configure gpio 7 input delay. 0: No delay; 1: one clock delay; 2: two clock delay;
         *  3: three clock delay.
         */
        uint32_t gpio_in_dly_ch7: 2;
        uint32_t reserved16: 16;
    };
    uint32_t val;
} dedic_gpio_in_dly_reg_t;

/** Type of gpio_intr_rcgn register
 *  Dedicated GPIO Interrupts Generate Mode register
 */
typedef union {
    struct {
        /** gpio_intr_mode_ch0 : R/W; bitpos: [2:0]; default: 0;
         *  Configure channel 0 interrupt generate mode.
         *  0/1: do not generate interrupt;
         *  2: low level trigger;
         *  3: high level trigger;
         *  4: falling edge trigger;
         *  5: raising edge trigger;
         *  6/7: falling and raising edge trigger.
         */
        uint32_t gpio_intr_mode_ch0: 3;
        /** gpio_intr_mode_ch1 : R/W; bitpos: [5:3]; default: 0;
         *  Configure channel 1 interrupt generate mode.
         *  0/1: do not generate interrupt;
         *  2: low level trigger;
         *  3: high level trigger;
         *  4: falling edge trigger;
         *  5: raising edge trigger;
         *  6/7: falling and raising edge trigger.
         */
        uint32_t gpio_intr_mode_ch1: 3;
        /** gpio_intr_mode_ch2 : R/W; bitpos: [8:6]; default: 0;
         *  Configure channel 2 interrupt generate mode.
         *  0/1: do not generate interrupt;
         *  2: low level trigger;
         *  3: high level trigger;
         *  4: falling edge trigger;
         *  5: raising edge trigger;
         *  6/7: falling and raising edge trigger.
         */
        uint32_t gpio_intr_mode_ch2: 3;
        /** gpio_intr_mode_ch3 : R/W; bitpos: [11:9]; default: 0;
         *  Configure channel 3 interrupt generate mode.
         *  0/1: do not generate interrupt;
         *  2: low level trigger;
         *  3: high level trigger;
         *  4: falling edge trigger;
         *  5: raising edge trigger;
         *  6/7: falling and raising edge trigger.
         */
        uint32_t gpio_intr_mode_ch3: 3;
        /** gpio_intr_mode_ch4 : R/W; bitpos: [14:12]; default: 0;
         *  Configure channel 4 interrupt generate mode.
         *  0/1: do not generate interrupt;
         *  2: low level trigger;
         *  3: high level trigger;
         *  4: falling edge trigger;
         *  5: raising edge trigger;
         *  6/7: falling and raising edge trigger.
         */
        uint32_t gpio_intr_mode_ch4: 3;
        /** gpio_intr_mode_ch5 : R/W; bitpos: [17:15]; default: 0;
         *  Configure channel 5 interrupt generate mode.
         *  0/1: do not generate interrupt;
         *  2: low level trigger;
         *  3: high level trigger;
         *  4: falling edge trigger;
         *  5: raising edge trigger;
         *  6/7: falling and raising edge trigger.
         */
        uint32_t gpio_intr_mode_ch5: 3;
        /** gpio_intr_mode_ch6 : R/W; bitpos: [20:18]; default: 0;
         *  Configure channel 6 interrupt generate mode.
         *  0/1: do not generate interrupt;
         *  2: low level trigger;
         *  3: high level trigger;
         *  4: falling edge trigger;
         *  5: raising edge trigger;
         *  6/7: falling and raising edge trigger.
         */
        uint32_t gpio_intr_mode_ch6: 3;
        /** gpio_intr_mode_ch7 : R/W; bitpos: [23:21]; default: 0;
         *  Configure channel 7 interrupt generate mode.
         *  0/1: do not generate interrupt;
         *  2: low level trigger;
         *  3: high level trigger;
         *  4: falling edge trigger;
         *  5: raising edge trigger;
         *  6/7: falling and raising edge trigger.
         */
        uint32_t gpio_intr_mode_ch7: 3;
        uint32_t reserved24: 8;
    };
    uint32_t val;
} dedic_gpio_intr_rcgn_reg_t;


/** Status registers */
/** Type of gpio_out_scan register
 *  Dedicated GPIO Output Status register
 */
typedef union {
    struct {
        /** gpio_out_status : RO; bitpos: [7:0]; default: 0;
         *  gpio out value configured by DEDIC_GPIO_OUT_DRT_REG, DEDIC_GPIO_OUT_MSK_REG,
         *  DEDIC_GPIO_OUT_IDV_REG.
         */
        uint32_t gpio_out_status: 8;
        uint32_t reserved8: 24;
    };
    uint32_t val;
} dedic_gpio_out_scan_reg_t;

/** Type of gpio_in_scan register
 *  Dedicated GPIO Input Status register
 */
typedef union {
    struct {
        /** gpio_in_status : RO; bitpos: [7:0]; default: 0;
         *  gpio in value after configured by DEDIC_GPIO_IN_DLY_REG.
         */
        uint32_t gpio_in_status: 8;
        uint32_t reserved8: 24;
    };
    uint32_t val;
} dedic_gpio_in_scan_reg_t;


/** Interrupt registers */
/** Type of gpio_intr_raw register
 *  Raw interrupt status
 */
typedef union {
    struct {
        /** gpio0_int_raw : RO; bitpos: [0]; default: 0;
         *  This interrupt raw bit turns to high level when dedicated GPIO 0 has level/edge
         *  change configured by DEDIC_GPIO_INTR_RCGN_REG.
         */
        uint32_t gpio0_int_raw: 1;
        /** gpio1_int_raw : RO; bitpos: [1]; default: 0;
         *  This interrupt raw bit turns to high level when dedicated GPIO 1 has level/edge
         *  change configured by DEDIC_GPIO_INTR_RCGN_REG.
         */
        uint32_t gpio1_int_raw: 1;
        /** gpio2_int_raw : RO; bitpos: [2]; default: 0;
         *  This interrupt raw bit turns to high level when dedicated GPIO 2 has level/edge
         *  change configured by DEDIC_GPIO_INTR_RCGN_REG.
         */
        uint32_t gpio2_int_raw: 1;
        /** gpio3_int_raw : RO; bitpos: [3]; default: 0;
         *  This interrupt raw bit turns to high level when dedicated GPIO 3 has level/edge
         *  change configured by DEDIC_GPIO_INTR_RCGN_REG.
         */
        uint32_t gpio3_int_raw: 1;
        /** gpio4_int_raw : RO; bitpos: [4]; default: 0;
         *  This interrupt raw bit turns to high level when dedicated GPIO 4 has level/edge
         *  change configured by DEDIC_GPIO_INTR_RCGN_REG.
         */
        uint32_t gpio4_int_raw: 1;
        /** gpio5_int_raw : RO; bitpos: [5]; default: 0;
         *  This interrupt raw bit turns to high level when dedicated GPIO 5 has level/edge
         *  change configured by DEDIC_GPIO_INTR_RCGN_REG.
         */
        uint32_t gpio5_int_raw: 1;
        /** gpio6_int_raw : RO; bitpos: [6]; default: 0;
         *  This interrupt raw bit turns to high level when dedicated GPIO 6 has level/edge
         *  change configured by DEDIC_GPIO_INTR_RCGN_REG.
         */
        uint32_t gpio6_int_raw: 1;
        /** gpio7_int_raw : RO; bitpos: [7]; default: 0;
         *  This interrupt raw bit turns to high level when dedicated GPIO 7 has level/edge
         *  change configured by DEDIC_GPIO_INTR_RCGN_REG.
         */
        uint32_t gpio7_int_raw: 1;
        uint32_t reserved8: 24;
    };
    uint32_t val;
} dedic_gpio_intr_raw_reg_t;

/** Type of gpio_intr_rls register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** gpio0_int_ena : R/W; bitpos: [0]; default: 0;
         *  This enable bit for reg_gpio0_int_st register.
         */
        uint32_t gpio0_int_ena: 1;
        /** gpio1_int_ena : R/W; bitpos: [1]; default: 0;
         *  This enable bit for reg_gpio1_int_st register.
         */
        uint32_t gpio1_int_ena: 1;
        /** gpio2_int_ena : R/W; bitpos: [2]; default: 0;
         *  This enable bit for reg_gpio2_int_st register.
         */
        uint32_t gpio2_int_ena: 1;
        /** gpio3_int_ena : R/W; bitpos: [3]; default: 0;
         *  This enable bit for reg_gpio3_int_st register.
         */
        uint32_t gpio3_int_ena: 1;
        /** gpio4_int_ena : R/W; bitpos: [4]; default: 0;
         *  This enable bit for reg_gpio4_int_st register.
         */
        uint32_t gpio4_int_ena: 1;
        /** gpio5_int_ena : R/W; bitpos: [5]; default: 0;
         *  This enable bit for reg_gpio5_int_st register.
         */
        uint32_t gpio5_int_ena: 1;
        /** gpio6_int_ena : R/W; bitpos: [6]; default: 0;
         *  This enable bit for reg_gpio6_int_st register.
         */
        uint32_t gpio6_int_ena: 1;
        /** gpio7_int_ena : R/W; bitpos: [7]; default: 0;
         *  This enable bit for reg_gpio7_int_st register.
         */
        uint32_t gpio7_int_ena: 1;
        uint32_t reserved8: 24;
    };
    uint32_t val;
} dedic_gpio_intr_rls_reg_t;

/** Type of gpio_intr_st register
 *  Masked interrupt status
 */
typedef union {
    struct {
        /** gpio0_int_st : RO; bitpos: [0]; default: 0;
         *  This is the status bit for reg_gpio0_int_raw when reg_gpio0_int_ena is set to 1.
         */
        uint32_t gpio0_int_st: 1;
        /** gpio1_int_st : RO; bitpos: [1]; default: 0;
         *  This is the status bit for reg_gpio1_int_raw when reg_gpio1_int_ena is set to 1.
         */
        uint32_t gpio1_int_st: 1;
        /** gpio2_int_st : RO; bitpos: [2]; default: 0;
         *  This is the status bit for reg_gpio2_int_raw when reg_gpio2_int_ena is set to 1.
         */
        uint32_t gpio2_int_st: 1;
        /** gpio3_int_st : RO; bitpos: [3]; default: 0;
         *  This is the status bit for reg_gpio3_int_raw when reg_gpio3_int_ena is set to 1.
         */
        uint32_t gpio3_int_st: 1;
        /** gpio4_int_st : RO; bitpos: [4]; default: 0;
         *  This is the status bit for reg_gpio4_int_raw when reg_gpio4_int_ena is set to 1.
         */
        uint32_t gpio4_int_st: 1;
        /** gpio5_int_st : RO; bitpos: [5]; default: 0;
         *  This is the status bit for reg_gpio5_int_raw when reg_gpio5_int_ena is set to 1.
         */
        uint32_t gpio5_int_st: 1;
        /** gpio6_int_st : RO; bitpos: [6]; default: 0;
         *  This is the status bit for reg_gpio6_int_raw when reg_gpio6_int_ena is set to 1.
         */
        uint32_t gpio6_int_st: 1;
        /** gpio7_int_st : RO; bitpos: [7]; default: 0;
         *  This is the status bit for reg_gpio7_int_raw when reg_gpio7_int_ena is set to 1.
         */
        uint32_t gpio7_int_st: 1;
        uint32_t reserved8: 24;
    };
    uint32_t val;
} dedic_gpio_intr_st_reg_t;

/** Type of gpio_intr_clr register
 *  Interrupt clear bits
 */
typedef union {
    struct {
        /** gpio0_int_clr : WO; bitpos: [0]; default: 0;
         *  Set this bit to clear the reg_gpio0_int_raw interrupt.
         */
        uint32_t gpio0_int_clr: 1;
        /** gpio1_int_clr : WO; bitpos: [1]; default: 0;
         *  Set this bit to clear the reg_gpio1_int_raw interrupt.
         */
        uint32_t gpio1_int_clr: 1;
        /** gpio2_int_clr : WO; bitpos: [2]; default: 0;
         *  Set this bit to clear the reg_gpio2_int_raw interrupt.
         */
        uint32_t gpio2_int_clr: 1;
        /** gpio3_int_clr : WO; bitpos: [3]; default: 0;
         *  Set this bit to clear the reg_gpio3_int_raw interrupt.
         */
        uint32_t gpio3_int_clr: 1;
        /** gpio4_int_clr : WO; bitpos: [4]; default: 0;
         *  Set this bit to clear the reg_gpio4_int_raw interrupt.
         */
        uint32_t gpio4_int_clr: 1;
        /** gpio5_int_clr : WO; bitpos: [5]; default: 0;
         *  Set this bit to clear the reg_gpio5_int_raw interrupt.
         */
        uint32_t gpio5_int_clr: 1;
        /** gpio6_int_clr : WO; bitpos: [6]; default: 0;
         *  Set this bit to clear the reg_gpio6_int_raw interrupt.
         */
        uint32_t gpio6_int_clr: 1;
        /** gpio7_int_clr : WO; bitpos: [7]; default: 0;
         *  Set this bit to clear the reg_gpio7_int_raw interrupt.
         */
        uint32_t gpio7_int_clr: 1;
        uint32_t reserved8: 24;
    };
    uint32_t val;
} dedic_gpio_intr_clr_reg_t;

typedef struct {
    volatile dedic_gpio_out_drt_reg_t gpio_out_drt;
    volatile dedic_gpio_out_msk_reg_t gpio_out_msk;
    volatile dedic_gpio_out_idv_reg_t gpio_out_idv;
    volatile dedic_gpio_out_scan_reg_t gpio_out_scan;
    volatile dedic_gpio_out_cpu_reg_t gpio_out_cpu;
    volatile dedic_gpio_in_dly_reg_t gpio_in_dly;
    volatile dedic_gpio_in_scan_reg_t gpio_in_scan;
    volatile dedic_gpio_intr_rcgn_reg_t gpio_intr_rcgn;
    volatile dedic_gpio_intr_raw_reg_t gpio_intr_raw;
    volatile dedic_gpio_intr_rls_reg_t gpio_intr_rls;
    volatile dedic_gpio_intr_st_reg_t gpio_intr_st;
    volatile dedic_gpio_intr_clr_reg_t gpio_intr_clr;
} dedic_dev_t;

#ifndef __cplusplus
_Static_assert(sizeof(dedic_dev_t) == 0x30, "dedic_dev_t should occupy 0x30 bytes in memory");
#endif

extern dedic_dev_t DEDIC_GPIO;

#ifdef __cplusplus
}
#endif
