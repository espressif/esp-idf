/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "soc/tee_reg.h"
#include "soc/plic_reg.h"

    .global   esp_tee_service_dispatcher

/* Entry point to the secure world (i.e. M-mode) - responsible for
 * setting up the execution environment for the secure world */
    .section .text
    .align  4
    .global _sec_world_entry
    .type _sec_world_entry, @function
_sec_world_entry:
    /* Disable the U-mode delegation of all interrupts */
    csrwi   mideleg, 0

    /* Jump to the secure service dispatcher */
    jal     esp_tee_service_dispatcher

    /* Enable the U-mode delegation of all interrupts (except the TEE secure interrupt) */
    li      t0, 0xffffbfff
    csrw    mideleg, t0

    /* Fire an M-ecall */
    mv      a1, zero
    ecall
    fence

    ret
