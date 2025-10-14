/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* ULP-RISC-V example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   This code runs on ULP-RISC-V  coprocessor
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "ulp_riscv.h"
#include "ulp_riscv_utils.h"
#include "ulp_riscv_gpio.h"

uint32_t edge_count = 0;
uint32_t edge_limit = 0;

int main(void)
{
    /*
    * Written pulse counter in assembly to make it optimized and achieve faster speed
    * Assembly registers usage
    * a2 - Stores GPIO register address
    * a3 - Stores previous GPIO level
    * a4 - Stores current GPIO level
    * a5 - Used to count number of edges
    * a6 - Used to check and exit loop after HP_CORE_PULSE_COUNT x 2 edges count limit is reached
    * NOTE: While reading GPIO pin, logical AND operation is not used to get exact pin level as it takes additional
    *       CPU cycles. Instead, GPIO register is right shifted to get the pin level bit at LSB position,
    *       which means the result stored in a4 will be logical OR of other GPIO pin levels as well, but that
    *       doesn't matter as we are only interested in change of state on the input pin and the other pins
    *       are left uninitialized.
    */
    __asm__ volatile (
        "lui    a2, 0xa\n"                      /* a2 = 0xa000 (upper 20 bits of GPIO register address) */
        "addi   a2, a2, 1060\n"                 /* a2 = 0xa424 (full GPIO register address for GPIO_PULSE_INPUT) */
        "li     a3, 0\n"                        /* a3 = prev (initialize previous GPIO level to 0) */
        "li     a5, 0\n"                        /* a5 = edge counter (initialize to 0) */
        "lw     a6, 0(%[edge_limit_addr])\n"    /* a6 = edge count limit (HP_CORE_PULSE_COUNT * 2) */
    "loop:\n"
        "lw     a4, 0(a2)\n"                    /* a4 = *(uint32_t*)0xa424 (read GPIO register) */
        "srli   a4, a4, 11\n"                   /* a4 = a4 >> 11 (shift right to get GPIO1 level bit at LSB) */
        "beq    a4, a3, loop\n"                 /* if (a4 == prev) loop (no edge detected) */
        "addi   a5, a5, 1\n"                    /* a5++ (increment edge counter) */
        "mv     a3, a4\n"                       /* prev = a4 (update previous GPIO level) */
        "blt    a5, a6, loop\n"                 /* if (a5 < edge_limit) loop (continue until limit) */
        "sw     a5, 0(%[edge_count_addr])\n"    /* edge_count = a5 (store final edge count to memory) */
        :
        : [edge_count_addr] "r" (&edge_count), [edge_limit_addr] "r" (&edge_limit)
        : "a2", "a3", "a4", "a5", "a6", "memory"
    );

    return 0;
}
