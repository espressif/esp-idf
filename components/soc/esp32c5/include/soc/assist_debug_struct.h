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

/** Group: monitor configuration registers */
/** Type of core_0_montr_ena register
 *  core0 monitor enable configuration register
 */
typedef union {
    struct {
        /** core_0_area_dram0_0_rd_ena : R/W; bitpos: [0]; default: 0;
         *  Core0 dram0 area0 read monitor enable
         */
        uint32_t core_0_area_dram0_0_rd_ena:1;
        /** core_0_area_dram0_0_wr_ena : R/W; bitpos: [1]; default: 0;
         *  Core0 dram0 area0 write monitor enable
         */
        uint32_t core_0_area_dram0_0_wr_ena:1;
        /** core_0_area_dram0_1_rd_ena : R/W; bitpos: [2]; default: 0;
         *  Core0 dram0 area1 read monitor enable
         */
        uint32_t core_0_area_dram0_1_rd_ena:1;
        /** core_0_area_dram0_1_wr_ena : R/W; bitpos: [3]; default: 0;
         *  Core0 dram0 area1 write monitor enable
         */
        uint32_t core_0_area_dram0_1_wr_ena:1;
        /** core_0_area_pif_0_rd_ena : R/W; bitpos: [4]; default: 0;
         *  Core0 PIF area0 read monitor enable
         */
        uint32_t core_0_area_pif_0_rd_ena:1;
        /** core_0_area_pif_0_wr_ena : R/W; bitpos: [5]; default: 0;
         *  Core0 PIF area0 write monitor enable
         */
        uint32_t core_0_area_pif_0_wr_ena:1;
        /** core_0_area_pif_1_rd_ena : R/W; bitpos: [6]; default: 0;
         *  Core0 PIF area1 read monitor enable
         */
        uint32_t core_0_area_pif_1_rd_ena:1;
        /** core_0_area_pif_1_wr_ena : R/W; bitpos: [7]; default: 0;
         *  Core0 PIF area1 write monitor enable
         */
        uint32_t core_0_area_pif_1_wr_ena:1;
        /** core_0_sp_spill_min_ena : R/W; bitpos: [8]; default: 0;
         *  Core0 stackpoint underflow monitor enable
         */
        uint32_t core_0_sp_spill_min_ena:1;
        /** core_0_sp_spill_max_ena : R/W; bitpos: [9]; default: 0;
         *  Core0 stackpoint overflow monitor enable
         */
        uint32_t core_0_sp_spill_max_ena:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} assist_debug_core_0_montr_ena_reg_t;

/** Type of core_0_area_dram0_0_min register
 *  core0 dram0 region0 addr configuration register
 */
typedef union {
    struct {
        /** core_0_area_dram0_0_min : R/W; bitpos: [31:0]; default: 4294967295;
         *  Core0 dram0 region0 start addr
         */
        uint32_t core_0_area_dram0_0_min:32;
    };
    uint32_t val;
} assist_debug_core_0_area_dram0_0_min_reg_t;

/** Type of core_0_area_dram0_0_max register
 *  core0 dram0 region0 addr configuration register
 */
typedef union {
    struct {
        /** core_0_area_dram0_0_max : R/W; bitpos: [31:0]; default: 0;
         *  Core0 dram0 region0 end addr
         */
        uint32_t core_0_area_dram0_0_max:32;
    };
    uint32_t val;
} assist_debug_core_0_area_dram0_0_max_reg_t;

/** Type of core_0_area_dram0_1_min register
 *  core0 dram0 region1 addr configuration register
 */
typedef union {
    struct {
        /** core_0_area_dram0_1_min : R/W; bitpos: [31:0]; default: 4294967295;
         *  Core0 dram0 region1 start addr
         */
        uint32_t core_0_area_dram0_1_min:32;
    };
    uint32_t val;
} assist_debug_core_0_area_dram0_1_min_reg_t;

/** Type of core_0_area_dram0_1_max register
 *  core0 dram0 region1 addr configuration register
 */
typedef union {
    struct {
        /** core_0_area_dram0_1_max : R/W; bitpos: [31:0]; default: 0;
         *  Core0 dram0 region1 end addr
         */
        uint32_t core_0_area_dram0_1_max:32;
    };
    uint32_t val;
} assist_debug_core_0_area_dram0_1_max_reg_t;

/** Type of core_0_area_pif_0_min register
 *  core0 PIF region0 addr configuration register
 */
typedef union {
    struct {
        /** core_0_area_pif_0_min : R/W; bitpos: [31:0]; default: 4294967295;
         *  Core0 PIF region0 start addr
         */
        uint32_t core_0_area_pif_0_min:32;
    };
    uint32_t val;
} assist_debug_core_0_area_pif_0_min_reg_t;

/** Type of core_0_area_pif_0_max register
 *  core0 PIF region0 addr configuration register
 */
typedef union {
    struct {
        /** core_0_area_pif_0_max : R/W; bitpos: [31:0]; default: 0;
         *  Core0 PIF region0 end addr
         */
        uint32_t core_0_area_pif_0_max:32;
    };
    uint32_t val;
} assist_debug_core_0_area_pif_0_max_reg_t;

/** Type of core_0_area_pif_1_min register
 *  core0 PIF region1 addr configuration register
 */
typedef union {
    struct {
        /** core_0_area_pif_1_min : R/W; bitpos: [31:0]; default: 4294967295;
         *  Core0 PIF region1 start addr
         */
        uint32_t core_0_area_pif_1_min:32;
    };
    uint32_t val;
} assist_debug_core_0_area_pif_1_min_reg_t;

/** Type of core_0_area_pif_1_max register
 *  core0 PIF region1 addr configuration register
 */
typedef union {
    struct {
        /** core_0_area_pif_1_max : R/W; bitpos: [31:0]; default: 0;
         *  Core0 PIF region1 end addr
         */
        uint32_t core_0_area_pif_1_max:32;
    };
    uint32_t val;
} assist_debug_core_0_area_pif_1_max_reg_t;

/** Type of core_0_area_pc register
 *  core0 area pc status register
 */
typedef union {
    struct {
        /** core_0_area_pc : RO; bitpos: [31:0]; default: 0;
         *  the stackpointer when first touch region monitor interrupt
         */
        uint32_t core_0_area_pc:32;
    };
    uint32_t val;
} assist_debug_core_0_area_pc_reg_t;

/** Type of core_0_area_sp register
 *  core0 area sp status register
 */
typedef union {
    struct {
        /** core_0_area_sp : RO; bitpos: [31:0]; default: 0;
         *  the PC when first touch region monitor interrupt
         */
        uint32_t core_0_area_sp:32;
    };
    uint32_t val;
} assist_debug_core_0_area_sp_reg_t;

/** Type of core_0_sp_min register
 *  stack min value
 */
typedef union {
    struct {
        /** core_0_sp_min : R/W; bitpos: [31:0]; default: 0;
         *  core0 sp region configuration regsiter
         */
        uint32_t core_0_sp_min:32;
    };
    uint32_t val;
} assist_debug_core_0_sp_min_reg_t;

/** Type of core_0_sp_max register
 *  stack max value
 */
typedef union {
    struct {
        /** core_0_sp_max : R/W; bitpos: [31:0]; default: 4294967295;
         *  core0 sp pc status register
         */
        uint32_t core_0_sp_max:32;
    };
    uint32_t val;
} assist_debug_core_0_sp_max_reg_t;

/** Type of core_0_sp_pc register
 *  stack monitor pc status register
 */
typedef union {
    struct {
        /** core_0_sp_pc : RO; bitpos: [31:0]; default: 0;
         *  This regsiter stores the PC when trigger stack monitor.
         */
        uint32_t core_0_sp_pc:32;
    };
    uint32_t val;
} assist_debug_core_0_sp_pc_reg_t;


/** Group: interrupt configuration register */
/** Type of core_0_intr_raw register
 *  core0 monitor interrupt status register
 */
typedef union {
    struct {
        /** core_0_area_dram0_0_rd_raw : RO; bitpos: [0]; default: 0;
         *  Core0 dram0 area0 read monitor interrupt status
         */
        uint32_t core_0_area_dram0_0_rd_raw:1;
        /** core_0_area_dram0_0_wr_raw : RO; bitpos: [1]; default: 0;
         *  Core0 dram0 area0 write monitor interrupt status
         */
        uint32_t core_0_area_dram0_0_wr_raw:1;
        /** core_0_area_dram0_1_rd_raw : RO; bitpos: [2]; default: 0;
         *  Core0 dram0 area1 read monitor interrupt status
         */
        uint32_t core_0_area_dram0_1_rd_raw:1;
        /** core_0_area_dram0_1_wr_raw : RO; bitpos: [3]; default: 0;
         *  Core0 dram0 area1 write monitor interrupt status
         */
        uint32_t core_0_area_dram0_1_wr_raw:1;
        /** core_0_area_pif_0_rd_raw : RO; bitpos: [4]; default: 0;
         *  Core0 PIF area0 read monitor interrupt status
         */
        uint32_t core_0_area_pif_0_rd_raw:1;
        /** core_0_area_pif_0_wr_raw : RO; bitpos: [5]; default: 0;
         *  Core0 PIF area0 write monitor interrupt status
         */
        uint32_t core_0_area_pif_0_wr_raw:1;
        /** core_0_area_pif_1_rd_raw : RO; bitpos: [6]; default: 0;
         *  Core0 PIF area1 read monitor interrupt status
         */
        uint32_t core_0_area_pif_1_rd_raw:1;
        /** core_0_area_pif_1_wr_raw : RO; bitpos: [7]; default: 0;
         *  Core0 PIF area1 write monitor interrupt status
         */
        uint32_t core_0_area_pif_1_wr_raw:1;
        /** core_0_sp_spill_min_raw : RO; bitpos: [8]; default: 0;
         *  Core0 stackpoint underflow monitor interrupt status
         */
        uint32_t core_0_sp_spill_min_raw:1;
        /** core_0_sp_spill_max_raw : RO; bitpos: [9]; default: 0;
         *  Core0 stackpoint overflow monitor interrupt status
         */
        uint32_t core_0_sp_spill_max_raw:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} assist_debug_core_0_intr_raw_reg_t;

/** Type of core_0_intr_ena register
 *  core0 monitor interrupt enable register
 */
typedef union {
    struct {
        /** core_0_area_dram0_0_rd_intr_ena : R/W; bitpos: [0]; default: 0;
         *  Core0 dram0 area0 read monitor interrupt enable
         */
        uint32_t core_0_area_dram0_0_rd_intr_ena:1;
        /** core_0_area_dram0_0_wr_intr_ena : R/W; bitpos: [1]; default: 0;
         *  Core0 dram0 area0 write monitor interrupt enable
         */
        uint32_t core_0_area_dram0_0_wr_intr_ena:1;
        /** core_0_area_dram0_1_rd_intr_ena : R/W; bitpos: [2]; default: 0;
         *  Core0 dram0 area1 read monitor interrupt enable
         */
        uint32_t core_0_area_dram0_1_rd_intr_ena:1;
        /** core_0_area_dram0_1_wr_intr_ena : R/W; bitpos: [3]; default: 0;
         *  Core0 dram0 area1 write monitor interrupt enable
         */
        uint32_t core_0_area_dram0_1_wr_intr_ena:1;
        /** core_0_area_pif_0_rd_intr_ena : R/W; bitpos: [4]; default: 0;
         *  Core0 PIF area0 read monitor interrupt enable
         */
        uint32_t core_0_area_pif_0_rd_intr_ena:1;
        /** core_0_area_pif_0_wr_intr_ena : R/W; bitpos: [5]; default: 0;
         *  Core0 PIF area0 write monitor interrupt enable
         */
        uint32_t core_0_area_pif_0_wr_intr_ena:1;
        /** core_0_area_pif_1_rd_intr_ena : R/W; bitpos: [6]; default: 0;
         *  Core0 PIF area1 read monitor interrupt enable
         */
        uint32_t core_0_area_pif_1_rd_intr_ena:1;
        /** core_0_area_pif_1_wr_intr_ena : R/W; bitpos: [7]; default: 0;
         *  Core0 PIF area1 write monitor interrupt enable
         */
        uint32_t core_0_area_pif_1_wr_intr_ena:1;
        /** core_0_sp_spill_min_intr_ena : R/W; bitpos: [8]; default: 0;
         *  Core0 stackpoint underflow monitor interrupt enable
         */
        uint32_t core_0_sp_spill_min_intr_ena:1;
        /** core_0_sp_spill_max_intr_ena : R/W; bitpos: [9]; default: 0;
         *  Core0 stackpoint overflow monitor interrupt enable
         */
        uint32_t core_0_sp_spill_max_intr_ena:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} assist_debug_core_0_intr_ena_reg_t;

/** Type of core_0_intr_clr register
 *  core0 monitor interrupt clr register
 */
typedef union {
    struct {
        /** core_0_area_dram0_0_rd_clr : WT; bitpos: [0]; default: 0;
         *  Core0 dram0 area0 read monitor interrupt clr
         */
        uint32_t core_0_area_dram0_0_rd_clr:1;
        /** core_0_area_dram0_0_wr_clr : WT; bitpos: [1]; default: 0;
         *  Core0 dram0 area0 write monitor interrupt clr
         */
        uint32_t core_0_area_dram0_0_wr_clr:1;
        /** core_0_area_dram0_1_rd_clr : WT; bitpos: [2]; default: 0;
         *  Core0 dram0 area1 read monitor interrupt clr
         */
        uint32_t core_0_area_dram0_1_rd_clr:1;
        /** core_0_area_dram0_1_wr_clr : WT; bitpos: [3]; default: 0;
         *  Core0 dram0 area1 write monitor interrupt clr
         */
        uint32_t core_0_area_dram0_1_wr_clr:1;
        /** core_0_area_pif_0_rd_clr : WT; bitpos: [4]; default: 0;
         *  Core0 PIF area0 read monitor interrupt clr
         */
        uint32_t core_0_area_pif_0_rd_clr:1;
        /** core_0_area_pif_0_wr_clr : WT; bitpos: [5]; default: 0;
         *  Core0 PIF area0 write monitor interrupt clr
         */
        uint32_t core_0_area_pif_0_wr_clr:1;
        /** core_0_area_pif_1_rd_clr : WT; bitpos: [6]; default: 0;
         *  Core0 PIF area1 read monitor interrupt clr
         */
        uint32_t core_0_area_pif_1_rd_clr:1;
        /** core_0_area_pif_1_wr_clr : WT; bitpos: [7]; default: 0;
         *  Core0 PIF area1 write monitor interrupt clr
         */
        uint32_t core_0_area_pif_1_wr_clr:1;
        /** core_0_sp_spill_min_clr : WT; bitpos: [8]; default: 0;
         *  Core0 stackpoint underflow monitor interrupt clr
         */
        uint32_t core_0_sp_spill_min_clr:1;
        /** core_0_sp_spill_max_clr : WT; bitpos: [9]; default: 0;
         *  Core0 stackpoint overflow monitor interrupt clr
         */
        uint32_t core_0_sp_spill_max_clr:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} assist_debug_core_0_intr_clr_reg_t;


/** Group: pc reording configuration register */
/** Type of core_0_rcd_en register
 *  record enable configuration register
 */
typedef union {
    struct {
        /** core_0_rcd_recorden : R/W; bitpos: [0]; default: 0;
         *  Set 1 to enable record PC
         */
        uint32_t core_0_rcd_recorden:1;
        /** core_0_rcd_pdebugen : R/W; bitpos: [1]; default: 0;
         *  Set 1 to enable cpu pdebug function, must set this bit can get cpu PC
         */
        uint32_t core_0_rcd_pdebugen:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} assist_debug_core_0_rcd_en_reg_t;


/** Group: pc reording status register */
/** Type of core_0_rcd_pdebugpc register
 *  record status regsiter
 */
typedef union {
    struct {
        /** core_0_rcd_pdebugpc : RO; bitpos: [31:0]; default: 0;
         *  recorded PC
         */
        uint32_t core_0_rcd_pdebugpc:32;
    };
    uint32_t val;
} assist_debug_core_0_rcd_pdebugpc_reg_t;

/** Type of core_0_rcd_pdebugsp register
 *  record status regsiter
 */
typedef union {
    struct {
        /** core_0_rcd_pdebugsp : RO; bitpos: [31:0]; default: 0;
         *  recorded sp
         */
        uint32_t core_0_rcd_pdebugsp:32;
    };
    uint32_t val;
} assist_debug_core_0_rcd_pdebugsp_reg_t;


/** Group: cpu status registers */
/** Type of core_0_lastpc_before_exception register
 *  cpu status register
 */
typedef union {
    struct {
        /** core_0_lastpc_before_exc : RO; bitpos: [31:0]; default: 0;
         *  cpu's lastpc before exception
         */
        uint32_t core_0_lastpc_before_exc:32;
    };
    uint32_t val;
} assist_debug_core_0_lastpc_before_exception_reg_t;

/** Type of core_0_debug_mode register
 *  cpu status register
 */
typedef union {
    struct {
        /** core_0_debug_mode : RO; bitpos: [0]; default: 0;
         *  cpu debug mode status, 1 means cpu enter debug mode.
         */
        uint32_t core_0_debug_mode:1;
        /** core_0_debug_module_active : RO; bitpos: [1]; default: 0;
         *  cpu debug_module active status
         */
        uint32_t core_0_debug_module_active:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} assist_debug_core_0_debug_mode_reg_t;

/** Type of core_0_dmactive_mode register
 *  cpu status register
 */
typedef union {
    struct {
        /** core_0_dmactive_lpcore : RO; bitpos: [0]; default: 0;
         *  need desc
         */
        uint32_t core_0_dmactive_lpcore:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} assist_debug_core_0_dmactive_mode_reg_t;


/** Group: Configuration Registers */
/** Type of clock_gate register
 *  clock register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Set 1 force on the clock gate
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} assist_debug_clock_gate_reg_t;

/** Type of date register
 *  version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35725648;
         *  version register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} assist_debug_date_reg_t;


typedef struct assist_debug_dev_t {
    volatile assist_debug_core_0_montr_ena_reg_t core_0_montr_ena;
    volatile assist_debug_core_0_intr_raw_reg_t core_0_intr_raw;
    volatile assist_debug_core_0_intr_ena_reg_t core_0_intr_ena;
    volatile assist_debug_core_0_intr_clr_reg_t core_0_intr_clr;
    volatile assist_debug_core_0_area_dram0_0_min_reg_t core_0_area_dram0_0_min;
    volatile assist_debug_core_0_area_dram0_0_max_reg_t core_0_area_dram0_0_max;
    volatile assist_debug_core_0_area_dram0_1_min_reg_t core_0_area_dram0_1_min;
    volatile assist_debug_core_0_area_dram0_1_max_reg_t core_0_area_dram0_1_max;
    volatile assist_debug_core_0_area_pif_0_min_reg_t core_0_area_pif_0_min;
    volatile assist_debug_core_0_area_pif_0_max_reg_t core_0_area_pif_0_max;
    volatile assist_debug_core_0_area_pif_1_min_reg_t core_0_area_pif_1_min;
    volatile assist_debug_core_0_area_pif_1_max_reg_t core_0_area_pif_1_max;
    volatile assist_debug_core_0_area_pc_reg_t core_0_area_pc;
    volatile assist_debug_core_0_area_sp_reg_t core_0_area_sp;
    volatile assist_debug_core_0_sp_min_reg_t core_0_sp_min;
    volatile assist_debug_core_0_sp_max_reg_t core_0_sp_max;
    volatile assist_debug_core_0_sp_pc_reg_t core_0_sp_pc;
    volatile assist_debug_core_0_rcd_en_reg_t core_0_rcd_en;
    volatile assist_debug_core_0_rcd_pdebugpc_reg_t core_0_rcd_pdebugpc;
    volatile assist_debug_core_0_rcd_pdebugsp_reg_t core_0_rcd_pdebugsp;
    uint32_t reserved_050[8];
    volatile assist_debug_core_0_lastpc_before_exception_reg_t core_0_lastpc_before_exception;
    volatile assist_debug_core_0_debug_mode_reg_t core_0_debug_mode;
    volatile assist_debug_core_0_dmactive_mode_reg_t core_0_dmactive_mode;
    volatile assist_debug_clock_gate_reg_t clock_gate;
    uint32_t reserved_080[223];
    volatile assist_debug_date_reg_t date;
} assist_debug_dev_t;

extern assist_debug_dev_t ASSIST_DEBUG;

#ifndef __cplusplus
_Static_assert(sizeof(assist_debug_dev_t) == 0x400, "Invalid size of assist_debug_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
