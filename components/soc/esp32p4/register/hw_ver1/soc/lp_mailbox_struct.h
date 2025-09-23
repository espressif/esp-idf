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

/** Group: configure_register */
/** Type of massege_0 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_0 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_0:32;
    };
    uint32_t val;
} mb_massege_0_reg_t;

/** Type of massege_1 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_1 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_1:32;
    };
    uint32_t val;
} mb_massege_1_reg_t;

/** Type of massege_2 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_2 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_2:32;
    };
    uint32_t val;
} mb_massege_2_reg_t;

/** Type of massege_3 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_3 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_3:32;
    };
    uint32_t val;
} mb_massege_3_reg_t;

/** Type of massege_4 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_4 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_4:32;
    };
    uint32_t val;
} mb_massege_4_reg_t;

/** Type of massege_5 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_5 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_5:32;
    };
    uint32_t val;
} mb_massege_5_reg_t;

/** Type of massege_6 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_6 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_6:32;
    };
    uint32_t val;
} mb_massege_6_reg_t;

/** Type of massege_7 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_7 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_7:32;
    };
    uint32_t val;
} mb_massege_7_reg_t;

/** Type of massege_8 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_8 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_8:32;
    };
    uint32_t val;
} mb_massege_8_reg_t;

/** Type of massege_9 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_9 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_9:32;
    };
    uint32_t val;
} mb_massege_9_reg_t;

/** Type of massege_10 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_10 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_10:32;
    };
    uint32_t val;
} mb_massege_10_reg_t;

/** Type of massege_11 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_11 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_11:32;
    };
    uint32_t val;
} mb_massege_11_reg_t;

/** Type of massege_12 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_12 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_12:32;
    };
    uint32_t val;
} mb_massege_12_reg_t;

/** Type of massege_13 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_13 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_13:32;
    };
    uint32_t val;
} mb_massege_13_reg_t;

/** Type of massege_14 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_14 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_14:32;
    };
    uint32_t val;
} mb_massege_14_reg_t;

/** Type of massege_15 register
 *  need_des
 */
