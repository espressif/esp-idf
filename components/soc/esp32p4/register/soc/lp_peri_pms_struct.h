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
/** Type of lp_peri_pms_date register
 *  Version control register
 */
typedef union {
    struct {
        /** lp_peri_pms_date : R/W; bitpos: [31:0]; default: 2294537;
         *  Version control register
         */
        uint32_t lp_peri_pms_date:32;
    };
    uint32_t val;
} pms_lp_peri_pms_date_reg_t;


/** Group: Clock Gating Registers */
/** Type of lp_peri_pms_clk_en register
 *  Clock gating register
 */
typedef union {
    struct {
        /** lp_peri_pms_clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to keep the clock always on.
         *  0: Enable automatic clock gating
         *  1: Keep the clock always on
         */
        uint32_t lp_peri_pms_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} pms_lp_peri_pms_clk_en_reg_t;


/** Group: LP CPU Permission Control Registers */
/** Type of lp_mm_lp_peri_pms_reg0 register
 *  Permission control register0 for LP CPU in machine mode
 */
typedef union {
    struct {
        /** lp_mm_lp_sysreg_allow : R/W; bitpos: [0]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP system
         *  registers.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_sysreg_allow:1;
        /** lp_mm_lp_aonclkrst_allow : R/W; bitpos: [1]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP_AONCLKRST (LP
         *  always-on clock and reset).
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_aonclkrst_allow:1;
        /** lp_mm_lp_timer_allow : R/W; bitpos: [2]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP timer.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_timer_allow:1;
        /** lp_mm_lp_anaperi_allow : R/W; bitpos: [3]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP ANAPERI
         *  (analog peripherals).
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_anaperi_allow:1;
        /** lp_mm_lp_pmu_allow : R/W; bitpos: [4]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP PMU (Power
         *  Management Unit).
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_pmu_allow:1;
        /** lp_mm_lp_wdt_allow : R/W; bitpos: [5]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP WDT (watchdog
         *  timer).
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_wdt_allow:1;
        /** lp_mm_lp_mailbox_allow : R/W; bitpos: [6]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP Mailbox
         *  Controller.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_mailbox_allow:1;
        /** lp_mm_lp_rtc_allow : R/W; bitpos: [7]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP RTC.
         *  0: Not allowed
         *  1: Allow
         */
        uint32_t lp_mm_lp_rtc_allow:1;
        /** lp_mm_lp_periclkrst_allow : R/W; bitpos: [8]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access  LP PREICLKRST
         *  (peripheral clock and reset).
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_periclkrst_allow:1;
        /** lp_mm_lp_uart_allow : R/W; bitpos: [9]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access  LP UART.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_uart_allow:1;
        /** lp_mm_lp_i2c_allow : R/W; bitpos: [10]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP I2S.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_i2c_allow:1;
        /** lp_mm_lp_spi_allow : R/W; bitpos: [11]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP SPI.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_spi_allow:1;
        /** lp_mm_lp_i2cmst_allow : R/W; bitpos: [12]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP I2C master.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_i2cmst_allow:1;
        /** lp_mm_lp_i2s_allow : R/W; bitpos: [13]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP I2S.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_i2s_allow:1;
        /** lp_mm_lp_adc_allow : R/W; bitpos: [14]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP ADC.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_adc_allow:1;
        /** lp_mm_lp_touch_allow : R/W; bitpos: [15]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP touch sensor.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_touch_allow:1;
        /** lp_mm_lp_iomux_allow : R/W; bitpos: [16]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP IO MUX.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_iomux_allow:1;
        /** lp_mm_lp_intr_allow : R/W; bitpos: [17]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP INTR
         *  (interrupt).
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_intr_allow:1;
        /** lp_mm_lp_efuse_allow : R/W; bitpos: [18]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP eFuse.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_efuse_allow:1;
        /** lp_mm_lp_pms_allow : R/W; bitpos: [19]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP_PERI_PMS_REG.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_pms_allow:1;
        /** lp_mm_hp2lp_pms_allow : R/W; bitpos: [20]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access
         *  HP2LP_PERI_PMS_REG.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp2lp_pms_allow:1;
        /** lp_mm_lp_tsens_allow : R/W; bitpos: [21]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP temperature
         *  sensor.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_tsens_allow:1;
        /** lp_mm_lp_huk_allow : R/W; bitpos: [22]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP HUK (Hardware
         *  Unique Key).
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp_huk_allow:1;
        /** lp_mm_lp_sram_allow : R/W; bitpos: [23]; default: 1;
         *  Configures whether LP CPU in machine mode has permission to access LP SRAM.
         *  0: Not allowed
         *  1: Allow
         */
        uint32_t lp_mm_lp_sram_allow:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} pms_lp_mm_lp_peri_pms_reg0_reg_t;


/** Group: Configurable Address Range Configuration Registers */
/** Type of peri_regionn_low register
 *  Regionn start address configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** peri_regionn_low : R/W; bitpos: [31:2]; default: 0;
         *  Configures the high 30 bits of the start address of peripheral register's regionn.
         */
        uint32_t peri_regionn_low:30;
    };
    uint32_t val;
} pms_peri_regionn_low_reg_t;

