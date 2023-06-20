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

/** Group: Tee version register. */
/** Type of date register
 *  NA
 */
typedef union {
    struct {
        /** tee_date : R/W; bitpos: [31:0]; default: 539165460;
         *  NA
         */
        uint32_t tee_date:32;
    };
    uint32_t val;
} tee_date_reg_t;


/** Group: Tee regbank clock gating control register. */
/** Type of clk_en register
 *  NA
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} tee_clk_en_reg_t;


/** Group: Tee region configuration registers. */
/** Type of region0_low register
 *  Region0 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region0_low : R/W; bitpos: [31:12]; default: 0;
         *  Region0 address low.
         */
        uint32_t region0_low:20;
    };
    uint32_t val;
} tee_region0_low_reg_t;

/** Type of region0_high register
 *  Region0 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region0_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region0 address high.
         */
        uint32_t region0_high:20;
    };
    uint32_t val;
} tee_region0_high_reg_t;

/** Type of region1_low register
 *  Region1 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region1_low : R/W; bitpos: [31:12]; default: 0;
         *  Region1 address low.
         */
        uint32_t region1_low:20;
    };
    uint32_t val;
} tee_region1_low_reg_t;

/** Type of region1_high register
 *  Region1 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region1_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region1 address high.
         */
        uint32_t region1_high:20;
    };
    uint32_t val;
} tee_region1_high_reg_t;

/** Type of region2_low register
 *  Region2 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region2_low : R/W; bitpos: [31:12]; default: 0;
         *  Region2 address low.
         */
        uint32_t region2_low:20;
    };
    uint32_t val;
} tee_region2_low_reg_t;

/** Type of region2_high register
 *  Region2 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region2_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region2 address high.
         */
        uint32_t region2_high:20;
    };
    uint32_t val;
} tee_region2_high_reg_t;

/** Type of region3_low register
 *  Region3 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region3_low : R/W; bitpos: [31:12]; default: 0;
         *  Region3 address low.
         */
        uint32_t region3_low:20;
    };
    uint32_t val;
} tee_region3_low_reg_t;

/** Type of region3_high register
 *  Region3 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region3_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region3 address high.
         */
        uint32_t region3_high:20;
    };
    uint32_t val;
} tee_region3_high_reg_t;

/** Type of region4_low register
 *  Region4 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region4_low : R/W; bitpos: [31:12]; default: 0;
         *  Region4 address low.
         */
        uint32_t region4_low:20;
    };
    uint32_t val;
} tee_region4_low_reg_t;

/** Type of region4_high register
 *  Region4 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region4_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region4 address high.
         */
        uint32_t region4_high:20;
    };
    uint32_t val;
} tee_region4_high_reg_t;

/** Type of region5_low register
 *  Region5 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region5_low : R/W; bitpos: [31:12]; default: 0;
         *  Region5 address low.
         */
        uint32_t region5_low:20;
    };
    uint32_t val;
} tee_region5_low_reg_t;

/** Type of region5_high register
 *  Region5 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region5_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region5 address high.
         */
        uint32_t region5_high:20;
    };
    uint32_t val;
} tee_region5_high_reg_t;

/** Type of region6_low register
 *  Region6 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region6_low : R/W; bitpos: [31:12]; default: 0;
         *  Region6 address low.
         */
        uint32_t region6_low:20;
    };
    uint32_t val;
} tee_region6_low_reg_t;

/** Type of region6_high register
 *  Region6 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region6_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region6 address high.
         */
        uint32_t region6_high:20;
    };
    uint32_t val;
} tee_region6_high_reg_t;

/** Type of region7_low register
 *  Region7 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region7_low : R/W; bitpos: [31:12]; default: 0;
         *  Region7 address low.
         */
        uint32_t region7_low:20;
    };
    uint32_t val;
} tee_region7_low_reg_t;

/** Type of region7_high register
 *  Region7 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region7_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region7 address high.
         */
        uint32_t region7_high:20;
    };
    uint32_t val;
} tee_region7_high_reg_t;

/** Type of region8_low register
 *  Region8 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region8_low : R/W; bitpos: [31:12]; default: 0;
         *  Region8 address low.
         */
        uint32_t region8_low:20;
    };
    uint32_t val;
} tee_region8_low_reg_t;

/** Type of region8_high register
 *  Region8 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region8_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region8 address high.
         */
        uint32_t region8_high:20;
    };
    uint32_t val;
} tee_region8_high_reg_t;

/** Type of region9_low register
 *  Region9 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region9_low : R/W; bitpos: [31:12]; default: 0;
         *  Region9 address low.
         */
        uint32_t region9_low:20;
    };
    uint32_t val;
} tee_region9_low_reg_t;

/** Type of region9_high register
 *  Region9 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region9_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region9 address high.
         */
        uint32_t region9_high:20;
    };
    uint32_t val;
} tee_region9_high_reg_t;

/** Type of region10_low register
 *  Region10 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region10_low : R/W; bitpos: [31:12]; default: 0;
         *  Region10 address low.
         */
        uint32_t region10_low:20;
    };
    uint32_t val;
} tee_region10_low_reg_t;

/** Type of region10_high register
 *  Region10 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region10_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region10 address high.
         */
        uint32_t region10_high:20;
    };
    uint32_t val;
} tee_region10_high_reg_t;

/** Type of region11_low register
 *  Region11 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region11_low : R/W; bitpos: [31:12]; default: 0;
         *  Region11 address low.
         */
        uint32_t region11_low:20;
    };
    uint32_t val;
} tee_region11_low_reg_t;

/** Type of region11_high register
 *  Region11 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region11_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region11 address high.
         */
        uint32_t region11_high:20;
    };
    uint32_t val;
} tee_region11_high_reg_t;

