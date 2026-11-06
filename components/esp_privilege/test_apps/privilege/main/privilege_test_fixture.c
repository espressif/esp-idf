/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Runs code in U-mode on the calling task for the tests */
#include <assert.h>
#include "esp_macros.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "riscv/rv_utils.h"
#include "privilege_code.h"
#include "privilege_test_fixture.h"

#define PRIVILEGE_MIN_STACK_SIZE 256

esp_err_t test_privilege_call(esp_privilege_user_fn_t entry, uint32_t arg0, uint32_t arg1,
                              void *user_stack, size_t user_stack_size, int *ret)
{
    assert(entry != NULL && user_stack != NULL && user_stack_size >= PRIVILEGE_MIN_STACK_SIZE);
    /* The U-mode code must be preemptible. It runs with interrupts enabled at the base interrupt level */
    assert(xPortCanYield() && (RV_READ_CSR(mstatus) & MSTATUS_MIE) != 0 &&
           xTaskGetSchedulerState() == taskSCHEDULER_RUNNING);
    /* Delegation is per core, so verify it on every core that runs U-mode code */
    assert(RV_READ_CSR(medeleg) == 0);
    assert(RV_READ_CSR(mideleg) == 0);

    void *user_sp = (void *)ESP_ALIGN_DOWN((uintptr_t)user_stack + user_stack_size, 16);
    const privilege_domain_run_context_t context = { .return_address = (uint32_t)privilege_exit_trampoline };
    const uint64_t result = esp_privilege_call_user_code(entry, arg0, arg1, user_sp, 0, &context);
    /* The high word is the fault status, never a value of the U-mode code */
    if ((result >> 32) != 0) {
        return ESP_FAIL;
    }
    if (ret != NULL) {
        *ret = (int)(uint32_t)result;
    }
    return ESP_OK;
}
