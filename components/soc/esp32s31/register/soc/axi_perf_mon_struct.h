/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Registers */
/** Type of clk_en register
 *  reserved
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  reserved
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} axi_perf_mon_clk_en_reg_t;

/** Type of sel_ag0_metric_range0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_metric_range0_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range0_cnt_high:16;
        /** sel_ag0_metric_range0_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range0_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_metric_range0_reg_t;

/** Type of sel_ag1_metric_range0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_metric_range0_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range0_cnt_high:16;
        /** sel_ag1_metric_range0_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range0_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_metric_range0_reg_t;

/** Type of sel_ag2_metric_range0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_metric_range0_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range0_cnt_high:16;
        /** sel_ag2_metric_range0_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range0_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_metric_range0_reg_t;

/** Type of sel_ag3_metric_range0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_metric_range0_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range0_cnt_high:16;
        /** sel_ag3_metric_range0_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range0_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_metric_range0_reg_t;

/** Type of sel_ag4_metric_range0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_metric_range0_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range0_cnt_high:16;
        /** sel_ag4_metric_range0_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range0_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_metric_range0_reg_t;

/** Type of sel_ag5_metric_range0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_metric_range0_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range0_cnt_high:16;
        /** sel_ag5_metric_range0_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range0_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_metric_range0_reg_t;

/** Type of sel_ag6_metric_range0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_metric_range0_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range0_cnt_high:16;
        /** sel_ag6_metric_range0_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range0_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_metric_range0_reg_t;

/** Type of sel_ag0_metric_range1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_metric_range1_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range1_cnt_high:16;
        /** sel_ag0_metric_range1_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range1_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_metric_range1_reg_t;

/** Type of sel_ag1_metric_range1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_metric_range1_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range1_cnt_high:16;
        /** sel_ag1_metric_range1_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range1_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_metric_range1_reg_t;

/** Type of sel_ag2_metric_range1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_metric_range1_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range1_cnt_high:16;
        /** sel_ag2_metric_range1_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range1_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_metric_range1_reg_t;

/** Type of sel_ag3_metric_range1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_metric_range1_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range1_cnt_high:16;
        /** sel_ag3_metric_range1_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range1_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_metric_range1_reg_t;

/** Type of sel_ag4_metric_range1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_metric_range1_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range1_cnt_high:16;
        /** sel_ag4_metric_range1_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range1_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_metric_range1_reg_t;

/** Type of sel_ag5_metric_range1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_metric_range1_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range1_cnt_high:16;
        /** sel_ag5_metric_range1_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range1_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_metric_range1_reg_t;

/** Type of sel_ag6_metric_range1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_metric_range1_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range1_cnt_high:16;
        /** sel_ag6_metric_range1_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range1_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_metric_range1_reg_t;

/** Type of sel_ag0_metric_range2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_metric_range2_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range2_cnt_high:16;
        /** sel_ag0_metric_range2_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range2_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_metric_range2_reg_t;

/** Type of sel_ag1_metric_range2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_metric_range2_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range2_cnt_high:16;
        /** sel_ag1_metric_range2_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range2_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_metric_range2_reg_t;

/** Type of sel_ag2_metric_range2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_metric_range2_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range2_cnt_high:16;
        /** sel_ag2_metric_range2_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range2_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_metric_range2_reg_t;

/** Type of sel_ag3_metric_range2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_metric_range2_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range2_cnt_high:16;
        /** sel_ag3_metric_range2_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range2_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_metric_range2_reg_t;

/** Type of sel_ag4_metric_range2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_metric_range2_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range2_cnt_high:16;
        /** sel_ag4_metric_range2_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range2_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_metric_range2_reg_t;

/** Type of sel_ag5_metric_range2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_metric_range2_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range2_cnt_high:16;
        /** sel_ag5_metric_range2_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range2_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_metric_range2_reg_t;

/** Type of sel_ag6_metric_range2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_metric_range2_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range2_cnt_high:16;
        /** sel_ag6_metric_range2_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range2_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_metric_range2_reg_t;

/** Type of sel_ag0_metric_range3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_metric_range3_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range3_cnt_high:16;
        /** sel_ag0_metric_range3_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range3_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_metric_range3_reg_t;

/** Type of sel_ag1_metric_range3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_metric_range3_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range3_cnt_high:16;
        /** sel_ag1_metric_range3_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range3_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_metric_range3_reg_t;

/** Type of sel_ag2_metric_range3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_metric_range3_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range3_cnt_high:16;
        /** sel_ag2_metric_range3_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range3_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_metric_range3_reg_t;

/** Type of sel_ag3_metric_range3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_metric_range3_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range3_cnt_high:16;
        /** sel_ag3_metric_range3_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range3_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_metric_range3_reg_t;

/** Type of sel_ag4_metric_range3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_metric_range3_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range3_cnt_high:16;
        /** sel_ag4_metric_range3_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range3_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_metric_range3_reg_t;

/** Type of sel_ag5_metric_range3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_metric_range3_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range3_cnt_high:16;
        /** sel_ag5_metric_range3_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range3_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_metric_range3_reg_t;

/** Type of sel_ag6_metric_range3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_metric_range3_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range3_cnt_high:16;
        /** sel_ag6_metric_range3_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range3_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_metric_range3_reg_t;

/** Type of sel_ag0_metric_range4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_metric_range4_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range4_cnt_high:16;
        /** sel_ag0_metric_range4_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range4_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_metric_range4_reg_t;

/** Type of sel_ag1_metric_range4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_metric_range4_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range4_cnt_high:16;
        /** sel_ag1_metric_range4_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range4_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_metric_range4_reg_t;

/** Type of sel_ag2_metric_range4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_metric_range4_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range4_cnt_high:16;
        /** sel_ag2_metric_range4_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range4_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_metric_range4_reg_t;

/** Type of sel_ag3_metric_range4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_metric_range4_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range4_cnt_high:16;
        /** sel_ag3_metric_range4_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range4_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_metric_range4_reg_t;

/** Type of sel_ag4_metric_range4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_metric_range4_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range4_cnt_high:16;
        /** sel_ag4_metric_range4_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range4_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_metric_range4_reg_t;

/** Type of sel_ag5_metric_range4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_metric_range4_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range4_cnt_high:16;
        /** sel_ag5_metric_range4_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range4_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_metric_range4_reg_t;

/** Type of sel_ag6_metric_range4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_metric_range4_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range4_cnt_high:16;
        /** sel_ag6_metric_range4_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range4_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_metric_range4_reg_t;

/** Type of sel_ag0_metric_range5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_metric_range5_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range5_cnt_high:16;
        /** sel_ag0_metric_range5_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range5_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_metric_range5_reg_t;

/** Type of sel_ag1_metric_range5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_metric_range5_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range5_cnt_high:16;
        /** sel_ag1_metric_range5_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range5_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_metric_range5_reg_t;

/** Type of sel_ag2_metric_range5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_metric_range5_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range5_cnt_high:16;
        /** sel_ag2_metric_range5_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range5_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_metric_range5_reg_t;

/** Type of sel_ag3_metric_range5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_metric_range5_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range5_cnt_high:16;
        /** sel_ag3_metric_range5_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range5_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_metric_range5_reg_t;

/** Type of sel_ag4_metric_range5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_metric_range5_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range5_cnt_high:16;
        /** sel_ag4_metric_range5_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range5_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_metric_range5_reg_t;

/** Type of sel_ag5_metric_range5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_metric_range5_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range5_cnt_high:16;
        /** sel_ag5_metric_range5_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range5_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_metric_range5_reg_t;

/** Type of sel_ag6_metric_range5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_metric_range5_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range5_cnt_high:16;
        /** sel_ag6_metric_range5_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range5_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_metric_range5_reg_t;

/** Type of sel_ag0_metric_range6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_metric_range6_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range6_cnt_high:16;
        /** sel_ag0_metric_range6_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range6_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_metric_range6_reg_t;

/** Type of sel_ag1_metric_range6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_metric_range6_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range6_cnt_high:16;
        /** sel_ag1_metric_range6_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range6_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_metric_range6_reg_t;

/** Type of sel_ag2_metric_range6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_metric_range6_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range6_cnt_high:16;
        /** sel_ag2_metric_range6_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range6_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_metric_range6_reg_t;

/** Type of sel_ag3_metric_range6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_metric_range6_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range6_cnt_high:16;
        /** sel_ag3_metric_range6_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range6_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_metric_range6_reg_t;

/** Type of sel_ag4_metric_range6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_metric_range6_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range6_cnt_high:16;
        /** sel_ag4_metric_range6_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range6_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_metric_range6_reg_t;

/** Type of sel_ag5_metric_range6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_metric_range6_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range6_cnt_high:16;
        /** sel_ag5_metric_range6_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range6_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_metric_range6_reg_t;

/** Type of sel_ag6_metric_range6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_metric_range6_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range6_cnt_high:16;
        /** sel_ag6_metric_range6_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range6_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_metric_range6_reg_t;

/** Type of sel_ag0_metric_range7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_metric_range7_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range7_cnt_high:16;
        /** sel_ag0_metric_range7_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag0_metric_range7_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_metric_range7_reg_t;

/** Type of sel_ag1_metric_range7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_metric_range7_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range7_cnt_high:16;
        /** sel_ag1_metric_range7_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag1_metric_range7_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_metric_range7_reg_t;

/** Type of sel_ag2_metric_range7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_metric_range7_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range7_cnt_high:16;
        /** sel_ag2_metric_range7_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag2_metric_range7_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_metric_range7_reg_t;

/** Type of sel_ag3_metric_range7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_metric_range7_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range7_cnt_high:16;
        /** sel_ag3_metric_range7_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag3_metric_range7_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_metric_range7_reg_t;

/** Type of sel_ag4_metric_range7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_metric_range7_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range7_cnt_high:16;
        /** sel_ag4_metric_range7_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag4_metric_range7_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_metric_range7_reg_t;

/** Type of sel_ag5_metric_range7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_metric_range7_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range7_cnt_high:16;
        /** sel_ag5_metric_range7_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag5_metric_range7_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_metric_range7_reg_t;

/** Type of sel_ag6_metric_range7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_metric_range7_cnt_high : R/W; bitpos: [15:0]; default: 0;
         *  The x Upper limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range7_cnt_high:16;
        /** sel_ag6_metric_range7_cnt_low : R/W; bitpos: [31:16]; default: 0;
         *  The x Lower limit of interval statistics for sel metric in sel agent
         */
        uint32_t sel_ag6_metric_range7_cnt_low:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_metric_range7_reg_t;

/** Type of sel_ag0_rd_addr_mask0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_addr_mask0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag0_rd_addr_mask0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_rd_addr_mask0_reg_t;

/** Type of sel_ag1_rd_addr_mask0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_rd_addr_mask0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag1_rd_addr_mask0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_rd_addr_mask0_reg_t;

/** Type of sel_ag2_rd_addr_mask0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_rd_addr_mask0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag2_rd_addr_mask0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_rd_addr_mask0_reg_t;

/** Type of sel_ag3_rd_addr_mask0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_rd_addr_mask0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag3_rd_addr_mask0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_rd_addr_mask0_reg_t;

/** Type of sel_ag4_rd_addr_mask0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_rd_addr_mask0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag4_rd_addr_mask0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_rd_addr_mask0_reg_t;

/** Type of sel_ag5_rd_addr_mask0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_rd_addr_mask0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag5_rd_addr_mask0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_rd_addr_mask0_reg_t;

/** Type of sel_ag6_rd_addr_mask0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_rd_addr_mask0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag6_rd_addr_mask0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_rd_addr_mask0_reg_t;

/** Type of sel_ag0_rd_addr_mask1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_addr_mask1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag0_rd_addr_mask1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_rd_addr_mask1_reg_t;

/** Type of sel_ag1_rd_addr_mask1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_rd_addr_mask1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag1_rd_addr_mask1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_rd_addr_mask1_reg_t;

/** Type of sel_ag2_rd_addr_mask1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_rd_addr_mask1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag2_rd_addr_mask1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_rd_addr_mask1_reg_t;

/** Type of sel_ag3_rd_addr_mask1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_rd_addr_mask1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag3_rd_addr_mask1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_rd_addr_mask1_reg_t;

/** Type of sel_ag4_rd_addr_mask1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_rd_addr_mask1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag4_rd_addr_mask1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_rd_addr_mask1_reg_t;

/** Type of sel_ag5_rd_addr_mask1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_rd_addr_mask1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag5_rd_addr_mask1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_rd_addr_mask1_reg_t;

/** Type of sel_ag6_rd_addr_mask1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_rd_addr_mask1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag6_rd_addr_mask1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_rd_addr_mask1_reg_t;

/** Type of sel_ag0_rd_addr_mask2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_addr_mask2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag0_rd_addr_mask2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_rd_addr_mask2_reg_t;

/** Type of sel_ag1_rd_addr_mask2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_rd_addr_mask2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag1_rd_addr_mask2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_rd_addr_mask2_reg_t;

/** Type of sel_ag2_rd_addr_mask2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_rd_addr_mask2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag2_rd_addr_mask2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_rd_addr_mask2_reg_t;

/** Type of sel_ag3_rd_addr_mask2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_rd_addr_mask2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag3_rd_addr_mask2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_rd_addr_mask2_reg_t;

/** Type of sel_ag4_rd_addr_mask2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_rd_addr_mask2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag4_rd_addr_mask2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_rd_addr_mask2_reg_t;

/** Type of sel_ag5_rd_addr_mask2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_rd_addr_mask2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag5_rd_addr_mask2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_rd_addr_mask2_reg_t;

/** Type of sel_ag6_rd_addr_mask2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_rd_addr_mask2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag6_rd_addr_mask2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_rd_addr_mask2_reg_t;

/** Type of sel_ag0_wr_addr_mask0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_wr_addr_mask0 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag0_wr_addr_mask0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_wr_addr_mask0_reg_t;

/** Type of sel_ag1_wr_addr_mask0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_wr_addr_mask0 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag1_wr_addr_mask0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_wr_addr_mask0_reg_t;

/** Type of sel_ag2_wr_addr_mask0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_wr_addr_mask0 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag2_wr_addr_mask0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_wr_addr_mask0_reg_t;

/** Type of sel_ag3_wr_addr_mask0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_wr_addr_mask0 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag3_wr_addr_mask0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_wr_addr_mask0_reg_t;

/** Type of sel_ag4_wr_addr_mask0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_wr_addr_mask0 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag4_wr_addr_mask0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_wr_addr_mask0_reg_t;

/** Type of sel_ag5_wr_addr_mask0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_wr_addr_mask0 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag5_wr_addr_mask0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_wr_addr_mask0_reg_t;

/** Type of sel_ag6_wr_addr_mask0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_wr_addr_mask0 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag6_wr_addr_mask0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_wr_addr_mask0_reg_t;

/** Type of sel_ag0_wr_addr_mask1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_wr_addr_mask1 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag0_wr_addr_mask1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_wr_addr_mask1_reg_t;

/** Type of sel_ag1_wr_addr_mask1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_wr_addr_mask1 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag1_wr_addr_mask1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_wr_addr_mask1_reg_t;

/** Type of sel_ag2_wr_addr_mask1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_wr_addr_mask1 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag2_wr_addr_mask1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_wr_addr_mask1_reg_t;

/** Type of sel_ag3_wr_addr_mask1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_wr_addr_mask1 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag3_wr_addr_mask1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_wr_addr_mask1_reg_t;

/** Type of sel_ag4_wr_addr_mask1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_wr_addr_mask1 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag4_wr_addr_mask1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_wr_addr_mask1_reg_t;

/** Type of sel_ag5_wr_addr_mask1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_wr_addr_mask1 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag5_wr_addr_mask1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_wr_addr_mask1_reg_t;

/** Type of sel_ag6_wr_addr_mask1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_wr_addr_mask1 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag6_wr_addr_mask1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_wr_addr_mask1_reg_t;

/** Type of sel_ag0_wr_addr_mask2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_wr_addr_mask2 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag0_wr_addr_mask2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_wr_addr_mask2_reg_t;

/** Type of sel_ag1_wr_addr_mask2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_wr_addr_mask2 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag1_wr_addr_mask2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_wr_addr_mask2_reg_t;

/** Type of sel_ag2_wr_addr_mask2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_wr_addr_mask2 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag2_wr_addr_mask2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_wr_addr_mask2_reg_t;

/** Type of sel_ag3_wr_addr_mask2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_wr_addr_mask2 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag3_wr_addr_mask2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_wr_addr_mask2_reg_t;

/** Type of sel_ag4_wr_addr_mask2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_wr_addr_mask2 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag4_wr_addr_mask2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_wr_addr_mask2_reg_t;

/** Type of sel_ag5_wr_addr_mask2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_wr_addr_mask2 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag5_wr_addr_mask2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_wr_addr_mask2_reg_t;

/** Type of sel_ag6_wr_addr_mask2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_wr_addr_mask2 : R/W; bitpos: [31:0]; default: 0;
         *  Write addr mask of addr filter function for sel agent, mask bit will not compare
         *  with addr
         */
        uint32_t sel_ag6_wr_addr_mask2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_wr_addr_mask2_reg_t;

