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

/** Group: Configuration Registers */
/** Type of bod_mode0_cntl register
 *  Brownout detector mode 0 configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:6;
        /** bod_mode0_close_flash_ena : R/W; bitpos: [6]; default: 0;
         *  Configures whether to enable the brown-out detector to trigger flash suspend.
         *  0: Disable
         *  1: Enable
         */
        uint32_t bod_mode0_close_flash_ena:1;
        /** bod_mode0_pd_rf_ena : R/W; bitpos: [7]; default: 0;
         *  Configures whether to enable the brown-out detector to power down the RF module.
         *  0: Disable
         *  1: Enable
         */
        uint32_t bod_mode0_pd_rf_ena:1;
        /** bod_mode0_intr_wait : R/W; bitpos: [17:8]; default: 1;
         *  Configures the time to generate an interrupt after the brown-out signal is valid.
         *  The unit is LP_FAST_CLK cycles.
         */
        uint32_t bod_mode0_intr_wait:10;
        /** bod_mode0_reset_wait : R/W; bitpos: [27:18]; default: 1023;
         *  Configures the time to generate a reset after the brown-out signal is valid. The
         *  unit is LP_FAST_CLK cycles.
         */
        uint32_t bod_mode0_reset_wait:10;
        /** bod_mode0_cnt_clr : R/W; bitpos: [28]; default: 0;
         *  Configures whether to clear the count value of the brown-out detector.
         *  0: Do not clear
         *  1: Clear
         */
        uint32_t bod_mode0_cnt_clr:1;
        /** bod_mode0_intr_ena : R/W; bitpos: [29]; default: 0;
         *  Enables the interrupts for the brown-out detector mode 0. LP_ANA_BOD_MODE0_INT_RAW
         *  and LP_ANA_BOD_MODE0_LP_INT_RAW are valid only when this field is set to 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t bod_mode0_intr_ena:1;
        /** bod_mode0_reset_sel : R/W; bitpos: [30]; default: 0;
         *  Configures the reset type when the brown-out detector is triggered.
         *  0: Chip reset
         *  1: System reset
         */
        uint32_t bod_mode0_reset_sel:1;
        /** bod_mode0_reset_ena : R/W; bitpos: [31]; default: 0;
         *  Configures whether to enable reset for the brown-out detector.
         *  0: Disable
         *  1: Enable
         */
        uint32_t bod_mode0_reset_ena:1;
    };
    uint32_t val;
} lp_ana_bod_mode0_cntl_reg_t;

/** Type of bod_mode1_cntl register
 *  Brownout detector mode 1 configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode1_reset_ena : R/W; bitpos: [31]; default: 0;
         *  Configures whether to enable brown-out detector mode 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t bod_mode1_reset_ena:1;
    };
    uint32_t val;
} lp_ana_bod_mode1_cntl_reg_t;

/** Type of power_glitch_cntl register
 *  Voltage glitch configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** pwr_glitch_reset_ena : R/W; bitpos: [30:27]; default: 0;
         *  Configures whether to enable the voltage glitch detectors. Bit0, bit1, bit2, bit3
         *  correspond to VDDPST2/3, VDDPST1, VDDA3, and VDDA8, respectively.
         *  0: Disable
         *  1: Enable
         */
        uint32_t pwr_glitch_reset_ena:4;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lp_ana_power_glitch_cntl_reg_t;

/** Type of fib_enable register
 *  Voltage glitch detectors' enable control register
 */
typedef union {
    struct {
        /** ana_fib_ena : R/W; bitpos: [31:0]; default: 4294967295;
         *  Controls the enable of the voltage glitch detectors. Bit2, bit3, bit4, bit5
         *  correspond to VDDPST2/3, VDDPST1, VDDA3, and VDDA8, respectively.
         *  0: Controlled by LP_ANA_PWR_GLITCH_RESET_ENA
         *  1: Forcibly enabled by hardware
         */
        uint32_t ana_fib_ena:32;
    };
    uint32_t val;
} lp_ana_fib_enable_reg_t;

/** Type of int_raw register
 *  LP_ANA_BOD_MODE0_INT raw interrupt
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  The raw interrupt status of LP_ANA_BOD_MODE0_INT.
         */
        uint32_t bod_mode0_int_raw:1;
    };
    uint32_t val;
} lp_ana_int_raw_reg_t;

