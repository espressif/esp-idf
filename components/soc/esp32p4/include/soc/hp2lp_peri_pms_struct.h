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

/** Group: Version Control Registers */
/** Type of hp2lp_peri_pms_date register
 *  Version control register
 */
typedef union {
    struct {
        /** hp2lp_peri_pms_date : R/W; bitpos: [31:0]; default: 2294790;
         *  Version control register
         */
        uint32_t hp2lp_peri_pms_date:32;
    };
    uint32_t val;
} pms_hp2lp_peri_pms_date_reg_t;


/** Group: Clock Gating Registers */
/** Type of hp2lp_peri_pms_clk_en register
 *  Clock gating register
 */
typedef union {
    struct {
        /** hp2lp_peri_pms_clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to keep the clock always on.
         *  0: Enable automatic clock gating
         *  1: Keep the clock always on
         */
        uint32_t hp2lp_peri_pms_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} pms_hp2lp_peri_pms_clk_en_reg_t;


/** Group: HP CPU Permission Control Registers */
/** Type of hp_coren_mm_pms_reg0 register
 *  Permission control register0 for HP CPUn in machine mode
 */
typedef union {
    struct {
        /** hp_coren_mm_lp_sysreg_allow : R/W; bitpos: [0]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP System
         *  Registers.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_sysreg_allow:1;
        /** hp_coren_mm_lp_aonclkrst_allow : R/W; bitpos: [1]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP_AONCLKRST.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_aonclkrst_allow:1;
        /** hp_coren_mm_lp_timer_allow : R/W; bitpos: [2]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP timer.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_timer_allow:1;
        /** hp_coren_mm_lp_anaperi_allow : R/W; bitpos: [3]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP ANAPERI.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_anaperi_allow:1;
        /** hp_coren_mm_lp_pmu_allow : R/W; bitpos: [4]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP PMU.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_pmu_allow:1;
        /** hp_coren_mm_lp_wdt_allow : R/W; bitpos: [5]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP WDT.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_wdt_allow:1;
        /** hp_coren_mm_lp_mailbox_allow : R/W; bitpos: [6]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP Mailbox
         *  Controller.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_mailbox_allow:1;
        /** hp_coren_mm_lp_rtc_allow : R/W; bitpos: [7]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP RTC.
         *  0: Not allowed
         *  1: Allow
         */
        uint32_t hp_coren_mm_lp_rtc_allow:1;
        /** hp_coren_mm_lp_periclkrst_allow : R/W; bitpos: [8]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP PERICLKRST.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_periclkrst_allow:1;
        /** hp_coren_mm_lp_uart_allow : R/W; bitpos: [9]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP UART.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_uart_allow:1;
        /** hp_coren_mm_lp_i2c_allow : R/W; bitpos: [10]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP I2C.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_i2c_allow:1;
        /** hp_coren_mm_lp_spi_allow : R/W; bitpos: [11]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP SPI.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_spi_allow:1;
        /** hp_coren_mm_lp_i2cmst_allow : R/W; bitpos: [12]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP I2C master.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_i2cmst_allow:1;
        /** hp_coren_mm_lp_i2s_allow : R/W; bitpos: [13]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP I2S.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_i2s_allow:1;
        /** hp_coren_mm_lp_adc_allow : R/W; bitpos: [14]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP ADC.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_adc_allow:1;
        /** hp_coren_mm_lp_touch_allow : R/W; bitpos: [15]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP touch
         *  sensor.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_touch_allow:1;
        /** hp_coren_mm_lp_iomux_allow : R/W; bitpos: [16]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP IO MUX.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_iomux_allow:1;
        /** hp_coren_mm_lp_intr_allow : R/W; bitpos: [17]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP INTR.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_intr_allow:1;
        /** hp_coren_mm_lp_efuse_allow : R/W; bitpos: [18]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP eFuse.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_efuse_allow:1;
        /** hp_coren_mm_lp_pms_allow : R/W; bitpos: [19]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access
         *  LP_PERI_PMS_REG.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_pms_allow:1;
        /** hp_coren_mm_hp2lp_pms_allow : R/W; bitpos: [20]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access
         *  HP2LP_PERI_PMS_REG.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_hp2lp_pms_allow:1;
        /** hp_coren_mm_lp_tsens_allow : R/W; bitpos: [21]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP temperature
         *  sensor.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_tsens_allow:1;
        /** hp_coren_mm_lp_huk_allow : R/W; bitpos: [22]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to LP HUK.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_huk_allow:1;
        /** hp_coren_mm_lp_sram_allow : R/W; bitpos: [23]; default: 1;
         *  Configures whether HP CPUn in machine mode has permission to access LP SRAM.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_mm_lp_sram_allow:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} pms_hp_coren_mm_pms_reg0_reg_t;

/** Type of hp_coren_um_pms_reg0 register
 *  Permission control register0 for HP CPUn in user mode
 */
typedef union {
    struct {
        /** hp_coren_um_lp_sysreg_allow : R/W; bitpos: [0]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP System
         *  Registers.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_sysreg_allow:1;
        /** hp_coren_um_lp_aonclkrst_allow : R/W; bitpos: [1]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP_AONCLKRST.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_aonclkrst_allow:1;
        /** hp_coren_um_lp_timer_allow : R/W; bitpos: [2]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP timer.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_timer_allow:1;
        /** hp_coren_um_lp_anaperi_allow : R/W; bitpos: [3]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP ANAPERI.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_anaperi_allow:1;
        /** hp_coren_um_lp_pmu_allow : R/W; bitpos: [4]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP PMU.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_pmu_allow:1;
        /** hp_coren_um_lp_wdt_allow : R/W; bitpos: [5]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP WDT.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_wdt_allow:1;
        /** hp_coren_um_lp_mailbox_allow : R/W; bitpos: [6]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP Mailbox
         *  Controller.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_mailbox_allow:1;
        /** hp_coren_um_lp_rtc_allow : R/W; bitpos: [7]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP RTC.
         *  0: Not allowed
         *  1: Allow
         */
        uint32_t hp_coren_um_lp_rtc_allow:1;
        /** hp_coren_um_lp_periclkrst_allow : R/W; bitpos: [8]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP PERICLKRST.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_periclkrst_allow:1;
        /** hp_coren_um_lp_uart_allow : R/W; bitpos: [9]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP UART.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_uart_allow:1;
        /** hp_coren_um_lp_i2c_allow : R/W; bitpos: [10]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP I2C.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_i2c_allow:1;
        /** hp_coren_um_lp_spi_allow : R/W; bitpos: [11]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP SPI.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_spi_allow:1;
        /** hp_coren_um_lp_i2cmst_allow : R/W; bitpos: [12]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP I2C master.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_i2cmst_allow:1;
        /** hp_coren_um_lp_i2s_allow : R/W; bitpos: [13]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP I2S.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_i2s_allow:1;
        /** hp_coren_um_lp_adc_allow : R/W; bitpos: [14]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP ADC.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_adc_allow:1;
        /** hp_coren_um_lp_touch_allow : R/W; bitpos: [15]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP touch sensor.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_touch_allow:1;
        /** hp_coren_um_lp_iomux_allow : R/W; bitpos: [16]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP IO MUX.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_iomux_allow:1;
        /** hp_coren_um_lp_intr_allow : R/W; bitpos: [17]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP INTR.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_intr_allow:1;
        /** hp_coren_um_lp_efuse_allow : R/W; bitpos: [18]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP eFuse.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_efuse_allow:1;
        /** hp_coren_um_lp_pms_allow : R/W; bitpos: [19]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP_PERI_PMS_REG.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_pms_allow:1;
        /** hp_coren_um_hp2lp_pms_allow : R/W; bitpos: [20]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access
         *  HP2LP_PERI_PMS_REG.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_hp2lp_pms_allow:1;
        /** hp_coren_um_lp_tsens_allow : R/W; bitpos: [21]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP temperature
         *  sensor.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_tsens_allow:1;
        /** hp_coren_um_lp_huk_allow : R/W; bitpos: [22]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to LP HUK.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_huk_allow:1;
        /** hp_coren_um_lp_sram_allow : R/W; bitpos: [23]; default: 1;
         *  Configures whether HP CPUn in user mode has permission to access LP SRAM.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t hp_coren_um_lp_sram_allow:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} pms_hp_coren_um_pms_reg0_reg_t;


/** Group: TEE Peripheral Permission Control Register */
/** Type of regdma_lp_peri_pms register
 *  LP Peripheral Permission register for REGDMA
 */
typedef union {
    struct {
        /** regdma_peri_lp_sram_allow : R/W; bitpos: [0]; default: 1;
         *  Configures whether REGDMA has permission to access LP SRAM.
         *  0: Not allowed
         *  1: Allow
         */
        uint32_t regdma_peri_lp_sram_allow:1;
        /** regdma_peri_lp_peri_allow : R/W; bitpos: [1]; default: 1;
         *  Configures whether REGDMA has permission to access  all LP peripherals.
         *  0: Not allowed
         *  1: Allow
         */
        uint32_t regdma_peri_lp_peri_allow:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pms_regdma_lp_peri_pms_reg_t;


typedef struct {
    volatile pms_hp2lp_peri_pms_date_reg_t hp2lp_peri_pms_date;
    volatile pms_hp2lp_peri_pms_clk_en_reg_t hp2lp_peri_pms_clk_en;
    volatile pms_hp_coren_mm_pms_reg0_reg_t hp_core0_mm_pms_reg0;
    volatile pms_hp_coren_um_pms_reg0_reg_t hp_core0_um_pms_reg0;
    volatile pms_hp_coren_mm_pms_reg0_reg_t hp_core1_mm_pms_reg0;
    volatile pms_hp_coren_um_pms_reg0_reg_t hp_core1_um_pms_reg0;
    volatile pms_regdma_lp_peri_pms_reg_t regdma_lp_peri_pms;
} hp2lp_peri_pms_dev_t;

extern hp2lp_peri_pms_dev_t HP2LP_PERI_PMS;

#ifndef __cplusplus
_Static_assert(sizeof(hp2lp_peri_pms_dev_t) == 0x1c, "Invalid size of hp2lp_peri_pms_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
