/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Entry into U-mode code, the run context that the entry code reads before mret, and the U-mode trap handlers */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_assert.h"
#include "esp_privilege_umode_domain.h"
#include "esp_private/privilege_trap.h"

typedef int (*esp_privilege_user_fn_t)(uint32_t arg0, uint32_t arg1);

/** Virtual address that the entry function of a domain returns to. No domain maps it, so the return
 *  raises an instruction page fault. */
#define PRIVILEGE_DOMAIN_RETURN_ADDRESS ESP_PRIVILEGE_UMODE_DOMAIN_ADDRESS_LIMIT

/* Run context that the entry code reads before it enters U-mode */
typedef struct privilege_domain_run_context privilege_domain_run_context_t;

struct privilege_domain_run_context {
    uint32_t gp;                /* initial user gp */
    uint32_t tp;                /* initial user tp */
    uint32_t return_address;    /* user address that the entry function returns to */
};

/**
 * @brief Run a function in U-mode on the calling task
 *
 * @param entry  U-mode entry point
 * @param arg0   First argument passed to entry
 * @param arg1   Second argument passed to entry
 * @param sp     Initial user stack pointer, 16-byte aligned
 * @param satp   satp value of the page tables that the U-mode code runs with. Translations and instruction
 *               fetch must have been flushed on every core since the page tables and the code last changed.
 * @param runtime Run context
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

ESP_STATIC_ASSERT(offsetof(privilege_domain_run_context_t, gp) == RV_URUN_GP, "assembly GP offset");
ESP_STATIC_ASSERT(offsetof(privilege_domain_run_context_t, tp) == RV_URUN_TP, "assembly TP offset");
ESP_STATIC_ASSERT(offsetof(privilege_domain_run_context_t, return_address) == RV_URUN_RETURN, "assembly return offset");
