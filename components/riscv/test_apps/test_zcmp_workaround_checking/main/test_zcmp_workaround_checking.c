/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>

void app_main(void)
{
    uint32_t mstatus;
#ifdef TEST_INVALID_WORKAROUND
    /* Intentional ZCMP violation: clear MIE without mintthresh write. */
    __asm__ volatile("csrrci %0, mstatus, 0x8" : "=r"(mstatus));
#else
    __asm__ volatile(
        "li t0, 0xff\n"
        "csrrw zero, 0x347, t0\n"
        "csrrci %0, mstatus, 0x8"
        : "=r"(mstatus)
        :
        : "t0");
#endif
    (void)mstatus;
}
