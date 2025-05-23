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
        /** region_filter_en : R/W; bitpos: [15:0]; default: 1;
         *  Configure bit $n (0-15) to enable region $n (0-15).
         *  0: Disable
         *  1: Enable
         */
        uint32_t region_filter_en:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} hp_apm_region_filter_en_reg_t;

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
} hp_apm_regionn_addr_start_reg_t;

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
} hp_apm_regionn_addr_end_reg_t;

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
         *  (HP_APM_REGIONn_ADDR_START_REG, HP_APM_REGIONn_ADDR_END_REG and
         *  HP_APM_REGIONn_ATTR_REG).
         *  0: Do not lock
         *  1: Lock
         */
        uint32_t regionn_lock:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} hp_apm_regionn_attr_reg_t;

/** Type of func_ctrl register
 *  APM access path permission management register
 */
typedef union {
    struct {
        /** m0_func_en : R/W; bitpos: [0]; default: 1;
         *  Configures to enable permission management for HP_APM_CTRL M0.
         */
        uint32_t m0_func_en:1;
        /** m1_func_en : R/W; bitpos: [1]; default: 1;
         *  Configures to enable permission management for HP_APM_CTRL M1.
         */
        uint32_t m1_func_en:1;
        /** m2_func_en : R/W; bitpos: [2]; default: 1;
         *  Configures to enable permission management for HP_APM_CTRL M2.
         */
        uint32_t m2_func_en:1;
        /** m3_func_en : R/W; bitpos: [3]; default: 1;
         *  Configures to enable permission management for HP_APM_CTRL M3.
         */
        uint32_t m3_func_en:1;
        /** m4_func_en : R/W; bitpos: [4]; default: 1;
         *  Configures to enable permission management for HP_APM_CTRL M4.
         */
        uint32_t m4_func_en:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} hp_apm_func_ctrl_reg_t;


/** Group: Status Registers */
/** Type of m0_status register
 *  HP_APM_CTRL M0 status register
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
} hp_apm_m0_status_reg_t;

/** Type of m0_status_clr register
 *  HP_APM_CTRL M0 status clear register
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
} hp_apm_m0_status_clr_reg_t;

/** Type of m0_exception_info0 register
 *  HP_APM_CTRL M0 exception information register
 */
typedef union {
    struct {
        /** m0_exception_region : RO; bitpos: [15:0]; default: 0;
         *  Represents the region where an exception occurs.
         */
        uint32_t m0_exception_region:16;
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
} hp_apm_m0_exception_info0_reg_t;

/** Type of m0_exception_info1 register
 *  HP_APM_CTRL M0 exception information register
 */
typedef union {
    struct {
        /** m0_exception_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the access address when an exception occurs.
         */
        uint32_t m0_exception_addr:32;
    };
    uint32_t val;
} hp_apm_m0_exception_info1_reg_t;

/** Type of m1_status register
 *  HP_APM_CTRL M1 status register
 */
typedef union {
    struct {
        /** m1_exception_status : RO; bitpos: [1:0]; default: 0;
         *  Represents exception status.
         *  bit0: 1 represents permission restrictions
         *  bit1: 1 represents address out of bounds
         */
        uint32_t m1_exception_status:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_apm_m1_status_reg_t;

/** Type of m1_status_clr register
 *  HP_APM_CTRL M1 status clear register
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
} hp_apm_m1_status_clr_reg_t;

/** Type of m1_exception_info0 register
 *  HP_APM_CTRL M1 exception information register
 */
typedef union {
    struct {
        /** m1_exception_region : RO; bitpos: [15:0]; default: 0;
         *  Represents the region where an exception occurs.
         */
        uint32_t m1_exception_region:16;
        /** m1_exception_mode : RO; bitpos: [17:16]; default: 0;
         *  Represents the master's security mode when an exception occurs.
         */
        uint32_t m1_exception_mode:2;
        /** m1_exception_id : RO; bitpos: [22:18]; default: 0;
         *  Represents master ID when an exception occurs.
         */
        uint32_t m1_exception_id:5;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} hp_apm_m1_exception_info0_reg_t;

/** Type of m1_exception_info1 register
 *  HP_APM_CTRL M1 exception information register
 */
typedef union {
    struct {
        /** m1_exception_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the access address when an exception occurs.
         */
        uint32_t m1_exception_addr:32;
    };
    uint32_t val;
} hp_apm_m1_exception_info1_reg_t;

/** Type of m2_status register
 *  HP_APM_CTRL M2 status register
 */
typedef union {
    struct {
        /** m2_exception_status : RO; bitpos: [1:0]; default: 0;
         *  Represents exception status.
         *  bit0: 1 represents permission restrictions
         *  bit1: 1 represents address out of bounds
         */
        uint32_t m2_exception_status:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_apm_m2_status_reg_t;

/** Type of m2_status_clr register
 *  HP_APM_CTRL M2 status clear register
 */
typedef union {
    struct {
        /** m2_exception_status_clr : WT; bitpos: [0]; default: 0;
         *  Configures to clear exception status.
         */
        uint32_t m2_exception_status_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_apm_m2_status_clr_reg_t;

/** Type of m2_exception_info0 register
 *  HP_APM_CTRL M2 exception information register
 */
typedef union {
    struct {
        /** m2_exception_region : RO; bitpos: [15:0]; default: 0;
         *  Represents the region where an exception occurs.
         */
        uint32_t m2_exception_region:16;
        /** m2_exception_mode : RO; bitpos: [17:16]; default: 0;
         *  Represents the master's security mode when an exception occurs.
         */
        uint32_t m2_exception_mode:2;
        /** m2_exception_id : RO; bitpos: [22:18]; default: 0;
         *  Represents master ID when an exception occurs.
         */
        uint32_t m2_exception_id:5;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} hp_apm_m2_exception_info0_reg_t;

/** Type of m2_exception_info1 register
 *  HP_APM_CTRL M2 exception information register
 */
typedef union {
    struct {
        /** m2_exception_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the access address when an exception occurs.
         */
        uint32_t m2_exception_addr:32;
    };
    uint32_t val;
} hp_apm_m2_exception_info1_reg_t;

/** Type of m3_status register
 *  HP_APM_CTRL M3 status register
 */
typedef union {
    struct {
        /** m3_exception_status : RO; bitpos: [1:0]; default: 0;
         *  Represents exception status.
         *  bit0: 1 represents permission restrictions
         *  bit1: 1 represents address out of bounds
         */
        uint32_t m3_exception_status:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_apm_m3_status_reg_t;

/** Type of m3_status_clr register
 *  HP_APM_CTRL M3 status clear register
 */
typedef union {
    struct {
        /** m3_exception_status_clr : WT; bitpos: [0]; default: 0;
         *  Configures to clear exception status.
         */
        uint32_t m3_exception_status_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_apm_m3_status_clr_reg_t;

/** Type of m3_exception_info0 register
 *  HP_APM_CTRL M3 exception information register
 */
typedef union {
    struct {
        /** m3_exception_region : RO; bitpos: [15:0]; default: 0;
         *  Represents the region where an exception occurs.
         */
        uint32_t m3_exception_region:16;
        /** m3_exception_mode : RO; bitpos: [17:16]; default: 0;
         *  Represents the master's security mode when an exception occurs.
         */
        uint32_t m3_exception_mode:2;
        /** m3_exception_id : RO; bitpos: [22:18]; default: 0;
         *  Represents master ID when an exception occurs.
         */
        uint32_t m3_exception_id:5;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} hp_apm_m3_exception_info0_reg_t;

/** Type of m3_exception_info1 register
 *  HP_APM_CTRL M3 exception information register
 */
typedef union {
    struct {
        /** m3_exception_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the access address when an exception occurs.
         */
        uint32_t m3_exception_addr:32;
    };
    uint32_t val;
} hp_apm_m3_exception_info1_reg_t;

/** Type of m4_status register
 *  HP_APM_CTRL M4 status register
 */
typedef union {
    struct {
        /** m4_exception_status : RO; bitpos: [1:0]; default: 0;
         *  Represents exception status.
         *  bit0: 1 represents permission restrictions
         *  bit1: 1 represents address out of bounds
         */
        uint32_t m4_exception_status:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_apm_m4_status_reg_t;

/** Type of m4_status_clr register
 *  HP_APM_CTRL M4 status clear register
 */
typedef union {
    struct {
        /** m4_exception_status_clr : WT; bitpos: [0]; default: 0;
         *  Configures to clear exception status.
         */
        uint32_t m4_exception_status_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_apm_m4_status_clr_reg_t;

/** Type of m4_exception_info0 register
 *  HP_APM_CTRL M4 exception information register
 */
typedef union {
    struct {
        /** m4_exception_region : RO; bitpos: [15:0]; default: 0;
         *  Represents the region where an exception occurs.
         */
        uint32_t m4_exception_region:16;
        /** m4_exception_mode : RO; bitpos: [17:16]; default: 0;
         *  Represents the master's security mode when an exception occurs.
         */
        uint32_t m4_exception_mode:2;
        /** m4_exception_id : RO; bitpos: [22:18]; default: 0;
         *  Represents master ID when an exception occurs.
         */
        uint32_t m4_exception_id:5;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} hp_apm_m4_exception_info0_reg_t;

/** Type of m4_exception_info1 register
 *  HP_APM_CTRL M4 exception information register
 */
typedef union {
    struct {
        /** m4_exception_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the access address when an exception occurs.
         */
        uint32_t m4_exception_addr:32;
    };
    uint32_t val;
} hp_apm_m4_exception_info1_reg_t;


/** Group: Interrupt Registers */
/** Type of int_en register
 *  HP_APM_CTRL M0/1/2/3/4 interrupt enable register
 */
typedef union {
    struct {
        /** m0_apm_int_en : R/W; bitpos: [0]; default: 0;
         *  Configures to enable HP_APM_CTRL M0 interrupt.
         *  0: Disable
         *  1: Enable
         */
        uint32_t m0_apm_int_en:1;
        /** m1_apm_int_en : R/W; bitpos: [1]; default: 0;
         *  Configures to enable HP_APM_CTRL M1 interrupt.
         *  0: Disable
         *  1: Enable
         */
        uint32_t m1_apm_int_en:1;
        /** m2_apm_int_en : R/W; bitpos: [2]; default: 0;
         *  Configures to enable HP_APM_CTRL M2 interrupt.
         *  0: Disable
         *  1: Enable
         */
        uint32_t m2_apm_int_en:1;
        /** m3_apm_int_en : R/W; bitpos: [3]; default: 0;
         *  Configures to enable HP_APM_CTRL M3 interrupt.
         *  0: Disable
         *  1: Enable
         */
        uint32_t m3_apm_int_en:1;
        /** m4_apm_int_en : R/W; bitpos: [4]; default: 0;
         *  Configures to enable HP_APM_CTRL M4 interrupt.
         *  0: Disable
         *  1: Enable
         */
        uint32_t m4_apm_int_en:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} hp_apm_int_en_reg_t;


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
} hp_apm_clock_gate_reg_t;


/** Group: Version Control Registers */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36773904;
         *  Version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_apm_date_reg_t;


typedef struct {
    volatile hp_apm_region_filter_en_reg_t region_filter_en;
    volatile hp_apm_regionn_addr_start_reg_t region0_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region0_addr_end;
    volatile hp_apm_regionn_attr_reg_t region0_attr;
    volatile hp_apm_regionn_addr_start_reg_t region1_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region1_addr_end;
    volatile hp_apm_regionn_attr_reg_t region1_attr;
    volatile hp_apm_regionn_addr_start_reg_t region2_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region2_addr_end;
    volatile hp_apm_regionn_attr_reg_t region2_attr;
    volatile hp_apm_regionn_addr_start_reg_t region3_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region3_addr_end;
    volatile hp_apm_regionn_attr_reg_t region3_attr;
    volatile hp_apm_regionn_addr_start_reg_t region4_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region4_addr_end;
    volatile hp_apm_regionn_attr_reg_t region4_attr;
    volatile hp_apm_regionn_addr_start_reg_t region5_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region5_addr_end;
    volatile hp_apm_regionn_attr_reg_t region5_attr;
    volatile hp_apm_regionn_addr_start_reg_t region6_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region6_addr_end;
    volatile hp_apm_regionn_attr_reg_t region6_attr;
    volatile hp_apm_regionn_addr_start_reg_t region7_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region7_addr_end;
    volatile hp_apm_regionn_attr_reg_t region7_attr;
    volatile hp_apm_regionn_addr_start_reg_t region8_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region8_addr_end;
    volatile hp_apm_regionn_attr_reg_t region8_attr;
    volatile hp_apm_regionn_addr_start_reg_t region9_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region9_addr_end;
    volatile hp_apm_regionn_attr_reg_t region9_attr;
    volatile hp_apm_regionn_addr_start_reg_t region10_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region10_addr_end;
    volatile hp_apm_regionn_attr_reg_t region10_attr;
    volatile hp_apm_regionn_addr_start_reg_t region11_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region11_addr_end;
    volatile hp_apm_regionn_attr_reg_t region11_attr;
    volatile hp_apm_regionn_addr_start_reg_t region12_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region12_addr_end;
    volatile hp_apm_regionn_attr_reg_t region12_attr;
    volatile hp_apm_regionn_addr_start_reg_t region13_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region13_addr_end;
    volatile hp_apm_regionn_attr_reg_t region13_attr;
    volatile hp_apm_regionn_addr_start_reg_t region14_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region14_addr_end;
    volatile hp_apm_regionn_attr_reg_t region14_attr;
    volatile hp_apm_regionn_addr_start_reg_t region15_addr_start;
    volatile hp_apm_regionn_addr_end_reg_t region15_addr_end;
    volatile hp_apm_regionn_attr_reg_t region15_attr;
    volatile hp_apm_func_ctrl_reg_t func_ctrl;
    volatile hp_apm_m0_status_reg_t m0_status;
    volatile hp_apm_m0_status_clr_reg_t m0_status_clr;
    volatile hp_apm_m0_exception_info0_reg_t m0_exception_info0;
    volatile hp_apm_m0_exception_info1_reg_t m0_exception_info1;
    volatile hp_apm_m1_status_reg_t m1_status;
    volatile hp_apm_m1_status_clr_reg_t m1_status_clr;
    volatile hp_apm_m1_exception_info0_reg_t m1_exception_info0;
    volatile hp_apm_m1_exception_info1_reg_t m1_exception_info1;
    volatile hp_apm_m2_status_reg_t m2_status;
    volatile hp_apm_m2_status_clr_reg_t m2_status_clr;
    volatile hp_apm_m2_exception_info0_reg_t m2_exception_info0;
    volatile hp_apm_m2_exception_info1_reg_t m2_exception_info1;
    volatile hp_apm_m3_status_reg_t m3_status;
    volatile hp_apm_m3_status_clr_reg_t m3_status_clr;
    volatile hp_apm_m3_exception_info0_reg_t m3_exception_info0;
    volatile hp_apm_m3_exception_info1_reg_t m3_exception_info1;
    volatile hp_apm_m4_status_reg_t m4_status;
    volatile hp_apm_m4_status_clr_reg_t m4_status_clr;
    volatile hp_apm_m4_exception_info0_reg_t m4_exception_info0;
    volatile hp_apm_m4_exception_info1_reg_t m4_exception_info1;
    volatile hp_apm_int_en_reg_t int_en;
    uint32_t reserved_11c[439];
    volatile hp_apm_clock_gate_reg_t clock_gate;
    volatile hp_apm_date_reg_t date;
} hp_apm_dev_t;

extern hp_apm_dev_t HP_APM;

#ifndef __cplusplus
_Static_assert(sizeof(hp_apm_dev_t) == 0x800, "Invalid size of hp_apm_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
