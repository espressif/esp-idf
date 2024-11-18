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

/** Group: Configuration Register */
/** Type of regdma_conf register
 *  Peri backup control register
 */
typedef union {
    struct {
        /** flow_err : RO; bitpos: [2:0]; default: 0;
         *  backup error type
         */
        uint32_t flow_err:3;
        /** start : WT; bitpos: [3]; default: 0;
         *  backup start signal
         */
        uint32_t start:1;
        /** to_mem : R/W; bitpos: [4]; default: 0;
         *  backup direction(reg to mem / mem to reg)
         */
        uint32_t to_mem:1;
        /** link_sel : R/W; bitpos: [8:5]; default: 0;
         *  Link select
         */
        uint32_t link_sel:4;
        /** start_mac : WT; bitpos: [9]; default: 0;
         *  mac sw backup start signal
         */
        uint32_t start_mac:1;
        /** to_mem_mac : R/W; bitpos: [10]; default: 0;
         *  mac sw backup direction(reg to mem / mem to reg)
         */
        uint32_t to_mem_mac:1;
        /** sel_mac : R/W; bitpos: [11]; default: 0;
         *  mac hw/sw select
         */
        uint32_t sel_mac:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} pau_regdma_conf_reg_t;

/** Type of regdma_clk_conf register
 *  Clock control register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  clock enable
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} pau_regdma_clk_conf_reg_t;

/** Type of regdma_etm_ctrl register
 *  ETM start ctrl reg
 */
typedef union {
    struct {
        /** etm_start_0 : WT; bitpos: [0]; default: 0;
         *  etm_start_0 reg
         */
        uint32_t etm_start_0:1;
        /** etm_start_1 : WT; bitpos: [1]; default: 0;
         *  etm_start_1 reg
         */
        uint32_t etm_start_1:1;
        /** etm_start_2 : WT; bitpos: [2]; default: 0;
         *  etm_start_2 reg
         */
        uint32_t etm_start_2:1;
        /** etm_start_3 : WT; bitpos: [3]; default: 0;
         *  etm_start_3 reg
         */
        uint32_t etm_start_3:1;
        /** etm_link_sel_0 : R/W; bitpos: [7:4]; default: 0;
         *  etm_link sel
         */
        uint32_t etm_link_sel_0:4;
        /** etm_link_sel_1 : R/W; bitpos: [11:8]; default: 0;
         *  etm_link sel
         */
        uint32_t etm_link_sel_1:4;
        /** etm_link_sel_2 : R/W; bitpos: [15:12]; default: 0;
         *  etm_link sel
         */
        uint32_t etm_link_sel_2:4;
        /** etm_link_sel_3 : R/W; bitpos: [19:16]; default: 0;
         *  etm_link sel
         */
        uint32_t etm_link_sel_3:4;
        /** etm_busy_cause : RO; bitpos: [23:20]; default: 0;
         *  debug
         */
        uint32_t etm_busy_cause:4;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} pau_regdma_etm_ctrl_reg_t;

/** Type of regdma_current_link_addr register
 *  current link addr
 */
typedef union {
    struct {
        /** current_link_addr : RO; bitpos: [31:0]; default: 0;
         *  current link addr reg
         */
        uint32_t current_link_addr:32;
    };
    uint32_t val;
} pau_regdma_current_link_addr_reg_t;

/** Type of regdma_peri_addr register
 *  Backup addr
 */
typedef union {
    struct {
        /** peri_addr : RO; bitpos: [31:0]; default: 0;
         *  peri addr reg
         */
        uint32_t peri_addr:32;
    };
    uint32_t val;
} pau_regdma_peri_addr_reg_t;

/** Type of regdma_mem_addr register
 *  mem addr
 */
typedef union {
    struct {
        /** mem_addr : RO; bitpos: [31:0]; default: 0;
         *  mem addr reg
         */
        uint32_t mem_addr:32;
    };
    uint32_t val;
} pau_regdma_mem_addr_reg_t;

/** Type of int_ena register
 *  Read only register for error and done
 */
typedef union {
    struct {
        /** done_int_ena : R/W; bitpos: [0]; default: 0;
         *  backup done flag
         */
        uint32_t done_int_ena:1;
        /** error_int_ena : R/W; bitpos: [1]; default: 0;
         *  error flag
         */
        uint32_t error_int_ena:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pau_int_ena_reg_t;

/** Type of int_raw register
 *  Read only register for error and done
 */
typedef union {
    struct {
        /** done_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  backup done flag
         */
        uint32_t done_int_raw:1;
        /** error_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  error flag
         */
        uint32_t error_int_raw:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pau_int_raw_reg_t;

/** Type of int_clr register
 *  Read only register for error and done
 */
typedef union {
    struct {
        /** done_int_clr : WT; bitpos: [0]; default: 0;
         *  backup done flag
         */
        uint32_t done_int_clr:1;
        /** error_int_clr : WT; bitpos: [1]; default: 0;
         *  error flag
         */
        uint32_t error_int_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pau_int_clr_reg_t;

/** Type of int_st register
 *  Read only register for error and done
 */
typedef union {
    struct {
        /** done_int_st : RO; bitpos: [0]; default: 0;
         *  backup done flag
         */
        uint32_t done_int_st:1;
        /** error_int_st : RO; bitpos: [1]; default: 0;
         *  error flag
         */
        uint32_t error_int_st:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pau_int_st_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Date register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36737360;
         *  REGDMA date information/ REGDMA version information.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} pau_date_reg_t;


typedef struct {
    volatile pau_regdma_conf_reg_t regdma_conf;
    volatile pau_regdma_clk_conf_reg_t regdma_clk_conf;
    volatile pau_regdma_etm_ctrl_reg_t regdma_etm_ctrl;
    volatile pau_regdma_current_link_addr_reg_t regdma_current_link_addr;
    volatile pau_regdma_peri_addr_reg_t regdma_peri_addr;
    volatile pau_regdma_mem_addr_reg_t regdma_mem_addr;
    volatile pau_int_ena_reg_t int_ena;
    volatile pau_int_raw_reg_t int_raw;
    volatile pau_int_clr_reg_t int_clr;
    volatile pau_int_st_reg_t int_st;
    uint32_t reserved_028[245];
    volatile pau_date_reg_t date;
} pau_dev_t;

extern pau_dev_t PAU;

#ifndef __cplusplus
_Static_assert(sizeof(pau_dev_t) == 0x400, "Invalid size of pau_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
