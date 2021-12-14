/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_CPU_UTILS_H
#define _ESP_CPU_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fetch the PC value of the previous instruction
 *
 * @param pc PC value of the current backtrace frame
 *
 */
static inline uint32_t esp_cpu_process_stack_pc(uint32_t pc)
{
    if (pc & 0x80000000) {
        //Top two bits of a0 (return address) specify window increment. Overwrite to map to address space.
        pc = (pc & 0x3fffffff) | 0x40000000;
    }
    //Minus 3 to get PC of previous instruction (i.e. instruction executed before return address)
    return pc - 3;
}

#ifdef __cplusplus
}
#endif

#endif // _ESP_CPU_UTILS_H
