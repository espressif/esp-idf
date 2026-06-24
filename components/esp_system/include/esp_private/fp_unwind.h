/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FP_UNWIND_H
#define FP_UNWIND_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Print backtrace for the given execution frame thanks to the frame pointers.
 *
 * @param frame_or Snapshot of the CPU registers when the program stopped its
 *                 normal execution. This frame is usually generated on the
 *                 stack when an exception or an interrupt occurs.
 */
void esp_fp_print_backtrace(const void *frame_or);

/**
 * @brief Generate a call stack starting at the given frame pointer.
 *
 * @param frame[in] Frame pointer to start unrolling from.
 * @param callers[out] Array of callers where 0 will store the most recent caller. Can be NULL.
 * @param stacks[out] Array of callers' stacks where 0 will store the most recent caller's stack. Can be NULL.
 * @param depth[in] Maximum number of entries to fill in the callers and stacks arrays.
 *                  Both arrays (when provided) must be able to hold at least this many entries.
 *
 * @returns Number of entries filled in the array.
 */
uint32_t esp_fp_get_callers(uint32_t frame, void** callers, void** stacks, uint32_t depth);

/**
 * @brief Output a single backtrace step to the serial.
 *
 * Defined as weak so that it can be overridden by the user, e.g. to capture each
 * backtrace step and store it or forward it elsewhere instead of printing.
 *
 * @warning This function is also used to print the backtrace from the panic handler.
 *          Overriding it therefore changes the backtrace output on a panic as well,
 *          so any override must remain safe to call from a panic/exception context.
 *
 * @param pc Program counter of the backtrace step.
 * @param sp Stack pointer of the backtrace step.
 */
void esp_fp_generated_step(uint32_t pc, uint32_t sp);

#ifdef __cplusplus
}
#endif

#endif // FP_UNWIND_H