/** Type of region12_low register
 *  Region12 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region12_low : R/W; bitpos: [31:12]; default: 0;
         *  Region12 address low.
         */
        uint32_t region12_low:20;
    };
    uint32_t val;
} tee_region12_low_reg_t;

/** Type of region12_high register
 *  Region12 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region12_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region12 address high.
         */
        uint32_t region12_high:20;
    };
    uint32_t val;
} tee_region12_high_reg_t;

/** Type of region13_low register
 *  Region13 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region13_low : R/W; bitpos: [31:12]; default: 0;
         *  Region13 address low.
         */
        uint32_t region13_low:20;
    };
    uint32_t val;
} tee_region13_low_reg_t;

/** Type of region13_high register
 *  Region13 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region13_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region13 address high.
         */
        uint32_t region13_high:20;
    };
    uint32_t val;
} tee_region13_high_reg_t;

/** Type of region14_low register
 *  Region14 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region14_low : R/W; bitpos: [31:12]; default: 0;
         *  Region14 address low.
         */
        uint32_t region14_low:20;
    };
    uint32_t val;
} tee_region14_low_reg_t;

/** Type of region14_high register
 *  Region14 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region14_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region14 address high.
         */
        uint32_t region14_high:20;
    };
    uint32_t val;
} tee_region14_high_reg_t;

/** Type of region15_low register
 *  Region15 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region15_low : R/W; bitpos: [31:12]; default: 0;
         *  Region15 address low.
         */
        uint32_t region15_low:20;
    };
    uint32_t val;
} tee_region15_low_reg_t;

/** Type of region15_high register
 *  Region15 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region15_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region15 address high.
         */
        uint32_t region15_high:20;
    };
    uint32_t val;
} tee_region15_high_reg_t;

/** Type of region16_low register
 *  Region16 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region16_low : R/W; bitpos: [31:12]; default: 0;
         *  Region16 address low.
         */
        uint32_t region16_low:20;
    };
    uint32_t val;
} tee_region16_low_reg_t;

/** Type of region16_high register
 *  Region16 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region16_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region16 address high.
         */
        uint32_t region16_high:20;
    };
    uint32_t val;
} tee_region16_high_reg_t;

/** Type of region17_low register
 *  Region17 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region17_low : R/W; bitpos: [31:12]; default: 0;
         *  Region17 address low.
         */
        uint32_t region17_low:20;
    };
    uint32_t val;
} tee_region17_low_reg_t;

/** Type of region17_high register
 *  Region17 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region17_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region17 address high.
         */
        uint32_t region17_high:20;
    };
    uint32_t val;
} tee_region17_high_reg_t;

/** Type of region18_low register
 *  Region18 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region18_low : R/W; bitpos: [31:12]; default: 0;
         *  Region18 address low.
         */
        uint32_t region18_low:20;
    };
    uint32_t val;
} tee_region18_low_reg_t;

/** Type of region18_high register
 *  Region18 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region18_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region18 address high.
         */
        uint32_t region18_high:20;
    };
    uint32_t val;
} tee_region18_high_reg_t;

/** Type of region19_low register
 *  Region19 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region19_low : R/W; bitpos: [31:12]; default: 0;
         *  Region19 address low.
         */
        uint32_t region19_low:20;
    };
    uint32_t val;
} tee_region19_low_reg_t;

/** Type of region19_high register
 *  Region19 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region19_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region19 address high.
         */
        uint32_t region19_high:20;
    };
    uint32_t val;
} tee_region19_high_reg_t;

/** Type of region20_low register
 *  Region20 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region20_low : R/W; bitpos: [31:12]; default: 0;
         *  Region20 address low.
         */
        uint32_t region20_low:20;
    };
    uint32_t val;
} tee_region20_low_reg_t;

/** Type of region20_high register
 *  Region20 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region20_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region20 address high.
         */
        uint32_t region20_high:20;
    };
    uint32_t val;
} tee_region20_high_reg_t;

/** Type of region21_low register
 *  Region21 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region21_low : R/W; bitpos: [31:12]; default: 0;
         *  Region21 address low.
         */
        uint32_t region21_low:20;
    };
    uint32_t val;
} tee_region21_low_reg_t;

/** Type of region21_high register
 *  Region21 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region21_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region21 address high.
         */
        uint32_t region21_high:20;
    };
    uint32_t val;
} tee_region21_high_reg_t;

/** Type of region22_low register
 *  Region22 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region22_low : R/W; bitpos: [31:12]; default: 0;
         *  Region22 address low.
         */
        uint32_t region22_low:20;
    };
    uint32_t val;
} tee_region22_low_reg_t;

/** Type of region22_high register
 *  Region22 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region22_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region22 address high.
         */
        uint32_t region22_high:20;
    };
    uint32_t val;
} tee_region22_high_reg_t;

/** Type of region23_low register
 *  Region23 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region23_low : R/W; bitpos: [31:12]; default: 0;
         *  Region23 address low.
         */
        uint32_t region23_low:20;
    };
    uint32_t val;
} tee_region23_low_reg_t;

/** Type of region23_high register
 *  Region23 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region23_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region23 address high.
         */
        uint32_t region23_high:20;
    };
    uint32_t val;
} tee_region23_high_reg_t;

/** Type of region24_low register
 *  Region24 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region24_low : R/W; bitpos: [31:12]; default: 0;
         *  Region24 address low.
         */
        uint32_t region24_low:20;
    };
    uint32_t val;
} tee_region24_low_reg_t;

/** Type of region24_high register
 *  Region24 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region24_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region24 address high.
         */
        uint32_t region24_high:20;
    };
    uint32_t val;
} tee_region24_high_reg_t;

/** Type of region25_low register
 *  Region25 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region25_low : R/W; bitpos: [31:12]; default: 0;
         *  Region25 address low.
         */
        uint32_t region25_low:20;
    };
    uint32_t val;
} tee_region25_low_reg_t;

