/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_private/privilege_entry.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Run a function in RISC-V U-mode, on a separate stack, and wait for it to return
 *
 * The calling task keeps being scheduled normally while the U-mode code runs. It can be preempted
 * by interrupts and other tasks, and resumes in U-mode afterwards.
 *
 * The U-mode code can not call ESP-IDF functions directly.
 *
 * Call from a task, with interrupts enabled and outside a critical section.
 *
 * @note No memory isolation is provided. The U-mode code has the same memory access as M-mode code.
 *
 * @param entry Function to run in U-mode
 * @param arg0 First argument given to entry
 * @param arg1 Second argument given to entry
 * @param user_stack Lowest address of the stack used by the U-mode code
 * @param user_stack_size Size of user_stack in bytes
 * @param[out] ret Value returned by entry, can be NULL. Unchanged on error
 *
 * @return
 *      - ESP_OK. Entry was called and returned
 */
esp_err_t test_privilege_call(esp_privilege_user_fn_t entry, uint32_t arg0, uint32_t arg1,
                              void *user_stack, size_t user_stack_size, int *ret);

#ifdef __cplusplus
}
#endif
