/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
    .section .iram1, "ax"
    .balign 4
    .global _u2m_switch
    .type   _u2m_switch, @function
_u2m_switch:
     ecall
     fence
     ret
