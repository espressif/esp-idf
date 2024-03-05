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

/** Group: configuration register */
/** Type of out register
 *  LP GPIO output register
 */
typedef union {
    struct {
        /** out_data_orig : R/W/WTC; bitpos: [7:0]; default: 0;
         *  Configures the output of GPIO0 ~ GPIO7.\\
         *  0: Low level\\
         *  1: High level\\
         *  bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.\\
         */
        uint32_t out_data_orig:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_out_reg_t;

/** Type of out_w1ts register
 *  LP GPIO output set register
 */
typedef union {
    struct {
        /** out_w1ts : WT; bitpos: [7:0]; default: 0;
         *  Configures whether or not to enable the output register LP_IO_OUT_REG of GPIO0 ~
         *  GPIO7.\\
         *
         *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
         *  - If the value 1 is written to a bit here, the corresponding bit in LP_IO_OUT_REG
         *  will be set to 1.
         *  - Recommended operation: use this register to set LP_IO_OUT_REG.
         */
        uint32_t out_w1ts:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_out_w1ts_reg_t;

/** Type of out_w1tc register
 *  LP GPIO output clear register
 */
typedef union {
    struct {
        /** out_w1tc : WT; bitpos: [7:0]; default: 0;
         *  Configures whether or not to clear the output register LP_IO_OUT_REG of GPIO0 ~
         *  GPIO7.\\
         *
         *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
         *  - If the value 1 is written to a bit here, the corresponding bit in LP_IO_OUT_REG
         *  will be cleared.
         *  - Recommended operation: use this register to clear LP_IO_OUT_REG.
         */
        uint32_t out_w1tc:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_out_w1tc_reg_t;

/** Type of enable register
 *  LP GPIO output enable register
 */
typedef union {
    struct {
        /** enable_data : R/W/WTC; bitpos: [7:0]; default: 0;
         *  Configures whether or not to enable the output of GPIO0 ~ GPIO7.\\
         *  0: Not enable\\
         *  1: Enable\\
         *  bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.\\
         */
        uint32_t enable_data:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_enable_reg_t;

/** Type of enable_w1ts register
 *  LP GPIO output enable set register
 */
typedef union {
    struct {
        /** enable_w1ts : WT; bitpos: [7:0]; default: 0;
         *  Configures whether or not to set the output enable register LP_IO_ENABLE_REG of
         *  GPIO0 ~ GPIO7.\\
         *
         *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
         *  - If the value 1 is written to a bit here, the corresponding bit in
         *  LP_IO_ENABLE_REG will be set to 1.
         *  - Recommended operation: use this register to set LP_IO_ENABLE_REG.
         */
        uint32_t enable_w1ts:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_enable_w1ts_reg_t;

/** Type of enable_w1tc register
 *  LP GPIO output enable clear register
 */
typedef union {
    struct {
        /** enable_w1tc : WT; bitpos: [7:0]; default: 0;
         *  Configures whether or not to clear the output enable register LP_IO_ENABLE_REG of
         *  GPIO0 ~ GPIO7.\\
         *
         *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
         *  - If the value 1 is written to a bit here, the corresponding bit in
         *  LP_IO_ENABLE_REG will be cleared.
         *  - Recommended operation: use this register to clear LP_IO_ENABLE_REG.
         */
        uint32_t enable_w1tc:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_enable_w1tc_reg_t;

/** Type of in register
 *  LP GPIO input register
 */
typedef union {
    struct {
        /** in_data_next : RO; bitpos: [7:0]; default: 0;
         *  Represents the input value of GPIO0 ~ GPIO7.\\
         *  0: Low level input\\
         *  1: High level input\\
         *  bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.\\
         */
        uint32_t in_data_next:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_in_reg_t;

/** Type of status register
 *  LP GPIO interrupt status register
 */
typedef union {
    struct {
        /** status_interrupt : R/W/WTC; bitpos: [7:0]; default: 0;
         *  Configures the interrupt status of GPIO0 ~ GPIO7.\\
         *  0: No interrupt\\
         *  1: Interrupt is triggered\\
         *  Bit0 is corresponding to GPIO0, bit1 is corresponding to GPIO1, and etc. This
         *  register is used together LP_IO_PIN$n_INT_TYPE in register LP_IO_PIN$n_REG.\\
         */
        uint32_t status_interrupt:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_status_reg_t;

/** Type of status_w1ts register
 *  LP GPIO interrupt status set register
 */
typedef union {
    struct {
        /** status_w1ts : WT; bitpos: [7:0]; default: 0;
         *  Configures whether or not to set the interrupt status register LP_IO_STATUS_INT of
         *  GPIO0 ~ GPIO7.\\
         *
         *  - Bit0 is corresponding to GPIO0, bit1 is corresponding to GPIO1, and etc.
         *  - If the value 1 is written to a bit here, the corresponding bit in
         *  LP_IO_STATUS_INT will be set to 1.
         *  - Recommended operation: use this register to set LP_IO_STATUS_INT.
         */
        uint32_t status_w1ts:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_status_w1ts_reg_t;

/** Type of status_w1tc register
 *  LP GPIO interrupt status clear register
 */
typedef union {
    struct {
        /** status_w1tc : WT; bitpos: [7:0]; default: 0;
         *  Configures whether or not to clear the interrupt status register LP_IO_STATUS_INT
         *  of GPIO0 ~ GPIO7. \\
         *
         *  - Bit0 is corresponding to GPIO0, bit1 is corresponding to GPIO1, and etc.
         *  - If the value 1 is written to a bit here, the corresponding bit in
         *  LP_IO_STATUS_INT will be cleared
         *  - ecommended operation: use this register to clear LP_IO_STATUS_INT.
         */
        uint32_t status_w1tc:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_status_w1tc_reg_t;

/** Type of status_next register
 *  LP GPIO interrupt source register
 */
typedef union {
    struct {
        /** status_interrupt_next : RO; bitpos: [7:0]; default: 0;
         *  Represents the interrupt source status of GPIO0 ~ GPIO7.\\
         *  bit0 ~ bit7 are corresponding to GPIO0 ~ 7. Each bit represents:\\
         *  0: Interrupt source status is invalid.\\
         *  1: Interrupt source status is valid.\\
         *  The interrupt here can be rising-edge triggered, falling-edge triggered, any edge
         *  triggered, or level triggered.\\
         */
        uint32_t status_interrupt_next:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_status_next_reg_t;

/** Type of pinn register
 *  LP GPIO0 configuration register
 */
typedef union {
    struct {
        /** pinn_sync2_bypass : R/W; bitpos: [1:0]; default: 0;
         *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
         *  MUX operating clock for the second-level synchronization.\\
         *  0: Not synchronize\\
         *  1: Synchronize on falling edge\\
         *  2: Synchronize on rising edge\\
         *  3: Synchronize on rising edge\\
         */
        uint32_t pinn_sync2_bypass:2;
        /** pinn_pad_driver : R/W; bitpos: [2]; default: 0;
         *  Configures to select the pin dirve mode of GPIOn.\\
         *  0: Normal output\\
         *  1: Open drain output\\
         */
        uint32_t pinn_pad_driver:1;
        /** pinn_sync1_bypass : R/W; bitpos: [4:3]; default: 0;
         *  Configures whether or not to synchronize GPIO input data on either edge of LP IO
         *  MUX operating clock for the first-level synchronization.\\
         *  0: Not synchronize\\
         *  1: Synchronize on falling edge\\
         *  2: Synchronize on rising edge\\
         *  3: Synchronize on rising edge\\
         */
        uint32_t pinn_sync1_bypass:2;
        /** pinn_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear the edge wake-up status of GPIO0 ~ GPIO7.\\
         *
         *  - bit0 ~ bit7 are corresponding to GPIO0 ~ GPIO7.
         *  - If the value 1 is written to a bit here, the edge wake-up status of corresponding
         *  GPIO will be cleared.
         */
        uint32_t pinn_edge_wakeup_clr:1;
        uint32_t reserved_6:1;
        /** pinn_int_type : R/W; bitpos: [9:7]; default: 0;
         *  Configures GPIOn interrupt type.\\
         *  0: GPIO interrupt disabled \\
         *  1: Rising edge trigger \\
         *  2: Falling edge trigger \\
         *  3: Any edge trigger \\
         *  4: Low level trigger \\
         *  5: High level trigger \\
         */
        uint32_t pinn_int_type:3;
        /** pinn_wakeup_enable : R/W; bitpos: [10]; default: 0;
         *  Configures whether or not to enable GPIOn wake-up function.\\
         *  0: Not enable\\
         *  1: Enable\\
         *  This function is disabled when PD_LP_PERI is powered off.\\
         */
        uint32_t pinn_wakeup_enable:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} lp_gpio_pinn_reg_t;

/** Type of funcn_out_sel_cfg register
 *  Configuration register for GPIO0 output
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

/** Type of clock_gate register
 *  GPIO clock gate register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  set this bit to enable GPIO clock gate.\\
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_gpio_clock_gate_reg_t;

/** Type of date register
 *  GPIO version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36773904;
         *  version register.\\
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
    volatile lp_gpio_pinn_reg_t pinn[8];
    uint32_t reserved_050[152];
    volatile lp_gpio_funcn_out_sel_cfg_reg_t funcn_out_sel_cfg[8];
    uint32_t reserved_2d0[74];
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
