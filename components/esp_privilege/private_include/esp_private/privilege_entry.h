/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Entry into U-mode code, the run context that the trap code reads, and the U-mode trap handlers */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_assert.h"
#include "esp_private/privilege_trap.h"

typedef int (*esp_privilege_user_fn_t)(uint32_t arg0, uint32_t arg1);

/* Run context read by the entry and trap code. It must stay valid, in M-mode memory, until
 * esp_privilege_call_user_code() returns. */
typedef struct privilege_domain_run_context privilege_domain_run_context_t;

/**
 * @brief Run a function in U-mode on the calling task
 *
 * @param entry  U-mode entry point
 * @param arg0   First argument passed to entry
 * @param arg1   Second argument passed to entry
 * @param sp     Initial user stack pointer, 16-byte aligned
 * @param satp   satp value used while the task runs U-mode code, 0 for no translation
 * @param runtime Reserved for a run context, must be NULL
 * @return The low word is the value returned by entry. The high word is 0.
 */
uint64_t esp_privilege_call_user_code(esp_privilege_user_fn_t entry, uint32_t arg0, uint32_t arg1, void *sp,
                                      uint32_t satp, const privilege_domain_run_context_t *runtime);

/* M-mode continuation that ends a run, implemented in privilege_user_mode_entry.S. */
void esp_privilege_return_to_caller(void);

/** Make the trap frame resume in M-mode, with the M-mode gp and tp. */
void esp_privilege_frame_to_machine(void *frame);

/**
 * @brief Handle an ecall from U-mode, called by the exception handler
 *
 * @param frame Exception frame, can be modified to change the context the task is resumed in
 * @return true if the ecall was handled, false if it must be treated as an unhandled exception
 */
bool esp_privilege_ecall_handler(void *frame);
