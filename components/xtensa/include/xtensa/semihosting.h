/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

static inline long semihosting_call_noerrno(long id, long *data)
{
    register long a2 asm ("a2") = id;
    register long a3 asm ("a3") = (long)data;

    __asm__ __volatile__ (
        "break 1, 14\n"
        : "+r"(a2) : "r"(a3)
        : "memory");
    return a2;
}

#ifdef __cplusplus
}
#endif
