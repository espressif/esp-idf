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

/** Group: Configuration Register */
/** Type of conf register
 *  CFG reg
 */
typedef union {
    struct {
        /** noise_source_sel : R/W; bitpos: [4:0]; default: 0;
         *  sel noise source
         */
        uint32_t noise_source_sel:5;
        /** noise_pos_sel : R/W; bitpos: [9:5]; default: 0;
         *  sel noise pos
         */
        uint32_t noise_pos_sel:5;
        /** repetition_value_c : R/W; bitpos: [18:10]; default: 0;
         *  parameter
         */
        uint32_t repetition_value_c:9;
        /** adpative_value_c : R/W; bitpos: [27:19]; default: 0;
         *  parameter
         */
        uint32_t adpative_value_c:9;
        /** fifo_reset : WT; bitpos: [28]; default: 0;
         *  fifo_reset
         */
        uint32_t fifo_reset:1;
        /** random_output_mode : R/W; bitpos: [29]; default: 0;
         *  standard and nonstandart output
         */
        uint32_t random_output_mode:1;
        /** noise_crc_en : R/W; bitpos: [30]; default: 1;
         *  standard and nonstandart output
         */
        uint32_t noise_crc_en:1;
        /** sample_enable : R/W; bitpos: [31]; default: 0;
         *  1: enable rng sample loop
         */
        uint32_t sample_enable:1;
    };
    uint32_t val;
} trng_conf_reg_t;

/** Type of debug_conf register
 *  reserved
 */