/** Type of sel_ag0_rd_addr_filter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_addr_filter0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag0_rd_addr_filter0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_rd_addr_filter0_reg_t;

/** Type of sel_ag1_rd_addr_filter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_rd_addr_filter0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag1_rd_addr_filter0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_rd_addr_filter0_reg_t;

/** Type of sel_ag2_rd_addr_filter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_rd_addr_filter0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag2_rd_addr_filter0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_rd_addr_filter0_reg_t;

/** Type of sel_ag3_rd_addr_filter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_rd_addr_filter0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag3_rd_addr_filter0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_rd_addr_filter0_reg_t;

/** Type of sel_ag4_rd_addr_filter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_rd_addr_filter0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag4_rd_addr_filter0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_rd_addr_filter0_reg_t;

/** Type of sel_ag5_rd_addr_filter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_rd_addr_filter0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag5_rd_addr_filter0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_rd_addr_filter0_reg_t;

/** Type of sel_ag6_rd_addr_filter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_rd_addr_filter0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag6_rd_addr_filter0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_rd_addr_filter0_reg_t;

/** Type of sel_ag0_rd_addr_filter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_addr_filter1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag0_rd_addr_filter1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_rd_addr_filter1_reg_t;

/** Type of sel_ag1_rd_addr_filter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_rd_addr_filter1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag1_rd_addr_filter1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_rd_addr_filter1_reg_t;

/** Type of sel_ag2_rd_addr_filter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_rd_addr_filter1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag2_rd_addr_filter1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_rd_addr_filter1_reg_t;

/** Type of sel_ag3_rd_addr_filter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_rd_addr_filter1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag3_rd_addr_filter1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_rd_addr_filter1_reg_t;

/** Type of sel_ag4_rd_addr_filter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_rd_addr_filter1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag4_rd_addr_filter1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_rd_addr_filter1_reg_t;

/** Type of sel_ag5_rd_addr_filter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_rd_addr_filter1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag5_rd_addr_filter1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_rd_addr_filter1_reg_t;

/** Type of sel_ag6_rd_addr_filter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_rd_addr_filter1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag6_rd_addr_filter1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_rd_addr_filter1_reg_t;

/** Type of sel_ag0_rd_addr_filter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_addr_filter2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag0_rd_addr_filter2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_rd_addr_filter2_reg_t;

/** Type of sel_ag1_rd_addr_filter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_rd_addr_filter2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag1_rd_addr_filter2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_rd_addr_filter2_reg_t;

/** Type of sel_ag2_rd_addr_filter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_rd_addr_filter2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag2_rd_addr_filter2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_rd_addr_filter2_reg_t;

/** Type of sel_ag3_rd_addr_filter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_rd_addr_filter2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag3_rd_addr_filter2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_rd_addr_filter2_reg_t;

/** Type of sel_ag4_rd_addr_filter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_rd_addr_filter2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag4_rd_addr_filter2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_rd_addr_filter2_reg_t;

/** Type of sel_ag5_rd_addr_filter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_rd_addr_filter2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag5_rd_addr_filter2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_rd_addr_filter2_reg_t;

/** Type of sel_ag6_rd_addr_filter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_rd_addr_filter2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag6_rd_addr_filter2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_rd_addr_filter2_reg_t;

/** Type of sel_ag0_wr_addr_filter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_wr_addr_filter0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag0_wr_addr_filter0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_wr_addr_filter0_reg_t;

/** Type of sel_ag1_wr_addr_filter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_wr_addr_filter0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag1_wr_addr_filter0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_wr_addr_filter0_reg_t;

/** Type of sel_ag2_wr_addr_filter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_wr_addr_filter0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag2_wr_addr_filter0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_wr_addr_filter0_reg_t;

/** Type of sel_ag3_wr_addr_filter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_wr_addr_filter0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag3_wr_addr_filter0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_wr_addr_filter0_reg_t;

/** Type of sel_ag4_wr_addr_filter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_wr_addr_filter0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag4_wr_addr_filter0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_wr_addr_filter0_reg_t;

/** Type of sel_ag5_wr_addr_filter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_wr_addr_filter0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag5_wr_addr_filter0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_wr_addr_filter0_reg_t;

/** Type of sel_ag6_wr_addr_filter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_wr_addr_filter0 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag6_wr_addr_filter0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_wr_addr_filter0_reg_t;

/** Type of sel_ag0_wr_addr_filter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_wr_addr_filter1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag0_wr_addr_filter1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_wr_addr_filter1_reg_t;

/** Type of sel_ag1_wr_addr_filter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_wr_addr_filter1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag1_wr_addr_filter1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_wr_addr_filter1_reg_t;

/** Type of sel_ag2_wr_addr_filter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_wr_addr_filter1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag2_wr_addr_filter1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_wr_addr_filter1_reg_t;

/** Type of sel_ag3_wr_addr_filter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_wr_addr_filter1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag3_wr_addr_filter1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_wr_addr_filter1_reg_t;

/** Type of sel_ag4_wr_addr_filter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_wr_addr_filter1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag4_wr_addr_filter1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_wr_addr_filter1_reg_t;

/** Type of sel_ag5_wr_addr_filter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_wr_addr_filter1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag5_wr_addr_filter1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_wr_addr_filter1_reg_t;

/** Type of sel_ag6_wr_addr_filter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_wr_addr_filter1 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag6_wr_addr_filter1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_wr_addr_filter1_reg_t;

/** Type of sel_ag0_wr_addr_filter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_wr_addr_filter2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag0_wr_addr_filter2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_wr_addr_filter2_reg_t;

/** Type of sel_ag1_wr_addr_filter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_wr_addr_filter2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag1_wr_addr_filter2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_wr_addr_filter2_reg_t;

/** Type of sel_ag2_wr_addr_filter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_wr_addr_filter2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag2_wr_addr_filter2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_wr_addr_filter2_reg_t;

/** Type of sel_ag3_wr_addr_filter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_wr_addr_filter2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag3_wr_addr_filter2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_wr_addr_filter2_reg_t;

/** Type of sel_ag4_wr_addr_filter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_wr_addr_filter2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag4_wr_addr_filter2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_wr_addr_filter2_reg_t;

/** Type of sel_ag5_wr_addr_filter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_wr_addr_filter2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag5_wr_addr_filter2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_wr_addr_filter2_reg_t;

/** Type of sel_ag6_wr_addr_filter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_wr_addr_filter2 : R/W; bitpos: [31:0]; default: 0;
         *  Read addr filter of addr filter function for sel agent, no mask bit in addr will
         *  compare with addr filter, if compare result same will pass filter
         */
        uint32_t sel_ag6_wr_addr_filter2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_wr_addr_filter2_reg_t;

/** Type of sel_ag0_metric_select1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_metric_select1 : R/W; bitpos: [31:0]; default: 0;
         *  Metric select Reg for sel agent, each agent use 8 bits, low 5 bits means which
         *  metric is selected for current counter, high 3 bits means range metric this counter
         *  measured, a counter can use metric and range measure at the same time
         */
        uint32_t sel_ag0_metric_select1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_metric_select1_reg_t;

/** Type of sel_ag1_metric_select1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_metric_select1 : R/W; bitpos: [31:0]; default: 0;
         *  Metric select Reg for sel agent, each agent use 8 bits, low 5 bits means which
         *  metric is selected for current counter, high 3 bits means range metric this counter
         *  measured, a counter can use metric and range measure at the same time
         */
        uint32_t sel_ag1_metric_select1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_metric_select1_reg_t;

/** Type of sel_ag2_metric_select1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_metric_select1 : R/W; bitpos: [31:0]; default: 0;
         *  Metric select Reg for sel agent, each agent use 8 bits, low 5 bits means which
         *  metric is selected for current counter, high 3 bits means range metric this counter
         *  measured, a counter can use metric and range measure at the same time
         */
        uint32_t sel_ag2_metric_select1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_metric_select1_reg_t;

/** Type of sel_ag3_metric_select1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_metric_select1 : R/W; bitpos: [31:0]; default: 0;
         *  Metric select Reg for sel agent, each agent use 8 bits, low 5 bits means which
         *  metric is selected for current counter, high 3 bits means range metric this counter
         *  measured, a counter can use metric and range measure at the same time
         */
        uint32_t sel_ag3_metric_select1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_metric_select1_reg_t;

/** Type of sel_ag4_metric_select1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_metric_select1 : R/W; bitpos: [31:0]; default: 0;
         *  Metric select Reg for sel agent, each agent use 8 bits, low 5 bits means which
         *  metric is selected for current counter, high 3 bits means range metric this counter
         *  measured, a counter can use metric and range measure at the same time
         */
        uint32_t sel_ag4_metric_select1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_metric_select1_reg_t;

/** Type of sel_ag5_metric_select1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_metric_select1 : R/W; bitpos: [31:0]; default: 0;
         *  Metric select Reg for sel agent, each agent use 8 bits, low 5 bits means which
         *  metric is selected for current counter, high 3 bits means range metric this counter
         *  measured, a counter can use metric and range measure at the same time
         */
        uint32_t sel_ag5_metric_select1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_metric_select1_reg_t;

/** Type of sel_ag6_metric_select1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_metric_select1 : R/W; bitpos: [31:0]; default: 0;
         *  Metric select Reg for sel agent, each agent use 8 bits, low 5 bits means which
         *  metric is selected for current counter, high 3 bits means range metric this counter
         *  measured, a counter can use metric and range measure at the same time
         */
        uint32_t sel_ag6_metric_select1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_metric_select1_reg_t;

/** Type of sel_ag0_metric_select2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_metric_select2 : R/W; bitpos: [31:0]; default: 0;
         *  Metric select Reg for sel agent, each agent use 8 bits, low 5 bits means which
         *  metric is selected for current counter, high 3 bits means range metric this counter
         *  measured, a counter can use metric and range measure at the same time
         */
        uint32_t sel_ag0_metric_select2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_metric_select2_reg_t;

/** Type of sel_ag1_metric_select2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_metric_select2 : R/W; bitpos: [31:0]; default: 0;
         *  Metric select Reg for sel agent, each agent use 8 bits, low 5 bits means which
         *  metric is selected for current counter, high 3 bits means range metric this counter
         *  measured, a counter can use metric and range measure at the same time
         */
        uint32_t sel_ag1_metric_select2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_metric_select2_reg_t;

/** Type of sel_ag2_metric_select2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_metric_select2 : R/W; bitpos: [31:0]; default: 0;
         *  Metric select Reg for sel agent, each agent use 8 bits, low 5 bits means which
         *  metric is selected for current counter, high 3 bits means range metric this counter
         *  measured, a counter can use metric and range measure at the same time
         */
        uint32_t sel_ag2_metric_select2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_metric_select2_reg_t;

/** Type of sel_ag3_metric_select2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_metric_select2 : R/W; bitpos: [31:0]; default: 0;
         *  Metric select Reg for sel agent, each agent use 8 bits, low 5 bits means which
         *  metric is selected for current counter, high 3 bits means range metric this counter
         *  measured, a counter can use metric and range measure at the same time
         */
        uint32_t sel_ag3_metric_select2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_metric_select2_reg_t;

/** Type of sel_ag4_metric_select2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_metric_select2 : R/W; bitpos: [31:0]; default: 0;
         *  Metric select Reg for sel agent, each agent use 8 bits, low 5 bits means which
         *  metric is selected for current counter, high 3 bits means range metric this counter
         *  measured, a counter can use metric and range measure at the same time
         */
        uint32_t sel_ag4_metric_select2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_metric_select2_reg_t;

/** Type of sel_ag5_metric_select2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_metric_select2 : R/W; bitpos: [31:0]; default: 0;
         *  Metric select Reg for sel agent, each agent use 8 bits, low 5 bits means which
         *  metric is selected for current counter, high 3 bits means range metric this counter
         *  measured, a counter can use metric and range measure at the same time
         */
        uint32_t sel_ag5_metric_select2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_metric_select2_reg_t;

/** Type of sel_ag6_metric_select2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_metric_select2 : R/W; bitpos: [31:0]; default: 0;
         *  Metric select Reg for sel agent, each agent use 8 bits, low 5 bits means which
         *  metric is selected for current counter, high 3 bits means range metric this counter
         *  measured, a counter can use metric and range measure at the same time
         */
        uint32_t sel_ag6_metric_select2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_metric_select2_reg_t;

/** Type of sel_ag_rd_addr_region_sel register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_addr_region_sel : R/W; bitpos: [2:0]; default: 0;
         *  SW config Read region select, use with mask and filter, only when addr in one
         *  region and this region has been sel, will measure the transaction data num
         */
        uint32_t sel_ag0_rd_addr_region_sel:3;
        /** sel_ag1_rd_addr_region_sel : R/W; bitpos: [5:3]; default: 0;
         *  SW config Read region select, use with mask and filter, only when addr in one
         *  region and this region has been sel, will measure the transaction data num
         */
        uint32_t sel_ag1_rd_addr_region_sel:3;
        /** sel_ag2_rd_addr_region_sel : R/W; bitpos: [8:6]; default: 0;
         *  SW config Read region select, use with mask and filter, only when addr in one
         *  region and this region has been sel, will measure the transaction data num
         */
        uint32_t sel_ag2_rd_addr_region_sel:3;
        /** sel_ag3_rd_addr_region_sel : R/W; bitpos: [11:9]; default: 0;
         *  SW config Read region select, use with mask and filter, only when addr in one
         *  region and this region has been sel, will measure the transaction data num
         */
        uint32_t sel_ag3_rd_addr_region_sel:3;
        /** sel_ag4_rd_addr_region_sel : R/W; bitpos: [14:12]; default: 0;
         *  SW config Read region select, use with mask and filter, only when addr in one
         *  region and this region has been sel, will measure the transaction data num
         */
        uint32_t sel_ag4_rd_addr_region_sel:3;
        /** sel_ag5_rd_addr_region_sel : R/W; bitpos: [17:15]; default: 0;
         *  SW config Read region select, use with mask and filter, only when addr in one
         *  region and this region has been sel, will measure the transaction data num
         */
        uint32_t sel_ag5_rd_addr_region_sel:3;
        /** sel_ag6_rd_addr_region_sel : R/W; bitpos: [20:18]; default: 0;
         *  SW config Read region select, use with mask and filter, only when addr in one
         *  region and this region has been sel, will measure the transaction data num
         */
        uint32_t sel_ag6_rd_addr_region_sel:3;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} axi_perf_mon_sel_ag_rd_addr_region_sel_reg_t;

/** Type of sel_ag_wr_addr_region_sel register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_wr_addr_region_sel : R/W; bitpos: [2:0]; default: 0;
         *  SW config Write region select, use with mask and filter, only when addr in one
         *  region and this region has been sel, will measure the transaction data num
         */
        uint32_t sel_ag0_wr_addr_region_sel:3;
        /** sel_ag1_wr_addr_region_sel : R/W; bitpos: [5:3]; default: 0;
         *  SW config Write region select, use with mask and filter, only when addr in one
         *  region and this region has been sel, will measure the transaction data num
         */
        uint32_t sel_ag1_wr_addr_region_sel:3;
        /** sel_ag2_wr_addr_region_sel : R/W; bitpos: [8:6]; default: 0;
         *  SW config Write region select, use with mask and filter, only when addr in one
         *  region and this region has been sel, will measure the transaction data num
         */
        uint32_t sel_ag2_wr_addr_region_sel:3;
        /** sel_ag3_wr_addr_region_sel : R/W; bitpos: [11:9]; default: 0;
         *  SW config Write region select, use with mask and filter, only when addr in one
         *  region and this region has been sel, will measure the transaction data num
         */
        uint32_t sel_ag3_wr_addr_region_sel:3;
        /** sel_ag4_wr_addr_region_sel : R/W; bitpos: [14:12]; default: 0;
         *  SW config Write region select, use with mask and filter, only when addr in one
         *  region and this region has been sel, will measure the transaction data num
         */
        uint32_t sel_ag4_wr_addr_region_sel:3;
        /** sel_ag5_wr_addr_region_sel : R/W; bitpos: [17:15]; default: 0;
         *  SW config Write region select, use with mask and filter, only when addr in one
         *  region and this region has been sel, will measure the transaction data num
         */
        uint32_t sel_ag5_wr_addr_region_sel:3;
        /** sel_ag6_wr_addr_region_sel : R/W; bitpos: [20:18]; default: 0;
         *  SW config Write region select, use with mask and filter, only when addr in one
         *  region and this region has been sel, will measure the transaction data num
         */
        uint32_t sel_ag6_wr_addr_region_sel:3;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} axi_perf_mon_sel_ag_wr_addr_region_sel_reg_t;

/** Type of sel_ag_addr_filter_en register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_addr_filter_en : R/W; bitpos: [0]; default: 0;
         *  Enable read addr filter function, if 0, all address access will be record
         */
        uint32_t sel_ag0_rd_addr_filter_en:1;
        /** sel_ag1_rd_addr_filter_en : R/W; bitpos: [1]; default: 0;
         *  Enable read addr filter function, if 0, all address access will be record
         */
        uint32_t sel_ag1_rd_addr_filter_en:1;
        /** sel_ag2_rd_addr_filter_en : R/W; bitpos: [2]; default: 0;
         *  Enable read addr filter function, if 0, all address access will be record
         */
        uint32_t sel_ag2_rd_addr_filter_en:1;
        /** sel_ag3_rd_addr_filter_en : R/W; bitpos: [3]; default: 0;
         *  Enable read addr filter function, if 0, all address access will be record
         */
        uint32_t sel_ag3_rd_addr_filter_en:1;
        /** sel_ag4_rd_addr_filter_en : R/W; bitpos: [4]; default: 0;
         *  Enable read addr filter function, if 0, all address access will be record
         */
        uint32_t sel_ag4_rd_addr_filter_en:1;
        /** sel_ag5_rd_addr_filter_en : R/W; bitpos: [5]; default: 0;
         *  Enable read addr filter function, if 0, all address access will be record
         */
        uint32_t sel_ag5_rd_addr_filter_en:1;
        /** sel_ag6_rd_addr_filter_en : R/W; bitpos: [6]; default: 0;
         *  Enable read addr filter function, if 0, all address access will be record
         */
        uint32_t sel_ag6_rd_addr_filter_en:1;
        /** sel_ag0_wr_addr_filter_en : R/W; bitpos: [7]; default: 0;
         *  Enable write addr filter function, if 0, all address access will be record
         */
        uint32_t sel_ag0_wr_addr_filter_en:1;
        /** sel_ag1_wr_addr_filter_en : R/W; bitpos: [8]; default: 0;
         *  Enable write addr filter function, if 0, all address access will be record
         */
        uint32_t sel_ag1_wr_addr_filter_en:1;
        /** sel_ag2_wr_addr_filter_en : R/W; bitpos: [9]; default: 0;
         *  Enable write addr filter function, if 0, all address access will be record
         */
        uint32_t sel_ag2_wr_addr_filter_en:1;
        /** sel_ag3_wr_addr_filter_en : R/W; bitpos: [10]; default: 0;
         *  Enable write addr filter function, if 0, all address access will be record
         */
        uint32_t sel_ag3_wr_addr_filter_en:1;
        /** sel_ag4_wr_addr_filter_en : R/W; bitpos: [11]; default: 0;
         *  Enable write addr filter function, if 0, all address access will be record
         */
        uint32_t sel_ag4_wr_addr_filter_en:1;
        /** sel_ag5_wr_addr_filter_en : R/W; bitpos: [12]; default: 0;
         *  Enable write addr filter function, if 0, all address access will be record
         */
        uint32_t sel_ag5_wr_addr_filter_en:1;
        /** sel_ag6_wr_addr_filter_en : R/W; bitpos: [13]; default: 0;
         *  Enable write addr filter function, if 0, all address access will be record
         */
        uint32_t sel_ag6_wr_addr_filter_en:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag_addr_filter_en_reg_t;

/** Type of sel_ag_sw_record_stop_en register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_sw_record_stop_en : WT; bitpos: [0]; default: 0;
         *  SW use to stop event log function, record information will keep
         */
        uint32_t sel_ag0_sw_record_stop_en:1;
        /** sel_ag1_sw_record_stop_en : WT; bitpos: [1]; default: 0;
         *  SW use to stop event log function, record information will keep
         */
        uint32_t sel_ag1_sw_record_stop_en:1;
        /** sel_ag2_sw_record_stop_en : WT; bitpos: [2]; default: 0;
         *  SW use to stop event log function, record information will keep
         */
        uint32_t sel_ag2_sw_record_stop_en:1;
        /** sel_ag3_sw_record_stop_en : WT; bitpos: [3]; default: 0;
         *  SW use to stop event log function, record information will keep
         */
        uint32_t sel_ag3_sw_record_stop_en:1;
        /** sel_ag4_sw_record_stop_en : WT; bitpos: [4]; default: 0;
         *  SW use to stop event log function, record information will keep
         */
        uint32_t sel_ag4_sw_record_stop_en:1;
        /** sel_ag5_sw_record_stop_en : WT; bitpos: [5]; default: 0;
         *  SW use to stop event log function, record information will keep
         */
        uint32_t sel_ag5_sw_record_stop_en:1;
        /** sel_ag6_sw_record_stop_en : WT; bitpos: [6]; default: 0;
         *  SW use to stop event log function, record information will keep
         */
        uint32_t sel_ag6_sw_record_stop_en:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} axi_perf_mon_sel_ag_sw_record_stop_en_reg_t;

/** Type of sel_ag_sw_record_stop_clr register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_sw_record_stop_clr : WT; bitpos: [0]; default: 0;
         *  SW use to clear event log function stop, record new transaction from now
         */
        uint32_t sel_ag0_sw_record_stop_clr:1;
        /** sel_ag1_sw_record_stop_clr : WT; bitpos: [1]; default: 0;
         *  SW use to clear event log function stop, record new transaction from now
         */
        uint32_t sel_ag1_sw_record_stop_clr:1;
        /** sel_ag2_sw_record_stop_clr : WT; bitpos: [2]; default: 0;
         *  SW use to clear event log function stop, record new transaction from now
         */
        uint32_t sel_ag2_sw_record_stop_clr:1;
        /** sel_ag3_sw_record_stop_clr : WT; bitpos: [3]; default: 0;
         *  SW use to clear event log function stop, record new transaction from now
         */
        uint32_t sel_ag3_sw_record_stop_clr:1;
        /** sel_ag4_sw_record_stop_clr : WT; bitpos: [4]; default: 0;
         *  SW use to clear event log function stop, record new transaction from now
         */
        uint32_t sel_ag4_sw_record_stop_clr:1;
        /** sel_ag5_sw_record_stop_clr : WT; bitpos: [5]; default: 0;
         *  SW use to clear event log function stop, record new transaction from now
         */
        uint32_t sel_ag5_sw_record_stop_clr:1;
        /** sel_ag6_sw_record_stop_clr : WT; bitpos: [6]; default: 0;
         *  SW use to clear event log function stop, record new transaction from now
         */
        uint32_t sel_ag6_sw_record_stop_clr:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} axi_perf_mon_sel_ag_sw_record_stop_clr_reg_t;

