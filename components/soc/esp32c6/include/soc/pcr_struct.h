/**
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Register */
/** Type of uart0_conf register
 *  UART0 configuration register
 */
typedef union {
    struct {
        /** uart0_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable uart0 apb clock
         */
        uint32_t uart0_clk_en:1;
        /** uart0_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset uart0 module
         */
        uint32_t uart0_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_uart0_conf_reg_t;

/** Type of uart0_sclk_conf register
 *  UART0_SCLK configuration register
 */
typedef union {
    struct {
        /** uart0_sclk_div_a : R/W; bitpos: [5:0]; default: 0;
         *  The  denominator of the frequency divider factor of the uart0 function clock.
         */
        uint32_t uart0_sclk_div_a:6;
        /** uart0_sclk_div_b : R/W; bitpos: [11:6]; default: 0;
         *  The numerator of the frequency divider factor of the uart0 function clock.
         */
        uint32_t uart0_sclk_div_b:6;
        /** uart0_sclk_div_num : R/W; bitpos: [19:12]; default: 0;
         *  The integral part of the frequency divider factor of the uart0 function clock.
         */
        uint32_t uart0_sclk_div_num:8;
        /** uart0_sclk_sel : R/W; bitpos: [21:20]; default: 3;
         *  set this field to select clock-source. 0: do not select anyone clock, 1: 80MHz, 2:
         *  FOSC, 3(default): XTAL.
         */
        uint32_t uart0_sclk_sel:2;
        /** uart0_sclk_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable uart0 function clock
         */
        uint32_t uart0_sclk_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_uart0_sclk_conf_reg_t;

/** Type of uart0_pd_ctrl register
 *  UART0 power control register
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** uart0_mem_force_pu : R/W; bitpos: [1]; default: 1;
         *  Set this bit to force power down UART0 memory.
         */
        uint32_t uart0_mem_force_pu:1;
        /** uart0_mem_force_pd : R/W; bitpos: [2]; default: 0;
         *  Set this bit to force power up UART0 memory.
         */
        uint32_t uart0_mem_force_pd:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} pcr_uart0_pd_ctrl_reg_t;

/** Type of uart1_conf register
 *  UART1 configuration register
 */
typedef union {
    struct {
        /** uart1_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable uart1 apb clock
         */
        uint32_t uart1_clk_en:1;
        /** uart1_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset uart1 module
         */
        uint32_t uart1_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_uart1_conf_reg_t;

/** Type of uart1_sclk_conf register
 *  UART1_SCLK configuration register
 */
typedef union {
    struct {
        /** uart1_sclk_div_a : R/W; bitpos: [5:0]; default: 0;
         *  The  denominator of the frequency divider factor of the uart1 function clock.
         */
        uint32_t uart1_sclk_div_a:6;
        /** uart1_sclk_div_b : R/W; bitpos: [11:6]; default: 0;
         *  The numerator of the frequency divider factor of the uart1 function clock.
         */
        uint32_t uart1_sclk_div_b:6;
        /** uart1_sclk_div_num : R/W; bitpos: [19:12]; default: 0;
         *  The integral part of the frequency divider factor of the uart1 function clock.
         */
        uint32_t uart1_sclk_div_num:8;
        /** uart1_sclk_sel : R/W; bitpos: [21:20]; default: 3;
         *  set this field to select clock-source. 0: do not select anyone clock, 1: 80MHz, 2:
         *  FOSC, 3(default): XTAL.
         */
        uint32_t uart1_sclk_sel:2;
        /** uart1_sclk_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable uart0 function clock
         */
        uint32_t uart1_sclk_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_uart1_sclk_conf_reg_t;

/** Type of uart1_pd_ctrl register
 *  UART1 power control register
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** uart1_mem_force_pu : R/W; bitpos: [1]; default: 1;
         *  Set this bit to force power down UART1 memory.
         */
        uint32_t uart1_mem_force_pu:1;
        /** uart1_mem_force_pd : R/W; bitpos: [2]; default: 0;
         *  Set this bit to force power up UART1 memory.
         */
        uint32_t uart1_mem_force_pd:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} pcr_uart1_pd_ctrl_reg_t;

/** Type of mspi_conf register
 *  MSPI configuration register
 */
typedef union {
    struct {
        /** mspi_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable mspi clock, include mspi pll clock
         */
        uint32_t mspi_clk_en:1;
        /** mspi_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset mspi module
         */
        uint32_t mspi_rst_en:1;
        /** mspi_pll_clk_en : R/W; bitpos: [2]; default: 1;
         *  Set 1 to enable mspi pll clock
         */
        uint32_t mspi_pll_clk_en:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} pcr_mspi_conf_reg_t;

/** Type of mspi_clk_conf register
 *  MSPI_CLK configuration register
 */
typedef union {
    struct {
        /** mspi_fast_ls_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Set as one within (0,1,2) to generate div1(default)/div2/div4 of low-speed
         *  clock-source to drive clk_mspi_fast. Only avaiable whe the clck-source is a
         *  low-speed clock-source such as XTAL/FOSC.
         */
        uint32_t mspi_fast_ls_div_num:8;
        /** mspi_fast_hs_div_num : R/W; bitpos: [15:8]; default: 3;
         *  Set as one within (3,4,5) to generate div4(default)/div5/div6 of high-speed
         *  clock-source to drive clk_mspi_fast. Only avaiable whe the clck-source is a
         *  high-speed clock-source such as SPLL.
         */
        uint32_t mspi_fast_hs_div_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} pcr_mspi_clk_conf_reg_t;

/** Type of i2c_conf register
 *  I2C configuration register
 */
typedef union {
    struct {
        /** i2c_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable i2c apb clock
         */
        uint32_t i2c_clk_en:1;
        /** i2c_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset i2c module
         */
        uint32_t i2c_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_i2c_conf_reg_t;

/** Type of i2c_sclk_conf register
 *  I2C_SCLK configuration register
 */
typedef union {
    struct {
        /** i2c_sclk_div_a : R/W; bitpos: [5:0]; default: 0;
         *  The  denominator of the frequency divider factor of the i2c function clock.
         */
        uint32_t i2c_sclk_div_a:6;
        /** i2c_sclk_div_b : R/W; bitpos: [11:6]; default: 0;
         *  The numerator of the frequency divider factor of the i2c function clock.
         */
        uint32_t i2c_sclk_div_b:6;
        /** i2c_sclk_div_num : R/W; bitpos: [19:12]; default: 0;
         *  The integral part of the frequency divider factor of the i2c function clock.
         */
        uint32_t i2c_sclk_div_num:8;
        /** i2c_sclk_sel : R/W; bitpos: [20]; default: 0;
         *  set this field to select clock-source. 0(default): XTAL, 1: FOSC.
         */
        uint32_t i2c_sclk_sel:1;
        uint32_t reserved_21:1;
        /** i2c_sclk_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable i2c function clock
         */
        uint32_t i2c_sclk_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_i2c_sclk_conf_reg_t;

/** Type of uhci_conf register
 *  UHCI configuration register
 */
typedef union {
    struct {
        /** uhci_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable uhci clock
         */
        uint32_t uhci_clk_en:1;
        /** uhci_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset uhci module
         */
        uint32_t uhci_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_uhci_conf_reg_t;

/** Type of rmt_conf register
 *  RMT configuration register
 */
typedef union {
    struct {
        /** rmt_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable rmt apb clock
         */
        uint32_t rmt_clk_en:1;
        /** rmt_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset rmt module
         */
        uint32_t rmt_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_rmt_conf_reg_t;

/** Type of rmt_sclk_conf register
 *  RMT_SCLK configuration register
 */
typedef union {
    struct {
        /** rmt_sclk_div_a : R/W; bitpos: [5:0]; default: 0;
         *  The  denominator of the frequency divider factor of the rmt function clock.
         */
        uint32_t rmt_sclk_div_a:6;
        /** rmt_sclk_div_b : R/W; bitpos: [11:6]; default: 0;
         *  The numerator of the frequency divider factor of the rmt function clock.
         */
        uint32_t rmt_sclk_div_b:6;
        /** rmt_sclk_div_num : R/W; bitpos: [19:12]; default: 1;
         *  The integral part of the frequency divider factor of the rmt function clock.
         */
        uint32_t rmt_sclk_div_num:8;
        /** rmt_sclk_sel : R/W; bitpos: [21:20]; default: 1;
         *  set this field to select clock-source. 0: do not select anyone clock, 1(default):
         *  80MHz, 2: FOSC, 3: XTAL.
         */
        uint32_t rmt_sclk_sel:2;
        /** rmt_sclk_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable rmt function clock
         */
        uint32_t rmt_sclk_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_rmt_sclk_conf_reg_t;

/** Type of ledc_conf register
 *  LEDC configuration register
 */
typedef union {
    struct {
        /** ledc_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable ledc apb clock
         */
        uint32_t ledc_clk_en:1;
        /** ledc_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset ledc module
         */
        uint32_t ledc_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_ledc_conf_reg_t;

/** Type of ledc_sclk_conf register
 *  LEDC_SCLK configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** ledc_sclk_sel : R/W; bitpos: [21:20]; default: 0;
         *  set this field to select clock-source. 0(default): do not select anyone clock, 1:
         *  80MHz, 2: FOSC, 3: XTAL.
         */
        uint32_t ledc_sclk_sel:2;
        /** ledc_sclk_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable ledc function clock
         */
        uint32_t ledc_sclk_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_ledc_sclk_conf_reg_t;

/** Type of timergroup0_conf register
 *  TIMERGROUP0 configuration register
 */
typedef union {
    struct {
        /** tg0_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable timer_group0 apb clock
         */
        uint32_t tg0_clk_en:1;
        /** tg0_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset timer_group0 module
         */
        uint32_t tg0_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_timergroup0_conf_reg_t;

/** Type of timergroup0_timer_clk_conf register
 *  TIMERGROUP0_TIMER_CLK configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** tg0_timer_clk_sel : R/W; bitpos: [21:20]; default: 0;
         *  set this field to select clock-source. 0(default): XTAL, 1: 80MHz, 2: FOSC, 3:
         *  reserved.
         */
        uint32_t tg0_timer_clk_sel:2;
        /** tg0_timer_clk_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable timer_group0 timer clock
         */
        uint32_t tg0_timer_clk_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_timergroup0_timer_clk_conf_reg_t;

/** Type of timergroup0_wdt_clk_conf register
 *  TIMERGROUP0_WDT_CLK configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** tg0_wdt_clk_sel : R/W; bitpos: [21:20]; default: 0;
         *  set this field to select clock-source. 0(default): XTAL, 1: 80MHz, 2: FOSC, 3:
         *  reserved.
         */
        uint32_t tg0_wdt_clk_sel:2;
        /** tg0_wdt_clk_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable timer_group0 wdt clock
         */
        uint32_t tg0_wdt_clk_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_timergroup0_wdt_clk_conf_reg_t;

/** Type of timergroup1_conf register
 *  TIMERGROUP1 configuration register
 */
typedef union {
    struct {
        /** tg1_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable timer_group1 apb clock
         */
        uint32_t tg1_clk_en:1;
        /** tg1_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset timer_group1 module
         */
        uint32_t tg1_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_timergroup1_conf_reg_t;

/** Type of timergroup1_timer_clk_conf register
 *  TIMERGROUP1_TIMER_CLK configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** tg1_timer_clk_sel : R/W; bitpos: [21:20]; default: 0;
         *  set this field to select clock-source. 0(default): XTAL, 1: 80MHz, 2: FOSC, 3:
         *  reserved.
         */
        uint32_t tg1_timer_clk_sel:2;
        /** tg1_timer_clk_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable timer_group1 timer clock
         */
        uint32_t tg1_timer_clk_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_timergroup1_timer_clk_conf_reg_t;

/** Type of timergroup1_wdt_clk_conf register
 *  TIMERGROUP1_WDT_CLK configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** tg1_wdt_clk_sel : R/W; bitpos: [21:20]; default: 0;
         *  set this field to select clock-source. 0(default): XTAL, 1: 80MHz, 2: FOSC, 3:
         *  reserved.
         */
        uint32_t tg1_wdt_clk_sel:2;
        /** tg1_wdt_clk_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable timer_group0 wdt clock
         */
        uint32_t tg1_wdt_clk_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_timergroup1_wdt_clk_conf_reg_t;

/** Type of systimer_conf register
 *  SYSTIMER configuration register
 */
typedef union {
    struct {
        /** systimer_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable systimer apb clock
         */
        uint32_t systimer_clk_en:1;
        /** systimer_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset systimer module
         */
        uint32_t systimer_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_systimer_conf_reg_t;

/** Type of systimer_func_clk_conf register
 *  SYSTIMER_FUNC_CLK configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** systimer_func_clk_sel : R/W; bitpos: [20]; default: 0;
         *  set this field to select clock-source. 0(default): XTAL, 1: FOSC.
         */
        uint32_t systimer_func_clk_sel:1;
        uint32_t reserved_21:1;
        /** systimer_func_clk_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable systimer function clock
         */
        uint32_t systimer_func_clk_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_systimer_func_clk_conf_reg_t;

/** Type of twai0_conf register
 *  TWAI0 configuration register
 */
typedef union {
    struct {
        /** twai0_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable twai0 apb clock
         */
        uint32_t twai0_clk_en:1;
        /** twai0_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset twai0 module
         */
        uint32_t twai0_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_twai0_conf_reg_t;

/** Type of twai0_func_clk_conf register
 *  TWAI0_FUNC_CLK configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** twai0_func_clk_sel : R/W; bitpos: [20]; default: 0;
         *  set this field to select clock-source. 0(default): XTAL, 1: FOSC.
         */
        uint32_t twai0_func_clk_sel:1;
        uint32_t reserved_21:1;
        /** twai0_func_clk_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable twai0 function clock
         */
        uint32_t twai0_func_clk_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_twai0_func_clk_conf_reg_t;

/** Type of twai1_conf register
 *  TWAI1 configuration register
 */
typedef union {
    struct {
        /** twai1_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable twai1 apb clock
         */
        uint32_t twai1_clk_en:1;
        /** twai1_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset twai1 module
         */
        uint32_t twai1_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_twai1_conf_reg_t;

/** Type of twai1_func_clk_conf register
 *  TWAI1_FUNC_CLK configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** twai1_func_clk_sel : R/W; bitpos: [20]; default: 0;
         *  set this field to select clock-source. 0(default): XTAL, 1: FOSC.
         */
        uint32_t twai1_func_clk_sel:1;
        uint32_t reserved_21:1;
        /** twai1_func_clk_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable twai1 function clock
         */
        uint32_t twai1_func_clk_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_twai1_func_clk_conf_reg_t;

/** Type of i2s_conf register
 *  I2S configuration register
 */
typedef union {
    struct {
        /** i2s_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable i2s apb clock
         */
        uint32_t i2s_clk_en:1;
        /** i2s_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset i2s module
         */
        uint32_t i2s_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_i2s_conf_reg_t;

/** Type of i2s_tx_clkm_conf register
 *  I2S_TX_CLKM configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** i2s_tx_clkm_div_num : R/W; bitpos: [19:12]; default: 2;
         *  Integral I2S TX clock divider value. f_I2S_CLK = f_I2S_CLK_S/(N+b/a). There will be
         *  (a-b) * n-div and b * (n+1)-div.  So the average combination will be:  for b <=
         *  a/2, z * [x * n-div + (n+1)-div] + y * n-div. For b > a/2, z * [n-div + x *
         *  (n+1)-div] + y * (n+1)-div.
         */
        uint32_t i2s_tx_clkm_div_num:8;
        /** i2s_tx_clkm_sel : R/W; bitpos: [21:20]; default: 0;
         *  Select I2S Tx module source clock. 0: XTAL clock. 1: PLL240M. 2: PLL160M. 3:
         *  I2S_MCLK_in.
         */
        uint32_t i2s_tx_clkm_sel:2;
        /** i2s_tx_clkm_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable i2s_tx function clock
         */
        uint32_t i2s_tx_clkm_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_i2s_tx_clkm_conf_reg_t;

/** Type of i2s_tx_clkm_div_conf register
 *  I2S_TX_CLKM_DIV configuration register
 */
typedef union {
    struct {
        /** i2s_tx_clkm_div_z : R/W; bitpos: [8:0]; default: 0;
         *  For b <= a/2, the value of I2S_TX_CLKM_DIV_Z is b. For b > a/2, the value of
         *  I2S_TX_CLKM_DIV_Z is (a-b).
         */
        uint32_t i2s_tx_clkm_div_z:9;
        /** i2s_tx_clkm_div_y : R/W; bitpos: [17:9]; default: 1;
         *  For b <= a/2, the value of I2S_TX_CLKM_DIV_Y is (a%b) . For b > a/2, the value of
         *  I2S_TX_CLKM_DIV_Y is (a%(a-b)).
         */
        uint32_t i2s_tx_clkm_div_y:9;
        /** i2s_tx_clkm_div_x : R/W; bitpos: [26:18]; default: 0;
         *  For b <= a/2, the value of I2S_TX_CLKM_DIV_X is (a/b) - 1. For b > a/2, the value
         *  of I2S_TX_CLKM_DIV_X is (a/(a-b)) - 1.
         */
        uint32_t i2s_tx_clkm_div_x:9;
        /** i2s_tx_clkm_div_yn1 : R/W; bitpos: [27]; default: 0;
         *  For b <= a/2, the value of I2S_TX_CLKM_DIV_YN1 is 0 . For b > a/2, the value of
         *  I2S_TX_CLKM_DIV_YN1 is 1.
         */
        uint32_t i2s_tx_clkm_div_yn1:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} pcr_i2s_tx_clkm_div_conf_reg_t;

/** Type of i2s_rx_clkm_conf register
 *  I2S_RX_CLKM configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** i2s_rx_clkm_div_num : R/W; bitpos: [19:12]; default: 2;
         *  Integral I2S clock divider value
         */
        uint32_t i2s_rx_clkm_div_num:8;
        /** i2s_rx_clkm_sel : R/W; bitpos: [21:20]; default: 0;
         *  Select I2S Rx module source clock. 0: XTAL clock. 1: PLL240M. 2: PLL160M. 3: I2S_MCLK_in.
         */
        uint32_t i2s_rx_clkm_sel:2;
        /** i2s_rx_clkm_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable i2s_rx function clock
         */
        uint32_t i2s_rx_clkm_en:1;
        /** i2s_mclk_sel : R/W; bitpos: [23]; default: 0;
         *  This field is used to select master-clock. 0(default): clk_i2s_rx, 1: clk_i2s_tx
         */
        uint32_t i2s_mclk_sel:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} pcr_i2s_rx_clkm_conf_reg_t;

/** Type of i2s_rx_clkm_div_conf register
 *  I2S_RX_CLKM_DIV configuration register
 */
typedef union {
    struct {
        /** i2s_rx_clkm_div_z : R/W; bitpos: [8:0]; default: 0;
         *  For b <= a/2, the value of I2S_RX_CLKM_DIV_Z is b. For b > a/2, the value of
         *  I2S_RX_CLKM_DIV_Z is (a-b).
         */
        uint32_t i2s_rx_clkm_div_z:9;
        /** i2s_rx_clkm_div_y : R/W; bitpos: [17:9]; default: 1;
         *  For b <= a/2, the value of I2S_RX_CLKM_DIV_Y is (a%b) . For b > a/2, the value of
         *  I2S_RX_CLKM_DIV_Y is (a%(a-b)).
         */
        uint32_t i2s_rx_clkm_div_y:9;
        /** i2s_rx_clkm_div_x : R/W; bitpos: [26:18]; default: 0;
         *  For b <= a/2, the value of I2S_RX_CLKM_DIV_X is (a/b) - 1. For b > a/2, the value
         *  of I2S_RX_CLKM_DIV_X is (a/(a-b)) - 1.
         */
        uint32_t i2s_rx_clkm_div_x:9;
        /** i2s_rx_clkm_div_yn1 : R/W; bitpos: [27]; default: 0;
         *  For b <= a/2, the value of I2S_RX_CLKM_DIV_YN1 is 0 . For b > a/2, the value of
         *  I2S_RX_CLKM_DIV_YN1 is 1.
         */
        uint32_t i2s_rx_clkm_div_yn1:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} pcr_i2s_rx_clkm_div_conf_reg_t;

/** Type of saradc_conf register
 *  SARADC configuration register
 */
typedef union {
    struct {
        /** saradc_clk_en : R/W; bitpos: [0]; default: 1;
         *  no use
         */
        uint32_t saradc_clk_en:1;
        /** saradc_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset function_register of saradc module
         */
        uint32_t saradc_rst_en:1;
        /** saradc_reg_clk_en : R/W; bitpos: [2]; default: 1;
         *  Set 1 to enable saradc apb clock
         */
        uint32_t saradc_reg_clk_en:1;
        /** saradc_reg_rst_en : R/W; bitpos: [3]; default: 0;
         *  Set 0 to reset apb_register of saradc module
         */
        uint32_t saradc_reg_rst_en:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} pcr_saradc_conf_reg_t;

/** Type of saradc_clkm_conf register
 *  SARADC_CLKM configuration register
 */
typedef union {
    struct {
        /** saradc_clkm_div_a : R/W; bitpos: [5:0]; default: 0;
         *  The  denominator of the frequency divider factor of the saradc function clock.
         */
        uint32_t saradc_clkm_div_a:6;
        /** saradc_clkm_div_b : R/W; bitpos: [11:6]; default: 0;
         *  The numerator of the frequency divider factor of the saradc function clock.
         */
        uint32_t saradc_clkm_div_b:6;
        /** saradc_clkm_div_num : R/W; bitpos: [19:12]; default: 4;
         *  The integral part of the frequency divider factor of the saradc function clock.
         */
        uint32_t saradc_clkm_div_num:8;
        /** saradc_clkm_sel : R/W; bitpos: [21:20]; default: 0;
         *  set this field to select clock-source. 0(default): XTAL, 1: 80MHz, 2: FOSC, 3:
         *  reserved.
         */
        uint32_t saradc_clkm_sel:2;
        /** saradc_clkm_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable saradc function clock
         */
        uint32_t saradc_clkm_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_saradc_clkm_conf_reg_t;

/** Type of tsens_clk_conf register
 *  TSENS_CLK configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** tsens_clk_sel : R/W; bitpos: [20]; default: 0;
         *  set this field to select clock-source. 0(default): FOSC, 1: XTAL.
         */
        uint32_t tsens_clk_sel:1;
        uint32_t reserved_21:1;
        /** tsens_clk_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable tsens clock
         */
        uint32_t tsens_clk_en:1;
        /** tsens_rst_en : R/W; bitpos: [23]; default: 0;
         *  Set 0 to reset tsens module
         */
        uint32_t tsens_rst_en:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} pcr_tsens_clk_conf_reg_t;

/** Type of usb_device_conf register
 *  USB_DEVICE configuration register
 */
typedef union {
    struct {
        /** usb_device_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable usb_device clock
         */
        uint32_t usb_device_clk_en:1;
        /** usb_device_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset usb_device module
         */
        uint32_t usb_device_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_usb_device_conf_reg_t;

/** Type of intmtx_conf register
 *  INTMTX configuration register
 */
typedef union {
    struct {
        /** intmtx_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable intmtx clock
         */
        uint32_t intmtx_clk_en:1;
        /** intmtx_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset intmtx module
         */
        uint32_t intmtx_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_intmtx_conf_reg_t;

/** Type of pcnt_conf register
 *  PCNT configuration register
 */
typedef union {
    struct {
        /** pcnt_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable pcnt clock
         */
        uint32_t pcnt_clk_en:1;
        /** pcnt_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset pcnt module
         */
        uint32_t pcnt_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_pcnt_conf_reg_t;

/** Type of etm_conf register
 *  ETM configuration register
 */
typedef union {
    struct {
        /** etm_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable etm clock
         */
        uint32_t etm_clk_en:1;
        /** etm_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset etm module
         */
        uint32_t etm_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_etm_conf_reg_t;

/** Type of pwm_conf register
 *  PWM configuration register
 */
typedef union {
    struct {
        /** pwm_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable pwm clock
         */
        uint32_t pwm_clk_en:1;
        /** pwm_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset pwm module
         */
        uint32_t pwm_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_pwm_conf_reg_t;

/** Type of pwm_clk_conf register
 *  PWM_CLK configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** pwm_div_num : R/W; bitpos: [19:12]; default: 4;
         *  The integral part of the frequency divider factor of the pwm function clock.
         */
        uint32_t pwm_div_num:8;
        /** pwm_clkm_sel : R/W; bitpos: [21:20]; default: 0;
         *  set this field to select clock-source. 0(default): do not select anyone clock, 1:
         *  160MHz, 2: XTAL, 3: FOSC.
         */
        uint32_t pwm_clkm_sel:2;
        /** pwm_clkm_en : R/W; bitpos: [22]; default: 1;
         *  set this field as 1 to activate pwm clkm.
         */
        uint32_t pwm_clkm_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_pwm_clk_conf_reg_t;

/** Type of parl_io_conf register
 *  PARL_IO configuration register
 */
typedef union {
    struct {
        /** parl_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable parl apb clock
         */
        uint32_t parl_clk_en:1;
        /** parl_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset parl apb reg
         */
        uint32_t parl_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_parl_io_conf_reg_t;

/** Type of parl_clk_rx_conf register
 *  PARL_CLK_RX configuration register
 */
typedef union {
    struct {
        /** parl_clk_rx_div_num : R/W; bitpos: [15:0]; default: 0;
         *  The integral part of the frequency divider factor of the parl rx clock.
         */
        uint32_t parl_clk_rx_div_num:16;
        /** parl_clk_rx_sel : R/W; bitpos: [17:16]; default: 0;
         *  set this field to select clock-source. 0(default): XTAL, 1: 240MHz, 2: FOSC, 3:
         *  user clock from pad.
         */
        uint32_t parl_clk_rx_sel:2;
        /** parl_clk_rx_en : R/W; bitpos: [18]; default: 1;
         *  Set 1 to enable parl rx clock
         */
        uint32_t parl_clk_rx_en:1;
        /** parl_rx_rst_en : R/W; bitpos: [19]; default: 0;
         *  Set 0 to reset parl rx module
         */
        uint32_t parl_rx_rst_en:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} pcr_parl_clk_rx_conf_reg_t;

/** Type of parl_clk_tx_conf register
 *  PARL_CLK_TX configuration register
 */
typedef union {
    struct {
        /** parl_clk_tx_div_num : R/W; bitpos: [15:0]; default: 0;
         *  The integral part of the frequency divider factor of the parl tx clock.
         */
        uint32_t parl_clk_tx_div_num:16;
        /** parl_clk_tx_sel : R/W; bitpos: [17:16]; default: 0;
         *  set this field to select clock-source. 0(default): XTAL, 1: 240MHz, 2: FOSC, 3:
         *  user clock from pad.
         */
        uint32_t parl_clk_tx_sel:2;
        /** parl_clk_tx_en : R/W; bitpos: [18]; default: 1;
         *  Set 1 to enable parl tx clock
         */
        uint32_t parl_clk_tx_en:1;
        /** parl_tx_rst_en : R/W; bitpos: [19]; default: 0;
         *  Set 0 to reset parl tx module
         */
        uint32_t parl_tx_rst_en:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} pcr_parl_clk_tx_conf_reg_t;

/** Type of sdio_slave_conf register
 *  SDIO_SLAVE configuration register
 */
typedef union {
    struct {
        /** sdio_slave_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable sdio_slave clock
         */
        uint32_t sdio_slave_clk_en:1;
        /** sdio_slave_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset sdio_slave module
         */
        uint32_t sdio_slave_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_sdio_slave_conf_reg_t;

/** Type of pvt_monitor_conf register
 *  PVT_MONITOR configuration register
 */
typedef union {
    struct {
        /** pvt_monitor_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable apb clock of pvt module
         */
        uint32_t pvt_monitor_clk_en:1;
        /** pvt_monitor_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset all pvt monitor module
         */
        uint32_t pvt_monitor_rst_en:1;
        /** pvt_monitor_site1_clk_en : R/W; bitpos: [2]; default: 1;
         *  Set 1 to enable function clock of modem pvt module
         */
        uint32_t pvt_monitor_site1_clk_en:1;
        /** pvt_monitor_site2_clk_en : R/W; bitpos: [3]; default: 1;
         *  Set 1 to enable function clock of cpu pvt module
         */
        uint32_t pvt_monitor_site2_clk_en:1;
        /** pvt_monitor_site3_clk_en : R/W; bitpos: [4]; default: 1;
         *  Set 1 to enable function clock of hp_peri pvt module
         */
        uint32_t pvt_monitor_site3_clk_en:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} pcr_pvt_monitor_conf_reg_t;

/** Type of pvt_monitor_func_clk_conf register
 *  PVT_MONITOR function clock configuration register
 */
typedef union {
    struct {
        /** pvt_monitor_func_clk_div_num : R/W; bitpos: [3:0]; default: 0;
         *  The integral part of the frequency divider factor of the pvt_monitor function clock.
         */
        uint32_t pvt_monitor_func_clk_div_num:4;
        uint32_t reserved_4:16;
        /** pvt_monitor_func_clk_sel : R/W; bitpos: [20]; default: 0;
         *  set this field to select clock-source. 0: XTAL, 1(default): 160MHz drived by SPLL
         *  divided by 3.
         */
        uint32_t pvt_monitor_func_clk_sel:1;
        uint32_t reserved_21:1;
        /** pvt_monitor_func_clk_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable source clock of pvt sitex
         */
        uint32_t pvt_monitor_func_clk_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_pvt_monitor_func_clk_conf_reg_t;

/** Type of gdma_conf register
 *  GDMA configuration register
 */
typedef union {
    struct {
        /** gdma_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable gdma clock
         */
        uint32_t gdma_clk_en:1;
        /** gdma_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset gdma module
         */
        uint32_t gdma_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_gdma_conf_reg_t;

/** Type of spi2_conf register
 *  SPI2 configuration register
 */
typedef union {
    struct {
        /** spi2_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable spi2 apb clock
         */
        uint32_t spi2_clk_en:1;
        /** spi2_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset spi2 module
         */
        uint32_t spi2_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_spi2_conf_reg_t;

/** Type of spi2_clkm_conf register
 *  SPI2_CLKM configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** spi2_clkm_sel : R/W; bitpos: [21:20]; default: 0;
         *  set this field to select clock-source. 0(default): XTAL, 1: 80MHz, 2: FOSC, 3:
         *  reserved.
         */
        uint32_t spi2_clkm_sel:2;
        /** spi2_clkm_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable spi2 function clock
         */
        uint32_t spi2_clkm_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_spi2_clkm_conf_reg_t;

/** Type of aes_conf register
 *  AES configuration register
 */
typedef union {
    struct {
        /** aes_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable aes clock
         */
        uint32_t aes_clk_en:1;
        /** aes_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset aes module
         */
        uint32_t aes_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_aes_conf_reg_t;

/** Type of sha_conf register
 *  SHA configuration register
 */
typedef union {
    struct {
        /** sha_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable sha clock
         */
        uint32_t sha_clk_en:1;
        /** sha_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset sha module
         */
        uint32_t sha_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_sha_conf_reg_t;

/** Type of rsa_conf register
 *  RSA configuration register
 */
typedef union {
    struct {
        /** rsa_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable rsa clock
         */
        uint32_t rsa_clk_en:1;
        /** rsa_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset rsa module
         */
        uint32_t rsa_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_rsa_conf_reg_t;

/** Type of rsa_pd_ctrl register
 *  RSA power control register
 */
typedef union {
    struct {
        /** rsa_mem_pd : R/W; bitpos: [0]; default: 0;
         *  Set this bit to power down rsa internal memory.
         */
        uint32_t rsa_mem_pd:1;
        /** rsa_mem_force_pu : R/W; bitpos: [1]; default: 1;
         *  Set this bit to force power up rsa internal memory
         */
        uint32_t rsa_mem_force_pu:1;
        /** rsa_mem_force_pd : R/W; bitpos: [2]; default: 0;
         *  Set this bit to force power down rsa internal memory.
         */
        uint32_t rsa_mem_force_pd:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} pcr_rsa_pd_ctrl_reg_t;

/** Type of ecc_conf register
 *  ECC configuration register
 */
typedef union {
    struct {
        /** ecc_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable ecc clock
         */
        uint32_t ecc_clk_en:1;
        /** ecc_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset ecc module
         */
        uint32_t ecc_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_ecc_conf_reg_t;

/** Type of ecc_pd_ctrl register
 *  ECC power control register
 */
typedef union {
    struct {
        /** ecc_mem_pd : R/W; bitpos: [0]; default: 0;
         *  Set this bit to power down ecc internal memory.
         */
        uint32_t ecc_mem_pd:1;
        /** ecc_mem_force_pu : R/W; bitpos: [1]; default: 1;
         *  Set this bit to force power up ecc internal memory
         */
        uint32_t ecc_mem_force_pu:1;
        /** ecc_mem_force_pd : R/W; bitpos: [2]; default: 0;
         *  Set this bit to force power down ecc internal memory.
         */
        uint32_t ecc_mem_force_pd:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} pcr_ecc_pd_ctrl_reg_t;

/** Type of ds_conf register
 *  DS configuration register
 */
typedef union {
    struct {
        /** ds_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable ds clock
         */
        uint32_t ds_clk_en:1;
        /** ds_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset ds module
         */
        uint32_t ds_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_ds_conf_reg_t;

/** Type of hmac_conf register
 *  HMAC configuration register
 */
typedef union {
    struct {
        /** hmac_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable hmac clock
         */
        uint32_t hmac_clk_en:1;
        /** hmac_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset hmac module
         */
        uint32_t hmac_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_hmac_conf_reg_t;

/** Type of iomux_conf register
 *  IOMUX configuration register
 */
typedef union {
    struct {
        /** iomux_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable iomux apb clock
         */
        uint32_t iomux_clk_en:1;
        /** iomux_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset iomux module
         */
        uint32_t iomux_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_iomux_conf_reg_t;

/** Type of iomux_clk_conf register
 *  IOMUX_CLK configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** iomux_func_clk_sel : R/W; bitpos: [21:20]; default: 3;
         *  set this field to select clock-source. 0: do not select anyone clock, 1: 80MHz, 2:
         *  FOSC, 3(default): XTAL.
         */
        uint32_t iomux_func_clk_sel:2;
        /** iomux_func_clk_en : R/W; bitpos: [22]; default: 1;
         *  Set 1 to enable iomux function clock
         */
        uint32_t iomux_func_clk_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} pcr_iomux_clk_conf_reg_t;

/** Type of mem_monitor_conf register
 *  MEM_MONITOR configuration register
 */
typedef union {
    struct {
        /** mem_monitor_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable mem_monitor clock
         */
        uint32_t mem_monitor_clk_en:1;
        /** mem_monitor_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset mem_monitor module
         */
        uint32_t mem_monitor_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_mem_monitor_conf_reg_t;

/** Type of regdma_conf register
 *  REGDMA configuration register
 */
typedef union {
    struct {
        /** regdma_clk_en : R/W; bitpos: [0]; default: 0;
         *  Set 1 to enable regdma clock
         */
        uint32_t regdma_clk_en:1;
        /** regdma_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset regdma module
         */
        uint32_t regdma_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_regdma_conf_reg_t;

/** Type of retention_conf register
 *  retention configuration register
 */
typedef union {
    struct {
        /** retention_clk_en : R/W; bitpos: [0]; default: 0;
         *  Set 1 to enable retention clock
         */
        uint32_t retention_clk_en:1;
        /** retention_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset retention module
         */
        uint32_t retention_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_retention_conf_reg_t;

/** Type of trace_conf register
 *  TRACE configuration register
 */
typedef union {
    struct {
        /** trace_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable trace clock
         */
        uint32_t trace_clk_en:1;
        /** trace_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset trace module
         */
        uint32_t trace_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_trace_conf_reg_t;

/** Type of assist_conf register
 *  ASSIST configuration register
 */
typedef union {
    struct {
        /** assist_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable assist clock
         */
        uint32_t assist_clk_en:1;
        /** assist_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset assist module
         */
        uint32_t assist_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_assist_conf_reg_t;

/** Type of cache_conf register
 *  CACHE configuration register
 */
typedef union {
    struct {
        /** cache_clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable cache clock
         */
        uint32_t cache_clk_en:1;
        /** cache_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset cache module
         */
        uint32_t cache_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_cache_conf_reg_t;

/** Type of modem_apb_conf register
 *  MODEM_APB configuration register
 */
typedef union {
    struct {
        /** modem_apb_clk_en : R/W; bitpos: [0]; default: 1;
         *  This field indicates if modem_apb clock is enable. 0: disable, 1: enable(default).
         */
        uint32_t modem_apb_clk_en:1;
        /** modem_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set this file as 1 to reset modem-subsystem.
         */
        uint32_t modem_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_modem_apb_conf_reg_t;

/** Type of timeout_conf register
 *  TIMEOUT configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** cpu_timeout_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set 0 to reset cpu_peri timeout module
         */
        uint32_t cpu_timeout_rst_en:1;
        /** hp_timeout_rst_en : R/W; bitpos: [2]; default: 0;
         *  Set 0 to reset hp_peri timeout module and hp_modem timeout module
         */
        uint32_t hp_timeout_rst_en:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} pcr_timeout_conf_reg_t;

/** Type of sysclk_conf register
 *  SYSCLK configuration register
 */
typedef union {
    struct {
        /** ls_div_num : HRO; bitpos: [7:0]; default: 0;
         *  clk_hproot is div1 of low-speed clock-source if clck-source is a low-speed
         *  clock-source such as XTAL/FOSC.
         */
        uint32_t ls_div_num:8;
        /** hs_div_num : HRO; bitpos: [15:8]; default: 2;
         *  clk_hproot is div3 of SPLL if the clock-source is high-speed clock SPLL.
         */
        uint32_t hs_div_num:8;
        /** soc_clk_sel : R/W; bitpos: [17:16]; default: 0;
         *  This field is used to select clock source. 0: XTAL, 1: SPLL, 2: FOSC, 3: reserved.
         */
        uint32_t soc_clk_sel:2;
        uint32_t reserved_18:6;
        /** clk_xtal_freq : RO; bitpos: [30:24]; default: 40;
         *  This field indicates the frequency(MHz) of XTAL.
         */
        uint32_t clk_xtal_freq:7;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} pcr_sysclk_conf_reg_t;

/** Type of cpu_waiti_conf register
 *  CPU_WAITI configuration register
 */
typedef union {
    struct {
        /** cpuperiod_sel : HRO; bitpos: [1:0]; default: 1;
         *  Reserved. This filed has been replaced by PCR_CPU_HS_DIV_NUM and PCR_CPU_LS_DIV_NUM
         */
        uint32_t cpuperiod_sel:2;
        /** pll_freq_sel : HRO; bitpos: [2]; default: 1;
         *  Reserved. This filed has been replaced by PCR_CPU_HS_DIV_NUM and PCR_CPU_LS_DIV_NUM
         */
        uint32_t pll_freq_sel:1;
        /** cpu_wait_mode_force_on : R/W; bitpos: [3]; default: 1;
         *  Set 1 to force cpu_waiti_clk enable.
         */
        uint32_t cpu_wait_mode_force_on:1;
        /** cpu_waiti_delay_num : R/W; bitpos: [7:4]; default: 0;
         *  This field used to set delay cycle when cpu enter waiti mode, after delay waiti_clk
         *  will close
         */
        uint32_t cpu_waiti_delay_num:4;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} pcr_cpu_waiti_conf_reg_t;

/** Type of cpu_freq_conf register
 *  CPU_FREQ configuration register
 */
typedef union {
    struct {
        /** cpu_ls_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Set as one within (0,1,3) to generate clk_cpu drived by clk_hproot. The clk_cpu is
         *  div1(default)/div2/div4 of clk_hproot. This field is only avaliable for low-speed
         *  clock-source such as XTAL/FOSC, and should be used together with PCR_AHB_LS_DIV_NUM.
         */
        uint32_t cpu_ls_div_num:8;
        /** cpu_hs_div_num : R/W; bitpos: [15:8]; default: 0;
         *  Set as one within (0,1,3) to generate clk_cpu drived by clk_hproot. The clk_cpu is
         *  div1(default)/div2/div4 of clk_hproot. This field is only avaliable for high-speed
         *  clock-source such as SPLL, and should be used together with PCR_AHB_HS_DIV_NUM.
         */
        uint32_t cpu_hs_div_num:8;
        /** cpu_hs_120m_force : R/W; bitpos: [16]; default: 0;
         *  Given that PCR_CPU_HS_DIV_NUM is 0, set this field as 1 to force clk_cpu at 120MHz.
         *  Only avaliable when PCR_CPU_HS_DIV_NUM is 0 and clk_cpu is driven by SPLL.
         */
        uint32_t cpu_hs_120m_force:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} pcr_cpu_freq_conf_reg_t;

/** Type of ahb_freq_conf register
 *  AHB_FREQ configuration register
 */
typedef union {
    struct {
        /** ahb_ls_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Set as one within (0,1,3,7) to generate clk_ahb drived by clk_hproot. The clk_ahb
         *  is div1(default)/div2/div4/div8 of clk_hproot. This field is only avaliable for
         *  low-speed clock-source such as XTAL/FOSC, and should be used together with
         *  PCR_CPU_LS_DIV_NUM.
         */
        uint32_t ahb_ls_div_num:8;
        /** ahb_hs_div_num : R/W; bitpos: [15:8]; default: 3;
         *  Set as one within (3,7,15) to generate clk_ahb drived by clk_hproot. The clk_ahb is
         *  div4(default)/div8/div16 of clk_hproot. This field is only avaliable for high-speed
         *  clock-source such as SPLL, and should be used together with PCR_CPU_HS_DIV_NUM.
         */
        uint32_t ahb_hs_div_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} pcr_ahb_freq_conf_reg_t;

/** Type of apb_freq_conf register
 *  APB_FREQ configuration register
 */
typedef union {
    struct {
        /** apb_decrease_div_num : R/W; bitpos: [7:0]; default: 0;
         *  If this field's value is grater than PCR_APB_DIV_NUM, the clk_apb will be
         *  automatically down to clk_apb_decrease only when no access is on apb-bus, and will
         *  recover to the previous frequency when a new access appears on apb-bus. Set as one
         *  within (0,1,3) to set clk_apb_decrease as div1/div2/div4(default) of clk_ahb. Note
         *  that enable this function will reduce performance. Users can set this field as zero
         *  to disable the auto-decrease-apb-freq function. By default, this function is
         *  disable.
         */
        uint32_t apb_decrease_div_num:8;
        /** apb_div_num : R/W; bitpos: [15:8]; default: 0;
         *  Set as one within (0,1,3) to generate clk_apb drived by clk_ahb. The clk_apb is
         *  div1(default)/div2/div4 of clk_ahb.
         */
        uint32_t apb_div_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} pcr_apb_freq_conf_reg_t;

/** Type of pll_div_clk_en register
 *  SPLL DIV clock-gating configuration register
 */
typedef union {
    struct {
        /** pll_240m_clk_en : R/W; bitpos: [0]; default: 1;
         *  This field is used to open 240 MHz clock (div2 of SPLL) drived from SPLL. 0: close,
         *  1: open(default). Only avaliable when high-speed clock-source SPLL is active.
         */
        uint32_t pll_240m_clk_en:1;
        /** pll_160m_clk_en : R/W; bitpos: [1]; default: 1;
         *  This field is used to open 160 MHz clock (div3 of SPLL) drived from SPLL. 0: close,
         *  1: open(default). Only avaliable when high-speed clock-source SPLL is active.
         */
        uint32_t pll_160m_clk_en:1;
        /** pll_120m_clk_en : R/W; bitpos: [2]; default: 1;
         *  This field is used to open 120 MHz clock (div4 of SPLL) drived from SPLL. 0: close,
         *  1: open(default). Only avaliable when high-speed clock-source SPLL is active.
         */
        uint32_t pll_120m_clk_en:1;
        /** pll_80m_clk_en : R/W; bitpos: [3]; default: 1;
         *  This field is used to open 80 MHz clock (div6  of SPLL) drived from SPLL. 0: close,
         *  1: open(default). Only avaliable when high-speed clock-source SPLL is active.
         */
        uint32_t pll_80m_clk_en:1;
        /** pll_48m_clk_en : R/W; bitpos: [4]; default: 1;
         *  This field is used to open 48 MHz clock (div10 of SPLL) drived from SPLL. 0: close,
         *  1: open(default). Only avaliable when high-speed clock-source SPLL is active.
         */
        uint32_t pll_48m_clk_en:1;
        /** pll_40m_clk_en : R/W; bitpos: [5]; default: 1;
         *  This field is used to open 40 MHz clock (div12 of SPLL) drived from SPLL. 0: close,
         *  1: open(default). Only avaliable when high-speed clock-source SPLL is active.
         */
        uint32_t pll_40m_clk_en:1;
        /** pll_20m_clk_en : R/W; bitpos: [6]; default: 1;
         *  This field is used to open 20 MHz clock (div24 of SPLL) drived from SPLL. 0: close,
         *  1: open(default). Only avaliable when high-speed clock-source SPLL is active.
         */
        uint32_t pll_20m_clk_en:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} pcr_pll_div_clk_en_reg_t;

/** Type of ctrl_clk_out_en register
 *  CLK_OUT_EN configuration register
 */
typedef union {
    struct {
        /** clk20_oen : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable 20m clock
         */
        uint32_t clk20_oen:1;
        /** clk22_oen : R/W; bitpos: [1]; default: 1;
         *  Set 1 to enable 22m clock
         */
        uint32_t clk22_oen:1;
        /** clk44_oen : R/W; bitpos: [2]; default: 1;
         *  Set 1 to enable 44m clock
         */
        uint32_t clk44_oen:1;
        /** clk_bb_oen : R/W; bitpos: [3]; default: 1;
         *  Set 1 to enable bb clock
         */
        uint32_t clk_bb_oen:1;
        /** clk80_oen : R/W; bitpos: [4]; default: 1;
         *  Set 1 to enable 80m clock
         */
        uint32_t clk80_oen:1;
        /** clk160_oen : R/W; bitpos: [5]; default: 1;
         *  Set 1 to enable 160m clock
         */
        uint32_t clk160_oen:1;
        /** clk_320m_oen : R/W; bitpos: [6]; default: 1;
         *  Set 1 to enable 320m clock
         */
        uint32_t clk_320m_oen:1;
        /** clk_adc_inf_oen : R/W; bitpos: [7]; default: 1;
         *  Reserved
         */
        uint32_t clk_adc_inf_oen:1;
        /** clk_dac_cpu_oen : R/W; bitpos: [8]; default: 1;
         *  Reserved
         */
        uint32_t clk_dac_cpu_oen:1;
        /** clk40x_bb_oen : R/W; bitpos: [9]; default: 1;
         *  Set 1 to enable 40x_bb clock
         */
        uint32_t clk40x_bb_oen:1;
        /** clk_xtal_oen : R/W; bitpos: [10]; default: 1;
         *  Set 1 to enable xtal clock
         */
        uint32_t clk_xtal_oen:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} pcr_ctrl_clk_out_en_reg_t;

/** Type of ctrl_tick_conf register
 *  TICK configuration register
 */
typedef union {
    struct {
        /** xtal_tick_num : R/W; bitpos: [7:0]; default: 39;
         *  ******* Description ***********
         */
        uint32_t xtal_tick_num:8;
        /** fosc_tick_num : R/W; bitpos: [15:8]; default: 7;
         *  ******* Description ***********
         */
        uint32_t fosc_tick_num:8;
        /** tick_enable : R/W; bitpos: [16]; default: 1;
         *  ******* Description ***********
         */
        uint32_t tick_enable:1;
        /** rst_tick_cnt : R/W; bitpos: [17]; default: 0;
         *  ******* Description ***********
         */
        uint32_t rst_tick_cnt:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} pcr_ctrl_tick_conf_reg_t;

/** Type of ctrl_32k_conf register
 *  32KHz clock configuration register
 */
typedef union {
    struct {
        /** clk_32k_sel : R/W; bitpos: [1:0]; default: 0;
         *  This field indicates which one 32KHz clock will be used by timergroup.
         *  0: OSC32K(default), 1: XTAL32K, 2/3: 32KHz from pad GPIO0.
         */
        uint32_t clk_32k_sel:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_ctrl_32k_conf_reg_t;

/** Type of sram_power_conf register
 *  HP SRAM/ROM configuration register
 */
typedef union {
    struct {
        /** sram_force_pu : R/W; bitpos: [3:0]; default: 15;
         *  Set this bit to force power up SRAM
         */
        uint32_t sram_force_pu:4;
        /** sram_force_pd : R/W; bitpos: [7:4]; default: 0;
         *  Set this bit to force power down SRAM.
         */
        uint32_t sram_force_pd:4;
        /** sram_clkgate_force_on : R/W; bitpos: [11:8]; default: 0;
         *  1: Force to open the clock and bypass the gate-clock when accessing the SRAM. 0: A
         *  gate-clock will be used when accessing the SRAM.
         */
        uint32_t sram_clkgate_force_on:4;
        /** rom_force_pu : R/W; bitpos: [14:12]; default: 7;
         *  Set this bit to force power up ROM
         */
        uint32_t rom_force_pu:3;
        /** rom_force_pd : R/W; bitpos: [17:15]; default: 0;
         *  Set this bit to force power down ROM.
         */
        uint32_t rom_force_pd:3;
        /** rom_clkgate_force_on : R/W; bitpos: [20:18]; default: 0;
         *  1: Force to open the clock and bypass the gate-clock when accessing the ROM. 0: A
         *  gate-clock will be used when accessing the ROM.
         */
        uint32_t rom_clkgate_force_on:3;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} pcr_sram_power_conf_reg_t;

/** Type of reset_event_bypass register
 *  reset event bypass backdoor configuration register
 */
typedef union {
    struct {
        /** reset_event_bypass_apm : R/W; bitpos: [0]; default: 0;
         *  This field is used to control reset event relationship for
         *  tee_reg/apm_reg/hp_system_reg. 1: tee_reg/apm_reg/hp_system_reg will only be reset
         *  by power-reset. some reset event will be bypass. 0: tee_reg/apm_reg/hp_system_reg
         *  will not only be reset by power-reset, but also some reset event.
         */
        uint32_t reset_event_bypass_apm:1;
        /** reset_event_bypass : R/W; bitpos: [1]; default: 1;
         *  This field is used to control reset event relationship for system-bus. 1: system
         *  bus (including arbiter/router) will only be reset by power-reset. some reset event
         *  will be bypass. 0: system bus (including arbiter/router) will not only be reset by
         *  power-reset, but also some reset event.
         */
        uint32_t reset_event_bypass:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pcr_reset_event_bypass_reg_t;

/** Type of clock_gate register
 *  PCR clock gating configure register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Set this bit as 1 to force on clock gating.
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} pcr_clock_gate_reg_t;


/** Group: Frequency Statistics Register */
/** Type of sysclk_freq_query_0 register
 *  SYSCLK frequency query 0 register
 */
typedef union {
    struct {
        /** fosc_freq : HRO; bitpos: [7:0]; default: 20;
         *  This field indicates the frequency(MHz) of FOSC.
         */
        uint32_t fosc_freq:8;
        /** pll_freq : HRO; bitpos: [17:8]; default: 480;
         *  This field indicates the frequency(MHz) of SPLL.
         */
        uint32_t pll_freq:10;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} pcr_sysclk_freq_query_0_reg_t;


/** Group: FPGA Debug Register */
/** Type of fpga_debug register
 *  fpga debug register
 */
typedef union {
    struct {
        /** fpga_debug : R/W; bitpos: [31:0]; default: 4294967295;
         *  Only used in fpga debug.
         */
        uint32_t fpga_debug:32;
    };
    uint32_t val;
} pcr_fpga_debug_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Date register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35676496;
         *  PCR version information.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} pcr_date_reg_t;


typedef struct pcr_dev_t {
    volatile pcr_uart0_conf_reg_t uart0_conf;
    volatile pcr_uart0_sclk_conf_reg_t uart0_sclk_conf;
    volatile pcr_uart0_pd_ctrl_reg_t uart0_pd_ctrl;
    volatile pcr_uart1_conf_reg_t uart1_conf;
    volatile pcr_uart1_sclk_conf_reg_t uart1_sclk_conf;
    volatile pcr_uart1_pd_ctrl_reg_t uart1_pd_ctrl;
    volatile pcr_mspi_conf_reg_t mspi_conf;
    volatile pcr_mspi_clk_conf_reg_t mspi_clk_conf;
    volatile pcr_i2c_conf_reg_t i2c_conf;
    volatile pcr_i2c_sclk_conf_reg_t i2c_sclk_conf;
    volatile pcr_uhci_conf_reg_t uhci_conf;
    volatile pcr_rmt_conf_reg_t rmt_conf;
    volatile pcr_rmt_sclk_conf_reg_t rmt_sclk_conf;
    volatile pcr_ledc_conf_reg_t ledc_conf;
    volatile pcr_ledc_sclk_conf_reg_t ledc_sclk_conf;
    volatile pcr_timergroup0_conf_reg_t timergroup0_conf;
    volatile pcr_timergroup0_timer_clk_conf_reg_t timergroup0_timer_clk_conf;
    volatile pcr_timergroup0_wdt_clk_conf_reg_t timergroup0_wdt_clk_conf;
    volatile pcr_timergroup1_conf_reg_t timergroup1_conf;
    volatile pcr_timergroup1_timer_clk_conf_reg_t timergroup1_timer_clk_conf;
    volatile pcr_timergroup1_wdt_clk_conf_reg_t timergroup1_wdt_clk_conf;
    volatile pcr_systimer_conf_reg_t systimer_conf;
    volatile pcr_systimer_func_clk_conf_reg_t systimer_func_clk_conf;
    volatile pcr_twai0_conf_reg_t twai0_conf;
    volatile pcr_twai0_func_clk_conf_reg_t twai0_func_clk_conf;
    volatile pcr_twai1_conf_reg_t twai1_conf;
    volatile pcr_twai1_func_clk_conf_reg_t twai1_func_clk_conf;
    volatile pcr_i2s_conf_reg_t i2s_conf;
    volatile pcr_i2s_tx_clkm_conf_reg_t i2s_tx_clkm_conf;
    volatile pcr_i2s_tx_clkm_div_conf_reg_t i2s_tx_clkm_div_conf;
    volatile pcr_i2s_rx_clkm_conf_reg_t i2s_rx_clkm_conf;
    volatile pcr_i2s_rx_clkm_div_conf_reg_t i2s_rx_clkm_div_conf;
    volatile pcr_saradc_conf_reg_t saradc_conf;
    volatile pcr_saradc_clkm_conf_reg_t saradc_clkm_conf;
    volatile pcr_tsens_clk_conf_reg_t tsens_clk_conf;
    volatile pcr_usb_device_conf_reg_t usb_device_conf;
    volatile pcr_intmtx_conf_reg_t intmtx_conf;
    volatile pcr_pcnt_conf_reg_t pcnt_conf;
    volatile pcr_etm_conf_reg_t etm_conf;
    volatile pcr_pwm_conf_reg_t pwm_conf;
    volatile pcr_pwm_clk_conf_reg_t pwm_clk_conf;
    volatile pcr_parl_io_conf_reg_t parl_io_conf;
    volatile pcr_parl_clk_rx_conf_reg_t parl_clk_rx_conf;
    volatile pcr_parl_clk_tx_conf_reg_t parl_clk_tx_conf;
    volatile pcr_sdio_slave_conf_reg_t sdio_slave_conf;
    volatile pcr_pvt_monitor_conf_reg_t pvt_monitor_conf;
    volatile pcr_pvt_monitor_func_clk_conf_reg_t pvt_monitor_func_clk_conf;
    volatile pcr_gdma_conf_reg_t gdma_conf;
    volatile pcr_spi2_conf_reg_t spi2_conf;
    volatile pcr_spi2_clkm_conf_reg_t spi2_clkm_conf;
    volatile pcr_aes_conf_reg_t aes_conf;
    volatile pcr_sha_conf_reg_t sha_conf;
    volatile pcr_rsa_conf_reg_t rsa_conf;
    volatile pcr_rsa_pd_ctrl_reg_t rsa_pd_ctrl;
    volatile pcr_ecc_conf_reg_t ecc_conf;
    volatile pcr_ecc_pd_ctrl_reg_t ecc_pd_ctrl;
    volatile pcr_ds_conf_reg_t ds_conf;
    volatile pcr_hmac_conf_reg_t hmac_conf;
    volatile pcr_iomux_conf_reg_t iomux_conf;
    volatile pcr_iomux_clk_conf_reg_t iomux_clk_conf;
    volatile pcr_mem_monitor_conf_reg_t mem_monitor_conf;
    volatile pcr_regdma_conf_reg_t regdma_conf;
    volatile pcr_retention_conf_reg_t retention_conf;
    volatile pcr_trace_conf_reg_t trace_conf;
    volatile pcr_assist_conf_reg_t assist_conf;
    volatile pcr_cache_conf_reg_t cache_conf;
    volatile pcr_modem_apb_conf_reg_t modem_apb_conf;
    volatile pcr_timeout_conf_reg_t timeout_conf;
    volatile pcr_sysclk_conf_reg_t sysclk_conf;
    volatile pcr_cpu_waiti_conf_reg_t cpu_waiti_conf;
    volatile pcr_cpu_freq_conf_reg_t cpu_freq_conf;
    volatile pcr_ahb_freq_conf_reg_t ahb_freq_conf;
    volatile pcr_apb_freq_conf_reg_t apb_freq_conf;
    volatile pcr_sysclk_freq_query_0_reg_t sysclk_freq_query_0;
    volatile pcr_pll_div_clk_en_reg_t pll_div_clk_en;
    volatile pcr_ctrl_clk_out_en_reg_t ctrl_clk_out_en;
    volatile pcr_ctrl_tick_conf_reg_t ctrl_tick_conf;
    volatile pcr_ctrl_32k_conf_reg_t ctrl_32k_conf;
    volatile pcr_sram_power_conf_reg_t sram_power_conf;
    uint32_t reserved_13c[941];
    volatile pcr_reset_event_bypass_reg_t reset_event_bypass;
    volatile pcr_fpga_debug_reg_t fpga_debug;
    volatile pcr_clock_gate_reg_t clock_gate;
    volatile pcr_date_reg_t date;
} pcr_dev_t;

extern pcr_dev_t PCR;

#ifndef __cplusplus
_Static_assert(sizeof(pcr_dev_t) == 0x1000, "Invalid size of pcr_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
