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

/** Group: Configuration Registers */
/** Type of sys_icm_intr_map register
 *  SYS_ICM_INTR mapping register
 */
typedef union {
    struct {
        /** sys_icm_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t sys_icm_intr_map:6;
        uint32_t reserved_6:2;
        /** sys_icm_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t sys_icm_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_sys_icm_intr_map_reg_t;

/** Type of axi_perf_mon_intr_map register
 *  AXI_PERF_MON_INTR mapping register
 */
typedef union {
    struct {
        /** axi_perf_mon_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t axi_perf_mon_intr_map:6;
        uint32_t reserved_6:2;
        /** axi_perf_mon_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t axi_perf_mon_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_axi_perf_mon_intr_map_reg_t;

/** Type of usb_device_intr_map register
 *  USB_DEVICE_INTR mapping register
 */
typedef union {
    struct {
        /** usb_device_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t usb_device_intr_map:6;
        uint32_t reserved_6:2;
        /** usb_device_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t usb_device_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_usb_device_intr_map_reg_t;

/** Type of sdio_host_intr_map register
 *  SDIO_HOST_INTR mapping register
 */
typedef union {
    struct {
        /** sdio_host_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t sdio_host_intr_map:6;
        uint32_t reserved_6:2;
        /** sdio_host_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t sdio_host_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_sdio_host_intr_map_reg_t;

/** Type of spi2_intr_map register
 *  SPI2_INTR mapping register
 */
typedef union {
    struct {
        /** spi2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t spi2_intr_map:6;
        uint32_t reserved_6:2;
        /** spi2_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t spi2_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_spi2_intr_map_reg_t;

/** Type of spi3_intr_map register
 *  SPI3_INTR mapping register
 */
typedef union {
    struct {
        /** spi3_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t spi3_intr_map:6;
        uint32_t reserved_6:2;
        /** spi3_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t spi3_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_spi3_intr_map_reg_t;

/** Type of i2s0_intr_map register
 *  I2S0_INTR mapping register
 */
typedef union {
    struct {
        /** i2s0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t i2s0_intr_map:6;
        uint32_t reserved_6:2;
        /** i2s0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t i2s0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_i2s0_intr_map_reg_t;

/** Type of i2s1_intr_map register
 *  I2S1_INTR mapping register
 */
typedef union {
    struct {
        /** i2s1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t i2s1_intr_map:6;
        uint32_t reserved_6:2;
        /** i2s1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t i2s1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_i2s1_intr_map_reg_t;

/** Type of uhci0_intr_map register
 *  UHCI0_INTR mapping register
 */
typedef union {
    struct {
        /** uhci0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t uhci0_intr_map:6;
        uint32_t reserved_6:2;
        /** uhci0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t uhci0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_uhci0_intr_map_reg_t;

/** Type of uart0_intr_map register
 *  UART0_INTR mapping register
 */
typedef union {
    struct {
        /** uart0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t uart0_intr_map:6;
        uint32_t reserved_6:2;
        /** uart0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t uart0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_uart0_intr_map_reg_t;

/** Type of uart1_intr_map register
 *  UART1_INTR mapping register
 */
typedef union {
    struct {
        /** uart1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t uart1_intr_map:6;
        uint32_t reserved_6:2;
        /** uart1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t uart1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_uart1_intr_map_reg_t;

/** Type of uart2_intr_map register
 *  UART2_INTR mapping register
 */
typedef union {
    struct {
        /** uart2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t uart2_intr_map:6;
        uint32_t reserved_6:2;
        /** uart2_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t uart2_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_uart2_intr_map_reg_t;

/** Type of uart3_intr_map register
 *  UART3_INTR mapping register
 */
typedef union {
    struct {
        /** uart3_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t uart3_intr_map:6;
        uint32_t reserved_6:2;
        /** uart3_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t uart3_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_uart3_intr_map_reg_t;

/** Type of lcd_cam_intr_map register
 *  LCD_CAM_INTR mapping register
 */
typedef union {
    struct {
        /** lcd_cam_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lcd_cam_intr_map:6;
        uint32_t reserved_6:2;
        /** lcd_cam_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lcd_cam_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lcd_cam_intr_map_reg_t;

/** Type of pwm0_intr_map register
 *  PWM0_INTR mapping register
 */
typedef union {
    struct {
        /** pwm0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t pwm0_intr_map:6;
        uint32_t reserved_6:2;
        /** pwm0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t pwm0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_pwm0_intr_map_reg_t;

/** Type of pwm1_intr_map register
 *  PWM1_INTR mapping register
 */
typedef union {
    struct {
        /** pwm1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t pwm1_intr_map:6;
        uint32_t reserved_6:2;
        /** pwm1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t pwm1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_pwm1_intr_map_reg_t;

/** Type of pwm2_intr_map register
 *  PWM2_INTR mapping register
 */
typedef union {
    struct {
        /** pwm2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t pwm2_intr_map:6;
        uint32_t reserved_6:2;
        /** pwm2_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t pwm2_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_pwm2_intr_map_reg_t;

/** Type of pwm3_intr_map register
 *  PWM3_INTR mapping register
 */
typedef union {
    struct {
        /** pwm3_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t pwm3_intr_map:6;
        uint32_t reserved_6:2;
        /** pwm3_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t pwm3_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_pwm3_intr_map_reg_t;

/** Type of can0_intr_map register
 *  CAN0_INTR mapping register
 */
typedef union {
    struct {
        /** can0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t can0_intr_map:6;
        uint32_t reserved_6:2;
        /** can0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t can0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_can0_intr_map_reg_t;

/** Type of can0_timer_intr_map register
 *  CAN0_TIMER_INTR mapping register
 */
typedef union {
    struct {
        /** can0_timer_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t can0_timer_intr_map:6;
        uint32_t reserved_6:2;
        /** can0_timer_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t can0_timer_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_can0_timer_intr_map_reg_t;

/** Type of can1_intr_map register
 *  CAN1_INTR mapping register
 */
typedef union {
    struct {
        /** can1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t can1_intr_map:6;
        uint32_t reserved_6:2;
        /** can1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t can1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_can1_intr_map_reg_t;

/** Type of can1_timer_intr_map register
 *  CAN1_TIMER_INTR mapping register
 */
typedef union {
    struct {
        /** can1_timer_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t can1_timer_intr_map:6;
        uint32_t reserved_6:2;
        /** can1_timer_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t can1_timer_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_can1_timer_intr_map_reg_t;

/** Type of rmt_intr_map register
 *  RMT_INTR mapping register
 */
typedef union {
    struct {
        /** rmt_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t rmt_intr_map:6;
        uint32_t reserved_6:2;
        /** rmt_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t rmt_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_rmt_intr_map_reg_t;

/** Type of i2c0_intr_map register
 *  I2C0_INTR mapping register
 */
typedef union {
    struct {
        /** i2c0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t i2c0_intr_map:6;
        uint32_t reserved_6:2;
        /** i2c0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t i2c0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_i2c0_intr_map_reg_t;

/** Type of i2c1_intr_map register
 *  I2C1_INTR mapping register
 */
typedef union {
    struct {
        /** i2c1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t i2c1_intr_map:6;
        uint32_t reserved_6:2;
        /** i2c1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t i2c1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_i2c1_intr_map_reg_t;

/** Type of timergrp0_t0_intr_map register
 *  TIMERGRP0_T0_INTR mapping register
 */
typedef union {
    struct {
        /** timergrp0_t0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t timergrp0_t0_intr_map:6;
        uint32_t reserved_6:2;
        /** timergrp0_t0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t timergrp0_t0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_timergrp0_t0_intr_map_reg_t;

/** Type of timergrp0_t1_intr_map register
 *  TIMERGRP0_T1_INTR mapping register
 */
typedef union {
    struct {
        /** timergrp0_t1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t timergrp0_t1_intr_map:6;
        uint32_t reserved_6:2;
        /** timergrp0_t1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t timergrp0_t1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_timergrp0_t1_intr_map_reg_t;

/** Type of timergrp0_wdt_intr_map register
 *  TIMERGRP0_WDT_INTR mapping register
 */
typedef union {
    struct {
        /** timergrp0_wdt_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t timergrp0_wdt_intr_map:6;
        uint32_t reserved_6:2;
        /** timergrp0_wdt_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t timergrp0_wdt_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_timergrp0_wdt_intr_map_reg_t;

/** Type of timergrp1_t0_intr_map register
 *  TIMERGRP1_T0_INTR mapping register
 */
typedef union {
    struct {
        /** timergrp1_t0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t timergrp1_t0_intr_map:6;
        uint32_t reserved_6:2;
        /** timergrp1_t0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t timergrp1_t0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_timergrp1_t0_intr_map_reg_t;

/** Type of timergrp1_t1_intr_map register
 *  TIMERGRP1_T1_INTR mapping register
 */
typedef union {
    struct {
        /** timergrp1_t1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t timergrp1_t1_intr_map:6;
        uint32_t reserved_6:2;
        /** timergrp1_t1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t timergrp1_t1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_timergrp1_t1_intr_map_reg_t;

/** Type of timergrp1_wdt_intr_map register
 *  TIMERGRP1_WDT_INTR mapping register
 */
typedef union {
    struct {
        /** timergrp1_wdt_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t timergrp1_wdt_intr_map:6;
        uint32_t reserved_6:2;
        /** timergrp1_wdt_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t timergrp1_wdt_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_timergrp1_wdt_intr_map_reg_t;

/** Type of ledc0_intr_map register
 *  LEDC0_INTR mapping register
 */
typedef union {
    struct {
        /** ledc0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ledc0_intr_map:6;
        uint32_t reserved_6:2;
        /** ledc0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t ledc0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_ledc0_intr_map_reg_t;

/** Type of ledc1_intr_map register
 *  LEDC1_INTR mapping register
 */
typedef union {
    struct {
        /** ledc1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ledc1_intr_map:6;
        uint32_t reserved_6:2;
        /** ledc1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t ledc1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_ledc1_intr_map_reg_t;

/** Type of systimer_target0_intr_map register
 *  SYSTIMER_TARGET0_INTR mapping register
 */
typedef union {
    struct {
        /** systimer_target0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t systimer_target0_intr_map:6;
        uint32_t reserved_6:2;
        /** systimer_target0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t systimer_target0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_systimer_target0_intr_map_reg_t;

/** Type of systimer_target1_intr_map register
 *  SYSTIMER_TARGET1_INTR mapping register
 */
typedef union {
    struct {
        /** systimer_target1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t systimer_target1_intr_map:6;
        uint32_t reserved_6:2;
        /** systimer_target1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t systimer_target1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_systimer_target1_intr_map_reg_t;

/** Type of systimer_target2_intr_map register
 *  SYSTIMER_TARGET2_INTR mapping register
 */
typedef union {
    struct {
        /** systimer_target2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t systimer_target2_intr_map:6;
        uint32_t reserved_6:2;
        /** systimer_target2_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t systimer_target2_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_systimer_target2_intr_map_reg_t;

/** Type of ahb_pdma_in_ch0_intr_map register
 *  AHB_PDMA_IN_CH0_INTR mapping register
 */
typedef union {
    struct {
        /** ahb_pdma_in_ch0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ahb_pdma_in_ch0_intr_map:6;
        uint32_t reserved_6:2;
        /** ahb_pdma_in_ch0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t ahb_pdma_in_ch0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_in_ch0_intr_map_reg_t;

/** Type of ahb_pdma_in_ch1_intr_map register
 *  AHB_PDMA_IN_CH1_INTR mapping register
 */
typedef union {
    struct {
        /** ahb_pdma_in_ch1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ahb_pdma_in_ch1_intr_map:6;
        uint32_t reserved_6:2;
        /** ahb_pdma_in_ch1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t ahb_pdma_in_ch1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_in_ch1_intr_map_reg_t;

/** Type of ahb_pdma_in_ch2_intr_map register
 *  AHB_PDMA_IN_CH2_INTR mapping register
 */
typedef union {
    struct {
        /** ahb_pdma_in_ch2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ahb_pdma_in_ch2_intr_map:6;
        uint32_t reserved_6:2;
        /** ahb_pdma_in_ch2_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t ahb_pdma_in_ch2_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_in_ch2_intr_map_reg_t;

/** Type of ahb_pdma_in_ch3_intr_map register
 *  AHB_PDMA_IN_CH3_INTR mapping register
 */
typedef union {
    struct {
        /** ahb_pdma_in_ch3_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ahb_pdma_in_ch3_intr_map:6;
        uint32_t reserved_6:2;
        /** ahb_pdma_in_ch3_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t ahb_pdma_in_ch3_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_in_ch3_intr_map_reg_t;

/** Type of ahb_pdma_in_ch4_intr_map register
 *  AHB_PDMA_IN_CH4_INTR mapping register
 */
typedef union {
    struct {
        /** ahb_pdma_in_ch4_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ahb_pdma_in_ch4_intr_map:6;
        uint32_t reserved_6:2;
        /** ahb_pdma_in_ch4_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t ahb_pdma_in_ch4_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_in_ch4_intr_map_reg_t;

/** Type of ahb_pdma_out_ch0_intr_map register
 *  AHB_PDMA_OUT_CH0_INTR mapping register
 */
typedef union {
    struct {
        /** ahb_pdma_out_ch0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ahb_pdma_out_ch0_intr_map:6;
        uint32_t reserved_6:2;
        /** ahb_pdma_out_ch0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t ahb_pdma_out_ch0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_out_ch0_intr_map_reg_t;

/** Type of ahb_pdma_out_ch1_intr_map register
 *  AHB_PDMA_OUT_CH1_INTR mapping register
 */
typedef union {
    struct {
        /** ahb_pdma_out_ch1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ahb_pdma_out_ch1_intr_map:6;
        uint32_t reserved_6:2;
        /** ahb_pdma_out_ch1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t ahb_pdma_out_ch1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_out_ch1_intr_map_reg_t;

/** Type of ahb_pdma_out_ch2_intr_map register
 *  AHB_PDMA_OUT_CH2_INTR mapping register
 */
typedef union {
    struct {
        /** ahb_pdma_out_ch2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ahb_pdma_out_ch2_intr_map:6;
        uint32_t reserved_6:2;
        /** ahb_pdma_out_ch2_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t ahb_pdma_out_ch2_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_out_ch2_intr_map_reg_t;

/** Type of ahb_pdma_out_ch3_intr_map register
 *  AHB_PDMA_OUT_CH3_INTR mapping register
 */
typedef union {
    struct {
        /** ahb_pdma_out_ch3_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ahb_pdma_out_ch3_intr_map:6;
        uint32_t reserved_6:2;
        /** ahb_pdma_out_ch3_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t ahb_pdma_out_ch3_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_out_ch3_intr_map_reg_t;

/** Type of ahb_pdma_out_ch4_intr_map register
 *  AHB_PDMA_OUT_CH4_INTR mapping register
 */
typedef union {
    struct {
        /** ahb_pdma_out_ch4_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ahb_pdma_out_ch4_intr_map:6;
        uint32_t reserved_6:2;
        /** ahb_pdma_out_ch4_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t ahb_pdma_out_ch4_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_out_ch4_intr_map_reg_t;

/** Type of asrc_chnl0_intr_map register
 *  ASRC_CHNL0_INTR mapping register
 */
typedef union {
    struct {
        /** asrc_chnl0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t asrc_chnl0_intr_map:6;
        uint32_t reserved_6:2;
        /** asrc_chnl0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t asrc_chnl0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_asrc_chnl0_intr_map_reg_t;

/** Type of asrc_chnl1_intr_map register
 *  ASRC_CHNL1_INTR mapping register
 */
typedef union {
    struct {
        /** asrc_chnl1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t asrc_chnl1_intr_map:6;
        uint32_t reserved_6:2;
        /** asrc_chnl1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t asrc_chnl1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_asrc_chnl1_intr_map_reg_t;

/** Type of axi_pdma_in_ch0_intr_map register
 *  AXI_PDMA_IN_CH0_INTR mapping register
 */
typedef union {
    struct {
        /** axi_pdma_in_ch0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t axi_pdma_in_ch0_intr_map:6;
        uint32_t reserved_6:2;
        /** axi_pdma_in_ch0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t axi_pdma_in_ch0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_axi_pdma_in_ch0_intr_map_reg_t;

/** Type of axi_pdma_in_ch1_intr_map register
 *  AXI_PDMA_IN_CH1_INTR mapping register
 */
typedef union {
    struct {
        /** axi_pdma_in_ch1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t axi_pdma_in_ch1_intr_map:6;
        uint32_t reserved_6:2;
        /** axi_pdma_in_ch1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t axi_pdma_in_ch1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_axi_pdma_in_ch1_intr_map_reg_t;

/** Type of axi_pdma_in_ch2_intr_map register
 *  AXI_PDMA_IN_CH2_INTR mapping register
 */
typedef union {
    struct {
        /** axi_pdma_in_ch2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t axi_pdma_in_ch2_intr_map:6;
        uint32_t reserved_6:2;
        /** axi_pdma_in_ch2_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t axi_pdma_in_ch2_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_axi_pdma_in_ch2_intr_map_reg_t;

/** Type of axi_pdma_out_ch0_intr_map register
 *  AXI_PDMA_OUT_CH0_INTR mapping register
 */
typedef union {
    struct {
        /** axi_pdma_out_ch0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t axi_pdma_out_ch0_intr_map:6;
        uint32_t reserved_6:2;
        /** axi_pdma_out_ch0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t axi_pdma_out_ch0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_axi_pdma_out_ch0_intr_map_reg_t;

/** Type of axi_pdma_out_ch1_intr_map register
 *  AXI_PDMA_OUT_CH1_INTR mapping register
 */
typedef union {
    struct {
        /** axi_pdma_out_ch1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t axi_pdma_out_ch1_intr_map:6;
        uint32_t reserved_6:2;
        /** axi_pdma_out_ch1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t axi_pdma_out_ch1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_axi_pdma_out_ch1_intr_map_reg_t;

/** Type of axi_pdma_out_ch2_intr_map register
 *  AXI_PDMA_OUT_CH2_INTR mapping register
 */
typedef union {
    struct {
        /** axi_pdma_out_ch2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t axi_pdma_out_ch2_intr_map:6;
        uint32_t reserved_6:2;
        /** axi_pdma_out_ch2_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t axi_pdma_out_ch2_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_axi_pdma_out_ch2_intr_map_reg_t;

/** Type of rsa_intr_map register
 *  RSA_INTR mapping register
 */
typedef union {
    struct {
        /** rsa_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t rsa_intr_map:6;
        uint32_t reserved_6:2;
        /** rsa_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t rsa_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_rsa_intr_map_reg_t;

/** Type of aes_intr_map register
 *  AES_INTR mapping register
 */
typedef union {
    struct {
        /** aes_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t aes_intr_map:6;
        uint32_t reserved_6:2;
        /** aes_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t aes_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_aes_intr_map_reg_t;

/** Type of sha_intr_map register
 *  SHA_INTR mapping register
 */
typedef union {
    struct {
        /** sha_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t sha_intr_map:6;
        uint32_t reserved_6:2;
        /** sha_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t sha_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_sha_intr_map_reg_t;

/** Type of ecc_intr_map register
 *  ECC_INTR mapping register
 */
typedef union {
    struct {
        /** ecc_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ecc_intr_map:6;
        uint32_t reserved_6:2;
        /** ecc_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t ecc_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_ecc_intr_map_reg_t;

/** Type of ecdsa_intr_map register
 *  ECDSA_INTR mapping register
 */
typedef union {
    struct {
        /** ecdsa_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ecdsa_intr_map:6;
        uint32_t reserved_6:2;
        /** ecdsa_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t ecdsa_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_ecdsa_intr_map_reg_t;

/** Type of km_intr_map register
 *  KM_INTR mapping register
 */
typedef union {
    struct {
        /** km_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t km_intr_map:6;
        uint32_t reserved_6:2;
        /** km_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t km_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_km_intr_map_reg_t;

/** Type of rma_intr_map register
 *  RMA_INTR mapping register
 */
typedef union {
    struct {
        /** rma_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t rma_intr_map:6;
        uint32_t reserved_6:2;
        /** rma_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t rma_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_rma_intr_map_reg_t;

/** Type of gpio_intr0_map register
 *  GPIO_INTR0 mapping register
 */
typedef union {
    struct {
        /** gpio_intr0_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t gpio_intr0_map:6;
        uint32_t reserved_6:2;
        /** gpio_intr0_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t gpio_intr0_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_gpio_intr0_map_reg_t;

/** Type of gpio_intr1_map register
 *  GPIO_INTR1 mapping register
 */
typedef union {
    struct {
        /** gpio_intr1_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t gpio_intr1_map:6;
        uint32_t reserved_6:2;
        /** gpio_intr1_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t gpio_intr1_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_gpio_intr1_map_reg_t;

/** Type of gpio_intr2_map register
 *  GPIO_INTR2 mapping register
 */
typedef union {
    struct {
        /** gpio_intr2_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t gpio_intr2_map:6;
        uint32_t reserved_6:2;
        /** gpio_intr2_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t gpio_intr2_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_gpio_intr2_map_reg_t;

/** Type of gpio_intr3_map register
 *  GPIO_INTR3 mapping register
 */
typedef union {
    struct {
        /** gpio_intr3_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t gpio_intr3_map:6;
        uint32_t reserved_6:2;
        /** gpio_intr3_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t gpio_intr3_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_gpio_intr3_map_reg_t;

/** Type of cpu_intr_from_cpu_0_map register
 *  CPU_INTR_FROM_CPU_0 mapping register
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_0_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu_intr_from_cpu_0_map:6;
        uint32_t reserved_6:2;
        /** cpu_intr_from_cpu_0_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t cpu_intr_from_cpu_0_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_cpu_intr_from_cpu_0_map_reg_t;

/** Type of cpu_intr_from_cpu_1_map register
 *  CPU_INTR_FROM_CPU_1 mapping register
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_1_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu_intr_from_cpu_1_map:6;
        uint32_t reserved_6:2;
        /** cpu_intr_from_cpu_1_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t cpu_intr_from_cpu_1_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_cpu_intr_from_cpu_1_map_reg_t;

/** Type of cpu_intr_from_cpu_2_map register
 *  CPU_INTR_FROM_CPU_2 mapping register
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_2_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu_intr_from_cpu_2_map:6;
        uint32_t reserved_6:2;
        /** cpu_intr_from_cpu_2_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t cpu_intr_from_cpu_2_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_cpu_intr_from_cpu_2_map_reg_t;

/** Type of cpu_intr_from_cpu_3_map register
 *  CPU_INTR_FROM_CPU_3 mapping register
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_3_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu_intr_from_cpu_3_map:6;
        uint32_t reserved_6:2;
        /** cpu_intr_from_cpu_3_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t cpu_intr_from_cpu_3_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_cpu_intr_from_cpu_3_map_reg_t;

/** Type of cache_intr_map register
 *  CACHE_INTR mapping register
 */
typedef union {
    struct {
        /** cache_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cache_intr_map:6;
        uint32_t reserved_6:2;
        /** cache_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t cache_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_cache_intr_map_reg_t;

/** Type of cpu_apm_m0_intr_map register
 *  CPU_APM_M0_INTR mapping register
 */
typedef union {
    struct {
        /** cpu_apm_m0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu_apm_m0_intr_map:6;
        uint32_t reserved_6:2;
        /** cpu_apm_m0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t cpu_apm_m0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_cpu_apm_m0_intr_map_reg_t;

/** Type of cpu_apm_m1_intr_map register
 *  CPU_APM_M1_INTR mapping register
 */
typedef union {
    struct {
        /** cpu_apm_m1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu_apm_m1_intr_map:6;
        uint32_t reserved_6:2;
        /** cpu_apm_m1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t cpu_apm_m1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_cpu_apm_m1_intr_map_reg_t;

/** Type of cpu_apm_m2_intr_map register
 *  CPU_APM_M2_INTR mapping register
 */
typedef union {
    struct {
        /** cpu_apm_m2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu_apm_m2_intr_map:6;
        uint32_t reserved_6:2;
        /** cpu_apm_m2_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t cpu_apm_m2_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_cpu_apm_m2_intr_map_reg_t;

/** Type of cpu_apm_m3_intr_map register
 *  CPU_APM_M3_INTR mapping register
 */
typedef union {
    struct {
        /** cpu_apm_m3_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu_apm_m3_intr_map:6;
        uint32_t reserved_6:2;
        /** cpu_apm_m3_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t cpu_apm_m3_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_cpu_apm_m3_intr_map_reg_t;

/** Type of hp_mem_apm_m0_intr_map register
 *  HP_MEM_APM_M0_INTR mapping register
 */
typedef union {
    struct {
        /** hp_mem_apm_m0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_mem_apm_m0_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_mem_apm_m0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_mem_apm_m0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_mem_apm_m0_intr_map_reg_t;

/** Type of hp_mem_apm_m1_intr_map register
 *  HP_MEM_APM_M1_INTR mapping register
 */
typedef union {
    struct {
        /** hp_mem_apm_m1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_mem_apm_m1_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_mem_apm_m1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_mem_apm_m1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_mem_apm_m1_intr_map_reg_t;

/** Type of hp_mem_apm_m2_intr_map register
 *  HP_MEM_APM_M2_INTR mapping register
 */
typedef union {
    struct {
        /** hp_mem_apm_m2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_mem_apm_m2_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_mem_apm_m2_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_mem_apm_m2_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_mem_apm_m2_intr_map_reg_t;

/** Type of hp_mem_apm_m3_intr_map register
 *  HP_MEM_APM_M3_INTR mapping register
 */
typedef union {
    struct {
        /** hp_mem_apm_m3_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_mem_apm_m3_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_mem_apm_m3_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_mem_apm_m3_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_mem_apm_m3_intr_map_reg_t;

/** Type of hp_mem_apm_m4_intr_map register
 *  HP_MEM_APM_M4_INTR mapping register
 */
typedef union {
    struct {
        /** hp_mem_apm_m4_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_mem_apm_m4_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_mem_apm_m4_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_mem_apm_m4_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_mem_apm_m4_intr_map_reg_t;

/** Type of hp_mem_apm_m5_intr_map register
 *  HP_MEM_APM_M5_INTR mapping register
 */
typedef union {
    struct {
        /** hp_mem_apm_m5_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_mem_apm_m5_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_mem_apm_m5_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_mem_apm_m5_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_mem_apm_m5_intr_map_reg_t;

/** Type of cpu_peri0_timeout_intr_map register
 *  CPU_PERI0_TIMEOUT_INTR mapping register
 */
typedef union {
    struct {
        /** cpu_peri0_timeout_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu_peri0_timeout_intr_map:6;
        uint32_t reserved_6:2;
        /** cpu_peri0_timeout_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t cpu_peri0_timeout_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_cpu_peri0_timeout_intr_map_reg_t;

/** Type of cpu_peri1_timeout_intr_map register
 *  CPU_PERI1_TIMEOUT_INTR mapping register
 */
typedef union {
    struct {
        /** cpu_peri1_timeout_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu_peri1_timeout_intr_map:6;
        uint32_t reserved_6:2;
        /** cpu_peri1_timeout_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t cpu_peri1_timeout_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_cpu_peri1_timeout_intr_map_reg_t;

/** Type of hp_peri0_timeout_intr_map register
 *  HP_PERI0_TIMEOUT_INTR mapping register
 */
typedef union {
    struct {
        /** hp_peri0_timeout_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_peri0_timeout_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_peri0_timeout_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_peri0_timeout_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_peri0_timeout_intr_map_reg_t;

/** Type of hp_peri1_timeout_intr_map register
 *  HP_PERI1_TIMEOUT_INTR mapping register
 */
typedef union {
    struct {
        /** hp_peri1_timeout_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_peri1_timeout_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_peri1_timeout_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_peri1_timeout_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_peri1_timeout_intr_map_reg_t;

/** Type of hp_apm_m0_intr_map register
 *  HP_APM_M0_INTR mapping register
 */
typedef union {
    struct {
        /** hp_apm_m0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_apm_m0_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_apm_m0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_apm_m0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_apm_m0_intr_map_reg_t;

/** Type of hp_apm_m1_intr_map register
 *  HP_APM_M1_INTR mapping register
 */
typedef union {
    struct {
        /** hp_apm_m1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_apm_m1_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_apm_m1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_apm_m1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_apm_m1_intr_map_reg_t;

/** Type of hp_apm_m2_intr_map register
 *  HP_APM_M2_INTR mapping register
 */
typedef union {
    struct {
        /** hp_apm_m2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_apm_m2_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_apm_m2_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_apm_m2_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_apm_m2_intr_map_reg_t;

/** Type of hp_apm_m3_intr_map register
 *  HP_APM_M3_INTR mapping register
 */
typedef union {
    struct {
        /** hp_apm_m3_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_apm_m3_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_apm_m3_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_apm_m3_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_apm_m3_intr_map_reg_t;

/** Type of hp_apm_m4_intr_map register
 *  HP_APM_M4_INTR mapping register
 */
typedef union {
    struct {
        /** hp_apm_m4_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_apm_m4_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_apm_m4_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_apm_m4_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_apm_m4_intr_map_reg_t;

/** Type of hp_apm_m5_intr_map register
 *  HP_APM_M5_INTR mapping register
 */
typedef union {
    struct {
        /** hp_apm_m5_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_apm_m5_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_apm_m5_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_apm_m5_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_apm_m5_intr_map_reg_t;

/** Type of hp_apm_m6_intr_map register
 *  HP_APM_M6_INTR mapping register
 */
typedef union {
    struct {
        /** hp_apm_m6_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_apm_m6_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_apm_m6_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_apm_m6_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_apm_m6_intr_map_reg_t;

/** Type of hp_peri0_pms_intr_map register
 *  HP_PERI0_PMS_INTR mapping register
 */
typedef union {
    struct {
        /** hp_peri0_pms_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_peri0_pms_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_peri0_pms_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_peri0_pms_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_peri0_pms_intr_map_reg_t;

/** Type of hp_peri1_pms_intr_map register
 *  HP_PERI1_PMS_INTR mapping register
 */
typedef union {
    struct {
        /** hp_peri1_pms_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_peri1_pms_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_peri1_pms_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_peri1_pms_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_peri1_pms_intr_map_reg_t;

/** Type of cpu0_peri_pms_intr_map register
 *  CPU0_PERI_PMS_INTR mapping register
 */
typedef union {
    struct {
        /** cpu0_peri_pms_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu0_peri_pms_intr_map:6;
        uint32_t reserved_6:2;
        /** cpu0_peri_pms_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t cpu0_peri_pms_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_cpu0_peri_pms_intr_map_reg_t;

/** Type of cpu1_peri_pms_intr_map register
 *  CPU1_PERI_PMS_INTR mapping register
 */
typedef union {
    struct {
        /** cpu1_peri_pms_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu1_peri_pms_intr_map:6;
        uint32_t reserved_6:2;
        /** cpu1_peri_pms_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t cpu1_peri_pms_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_cpu1_peri_pms_intr_map_reg_t;

/** Type of flash_mspi_intr_map register
 *  FLASH_MSPI_INTR mapping register
 */
typedef union {
    struct {
        /** flash_mspi_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t flash_mspi_intr_map:6;
        uint32_t reserved_6:2;
        /** flash_mspi_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t flash_mspi_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_flash_mspi_intr_map_reg_t;

/** Type of lpi_intr_map register
 *  LPI_INTR mapping register
 */
typedef union {
    struct {
        /** lpi_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lpi_intr_map:6;
        uint32_t reserved_6:2;
        /** lpi_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lpi_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lpi_intr_map_reg_t;

/** Type of pmt_intr_map register
 *  PMT_INTR mapping register
 */
typedef union {
    struct {
        /** pmt_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t pmt_intr_map:6;
        uint32_t reserved_6:2;
        /** pmt_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t pmt_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_pmt_intr_map_reg_t;

/** Type of sbd_intr_map register
 *  SBD_INTR mapping register
 */
typedef union {
    struct {
        /** sbd_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t sbd_intr_map:6;
        uint32_t reserved_6:2;
        /** sbd_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t sbd_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_sbd_intr_map_reg_t;

/** Type of usb_otghs_intr_map register
 *  USB_OTGHS_INTR mapping register
 */
typedef union {
    struct {
        /** usb_otghs_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t usb_otghs_intr_map:6;
        uint32_t reserved_6:2;
        /** usb_otghs_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t usb_otghs_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_usb_otghs_intr_map_reg_t;

/** Type of usb_otghs_endp_multi_proc_intr_map register
 *  USB_OTGHS_ENDP_MULTI_PROC_INTR mapping register
 */
typedef union {
    struct {
        /** usb_otghs_endp_multi_proc_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t usb_otghs_endp_multi_proc_intr_map:6;
        uint32_t reserved_6:2;
        /** usb_otghs_endp_multi_proc_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t usb_otghs_endp_multi_proc_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_usb_otghs_endp_multi_proc_intr_map_reg_t;

/** Type of jpeg_intr_map register
 *  JPEG_INTR mapping register
 */
typedef union {
    struct {
        /** jpeg_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t jpeg_intr_map:6;
        uint32_t reserved_6:2;
        /** jpeg_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t jpeg_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_jpeg_intr_map_reg_t;

/** Type of ppa_intr_map register
 *  PPA_INTR mapping register
 */
typedef union {
    struct {
        /** ppa_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ppa_intr_map:6;
        uint32_t reserved_6:2;
        /** ppa_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t ppa_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_ppa_intr_map_reg_t;

/** Type of core0_trace_intr_map register
 *  CORE0_TRACE_INTR mapping register
 */
typedef union {
    struct {
        /** core0_trace_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_trace_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_trace_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t core0_trace_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_core0_trace_intr_map_reg_t;

/** Type of core1_trace_intr_map register
 *  CORE1_TRACE_INTR mapping register
 */
typedef union {
    struct {
        /** core1_trace_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core1_trace_intr_map:6;
        uint32_t reserved_6:2;
        /** core1_trace_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t core1_trace_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_core1_trace_intr_map_reg_t;

/** Type of dma2d_in_ch0_intr_map register
 *  DMA2D_IN_CH0_INTR mapping register
 */
typedef union {
    struct {
        /** dma2d_in_ch0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t dma2d_in_ch0_intr_map:6;
        uint32_t reserved_6:2;
        /** dma2d_in_ch0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t dma2d_in_ch0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_dma2d_in_ch0_intr_map_reg_t;

/** Type of dma2d_in_ch1_intr_map register
 *  DMA2D_IN_CH1_INTR mapping register
 */
typedef union {
    struct {
        /** dma2d_in_ch1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t dma2d_in_ch1_intr_map:6;
        uint32_t reserved_6:2;
        /** dma2d_in_ch1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t dma2d_in_ch1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_dma2d_in_ch1_intr_map_reg_t;

/** Type of dma2d_in_ch2_intr_map register
 *  DMA2D_IN_CH2_INTR mapping register
 */
typedef union {
    struct {
        /** dma2d_in_ch2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t dma2d_in_ch2_intr_map:6;
        uint32_t reserved_6:2;
        /** dma2d_in_ch2_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t dma2d_in_ch2_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_dma2d_in_ch2_intr_map_reg_t;

/** Type of dma2d_out_ch0_intr_map register
 *  DMA2D_OUT_CH0_INTR mapping register
 */
typedef union {
    struct {
        /** dma2d_out_ch0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t dma2d_out_ch0_intr_map:6;
        uint32_t reserved_6:2;
        /** dma2d_out_ch0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t dma2d_out_ch0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_dma2d_out_ch0_intr_map_reg_t;

/** Type of dma2d_out_ch1_intr_map register
 *  DMA2D_OUT_CH1_INTR mapping register
 */
typedef union {
    struct {
        /** dma2d_out_ch1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t dma2d_out_ch1_intr_map:6;
        uint32_t reserved_6:2;
        /** dma2d_out_ch1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t dma2d_out_ch1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_dma2d_out_ch1_intr_map_reg_t;

/** Type of dma2d_out_ch2_intr_map register
 *  DMA2D_OUT_CH2_INTR mapping register
 */
typedef union {
    struct {
        /** dma2d_out_ch2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t dma2d_out_ch2_intr_map:6;
        uint32_t reserved_6:2;
        /** dma2d_out_ch2_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t dma2d_out_ch2_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_dma2d_out_ch2_intr_map_reg_t;

/** Type of dma2d_out_ch3_intr_map register
 *  DMA2D_OUT_CH3_INTR mapping register
 */
typedef union {
    struct {
        /** dma2d_out_ch3_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t dma2d_out_ch3_intr_map:6;
        uint32_t reserved_6:2;
        /** dma2d_out_ch3_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t dma2d_out_ch3_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_dma2d_out_ch3_intr_map_reg_t;

/** Type of psram_mspi_intr_map register
 *  PSRAM_MSPI_INTR mapping register
 */
typedef union {
    struct {
        /** psram_mspi_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t psram_mspi_intr_map:6;
        uint32_t reserved_6:2;
        /** psram_mspi_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t psram_mspi_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_psram_mspi_intr_map_reg_t;

/** Type of hp_sysreg_intr_map register
 *  HP_SYSREG_INTR mapping register
 */
typedef union {
    struct {
        /** hp_sysreg_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_sysreg_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_sysreg_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_sysreg_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_sysreg_intr_map_reg_t;

/** Type of pcnt0_intr_map register
 *  PCNT0_INTR mapping register
 */
typedef union {
    struct {
        /** pcnt0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t pcnt0_intr_map:6;
        uint32_t reserved_6:2;
        /** pcnt0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t pcnt0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_pcnt0_intr_map_reg_t;

/** Type of pcnt1_intr_map register
 *  PCNT1_INTR mapping register
 */
typedef union {
    struct {
        /** pcnt1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t pcnt1_intr_map:6;
        uint32_t reserved_6:2;
        /** pcnt1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t pcnt1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_pcnt1_intr_map_reg_t;

/** Type of hp_pau_intr_map register
 *  HP_PAU_INTR mapping register
 */
typedef union {
    struct {
        /** hp_pau_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_pau_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_pau_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_pau_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_pau_intr_map_reg_t;

/** Type of hp_parlio_rx_intr_map register
 *  HP_PARLIO_RX_INTR mapping register
 */
typedef union {
    struct {
        /** hp_parlio_rx_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_parlio_rx_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_parlio_rx_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_parlio_rx_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_parlio_rx_intr_map_reg_t;

/** Type of hp_parlio_tx_intr_map register
 *  HP_PARLIO_TX_INTR mapping register
 */
typedef union {
    struct {
        /** hp_parlio_tx_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_parlio_tx_intr_map:6;
        uint32_t reserved_6:2;
        /** hp_parlio_tx_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t hp_parlio_tx_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_hp_parlio_tx_intr_map_reg_t;

/** Type of bus_monitor_intr_map register
 *  BUS_MONITOR_INTR mapping register
 */
typedef union {
    struct {
        /** bus_monitor_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t bus_monitor_intr_map:6;
        uint32_t reserved_6:2;
        /** bus_monitor_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t bus_monitor_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_bus_monitor_intr_map_reg_t;

/** Type of modem_wifi_mac_intr_map register
 *  MODEM_WIFI_MAC_INTR mapping register
 */
typedef union {
    struct {
        /** modem_wifi_mac_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t modem_wifi_mac_intr_map:6;
        uint32_t reserved_6:2;
        /** modem_wifi_mac_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t modem_wifi_mac_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_modem_wifi_mac_intr_map_reg_t;

/** Type of modem_wifi_mac_nmi_intr_map register
 *  MODEM_WIFI_MAC_NMI_INTR mapping register
 */
typedef union {
    struct {
        /** modem_wifi_mac_nmi_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t modem_wifi_mac_nmi_intr_map:6;
        uint32_t reserved_6:2;
        /** modem_wifi_mac_nmi_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t modem_wifi_mac_nmi_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_modem_wifi_mac_nmi_intr_map_reg_t;

/** Type of modem_wifi_pwr_intr_map register
 *  MODEM_WIFI_PWR_INTR mapping register
 */
typedef union {
    struct {
        /** modem_wifi_pwr_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t modem_wifi_pwr_intr_map:6;
        uint32_t reserved_6:2;
        /** modem_wifi_pwr_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t modem_wifi_pwr_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_modem_wifi_pwr_intr_map_reg_t;

/** Type of modem_wifi_bb_intr_map register
 *  MODEM_WIFI_BB_INTR mapping register
 */
typedef union {
    struct {
        /** modem_wifi_bb_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t modem_wifi_bb_intr_map:6;
        uint32_t reserved_6:2;
        /** modem_wifi_bb_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t modem_wifi_bb_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_modem_wifi_bb_intr_map_reg_t;

/** Type of modem_bt_mac_intr_map register
 *  MODEM_BT_MAC_INTR mapping register
 */
typedef union {
    struct {
        /** modem_bt_mac_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t modem_bt_mac_intr_map:6;
        uint32_t reserved_6:2;
        /** modem_bt_mac_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t modem_bt_mac_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_modem_bt_mac_intr_map_reg_t;

/** Type of modem_bt_bb_intr_map register
 *  MODEM_BT_BB_INTR mapping register
 */
typedef union {
    struct {
        /** modem_bt_bb_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t modem_bt_bb_intr_map:6;
        uint32_t reserved_6:2;
        /** modem_bt_bb_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t modem_bt_bb_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_modem_bt_bb_intr_map_reg_t;

/** Type of modem_bt_bb_nmi_intr_map register
 *  MODEM_BT_BB_NMI_INTR mapping register
 */
typedef union {
    struct {
        /** modem_bt_bb_nmi_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t modem_bt_bb_nmi_intr_map:6;
        uint32_t reserved_6:2;
        /** modem_bt_bb_nmi_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t modem_bt_bb_nmi_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_modem_bt_bb_nmi_intr_map_reg_t;

/** Type of modem_lp_timer_intr_map register
 *  MODEM_LP_TIMER_INTR mapping register
 */
typedef union {
    struct {
        /** modem_lp_timer_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t modem_lp_timer_intr_map:6;
        uint32_t reserved_6:2;
        /** modem_lp_timer_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t modem_lp_timer_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_modem_lp_timer_intr_map_reg_t;

/** Type of modem_coex_intr_map register
 *  MODEM_COEX_INTR mapping register
 */
typedef union {
    struct {
        /** modem_coex_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t modem_coex_intr_map:6;
        uint32_t reserved_6:2;
        /** modem_coex_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t modem_coex_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_modem_coex_intr_map_reg_t;

/** Type of modem_ble_timer_intr_map register
 *  MODEM_BLE_TIMER_INTR mapping register
 */
typedef union {
    struct {
        /** modem_ble_timer_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t modem_ble_timer_intr_map:6;
        uint32_t reserved_6:2;
        /** modem_ble_timer_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t modem_ble_timer_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_modem_ble_timer_intr_map_reg_t;

/** Type of modem_ble_sec_intr_map register
 *  MODEM_BLE_SEC_INTR mapping register
 */
typedef union {
    struct {
        /** modem_ble_sec_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t modem_ble_sec_intr_map:6;
        uint32_t reserved_6:2;
        /** modem_ble_sec_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t modem_ble_sec_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_modem_ble_sec_intr_map_reg_t;

/** Type of modem_i2c_mst_intr_map register
 *  MODEM_I2C_MST_INTR mapping register
 */
typedef union {
    struct {
        /** modem_i2c_mst_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t modem_i2c_mst_intr_map:6;
        uint32_t reserved_6:2;
        /** modem_i2c_mst_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t modem_i2c_mst_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_modem_i2c_mst_intr_map_reg_t;

/** Type of modem_zb_mac_intr_map register
 *  MODEM_ZB_MAC_INTR mapping register
 */
typedef union {
    struct {
        /** modem_zb_mac_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t modem_zb_mac_intr_map:6;
        uint32_t reserved_6:2;
        /** modem_zb_mac_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t modem_zb_mac_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_modem_zb_mac_intr_map_reg_t;

/** Type of modem_bt_mac_int1_intr_map register
 *  MODEM_BT_MAC_INT1_INTR mapping register
 */
typedef union {
    struct {
        /** modem_bt_mac_int1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t modem_bt_mac_int1_intr_map:6;
        uint32_t reserved_6:2;
        /** modem_bt_mac_int1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t modem_bt_mac_int1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_modem_bt_mac_int1_intr_map_reg_t;

/** Type of cordic_intr_map register
 *  CORDIC_INTR mapping register
 */
typedef union {
    struct {
        /** cordic_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cordic_intr_map:6;
        uint32_t reserved_6:2;
        /** cordic_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t cordic_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_cordic_intr_map_reg_t;

/** Type of zero_det_intr_map register
 *  ZERO_DET_INTR mapping register
 */
typedef union {
    struct {
        /** zero_det_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t zero_det_intr_map:6;
        uint32_t reserved_6:2;
        /** zero_det_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t zero_det_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_zero_det_intr_map_reg_t;

/** Type of lp_wdt_intr_map register
 *  LP_WDT_INTR mapping register
 */
typedef union {
    struct {
        /** lp_wdt_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_wdt_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_wdt_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_wdt_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_wdt_intr_map_reg_t;

/** Type of lp_timer_reg_0_intr_map register
 *  LP_TIMER_REG_0_INTR mapping register
 */
typedef union {
    struct {
        /** lp_timer_reg_0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_timer_reg_0_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_timer_reg_0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_timer_reg_0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_timer_reg_0_intr_map_reg_t;

/** Type of lp_timer_reg_1_intr_map register
 *  LP_TIMER_REG_1_INTR mapping register
 */
typedef union {
    struct {
        /** lp_timer_reg_1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_timer_reg_1_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_timer_reg_1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_timer_reg_1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_timer_reg_1_intr_map_reg_t;

/** Type of mb_hp_intr_map register
 *  MB_HP_INTR mapping register
 */
typedef union {
    struct {
        /** mb_hp_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t mb_hp_intr_map:6;
        uint32_t reserved_6:2;
        /** mb_hp_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t mb_hp_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_mb_hp_intr_map_reg_t;

/** Type of mb_lp_intr_map register
 *  MB_LP_INTR mapping register
 */
typedef union {
    struct {
        /** mb_lp_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t mb_lp_intr_map:6;
        uint32_t reserved_6:2;
        /** mb_lp_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t mb_lp_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_mb_lp_intr_map_reg_t;

/** Type of pmu_reg_0_intr_map register
 *  PMU_REG_0_INTR mapping register
 */
typedef union {
    struct {
        /** pmu_reg_0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t pmu_reg_0_intr_map:6;
        uint32_t reserved_6:2;
        /** pmu_reg_0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t pmu_reg_0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_pmu_reg_0_intr_map_reg_t;

/** Type of pmu_reg_1_intr_map register
 *  PMU_REG_1_INTR mapping register
 */
typedef union {
    struct {
        /** pmu_reg_1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t pmu_reg_1_intr_map:6;
        uint32_t reserved_6:2;
        /** pmu_reg_1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t pmu_reg_1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_pmu_reg_1_intr_map_reg_t;

/** Type of lp_anaperi_intr_map register
 *  LP_ANAPERI_INTR mapping register
 */
typedef union {
    struct {
        /** lp_anaperi_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_anaperi_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_anaperi_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_anaperi_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_anaperi_intr_map_reg_t;

/** Type of lp_adc_intr_map register
 *  LP_ADC_INTR mapping register
 */
typedef union {
    struct {
        /** lp_adc_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_adc_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_adc_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_adc_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_adc_intr_map_reg_t;

/** Type of lp_dac_intr_map register
 *  LP_DAC_INTR mapping register
 */
typedef union {
    struct {
        /** lp_dac_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_dac_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_dac_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_dac_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_dac_intr_map_reg_t;

/** Type of lp_gpio_intr_map register
 *  LP_GPIO_INTR mapping register
 */
typedef union {
    struct {
        /** lp_gpio_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_gpio_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_gpio_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_gpio_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_gpio_intr_map_reg_t;

/** Type of lp_i2c_intr_map register
 *  LP_I2C_INTR mapping register
 */
typedef union {
    struct {
        /** lp_i2c_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_i2c_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_i2c_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_i2c_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_i2c_intr_map_reg_t;

/** Type of lp_spi_intr_map register
 *  LP_SPI_INTR mapping register
 */
typedef union {
    struct {
        /** lp_spi_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_spi_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_spi_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_spi_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_spi_intr_map_reg_t;

/** Type of lp_touch_intr_map register
 *  LP_TOUCH_INTR mapping register
 */
typedef union {
    struct {
        /** lp_touch_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_touch_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_touch_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_touch_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_touch_intr_map_reg_t;

/** Type of lp_tsens_intr_map register
 *  LP_TSENS_INTR mapping register
 */
typedef union {
    struct {
        /** lp_tsens_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_tsens_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_tsens_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_tsens_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_tsens_intr_map_reg_t;

/** Type of lp_uart_intr_map register
 *  LP_UART_INTR mapping register
 */
typedef union {
    struct {
        /** lp_uart_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_uart_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_uart_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_uart_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_uart_intr_map_reg_t;

/** Type of lp_efuse_intr_map register
 *  LP_EFUSE_INTR mapping register
 */
typedef union {
    struct {
        /** lp_efuse_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_efuse_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_efuse_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_efuse_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_efuse_intr_map_reg_t;

/** Type of lp_sw_intr_map register
 *  LP_SW_INTR mapping register
 */
typedef union {
    struct {
        /** lp_sw_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_sw_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_sw_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_sw_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_sw_intr_map_reg_t;

/** Type of lp_trng_intr_map register
 *  LP_TRNG_INTR mapping register
 */
typedef union {
    struct {
        /** lp_trng_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_trng_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_trng_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_trng_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_trng_intr_map_reg_t;

/** Type of lp_sysreg_intr_map register
 *  LP_SYSREG_INTR mapping register
 */
typedef union {
    struct {
        /** lp_sysreg_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_sysreg_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_sysreg_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_sysreg_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_sysreg_intr_map_reg_t;

/** Type of lp_apm_m0_intr_map register
 *  LP_APM_M0_INTR mapping register
 */
typedef union {
    struct {
        /** lp_apm_m0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_apm_m0_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_apm_m0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_apm_m0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_apm_m0_intr_map_reg_t;

/** Type of lp_apm_m1_intr_map register
 *  LP_APM_M1_INTR mapping register
 */
typedef union {
    struct {
        /** lp_apm_m1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_apm_m1_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_apm_m1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_apm_m1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_apm_m1_intr_map_reg_t;

/** Type of lp_apm_m2_intr_map register
 *  LP_APM_M2_INTR mapping register
 */
typedef union {
    struct {
        /** lp_apm_m2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_apm_m2_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_apm_m2_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_apm_m2_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_apm_m2_intr_map_reg_t;

/** Type of lp_apm_m3_intr_map register
 *  LP_APM_M3_INTR mapping register
 */
typedef union {
    struct {
        /** lp_apm_m3_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_apm_m3_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_apm_m3_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_apm_m3_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_apm_m3_intr_map_reg_t;

/** Type of lp_peri0_pms_intr_map register
 *  LP_PERI0_PMS_INTR mapping register
 */
typedef union {
    struct {
        /** lp_peri0_pms_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_peri0_pms_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_peri0_pms_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_peri0_pms_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_peri0_pms_intr_map_reg_t;

/** Type of lp_peri1_pms_intr_map register
 *  LP_PERI1_PMS_INTR mapping register
 */
typedef union {
    struct {
        /** lp_peri1_pms_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_peri1_pms_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_peri1_pms_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_peri1_pms_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_peri1_pms_intr_map_reg_t;

/** Type of lp_huk_intr_map register
 *  LP_HUK_INTR mapping register
 */
typedef union {
    struct {
        /** lp_huk_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_huk_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_huk_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_huk_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_huk_intr_map_reg_t;

/** Type of lp_peri_timeout_intr_map register
 *  LP_PERI_TIMEOUT_INTR mapping register
 */
typedef union {
    struct {
        /** lp_peri_timeout_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_peri_timeout_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_peri_timeout_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_peri_timeout_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_peri_timeout_intr_map_reg_t;

/** Type of lp_ahb_pdma_in_ch0_intr_map register
 *  LP_AHB_PDMA_IN_CH0_INTR mapping register
 */
typedef union {
    struct {
        /** lp_ahb_pdma_in_ch0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_ahb_pdma_in_ch0_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_ahb_pdma_in_ch0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_ahb_pdma_in_ch0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_ahb_pdma_in_ch0_intr_map_reg_t;

/** Type of lp_ahb_pdma_in_ch1_intr_map register
 *  LP_AHB_PDMA_IN_CH1_INTR mapping register
 */
typedef union {
    struct {
        /** lp_ahb_pdma_in_ch1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_ahb_pdma_in_ch1_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_ahb_pdma_in_ch1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_ahb_pdma_in_ch1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_ahb_pdma_in_ch1_intr_map_reg_t;

/** Type of lp_ahb_pdma_out_ch0_intr_map register
 *  LP_AHB_PDMA_OUT_CH0_INTR mapping register
 */
typedef union {
    struct {
        /** lp_ahb_pdma_out_ch0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_ahb_pdma_out_ch0_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_ahb_pdma_out_ch0_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_ahb_pdma_out_ch0_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_ahb_pdma_out_ch0_intr_map_reg_t;

/** Type of lp_ahb_pdma_out_ch1_intr_map register
 *  LP_AHB_PDMA_OUT_CH1_INTR mapping register
 */
typedef union {
    struct {
        /** lp_ahb_pdma_out_ch1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_ahb_pdma_out_ch1_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_ahb_pdma_out_ch1_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_ahb_pdma_out_ch1_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_ahb_pdma_out_ch1_intr_map_reg_t;

/** Type of lp_sw_invalid_sleep_intr_map register
 *  LP_SW_INVALID_SLEEP_INTR mapping register
 */
typedef union {
    struct {
        /** lp_sw_invalid_sleep_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_sw_invalid_sleep_intr_map:6;
        uint32_t reserved_6:2;
        /** lp_sw_invalid_sleep_intr_pass_level : R/W; bitpos: [9:8]; default: 0;
         *  Set as Non-zero value to enable interrupt-remap feature of the interrupt source.
         *  0=disable remap feature. 1=enable remap feature with S-Level. 2/3=enable remap
         *  feature with M-Level.
         */
        uint32_t lp_sw_invalid_sleep_intr_pass_level:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} interrupt_core0_lp_sw_invalid_sleep_intr_map_reg_t;

/** Type of sig_idx_assert_in_sec register
 *  reserved
 */
typedef union {
    struct {
        /** int_sig_idx_assert_in_s : R/W; bitpos: [5:0]; default: 0;
         *  reserved
         */
        uint32_t int_sig_idx_assert_in_s:6;
        /** int_sig_idx_assert_in_m : R/W; bitpos: [11:6]; default: 0;
         *  reserved
         */
        uint32_t int_sig_idx_assert_in_m:6;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} interrupt_core0_sig_idx_assert_in_sec_reg_t;

/** Type of clock_gate register
 *  Interrupt clock gating configure register
 */
typedef union {
    struct {
        /** reg_clk_en : R/W; bitpos: [0]; default: 0;
         *  Interrupt clock gating configure register
         */
        uint32_t reg_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} interrupt_core0_clock_gate_reg_t;


/** Group: Status Registers */
/** Type of int_status_reg_0 register
 *  Status register for interrupt sources 0 ~ 31
 */
typedef union {
    struct {
        /** int_status_0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the status of the interrupt sources within interrupt-index-range 0 ~ 31.
         *  Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source triggered an interrupt
         *  1:No interrupt triggered
         */
        uint32_t int_status_0:32;
    };
    uint32_t val;
} interrupt_core0_int_status_reg_0_reg_t;

/** Type of int_status_reg_1 register
 *  Status register for interrupt sources 32 ~ 63
 */
typedef union {
    struct {
        /** int_status_1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the status of the interrupt sources within interrupt-index-range 32 ~
         *  63. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source triggered an interrupt
         *  1:No interrupt triggered
         */
        uint32_t int_status_1:32;
    };
    uint32_t val;
} interrupt_core0_int_status_reg_1_reg_t;

/** Type of int_status_reg_2 register
 *  Status register for interrupt sources 64 ~ 95
 */
typedef union {
    struct {
        /** int_status_2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the status of the interrupt sources within interrupt-index-range 64 ~
         *  95. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source triggered an interrupt
         *  1:No interrupt triggered
         */
        uint32_t int_status_2:32;
    };
    uint32_t val;
} interrupt_core0_int_status_reg_2_reg_t;

/** Type of int_status_reg_3 register
 *  Status register for interrupt sources 96 ~ 127
 */
typedef union {
    struct {
        /** int_status_3 : RO; bitpos: [31:0]; default: 0;
         *  Represents the status of the interrupt sources within interrupt-index-range 96 ~
         *  127. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source triggered an interrupt
         *  1:No interrupt triggered
         */
        uint32_t int_status_3:32;
    };
    uint32_t val;
} interrupt_core0_int_status_reg_3_reg_t;

/** Type of int_status_reg_4 register
 *  Status register for interrupt sources 128 ~ 159
 */
typedef union {
    struct {
        /** int_status_4 : RO; bitpos: [31:0]; default: 0;
         *  Represents the status of the interrupt sources within interrupt-index-range 128 ~
         *  159. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source triggered an interrupt
         *  1:No interrupt triggered
         */
        uint32_t int_status_4:32;
    };
    uint32_t val;
} interrupt_core0_int_status_reg_4_reg_t;

/** Type of int_status_reg_5 register
 *  Status register for interrupt sources 160 ~ 168
 */
typedef union {
    struct {
        /** int_status_5 : RO; bitpos: [8:0]; default: 0;
         *  Represents the status of the interrupt sources within interrupt-index-range 160 ~
         *  168. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source triggered an interrupt
         *  1:No interrupt triggered
         */
        uint32_t int_status_5:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_int_status_reg_5_reg_t;

/** Type of src_pass_in_s_status_0 register
 *  supervisor-mode PASS_IN status register for interrupt sources 0 ~ 31
 */
typedef union {
    struct {
        /** int_src_pass_in_s_status_0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the supervisor-mode PASS_IN status of the interrupt sources within
         *  interrupt-index-range 0 ~ 31. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source is not PASS in supervisor-mode.
         *  1:The corresponding interrupt source is PASS in supervisor-mode.
         */
        uint32_t int_src_pass_in_s_status_0:32;
    };
    uint32_t val;
} interrupt_core0_src_pass_in_s_status_0_reg_t;

/** Type of src_pass_in_s_status_1 register
 *  supervisor-mode PASS_IN status register for interrupt sources 32 ~ 63
 */
typedef union {
    struct {
        /** int_src_pass_in_s_status_1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the supervisor-mode PASS_IN status of the interrupt sources within
         *  interrupt-index-range 32 ~ 63. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source is not PASS in supervisor-mode.
         *  1:The corresponding interrupt source is PASS in supervisor-mode.
         */
        uint32_t int_src_pass_in_s_status_1:32;
    };
    uint32_t val;
} interrupt_core0_src_pass_in_s_status_1_reg_t;

/** Type of src_pass_in_s_status_2 register
 *  supervisor-mode PASS_IN status register for interrupt sources 64 ~ 95
 */
typedef union {
    struct {
        /** int_src_pass_in_s_status_2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the supervisor-mode PASS_IN status of the interrupt sources within
         *  interrupt-index-range 64 ~ 95. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source is not PASS in supervisor-mode.
         *  1:The corresponding interrupt source is PASS in supervisor-mode.
         */
        uint32_t int_src_pass_in_s_status_2:32;
    };
    uint32_t val;
} interrupt_core0_src_pass_in_s_status_2_reg_t;

/** Type of src_pass_in_s_status_3 register
 *  supervisor-mode PASS_IN status register for interrupt sources 96 ~ 127
 */
typedef union {
    struct {
        /** int_src_pass_in_s_status_3 : RO; bitpos: [31:0]; default: 0;
         *  Represents the supervisor-mode PASS_IN status of the interrupt sources within
         *  interrupt-index-range 96 ~ 127. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source is not PASS in supervisor-mode.
         *  1:The corresponding interrupt source is PASS in supervisor-mode.
         */
        uint32_t int_src_pass_in_s_status_3:32;
    };
    uint32_t val;
} interrupt_core0_src_pass_in_s_status_3_reg_t;

/** Type of src_pass_in_s_status_4 register
 *  supervisor-mode PASS_IN status register for interrupt sources 128 ~ 159
 */
typedef union {
    struct {
        /** int_src_pass_in_s_status_4 : RO; bitpos: [31:0]; default: 0;
         *  Represents the supervisor-mode PASS_IN status of the interrupt sources within
         *  interrupt-index-range 128 ~ 159. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source is not PASS in supervisor-mode.
         *  1:The corresponding interrupt source is PASS in supervisor-mode.
         */
        uint32_t int_src_pass_in_s_status_4:32;
    };
    uint32_t val;
} interrupt_core0_src_pass_in_s_status_4_reg_t;

/** Type of src_pass_in_s_status_5 register
 *  supervisor-mode PASS_IN status register for interrupt sources 160 ~ 168
 */
typedef union {
    struct {
        /** int_src_pass_in_s_status_5 : RO; bitpos: [8:0]; default: 0;
         *  Represents the supervisor-mode PASS_IN status of the interrupt sources with
         *  interrupt-index-range 160 ~ 168. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source is not PASS in supervisor-mode.
         *  1:The corresponding interrupt source is PASS in supervisor-mode.
         */
        uint32_t int_src_pass_in_s_status_5:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_src_pass_in_s_status_5_reg_t;

/** Type of src_pass_in_m_status_0 register
 *  machine-mode PASS_IN_M status register for interrupt sources 0 ~ 31
 */
typedef union {
    struct {
        /** int_src_pass_in_m_status_0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the machine-mode PASS_IN status of the interrupt sources within
         *  interrupt-index-range 0 ~ 31. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source is not PASS in machine-mode.
         *  1:The corresponding interrupt source is PASS in machine-mode.
         */
        uint32_t int_src_pass_in_m_status_0:32;
    };
    uint32_t val;
} interrupt_core0_src_pass_in_m_status_0_reg_t;

/** Type of src_pass_in_m_status_1 register
 *  machine-mode PASS_IN_M status register for interrupt sources 32 ~ 63
 */
typedef union {
    struct {
        /** int_src_pass_in_m_status_1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the machine-mode PASS_IN status of the interrupt sources within
         *  interrupt-index-range 32 ~ 63. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source is not PASS in machine-mode.
         *  1:The corresponding interrupt source is PASS in machine-mode.
         */
        uint32_t int_src_pass_in_m_status_1:32;
    };
    uint32_t val;
} interrupt_core0_src_pass_in_m_status_1_reg_t;

/** Type of src_pass_in_m_status_2 register
 *  machine-mode PASS_IN_M status register for interrupt sources 64 ~ 95
 */
typedef union {
    struct {
        /** int_src_pass_in_m_status_2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the machine-mode PASS_IN status of the interrupt sources within
         *  interrupt-index-range 64 ~ 95. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source is not PASS in machine-mode.
         *  1:The corresponding interrupt source is PASS in machine-mode.
         */
        uint32_t int_src_pass_in_m_status_2:32;
    };
    uint32_t val;
} interrupt_core0_src_pass_in_m_status_2_reg_t;

/** Type of src_pass_in_m_status_3 register
 *  machine-mode PASS_IN_M status register for interrupt sources 96 ~ 127
 */
typedef union {
    struct {
        /** int_src_pass_in_m_status_3 : RO; bitpos: [31:0]; default: 0;
         *  Represents the machine-mode PASS_IN status of the interrupt sources within
         *  interrupt-index-range 96 ~ 127. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source is not PASS in machine-mode.
         *  1:The corresponding interrupt source is PASS in machine-mode.
         */
        uint32_t int_src_pass_in_m_status_3:32;
    };
    uint32_t val;
} interrupt_core0_src_pass_in_m_status_3_reg_t;

/** Type of src_pass_in_m_status_4 register
 *  machine-mode PASS_IN_M status register for interrupt sources 128 ~ 159
 */
typedef union {
    struct {
        /** int_src_pass_in_m_status_4 : RO; bitpos: [31:0]; default: 0;
         *  Represents the machine-mode PASS_IN status of the interrupt sources within
         *  interrupt-index-range 128 ~ 159. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source is not PASS in machine-mode.
         *  1:The corresponding interrupt source is PASS in machine-mode.
         */
        uint32_t int_src_pass_in_m_status_4:32;
    };
    uint32_t val;
} interrupt_core0_src_pass_in_m_status_4_reg_t;

/** Type of src_pass_in_m_status_5 register
 *  machine-mode PASS_IN_M status register for interrupt sources 160 ~ 168
 */
typedef union {
    struct {
        /** int_src_pass_in_m_status_5 : RO; bitpos: [8:0]; default: 0;
         *  Represents the machine-mode PASS_IN status of the interrupt sources with
         *  interrupt-index-range 160 ~ 168. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source is not PASS in machine-mode.
         *  1:The corresponding interrupt source is PASS in machine-mode.
         */
        uint32_t int_src_pass_in_m_status_5:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_src_pass_in_m_status_5_reg_t;

/** Type of config_state register
 *  reserved
 */
typedef union {
    struct {
        /** sig_idx_assert_s_err : RO; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t sig_idx_assert_s_err:1;
        /** sig_idx_assert_m_err : RO; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t sig_idx_assert_m_err:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} interrupt_core0_config_state_reg_t;

/** Type of s_status register
 *  reserved
 */
typedef union {
    struct {
        /** int_s_status : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t int_s_status:32;
    };
    uint32_t val;
} interrupt_core0_s_status_reg_t;

/** Type of m_status register
 *  reserved
 */
typedef union {
    struct {
        /** int_m_status : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t int_m_status:32;
    };
    uint32_t val;
} interrupt_core0_m_status_reg_t;


/** Group: Version Register */
/** Type of interrupt_date register
 *  Version control register
 */
typedef union {
    struct {
        /** interrupt_date : R/W; bitpos: [27:0]; default: 38834448;
         *  Version control register
         */
        uint32_t interrupt_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} interrupt_core0_interrupt_date_reg_t;


typedef struct {
    volatile interrupt_core0_sys_icm_intr_map_reg_t sys_icm_intr_map;
    volatile interrupt_core0_axi_perf_mon_intr_map_reg_t axi_perf_mon_intr_map;
    volatile interrupt_core0_usb_device_intr_map_reg_t usb_device_intr_map;
    volatile interrupt_core0_sdio_host_intr_map_reg_t sdio_host_intr_map;
    volatile interrupt_core0_spi2_intr_map_reg_t spi2_intr_map;
    volatile interrupt_core0_spi3_intr_map_reg_t spi3_intr_map;
    volatile interrupt_core0_i2s0_intr_map_reg_t i2s0_intr_map;
    volatile interrupt_core0_i2s1_intr_map_reg_t i2s1_intr_map;
    volatile interrupt_core0_uhci0_intr_map_reg_t uhci0_intr_map;
    volatile interrupt_core0_uart0_intr_map_reg_t uart0_intr_map;
    volatile interrupt_core0_uart1_intr_map_reg_t uart1_intr_map;
    volatile interrupt_core0_uart2_intr_map_reg_t uart2_intr_map;
    volatile interrupt_core0_uart3_intr_map_reg_t uart3_intr_map;
    volatile interrupt_core0_lcd_cam_intr_map_reg_t lcd_cam_intr_map;
    volatile interrupt_core0_pwm0_intr_map_reg_t pwm0_intr_map;
    volatile interrupt_core0_pwm1_intr_map_reg_t pwm1_intr_map;
    volatile interrupt_core0_pwm2_intr_map_reg_t pwm2_intr_map;
    volatile interrupt_core0_pwm3_intr_map_reg_t pwm3_intr_map;
    volatile interrupt_core0_can0_intr_map_reg_t can0_intr_map;
    volatile interrupt_core0_can0_timer_intr_map_reg_t can0_timer_intr_map;
    volatile interrupt_core0_can1_intr_map_reg_t can1_intr_map;
    volatile interrupt_core0_can1_timer_intr_map_reg_t can1_timer_intr_map;
    volatile interrupt_core0_rmt_intr_map_reg_t rmt_intr_map;
    volatile interrupt_core0_i2c0_intr_map_reg_t i2c0_intr_map;
    volatile interrupt_core0_i2c1_intr_map_reg_t i2c1_intr_map;
    volatile interrupt_core0_timergrp0_t0_intr_map_reg_t timergrp0_t0_intr_map;
    volatile interrupt_core0_timergrp0_t1_intr_map_reg_t timergrp0_t1_intr_map;
    volatile interrupt_core0_timergrp0_wdt_intr_map_reg_t timergrp0_wdt_intr_map;
    volatile interrupt_core0_timergrp1_t0_intr_map_reg_t timergrp1_t0_intr_map;
    volatile interrupt_core0_timergrp1_t1_intr_map_reg_t timergrp1_t1_intr_map;
    volatile interrupt_core0_timergrp1_wdt_intr_map_reg_t timergrp1_wdt_intr_map;
    volatile interrupt_core0_ledc0_intr_map_reg_t ledc0_intr_map;
    volatile interrupt_core0_ledc1_intr_map_reg_t ledc1_intr_map;
    volatile interrupt_core0_systimer_target0_intr_map_reg_t systimer_target0_intr_map;
    volatile interrupt_core0_systimer_target1_intr_map_reg_t systimer_target1_intr_map;
    volatile interrupt_core0_systimer_target2_intr_map_reg_t systimer_target2_intr_map;
    volatile interrupt_core0_ahb_pdma_in_ch0_intr_map_reg_t ahb_pdma_in_ch0_intr_map;
    volatile interrupt_core0_ahb_pdma_in_ch1_intr_map_reg_t ahb_pdma_in_ch1_intr_map;
    volatile interrupt_core0_ahb_pdma_in_ch2_intr_map_reg_t ahb_pdma_in_ch2_intr_map;
    volatile interrupt_core0_ahb_pdma_in_ch3_intr_map_reg_t ahb_pdma_in_ch3_intr_map;
    volatile interrupt_core0_ahb_pdma_in_ch4_intr_map_reg_t ahb_pdma_in_ch4_intr_map;
    volatile interrupt_core0_ahb_pdma_out_ch0_intr_map_reg_t ahb_pdma_out_ch0_intr_map;
    volatile interrupt_core0_ahb_pdma_out_ch1_intr_map_reg_t ahb_pdma_out_ch1_intr_map;
    volatile interrupt_core0_ahb_pdma_out_ch2_intr_map_reg_t ahb_pdma_out_ch2_intr_map;
    volatile interrupt_core0_ahb_pdma_out_ch3_intr_map_reg_t ahb_pdma_out_ch3_intr_map;
    volatile interrupt_core0_ahb_pdma_out_ch4_intr_map_reg_t ahb_pdma_out_ch4_intr_map;
    volatile interrupt_core0_asrc_chnl0_intr_map_reg_t asrc_chnl0_intr_map;
    volatile interrupt_core0_asrc_chnl1_intr_map_reg_t asrc_chnl1_intr_map;
    volatile interrupt_core0_axi_pdma_in_ch0_intr_map_reg_t axi_pdma_in_ch0_intr_map;
    volatile interrupt_core0_axi_pdma_in_ch1_intr_map_reg_t axi_pdma_in_ch1_intr_map;
    volatile interrupt_core0_axi_pdma_in_ch2_intr_map_reg_t axi_pdma_in_ch2_intr_map;
    volatile interrupt_core0_axi_pdma_out_ch0_intr_map_reg_t axi_pdma_out_ch0_intr_map;
    volatile interrupt_core0_axi_pdma_out_ch1_intr_map_reg_t axi_pdma_out_ch1_intr_map;
    volatile interrupt_core0_axi_pdma_out_ch2_intr_map_reg_t axi_pdma_out_ch2_intr_map;
    volatile interrupt_core0_rsa_intr_map_reg_t rsa_intr_map;
    volatile interrupt_core0_aes_intr_map_reg_t aes_intr_map;
    volatile interrupt_core0_sha_intr_map_reg_t sha_intr_map;
    volatile interrupt_core0_ecc_intr_map_reg_t ecc_intr_map;
    volatile interrupt_core0_ecdsa_intr_map_reg_t ecdsa_intr_map;
    volatile interrupt_core0_km_intr_map_reg_t km_intr_map;
    volatile interrupt_core0_rma_intr_map_reg_t rma_intr_map;
    volatile interrupt_core0_gpio_intr0_map_reg_t gpio_intr0_map;
    volatile interrupt_core0_gpio_intr1_map_reg_t gpio_intr1_map;
    volatile interrupt_core0_gpio_intr2_map_reg_t gpio_intr2_map;
    volatile interrupt_core0_gpio_intr3_map_reg_t gpio_intr3_map;
    volatile interrupt_core0_cpu_intr_from_cpu_0_map_reg_t cpu_intr_from_cpu_0_map;
    volatile interrupt_core0_cpu_intr_from_cpu_1_map_reg_t cpu_intr_from_cpu_1_map;
    volatile interrupt_core0_cpu_intr_from_cpu_2_map_reg_t cpu_intr_from_cpu_2_map;
    volatile interrupt_core0_cpu_intr_from_cpu_3_map_reg_t cpu_intr_from_cpu_3_map;
    volatile interrupt_core0_cache_intr_map_reg_t cache_intr_map;
    volatile interrupt_core0_cpu_apm_m0_intr_map_reg_t cpu_apm_m0_intr_map;
    volatile interrupt_core0_cpu_apm_m1_intr_map_reg_t cpu_apm_m1_intr_map;
    volatile interrupt_core0_cpu_apm_m2_intr_map_reg_t cpu_apm_m2_intr_map;
    volatile interrupt_core0_cpu_apm_m3_intr_map_reg_t cpu_apm_m3_intr_map;
    volatile interrupt_core0_hp_mem_apm_m0_intr_map_reg_t hp_mem_apm_m0_intr_map;
    volatile interrupt_core0_hp_mem_apm_m1_intr_map_reg_t hp_mem_apm_m1_intr_map;
    volatile interrupt_core0_hp_mem_apm_m2_intr_map_reg_t hp_mem_apm_m2_intr_map;
    volatile interrupt_core0_hp_mem_apm_m3_intr_map_reg_t hp_mem_apm_m3_intr_map;
    volatile interrupt_core0_hp_mem_apm_m4_intr_map_reg_t hp_mem_apm_m4_intr_map;
    volatile interrupt_core0_hp_mem_apm_m5_intr_map_reg_t hp_mem_apm_m5_intr_map;
    volatile interrupt_core0_cpu_peri0_timeout_intr_map_reg_t cpu_peri0_timeout_intr_map;
    volatile interrupt_core0_cpu_peri1_timeout_intr_map_reg_t cpu_peri1_timeout_intr_map;
    volatile interrupt_core0_hp_peri0_timeout_intr_map_reg_t hp_peri0_timeout_intr_map;
    volatile interrupt_core0_hp_peri1_timeout_intr_map_reg_t hp_peri1_timeout_intr_map;
    volatile interrupt_core0_hp_apm_m0_intr_map_reg_t hp_apm_m0_intr_map;
    volatile interrupt_core0_hp_apm_m1_intr_map_reg_t hp_apm_m1_intr_map;
    volatile interrupt_core0_hp_apm_m2_intr_map_reg_t hp_apm_m2_intr_map;
    volatile interrupt_core0_hp_apm_m3_intr_map_reg_t hp_apm_m3_intr_map;
    volatile interrupt_core0_hp_apm_m4_intr_map_reg_t hp_apm_m4_intr_map;
    volatile interrupt_core0_hp_apm_m5_intr_map_reg_t hp_apm_m5_intr_map;
    volatile interrupt_core0_hp_apm_m6_intr_map_reg_t hp_apm_m6_intr_map;
    volatile interrupt_core0_hp_peri0_pms_intr_map_reg_t hp_peri0_pms_intr_map;
    volatile interrupt_core0_hp_peri1_pms_intr_map_reg_t hp_peri1_pms_intr_map;
    volatile interrupt_core0_cpu0_peri_pms_intr_map_reg_t cpu0_peri_pms_intr_map;
    volatile interrupt_core0_cpu1_peri_pms_intr_map_reg_t cpu1_peri_pms_intr_map;
    volatile interrupt_core0_flash_mspi_intr_map_reg_t flash_mspi_intr_map;
    volatile interrupt_core0_lpi_intr_map_reg_t lpi_intr_map;
    volatile interrupt_core0_pmt_intr_map_reg_t pmt_intr_map;
    volatile interrupt_core0_sbd_intr_map_reg_t sbd_intr_map;
    volatile interrupt_core0_usb_otghs_intr_map_reg_t usb_otghs_intr_map;
    volatile interrupt_core0_usb_otghs_endp_multi_proc_intr_map_reg_t usb_otghs_endp_multi_proc_intr_map;
    volatile interrupt_core0_jpeg_intr_map_reg_t jpeg_intr_map;
    volatile interrupt_core0_ppa_intr_map_reg_t ppa_intr_map;
    volatile interrupt_core0_core0_trace_intr_map_reg_t core0_trace_intr_map;
    volatile interrupt_core0_core1_trace_intr_map_reg_t core1_trace_intr_map;
    volatile interrupt_core0_dma2d_in_ch0_intr_map_reg_t dma2d_in_ch0_intr_map;
    volatile interrupt_core0_dma2d_in_ch1_intr_map_reg_t dma2d_in_ch1_intr_map;
    volatile interrupt_core0_dma2d_in_ch2_intr_map_reg_t dma2d_in_ch2_intr_map;
    volatile interrupt_core0_dma2d_out_ch0_intr_map_reg_t dma2d_out_ch0_intr_map;
    volatile interrupt_core0_dma2d_out_ch1_intr_map_reg_t dma2d_out_ch1_intr_map;
    volatile interrupt_core0_dma2d_out_ch2_intr_map_reg_t dma2d_out_ch2_intr_map;
    volatile interrupt_core0_dma2d_out_ch3_intr_map_reg_t dma2d_out_ch3_intr_map;
    volatile interrupt_core0_psram_mspi_intr_map_reg_t psram_mspi_intr_map;
    volatile interrupt_core0_hp_sysreg_intr_map_reg_t hp_sysreg_intr_map;
    volatile interrupt_core0_pcnt0_intr_map_reg_t pcnt0_intr_map;
    volatile interrupt_core0_pcnt1_intr_map_reg_t pcnt1_intr_map;
    volatile interrupt_core0_hp_pau_intr_map_reg_t hp_pau_intr_map;
    volatile interrupt_core0_hp_parlio_rx_intr_map_reg_t hp_parlio_rx_intr_map;
    volatile interrupt_core0_hp_parlio_tx_intr_map_reg_t hp_parlio_tx_intr_map;
    volatile interrupt_core0_bus_monitor_intr_map_reg_t bus_monitor_intr_map;
    volatile interrupt_core0_modem_wifi_mac_intr_map_reg_t modem_wifi_mac_intr_map;
    volatile interrupt_core0_modem_wifi_mac_nmi_intr_map_reg_t modem_wifi_mac_nmi_intr_map;
    volatile interrupt_core0_modem_wifi_pwr_intr_map_reg_t modem_wifi_pwr_intr_map;
    volatile interrupt_core0_modem_wifi_bb_intr_map_reg_t modem_wifi_bb_intr_map;
    volatile interrupt_core0_modem_bt_mac_intr_map_reg_t modem_bt_mac_intr_map;
    volatile interrupt_core0_modem_bt_bb_intr_map_reg_t modem_bt_bb_intr_map;
    volatile interrupt_core0_modem_bt_bb_nmi_intr_map_reg_t modem_bt_bb_nmi_intr_map;
    volatile interrupt_core0_modem_lp_timer_intr_map_reg_t modem_lp_timer_intr_map;
    volatile interrupt_core0_modem_coex_intr_map_reg_t modem_coex_intr_map;
    volatile interrupt_core0_modem_ble_timer_intr_map_reg_t modem_ble_timer_intr_map;
    volatile interrupt_core0_modem_ble_sec_intr_map_reg_t modem_ble_sec_intr_map;
    volatile interrupt_core0_modem_i2c_mst_intr_map_reg_t modem_i2c_mst_intr_map;
    volatile interrupt_core0_modem_zb_mac_intr_map_reg_t modem_zb_mac_intr_map;
    volatile interrupt_core0_modem_bt_mac_int1_intr_map_reg_t modem_bt_mac_int1_intr_map;
    volatile interrupt_core0_cordic_intr_map_reg_t cordic_intr_map;
    volatile interrupt_core0_zero_det_intr_map_reg_t zero_det_intr_map;
    volatile interrupt_core0_lp_wdt_intr_map_reg_t lp_wdt_intr_map;
    volatile interrupt_core0_lp_timer_reg_0_intr_map_reg_t lp_timer_reg_0_intr_map;
    volatile interrupt_core0_lp_timer_reg_1_intr_map_reg_t lp_timer_reg_1_intr_map;
    volatile interrupt_core0_mb_hp_intr_map_reg_t mb_hp_intr_map;
    volatile interrupt_core0_mb_lp_intr_map_reg_t mb_lp_intr_map;
    volatile interrupt_core0_pmu_reg_0_intr_map_reg_t pmu_reg_0_intr_map;
    volatile interrupt_core0_pmu_reg_1_intr_map_reg_t pmu_reg_1_intr_map;
    volatile interrupt_core0_lp_anaperi_intr_map_reg_t lp_anaperi_intr_map;
    volatile interrupt_core0_lp_adc_intr_map_reg_t lp_adc_intr_map;
    volatile interrupt_core0_lp_dac_intr_map_reg_t lp_dac_intr_map;
    volatile interrupt_core0_lp_gpio_intr_map_reg_t lp_gpio_intr_map;
    volatile interrupt_core0_lp_i2c_intr_map_reg_t lp_i2c_intr_map;
    volatile interrupt_core0_lp_spi_intr_map_reg_t lp_spi_intr_map;
    volatile interrupt_core0_lp_touch_intr_map_reg_t lp_touch_intr_map;
    volatile interrupt_core0_lp_tsens_intr_map_reg_t lp_tsens_intr_map;
    volatile interrupt_core0_lp_uart_intr_map_reg_t lp_uart_intr_map;
    volatile interrupt_core0_lp_efuse_intr_map_reg_t lp_efuse_intr_map;
    volatile interrupt_core0_lp_sw_intr_map_reg_t lp_sw_intr_map;
    volatile interrupt_core0_lp_trng_intr_map_reg_t lp_trng_intr_map;
    volatile interrupt_core0_lp_sysreg_intr_map_reg_t lp_sysreg_intr_map;
    volatile interrupt_core0_lp_apm_m0_intr_map_reg_t lp_apm_m0_intr_map;
    volatile interrupt_core0_lp_apm_m1_intr_map_reg_t lp_apm_m1_intr_map;
    volatile interrupt_core0_lp_apm_m2_intr_map_reg_t lp_apm_m2_intr_map;
    volatile interrupt_core0_lp_apm_m3_intr_map_reg_t lp_apm_m3_intr_map;
    volatile interrupt_core0_lp_peri0_pms_intr_map_reg_t lp_peri0_pms_intr_map;
    volatile interrupt_core0_lp_peri1_pms_intr_map_reg_t lp_peri1_pms_intr_map;
    volatile interrupt_core0_lp_huk_intr_map_reg_t lp_huk_intr_map;
    volatile interrupt_core0_lp_peri_timeout_intr_map_reg_t lp_peri_timeout_intr_map;
    volatile interrupt_core0_lp_ahb_pdma_in_ch0_intr_map_reg_t lp_ahb_pdma_in_ch0_intr_map;
    volatile interrupt_core0_lp_ahb_pdma_in_ch1_intr_map_reg_t lp_ahb_pdma_in_ch1_intr_map;
    volatile interrupt_core0_lp_ahb_pdma_out_ch0_intr_map_reg_t lp_ahb_pdma_out_ch0_intr_map;
    volatile interrupt_core0_lp_ahb_pdma_out_ch1_intr_map_reg_t lp_ahb_pdma_out_ch1_intr_map;
    volatile interrupt_core0_lp_sw_invalid_sleep_intr_map_reg_t lp_sw_invalid_sleep_intr_map;
    volatile interrupt_core0_sig_idx_assert_in_sec_reg_t sig_idx_assert_in_sec;
    volatile interrupt_core0_int_status_reg_0_reg_t int_status_reg_0;
    volatile interrupt_core0_int_status_reg_1_reg_t int_status_reg_1;
    volatile interrupt_core0_int_status_reg_2_reg_t int_status_reg_2;
    volatile interrupt_core0_int_status_reg_3_reg_t int_status_reg_3;
    volatile interrupt_core0_int_status_reg_4_reg_t int_status_reg_4;
    volatile interrupt_core0_int_status_reg_5_reg_t int_status_reg_5;
    volatile interrupt_core0_src_pass_in_s_status_0_reg_t src_pass_in_s_status_0;
    volatile interrupt_core0_src_pass_in_s_status_1_reg_t src_pass_in_s_status_1;
    volatile interrupt_core0_src_pass_in_s_status_2_reg_t src_pass_in_s_status_2;
    volatile interrupt_core0_src_pass_in_s_status_3_reg_t src_pass_in_s_status_3;
    volatile interrupt_core0_src_pass_in_s_status_4_reg_t src_pass_in_s_status_4;
    volatile interrupt_core0_src_pass_in_s_status_5_reg_t src_pass_in_s_status_5;
    volatile interrupt_core0_src_pass_in_m_status_0_reg_t src_pass_in_m_status_0;
    volatile interrupt_core0_src_pass_in_m_status_1_reg_t src_pass_in_m_status_1;
    volatile interrupt_core0_src_pass_in_m_status_2_reg_t src_pass_in_m_status_2;
    volatile interrupt_core0_src_pass_in_m_status_3_reg_t src_pass_in_m_status_3;
    volatile interrupt_core0_src_pass_in_m_status_4_reg_t src_pass_in_m_status_4;
    volatile interrupt_core0_src_pass_in_m_status_5_reg_t src_pass_in_m_status_5;
    volatile interrupt_core0_config_state_reg_t config_state;
    volatile interrupt_core0_s_status_reg_t s_status;
    volatile interrupt_core0_m_status_reg_t m_status;
    volatile interrupt_core0_clock_gate_reg_t clock_gate;
    uint32_t reserved_300[319];
    volatile interrupt_core0_interrupt_date_reg_t interrupt_date;
} interrupt_core0_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(interrupt_core0_dev_t) == 0x800, "Invalid size of interrupt_core0_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
