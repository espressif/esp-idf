/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: INTERRUPT_CORE0_LP RTC INT MAP REG */
/** Type of lp_rtc_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_rtc_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_rtc_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_rtc_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP WDT INT MAP REG */
/** Type of lp_wdt_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_wdt_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_wdt_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_wdt_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP TIMER REG 0 INT MAP REG */
/** Type of lp_timer_reg_0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_timer_reg_0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_timer_reg_0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_timer_reg_0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP TIMER REG 1 INT MAP REG */
/** Type of lp_timer_reg_1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_timer_reg_1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_timer_reg_1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_timer_reg_1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_MB HP INT MAP REG */
/** Type of mb_hp_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_mb_hp_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_mb_hp_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_mb_hp_int_map_reg_t;


/** Group: INTERRUPT_CORE0_MB LP INT MAP REG */
/** Type of mb_lp_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_mb_lp_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_mb_lp_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_mb_lp_int_map_reg_t;


/** Group: INTERRUPT_CORE0_PMU REG 0 INT MAP REG */
/** Type of pmu_reg_0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_pmu_reg_0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_pmu_reg_0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_pmu_reg_0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_PMU REG 1 INT MAP REG */
/** Type of pmu_reg_1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_pmu_reg_1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_pmu_reg_1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_pmu_reg_1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP ANAPERI INT MAP REG */
/** Type of lp_anaperi_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_anaperi_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_anaperi_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_anaperi_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP ADC INT MAP REG */
/** Type of lp_adc_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_adc_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_adc_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_adc_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP GPIO INT MAP REG */
/** Type of lp_gpio_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_gpio_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_gpio_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_gpio_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP I2C INT MAP REG */
/** Type of lp_i2c_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_i2c_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_i2c_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_i2c_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP I2S INT MAP REG */
/** Type of lp_i2s_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_i2s_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_i2s_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_i2s_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP SPI INT MAP REG */
/** Type of lp_spi_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_spi_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_spi_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_spi_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP TOUCH INT MAP REG */
/** Type of lp_touch_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_touch_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_touch_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_touch_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP TSENS INT MAP REG */
/** Type of lp_tsens_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_tsens_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_tsens_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_tsens_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP UART INT MAP REG */
/** Type of lp_uart_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_uart_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_uart_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_uart_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP EFUSE INT MAP REG */
/** Type of lp_efuse_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_efuse_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_efuse_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_efuse_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP SW INT MAP REG */
/** Type of lp_sw_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_sw_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_sw_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_sw_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP SYSREG INT MAP REG */
/** Type of lp_sysreg_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_sysreg_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_sysreg_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_sysreg_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LP HUK INT MAP REG */
/** Type of lp_huk_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lp_huk_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lp_huk_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lp_huk_int_map_reg_t;


/** Group: INTERRUPT_CORE0_SYS ICM INT MAP REG */
/** Type of sys_icm_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_sys_icm_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_sys_icm_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_sys_icm_int_map_reg_t;


/** Group: INTERRUPT_CORE0_USB DEVICE INT MAP REG */
/** Type of usb_device_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_usb_device_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_usb_device_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_usb_device_int_map_reg_t;


/** Group: INTERRUPT_CORE0_SDIO HOST INT MAP REG */
/** Type of sdio_host_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_sdio_host_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_sdio_host_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_sdio_host_int_map_reg_t;


/** Group: INTERRUPT_CORE0_GDMA INT MAP REG */
/** Type of gdma_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_gdma_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_gdma_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_gdma_int_map_reg_t;


/** Group: INTERRUPT_CORE0_SPI2 INT MAP REG */
/** Type of spi2_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_spi2_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_spi2_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_spi2_int_map_reg_t;


/** Group: INTERRUPT_CORE0_SPI3 INT MAP REG */
/** Type of spi3_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_spi3_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_spi3_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_spi3_int_map_reg_t;


/** Group: INTERRUPT_CORE0_I2S0 INT MAP REG */
/** Type of i2s0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_i2s0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_i2s0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_i2s0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_I2S1 INT MAP REG */
/** Type of i2s1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_i2s1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_i2s1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_i2s1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_I2S2 INT MAP REG */
/** Type of i2s2_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_i2s2_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_i2s2_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_i2s2_int_map_reg_t;


/** Group: INTERRUPT_CORE0_UHCI0 INT MAP REG */
/** Type of uhci0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_uhci0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_uhci0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_uhci0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_UART0 INT MAP REG */
/** Type of uart0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_uart0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_uart0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_uart0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_UART1 INT MAP REG */
/** Type of uart1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_uart1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_uart1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_uart1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_UART2 INT MAP REG */
/** Type of uart2_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_uart2_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_uart2_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_uart2_int_map_reg_t;


/** Group: INTERRUPT_CORE0_UART3 INT MAP REG */
/** Type of uart3_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_uart3_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_uart3_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_uart3_int_map_reg_t;


/** Group: INTERRUPT_CORE0_UART4 INT MAP REG */
/** Type of uart4_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_uart4_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_uart4_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_uart4_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LCD CAM INT MAP REG */
/** Type of lcd_cam_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lcd_cam_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lcd_cam_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lcd_cam_int_map_reg_t;


/** Group: INTERRUPT_CORE0_ADC INT MAP REG */
/** Type of adc_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_adc_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_adc_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_adc_int_map_reg_t;


/** Group: INTERRUPT_CORE0_PWM0 INT MAP REG */
/** Type of pwm0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_pwm0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_pwm0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_pwm0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_PWM1 INT MAP REG */
/** Type of pwm1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_pwm1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_pwm1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_pwm1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_CAN0 INT MAP REG */
/** Type of can0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_can0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_can0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_can0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_CAN1 INT MAP REG */
/** Type of can1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_can1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_can1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_can1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_CAN2 INT MAP REG */
/** Type of can2_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_can2_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_can2_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_can2_int_map_reg_t;


/** Group: INTERRUPT_CORE0_RMT INT MAP REG */
/** Type of rmt_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_rmt_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_rmt_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_rmt_int_map_reg_t;


/** Group: INTERRUPT_CORE0_I2C0 INT MAP REG */
/** Type of i2c0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_i2c0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_i2c0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_i2c0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_I2C1 INT MAP REG */
/** Type of i2c1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_i2c1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_i2c1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_i2c1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_TIMERGRP0 T0 INT MAP REG */
/** Type of timergrp0_t0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_timergrp0_t0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_timergrp0_t0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_timergrp0_t0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_TIMERGRP0 T1 INT MAP REG */
/** Type of timergrp0_t1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_timergrp0_t1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_timergrp0_t1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_timergrp0_t1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_TIMERGRP0 WDT INT MAP REG */
/** Type of timergrp0_wdt_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_timergrp0_wdt_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_timergrp0_wdt_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_timergrp0_wdt_int_map_reg_t;


/** Group: INTERRUPT_CORE0_TIMERGRP1 T0 INT MAP REG */
/** Type of timergrp1_t0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_timergrp1_t0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_timergrp1_t0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_timergrp1_t0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_TIMERGRP1 T1 INT MAP REG */
/** Type of timergrp1_t1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_timergrp1_t1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_timergrp1_t1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_timergrp1_t1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_TIMERGRP1 WDT INT MAP REG */
/** Type of timergrp1_wdt_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_timergrp1_wdt_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_timergrp1_wdt_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_timergrp1_wdt_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LEDC INT MAP REG */
/** Type of ledc_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_ledc_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_ledc_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_ledc_int_map_reg_t;


/** Group: INTERRUPT_CORE0_SYSTIMER TARGET0 INT MAP REG */
/** Type of systimer_target0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_systimer_target0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_systimer_target0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_systimer_target0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_SYSTIMER TARGET1 INT MAP REG */
/** Type of systimer_target1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_systimer_target1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_systimer_target1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_systimer_target1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_SYSTIMER TARGET2 INT MAP REG */
/** Type of systimer_target2_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_systimer_target2_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_systimer_target2_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_systimer_target2_int_map_reg_t;


/** Group: INTERRUPT_CORE0_AHB PDMA IN CH0 INT MAP REG */
/** Type of ahb_pdma_in_ch0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_ahb_pdma_in_ch0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_ahb_pdma_in_ch0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_in_ch0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_AHB PDMA IN CH1 INT MAP REG */
/** Type of ahb_pdma_in_ch1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_ahb_pdma_in_ch1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_ahb_pdma_in_ch1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_in_ch1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_AHB PDMA IN CH2 INT MAP REG */
/** Type of ahb_pdma_in_ch2_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_ahb_pdma_in_ch2_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_ahb_pdma_in_ch2_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_in_ch2_int_map_reg_t;


/** Group: INTERRUPT_CORE0_AHB PDMA OUT CH0 INT MAP REG */
/** Type of ahb_pdma_out_ch0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_ahb_pdma_out_ch0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_ahb_pdma_out_ch0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_out_ch0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_AHB PDMA OUT CH1 INT MAP REG */
/** Type of ahb_pdma_out_ch1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_ahb_pdma_out_ch1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_ahb_pdma_out_ch1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_out_ch1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_AHB PDMA OUT CH2 INT MAP REG */
/** Type of ahb_pdma_out_ch2_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_ahb_pdma_out_ch2_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_ahb_pdma_out_ch2_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_ahb_pdma_out_ch2_int_map_reg_t;


/** Group: INTERRUPT_CORE0_AXI PDMA IN CH0 INT MAP REG */
/** Type of axi_pdma_in_ch0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_axi_pdma_in_ch0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_axi_pdma_in_ch0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_axi_pdma_in_ch0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_AXI PDMA IN CH1 INT MAP REG */
/** Type of axi_pdma_in_ch1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_axi_pdma_in_ch1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_axi_pdma_in_ch1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_axi_pdma_in_ch1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_AXI PDMA IN CH2 INT MAP REG */
/** Type of axi_pdma_in_ch2_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_axi_pdma_in_ch2_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_axi_pdma_in_ch2_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_axi_pdma_in_ch2_int_map_reg_t;


/** Group: INTERRUPT_CORE0_AXI PDMA OUT CH0 INT MAP REG */
/** Type of axi_pdma_out_ch0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_axi_pdma_out_ch0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_axi_pdma_out_ch0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_axi_pdma_out_ch0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_AXI PDMA OUT CH1 INT MAP REG */
/** Type of axi_pdma_out_ch1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_axi_pdma_out_ch1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_axi_pdma_out_ch1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_axi_pdma_out_ch1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_AXI PDMA OUT CH2 INT MAP REG */
/** Type of axi_pdma_out_ch2_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_axi_pdma_out_ch2_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_axi_pdma_out_ch2_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_axi_pdma_out_ch2_int_map_reg_t;


/** Group: INTERRUPT_CORE0_RSA INT MAP REG */
/** Type of rsa_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_rsa_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_rsa_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_rsa_int_map_reg_t;


/** Group: INTERRUPT_CORE0_AES INT MAP REG */
/** Type of aes_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_aes_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_aes_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_aes_int_map_reg_t;


/** Group: INTERRUPT_CORE0_SHA INT MAP REG */
/** Type of sha_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_sha_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_sha_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_sha_int_map_reg_t;


/** Group: INTERRUPT_CORE0_ECC INT MAP REG */
/** Type of ecc_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_ecc_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_ecc_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_ecc_int_map_reg_t;


/** Group: INTERRUPT_CORE0_ECDSA INT MAP REG */
/** Type of ecdsa_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_ecdsa_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_ecdsa_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_ecdsa_int_map_reg_t;


/** Group: INTERRUPT_CORE0_KM INT MAP REG */
/** Type of km_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_km_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_km_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_km_int_map_reg_t;


/** Group: INTERRUPT_CORE0_GPIO INT0 MAP REG */
/** Type of gpio_int0_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_gpio_int0_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_gpio_int0_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_gpio_int0_map_reg_t;


/** Group: INTERRUPT_CORE0_GPIO INT1 MAP REG */
/** Type of gpio_int1_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_gpio_int1_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_gpio_int1_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_gpio_int1_map_reg_t;


/** Group: INTERRUPT_CORE0_GPIO INT2 MAP REG */
/** Type of gpio_int2_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_gpio_int2_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_gpio_int2_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_gpio_int2_map_reg_t;


/** Group: INTERRUPT_CORE0_GPIO INT3 MAP REG */
/** Type of gpio_int3_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_gpio_int3_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_gpio_int3_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_gpio_int3_map_reg_t;


/** Group: INTERRUPT_CORE0_GPIO PAD COMP INT MAP REG */
/** Type of gpio_pad_comp_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_gpio_pad_comp_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_gpio_pad_comp_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_gpio_pad_comp_int_map_reg_t;


/** Group: INTERRUPT_CORE0_CPU INT FROM CPU 0 MAP REG */
/** Type of cpu_int_from_cpu_0_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_cpu_int_from_cpu_0_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_cpu_int_from_cpu_0_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_cpu_int_from_cpu_0_map_reg_t;


/** Group: INTERRUPT_CORE0_CPU INT FROM CPU 1 MAP REG */
/** Type of cpu_int_from_cpu_1_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_cpu_int_from_cpu_1_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_cpu_int_from_cpu_1_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_cpu_int_from_cpu_1_map_reg_t;


/** Group: INTERRUPT_CORE0_CPU INT FROM CPU 2 MAP REG */
/** Type of cpu_int_from_cpu_2_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_cpu_int_from_cpu_2_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_cpu_int_from_cpu_2_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_cpu_int_from_cpu_2_map_reg_t;


/** Group: INTERRUPT_CORE0_CPU INT FROM CPU 3 MAP REG */
/** Type of cpu_int_from_cpu_3_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_cpu_int_from_cpu_3_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_cpu_int_from_cpu_3_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_cpu_int_from_cpu_3_map_reg_t;


/** Group: INTERRUPT_CORE0_CACHE INT MAP REG */
/** Type of cache_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_cache_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_cache_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_cache_int_map_reg_t;


/** Group: INTERRUPT_CORE0_FLASH MSPI INT MAP REG */
/** Type of flash_mspi_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_flash_mspi_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_flash_mspi_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_flash_mspi_int_map_reg_t;


/** Group: INTERRUPT_CORE0_CSI BRIDGE INT MAP REG */
/** Type of csi_bridge_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_csi_bridge_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_csi_bridge_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_csi_bridge_int_map_reg_t;


/** Group: INTERRUPT_CORE0_DSI BRIDGE INT MAP REG */
/** Type of dsi_bridge_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_dsi_bridge_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_dsi_bridge_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_dsi_bridge_int_map_reg_t;


/** Group: INTERRUPT_CORE0_CSI INT MAP REG */
/** Type of csi_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_csi_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_csi_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_csi_int_map_reg_t;


/** Group: INTERRUPT_CORE0_DSI INT MAP REG */
/** Type of dsi_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_dsi_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_dsi_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_dsi_int_map_reg_t;


/** Group: INTERRUPT_CORE0_GMII PHY INT MAP REG */
/** Type of gmii_phy_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_gmii_phy_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_gmii_phy_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_gmii_phy_int_map_reg_t;


/** Group: INTERRUPT_CORE0_LPI INT MAP REG */
/** Type of lpi_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_lpi_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_lpi_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_lpi_int_map_reg_t;


/** Group: INTERRUPT_CORE0_PMT INT MAP REG */
/** Type of pmt_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_pmt_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_pmt_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_pmt_int_map_reg_t;


/** Group: INTERRUPT_CORE0_SBD INT MAP REG */
/** Type of sbd_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_sbd_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_sbd_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_sbd_int_map_reg_t;


/** Group: INTERRUPT_CORE0_USB OTG INT MAP REG */
/** Type of usb_otg_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_usb_otg_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_usb_otg_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_usb_otg_int_map_reg_t;


/** Group: INTERRUPT_CORE0_USB OTG ENDP MULTI PROC INT MAP REG */
/** Type of usb_otg_endp_multi_proc_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_usb_otg_endp_multi_proc_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_usb_otg_endp_multi_proc_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_usb_otg_endp_multi_proc_int_map_reg_t;


/** Group: INTERRUPT_CORE0_JPEG INT MAP REG */
/** Type of jpeg_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_jpeg_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_jpeg_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_jpeg_int_map_reg_t;


/** Group: INTERRUPT_CORE0_PPA INT MAP REG */
/** Type of ppa_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_ppa_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_ppa_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_ppa_int_map_reg_t;


/** Group: INTERRUPT_CORE0_INTERRUPT_CORE0_TRACE INT MAP REG */
/** Type of interrupt_core0_trace_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_trace_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_trace_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_trace_int_map_reg_t;


/** Group: INTERRUPT_CORE0_CORE1 TRACE INT MAP REG */
/** Type of core1_trace_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_core1_trace_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_core1_trace_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_core1_trace_int_map_reg_t;


/** Group: INTERRUPT_CORE0_HP CORE CTRL INT MAP REG */
/** Type of hp_core_ctrl_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_hp_core_ctrl_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_hp_core_ctrl_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_hp_core_ctrl_int_map_reg_t;


/** Group: INTERRUPT_CORE0_ISP INT MAP REG */
/** Type of isp_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_isp_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_isp_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_isp_int_map_reg_t;


/** Group: INTERRUPT_CORE0_I3C MST INT MAP REG */
/** Type of i3c_mst_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_i3c_mst_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_i3c_mst_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_i3c_mst_int_map_reg_t;


/** Group: INTERRUPT_CORE0_I3C SLV INT MAP REG */
/** Type of i3c_slv_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_i3c_slv_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_i3c_slv_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_i3c_slv_int_map_reg_t;


/** Group: INTERRUPT_CORE0_USB OTG11 INT MAP REG */
/** Type of usb_otg11_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_usb_otg11_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_usb_otg11_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_usb_otg11_int_map_reg_t;


/** Group: INTERRUPT_CORE0_DMA2D IN CH0 INT MAP REG */
/** Type of dma2d_in_ch0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_dma2d_in_ch0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_dma2d_in_ch0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_dma2d_in_ch0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_DMA2D IN CH1 INT MAP REG */
/** Type of dma2d_in_ch1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_dma2d_in_ch1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_dma2d_in_ch1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_dma2d_in_ch1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_DMA2D OUT CH0 INT MAP REG */
/** Type of dma2d_out_ch0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_dma2d_out_ch0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_dma2d_out_ch0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_dma2d_out_ch0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_DMA2D OUT CH1 INT MAP REG */
/** Type of dma2d_out_ch1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_dma2d_out_ch1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_dma2d_out_ch1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_dma2d_out_ch1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_DMA2D OUT CH2 INT MAP REG */
/** Type of dma2d_out_ch2_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_dma2d_out_ch2_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_dma2d_out_ch2_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_dma2d_out_ch2_int_map_reg_t;


/** Group: INTERRUPT_CORE0_PSRAM MSPI INT MAP REG */
/** Type of psram_mspi_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_psram_mspi_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_psram_mspi_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_psram_mspi_int_map_reg_t;


/** Group: INTERRUPT_CORE0_HP SYSREG INT MAP REG */
/** Type of hp_sysreg_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_hp_sysreg_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_hp_sysreg_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_hp_sysreg_int_map_reg_t;


/** Group: INTERRUPT_CORE0_PCNT INT MAP REG */
/** Type of pcnt_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_pcnt_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_pcnt_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_pcnt_int_map_reg_t;


/** Group: INTERRUPT_CORE0_HP PAU INT MAP REG */
/** Type of hp_pau_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_hp_pau_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_hp_pau_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_hp_pau_int_map_reg_t;


/** Group: INTERRUPT_CORE0_HP PARLIO RX INT MAP REG */
/** Type of hp_parlio_rx_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_hp_parlio_rx_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_hp_parlio_rx_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_hp_parlio_rx_int_map_reg_t;


/** Group: INTERRUPT_CORE0_HP PARLIO TX INT MAP REG */
/** Type of hp_parlio_tx_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_hp_parlio_tx_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_hp_parlio_tx_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_hp_parlio_tx_int_map_reg_t;


/** Group: INTERRUPT_CORE0_H264 DMA2D OUT CH0 INT MAP REG */
/** Type of h264_dma2d_out_ch0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_h264_dma2d_out_ch0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_h264_dma2d_out_ch0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_h264_dma2d_out_ch0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_H264 DMA2D OUT CH1 INT MAP REG */
/** Type of h264_dma2d_out_ch1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_h264_dma2d_out_ch1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_h264_dma2d_out_ch1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_h264_dma2d_out_ch1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_H264 DMA2D OUT CH2 INT MAP REG */
/** Type of h264_dma2d_out_ch2_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_h264_dma2d_out_ch2_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_h264_dma2d_out_ch2_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_h264_dma2d_out_ch2_int_map_reg_t;


/** Group: INTERRUPT_CORE0_H264 DMA2D OUT CH3 INT MAP REG */
/** Type of h264_dma2d_out_ch3_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_h264_dma2d_out_ch3_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_h264_dma2d_out_ch3_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_h264_dma2d_out_ch3_int_map_reg_t;


/** Group: INTERRUPT_CORE0_H264 DMA2D OUT CH4 INT MAP REG */
/** Type of h264_dma2d_out_ch4_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_h264_dma2d_out_ch4_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_h264_dma2d_out_ch4_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_h264_dma2d_out_ch4_int_map_reg_t;


/** Group: INTERRUPT_CORE0_H264 DMA2D IN CH0 INT MAP REG */
/** Type of h264_dma2d_in_ch0_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_h264_dma2d_in_ch0_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_h264_dma2d_in_ch0_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_h264_dma2d_in_ch0_int_map_reg_t;


/** Group: INTERRUPT_CORE0_H264 DMA2D IN CH1 INT MAP REG */
/** Type of h264_dma2d_in_ch1_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_h264_dma2d_in_ch1_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_h264_dma2d_in_ch1_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_h264_dma2d_in_ch1_int_map_reg_t;


/** Group: INTERRUPT_CORE0_H264 DMA2D IN CH2 INT MAP REG */
/** Type of h264_dma2d_in_ch2_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_h264_dma2d_in_ch2_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_h264_dma2d_in_ch2_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_h264_dma2d_in_ch2_int_map_reg_t;


/** Group: INTERRUPT_CORE0_H264 DMA2D IN CH3 INT MAP REG */
/** Type of h264_dma2d_in_ch3_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_h264_dma2d_in_ch3_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_h264_dma2d_in_ch3_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_h264_dma2d_in_ch3_int_map_reg_t;


/** Group: INTERRUPT_CORE0_H264 DMA2D IN CH4 INT MAP REG */
/** Type of h264_dma2d_in_ch4_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_h264_dma2d_in_ch4_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_h264_dma2d_in_ch4_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_h264_dma2d_in_ch4_int_map_reg_t;


/** Group: INTERRUPT_CORE0_H264 DMA2D IN CH5 INT MAP REG */
/** Type of h264_dma2d_in_ch5_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_h264_dma2d_in_ch5_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_h264_dma2d_in_ch5_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_h264_dma2d_in_ch5_int_map_reg_t;


/** Group: INTERRUPT_CORE0_H264 REG INT MAP REG */
/** Type of h264_reg_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_h264_reg_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_h264_reg_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_h264_reg_int_map_reg_t;


/** Group: INTERRUPT_CORE0_ASSIST DEBUG INT MAP REG */
/** Type of assist_debug_int_map register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_assist_debug_int_map : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_assist_debug_int_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_assist_debug_int_map_reg_t;


/** Group: INTERRUPT_CORE0_INTR STATUS REG 0 REG */
/** Type of intr_status_reg_0 register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_intr_status_0 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_intr_status_0:32;
    };
    uint32_t val;
} interrupt_core0_intr_status_reg_0_reg_t;


/** Group: INTERRUPT_CORE0_INTR STATUS REG 1 REG */
/** Type of intr_status_reg_1 register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_intr_status_1 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_intr_status_1:32;
    };
    uint32_t val;
} interrupt_core0_intr_status_reg_1_reg_t;


/** Group: INTERRUPT_CORE0_INTR STATUS REG 2 REG */
/** Type of intr_status_reg_2 register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_intr_status_2 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_intr_status_2:32;
    };
    uint32_t val;
} interrupt_core0_intr_status_reg_2_reg_t;


/** Group: INTERRUPT_CORE0_INTR STATUS REG 3 REG */
/** Type of intr_status_reg_3 register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_intr_status_3 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t interrupt_core0_intr_status_3:32;
    };
    uint32_t val;
} interrupt_core0_intr_status_reg_3_reg_t;


/** Group: INTERRUPT_CORE0_CLOCK GATE REG */
/** Type of clock_gate register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_reg_clk_en : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t interrupt_core0_reg_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} interrupt_core0_clock_gate_reg_t;


/** Group: INTERRUPT_CORE0_INTERRUPT REG DATE REG */
/** Type of interrupt_reg_date register
 *  NA
 */
