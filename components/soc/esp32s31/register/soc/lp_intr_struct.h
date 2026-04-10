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
} lpintr_sw_int_raw_reg_t;

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
} lpintr_sw_int_st_reg_t;

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
} lpintr_sw_int_ena_reg_t;

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
} lpintr_sw_int_clr_reg_t;


/** Group: Status Registers */
/** Type of status register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** lp_dac_intr_st : RO; bitpos: [2]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 6
         */
        uint32_t lp_dac_intr_st:1;
        /** lp_sw_invalid_sleep_intr_st : RO; bitpos: [3]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 18
         */
        uint32_t lp_sw_invalid_sleep_intr_st:1;
        /** lp_ahb_pdma_out_ch1_intr_st : RO; bitpos: [4]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 16
         */
        uint32_t lp_ahb_pdma_out_ch1_intr_st:1;
        /** lp_ahb_pdma_out_ch0_intr_st : RO; bitpos: [5]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 16
         */
        uint32_t lp_ahb_pdma_out_ch0_intr_st:1;
        /** lp_ahb_pdma_in_ch1_intr_st : RO; bitpos: [6]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 16
         */
        uint32_t lp_ahb_pdma_in_ch1_intr_st:1;
        /** lp_ahb_pdma_in_ch0_intr_st : RO; bitpos: [7]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 16
         */
        uint32_t lp_ahb_pdma_in_ch0_intr_st:1;
        /** lp_peri_pms_intr_st : RO; bitpos: [8]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 10
         */
        uint32_t lp_peri_pms_intr_st:1;
        /** lp_peri_timeout_intr_st : RO; bitpos: [9]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 10
         */
        uint32_t lp_peri_timeout_intr_st:1;
        /** lp_trng_intr_st : RO; bitpos: [10]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 3
         */
        uint32_t lp_trng_intr_st:1;
        /** lp_huk_intr_st : RO; bitpos: [11]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 0
         */
        uint32_t lp_huk_intr_st:1;
        /** sysreg_intr_st : RO; bitpos: [12]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 10
         */
        uint32_t sysreg_intr_st:1;
        /** lp_sw_intr_st : RO; bitpos: [13]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 0
         */
        uint32_t lp_sw_intr_st:1;
        /** lp_efuse_intr_st : RO; bitpos: [14]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 9
         */
        uint32_t lp_efuse_intr_st:1;
        /** lp_uart_intr_st : RO; bitpos: [15]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 1
         */
        uint32_t lp_uart_intr_st:1;
        /** lp_tsens_intr_st : RO; bitpos: [16]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 8
         */
        uint32_t lp_tsens_intr_st:1;
        /** lp_touch_intr_st : RO; bitpos: [17]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 7
         */
        uint32_t lp_touch_intr_st:1;
        /** lp_spi_intr_st : RO; bitpos: [18]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 2
         */
        uint32_t lp_spi_intr_st:1;
        /** lp_i2c_intr_st : RO; bitpos: [19]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 4
         */
        uint32_t lp_i2c_intr_st:1;
        /** lp_gpio_intr_st : RO; bitpos: [20]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 5
         */
        uint32_t lp_gpio_intr_st:1;
        /** lp_adc_intr_st : RO; bitpos: [21]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 6
         */
        uint32_t lp_adc_intr_st:1;
        /** anaperi_intr_st : RO; bitpos: [22]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 11
         */
        uint32_t anaperi_intr_st:1;
        /** pmu_reg_1_intr_st : RO; bitpos: [23]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 12
         */
        uint32_t pmu_reg_1_intr_st:1;
        /** pmu_reg_0_intr_st : RO; bitpos: [24]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 12
         */
        uint32_t pmu_reg_0_intr_st:1;
        /** mb_lp_intr_st : RO; bitpos: [25]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 13
         */
        uint32_t mb_lp_intr_st:1;
        /** mb_hp_intr_st : RO; bitpos: [26]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 13
         */
        uint32_t mb_hp_intr_st:1;
        /** lp_timer_reg_1_intr_st : RO; bitpos: [27]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 14
         */
        uint32_t lp_timer_reg_1_intr_st:1;
        /** lp_timer_reg_0_intr_st : RO; bitpos: [28]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 14
         */
        uint32_t lp_timer_reg_0_intr_st:1;
        /** lp_wdt_intr_st : RO; bitpos: [29]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 15
         */
        uint32_t lp_wdt_intr_st:1;
        /** apm_intr_st : RO; bitpos: [30]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 10
         */
        uint32_t apm_intr_st:1;
        /** hp_intr_st : RO; bitpos: [31]; default: 0;
         *  Represents the interrupt of lp cpu interrupt number 17
         */
        uint32_t hp_intr_st:1;
    };
    uint32_t val;
} lpintr_status_reg_t;


