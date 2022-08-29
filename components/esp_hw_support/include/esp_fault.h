/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "esp_rom_sys.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Assert a condition is true, in a way that should be resistant to fault injection for
 * single fault attacks.
 *
 * - Expands CONDITION multiple times (condition must have no side effects)
 * - Compiler is told all registers are invalid before evaluating CONDITION each time, to avoid a fault
 *   causing a misread of a register used in all three evaluations of CONDITION.
 * - If CONDITION is ever false, a system reset is triggered.
 *
 * @note Place this macro after a "normal" check of CONDITION that will fail with a normal error
 * message. This is the fallback in case a fault injection attack skips or corrupts the result of
 * that check. (Although ensure that an attacker can't use fault injection to skip past the "normal"
 * error message, to avoid this check entirely.)
 *
 * @note This macro increases binary size and is slow and should be used sparingly.
 *
 * @note This macro does not guarantee fault injection resistance. In particular CONDITION must be
 * chosen carefully - a fault injection attack which sets CONDITION to true will not be detected by
 * this macro. Care must also be taken that an attacker can't use a fault to completely bypass calling
 * whatever function tests ESP_FAULT_ASSERT.
 *
 * @note This is difficult to debug as a failure triggers an instant software reset, and UART output
 * is often truncated (as FIFO is not flushed). Define the ESP_FAULT_ASSERT_DEBUG macro to debug any
 * failures of this macro due to software bugs.
 *
 * @param CONDITION A condition which will evaluate true unless an attacker used fault injection to skip or corrupt some other critical system calculation.
 *
 */
#define ESP_FAULT_ASSERT(CONDITION) do {                \
        asm volatile ("" ::: "memory");                 \
        if(!(CONDITION)) _ESP_FAULT_RESET();            \
        asm volatile ("" ::: "memory");                 \
        if(!(CONDITION)) _ESP_FAULT_RESET();            \
        asm volatile ("" ::: "memory");                 \
        if(!(CONDITION)) _ESP_FAULT_RESET();            \
} while(0)

#ifndef CONFIG_IDF_TARGET_ARCH_RISCV
#define _ESP_FAULT_ILLEGAL_INSTRUCTION asm volatile("ill.n; ill.n; ill.n; ill.n; ill.n; ill.n; ill.n;")
#else
#define _ESP_FAULT_ILLEGAL_INSTRUCTION asm volatile("unimp; unimp; unimp; unimp; unimp;")
#endif

// Uncomment this macro to get debug output if ESP_FAULT_ASSERT() fails
//
// Note that uncommenting this macro reduces the anti-FI effectiveness
//
//#define ESP_FAULT_ASSERT_DEBUG

/* Internal macro, purpose is to trigger a system reset if an inconsistency due to fault injection
   is detected.

   Illegal instruction opcodes are there as a fallback to crash the CPU in case it doesn't
   reset as expected.
*/
#ifndef ESP_FAULT_ASSERT_DEBUG

#define _ESP_FAULT_RESET()  do {        \
        esp_rom_software_reset_system();       \
        _ESP_FAULT_ILLEGAL_INSTRUCTION; \
    } while(0)

#else // ESP_FAULT_ASSERT_DEBUG

#warning "Enabling ESP_FAULT_ASSERT_DEBUG makes ESP_FAULT_ASSERT() less effective"

#define _ESP_FAULT_RESET()  do {                                        \
        esp_rom_printf("ESP_FAULT_ASSERT %s:%d\n", __FILE__, __LINE__); \
        _ESP_FAULT_ILLEGAL_INSTRUCTION;                                 \
    } while(0)

#endif // ESP_FAULT_ASSERT_DEBUG

#ifdef __cplusplus
}
#endif
