/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_print.h"

volatile uint32_t hp_mem_data_marker = 0x12345678;
volatile uint32_t hp_mem_bss_marker;
volatile uint32_t hp_mem_done_marker;
const uint32_t hp_mem_rodata_marker __attribute__((used)) = 0xA5A5F00D;

/* Pad rodata to push the LP Core binary above the LP RAM reservation limit
 * (CONFIG_ULP_COPROC_RESERVE_MEM), proving this binary can only run from HP RAM. */
static const volatile uint8_t s_hp_mem_size_padding[50 * 1024] __attribute__((used)) = {[0] = 0xAA};

int main(void)
{
    lp_core_printf("LP core running from HP memory\n");
    hp_mem_data_marker = 0x89ABCDEF;
    hp_mem_bss_marker = 0x13579BDF;
    hp_mem_done_marker = 0x2468ACE0;

    lp_core_printf("s_hp_mem_size_padding[0] = 0x%02X\n", s_hp_mem_size_padding[0]);

    return 0;
}
