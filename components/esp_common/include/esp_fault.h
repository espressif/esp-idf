// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "sdkconfig.h"
#include "soc/rtc_cntl_reg.h"
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

#define _ESP_FAULT_RESET()  do {                                \
        REG_WRITE(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_SYS_RST);  \
        asm volatile("ill; ill; ill;");                         \
    } while(0)

#else // ESP_FAULT_ASSERT_DEBUG

#warning "Enabling ESP_FAULT_ASSERT_DEBUG makes ESP_FAULT_ASSERT() less effective"

#define _ESP_FAULT_RESET()  do {                                        \
        esp_rom_printf("ESP_FAULT_ASSERT %s:%d\n", __FILE__, __LINE__); \
        asm volatile("ill;");                                           \
    } while(0)

#endif // ESP_FAULT_ASSERT_DEBUG

#ifdef __cplusplus
}
#endif
