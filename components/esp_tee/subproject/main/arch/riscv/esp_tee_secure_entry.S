/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
    /* Setup the APM for HP CPU in TEE mode */
    li      t0, TEE_M0_MODE_CTRL_REG
    sw      zero, 0(t0)                  /* APM_LL_SECURE_MODE_TEE = 0 */

    /* Disable the U-mode delegation of all interrupts */
    csrwi   mideleg, 0

    /* Jump to the secure service dispatcher */
    jal     esp_tee_service_dispatcher

    /* Setup the APM for HP CPU in REE mode */
    li      t0, TEE_M0_MODE_CTRL_REG
    li      t1, 0x1                      /* APM_LL_SECURE_MODE_REE = 1 */
    sw      t1, 0(t0)

    /* Enable the U-mode delegation of all interrupts (except the TEE secure interrupt) */
    li      t0, 0xffffbfff
    csrw    mideleg, t0

    /* Fire an M-ecall */
    mv      a1, zero
    ecall
    fence

    ret
