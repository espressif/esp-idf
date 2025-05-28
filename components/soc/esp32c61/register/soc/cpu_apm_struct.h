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

/** Group: Region filter enable register */
/** Type of apm_region_filter_en register
 *  Region filter enable register
 */
typedef union {
    struct {
        /** apm_region_filter_en : R/W; bitpos: [7:0]; default: 1;
         *  Configure bit $n (0-7) to enable region $n.
         *  0: disable
         *  1: enable
         */
        uint32_t apm_region_filter_en:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} cpu_apm_region_filter_en_reg_t;


/** Group: Region address register */
/** Type of apm_regionn_addr_start register
 *  Region address register
 */
typedef union {
    struct {
        /** apm_regionn_addr_start_l : HRO; bitpos: [11:0]; default: 0;
         *  Low 12 bit, start address of region n.
         */
        uint32_t apm_regionn_addr_start_l:12;
        /** apm_regionn_addr_start : R/W; bitpos: [18:12]; default: 0;
         *  Configures start address of region n.
         */
        uint32_t apm_regionn_addr_start:7;
        /** apm_regionn_addr_start_h : HRO; bitpos: [31:19]; default: 2064;
         *  High 13 bit,  start address of region n.
         */
        uint32_t apm_regionn_addr_start_h:13;
    };
    uint32_t val;
} cpu_apm_regionn_addr_start_reg_t;

/** Type of apm_regionn_addr_end register
 *  Region address register
 */
typedef union {
    struct {
        /** apm_regionn_addr_end_l : HRO; bitpos: [11:0]; default: 4095;
         *  Low 12 bit, end address of region n.
         */
        uint32_t apm_regionn_addr_end_l:12;
        /** apm_regionn_addr_end : R/W; bitpos: [18:12]; default: 127;
         *  Configures end address of region n.
         */
        uint32_t apm_regionn_addr_end:7;
        /** apm_regionn_addr_end_h : HRO; bitpos: [31:19]; default: 2064;
         *  High 13 bit,  end address of region n.
         */
        uint32_t apm_regionn_addr_end_h:13;
    };
    uint32_t val;
} cpu_apm_regionn_addr_end_reg_t;


/** Group: Region access authority attribute register */
/** Type of apm_regionn_attr register
 *  Region access authority attribute register
 */
typedef union {
    struct {
        /** apm_regionn_r0_x : R/W; bitpos: [0]; default: 0;
         *  Configures the execution authority of REE_MODE 0 in region n.
         */
        uint32_t apm_regionn_r0_x:1;
        /** apm_regionn_r0_w : R/W; bitpos: [1]; default: 0;
         *  Configures the write authority of REE_MODE 0 in region n.
         */
        uint32_t apm_regionn_r0_w:1;
        /** apm_regionn_r0_r : R/W; bitpos: [2]; default: 0;
         *  Configures the read authority of REE_MODE 0 in region n.
         */
        uint32_t apm_regionn_r0_r:1;
        uint32_t reserved_3:1;
        /** apm_regionn_r1_x : R/W; bitpos: [4]; default: 0;
         *  Configures the execution authority of REE_MODE 1 in region n.
         */
        uint32_t apm_regionn_r1_x:1;
        /** apm_regionn_r1_w : R/W; bitpos: [5]; default: 0;
         *  Configures the write authority of REE_MODE 1 in region n.
         */
        uint32_t apm_regionn_r1_w:1;
        /** apm_regionn_r1_r : R/W; bitpos: [6]; default: 0;
         *  Configures the read authority of REE_MODE 1 in region n.
         */
        uint32_t apm_regionn_r1_r:1;
        uint32_t reserved_7:1;
        /** apm_regionn_r2_x : R/W; bitpos: [8]; default: 0;
         *  Configures the execution authority of REE_MODE 2 in region n.
         */
        uint32_t apm_regionn_r2_x:1;
        /** apm_regionn_r2_w : R/W; bitpos: [9]; default: 0;
         *  Configures the write authority of REE_MODE 2 in region n.
         */
        uint32_t apm_regionn_r2_w:1;
        /** apm_regionn_r2_r : R/W; bitpos: [10]; default: 0;
         *  Configures the read authority of REE_MODE 2 in region n.
         */
        uint32_t apm_regionn_r2_r:1;
        /** apm_regionn_lock : R/W; bitpos: [11]; default: 0;
         *  Set 1 to lock region0 configuration
         */
        uint32_t apm_regionn_lock:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} cpu_apm_regionn_attr_reg_t;


/** Group: function control register */
/** Type of apm_func_ctrl register
 *  APM function control register
 */
typedef union {
    struct {
        /** apm_m0_func_en : R/W; bitpos: [0]; default: 1;
         *  PMS M0 function enable
         */
        uint32_t apm_m0_func_en:1;
        /** apm_m1_func_en : R/W; bitpos: [1]; default: 1;
         *  PMS M1 function enable
         */
        uint32_t apm_m1_func_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} cpu_apm_func_ctrl_reg_t;


/** Group: M0 status register */
/** Type of apm_m0_status register
 *  M0 status register
 */
typedef union {
    struct {
        /** apm_m0_exception_status : RO; bitpos: [1:0]; default: 0;
         *  Represents exception status.
         *  bit0: 1 represents authority_exception
         *  bit1: 1 represents space_exception
         */
        uint32_t apm_m0_exception_status:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} cpu_apm_m0_status_reg_t;


/** Group: M0 status clear register */
/** Type of apm_m0_status_clr register
 *  M0 status clear register
 */
typedef union {
    struct {
        /** apm_m0_exception_status_clr : WT; bitpos: [0]; default: 0;
         *  Configures to clear exception status.
         */
        uint32_t apm_m0_exception_status_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} cpu_apm_m0_status_clr_reg_t;


/** Group: M0 exception_info0 register */
/** Type of apm_m0_exception_info0 register
 *  M0 exception_info0 register
 */
typedef union {
    struct {
        /** apm_m0_exception_region : RO; bitpos: [15:0]; default: 0;
         *  Represents exception region.
         */
        uint32_t apm_m0_exception_region:16;
        /** apm_m0_exception_mode : RO; bitpos: [17:16]; default: 0;
         *  Represents exception mode.
         */
        uint32_t apm_m0_exception_mode:2;
        /** apm_m0_exception_id : RO; bitpos: [22:18]; default: 0;
         *  Represents exception id information.
         */
        uint32_t apm_m0_exception_id:5;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} cpu_apm_m0_exception_info0_reg_t;


/** Group: M0 exception_info1 register */
/** Type of apm_m0_exception_info1 register
 *  M0 exception_info1 register
 */
typedef union {
    struct {
        /** apm_m0_exception_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents exception addr.
         */
        uint32_t apm_m0_exception_addr:32;
    };
    uint32_t val;
} cpu_apm_m0_exception_info1_reg_t;


/** Group: M1 status register */
/** Type of apm_m1_status register
 *  M1 status register
 */
typedef union {
    struct {
        /** apm_m1_exception_status : RO; bitpos: [1:0]; default: 0;
         *  Represents exception status.
         *  bit0: 1 represents authority_exception
         *  bit1: 1 represents space_exception
         */
        uint32_t apm_m1_exception_status:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} cpu_apm_m1_status_reg_t;


/** Group: M1 status clear register */
/** Type of apm_m1_status_clr register
 *  M1 status clear register
 */
typedef union {
    struct {
        /** apm_m1_exception_status_clr : WT; bitpos: [0]; default: 0;
         *  Configures to clear exception status.
         */
        uint32_t apm_m1_exception_status_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} cpu_apm_m1_status_clr_reg_t;


/** Group: M1 exception_info0 register */
/** Type of apm_m1_exception_info0 register
 *  M1 exception_info0 register
 */
typedef union {
    struct {
        /** apm_m1_exception_region : RO; bitpos: [15:0]; default: 0;
         *  Represents exception region.
         */
        uint32_t apm_m1_exception_region:16;
        /** apm_m1_exception_mode : RO; bitpos: [17:16]; default: 0;
         *  Represents exception mode.
         */
        uint32_t apm_m1_exception_mode:2;
        /** apm_m1_exception_id : RO; bitpos: [22:18]; default: 0;
         *  Represents exception id information.
         */
        uint32_t apm_m1_exception_id:5;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} cpu_apm_m1_exception_info0_reg_t;


/** Group: M1 exception_info1 register */
/** Type of apm_m1_exception_info1 register
 *  M1 exception_info1 register
 */
typedef union {
    struct {
        /** apm_m1_exception_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents exception addr.
         */
        uint32_t apm_m1_exception_addr:32;
    };
    uint32_t val;
} cpu_apm_m1_exception_info1_reg_t;


/** Group: APM interrupt enable register */
/** Type of apm_int_en register
 *  APM interrupt enable register
 */
typedef union {
    struct {
        /** apm_m0_apm_int_en : R/W; bitpos: [0]; default: 0;
         *  Configures to enable APM M0 interrupt.
         *  0: disable
         *  1: enable
         */
        uint32_t apm_m0_apm_int_en:1;
        /** apm_m1_apm_int_en : R/W; bitpos: [1]; default: 0;
         *  Configures to enable APM M1 interrupt.
         *  0: disable
         *  1: enable
         */
        uint32_t apm_m1_apm_int_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} cpu_apm_int_en_reg_t;


/** Group: Clock gating register */
/** Type of apm_clock_gate register
 *  Clock gating register
 */
typedef union {
    struct {
        /** apm_clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to keep the clock always on.
         *  0: enable automatic clock gating
         *  1: keep the clock always on
         */
        uint32_t apm_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} cpu_apm_clock_gate_reg_t;


/** Group: Version control register */
/** Type of apm_date register
 *  Version control register
 */
typedef union {
    struct {
        /** apm_date : R/W; bitpos: [27:0]; default: 37823248;
         *  Version control register.
         */
        uint32_t apm_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cpu_apm_date_reg_t;


typedef struct {
    volatile cpu_apm_region_filter_en_reg_t apm_region_filter_en;
    volatile cpu_apm_regionn_addr_start_reg_t apm_region0_addr_start;
    volatile cpu_apm_regionn_addr_end_reg_t apm_region0_addr_end;
    volatile cpu_apm_regionn_attr_reg_t apm_region0_attr;
    volatile cpu_apm_regionn_addr_start_reg_t apm_region1_addr_start;
    volatile cpu_apm_regionn_addr_end_reg_t apm_region1_addr_end;
    volatile cpu_apm_regionn_attr_reg_t apm_region1_attr;
    volatile cpu_apm_regionn_addr_start_reg_t apm_region2_addr_start;
    volatile cpu_apm_regionn_addr_end_reg_t apm_region2_addr_end;
    volatile cpu_apm_regionn_attr_reg_t apm_region2_attr;
    volatile cpu_apm_regionn_addr_start_reg_t apm_region3_addr_start;
    volatile cpu_apm_regionn_addr_end_reg_t apm_region3_addr_end;
    volatile cpu_apm_regionn_attr_reg_t apm_region3_attr;
    volatile cpu_apm_regionn_addr_start_reg_t apm_region4_addr_start;
    volatile cpu_apm_regionn_addr_end_reg_t apm_region4_addr_end;
    volatile cpu_apm_regionn_attr_reg_t apm_region4_attr;
    volatile cpu_apm_regionn_addr_start_reg_t apm_region5_addr_start;
    volatile cpu_apm_regionn_addr_end_reg_t apm_region5_addr_end;
    volatile cpu_apm_regionn_attr_reg_t apm_region5_attr;
    volatile cpu_apm_regionn_addr_start_reg_t apm_region6_addr_start;
    volatile cpu_apm_regionn_addr_end_reg_t apm_region6_addr_end;
    volatile cpu_apm_regionn_attr_reg_t apm_region6_attr;
    volatile cpu_apm_regionn_addr_start_reg_t apm_region7_addr_start;
    volatile cpu_apm_regionn_addr_end_reg_t apm_region7_addr_end;
    volatile cpu_apm_regionn_attr_reg_t apm_region7_attr;
    uint32_t reserved_064[24];
    volatile cpu_apm_func_ctrl_reg_t apm_func_ctrl;
    volatile cpu_apm_m0_status_reg_t apm_m0_status;
    volatile cpu_apm_m0_status_clr_reg_t apm_m0_status_clr;
    volatile cpu_apm_m0_exception_info0_reg_t apm_m0_exception_info0;
    volatile cpu_apm_m0_exception_info1_reg_t apm_m0_exception_info1;
    volatile cpu_apm_m1_status_reg_t apm_m1_status;
    volatile cpu_apm_m1_status_clr_reg_t apm_m1_status_clr;
    volatile cpu_apm_m1_exception_info0_reg_t apm_m1_exception_info0;
    volatile cpu_apm_m1_exception_info1_reg_t apm_m1_exception_info1;
    uint32_t reserved_0e8[12];
    volatile cpu_apm_int_en_reg_t apm_int_en;
    uint32_t reserved_11c[439];
    volatile cpu_apm_clock_gate_reg_t apm_clock_gate;
    volatile cpu_apm_date_reg_t apm_date;
} cpu_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(cpu_dev_t) == 0x800, "Invalid size of cpu_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
