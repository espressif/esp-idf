/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Trace encoder interrupt flags. */
typedef enum {
    RISCV_TRACE_INTR_FIFO_OVERFLOW = BIT(0), /*!< Trace FIFO overflowed. Some packets were lost */
    RISCV_TRACE_INTR_MEM_FULL      = BIT(1), /*!< Trace memory region became full. */
} riscv_trace_intr_t;

/**
 * @brief Privilege level selected by the filter privilege qualifier.
 *
 * Values are the RISC-V architectural privilege encoding. Targets whose
 * match_choice_privilege field is narrower implement only a subset, and their
 * LL narrows the value when writing the register.
 */
typedef enum {
    RISCV_TRACE_PRIV_USER       = 0, /*!< User mode */
    RISCV_TRACE_PRIV_SUPERVISOR = 1, /*!< Supervisor mode */
    RISCV_TRACE_PRIV_MACHINE    = 3, /*!< Machine mode */
} riscv_trace_priv_t;

/**
 * @brief Resynchronization mode selected by the resync counter.
 *
 * Values follow the v2.0 (2-bit) register encoding, which the driver's public
 * enum also uses. Targets with a narrower field implement a subset and their LL
 * translates the value when writing the register (see
 * riscv_trace_ll_resync_mode_is_supported()).
 */
typedef enum {
    RISCV_TRACE_RESYNC_DISABLED = 0, /*!< Periodic resync disabled */
    RISCV_TRACE_RESYNC_PACKET   = 2, /*!< Resync counter counts by packet */
    RISCV_TRACE_RESYNC_CYCLE    = 3, /*!< Resync counter counts by cycle */
} riscv_trace_resync_mode_t;

/**
 * @brief Trace encoder work-status field.
 *
 * Some targets implement a narrower work_status field and only produce
 * the lower values (e.g., IDLE and WORKING). The enum is the union of
 * all observable values across targets.
 */
typedef enum {
    RISCV_TRACE_WORK_IDLE    = 0, /*!< Encoder is not tracing */
    RISCV_TRACE_WORK_WORKING = 1, /*!< Encoder is actively tracing */
    RISCV_TRACE_WORK_WAIT    = 2, /*!< Paused because the hart is halted or in reset */
    RISCV_TRACE_WORK_LOST    = 3, /*!< Trace data was lost (e.g. FIFO overflow) */
} riscv_trace_work_status_t;

#ifdef __cplusplus
}
#endif
