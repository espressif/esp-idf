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
