// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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

    .section .text

    .global esp_shared_stack_invoke_function
    .type esp_shared_stack_invoke_function, @function
esp_shared_stack_invoke_function:
    /* save current stack and return address */
    mv t0, sp
    mv t1, ra

    /* Set shared stack as new stack pointer */
    mv sp, a1

    /* store the ra and previous stack pointer in a safe place
       stack pointer for riscv should always be 16 byte aligned */
    addi sp,sp,-16
    sw  t0, 0(sp)
    sw  t1, 4(sp)

    /* call the subroutine */
    jalr a0, 0

    /* gets the ra and stack pointer saved previously */
    lw  t0, 0(sp)
    lw  t1, 4(sp)
    addi sp, sp, 16

    /* restore both ra and real stack pointer of current task */
    mv ra, t1
    mv sp, t0
    ret