/** Type of sel_ag_ins_bandw_test_en register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_ins_bandw_test_en : R/W; bitpos: [0]; default: 0;
         *  Enable Read instantaneous bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag0_rd_ins_bandw_test_en:1;
        /** sel_ag0_wr_ins_bandw_test_en : R/W; bitpos: [1]; default: 0;
         *  Enable Read instantaneous bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag0_wr_ins_bandw_test_en:1;
        /** sel_ag1_rd_ins_bandw_test_en : R/W; bitpos: [2]; default: 0;
         *  Enable Read instantaneous bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag1_rd_ins_bandw_test_en:1;
        /** sel_ag1_wr_ins_bandw_test_en : R/W; bitpos: [3]; default: 0;
         *  Enable Read instantaneous bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag1_wr_ins_bandw_test_en:1;
        /** sel_ag2_rd_ins_bandw_test_en : R/W; bitpos: [4]; default: 0;
         *  Enable Read instantaneous bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag2_rd_ins_bandw_test_en:1;
        /** sel_ag2_wr_ins_bandw_test_en : R/W; bitpos: [5]; default: 0;
         *  Enable Read instantaneous bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag2_wr_ins_bandw_test_en:1;
        /** sel_ag3_rd_ins_bandw_test_en : R/W; bitpos: [6]; default: 0;
         *  Enable Read instantaneous bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag3_rd_ins_bandw_test_en:1;
        /** sel_ag3_wr_ins_bandw_test_en : R/W; bitpos: [7]; default: 0;
         *  Enable Read instantaneous bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag3_wr_ins_bandw_test_en:1;
        /** sel_ag4_rd_ins_bandw_test_en : R/W; bitpos: [8]; default: 0;
         *  Enable Read instantaneous bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag4_rd_ins_bandw_test_en:1;
        /** sel_ag4_wr_ins_bandw_test_en : R/W; bitpos: [9]; default: 0;
         *  Enable Read instantaneous bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag4_wr_ins_bandw_test_en:1;
        /** sel_ag5_rd_ins_bandw_test_en : R/W; bitpos: [10]; default: 0;
         *  Enable Read instantaneous bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag5_rd_ins_bandw_test_en:1;
        /** sel_ag5_wr_ins_bandw_test_en : R/W; bitpos: [11]; default: 0;
         *  Enable Read instantaneous bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag5_wr_ins_bandw_test_en:1;
        /** sel_ag6_rd_ins_bandw_test_en : R/W; bitpos: [12]; default: 0;
         *  Enable Read instantaneous bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag6_rd_ins_bandw_test_en:1;
        /** sel_ag6_wr_ins_bandw_test_en : R/W; bitpos: [13]; default: 0;
         *  Enable Read instantaneous bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag6_wr_ins_bandw_test_en:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag_ins_bandw_test_en_reg_t;

/** Type of sel_ag0_sw_record_stop_data_limit register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_sw_record_stop_rd_data_limit : R/W; bitpos: [15:0]; default: 0;
         *  Read instantaneous bandwidth test data lower limit, when touch this limit, intr
         *  enable
         */
        uint32_t sel_ag0_sw_record_stop_rd_data_limit:16;
        /** sel_ag0_sw_record_stop_wr_data_limit : R/W; bitpos: [31:16]; default: 0;
         *  Read instantaneous bandwidth test data lower limit, when touch this limit, intr
         *  enable
         */
        uint32_t sel_ag0_sw_record_stop_wr_data_limit:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_sw_record_stop_data_limit_reg_t;

/** Type of sel_ag1_sw_record_stop_data_limit register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_sw_record_stop_rd_data_limit : R/W; bitpos: [15:0]; default: 0;
         *  Read instantaneous bandwidth test data lower limit, when touch this limit, intr
         *  enable
         */
        uint32_t sel_ag1_sw_record_stop_rd_data_limit:16;
        /** sel_ag1_sw_record_stop_wr_data_limit : R/W; bitpos: [31:16]; default: 0;
         *  Read instantaneous bandwidth test data lower limit, when touch this limit, intr
         *  enable
         */
        uint32_t sel_ag1_sw_record_stop_wr_data_limit:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_sw_record_stop_data_limit_reg_t;

/** Type of sel_ag2_sw_record_stop_data_limit register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_sw_record_stop_rd_data_limit : R/W; bitpos: [15:0]; default: 0;
         *  Read instantaneous bandwidth test data lower limit, when touch this limit, intr
         *  enable
         */
        uint32_t sel_ag2_sw_record_stop_rd_data_limit:16;
        /** sel_ag2_sw_record_stop_wr_data_limit : R/W; bitpos: [31:16]; default: 0;
         *  Read instantaneous bandwidth test data lower limit, when touch this limit, intr
         *  enable
         */
        uint32_t sel_ag2_sw_record_stop_wr_data_limit:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_sw_record_stop_data_limit_reg_t;

/** Type of sel_ag3_sw_record_stop_data_limit register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_sw_record_stop_rd_data_limit : R/W; bitpos: [15:0]; default: 0;
         *  Read instantaneous bandwidth test data lower limit, when touch this limit, intr
         *  enable
         */
        uint32_t sel_ag3_sw_record_stop_rd_data_limit:16;
        /** sel_ag3_sw_record_stop_wr_data_limit : R/W; bitpos: [31:16]; default: 0;
         *  Read instantaneous bandwidth test data lower limit, when touch this limit, intr
         *  enable
         */
        uint32_t sel_ag3_sw_record_stop_wr_data_limit:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_sw_record_stop_data_limit_reg_t;

/** Type of sel_ag4_sw_record_stop_data_limit register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_sw_record_stop_rd_data_limit : R/W; bitpos: [15:0]; default: 0;
         *  Read instantaneous bandwidth test data lower limit, when touch this limit, intr
         *  enable
         */
        uint32_t sel_ag4_sw_record_stop_rd_data_limit:16;
        /** sel_ag4_sw_record_stop_wr_data_limit : R/W; bitpos: [31:16]; default: 0;
         *  Read instantaneous bandwidth test data lower limit, when touch this limit, intr
         *  enable
         */
        uint32_t sel_ag4_sw_record_stop_wr_data_limit:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_sw_record_stop_data_limit_reg_t;

/** Type of sel_ag5_sw_record_stop_data_limit register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_sw_record_stop_rd_data_limit : R/W; bitpos: [15:0]; default: 0;
         *  Read instantaneous bandwidth test data lower limit, when touch this limit, intr
         *  enable
         */
        uint32_t sel_ag5_sw_record_stop_rd_data_limit:16;
        /** sel_ag5_sw_record_stop_wr_data_limit : R/W; bitpos: [31:16]; default: 0;
         *  Read instantaneous bandwidth test data lower limit, when touch this limit, intr
         *  enable
         */
        uint32_t sel_ag5_sw_record_stop_wr_data_limit:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_sw_record_stop_data_limit_reg_t;

/** Type of sel_ag6_sw_record_stop_data_limit register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_sw_record_stop_rd_data_limit : R/W; bitpos: [15:0]; default: 0;
         *  Read instantaneous bandwidth test data lower limit, when touch this limit, intr
         *  enable
         */
        uint32_t sel_ag6_sw_record_stop_rd_data_limit:16;
        /** sel_ag6_sw_record_stop_wr_data_limit : R/W; bitpos: [31:16]; default: 0;
         *  Read instantaneous bandwidth test data lower limit, when touch this limit, intr
         *  enable
         */
        uint32_t sel_ag6_sw_record_stop_wr_data_limit:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_sw_record_stop_data_limit_reg_t;

/** Type of sel_ag0_id_mask register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_id_mask : R/W; bitpos: [6:0]; default: 0;
         *  Read id mask, ignore mask id bits
         */
        uint32_t sel_ag0_rd_id_mask:7;
        /** sel_ag0_wr_id_mask : R/W; bitpos: [13:7]; default: 0;
         *  Write id mask, ignore mask id bits
         */
        uint32_t sel_ag0_wr_id_mask:7;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_id_mask_reg_t;

/** Type of sel_ag1_id_mask register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_rd_id_mask : R/W; bitpos: [6:0]; default: 0;
         *  Read id mask, ignore mask id bits
         */
        uint32_t sel_ag1_rd_id_mask:7;
        /** sel_ag1_wr_id_mask : R/W; bitpos: [13:7]; default: 0;
         *  Write id mask, ignore mask id bits
         */
        uint32_t sel_ag1_wr_id_mask:7;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_id_mask_reg_t;

/** Type of sel_ag2_id_mask register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_rd_id_mask : R/W; bitpos: [6:0]; default: 0;
         *  Read id mask, ignore mask id bits
         */
        uint32_t sel_ag2_rd_id_mask:7;
        /** sel_ag2_wr_id_mask : R/W; bitpos: [13:7]; default: 0;
         *  Write id mask, ignore mask id bits
         */
        uint32_t sel_ag2_wr_id_mask:7;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_id_mask_reg_t;

/** Type of sel_ag3_id_mask register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_rd_id_mask : R/W; bitpos: [6:0]; default: 0;
         *  Read id mask, ignore mask id bits
         */
        uint32_t sel_ag3_rd_id_mask:7;
        /** sel_ag3_wr_id_mask : R/W; bitpos: [13:7]; default: 0;
         *  Write id mask, ignore mask id bits
         */
        uint32_t sel_ag3_wr_id_mask:7;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_id_mask_reg_t;

/** Type of sel_ag4_id_mask register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_rd_id_mask : R/W; bitpos: [6:0]; default: 0;
         *  Read id mask, ignore mask id bits
         */
        uint32_t sel_ag4_rd_id_mask:7;
        /** sel_ag4_wr_id_mask : R/W; bitpos: [13:7]; default: 0;
         *  Write id mask, ignore mask id bits
         */
        uint32_t sel_ag4_wr_id_mask:7;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_id_mask_reg_t;

/** Type of sel_ag5_id_mask register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_rd_id_mask : R/W; bitpos: [6:0]; default: 0;
         *  Read id mask, ignore mask id bits
         */
        uint32_t sel_ag5_rd_id_mask:7;
        /** sel_ag5_wr_id_mask : R/W; bitpos: [13:7]; default: 0;
         *  Write id mask, ignore mask id bits
         */
        uint32_t sel_ag5_wr_id_mask:7;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_id_mask_reg_t;

/** Type of sel_ag6_id_mask register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_rd_id_mask : R/W; bitpos: [6:0]; default: 0;
         *  Read id mask, ignore mask id bits
         */
        uint32_t sel_ag6_rd_id_mask:7;
        /** sel_ag6_wr_id_mask : R/W; bitpos: [13:7]; default: 0;
         *  Write id mask, ignore mask id bits
         */
        uint32_t sel_ag6_wr_id_mask:7;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_id_mask_reg_t;

/** Type of sel_ag0_id_filter register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_id_filter : R/W; bitpos: [6:0]; default: 0;
         *  Use with mask, no mask bits must same as filter bits, the pass filter
         */
        uint32_t sel_ag0_rd_id_filter:7;
        /** sel_ag0_wr_id_filter : R/W; bitpos: [13:7]; default: 0;
         *  Use with mask, no mask bits must same as filter bits, the pass filter
         */
        uint32_t sel_ag0_wr_id_filter:7;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_id_filter_reg_t;

/** Type of sel_ag1_id_filter register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_rd_id_filter : R/W; bitpos: [6:0]; default: 0;
         *  Use with mask, no mask bits must same as filter bits, the pass filter
         */
        uint32_t sel_ag1_rd_id_filter:7;
        /** sel_ag1_wr_id_filter : R/W; bitpos: [13:7]; default: 0;
         *  Use with mask, no mask bits must same as filter bits, the pass filter
         */
        uint32_t sel_ag1_wr_id_filter:7;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_id_filter_reg_t;

/** Type of sel_ag2_id_filter register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_rd_id_filter : R/W; bitpos: [6:0]; default: 0;
         *  Use with mask, no mask bits must same as filter bits, the pass filter
         */
        uint32_t sel_ag2_rd_id_filter:7;
        /** sel_ag2_wr_id_filter : R/W; bitpos: [13:7]; default: 0;
         *  Use with mask, no mask bits must same as filter bits, the pass filter
         */
        uint32_t sel_ag2_wr_id_filter:7;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_id_filter_reg_t;

/** Type of sel_ag3_id_filter register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_rd_id_filter : R/W; bitpos: [6:0]; default: 0;
         *  Use with mask, no mask bits must same as filter bits, the pass filter
         */
        uint32_t sel_ag3_rd_id_filter:7;
        /** sel_ag3_wr_id_filter : R/W; bitpos: [13:7]; default: 0;
         *  Use with mask, no mask bits must same as filter bits, the pass filter
         */
        uint32_t sel_ag3_wr_id_filter:7;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_id_filter_reg_t;

/** Type of sel_ag4_id_filter register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_rd_id_filter : R/W; bitpos: [6:0]; default: 0;
         *  Use with mask, no mask bits must same as filter bits, the pass filter
         */
        uint32_t sel_ag4_rd_id_filter:7;
        /** sel_ag4_wr_id_filter : R/W; bitpos: [13:7]; default: 0;
         *  Use with mask, no mask bits must same as filter bits, the pass filter
         */
        uint32_t sel_ag4_wr_id_filter:7;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_id_filter_reg_t;

/** Type of sel_ag5_id_filter register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_rd_id_filter : R/W; bitpos: [6:0]; default: 0;
         *  Use with mask, no mask bits must same as filter bits, the pass filter
         */
        uint32_t sel_ag5_rd_id_filter:7;
        /** sel_ag5_wr_id_filter : R/W; bitpos: [13:7]; default: 0;
         *  Use with mask, no mask bits must same as filter bits, the pass filter
         */
        uint32_t sel_ag5_wr_id_filter:7;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_id_filter_reg_t;

/** Type of sel_ag6_id_filter register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_rd_id_filter : R/W; bitpos: [6:0]; default: 0;
         *  Use with mask, no mask bits must same as filter bits, the pass filter
         */
        uint32_t sel_ag6_rd_id_filter:7;
        /** sel_ag6_wr_id_filter : R/W; bitpos: [13:7]; default: 0;
         *  Use with mask, no mask bits must same as filter bits, the pass filter
         */
        uint32_t sel_ag6_wr_id_filter:7;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_id_filter_reg_t;

/** Type of sel_ag_bandw_test_en register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_bandw_test_en : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t sel_ag0_rd_bandw_test_en:1;
        /** sel_ag0_wr_bandw_test_en : R/W; bitpos: [1]; default: 0;
         *  Enable Read average bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag0_wr_bandw_test_en:1;
        /** sel_ag1_rd_bandw_test_en : R/W; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t sel_ag1_rd_bandw_test_en:1;
        /** sel_ag1_wr_bandw_test_en : R/W; bitpos: [3]; default: 0;
         *  Enable Read average bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag1_wr_bandw_test_en:1;
        /** sel_ag2_rd_bandw_test_en : R/W; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t sel_ag2_rd_bandw_test_en:1;
        /** sel_ag2_wr_bandw_test_en : R/W; bitpos: [5]; default: 0;
         *  Enable Read average bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag2_wr_bandw_test_en:1;
        /** sel_ag3_rd_bandw_test_en : R/W; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t sel_ag3_rd_bandw_test_en:1;
        /** sel_ag3_wr_bandw_test_en : R/W; bitpos: [7]; default: 0;
         *  Enable Read average bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag3_wr_bandw_test_en:1;
        /** sel_ag4_rd_bandw_test_en : R/W; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t sel_ag4_rd_bandw_test_en:1;
        /** sel_ag4_wr_bandw_test_en : R/W; bitpos: [9]; default: 0;
         *  Enable Read average bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag4_wr_bandw_test_en:1;
        /** sel_ag5_rd_bandw_test_en : R/W; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t sel_ag5_rd_bandw_test_en:1;
        /** sel_ag5_wr_bandw_test_en : R/W; bitpos: [11]; default: 0;
         *  Enable Read average bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag5_wr_bandw_test_en:1;
        /** sel_ag6_rd_bandw_test_en : R/W; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t sel_ag6_rd_bandw_test_en:1;
        /** sel_ag6_wr_bandw_test_en : R/W; bitpos: [13]; default: 0;
         *  Enable Read average bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag6_wr_bandw_test_en:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag_bandw_test_en_reg_t;

/** Type of sel_ag_bandw_test_stop register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_bandw_test_stop : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t sel_ag0_rd_bandw_test_stop:1;
        /** sel_ag0_wr_bandw_test_stop : R/W; bitpos: [1]; default: 0;
         *  Stop Read average bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag0_wr_bandw_test_stop:1;
        /** sel_ag1_rd_bandw_test_stop : R/W; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t sel_ag1_rd_bandw_test_stop:1;
        /** sel_ag1_wr_bandw_test_stop : R/W; bitpos: [3]; default: 0;
         *  Stop Read average bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag1_wr_bandw_test_stop:1;
        /** sel_ag2_rd_bandw_test_stop : R/W; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t sel_ag2_rd_bandw_test_stop:1;
        /** sel_ag2_wr_bandw_test_stop : R/W; bitpos: [5]; default: 0;
         *  Stop Read average bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag2_wr_bandw_test_stop:1;
        /** sel_ag3_rd_bandw_test_stop : R/W; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t sel_ag3_rd_bandw_test_stop:1;
        /** sel_ag3_wr_bandw_test_stop : R/W; bitpos: [7]; default: 0;
         *  Stop Read average bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag3_wr_bandw_test_stop:1;
        /** sel_ag4_rd_bandw_test_stop : R/W; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t sel_ag4_rd_bandw_test_stop:1;
        /** sel_ag4_wr_bandw_test_stop : R/W; bitpos: [9]; default: 0;
         *  Stop Read average bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag4_wr_bandw_test_stop:1;
        /** sel_ag5_rd_bandw_test_stop : R/W; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t sel_ag5_rd_bandw_test_stop:1;
        /** sel_ag5_wr_bandw_test_stop : R/W; bitpos: [11]; default: 0;
         *  Stop Read average bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag5_wr_bandw_test_stop:1;
        /** sel_ag6_rd_bandw_test_stop : R/W; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t sel_ag6_rd_bandw_test_stop:1;
        /** sel_ag6_wr_bandw_test_stop : R/W; bitpos: [13]; default: 0;
         *  Stop Read average bandwidth test for all select agent in the same time
         */
        uint32_t sel_ag6_wr_bandw_test_stop:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} axi_perf_mon_sel_ag_bandw_test_stop_reg_t;

