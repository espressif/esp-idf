// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#include <freertos/xtensa_context.h>
    
    .extern esp_clear_watchpoint
    .text

/**
 * extern void switch_stack_enter(portSTACK_TYPE *stack, portSTACK_TYPE *backup_stack);
 */
    .globl     esp_switch_stack_enter
    .type       esp_switch_stack_enter,@function
    .align      4
esp_switch_stack_enter:

    #ifndef __XTENSA_CALL0_ABI__
        entry   sp, 0x10
        mov   a4, a1
        s32i   a4, a3, 0 /* on a3 there is a safe place to save the current stack */
        l32i   a4, a2, 0 /* obtains the user allocated stack buffer */
        mov   a1, a4     /* sp register now contains caller specified stack */
        retw
    #else 
        #error "this code is written for Window ABI"
    #endif

/**
 * extern void switch_stack_exit(portSTACK_TYPE *backup_stack);
 */
    .globl  esp_switch_stack_exit
    .type   esp_switch_stack_exit,@function
    .align  4
esp_switch_stack_exit:

    #ifndef __XTENSA_CALL0_ABI__
        entry   sp, 0x10

#if CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK        
        movi a6, 2
        movi a4, esp_clear_watchpoint
        callx4 a4 /* clear the watchpoint before releasing stack */
#endif

        l32i a4, a2, 0  /* recover the original task stack */
        mov a1, a4      /* put it on sp register again */
        retw
    #else 
        #error "this code is written for Window ABI"
    #endif
