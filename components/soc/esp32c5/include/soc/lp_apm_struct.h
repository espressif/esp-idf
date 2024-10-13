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

/** Group: Region filter enable register */
/** Type of region_filter_en register
 *  Region filter enable register
 */
typedef union {
    struct {
        /** region_filter_en : R/W; bitpos: [3:0]; default: 1;
         *  Configure bit $n (0-3) to enable region $n.\\
         *  0: disable \\
         *  1: enable  \\
         */
        uint32_t region_filter_en:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lp_apm_region_filter_en_reg_t;


/** Group: Region address register */
/** Type of regionn_addr_start register
 *  Region address register
 */
typedef union {
    struct {
        /** regionn_addr_start : R/W; bitpos: [31:0]; default: 0;
         *  Configures start address of region n.
         */
        uint32_t regionn_addr_start:32;
    };
    uint32_t val;
} lp_apm_regionn_addr_start_reg_t;

/** Type of regionn_addr_end register
 *  Region address register
 */
typedef union {
    struct {
        /** regionn_addr_end : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures end address of region n.
         */
        uint32_t regionn_addr_end:32;
    };
    uint32_t val;
} lp_apm_regionn_addr_end_reg_t;


/** Group: Region access authority attribute register */
/** Type of regionn_attr register
 *  Region access authority attribute register
 */
typedef union {
    struct {
        /** regionn_r0_x : R/W; bitpos: [0]; default: 0;
         *  Configures the execution authority of REE_MODE 0 in region n.
         */
        uint32_t regionn_r0_x:1;
        /** regionn_r0_w : R/W; bitpos: [1]; default: 0;
         *  Configures the write authority of REE_MODE 0 in region n.
         */
        uint32_t regionn_r0_w:1;
        /** regionn_r0_r : R/W; bitpos: [2]; default: 0;
         *  Configures the read authority of REE_MODE 0 in region n.
         */
        uint32_t regionn_r0_r:1;
        uint32_t reserved_3:1;
        /** regionn_r1_x : R/W; bitpos: [4]; default: 0;
         *  Configures the execution authority of REE_MODE 1 in region n.
         */
        uint32_t regionn_r1_x:1;
        /** regionn_r1_w : R/W; bitpos: [5]; default: 0;
         *  Configures the write authority of REE_MODE 1 in region n.
         */
        uint32_t regionn_r1_w:1;
        /** regionn_r1_r : R/W; bitpos: [6]; default: 0;
         *  Configures the read authority of REE_MODE 1 in region n.
         */
        uint32_t regionn_r1_r:1;
        uint32_t reserved_7:1;
        /** regionn_r2_x : R/W; bitpos: [8]; default: 0;
         *  Configures the execution authority of REE_MODE 2 in region n.
         */
        uint32_t regionn_r2_x:1;
        /** regionn_r2_w : R/W; bitpos: [9]; default: 0;
         *  Configures the write authority of REE_MODE 2 in region n.
         */
        uint32_t regionn_r2_w:1;
        /** regionn_r2_r : R/W; bitpos: [10]; default: 0;
         *  Configures the read authority of REE_MODE 2 in region n.
         */
        uint32_t regionn_r2_r:1;
        /** regionn_lock : R/W; bitpos: [11]; default: 0;
         *  Set 1 to lock region0 configuration
         */
        uint32_t regionn_lock:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_apm_regionn_attr_reg_t;


/** Group: function control register */
/** Type of func_ctrl register
 *  APM function control register
 */
typedef union {
    struct {
        /** m0_func_en : R/W; bitpos: [0]; default: 1;
         *  PMS M0 function enable
         */
        uint32_t m0_func_en:1;
        /** m1_func_en : R/W; bitpos: [1]; default: 1;
         *  PMS M1 function enable
         */
        uint32_t m1_func_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_apm_func_ctrl_reg_t;


/** Group: M0 status register */
/** Type of m0_status register
 *  M0 status register
 */
typedef union {
    struct {
        /** m0_exception_status : RO; bitpos: [1:0]; default: 0;
         *  Represents exception status.\\
         *  bit0: 1 represents authority_exception \\
         *  bit1: 1 represents space_exception  \\
         */
        uint32_t m0_exception_status:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_apm_m0_status_reg_t;


/** Group: M0 status clear register */
/** Type of m0_status_clr register
 *  M0 status clear register
 */
typedef union {
    struct {
        /** m0_exception_status_clr : WT; bitpos: [0]; default: 0;
         *  Configures to clear exception status.
         */
        uint32_t m0_exception_status_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_apm_m0_status_clr_reg_t;


/** Group: M0 exception_info0 register */
/** Type of m0_exception_info0 register
 *  M0 exception_info0 register
 */
typedef union {
    struct {
        /** m0_exception_region : RO; bitpos: [3:0]; default: 0;
         *  Represents exception region.
         */
        uint32_t m0_exception_region:4;
        uint32_t reserved_4:12;
        /** m0_exception_mode : RO; bitpos: [17:16]; default: 0;
         *  Represents exception mode.
         */
        uint32_t m0_exception_mode:2;
        /** m0_exception_id : RO; bitpos: [22:18]; default: 0;
         *  Represents exception id information.
         */
        uint32_t m0_exception_id:5;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} lp_apm_m0_exception_info0_reg_t;


/** Group: M0 exception_info1 register */
/** Type of m0_exception_info1 register
 *  M0 exception_info1 register
 */
typedef union {
    struct {
        /** m0_exception_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents exception addr.
         */
        uint32_t m0_exception_addr:32;
    };
    uint32_t val;
} lp_apm_m0_exception_info1_reg_t;


/** Group: M1 status register */
/** Type of m1_status register
 *  M1 status register
 */
typedef union {
    struct {
        /** m1_exception_status : RO; bitpos: [1:0]; default: 0;
         *  Represents exception status.\\
         *  bit0: 1 represents authority_exception \\
         *  bit1: 1 represents space_exception  \\
         */
        uint32_t m1_exception_status:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_apm_m1_status_reg_t;


/** Group: M1 status clear register */
/** Type of m1_status_clr register
 *  M1 status clear register
 */
typedef union {
    struct {
        /** m1_exception_status_clr : WT; bitpos: [0]; default: 0;
         *  Configures to clear exception status.
         */
        uint32_t m1_exception_status_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_apm_m1_status_clr_reg_t;


/** Group: M1 exception_info0 register */
/** Type of m1_exception_info0 register
 *  M1 exception_info0 register
 */
typedef union {
    struct {
        /** m1_exception_region : RO; bitpos: [3:0]; default: 0;
         *  Represents exception region.
         */
        uint32_t m1_exception_region:4;
        uint32_t reserved_4:12;
        /** m1_exception_mode : RO; bitpos: [17:16]; default: 0;
         *  Represents exception mode.
         */
        uint32_t m1_exception_mode:2;
        /** m1_exception_id : RO; bitpos: [22:18]; default: 0;
         *  Represents exception id information.
         */
        uint32_t m1_exception_id:5;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} lp_apm_m1_exception_info0_reg_t;


/** Group: M1 exception_info1 register */
/** Type of m1_exception_info1 register
 *  M1 exception_info1 register
 */
typedef union {
    struct {
        /** m1_exception_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents exception addr.
         */
        uint32_t m1_exception_addr:32;
    };
    uint32_t val;
} lp_apm_m1_exception_info1_reg_t;


/** Group: APM interrupt enable register */
/** Type of int_en register
 *  APM interrupt enable register
 */
typedef union {
    struct {
        /** m0_apm_int_en : R/W; bitpos: [0]; default: 0;
         *  Configures to enable APM M0 interrupt.\\
         *  0: disable \\
         *  1: enable  \\
         */
        uint32_t m0_apm_int_en:1;
        /** m1_apm_int_en : R/W; bitpos: [1]; default: 0;
         *  Configures to enable APM M1 interrupt.\\
         *  0: disable \\
         *  1: enable  \\
         */
        uint32_t m1_apm_int_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_apm_int_en_reg_t;


/** Group: clock gating register */
/** Type of clock_gate register
 *  clock gating register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to keep the clock always on.\\
         *  0: enable automatic clock gating \\
         *  1: keep the clock always on  \\
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_apm_clock_gate_reg_t;


/** Group: Version control register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35725664;
         *  Version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_apm_date_reg_t;


typedef struct {
    volatile lp_apm_region_filter_en_reg_t region_filter_en;
    volatile lp_apm_regionn_addr_start_reg_t region0_addr_start;
    volatile lp_apm_regionn_addr_end_reg_t region0_addr_end;
    volatile lp_apm_regionn_attr_reg_t region0_attr;
    volatile lp_apm_regionn_addr_start_reg_t region1_addr_start;
    volatile lp_apm_regionn_addr_end_reg_t region1_addr_end;
    volatile lp_apm_regionn_attr_reg_t region1_attr;
    volatile lp_apm_regionn_addr_start_reg_t region2_addr_start;
    volatile lp_apm_regionn_addr_end_reg_t region2_addr_end;
    volatile lp_apm_regionn_attr_reg_t region2_attr;
    volatile lp_apm_regionn_addr_start_reg_t region3_addr_start;
    volatile lp_apm_regionn_addr_end_reg_t region3_addr_end;
    volatile lp_apm_regionn_attr_reg_t region3_attr;
    uint32_t reserved_034[36];
    volatile lp_apm_func_ctrl_reg_t func_ctrl;
    volatile lp_apm_m0_status_reg_t m0_status;
    volatile lp_apm_m0_status_clr_reg_t m0_status_clr;
    volatile lp_apm_m0_exception_info0_reg_t m0_exception_info0;
    volatile lp_apm_m0_exception_info1_reg_t m0_exception_info1;
    volatile lp_apm_m1_status_reg_t m1_status;
    volatile lp_apm_m1_status_clr_reg_t m1_status_clr;
    volatile lp_apm_m1_exception_info0_reg_t m1_exception_info0;
    volatile lp_apm_m1_exception_info1_reg_t m1_exception_info1;
    volatile lp_apm_int_en_reg_t int_en;
    volatile lp_apm_clock_gate_reg_t clock_gate;
    uint32_t reserved_0f0[3];
    volatile lp_apm_date_reg_t date;
} lp_apm_dev_t;

extern lp_apm_dev_t LP_APM;

#ifndef __cplusplus
_Static_assert(sizeof(lp_apm_dev_t) == 0x100, "Invalid size of lp_apm_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
