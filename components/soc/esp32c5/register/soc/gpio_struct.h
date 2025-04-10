/**
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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

/** Group: configuration register */
/** Type of out1 register
 *  GPIO output register for GPIO32-32
 */
typedef union {
    struct {
        /** out1_data_orig : R/W/SC/WTC; bitpos: [0]; default: 0;
         *  Configures the output value of GPIO32 ~ 32 output in simple GPIO output mode.
         *  0: Low level
         *  1: High level
         *  The value of bit32 ~ bit32 correspond to the output value of GPIO32 ~ GPIO32
         *  respectively. Bitxx ~ bitxx is invalid.
         */
        uint32_t out1_data_orig:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_out1_reg_t;

/** Type of out1_w1ts register
 *  GPIO output set register for GPIO32-32
 */
typedef union {
    struct {
        /** out1_w1ts : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to set the output register GPIO_OUT1_REG of GPIO32 ~
         *  GPIO32.
         *  0: Not set
         *  1: The corresponding bit in GPIO_OUT1_REG will be set to 1
         *  Bit32 ~ bit32 are corresponding to GPIO32 ~ GPIO32. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to set GPIO_OUT1_REG.
         */
        uint32_t out1_w1ts:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_out1_w1ts_reg_t;

/** Type of out1_w1tc register
 *  GPIO output clear register for GPIO32-32
 */
typedef union {
    struct {
        /** out1_w1tc : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear the output register GPIO_OUT1_REG of GPIO32 ~
         *  GPIO32 output.
         *  0: Not clear
         *  1: The corresponding bit in GPIO_OUT1_REG will be cleared.
         *  Bit32 ~ bit32 are corresponding to GPIO32 ~ GPIO32. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to clear GPIO_OUT1_REG.
         */
        uint32_t out1_w1tc:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_out1_w1tc_reg_t;

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
 *  GPIO output enable register for GPIO32-32
 */
typedef union {
    struct {
        /** enable1_data : R/W/WTC; bitpos: [0]; default: 0;
         *  Configures whether or not to enable the output of GPIO32 ~ GPIO32.
         *  0: Not enable
         *  1: Enable
         *  Bit32 ~ bit32 are corresponding to GPIO32 ~ GPIO32. Bitxx ~ bitxx is invalid.
         */
        uint32_t enable1_data:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_enable1_reg_t;

/** Type of enable1_w1ts register
 *  GPIO output enable set register for GPIO32-32
 */
typedef union {
    struct {
        /** enable1_w1ts : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to set the output enable register GPIO_ENABLE1_REG of
         *  GPIO32 ~ GPIO32.
         *  0: Not set
         *  1: The corresponding bit in GPIO_ENABLE1_REG will be set to 1
         *  Bit32 ~ bit32 are corresponding to GPIO32 ~ GPIO32. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to set GPIO_ENABLE1_REG.
         */
        uint32_t enable1_w1ts:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_enable1_w1ts_reg_t;

/** Type of enable1_w1tc register
 *  GPIO output enable clear register for GPIO32-32
 */
typedef union {
    struct {
        /** enable1_w1tc : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear the output enable register GPIO_ENABLE1_REG of
         *  GPIO32 ~ GPIO32.
         *  0: Not clear
         *  1: The corresponding bit in GPIO_ENABLE1_REG will be cleared
         *  Bit32 ~ bit32 are corresponding to GPIO32 ~ GPIO32. Bitxx ~ bitxx is invalid.
         *  Recommended operation: use this register to clear GPIO_ENABLE1_REG.
         */
        uint32_t enable1_w1tc:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_enable1_w1tc_reg_t;

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
 *  GPIO input register for GPIO32-32
 */
typedef union {
    struct {
        /** in1_data_next : RO; bitpos: [0]; default: 0;
         *  Represents the input value of GPIO32 ~ GPIO32. Each bit represents a pin input
         *  value:
         *  0: Low level
         *  1: High level
         *  Bit32 ~ bit32 are corresponding to GPIO32 ~ GPIO32. Bitxx ~ bitxx is invalid.
         */
        uint32_t in1_data_next:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_in1_reg_t;

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
 *  GPIO interrupt status register for GPIO32-32
 */
typedef union {
    struct {
        /** status1_interrupt : R/W/WTC; bitpos: [0]; default: 0;
         *  The interrupt status of GPIO32 ~ GPIO32, can be configured by the software.
         *
         *  - Bit32 ~ bit32 are corresponding to GPIO32 ~ GPIO32. Bitxx ~ bitxx is invalid.
         *  - Each bit represents the status of its corresponding GPIO:
         *
         *  - 0: Represents the GPIO does not generate the interrupt configured by
         *  GPIO_PIN$n_INT_TYPE, or this bit is configured to 0 by the software.
         *  - 1: Represents the GPIO generates the interrupt configured by GPIO_PIN$n_INT_TYPE,
         *  or this bit is configured to 1 by the software.
         *
         */
        uint32_t status1_interrupt:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_status1_reg_t;

/** Type of status1_w1ts register
 *  GPIO interrupt status set register for GPIO32-32
 */
typedef union {
    struct {
        /** status1_w1ts : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to set the interrupt status register
         *  GPIO_STATUS1_INTERRUPT of GPIO32 ~ GPIO32.
         *
         *  - Bit32 ~ bit32 are corresponding to GPIO32 ~ GPIO32. Bitxx ~ bitxx is invalid.
         *  - If the value 1 is written to a bit here, the corresponding bit in
         *  GPIO_STATUS1_INTERRUPT will be set to 1. \item Recommended operation: use this
         *  register to set GPIO_STATUS1_INTERRUPT.
         */
        uint32_t status1_w1ts:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_status1_w1ts_reg_t;

/** Type of status1_w1tc register
 *  GPIO interrupt status clear register for GPIO32-32
 */
typedef union {
    struct {
        /** status1_w1tc : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear the interrupt status register
         *  GPIO_STATUS1_INTERRUPT of GPIO32 ~ GPIO32.
         *
         *  - Bit32 ~ bit32 are corresponding to GPIO32 ~ GPIO32. Bitxx ~ bitxx is invalid.
         *  - If the value 1 is written to a bit here, the corresponding bit in
         *  GPIO_STATUS1_INTERRUPT will be cleared. \item Recommended operation: use this
         *  register to clear GPIO_STATUS1_INTERRUPT.
         */
        uint32_t status1_w1tc:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_status1_w1tc_reg_t;

/** Type of procpu_int register
 *  GPIO_PROCPU_INT interrupt status register for GPIO0-31
 */
typedef union {
    struct {
        /** procpu_int : RO; bitpos: [31:0]; default: 0;
         *  Represents the GPIO_PROCPU_INT interrupt status of GPIO0 ~ GPIO31. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_PROCPU_INT interrupt is not enabled, or the GPIO does not
         *  generate the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_PROCPU_INT interrupt is enabled.
         *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t procpu_int:32;
    };
    uint32_t val;
} gpio_procpu_int_reg_t;

/** Type of sdio_int register
 *  GPIO_SDIO_INT interrupt status register
 */
typedef union {
    struct {
        /** sdio_int : RO; bitpos: [31:0]; default: 0;
         *  Represents the GPIO_SDIO_INT interrupt status of GPIO0 ~ GPIO31. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_SDIO_INT interrupt is not enabled, or the GPIO does not generate
         *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_SDIO_INT interrupt is enabled.
         *  Bit0 ~ bit31 are corresponding to GPIO0 ~ GPIO31. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t sdio_int:32;
    };
    uint32_t val;
} gpio_sdio_int_reg_t;

/** Type of procpu_int1 register
 *  GPIO_PROCPU_INT interrupt status register for GPIO32-32
 */
typedef union {
    struct {
        /** procpu_int1 : RO; bitpos: [0]; default: 0;
         *  Represents the GPIO_PROCPU_INT interrupt status of GPIO32 ~ GPIO32. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_PROCPU_INT interrupt is not enabled, or the GPIO does not
         *  generate the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_PROCPU_INT interrupt is enabled.
         *  Bit32 ~ bit32 are corresponding to GPIO32 ~ GPIO32. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS1_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t procpu_int1:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_procpu_int1_reg_t;

/** Type of sdio_int1 register
 *  GPIO_SDIO_INT interrupt status register
 */
typedef union {
    struct {
        /** sdio_int1 : RO; bitpos: [0]; default: 0;
         *  Represents the GPIO_SDIO_INT interrupt status of GPIO32 ~ GPIO32. Each bit
         *  represents:(need update in different project)
         *  0: Represents GPIO_SDIO_INT interrupt is not enabled, or the GPIO does not generate
         *  the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: Represents the GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE
         *  after the GPIO_SDIO_INT interrupt is enabled.
         *  Bit32 ~ bit32 are corresponding to GPIO32 ~ GPIO32. Bitxx ~ bitxx is invalid. This
         *  interrupt status is corresponding to the bit in GPIO_STATUS1_REG when assert (high)
         *  enable signal (bit13 of GPIO_PIN$n_REG).
         */
        uint32_t sdio_int1:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_sdio_int1_reg_t;

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
 *  GPIO interrupt source register for GPIO32-32
 */
typedef union {
    struct {
        /** status_interrupt_next1 : RO; bitpos: [0]; default: 0;
         *  Represents the interrupt source signal of GPIO32 ~ GPIO32.
         *  Bit0 ~ bit24 are corresponding to GPIO32 ~ GPIO32. Bitxx ~ bitxx is invalid. Each
         *  bit represents:
         *  0: The GPIO does not generate the interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  1: The GPIO generates an interrupt configured by GPIO_PIN$n_INT_TYPE.
         *  The interrupt could be rising edge interrupt, falling edge interrupt, level
         *  sensitive interrupt and any edge interrupt.
         */
        uint32_t status_interrupt_next1:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_status_next1_reg_t;

/** Group: Pin Configuration Registers */
/** Type of pinn register
 *  GPIO$N configuration register
 */
typedef union {
    struct {
        /** sync2_bypass : R/W; bitpos: [1:0]; default: 0;
         *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
         *  operating clock for the second-level synchronization.
         *  0: Not synchronize
         *  1: Synchronize on falling edge
         *  2: Synchronize on rising edge
         *  3: Synchronize on rising edge
         */
        uint32_t sync2_bypass:2;
        /** pad_driver : R/W; bitpos: [2]; default: 0;
         *  Configures to select pin drive mode.
         *  0: Normal output
         *  1: Open drain output
         */
        uint32_t pad_driver:1;
        /** sync1_bypass : R/W; bitpos: [4:3]; default: 0;
         *  Configures whether or not to synchronize GPIO input data on either edge of IO MUX
         *  operating clock for the first-level synchronization.
         *  0: Not synchronize
         *  1: Synchronize on falling edge
         *  2: Synchronize on rising edge
         *  3: Synchronize on rising edge
         */
        uint32_t sync1_bypass:2;
        uint32_t reserved_5:2;
        /** int_type : R/W; bitpos: [9:7]; default: 0;
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
         *  Configures whether or not to enable GPIO wake-up function.
         *  0: Disable
         *  1: Enable
         *  This function only wakes up the CPU from Light-sleep.
         */
        uint32_t wakeup_enable:1;
        uint32_t reserved_11:2;
        /** int_ena : R/W; bitpos: [17:13]; default: 0;
         *  Configures whether or not to enable gpio_procpu_int or gpio_sdio_int(need update in
         *  different project).
         *
         *  - bit13: Configures whether or not to enable gpio_procpu_int(need update in
         *  different project):
         *  0: Disable
         *  1: Enable
         *  - bit15: Configures whether or not to enable gpio_sdio_int(need update in different
         *  project):
         *  0: Disable
         *  1: Enable
         *  - bit16 ~ bit17: invalid
         */
        uint32_t int_ena:5;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} gpio_pinn_reg_t;


/** Group: Input Configuration Registers */
/** Type of funcn_in_sel_cfg register
 *  Configuration register for input signal n
 */
typedef union {
    struct {
        /** in_sel : R/W; bitpos: [6:0]; default: 96;
         *  Configures to select a pin from the 29 GPIO pins to connect the input signal f.
         *  0: Select GPIO0
         *  1: Select GPIO1
         *  ......
         *  27: Select GPIO27
         *  28: Select GPIO28
         *  Or
         *  0x40: A constantly high input
         *  0x60: A constantly low input
         */
        uint32_t in_sel:7;
        /** in_inv_sel : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to invert the input value.
         *  0: Not invert
         *  1: Invert
         */
        uint32_t in_inv_sel:1;
        /** sig_in_sel : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to route signals via GPIO matrix.
         *  0: Bypass GPIO matrix, i.e., connect signals directly to peripheral configured in
         *  IO MUX.
         *  1: Route signals via GPIO matrix.
         */
        uint32_t sig_in_sel:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} gpio_func_in_sel_cfg_reg_t;



/** Group: Output Configuration Registers */
/** Type of funcn_out_sel_cfg register
 *  Configuration register for GPIOn output
 */
typedef union {
    struct {
        /** out_sel : R/W/SC; bitpos: [8:0]; default: 256;
         *  Configures to select a signal $Y (0 <= $Y < 256) from 256 peripheral signals to be
         *  output from GPIOn.
         *  0: Select signal 0
         *  1: Select signal 1
         *  ......
         *  254: Select signal 254
         *  255: Select signal 255
         *  Or
         *  256: Bit n of GPIO_OUT_REG and GPIO_ENABLE_REG are selected as the output value and
         *  output enable.
         *
         *  For the detailed signal list, see Table .
         *  "
         */
        uint32_t out_sel:9;
        /** out_inv_sel : R/W/SC; bitpos: [9]; default: 0;
         *  Configures whether or not to invert the output value.
         *  0: Not invert
         *  1: Invert
         */
        uint32_t out_inv_sel:1;
        /** oen_sel : R/W; bitpos: [10]; default: 0;
         *  Configures to select the source of output enable signal.
         *  0: Use output enable signal from peripheral.
         *  1: Force the output enable signal to be sourced from bit n of GPIO_ENABLE_REG.
         */
        uint32_t oen_sel:1;
        /** oe_inv_sel : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to invert the output enable signal.
         *  0: Not invert
         *  1: Invert
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
        /** date : R/W; bitpos: [27:0]; default: 37815040;
         *  Version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_date_reg_t;


typedef struct {
    volatile gpio_strap_reg_t strap;
    volatile gpio_out_reg_t out;
    volatile gpio_out_w1ts_reg_t out_w1ts;
    volatile gpio_out_w1tc_reg_t out_w1tc;
    volatile gpio_out1_reg_t out1;
    volatile gpio_out1_w1ts_reg_t out1_w1ts;
    volatile gpio_out1_w1tc_reg_t out1_w1tc;
    uint32_t reserved_01c[6];
    volatile gpio_enable_reg_t enable;
    volatile gpio_enable_w1ts_reg_t enable_w1ts;
    volatile gpio_enable_w1tc_reg_t enable_w1tc;
    volatile gpio_enable1_reg_t enable1;
    volatile gpio_enable1_w1ts_reg_t enable1_w1ts;
    volatile gpio_enable1_w1tc_reg_t enable1_w1tc;
    uint32_t reserved_04c[6];
    volatile gpio_in_reg_t in;
    volatile gpio_in1_reg_t in1;
    uint32_t reserved_06c[2];
    volatile gpio_status_reg_t status;
    volatile gpio_status_w1ts_reg_t status_w1ts;
    volatile gpio_status_w1tc_reg_t status_w1tc;
    volatile gpio_status1_reg_t status1;
    volatile gpio_status1_w1ts_reg_t status1_w1ts;
    volatile gpio_status1_w1tc_reg_t status1_w1tc;
    uint32_t reserved_08c[6];
    volatile gpio_procpu_int_reg_t pcpu_int;
    volatile gpio_sdio_int_reg_t sdio_int;
    volatile gpio_procpu_int1_reg_t pcpu_int1;
    volatile gpio_sdio_int1_reg_t sdio_int1;
    uint32_t reserved_0b4[4];
    volatile gpio_status_next_reg_t status_next;
    volatile gpio_status_next1_reg_t status_next1;
    uint32_t reserved_0cc[2];
    volatile gpio_pinn_reg_t pin[29];
    volatile gpio_pinn_reg_t pin29;
    volatile gpio_pinn_reg_t pin30;
    volatile gpio_pinn_reg_t pin31;
    volatile gpio_pinn_reg_t pin32;
    uint32_t reserved_158[95];
    volatile gpio_func_in_sel_cfg_reg_t func_in_sel_cfg[256]; /* 1-5, 18-26, 36-40, 44-45, 67-69, 71-73, 93-96, 117-255 are reserved
                                                               *  The length ought to match the SIG_GPIO_OUT_IDX that defined in `gpio_sig_map.h`
                                                               */
    uint32_t reserved_49d[256];
    volatile gpio_func_out_sel_cfg_reg_t func_out_sel_cfg[29];
    volatile gpio_func_out_sel_cfg_reg_t func29_out_sel_cfg;
    volatile gpio_func_out_sel_cfg_reg_t func30_out_sel_cfg;
    volatile gpio_func_out_sel_cfg_reg_t func31_out_sel_cfg;
    volatile gpio_func_out_sel_cfg_reg_t func32_out_sel_cfg;
    uint32_t reserved_b58[168];
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
