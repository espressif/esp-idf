/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/riscv_trace_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief HAL context for one trace encoder core. */
typedef struct {
    void *dev;     /*!< Register block */
    int core_id;   /*!< Core index */
} riscv_trace_hal_context_t;

/** @brief Hardware-level capture configuration. */
typedef struct {
    uint32_t mem_start_addr;       /*!< Trace memory start address */
    uint32_t mem_end_addr;         /*!< Trace memory end address */
    bool mem_loop;                 /*!< Loop/wrap memory mode (vs stop when full) */
    bool auto_restart;             /*!< Auto-restart encoder after a FIFO overflow */
    bool full_address;             /*!< Full address mode (vs delta) */
    bool stall_cpu;                /*!< Stall CPU when the FIFO is almost full */
    bool halt_enable;              /*!< Keep tracing through hart halt */
    bool reset_enable;             /*!< Keep tracing through hart reset */
    bool debug_trigger_enable;     /*!< Enable Debug Module trigger input */
    uint32_t resync_mode;          /*!< Resynchronization mode */
    uint32_t resync_threshold;     /*!< Resynchronization counter threshold */
    uint32_t ahb_burst;            /*!< AHB burst type (hburst) */
    uint32_t ahb_max_incr;         /*!< Max INCR burst beats */
    uint32_t intr_mask;            /*!< Interrupt enable mask (riscv_trace_intr_t flags) */
} riscv_trace_hal_config_t;

/** @brief One comparator of the filter unit (raw register values). */
typedef struct {
    uint32_t input;                /*!< Compared input: 0 = iaddr, 1 = tval */
    uint32_t function;             /*!< Compare function: 0 ==, 1 !=, 2 <, 3 <=, 4 >, 5 >= */
    uint32_t match_value;          /*!< 32-bit value compared against the input */
    bool notify;                   /*!< Emit a packet reporting the address that caused the match */
} riscv_trace_hal_comparator_t;

/** @brief Filter (trace qualifier) configuration. */
typedef struct {
    bool enable;                   /*!< Master filter enable (filter_en). false = trace everything */
    bool match_comparators;        /*!< Gate matching on the comparators (match_comp) */
    bool match_privilege;          /*!< Gate matching on the privilege level (match_privilege) */
    bool match_ecause;             /*!< Match from an exception cause (match_ecause) */
    bool match_interrupt;          /*!< Match from an interrupt trap (match_interrupt) */
    bool privilege_machine;        /*!< match_choice_privilege: true = machine, false = user */
    bool interrupt_itype2;         /*!< match_value_interrupt: true = itype 2, false = itype 1 */
    uint32_t ecause;               /*!< match_choice_ecause (6-bit exception cause code) */
    riscv_trace_hal_comparator_t primary;   /*!< Primary comparator */
    riscv_trace_hal_comparator_t secondary; /*!< Secondary comparator */
    uint32_t match_mode;           /*!< 0 primary only, 1 P&&S, 2 !(P&&S), 3 range (start P..until S) */
} riscv_trace_hal_filter_config_t;

void riscv_trace_hal_init(int core_id, const riscv_trace_hal_config_t *config, riscv_trace_hal_context_t *ctx);

/** @brief Start tracing */
void riscv_trace_hal_start(riscv_trace_hal_context_t *ctx);

/** @brief Enable or disable auto-restart after FIFO overflow. */
void riscv_trace_hal_set_auto_restart(riscv_trace_hal_context_t *ctx, bool enable);

/** @brief Set the interrupt enable mask (see riscv_trace_intr_t flags). 0 disables all. */
void riscv_trace_hal_set_intr_enable(riscv_trace_hal_context_t *ctx, uint32_t mask);

/** @brief Stop tracing and wait for the FIFO to empty. */
bool riscv_trace_hal_stop(riscv_trace_hal_context_t *ctx, uint32_t timeout_us);

/** @brief Deinitialize the trace HAL driver */
void riscv_trace_hal_deinit(riscv_trace_hal_context_t *ctx);

/** @brief Read the raw TRACE_FIFO_STATUS register value. */
uint32_t riscv_trace_hal_read_fifo_status(riscv_trace_hal_context_t *ctx);

/** @brief Read the current (next) write address from the encoder. */
uint32_t riscv_trace_hal_get_current_addr(riscv_trace_hal_context_t *ctx);

/** @brief Read the raw TRACE_INTR_RAW register value. */
uint32_t riscv_trace_hal_read_intr_raw(riscv_trace_hal_context_t *ctx);

/** @brief Check whether TRACE_FIFO_STATUS reports an empty FIFO. */
bool riscv_trace_hal_fifo_is_empty(uint32_t fifo_status);

/** @brief Get the work-status field from TRACE_FIFO_STATUS. */
riscv_trace_work_status_t riscv_trace_hal_get_work_status(uint32_t fifo_status);

/** @brief Check whether TRACE_INTR_RAW reports trace memory full. */
bool riscv_trace_hal_memory_is_full(uint32_t intr_status);

/** @brief Check whether TRACE_INTR_RAW reports FIFO overflow. */
bool riscv_trace_hal_fifo_is_overflowed(uint32_t intr_status);

/** @brief Reset the hardware write pointer and clear interrupts before a new capture. */
void riscv_trace_hal_prepare_capture(riscv_trace_hal_context_t *ctx);

/** @brief Apply a filter (trace qualifier) configuration. Set before starting a capture. */
void riscv_trace_hal_set_filter(riscv_trace_hal_context_t *ctx, const riscv_trace_hal_filter_config_t *config);

#ifdef __cplusplus
}
#endif
