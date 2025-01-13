/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FP_UNWIND_H
#define FP_UNWIND_H

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

#ifdef __cplusplus
}
#endif

#endif // FP_UNWIND_H