/** Type of region25_high register
 *  Region25 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region25_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region25 address high.
         */
        uint32_t region25_high:20;
    };
    uint32_t val;
} tee_region25_high_reg_t;

/** Type of region26_low register
 *  Region26 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region26_low : R/W; bitpos: [31:12]; default: 0;
         *  Region26 address low.
         */
        uint32_t region26_low:20;
    };
    uint32_t val;
} tee_region26_low_reg_t;

/** Type of region26_high register
 *  Region26 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region26_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region26 address high.
         */
        uint32_t region26_high:20;
    };
    uint32_t val;
} tee_region26_high_reg_t;

/** Type of region27_low register
 *  Region27 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region27_low : R/W; bitpos: [31:12]; default: 0;
         *  Region27 address low.
         */
        uint32_t region27_low:20;
    };
    uint32_t val;
} tee_region27_low_reg_t;

/** Type of region27_high register
 *  Region27 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region27_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region27 address high.
         */
        uint32_t region27_high:20;
    };
    uint32_t val;
} tee_region27_high_reg_t;

/** Type of region28_low register
 *  Region28 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region28_low : R/W; bitpos: [31:12]; default: 0;
         *  Region28 address low.
         */
        uint32_t region28_low:20;
    };
    uint32_t val;
} tee_region28_low_reg_t;

/** Type of region28_high register
 *  Region28 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region28_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region28 address high.
         */
        uint32_t region28_high:20;
    };
    uint32_t val;
} tee_region28_high_reg_t;

/** Type of region29_low register
 *  Region29 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region29_low : R/W; bitpos: [31:12]; default: 0;
         *  Region29 address low.
         */
        uint32_t region29_low:20;
    };
    uint32_t val;
} tee_region29_low_reg_t;

/** Type of region29_high register
 *  Region29 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region29_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region29 address high.
         */
        uint32_t region29_high:20;
    };
    uint32_t val;
} tee_region29_high_reg_t;

/** Type of region30_low register
 *  Region30 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region30_low : R/W; bitpos: [31:12]; default: 0;
         *  Region30 address low.
         */
        uint32_t region30_low:20;
    };
    uint32_t val;
} tee_region30_low_reg_t;

/** Type of region30_high register
 *  Region30 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region30_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region30 address high.
         */
        uint32_t region30_high:20;
    };
    uint32_t val;
} tee_region30_high_reg_t;

/** Type of region31_low register
 *  Region31 address low register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region31_low : R/W; bitpos: [31:12]; default: 0;
         *  Region31 address low.
         */
        uint32_t region31_low:20;
    };
    uint32_t val;
} tee_region31_low_reg_t;

/** Type of region31_high register
 *  Region31 address high register.
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** region31_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Region31 address high.
         */
        uint32_t region31_high:20;
    };
    uint32_t val;
} tee_region31_high_reg_t;


/** Group: Tee permission control registers. */
/** Type of gmda_ch0_r_pms register
 *  GDMA ch0 read permission control registers.
 */
typedef union {
    struct {
        /** gdma_ch0_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  GDMA ch0 read permission control, each bit corresponds to a region.
         */
        uint32_t gdma_ch0_r_pms:32;
    };
    uint32_t val;
} tee_gmda_ch0_r_pms_reg_t;

/** Type of gmda_ch0_w_pms register
 *  GDMA ch0 write permission control registers.
 */
typedef union {
    struct {
        /** gdma_ch0_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  GDMA ch0 write permission control, each bit corresponds to a region.
         */
        uint32_t gdma_ch0_w_pms:32;
    };
    uint32_t val;
} tee_gmda_ch0_w_pms_reg_t;

/** Type of gmda_ch1_r_pms register
 *  GDMA ch1 read permission control registers.
 */
typedef union {
    struct {
        /** gdma_ch1_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  GDMA ch1 read permission control, each bit corresponds to a region.
         */
        uint32_t gdma_ch1_r_pms:32;
    };
    uint32_t val;
} tee_gmda_ch1_r_pms_reg_t;

/** Type of gmda_ch1_w_pms register
 *  GDMA ch1 write permission control registers.
 */
typedef union {
    struct {
        /** gdma_ch1_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  GDMA ch1 write permission control, each bit corresponds to a region.
         */
        uint32_t gdma_ch1_w_pms:32;
    };
    uint32_t val;
} tee_gmda_ch1_w_pms_reg_t;

/** Type of gmda_ch2_r_pms register
 *  GDMA ch2 read permission control registers.
 */
typedef union {
    struct {
        /** gdma_ch2_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  GDMA ch2 read permission control, each bit corresponds to a region.
         */
        uint32_t gdma_ch2_r_pms:32;
    };
    uint32_t val;
} tee_gmda_ch2_r_pms_reg_t;

/** Type of gmda_ch2_w_pms register
 *  GDMA ch2 write permission control registers.
 */
typedef union {
    struct {
        /** gdma_ch2_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  GDMA ch2 write permission control, each bit corresponds to a region.
         */
        uint32_t gdma_ch2_w_pms:32;
    };
    uint32_t val;
} tee_gmda_ch2_w_pms_reg_t;

/** Type of gmda_ch3_r_pms register
 *  GDMA ch3 read permission control registers.
 */
typedef union {
    struct {
        /** gdma_ch3_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  GDMA ch3 read permission control, each bit corresponds to a region.
         */
        uint32_t gdma_ch3_r_pms:32;
    };
    uint32_t val;
} tee_gmda_ch3_r_pms_reg_t;

/** Type of gmda_ch3_w_pms register
 *  GDMA ch3 write permission control registers.
 */
