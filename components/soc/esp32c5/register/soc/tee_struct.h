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
/** Type of mn_mode_ctrl register
 *  Security mode configuration register
 */
typedef union {
    struct {
        /** mn_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures the security mode for master n.
         *  0: TEE
         *  1: REE0
         *  2: REE1
         *  3: REE2
         */
        uint32_t mn_mode:2;
        /** mn_lock : R/W; bitpos: [2]; default: 0;
         *  Configures whether to lock the value of TEE_Mn_MODE.
         *  0: Do not lock
         *  1: Lock
         */
        uint32_t mn_lock:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} tee_mn_mode_ctrl_reg_t;


/** Group: Peripheral Read/Write Control Registers */
/** Type of uart0_ctrl register
 *  UART0 read/write control register
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
 *  UART1 read/write control register
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

/** Type of uhci0_ctrl register
 *  UHCI read/write control register
 */
typedef union {
    struct {
        /** read_tee_uhci0 : R/W; bitpos: [0]; default: 1;
         *  Configures uhci0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_uhci0:1;
        /** read_ree0_uhci0 : R/W; bitpos: [1]; default: 0;
         *  Configures uhci0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_uhci0:1;
        /** read_ree1_uhci0 : R/W; bitpos: [2]; default: 0;
         *  Configures uhci0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_uhci0:1;
        /** read_ree2_uhci0 : R/W; bitpos: [3]; default: 0;
         *  Configures uhci0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_uhci0:1;
        /** write_tee_uhci0 : R/W; bitpos: [4]; default: 1;
         *  Configures uhci0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_uhci0:1;
        /** write_ree0_uhci0 : R/W; bitpos: [5]; default: 0;
         *  Configures uhci0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_uhci0:1;
        /** write_ree1_uhci0 : R/W; bitpos: [6]; default: 0;
         *  Configures uhci0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_uhci0:1;
        /** write_ree2_uhci0 : R/W; bitpos: [7]; default: 0;
         *  Configures uhci0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_uhci0:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_uhci0_ctrl_reg_t;

/** Type of i2c_ext0_ctrl register
 *  I2C read/write control register
 */
typedef union {
    struct {
        /** read_tee_i2c_ext0 : R/W; bitpos: [0]; default: 1;
         *  Configures i2c_ext0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_i2c_ext0:1;
        /** read_ree0_i2c_ext0 : R/W; bitpos: [1]; default: 0;
         *  Configures i2c_ext0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_i2c_ext0:1;
        /** read_ree1_i2c_ext0 : R/W; bitpos: [2]; default: 0;
         *  Configures i2c_ext0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_i2c_ext0:1;
        /** read_ree2_i2c_ext0 : R/W; bitpos: [3]; default: 0;
         *  Configures i2c_ext0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_i2c_ext0:1;
        /** write_tee_i2c_ext0 : R/W; bitpos: [4]; default: 1;
         *  Configures i2c_ext0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_i2c_ext0:1;
        /** write_ree0_i2c_ext0 : R/W; bitpos: [5]; default: 0;
         *  Configures i2c_ext0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_i2c_ext0:1;
        /** write_ree1_i2c_ext0 : R/W; bitpos: [6]; default: 0;
         *  Configures i2c_ext0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_i2c_ext0:1;
        /** write_ree2_i2c_ext0 : R/W; bitpos: [7]; default: 0;
         *  Configures i2c_ext0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_i2c_ext0:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_i2c_ext0_ctrl_reg_t;

/** Type of i2s_ctrl register
 *  I2S read/write control register
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
 *  PARL_IO read/write control register
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

/** Type of pwm_ctrl register
 *  MCPWM read/write control register
 */
typedef union {
    struct {
        /** read_tee_pwm : R/W; bitpos: [0]; default: 1;
         *  Configures pwm registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_pwm:1;
        /** read_ree0_pwm : R/W; bitpos: [1]; default: 0;
         *  Configures pwm registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_pwm:1;
        /** read_ree1_pwm : R/W; bitpos: [2]; default: 0;
         *  Configures pwm registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_pwm:1;
        /** read_ree2_pwm : R/W; bitpos: [3]; default: 0;
         *  Configures pwm registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_pwm:1;
        /** write_tee_pwm : R/W; bitpos: [4]; default: 1;
         *  Configures pwm registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_pwm:1;
        /** write_ree0_pwm : R/W; bitpos: [5]; default: 0;
         *  Configures pwm registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_pwm:1;
        /** write_ree1_pwm : R/W; bitpos: [6]; default: 0;
         *  Configures pwm registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_pwm:1;
        /** write_ree2_pwm : R/W; bitpos: [7]; default: 0;
         *  Configures pwm registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_pwm:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_pwm_ctrl_reg_t;

/** Type of ledc_ctrl register
 *  LEDC read/write control register
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

/** Type of twai0_ctrl register
 *  TWAI0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_twai0 : R/W; bitpos: [0]; default: 1;
         *  Configures can0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_twai0:1;
        /** read_ree0_twai0 : R/W; bitpos: [1]; default: 0;
         *  Configures can0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_twai0:1;
        /** read_ree1_twai0 : R/W; bitpos: [2]; default: 0;
         *  Configures can0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_twai0:1;
        /** read_ree2_twai0 : R/W; bitpos: [3]; default: 0;
         *  Configures can0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_twai0:1;
        /** write_tee_twai0 : R/W; bitpos: [4]; default: 1;
         *  Configures can0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_twai0:1;
        /** write_ree0_twai0 : R/W; bitpos: [5]; default: 0;
         *  Configures can0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_twai0:1;
        /** write_ree1_twai0 : R/W; bitpos: [6]; default: 0;
         *  Configures can0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_twai0:1;
        /** write_ree2_twai0 : R/W; bitpos: [7]; default: 0;
         *  Configures can0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_twai0:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_twai0_ctrl_reg_t;

/** Type of usb_serial_jtag_ctrl register
 *  USB_SERIAL_JTAG read/write control register
 */
typedef union {
    struct {
        /** read_tee_usb_serial_jtag : R/W; bitpos: [0]; default: 1;
         *  Configures usb_device registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_usb_serial_jtag:1;
        /** read_ree0_usb_serial_jtag : R/W; bitpos: [1]; default: 0;
         *  Configures usb_device registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_usb_serial_jtag:1;
        /** read_ree1_usb_serial_jtag : R/W; bitpos: [2]; default: 0;
         *  Configures usb_device registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_usb_serial_jtag:1;
        /** read_ree2_usb_serial_jtag : R/W; bitpos: [3]; default: 0;
         *  Configures usb_device registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_usb_serial_jtag:1;
        /** write_tee_usb_serial_jtag : R/W; bitpos: [4]; default: 1;
         *  Configures usb_device registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_usb_serial_jtag:1;
        /** write_ree0_usb_serial_jtag : R/W; bitpos: [5]; default: 0;
         *  Configures usb_device registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_usb_serial_jtag:1;
        /** write_ree1_usb_serial_jtag : R/W; bitpos: [6]; default: 0;
         *  Configures usb_device registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_usb_serial_jtag:1;
        /** write_ree2_usb_serial_jtag : R/W; bitpos: [7]; default: 0;
         *  Configures usb_device registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_usb_serial_jtag:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_usb_serial_jtag_ctrl_reg_t;

/** Type of rmt_ctrl register
 *  RMT read/write control register
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
 *  GDMA read/write control register
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

/** Type of etm_ctrl register
 *  SOC_ETM read/write control register
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

/** Type of intmtx_ctrl register
 *  INTMTX read/write control register
 */
typedef union {
    struct {
        /** read_tee_intmtx_core : R/W; bitpos: [0]; default: 1;
         *  Configures intmtx registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_intmtx_core:1;
        /** read_ree0_intmtx_core : R/W; bitpos: [1]; default: 0;
         *  Configures intmtx registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_intmtx_core:1;
        /** read_ree1_intmtx_core : R/W; bitpos: [2]; default: 0;
         *  Configures intmtx registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_intmtx_core:1;
        /** read_ree2_intmtx_core : R/W; bitpos: [3]; default: 0;
         *  Configures intmtx registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_intmtx_core:1;
        /** write_tee_intmtx_core : R/W; bitpos: [4]; default: 1;
         *  Configures intmtx registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_intmtx_core:1;
        /** write_ree0_intmtx_core : R/W; bitpos: [5]; default: 0;
         *  Configures intmtx registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_intmtx_core:1;
        /** write_ree1_intmtx_core : R/W; bitpos: [6]; default: 0;
         *  Configures intmtx registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_intmtx_core:1;
        /** write_ree2_intmtx_core : R/W; bitpos: [7]; default: 0;
         *  Configures intmtx registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_intmtx_core:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_intmtx_ctrl_reg_t;

/** Type of apb_adc_ctrl register
 *  SAR ADC read/write control register
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
 *  TIMG0 read/write control register
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
 *  TIMG1 read/write control register
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
 *  SYSTIMER read/write control register
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

/** Type of pcnt_ctrl register
 *  PCNT read/write control register
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
 *  IO MUX read/write control register
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
 *  PSRAM_MEM_MONITOR read/write control register
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
 *  TCM_MEM_MONITOR read/write control register
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
 *  HP_SYSREG read/write control register
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
 *  PCR read/write control register
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
 *  SPI01 read/write control register
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
 *  HP_APM and LP_APM0 read/write control register
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

/** Type of cpu_apm_ctrl register
 *  CPU_APM_REG read/write control register
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
 *  TEE read/write control register
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

/** Type of crypt_ctrl register
 *  CRYPT read/write control register, including security peripherals from AES to ECDSA
 *  address range
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

/** Type of trace_ctrl register
 *  TRACE read/write control register
 */
typedef union {
    struct {
        /** read_tee_trace : R/W; bitpos: [0]; default: 1;
         *  Configures trace registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_trace:1;
        /** read_ree0_trace : R/W; bitpos: [1]; default: 0;
         *  Configures trace registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_trace:1;
        /** read_ree1_trace : R/W; bitpos: [2]; default: 0;
         *  Configures trace registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_trace:1;
        /** read_ree2_trace : R/W; bitpos: [3]; default: 0;
         *  Configures trace registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_trace:1;
        /** write_tee_trace : R/W; bitpos: [4]; default: 1;
         *  Configures trace registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_trace:1;
        /** write_ree0_trace : R/W; bitpos: [5]; default: 0;
         *  Configures trace registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_trace:1;
        /** write_ree1_trace : R/W; bitpos: [6]; default: 0;
         *  Configures trace registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_trace:1;
        /** write_ree2_trace : R/W; bitpos: [7]; default: 0;
         *  Configures trace registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_trace:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_trace_ctrl_reg_t;

/** Type of cpu_bus_monitor_ctrl register
 *  BUS_MONITOR read/write control register
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
 *  INTPRI_REG read/write control register
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

/** Type of twai1_ctrl register
 *  TWAI1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_twai1 : R/W; bitpos: [0]; default: 1;
         *  Configures can1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_twai1:1;
        /** read_ree0_twai1 : R/W; bitpos: [1]; default: 0;
         *  Configures can1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_twai1:1;
        /** read_ree1_twai1 : R/W; bitpos: [2]; default: 0;
         *  Configures can1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_twai1:1;
        /** read_ree2_twai1 : R/W; bitpos: [3]; default: 0;
         *  Configures can1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_twai1:1;
        /** write_tee_twai1 : R/W; bitpos: [4]; default: 1;
         *  Configures can1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_twai1:1;
        /** write_ree0_twai1 : R/W; bitpos: [5]; default: 0;
         *  Configures can1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_twai1:1;
        /** write_ree1_twai1 : R/W; bitpos: [6]; default: 0;
         *  Configures can1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_twai1:1;
        /** write_ree2_twai1 : R/W; bitpos: [7]; default: 0;
         *  Configures can1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_twai1:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_twai1_ctrl_reg_t;

/** Type of spi2_ctrl register
 *  SPI2 read/write control register
 */
typedef union {
    struct {
        /** read_tee_spi2 : R/W; bitpos: [0]; default: 1;
         *  Configures spi2 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_spi2:1;
        /** read_ree0_spi2 : R/W; bitpos: [1]; default: 0;
         *  Configures spi2 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_spi2:1;
        /** read_ree1_spi2 : R/W; bitpos: [2]; default: 0;
         *  Configures spi2 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_spi2:1;
        /** read_ree2_spi2 : R/W; bitpos: [3]; default: 0;
         *  Configures spi2 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_spi2:1;
        /** write_tee_spi2 : R/W; bitpos: [4]; default: 1;
         *  Configures spi2 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_spi2:1;
        /** write_ree0_spi2 : R/W; bitpos: [5]; default: 0;
         *  Configures spi2 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_spi2:1;
        /** write_ree1_spi2 : R/W; bitpos: [6]; default: 0;
         *  Configures spi2 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_spi2:1;
        /** write_ree2_spi2 : R/W; bitpos: [7]; default: 0;
         *  Configures spi2 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_spi2:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_spi2_ctrl_reg_t;

/** Type of bs_ctrl register
 *  BITSCRAMBLER read/write control register
 */
typedef union {
    struct {
        /** read_tee_bs : R/W; bitpos: [0]; default: 1;
         *  Configures bs registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_bs:1;
        /** read_ree0_bs : R/W; bitpos: [1]; default: 0;
         *  Configures bs registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_bs:1;
        /** read_ree1_bs : R/W; bitpos: [2]; default: 0;
         *  Configures bs registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_bs:1;
        /** read_ree2_bs : R/W; bitpos: [3]; default: 0;
         *  Configures bs registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_bs:1;
        /** write_tee_bs : R/W; bitpos: [4]; default: 1;
         *  Configures bs registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_bs:1;
        /** write_ree0_bs : R/W; bitpos: [5]; default: 0;
         *  Configures bs registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_bs:1;
        /** write_ree1_bs : R/W; bitpos: [6]; default: 0;
         *  Configures bs registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_bs:1;
        /** write_ree2_bs : R/W; bitpos: [7]; default: 0;
         *  Configures bs registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_bs:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} tee_bs_ctrl_reg_t;

/** Type of bus_err_conf register
 *  Error message return configuration register
 */
typedef union {
    struct {
        /** bus_err_resp_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether to return error message to CPU when access is blocked.
         *  0: Disable
         *  1: Enable
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
         *  0: Enable automatic clock gating
         *  1: Keep the clock always on
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} tee_clock_gate_reg_t;


/** Group: Version Control Registers */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 37773824;
         *  Version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} tee_date_reg_t;


typedef struct {
    volatile tee_mn_mode_ctrl_reg_t mn_mode_ctrl[32];
    uint32_t reserved_080[2];
    volatile tee_uart0_ctrl_reg_t uart0_ctrl;
    volatile tee_uart1_ctrl_reg_t uart1_ctrl;
    volatile tee_uhci0_ctrl_reg_t uhci0_ctrl;
    volatile tee_i2c_ext0_ctrl_reg_t i2c_ext0_ctrl;
    uint32_t reserved_098;
    volatile tee_i2s_ctrl_reg_t i2s_ctrl;
    volatile tee_parl_io_ctrl_reg_t parl_io_ctrl;
    volatile tee_pwm_ctrl_reg_t pwm_ctrl;
    uint32_t reserved_0a8;
    volatile tee_ledc_ctrl_reg_t ledc_ctrl;
    volatile tee_twai0_ctrl_reg_t twai0_ctrl;
    volatile tee_usb_serial_jtag_ctrl_reg_t usb_serial_jtag_ctrl;
    volatile tee_rmt_ctrl_reg_t rmt_ctrl;
    volatile tee_gdma_ctrl_reg_t gdma_ctrl;
    uint32_t reserved_0c0;
    volatile tee_etm_ctrl_reg_t etm_ctrl;
    volatile tee_intmtx_ctrl_reg_t intmtx_ctrl;
    uint32_t reserved_0cc;
    volatile tee_apb_adc_ctrl_reg_t apb_adc_ctrl;
    volatile tee_timergroup0_ctrl_reg_t timergroup0_ctrl;
    volatile tee_timergroup1_ctrl_reg_t timergroup1_ctrl;
    volatile tee_systimer_ctrl_reg_t systimer_ctrl;
    uint32_t reserved_0e0[5];
    volatile tee_pcnt_ctrl_reg_t pcnt_ctrl;
    volatile tee_iomux_ctrl_reg_t iomux_ctrl;
    volatile tee_psram_mem_monitor_ctrl_reg_t psram_mem_monitor_ctrl;
    volatile tee_mem_acs_monitor_ctrl_reg_t mem_acs_monitor_ctrl;
    volatile tee_hp_system_reg_ctrl_reg_t hp_system_reg_ctrl;
    volatile tee_pcr_reg_ctrl_reg_t pcr_reg_ctrl;
    volatile tee_mspi_ctrl_reg_t mspi_ctrl;
    volatile tee_hp_apm_ctrl_reg_t hp_apm_ctrl;
    volatile tee_cpu_apm_ctrl_reg_t cpu_apm_ctrl;
    volatile tee_tee_ctrl_reg_t tee_ctrl;
    volatile tee_crypt_ctrl_reg_t crypt_ctrl;
    volatile tee_trace_ctrl_reg_t trace_ctrl;
    uint32_t reserved_124;
    volatile tee_cpu_bus_monitor_ctrl_reg_t cpu_bus_monitor_ctrl;
    volatile tee_intpri_reg_ctrl_reg_t intpri_reg_ctrl;
    uint32_t reserved_130[2];
    volatile tee_twai1_ctrl_reg_t twai1_ctrl;
    volatile tee_spi2_ctrl_reg_t spi2_ctrl;
    volatile tee_bs_ctrl_reg_t bs_ctrl;
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
