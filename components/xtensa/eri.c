/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#include <stdint.h>
#include "eri.h"

uint32_t eri_read(int addr) {
    uint32_t ret;
    asm volatile (
        "RER %0,%1"
        :"=r"(ret):"r"(addr)
        );
    return ret;
}

void eri_write(int addr, uint32_t data) {
    asm volatile (
        "WER %0,%1"
        ::"r"(data),"r"(addr)
    );
}