typedef union {
    struct {
        /** gdma_ch3_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  GDMA ch3 write permission control, each bit corresponds to a region.
         */
        uint32_t gdma_ch3_w_pms:32;
    };
    uint32_t val;
} tee_gmda_ch3_w_pms_reg_t;

/** Type of ahb_pdma_adc_r_pms register
 *  AHB PDMA adc read permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_adc_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA adc read permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_adc_r_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_adc_r_pms_reg_t;

/** Type of ahb_pdma_adc_w_pms register
 *  AHB PDMA adc write permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_adc_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA adc write permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_adc_w_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_adc_w_pms_reg_t;

/** Type of ahb_pdma_i2s0_r_pms register
 *  AHB PDMA i2s0 read permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_i2s0_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA i2s0 read permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_i2s0_r_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_i2s0_r_pms_reg_t;

/** Type of ahb_pdma_i2s0_w_pms register
 *  AHB PDMA i2s0 write permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_i2s0_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA i2s0 write permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_i2s0_w_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_i2s0_w_pms_reg_t;

/** Type of ahb_pdma_i2s1_r_pms register
 *  AHB PDMA i2s1 read permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_i2s1_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA i2s1 read permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_i2s1_r_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_i2s1_r_pms_reg_t;

/** Type of ahb_pdma_i2s1_w_pms register
 *  AHB PDMA i2s1 write permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_i2s1_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA i2s1 write permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_i2s1_w_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_i2s1_w_pms_reg_t;

/** Type of ahb_pdma_i2s2_r_pms register
 *  AHB PDMA i2s2 read permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_i2s2_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA i2s2 read permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_i2s2_r_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_i2s2_r_pms_reg_t;

/** Type of ahb_pdma_i2s2_w_pms register
 *  AHB PDMA i2s2 write permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_i2s2_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA i2s2 write permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_i2s2_w_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_i2s2_w_pms_reg_t;

/** Type of ahb_pdma_i3c_mst_r_pms register
 *  AHB PDMA i3s mst read permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_i3c_mst_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA i3c mst read permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_i3c_mst_r_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_i3c_mst_r_pms_reg_t;

/** Type of ahb_pdma_i3c_mst_w_pms register
 *  AHB PDMA i3c mst  write permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_i3c_mst_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA i3c mst  write permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_i3c_mst_w_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_i3c_mst_w_pms_reg_t;

/** Type of ahb_pdma_uhci0_r_pms register
 *  AHB PDMA uhci0 read permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_uhci0_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA uhci0 read permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_uhci0_r_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_uhci0_r_pms_reg_t;

/** Type of ahb_pdma_uhci0_w_pms register
 *  AHB PDMA uhci0 write permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_uhci0_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA uhci0 write permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_uhci0_w_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_uhci0_w_pms_reg_t;

/** Type of ahb_pdma_rmt_r_pms register
 *  AHB PDMA rmt read permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_rmt_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA rmt read permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_rmt_r_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_rmt_r_pms_reg_t;

/** Type of ahb_pdma_rmt_w_pms register
 *  AHB PDMA rmt write permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_rmt_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA rmt write permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_rmt_w_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_rmt_w_pms_reg_t;

/** Type of axi_pdma_lcdcam_r_pms register
 *  AXI PDMA lcdcam read permission control registers.
 */
typedef union {
    struct {
        /** axi_pdma_lcdcam_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AXI PDMA lcdcam read permission control, each bit corresponds to a region.
         */
        uint32_t axi_pdma_lcdcam_r_pms:32;
    };
    uint32_t val;
} tee_axi_pdma_lcdcam_r_pms_reg_t;

/** Type of axi_pdma_lcdcam_w_pms register
 *  AXI PDMA lcdcam write permission control registers.
 */
typedef union {
    struct {
        /** axi_pdma_lcdcam_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AXI PDMA lcdcam write permission control, each bit corresponds to a region.
         */
        uint32_t axi_pdma_lcdcam_w_pms:32;
    };
    uint32_t val;
} tee_axi_pdma_lcdcam_w_pms_reg_t;

/** Type of axi_pdma_gpspi2_r_pms register
 *  AXI PDMA gpspi2 read permission control registers.
 */
typedef union {
    struct {
        /** axi_pdma_gpspi2_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AXI PDMA gpspi2 read permission control, each bit corresponds to a region.
         */
        uint32_t axi_pdma_gpspi2_r_pms:32;
    };
    uint32_t val;
} tee_axi_pdma_gpspi2_r_pms_reg_t;

/** Type of axi_pdma_gpspi2_w_pms register
 *  AXI PDMA gpspi2 write permission control registers.
 */
typedef union {
    struct {
        /** axi_pdma_gpspi2_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AXI PDMA gpspi2 write permission control, each bit corresponds to a region.
         */
        uint32_t axi_pdma_gpspi2_w_pms:32;
    };
    uint32_t val;
} tee_axi_pdma_gpspi2_w_pms_reg_t;

/** Type of axi_pdma_gpspi3_r_pms register
 *  AXI PDMA gpspi3 read permission control registers.
 */
typedef union {
    struct {
        /** axi_pdma_gpspi3_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AXI PDMA gpspi3 read permission control, each bit corresponds to a region.
         */
        uint32_t axi_pdma_gpspi3_r_pms:32;
    };
    uint32_t val;
} tee_axi_pdma_gpspi3_r_pms_reg_t;

/** Type of axi_pdma_gpspi3_w_pms register
 *  AXI PDMA gpspi3 write permission control registers.
 */
typedef union {
    struct {
        /** axi_pdma_gpspi3_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AXI PDMA gpspi3 write permission control, each bit corresponds to a region.
         */
        uint32_t axi_pdma_gpspi3_w_pms:32;
    };
    uint32_t val;
} tee_axi_pdma_gpspi3_w_pms_reg_t;

