/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdint.h>

#if CONFIG_IDF_TARGET_ARCH_RISCV

#if SOC_INT_CLIC_SUPPORTED

/**
 * @brief Checks whether the given interrupt number is reserved either in the given mask or in the
 *        _mtvt_table, which contains the routines the CPU will jump to when an interrupt or an exception
 *        occurs, on RISC-V targets.
 *
 * @param intr_num  External interrupt number to check, in range 0~32
 * @param rsvd_mask Reserved interrupt mask, where bit i is 1 if interrupt i is reserved.
 *
 * @returns ESP_CPU_INTR_DESC_FLAG_RESVD if the interrupt is reserved, 0 else
 */
static inline uint32_t esp_riscv_intr_num_flags(int intr_num, uint32_t rsvd_mask)
{
    if (rsvd_mask & BIT(intr_num)) {
        return ESP_CPU_INTR_DESC_FLAG_RESVD;
    }

    extern intptr_t _mtvt_table[48];
    extern intptr_t _interrupt_handler;

    /* The first 16 entries of the array are internal interrupt, ignore them  */
    const intptr_t destination = _mtvt_table[16 + intr_num];

    return (destination != (intptr_t)&_interrupt_handler) ? ESP_CPU_INTR_DESC_FLAG_RESVD : 0;
}



#else // !SOC_INT_CLIC_SUPPORTED

#include "esp_cpu.h"
#include "riscv/instruction_decode.h"

/**
 * @brief Checks whether the given interrupt number is reserved either in the given mask or in the
 *        _vector_table, which contains the routines the CPU will jump to when an interrupt or an exception
 *        occurs, on RISC-V targets.
 *
 * @param intr_num  Interrupt number to check, in range 0~32
 * @param rsvd_mask Reserved interrupt mask, where bit i is 1 if interrupt i is reserved.
 *
 * @returns ESP_CPU_INTR_DESC_FLAG_RESVD if the interrupt is reserved, 0 else
 */
static inline uint32_t esp_riscv_intr_num_flags(int intr_num, uint32_t rsvd_mask)
{
    if (rsvd_mask & BIT(intr_num)) {
        return ESP_CPU_INTR_DESC_FLAG_RESVD;
    }

    extern intptr_t _vector_table[32];
    extern int _interrupt_handler;
    const intptr_t pc = (intptr_t) &_vector_table[intr_num];

    /* JAL instructions are relative to the PC they are executed from. */
    const intptr_t destination = pc + riscv_decode_offset_from_jal_instruction(pc);

    return (destination != (intptr_t)&_interrupt_handler) ? ESP_CPU_INTR_DESC_FLAG_RESVD : 0;
}

#endif // SOC_INT_CLIC_SUPPORTED

#endif // CONFIG_IDF_TARGET_ARCH_RISCV
