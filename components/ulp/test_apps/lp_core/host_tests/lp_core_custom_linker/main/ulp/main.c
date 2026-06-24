/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>

#include "shared.h"

/* Defined by the custom layout via PROVIDE(); reference keeps it from --gc-sections. */
extern const uint32_t _custom_linker_marker;

custom_linker_shared_t shared_state __attribute__((used, section(".custom_state")));

int main(void)
{
    shared_state.marker = (uint32_t)(uintptr_t)&_custom_linker_marker;

    while (1) {
        shared_state.lp_heartbeat++;
        for (volatile uint32_t i = 0; i < 10000; i++) {
            __asm__ volatile("nop");
        }
    }
    return 0;
}