typedef union {
    struct {
        /** startup_test_start : WT; bitpos: [0]; default: 0;
         *  startup start
         */
        uint32_t startup_test_start:1;
        /** startup_test_limit : R/W; bitpos: [16:1]; default: 1024;
         *  startup limit
         */
        uint32_t startup_test_limit:16;
        /** debug_data : RO; bitpos: [24:17]; default: 0;
         *  debug data
         */
        uint32_t debug_data:8;
        /** debug_mode : R/W; bitpos: [25]; default: 0;
         *  1 for debug mode 0 for nomarl mode
         */
        uint32_t debug_mode:1;
        /** health_test_error_code : RO; bitpos: [28:26]; default: 0;
         *  error code
         */
        uint32_t health_test_error_code:3;
        /** health_test_end : WT; bitpos: [29]; default: 0;
         *  health test end
         */
        uint32_t health_test_end:1;
        /** health_test_bypass : R/W; bitpos: [30]; default: 0;
         *  health test bypass
         */
        uint32_t health_test_bypass:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} trng_debug_conf_reg_t;

/** Type of condition_key0 register
 *  condition key reg
 */
typedef union {
    struct {
        /** condition_key0 : R/W; bitpos: [31:0]; default: 0;
         *  condition key
         */
        uint32_t condition_key0:32;
    };
    uint32_t val;
} trng_condition_key0_reg_t;

/** Type of condition_key1 register
 *  condition key reg
 */
typedef union {
    struct {
        /** condition_key1 : R/W; bitpos: [31:0]; default: 0;
         *  condition key
         */
        uint32_t condition_key1:32;
    };
    uint32_t val;
} trng_condition_key1_reg_t;

/** Type of condition_key2 register
 *  condition key reg
 */
typedef union {
    struct {
        /** condition_key2 : R/W; bitpos: [31:0]; default: 0;
         *  condition key
         */
        uint32_t condition_key2:32;
    };
    uint32_t val;
} trng_condition_key2_reg_t;

/** Type of condition_key3 register
 *  condition key reg
 */
typedef union {
    struct {
        /** condition_key3 : R/W; bitpos: [31:0]; default: 0;
         *  condition key
         */
        uint32_t condition_key3:32;
    };
    uint32_t val;
} trng_condition_key3_reg_t;

/** Type of condition_key4 register
 *  condition key reg
 */
typedef union {
    struct {
        /** condition_key4 : R/W; bitpos: [31:0]; default: 0;
         *  condition key
         */
        uint32_t condition_key4:32;
    };
    uint32_t val;
} trng_condition_key4_reg_t;

/** Type of condition_key5 register
 *  condition key reg
 */
typedef union {
    struct {
        /** condition_key5 : R/W; bitpos: [31:0]; default: 0;
         *  condition key
         */
        uint32_t condition_key5:32;
    };
    uint32_t val;
} trng_condition_key5_reg_t;

/** Type of condition_key6 register
 *  condition key reg
 */
typedef union {
    struct {
        /** condition_key6 : R/W; bitpos: [31:0]; default: 0;
         *  condition key
         */
        uint32_t condition_key6:32;
    };
    uint32_t val;
} trng_condition_key6_reg_t;

/** Type of condition_key7 register
 *  condition key reg
 */
typedef union {
    struct {
        /** condition_key7 : R/W; bitpos: [31:0]; default: 0;
         *  condition key
         */
        uint32_t condition_key7:32;
    };
    uint32_t val;
} trng_condition_key7_reg_t;

/** Type of int_ena register
 *  Read only register for error and done
 */
typedef union {
    struct {
        /** error_int_ena : R/W; bitpos: [0]; default: 0;
         *  error flag
         */
        uint32_t error_int_ena:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} trng_int_ena_reg_t;

/** Type of int_raw register
 *  Read only register for error and done
 */
typedef union {
    struct {
        /** error_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  error flag
         */
        uint32_t error_int_raw:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} trng_int_raw_reg_t;

/** Type of int_clr register
 *  Read only register for error and done
 */
typedef union {
    struct {
        /** error_int_clr : WT; bitpos: [0]; default: 0;
         *  error flag
         */
        uint32_t error_int_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} trng_int_clr_reg_t;

/** Type of int_st register
 *  Read only register for error and done
 */
typedef union {
    struct {
        /** error_int_st : RO; bitpos: [0]; default: 0;
         *  error flag
         */
        uint32_t error_int_st:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} trng_int_st_reg_t;

/** Type of arbit_cfg register
 *  arbit cfg
 */
typedef union {
    struct {
        /** arbit_priority_0 : R/W; bitpos: [3:0]; default: 0;
         *  reg_arbit_priority_0
         */
        uint32_t arbit_priority_0:4;
        /** arbit_priority_1 : R/W; bitpos: [7:4]; default: 0;
         *  reg_arbit_priority_1
         */
        uint32_t arbit_priority_1:4;
        /** arbit_priority_2 : R/W; bitpos: [11:8]; default: 0;
         *  reg_arbit_priority_2
         */
        uint32_t arbit_priority_2:4;
        /** arbit_priority_3 : R/W; bitpos: [15:12]; default: 0;
         *  reg_arbit_priority_3
         */
        uint32_t arbit_priority_3:4;
        /** arbit_en : R/W; bitpos: [16]; default: 0;
         *  reg_arbit_en
         */
        uint32_t arbit_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} trng_arbit_cfg_reg_t;

/** Type of sw_read register
 *  sw read random reg
 */
typedef union {
    struct {
        /** sw_random_data : RO; bitpos: [31:0]; default: 0;
         *  sw read random reg
         */
        uint32_t sw_random_data:32;
    };
    uint32_t val;
} trng_sw_read_reg_t;

/** Type of sw_read_st register
 *  sw read st reg
 */
typedef union {
    struct {
        /** sw_data_valid : RO; bitpos: [0]; default: 0;
         *  sw random data st
         */
        uint32_t sw_data_valid:1;
        /** sw_random_req : WT; bitpos: [1]; default: 0;
         *  sw read random req
         */
        uint32_t sw_random_req:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} trng_sw_read_st_reg_t;

/** Type of crc_data register
 *  sw read data
 */
typedef union {
    struct {
        /** sw_crc_random_data : RO; bitpos: [31:0]; default: 0;
         *  sw read crc random data
         */
        uint32_t sw_crc_random_data:32;
    };
    uint32_t val;
} trng_crc_data_reg_t;

/** Type of crc_sync_data register
 *  sw read data sync
 */
typedef union {
    struct {
        /** sw_crc_random_data_sync : RO; bitpos: [31:0]; default: 0;
         *  sw read crc random sync data
         */
        uint32_t sw_crc_random_data_sync:32;
    };
    uint32_t val;
} trng_crc_sync_data_reg_t;

/** Type of date register
 *  Date register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 2426672;
         *  TRNG date information
         */
        uint32_t date:28;
        /** clk_en : R/W; bitpos: [28]; default: 0;
         *  clock enable
         */
        uint32_t clk_en:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} trng_date_reg_t;


typedef struct {
    volatile trng_conf_reg_t conf;
    volatile trng_debug_conf_reg_t debug_conf;
    volatile trng_condition_key0_reg_t condition_key0;
    volatile trng_condition_key1_reg_t condition_key1;
    volatile trng_condition_key2_reg_t condition_key2;
    volatile trng_condition_key3_reg_t condition_key3;
    volatile trng_condition_key4_reg_t condition_key4;
    volatile trng_condition_key5_reg_t condition_key5;
    volatile trng_condition_key6_reg_t condition_key6;
    volatile trng_condition_key7_reg_t condition_key7;
    volatile trng_int_ena_reg_t int_ena;
    volatile trng_int_raw_reg_t int_raw;
    volatile trng_int_clr_reg_t int_clr;
    volatile trng_int_st_reg_t int_st;
    volatile trng_arbit_cfg_reg_t arbit_cfg;
    volatile trng_sw_read_reg_t sw_read;
    volatile trng_sw_read_st_reg_t sw_read_st;
    volatile trng_crc_data_reg_t crc_data;
    volatile trng_crc_sync_data_reg_t crc_sync_data;
    uint32_t reserved_04c[44];
    volatile trng_date_reg_t date;
} trng_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(trng_dev_t) == 0x100, "Invalid size of trng_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