typedef union {
    struct {
        /** massege_15 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t massege_15:32;
    };
    uint32_t val;
} mb_massege_15_reg_t;

/** Type of reg_clk_en register
 *  need_des
 */
typedef union {
    struct {
        /** reg_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} mb_reg_clk_en_reg_t;


/** Group: Interrupt Registers */
/** Type of lp_int_raw register
 *  need_des
 */
typedef union {
    struct {
        /** lp_0_int_raw : RO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t lp_0_int_raw:1;
        /** lp_1_int_raw : RO; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t lp_1_int_raw:1;
        /** lp_2_int_raw : RO; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t lp_2_int_raw:1;
        /** lp_3_int_raw : RO; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t lp_3_int_raw:1;
        /** lp_4_int_raw : RO; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t lp_4_int_raw:1;
        /** lp_5_int_raw : RO; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t lp_5_int_raw:1;
        /** lp_6_int_raw : RO; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t lp_6_int_raw:1;
        /** lp_7_int_raw : RO; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t lp_7_int_raw:1;
        /** lp_8_int_raw : RO; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t lp_8_int_raw:1;
        /** lp_9_int_raw : RO; bitpos: [9]; default: 0;
         *  need_des
         */
        uint32_t lp_9_int_raw:1;
        /** lp_10_int_raw : RO; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t lp_10_int_raw:1;
        /** lp_11_int_raw : RO; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t lp_11_int_raw:1;
        /** lp_12_int_raw : RO; bitpos: [12]; default: 0;
         *  need_des
         */
        uint32_t lp_12_int_raw:1;
        /** lp_13_int_raw : RO; bitpos: [13]; default: 0;
         *  need_des
         */
        uint32_t lp_13_int_raw:1;
        /** lp_14_int_raw : RO; bitpos: [14]; default: 0;
         *  need_des
         */
        uint32_t lp_14_int_raw:1;
        /** lp_15_int_raw : RO; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t lp_15_int_raw:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mb_lp_int_raw_reg_t;

/** Type of lp_int_st register
 *  need_des
 */
typedef union {
    struct {
        /** lp_0_int_st : RO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t lp_0_int_st:1;
        /** lp_1_int_st : RO; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t lp_1_int_st:1;
        /** lp_2_int_st : RO; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t lp_2_int_st:1;
        /** lp_3_int_st : RO; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t lp_3_int_st:1;
        /** lp_4_int_st : RO; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t lp_4_int_st:1;
        /** lp_5_int_st : RO; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t lp_5_int_st:1;
        /** lp_6_int_st : RO; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t lp_6_int_st:1;
        /** lp_7_int_st : RO; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t lp_7_int_st:1;
        /** lp_8_int_st : RO; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t lp_8_int_st:1;
        /** lp_9_int_st : RO; bitpos: [9]; default: 0;
         *  need_des
         */
        uint32_t lp_9_int_st:1;
        /** lp_10_int_st : RO; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t lp_10_int_st:1;
        /** lp_11_int_st : RO; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t lp_11_int_st:1;
        /** lp_12_int_st : RO; bitpos: [12]; default: 0;
         *  need_des
         */
        uint32_t lp_12_int_st:1;
        /** lp_13_int_st : RO; bitpos: [13]; default: 0;
         *  need_des
         */
        uint32_t lp_13_int_st:1;
        /** lp_14_int_st : RO; bitpos: [14]; default: 0;
         *  need_des
         */
        uint32_t lp_14_int_st:1;
        /** lp_15_int_st : RO; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t lp_15_int_st:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mb_lp_int_st_reg_t;

/** Type of lp_int_ena register
 *  need_des
 */
typedef union {
    struct {
        /** lp_0_int_ena : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t lp_0_int_ena:1;
        /** lp_1_int_ena : R/W; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t lp_1_int_ena:1;
        /** lp_2_int_ena : R/W; bitpos: [2]; default: 1;
         *  need_des
         */
        uint32_t lp_2_int_ena:1;
        /** lp_3_int_ena : R/W; bitpos: [3]; default: 1;
         *  need_des
         */
        uint32_t lp_3_int_ena:1;
        /** lp_4_int_ena : R/W; bitpos: [4]; default: 1;
         *  need_des
         */
        uint32_t lp_4_int_ena:1;
        /** lp_5_int_ena : R/W; bitpos: [5]; default: 1;
         *  need_des
         */
        uint32_t lp_5_int_ena:1;
        /** lp_6_int_ena : R/W; bitpos: [6]; default: 1;
         *  need_des
         */
        uint32_t lp_6_int_ena:1;
        /** lp_7_int_ena : R/W; bitpos: [7]; default: 1;
         *  need_des
         */
        uint32_t lp_7_int_ena:1;
        /** lp_8_int_ena : R/W; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t lp_8_int_ena:1;
        /** lp_9_int_ena : R/W; bitpos: [9]; default: 0;
         *  need_des
         */
        uint32_t lp_9_int_ena:1;
        /** lp_10_int_ena : R/W; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t lp_10_int_ena:1;
        /** lp_11_int_ena : R/W; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t lp_11_int_ena:1;
        /** lp_12_int_ena : R/W; bitpos: [12]; default: 0;
         *  need_des
         */
        uint32_t lp_12_int_ena:1;
        /** lp_13_int_ena : R/W; bitpos: [13]; default: 0;
         *  need_des
         */
        uint32_t lp_13_int_ena:1;
        /** lp_14_int_ena : R/W; bitpos: [14]; default: 0;
         *  need_des
         */
        uint32_t lp_14_int_ena:1;
        /** lp_15_int_ena : R/W; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t lp_15_int_ena:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mb_lp_int_ena_reg_t;

/** Type of lp_int_clr register
 *  need_des
 */
typedef union {
    struct {
        /** lp_0_int_clr : WO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t lp_0_int_clr:1;
        /** lp_1_int_clr : WO; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t lp_1_int_clr:1;
        /** lp_2_int_clr : WO; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t lp_2_int_clr:1;
        /** lp_3_int_clr : WO; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t lp_3_int_clr:1;
        /** lp_4_int_clr : WO; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t lp_4_int_clr:1;
        /** lp_5_int_clr : WO; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t lp_5_int_clr:1;
        /** lp_6_int_clr : WO; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t lp_6_int_clr:1;
        /** lp_7_int_clr : WO; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t lp_7_int_clr:1;
        /** lp_8_int_clr : WO; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t lp_8_int_clr:1;
        /** lp_9_int_clr : WO; bitpos: [9]; default: 0;
         *  need_des
         */
        uint32_t lp_9_int_clr:1;
        /** lp_10_int_clr : WO; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t lp_10_int_clr:1;
        /** lp_11_int_clr : WO; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t lp_11_int_clr:1;
        /** lp_12_int_clr : WO; bitpos: [12]; default: 0;
         *  need_des
         */
        uint32_t lp_12_int_clr:1;
        /** lp_13_int_clr : WO; bitpos: [13]; default: 0;
         *  need_des
         */
        uint32_t lp_13_int_clr:1;
        /** lp_14_int_clr : WO; bitpos: [14]; default: 0;
         *  need_des
         */
        uint32_t lp_14_int_clr:1;
        /** lp_15_int_clr : WO; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t lp_15_int_clr:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mb_lp_int_clr_reg_t;

/** Type of hp_int_raw register
 *  need_des
 */
typedef union {
    struct {
        /** hp_0_int_raw : RO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t hp_0_int_raw:1;
        /** hp_1_int_raw : RO; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t hp_1_int_raw:1;
        /** hp_2_int_raw : RO; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t hp_2_int_raw:1;
        /** hp_3_int_raw : RO; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t hp_3_int_raw:1;
        /** hp_4_int_raw : RO; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t hp_4_int_raw:1;
        /** hp_5_int_raw : RO; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t hp_5_int_raw:1;
        /** hp_6_int_raw : RO; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t hp_6_int_raw:1;
        /** hp_7_int_raw : RO; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t hp_7_int_raw:1;
        /** hp_8_int_raw : RO; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t hp_8_int_raw:1;
        /** hp_9_int_raw : RO; bitpos: [9]; default: 0;
         *  need_des
         */
        uint32_t hp_9_int_raw:1;
        /** hp_10_int_raw : RO; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t hp_10_int_raw:1;
        /** hp_11_int_raw : RO; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t hp_11_int_raw:1;
        /** hp_12_int_raw : RO; bitpos: [12]; default: 0;
         *  need_des
         */
        uint32_t hp_12_int_raw:1;
        /** hp_13_int_raw : RO; bitpos: [13]; default: 0;
         *  need_des
         */
        uint32_t hp_13_int_raw:1;
        /** hp_14_int_raw : RO; bitpos: [14]; default: 0;
         *  need_des
         */
        uint32_t hp_14_int_raw:1;
        /** hp_15_int_raw : RO; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t hp_15_int_raw:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mb_hp_int_raw_reg_t;

/** Type of hp_int_st register
 *  need_des
 */
typedef union {
    struct {
        /** hp_0_int_st : RO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t hp_0_int_st:1;
        /** hp_1_int_st : RO; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t hp_1_int_st:1;
        /** hp_2_int_st : RO; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t hp_2_int_st:1;
        /** hp_3_int_st : RO; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t hp_3_int_st:1;
        /** hp_4_int_st : RO; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t hp_4_int_st:1;
        /** hp_5_int_st : RO; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t hp_5_int_st:1;
        /** hp_6_int_st : RO; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t hp_6_int_st:1;
        /** hp_7_int_st : RO; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t hp_7_int_st:1;
        /** hp_8_int_st : RO; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t hp_8_int_st:1;
        /** hp_9_int_st : RO; bitpos: [9]; default: 0;
         *  need_des
         */
        uint32_t hp_9_int_st:1;
        /** hp_10_int_st : RO; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t hp_10_int_st:1;
        /** hp_11_int_st : RO; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t hp_11_int_st:1;
        /** hp_12_int_st : RO; bitpos: [12]; default: 0;
         *  need_des
         */
        uint32_t hp_12_int_st:1;
        /** hp_13_int_st : RO; bitpos: [13]; default: 0;
         *  need_des
         */
        uint32_t hp_13_int_st:1;
        /** hp_14_int_st : RO; bitpos: [14]; default: 0;
         *  need_des
         */
        uint32_t hp_14_int_st:1;
        /** hp_15_int_st : RO; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t hp_15_int_st:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mb_hp_int_st_reg_t;

/** Type of hp_int_ena register
 *  need_des
 */
typedef union {
    struct {
        /** hp_0_int_ena : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t hp_0_int_ena:1;
        /** hp_1_int_ena : R/W; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t hp_1_int_ena:1;
        /** hp_2_int_ena : R/W; bitpos: [2]; default: 1;
         *  need_des
         */
        uint32_t hp_2_int_ena:1;
        /** hp_3_int_ena : R/W; bitpos: [3]; default: 1;
         *  need_des
         */
        uint32_t hp_3_int_ena:1;
        /** hp_4_int_ena : R/W; bitpos: [4]; default: 1;
         *  need_des
         */
        uint32_t hp_4_int_ena:1;
        /** hp_5_int_ena : R/W; bitpos: [5]; default: 1;
         *  need_des
         */
        uint32_t hp_5_int_ena:1;
        /** hp_6_int_ena : R/W; bitpos: [6]; default: 1;
         *  need_des
         */
        uint32_t hp_6_int_ena:1;
        /** hp_7_int_ena : R/W; bitpos: [7]; default: 1;
         *  need_des
         */
        uint32_t hp_7_int_ena:1;
        /** hp_8_int_ena : R/W; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t hp_8_int_ena:1;
        /** hp_9_int_ena : R/W; bitpos: [9]; default: 0;
         *  need_des
         */
        uint32_t hp_9_int_ena:1;
        /** hp_10_int_ena : R/W; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t hp_10_int_ena:1;
        /** hp_11_int_ena : R/W; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t hp_11_int_ena:1;
        /** hp_12_int_ena : R/W; bitpos: [12]; default: 0;
         *  need_des
         */
        uint32_t hp_12_int_ena:1;
        /** hp_13_int_ena : R/W; bitpos: [13]; default: 0;
         *  need_des
         */
        uint32_t hp_13_int_ena:1;
        /** hp_14_int_ena : R/W; bitpos: [14]; default: 0;
         *  need_des
         */
        uint32_t hp_14_int_ena:1;
        /** hp_15_int_ena : R/W; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t hp_15_int_ena:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mb_hp_int_ena_reg_t;

/** Type of hp_int_clr register
 *  need_des
 */
typedef union {
    struct {
        /** hp_0_int_clr : WO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t hp_0_int_clr:1;
        /** hp_1_int_clr : WO; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t hp_1_int_clr:1;
        /** hp_2_int_clr : WO; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t hp_2_int_clr:1;
        /** hp_3_int_clr : WO; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t hp_3_int_clr:1;
        /** hp_4_int_clr : WO; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t hp_4_int_clr:1;
        /** hp_5_int_clr : WO; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t hp_5_int_clr:1;
        /** hp_6_int_clr : WO; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t hp_6_int_clr:1;
        /** hp_7_int_clr : WO; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t hp_7_int_clr:1;
        /** hp_8_int_clr : WO; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t hp_8_int_clr:1;
        /** hp_9_int_clr : WO; bitpos: [9]; default: 0;
         *  need_des
         */
        uint32_t hp_9_int_clr:1;
        /** hp_10_int_clr : WO; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t hp_10_int_clr:1;
        /** hp_11_int_clr : WO; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t hp_11_int_clr:1;
        /** hp_12_int_clr : WO; bitpos: [12]; default: 0;
         *  need_des
         */
        uint32_t hp_12_int_clr:1;
        /** hp_13_int_clr : WO; bitpos: [13]; default: 0;
         *  need_des
         */
        uint32_t hp_13_int_clr:1;
        /** hp_14_int_clr : WO; bitpos: [14]; default: 0;
         *  need_des
         */
        uint32_t hp_14_int_clr:1;
        /** hp_15_int_clr : WO; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t hp_15_int_clr:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mb_hp_int_clr_reg_t;


typedef struct {
    volatile mb_massege_0_reg_t massege_0;
    volatile mb_massege_1_reg_t massege_1;
    volatile mb_massege_2_reg_t massege_2;
    volatile mb_massege_3_reg_t massege_3;
    volatile mb_massege_4_reg_t massege_4;
    volatile mb_massege_5_reg_t massege_5;
    volatile mb_massege_6_reg_t massege_6;
    volatile mb_massege_7_reg_t massege_7;
    volatile mb_massege_8_reg_t massege_8;
    volatile mb_massege_9_reg_t massege_9;
    volatile mb_massege_10_reg_t massege_10;
    volatile mb_massege_11_reg_t massege_11;
    volatile mb_massege_12_reg_t massege_12;
    volatile mb_massege_13_reg_t massege_13;
    volatile mb_massege_14_reg_t massege_14;
    volatile mb_massege_15_reg_t massege_15;
    volatile mb_lp_int_raw_reg_t lp_int_raw;
    volatile mb_lp_int_st_reg_t lp_int_st;
    volatile mb_lp_int_ena_reg_t lp_int_ena;
    volatile mb_lp_int_clr_reg_t lp_int_clr;
    volatile mb_hp_int_raw_reg_t hp_int_raw;
    volatile mb_hp_int_st_reg_t hp_int_st;
    volatile mb_hp_int_ena_reg_t hp_int_ena;
    volatile mb_hp_int_clr_reg_t hp_int_clr;
    volatile mb_reg_clk_en_reg_t reg_clk_en;
} mb_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(mb_dev_t) == 0x64, "Invalid size of mb_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