/** Type of axi_pdma_parlio_r_pms register
 *  AXI PDMA parl io read permission control registers.
 */
typedef union {
    struct {
        /** axi_pdma_parlio_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AXI PDMA parl io read permission control, each bit corresponds to a region.
         */
        uint32_t axi_pdma_parlio_r_pms:32;
    };
    uint32_t val;
} tee_axi_pdma_parlio_r_pms_reg_t;

/** Type of axi_pdma_parlio_w_pms register
 *  AXI PDMA parl io write permission control registers.
 */
typedef union {
    struct {
        /** axi_pdma_parlio_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AXI PDMA parl io write permission control, each bit corresponds to a region.
         */
        uint32_t axi_pdma_parlio_w_pms:32;
    };
    uint32_t val;
} tee_axi_pdma_parlio_w_pms_reg_t;

/** Type of axi_pdma_aes_r_pms register
 *  AXI PDMA aes read permission control registers.
 */
typedef union {
    struct {
        /** axi_pdma_aes_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AXI PDMA aes read permission control, each bit corresponds to a region.
         */
        uint32_t axi_pdma_aes_r_pms:32;
    };
    uint32_t val;
} tee_axi_pdma_aes_r_pms_reg_t;

/** Type of axi_pdma_aes_w_pms register
 *  AXI PDMA aes write permission control registers.
 */
typedef union {
    struct {
        /** axi_pdma_aes_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AXI PDMA aes write permission control, each bit corresponds to a region.
         */
        uint32_t axi_pdma_aes_w_pms:32;
    };
    uint32_t val;
} tee_axi_pdma_aes_w_pms_reg_t;

/** Type of axi_pdma_sha_r_pms register
 *  AXI PDMA sha read permission control registers.
 */
typedef union {
    struct {
        /** axi_pdma_sha_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AXI PDMA sha read permission control, each bit corresponds to a region.
         */
        uint32_t axi_pdma_sha_r_pms:32;
    };
    uint32_t val;
} tee_axi_pdma_sha_r_pms_reg_t;

/** Type of axi_pdma_sha_w_pms register
 *  AXI PDMA sha write permission control registers.
 */
typedef union {
    struct {
        /** axi_pdma_sha_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AXI PDMA sha write permission control, each bit corresponds to a region.
         */
        uint32_t axi_pdma_sha_w_pms:32;
    };
    uint32_t val;
} tee_axi_pdma_sha_w_pms_reg_t;

/** Type of dma2d_jpeg_pms_r register
 *  DMA2D JPEG read permission control registers.
 */
typedef union {
    struct {
        /** dma2d_jpeg_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  DMA2D JPEG read permission control, each bit corresponds to a region.
         */
        uint32_t dma2d_jpeg_r_pms:32;
    };
    uint32_t val;
} tee_dma2d_jpeg_pms_r_reg_t;

/** Type of dma2d_jpeg_pms_w register
 *  DMA2D JPEG write permission control registers.
 */
typedef union {
    struct {
        /** dma2d_jpeg_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  DMA2D  JPEG write permission control, each bit corresponds to a region.
         */
        uint32_t dma2d_jpeg_w_pms:32;
    };
    uint32_t val;
} tee_dma2d_jpeg_pms_w_reg_t;

/** Type of usb_pms_r register
 *  USB read permission control registers.
 */
typedef union {
    struct {
        /** usb_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  USB read permission control, each bit corresponds to a region.
         */
        uint32_t usb_r_pms:32;
    };
    uint32_t val;
} tee_usb_pms_r_reg_t;

/** Type of usb_pms_w register
 *  USB write permission control registers.
 */
typedef union {
    struct {
        /** usb_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  USB write permission control, each bit corresponds to a region.
         */
        uint32_t usb_w_pms:32;
    };
    uint32_t val;
} tee_usb_pms_w_reg_t;

/** Type of gmac_pms_r register
 *  GMAC read permission control registers.
 */
typedef union {
    struct {
        /** gmac_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  GMAC read permission control, each bit corresponds to a region.
         */
        uint32_t gmac_r_pms:32;
    };
    uint32_t val;
} tee_gmac_pms_r_reg_t;

/** Type of gmac_pms_w register
 *  GMAC write permission control registers.
 */
typedef union {
    struct {
        /** gmac_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  GMAC write permission control, each bit corresponds to a region.
         */
        uint32_t gmac_w_pms:32;
    };
    uint32_t val;
} tee_gmac_pms_w_reg_t;

/** Type of sdmmc_pms_r register
 *  SDMMC read permission control registers.
 */
typedef union {
    struct {
        /** sdmmc_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  SDMMC read permission control, each bit corresponds to a region.
         */
        uint32_t sdmmc_r_pms:32;
    };
    uint32_t val;
} tee_sdmmc_pms_r_reg_t;

/** Type of sdmmc_pms_w register
 *  SDMMC write permission control registers.
 */
typedef union {
    struct {
        /** sdmmc_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  SDMMC write permission control, each bit corresponds to a region.
         */
        uint32_t sdmmc_w_pms:32;
    };
    uint32_t val;
} tee_sdmmc_pms_w_reg_t;

/** Type of usbotg11_pms_r register
 *  USBOTG11 read permission control registers.
 */
typedef union {
    struct {
        /** usbotg11_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  USBOTG11 read permission control, each bit corresponds to a region.
         */
        uint32_t usbotg11_r_pms:32;
    };
    uint32_t val;
} tee_usbotg11_pms_r_reg_t;

/** Type of usbotg11_pms_w register
 *  USBOTG11 write permission control registers.
 */
typedef union {
    struct {
        /** usbotg11_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  USBOTG11 write permission control, each bit corresponds to a region.
         */
        uint32_t usbotg11_w_pms:32;
    };
    uint32_t val;
} tee_usbotg11_pms_w_reg_t;