/** Type of peri_regionn_high register
 *  Regionn end address configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** peri_regionn_high : R/W; bitpos: [31:2]; default: 1073741823;
         *  Configures the high 30 bits of the end address of peripheral register's regionn.
         */
        uint32_t peri_regionn_high:30;
    };
    uint32_t val;
} pms_peri_regionn_high_reg_t;


/** Group: PMS Peripheral Region Permission Control Registers */
/** Type of peri_region_pms register
 *  Permission register of  region
 */
typedef union {
    struct {
        /** lp_core_region_pms : R/W; bitpos: [1:0]; default: 3;
         *  Configures whether LP core in machine mode has permission to access address region0
         *  and address region1. Bit0 corresponds to region0 and bit1 corresponds to region1.
         *  0: Not allowed
         *  1: Allow
         */
        uint32_t lp_core_region_pms:2;
        /** hp_core0_um_region_pms : R/W; bitpos: [3:2]; default: 3;
         *  Configures whether HP CPU0 in user mode has permission to access address region0
         *  and address region1. Bit2 corresponds to region0 and bit3 corresponds to region1.
         *  0: Not allowed
         *  1: Allow
         */
        uint32_t hp_core0_um_region_pms:2;
        /** hp_core0_mm_region_pms : R/W; bitpos: [5:4]; default: 3;
         *  Configures whether HP CPU0 in machine mode has permission to access address region0
         *  and address region1. Bit4 corresponds to region0 and bit5 corresponds to region1.
         *  0: Not allowed
         *  1: Allow
         */
        uint32_t hp_core0_mm_region_pms:2;
        /** hp_core1_um_region_pms : R/W; bitpos: [7:6]; default: 3;
         *  Configures whether HP CPU1 in user mode has permission to access address region0
         *  and address region1. Bit6 corresponds to region0 and bit7 corresponds to region1.
         *  0: Not allowed
         *  1: Allow
         */
        uint32_t hp_core1_um_region_pms:2;
        /** hp_core1_mm_region_pms : R/W; bitpos: [9:8]; default: 3;
         *  Configures whether HP CPU1 in machine mode has permission to access address region0
         *  and address region1. Bit8 corresponds to region0 and bit9 corresponds to region1.
         *  0: Not allowed
         *  1: Allow
         */
        uint32_t hp_core1_mm_region_pms:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} pms_peri_region_pms_reg_t;


typedef struct {
    volatile pms_lp_peri_pms_date_reg_t lp_peri_pms_date;
    volatile pms_lp_peri_pms_clk_en_reg_t lp_peri_pms_clk_en;
    volatile pms_lp_mm_lp_peri_pms_reg0_reg_t lp_mm_lp_peri_pms_reg0;
    volatile pms_peri_regionn_low_reg_t peri_region0_low;
    volatile pms_peri_regionn_high_reg_t peri_region0_high;
    volatile pms_peri_regionn_low_reg_t peri_region1_low;
    volatile pms_peri_regionn_high_reg_t peri_region1_high;
    volatile pms_peri_region_pms_reg_t peri_region_pms;
} lp_peri_pms_dev_t;

extern lp_peri_pms_dev_t LP_PERI_PMS;

#ifndef __cplusplus
_Static_assert(sizeof(lp_peri_pms_dev_t) == 0x20, "Invalid size of lp_peri_pms_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
