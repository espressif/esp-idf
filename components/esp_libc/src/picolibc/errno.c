/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <errno.h>

/*
 * Picolibc does not initialize 'errno' and places it in the TBSS section.
 *
 * To allow convenient initialization and support interoperability with Newlib,
 * 'errno' is defined in the TDATA section. The linker script ensures that
 * it is positioned at the beginning of the TDATA segment.
 */
__thread int errno __attribute__((section(".tdata.errno"))) = 0;

#if CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY
int *__errno(void)
{
    return &errno;
}
#endif