/** Type of trace0_pms_r register
 *  TRACE0 read permission control registers.
 */
typedef union {
    struct {
        /** trace0_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  TRACE0 read permission control, each bit corresponds to a region.
         */
        uint32_t trace0_r_pms:32;
    };
    uint32_t val;
} tee_trace0_pms_r_reg_t;

/** Type of trace0_pms_w register
 *  TRACE0 write permission control registers.
 */
typedef union {
    struct {
        /** trace0_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  TRACE0 write permission control, each bit corresponds to a region.
         */
        uint32_t trace0_w_pms:32;
    };
    uint32_t val;
} tee_trace0_pms_w_reg_t;

/** Type of trace1_pms_r register
 *  TRACE1 read permission control registers.
 */
typedef union {
    struct {
        /** trace1_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  TRACE1 read permission control, each bit corresponds to a region.
         */
        uint32_t trace1_r_pms:32;
    };
    uint32_t val;
} tee_trace1_pms_r_reg_t;

/** Type of trace1_pms_w register
 *  TRACE1 write permission control registers.
 */
typedef union {
    struct {
        /** trace1_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  TRACE1 write permission control, each bit corresponds to a region.
         */
        uint32_t trace1_w_pms:32;
    };
    uint32_t val;
} tee_trace1_pms_w_reg_t;

/** Type of l2mem_mon_pms_r register
 *  L2MEM MON read permission control registers.
 */
typedef union {
    struct {
        /** l2mem_mon_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  L2MEM MON read permission control, each bit corresponds to a region.
         */
        uint32_t l2mem_mon_r_pms:32;
    };
    uint32_t val;
} tee_l2mem_mon_pms_r_reg_t;

/** Type of l2mem_mon_pms_w register
 *  L2MEM MON write permission control registers.
 */
typedef union {
    struct {
        /** l2mem_mon_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  L2MEM MON write permission control, each bit corresponds to a region.
         */
        uint32_t l2mem_mon_w_pms:32;
    };
    uint32_t val;
} tee_l2mem_mon_pms_w_reg_t;

/** Type of tcm_mon_pms_r register
 *  TCM MON read permission control registers.
 */
typedef union {
    struct {
        /** tcm_mon_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  TCM MON read permission control, each bit corresponds to a region.
         */
        uint32_t tcm_mon_r_pms:32;
    };
    uint32_t val;
} tee_tcm_mon_pms_r_reg_t;

/** Type of tcm_mon_pms_w register
 *  TCM MON write permission control registers.
 */
typedef union {
    struct {
        /** tcm_mon_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  TCM MON write permission control, each bit corresponds to a region.
         */
        uint32_t tcm_mon_w_pms:32;
    };
    uint32_t val;
} tee_tcm_mon_pms_w_reg_t;

/** Type of regdma_pms_r register
 *  REGDMA read permission control registers.
 */
typedef union {
    struct {
        /** regdma_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  REGDMA read permission control, each bit corresponds to a region.
         */
        uint32_t regdma_r_pms:32;
    };
    uint32_t val;
} tee_regdma_pms_r_reg_t;

/** Type of regdma_pms_w register
 *  REGDMA write permission control registers.
 */
typedef union {
    struct {
        /** regdma_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  REGDMA write permission control, each bit corresponds to a region.
         */
        uint32_t regdma_w_pms:32;
    };
    uint32_t val;
} tee_regdma_pms_w_reg_t;

/** Type of h264_pms_r register
 *  H264 read permission control registers.
 */
typedef union {
    struct {
        /** h264_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  H264 read permission control, each bit corresponds to a region.
         */
        uint32_t h264_r_pms:32;
    };
    uint32_t val;
} tee_h264_pms_r_reg_t;

/** Type of h264_pms_w register
 *  H264 write permission control registers.
 */
typedef union {
    struct {
        /** h264_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  H264 write permission control, each bit corresponds to a region.
         */
        uint32_t h264_w_pms:32;
    };
    uint32_t val;
} tee_h264_pms_w_reg_t;

/** Type of dma2d_ppa_pms_r register
 *  DMA2D PPA read permission control registers.
 */
typedef union {
    struct {
        /** dma2d_ppa_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  DMA2D PPA  read permission control, each bit corresponds to a region.
         */
        uint32_t dma2d_ppa_r_pms:32;
    };
    uint32_t val;
} tee_dma2d_ppa_pms_r_reg_t;

/** Type of dma2d_ppa_pms_w register
 *  DMA2D PPA  write permission control registers.
 */
typedef union {
    struct {
        /** dma2d_ppa_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  DMA2D  PPA write permission control, each bit corresponds to a region.
         */
        uint32_t dma2d_ppa_w_pms:32;
    };
    uint32_t val;
} tee_dma2d_ppa_pms_w_reg_t;

/** Type of dma2d_dummy_pms_r register
 *  DMA2D dummy read permission control registers.
 */
typedef union {
    struct {
        /** dma2d_dummy_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  DMA2D dummy  read permission control, each bit corresponds to a region.
         */
        uint32_t dma2d_dummy_r_pms:32;
    };
    uint32_t val;
} tee_dma2d_dummy_pms_r_reg_t;

/** Type of dma2d_dummy_pms_w register
 *  DMA2D dummy  write permission control registers.
 */
typedef union {
    struct {
        /** dma2d_dummy_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  DMA2D dummy  write permission control, each bit corresponds to a region.
         */
        uint32_t dma2d_dummy_w_pms:32;
    };
    uint32_t val;
} tee_dma2d_dummy_pms_w_reg_t;

