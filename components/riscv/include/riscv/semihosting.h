/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/* ESP custom semihosting calls numbers */

/**
 * @brief Initialize apptrace data at host side
 *
 * @param addr    address of apptrace control data block
 * @return        return 0 on sucess or non-zero error code
 */
#define ESP_SEMIHOSTING_SYS_APPTRACE_INIT   0x101

/**
 * @brief Initialize debug stubs table at host side
 *
 * @param addr    address of debug stubs table
 * @return        return 0 on sucess or non-zero error code
 */
#define ESP_SEMIHOSTING_SYS_DBG_STUBS_INIT  0x102

/**
 * @brief Set/clear breakpoint
 *
 * @param set     if true set breakpoint, otherwise clear it
 * @param id      breakpoint ID
 * @param addr    address to set breakpoint at. Ignored if `set` is false.
 * @return        return 0 on sucess or non-zero error code
 */
#define ESP_SEMIHOSTING_SYS_BREAKPOINT_SET  0x103

/**
 * @brief Set/clear watchpoint
 *
 * @param set     if true set watchpoint, otherwise clear it
 * @param id      watchpoint ID
 * @param addr    address to set watchpoint at. Ignored if `set` is false.
 * @param size    size of watchpoint. Ignored if `set` is false.
 * @param flags   watchpoint flags, see description below. Ignored if `set` is false.
 * @return        return 0 on sucess or non-zero error code
 */
#define ESP_SEMIHOSTING_SYS_WATCHPOINT_SET  0x104

/* bit values for `flags` argument of ESP_SEMIHOSTING_SYS_WATCHPOINT_SET call. Can be ORed. */
/* watch for 'reads' at `addr` */
#define ESP_SEMIHOSTING_WP_FLG_RD   (1UL << 0)
/* watch for 'writes' at `addr` */
#define ESP_SEMIHOSTING_WP_FLG_WR   (1UL << 1)

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

#ifdef __cplusplus
}
#endif
