/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Registers */
/** Type of out register
 *  LP_GPIO output register
 */
typedef union {
    struct {
        /** out_data_orig : R/W/WTC; bitpos: [6:0]; default: 0;
         *  Configures the output value of LP_GPIO0 ~ 6 output in simple LP_GPIO output mode.\\
         *  0: Low level\\
         *  1: High level\\
         *  The value of bit0 ~ bit6 correspond to the output value of LP_GPIO0 ~ LP_GPIO6
         *  respectively. Bit7 ~ bit31 are invalid.\\
         */
        uint32_t out_data_orig:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_out_reg_t;

/** Type of out_w1ts register
 *  LP_GPIO output set register
 */
typedef union {
    struct {
        /** out_w1ts : WT; bitpos: [6:0]; default: 0;
         *  Configures whether or not to set the output register LP_GPIO_OUT_REG of LP_GPIO0 ~
         *  LP_GPIO6.\\
         *  0: Not set\\
         *  1: The corresponding bit in LP_GPIO_OUT_REG will be set to 1\\
         *  Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid. \\
         *  Recommended operation: use this register to set LP_GPIO_OUT_REG. \\
         */
        uint32_t out_w1ts:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_out_w1ts_reg_t;

/** Type of out_w1tc register
 *  LP_GPIO output clear register
 */
typedef union {
    struct {
        /** out_w1tc : WT; bitpos: [6:0]; default: 0;
         *  Configures whether or not to clear the output register LP_GPIO_OUT_REG of LP_GPIO0
         *  ~ LP_GPIO6 output.\\
         *  0: Not clear\\
         *  1: The corresponding bit in LP_GPIO_OUT_REG will be cleared.\\
         *  Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid. \\
         *  Recommended operation: use this register to clear LP_GPIO_OUT_REG. \\
         */
        uint32_t out_w1tc:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_out_w1tc_reg_t;

/** Type of enable register
 *  LP_GPIO output enable register
 */
typedef union {
    struct {
        /** enable_data : R/W/WTC; bitpos: [6:0]; default: 0;
         *  Configures whether or not to enable the output of LP_GPIO0 ~ LP_GPIO6.\\
         *  0: Not enable\\
         *  1: Enable\\
         *  Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid.\\
         */
        uint32_t enable_data:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_enable_reg_t;

/** Type of enable_w1ts register
 *  LP_GPIO output enable set register
 */
typedef union {
    struct {
        /** enable_w1ts : WT; bitpos: [6:0]; default: 0;
         *  Configures whether or not to set the output enable register LP_GPIO_ENABLE_REG of
         *  LP_GPIO0 ~ LP_GPIO6.\\
         *  0: Not set\\
         *  1: The corresponding bit in LP_GPIO_ENABLE_REG will be set to 1\\
         *  Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid. \\
         *  Recommended operation: use this register to set LP_GPIO_ENABLE_REG.\\
         */
        uint32_t enable_w1ts:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_enable_w1ts_reg_t;

/** Type of enable_w1tc register
 *  LP_GPIO output enable clear register
 */
typedef union {
    struct {
        /** enable_w1tc : WT; bitpos: [6:0]; default: 0;
         *  Configures whether or not to clear the output enable register LP_GPIO_ENABLE_REG of
         *  LP_GPIO0 ~ LP_GPIO6. \\
         *  0: Not clear\\
         *  1: The corresponding bit in LP_GPIO_ENABLE_REG will be cleared\\
         *  Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid. \\
         *  Recommended operation: use this register to clear LP_GPIO_ENABLE_REG.\\
         */
        uint32_t enable_w1tc:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_enable_w1tc_reg_t;

/** Type of in register
 *  LP_GPIO input register
 */
typedef union {
    struct {
        /** in_data_next : RO; bitpos: [6:0]; default: 0;
         *  Represents the input value of LP_GPIO0 ~ LP_GPIO6. Each bit represents a pin input
         *  value:\\
         *  0: Low level\\
         *  1: High level\\
         *  Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid.\\
         */
        uint32_t in_data_next:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_in_reg_t;


/** Group: Interrupt Status Registers */
/** Type of status register
 *  LP_GPIO interrupt status register
 */
typedef union {
    struct {
        /** status_interrupt : R/W/WTC; bitpos: [6:0]; default: 0;
         *  The interrupt status of LP_GPIO0 ~ LP_GPIO6, can be configured by the software.
         *
         *  - Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid.
         *  - Each bit represents the status of its corresponding LP_GPIO:
         *
         *  - 0: Represents the LP_GPIO does not generate the interrupt configured by
         *  LP_GPIO_PIN$n_INT_TYPE, or this bit is configured to 0 by the software.
         *  - 1: Represents the LP_GPIO generates the interrupt configured by
         *  LP_GPIO_PIN$n_INT_TYPE, or this bit is configured to 1 by the software.
         *
         */
        uint32_t status_interrupt:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_status_reg_t;

/** Type of status_w1ts register
 *  LP_GPIO interrupt status set register
 */
typedef union {
    struct {
        /** status_w1ts : WT; bitpos: [6:0]; default: 0;
         *  Configures whether or not to set the interrupt status register
         *  LP_GPIO_STATUS_INTERRUPT of LP_GPIO0 ~ LP_GPIO6.
         *
         *  - Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid.
         *  - If the value 1 is written to a bit here, the corresponding bit in
         *  LP_GPIO_STATUS_INTERRUPT will be set to 1. \item Recommended operation: use this
         *  register to set LP_GPIO_STATUS_INTERRUPT.
         */
        uint32_t status_w1ts:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_status_w1ts_reg_t;

/** Type of status_w1tc register
 *  LP_GPIO interrupt status clear register
 */
typedef union {
    struct {
        /** status_w1tc : WT; bitpos: [6:0]; default: 0;
         *  Configures whether or not to clear the interrupt status register
         *  LP_GPIO_STATUS_INTERRUPT of LP_GPIO0 ~ LP_GPIO6.
         *
         *  - Bit0 ~ bit6 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid.
         *  - If the value 1 is written to a bit here, the corresponding bit in
         *  LP_GPIO_STATUS_INTERRUPT will be cleared. \item Recommended operation: use this
         *  register to clear LP_GPIO_STATUS_INTERRUPT.
         */
        uint32_t status_w1tc:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_status_w1tc_reg_t;

/** Type of status_next register
 *  LP_GPIO interrupt source register
 */
typedef union {
    struct {
        /** status_interrupt_next : RO; bitpos: [6:0]; default: 0;
         *  Represents the interrupt source signal of LP_GPIO0 ~ LP_GPIO6.\\
         *  Bit0 ~ bit24 are corresponding to LP_GPIO0 ~ LP_GPIO6. Bit7 ~ bit31 are invalid.
         *  Each bit represents:\\
         *  0: The LP_GPIO does not generate the interrupt configured by
         *  LP_GPIO_PIN$n_INT_TYPE.\\
         *  1: The LP_GPIO generates an interrupt configured by LP_GPIO_PIN$n_INT_TYPE.\\
         *  The interrupt could be rising edge interrupt, falling edge interrupt, level
         *  sensitive interrupt and any edge interrupt.\\
         */
        uint32_t status_interrupt_next:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_status_next_reg_t;


/** Group: Pin Configuration Registers */
/** Type of pinn register
 *  LP_GPIO0 configuration register
 */
typedef union {
    struct {
        /** pinn_sync2_bypass : R/W; bitpos: [1:0]; default: 0;
         *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
         *  MUX operating clock for the second-level synchronization.\\
         *  0: Not synchronize\\
         *  1: Synchronize on falling edge\\
         *  2: Synchronize on rising edge\\
         *  3: Synchronize on rising edge\\
         */
        uint32_t pinn_sync2_bypass:2;
        /** pinn_pad_driver : R/W; bitpos: [2]; default: 0;
         *  Configures to select pin drive mode. \\
         *  0: Normal output\\
         *  1: Open drain output \\
         */
        uint32_t pinn_pad_driver:1;
        /** pinn_sync1_bypass : R/W; bitpos: [4:3]; default: 0;
         *  Configures whether or not to synchronize LP_GPIO input data on either edge of LP IO
         *  MUX operating clock for the first-level synchronization.\\
         *  0: Not synchronize\\
         *  1: Synchronize on falling edge\\
         *  2: Synchronize on rising edge\\
         *  3: Synchronize on rising edge\\
         */
        uint32_t pinn_sync1_bypass:2;
        /** pinn_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  LP_GPIO wakeup clear register.
         */
        uint32_t pinn_edge_wakeup_clr:1;
        uint32_t reserved_6:1;
        /** pinn_int_type : R/W; bitpos: [9:7]; default: 0;
         *  Configures LP_GPIO interrupt type.\\
         *  0: LP_GPIO interrupt disabled\\
         *  1: Rising edge trigger\\
         *  2: Falling edge trigger\\
         *  3: Any edge trigger\\
         *  4: Low level trigger\\
         *  5: High level trigger\\
         */
        uint32_t pinn_int_type:3;
        /** pinn_wakeup_enable : R/W; bitpos: [10]; default: 0;
         *  Configures whether or not to enable LP_GPIO wake-up function.\\
         *  0: Disable\\
         *  1: Enable\\
         *  This function only wakes up the CPU from Light-sleep. \\
         */
        uint32_t pinn_wakeup_enable:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} lp_gpio_pinn_reg_t;


/** Group: Output Configuration Registers */
/** Type of funcn_out_sel_cfg register
 *  Configuration register for LP_GPIOn output
 */
typedef union {
    struct {
        /** funcn_out_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to invert the output value.\\
         *  0: Not invert\\
         *  1: Invert\\
         */
        uint32_t funcn_out_inv_sel:1;
        uint32_t reserved_1:1;
        /** funcn_oe_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to invert the output enable signal.\\
         *  0: Not invert\\
         *  1: Invert\\
         */
        uint32_t funcn_oe_inv_sel:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_gpio_funcn_out_sel_cfg_reg_t;


/** Group: Clock Gate Register */
/** Type of clock_gate register
 *  LP_GPIO clock gate register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether or not to enable clock gate.\\
         *  0: Not enable\\
         *  1: Enable, the clock is free running. \\
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_gpio_clock_gate_reg_t;


/** Group: Version Register */
/** Type of date register
 *  LP_GPIO version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36766272;
         *  Version control register. \\
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_gpio_date_reg_t;


typedef struct {
    uint32_t reserved_000;
    volatile lp_gpio_out_reg_t out;
    volatile lp_gpio_out_w1ts_reg_t out_w1ts;
    volatile lp_gpio_out_w1tc_reg_t out_w1tc;
    volatile lp_gpio_enable_reg_t enable;
    volatile lp_gpio_enable_w1ts_reg_t enable_w1ts;
    volatile lp_gpio_enable_w1tc_reg_t enable_w1tc;
    volatile lp_gpio_in_reg_t in;
    volatile lp_gpio_status_reg_t status;
    volatile lp_gpio_status_w1ts_reg_t status_w1ts;
    volatile lp_gpio_status_w1tc_reg_t status_w1tc;
    volatile lp_gpio_status_next_reg_t status_next;
    volatile lp_gpio_pinn_reg_t pinn[7];
    uint32_t reserved_04c[153];
    volatile lp_gpio_funcn_out_sel_cfg_reg_t funcn_out_sel_cfg[7];
    uint32_t reserved_2cc[75];
    volatile lp_gpio_clock_gate_reg_t clock_gate;
    volatile lp_gpio_date_reg_t date;
} lp_gpio_dev_t;

extern lp_gpio_dev_t LP_GPIO;

#ifndef __cplusplus
_Static_assert(sizeof(lp_gpio_dev_t) == 0x400, "Invalid size of lp_gpio_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
