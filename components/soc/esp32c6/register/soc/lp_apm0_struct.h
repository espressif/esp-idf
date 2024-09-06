/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Region filter enable register */
/** Type of region_filter_en register
 *  Region filter enable register
 */
typedef union {
    struct {
        /** region_filter_en : R/W; bitpos: [3:0]; default: 1;
         *  Region filter enable
         */
        uint32_t region_filter_en:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lp_apm0_region_filter_en_reg_t;


/** Group: Region address register */
/** Type of region0_addr_start register
 *  Region address register
 */
typedef union {
    struct {
        /** region0_addr_start : R/W; bitpos: [31:0]; default: 0;
         *  Start address of region0
         */
        uint32_t region0_addr_start:32;
    };
    uint32_t val;
} lp_apm0_region0_addr_start_reg_t;

/** Type of region0_addr_end register
 *  Region address register
 */
typedef union {
    struct {
        /** region0_addr_end : R/W; bitpos: [31:0]; default: 4294967295;
         *  End address of region0
         */
        uint32_t region0_addr_end:32;
    };
    uint32_t val;
} lp_apm0_region0_addr_end_reg_t;

/** Type of region1_addr_start register
 *  Region address register
 */
typedef union {
    struct {
        /** region1_addr_start : R/W; bitpos: [31:0]; default: 0;
         *  Start address of region1
         */
        uint32_t region1_addr_start:32;
    };
    uint32_t val;
} lp_apm0_region1_addr_start_reg_t;

/** Type of region1_addr_end register
 *  Region address register
 */
typedef union {
    struct {
        /** region1_addr_end : R/W; bitpos: [31:0]; default: 4294967295;
         *  End address of region1
         */
        uint32_t region1_addr_end:32;
    };
    uint32_t val;
} lp_apm0_region1_addr_end_reg_t;

/** Type of region2_addr_start register
 *  Region address register
 */
typedef union {
    struct {
        /** region2_addr_start : R/W; bitpos: [31:0]; default: 0;
         *  Start address of region2
         */
        uint32_t region2_addr_start:32;
    };
    uint32_t val;
} lp_apm0_region2_addr_start_reg_t;

/** Type of region2_addr_end register
 *  Region address register
 */
typedef union {
    struct {
        /** region2_addr_end : R/W; bitpos: [31:0]; default: 4294967295;
         *  End address of region2
         */
        uint32_t region2_addr_end:32;
    };
    uint32_t val;
} lp_apm0_region2_addr_end_reg_t;

/** Type of region3_addr_start register
 *  Region address register
 */
typedef union {
    struct {
        /** region3_addr_start : R/W; bitpos: [31:0]; default: 0;
         *  Start address of region3
         */
        uint32_t region3_addr_start:32;
    };
    uint32_t val;
} lp_apm0_region3_addr_start_reg_t;

/** Type of region3_addr_end register
 *  Region address register
 */
typedef union {
    struct {
        /** region3_addr_end : R/W; bitpos: [31:0]; default: 4294967295;
         *  End address of region3
         */
        uint32_t region3_addr_end:32;
    };
    uint32_t val;
} lp_apm0_region3_addr_end_reg_t;


/** Group: Region access authority attribute register */
/** Type of region0_pms_attr register
 *  Region access authority attribute register
 */
typedef union {
    struct {
        /** region0_r0_pms_x : R/W; bitpos: [0]; default: 0;
         *  Region execute authority in REE_MODE0
         */
        uint32_t region0_r0_pms_x:1;
        /** region0_r0_pms_w : R/W; bitpos: [1]; default: 0;
         *  Region write authority in REE_MODE0
         */
        uint32_t region0_r0_pms_w:1;
        /** region0_r0_pms_r : R/W; bitpos: [2]; default: 0;
         *  Region read authority in REE_MODE0
         */
        uint32_t region0_r0_pms_r:1;
        uint32_t reserved_3:1;
        /** region0_r1_pms_x : R/W; bitpos: [4]; default: 0;
         *  Region execute authority in REE_MODE1
         */
        uint32_t region0_r1_pms_x:1;
        /** region0_r1_pms_w : R/W; bitpos: [5]; default: 0;
         *  Region write authority in REE_MODE1
         */
        uint32_t region0_r1_pms_w:1;
        /** region0_r1_pms_r : R/W; bitpos: [6]; default: 0;
         *  Region read authority in REE_MODE1
         */
        uint32_t region0_r1_pms_r:1;
        uint32_t reserved_7:1;
        /** region0_r2_pms_x : R/W; bitpos: [8]; default: 0;
         *  Region execute authority in REE_MODE2
         */
        uint32_t region0_r2_pms_x:1;
        /** region0_r2_pms_w : R/W; bitpos: [9]; default: 0;
         *  Region write authority in REE_MODE2
         */
        uint32_t region0_r2_pms_w:1;
        /** region0_r2_pms_r : R/W; bitpos: [10]; default: 0;
         *  Region read authority in REE_MODE2
         */
        uint32_t region0_r2_pms_r:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} lp_apm0_region0_pms_attr_reg_t;

/** Type of region1_pms_attr register
 *  Region access authority attribute register
 */
typedef union {
    struct {
        /** region1_r0_pms_x : R/W; bitpos: [0]; default: 0;
         *  Region execute authority in REE_MODE0
         */
        uint32_t region1_r0_pms_x:1;
        /** region1_r0_pms_w : R/W; bitpos: [1]; default: 0;
         *  Region write authority in REE_MODE0
         */
        uint32_t region1_r0_pms_w:1;
        /** region1_r0_pms_r : R/W; bitpos: [2]; default: 0;
         *  Region read authority in REE_MODE0
         */
        uint32_t region1_r0_pms_r:1;
        uint32_t reserved_3:1;
        /** region1_r1_pms_x : R/W; bitpos: [4]; default: 0;
         *  Region execute authority in REE_MODE1
         */
        uint32_t region1_r1_pms_x:1;
        /** region1_r1_pms_w : R/W; bitpos: [5]; default: 0;
         *  Region write authority in REE_MODE1
         */
        uint32_t region1_r1_pms_w:1;
        /** region1_r1_pms_r : R/W; bitpos: [6]; default: 0;
         *  Region read authority in REE_MODE1
         */
        uint32_t region1_r1_pms_r:1;
        uint32_t reserved_7:1;
        /** region1_r2_pms_x : R/W; bitpos: [8]; default: 0;
         *  Region execute authority in REE_MODE2
         */
        uint32_t region1_r2_pms_x:1;
        /** region1_r2_pms_w : R/W; bitpos: [9]; default: 0;
         *  Region write authority in REE_MODE2
         */
        uint32_t region1_r2_pms_w:1;
        /** region1_r2_pms_r : R/W; bitpos: [10]; default: 0;
         *  Region read authority in REE_MODE2
         */
        uint32_t region1_r2_pms_r:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} lp_apm0_region1_pms_attr_reg_t;

/** Type of region2_pms_attr register
 *  Region access authority attribute register
 */
typedef union {
    struct {
        /** region2_r0_pms_x : R/W; bitpos: [0]; default: 0;
         *  Region execute authority in REE_MODE0
         */
        uint32_t region2_r0_pms_x:1;
        /** region2_r0_pms_w : R/W; bitpos: [1]; default: 0;
         *  Region write authority in REE_MODE0
         */
        uint32_t region2_r0_pms_w:1;
        /** region2_r0_pms_r : R/W; bitpos: [2]; default: 0;
         *  Region read authority in REE_MODE0
         */
        uint32_t region2_r0_pms_r:1;
        uint32_t reserved_3:1;
        /** region2_r1_pms_x : R/W; bitpos: [4]; default: 0;
         *  Region execute authority in REE_MODE1
         */
        uint32_t region2_r1_pms_x:1;
        /** region2_r1_pms_w : R/W; bitpos: [5]; default: 0;
         *  Region write authority in REE_MODE1
         */
        uint32_t region2_r1_pms_w:1;
        /** region2_r1_pms_r : R/W; bitpos: [6]; default: 0;
         *  Region read authority in REE_MODE1
         */
        uint32_t region2_r1_pms_r:1;
        uint32_t reserved_7:1;
        /** region2_r2_pms_x : R/W; bitpos: [8]; default: 0;
         *  Region execute authority in REE_MODE2
         */
        uint32_t region2_r2_pms_x:1;
        /** region2_r2_pms_w : R/W; bitpos: [9]; default: 0;
         *  Region write authority in REE_MODE2
         */
        uint32_t region2_r2_pms_w:1;
        /** region2_r2_pms_r : R/W; bitpos: [10]; default: 0;
         *  Region read authority in REE_MODE2
         */
        uint32_t region2_r2_pms_r:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} lp_apm0_region2_pms_attr_reg_t;

/** Type of region3_pms_attr register
 *  Region access authority attribute register
 */
typedef union {
    struct {
        /** region3_r0_pms_x : R/W; bitpos: [0]; default: 0;
         *  Region execute authority in REE_MODE0
         */
        uint32_t region3_r0_pms_x:1;
        /** region3_r0_pms_w : R/W; bitpos: [1]; default: 0;
         *  Region write authority in REE_MODE0
         */
        uint32_t region3_r0_pms_w:1;
        /** region3_r0_pms_r : R/W; bitpos: [2]; default: 0;
         *  Region read authority in REE_MODE0
         */
        uint32_t region3_r0_pms_r:1;
        uint32_t reserved_3:1;
        /** region3_r1_pms_x : R/W; bitpos: [4]; default: 0;
         *  Region execute authority in REE_MODE1
         */
        uint32_t region3_r1_pms_x:1;
        /** region3_r1_pms_w : R/W; bitpos: [5]; default: 0;
         *  Region write authority in REE_MODE1
         */
        uint32_t region3_r1_pms_w:1;
        /** region3_r1_pms_r : R/W; bitpos: [6]; default: 0;
         *  Region read authority in REE_MODE1
         */
        uint32_t region3_r1_pms_r:1;
        uint32_t reserved_7:1;
        /** region3_r2_pms_x : R/W; bitpos: [8]; default: 0;
         *  Region execute authority in REE_MODE2
         */
        uint32_t region3_r2_pms_x:1;
        /** region3_r2_pms_w : R/W; bitpos: [9]; default: 0;
         *  Region write authority in REE_MODE2
         */
        uint32_t region3_r2_pms_w:1;
        /** region3_r2_pms_r : R/W; bitpos: [10]; default: 0;
         *  Region read authority in REE_MODE2
         */
        uint32_t region3_r2_pms_r:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} lp_apm0_region3_pms_attr_reg_t;


/** Group: PMS function control register */
/** Type of func_ctrl register
 *  PMS function control register
 */
typedef union {
    struct {
        /** m0_pms_func_en : R/W; bitpos: [0]; default: 1;
         *  PMS M0 function enable
         */
        uint32_t m0_pms_func_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_apm0_func_ctrl_reg_t;


/** Group: M0 status register */
/** Type of m0_status register
 *  M0 status register
 */
typedef union {
    struct {
        /** m0_exception_status : RO; bitpos: [1:0]; default: 0;
         *  Exception status
         */
        uint32_t m0_exception_status:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_apm0_m0_status_reg_t;


/** Group: M0 status clear register */
/** Type of m0_status_clr register
 *  M0 status clear register
 */
typedef union {
    struct {
        /** m0_region_status_clr : WT; bitpos: [0]; default: 0;
         *  Clear exception status
         */
        uint32_t m0_region_status_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_apm0_m0_status_clr_reg_t;


/** Group: M0 exception_info0 register */
/** Type of m0_exception_info0 register
 *  M0 exception_info0 register
 */
typedef union {
    struct {
        /** m0_exception_region : RO; bitpos: [3:0]; default: 0;
         *  Exception region
         */
        uint32_t m0_exception_region:4;
        uint32_t reserved_4:12;
        /** m0_exception_mode : RO; bitpos: [17:16]; default: 0;
         *  Exception mode
         */
        uint32_t m0_exception_mode:2;
        /** m0_exception_id : RO; bitpos: [22:18]; default: 0;
         *  Exception id information
         */
        uint32_t m0_exception_id:5;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} lp_apm0_m0_exception_info0_reg_t;


/** Group: M0 exception_info1 register */
/** Type of m0_exception_info1 register
 *  M0 exception_info1 register
 */
typedef union {
    struct {
        /** m0_exception_addr : RO; bitpos: [31:0]; default: 0;
         *  Exception addr
         */
        uint32_t m0_exception_addr:32;
    };
    uint32_t val;
} lp_apm0_m0_exception_info1_reg_t;


/** Group: APM interrupt enable register */
/** Type of int_en register
 *  APM interrupt enable register
 */
typedef union {
    struct {
        /** m0_apm_int_en : R/W; bitpos: [0]; default: 0;
         *  APM M0 interrupt enable
         */
        uint32_t m0_apm_int_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_apm0_int_en_reg_t;


/** Group: clock gating register */
/** Type of clock_gate register
 *  clock gating register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  reg_clk_en
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_apm0_clock_gate_reg_t;


/** Group: Version register */
/** Type of date register
 *  Version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35672640;
         *  reg_date
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_apm0_date_reg_t;


typedef struct lp_apm0_dev_t {
    volatile lp_apm0_region_filter_en_reg_t region_filter_en;
    volatile lp_apm0_region0_addr_start_reg_t region0_addr_start;
    volatile lp_apm0_region0_addr_end_reg_t region0_addr_end;
    volatile lp_apm0_region0_pms_attr_reg_t region0_pms_attr;
    volatile lp_apm0_region1_addr_start_reg_t region1_addr_start;
    volatile lp_apm0_region1_addr_end_reg_t region1_addr_end;
    volatile lp_apm0_region1_pms_attr_reg_t region1_pms_attr;
    volatile lp_apm0_region2_addr_start_reg_t region2_addr_start;
    volatile lp_apm0_region2_addr_end_reg_t region2_addr_end;
    volatile lp_apm0_region2_pms_attr_reg_t region2_pms_attr;
    volatile lp_apm0_region3_addr_start_reg_t region3_addr_start;
    volatile lp_apm0_region3_addr_end_reg_t region3_addr_end;
    volatile lp_apm0_region3_pms_attr_reg_t region3_pms_attr;
    uint32_t reserved_034[36];
    volatile lp_apm0_func_ctrl_reg_t func_ctrl;
    volatile lp_apm0_m0_status_reg_t m0_status;
    volatile lp_apm0_m0_status_clr_reg_t m0_status_clr;
    volatile lp_apm0_m0_exception_info0_reg_t m0_exception_info0;
    volatile lp_apm0_m0_exception_info1_reg_t m0_exception_info1;
    volatile lp_apm0_int_en_reg_t int_en;
    volatile lp_apm0_clock_gate_reg_t clock_gate;
    uint32_t reserved_0e0[455];
    volatile lp_apm0_date_reg_t date;
} lp_apm0_dev_t;

extern lp_apm0_dev_t LP_APM0;

#ifndef __cplusplus
_Static_assert(sizeof(lp_apm0_dev_t) == 0x800, "Invalid size of lp_apm0_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
