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
    /* GCC doesn't allow using specific register names in constraints for Xtensa.
     * For this case, GCC extended inline assembly manual says the following:
     *   If you must use a specific register, but your Machine Constraints do not provide
     *   sufficient control to select the specific register you want, local register variables
     *   may provide a solution.
     * Using local register variables results in simpler generated code than
     * the previous implementation which listed a2-a6 as clobbered registers.
     */
    register long a2 asm ("a2") = id;
    register long a3 asm ("a3") = (long) data[0];
    register long a4 asm ("a4") = (long) data[1];
    register long a5 asm ("a5") = (long) data[2];
    register long a6 asm ("a6") = (long) data[3];

    /* The break instruction operands should be (1, 14) according to the ISA manual.
     * We keep (1, 1) for compatibility, until OpenOCD is updated to support both
     * conventions.
     */
    __asm__ __volatile__ (
        "break 1, 1\n"
        : "+r"(a2), "+r"(a3)
        : "r"(a4), "r"(a5), "r"(a6)
        : "memory");

    long host_ret = a2;
    long host_errno = a3;
    if (host_ret < 0) {
        *out_errno = host_errno;
    }
    return host_ret;
}

#ifdef __cplusplus
}
#endif
