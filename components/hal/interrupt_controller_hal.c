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

#include "hal/interrupt_controller_hal.h"
#include "soc/soc_caps.h"

#if __riscv
#include "riscv/instruction_decode.h"

static bool is_interrupt_number_reserved(int interrupt_number)
{
    // Workaround to reserve interrupt number 1 for Wi-Fi, 5,8 for Bluetooth, 6 for "permanently disabled interrupt"
    // [TODO: IDF-2465]
    const uint32_t reserved = BIT(1) | BIT(5) | BIT(6) | BIT(8);
    if (reserved & BIT(interrupt_number)) {
        return true;
    }

    extern int _vector_table;
    extern int _interrupt_handler;
    const intptr_t pc = (intptr_t)(&_vector_table + interrupt_number);

    /* JAL instructions are relative to the PC there are executed from. */
    const intptr_t destination = pc + riscv_decode_offset_from_jal_instruction(pc);

    return destination != (intptr_t)&_interrupt_handler;
}
#endif

int_type_t interrupt_controller_hal_desc_type(int interrupt_number)
{
#ifndef SOC_CPU_HAS_FLEXIBLE_INTC
    const int_desc_t *int_desc = interrupt_controller_hal_desc_table();
    return (int_desc[interrupt_number].type);
#else
    return (INTTP_NA);
#endif
}

int interrupt_controller_hal_desc_level(int interrupt_number)
{
#ifndef SOC_CPU_HAS_FLEXIBLE_INTC
    const int_desc_t *int_desc = interrupt_controller_hal_desc_table();
    return (int_desc[interrupt_number].level);
#else
    return 1;
#endif
}

int_desc_flag_t interrupt_controller_hal_desc_flags(int interrupt_number, int cpu_number)
{
#ifndef SOC_CPU_HAS_FLEXIBLE_INTC
    const int_desc_t *int_desc = interrupt_controller_hal_desc_table();
    return (int_desc[interrupt_number].cpuflags[cpu_number]);
#else
#if __riscv
    return is_interrupt_number_reserved(interrupt_number) ? INTDESC_RESVD : INTDESC_NORMAL;
#else
    return INTDESC_NORMAL;
#endif
#endif
}