typedef union {
    struct {
        /** interrupt_core0_interrupt_reg_date : R/W; bitpos: [27:0]; default: 33566752;
         *  NA
         */
        uint32_t interrupt_core0_interrupt_reg_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} interrupt_core0_interrupt_reg_date_reg_t;


typedef struct {
    volatile interrupt_core0_lp_rtc_int_map_reg_t lp_rtc_int_map;
    volatile interrupt_core0_lp_wdt_int_map_reg_t lp_wdt_int_map;
    volatile interrupt_core0_lp_timer_reg_0_int_map_reg_t lp_timer_reg_0_int_map;
    volatile interrupt_core0_lp_timer_reg_1_int_map_reg_t lp_timer_reg_1_int_map;
    volatile interrupt_core0_mb_hp_int_map_reg_t mb_hp_int_map;
    volatile interrupt_core0_mb_lp_int_map_reg_t mb_lp_int_map;
    volatile interrupt_core0_pmu_reg_0_int_map_reg_t pmu_reg_0_int_map;
    volatile interrupt_core0_pmu_reg_1_int_map_reg_t pmu_reg_1_int_map;
    volatile interrupt_core0_lp_anaperi_int_map_reg_t lp_anaperi_int_map;
    volatile interrupt_core0_lp_adc_int_map_reg_t lp_adc_int_map;
    volatile interrupt_core0_lp_gpio_int_map_reg_t lp_gpio_int_map;
    volatile interrupt_core0_lp_i2c_int_map_reg_t lp_i2c_int_map;
    volatile interrupt_core0_lp_i2s_int_map_reg_t lp_i2s_int_map;
    volatile interrupt_core0_lp_spi_int_map_reg_t lp_spi_int_map;
    volatile interrupt_core0_lp_touch_int_map_reg_t lp_touch_int_map;
    volatile interrupt_core0_lp_tsens_int_map_reg_t lp_tsens_int_map;
    volatile interrupt_core0_lp_uart_int_map_reg_t lp_uart_int_map;
    volatile interrupt_core0_lp_efuse_int_map_reg_t lp_efuse_int_map;
    volatile interrupt_core0_lp_sw_int_map_reg_t lp_sw_int_map;
    volatile interrupt_core0_lp_sysreg_int_map_reg_t lp_sysreg_int_map;
    volatile interrupt_core0_lp_huk_int_map_reg_t lp_huk_int_map;
    volatile interrupt_core0_sys_icm_int_map_reg_t sys_icm_int_map;
    volatile interrupt_core0_usb_device_int_map_reg_t usb_device_int_map;
    volatile interrupt_core0_sdio_host_int_map_reg_t sdio_host_int_map;
    volatile interrupt_core0_gdma_int_map_reg_t gdma_int_map;
    volatile interrupt_core0_spi2_int_map_reg_t spi2_int_map;
    volatile interrupt_core0_spi3_int_map_reg_t spi3_int_map;
    volatile interrupt_core0_i2s0_int_map_reg_t i2s0_int_map;
    volatile interrupt_core0_i2s1_int_map_reg_t i2s1_int_map;
    volatile interrupt_core0_i2s2_int_map_reg_t i2s2_int_map;
    volatile interrupt_core0_uhci0_int_map_reg_t uhci0_int_map;
    volatile interrupt_core0_uart0_int_map_reg_t uart0_int_map;
    volatile interrupt_core0_uart1_int_map_reg_t uart1_int_map;
    volatile interrupt_core0_uart2_int_map_reg_t uart2_int_map;
    volatile interrupt_core0_uart3_int_map_reg_t uart3_int_map;
    volatile interrupt_core0_uart4_int_map_reg_t uart4_int_map;
    volatile interrupt_core0_lcd_cam_int_map_reg_t lcd_cam_int_map;
    volatile interrupt_core0_adc_int_map_reg_t adc_int_map;
    volatile interrupt_core0_pwm0_int_map_reg_t pwm0_int_map;
    volatile interrupt_core0_pwm1_int_map_reg_t pwm1_int_map;
    volatile interrupt_core0_can0_int_map_reg_t can0_int_map;
    volatile interrupt_core0_can1_int_map_reg_t can1_int_map;
    volatile interrupt_core0_can2_int_map_reg_t can2_int_map;
    volatile interrupt_core0_rmt_int_map_reg_t rmt_int_map;
    volatile interrupt_core0_i2c0_int_map_reg_t i2c0_int_map;
    volatile interrupt_core0_i2c1_int_map_reg_t i2c1_int_map;
    volatile interrupt_core0_timergrp0_t0_int_map_reg_t timergrp0_t0_int_map;
    volatile interrupt_core0_timergrp0_t1_int_map_reg_t timergrp0_t1_int_map;
    volatile interrupt_core0_timergrp0_wdt_int_map_reg_t timergrp0_wdt_int_map;
    volatile interrupt_core0_timergrp1_t0_int_map_reg_t timergrp1_t0_int_map;
    volatile interrupt_core0_timergrp1_t1_int_map_reg_t timergrp1_t1_int_map;
    volatile interrupt_core0_timergrp1_wdt_int_map_reg_t timergrp1_wdt_int_map;
    volatile interrupt_core0_ledc_int_map_reg_t ledc_int_map;
    volatile interrupt_core0_systimer_target0_int_map_reg_t systimer_target0_int_map;
    volatile interrupt_core0_systimer_target1_int_map_reg_t systimer_target1_int_map;
    volatile interrupt_core0_systimer_target2_int_map_reg_t systimer_target2_int_map;
    volatile interrupt_core0_ahb_pdma_in_ch0_int_map_reg_t ahb_pdma_in_ch0_int_map;
    volatile interrupt_core0_ahb_pdma_in_ch1_int_map_reg_t ahb_pdma_in_ch1_int_map;
    volatile interrupt_core0_ahb_pdma_in_ch2_int_map_reg_t ahb_pdma_in_ch2_int_map;
    volatile interrupt_core0_ahb_pdma_out_ch0_int_map_reg_t ahb_pdma_out_ch0_int_map;
    volatile interrupt_core0_ahb_pdma_out_ch1_int_map_reg_t ahb_pdma_out_ch1_int_map;
    volatile interrupt_core0_ahb_pdma_out_ch2_int_map_reg_t ahb_pdma_out_ch2_int_map;
    volatile interrupt_core0_axi_pdma_in_ch0_int_map_reg_t axi_pdma_in_ch0_int_map;
    volatile interrupt_core0_axi_pdma_in_ch1_int_map_reg_t axi_pdma_in_ch1_int_map;
    volatile interrupt_core0_axi_pdma_in_ch2_int_map_reg_t axi_pdma_in_ch2_int_map;
    volatile interrupt_core0_axi_pdma_out_ch0_int_map_reg_t axi_pdma_out_ch0_int_map;
    volatile interrupt_core0_axi_pdma_out_ch1_int_map_reg_t axi_pdma_out_ch1_int_map;
    volatile interrupt_core0_axi_pdma_out_ch2_int_map_reg_t axi_pdma_out_ch2_int_map;
    volatile interrupt_core0_rsa_int_map_reg_t rsa_int_map;
    volatile interrupt_core0_aes_int_map_reg_t aes_int_map;
    volatile interrupt_core0_sha_int_map_reg_t sha_int_map;
    volatile interrupt_core0_ecc_int_map_reg_t ecc_int_map;
    volatile interrupt_core0_ecdsa_int_map_reg_t ecdsa_int_map;
    volatile interrupt_core0_km_int_map_reg_t km_int_map;
    volatile interrupt_core0_gpio_int0_map_reg_t gpio_int0_map;
    volatile interrupt_core0_gpio_int1_map_reg_t gpio_int1_map;
    volatile interrupt_core0_gpio_int2_map_reg_t gpio_int2_map;
    volatile interrupt_core0_gpio_int3_map_reg_t gpio_int3_map;
    volatile interrupt_core0_gpio_pad_comp_int_map_reg_t gpio_pad_comp_int_map;
    volatile interrupt_core0_cpu_int_from_cpu_0_map_reg_t cpu_int_from_cpu_0_map;
    volatile interrupt_core0_cpu_int_from_cpu_1_map_reg_t cpu_int_from_cpu_1_map;
    volatile interrupt_core0_cpu_int_from_cpu_2_map_reg_t cpu_int_from_cpu_2_map;
    volatile interrupt_core0_cpu_int_from_cpu_3_map_reg_t cpu_int_from_cpu_3_map;
    volatile interrupt_core0_cache_int_map_reg_t cache_int_map;
    volatile interrupt_core0_flash_mspi_int_map_reg_t flash_mspi_int_map;
    volatile interrupt_core0_csi_bridge_int_map_reg_t csi_bridge_int_map;
    volatile interrupt_core0_dsi_bridge_int_map_reg_t dsi_bridge_int_map;
    volatile interrupt_core0_csi_int_map_reg_t csi_int_map;
    volatile interrupt_core0_dsi_int_map_reg_t dsi_int_map;
    volatile interrupt_core0_gmii_phy_int_map_reg_t gmii_phy_int_map;
    volatile interrupt_core0_lpi_int_map_reg_t lpi_int_map;
    volatile interrupt_core0_pmt_int_map_reg_t pmt_int_map;
    volatile interrupt_core0_sbd_int_map_reg_t sbd_int_map;
    volatile interrupt_core0_usb_otg_int_map_reg_t usb_otg_int_map;
    volatile interrupt_core0_usb_otg_endp_multi_proc_int_map_reg_t usb_otg_endp_multi_proc_int_map;
    volatile interrupt_core0_jpeg_int_map_reg_t jpeg_int_map;
    volatile interrupt_core0_ppa_int_map_reg_t ppa_int_map;
    volatile interrupt_core0_trace_int_map_reg_t interrupt_core0_trace_int_map;
    volatile interrupt_core0_core1_trace_int_map_reg_t core1_trace_int_map;
    volatile interrupt_core0_hp_core_ctrl_int_map_reg_t hp_core_ctrl_int_map;
    volatile interrupt_core0_isp_int_map_reg_t isp_int_map;
    volatile interrupt_core0_i3c_mst_int_map_reg_t i3c_mst_int_map;
    volatile interrupt_core0_i3c_slv_int_map_reg_t i3c_slv_int_map;
    volatile interrupt_core0_usb_otg11_int_map_reg_t usb_otg11_int_map;
    volatile interrupt_core0_dma2d_in_ch0_int_map_reg_t dma2d_in_ch0_int_map;
    volatile interrupt_core0_dma2d_in_ch1_int_map_reg_t dma2d_in_ch1_int_map;
    volatile interrupt_core0_dma2d_out_ch0_int_map_reg_t dma2d_out_ch0_int_map;
    volatile interrupt_core0_dma2d_out_ch1_int_map_reg_t dma2d_out_ch1_int_map;
    volatile interrupt_core0_dma2d_out_ch2_int_map_reg_t dma2d_out_ch2_int_map;
    volatile interrupt_core0_psram_mspi_int_map_reg_t psram_mspi_int_map;
    volatile interrupt_core0_hp_sysreg_int_map_reg_t hp_sysreg_int_map;
    volatile interrupt_core0_pcnt_int_map_reg_t pcnt_int_map;
    volatile interrupt_core0_hp_pau_int_map_reg_t hp_pau_int_map;
    volatile interrupt_core0_hp_parlio_rx_int_map_reg_t hp_parlio_rx_int_map;
    volatile interrupt_core0_hp_parlio_tx_int_map_reg_t hp_parlio_tx_int_map;
    volatile interrupt_core0_h264_dma2d_out_ch0_int_map_reg_t h264_dma2d_out_ch0_int_map;
    volatile interrupt_core0_h264_dma2d_out_ch1_int_map_reg_t h264_dma2d_out_ch1_int_map;
    volatile interrupt_core0_h264_dma2d_out_ch2_int_map_reg_t h264_dma2d_out_ch2_int_map;
    volatile interrupt_core0_h264_dma2d_out_ch3_int_map_reg_t h264_dma2d_out_ch3_int_map;
    volatile interrupt_core0_h264_dma2d_out_ch4_int_map_reg_t h264_dma2d_out_ch4_int_map;
    volatile interrupt_core0_h264_dma2d_in_ch0_int_map_reg_t h264_dma2d_in_ch0_int_map;
    volatile interrupt_core0_h264_dma2d_in_ch1_int_map_reg_t h264_dma2d_in_ch1_int_map;
    volatile interrupt_core0_h264_dma2d_in_ch2_int_map_reg_t h264_dma2d_in_ch2_int_map;
    volatile interrupt_core0_h264_dma2d_in_ch3_int_map_reg_t h264_dma2d_in_ch3_int_map;
    volatile interrupt_core0_h264_dma2d_in_ch4_int_map_reg_t h264_dma2d_in_ch4_int_map;
    volatile interrupt_core0_h264_dma2d_in_ch5_int_map_reg_t h264_dma2d_in_ch5_int_map;
    volatile interrupt_core0_h264_reg_int_map_reg_t h264_reg_int_map;
    volatile interrupt_core0_assist_debug_int_map_reg_t assist_debug_int_map;
    volatile interrupt_core0_intr_status_reg_0_reg_t intr_status_reg_0;
    volatile interrupt_core0_intr_status_reg_1_reg_t intr_status_reg_1;
    volatile interrupt_core0_intr_status_reg_2_reg_t intr_status_reg_2;
    volatile interrupt_core0_intr_status_reg_3_reg_t intr_status_reg_3;
    volatile interrupt_core0_clock_gate_reg_t clock_gate;
    uint32_t reserved_214[122];
    volatile interrupt_core0_interrupt_reg_date_reg_t interrupt_reg_date;
} interrupt_core0_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(interrupt_core0_dev_t) == 0x400, "Invalid size of interrupt_core0_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
