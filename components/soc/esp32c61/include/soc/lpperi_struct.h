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

/** Group: configure_register */
/** Type of clk_en register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** rng_ck_en : R/W; bitpos: [24]; default: 1;
         *  need_des
         */
        uint32_t rng_ck_en:1;
        /** otp_dbg_ck_en : R/W; bitpos: [25]; default: 1;
         *  need_des
         */
        uint32_t otp_dbg_ck_en:1;
        uint32_t reserved_26:1;
        /** lp_io_ck_en : R/W; bitpos: [27]; default: 1;
         *  need_des
         */
        uint32_t lp_io_ck_en:1;
        uint32_t reserved_28:2;
        /** efuse_ck_en : R/W; bitpos: [30]; default: 1;
         *  need_des
         */
        uint32_t efuse_ck_en:1;
        /** lp_cpu_ck_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_ck_en:1;
    };
    uint32_t val;
} lpperi_clk_en_reg_t;

/** Type of reset_en register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:23;
        /** bus_reset_en : WT; bitpos: [23]; default: 0;
         *  need_des
         */
        uint32_t bus_reset_en:1;
        /** lp_rng_reset_en : R/W; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t lp_rng_reset_en:1;
        /** otp_dbg_reset_en : R/W; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t otp_dbg_reset_en:1;
        uint32_t reserved_26:1;
        /** lp_io_reset_en : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t lp_io_reset_en:1;
        uint32_t reserved_28:2;
        /** efuse_reset_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t efuse_reset_en:1;
        /** lp_cpu_reset_en : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_reset_en:1;
    };
    uint32_t val;
} lpperi_reset_en_reg_t;

/** Type of rng_data register
 *  need_des
 */
typedef union {
    struct {
        /** rnd_data : RO; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t rnd_data:32;
    };
    uint32_t val;
} lpperi_rng_data_reg_t;

/** Type of cpu register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lpcore_dbgm_unavaliable : R/W; bitpos: [31]; default: 1;
         *  need_des
         */
        uint32_t lpcore_dbgm_unavaliable:1;
    };
    uint32_t val;
} lpperi_cpu_reg_t;

/** Type of bus_timeout register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:14;
        /** lp_peri_timeout_thres : R/W; bitpos: [29:14]; default: 65535;
         *  need_des
         */
        uint32_t lp_peri_timeout_thres:16;
        /** lp_peri_timeout_int_clear : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_peri_timeout_int_clear:1;
        /** lp_peri_timeout_protect_en : R/W; bitpos: [31]; default: 1;
         *  need_des
         */
        uint32_t lp_peri_timeout_protect_en:1;
    };
    uint32_t val;
} lpperi_bus_timeout_reg_t;

/** Type of bus_timeout_addr register
 *  need_des
 */
typedef union {
    struct {
        /** lp_peri_timeout_addr : RO; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t lp_peri_timeout_addr:32;
    };
    uint32_t val;
} lpperi_bus_timeout_addr_reg_t;

/** Type of bus_timeout_uid register
 *  need_des
 */
typedef union {
    struct {
        /** lp_peri_timeout_uid : RO; bitpos: [6:0]; default: 0;
         *  need_des
         */
        uint32_t lp_peri_timeout_uid:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lpperi_bus_timeout_uid_reg_t;

/** Type of interrupt_source register
 *  need_des
 */
typedef union {
    struct {
        /** lp_interrupt_source : RO; bitpos: [5:0]; default: 0;
         *  BIT5~BIT0: pmu_lp_int, modem_lp_int, lp_timer_lp_int, reserved, reserved, lp_io_int
         */
        uint32_t lp_interrupt_source:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lpperi_interrupt_source_reg_t;

/** Type of rng_cfg register
 *  need_des
 */
typedef union {
    struct {
        /** rng_sample_enable : R/W; bitpos: [0]; default: 0;
         *  need des
         */
        uint32_t rng_sample_enable:1;
        /** rng_timer_pscale : R/W; bitpos: [8:1]; default: 255;
         *  need des
         */
        uint32_t rng_timer_pscale:8;
        /** rng_timer_en : R/W; bitpos: [9]; default: 1;
         *  need des
         */
        uint32_t rng_timer_en:1;
        /** rtc_timer_en : R/W; bitpos: [11:10]; default: 3;
         *  need des
         */
        uint32_t rtc_timer_en:2;
        uint32_t reserved_12:12;
        /** rng_sample_cnt : RO; bitpos: [31:24]; default: 0;
         *  need des
         */
        uint32_t rng_sample_cnt:8;
    };
    uint32_t val;
} lpperi_rng_cfg_reg_t;

/** Type of rng_data_sync register
 *  need_des
 */
typedef union {
    struct {
        /** rnd_sync_data : RO; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t rnd_sync_data:32;
    };
    uint32_t val;
} lpperi_rng_data_sync_reg_t;


/** Group: Version register */
/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** lpperi_date : R/W; bitpos: [30:0]; default: 37753168;
         *  need_des
         */
        uint32_t lpperi_date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lpperi_date_reg_t;


typedef struct {
    volatile lpperi_clk_en_reg_t clk_en;
    volatile lpperi_reset_en_reg_t reset_en;
    volatile lpperi_rng_data_reg_t rng_data;
    volatile lpperi_cpu_reg_t cpu;
    volatile lpperi_bus_timeout_reg_t bus_timeout;
    volatile lpperi_bus_timeout_addr_reg_t bus_timeout_addr;
    volatile lpperi_bus_timeout_uid_reg_t bus_timeout_uid;
    uint32_t reserved_01c;
    volatile lpperi_interrupt_source_reg_t interrupt_source;
    volatile lpperi_rng_cfg_reg_t rng_cfg;
    volatile lpperi_rng_data_sync_reg_t rng_data_sync;
    uint32_t reserved_02c[244];
    volatile lpperi_date_reg_t date;
} lpperi_dev_t;

extern lpperi_dev_t LPPERI;

#ifndef __cplusplus
_Static_assert(sizeof(lpperi_dev_t) == 0x400, "Invalid size of lpperi_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
