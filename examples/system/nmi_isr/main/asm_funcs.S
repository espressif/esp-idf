/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <xtensa/coreasm.h>
#include "soc/gpio_reg.h"
#include "example_gpio.h"

    .global nmi_triggered

    .section    .bss
nmi_triggered:
    .space 4


/**
 * @brief This current ISR was called via `call0` instruction, so `a0` (return address)
 * was altered. Fortunately, `a0` was saved in EXCSAVE registers, restore it before
 * returning
 */
    .section    .iram1, "ax"
    .align      4
    .global     xt_nmi
    .type       xt_nmi, @function
xt_nmi:
    addi    sp, sp, -16
    s32i    a3, sp, 0

    /* Set the interrupt flag to 1 */
    movi    a0, nmi_triggered
    movi    a3, 1
    s32i    a3, a0, 0

    /* Set the GPIO level back to low to prevent triggering an interrupt again */
    movi    a0, GPIO_OUT_W1TC_REG
    movi    a3, 1 << EXAMPLE_GPIO_IN
    s32i    a3, a0, 0

    /* Restore a3 and a0 before leaving*/
    l32i    a3, sp, 0
    addi    sp, sp, 16
    rsr     a0, EXCSAVE + XCHAL_NMILEVEL

    /* Return from NMI, we need to specify the level */
    rfi     XCHAL_NMILEVEL
