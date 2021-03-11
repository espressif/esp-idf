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

#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include <xtensa/hal.h>

/*
 * esp_backtrace_get_start(uint32_t *pc, uint32_t *sp, uint32_t *next_pc)
 *
 *     High Addr
 * ..................
 * |     i-3 BS     |
 * |   i-1 locals   | Function B
 * .................. i-1 SP
 * |     i-2 BS     |
 * |    i locals    | Function A (Start of backtrace)
 * ------------------ i SP
 * |     i-1 BS     |
 * |   i+1 locals   | Backtracing function (e.g. esp_backtrace_print())
 * ------------------ i+1 SP
 * |      i BS      |
 * |   i+2 locals   | esp_backtrace_get_start() <- This function
 * ------------------ i+2 SP
 * |     i+1 BS     |
 * |   i+3 locals   | xthal_window_spill()
 * ------------------ i+3 SP
 * .................. Low Addr
 */
    .section    .iram1, "ax"
    .align      4
    .global     esp_backtrace_get_start
    .type       esp_backtrace_get_start, @function
esp_backtrace_get_start:
    entry   a1, 32
    call8   xthal_window_spill  //Spill registers onto stack (excluding this function)
    //a2, a3, a4 should be out arguments for i SP, i PC, i-1 PC respectively. Use a5 and a6 as scratch
    l32e    a5, sp, -16         //Get i PC, which is ret addres of i+1
    s32i    a5, a2, 0           //Store i PC to arg *pc
    l32e    a6, sp, -12         //Get i+1 SP. Used to access i BS
    l32e    a5, a6, -12         //Get i SP
    s32i    a5, a3, 0           //Store i SP to arg *sp
    l32e    a5, a6, -16         //Get i-1 PC, which is ret address of i
    s32i    a5, a4, 0           //Store i-1 PC to arg *next_pc
    retw
