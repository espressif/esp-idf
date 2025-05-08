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

/** Group: configure_register */
/** Type of clk_en register
 *  configure peri in lp system clk enable
 */
typedef union {
    struct {
        uint32_t reserved_0:23;
        /** rng_apb_ck_en : R/W; bitpos: [23]; default: 1;
         *  lp rng apb clk enable
         *  1: enable clock
         *  0: disable clock
         */
        uint32_t rng_apb_ck_en:1;
        /** rng_ck_en : R/W; bitpos: [24]; default: 1;
         *  lp rng clk enable
         *  1: enable clock
         *  0: disable clock
         */
        uint32_t rng_ck_en:1;
        /** otp_dbg_ck_en : R/W; bitpos: [25]; default: 1;
         *  lp optdebug clk enable
         *  1: enable clock
         *  0: disable clock
         */
        uint32_t otp_dbg_ck_en:1;
        /** lp_touch_ck_en : R/W; bitpos: [26]; default: 1;
         *  lp touch clk enable
         *  1: enable clock
         *  0: disable clock
         */
        uint32_t lp_touch_ck_en:1;
        /** lp_io_ck_en : R/W; bitpos: [27]; default: 1;
         *  lp io clk enable
         *  1: enable clock
         *  0: disable clock
         */
        uint32_t lp_io_ck_en:1;
        uint32_t reserved_28:2;
        /** efuse_ck_en : R/W; bitpos: [30]; default: 1;
         *  efuse core clk enable
         *  1: enable clock
         *  0: disable clock
         */
        uint32_t efuse_ck_en:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lpperi_clk_en_reg_t;

/** Type of reset_en register
 *  configure peri in lp system reset enable
 */
typedef union {
    struct {
        uint32_t reserved_0:23;
        /** bus_reset_en : WT; bitpos: [23]; default: 0;
         *  lp bus reset enable
         *  1: enable reset
         *  0: disable reset
         */
        uint32_t bus_reset_en:1;
        /** lp_rng_apb_reset_en : R/W; bitpos: [24]; default: 0;
         *  lp rng apb reset enable
         *  1: enable reset
         *  0: disable reset
         */
        uint32_t lp_rng_apb_reset_en:1;
        /** otp_dbg_reset_en : R/W; bitpos: [25]; default: 0;
         *  lp optdebug reset enable
         *  1: enable reset
         *  0: disable reset
         */
        uint32_t otp_dbg_reset_en:1;
        /** lp_touch_reset_en : R/W; bitpos: [26]; default: 0;
         *  lp touch reset enable
         *  1: enable reset
         *  0: disable reset
         */
        uint32_t lp_touch_reset_en:1;
        /** lp_io_reset_en : R/W; bitpos: [27]; default: 0;
         *  lp io reset enable
         *  1: enable reset
         *  0: disable reset
         */
        uint32_t lp_io_reset_en:1;
        uint32_t reserved_28:2;
        /** efuse_reset_en : R/W; bitpos: [30]; default: 0;
         *  efuse core reset enable
         *  1: enable reset
         *  0: disable reset
         */
        uint32_t efuse_reset_en:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lpperi_reset_en_reg_t;

/** Type of interrupt_source register
 *  record the lp cpu interrupt
 */
typedef union {
    struct {
        /** lp_interrupt_source : RO; bitpos: [4:0]; default: 0;
         *  BIT4~BIT0: pmu_lp_int, modem_lp_int, lp_timer_lp_int, lp_analog_peri_int, lp_io_int
         */
        uint32_t lp_interrupt_source:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} lpperi_interrupt_source_reg_t;


/** Group: PMS Register */
/** Type of lp_peri_pms_conf register
 *  LP Peripherals PMS configuration register
 */
typedef union {
    struct {
        /** lp_peri_pms_exception_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear lp peri_pms_record_reg.
         *  0: No clear
         *  1: Clear peri_pms_record_reg
         */
        uint32_t lp_peri_pms_exception_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lpperi_lp_peri_pms_conf_reg_t;

/** Type of lp_peri_pms_exception_info register
 *  LP Peripherals PMS exception info record register
 */
typedef union {
    struct {
        /** lp_peri_pms_exception_det : RO; bitpos: [0]; default: 0;
         *  Represents whether the lp peripheral pms has been triggered.
         *  0: No triggered
         *  1: Has been triggered
         */
        uint32_t lp_peri_pms_exception_det:1;
        /** lp_peri_pms_exception_id : RO; bitpos: [5:1]; default: 0;
         *  Represents the master id when lp peripheral pms has been triggered.
         */
        uint32_t lp_peri_pms_exception_id:5;
        /** lp_peri_pms_exception_mode : RO; bitpos: [7:6]; default: 0;
         *  Represents the security mode when lp peripheral pms has been triggered.
         */
        uint32_t lp_peri_pms_exception_mode:2;
        /** lp_peri_pms_exception_addr : RO; bitpos: [31:8]; default: 0;
         *  Represents the access address (bit23~bit0) when lp peripheral pms has been
         *  triggered.
         */
        uint32_t lp_peri_pms_exception_addr:24;
    };
    uint32_t val;
} lpperi_lp_peri_pms_exception_info_reg_t;


/** Group: Version register */
/** Type of date register
 *  version register
 */
typedef union {
    struct {
        /** lpperi_date : R/W; bitpos: [30:0]; default: 37819136;
         *  version register
         */
        uint32_t lpperi_date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  force on reg clk
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lpperi_date_reg_t;


typedef struct {
    volatile lpperi_clk_en_reg_t clk_en;
    volatile lpperi_reset_en_reg_t reset_en;
    uint32_t reserved_008[2];
    volatile lpperi_lp_peri_pms_conf_reg_t lp_peri_pms_conf;
    volatile lpperi_lp_peri_pms_exception_info_reg_t lp_peri_pms_exception_info;
    uint32_t reserved_018[2];
    volatile lpperi_interrupt_source_reg_t interrupt_source;
    uint32_t reserved_024[246];
    volatile lpperi_date_reg_t date;
} lpperi_dev_t;

extern lpperi_dev_t LPPERI;

#ifndef __cplusplus
_Static_assert(sizeof(lpperi_dev_t) == 0x400, "Invalid size of lpperi_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
