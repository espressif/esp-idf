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

/** Group: Memory */

/** Group: Control / Configuration Registers */
/** Type of m_prime register
 *  Represents M’
 */
typedef union {
    struct {
        /** m_prime : R/W; bitpos: [31:0]; default: 0;
         *  Represents M’
         */
        uint32_t m_prime:32;
    };
    uint32_t val;
} rsa_m_prime_reg_t;

/** Type of mode register
 *  Configures RSA length
 */
typedef union {
    struct {
        /** mode : R/W; bitpos: [6:0]; default: 0;
         *  Configures the RSA length.
         */
        uint32_t mode:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} rsa_mode_reg_t;

/** Type of set_start_modexp register
 *  Starts modular exponentiation
 */
typedef union {
    struct {
        /** set_start_modexp : WT; bitpos: [0]; default: 0;
         *  Configure whether or not to start the modular exponentiation.
         *
         *  0: No effect
         *
         *  1: Start
         *
         */
        uint32_t set_start_modexp:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rsa_set_start_modexp_reg_t;

/** Type of set_start_modmult register
 *  Starts modular multiplication
 */
typedef union {
    struct {
        /** set_start_modmult : WT; bitpos: [0]; default: 0;
         *  Configure whether or not to start the modular multiplication.
         *
         *  0: No effect
         *
         *  1: Start
         *
         */
        uint32_t set_start_modmult:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rsa_set_start_modmult_reg_t;

/** Type of set_start_mult register
 *  Starts multiplication
 */
typedef union {
    struct {
        /** set_start_mult : WT; bitpos: [0]; default: 0;
         *  Configure whether or not to start the multiplication.
         *
         *  0: No effect
         *
         *  1: Start
         *
         */
        uint32_t set_start_mult:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rsa_set_start_mult_reg_t;

/** Type of query_idle register
 *  Represents the RSA status
 */
typedef union {
    struct {
        /** query_idle : RO; bitpos: [0]; default: 0;
         *  Represents the RSA status.
         *
         *  0: Busy
         *
         *  1: Idle
         *
         */
        uint32_t query_idle:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rsa_query_idle_reg_t;

/** Type of constant_time register
 *  Configures the constant_time option
 */
typedef union {
    struct {
        /** constant_time : R/W; bitpos: [0]; default: 1;
         *  Configures the constant_time option.
         *
         *  0: Acceleration
         *
         *  1: No acceleration (default)
         *
         */
        uint32_t constant_time:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rsa_constant_time_reg_t;

/** Type of search_enable register
 *  Configures the search option
 */
typedef union {
    struct {
        /** search_enable : R/W; bitpos: [0]; default: 0;
         *  Configure the search option.
         *
         *  0: No acceleration (default)
         *
         *  1: Acceleration
         *
         *  This option should be used together with RSA_SEARCH_POS.
         */
        uint32_t search_enable:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rsa_search_enable_reg_t;

/** Type of search_pos register
 *  Configures the search position
 */
typedef union {
    struct {
        /** search_pos : R/W; bitpos: [11:0]; default: 0;
         *  Configures the starting address to start search. This field should be used together
         *  with RSA_SEARCH_ENABLE. The field is only valid when RSA_SEARCH_ENABLE is high.
         */
        uint32_t search_pos:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} rsa_search_pos_reg_t;


/** Group: Status Register */
/** Type of query_clean register
 *  RSA clean register
 */
typedef union {
    struct {
        /** query_clean : RO; bitpos: [0]; default: 0;
         *  Represents whether or not the RSA memory completes initialization.
         *
         *  0: Not complete
         *
         *  1: Completed
         *
         */
        uint32_t query_clean:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rsa_query_clean_reg_t;


/** Group: Interrupt Registers */
/** Type of int_clr register
 *  Clears RSA interrupt
 */
typedef union {
    struct {
        /** clear_interrupt : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear the RSA interrupt.
         */
        uint32_t clear_interrupt:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rsa_int_clr_reg_t;

/** Type of int_ena register
 *  Enables the RSA interrupt
 */
typedef union {
    struct {
        /** int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable the RSA interrupt.
         */
        uint32_t int_ena:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rsa_int_ena_reg_t;


/** Group: Version Control Register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [29:0]; default: 538969624;
         *  Version control register.
         */
        uint32_t date:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} rsa_date_reg_t;


typedef struct {
    volatile uint32_t m[4];
    uint32_t reserved_010[124];
    volatile uint32_t z[4];
    uint32_t reserved_210[124];
    volatile uint32_t y[4];
    uint32_t reserved_410[124];
    volatile uint32_t x[4];
    uint32_t reserved_610[124];
    volatile rsa_m_prime_reg_t m_prime;
    volatile rsa_mode_reg_t mode;
    volatile rsa_query_clean_reg_t query_clean;
    volatile rsa_set_start_modexp_reg_t set_start_modexp;
    volatile rsa_set_start_modmult_reg_t set_start_modmult;
    volatile rsa_set_start_mult_reg_t set_start_mult;
    volatile rsa_query_idle_reg_t query_idle;
    volatile rsa_int_clr_reg_t int_clr;
    volatile rsa_constant_time_reg_t constant_time;
    volatile rsa_search_enable_reg_t search_enable;
    volatile rsa_search_pos_reg_t search_pos;
    volatile rsa_int_ena_reg_t int_ena;
    volatile rsa_date_reg_t date;
} rsa_dev_t;

extern rsa_dev_t RSA;

#ifndef __cplusplus
_Static_assert(sizeof(rsa_dev_t) == 0x834, "Invalid size of rsa_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
