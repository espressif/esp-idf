// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#include "riscv/rvruntime-frames.h"
#include "esp_private/panic_internal.h"

#define MCAUSE_ILLEGAL_INSTRUCTION  2

extern void esp_panic_handler(panic_info_t *info);
volatile bool g_override_illegal_instruction = false;

void __real_esp_panic_handler(panic_info_t *info);

void return_from_panic_handler(RvExcFrame *frm) __attribute__((noreturn));

/* Memprot test specific IllegalInstruction exception handler:
 * when testing the protection against a code execution, sample code
 * is being injected into various memory regions which produces
 * Illegal instruction on execution attempt. Such a result is expected
 * but it causes system reboot in the standard panic handler.
 * The following variant of panic handling simply returns back to the
 * next instruction and continues normal execution.
 *
 * NOTE: if Illegal instruction comes from a different source than the testing code
 * the behavior is undefined
 * */
void __wrap_esp_panic_handler(panic_info_t *info)
{
    RvExcFrame *frm = (RvExcFrame *)info->frame;
    if ( frm->mcause == MCAUSE_ILLEGAL_INSTRUCTION && g_override_illegal_instruction == true ) {
        /* Return from exception to the return address that called the faulting function.
        */
        frm->mepc = frm->ra;

        /* Restore the CPU state and return from the exception.
        */
        return_from_panic_handler(frm);
    } else {
        __real_esp_panic_handler(info);
    }
}
