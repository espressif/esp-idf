/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enum representing which processor is allowed to enter the critical section
 *
 */
typedef enum {
    ULP_RISCV_LOCK_TURN_ULP,        /*!< ULP's turn to enter the critical section */
    ULP_RISCV_LOCK_TURN_MAIN_CPU,   /*!< Main CPU's turn to enter the critical section */
} ulp_riscv_lock_turn_t;

/**
 * @brief Structure representing a lock shared between ULP and main CPU
 *
 */
typedef struct {
    volatile bool critical_section_flag_ulp;        /*!<  ULP wants to enter the critical sections */
    volatile bool critical_section_flag_main_cpu;   /*!<  Main CPU wants to enter the critical sections */
    volatile ulp_riscv_lock_turn_t turn;            /*!<  Which CPU is allowed to enter the critical section */
} ulp_riscv_lock_t;

#ifdef __cplusplus
}
#endif
