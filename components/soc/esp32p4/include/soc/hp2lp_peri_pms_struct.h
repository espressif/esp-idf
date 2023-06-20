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

/** Group: TEE HP2LP TEE PMS DATE REG */
/** Type of hp2lp_tee_pms_date register
 *  NA
 */
typedef union {
    struct {
        /** tee_date : R/W; bitpos: [31:0]; default: 2294790;
         *  NA
         */
        uint32_t tee_date:32;
    };
    uint32_t val;
} tee_hp2lp_tee_pms_date_reg_t;


/** Group: TEE PMS CLK EN REG */
/** Type of pms_clk_en register
 *  NA
 */
typedef union {
    struct {
        /** reg_clk_en : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t reg_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} tee_pms_clk_en_reg_t;


/** Group: TEE HP CORE0 MM PMS REG0 REG */
/** Type of hp_core0_mm_pms_reg0 register
 *  NA
 */
typedef union {
    struct {
        /** reg_hp_core0_mm_lp_sysreg_allow : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_sysreg_allow:1;
        /** reg_hp_core0_mm_lp_aonclkrst_allow : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_aonclkrst_allow:1;
        /** reg_hp_core0_mm_lp_timer_allow : R/W; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_timer_allow:1;
        /** reg_hp_core0_mm_lp_anaperi_allow : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_anaperi_allow:1;
        /** reg_hp_core0_mm_lp_pmu_allow : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_pmu_allow:1;
        /** reg_hp_core0_mm_lp_wdt_allow : R/W; bitpos: [5]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_wdt_allow:1;
        /** reg_hp_core0_mm_lp_mailbox_allow : R/W; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_mailbox_allow:1;
        /** reg_hp_core0_mm_lp_rtc_allow : R/W; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_rtc_allow:1;
        /** reg_hp_core0_mm_lp_periclkrst_allow : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_periclkrst_allow:1;
        /** reg_hp_core0_mm_lp_uart_allow : R/W; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_uart_allow:1;
        /** reg_hp_core0_mm_lp_i2c_allow : R/W; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_i2c_allow:1;
        /** reg_hp_core0_mm_lp_spi_allow : R/W; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_spi_allow:1;
        /** reg_hp_core0_mm_lp_i2cmst_allow : R/W; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_i2cmst_allow:1;
        /** reg_hp_core0_mm_lp_i2s_allow : R/W; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_i2s_allow:1;
        /** reg_hp_core0_mm_lp_adc_allow : R/W; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_adc_allow:1;
        /** reg_hp_core0_mm_lp_touch_allow : R/W; bitpos: [15]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_touch_allow:1;
        /** reg_hp_core0_mm_lp_iomux_allow : R/W; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_iomux_allow:1;
        /** reg_hp_core0_mm_lp_intr_allow : R/W; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_intr_allow:1;
        /** reg_hp_core0_mm_lp_efuse_allow : R/W; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_efuse_allow:1;
        /** reg_hp_core0_mm_lp_pms_allow : R/W; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_pms_allow:1;
        /** reg_hp_core0_mm_hp2lp_pms_allow : R/W; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_hp2lp_pms_allow:1;
        /** reg_hp_core0_mm_lp_tsens_allow : R/W; bitpos: [21]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_tsens_allow:1;
        /** reg_hp_core0_mm_lp_huk_allow : R/W; bitpos: [22]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_huk_allow:1;
        /** reg_hp_core0_mm_lp_tcm_ram_allow : R/W; bitpos: [23]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_mm_lp_tcm_ram_allow:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} tee_hp_core0_mm_pms_reg0_reg_t;


/** Group: TEE HP CORE0 UM PMS REG0 REG */
/** Type of hp_core0_um_pms_reg0 register
 *  NA
 */
typedef union {
    struct {
        /** reg_hp_core0_um_lp_sysreg_allow : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_sysreg_allow:1;
        /** reg_hp_core0_um_lp_aonclkrst_allow : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_aonclkrst_allow:1;
        /** reg_hp_core0_um_lp_timer_allow : R/W; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_timer_allow:1;
        /** reg_hp_core0_um_lp_anaperi_allow : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_anaperi_allow:1;
        /** reg_hp_core0_um_lp_pmu_allow : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_pmu_allow:1;
        /** reg_hp_core0_um_lp_wdt_allow : R/W; bitpos: [5]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_wdt_allow:1;
        /** reg_hp_core0_um_lp_mailbox_allow : R/W; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_mailbox_allow:1;
        /** reg_hp_core0_um_lp_rtc_allow : R/W; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_rtc_allow:1;
        /** reg_hp_core0_um_lp_periclkrst_allow : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_periclkrst_allow:1;
        /** reg_hp_core0_um_lp_uart_allow : R/W; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_uart_allow:1;
        /** reg_hp_core0_um_lp_i2c_allow : R/W; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_i2c_allow:1;
        /** reg_hp_core0_um_lp_spi_allow : R/W; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_spi_allow:1;
        /** reg_hp_core0_um_lp_i2cmst_allow : R/W; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_i2cmst_allow:1;
        /** reg_hp_core0_um_lp_i2s_allow : R/W; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_i2s_allow:1;
        /** reg_hp_core0_um_lp_adc_allow : R/W; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_adc_allow:1;
        /** reg_hp_core0_um_lp_touch_allow : R/W; bitpos: [15]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_touch_allow:1;
        /** reg_hp_core0_um_lp_iomux_allow : R/W; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_iomux_allow:1;
        /** reg_hp_core0_um_lp_intr_allow : R/W; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_intr_allow:1;
        /** reg_hp_core0_um_lp_efuse_allow : R/W; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_efuse_allow:1;
        /** reg_hp_core0_um_lp_pms_allow : R/W; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_pms_allow:1;
        /** reg_hp_core0_um_hp2lp_pms_allow : R/W; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_hp2lp_pms_allow:1;
        /** reg_hp_core0_um_lp_tsens_allow : R/W; bitpos: [21]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_tsens_allow:1;
        /** reg_hp_core0_um_lp_huk_allow : R/W; bitpos: [22]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_huk_allow:1;
        /** reg_hp_core0_um_lp_tcm_ram_allow : R/W; bitpos: [23]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core0_um_lp_tcm_ram_allow:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} tee_hp_core0_um_pms_reg0_reg_t;


/** Group: TEE HP CORE1 MM PMS REG0 REG */
/** Type of hp_core1_mm_pms_reg0 register
 *  NA
 */
typedef union {
    struct {
        /** reg_hp_core1_mm_lp_sysreg_allow : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_sysreg_allow:1;
        /** reg_hp_core1_mm_lp_aonclkrst_allow : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_aonclkrst_allow:1;
        /** reg_hp_core1_mm_lp_timer_allow : R/W; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_timer_allow:1;
        /** reg_hp_core1_mm_lp_anaperi_allow : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_anaperi_allow:1;
        /** reg_hp_core1_mm_lp_pmu_allow : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_pmu_allow:1;
        /** reg_hp_core1_mm_lp_wdt_allow : R/W; bitpos: [5]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_wdt_allow:1;
        /** reg_hp_core1_mm_lp_mailbox_allow : R/W; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_mailbox_allow:1;
        /** reg_hp_core1_mm_lp_rtc_allow : R/W; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_rtc_allow:1;
        /** reg_hp_core1_mm_lp_periclkrst_allow : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_periclkrst_allow:1;
        /** reg_hp_core1_mm_lp_uart_allow : R/W; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_uart_allow:1;
        /** reg_hp_core1_mm_lp_i2c_allow : R/W; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_i2c_allow:1;
        /** reg_hp_core1_mm_lp_spi_allow : R/W; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_spi_allow:1;
        /** reg_hp_core1_mm_lp_i2cmst_allow : R/W; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_i2cmst_allow:1;
        /** reg_hp_core1_mm_lp_i2s_allow : R/W; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_i2s_allow:1;
        /** reg_hp_core1_mm_lp_adc_allow : R/W; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_adc_allow:1;
        /** reg_hp_core1_mm_lp_touch_allow : R/W; bitpos: [15]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_touch_allow:1;
        /** reg_hp_core1_mm_lp_iomux_allow : R/W; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_iomux_allow:1;
        /** reg_hp_core1_mm_lp_intr_allow : R/W; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_intr_allow:1;
        /** reg_hp_core1_mm_lp_efuse_allow : R/W; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_efuse_allow:1;
        /** reg_hp_core1_mm_lp_pms_allow : R/W; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_pms_allow:1;
        /** reg_hp_core1_mm_hp2lp_pms_allow : R/W; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_hp2lp_pms_allow:1;
        /** reg_hp_core1_mm_lp_tsens_allow : R/W; bitpos: [21]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_tsens_allow:1;
        /** reg_hp_core1_mm_lp_huk_allow : R/W; bitpos: [22]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_huk_allow:1;
        /** reg_hp_core1_mm_lp_tcm_ram_allow : R/W; bitpos: [23]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_mm_lp_tcm_ram_allow:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} tee_hp_core1_mm_pms_reg0_reg_t;


/** Group: TEE HP CORE1 UM PMS REG0 REG */
/** Type of hp_core1_um_pms_reg0 register
 *  NA
 */
typedef union {
    struct {
        /** reg_hp_core1_um_lp_sysreg_allow : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_sysreg_allow:1;
        /** reg_hp_core1_um_lp_aonclkrst_allow : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_aonclkrst_allow:1;
        /** reg_hp_core1_um_lp_timer_allow : R/W; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_timer_allow:1;
        /** reg_hp_core1_um_lp_anaperi_allow : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_anaperi_allow:1;
        /** reg_hp_core1_um_lp_pmu_allow : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_pmu_allow:1;
        /** reg_hp_core1_um_lp_wdt_allow : R/W; bitpos: [5]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_wdt_allow:1;
        /** reg_hp_core1_um_lp_mailbox_allow : R/W; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_mailbox_allow:1;
        /** reg_hp_core1_um_lp_rtc_allow : R/W; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_rtc_allow:1;
        /** reg_hp_core1_um_lp_periclkrst_allow : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_periclkrst_allow:1;
        /** reg_hp_core1_um_lp_uart_allow : R/W; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_uart_allow:1;
        /** reg_hp_core1_um_lp_i2c_allow : R/W; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_i2c_allow:1;
        /** reg_hp_core1_um_lp_spi_allow : R/W; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_spi_allow:1;
        /** reg_hp_core1_um_lp_i2cmst_allow : R/W; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_i2cmst_allow:1;
        /** reg_hp_core1_um_lp_i2s_allow : R/W; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_i2s_allow:1;
        /** reg_hp_core1_um_lp_adc_allow : R/W; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_adc_allow:1;
        /** reg_hp_core1_um_lp_touch_allow : R/W; bitpos: [15]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_touch_allow:1;
        /** reg_hp_core1_um_lp_iomux_allow : R/W; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_iomux_allow:1;
        /** reg_hp_core1_um_lp_intr_allow : R/W; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_intr_allow:1;
        /** reg_hp_core1_um_lp_efuse_allow : R/W; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_efuse_allow:1;
        /** reg_hp_core1_um_lp_pms_allow : R/W; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_pms_allow:1;
        /** reg_hp_core1_um_hp2lp_pms_allow : R/W; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_hp2lp_pms_allow:1;
        /** reg_hp_core1_um_lp_tsens_allow : R/W; bitpos: [21]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_tsens_allow:1;
        /** reg_hp_core1_um_lp_huk_allow : R/W; bitpos: [22]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_huk_allow:1;
        /** reg_hp_core1_um_lp_tcm_ram_allow : R/W; bitpos: [23]; default: 1;
         *  NA
         */
        uint32_t reg_hp_core1_um_lp_tcm_ram_allow:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} tee_hp_core1_um_pms_reg0_reg_t;


/** Group: TEE REGDMA PERI PMS REG */
/** Type of regdma_peri_pms register
 *  NA
 */
typedef union {
    struct {
        /** reg_regdma_peri_lp_ram_allow : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t reg_regdma_peri_lp_ram_allow:1;
        /** reg_regdma_peri_lp_peri_allow : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t reg_regdma_peri_lp_peri_allow:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} tee_regdma_peri_pms_reg_t;


typedef struct {
    volatile tee_hp2lp_tee_pms_date_reg_t hp2lp_tee_pms_date;
    volatile tee_pms_clk_en_reg_t pms_clk_en;
    volatile tee_hp_core0_mm_pms_reg0_reg_t hp_core0_mm_pms_reg0;
    volatile tee_hp_core0_um_pms_reg0_reg_t hp_core0_um_pms_reg0;
    volatile tee_hp_core1_mm_pms_reg0_reg_t hp_core1_mm_pms_reg0;
    volatile tee_hp_core1_um_pms_reg0_reg_t hp_core1_um_pms_reg0;
    volatile tee_regdma_peri_pms_reg_t regdma_peri_pms;
} tee_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(tee_dev_t) == 0x1c, "Invalid size of tee_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