/** Group: configure_register */
/** Type of enable register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** lp_dac_intr_en : R/W; bitpos: [2]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 6
         */
        uint32_t lp_dac_intr_en:1;
        /** lp_sw_invalid_sleep_intr_en : R/W; bitpos: [3]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 18
         */
        uint32_t lp_sw_invalid_sleep_intr_en:1;
        /** lp_ahb_pdma_out_ch1_intr_en : R/W; bitpos: [4]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 16
         */
        uint32_t lp_ahb_pdma_out_ch1_intr_en:1;
        /** lp_ahb_pdma_out_ch0_intr_en : R/W; bitpos: [5]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 16
         */
        uint32_t lp_ahb_pdma_out_ch0_intr_en:1;
        /** lp_ahb_pdma_in_ch1_intr_en : R/W; bitpos: [6]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 16
         */
        uint32_t lp_ahb_pdma_in_ch1_intr_en:1;
        /** lp_ahb_pdma_in_ch0_intr_en : R/W; bitpos: [7]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 16
         */
        uint32_t lp_ahb_pdma_in_ch0_intr_en:1;
        /** lp_peri_pms_intr_en : R/W; bitpos: [8]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 10
         */
        uint32_t lp_peri_pms_intr_en:1;
        /** lp_peri_timeout_intr_en : R/W; bitpos: [9]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 10
         */
        uint32_t lp_peri_timeout_intr_en:1;
        /** lp_trng_intr_en : R/W; bitpos: [10]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 3
         */
        uint32_t lp_trng_intr_en:1;
        /** lp_huk_intr_en : R/W; bitpos: [11]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 0
         */
        uint32_t lp_huk_intr_en:1;
        /** sysreg_intr_en : R/W; bitpos: [12]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 10
         */
        uint32_t sysreg_intr_en:1;
        /** lp_sw_intr_en : R/W; bitpos: [13]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 0
         */
        uint32_t lp_sw_intr_en:1;
        /** lp_efuse_intr_en : R/W; bitpos: [14]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 9
         */
        uint32_t lp_efuse_intr_en:1;
        /** lp_uart_intr_en : R/W; bitpos: [15]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 1
         */
        uint32_t lp_uart_intr_en:1;
        /** lp_tsens_intr_en : R/W; bitpos: [16]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 8
         */
        uint32_t lp_tsens_intr_en:1;
        /** lp_touch_intr_en : R/W; bitpos: [17]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 7
         */
        uint32_t lp_touch_intr_en:1;
        /** lp_spi_intr_en : R/W; bitpos: [18]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 2
         */
        uint32_t lp_spi_intr_en:1;
        /** lp_i2c_intr_en : R/W; bitpos: [19]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 4
         */
        uint32_t lp_i2c_intr_en:1;
        /** lp_gpio_intr_en : R/W; bitpos: [20]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 5
         */
        uint32_t lp_gpio_intr_en:1;
        /** lp_adc_intr_en : R/W; bitpos: [21]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 6
         */
        uint32_t lp_adc_intr_en:1;
        /** anaperi_intr_en : R/W; bitpos: [22]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 11
         */
        uint32_t anaperi_intr_en:1;
        /** pmu_reg_1_intr_en : R/W; bitpos: [23]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 12
         */
        uint32_t pmu_reg_1_intr_en:1;
        /** pmu_reg_0_intr_en : R/W; bitpos: [24]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 12
         */
        uint32_t pmu_reg_0_intr_en:1;
        /** mb_lp_intr_en : R/W; bitpos: [25]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 13
         */
        uint32_t mb_lp_intr_en:1;
        /** mb_hp_intr_en : R/W; bitpos: [26]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 13
         */
        uint32_t mb_hp_intr_en:1;
        /** lp_timer_reg_1_intr_en : R/W; bitpos: [27]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 14
         */
        uint32_t lp_timer_reg_1_intr_en:1;
        /** lp_timer_reg_0_intr_en : R/W; bitpos: [28]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 14
         */
        uint32_t lp_timer_reg_0_intr_en:1;
        /** lp_wdt_intr_en : R/W; bitpos: [29]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 15
         */
        uint32_t lp_wdt_intr_en:1;
        /** apm_intr_en : R/W; bitpos: [30]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 10
         */
        uint32_t apm_intr_en:1;
        /** hp_intr_en : R/W; bitpos: [31]; default: 1;
         *  Represents the interrupt of lp cpu interrupt number 17
         */
        uint32_t hp_intr_en:1;
    };
    uint32_t val;
} lpintr_enable_reg_t;

/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [30:0]; default: 38813744;
         *  need_des
         */
        uint32_t date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lpintr_date_reg_t;


typedef struct {
    volatile lpintr_sw_int_raw_reg_t sw_int_raw;
    volatile lpintr_sw_int_st_reg_t sw_int_st;
    volatile lpintr_sw_int_ena_reg_t sw_int_ena;
    volatile lpintr_sw_int_clr_reg_t sw_int_clr;
    volatile lpintr_status_reg_t status;
    volatile lpintr_enable_reg_t enable;
    uint32_t reserved_018[249];
    volatile lpintr_date_reg_t date;
} lpintr_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(lpintr_dev_t) == 0x400, "Invalid size of lpintr_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
