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

/** Group: Interrupt Registers */
/** Type of sw_int_raw register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lp_sw_int_raw : R/W/WTC; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_sw_int_raw:1;
    };
    uint32_t val;
} lp_intr_sw_int_raw_reg_t;

/** Type of sw_int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lp_sw_int_st : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_sw_int_st:1;
    };
    uint32_t val;
} lp_intr_sw_int_st_reg_t;

/** Type of sw_int_ena register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lp_sw_int_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_sw_int_ena:1;
    };
    uint32_t val;
} lp_intr_sw_int_ena_reg_t;

/** Type of sw_int_clr register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lp_sw_int_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_sw_int_clr:1;
    };
    uint32_t val;
} lp_intr_sw_int_clr_reg_t;


/** Group: Status Registers */
/** Type of status register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:10;
        /** lp_huk_intr_st : RO; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t lp_huk_intr_st:1;
        /** sysreg_intr_st : RO; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t sysreg_intr_st:1;
        /** lp_sw_intr_st : RO; bitpos: [12]; default: 0;
         *  need_des
         */
        uint32_t lp_sw_intr_st:1;
        /** lp_efuse_intr_st : RO; bitpos: [13]; default: 0;
         *  need_des
         */
        uint32_t lp_efuse_intr_st:1;
        /** lp_uart_intr_st : RO; bitpos: [14]; default: 0;
         *  need_des
         */
        uint32_t lp_uart_intr_st:1;
        /** lp_tsens_intr_st : RO; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t lp_tsens_intr_st:1;
        /** lp_touch_intr_st : RO; bitpos: [16]; default: 0;
         *  need_des
         */
        uint32_t lp_touch_intr_st:1;
        /** lp_spi_intr_st : RO; bitpos: [17]; default: 0;
         *  need_des
         */
        uint32_t lp_spi_intr_st:1;
        /** lp_i2s_intr_st : RO; bitpos: [18]; default: 0;
         *  need_des
         */
        uint32_t lp_i2s_intr_st:1;
        /** lp_i2c_intr_st : RO; bitpos: [19]; default: 0;
         *  need_des
         */
        uint32_t lp_i2c_intr_st:1;
        /** lp_gpio_intr_st : RO; bitpos: [20]; default: 0;
         *  need_des
         */
        uint32_t lp_gpio_intr_st:1;
        /** lp_adc_intr_st : RO; bitpos: [21]; default: 0;
         *  need_des
         */
        uint32_t lp_adc_intr_st:1;
        /** anaperi_intr_st : RO; bitpos: [22]; default: 0;
         *  need_des
         */
        uint32_t anaperi_intr_st:1;
        /** pmu_reg_1_intr_st : RO; bitpos: [23]; default: 0;
         *  need_des
         */
        uint32_t pmu_reg_1_intr_st:1;
        /** pmu_reg_0_intr_st : RO; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t pmu_reg_0_intr_st:1;
        /** mb_lp_intr_st : RO; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t mb_lp_intr_st:1;
        /** mb_hp_intr_st : RO; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t mb_hp_intr_st:1;
        /** lp_timer_reg_1_intr_st : RO; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t lp_timer_reg_1_intr_st:1;
        /** lp_timer_reg_0_intr_st : RO; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t lp_timer_reg_0_intr_st:1;
        /** lp_wdt_intr_st : RO; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t lp_wdt_intr_st:1;
        /** lp_rtc_intr_st : RO; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_rtc_intr_st:1;
        /** hp_intr_st : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hp_intr_st:1;
    };
    uint32_t val;
} lp_intr_status_reg_t;


/** Group: configure_register */
/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lp_intr_date_reg_t;


typedef struct {
    volatile lp_intr_sw_int_raw_reg_t sw_int_raw;
    volatile lp_intr_sw_int_st_reg_t sw_int_st;
    volatile lp_intr_sw_int_ena_reg_t sw_int_ena;
    volatile lp_intr_sw_int_clr_reg_t sw_int_clr;
    volatile lp_intr_status_reg_t status;
    uint32_t reserved_014[250];
    volatile lp_intr_date_reg_t date;
} lp_intr_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(lp_intr_dev_t) == 0x400, "Invalid size of lp_intr_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
