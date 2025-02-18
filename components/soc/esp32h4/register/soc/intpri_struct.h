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

/** Group: Interrupt Registers */
/** Type of cpu_intr_from_cpu_n register
 *  CPU_INTR_FROM_CPU_n mapping register
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_n : R/W; bitpos: [0]; default: 0;
         *  CPU_INTR_FROM_CPU_n mapping register.
         */
        uint32_t cpu_intr_from_cpu_n:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} intpri_cpu_intr_from_cpu_n_reg_t;


/** Group: Version Registers */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36712784;
         *  Version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} intpri_date_reg_t;


typedef struct {
    uint32_t reserved_000[36];
    volatile intpri_cpu_intr_from_cpu_n_reg_t cpu_intr_from_cpu_n[4];
    volatile intpri_date_reg_t date;
} intpri_dev_t;

extern intpri_dev_t INTPRI;

#ifndef __cplusplus
_Static_assert(sizeof(intpri_dev_t) == 0xa4, "Invalid size of intpri_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
