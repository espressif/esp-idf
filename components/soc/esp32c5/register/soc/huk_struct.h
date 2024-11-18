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

/** Group: Memory data */

/** Group: Clock gate register */
/** Type of clk register
 *  HUK Generator clock gate control register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Write 1 to force on register clock gate.
         */
        uint32_t clk_en:1;
        /** mem_cg_force_on : R/W; bitpos: [1]; default: 0;
         *  Write 1 to force on memory clock gate.
         */
        uint32_t mem_cg_force_on:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} huk_clk_reg_t;


/** Group: Interrupt registers */
/** Type of int_raw register
 *  HUK Generator interrupt raw register, valid in level.
 */
typedef union {
    struct {
        /** prep_done_int_raw : RO/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status bit  for the huk_prep_done_int interrupt
         */
        uint32_t prep_done_int_raw:1;
        /** proc_done_int_raw : RO/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status bit  for the huk_proc_done_int interrupt
         */
        uint32_t proc_done_int_raw:1;
        /** post_done_int_raw : RO/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status bit  for the huk_post_done_int interrupt
         */
        uint32_t post_done_int_raw:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} huk_int_raw_reg_t;

/** Type of int_st register
 *  HUK Generator interrupt status register.
 */
typedef union {
    struct {
        /** prep_done_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status bit  for the huk_prep_done_int interrupt
         */
        uint32_t prep_done_int_st:1;
        /** proc_done_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status bit  for the huk_proc_done_int interrupt
         */
        uint32_t proc_done_int_st:1;
        /** post_done_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status bit  for the huk_post_done_int interrupt
         */
        uint32_t post_done_int_st:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} huk_int_st_reg_t;

/** Type of int_ena register
 *  HUK Generator interrupt enable register.
 */
typedef union {
    struct {
        /** prep_done_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit  for the huk_prep_done_int interrupt
         */
        uint32_t prep_done_int_ena:1;
        /** proc_done_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit  for the huk_proc_done_int interrupt
         */
        uint32_t proc_done_int_ena:1;
        /** post_done_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit  for the huk_post_done_int interrupt
         */
        uint32_t post_done_int_ena:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} huk_int_ena_reg_t;

/** Type of int_clr register
 *  HUK Generator interrupt clear register.
 */
typedef union {
    struct {
        /** prep_done_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the huk_prep_done_int interrupt
         */
        uint32_t prep_done_int_clr:1;
        /** proc_done_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the huk_proc_done_int interrupt
         */
        uint32_t proc_done_int_clr:1;
        /** post_done_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the huk_post_done_int interrupt
         */
        uint32_t post_done_int_clr:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} huk_int_clr_reg_t;


/** Group: Configuration registers */
/** Type of conf register
 *  HUK Generator configuration register
 */
typedef union {
    struct {
        /** mode : R/W; bitpos: [0]; default: 0;
         *  Set this field to choose the huk process. 1: process huk generate mode. 0: process
         *  huk recovery mode.
         */
        uint32_t mode:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} huk_conf_reg_t;


/** Group: Control registers */
/** Type of start register
 *  HUK Generator control register
 */
typedef union {
    struct {
        /** start : WT; bitpos: [0]; default: 0;
         *  Write 1 to continue HUK Generator operation at LOAD/GAIN state.
         */
        uint32_t start:1;
        /** conti : WT; bitpos: [1]; default: 0;
         *  Write 1 to start HUK Generator at IDLE state.
         */
        uint32_t conti:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} huk_start_reg_t;


/** Group: State registers */
/** Type of state register
 *  HUK Generator state register
 */
typedef union {
    struct {
        /** state : RO; bitpos: [1:0]; default: 0;
         *  The state of HUK Generator. 0: IDLE. 1: LOAD. 2: GAIN. 3: BUSY.
         */
        uint32_t state:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} huk_state_reg_t;


/** Group: Result registers */
/** Type of status register
 *  HUK Generator HUK status register
 */
typedef union {
    struct {
        /** status : RO; bitpos: [1:0]; default: 0;
         *  The HUK generation status. 0: HUK is not generated. 1: HUK is generated and valid.
         *  2: HUK is generated but invalid. 3: reserved.
         */
        uint32_t status:2;
        /** risk_level : RO; bitpos: [4:2]; default: 0;
         *  The risk level of HUK. 0-6: the higher the risk level is, the more error bits there
         *  are in the PUF SRAM. 7: Error Level, HUK is invalid.
         */
        uint32_t risk_level:3;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} huk_status_reg_t;


/** Group: Version register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36720704;
         *  HUK Generator version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} huk_date_reg_t;


typedef struct {
    uint32_t reserved_000;
    volatile huk_clk_reg_t clk;
    volatile huk_int_raw_reg_t int_raw;
    volatile huk_int_st_reg_t int_st;
    volatile huk_int_ena_reg_t int_ena;
    volatile huk_int_clr_reg_t int_clr;
    uint32_t reserved_018[2];
    volatile huk_conf_reg_t conf;
    volatile huk_start_reg_t start;
    volatile huk_state_reg_t state;
    uint32_t reserved_02c[2];
    volatile huk_status_reg_t status;
    uint32_t reserved_038[49];
    volatile huk_date_reg_t date;
    volatile uint32_t info[96];
} huk_dev_t;

extern huk_dev_t HUK;

#ifndef __cplusplus
_Static_assert(sizeof(huk_dev_t) == 0x280, "Invalid size of huk_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
