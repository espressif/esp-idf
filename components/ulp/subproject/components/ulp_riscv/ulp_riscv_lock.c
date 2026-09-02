/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ulp_riscv_lock.h"
#include "ulp_riscv_lock_shared.h"

void ulp_riscv_lock_acquire(ulp_riscv_lock_t *lock)
{
    lock->critical_section_flag_ulp = true;
    lock->turn = ULP_RISCV_LOCK_TURN_MAIN_CPU;

    while (lock->critical_section_flag_main_cpu && (lock->turn == ULP_RISCV_LOCK_TURN_MAIN_CPU)) {
    }
}

void ulp_riscv_lock_release(ulp_riscv_lock_t *lock)
{
    lock->critical_section_flag_ulp = false;
}
