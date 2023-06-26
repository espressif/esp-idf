/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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

#include "ulp_riscv.h"
#include "ulp_riscv_utils.h"
#include "ulp_riscv_print.h"

#include "ulp_riscv_uart_ulp_core.h"
#include "sdkconfig.h"

static ulp_riscv_uart_t s_print_uart;

int main (void)
{
    ulp_riscv_uart_cfg_t cfg = {
        .tx_pin = CONFIG_EXAMPLE_UART_TXD,
    };

    ulp_riscv_uart_init(&s_print_uart, &cfg);
    ulp_riscv_print_install((putc_fn_t)ulp_riscv_uart_putc, &s_print_uart);

    int cnt = 0;
    while(1) {

        ulp_riscv_print_str("Hello World from ULP-RISCV!\n");
        ulp_riscv_print_str("Cnt: 0x");
        ulp_riscv_print_hex(cnt);
        ulp_riscv_print_str("\n");

        cnt++;
        ulp_riscv_delay_cycles(1000 * ULP_RISCV_CYCLES_PER_MS);
    }
}
