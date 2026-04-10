/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Tee mode control register */
/** Type of mn_mode_ctrl register
 *  TEE mode control register
 */
typedef union {
    struct {
        /** mn_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures Mn security level mode.
         *  0: tee_mode
         *  1: ree_mode0
         *  2: ree_mode1
         *  3: ree_mode2
         */
        uint32_t mn_mode:2;
        /** mn_lock : R/W; bitpos: [2]; default: 0;
         *  Set 1 to lock m0 tee configuration
         */
        uint32_t mn_lock:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} tee_mn_mode_ctrl_reg_t;


/** Group: read write control register */
/** Type of gpspi0_ctrl register
 *  gpspi0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_gpspi0 : R/W; bitpos: [0]; default: 1;
         *  Configures gpspi0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_gpspi0:1;
        /** read_ree0_gpspi0 : R/W; bitpos: [1]; default: 0;
         *  Configures gpspi0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_gpspi0:1;
        /** read_ree1_gpspi0 : R/W; bitpos: [2]; default: 0;
         *  Configures gpspi0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_gpspi0:1;
        /** read_ree2_gpspi0 : R/W; bitpos: [3]; default: 0;
         *  Configures gpspi0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_gpspi0:1;
        /** write_tee_gpspi0 : R/W; bitpos: [4]; default: 1;
         *  Configures gpspi0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_gpspi0:1;
        /** write_ree0_gpspi0 : R/W; bitpos: [5]; default: 0;
         *  Configures gpspi0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_gpspi0:1;
        /** write_ree1_gpspi0 : R/W; bitpos: [6]; default: 0;
         *  Configures gpspi0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_gpspi0:1;
        /** write_ree2_gpspi0 : R/W; bitpos: [7]; default: 0;
         *  Configures gpspi0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_gpspi0:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_gpspi0_ctrl_reg_t;

/** Type of gpspi1_ctrl register
 *  gpspi1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_gpspi1 : R/W; bitpos: [0]; default: 1;
         *  Configures gpspi1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_gpspi1:1;
        /** read_ree0_gpspi1 : R/W; bitpos: [1]; default: 0;
         *  Configures gpspi1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_gpspi1:1;
        /** read_ree1_gpspi1 : R/W; bitpos: [2]; default: 0;
         *  Configures gpspi1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_gpspi1:1;
        /** read_ree2_gpspi1 : R/W; bitpos: [3]; default: 0;
         *  Configures gpspi1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_gpspi1:1;
        /** write_tee_gpspi1 : R/W; bitpos: [4]; default: 1;
         *  Configures gpspi1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_gpspi1:1;
        /** write_ree0_gpspi1 : R/W; bitpos: [5]; default: 0;
         *  Configures gpspi1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_gpspi1:1;
        /** write_ree1_gpspi1 : R/W; bitpos: [6]; default: 0;
         *  Configures gpspi1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_gpspi1:1;
        /** write_ree2_gpspi1 : R/W; bitpos: [7]; default: 0;
         *  Configures gpspi1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_gpspi1:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_gpspi1_ctrl_reg_t;

/** Type of uart0_ctrl register
 *  uart0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_uart0 : R/W; bitpos: [0]; default: 1;
         *  Configures uart0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_uart0:1;
        /** read_ree0_uart0 : R/W; bitpos: [1]; default: 0;
         *  Configures uart0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_uart0:1;
        /** read_ree1_uart0 : R/W; bitpos: [2]; default: 0;
         *  Configures uart0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_uart0:1;
        /** read_ree2_uart0 : R/W; bitpos: [3]; default: 0;
         *  Configures uart0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_uart0:1;
        /** write_tee_uart0 : R/W; bitpos: [4]; default: 1;
         *  Configures uart0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_uart0:1;
        /** write_ree0_uart0 : R/W; bitpos: [5]; default: 0;
         *  Configures uart0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_uart0:1;
        /** write_ree1_uart0 : R/W; bitpos: [6]; default: 0;
         *  Configures uart0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_uart0:1;
        /** write_ree2_uart0 : R/W; bitpos: [7]; default: 0;
         *  Configures uart0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_uart0:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_uart0_ctrl_reg_t;

/** Type of uart1_ctrl register
 *  uart1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_uart1 : R/W; bitpos: [0]; default: 1;
         *  Configures uart1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_uart1:1;
        /** read_ree0_uart1 : R/W; bitpos: [1]; default: 0;
         *  Configures uart1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_uart1:1;
        /** read_ree1_uart1 : R/W; bitpos: [2]; default: 0;
         *  Configures uart1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_uart1:1;
        /** read_ree2_uart1 : R/W; bitpos: [3]; default: 0;
         *  Configures uart1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_uart1:1;
        /** write_tee_uart1 : R/W; bitpos: [4]; default: 1;
         *  Configures uart1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_uart1:1;
        /** write_ree0_uart1 : R/W; bitpos: [5]; default: 0;
         *  Configures uart1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_uart1:1;
        /** write_ree1_uart1 : R/W; bitpos: [6]; default: 0;
         *  Configures uart1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_uart1:1;
        /** write_ree2_uart1 : R/W; bitpos: [7]; default: 0;
         *  Configures uart1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_uart1:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_uart1_ctrl_reg_t;

/** Type of uhci_ctrl register
 *  uhci read/write control register
 */
typedef union {
    struct {
        /** read_tee_uhci : R/W; bitpos: [0]; default: 1;
         *  Configures uhci registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_uhci:1;
        /** read_ree0_uhci : R/W; bitpos: [1]; default: 0;
         *  Configures uhci registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_uhci:1;
        /** read_ree1_uhci : R/W; bitpos: [2]; default: 0;
         *  Configures uhci registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_uhci:1;
        /** read_ree2_uhci : R/W; bitpos: [3]; default: 0;
         *  Configures uhci registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_uhci:1;
        /** write_tee_uhci : R/W; bitpos: [4]; default: 1;
         *  Configures uhci registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_uhci:1;
        /** write_ree0_uhci : R/W; bitpos: [5]; default: 0;
         *  Configures uhci registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_uhci:1;
        /** write_ree1_uhci : R/W; bitpos: [6]; default: 0;
         *  Configures uhci registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_uhci:1;
        /** write_ree2_uhci : R/W; bitpos: [7]; default: 0;
         *  Configures uhci registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_uhci:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_uhci_ctrl_reg_t;

/** Type of i2c0_ctrl register
 *  i2c0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_i2c0 : R/W; bitpos: [0]; default: 1;
         *  Configures i2c0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_i2c0:1;
        /** read_ree0_i2c0 : R/W; bitpos: [1]; default: 0;
         *  Configures i2c0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_i2c0:1;
        /** read_ree1_i2c0 : R/W; bitpos: [2]; default: 0;
         *  Configures i2c0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_i2c0:1;
        /** read_ree2_i2c0 : R/W; bitpos: [3]; default: 0;
         *  Configures i2c0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_i2c0:1;
        /** write_tee_i2c0 : R/W; bitpos: [4]; default: 1;
         *  Configures i2c0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_i2c0:1;
        /** write_ree0_i2c0 : R/W; bitpos: [5]; default: 0;
         *  Configures i2c0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_i2c0:1;
        /** write_ree1_i2c0 : R/W; bitpos: [6]; default: 0;
         *  Configures i2c0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_i2c0:1;
        /** write_ree2_i2c0 : R/W; bitpos: [7]; default: 0;
         *  Configures i2c0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_i2c0:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_i2c0_ctrl_reg_t;

/** Type of i2c1_ctrl register
 *  i2c1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_i2c1 : R/W; bitpos: [0]; default: 1;
         *  Configures i2c1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_i2c1:1;
        /** read_ree0_i2c1 : R/W; bitpos: [1]; default: 0;
         *  Configures i2c1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_i2c1:1;
        /** read_ree1_i2c1 : R/W; bitpos: [2]; default: 0;
         *  Configures i2c1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_i2c1:1;
        /** read_ree2_i2c1 : R/W; bitpos: [3]; default: 0;
         *  Configures i2c1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_i2c1:1;
        /** write_tee_i2c1 : R/W; bitpos: [4]; default: 1;
         *  Configures i2c1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_i2c1:1;
        /** write_ree0_i2c1 : R/W; bitpos: [5]; default: 0;
         *  Configures i2c1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_i2c1:1;
        /** write_ree1_i2c1 : R/W; bitpos: [6]; default: 0;
         *  Configures i2c1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_i2c1:1;
        /** write_ree2_i2c1 : R/W; bitpos: [7]; default: 0;
         *  Configures i2c1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_i2c1:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_i2c1_ctrl_reg_t;

/** Type of i2s_ctrl register
 *  i2s read/write control register
 */
typedef union {
    struct {
        /** read_tee_i2s : R/W; bitpos: [0]; default: 1;
         *  Configures i2s registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_i2s:1;
        /** read_ree0_i2s : R/W; bitpos: [1]; default: 0;
         *  Configures i2s registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_i2s:1;
        /** read_ree1_i2s : R/W; bitpos: [2]; default: 0;
         *  Configures i2s registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_i2s:1;
        /** read_ree2_i2s : R/W; bitpos: [3]; default: 0;
         *  Configures i2s registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_i2s:1;
        /** write_tee_i2s : R/W; bitpos: [4]; default: 1;
         *  Configures i2s registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_i2s:1;
        /** write_ree0_i2s : R/W; bitpos: [5]; default: 0;
         *  Configures i2s registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_i2s:1;
        /** write_ree1_i2s : R/W; bitpos: [6]; default: 0;
         *  Configures i2s registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_i2s:1;
        /** write_ree2_i2s : R/W; bitpos: [7]; default: 0;
         *  Configures i2s registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_i2s:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_i2s_ctrl_reg_t;

/** Type of parl_io_ctrl register
 *  parl_io read/write control register
 */
typedef union {
    struct {
        /** read_tee_parl_io : R/W; bitpos: [0]; default: 1;
         *  Configures parl_io registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_parl_io:1;
        /** read_ree0_parl_io : R/W; bitpos: [1]; default: 0;
         *  Configures parl_io registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_parl_io:1;
        /** read_ree1_parl_io : R/W; bitpos: [2]; default: 0;
         *  Configures parl_io registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_parl_io:1;
        /** read_ree2_parl_io : R/W; bitpos: [3]; default: 0;
         *  Configures parl_io registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_parl_io:1;
        /** write_tee_parl_io : R/W; bitpos: [4]; default: 1;
         *  Configures parl_io registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_parl_io:1;
        /** write_ree0_parl_io : R/W; bitpos: [5]; default: 0;
         *  Configures parl_io registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_parl_io:1;
        /** write_ree1_parl_io : R/W; bitpos: [6]; default: 0;
         *  Configures parl_io registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_parl_io:1;
        /** write_ree2_parl_io : R/W; bitpos: [7]; default: 0;
         *  Configures parl_io registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_parl_io:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_parl_io_ctrl_reg_t;

/** Type of pwm0_ctrl register
 *  pwm0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_pwm0 : R/W; bitpos: [0]; default: 1;
         *  Configures pwm0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_pwm0:1;
        /** read_ree0_pwm0 : R/W; bitpos: [1]; default: 0;
         *  Configures pwm0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_pwm0:1;
        /** read_ree1_pwm0 : R/W; bitpos: [2]; default: 0;
         *  Configures pwm0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_pwm0:1;
        /** read_ree2_pwm0 : R/W; bitpos: [3]; default: 0;
         *  Configures pwm0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_pwm0:1;
        /** write_tee_pwm0 : R/W; bitpos: [4]; default: 1;
         *  Configures pwm0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_pwm0:1;
        /** write_ree0_pwm0 : R/W; bitpos: [5]; default: 0;
         *  Configures pwm0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_pwm0:1;
        /** write_ree1_pwm0 : R/W; bitpos: [6]; default: 0;
         *  Configures pwm0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_pwm0:1;
        /** write_ree2_pwm0 : R/W; bitpos: [7]; default: 0;
         *  Configures pwm0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_pwm0:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_pwm0_ctrl_reg_t;

/** Type of pwm1_ctrl register
 *  pwm1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_pwm1 : R/W; bitpos: [0]; default: 1;
         *  Configures pwm1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_pwm1:1;
        /** read_ree0_pwm1 : R/W; bitpos: [1]; default: 0;
         *  Configures pwm1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_pwm1:1;
        /** read_ree1_pwm1 : R/W; bitpos: [2]; default: 0;
         *  Configures pwm1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_pwm1:1;
        /** read_ree2_pwm1 : R/W; bitpos: [3]; default: 0;
         *  Configures pwm1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_pwm1:1;
        /** write_tee_pwm1 : R/W; bitpos: [4]; default: 1;
         *  Configures pwm1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_pwm1:1;
        /** write_ree0_pwm1 : R/W; bitpos: [5]; default: 0;
         *  Configures pwm1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_pwm1:1;
        /** write_ree1_pwm1 : R/W; bitpos: [6]; default: 0;
         *  Configures pwm1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_pwm1:1;
        /** write_ree2_pwm1 : R/W; bitpos: [7]; default: 0;
         *  Configures pwm1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_pwm1:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_pwm1_ctrl_reg_t;

/** Type of ledc_ctrl register
 *  ledc read/write control register
 */
typedef union {
    struct {
        /** read_tee_ledc : R/W; bitpos: [0]; default: 1;
         *  Configures ledc registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_ledc:1;
        /** read_ree0_ledc : R/W; bitpos: [1]; default: 0;
         *  Configures ledc registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_ledc:1;
        /** read_ree1_ledc : R/W; bitpos: [2]; default: 0;
         *  Configures ledc registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_ledc:1;
        /** read_ree2_ledc : R/W; bitpos: [3]; default: 0;
         *  Configures ledc registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_ledc:1;
        /** write_tee_ledc : R/W; bitpos: [4]; default: 1;
         *  Configures ledc registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_ledc:1;
        /** write_ree0_ledc : R/W; bitpos: [5]; default: 0;
         *  Configures ledc registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_ledc:1;
        /** write_ree1_ledc : R/W; bitpos: [6]; default: 0;
         *  Configures ledc registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_ledc:1;
        /** write_ree2_ledc : R/W; bitpos: [7]; default: 0;
         *  Configures ledc registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_ledc:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_ledc_ctrl_reg_t;

/** Type of can_ctrl register
 *  can read/write control register
 */
typedef union {
    struct {
        /** read_tee_can : R/W; bitpos: [0]; default: 1;
         *  Configures can registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_can:1;
        /** read_ree0_can : R/W; bitpos: [1]; default: 0;
         *  Configures can registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_can:1;
        /** read_ree1_can : R/W; bitpos: [2]; default: 0;
         *  Configures can registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_can:1;
        /** read_ree2_can : R/W; bitpos: [3]; default: 0;
         *  Configures can registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_can:1;
        /** write_tee_can : R/W; bitpos: [4]; default: 1;
         *  Configures can registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_can:1;
        /** write_ree0_can : R/W; bitpos: [5]; default: 0;
         *  Configures can registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_can:1;
        /** write_ree1_can : R/W; bitpos: [6]; default: 0;
         *  Configures can registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_can:1;
        /** write_ree2_can : R/W; bitpos: [7]; default: 0;
         *  Configures can registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_can:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_can_ctrl_reg_t;

/** Type of usb_serial_jtag_ctrl register
 *  usb_serial_jtag read/write control register
 */
typedef union {
    struct {
        /** read_tee_usb_serial_jtag : R/W; bitpos: [0]; default: 1;
         *  Configures usb_serial_jtag registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_usb_serial_jtag:1;
        /** read_ree0_usb_serial_jtag : R/W; bitpos: [1]; default: 0;
         *  Configures usb_serial_jtag registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_usb_serial_jtag:1;
        /** read_ree1_usb_serial_jtag : R/W; bitpos: [2]; default: 0;
         *  Configures usb_serial_jtag registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_usb_serial_jtag:1;
        /** read_ree2_usb_serial_jtag : R/W; bitpos: [3]; default: 0;
         *  Configures usb_serial_jtag registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_usb_serial_jtag:1;
        /** write_tee_usb_serial_jtag : R/W; bitpos: [4]; default: 1;
         *  Configures usb_serial_jtag registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_usb_serial_jtag:1;
        /** write_ree0_usb_serial_jtag : R/W; bitpos: [5]; default: 0;
         *  Configures usb_serial_jtag registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_usb_serial_jtag:1;
        /** write_ree1_usb_serial_jtag : R/W; bitpos: [6]; default: 0;
         *  Configures usb_serial_jtag registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_usb_serial_jtag:1;
        /** write_ree2_usb_serial_jtag : R/W; bitpos: [7]; default: 0;
         *  Configures usb_serial_jtag registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_usb_serial_jtag:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_usb_serial_jtag_ctrl_reg_t;

/** Type of rmt_ctrl register
 *  rmt read/write control register
 */
typedef union {
    struct {
        /** read_tee_rmt : R/W; bitpos: [0]; default: 1;
         *  Configures rmt registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_rmt:1;
        /** read_ree0_rmt : R/W; bitpos: [1]; default: 0;
         *  Configures rmt registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_rmt:1;
        /** read_ree1_rmt : R/W; bitpos: [2]; default: 0;
         *  Configures rmt registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_rmt:1;
        /** read_ree2_rmt : R/W; bitpos: [3]; default: 0;
         *  Configures rmt registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_rmt:1;
        /** write_tee_rmt : R/W; bitpos: [4]; default: 1;
         *  Configures rmt registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_rmt:1;
        /** write_ree0_rmt : R/W; bitpos: [5]; default: 0;
         *  Configures rmt registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_rmt:1;
        /** write_ree1_rmt : R/W; bitpos: [6]; default: 0;
         *  Configures rmt registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_rmt:1;
        /** write_ree2_rmt : R/W; bitpos: [7]; default: 0;
         *  Configures rmt registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_rmt:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_rmt_ctrl_reg_t;

/** Type of gdma_ctrl register
 *  gdma read/write control register
 */
typedef union {
    struct {
        /** read_tee_gdma : R/W; bitpos: [0]; default: 1;
         *  Configures gdma registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_gdma:1;
        /** read_ree0_gdma : R/W; bitpos: [1]; default: 0;
         *  Configures gdma registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_gdma:1;
        /** read_ree1_gdma : R/W; bitpos: [2]; default: 0;
         *  Configures gdma registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_gdma:1;
        /** read_ree2_gdma : R/W; bitpos: [3]; default: 0;
         *  Configures gdma registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_gdma:1;
        /** write_tee_gdma : R/W; bitpos: [4]; default: 1;
         *  Configures gdma registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_gdma:1;
        /** write_ree0_gdma : R/W; bitpos: [5]; default: 0;
         *  Configures gdma registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_gdma:1;
        /** write_ree1_gdma : R/W; bitpos: [6]; default: 0;
         *  Configures gdma registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_gdma:1;
        /** write_ree2_gdma : R/W; bitpos: [7]; default: 0;
         *  Configures gdma registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_gdma:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_gdma_ctrl_reg_t;

/** Type of regdma_ctrl register
 *  regdma read/write control register
 */
typedef union {
    struct {
        /** read_tee_regdma : R/W; bitpos: [0]; default: 1;
         *  Configures regdma registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_regdma:1;
        /** read_ree0_regdma : R/W; bitpos: [1]; default: 0;
         *  Configures regdma registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_regdma:1;
        /** read_ree1_regdma : R/W; bitpos: [2]; default: 0;
         *  Configures regdma registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_regdma:1;
        /** read_ree2_regdma : R/W; bitpos: [3]; default: 0;
         *  Configures regdma registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_regdma:1;
        /** write_tee_regdma : R/W; bitpos: [4]; default: 1;
         *  Configures regdma registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_regdma:1;
        /** write_ree0_regdma : R/W; bitpos: [5]; default: 0;
         *  Configures regdma registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_regdma:1;
        /** write_ree1_regdma : R/W; bitpos: [6]; default: 0;
         *  Configures regdma registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_regdma:1;
        /** write_ree2_regdma : R/W; bitpos: [7]; default: 0;
         *  Configures regdma registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_regdma:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_regdma_ctrl_reg_t;

/** Type of etm_ctrl register
 *  etm read/write control register
 */
typedef union {
    struct {
        /** read_tee_etm : R/W; bitpos: [0]; default: 1;
         *  Configures etm registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_etm:1;
        /** read_ree0_etm : R/W; bitpos: [1]; default: 0;
         *  Configures etm registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_etm:1;
        /** read_ree1_etm : R/W; bitpos: [2]; default: 0;
         *  Configures etm registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_etm:1;
        /** read_ree2_etm : R/W; bitpos: [3]; default: 0;
         *  Configures etm registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_etm:1;
        /** write_tee_etm : R/W; bitpos: [4]; default: 1;
         *  Configures etm registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_etm:1;
        /** write_ree0_etm : R/W; bitpos: [5]; default: 0;
         *  Configures etm registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_etm:1;
        /** write_ree1_etm : R/W; bitpos: [6]; default: 0;
         *  Configures etm registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_etm:1;
        /** write_ree2_etm : R/W; bitpos: [7]; default: 0;
         *  Configures etm registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_etm:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_etm_ctrl_reg_t;

/** Type of intmtx_core0_ctrl register
 *  intmtx_core0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_intmtx_core0 : R/W; bitpos: [0]; default: 1;
         *  Configures intmtx_core0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_intmtx_core0:1;
        /** read_ree0_intmtx_core0 : R/W; bitpos: [1]; default: 0;
         *  Configures intmtx_core0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_intmtx_core0:1;
        /** read_ree1_intmtx_core0 : R/W; bitpos: [2]; default: 0;
         *  Configures intmtx_core0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_intmtx_core0:1;
        /** read_ree2_intmtx_core0 : R/W; bitpos: [3]; default: 0;
         *  Configures intmtx_core0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_intmtx_core0:1;
        /** write_tee_intmtx_core0 : R/W; bitpos: [4]; default: 1;
         *  Configures intmtx_core0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_intmtx_core0:1;
        /** write_ree0_intmtx_core0 : R/W; bitpos: [5]; default: 0;
         *  Configures intmtx_core0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_intmtx_core0:1;
        /** write_ree1_intmtx_core0 : R/W; bitpos: [6]; default: 0;
         *  Configures intmtx_core0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_intmtx_core0:1;
        /** write_ree2_intmtx_core0 : R/W; bitpos: [7]; default: 0;
         *  Configures intmtx_core0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_intmtx_core0:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_intmtx_core0_ctrl_reg_t;

/** Type of intmtx_core1_ctrl register
 *  intmtx_core1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_intmtx_core1 : R/W; bitpos: [0]; default: 1;
         *  Configures intmtx_core1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_intmtx_core1:1;
        /** read_ree0_intmtx_core1 : R/W; bitpos: [1]; default: 0;
         *  Configures intmtx_core1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_intmtx_core1:1;
        /** read_ree1_intmtx_core1 : R/W; bitpos: [2]; default: 0;
         *  Configures intmtx_core1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_intmtx_core1:1;
        /** read_ree2_intmtx_core1 : R/W; bitpos: [3]; default: 0;
         *  Configures intmtx_core1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_intmtx_core1:1;
        /** write_tee_intmtx_core1 : R/W; bitpos: [4]; default: 1;
         *  Configures intmtx_core1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_intmtx_core1:1;
        /** write_ree0_intmtx_core1 : R/W; bitpos: [5]; default: 0;
         *  Configures intmtx_core1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_intmtx_core1:1;
        /** write_ree1_intmtx_core1 : R/W; bitpos: [6]; default: 0;
         *  Configures intmtx_core1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_intmtx_core1:1;
        /** write_ree2_intmtx_core1 : R/W; bitpos: [7]; default: 0;
         *  Configures intmtx_core1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_intmtx_core1:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_intmtx_core1_ctrl_reg_t;

/** Type of apb_adc_ctrl register
 *  apb_adc read/write control register
 */
typedef union {
    struct {
        /** read_tee_apb_adc : R/W; bitpos: [0]; default: 1;
         *  Configures apb_adc registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_apb_adc:1;
        /** read_ree0_apb_adc : R/W; bitpos: [1]; default: 0;
         *  Configures apb_adc registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_apb_adc:1;
        /** read_ree1_apb_adc : R/W; bitpos: [2]; default: 0;
         *  Configures apb_adc registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_apb_adc:1;
        /** read_ree2_apb_adc : R/W; bitpos: [3]; default: 0;
         *  Configures apb_adc registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_apb_adc:1;
        /** write_tee_apb_adc : R/W; bitpos: [4]; default: 1;
         *  Configures apb_adc registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_apb_adc:1;
        /** write_ree0_apb_adc : R/W; bitpos: [5]; default: 0;
         *  Configures apb_adc registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_apb_adc:1;
        /** write_ree1_apb_adc : R/W; bitpos: [6]; default: 0;
         *  Configures apb_adc registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_apb_adc:1;
        /** write_ree2_apb_adc : R/W; bitpos: [7]; default: 0;
         *  Configures apb_adc registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_apb_adc:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_apb_adc_ctrl_reg_t;

/** Type of timergroup0_ctrl register
 *  timergroup0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_timergroup0 : R/W; bitpos: [0]; default: 1;
         *  Configures timergroup0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_timergroup0:1;
        /** read_ree0_timergroup0 : R/W; bitpos: [1]; default: 0;
         *  Configures timergroup0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_timergroup0:1;
        /** read_ree1_timergroup0 : R/W; bitpos: [2]; default: 0;
         *  Configures timergroup0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_timergroup0:1;
        /** read_ree2_timergroup0 : R/W; bitpos: [3]; default: 0;
         *  Configures timergroup0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_timergroup0:1;
        /** write_tee_timergroup0 : R/W; bitpos: [4]; default: 1;
         *  Configures timergroup0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_timergroup0:1;
        /** write_ree0_timergroup0 : R/W; bitpos: [5]; default: 0;
         *  Configures timergroup0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_timergroup0:1;
        /** write_ree1_timergroup0 : R/W; bitpos: [6]; default: 0;
         *  Configures timergroup0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_timergroup0:1;
        /** write_ree2_timergroup0 : R/W; bitpos: [7]; default: 0;
         *  Configures timergroup0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_timergroup0:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_timergroup0_ctrl_reg_t;

/** Type of timergroup1_ctrl register
 *  timergroup1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_timergroup1 : R/W; bitpos: [0]; default: 1;
         *  Configures timergroup1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_timergroup1:1;
        /** read_ree0_timergroup1 : R/W; bitpos: [1]; default: 0;
         *  Configures timergroup1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_timergroup1:1;
        /** read_ree1_timergroup1 : R/W; bitpos: [2]; default: 0;
         *  Configures timergroup1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_timergroup1:1;
        /** read_ree2_timergroup1 : R/W; bitpos: [3]; default: 0;
         *  Configures timergroup1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_timergroup1:1;
        /** write_tee_timergroup1 : R/W; bitpos: [4]; default: 1;
         *  Configures timergroup1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_timergroup1:1;
        /** write_ree0_timergroup1 : R/W; bitpos: [5]; default: 0;
         *  Configures timergroup1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_timergroup1:1;
        /** write_ree1_timergroup1 : R/W; bitpos: [6]; default: 0;
         *  Configures timergroup1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_timergroup1:1;
        /** write_ree2_timergroup1 : R/W; bitpos: [7]; default: 0;
         *  Configures timergroup1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_timergroup1:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_timergroup1_ctrl_reg_t;

/** Type of systimer_ctrl register
 *  systimer read/write control register
 */
typedef union {
    struct {
        /** read_tee_systimer : R/W; bitpos: [0]; default: 1;
         *  Configures systimer registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_systimer:1;
        /** read_ree0_systimer : R/W; bitpos: [1]; default: 0;
         *  Configures systimer registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_systimer:1;
        /** read_ree1_systimer : R/W; bitpos: [2]; default: 0;
         *  Configures systimer registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_systimer:1;
        /** read_ree2_systimer : R/W; bitpos: [3]; default: 0;
         *  Configures systimer registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_systimer:1;
        /** write_tee_systimer : R/W; bitpos: [4]; default: 1;
         *  Configures systimer registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_systimer:1;
        /** write_ree0_systimer : R/W; bitpos: [5]; default: 0;
         *  Configures systimer registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_systimer:1;
        /** write_ree1_systimer : R/W; bitpos: [6]; default: 0;
         *  Configures systimer registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_systimer:1;
        /** write_ree2_systimer : R/W; bitpos: [7]; default: 0;
         *  Configures systimer registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_systimer:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_systimer_ctrl_reg_t;

/** Type of misc_ctrl register
 *  misc read/write control register
 */
typedef union {
    struct {
        /** read_tee_misc : R/W; bitpos: [0]; default: 1;
         *  Configures misc registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_misc:1;
        /** read_ree0_misc : R/W; bitpos: [1]; default: 0;
         *  Configures misc registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_misc:1;
        /** read_ree1_misc : R/W; bitpos: [2]; default: 0;
         *  Configures misc registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_misc:1;
        /** read_ree2_misc : R/W; bitpos: [3]; default: 0;
         *  Configures misc registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_misc:1;
        /** write_tee_misc : R/W; bitpos: [4]; default: 1;
         *  Configures misc registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_misc:1;
        /** write_ree0_misc : R/W; bitpos: [5]; default: 0;
         *  Configures misc registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_misc:1;
        /** write_ree1_misc : R/W; bitpos: [6]; default: 0;
         *  Configures misc registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_misc:1;
        /** write_ree2_misc : R/W; bitpos: [7]; default: 0;
         *  Configures misc registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_misc:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_misc_ctrl_reg_t;

/** Type of src_ctrl register
 *  src read/write control register
 */
typedef union {
    struct {
        /** read_tee_src : R/W; bitpos: [0]; default: 1;
         *  Configures src registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_src:1;
        /** read_ree0_src : R/W; bitpos: [1]; default: 0;
         *  Configures src registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_src:1;
        /** read_ree1_src : R/W; bitpos: [2]; default: 0;
         *  Configures src registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_src:1;
        /** read_ree2_src : R/W; bitpos: [3]; default: 0;
         *  Configures src registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_src:1;
        /** write_tee_src : R/W; bitpos: [4]; default: 1;
         *  Configures src registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_src:1;
        /** write_ree0_src : R/W; bitpos: [5]; default: 0;
         *  Configures src registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_src:1;
        /** write_ree1_src : R/W; bitpos: [6]; default: 0;
         *  Configures src registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_src:1;
        /** write_ree2_src : R/W; bitpos: [7]; default: 0;
         *  Configures src registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_src:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_src_ctrl_reg_t;

/** Type of usb_otg_fs_core_ctrl register
 *  usb_otg_fs_core read/write control register
 */
typedef union {
    struct {
        /** read_tee_usb_otg_fs_core : R/W; bitpos: [0]; default: 1;
         *  Configures usb_otg_fs_core registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_usb_otg_fs_core:1;
        /** read_ree0_usb_otg_fs_core : R/W; bitpos: [1]; default: 0;
         *  Configures usb_otg_fs_core registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_usb_otg_fs_core:1;
        /** read_ree1_usb_otg_fs_core : R/W; bitpos: [2]; default: 0;
         *  Configures usb_otg_fs_core registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_usb_otg_fs_core:1;
        /** read_ree2_usb_otg_fs_core : R/W; bitpos: [3]; default: 0;
         *  Configures usb_otg_fs_core registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_usb_otg_fs_core:1;
        /** write_tee_usb_otg_fs_core : R/W; bitpos: [4]; default: 1;
         *  Configures usb_otg_fs_core registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_usb_otg_fs_core:1;
        /** write_ree0_usb_otg_fs_core : R/W; bitpos: [5]; default: 0;
         *  Configures usb_otg_fs_core registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_usb_otg_fs_core:1;
        /** write_ree1_usb_otg_fs_core : R/W; bitpos: [6]; default: 0;
         *  Configures usb_otg_fs_core registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_usb_otg_fs_core:1;
        /** write_ree2_usb_otg_fs_core : R/W; bitpos: [7]; default: 0;
         *  Configures usb_otg_fs_core registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_usb_otg_fs_core:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_usb_otg_fs_core_ctrl_reg_t;

/** Type of usb_otg_fs_phy_ctrl register
 *  usb_otg_fs_phy read/write control register
 */
typedef union {
    struct {
        /** read_tee_usb_otg_fs_phy : R/W; bitpos: [0]; default: 1;
         *  Configures usb_otg_fs_phy registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_usb_otg_fs_phy:1;
        /** read_ree0_usb_otg_fs_phy : R/W; bitpos: [1]; default: 0;
         *  Configures usb_otg_fs_phy registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_usb_otg_fs_phy:1;
        /** read_ree1_usb_otg_fs_phy : R/W; bitpos: [2]; default: 0;
         *  Configures usb_otg_fs_phy registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_usb_otg_fs_phy:1;
        /** read_ree2_usb_otg_fs_phy : R/W; bitpos: [3]; default: 0;
         *  Configures usb_otg_fs_phy registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_usb_otg_fs_phy:1;
        /** write_tee_usb_otg_fs_phy : R/W; bitpos: [4]; default: 1;
         *  Configures usb_otg_fs_phy registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_usb_otg_fs_phy:1;
        /** write_ree0_usb_otg_fs_phy : R/W; bitpos: [5]; default: 0;
         *  Configures usb_otg_fs_phy registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_usb_otg_fs_phy:1;
        /** write_ree1_usb_otg_fs_phy : R/W; bitpos: [6]; default: 0;
         *  Configures usb_otg_fs_phy registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_usb_otg_fs_phy:1;
        /** write_ree2_usb_otg_fs_phy : R/W; bitpos: [7]; default: 0;
         *  Configures usb_otg_fs_phy registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_usb_otg_fs_phy:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_usb_otg_fs_phy_ctrl_reg_t;

/** Type of pvt_monitor_ctrl register
 *  pvt_monitor read/write control register
 */
typedef union {
    struct {
        /** read_tee_pvt_monitor : R/W; bitpos: [0]; default: 1;
         *  Configures pvt_monitor registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_pvt_monitor:1;
        /** read_ree0_pvt_monitor : R/W; bitpos: [1]; default: 0;
         *  Configures pvt_monitor registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_pvt_monitor:1;
        /** read_ree1_pvt_monitor : R/W; bitpos: [2]; default: 0;
         *  Configures pvt_monitor registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_pvt_monitor:1;
        /** read_ree2_pvt_monitor : R/W; bitpos: [3]; default: 0;
         *  Configures pvt_monitor registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_pvt_monitor:1;
        /** write_tee_pvt_monitor : R/W; bitpos: [4]; default: 1;
         *  Configures pvt_monitor registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_pvt_monitor:1;
        /** write_ree0_pvt_monitor : R/W; bitpos: [5]; default: 0;
         *  Configures pvt_monitor registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_pvt_monitor:1;
        /** write_ree1_pvt_monitor : R/W; bitpos: [6]; default: 0;
         *  Configures pvt_monitor registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_pvt_monitor:1;
        /** write_ree2_pvt_monitor : R/W; bitpos: [7]; default: 0;
         *  Configures pvt_monitor registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_pvt_monitor:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_pvt_monitor_ctrl_reg_t;

/** Type of pcnt_ctrl register
 *  pcnt read/write control register
 */
typedef union {
    struct {
        /** read_tee_pcnt : R/W; bitpos: [0]; default: 1;
         *  Configures pcnt registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_pcnt:1;
        /** read_ree0_pcnt : R/W; bitpos: [1]; default: 0;
         *  Configures pcnt registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_pcnt:1;
        /** read_ree1_pcnt : R/W; bitpos: [2]; default: 0;
         *  Configures pcnt registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_pcnt:1;
        /** read_ree2_pcnt : R/W; bitpos: [3]; default: 0;
         *  Configures pcnt registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_pcnt:1;
        /** write_tee_pcnt : R/W; bitpos: [4]; default: 1;
         *  Configures pcnt registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_pcnt:1;
        /** write_ree0_pcnt : R/W; bitpos: [5]; default: 0;
         *  Configures pcnt registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_pcnt:1;
        /** write_ree1_pcnt : R/W; bitpos: [6]; default: 0;
         *  Configures pcnt registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_pcnt:1;
        /** write_ree2_pcnt : R/W; bitpos: [7]; default: 0;
         *  Configures pcnt registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_pcnt:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_pcnt_ctrl_reg_t;

/** Type of iomux_ctrl register
 *  iomux read/write control register
 */
typedef union {
    struct {
        /** read_tee_iomux : R/W; bitpos: [0]; default: 1;
         *  Configures iomux registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_iomux:1;
        /** read_ree0_iomux : R/W; bitpos: [1]; default: 0;
         *  Configures iomux registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_iomux:1;
        /** read_ree1_iomux : R/W; bitpos: [2]; default: 0;
         *  Configures iomux registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_iomux:1;
        /** read_ree2_iomux : R/W; bitpos: [3]; default: 0;
         *  Configures iomux registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_iomux:1;
        /** write_tee_iomux : R/W; bitpos: [4]; default: 1;
         *  Configures iomux registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_iomux:1;
        /** write_ree0_iomux : R/W; bitpos: [5]; default: 0;
         *  Configures iomux registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_iomux:1;
        /** write_ree1_iomux : R/W; bitpos: [6]; default: 0;
         *  Configures iomux registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_iomux:1;
        /** write_ree2_iomux : R/W; bitpos: [7]; default: 0;
         *  Configures iomux registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_iomux:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_iomux_ctrl_reg_t;

/** Type of psram_mem_monitor_ctrl register
 *  psram_mem_monitor read/write control register
 */
typedef union {
    struct {
        /** read_tee_psram_mem_monitor : R/W; bitpos: [0]; default: 1;
         *  Configures psram_mem_monitor registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_psram_mem_monitor:1;
        /** read_ree0_psram_mem_monitor : R/W; bitpos: [1]; default: 0;
         *  Configures psram_mem_monitor registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_psram_mem_monitor:1;
        /** read_ree1_psram_mem_monitor : R/W; bitpos: [2]; default: 0;
         *  Configures psram_mem_monitor registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_psram_mem_monitor:1;
        /** read_ree2_psram_mem_monitor : R/W; bitpos: [3]; default: 0;
         *  Configures psram_mem_monitor registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_psram_mem_monitor:1;
        /** write_tee_psram_mem_monitor : R/W; bitpos: [4]; default: 1;
         *  Configures psram_mem_monitor registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_psram_mem_monitor:1;
        /** write_ree0_psram_mem_monitor : R/W; bitpos: [5]; default: 0;
         *  Configures psram_mem_monitor registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_psram_mem_monitor:1;
        /** write_ree1_psram_mem_monitor : R/W; bitpos: [6]; default: 0;
         *  Configures psram_mem_monitor registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_psram_mem_monitor:1;
        /** write_ree2_psram_mem_monitor : R/W; bitpos: [7]; default: 0;
         *  Configures psram_mem_monitor registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_psram_mem_monitor:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_psram_mem_monitor_ctrl_reg_t;

/** Type of mem_acs_monitor_ctrl register
 *  mem_acs_monitor read/write control register
 */
typedef union {
    struct {
        /** read_tee_mem_acs_monitor : R/W; bitpos: [0]; default: 1;
         *  Configures mem_acs_monitor registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_mem_acs_monitor:1;
        /** read_ree0_mem_acs_monitor : R/W; bitpos: [1]; default: 0;
         *  Configures mem_acs_monitor registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_mem_acs_monitor:1;
        /** read_ree1_mem_acs_monitor : R/W; bitpos: [2]; default: 0;
         *  Configures mem_acs_monitor registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_mem_acs_monitor:1;
        /** read_ree2_mem_acs_monitor : R/W; bitpos: [3]; default: 0;
         *  Configures mem_acs_monitor registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_mem_acs_monitor:1;
        /** write_tee_mem_acs_monitor : R/W; bitpos: [4]; default: 1;
         *  Configures mem_acs_monitor registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_mem_acs_monitor:1;
        /** write_ree0_mem_acs_monitor : R/W; bitpos: [5]; default: 0;
         *  Configures mem_acs_monitor registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_mem_acs_monitor:1;
        /** write_ree1_mem_acs_monitor : R/W; bitpos: [6]; default: 0;
         *  Configures mem_acs_monitor registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_mem_acs_monitor:1;
        /** write_ree2_mem_acs_monitor : R/W; bitpos: [7]; default: 0;
         *  Configures mem_acs_monitor registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_mem_acs_monitor:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_mem_acs_monitor_ctrl_reg_t;

/** Type of hp_system_reg_ctrl register
 *  hp_system_reg read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_system_reg : R/W; bitpos: [0]; default: 1;
         *  Configures hp_system_reg registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_system_reg:1;
        /** read_ree0_hp_system_reg : R/W; bitpos: [1]; default: 0;
         *  Configures hp_system_reg registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_system_reg:1;
        /** read_ree1_hp_system_reg : R/W; bitpos: [2]; default: 0;
         *  Configures hp_system_reg registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_system_reg:1;
        /** read_ree2_hp_system_reg : R/W; bitpos: [3]; default: 0;
         *  Configures hp_system_reg registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_system_reg:1;
        /** write_tee_hp_system_reg : R/W; bitpos: [4]; default: 1;
         *  Configures hp_system_reg registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_system_reg:1;
        /** write_ree0_hp_system_reg : R/W; bitpos: [5]; default: 0;
         *  Configures hp_system_reg registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_system_reg:1;
        /** write_ree1_hp_system_reg : R/W; bitpos: [6]; default: 0;
         *  Configures hp_system_reg registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_system_reg:1;
        /** write_ree2_hp_system_reg : R/W; bitpos: [7]; default: 0;
         *  Configures hp_system_reg registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_system_reg:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_hp_system_reg_ctrl_reg_t;

/** Type of pcr_reg_ctrl register
 *  pcr_reg read/write control register
 */
typedef union {
    struct {
        /** read_tee_pcr_reg : R/W; bitpos: [0]; default: 1;
         *  Configures pcr_reg registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_pcr_reg:1;
        /** read_ree0_pcr_reg : R/W; bitpos: [1]; default: 0;
         *  Configures pcr_reg registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_pcr_reg:1;
        /** read_ree1_pcr_reg : R/W; bitpos: [2]; default: 0;
         *  Configures pcr_reg registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_pcr_reg:1;
        /** read_ree2_pcr_reg : R/W; bitpos: [3]; default: 0;
         *  Configures pcr_reg registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_pcr_reg:1;
        /** write_tee_pcr_reg : R/W; bitpos: [4]; default: 1;
         *  Configures pcr_reg registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_pcr_reg:1;
        /** write_ree0_pcr_reg : R/W; bitpos: [5]; default: 0;
         *  Configures pcr_reg registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_pcr_reg:1;
        /** write_ree1_pcr_reg : R/W; bitpos: [6]; default: 0;
         *  Configures pcr_reg registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_pcr_reg:1;
        /** write_ree2_pcr_reg : R/W; bitpos: [7]; default: 0;
         *  Configures pcr_reg registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_pcr_reg:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_pcr_reg_ctrl_reg_t;

/** Type of mspi_ctrl register
 *  mspi read/write control register
 */
typedef union {
    struct {
        /** read_tee_mspi : R/W; bitpos: [0]; default: 1;
         *  Configures mspi registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_mspi:1;
        /** read_ree0_mspi : R/W; bitpos: [1]; default: 0;
         *  Configures mspi registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_mspi:1;
        /** read_ree1_mspi : R/W; bitpos: [2]; default: 0;
         *  Configures mspi registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_mspi:1;
        /** read_ree2_mspi : R/W; bitpos: [3]; default: 0;
         *  Configures mspi registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_mspi:1;
        /** write_tee_mspi : R/W; bitpos: [4]; default: 1;
         *  Configures mspi registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_mspi:1;
        /** write_ree0_mspi : R/W; bitpos: [5]; default: 0;
         *  Configures mspi registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_mspi:1;
        /** write_ree1_mspi : R/W; bitpos: [6]; default: 0;
         *  Configures mspi registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_mspi:1;
        /** write_ree2_mspi : R/W; bitpos: [7]; default: 0;
         *  Configures mspi registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_mspi:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_mspi_ctrl_reg_t;

/** Type of hp_apm_ctrl register
 *  hp_apm read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_apm : R/W; bitpos: [0]; default: 1;
         *  Configures hp_apm registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_apm:1;
        /** read_ree0_hp_apm : HRO; bitpos: [1]; default: 0;
         *  Configures hp_apm registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_apm:1;
        /** read_ree1_hp_apm : HRO; bitpos: [2]; default: 0;
         *  Configures hp_apm registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_apm:1;
        /** read_ree2_hp_apm : HRO; bitpos: [3]; default: 0;
         *  Configures hp_apm registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_apm:1;
        /** write_tee_hp_apm : R/W; bitpos: [4]; default: 1;
         *  Configures hp_apm registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_apm:1;
        /** write_ree0_hp_apm : HRO; bitpos: [5]; default: 0;
         *  Configures hp_apm registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_apm:1;
        /** write_ree1_hp_apm : HRO; bitpos: [6]; default: 0;
         *  Configures hp_apm registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_apm:1;
        /** write_ree2_hp_apm : HRO; bitpos: [7]; default: 0;
         *  Configures hp_apm registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_apm:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_hp_apm_ctrl_reg_t;

/** Type of hp_mem_apm_ctrl register
 *  hp_mem_apm read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_mem_apm : R/W; bitpos: [0]; default: 1;
         *  Configures hp_mem_apm registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_mem_apm:1;
        /** read_ree0_hp_mem_apm : HRO; bitpos: [1]; default: 0;
         *  Configures hp_mem_apm registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_mem_apm:1;
        /** read_ree1_hp_mem_apm : HRO; bitpos: [2]; default: 0;
         *  Configures hp_mem_apm registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_mem_apm:1;
        /** read_ree2_hp_mem_apm : HRO; bitpos: [3]; default: 0;
         *  Configures hp_mem_apm registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_mem_apm:1;
        /** write_tee_hp_mem_apm : R/W; bitpos: [4]; default: 1;
         *  Configures hp_mem_apm registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_mem_apm:1;
        /** write_ree0_hp_mem_apm : HRO; bitpos: [5]; default: 0;
         *  Configures hp_mem_apm registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_mem_apm:1;
        /** write_ree1_hp_mem_apm : HRO; bitpos: [6]; default: 0;
         *  Configures hp_mem_apm registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_mem_apm:1;
        /** write_ree2_hp_mem_apm : HRO; bitpos: [7]; default: 0;
         *  Configures hp_mem_apm registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_mem_apm:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_hp_mem_apm_ctrl_reg_t;

/** Type of cpu_apm_ctrl register
 *  cpu_apm read/write control register
 */
typedef union {
    struct {
        /** read_tee_cpu_apm : R/W; bitpos: [0]; default: 1;
         *  Configures cpu_apm registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_cpu_apm:1;
        /** read_ree0_cpu_apm : HRO; bitpos: [1]; default: 0;
         *  Configures cpu_apm registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_cpu_apm:1;
        /** read_ree1_cpu_apm : HRO; bitpos: [2]; default: 0;
         *  Configures cpu_apm registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_cpu_apm:1;
        /** read_ree2_cpu_apm : HRO; bitpos: [3]; default: 0;
         *  Configures cpu_apm registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_cpu_apm:1;
        /** write_tee_cpu_apm : R/W; bitpos: [4]; default: 1;
         *  Configures cpu_apm registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_cpu_apm:1;
        /** write_ree0_cpu_apm : HRO; bitpos: [5]; default: 0;
         *  Configures cpu_apm registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_cpu_apm:1;
        /** write_ree1_cpu_apm : HRO; bitpos: [6]; default: 0;
         *  Configures cpu_apm registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_cpu_apm:1;
        /** write_ree2_cpu_apm : HRO; bitpos: [7]; default: 0;
         *  Configures cpu_apm registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_cpu_apm:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_cpu_apm_ctrl_reg_t;

/** Type of tee_ctrl register
 *  tee read/write control register
 */
typedef union {
    struct {
        /** read_tee_tee : R/W; bitpos: [0]; default: 1;
         *  Configures tee registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_tee:1;
        /** read_ree0_tee : HRO; bitpos: [1]; default: 0;
         *  Configures tee registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_tee:1;
        /** read_ree1_tee : HRO; bitpos: [2]; default: 0;
         *  Configures tee registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_tee:1;
        /** read_ree2_tee : HRO; bitpos: [3]; default: 0;
         *  Configures tee registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_tee:1;
        /** write_tee_tee : R/W; bitpos: [4]; default: 1;
         *  Configures tee registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_tee:1;
        /** write_ree0_tee : HRO; bitpos: [5]; default: 0;
         *  Configures tee registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_tee:1;
        /** write_ree1_tee : HRO; bitpos: [6]; default: 0;
         *  Configures tee registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_tee:1;
        /** write_ree2_tee : HRO; bitpos: [7]; default: 0;
         *  Configures tee registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_tee:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_tee_ctrl_reg_t;

/** Type of km_ctrl register
 *  crypt read/write control register
 */
typedef union {
    struct {
        /** read_tee_km : R/W; bitpos: [0]; default: 1;
         *  Configures km registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_km:1;
        /** read_ree0_km : R/W; bitpos: [1]; default: 0;
         *  Configures km registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_km:1;
        /** read_ree1_km : R/W; bitpos: [2]; default: 0;
         *  Configures km registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_km:1;
        /** read_ree2_km : R/W; bitpos: [3]; default: 0;
         *  Configures km registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_km:1;
        /** write_tee_km : R/W; bitpos: [4]; default: 1;
         *  Configures km registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_km:1;
        /** write_ree0_km : R/W; bitpos: [5]; default: 0;
         *  Configures km registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_km:1;
        /** write_ree1_km : R/W; bitpos: [6]; default: 0;
         *  Configures km registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_km:1;
        /** write_ree2_km : R/W; bitpos: [7]; default: 0;
         *  Configures km registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_km:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_km_ctrl_reg_t;

/** Type of crypt_ctrl register
 *  crypt read/write control register
 */
typedef union {
    struct {
        /** read_tee_crypt : R/W; bitpos: [0]; default: 1;
         *  Configures crypt registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_crypt:1;
        /** read_ree0_crypt : R/W; bitpos: [1]; default: 0;
         *  Configures crypt registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_crypt:1;
        /** read_ree1_crypt : R/W; bitpos: [2]; default: 0;
         *  Configures crypt registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_crypt:1;
        /** read_ree2_crypt : R/W; bitpos: [3]; default: 0;
         *  Configures crypt registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_crypt:1;
        /** write_tee_crypt : R/W; bitpos: [4]; default: 1;
         *  Configures crypt registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_crypt:1;
        /** write_ree0_crypt : R/W; bitpos: [5]; default: 0;
         *  Configures crypt registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_crypt:1;
        /** write_ree1_crypt : R/W; bitpos: [6]; default: 0;
         *  Configures crypt registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_crypt:1;
        /** write_ree2_crypt : R/W; bitpos: [7]; default: 0;
         *  Configures crypt registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_crypt:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_crypt_ctrl_reg_t;

/** Type of core0_trace_ctrl register
 *  core0_trace read/write control register
 */
typedef union {
    struct {
        /** read_tee_core0_trace : R/W; bitpos: [0]; default: 1;
         *  Configures core0_trace registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_core0_trace:1;
        /** read_ree0_core0_trace : R/W; bitpos: [1]; default: 0;
         *  Configures core0_trace registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_core0_trace:1;
        /** read_ree1_core0_trace : R/W; bitpos: [2]; default: 0;
         *  Configures core0_trace registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_core0_trace:1;
        /** read_ree2_core0_trace : R/W; bitpos: [3]; default: 0;
         *  Configures core0_trace registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_core0_trace:1;
        /** write_tee_core0_trace : R/W; bitpos: [4]; default: 1;
         *  Configures core0_trace registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_core0_trace:1;
        /** write_ree0_core0_trace : R/W; bitpos: [5]; default: 0;
         *  Configures core0_trace registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_core0_trace:1;
        /** write_ree1_core0_trace : R/W; bitpos: [6]; default: 0;
         *  Configures core0_trace registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_core0_trace:1;
        /** write_ree2_core0_trace : R/W; bitpos: [7]; default: 0;
         *  Configures core0_trace registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_core0_trace:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_core0_trace_ctrl_reg_t;

/** Type of core1_trace_ctrl register
 *  core1_trace read/write control register
 */
typedef union {
    struct {
        /** read_tee_core1_trace : R/W; bitpos: [0]; default: 1;
         *  Configures core1_trace registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_core1_trace:1;
        /** read_ree0_core1_trace : R/W; bitpos: [1]; default: 0;
         *  Configures core1_trace registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_core1_trace:1;
        /** read_ree1_core1_trace : R/W; bitpos: [2]; default: 0;
         *  Configures core1_trace registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_core1_trace:1;
        /** read_ree2_core1_trace : R/W; bitpos: [3]; default: 0;
         *  Configures core1_trace registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_core1_trace:1;
        /** write_tee_core1_trace : R/W; bitpos: [4]; default: 1;
         *  Configures core1_trace registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_core1_trace:1;
        /** write_ree0_core1_trace : R/W; bitpos: [5]; default: 0;
         *  Configures core1_trace registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_core1_trace:1;
        /** write_ree1_core1_trace : R/W; bitpos: [6]; default: 0;
         *  Configures core1_trace registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_core1_trace:1;
        /** write_ree2_core1_trace : R/W; bitpos: [7]; default: 0;
         *  Configures core1_trace registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_core1_trace:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_core1_trace_ctrl_reg_t;

/** Type of cpu_bus_monitor_ctrl register
 *  cpu_bus_monitor read/write control register
 */
typedef union {
    struct {
        /** read_tee_cpu_bus_monitor : R/W; bitpos: [0]; default: 1;
         *  Configures cpu_bus_monitor registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_cpu_bus_monitor:1;
        /** read_ree0_cpu_bus_monitor : R/W; bitpos: [1]; default: 0;
         *  Configures cpu_bus_monitor registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_cpu_bus_monitor:1;
        /** read_ree1_cpu_bus_monitor : R/W; bitpos: [2]; default: 0;
         *  Configures cpu_bus_monitor registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_cpu_bus_monitor:1;
        /** read_ree2_cpu_bus_monitor : R/W; bitpos: [3]; default: 0;
         *  Configures cpu_bus_monitor registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_cpu_bus_monitor:1;
        /** write_tee_cpu_bus_monitor : R/W; bitpos: [4]; default: 1;
         *  Configures cpu_bus_monitor registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_cpu_bus_monitor:1;
        /** write_ree0_cpu_bus_monitor : R/W; bitpos: [5]; default: 0;
         *  Configures cpu_bus_monitor registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_cpu_bus_monitor:1;
        /** write_ree1_cpu_bus_monitor : R/W; bitpos: [6]; default: 0;
         *  Configures cpu_bus_monitor registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_cpu_bus_monitor:1;
        /** write_ree2_cpu_bus_monitor : R/W; bitpos: [7]; default: 0;
         *  Configures cpu_bus_monitor registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_cpu_bus_monitor:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_cpu_bus_monitor_ctrl_reg_t;

/** Type of intpri_reg_ctrl register
 *  intpri_reg read/write control register
 */
typedef union {
    struct {
        /** read_tee_intpri_reg : R/W; bitpos: [0]; default: 1;
         *  Configures intpri_reg registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_intpri_reg:1;
        /** read_ree0_intpri_reg : R/W; bitpos: [1]; default: 0;
         *  Configures intpri_reg registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_intpri_reg:1;
        /** read_ree1_intpri_reg : R/W; bitpos: [2]; default: 0;
         *  Configures intpri_reg registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_intpri_reg:1;
        /** read_ree2_intpri_reg : R/W; bitpos: [3]; default: 0;
         *  Configures intpri_reg registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_intpri_reg:1;
        /** write_tee_intpri_reg : R/W; bitpos: [4]; default: 1;
         *  Configures intpri_reg registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_intpri_reg:1;
        /** write_ree0_intpri_reg : R/W; bitpos: [5]; default: 0;
         *  Configures intpri_reg registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_intpri_reg:1;
        /** write_ree1_intpri_reg : R/W; bitpos: [6]; default: 0;
         *  Configures intpri_reg registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_intpri_reg:1;
        /** write_ree2_intpri_reg : R/W; bitpos: [7]; default: 0;
         *  Configures intpri_reg registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_intpri_reg:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_intpri_reg_ctrl_reg_t;

/** Type of cache_cfg_ctrl register
 *  cache_cfg read/write control register
 */
typedef union {
    struct {
        /** read_tee_cache_cfg : R/W; bitpos: [0]; default: 1;
         *  Configures cache_cfg registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_cache_cfg:1;
        /** read_ree0_cache_cfg : R/W; bitpos: [1]; default: 0;
         *  Configures cache_cfg registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_cache_cfg:1;
        /** read_ree1_cache_cfg : R/W; bitpos: [2]; default: 0;
         *  Configures cache_cfg registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_cache_cfg:1;
        /** read_ree2_cache_cfg : R/W; bitpos: [3]; default: 0;
         *  Configures cache_cfg registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_cache_cfg:1;
        /** write_tee_cache_cfg : R/W; bitpos: [4]; default: 1;
         *  Configures cache_cfg registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_cache_cfg:1;
        /** write_ree0_cache_cfg : R/W; bitpos: [5]; default: 0;
         *  Configures cache_cfg registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_cache_cfg:1;
        /** write_ree1_cache_cfg : R/W; bitpos: [6]; default: 0;
         *  Configures cache_cfg registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_cache_cfg:1;
        /** write_ree2_cache_cfg : R/W; bitpos: [7]; default: 0;
         *  Configures cache_cfg registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_cache_cfg:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_cache_cfg_ctrl_reg_t;

/** Type of modem_ctrl register
 *  modem read/write control register
 */
typedef union {
    struct {
        /** read_tee_modem : R/W; bitpos: [0]; default: 1;
         *  Configures modem registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_modem:1;
        /** read_ree0_modem : R/W; bitpos: [1]; default: 0;
         *  Configures modem registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_modem:1;
        /** read_ree1_modem : R/W; bitpos: [2]; default: 0;
         *  Configures modem registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_modem:1;
        /** read_ree2_modem : R/W; bitpos: [3]; default: 0;
         *  Configures modem registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_modem:1;
        /** write_tee_modem : R/W; bitpos: [4]; default: 1;
         *  Configures modem registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_modem:1;
        /** write_ree0_modem : R/W; bitpos: [5]; default: 0;
         *  Configures modem registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_modem:1;
        /** write_ree1_modem : R/W; bitpos: [6]; default: 0;
         *  Configures modem registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_modem:1;
        /** write_ree2_modem : R/W; bitpos: [7]; default: 0;
         *  Configures modem registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_modem:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_modem_ctrl_reg_t;

/** Type of zero_det_ctrl register
 *  zero_det read/write control register
 */
typedef union {
    struct {
        /** read_tee_zero_det : R/W; bitpos: [0]; default: 1;
         *  Configures zero_det registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_zero_det:1;
        /** read_ree0_zero_det : R/W; bitpos: [1]; default: 0;
         *  Configures zero_det registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_zero_det:1;
        /** read_ree1_zero_det : R/W; bitpos: [2]; default: 0;
         *  Configures zero_det registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_zero_det:1;
        /** read_ree2_zero_det : R/W; bitpos: [3]; default: 0;
         *  Configures zero_det registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_zero_det:1;
        /** write_tee_zero_det : R/W; bitpos: [4]; default: 1;
         *  Configures zero_det registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_zero_det:1;
        /** write_ree0_zero_det : R/W; bitpos: [5]; default: 0;
         *  Configures zero_det registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_zero_det:1;
        /** write_ree1_zero_det : R/W; bitpos: [6]; default: 0;
         *  Configures zero_det registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_zero_det:1;
        /** write_ree2_zero_det : R/W; bitpos: [7]; default: 0;
         *  Configures zero_det registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_zero_det:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_zero_det_ctrl_reg_t;


/** Group: config register */
/** Type of bus_err_conf register
 *  Clock gating register
 */
typedef union {
    struct {
        /** bus_err_resp_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether return error response to cpu when access blocked
         *  0: disable error response
         *  1: enable error response
         */
        uint32_t bus_err_resp_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} tee_bus_err_conf_reg_t;


/** Group: clock gating register */
/** Type of clock_gate register
 *  Clock gating register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to keep the clock always on.
         *  0: enable automatic clock gating
         *  1: keep the clock always on
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} tee_clock_gate_reg_t;


/** Group: Version control register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 38810240;
         *  Version control register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} tee_date_reg_t;


typedef struct {
    volatile tee_mn_mode_ctrl_reg_t mn_mode_ctrl[32];
    volatile tee_gpspi0_ctrl_reg_t gpspi0_ctrl;
    volatile tee_gpspi1_ctrl_reg_t gpspi1_ctrl;
    volatile tee_uart0_ctrl_reg_t uart0_ctrl;
    volatile tee_uart1_ctrl_reg_t uart1_ctrl;
    volatile tee_uhci_ctrl_reg_t uhci_ctrl;
    volatile tee_i2c0_ctrl_reg_t i2c0_ctrl;
    volatile tee_i2c1_ctrl_reg_t i2c1_ctrl;
    volatile tee_i2s_ctrl_reg_t i2s_ctrl;
    volatile tee_parl_io_ctrl_reg_t parl_io_ctrl;
    volatile tee_pwm0_ctrl_reg_t pwm0_ctrl;
    volatile tee_pwm1_ctrl_reg_t pwm1_ctrl;
    volatile tee_ledc_ctrl_reg_t ledc_ctrl;
    volatile tee_can_ctrl_reg_t can_ctrl;
    volatile tee_usb_serial_jtag_ctrl_reg_t usb_serial_jtag_ctrl;
    volatile tee_rmt_ctrl_reg_t rmt_ctrl;
    volatile tee_gdma_ctrl_reg_t gdma_ctrl;
    volatile tee_regdma_ctrl_reg_t regdma_ctrl;
    volatile tee_etm_ctrl_reg_t etm_ctrl;
    volatile tee_intmtx_core0_ctrl_reg_t intmtx_core0_ctrl;
    volatile tee_intmtx_core1_ctrl_reg_t intmtx_core1_ctrl;
    volatile tee_apb_adc_ctrl_reg_t apb_adc_ctrl;
    volatile tee_timergroup0_ctrl_reg_t timergroup0_ctrl;
    volatile tee_timergroup1_ctrl_reg_t timergroup1_ctrl;
    volatile tee_systimer_ctrl_reg_t systimer_ctrl;
    volatile tee_misc_ctrl_reg_t misc_ctrl;
    volatile tee_src_ctrl_reg_t src_ctrl;
    volatile tee_usb_otg_fs_core_ctrl_reg_t usb_otg_fs_core_ctrl;
    volatile tee_usb_otg_fs_phy_ctrl_reg_t usb_otg_fs_phy_ctrl;
    volatile tee_pvt_monitor_ctrl_reg_t pvt_monitor_ctrl;
    volatile tee_pcnt_ctrl_reg_t pcnt_ctrl;
    volatile tee_iomux_ctrl_reg_t iomux_ctrl;
    volatile tee_psram_mem_monitor_ctrl_reg_t psram_mem_monitor_ctrl;
    volatile tee_mem_acs_monitor_ctrl_reg_t mem_acs_monitor_ctrl;
    volatile tee_hp_system_reg_ctrl_reg_t hp_system_reg_ctrl;
    volatile tee_pcr_reg_ctrl_reg_t pcr_reg_ctrl;
    volatile tee_mspi_ctrl_reg_t mspi_ctrl;
    volatile tee_hp_apm_ctrl_reg_t hp_apm_ctrl;
    volatile tee_hp_mem_apm_ctrl_reg_t hp_mem_apm_ctrl;
    volatile tee_cpu_apm_ctrl_reg_t cpu_apm_ctrl;
    volatile tee_tee_ctrl_reg_t tee_ctrl;
    volatile tee_km_ctrl_reg_t km_ctrl;
    volatile tee_crypt_ctrl_reg_t crypt_ctrl;
    volatile tee_core0_trace_ctrl_reg_t core0_trace_ctrl;
    volatile tee_core1_trace_ctrl_reg_t core1_trace_ctrl;
    volatile tee_cpu_bus_monitor_ctrl_reg_t cpu_bus_monitor_ctrl;
    volatile tee_intpri_reg_ctrl_reg_t intpri_reg_ctrl;
    volatile tee_cache_cfg_ctrl_reg_t cache_cfg_ctrl;
    volatile tee_modem_ctrl_reg_t modem_ctrl;
    volatile tee_zero_det_ctrl_reg_t zero_det_ctrl;
    uint32_t reserved_144[939];
    volatile tee_bus_err_conf_reg_t bus_err_conf;
    uint32_t reserved_ff4;
    volatile tee_clock_gate_reg_t clock_gate;
    volatile tee_date_reg_t date;
} tee_dev_t;

extern tee_dev_t TEE;

#ifndef __cplusplus
_Static_assert(sizeof(tee_dev_t) == 0x1000, "Invalid size of tee_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
