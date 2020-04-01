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

    .extern xtensa_shared_stack
    .extern xtensa_shared_stack_callback
    .extern xtensa_shared_stack_function_done
    .extern xtensa_shared_stack_env
    .extern longjmp
    .text


/* extern void esp_shared_stack_invoke_function(void) */

    .globl     esp_shared_stack_invoke_function
    .type       esp_shared_stack_invoke_function,@function
    .align      4
esp_shared_stack_invoke_function:

    #ifndef __XTENSA_CALL0_ABI__
    movi    a0, 0                   /* must not rotate the window here, */
                                    /* the state of execution for shared stack */
                                    /* functions will be completely destroyed at end */
    movi    a6, xtensa_shared_stack
    l32i    sp, a6, 0               /* load shared stack pointer */
    movi    a12, xtensa_shared_stack_callback
    l32i    a12, a12, 0
    callx4  a12                     /* call user function */
    movi    a6, xtensa_shared_stack_function_done
    movi    a7, 1
    s32i    a7, a6, 0               /* hint the function was finished */
    movi    a6, xtensa_shared_stack_env
    movi    a7, 0
    movi    a12, longjmp
    callx4  a12                 /* jump to last clean state previously saved */
    ret
    #else
    #error "this code is written for Window ABI"
    #endif