/** Type of int_st register
 *  LP_ANA_BOD_MODE0_INT state interrupt
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_int_st : RO; bitpos: [31]; default: 0;
         *  The masked interrupt status of LP_ANA_BOD_MODE0_INT.
         */
        uint32_t bod_mode0_int_st:1;
    };
    uint32_t val;
} lp_ana_int_st_reg_t;

/** Type of int_ena register
 *  LP_ANA_BOD_MODE0_INT enable register
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_int_ena : R/W; bitpos: [31]; default: 0;
         *  Write 1 to enable LP_ANA_BOD_MODE0_INT.
         */
        uint32_t bod_mode0_int_ena:1;
    };
    uint32_t val;
} lp_ana_int_ena_reg_t;

/** Type of int_clr register
 *  LP_ANA_BOD_MODE0_INT clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_int_clr : WT; bitpos: [31]; default: 0;
         *  Write 1 to clear LP_ANA_BOD_MODE0_INT.
         */
        uint32_t bod_mode0_int_clr:1;
    };
    uint32_t val;
} lp_ana_int_clr_reg_t;

/** Type of lp_int_raw register
 *  LP_ANA_BOD_MODE0_LP_INT raw interrupt
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_lp_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  The raw interrupt status of LP_ANA_BOD_MODE0_LP_INT.
         */
        uint32_t bod_mode0_lp_int_raw:1;
    };
    uint32_t val;
} lp_ana_lp_int_raw_reg_t;

/** Type of lp_int_st register
 *  LP_ANA_BOD_MODE0_LP_INT state interrupt
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_lp_int_st : RO; bitpos: [31]; default: 0;
         *  The masked interrupt status of LP_ANA_BOD_MODE0_LP_INT.
         */
        uint32_t bod_mode0_lp_int_st:1;
    };
    uint32_t val;
} lp_ana_lp_int_st_reg_t;

/** Type of lp_int_ena register
 *  LP_ANA_BOD_MODE0_LP_INT enable register
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_lp_int_ena : R/W; bitpos: [31]; default: 0;
         *  Write 1 to enable LP_ANA_BOD_MODE0_LP_INT.
         */
        uint32_t bod_mode0_lp_int_ena:1;
    };
    uint32_t val;
} lp_ana_lp_int_ena_reg_t;

/** Type of lp_int_clr register
 *  LP_ANA_BOD_MODE0_LP_INT  clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_lp_int_clr : WT; bitpos: [31]; default: 0;
         *  Write 1 to clear LP_ANA_BOD_MODE0_LP_INT.
         */
        uint32_t bod_mode0_lp_int_clr:1;
    };
    uint32_t val;
} lp_ana_lp_int_clr_reg_t;


/** Group: Version Control Registers */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** lp_ana_date : R/W; bitpos: [30:0]; default: 36774528;
         *  Version control register.
         */
        uint32_t lp_ana_date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  Configures whether to force enable register clock.
         *  0: Automatic clock gating
         *  1: Force enable register clock
         *  The configuration of this field does not effect the access of registers.
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lp_ana_date_reg_t;


typedef struct {
    volatile lp_ana_bod_mode0_cntl_reg_t bod_mode0_cntl;
    volatile lp_ana_bod_mode1_cntl_reg_t bod_mode1_cntl;
    volatile lp_ana_power_glitch_cntl_reg_t power_glitch_cntl;
    volatile lp_ana_fib_enable_reg_t fib_enable;
    volatile lp_ana_int_raw_reg_t int_raw;
    volatile lp_ana_int_st_reg_t int_st;
    volatile lp_ana_int_ena_reg_t int_ena;
    volatile lp_ana_int_clr_reg_t int_clr;
    volatile lp_ana_lp_int_raw_reg_t lp_int_raw;
    volatile lp_ana_lp_int_st_reg_t lp_int_st;
    volatile lp_ana_lp_int_ena_reg_t lp_int_ena;
    volatile lp_ana_lp_int_clr_reg_t lp_int_clr;
    uint32_t reserved_030[243];
    volatile lp_ana_date_reg_t date;
} lp_ana_dev_t;

extern lp_ana_dev_t LP_ANA_PERI;

#ifndef __cplusplus
_Static_assert(sizeof(lp_ana_dev_t) == 0x400, "Invalid size of lp_ana_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
