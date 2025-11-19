/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Registers */
/** Type of strap register
 *  Strapping pin register
 */
typedef union {
    struct {
        /** strapping : RO; bitpos: [15:0]; default: 0;
         *  Represents the values of GPIO strapping pins. (need update the description, for
         *  example)
         *
         *  - bit0: invalid
         *  - bit1: MTMS
         *  - bit2: MTDI
         *  - bit3: GPIO27
         *  - bit4: GPIO28
         *  - bit5: GPIO7
         *  - bit6 ~ bit15: invalid
         */
        uint32_t strapping:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} gpio_strap_reg_t;

/** Type of out register
 *  GPIO output register
 */
typedef union {
    struct {
        /** out_data_orig : R/W/SC/WTC; bitpos: [31:0]; default: 0;
         *  Configures the output value of GPIO0 ~ 31 output in simple GPIO output mode.
         *  0: Low level
         *  1: High level
         *  The value of bit0 ~ bit31 correspond to the output value of GPIO0 ~ GPIO31
         *  respectively. Bitxx ~ bitxx is invalid.
         */
        uint32_t out_data_orig:32;
    };
    uint32_t val;
} gpio_out_reg_t;

/** Type of out_w1ts register
 *  GPIO output set register
 */
typedef union {
    struct {
        /** out_w1ts : WT; bitpos: [31:0]; default: 0;
         *  Configures whether or not to set the output register GPIO_OUT_REG of GPIO0 ~ GPIO31.
         *  0: Not set
         *  1: The corresponding bit in GPIO_OUT_REG will be set to 1
         *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to set GPIO_OUT_REG.
         */
        uint32_t out_w1ts:32;
    };
    uint32_t val;
} gpio_out_w1ts_reg_t;

/** Type of out_w1tc register
 *  GPIO output clear register
 */
typedef union {
    struct {
        /** out_w1tc : WT; bitpos: [31:0]; default: 0;
         *  Configures whether or not to clear the output register GPIO_OUT_REG of GPIO0 ~
         *  GPIO31 output.
         *  0: Not clear
         *  1: The corresponding bit in GPIO_OUT_REG will be cleared.
         *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to clear GPIO_OUT_REG.
         */
        uint32_t out_w1tc:32;
    };
    uint32_t val;
} gpio_out_w1tc_reg_t;

/** Type of out1 register
 *  GPIO output register
 */
typedef union {
    struct {
        /** out1_data_orig : R/W/SC/WTC; bitpos: [31:0]; default: 0;
         *  Configures the output value of GPIO32 ~ 63 output in simple GPIO output mode.
         *  0: Low level
         *  1: High level
         *  The value of bit32 ~ bit63 correspond to the output value of GPIO32 ~ GPIO63
         *  respectively. Bitxx ~ bitxx is invalid.
         */
        uint32_t out1_data_orig:32;
    };
    uint32_t val;
} gpio_out1_reg_t;

/** Type of out1_w1ts register
 *  GPIO output set register
 */
typedef union {
    struct {
        /** out1_w1ts : WT; bitpos: [31:0]; default: 0;
         *  Configures whether or not to set the output register GPIO_OUT1_REG of GPIO32 ~
         *  GPIO63.
         *  0: Not set
         *  1: The corresponding bit in GPIO_OUT1_REG will be set to 1
         *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to set GPIO_OUT1_REG.
         */
        uint32_t out1_w1ts:32;
    };
    uint32_t val;
} gpio_out1_w1ts_reg_t;

/** Type of out1_w1tc register
 *  GPIO output clear register
 */
typedef union {
    struct {
        /** out1_w1tc : WT; bitpos: [31:0]; default: 0;
         *  Configures whether or not to clear the output register GPIO_OUT1_REG of GPIO32 ~
         *  GPIO63 output.
         *  0: Not clear
         *  1: The corresponding bit in GPIO_OUT1_REG will be cleared.
         *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to clear GPIO_OUT1_REG.
         */
        uint32_t out1_w1tc:32;
    };
    uint32_t val;
} gpio_out1_w1tc_reg_t;

/** Type of out2 register
 *  GPIO output register
 */
typedef union {
    struct {
        /** out2_data_orig : R/W/SC/WTC; bitpos: [2:0]; default: 0;
         *  Configures the output value of GPIO64 ~ 66 output in simple GPIO output mode.
         *  0: Low level
         *  1: High level
         *  The value of bit64 ~ bit66 correspond to the output value of GPIO64 ~ GPIO66
         *  respectively. Bitxx ~ bitxx is invalid.
         */
        uint32_t out2_data_orig:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_out2_reg_t;

/** Type of out2_w1ts register
 *  GPIO output set register
 */
typedef union {
    struct {
        /** out2_w1ts : WT; bitpos: [2:0]; default: 0;
         *  Configures whether or not to set the output register GPIO_OUT2_REG of GPIO64 ~
         *  GPIO66.
         *  0: Not set
         *  1: The corresponding bit in GPIO_OUT2_REG will be set to 1
         *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to set GPIO_OUT2_REG.
         */
        uint32_t out2_w1ts:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_out2_w1ts_reg_t;

/** Type of out2_w1tc register
 *  GPIO output clear register
 */
typedef union {
    struct {
        /** out2_w1tc : WT; bitpos: [2:0]; default: 0;
         *  Configures whether or not to clear the output register GPIO_OUT2_REG of GPIO64 ~
         *  GPIO66 output.
         *  0: Not clear
         *  1: The corresponding bit in GPIO_OUT2_REG will be cleared.
         *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to clear GPIO_OUT2_REG.
         */
        uint32_t out2_w1tc:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_out2_w1tc_reg_t;

/** Type of enable register
 *  GPIO output enable register
 */
typedef union {
    struct {
        /** enable_data : R/W/WTC; bitpos: [31:0]; default: 0;
         *  Configures whether or not to enable the output of GPIO0 ~ GPIO31.
         *  0: Not enable
         *  1: Enable
         *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
         */
        uint32_t enable_data:32;
    };
    uint32_t val;
} gpio_enable_reg_t;

/** Type of enable_w1ts register
 *  GPIO output enable set register
 */
typedef union {
    struct {
        /** enable_w1ts : WT; bitpos: [31:0]; default: 0;
         *  Configures whether or not to set the output enable register GPIO_ENABLE_REG of
         *  GPIO0 ~ GPIO31.
         *  0: Not set
         *  1: The corresponding bit in GPIO_ENABLE_REG will be set to 1
         *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to set GPIO_ENABLE_REG.
         */
        uint32_t enable_w1ts:32;
    };
    uint32_t val;
} gpio_enable_w1ts_reg_t;

/** Type of enable_w1tc register
 *  GPIO output enable clear register
 */
typedef union {
    struct {
        /** enable_w1tc : WT; bitpos: [31:0]; default: 0;
         *  Configures whether or not to clear the output enable register GPIO_ENABLE_REG of
         *  GPIO0 ~ GPIO31.
         *  0: Not clear
         *  1: The corresponding bit in GPIO_ENABLE_REG will be cleared
         *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to clear GPIO_ENABLE_REG.
         */
        uint32_t enable_w1tc:32;
    };
    uint32_t val;
} gpio_enable_w1tc_reg_t;

/** Type of enable1 register
 *  GPIO output enable register
 */
typedef union {
    struct {
        /** enable1_data : R/W/WTC; bitpos: [31:0]; default: 0;
         *  Configures whether or not to enable the output of GPIO32 ~ GPIO63.
         *  0: Not enable
         *  1: Enable
         *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
         */
        uint32_t enable1_data:32;
    };
    uint32_t val;
} gpio_enable1_reg_t;

/** Type of enable1_w1ts register
 *  GPIO output enable set register
 */
typedef union {
    struct {
        /** enable1_w1ts : WT; bitpos: [31:0]; default: 0;
         *  Configures whether or not to set the output enable register GPIO_ENABLE1_REG of
         *  GPIO32 ~ GPIO63.
         *  0: Not set
         *  1: The corresponding bit in GPIO_ENABLE1_REG will be set to 1
         *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to set GPIO_ENABLE1_REG.
         */
        uint32_t enable1_w1ts:32;
    };
    uint32_t val;
} gpio_enable1_w1ts_reg_t;

/** Type of enable1_w1tc register
 *  GPIO output enable clear register
 */
typedef union {
    struct {
        /** enable1_w1tc : WT; bitpos: [31:0]; default: 0;
         *  Configures whether or not to clear the output enable register GPIO_ENABLE1_REG of
         *  GPIO32 ~ GPIO63.
         *  0: Not clear
         *  1: The corresponding bit in GPIO_ENABLE1_REG will be cleared
         *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to clear GPIO_ENABLE1_REG.
         */
        uint32_t enable1_w1tc:32;
    };
    uint32_t val;
} gpio_enable1_w1tc_reg_t;

/** Type of enable2 register
 *  GPIO output enable register
 */
typedef union {
    struct {
        /** enable2_data : R/W/WTC; bitpos: [2:0]; default: 0;
         *  Configures whether or not to enable the output of GPIO64 ~ GPIO66.
         *  0: Not enable
         *  1: Enable
         *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
         */
        uint32_t enable2_data:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_enable2_reg_t;

/** Type of enable2_w1ts register
 *  GPIO output enable set register
 */
typedef union {
    struct {
        /** enable2_w1ts : WT; bitpos: [2:0]; default: 0;
         *  Configures whether or not to set the output enable register GPIO_ENABLE2_REG of
         *  GPIO64 ~ GPIO66.
         *  0: Not set
         *  1: The corresponding bit in GPIO_ENABLE2_REG will be set to 1
         *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to set GPIO_ENABLE2_REG.
         */
        uint32_t enable2_w1ts:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_enable2_w1ts_reg_t;

/** Type of enable2_w1tc register
 *  GPIO output enable clear register
 */
typedef union {
    struct {
        /** enable2_w1tc : WT; bitpos: [2:0]; default: 0;
         *  Configures whether or not to clear the output enable register GPIO_ENABLE2_REG of
         *  GPIO64 ~ GPIO66.
         *  0: Not clear
         *  1: The corresponding bit in GPIO_ENABLE2_REG will be cleared
         *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to clear GPIO_ENABLE2_REG.
         */
        uint32_t enable2_w1tc:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_enable2_w1tc_reg_t;

/** Type of in register
 *  GPIO input register
 */
typedef union {
    struct {
        /** in_data_next : RO; bitpos: [31:0]; default: 0;
         *  Represents the input value of GPIO0 ~ GPIO31. Each bit represents a pin input value:
         *  0: Low level
         *  1: High level
         *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
         */
        uint32_t in_data_next:32;
    };
    uint32_t val;
} gpio_in_reg_t;

/** Type of in1 register
 *  GPIO input register
 */
typedef union {
    struct {
        /** in1_data_next : RO; bitpos: [31:0]; default: 0;
         *  Represents the input value of GPIO32 ~ GPIO63. Each bit represents a pin input
         *  value:
         *  0: Low level
         *  1: High level
         *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
         */
        uint32_t in1_data_next:32;
    };
    uint32_t val;
} gpio_in1_reg_t;

/** Type of in2 register
 *  GPIO input register
 */
typedef union {
    struct {
        /** in2_data_next : RO; bitpos: [2:0]; default: 0;
         *  Represents the input value of GPIO64 ~ GPIO66. Each bit represents a pin input
         *  value:
         *  0: Low level
         *  1: High level
         *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
         */
        uint32_t in2_data_next:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_in2_reg_t;


/** Group: Interrupt Status Registers */
/** Type of status register
 *  GPIO interrupt status register
 */
typedef union {
    struct {
        /** status_interrupt : R/W/WTC; bitpos: [31:0]; default: 0;
         *  The interrupt status of GPIO0 ~ GPIO31, can be configured by the software.
         *
         *  - Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
         *  - Each bit represents the status of its corresponding GPIO:
         *
         *  - 0: Represents the GPIO does not generate the interrupt configured by
         *  GPIO_PIN$n_INT_TYPE, or this bit is configured to 0 by the software.
         *  - 1: Represents the GPIO generates the interrupt configured by GPIO_PIN$n_INT_TYPE,
         *  or this bit is configured to 1 by the software.
         *
         */
        uint32_t status_interrupt:32;
    };
    uint32_t val;
} gpio_status_reg_t;

/** Type of status_w1ts register
 *  GPIO interrupt status set register
 */
typedef union {
    struct {
        /** status_w1ts : WT; bitpos: [31:0]; default: 0;
         *  Configures whether or not to set the interrupt status register
         *  GPIO_STATUS_INTERRUPT of GPIO0 ~ GPIO31.
         *
         *  - Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
         *  - If the value 1 is written to a bit here, the corresponding bit in
         *  GPIO_STATUS_INTERRUPT will be set to 1. \item Recommended operation: use this
         *  register to set GPIO_STATUS_INTERRUPT.
         */
        uint32_t status_w1ts:32;
    };
    uint32_t val;
} gpio_status_w1ts_reg_t;

/** Type of status_w1tc register
 *  GPIO interrupt status clear register
 */
typedef union {
    struct {
        /** status_w1tc : WT; bitpos: [31:0]; default: 0;
         *  Configures whether or not to clear the interrupt status register
         *  GPIO_STATUS_INTERRUPT of GPIO0 ~ GPIO31.
         *
         *  - Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid.
         *  - If the value 1 is written to a bit here, the corresponding bit in
         *  GPIO_STATUS_INTERRUPT will be cleared. \item Recommended operation: use this
         *  register to clear GPIO_STATUS_INTERRUPT.
         */
        uint32_t status_w1tc:32;
    };
    uint32_t val;
} gpio_status_w1tc_reg_t;

/** Type of status1 register
 *  GPIO interrupt status register
 */
typedef union {
    struct {
        /** status1_interrupt : R/W/WTC; bitpos: [31:0]; default: 0;
         *  The interrupt status of GPIO32 ~ GPIO63, can be configured by the software.
         *
         *  - Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
         *  - Each bit represents the status of its corresponding GPIO:
         *
         *  - 0: Represents the GPIO does not generate the interrupt configured by
         *  GPIO_PIN$n_INT_TYPE, or this bit is configured to 0 by the software.
         *  - 1: Represents the GPIO generates the interrupt configured by GPIO_PIN$n_INT_TYPE,
         *  or this bit is configured to 1 by the software.
         *
         */
        uint32_t status1_interrupt:32;
    };
    uint32_t val;
} gpio_status1_reg_t;

/** Type of status1_w1ts register
 *  GPIO interrupt status set register
 */
typedef union {
    struct {
        /** status1_w1ts : WT; bitpos: [31:0]; default: 0;
         *  Configures whether or not to set the interrupt status register
         *  GPIO_STATUS1_INTERRUPT of GPIO32 ~ GPIO63.
         *
         *  - Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
         *  - If the value 1 is written to a bit here, the corresponding bit in
         *  GPIO_STATUS1_INTERRUPT will be set to 1. \item Recommended operation: use this
         *  register to set GPIO_STATUS1_INTERRUPT.
         */
        uint32_t status1_w1ts:32;
    };
    uint32_t val;
} gpio_status1_w1ts_reg_t;

/** Type of status1_w1tc register
 *  GPIO interrupt status clear register
 */
typedef union {
    struct {
        /** status1_w1tc : WT; bitpos: [31:0]; default: 0;
         *  Configures whether or not to clear the interrupt status register
         *  GPIO_STATUS1_INTERRUPT of GPIO32 ~ GPIO63.
         *
         *  - Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid.
         *  - If the value 1 is written to a bit here, the corresponding bit in
         *  GPIO_STATUS1_INTERRUPT will be cleared. \item Recommended operation: use this
         *  register to clear GPIO_STATUS1_INTERRUPT.
         */
        uint32_t status1_w1tc:32;
    };
    uint32_t val;
} gpio_status1_w1tc_reg_t;

/** Type of status2 register
 *  GPIO interrupt status register
 */
typedef union {
    struct {
        /** status2_interrupt : R/W/WTC; bitpos: [2:0]; default: 0;
         *  The interrupt status of GPIO64 ~ GPIO66, can be configured by the software.
         *
         *  - Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
         *  - Each bit represents the status of its corresponding GPIO:
         *
         *  - 0: Represents the GPIO does not generate the interrupt configured by
         *  GPIO_PIN$n_INT_TYPE, or this bit is configured to 0 by the software.
         *  - 1: Represents the GPIO generates the interrupt configured by GPIO_PIN$n_INT_TYPE,
         *  or this bit is configured to 1 by the software.
         *
         */
        uint32_t status2_interrupt:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_status2_reg_t;

/** Type of status2_w1ts register
 *  GPIO interrupt status set register
 */
typedef union {
    struct {
        /** status2_w1ts : WT; bitpos: [2:0]; default: 0;
         *  Configures whether or not to set the interrupt status register
         *  GPIO_STATUS2_INTERRUPT of GPIO64 ~ GPIO66.
         *
         *  - Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
         *  - If the value 1 is written to a bit here, the corresponding bit in
         *  GPIO_STATUS2_INTERRUPT will be set to 1. \item Recommended operation: use this
         *  register to set GPIO_STATUS2_INTERRUPT.
         */
        uint32_t status2_w1ts:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_status2_w1ts_reg_t;

/** Type of status2_w1tc register
 *  GPIO interrupt status clear register
 */
typedef union {
    struct {
        /** status2_w1tc : WT; bitpos: [2:0]; default: 0;
         *  Configures whether or not to clear the interrupt status register
         *  GPIO_STATUS2_INTERRUPT of GPIO64 ~ GPIO66.
         *
         *  - Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid.
         *  - If the value 1 is written to a bit here, the corresponding bit in
         *  GPIO_STATUS2_INTERRUPT will be cleared. \item Recommended operation: use this
         *  register to clear GPIO_STATUS2_INTERRUPT.
         */
        uint32_t status2_w1tc:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_status2_w1tc_reg_t;

/** Type of int_0 register
 *  GPIO_INT_0 interrupt status register
 */
typedef union {
    struct {
        /** int_0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the GPIO_INT_0 interrupt status of GPIO0 ~ GPIO31. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_INT_0 interrupt is not enabled, or the GPIO does not generate
         *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_INT_0 interrupt is enabled.
         *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t int_0:32;
    };
    uint32_t val;
} gpio_int_0_reg_t;

/** Type of int_1 register
 *  GPIO_INT_1 interrupt status register
 */
typedef union {
    struct {
        /** int_1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the GPIO_INT_1 interrupt status of GPIO0 ~ GPIO31. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_INT_1 interrupt is not enabled, or the GPIO does not generate
         *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_INT_1 interrupt is enabled.
         *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t int_1:32;
    };
    uint32_t val;
} gpio_int_1_reg_t;

/** Type of int_2 register
 *  GPIO_INT_2 interrupt status register
 */
typedef union {
    struct {
        /** int_2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the GPIO_INT_2 interrupt status of GPIO0 ~ GPIO31. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_INT_2 interrupt is not enabled, or the GPIO does not generate
         *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_INT_2 interrupt is enabled.
         *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t int_2:32;
    };
    uint32_t val;
} gpio_int_2_reg_t;

/** Type of int_3 register
 *  GPIO_INT_3 interrupt status register
 */
typedef union {
    struct {
        /** int_3 : RO; bitpos: [31:0]; default: 0;
         *  Represents the GPIO_INT_3 interrupt status of GPIO0 ~ GPIO31. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_INT_3 interrupt is not enabled, or the GPIO does not generate
         *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_INT_3 interrupt is enabled.
         *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t int_3:32;
    };
    uint32_t val;
} gpio_int_3_reg_t;

/** Type of int_01 register
 *  GPIO_INT_0 interrupt status register
 */
typedef union {
    struct {
        /** int_01 : RO; bitpos: [31:0]; default: 0;
         *  Represents the GPIO_INT_0 interrupt status of GPIO32 ~ GPIO63. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_INT_0 interrupt is not enabled, or the GPIO does not generate
         *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_INT_0 interrupt is enabled.
         *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS1_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t int_01:32;
    };
    uint32_t val;
} gpio_int_01_reg_t;

/** Type of int_11 register
 *  GPIO_INT_1 interrupt status register
 */
typedef union {
    struct {
        /** int_11 : RO; bitpos: [31:0]; default: 0;
         *  Represents the GPIO_INT_1 interrupt status of GPIO32 ~ GPIO63. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_INT_1 interrupt is not enabled, or the GPIO does not generate
         *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_INT_1 interrupt is enabled.
         *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS1_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t int_11:32;
    };
    uint32_t val;
} gpio_int_11_reg_t;

/** Type of int_21 register
 *  GPIO_INT_2 interrupt status register
 */
typedef union {
    struct {
        /** int_21 : RO; bitpos: [31:0]; default: 0;
         *  Represents the GPIO_INT_2 interrupt status of GPIO32 ~ GPIO63. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_INT_2 interrupt is not enabled, or the GPIO does not generate
         *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_INT_2 interrupt is enabled.
         *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS1_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t int_21:32;
    };
    uint32_t val;
} gpio_int_21_reg_t;

/** Type of int_31 register
 *  GPIO_INT_3 interrupt status register
 */
typedef union {
    struct {
        /** int_31 : RO; bitpos: [31:0]; default: 0;
         *  Represents the GPIO_INT_3 interrupt status of GPIO32 ~ GPIO63. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_INT_3 interrupt is not enabled, or the GPIO does not generate
         *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_INT_3 interrupt is enabled.
         *  Bit32 ~ bit63 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS1_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t int_31:32;
    };
    uint32_t val;
} gpio_int_31_reg_t;

/** Type of int_02 register
 *  GPIO_INT_0 interrupt status register
 */
typedef union {
    struct {
        /** int_02 : RO; bitpos: [2:0]; default: 0;
         *  Represents the GPIO_INT_0 interrupt status of GPIO64 ~ GPIO66. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_INT_0 interrupt is not enabled, or the GPIO does not generate
         *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_INT_0 interrupt is enabled.
         *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS2_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t int_02:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_int_02_reg_t;

/** Type of int_12 register
 *  GPIO_INT_1 interrupt status register
 */
typedef union {
    struct {
        /** int_12 : RO; bitpos: [2:0]; default: 0;
         *  Represents the GPIO_INT_1 interrupt status of GPIO64 ~ GPIO66. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_INT_1 interrupt is not enabled, or the GPIO does not generate
         *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_INT_1 interrupt is enabled.
         *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS2_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t int_12:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_int_12_reg_t;

/** Type of int_22 register
 *  GPIO_INT_2 interrupt status register
 */
typedef union {
    struct {
        /** int_22 : RO; bitpos: [2:0]; default: 0;
         *  Represents the GPIO_INT_2 interrupt status of GPIO64 ~ GPIO66. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_INT_2 interrupt is not enabled, or the GPIO does not generate
         *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_INT_2 interrupt is enabled.
         *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS2_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t int_22:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_int_22_reg_t;

/** Type of int_32 register
 *  GPIO_INT_3 interrupt status register
 */
typedef union {
    struct {
        /** int_32 : RO; bitpos: [2:0]; default: 0;
         *  Represents the GPIO_INT_3 interrupt status of GPIO64 ~ GPIO66. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_INT_3 interrupt is not enabled, or the GPIO does not generate
         *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_INT_3 interrupt is enabled.
         *  Bit64 ~ bit66 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS2_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t int_32:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_int_32_reg_t;

/** Type of status_next register
 *  GPIO interrupt source register
 */
typedef union {
    struct {
        /** status_interrupt_next : RO; bitpos: [31:0]; default: 0;
         *  Represents the interrupt source signal of GPIO0 ~ GPIO31.
         *  Bit0 ~ bit24 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid. Each
         *  bit represents:
         *  0: The GPIO does not generate the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: The GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  The interrupt could be rising edge interrupt, falling edge interrupt, level
         *  sensitive interrupt and any edge interrupt.
         */
        uint32_t status_interrupt_next:32;
    };
    uint32_t val;
} gpio_status_next_reg_t;

/** Type of status_next1 register
 *  GPIO interrupt source register
 */
typedef union {
    struct {
        /** status_interrupt_next1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the interrupt source signal of GPIO32 ~ GPIO63.
         *  Bit0 ~ bit24 are corresponding to GPIO32 ~ GPIO63. Bitxx ~ bitxx is invalid. Each
         *  bit represents:
         *  0: The GPIO does not generate the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: The GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  The interrupt could be rising edge interrupt, falling edge interrupt, level
         *  sensitive interrupt and any edge interrupt.
         */
        uint32_t status_interrupt_next1:32;
    };
    uint32_t val;
} gpio_status_next1_reg_t;

/** Type of status_next2 register
 *  GPIO interrupt source register
 */
typedef union {
    struct {
        /** status_interrupt_next2 : RO; bitpos: [2:0]; default: 0;
         *  Represents the interrupt source signal of GPIO64 ~ GPIO66.
         *  Bit0 ~ bit24 are corresponding to GPIO64 ~ GPIO66. Bitxx ~ bitxx is invalid. Each
         *  bit represents:
         *  0: The GPIO does not generate the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: The GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  The interrupt could be rising edge interrupt, falling edge interrupt, level
         *  sensitive interrupt and any edge interrupt.
         */
        uint32_t status_interrupt_next2:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_status_next2_reg_t;


/** Group: Pin Configuration Registers */
/** Type of pinn register
 *  GPIOn configuration register
 */
typedef union {
    struct {
        /** sync2_bypass : R/W; bitpos: [1:0]; default: 0;
         *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
         *  posedge.
         */
        uint32_t sync2_bypass:2;
        /** pad_driver : R/W; bitpos: [2]; default: 0;
         *  set this bit to select pad driver. 1:open-drain. 0:normal.
         */
        uint32_t pad_driver:1;
        /** sync1_bypass : R/W; bitpos: [4:3]; default: 0;
         *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
         *  posedge.
         */
        uint32_t sync1_bypass:2;
        uint32_t reserved_5:2;
        /** pinn_int_type : R/W; bitpos: [9:7]; default: 0;
         *  Configures GPIO interrupt type.
         *  0: GPIO interrupt disabled
         *  1: Rising edge trigger
         *  2: Falling edge trigger
         *  3: Any edge trigger
         *  4: Low level trigger
         *  5: High level trigger
         */
        uint32_t int_type:3;
        /** wakeup_enable : R/W; bitpos: [10]; default: 0;
         *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
         */
        uint32_t wakeup_enable:1;
        uint32_t reserved_11:2;
        /** pinn_int_ena : R/W; bitpos: [17:13]; default: 0;
         *  Configures whether or not to enable gpio_int_0 or gpio_int_1 or gpio_int_2 or
         *  gpio_int_3(need update in different project).
         *
         *  - bit13: Configures whether or not to enable gpio_int_0(need update in different
         *  project):
         *  0: Disable
         *  1: Enable
         *  - bit14: Configures whether or not to enable gpio_int_1(need update in different
         *  project):
         *  0: Disable
         *  1: Enable
         *  - bit15: Configures whether or not to enable gpio_int_2(need update in different
         *  project):
         *  0: Disable
         *  1: Enable
         *  - bit16: Configures whether or not to enable gpio_int_3(need update in different
         *  project):
         *  0: Disable
         *  1: Enable
         *  - bit17: invalid
         */
        uint32_t int_ena:5;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} gpio_pin_reg_t;

/** Type of func_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** in_sel : R/W; bitpos: [7:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t in_sel:8;
        /** in_inv_sel : R/W; bitpos: [8]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t in_inv_sel:1;
        /** sig_in_sel : R/W; bitpos: [9]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig_in_sel:1;
        uint32_t reserved_9:22;
    };
    uint32_t val;
} gpio_func_in_sel_cfg_reg_t;

/** Type of funcb_in_sel_cfg register
 *  Configuration register for input signal b
 */
typedef union {
    struct {
        /** out_sel : R/W/SC; bitpos: [8:0]; default: 256;
         *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
         *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
         *  GPIO_OUT_REG[n].
         */
        uint32_t out_sel:9;
        /** out_inv_sel : R/W/SC; bitpos: [9]; default: 0;
         *  set this bit to invert output signal.1:invert.0:not invert.
         */
        uint32_t out_inv_sel:1;
        /** oen_sel : R/W; bitpos: [10]; default: 0;
         *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
         *  enable signal.0:use peripheral output enable signal.
         */
        uint32_t oen_sel:1;
        /** oen_inv_sel : R/W; bitpos: [11]; default: 0;
         *  set this bit to invert output enable signal.1:invert.0:not invert.
         */
        uint32_t oen_inv_sel:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} gpio_func_out_sel_cfg_reg_t;


/** Group: Clock Gate Register */
/** Type of clock_gate register
 *  GPIO clock gate register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether or not to enable clock gate.
         *  0: Not enable
         *  1: Enable, the clock is free running.
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_clock_gate_reg_t;


/** Group: Version Register */
/** Type of date register
 *  GPIO version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 38830416;
         *  Version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_date_reg_t;

typedef struct gpio_dev_t {
    volatile gpio_strap_reg_t strap;
    volatile gpio_out_reg_t out;
    volatile gpio_out_w1ts_reg_t out_w1ts;
    volatile gpio_out_w1tc_reg_t out_w1tc;
    volatile gpio_out1_reg_t out1;
    volatile gpio_out1_w1ts_reg_t out1_w1ts;
    volatile gpio_out1_w1tc_reg_t out1_w1tc;
    volatile gpio_out2_reg_t out2;
    volatile gpio_out2_w1ts_reg_t out2_w1ts;
    volatile gpio_out2_w1tc_reg_t out2_w1tc;
    uint32_t reserved_028[3];
    volatile gpio_enable_reg_t enable;
    volatile gpio_enable_w1ts_reg_t enable_w1ts;
    volatile gpio_enable_w1tc_reg_t enable_w1tc;
    volatile gpio_enable1_reg_t enable1;
    volatile gpio_enable1_w1ts_reg_t enable1_w1ts;
    volatile gpio_enable1_w1tc_reg_t enable1_w1tc;
    volatile gpio_enable2_reg_t enable2;
    volatile gpio_enable2_w1ts_reg_t enable2_w1ts;
    volatile gpio_enable2_w1tc_reg_t enable2_w1tc;
    uint32_t reserved_058[3];
    volatile gpio_in_reg_t in;
    volatile gpio_in1_reg_t in1;
    volatile gpio_in2_reg_t in2;
    uint32_t reserved_070;
    volatile gpio_status_reg_t status;
    volatile gpio_status_w1ts_reg_t status_w1ts;
    volatile gpio_status_w1tc_reg_t status_w1tc;
    volatile gpio_status1_reg_t status1;
    volatile gpio_status1_w1ts_reg_t status1_w1ts;
    volatile gpio_status1_w1tc_reg_t status1_w1tc;
    volatile gpio_status2_reg_t status2;
    volatile gpio_status2_w1ts_reg_t status2_w1ts;
    volatile gpio_status2_w1tc_reg_t status2_w1tc;
    uint32_t reserved_098[3];
    volatile gpio_int_0_reg_t intr_0;
    volatile gpio_int_1_reg_t intr_1;
    volatile gpio_int_2_reg_t intr_2;
    volatile gpio_int_3_reg_t intr_3;
    volatile gpio_int_01_reg_t intr_01;
    volatile gpio_int_11_reg_t intr_11;
    volatile gpio_int_21_reg_t intr_21;
    volatile gpio_int_31_reg_t intr_31;
    volatile gpio_int_02_reg_t int_02;
    volatile gpio_int_12_reg_t int_12;
    volatile gpio_int_22_reg_t int_22;
    volatile gpio_int_32_reg_t int_32;
    uint32_t reserved_0d4[4];
    volatile gpio_status_next_reg_t status_next;
    volatile gpio_status_next1_reg_t status_next1;
    volatile gpio_status_next2_reg_t status_next2;
    uint32_t reserved_0f0;
    volatile gpio_pin_reg_t pin[63];
    uint32_t reserved_200[65];
    volatile gpio_func_in_sel_cfg_reg_t func_in_sel_cfg[256];
    uint32_t reserved_6f4[256];
    volatile gpio_func_out_sel_cfg_reg_t func_out_sel_cfg[63];
    uint32_t reserved_bf0[130];
    volatile gpio_clock_gate_reg_t clock_gate;
    volatile gpio_date_reg_t date;
} gpio_dev_t;

extern gpio_dev_t GPIO;

#ifndef __cplusplus
_Static_assert(sizeof(gpio_dev_t) == 0xe00, "Invalid size of gpio_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