/** Type of sel_ag0_bandw_trigger_in_sel register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_bandw_trigger_en_sel : R/W; bitpos: [3:0]; default: 0;
         *  Read average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag0_rd_bandw_trigger_en_sel:4;
        /** sel_ag0_rd_bandw_trigger_stop_sel : R/W; bitpos: [7:4]; default: 0;
         *  Read average bandwidth test, trigger by soc, sel source, SW register config is one
         *  source
         */
        uint32_t sel_ag0_rd_bandw_trigger_stop_sel:4;
        /** sel_ag0_wr_bandw_trigger_en_sel : R/W; bitpos: [11:8]; default: 0;
         *  Write average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag0_wr_bandw_trigger_en_sel:4;
        /** sel_ag0_wr_bandw_trigger_stop_sel : R/W; bitpos: [15:12]; default: 0;
         *  Write average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag0_wr_bandw_trigger_stop_sel:4;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_bandw_trigger_in_sel_reg_t;

/** Type of sel_ag1_bandw_trigger_in_sel register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_rd_bandw_trigger_en_sel : R/W; bitpos: [3:0]; default: 0;
         *  Read average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag1_rd_bandw_trigger_en_sel:4;
        /** sel_ag1_rd_bandw_trigger_stop_sel : R/W; bitpos: [7:4]; default: 0;
         *  Read average bandwidth test, trigger by soc, sel source, SW register config is one
         *  source
         */
        uint32_t sel_ag1_rd_bandw_trigger_stop_sel:4;
        /** sel_ag1_wr_bandw_trigger_en_sel : R/W; bitpos: [11:8]; default: 0;
         *  Write average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag1_wr_bandw_trigger_en_sel:4;
        /** sel_ag1_wr_bandw_trigger_stop_sel : R/W; bitpos: [15:12]; default: 0;
         *  Write average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag1_wr_bandw_trigger_stop_sel:4;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_bandw_trigger_in_sel_reg_t;

/** Type of sel_ag2_bandw_trigger_in_sel register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_rd_bandw_trigger_en_sel : R/W; bitpos: [3:0]; default: 0;
         *  Read average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag2_rd_bandw_trigger_en_sel:4;
        /** sel_ag2_rd_bandw_trigger_stop_sel : R/W; bitpos: [7:4]; default: 0;
         *  Read average bandwidth test, trigger by soc, sel source, SW register config is one
         *  source
         */
        uint32_t sel_ag2_rd_bandw_trigger_stop_sel:4;
        /** sel_ag2_wr_bandw_trigger_en_sel : R/W; bitpos: [11:8]; default: 0;
         *  Write average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag2_wr_bandw_trigger_en_sel:4;
        /** sel_ag2_wr_bandw_trigger_stop_sel : R/W; bitpos: [15:12]; default: 0;
         *  Write average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag2_wr_bandw_trigger_stop_sel:4;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_bandw_trigger_in_sel_reg_t;

/** Type of sel_ag3_bandw_trigger_in_sel register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_rd_bandw_trigger_en_sel : R/W; bitpos: [3:0]; default: 0;
         *  Read average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag3_rd_bandw_trigger_en_sel:4;
        /** sel_ag3_rd_bandw_trigger_stop_sel : R/W; bitpos: [7:4]; default: 0;
         *  Read average bandwidth test, trigger by soc, sel source, SW register config is one
         *  source
         */
        uint32_t sel_ag3_rd_bandw_trigger_stop_sel:4;
        /** sel_ag3_wr_bandw_trigger_en_sel : R/W; bitpos: [11:8]; default: 0;
         *  Write average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag3_wr_bandw_trigger_en_sel:4;
        /** sel_ag3_wr_bandw_trigger_stop_sel : R/W; bitpos: [15:12]; default: 0;
         *  Write average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag3_wr_bandw_trigger_stop_sel:4;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_bandw_trigger_in_sel_reg_t;

/** Type of sel_ag4_bandw_trigger_in_sel register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_rd_bandw_trigger_en_sel : R/W; bitpos: [3:0]; default: 0;
         *  Read average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag4_rd_bandw_trigger_en_sel:4;
        /** sel_ag4_rd_bandw_trigger_stop_sel : R/W; bitpos: [7:4]; default: 0;
         *  Read average bandwidth test, trigger by soc, sel source, SW register config is one
         *  source
         */
        uint32_t sel_ag4_rd_bandw_trigger_stop_sel:4;
        /** sel_ag4_wr_bandw_trigger_en_sel : R/W; bitpos: [11:8]; default: 0;
         *  Write average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag4_wr_bandw_trigger_en_sel:4;
        /** sel_ag4_wr_bandw_trigger_stop_sel : R/W; bitpos: [15:12]; default: 0;
         *  Write average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag4_wr_bandw_trigger_stop_sel:4;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_bandw_trigger_in_sel_reg_t;

/** Type of sel_ag5_bandw_trigger_in_sel register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_rd_bandw_trigger_en_sel : R/W; bitpos: [3:0]; default: 0;
         *  Read average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag5_rd_bandw_trigger_en_sel:4;
        /** sel_ag5_rd_bandw_trigger_stop_sel : R/W; bitpos: [7:4]; default: 0;
         *  Read average bandwidth test, trigger by soc, sel source, SW register config is one
         *  source
         */
        uint32_t sel_ag5_rd_bandw_trigger_stop_sel:4;
        /** sel_ag5_wr_bandw_trigger_en_sel : R/W; bitpos: [11:8]; default: 0;
         *  Write average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag5_wr_bandw_trigger_en_sel:4;
        /** sel_ag5_wr_bandw_trigger_stop_sel : R/W; bitpos: [15:12]; default: 0;
         *  Write average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag5_wr_bandw_trigger_stop_sel:4;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_bandw_trigger_in_sel_reg_t;

/** Type of sel_ag6_bandw_trigger_in_sel register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_rd_bandw_trigger_en_sel : R/W; bitpos: [3:0]; default: 0;
         *  Read average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag6_rd_bandw_trigger_en_sel:4;
        /** sel_ag6_rd_bandw_trigger_stop_sel : R/W; bitpos: [7:4]; default: 0;
         *  Read average bandwidth test, trigger by soc, sel source, SW register config is one
         *  source
         */
        uint32_t sel_ag6_rd_bandw_trigger_stop_sel:4;
        /** sel_ag6_wr_bandw_trigger_en_sel : R/W; bitpos: [11:8]; default: 0;
         *  Write average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag6_wr_bandw_trigger_en_sel:4;
        /** sel_ag6_wr_bandw_trigger_stop_sel : R/W; bitpos: [15:12]; default: 0;
         *  Write average bandwidth test, trigger enable by soc, sel source, SW register config
         *  is one source
         */
        uint32_t sel_ag6_wr_bandw_trigger_stop_sel:4;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_bandw_trigger_in_sel_reg_t;

/** Type of sel_ag0_ins_bandw_time_thr register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_ins_bandw_time_thr : R/W; bitpos: [15:0]; default: 0;
         *  Read instantaneous bandwidth test time limit, counter data num in this time unit,
         *  and will fresh counter to count again
         */
        uint32_t sel_ag0_rd_ins_bandw_time_thr:16;
        /** sel_ag0_wr_ins_bandw_time_thr : R/W; bitpos: [31:16]; default: 0;
         *  Write instantaneous bandwidth test time limit, counter data num in this time unit,
         *  and will fresh counter to count again
         */
        uint32_t sel_ag0_wr_ins_bandw_time_thr:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_ins_bandw_time_thr_reg_t;

/** Type of sel_ag1_ins_bandw_time_thr register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_rd_ins_bandw_time_thr : R/W; bitpos: [15:0]; default: 0;
         *  Read instantaneous bandwidth test time limit, counter data num in this time unit,
         *  and will fresh counter to count again
         */
        uint32_t sel_ag1_rd_ins_bandw_time_thr:16;
        /** sel_ag1_wr_ins_bandw_time_thr : R/W; bitpos: [31:16]; default: 0;
         *  Write instantaneous bandwidth test time limit, counter data num in this time unit,
         *  and will fresh counter to count again
         */
        uint32_t sel_ag1_wr_ins_bandw_time_thr:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_ins_bandw_time_thr_reg_t;

/** Type of sel_ag2_ins_bandw_time_thr register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_rd_ins_bandw_time_thr : R/W; bitpos: [15:0]; default: 0;
         *  Read instantaneous bandwidth test time limit, counter data num in this time unit,
         *  and will fresh counter to count again
         */
        uint32_t sel_ag2_rd_ins_bandw_time_thr:16;
        /** sel_ag2_wr_ins_bandw_time_thr : R/W; bitpos: [31:16]; default: 0;
         *  Write instantaneous bandwidth test time limit, counter data num in this time unit,
         *  and will fresh counter to count again
         */
        uint32_t sel_ag2_wr_ins_bandw_time_thr:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_ins_bandw_time_thr_reg_t;

/** Type of sel_ag3_ins_bandw_time_thr register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_rd_ins_bandw_time_thr : R/W; bitpos: [15:0]; default: 0;
         *  Read instantaneous bandwidth test time limit, counter data num in this time unit,
         *  and will fresh counter to count again
         */
        uint32_t sel_ag3_rd_ins_bandw_time_thr:16;
        /** sel_ag3_wr_ins_bandw_time_thr : R/W; bitpos: [31:16]; default: 0;
         *  Write instantaneous bandwidth test time limit, counter data num in this time unit,
         *  and will fresh counter to count again
         */
        uint32_t sel_ag3_wr_ins_bandw_time_thr:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_ins_bandw_time_thr_reg_t;

/** Type of sel_ag4_ins_bandw_time_thr register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_rd_ins_bandw_time_thr : R/W; bitpos: [15:0]; default: 0;
         *  Read instantaneous bandwidth test time limit, counter data num in this time unit,
         *  and will fresh counter to count again
         */
        uint32_t sel_ag4_rd_ins_bandw_time_thr:16;
        /** sel_ag4_wr_ins_bandw_time_thr : R/W; bitpos: [31:16]; default: 0;
         *  Write instantaneous bandwidth test time limit, counter data num in this time unit,
         *  and will fresh counter to count again
         */
        uint32_t sel_ag4_wr_ins_bandw_time_thr:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_ins_bandw_time_thr_reg_t;

/** Type of sel_ag5_ins_bandw_time_thr register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_rd_ins_bandw_time_thr : R/W; bitpos: [15:0]; default: 0;
         *  Read instantaneous bandwidth test time limit, counter data num in this time unit,
         *  and will fresh counter to count again
         */
        uint32_t sel_ag5_rd_ins_bandw_time_thr:16;
        /** sel_ag5_wr_ins_bandw_time_thr : R/W; bitpos: [31:16]; default: 0;
         *  Write instantaneous bandwidth test time limit, counter data num in this time unit,
         *  and will fresh counter to count again
         */
        uint32_t sel_ag5_wr_ins_bandw_time_thr:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_ins_bandw_time_thr_reg_t;

/** Type of sel_ag6_ins_bandw_time_thr register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_rd_ins_bandw_time_thr : R/W; bitpos: [15:0]; default: 0;
         *  Read instantaneous bandwidth test time limit, counter data num in this time unit,
         *  and will fresh counter to count again
         */
        uint32_t sel_ag6_rd_ins_bandw_time_thr:16;
        /** sel_ag6_wr_ins_bandw_time_thr : R/W; bitpos: [31:16]; default: 0;
         *  Write instantaneous bandwidth test time limit, counter data num in this time unit,
         *  and will fresh counter to count again
         */
        uint32_t sel_ag6_wr_ins_bandw_time_thr:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_ins_bandw_time_thr_reg_t;


/** Group: Peripheral Select Registers */
/** Type of agent_select register
 *  reserved
 */
typedef union {
    struct {
        /** agent_select : R/W; bitpos: [31:0]; default: 0;
         *  Select Agent in slot to be monitored, 4 bits means one agent number
         */
        uint32_t agent_select:32;
    };
    uint32_t val;
} axi_perf_mon_agent_select_reg_t;


/** Group: Status Registers */
/** Type of sel_ag0_counter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_counter0_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag0_counter0_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_counter0_reg_t;

/** Type of sel_ag1_counter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_counter0_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag1_counter0_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_counter0_reg_t;

/** Type of sel_ag2_counter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_counter0_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag2_counter0_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_counter0_reg_t;

/** Type of sel_ag3_counter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_counter0_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag3_counter0_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_counter0_reg_t;

/** Type of sel_ag4_counter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_counter0_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag4_counter0_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_counter0_reg_t;

/** Type of sel_ag5_counter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_counter0_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag5_counter0_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_counter0_reg_t;

/** Type of sel_ag6_counter0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_counter0_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag6_counter0_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_counter0_reg_t;

/** Type of sel_ag0_counter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_counter1_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag0_counter1_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_counter1_reg_t;

/** Type of sel_ag1_counter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_counter1_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag1_counter1_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_counter1_reg_t;

/** Type of sel_ag2_counter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_counter1_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag2_counter1_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_counter1_reg_t;

/** Type of sel_ag3_counter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_counter1_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag3_counter1_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_counter1_reg_t;

/** Type of sel_ag4_counter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_counter1_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag4_counter1_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_counter1_reg_t;

/** Type of sel_ag5_counter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_counter1_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag5_counter1_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_counter1_reg_t;

/** Type of sel_ag6_counter1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_counter1_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag6_counter1_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_counter1_reg_t;

/** Type of sel_ag0_counter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_counter2_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag0_counter2_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_counter2_reg_t;

/** Type of sel_ag1_counter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_counter2_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag1_counter2_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_counter2_reg_t;

/** Type of sel_ag2_counter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_counter2_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag2_counter2_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_counter2_reg_t;

/** Type of sel_ag3_counter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_counter2_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag3_counter2_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_counter2_reg_t;

/** Type of sel_ag4_counter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_counter2_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag4_counter2_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_counter2_reg_t;

/** Type of sel_ag5_counter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_counter2_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag5_counter2_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_counter2_reg_t;

/** Type of sel_ag6_counter2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_counter2_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag6_counter2_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_counter2_reg_t;

/** Type of sel_ag0_counter3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_counter3_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag0_counter3_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_counter3_reg_t;

/** Type of sel_ag1_counter3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_counter3_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag1_counter3_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_counter3_reg_t;

/** Type of sel_ag2_counter3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_counter3_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag2_counter3_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_counter3_reg_t;

/** Type of sel_ag3_counter3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_counter3_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag3_counter3_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_counter3_reg_t;

/** Type of sel_ag4_counter3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_counter3_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag4_counter3_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_counter3_reg_t;

/** Type of sel_ag5_counter3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_counter3_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag5_counter3_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_counter3_reg_t;

/** Type of sel_ag6_counter3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_counter3_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag6_counter3_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_counter3_reg_t;

/** Type of sel_ag0_counter4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_counter4_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag0_counter4_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_counter4_reg_t;

/** Type of sel_ag1_counter4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_counter4_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag1_counter4_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_counter4_reg_t;

/** Type of sel_ag2_counter4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_counter4_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag2_counter4_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_counter4_reg_t;

/** Type of sel_ag3_counter4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_counter4_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag3_counter4_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_counter4_reg_t;

/** Type of sel_ag4_counter4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_counter4_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag4_counter4_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_counter4_reg_t;

/** Type of sel_ag5_counter4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_counter4_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag5_counter4_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_counter4_reg_t;

/** Type of sel_ag6_counter4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_counter4_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag6_counter4_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_counter4_reg_t;

/** Type of sel_ag0_counter5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_counter5_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag0_counter5_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_counter5_reg_t;

/** Type of sel_ag1_counter5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_counter5_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag1_counter5_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_counter5_reg_t;

/** Type of sel_ag2_counter5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_counter5_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag2_counter5_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_counter5_reg_t;

/** Type of sel_ag3_counter5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_counter5_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag3_counter5_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_counter5_reg_t;

/** Type of sel_ag4_counter5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_counter5_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag4_counter5_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_counter5_reg_t;

/** Type of sel_ag5_counter5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_counter5_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag5_counter5_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_counter5_reg_t;

/** Type of sel_ag6_counter5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_counter5_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag6_counter5_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_counter5_reg_t;

/** Type of sel_ag0_counter6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_counter6_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag0_counter6_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_counter6_reg_t;

/** Type of sel_ag1_counter6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_counter6_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag1_counter6_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_counter6_reg_t;

/** Type of sel_ag2_counter6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_counter6_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag2_counter6_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_counter6_reg_t;

/** Type of sel_ag3_counter6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_counter6_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag3_counter6_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_counter6_reg_t;

/** Type of sel_ag4_counter6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_counter6_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag4_counter6_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_counter6_reg_t;

/** Type of sel_ag5_counter6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_counter6_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag5_counter6_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_counter6_reg_t;

/** Type of sel_ag6_counter6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_counter6_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag6_counter6_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_counter6_reg_t;

/** Type of sel_ag0_counter7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_counter7_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag0_counter7_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_counter7_reg_t;

/** Type of sel_ag1_counter7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_counter7_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag1_counter7_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_counter7_reg_t;

/** Type of sel_ag2_counter7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_counter7_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag2_counter7_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_counter7_reg_t;

/** Type of sel_ag3_counter7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_counter7_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag3_counter7_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_counter7_reg_t;

/** Type of sel_ag4_counter7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_counter7_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag4_counter7_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_counter7_reg_t;

/** Type of sel_ag5_counter7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_counter7_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag5_counter7_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_counter7_reg_t;

/** Type of sel_ag6_counter7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_counter7_result : RO; bitpos: [31:0]; default: 0;
         *  The result for sel agent metric measured in this counter, [15:0] is min result, and
         *  [31:16] is max result
         */
        uint32_t sel_ag6_counter7_result:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_counter7_reg_t;

