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

/** Group: configuration registers */
/** Type of addr_lock register
 *  hardware lock regsiter
 */
typedef union {
    struct {
        /** lock : R/W; bitpos: [1:0]; default: 0;
         *  read to acquire hardware lock, write to release hardware lock
         */
        uint32_t lock:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} atomic_addr_lock_reg_t;

/** Type of lr_addr register
 *  gloable lr address regsiter
 */
typedef union {
    struct {
        /** gloable_lr_addr : R/W; bitpos: [31:0]; default: 0;
         *  backup gloable address
         */
        uint32_t gloable_lr_addr:32;
    };
    uint32_t val;
} atomic_lr_addr_reg_t;

/** Type of lr_value register
 *  gloable lr value regsiter
 */
typedef union {
    struct {
        /** gloable_lr_value : R/W; bitpos: [31:0]; default: 0;
         *  backup gloable value
         */
        uint32_t gloable_lr_value:32;
    };
    uint32_t val;
} atomic_lr_value_reg_t;

/** Type of lock_status register
 *  lock status regsiter
 */
typedef union {
    struct {
        /** lock_status : RO; bitpos: [1:0]; default: 0;
         *  read hareware lock status for debug
         */
        uint32_t lock_status:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} atomic_lock_status_reg_t;

/** Type of counter register
 *  wait counter register
 */
typedef union {
    struct {
        /** wait_counter : R/W; bitpos: [15:0]; default: 0;
         *  delay counter
         */
        uint32_t wait_counter:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} atomic_counter_reg_t;


typedef struct atomic_dev_t {
    volatile atomic_addr_lock_reg_t addr_lock;
    volatile atomic_lr_addr_reg_t lr_addr;
    volatile atomic_lr_value_reg_t lr_value;
    volatile atomic_lock_status_reg_t lock_status;
    volatile atomic_counter_reg_t counter;
} atomic_dev_t;

extern atomic_dev_t ATOMIC_LOCKER;

#ifndef __cplusplus
_Static_assert(sizeof(atomic_dev_t) == 0x14, "Invalid size of atomic_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
