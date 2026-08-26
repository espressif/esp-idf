/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>

/* Defined by the custom layout via PROVIDE(); reference keeps it from --gc-sections. */
extern const uint32_t _custom_linker_marker;

volatile uint32_t custom_marker_copy __attribute__((used, section(".custom_state")));

int main(void)
{
    custom_marker_copy = (uint32_t)(uintptr_t)&_custom_linker_marker;

    while (1) {
        custom_marker_copy++;
        for (volatile uint32_t i = 0; i < 10000; i++) {
            __asm__ volatile("nop");
        }
    }
    return 0;
}