/** Type of sel_ag0_range0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_range0_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag0_range0_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_range0_reg_t;

/** Type of sel_ag1_range0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_range0_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag1_range0_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_range0_reg_t;

/** Type of sel_ag2_range0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_range0_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag2_range0_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_range0_reg_t;

/** Type of sel_ag3_range0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_range0_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag3_range0_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_range0_reg_t;

/** Type of sel_ag4_range0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_range0_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag4_range0_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_range0_reg_t;

/** Type of sel_ag5_range0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_range0_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag5_range0_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_range0_reg_t;

/** Type of sel_ag6_range0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_range0_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag6_range0_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_range0_reg_t;

/** Type of sel_ag0_range1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_range1_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag0_range1_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_range1_reg_t;

/** Type of sel_ag1_range1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_range1_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag1_range1_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_range1_reg_t;

/** Type of sel_ag2_range1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_range1_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag2_range1_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_range1_reg_t;

/** Type of sel_ag3_range1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_range1_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag3_range1_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_range1_reg_t;

/** Type of sel_ag4_range1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_range1_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag4_range1_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_range1_reg_t;

/** Type of sel_ag5_range1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_range1_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag5_range1_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_range1_reg_t;

/** Type of sel_ag6_range1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_range1_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag6_range1_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_range1_reg_t;

/** Type of sel_ag0_range2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_range2_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag0_range2_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_range2_reg_t;

/** Type of sel_ag1_range2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_range2_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag1_range2_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_range2_reg_t;

/** Type of sel_ag2_range2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_range2_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag2_range2_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_range2_reg_t;

/** Type of sel_ag3_range2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_range2_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag3_range2_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_range2_reg_t;

/** Type of sel_ag4_range2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_range2_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag4_range2_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_range2_reg_t;

/** Type of sel_ag5_range2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_range2_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag5_range2_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_range2_reg_t;

/** Type of sel_ag6_range2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_range2_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag6_range2_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_range2_reg_t;

/** Type of sel_ag0_range3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_range3_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag0_range3_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_range3_reg_t;

/** Type of sel_ag1_range3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_range3_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag1_range3_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_range3_reg_t;

/** Type of sel_ag2_range3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_range3_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag2_range3_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_range3_reg_t;

/** Type of sel_ag3_range3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_range3_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag3_range3_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_range3_reg_t;

/** Type of sel_ag4_range3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_range3_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag4_range3_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_range3_reg_t;

/** Type of sel_ag5_range3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_range3_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag5_range3_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_range3_reg_t;

/** Type of sel_ag6_range3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_range3_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag6_range3_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_range3_reg_t;

/** Type of sel_ag0_range4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_range4_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag0_range4_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_range4_reg_t;

/** Type of sel_ag1_range4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_range4_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag1_range4_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_range4_reg_t;

/** Type of sel_ag2_range4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_range4_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag2_range4_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_range4_reg_t;

/** Type of sel_ag3_range4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_range4_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag3_range4_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_range4_reg_t;

/** Type of sel_ag4_range4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_range4_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag4_range4_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_range4_reg_t;

/** Type of sel_ag5_range4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_range4_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag5_range4_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_range4_reg_t;

/** Type of sel_ag6_range4 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_range4_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag6_range4_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_range4_reg_t;

/** Type of sel_ag0_range5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_range5_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag0_range5_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_range5_reg_t;

/** Type of sel_ag1_range5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_range5_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag1_range5_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_range5_reg_t;

/** Type of sel_ag2_range5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_range5_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag2_range5_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_range5_reg_t;

/** Type of sel_ag3_range5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_range5_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag3_range5_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_range5_reg_t;

/** Type of sel_ag4_range5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_range5_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag4_range5_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_range5_reg_t;

/** Type of sel_ag5_range5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_range5_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag5_range5_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_range5_reg_t;

/** Type of sel_ag6_range5 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_range5_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag6_range5_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_range5_reg_t;

/** Type of sel_ag0_range6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_range6_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag0_range6_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_range6_reg_t;

/** Type of sel_ag1_range6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_range6_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag1_range6_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_range6_reg_t;

/** Type of sel_ag2_range6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_range6_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag2_range6_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_range6_reg_t;

/** Type of sel_ag3_range6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_range6_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag3_range6_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_range6_reg_t;

/** Type of sel_ag4_range6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_range6_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag4_range6_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_range6_reg_t;

/** Type of sel_ag5_range6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_range6_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag5_range6_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_range6_reg_t;

/** Type of sel_ag6_range6 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_range6_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag6_range6_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_range6_reg_t;

/** Type of sel_ag0_range7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_range7_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag0_range7_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_range7_reg_t;

/** Type of sel_ag1_range7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_range7_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag1_range7_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_range7_reg_t;

/** Type of sel_ag2_range7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_range7_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag2_range7_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_range7_reg_t;

/** Type of sel_ag3_range7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_range7_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag3_range7_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_range7_reg_t;

/** Type of sel_ag4_range7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_range7_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag4_range7_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_range7_reg_t;

/** Type of sel_ag5_range7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_range7_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag5_range7_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_range7_reg_t;

/** Type of sel_ag6_range7 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_range7_result : RO; bitpos: [15:0]; default: 0;
         *  The interval statistics results in this counter for sel agent
         */
        uint32_t sel_ag6_range7_result:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_range7_reg_t;

/** Type of sel_ag0_rd_axi_info_record0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_axi_info_record0 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag0_rd_axi_info_record0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_rd_axi_info_record0_reg_t;

/** Type of sel_ag1_rd_axi_info_record0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_rd_axi_info_record0 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag1_rd_axi_info_record0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_rd_axi_info_record0_reg_t;

/** Type of sel_ag2_rd_axi_info_record0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_rd_axi_info_record0 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag2_rd_axi_info_record0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_rd_axi_info_record0_reg_t;

/** Type of sel_ag3_rd_axi_info_record0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_rd_axi_info_record0 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag3_rd_axi_info_record0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_rd_axi_info_record0_reg_t;

/** Type of sel_ag4_rd_axi_info_record0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_rd_axi_info_record0 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag4_rd_axi_info_record0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_rd_axi_info_record0_reg_t;

/** Type of sel_ag5_rd_axi_info_record0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_rd_axi_info_record0 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag5_rd_axi_info_record0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_rd_axi_info_record0_reg_t;

/** Type of sel_ag6_rd_axi_info_record0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_rd_axi_info_record0 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag6_rd_axi_info_record0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_rd_axi_info_record0_reg_t;

/** Type of sel_ag0_rd_axi_info_record1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_axi_info_record1 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag0_rd_axi_info_record1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_rd_axi_info_record1_reg_t;

/** Type of sel_ag1_rd_axi_info_record1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_rd_axi_info_record1 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag1_rd_axi_info_record1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_rd_axi_info_record1_reg_t;

/** Type of sel_ag2_rd_axi_info_record1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_rd_axi_info_record1 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag2_rd_axi_info_record1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_rd_axi_info_record1_reg_t;

/** Type of sel_ag3_rd_axi_info_record1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_rd_axi_info_record1 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag3_rd_axi_info_record1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_rd_axi_info_record1_reg_t;

/** Type of sel_ag4_rd_axi_info_record1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_rd_axi_info_record1 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag4_rd_axi_info_record1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_rd_axi_info_record1_reg_t;

/** Type of sel_ag5_rd_axi_info_record1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_rd_axi_info_record1 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag5_rd_axi_info_record1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_rd_axi_info_record1_reg_t;

/** Type of sel_ag6_rd_axi_info_record1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_rd_axi_info_record1 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag6_rd_axi_info_record1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_rd_axi_info_record1_reg_t;

/** Type of sel_ag0_rd_axi_info_record2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_axi_info_record2 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag0_rd_axi_info_record2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_rd_axi_info_record2_reg_t;

/** Type of sel_ag1_rd_axi_info_record2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_rd_axi_info_record2 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag1_rd_axi_info_record2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_rd_axi_info_record2_reg_t;

/** Type of sel_ag2_rd_axi_info_record2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_rd_axi_info_record2 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag2_rd_axi_info_record2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_rd_axi_info_record2_reg_t;

/** Type of sel_ag3_rd_axi_info_record2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_rd_axi_info_record2 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag3_rd_axi_info_record2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_rd_axi_info_record2_reg_t;

/** Type of sel_ag4_rd_axi_info_record2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_rd_axi_info_record2 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag4_rd_axi_info_record2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_rd_axi_info_record2_reg_t;

/** Type of sel_ag5_rd_axi_info_record2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_rd_axi_info_record2 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag5_rd_axi_info_record2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_rd_axi_info_record2_reg_t;

/** Type of sel_ag6_rd_axi_info_record2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_rd_axi_info_record2 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag6_rd_axi_info_record2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_rd_axi_info_record2_reg_t;

/** Type of sel_ag0_rd_axi_info_record3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_axi_info_record3 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag0_rd_axi_info_record3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_rd_axi_info_record3_reg_t;

/** Type of sel_ag1_rd_axi_info_record3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_rd_axi_info_record3 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag1_rd_axi_info_record3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_rd_axi_info_record3_reg_t;

/** Type of sel_ag2_rd_axi_info_record3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_rd_axi_info_record3 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag2_rd_axi_info_record3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_rd_axi_info_record3_reg_t;

/** Type of sel_ag3_rd_axi_info_record3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_rd_axi_info_record3 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag3_rd_axi_info_record3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_rd_axi_info_record3_reg_t;

/** Type of sel_ag4_rd_axi_info_record3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_rd_axi_info_record3 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag4_rd_axi_info_record3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_rd_axi_info_record3_reg_t;

/** Type of sel_ag5_rd_axi_info_record3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_rd_axi_info_record3 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag5_rd_axi_info_record3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_rd_axi_info_record3_reg_t;

/** Type of sel_ag6_rd_axi_info_record3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_rd_axi_info_record3 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for ARLEN,
         *  [23:8] for ARID
         */
        uint32_t sel_ag6_rd_axi_info_record3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_rd_axi_info_record3_reg_t;

/** Type of sel_ag0_wr_axi_info_record0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_wr_axi_info_record0 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag0_wr_axi_info_record0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_wr_axi_info_record0_reg_t;

/** Type of sel_ag1_wr_axi_info_record0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_wr_axi_info_record0 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag1_wr_axi_info_record0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_wr_axi_info_record0_reg_t;

/** Type of sel_ag2_wr_axi_info_record0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_wr_axi_info_record0 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag2_wr_axi_info_record0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_wr_axi_info_record0_reg_t;

/** Type of sel_ag3_wr_axi_info_record0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_wr_axi_info_record0 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag3_wr_axi_info_record0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_wr_axi_info_record0_reg_t;

/** Type of sel_ag4_wr_axi_info_record0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_wr_axi_info_record0 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag4_wr_axi_info_record0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_wr_axi_info_record0_reg_t;

/** Type of sel_ag5_wr_axi_info_record0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_wr_axi_info_record0 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag5_wr_axi_info_record0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_wr_axi_info_record0_reg_t;

/** Type of sel_ag6_wr_axi_info_record0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_wr_axi_info_record0 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag6_wr_axi_info_record0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_wr_axi_info_record0_reg_t;

/** Type of sel_ag0_wr_axi_info_record1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_wr_axi_info_record1 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag0_wr_axi_info_record1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_wr_axi_info_record1_reg_t;

/** Type of sel_ag1_wr_axi_info_record1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_wr_axi_info_record1 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag1_wr_axi_info_record1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_wr_axi_info_record1_reg_t;

/** Type of sel_ag2_wr_axi_info_record1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_wr_axi_info_record1 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag2_wr_axi_info_record1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_wr_axi_info_record1_reg_t;

/** Type of sel_ag3_wr_axi_info_record1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_wr_axi_info_record1 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag3_wr_axi_info_record1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_wr_axi_info_record1_reg_t;

/** Type of sel_ag4_wr_axi_info_record1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_wr_axi_info_record1 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag4_wr_axi_info_record1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_wr_axi_info_record1_reg_t;

/** Type of sel_ag5_wr_axi_info_record1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_wr_axi_info_record1 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag5_wr_axi_info_record1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_wr_axi_info_record1_reg_t;

/** Type of sel_ag6_wr_axi_info_record1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_wr_axi_info_record1 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag6_wr_axi_info_record1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_wr_axi_info_record1_reg_t;

/** Type of sel_ag0_wr_axi_info_record2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_wr_axi_info_record2 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag0_wr_axi_info_record2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_wr_axi_info_record2_reg_t;

/** Type of sel_ag1_wr_axi_info_record2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_wr_axi_info_record2 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag1_wr_axi_info_record2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_wr_axi_info_record2_reg_t;

/** Type of sel_ag2_wr_axi_info_record2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_wr_axi_info_record2 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag2_wr_axi_info_record2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_wr_axi_info_record2_reg_t;

/** Type of sel_ag3_wr_axi_info_record2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_wr_axi_info_record2 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag3_wr_axi_info_record2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_wr_axi_info_record2_reg_t;

/** Type of sel_ag4_wr_axi_info_record2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_wr_axi_info_record2 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag4_wr_axi_info_record2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_wr_axi_info_record2_reg_t;

/** Type of sel_ag5_wr_axi_info_record2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_wr_axi_info_record2 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag5_wr_axi_info_record2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_wr_axi_info_record2_reg_t;

/** Type of sel_ag6_wr_axi_info_record2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_wr_axi_info_record2 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag6_wr_axi_info_record2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_wr_axi_info_record2_reg_t;

/** Type of sel_ag0_wr_axi_info_record3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_wr_axi_info_record3 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag0_wr_axi_info_record3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_wr_axi_info_record3_reg_t;

/** Type of sel_ag1_wr_axi_info_record3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_wr_axi_info_record3 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag1_wr_axi_info_record3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_wr_axi_info_record3_reg_t;

/** Type of sel_ag2_wr_axi_info_record3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_wr_axi_info_record3 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag2_wr_axi_info_record3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_wr_axi_info_record3_reg_t;

/** Type of sel_ag3_wr_axi_info_record3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_wr_axi_info_record3 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag3_wr_axi_info_record3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_wr_axi_info_record3_reg_t;

/** Type of sel_ag4_wr_axi_info_record3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_wr_axi_info_record3 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag4_wr_axi_info_record3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_wr_axi_info_record3_reg_t;

/** Type of sel_ag5_wr_axi_info_record3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_wr_axi_info_record3 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag5_wr_axi_info_record3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_wr_axi_info_record3_reg_t;

/** Type of sel_ag6_wr_axi_info_record3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_wr_axi_info_record3 : RO; bitpos: [15:0]; default: 0;
         *  The latest x axi transaction information record for sel agent, [7:0] for AWLEN,
         *  [23:8] for AWID
         */
        uint32_t sel_ag6_wr_axi_info_record3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_wr_axi_info_record3_reg_t;

/** Type of sel_ag0_ins_bandw_data0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_ins_bandw_data0 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag0_ins_bandw_data0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_ins_bandw_data0_reg_t;

/** Type of sel_ag1_ins_bandw_data0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_ins_bandw_data0 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag1_ins_bandw_data0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_ins_bandw_data0_reg_t;

/** Type of sel_ag2_ins_bandw_data0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_ins_bandw_data0 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag2_ins_bandw_data0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_ins_bandw_data0_reg_t;

/** Type of sel_ag3_ins_bandw_data0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_ins_bandw_data0 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag3_ins_bandw_data0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_ins_bandw_data0_reg_t;

/** Type of sel_ag4_ins_bandw_data0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_ins_bandw_data0 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag4_ins_bandw_data0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_ins_bandw_data0_reg_t;

/** Type of sel_ag5_ins_bandw_data0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_ins_bandw_data0 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag5_ins_bandw_data0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_ins_bandw_data0_reg_t;

/** Type of sel_ag6_ins_bandw_data0 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_ins_bandw_data0 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag6_ins_bandw_data0:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_ins_bandw_data0_reg_t;

/** Type of sel_ag0_ins_bandw_data1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_ins_bandw_data1 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag0_ins_bandw_data1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_ins_bandw_data1_reg_t;

/** Type of sel_ag1_ins_bandw_data1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_ins_bandw_data1 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag1_ins_bandw_data1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_ins_bandw_data1_reg_t;

/** Type of sel_ag2_ins_bandw_data1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_ins_bandw_data1 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag2_ins_bandw_data1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_ins_bandw_data1_reg_t;

/** Type of sel_ag3_ins_bandw_data1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_ins_bandw_data1 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag3_ins_bandw_data1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_ins_bandw_data1_reg_t;

/** Type of sel_ag4_ins_bandw_data1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_ins_bandw_data1 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag4_ins_bandw_data1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_ins_bandw_data1_reg_t;

/** Type of sel_ag5_ins_bandw_data1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_ins_bandw_data1 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag5_ins_bandw_data1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_ins_bandw_data1_reg_t;

/** Type of sel_ag6_ins_bandw_data1 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_ins_bandw_data1 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag6_ins_bandw_data1:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_ins_bandw_data1_reg_t;

/** Type of sel_ag0_ins_bandw_data2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_ins_bandw_data2 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag0_ins_bandw_data2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_ins_bandw_data2_reg_t;

/** Type of sel_ag1_ins_bandw_data2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_ins_bandw_data2 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag1_ins_bandw_data2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_ins_bandw_data2_reg_t;

/** Type of sel_ag2_ins_bandw_data2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_ins_bandw_data2 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag2_ins_bandw_data2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_ins_bandw_data2_reg_t;

/** Type of sel_ag3_ins_bandw_data2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_ins_bandw_data2 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag3_ins_bandw_data2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_ins_bandw_data2_reg_t;

/** Type of sel_ag4_ins_bandw_data2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_ins_bandw_data2 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag4_ins_bandw_data2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_ins_bandw_data2_reg_t;

/** Type of sel_ag5_ins_bandw_data2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_ins_bandw_data2 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag5_ins_bandw_data2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_ins_bandw_data2_reg_t;

/** Type of sel_ag6_ins_bandw_data2 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_ins_bandw_data2 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag6_ins_bandw_data2:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_ins_bandw_data2_reg_t;

/** Type of sel_ag0_ins_bandw_data3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_ins_bandw_data3 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag0_ins_bandw_data3:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_ins_bandw_data3_reg_t;

/** Type of sel_ag1_ins_bandw_data3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_ins_bandw_data3 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag1_ins_bandw_data3:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_ins_bandw_data3_reg_t;

/** Type of sel_ag2_ins_bandw_data3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_ins_bandw_data3 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag2_ins_bandw_data3:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_ins_bandw_data3_reg_t;

/** Type of sel_ag3_ins_bandw_data3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_ins_bandw_data3 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag3_ins_bandw_data3:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_ins_bandw_data3_reg_t;

/** Type of sel_ag4_ins_bandw_data3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_ins_bandw_data3 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag4_ins_bandw_data3:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_ins_bandw_data3_reg_t;

/** Type of sel_ag5_ins_bandw_data3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_ins_bandw_data3 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag5_ins_bandw_data3:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_ins_bandw_data3_reg_t;

/** Type of sel_ag6_ins_bandw_data3 register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_ins_bandw_data3 : RO; bitpos: [31:0]; default: 0;
         *  The latest x bandwidth date num in config time record for sel agent, [15:0] for
         *  Read and [31:16] for write
         */
        uint32_t sel_ag6_ins_bandw_data3:32;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_ins_bandw_data3_reg_t;

/** Type of sel_ag0_wr_bandw_cnt_valid_strobe_num register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_wr_bandw_time_cnt_valid_strobe_num : R/W; bitpos: [7:0]; default: 0;
         *  Set this register to configure the time valid scaling multiplier
         */
        uint32_t sel_ag0_wr_bandw_time_cnt_valid_strobe_num:8;
        /** sel_ag0_wr_bandw_data_cnt_valid_strobe_num : R/W; bitpos: [15:8]; default: 0;
         *  Set this register to configure the data valid scaling multiplier
         */
        uint32_t sel_ag0_wr_bandw_data_cnt_valid_strobe_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_wr_bandw_cnt_valid_strobe_num_reg_t;

/** Type of sel_ag1_wr_bandw_cnt_valid_strobe_num register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_wr_bandw_time_cnt_valid_strobe_num : R/W; bitpos: [7:0]; default: 0;
         *  Set this register to configure the time valid scaling multiplier
         */
        uint32_t sel_ag1_wr_bandw_time_cnt_valid_strobe_num:8;
        /** sel_ag1_wr_bandw_data_cnt_valid_strobe_num : R/W; bitpos: [15:8]; default: 0;
         *  Set this register to configure the data valid scaling multiplier
         */
        uint32_t sel_ag1_wr_bandw_data_cnt_valid_strobe_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_wr_bandw_cnt_valid_strobe_num_reg_t;

/** Type of sel_ag2_wr_bandw_cnt_valid_strobe_num register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_wr_bandw_time_cnt_valid_strobe_num : R/W; bitpos: [7:0]; default: 0;
         *  Set this register to configure the time valid scaling multiplier
         */
        uint32_t sel_ag2_wr_bandw_time_cnt_valid_strobe_num:8;
        /** sel_ag2_wr_bandw_data_cnt_valid_strobe_num : R/W; bitpos: [15:8]; default: 0;
         *  Set this register to configure the data valid scaling multiplier
         */
        uint32_t sel_ag2_wr_bandw_data_cnt_valid_strobe_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_wr_bandw_cnt_valid_strobe_num_reg_t;

/** Type of sel_ag3_wr_bandw_cnt_valid_strobe_num register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_wr_bandw_time_cnt_valid_strobe_num : R/W; bitpos: [7:0]; default: 0;
         *  Set this register to configure the time valid scaling multiplier
         */
        uint32_t sel_ag3_wr_bandw_time_cnt_valid_strobe_num:8;
        /** sel_ag3_wr_bandw_data_cnt_valid_strobe_num : R/W; bitpos: [15:8]; default: 0;
         *  Set this register to configure the data valid scaling multiplier
         */
        uint32_t sel_ag3_wr_bandw_data_cnt_valid_strobe_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_wr_bandw_cnt_valid_strobe_num_reg_t;

