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
 * @brief Perform semihosting call and retrieve errno
 *
 * @param id    semihosting call number
 * @param data  data block to pass to the host; number of items and their
 *              meaning depends on the semihosting call. See the spec for
 *              details.
 *              On Xtensa, this function assumes that the array contains at
 *              least 4 elements, but no effort is made to guarantee that.
 *              Passing a shorter array will still work, as long as it contains
 *              sufficient values for the corresponding semihosting call.
 * @param[out] out_errno  output, errno value from the host. Only set if
 *                        the return value is negative.
 * @return   return value from the host
 */
static inline long semihosting_call(long id, long *data, int *out_errno)    // NOLINT(readability-non-const-parameter)
{
    long host_ret;
    long host_errno;
    /* The break instruction operands should be (1, 14) according to the ISA manual.
     * We keep (1, 1) for compatibility, until OpenOCD is updated to support both
     * conventions.
     */
    __asm__ __volatile__ (
        "mov a2, %[sys_nr]\n" \
        "mov a3, %[arg1]\n" \
        "mov a4, %[arg2]\n" \
        "mov a5, %[arg3]\n" \
        "mov a6, %[arg4]\n" \
        "break 1, 1\n" \
        "mov %[host_ret], a2\n" \
        "mov %[host_errno], a3\n" \
        :[host_ret]"=r"(host_ret), [host_errno]"=r"(host_errno)
        :[sys_nr]"r"(id),
        [arg1]"r"(data[0]),
        [arg2]"r"(data[1]),
        [arg3]"r"(data[2]),
        [arg4]"r"(data[3])
        :"a2", "a3", "a4", "a5", "a6");
    if (host_ret < 0) {
        *out_errno = host_errno;
    }
    return host_ret;
}

#ifdef __cplusplus
}
#endif
