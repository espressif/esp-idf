/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Perform semihosting call
 *
 * See ARM semihosting spec for details.
 * https://static.docs.arm.com/100863/0200/semihosting.pdf
 *
 * @param id    semihosting call number
 * @param data  data block to pass to the host; number of items and their
 *              meaning depends on the semihosting call. See the spec for
 *              details.
 *
 * @return  return value from the host
 */
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