/** Type of sel_ag4_wr_bandw_cnt_valid_strobe_num register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_wr_bandw_time_cnt_valid_strobe_num : R/W; bitpos: [7:0]; default: 0;
         *  Set this register to configure the time valid scaling multiplier
         */
        uint32_t sel_ag4_wr_bandw_time_cnt_valid_strobe_num:8;
        /** sel_ag4_wr_bandw_data_cnt_valid_strobe_num : R/W; bitpos: [15:8]; default: 0;
         *  Set this register to configure the data valid scaling multiplier
         */
        uint32_t sel_ag4_wr_bandw_data_cnt_valid_strobe_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_wr_bandw_cnt_valid_strobe_num_reg_t;

/** Type of sel_ag5_wr_bandw_cnt_valid_strobe_num register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_wr_bandw_time_cnt_valid_strobe_num : R/W; bitpos: [7:0]; default: 0;
         *  Set this register to configure the time valid scaling multiplier
         */
        uint32_t sel_ag5_wr_bandw_time_cnt_valid_strobe_num:8;
        /** sel_ag5_wr_bandw_data_cnt_valid_strobe_num : R/W; bitpos: [15:8]; default: 0;
         *  Set this register to configure the data valid scaling multiplier
         */
        uint32_t sel_ag5_wr_bandw_data_cnt_valid_strobe_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_wr_bandw_cnt_valid_strobe_num_reg_t;

/** Type of sel_ag6_wr_bandw_cnt_valid_strobe_num register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_wr_bandw_time_cnt_valid_strobe_num : R/W; bitpos: [7:0]; default: 0;
         *  Set this register to configure the time valid scaling multiplier
         */
        uint32_t sel_ag6_wr_bandw_time_cnt_valid_strobe_num:8;
        /** sel_ag6_wr_bandw_data_cnt_valid_strobe_num : R/W; bitpos: [15:8]; default: 0;
         *  Set this register to configure the data valid scaling multiplier
         */
        uint32_t sel_ag6_wr_bandw_data_cnt_valid_strobe_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_wr_bandw_cnt_valid_strobe_num_reg_t;

/** Type of sel_ag0_rd_bandw_cnt_valid_strobe_num register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_rd_bandw_time_cnt_valid_strobe_num : R/W; bitpos: [7:0]; default: 0;
         *  Set this register to configure the time valid scaling multiplier
         */
        uint32_t sel_ag0_rd_bandw_time_cnt_valid_strobe_num:8;
        /** sel_ag0_rd_bandw_data_cnt_valid_strobe_num : R/W; bitpos: [15:8]; default: 0;
         *  Set this register to configure the data valid scaling multiplier
         */
        uint32_t sel_ag0_rd_bandw_data_cnt_valid_strobe_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag0_rd_bandw_cnt_valid_strobe_num_reg_t;

/** Type of sel_ag1_rd_bandw_cnt_valid_strobe_num register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag1_rd_bandw_time_cnt_valid_strobe_num : R/W; bitpos: [7:0]; default: 0;
         *  Set this register to configure the time valid scaling multiplier
         */
        uint32_t sel_ag1_rd_bandw_time_cnt_valid_strobe_num:8;
        /** sel_ag1_rd_bandw_data_cnt_valid_strobe_num : R/W; bitpos: [15:8]; default: 0;
         *  Set this register to configure the data valid scaling multiplier
         */
        uint32_t sel_ag1_rd_bandw_data_cnt_valid_strobe_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag1_rd_bandw_cnt_valid_strobe_num_reg_t;

/** Type of sel_ag2_rd_bandw_cnt_valid_strobe_num register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag2_rd_bandw_time_cnt_valid_strobe_num : R/W; bitpos: [7:0]; default: 0;
         *  Set this register to configure the time valid scaling multiplier
         */
        uint32_t sel_ag2_rd_bandw_time_cnt_valid_strobe_num:8;
        /** sel_ag2_rd_bandw_data_cnt_valid_strobe_num : R/W; bitpos: [15:8]; default: 0;
         *  Set this register to configure the data valid scaling multiplier
         */
        uint32_t sel_ag2_rd_bandw_data_cnt_valid_strobe_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag2_rd_bandw_cnt_valid_strobe_num_reg_t;

/** Type of sel_ag3_rd_bandw_cnt_valid_strobe_num register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag3_rd_bandw_time_cnt_valid_strobe_num : R/W; bitpos: [7:0]; default: 0;
         *  Set this register to configure the time valid scaling multiplier
         */
        uint32_t sel_ag3_rd_bandw_time_cnt_valid_strobe_num:8;
        /** sel_ag3_rd_bandw_data_cnt_valid_strobe_num : R/W; bitpos: [15:8]; default: 0;
         *  Set this register to configure the data valid scaling multiplier
         */
        uint32_t sel_ag3_rd_bandw_data_cnt_valid_strobe_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag3_rd_bandw_cnt_valid_strobe_num_reg_t;

/** Type of sel_ag4_rd_bandw_cnt_valid_strobe_num register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag4_rd_bandw_time_cnt_valid_strobe_num : R/W; bitpos: [7:0]; default: 0;
         *  Set this register to configure the time valid scaling multiplier
         */
        uint32_t sel_ag4_rd_bandw_time_cnt_valid_strobe_num:8;
        /** sel_ag4_rd_bandw_data_cnt_valid_strobe_num : R/W; bitpos: [15:8]; default: 0;
         *  Set this register to configure the data valid scaling multiplier
         */
        uint32_t sel_ag4_rd_bandw_data_cnt_valid_strobe_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag4_rd_bandw_cnt_valid_strobe_num_reg_t;

/** Type of sel_ag5_rd_bandw_cnt_valid_strobe_num register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag5_rd_bandw_time_cnt_valid_strobe_num : R/W; bitpos: [7:0]; default: 0;
         *  Set this register to configure the time valid scaling multiplier
         */
        uint32_t sel_ag5_rd_bandw_time_cnt_valid_strobe_num:8;
        /** sel_ag5_rd_bandw_data_cnt_valid_strobe_num : R/W; bitpos: [15:8]; default: 0;
         *  Set this register to configure the data valid scaling multiplier
         */
        uint32_t sel_ag5_rd_bandw_data_cnt_valid_strobe_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag5_rd_bandw_cnt_valid_strobe_num_reg_t;

/** Type of sel_ag6_rd_bandw_cnt_valid_strobe_num register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag6_rd_bandw_time_cnt_valid_strobe_num : R/W; bitpos: [7:0]; default: 0;
         *  Set this register to configure the time valid scaling multiplier
         */
        uint32_t sel_ag6_rd_bandw_time_cnt_valid_strobe_num:8;
        /** sel_ag6_rd_bandw_data_cnt_valid_strobe_num : R/W; bitpos: [15:8]; default: 0;
         *  Set this register to configure the data valid scaling multiplier
         */
        uint32_t sel_ag6_rd_bandw_data_cnt_valid_strobe_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_perf_mon_sel_ag6_rd_bandw_cnt_valid_strobe_num_reg_t;


/** Group: Interrupt Registers */
/** Type of sel_ag_int_raw register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_record_cnt_under_limit_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag0_record_cnt_under_limit_int_raw:1;
        /** sel_ag1_record_cnt_under_limit_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag1_record_cnt_under_limit_int_raw:1;
        /** sel_ag2_record_cnt_under_limit_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag2_record_cnt_under_limit_int_raw:1;
        /** sel_ag3_record_cnt_under_limit_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status of instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag3_record_cnt_under_limit_int_raw:1;
        /** sel_ag4_record_cnt_under_limit_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag4_record_cnt_under_limit_int_raw:1;
        /** sel_ag5_record_cnt_under_limit_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag5_record_cnt_under_limit_int_raw:1;
        /** sel_ag6_record_cnt_under_limit_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt status of instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag6_record_cnt_under_limit_int_raw:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} axi_perf_mon_sel_ag_int_raw_reg_t;

/** Type of sel_ag_int_st register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_record_cnt_under_limit_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag0_record_cnt_under_limit_int_st:1;
        /** sel_ag1_record_cnt_under_limit_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag1_record_cnt_under_limit_int_st:1;
        /** sel_ag2_record_cnt_under_limit_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag2_record_cnt_under_limit_int_st:1;
        /** sel_ag3_record_cnt_under_limit_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status of instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag3_record_cnt_under_limit_int_st:1;
        /** sel_ag4_record_cnt_under_limit_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag4_record_cnt_under_limit_int_st:1;
        /** sel_ag5_record_cnt_under_limit_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status of instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag5_record_cnt_under_limit_int_st:1;
        /** sel_ag6_record_cnt_under_limit_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status of instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag6_record_cnt_under_limit_int_st:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} axi_perf_mon_sel_ag_int_st_reg_t;

/** Type of sel_ag_int_ena register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_record_cnt_under_limit_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag0_record_cnt_under_limit_int_ena:1;
        /** sel_ag1_record_cnt_under_limit_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag1_record_cnt_under_limit_int_ena:1;
        /** sel_ag2_record_cnt_under_limit_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag2_record_cnt_under_limit_int_ena:1;
        /** sel_ag3_record_cnt_under_limit_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag3_record_cnt_under_limit_int_ena:1;
        /** sel_ag4_record_cnt_under_limit_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag4_record_cnt_under_limit_int_ena:1;
        /** sel_ag5_record_cnt_under_limit_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag5_record_cnt_under_limit_int_ena:1;
        /** sel_ag6_record_cnt_under_limit_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag6_record_cnt_under_limit_int_ena:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} axi_perf_mon_sel_ag_int_ena_reg_t;

/** Type of sel_ag_int_clr register
 *  reserved
 */
