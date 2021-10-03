/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SOC_CPU_H
#define _SOC_CPU_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "esp_cpu.h"

#if __XTENSA__
#include "xt_instr_macros.h"
// [refactor-todo] not actually needed in this header now,
// but kept for compatibility
#include "xtensa/corebits.h"
#include "xtensa/config/core.h"

#include "xtensa/config/specreg.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Read current stack pointer address.
 * Superseded by esp_cpu_get_sp in esp_cpu.h.
 */
static inline __attribute__((deprecated)) void *get_sp(void)
{
    return esp_cpu_get_sp();
}

static inline uint32_t esp_cpu_process_stack_pc(uint32_t pc)
{
    if (pc & 0x80000000) {
        //Top two bits of a0 (return address) specify window increment. Overwrite to map to address space.
        pc = (pc & 0x3fffffff) | 0x40000000;
    }
    //Minus 3 to get PC of previous instruction (i.e. instruction executed before return address)
    return pc - 3;
}

/**
 * @brief Configure CPU to disable access to invalid memory regions
 *
 */
void esp_cpu_configure_region_protection(void);

#ifdef __cplusplus
}
#endif

#endif
