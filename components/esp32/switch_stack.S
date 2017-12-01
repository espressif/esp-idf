// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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

.text
    /* Function to reset the stack pointer to beginning of bootloader bss,
       and jump to a given entry point function.
    */
    .global     switch_stack_and_jump
    .type       switch_stack_and_jump, @function
    .section ".iram1"
    .align      4

switch_stack_and_jump:
    entry sp, 16 /* We are called via call8/etc, so need to rotate */

    /* a2: new stack area, a3: size, a4: jump target */

    /* save args. uses m0-m2 as scratch regs. */
    wsr a2, m0
    wsr a3, m1
    wsr a4, m2

    /* Reset register window and clobber registers */
    movi a0, 1
    wsr  a0, WindowStart
    movi a0, 0
    wsr  a0, WindowBase

    /* reload args */
    rsr a2, m0
    rsr a3, m1
    rsr a4, m2

    /* initialize a call8-compatible stack, see ISA RM 8.7 */
    add  a3, a2, a3    /* a3 points to the end of stack now */
    addi sp, a3, -32  /* save area */
    addi a3, sp, 32   /* 16 past save area */
    s32e a3, sp, -12  /* write pointer to end of stack */
    isync

    callx8 a4
