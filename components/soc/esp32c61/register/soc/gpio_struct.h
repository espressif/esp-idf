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

/** Group: configuration register */
/** Type of strap register
 *  pad strapping register
 */
typedef union {
    struct {
        /** strapping : RO; bitpos: [15:0]; default: 0;
         *  pad strapping register
         */
        uint32_t strapping:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} gpio_strap_reg_t;

/** Type of out register
 *  GPIO output register for GPIO0-31
 */
typedef union {
    struct {
        /** out_data_orig : R/W/SC/WTC; bitpos: [31:0]; default: 0;
         *  GPIO output register for GPIO0-31
         */
        uint32_t out_data_orig:32;
    };
    uint32_t val;
} gpio_out_reg_t;

/** Type of out_w1ts register
 *  GPIO output set register for GPIO0-31
 */
typedef union {
    struct {
        /** out_w1ts : WT; bitpos: [31:0]; default: 0;
         *  GPIO output set register for GPIO0-31
         */
        uint32_t out_w1ts:32;
    };
    uint32_t val;
} gpio_out_w1ts_reg_t;

/** Type of out_w1tc register
 *  GPIO output clear register for GPIO0-31
 */
typedef union {
    struct {
        /** out_w1tc : WT; bitpos: [31:0]; default: 0;
         *  GPIO output clear register for GPIO0-31
         */
        uint32_t out_w1tc:32;
    };
    uint32_t val;
} gpio_out_w1tc_reg_t;

/** Type of out1 register
 *  GPIO output register for GPIO32-33
 */
typedef union {
    struct {
        /** out1_data_orig : R/W/SC/WTC; bitpos: [1:0]; default: 0;
         *  GPIO output register for GPIO32-33
         */
        uint32_t out1_data_orig:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} gpio_out1_reg_t;

/** Type of out1_w1ts register
 *  GPIO output set register for GPIO32-33
 */
typedef union {
    struct {
        /** out1_w1ts : WT; bitpos: [1:0]; default: 0;
         *  GPIO output set register for GPIO32-33
         */
        uint32_t out1_w1ts:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} gpio_out1_w1ts_reg_t;

/** Type of out1_w1tc register
 *  GPIO output clear register for GPIO32-33
 */
typedef union {
    struct {
        /** out1_w1tc : WT; bitpos: [1:0]; default: 0;
         *  GPIO output clear register for GPIO32-33
         */
        uint32_t out1_w1tc:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} gpio_out1_w1tc_reg_t;

/** Type of enable register
 *  GPIO output enable register for GPIO0-31
 */
typedef union {
    struct {
        /** enable_data : R/W/WTC; bitpos: [31:0]; default: 0;
         *  GPIO output enable register for GPIO0-31
         */
        uint32_t enable_data:32;
    };
    uint32_t val;
} gpio_enable_reg_t;

/** Type of enable_w1ts register
 *  GPIO output enable set register for GPIO0-31
 */
typedef union {
    struct {
        /** enable_w1ts : WT; bitpos: [31:0]; default: 0;
         *  GPIO output enable set register for GPIO0-31
         */
        uint32_t enable_w1ts:32;
    };
    uint32_t val;
} gpio_enable_w1ts_reg_t;

/** Type of enable_w1tc register
 *  GPIO output enable clear register for GPIO0-31
 */
typedef union {
    struct {
        /** enable_w1tc : WT; bitpos: [31:0]; default: 0;
         *  GPIO output enable clear register for GPIO0-31
         */
        uint32_t enable_w1tc:32;
    };
    uint32_t val;
} gpio_enable_w1tc_reg_t;

/** Type of enable1 register
 *  GPIO output enable register for GPIO32-33
 */
typedef union {
    struct {
        /** enable1_data : R/W/WTC; bitpos: [1:0]; default: 0;
         *  GPIO output enable register for GPIO32-33
         */
        uint32_t enable1_data:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} gpio_enable1_reg_t;

/** Type of enable1_w1ts register
 *  GPIO output enable set register for GPIO32-33
 */
typedef union {
    struct {
        /** enable1_w1ts : WT; bitpos: [1:0]; default: 0;
         *  GPIO output enable set register for GPIO32-33
         */
        uint32_t enable1_w1ts:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} gpio_enable1_w1ts_reg_t;

/** Type of enable1_w1tc register
 *  GPIO output enable clear register for GPIO32-33
 */
typedef union {
    struct {
        /** enable1_w1tc : WT; bitpos: [1:0]; default: 0;
         *  GPIO output enable clear register for GPIO32-33
         */
        uint32_t enable1_w1tc:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} gpio_enable1_w1tc_reg_t;

/** Type of in register
 *  GPIO input register for GPIO0-31
 */
typedef union {
    struct {
        /** in_data_next : RO; bitpos: [31:0]; default: 0;
         *  GPIO input register for GPIO0-31
         */
        uint32_t in_data_next:32;
    };
    uint32_t val;
} gpio_in_reg_t;

/** Type of in1 register
 *  GPIO input register for GPIO32-33
 */
typedef union {
    struct {
        /** in1_data_next : RO; bitpos: [1:0]; default: 0;
         *  GPIO input register for GPIO32-33
         */
        uint32_t in1_data_next:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} gpio_in1_reg_t;

/** Type of status register
 *  GPIO interrupt status register for GPIO0-31
 */
typedef union {
    struct {
        /** status_interrupt : R/W/WTC; bitpos: [31:0]; default: 0;
         *  GPIO interrupt status register for GPIO0-31
         */
        uint32_t status_interrupt:32;
    };
    uint32_t val;
} gpio_status_reg_t;

/** Type of status_w1ts register
 *  GPIO interrupt status set register for GPIO0-31
 */
typedef union {
    struct {
        /** status_w1ts : WT; bitpos: [31:0]; default: 0;
         *  GPIO interrupt status set register for GPIO0-31
         */
        uint32_t status_w1ts:32;
    };
    uint32_t val;
} gpio_status_w1ts_reg_t;

/** Type of status_w1tc register
 *  GPIO interrupt status clear register for GPIO0-31
 */
typedef union {
    struct {
        /** status_w1tc : WT; bitpos: [31:0]; default: 0;
         *  GPIO interrupt status clear register for GPIO0-31
         */
        uint32_t status_w1tc:32;
    };
    uint32_t val;
} gpio_status_w1tc_reg_t;

/** Type of status1 register
 *  GPIO interrupt status register for GPIO32-33
 */
typedef union {
    struct {
        /** status1_interrupt : R/W/WTC; bitpos: [1:0]; default: 0;
         *  GPIO interrupt status register for GPIO32-33
         */
        uint32_t status1_interrupt:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} gpio_status1_reg_t;

/** Type of status1_w1ts register
 *  GPIO interrupt status set register for GPIO32-33
 */
typedef union {
    struct {
        /** status1_w1ts : WT; bitpos: [1:0]; default: 0;
         *  GPIO interrupt status set register for GPIO32-33
         */
        uint32_t status1_w1ts:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} gpio_status1_w1ts_reg_t;

/** Type of status1_w1tc register
 *  GPIO interrupt status clear register for GPIO32-33
 */
typedef union {
    struct {
        /** status1_w1tc : WT; bitpos: [1:0]; default: 0;
         *  GPIO interrupt status clear register for GPIO32-33
         */
        uint32_t status1_w1tc:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} gpio_status1_w1tc_reg_t;

/** Type of procpu_int register
 *  GPIO_PROCPU_INT interrupt status register for GPIO0-31
 */
typedef union {
    struct {
        /** procpu_int : RO; bitpos: [31:0]; default: 0;
         *  GPIO_PROCPU_INT interrupt status register for GPIO0-31
         */
        uint32_t procpu_int:32;
    };
    uint32_t val;
} gpio_procpu_int_reg_t;

/** Type of sdio_int register
 *  GPIO_SDIO_INT interrupt status register for GPIO0-31
 */
typedef union {
    struct {
        /** sdio_int : RO; bitpos: [31:0]; default: 0;
         *  GPIO_SDIO_INT interrupt status register for GPIO0-31
         */
        uint32_t sdio_int:32;
    };
    uint32_t val;
} gpio_sdio_int_reg_t;

/** Type of procpu_int1 register
 *  GPIO_PROCPU_INT interrupt status register for GPIO32-33
 */
typedef union {
    struct {
        /** procpu_int1 : RO; bitpos: [1:0]; default: 0;
         *  GPIO_PROCPU_INT interrupt status register for GPIO32-33
         */
        uint32_t procpu_int1:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} gpio_procpu_int1_reg_t;

/** Type of sdio_int1 register
 *  GPIO_SDIO_INT interrupt status register for GPIO32-33
 */
typedef union {
    struct {
        /** sdio_int1 : RO; bitpos: [1:0]; default: 0;
         *  GPIO_SDIO_INT interrupt status register for GPIO32-33
         */
        uint32_t sdio_int1:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} gpio_sdio_int1_reg_t;

/** Type of status_next register
 *  GPIO interrupt source register for GPIO0-31
 */
typedef union {
    struct {
        /** status_interrupt_next : RO; bitpos: [31:0]; default: 0;
         *  GPIO interrupt source register for GPIO0-31
         */
        uint32_t status_interrupt_next:32;
    };
    uint32_t val;
} gpio_status_next_reg_t;

/** Type of status_next1 register
 *  GPIO interrupt source register for GPIO32-33
 */
typedef union {
    struct {
        /** status_interrupt_next1 : RO; bitpos: [1:0]; default: 0;
         *  GPIO interrupt source register for GPIO32-33
         */
        uint32_t status_interrupt_next1:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} gpio_status_next1_reg_t;

/** Type of pinn register
 *  GPIO pin configuration register
 */
typedef union {
    struct {
        /** pinn_sync2_bypass : R/W; bitpos: [1:0]; default: 0;
         *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
         *  posedge.
         */
        uint32_t pinn_sync2_bypass:2;
        /** pinn_pad_driver : R/W; bitpos: [2]; default: 0;
         *  set this bit to select pad driver. 1:open-drain. 0:normal.
         */
        uint32_t pinn_pad_driver:1;
        /** pinn_sync1_bypass : R/W; bitpos: [4:3]; default: 0;
         *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
         *  posedge.
         */
        uint32_t pinn_sync1_bypass:2;
        uint32_t reserved_5:2;
        /** pinn_int_type : R/W; bitpos: [9:7]; default: 0;
         *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
         *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
         *  at high level
         */
        uint32_t pinn_int_type:3;
        /** pinn_wakeup_enable : R/W; bitpos: [10]; default: 0;
         *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
         */
        uint32_t pinn_wakeup_enable:1;
        uint32_t reserved_11:2;
        /** pinn_int_ena : R/W; bitpos: [17:13]; default: 0;
         *  set bit 13 to enable gpio_procpu_int. set bit 15 to enable gpio_sdio_int.
         */
        uint32_t pinn_int_ena:5;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} gpio_pinn_reg_t;

/** Type of func_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func_in_sel : R/W; bitpos: [6:0]; default: 96;
         *  set this value: s=0-33: connect GPIO[s] to this port. s=0x40: set this port always
         *  high level. s=0x60: set this port always low level.
         */
        uint32_t func_in_sel:7;
        /** func_in_inv_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func_in_inv_sel:1;
        /** sig0_in_sel : R/W; bitpos: [8]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig_in_sel:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} gpio_func_in_sel_cfg_reg_t;

/** Type of funcn_out_sel_cfg register
 *  GPIO output function select register
 */
typedef union {
    struct {
        /** funcn_out_sel : R/W/SC; bitpos: [8:0]; default: 256;
         *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
         *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
         *  GPIO_OUT_REG[n].
         */
        uint32_t funcn_out_sel:9;
        /** funcn_out_inv_sel : R/W/SC; bitpos: [9]; default: 0;
         *  set this bit to invert output signal.1:invert.0:not invert.
         */
        uint32_t funcn_out_inv_sel:1;
        /** funcn_oe_sel : R/W; bitpos: [10]; default: 0;
         *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
         *  enable signal.0:use peripheral output enable signal.
         */
        uint32_t funcn_oe_sel:1;
        /** funcn_oe_inv_sel : R/W; bitpos: [11]; default: 0;
         *  set this bit to invert output enable signal.1:invert.0:not invert.
         */
        uint32_t funcn_oe_inv_sel:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} gpio_funcn_out_sel_cfg_reg_t;

/** Type of clock_gate register
 *  GPIO clock gate register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  set this bit to enable GPIO clock gate
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_clock_gate_reg_t;

/** Type of date register
 *  GPIO version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 37823120;
         *  version register
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
    volatile gpio_procpu_int_reg_t procpu_int;
    volatile gpio_sdio_int_reg_t sdio_int;
    volatile gpio_procpu_int1_reg_t procpu_int1;
    volatile gpio_sdio_int1_reg_t sdio_int1;
    uint32_t reserved_0b4[4];
    volatile gpio_status_next_reg_t status_next;
    volatile gpio_status_next1_reg_t status_next1;
    uint32_t reserved_0cc[2];
    volatile gpio_pinn_reg_t pinn[34];  // 0~29 available, 30~34 not fan out, N/A
    uint32_t reserved_15c[94];
    volatile gpio_func_in_sel_cfg_reg_t func_in_sel_cfg[256];  // 0-255. reserved: 1-5, 18-26, 36-40, 44-45, 48-63, 70-71, 75-81, 84-96, 101-117, 122-255
    uint32_t reserved_542[256];
    volatile gpio_funcn_out_sel_cfg_reg_t funcn_out_sel_cfg[34]; // 0~29 available, 30~34 not fan out, N/A
    uint32_t reserved_b5c[167];
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
