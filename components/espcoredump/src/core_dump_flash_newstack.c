// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <setjmp.h>
#include <string.h>
#include "soc/soc.h"
#include "soc/soc_memory_layout.h"
#include "esp_core_dump_priv.h"

#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH && CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY

const static DRAM_ATTR char TAG[] __attribute__((unused)) = "esp_core_dump_flash_newstack";

/* Temporary stack used by the core dump; Stack size is chosen to be sufficient in case logging is enabled. */
#define CORE_DUMP_STACK_SIZE 1024
static uint8_t __attribute__((aligned(16))) core_dump_stack[CORE_DUMP_STACK_SIZE];

/* This jmp_buf is used to jump *into* switch_stack_helper function, simultaneously setting
 * the stack pointer to the temporary stack. It is initialized manually, not using setjmp.
 */
static jmp_buf switch_stack_jmp;

/* This jmp_buf is used to return to the original stack (in external RAM) after the core dump is done. */
static jmp_buf return_jmp;

/* Internals of jmp_buf for Xtensa Window ABI. See setjmp.S for details */
struct jmp_buf_impl {
    int regs[12];
    int save[4];
    void *return_address;
};

/* Defined in core_dump_flash.c if CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY is set */
extern void esp_core_dump_to_flash_inner(XtExcFrame *frame);

/* We are going to jump to switch_stack_helper+3 (i.e. past the ENTRY instruction)
 * using longjmp(switch_stack_jmp). Execution will start on the temporary stack.
 */
static void switch_stack_helper(void* arg)
{
    /* Using the new stack now, can perform the core dump */
    esp_core_dump_to_flash_inner((XtExcFrame*) arg);
    /* Jump back to where setjmp(return_jmp) was called, switching to the original stack */
    longjmp(return_jmp, 1);
}

void esp_core_dump_to_flash(XtExcFrame *frame)
{
    if (esp_ptr_external_ram(get_sp())) {
        ESP_COREDUMP_LOGI("Stack in external RAM, switching to new stack %p - %p", core_dump_stack, core_dump_stack + sizeof(core_dump_stack));
        /* Prepare new stack */
        uint32_t* stack_top = (uint32_t*) (core_dump_stack + sizeof(core_dump_stack)/sizeof(core_dump_stack[0]));
        stack_top -= 4;
        /* Prepare jmp_buf to jump into the switch_stack_helper, simultaneously switching to the new stack */
        struct jmp_buf_impl* jb = (struct jmp_buf_impl*) &switch_stack_jmp;
        jb->regs[1] = (int) stack_top;              /* SP */
        jb->regs[2] = (int) frame;                  /* function argument (a2) */
        jb->save[1] = ((int)stack_top) + 32;        /* pointer to base save area for the current frame */
        jb->return_address = ((uint8_t*)&switch_stack_helper) + 3; /* First instruction of switch_stack_helper, after ENTRY */

        /* Set up return_jmp for returning here */
        if (!setjmp(return_jmp)) {
            longjmp(switch_stack_jmp, 1);
        }
    } else {
        ESP_COREDUMP_LOGI("Stack in DRAM");
        esp_core_dump_to_flash_inner(frame);
    }
}

#endif // CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH && CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