/** Type of ahb_pdma_dummy_r_pms register
 *  AHB PDMA dummy read permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_dummy_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA dummy read permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_dummy_r_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_dummy_r_pms_reg_t;

/** Type of ahb_pdma_dummy_w_pms register
 *  AHB PDMA dummy write permission control registers.
 */
typedef union {
    struct {
        /** ahb_pdma_dummy_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AHB PDMA dummy write permission control, each bit corresponds to a region.
         */
        uint32_t ahb_pdma_dummy_w_pms:32;
    };
    uint32_t val;
} tee_ahb_pdma_dummy_w_pms_reg_t;

/** Type of axi_pdma_dummy_r_pms register
 *  AXI PDMA dummy read permission control registers.
 */
typedef union {
    struct {
        /** axi_pdma_dummy_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AXI PDMA dummy read permission control, each bit corresponds to a region.
         */
        uint32_t axi_pdma_dummy_r_pms:32;
    };
    uint32_t val;
} tee_axi_pdma_dummy_r_pms_reg_t;

/** Type of axi_pdma_dummy_w_pms register
 *  AXI PDMA dummy write permission control registers.
 */
typedef union {
    struct {
        /** axi_pdma_dummy_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  AXI PDMA dummy write permission control, each bit corresponds to a region.
         */
        uint32_t axi_pdma_dummy_w_pms:32;
    };
    uint32_t val;
} tee_axi_pdma_dummy_w_pms_reg_t;


