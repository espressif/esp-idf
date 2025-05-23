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

/** Group: Configuration Registers */
/** Type of region_filter_en register
 *  Region enable register
 */
typedef union {
    struct {
        /** region_filter_en : R/W; bitpos: [7:0]; default: 1;
         *  Configure bit $n (0-7) to enable region $n (0-7).
         *  0: Disable
         *  1: Enable
         */
        uint32_t region_filter_en:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_apm0_region_filter_en_reg_t;

/** Type of regionn_addr_start register
 *  Region address register
 */
typedef union {
    struct {
        /** regionn_addr_start : R/W; bitpos: [31:0]; default: 0;
         *  Configures the start address of region n.
         */
        uint32_t regionn_addr_start:32;
    };
    uint32_t val;
} lp_apm0_regionn_addr_start_reg_t;

/** Type of regionn_addr_end register
 *  Region address register
 */
typedef union {
    struct {
        /** regionn_addr_end : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures the end address of region n.
         */
        uint32_t regionn_addr_end:32;
    };
    uint32_t val;
} lp_apm0_regionn_addr_end_reg_t;

/** Type of regionn_attr register
 *  Region access permissions configuration register
 */
typedef union {
    struct {
        /** regionn_r0_x : R/W; bitpos: [0]; default: 0;
         *  Configures the execution permission in region n in REE0 mode.
         */
        uint32_t regionn_r0_x:1;
        /** regionn_r0_w : R/W; bitpos: [1]; default: 0;
         *  Configures the write permission in region n in REE0 mode.
         */
        uint32_t regionn_r0_w:1;
        /** regionn_r0_r : R/W; bitpos: [2]; default: 0;
         *  Configures the read permission in region n in REE0 mode.
         */
        uint32_t regionn_r0_r:1;
        uint32_t reserved_3:1;
        /** regionn_r1_x : R/W; bitpos: [4]; default: 0;
         *  Configures the execution permission in region n in REE1 mode.
         */
        uint32_t regionn_r1_x:1;
        /** regionn_r1_w : R/W; bitpos: [5]; default: 0;
         *  Configures the write permission in region n in REE1 mode.
         */
        uint32_t regionn_r1_w:1;
        /** regionn_r1_r : R/W; bitpos: [6]; default: 0;
         *  Configures the read permission in region n in REE1 mode.
         */
        uint32_t regionn_r1_r:1;
        uint32_t reserved_7:1;
        /** regionn_r2_x : R/W; bitpos: [8]; default: 0;
         *  Configures the execution permission in region n in REE2 mode.
         */
        uint32_t regionn_r2_x:1;
        /** regionn_r2_w : R/W; bitpos: [9]; default: 0;
         *  Configures the write permission in region n in REE2 mode.
         */
        uint32_t regionn_r2_w:1;
        /** regionn_r2_r : R/W; bitpos: [10]; default: 0;
         *  Configures the read permission in region n in REE2 mode.
         */
        uint32_t regionn_r2_r:1;
        /** regionn_lock : R/W; bitpos: [11]; default: 0;
         *  Configures to lock the value of region n configuration registers
         *  (LP_APM0_REGIONn_ADDR_START_REG, LP_APM0_REGIONn_ADDR_END_REG and
         *  LP_APM0_REGIONn_ATTR_REG).
         *  0: Do not lock
         *  1: Lock
         */
        uint32_t regionn_lock:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_apm0_regionn_attr_reg_t;

/** Type of func_ctrl register
 *  APM access path permission management register
 */
typedef union {
    struct {
        /** m0_func_en : R/W; bitpos: [0]; default: 1;
         *  Configures to enable permission management for LP_APM0_CTRL M0.
         */
        uint32_t m0_func_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_apm0_func_ctrl_reg_t;


/** Group: Status Registers */
/** Type of m0_status register
 *  LP_APM0_CTRL M0 status register
 */
typedef union {
    struct {
        /** m0_exception_status : RO; bitpos: [1:0]; default: 0;
         *  Represents exception status.
         *  bit0: 1 represents permission restrictions
         *  bit1: 1 represents address out of bounds
         */
        uint32_t m0_exception_status:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_apm0_m0_status_reg_t;

/** Type of m0_status_clr register
 *  LP_APM0_CTRL M0 status clear register
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
} lp_apm0_m0_status_clr_reg_t;

/** Type of m0_exception_info0 register
 *  LP_APM0_CTRL M0 exception information register
 */
typedef union {
    struct {
        /** m0_exception_region : RO; bitpos: [7:0]; default: 0;
         *  Represents the region where an exception occurs.
         */
        uint32_t m0_exception_region:8;
        uint32_t reserved_8:8;
        /** m0_exception_mode : RO; bitpos: [17:16]; default: 0;
         *  Represents the master's security mode when an exception occurs.
         */
        uint32_t m0_exception_mode:2;
        /** m0_exception_id : RO; bitpos: [22:18]; default: 0;
         *  Represents master ID when an exception occurs.
         */
        uint32_t m0_exception_id:5;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} lp_apm0_m0_exception_info0_reg_t;

/** Type of m0_exception_info1 register
 *  LP_APM0_CTRL M0 exception information register
 */
typedef union {
    struct {
        /** m0_exception_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the access address when an exception occurs.
         */
        uint32_t m0_exception_addr:32;
    };
    uint32_t val;
} lp_apm0_m0_exception_info1_reg_t;


/** Group: Interrupt Registers */
/** Type of int_en register
 *  LP_APM0_CTRL interrupt enable register
 */
typedef union {
    struct {
        /** m0_apm_int_en : R/W; bitpos: [0]; default: 0;
         *  Configures to enable LP_APM0_CTRL M0 interrupt.
         *  0: Disable
         *  1: Enable
         */
        uint32_t m0_apm_int_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_apm0_int_en_reg_t;


/** Group: Clock Gating Registers */
/** Type of clock_gate register
 *  Clock gating register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to keep the clock always on.
         *  0: Enable automatic clock gating
         *  1: Keep the clock always on
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_apm0_clock_gate_reg_t;


/** Group: Version Control Registers */
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
} lp_apm0_date_reg_t;


typedef struct {
    volatile lp_apm0_region_filter_en_reg_t region_filter_en;
    volatile lp_apm0_regionn_addr_start_reg_t region0_addr_start;
    volatile lp_apm0_regionn_addr_end_reg_t region0_addr_end;
    volatile lp_apm0_regionn_attr_reg_t region0_attr;
    volatile lp_apm0_regionn_addr_start_reg_t region1_addr_start;
    volatile lp_apm0_regionn_addr_end_reg_t region1_addr_end;
    volatile lp_apm0_regionn_attr_reg_t region1_attr;
    volatile lp_apm0_regionn_addr_start_reg_t region2_addr_start;
    volatile lp_apm0_regionn_addr_end_reg_t region2_addr_end;
    volatile lp_apm0_regionn_attr_reg_t region2_attr;
    volatile lp_apm0_regionn_addr_start_reg_t region3_addr_start;
    volatile lp_apm0_regionn_addr_end_reg_t region3_addr_end;
    volatile lp_apm0_regionn_attr_reg_t region3_attr;
    volatile lp_apm0_regionn_addr_start_reg_t region4_addr_start;
    volatile lp_apm0_regionn_addr_end_reg_t region4_addr_end;
    volatile lp_apm0_regionn_attr_reg_t region4_attr;
    volatile lp_apm0_regionn_addr_start_reg_t region5_addr_start;
    volatile lp_apm0_regionn_addr_end_reg_t region5_addr_end;
    volatile lp_apm0_regionn_attr_reg_t region5_attr;
    volatile lp_apm0_regionn_addr_start_reg_t region6_addr_start;
    volatile lp_apm0_regionn_addr_end_reg_t region6_addr_end;
    volatile lp_apm0_regionn_attr_reg_t region6_attr;
    volatile lp_apm0_regionn_addr_start_reg_t region7_addr_start;
    volatile lp_apm0_regionn_addr_end_reg_t region7_addr_end;
    volatile lp_apm0_regionn_attr_reg_t region7_attr;
    uint32_t reserved_064[24];
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


#ifndef __cplusplus
_Static_assert(sizeof(lp_apm0_dev_t) == 0x800, "Invalid size of lp_apm0_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