typedef union {
    struct {
        /** sel_ag0_record_cnt_under_limit_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag0_record_cnt_under_limit_int_clr:1;
        /** sel_ag1_record_cnt_under_limit_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag1_record_cnt_under_limit_int_clr:1;
        /** sel_ag2_record_cnt_under_limit_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag2_record_cnt_under_limit_int_clr:1;
        /** sel_ag3_record_cnt_under_limit_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag3_record_cnt_under_limit_int_clr:1;
        /** sel_ag4_record_cnt_under_limit_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag4_record_cnt_under_limit_int_clr:1;
        /** sel_ag5_record_cnt_under_limit_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag5_record_cnt_under_limit_int_clr:1;
        /** sel_ag6_record_cnt_under_limit_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear instantaneous bandwidth test data under limit int
         */
        uint32_t sel_ag6_record_cnt_under_limit_int_clr:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} axi_perf_mon_sel_ag_int_clr_reg_t;


/** Group: Version Control Registers */
/** Type of date register
 *  reserved
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 2506030;
         *  reserved
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} axi_perf_mon_date_reg_t;


typedef struct {
    volatile axi_perf_mon_clk_en_reg_t clk_en;
    volatile axi_perf_mon_agent_select_reg_t agent_select;
    volatile axi_perf_mon_sel_ag0_counter0_reg_t sel_ag0_counter0;
    volatile axi_perf_mon_sel_ag1_counter0_reg_t sel_ag1_counter0;
    volatile axi_perf_mon_sel_ag2_counter0_reg_t sel_ag2_counter0;
    volatile axi_perf_mon_sel_ag3_counter0_reg_t sel_ag3_counter0;
    volatile axi_perf_mon_sel_ag4_counter0_reg_t sel_ag4_counter0;
    volatile axi_perf_mon_sel_ag5_counter0_reg_t sel_ag5_counter0;
    volatile axi_perf_mon_sel_ag6_counter0_reg_t sel_ag6_counter0;
    volatile axi_perf_mon_sel_ag0_counter1_reg_t sel_ag0_counter1;
    volatile axi_perf_mon_sel_ag1_counter1_reg_t sel_ag1_counter1;
    volatile axi_perf_mon_sel_ag2_counter1_reg_t sel_ag2_counter1;
    volatile axi_perf_mon_sel_ag3_counter1_reg_t sel_ag3_counter1;
    volatile axi_perf_mon_sel_ag4_counter1_reg_t sel_ag4_counter1;
    volatile axi_perf_mon_sel_ag5_counter1_reg_t sel_ag5_counter1;
    volatile axi_perf_mon_sel_ag6_counter1_reg_t sel_ag6_counter1;
    volatile axi_perf_mon_sel_ag0_counter2_reg_t sel_ag0_counter2;
    volatile axi_perf_mon_sel_ag1_counter2_reg_t sel_ag1_counter2;
    volatile axi_perf_mon_sel_ag2_counter2_reg_t sel_ag2_counter2;
    volatile axi_perf_mon_sel_ag3_counter2_reg_t sel_ag3_counter2;
    volatile axi_perf_mon_sel_ag4_counter2_reg_t sel_ag4_counter2;
    volatile axi_perf_mon_sel_ag5_counter2_reg_t sel_ag5_counter2;
    volatile axi_perf_mon_sel_ag6_counter2_reg_t sel_ag6_counter2;
    volatile axi_perf_mon_sel_ag0_counter3_reg_t sel_ag0_counter3;
    volatile axi_perf_mon_sel_ag1_counter3_reg_t sel_ag1_counter3;
    volatile axi_perf_mon_sel_ag2_counter3_reg_t sel_ag2_counter3;
    volatile axi_perf_mon_sel_ag3_counter3_reg_t sel_ag3_counter3;
    volatile axi_perf_mon_sel_ag4_counter3_reg_t sel_ag4_counter3;
    volatile axi_perf_mon_sel_ag5_counter3_reg_t sel_ag5_counter3;
    volatile axi_perf_mon_sel_ag6_counter3_reg_t sel_ag6_counter3;
    volatile axi_perf_mon_sel_ag0_counter4_reg_t sel_ag0_counter4;
    volatile axi_perf_mon_sel_ag1_counter4_reg_t sel_ag1_counter4;
    volatile axi_perf_mon_sel_ag2_counter4_reg_t sel_ag2_counter4;
    volatile axi_perf_mon_sel_ag3_counter4_reg_t sel_ag3_counter4;
    volatile axi_perf_mon_sel_ag4_counter4_reg_t sel_ag4_counter4;
    volatile axi_perf_mon_sel_ag5_counter4_reg_t sel_ag5_counter4;
    volatile axi_perf_mon_sel_ag6_counter4_reg_t sel_ag6_counter4;
    volatile axi_perf_mon_sel_ag0_counter5_reg_t sel_ag0_counter5;
    volatile axi_perf_mon_sel_ag1_counter5_reg_t sel_ag1_counter5;
    volatile axi_perf_mon_sel_ag2_counter5_reg_t sel_ag2_counter5;
    volatile axi_perf_mon_sel_ag3_counter5_reg_t sel_ag3_counter5;
    volatile axi_perf_mon_sel_ag4_counter5_reg_t sel_ag4_counter5;
    volatile axi_perf_mon_sel_ag5_counter5_reg_t sel_ag5_counter5;
    volatile axi_perf_mon_sel_ag6_counter5_reg_t sel_ag6_counter5;
    volatile axi_perf_mon_sel_ag0_counter6_reg_t sel_ag0_counter6;
    volatile axi_perf_mon_sel_ag1_counter6_reg_t sel_ag1_counter6;
    volatile axi_perf_mon_sel_ag2_counter6_reg_t sel_ag2_counter6;
    volatile axi_perf_mon_sel_ag3_counter6_reg_t sel_ag3_counter6;
    volatile axi_perf_mon_sel_ag4_counter6_reg_t sel_ag4_counter6;
    volatile axi_perf_mon_sel_ag5_counter6_reg_t sel_ag5_counter6;
    volatile axi_perf_mon_sel_ag6_counter6_reg_t sel_ag6_counter6;
    volatile axi_perf_mon_sel_ag0_counter7_reg_t sel_ag0_counter7;
    volatile axi_perf_mon_sel_ag1_counter7_reg_t sel_ag1_counter7;
    volatile axi_perf_mon_sel_ag2_counter7_reg_t sel_ag2_counter7;
    volatile axi_perf_mon_sel_ag3_counter7_reg_t sel_ag3_counter7;
    volatile axi_perf_mon_sel_ag4_counter7_reg_t sel_ag4_counter7;
    volatile axi_perf_mon_sel_ag5_counter7_reg_t sel_ag5_counter7;
    volatile axi_perf_mon_sel_ag6_counter7_reg_t sel_ag6_counter7;
    volatile axi_perf_mon_sel_ag0_range0_reg_t sel_ag0_range0;
    volatile axi_perf_mon_sel_ag1_range0_reg_t sel_ag1_range0;
    volatile axi_perf_mon_sel_ag2_range0_reg_t sel_ag2_range0;
    volatile axi_perf_mon_sel_ag3_range0_reg_t sel_ag3_range0;
    volatile axi_perf_mon_sel_ag4_range0_reg_t sel_ag4_range0;
    volatile axi_perf_mon_sel_ag5_range0_reg_t sel_ag5_range0;
    volatile axi_perf_mon_sel_ag6_range0_reg_t sel_ag6_range0;
    volatile axi_perf_mon_sel_ag0_range1_reg_t sel_ag0_range1;
    volatile axi_perf_mon_sel_ag1_range1_reg_t sel_ag1_range1;
    volatile axi_perf_mon_sel_ag2_range1_reg_t sel_ag2_range1;
    volatile axi_perf_mon_sel_ag3_range1_reg_t sel_ag3_range1;
    volatile axi_perf_mon_sel_ag4_range1_reg_t sel_ag4_range1;
    volatile axi_perf_mon_sel_ag5_range1_reg_t sel_ag5_range1;
    volatile axi_perf_mon_sel_ag6_range1_reg_t sel_ag6_range1;
    volatile axi_perf_mon_sel_ag0_range2_reg_t sel_ag0_range2;
    volatile axi_perf_mon_sel_ag1_range2_reg_t sel_ag1_range2;
    volatile axi_perf_mon_sel_ag2_range2_reg_t sel_ag2_range2;
    volatile axi_perf_mon_sel_ag3_range2_reg_t sel_ag3_range2;
    volatile axi_perf_mon_sel_ag4_range2_reg_t sel_ag4_range2;
    volatile axi_perf_mon_sel_ag5_range2_reg_t sel_ag5_range2;
    volatile axi_perf_mon_sel_ag6_range2_reg_t sel_ag6_range2;
    volatile axi_perf_mon_sel_ag0_range3_reg_t sel_ag0_range3;
    volatile axi_perf_mon_sel_ag1_range3_reg_t sel_ag1_range3;
    volatile axi_perf_mon_sel_ag2_range3_reg_t sel_ag2_range3;
    volatile axi_perf_mon_sel_ag3_range3_reg_t sel_ag3_range3;
    volatile axi_perf_mon_sel_ag4_range3_reg_t sel_ag4_range3;
    volatile axi_perf_mon_sel_ag5_range3_reg_t sel_ag5_range3;
    volatile axi_perf_mon_sel_ag6_range3_reg_t sel_ag6_range3;
    volatile axi_perf_mon_sel_ag0_range4_reg_t sel_ag0_range4;
    volatile axi_perf_mon_sel_ag1_range4_reg_t sel_ag1_range4;
    volatile axi_perf_mon_sel_ag2_range4_reg_t sel_ag2_range4;
    volatile axi_perf_mon_sel_ag3_range4_reg_t sel_ag3_range4;
    volatile axi_perf_mon_sel_ag4_range4_reg_t sel_ag4_range4;
    volatile axi_perf_mon_sel_ag5_range4_reg_t sel_ag5_range4;
    volatile axi_perf_mon_sel_ag6_range4_reg_t sel_ag6_range4;
    volatile axi_perf_mon_sel_ag0_range5_reg_t sel_ag0_range5;
    volatile axi_perf_mon_sel_ag1_range5_reg_t sel_ag1_range5;
    volatile axi_perf_mon_sel_ag2_range5_reg_t sel_ag2_range5;
    volatile axi_perf_mon_sel_ag3_range5_reg_t sel_ag3_range5;
    volatile axi_perf_mon_sel_ag4_range5_reg_t sel_ag4_range5;
    volatile axi_perf_mon_sel_ag5_range5_reg_t sel_ag5_range5;
    volatile axi_perf_mon_sel_ag6_range5_reg_t sel_ag6_range5;
    volatile axi_perf_mon_sel_ag0_range6_reg_t sel_ag0_range6;
    volatile axi_perf_mon_sel_ag1_range6_reg_t sel_ag1_range6;
    volatile axi_perf_mon_sel_ag2_range6_reg_t sel_ag2_range6;
    volatile axi_perf_mon_sel_ag3_range6_reg_t sel_ag3_range6;
    volatile axi_perf_mon_sel_ag4_range6_reg_t sel_ag4_range6;
    volatile axi_perf_mon_sel_ag5_range6_reg_t sel_ag5_range6;
    volatile axi_perf_mon_sel_ag6_range6_reg_t sel_ag6_range6;
    volatile axi_perf_mon_sel_ag0_range7_reg_t sel_ag0_range7;
    volatile axi_perf_mon_sel_ag1_range7_reg_t sel_ag1_range7;
    volatile axi_perf_mon_sel_ag2_range7_reg_t sel_ag2_range7;
    volatile axi_perf_mon_sel_ag3_range7_reg_t sel_ag3_range7;
    volatile axi_perf_mon_sel_ag4_range7_reg_t sel_ag4_range7;
    volatile axi_perf_mon_sel_ag5_range7_reg_t sel_ag5_range7;
    volatile axi_perf_mon_sel_ag6_range7_reg_t sel_ag6_range7;
    volatile axi_perf_mon_sel_ag0_rd_axi_info_record0_reg_t sel_ag0_rd_axi_info_record0;
    volatile axi_perf_mon_sel_ag1_rd_axi_info_record0_reg_t sel_ag1_rd_axi_info_record0;
    volatile axi_perf_mon_sel_ag2_rd_axi_info_record0_reg_t sel_ag2_rd_axi_info_record0;
    volatile axi_perf_mon_sel_ag3_rd_axi_info_record0_reg_t sel_ag3_rd_axi_info_record0;
    volatile axi_perf_mon_sel_ag4_rd_axi_info_record0_reg_t sel_ag4_rd_axi_info_record0;
    volatile axi_perf_mon_sel_ag5_rd_axi_info_record0_reg_t sel_ag5_rd_axi_info_record0;
    volatile axi_perf_mon_sel_ag6_rd_axi_info_record0_reg_t sel_ag6_rd_axi_info_record0;
    volatile axi_perf_mon_sel_ag0_rd_axi_info_record1_reg_t sel_ag0_rd_axi_info_record1;
    volatile axi_perf_mon_sel_ag1_rd_axi_info_record1_reg_t sel_ag1_rd_axi_info_record1;
    volatile axi_perf_mon_sel_ag2_rd_axi_info_record1_reg_t sel_ag2_rd_axi_info_record1;
    volatile axi_perf_mon_sel_ag3_rd_axi_info_record1_reg_t sel_ag3_rd_axi_info_record1;
    volatile axi_perf_mon_sel_ag4_rd_axi_info_record1_reg_t sel_ag4_rd_axi_info_record1;
    volatile axi_perf_mon_sel_ag5_rd_axi_info_record1_reg_t sel_ag5_rd_axi_info_record1;
    volatile axi_perf_mon_sel_ag6_rd_axi_info_record1_reg_t sel_ag6_rd_axi_info_record1;
    volatile axi_perf_mon_sel_ag0_rd_axi_info_record2_reg_t sel_ag0_rd_axi_info_record2;
    volatile axi_perf_mon_sel_ag1_rd_axi_info_record2_reg_t sel_ag1_rd_axi_info_record2;
    volatile axi_perf_mon_sel_ag2_rd_axi_info_record2_reg_t sel_ag2_rd_axi_info_record2;
    volatile axi_perf_mon_sel_ag3_rd_axi_info_record2_reg_t sel_ag3_rd_axi_info_record2;
    volatile axi_perf_mon_sel_ag4_rd_axi_info_record2_reg_t sel_ag4_rd_axi_info_record2;
    volatile axi_perf_mon_sel_ag5_rd_axi_info_record2_reg_t sel_ag5_rd_axi_info_record2;
    volatile axi_perf_mon_sel_ag6_rd_axi_info_record2_reg_t sel_ag6_rd_axi_info_record2;
    volatile axi_perf_mon_sel_ag0_rd_axi_info_record3_reg_t sel_ag0_rd_axi_info_record3;
    volatile axi_perf_mon_sel_ag1_rd_axi_info_record3_reg_t sel_ag1_rd_axi_info_record3;
    volatile axi_perf_mon_sel_ag2_rd_axi_info_record3_reg_t sel_ag2_rd_axi_info_record3;
    volatile axi_perf_mon_sel_ag3_rd_axi_info_record3_reg_t sel_ag3_rd_axi_info_record3;
    volatile axi_perf_mon_sel_ag4_rd_axi_info_record3_reg_t sel_ag4_rd_axi_info_record3;
    volatile axi_perf_mon_sel_ag5_rd_axi_info_record3_reg_t sel_ag5_rd_axi_info_record3;
    volatile axi_perf_mon_sel_ag6_rd_axi_info_record3_reg_t sel_ag6_rd_axi_info_record3;
    volatile axi_perf_mon_sel_ag0_wr_axi_info_record0_reg_t sel_ag0_wr_axi_info_record0;
    volatile axi_perf_mon_sel_ag1_wr_axi_info_record0_reg_t sel_ag1_wr_axi_info_record0;
    volatile axi_perf_mon_sel_ag2_wr_axi_info_record0_reg_t sel_ag2_wr_axi_info_record0;
    volatile axi_perf_mon_sel_ag3_wr_axi_info_record0_reg_t sel_ag3_wr_axi_info_record0;
    volatile axi_perf_mon_sel_ag4_wr_axi_info_record0_reg_t sel_ag4_wr_axi_info_record0;
    volatile axi_perf_mon_sel_ag5_wr_axi_info_record0_reg_t sel_ag5_wr_axi_info_record0;
    volatile axi_perf_mon_sel_ag6_wr_axi_info_record0_reg_t sel_ag6_wr_axi_info_record0;
    volatile axi_perf_mon_sel_ag0_wr_axi_info_record1_reg_t sel_ag0_wr_axi_info_record1;
    volatile axi_perf_mon_sel_ag1_wr_axi_info_record1_reg_t sel_ag1_wr_axi_info_record1;
    volatile axi_perf_mon_sel_ag2_wr_axi_info_record1_reg_t sel_ag2_wr_axi_info_record1;
    volatile axi_perf_mon_sel_ag3_wr_axi_info_record1_reg_t sel_ag3_wr_axi_info_record1;
    volatile axi_perf_mon_sel_ag4_wr_axi_info_record1_reg_t sel_ag4_wr_axi_info_record1;
    volatile axi_perf_mon_sel_ag5_wr_axi_info_record1_reg_t sel_ag5_wr_axi_info_record1;
    volatile axi_perf_mon_sel_ag6_wr_axi_info_record1_reg_t sel_ag6_wr_axi_info_record1;
    volatile axi_perf_mon_sel_ag0_wr_axi_info_record2_reg_t sel_ag0_wr_axi_info_record2;
    volatile axi_perf_mon_sel_ag1_wr_axi_info_record2_reg_t sel_ag1_wr_axi_info_record2;
    volatile axi_perf_mon_sel_ag2_wr_axi_info_record2_reg_t sel_ag2_wr_axi_info_record2;
    volatile axi_perf_mon_sel_ag3_wr_axi_info_record2_reg_t sel_ag3_wr_axi_info_record2;
    volatile axi_perf_mon_sel_ag4_wr_axi_info_record2_reg_t sel_ag4_wr_axi_info_record2;
    volatile axi_perf_mon_sel_ag5_wr_axi_info_record2_reg_t sel_ag5_wr_axi_info_record2;
    volatile axi_perf_mon_sel_ag6_wr_axi_info_record2_reg_t sel_ag6_wr_axi_info_record2;
    volatile axi_perf_mon_sel_ag0_wr_axi_info_record3_reg_t sel_ag0_wr_axi_info_record3;
    volatile axi_perf_mon_sel_ag1_wr_axi_info_record3_reg_t sel_ag1_wr_axi_info_record3;
    volatile axi_perf_mon_sel_ag2_wr_axi_info_record3_reg_t sel_ag2_wr_axi_info_record3;
    volatile axi_perf_mon_sel_ag3_wr_axi_info_record3_reg_t sel_ag3_wr_axi_info_record3;
    volatile axi_perf_mon_sel_ag4_wr_axi_info_record3_reg_t sel_ag4_wr_axi_info_record3;
    volatile axi_perf_mon_sel_ag5_wr_axi_info_record3_reg_t sel_ag5_wr_axi_info_record3;
    volatile axi_perf_mon_sel_ag6_wr_axi_info_record3_reg_t sel_ag6_wr_axi_info_record3;
    volatile axi_perf_mon_sel_ag0_ins_bandw_data0_reg_t sel_ag0_ins_bandw_data0;
    volatile axi_perf_mon_sel_ag1_ins_bandw_data0_reg_t sel_ag1_ins_bandw_data0;
    volatile axi_perf_mon_sel_ag2_ins_bandw_data0_reg_t sel_ag2_ins_bandw_data0;
    volatile axi_perf_mon_sel_ag3_ins_bandw_data0_reg_t sel_ag3_ins_bandw_data0;
    volatile axi_perf_mon_sel_ag4_ins_bandw_data0_reg_t sel_ag4_ins_bandw_data0;
    volatile axi_perf_mon_sel_ag5_ins_bandw_data0_reg_t sel_ag5_ins_bandw_data0;
    volatile axi_perf_mon_sel_ag6_ins_bandw_data0_reg_t sel_ag6_ins_bandw_data0;
    volatile axi_perf_mon_sel_ag0_ins_bandw_data1_reg_t sel_ag0_ins_bandw_data1;
    volatile axi_perf_mon_sel_ag1_ins_bandw_data1_reg_t sel_ag1_ins_bandw_data1;
    volatile axi_perf_mon_sel_ag2_ins_bandw_data1_reg_t sel_ag2_ins_bandw_data1;
    volatile axi_perf_mon_sel_ag3_ins_bandw_data1_reg_t sel_ag3_ins_bandw_data1;
    volatile axi_perf_mon_sel_ag4_ins_bandw_data1_reg_t sel_ag4_ins_bandw_data1;
    volatile axi_perf_mon_sel_ag5_ins_bandw_data1_reg_t sel_ag5_ins_bandw_data1;
    volatile axi_perf_mon_sel_ag6_ins_bandw_data1_reg_t sel_ag6_ins_bandw_data1;
    volatile axi_perf_mon_sel_ag0_ins_bandw_data2_reg_t sel_ag0_ins_bandw_data2;
    volatile axi_perf_mon_sel_ag1_ins_bandw_data2_reg_t sel_ag1_ins_bandw_data2;
    volatile axi_perf_mon_sel_ag2_ins_bandw_data2_reg_t sel_ag2_ins_bandw_data2;
    volatile axi_perf_mon_sel_ag3_ins_bandw_data2_reg_t sel_ag3_ins_bandw_data2;
    volatile axi_perf_mon_sel_ag4_ins_bandw_data2_reg_t sel_ag4_ins_bandw_data2;
    volatile axi_perf_mon_sel_ag5_ins_bandw_data2_reg_t sel_ag5_ins_bandw_data2;
    volatile axi_perf_mon_sel_ag6_ins_bandw_data2_reg_t sel_ag6_ins_bandw_data2;
    volatile axi_perf_mon_sel_ag0_ins_bandw_data3_reg_t sel_ag0_ins_bandw_data3;
    volatile axi_perf_mon_sel_ag1_ins_bandw_data3_reg_t sel_ag1_ins_bandw_data3;
    volatile axi_perf_mon_sel_ag2_ins_bandw_data3_reg_t sel_ag2_ins_bandw_data3;
    volatile axi_perf_mon_sel_ag3_ins_bandw_data3_reg_t sel_ag3_ins_bandw_data3;
    volatile axi_perf_mon_sel_ag4_ins_bandw_data3_reg_t sel_ag4_ins_bandw_data3;
    volatile axi_perf_mon_sel_ag5_ins_bandw_data3_reg_t sel_ag5_ins_bandw_data3;
    volatile axi_perf_mon_sel_ag6_ins_bandw_data3_reg_t sel_ag6_ins_bandw_data3;
    volatile axi_perf_mon_sel_ag0_metric_range0_reg_t sel_ag0_metric_range0;
    volatile axi_perf_mon_sel_ag1_metric_range0_reg_t sel_ag1_metric_range0;
    volatile axi_perf_mon_sel_ag2_metric_range0_reg_t sel_ag2_metric_range0;
    volatile axi_perf_mon_sel_ag3_metric_range0_reg_t sel_ag3_metric_range0;
    volatile axi_perf_mon_sel_ag4_metric_range0_reg_t sel_ag4_metric_range0;
    volatile axi_perf_mon_sel_ag5_metric_range0_reg_t sel_ag5_metric_range0;
    volatile axi_perf_mon_sel_ag6_metric_range0_reg_t sel_ag6_metric_range0;
    volatile axi_perf_mon_sel_ag0_metric_range1_reg_t sel_ag0_metric_range1;
    volatile axi_perf_mon_sel_ag1_metric_range1_reg_t sel_ag1_metric_range1;
    volatile axi_perf_mon_sel_ag2_metric_range1_reg_t sel_ag2_metric_range1;
    volatile axi_perf_mon_sel_ag3_metric_range1_reg_t sel_ag3_metric_range1;
    volatile axi_perf_mon_sel_ag4_metric_range1_reg_t sel_ag4_metric_range1;
    volatile axi_perf_mon_sel_ag5_metric_range1_reg_t sel_ag5_metric_range1;
    volatile axi_perf_mon_sel_ag6_metric_range1_reg_t sel_ag6_metric_range1;
    volatile axi_perf_mon_sel_ag0_metric_range2_reg_t sel_ag0_metric_range2;
    volatile axi_perf_mon_sel_ag1_metric_range2_reg_t sel_ag1_metric_range2;
    volatile axi_perf_mon_sel_ag2_metric_range2_reg_t sel_ag2_metric_range2;
    volatile axi_perf_mon_sel_ag3_metric_range2_reg_t sel_ag3_metric_range2;
    volatile axi_perf_mon_sel_ag4_metric_range2_reg_t sel_ag4_metric_range2;
    volatile axi_perf_mon_sel_ag5_metric_range2_reg_t sel_ag5_metric_range2;
    volatile axi_perf_mon_sel_ag6_metric_range2_reg_t sel_ag6_metric_range2;
    volatile axi_perf_mon_sel_ag0_metric_range3_reg_t sel_ag0_metric_range3;
    volatile axi_perf_mon_sel_ag1_metric_range3_reg_t sel_ag1_metric_range3;
    volatile axi_perf_mon_sel_ag2_metric_range3_reg_t sel_ag2_metric_range3;
    volatile axi_perf_mon_sel_ag3_metric_range3_reg_t sel_ag3_metric_range3;
    volatile axi_perf_mon_sel_ag4_metric_range3_reg_t sel_ag4_metric_range3;
    volatile axi_perf_mon_sel_ag5_metric_range3_reg_t sel_ag5_metric_range3;
    volatile axi_perf_mon_sel_ag6_metric_range3_reg_t sel_ag6_metric_range3;
    volatile axi_perf_mon_sel_ag0_metric_range4_reg_t sel_ag0_metric_range4;
    volatile axi_perf_mon_sel_ag1_metric_range4_reg_t sel_ag1_metric_range4;
    volatile axi_perf_mon_sel_ag2_metric_range4_reg_t sel_ag2_metric_range4;
    volatile axi_perf_mon_sel_ag3_metric_range4_reg_t sel_ag3_metric_range4;
    volatile axi_perf_mon_sel_ag4_metric_range4_reg_t sel_ag4_metric_range4;
    volatile axi_perf_mon_sel_ag5_metric_range4_reg_t sel_ag5_metric_range4;
    volatile axi_perf_mon_sel_ag6_metric_range4_reg_t sel_ag6_metric_range4;
    volatile axi_perf_mon_sel_ag0_metric_range5_reg_t sel_ag0_metric_range5;
    volatile axi_perf_mon_sel_ag1_metric_range5_reg_t sel_ag1_metric_range5;
    volatile axi_perf_mon_sel_ag2_metric_range5_reg_t sel_ag2_metric_range5;
    volatile axi_perf_mon_sel_ag3_metric_range5_reg_t sel_ag3_metric_range5;
    volatile axi_perf_mon_sel_ag4_metric_range5_reg_t sel_ag4_metric_range5;
    volatile axi_perf_mon_sel_ag5_metric_range5_reg_t sel_ag5_metric_range5;
    volatile axi_perf_mon_sel_ag6_metric_range5_reg_t sel_ag6_metric_range5;
    volatile axi_perf_mon_sel_ag0_metric_range6_reg_t sel_ag0_metric_range6;
    volatile axi_perf_mon_sel_ag1_metric_range6_reg_t sel_ag1_metric_range6;
    volatile axi_perf_mon_sel_ag2_metric_range6_reg_t sel_ag2_metric_range6;
    volatile axi_perf_mon_sel_ag3_metric_range6_reg_t sel_ag3_metric_range6;
    volatile axi_perf_mon_sel_ag4_metric_range6_reg_t sel_ag4_metric_range6;
    volatile axi_perf_mon_sel_ag5_metric_range6_reg_t sel_ag5_metric_range6;
    volatile axi_perf_mon_sel_ag6_metric_range6_reg_t sel_ag6_metric_range6;
    volatile axi_perf_mon_sel_ag0_metric_range7_reg_t sel_ag0_metric_range7;
    volatile axi_perf_mon_sel_ag1_metric_range7_reg_t sel_ag1_metric_range7;
    volatile axi_perf_mon_sel_ag2_metric_range7_reg_t sel_ag2_metric_range7;
    volatile axi_perf_mon_sel_ag3_metric_range7_reg_t sel_ag3_metric_range7;
    volatile axi_perf_mon_sel_ag4_metric_range7_reg_t sel_ag4_metric_range7;
    volatile axi_perf_mon_sel_ag5_metric_range7_reg_t sel_ag5_metric_range7;
    volatile axi_perf_mon_sel_ag6_metric_range7_reg_t sel_ag6_metric_range7;
    volatile axi_perf_mon_sel_ag0_rd_addr_mask0_reg_t sel_ag0_rd_addr_mask0;
    volatile axi_perf_mon_sel_ag1_rd_addr_mask0_reg_t sel_ag1_rd_addr_mask0;
    volatile axi_perf_mon_sel_ag2_rd_addr_mask0_reg_t sel_ag2_rd_addr_mask0;
    volatile axi_perf_mon_sel_ag3_rd_addr_mask0_reg_t sel_ag3_rd_addr_mask0;
    volatile axi_perf_mon_sel_ag4_rd_addr_mask0_reg_t sel_ag4_rd_addr_mask0;
    volatile axi_perf_mon_sel_ag5_rd_addr_mask0_reg_t sel_ag5_rd_addr_mask0;
    volatile axi_perf_mon_sel_ag6_rd_addr_mask0_reg_t sel_ag6_rd_addr_mask0;
    volatile axi_perf_mon_sel_ag0_rd_addr_mask1_reg_t sel_ag0_rd_addr_mask1;
    volatile axi_perf_mon_sel_ag1_rd_addr_mask1_reg_t sel_ag1_rd_addr_mask1;
    volatile axi_perf_mon_sel_ag2_rd_addr_mask1_reg_t sel_ag2_rd_addr_mask1;
    volatile axi_perf_mon_sel_ag3_rd_addr_mask1_reg_t sel_ag3_rd_addr_mask1;
    volatile axi_perf_mon_sel_ag4_rd_addr_mask1_reg_t sel_ag4_rd_addr_mask1;
    volatile axi_perf_mon_sel_ag5_rd_addr_mask1_reg_t sel_ag5_rd_addr_mask1;
    volatile axi_perf_mon_sel_ag6_rd_addr_mask1_reg_t sel_ag6_rd_addr_mask1;
    volatile axi_perf_mon_sel_ag0_rd_addr_mask2_reg_t sel_ag0_rd_addr_mask2;
    volatile axi_perf_mon_sel_ag1_rd_addr_mask2_reg_t sel_ag1_rd_addr_mask2;
    volatile axi_perf_mon_sel_ag2_rd_addr_mask2_reg_t sel_ag2_rd_addr_mask2;
    volatile axi_perf_mon_sel_ag3_rd_addr_mask2_reg_t sel_ag3_rd_addr_mask2;
    volatile axi_perf_mon_sel_ag4_rd_addr_mask2_reg_t sel_ag4_rd_addr_mask2;
    volatile axi_perf_mon_sel_ag5_rd_addr_mask2_reg_t sel_ag5_rd_addr_mask2;
    volatile axi_perf_mon_sel_ag6_rd_addr_mask2_reg_t sel_ag6_rd_addr_mask2;
    volatile axi_perf_mon_sel_ag0_wr_addr_mask0_reg_t sel_ag0_wr_addr_mask0;
    volatile axi_perf_mon_sel_ag1_wr_addr_mask0_reg_t sel_ag1_wr_addr_mask0;
    volatile axi_perf_mon_sel_ag2_wr_addr_mask0_reg_t sel_ag2_wr_addr_mask0;
    volatile axi_perf_mon_sel_ag3_wr_addr_mask0_reg_t sel_ag3_wr_addr_mask0;
    volatile axi_perf_mon_sel_ag4_wr_addr_mask0_reg_t sel_ag4_wr_addr_mask0;
    volatile axi_perf_mon_sel_ag5_wr_addr_mask0_reg_t sel_ag5_wr_addr_mask0;
    volatile axi_perf_mon_sel_ag6_wr_addr_mask0_reg_t sel_ag6_wr_addr_mask0;
    volatile axi_perf_mon_sel_ag0_wr_addr_mask1_reg_t sel_ag0_wr_addr_mask1;
    volatile axi_perf_mon_sel_ag1_wr_addr_mask1_reg_t sel_ag1_wr_addr_mask1;
    volatile axi_perf_mon_sel_ag2_wr_addr_mask1_reg_t sel_ag2_wr_addr_mask1;
    volatile axi_perf_mon_sel_ag3_wr_addr_mask1_reg_t sel_ag3_wr_addr_mask1;
    volatile axi_perf_mon_sel_ag4_wr_addr_mask1_reg_t sel_ag4_wr_addr_mask1;
    volatile axi_perf_mon_sel_ag5_wr_addr_mask1_reg_t sel_ag5_wr_addr_mask1;
    volatile axi_perf_mon_sel_ag6_wr_addr_mask1_reg_t sel_ag6_wr_addr_mask1;
    volatile axi_perf_mon_sel_ag0_wr_addr_mask2_reg_t sel_ag0_wr_addr_mask2;
    volatile axi_perf_mon_sel_ag1_wr_addr_mask2_reg_t sel_ag1_wr_addr_mask2;
    volatile axi_perf_mon_sel_ag2_wr_addr_mask2_reg_t sel_ag2_wr_addr_mask2;
    volatile axi_perf_mon_sel_ag3_wr_addr_mask2_reg_t sel_ag3_wr_addr_mask2;
    volatile axi_perf_mon_sel_ag4_wr_addr_mask2_reg_t sel_ag4_wr_addr_mask2;
    volatile axi_perf_mon_sel_ag5_wr_addr_mask2_reg_t sel_ag5_wr_addr_mask2;
    volatile axi_perf_mon_sel_ag6_wr_addr_mask2_reg_t sel_ag6_wr_addr_mask2;
    volatile axi_perf_mon_sel_ag0_rd_addr_filter0_reg_t sel_ag0_rd_addr_filter0;
    volatile axi_perf_mon_sel_ag1_rd_addr_filter0_reg_t sel_ag1_rd_addr_filter0;
    volatile axi_perf_mon_sel_ag2_rd_addr_filter0_reg_t sel_ag2_rd_addr_filter0;
    volatile axi_perf_mon_sel_ag3_rd_addr_filter0_reg_t sel_ag3_rd_addr_filter0;
    volatile axi_perf_mon_sel_ag4_rd_addr_filter0_reg_t sel_ag4_rd_addr_filter0;
    volatile axi_perf_mon_sel_ag5_rd_addr_filter0_reg_t sel_ag5_rd_addr_filter0;
    volatile axi_perf_mon_sel_ag6_rd_addr_filter0_reg_t sel_ag6_rd_addr_filter0;
    volatile axi_perf_mon_sel_ag0_rd_addr_filter1_reg_t sel_ag0_rd_addr_filter1;
    volatile axi_perf_mon_sel_ag1_rd_addr_filter1_reg_t sel_ag1_rd_addr_filter1;
    volatile axi_perf_mon_sel_ag2_rd_addr_filter1_reg_t sel_ag2_rd_addr_filter1;
    volatile axi_perf_mon_sel_ag3_rd_addr_filter1_reg_t sel_ag3_rd_addr_filter1;
    volatile axi_perf_mon_sel_ag4_rd_addr_filter1_reg_t sel_ag4_rd_addr_filter1;
    volatile axi_perf_mon_sel_ag5_rd_addr_filter1_reg_t sel_ag5_rd_addr_filter1;
    volatile axi_perf_mon_sel_ag6_rd_addr_filter1_reg_t sel_ag6_rd_addr_filter1;
    volatile axi_perf_mon_sel_ag0_rd_addr_filter2_reg_t sel_ag0_rd_addr_filter2;
    volatile axi_perf_mon_sel_ag1_rd_addr_filter2_reg_t sel_ag1_rd_addr_filter2;
    volatile axi_perf_mon_sel_ag2_rd_addr_filter2_reg_t sel_ag2_rd_addr_filter2;
    volatile axi_perf_mon_sel_ag3_rd_addr_filter2_reg_t sel_ag3_rd_addr_filter2;
    volatile axi_perf_mon_sel_ag4_rd_addr_filter2_reg_t sel_ag4_rd_addr_filter2;
    volatile axi_perf_mon_sel_ag5_rd_addr_filter2_reg_t sel_ag5_rd_addr_filter2;
    volatile axi_perf_mon_sel_ag6_rd_addr_filter2_reg_t sel_ag6_rd_addr_filter2;
    volatile axi_perf_mon_sel_ag0_wr_addr_filter0_reg_t sel_ag0_wr_addr_filter0;
    volatile axi_perf_mon_sel_ag1_wr_addr_filter0_reg_t sel_ag1_wr_addr_filter0;
    volatile axi_perf_mon_sel_ag2_wr_addr_filter0_reg_t sel_ag2_wr_addr_filter0;
    volatile axi_perf_mon_sel_ag3_wr_addr_filter0_reg_t sel_ag3_wr_addr_filter0;
    volatile axi_perf_mon_sel_ag4_wr_addr_filter0_reg_t sel_ag4_wr_addr_filter0;
    volatile axi_perf_mon_sel_ag5_wr_addr_filter0_reg_t sel_ag5_wr_addr_filter0;
    volatile axi_perf_mon_sel_ag6_wr_addr_filter0_reg_t sel_ag6_wr_addr_filter0;
    volatile axi_perf_mon_sel_ag0_wr_addr_filter1_reg_t sel_ag0_wr_addr_filter1;
    volatile axi_perf_mon_sel_ag1_wr_addr_filter1_reg_t sel_ag1_wr_addr_filter1;
    volatile axi_perf_mon_sel_ag2_wr_addr_filter1_reg_t sel_ag2_wr_addr_filter1;
    volatile axi_perf_mon_sel_ag3_wr_addr_filter1_reg_t sel_ag3_wr_addr_filter1;
    volatile axi_perf_mon_sel_ag4_wr_addr_filter1_reg_t sel_ag4_wr_addr_filter1;
    volatile axi_perf_mon_sel_ag5_wr_addr_filter1_reg_t sel_ag5_wr_addr_filter1;
    volatile axi_perf_mon_sel_ag6_wr_addr_filter1_reg_t sel_ag6_wr_addr_filter1;
    volatile axi_perf_mon_sel_ag0_wr_addr_filter2_reg_t sel_ag0_wr_addr_filter2;
    volatile axi_perf_mon_sel_ag1_wr_addr_filter2_reg_t sel_ag1_wr_addr_filter2;
    volatile axi_perf_mon_sel_ag2_wr_addr_filter2_reg_t sel_ag2_wr_addr_filter2;
    volatile axi_perf_mon_sel_ag3_wr_addr_filter2_reg_t sel_ag3_wr_addr_filter2;
    volatile axi_perf_mon_sel_ag4_wr_addr_filter2_reg_t sel_ag4_wr_addr_filter2;
    volatile axi_perf_mon_sel_ag5_wr_addr_filter2_reg_t sel_ag5_wr_addr_filter2;
    volatile axi_perf_mon_sel_ag6_wr_addr_filter2_reg_t sel_ag6_wr_addr_filter2;
    volatile axi_perf_mon_sel_ag0_metric_select1_reg_t sel_ag0_metric_select1;
    volatile axi_perf_mon_sel_ag1_metric_select1_reg_t sel_ag1_metric_select1;
    volatile axi_perf_mon_sel_ag2_metric_select1_reg_t sel_ag2_metric_select1;
    volatile axi_perf_mon_sel_ag3_metric_select1_reg_t sel_ag3_metric_select1;
    volatile axi_perf_mon_sel_ag4_metric_select1_reg_t sel_ag4_metric_select1;
    volatile axi_perf_mon_sel_ag5_metric_select1_reg_t sel_ag5_metric_select1;
    volatile axi_perf_mon_sel_ag6_metric_select1_reg_t sel_ag6_metric_select1;
    volatile axi_perf_mon_sel_ag0_metric_select2_reg_t sel_ag0_metric_select2;
    volatile axi_perf_mon_sel_ag1_metric_select2_reg_t sel_ag1_metric_select2;
    volatile axi_perf_mon_sel_ag2_metric_select2_reg_t sel_ag2_metric_select2;
    volatile axi_perf_mon_sel_ag3_metric_select2_reg_t sel_ag3_metric_select2;
    volatile axi_perf_mon_sel_ag4_metric_select2_reg_t sel_ag4_metric_select2;
    volatile axi_perf_mon_sel_ag5_metric_select2_reg_t sel_ag5_metric_select2;
    volatile axi_perf_mon_sel_ag6_metric_select2_reg_t sel_ag6_metric_select2;
    volatile axi_perf_mon_sel_ag_rd_addr_region_sel_reg_t sel_ag_rd_addr_region_sel;
    volatile axi_perf_mon_sel_ag_wr_addr_region_sel_reg_t sel_ag_wr_addr_region_sel;
    volatile axi_perf_mon_sel_ag_addr_filter_en_reg_t sel_ag_addr_filter_en;
    volatile axi_perf_mon_sel_ag_sw_record_stop_en_reg_t sel_ag_sw_record_stop_en;
    volatile axi_perf_mon_sel_ag_sw_record_stop_clr_reg_t sel_ag_sw_record_stop_clr;
    volatile axi_perf_mon_sel_ag_ins_bandw_test_en_reg_t sel_ag_ins_bandw_test_en;
    volatile axi_perf_mon_sel_ag0_sw_record_stop_data_limit_reg_t sel_ag0_sw_record_stop_data_limit;
    volatile axi_perf_mon_sel_ag1_sw_record_stop_data_limit_reg_t sel_ag1_sw_record_stop_data_limit;
    volatile axi_perf_mon_sel_ag2_sw_record_stop_data_limit_reg_t sel_ag2_sw_record_stop_data_limit;
    volatile axi_perf_mon_sel_ag3_sw_record_stop_data_limit_reg_t sel_ag3_sw_record_stop_data_limit;
    volatile axi_perf_mon_sel_ag4_sw_record_stop_data_limit_reg_t sel_ag4_sw_record_stop_data_limit;
    volatile axi_perf_mon_sel_ag5_sw_record_stop_data_limit_reg_t sel_ag5_sw_record_stop_data_limit;
    volatile axi_perf_mon_sel_ag6_sw_record_stop_data_limit_reg_t sel_ag6_sw_record_stop_data_limit;
    volatile axi_perf_mon_sel_ag0_id_mask_reg_t sel_ag0_id_mask;
    volatile axi_perf_mon_sel_ag1_id_mask_reg_t sel_ag1_id_mask;
    volatile axi_perf_mon_sel_ag2_id_mask_reg_t sel_ag2_id_mask;
    volatile axi_perf_mon_sel_ag3_id_mask_reg_t sel_ag3_id_mask;
    volatile axi_perf_mon_sel_ag4_id_mask_reg_t sel_ag4_id_mask;
    volatile axi_perf_mon_sel_ag5_id_mask_reg_t sel_ag5_id_mask;
    volatile axi_perf_mon_sel_ag6_id_mask_reg_t sel_ag6_id_mask;
    volatile axi_perf_mon_sel_ag0_id_filter_reg_t sel_ag0_id_filter;
    volatile axi_perf_mon_sel_ag1_id_filter_reg_t sel_ag1_id_filter;
    volatile axi_perf_mon_sel_ag2_id_filter_reg_t sel_ag2_id_filter;
    volatile axi_perf_mon_sel_ag3_id_filter_reg_t sel_ag3_id_filter;
    volatile axi_perf_mon_sel_ag4_id_filter_reg_t sel_ag4_id_filter;
    volatile axi_perf_mon_sel_ag5_id_filter_reg_t sel_ag5_id_filter;
    volatile axi_perf_mon_sel_ag6_id_filter_reg_t sel_ag6_id_filter;
    volatile axi_perf_mon_sel_ag_bandw_test_en_reg_t sel_ag_bandw_test_en;
    volatile axi_perf_mon_sel_ag_bandw_test_stop_reg_t sel_ag_bandw_test_stop;
    volatile axi_perf_mon_sel_ag0_bandw_trigger_in_sel_reg_t sel_ag0_bandw_trigger_in_sel;
    volatile axi_perf_mon_sel_ag1_bandw_trigger_in_sel_reg_t sel_ag1_bandw_trigger_in_sel;
    volatile axi_perf_mon_sel_ag2_bandw_trigger_in_sel_reg_t sel_ag2_bandw_trigger_in_sel;
    volatile axi_perf_mon_sel_ag3_bandw_trigger_in_sel_reg_t sel_ag3_bandw_trigger_in_sel;
    volatile axi_perf_mon_sel_ag4_bandw_trigger_in_sel_reg_t sel_ag4_bandw_trigger_in_sel;
    volatile axi_perf_mon_sel_ag5_bandw_trigger_in_sel_reg_t sel_ag5_bandw_trigger_in_sel;
    volatile axi_perf_mon_sel_ag6_bandw_trigger_in_sel_reg_t sel_ag6_bandw_trigger_in_sel;
    volatile axi_perf_mon_sel_ag0_wr_bandw_cnt_valid_strobe_num_reg_t sel_ag0_wr_bandw_cnt_valid_strobe_num;
    volatile axi_perf_mon_sel_ag1_wr_bandw_cnt_valid_strobe_num_reg_t sel_ag1_wr_bandw_cnt_valid_strobe_num;
    volatile axi_perf_mon_sel_ag2_wr_bandw_cnt_valid_strobe_num_reg_t sel_ag2_wr_bandw_cnt_valid_strobe_num;
    volatile axi_perf_mon_sel_ag3_wr_bandw_cnt_valid_strobe_num_reg_t sel_ag3_wr_bandw_cnt_valid_strobe_num;
    volatile axi_perf_mon_sel_ag4_wr_bandw_cnt_valid_strobe_num_reg_t sel_ag4_wr_bandw_cnt_valid_strobe_num;
    volatile axi_perf_mon_sel_ag5_wr_bandw_cnt_valid_strobe_num_reg_t sel_ag5_wr_bandw_cnt_valid_strobe_num;
    volatile axi_perf_mon_sel_ag6_wr_bandw_cnt_valid_strobe_num_reg_t sel_ag6_wr_bandw_cnt_valid_strobe_num;
    volatile axi_perf_mon_sel_ag0_rd_bandw_cnt_valid_strobe_num_reg_t sel_ag0_rd_bandw_cnt_valid_strobe_num;
    volatile axi_perf_mon_sel_ag1_rd_bandw_cnt_valid_strobe_num_reg_t sel_ag1_rd_bandw_cnt_valid_strobe_num;
    volatile axi_perf_mon_sel_ag2_rd_bandw_cnt_valid_strobe_num_reg_t sel_ag2_rd_bandw_cnt_valid_strobe_num;
    volatile axi_perf_mon_sel_ag3_rd_bandw_cnt_valid_strobe_num_reg_t sel_ag3_rd_bandw_cnt_valid_strobe_num;
    volatile axi_perf_mon_sel_ag4_rd_bandw_cnt_valid_strobe_num_reg_t sel_ag4_rd_bandw_cnt_valid_strobe_num;
    volatile axi_perf_mon_sel_ag5_rd_bandw_cnt_valid_strobe_num_reg_t sel_ag5_rd_bandw_cnt_valid_strobe_num;
    volatile axi_perf_mon_sel_ag6_rd_bandw_cnt_valid_strobe_num_reg_t sel_ag6_rd_bandw_cnt_valid_strobe_num;
    volatile axi_perf_mon_sel_ag0_ins_bandw_time_thr_reg_t sel_ag0_ins_bandw_time_thr;
    volatile axi_perf_mon_sel_ag1_ins_bandw_time_thr_reg_t sel_ag1_ins_bandw_time_thr;
    volatile axi_perf_mon_sel_ag2_ins_bandw_time_thr_reg_t sel_ag2_ins_bandw_time_thr;
    volatile axi_perf_mon_sel_ag3_ins_bandw_time_thr_reg_t sel_ag3_ins_bandw_time_thr;
    volatile axi_perf_mon_sel_ag4_ins_bandw_time_thr_reg_t sel_ag4_ins_bandw_time_thr;
    volatile axi_perf_mon_sel_ag5_ins_bandw_time_thr_reg_t sel_ag5_ins_bandw_time_thr;
    volatile axi_perf_mon_sel_ag6_ins_bandw_time_thr_reg_t sel_ag6_ins_bandw_time_thr;
    volatile axi_perf_mon_sel_ag_int_raw_reg_t sel_ag_int_raw;
    volatile axi_perf_mon_sel_ag_int_st_reg_t sel_ag_int_st;
    volatile axi_perf_mon_sel_ag_int_ena_reg_t sel_ag_int_ena;
    volatile axi_perf_mon_sel_ag_int_clr_reg_t sel_ag_int_clr;
    volatile axi_perf_mon_date_reg_t date;
} axi_perf_mon_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(axi_perf_mon_dev_t) == 0x678, "Invalid size of axi_perf_mon_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