typedef struct {
    volatile tee_date_reg_t date;
    volatile tee_clk_en_reg_t clk_en;
    volatile tee_region0_low_reg_t region0_low;
    volatile tee_region0_high_reg_t region0_high;
    volatile tee_region1_low_reg_t region1_low;
    volatile tee_region1_high_reg_t region1_high;
    volatile tee_region2_low_reg_t region2_low;
    volatile tee_region2_high_reg_t region2_high;
    volatile tee_region3_low_reg_t region3_low;
    volatile tee_region3_high_reg_t region3_high;
    volatile tee_region4_low_reg_t region4_low;
    volatile tee_region4_high_reg_t region4_high;
    volatile tee_region5_low_reg_t region5_low;
    volatile tee_region5_high_reg_t region5_high;
    volatile tee_region6_low_reg_t region6_low;
    volatile tee_region6_high_reg_t region6_high;
    volatile tee_region7_low_reg_t region7_low;
    volatile tee_region7_high_reg_t region7_high;
    volatile tee_region8_low_reg_t region8_low;
    volatile tee_region8_high_reg_t region8_high;
    volatile tee_region9_low_reg_t region9_low;
    volatile tee_region9_high_reg_t region9_high;
    volatile tee_region10_low_reg_t region10_low;
    volatile tee_region10_high_reg_t region10_high;
    volatile tee_region11_low_reg_t region11_low;
    volatile tee_region11_high_reg_t region11_high;
    volatile tee_region12_low_reg_t region12_low;
    volatile tee_region12_high_reg_t region12_high;
    volatile tee_region13_low_reg_t region13_low;
    volatile tee_region13_high_reg_t region13_high;
    volatile tee_region14_low_reg_t region14_low;
    volatile tee_region14_high_reg_t region14_high;
    volatile tee_region15_low_reg_t region15_low;
    volatile tee_region15_high_reg_t region15_high;
    volatile tee_region16_low_reg_t region16_low;
    volatile tee_region16_high_reg_t region16_high;
    volatile tee_region17_low_reg_t region17_low;
    volatile tee_region17_high_reg_t region17_high;
    volatile tee_region18_low_reg_t region18_low;
    volatile tee_region18_high_reg_t region18_high;
    volatile tee_region19_low_reg_t region19_low;
    volatile tee_region19_high_reg_t region19_high;
    volatile tee_region20_low_reg_t region20_low;
    volatile tee_region20_high_reg_t region20_high;
    volatile tee_region21_low_reg_t region21_low;
    volatile tee_region21_high_reg_t region21_high;
    volatile tee_region22_low_reg_t region22_low;
    volatile tee_region22_high_reg_t region22_high;
    volatile tee_region23_low_reg_t region23_low;
    volatile tee_region23_high_reg_t region23_high;
    volatile tee_region24_low_reg_t region24_low;
    volatile tee_region24_high_reg_t region24_high;
    volatile tee_region25_low_reg_t region25_low;
    volatile tee_region25_high_reg_t region25_high;
    volatile tee_region26_low_reg_t region26_low;
    volatile tee_region26_high_reg_t region26_high;
    volatile tee_region27_low_reg_t region27_low;
    volatile tee_region27_high_reg_t region27_high;
    volatile tee_region28_low_reg_t region28_low;
    volatile tee_region28_high_reg_t region28_high;
    volatile tee_region29_low_reg_t region29_low;
    volatile tee_region29_high_reg_t region29_high;
    volatile tee_region30_low_reg_t region30_low;
    volatile tee_region30_high_reg_t region30_high;
    volatile tee_region31_low_reg_t region31_low;
    volatile tee_region31_high_reg_t region31_high;
    volatile tee_gmda_ch0_r_pms_reg_t gmda_ch0_r_pms;
    volatile tee_gmda_ch0_w_pms_reg_t gmda_ch0_w_pms;
    volatile tee_gmda_ch1_r_pms_reg_t gmda_ch1_r_pms;
    volatile tee_gmda_ch1_w_pms_reg_t gmda_ch1_w_pms;
    volatile tee_gmda_ch2_r_pms_reg_t gmda_ch2_r_pms;
    volatile tee_gmda_ch2_w_pms_reg_t gmda_ch2_w_pms;
    volatile tee_gmda_ch3_r_pms_reg_t gmda_ch3_r_pms;
    volatile tee_gmda_ch3_w_pms_reg_t gmda_ch3_w_pms;
    volatile tee_ahb_pdma_adc_r_pms_reg_t ahb_pdma_adc_r_pms;
    volatile tee_ahb_pdma_adc_w_pms_reg_t ahb_pdma_adc_w_pms;
    volatile tee_ahb_pdma_i2s0_r_pms_reg_t ahb_pdma_i2s0_r_pms;
    volatile tee_ahb_pdma_i2s0_w_pms_reg_t ahb_pdma_i2s0_w_pms;
    volatile tee_ahb_pdma_i2s1_r_pms_reg_t ahb_pdma_i2s1_r_pms;
    volatile tee_ahb_pdma_i2s1_w_pms_reg_t ahb_pdma_i2s1_w_pms;
    volatile tee_ahb_pdma_i2s2_r_pms_reg_t ahb_pdma_i2s2_r_pms;
    volatile tee_ahb_pdma_i2s2_w_pms_reg_t ahb_pdma_i2s2_w_pms;
    volatile tee_ahb_pdma_i3c_mst_r_pms_reg_t ahb_pdma_i3c_mst_r_pms;
    volatile tee_ahb_pdma_i3c_mst_w_pms_reg_t ahb_pdma_i3c_mst_w_pms;
    volatile tee_ahb_pdma_uhci0_r_pms_reg_t ahb_pdma_uhci0_r_pms;
    volatile tee_ahb_pdma_uhci0_w_pms_reg_t ahb_pdma_uhci0_w_pms;
    volatile tee_ahb_pdma_rmt_r_pms_reg_t ahb_pdma_rmt_r_pms;
    uint32_t reserved_15c[5];
    volatile tee_ahb_pdma_rmt_w_pms_reg_t ahb_pdma_rmt_w_pms;
    volatile tee_axi_pdma_lcdcam_r_pms_reg_t axi_pdma_lcdcam_r_pms;
    volatile tee_axi_pdma_lcdcam_w_pms_reg_t axi_pdma_lcdcam_w_pms;
    volatile tee_axi_pdma_gpspi2_r_pms_reg_t axi_pdma_gpspi2_r_pms;
    volatile tee_axi_pdma_gpspi2_w_pms_reg_t axi_pdma_gpspi2_w_pms;
    volatile tee_axi_pdma_gpspi3_r_pms_reg_t axi_pdma_gpspi3_r_pms;
    volatile tee_axi_pdma_gpspi3_w_pms_reg_t axi_pdma_gpspi3_w_pms;
    volatile tee_axi_pdma_parlio_r_pms_reg_t axi_pdma_parlio_r_pms;
    volatile tee_axi_pdma_parlio_w_pms_reg_t axi_pdma_parlio_w_pms;
    volatile tee_axi_pdma_aes_r_pms_reg_t axi_pdma_aes_r_pms;
    volatile tee_axi_pdma_aes_w_pms_reg_t axi_pdma_aes_w_pms;
    volatile tee_axi_pdma_sha_r_pms_reg_t axi_pdma_sha_r_pms;
    volatile tee_axi_pdma_sha_w_pms_reg_t axi_pdma_sha_w_pms;
    volatile tee_dma2d_jpeg_pms_r_reg_t dma2d_jpeg_pms_r;
    volatile tee_dma2d_jpeg_pms_w_reg_t dma2d_jpeg_pms_w;
    volatile tee_usb_pms_r_reg_t usb_pms_r;
    volatile tee_usb_pms_w_reg_t usb_pms_w;
    volatile tee_gmac_pms_r_reg_t gmac_pms_r;
    volatile tee_gmac_pms_w_reg_t gmac_pms_w;
    volatile tee_sdmmc_pms_r_reg_t sdmmc_pms_r;
    volatile tee_sdmmc_pms_w_reg_t sdmmc_pms_w;
    volatile tee_usbotg11_pms_r_reg_t usbotg11_pms_r;
    volatile tee_usbotg11_pms_w_reg_t usbotg11_pms_w;
    volatile tee_trace0_pms_r_reg_t trace0_pms_r;
    volatile tee_trace0_pms_w_reg_t trace0_pms_w;
    volatile tee_trace1_pms_r_reg_t trace1_pms_r;
    volatile tee_trace1_pms_w_reg_t trace1_pms_w;
    volatile tee_l2mem_mon_pms_r_reg_t l2mem_mon_pms_r;
    volatile tee_l2mem_mon_pms_w_reg_t l2mem_mon_pms_w;
    volatile tee_tcm_mon_pms_r_reg_t tcm_mon_pms_r;
    volatile tee_tcm_mon_pms_w_reg_t tcm_mon_pms_w;
    volatile tee_regdma_pms_r_reg_t regdma_pms_r;
    volatile tee_regdma_pms_w_reg_t regdma_pms_w;
    uint32_t reserved_1f4[2];
    volatile tee_h264_pms_r_reg_t h264_pms_r;
    volatile tee_h264_pms_w_reg_t h264_pms_w;
    volatile tee_dma2d_ppa_pms_r_reg_t dma2d_ppa_pms_r;
    volatile tee_dma2d_ppa_pms_w_reg_t dma2d_ppa_pms_w;
    volatile tee_dma2d_dummy_pms_r_reg_t dma2d_dummy_pms_r;
    volatile tee_dma2d_dummy_pms_w_reg_t dma2d_dummy_pms_w;
    volatile tee_ahb_pdma_dummy_r_pms_reg_t ahb_pdma_dummy_r_pms;
    volatile tee_ahb_pdma_dummy_w_pms_reg_t ahb_pdma_dummy_w_pms;
    volatile tee_axi_pdma_dummy_r_pms_reg_t axi_pdma_dummy_r_pms;
    volatile tee_axi_pdma_dummy_w_pms_reg_t axi_pdma_dummy_w_pms;
} tee_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(tee_dev_t) == 0x224, "Invalid size of tee_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
