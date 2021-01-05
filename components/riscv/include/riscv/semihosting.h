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
 * See https://github.com/riscv/riscv-semihosting-spec/ and the linked
 * ARM semihosting spec for details.
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
    register long a0 asm ("a0") = id;
    register long a1 asm ("a1") = (long) data;
    __asm__ __volatile__ (
        ".option push\n"
        ".option norvc\n"
        "slli zero, zero, 0x1f\n"
        "ebreak\n"
        "srai zero, zero, 0x7\n"
        ".option pop\n"
        : "+r"(a0) : "r"(a1) : "memory");
    return a0;
}

/**
 * @brief Perform semihosting call and retrieve errno
 *
 * @param id    semihosting call number
 * @param data  data block to pass to the host; number of items and their
 *              meaning depends on the semihosting call. See the spec for
 *              details.
 * @param[out] out_errno  output, errno value from the host. Only set if
 *                        the return value is negative.
 * @return   return value from the host
 */
static inline long semihosting_call(long id, long *data, int *out_errno)
{
    long ret = semihosting_call_noerrno(id, data);
    if (ret < 0) {
        /* Constant also defined in openocd_semihosting.h,
         * which is common for RISC-V and Xtensa; it is not included here
         * to avoid a circular dependency.
         */
        const int semihosting_sys_errno = 0x13;
        *out_errno = (int) semihosting_call_noerrno(semihosting_sys_errno, NULL);
    }
    return ret;
}

#ifdef __cplusplus
}
#endif
