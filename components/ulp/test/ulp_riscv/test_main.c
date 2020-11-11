// Copyright 2010-2020 Espressif Systems (Shanghai) PTE LTD
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "ulp_riscv/ulp_riscv.h"
#include "ulp_riscv/ulp_riscv_utils.h"

typedef enum{
    RISCV_READ_WRITE_TEST = 1,
    RISCV_DEEP_SLEEP_WAKEUP_TEST,
    RISCV_LIGHT_SLEEP_WAKEUP_TEST,
    RISCV_NO_COMMAND,
} riscv_test_commands_t;

typedef enum {
    RISCV_COMMAND_OK = 1,
    RISCV_COMMAND_NOK,
    RISCV_COMMAND_INVALID,
} riscv_test_command_reply_t;

#define XOR_MASK 0xDEADBEEF

volatile riscv_test_commands_t main_cpu_command = RISCV_NO_COMMAND;
volatile riscv_test_command_reply_t main_cpu_reply = RISCV_COMMAND_INVALID;
volatile uint32_t riscv_test_data_in = 0;
volatile uint32_t riscv_test_data_out = 0;

void handle_commands(riscv_test_commands_t cmd)
{
    switch (cmd) {
        case RISCV_READ_WRITE_TEST:
            riscv_test_data_out =riscv_test_data_in ^ XOR_MASK;
            main_cpu_reply =  RISCV_COMMAND_OK;
            break;

        case RISCV_DEEP_SLEEP_WAKEUP_TEST:
        case RISCV_LIGHT_SLEEP_WAKEUP_TEST:
            main_cpu_reply = RISCV_COMMAND_OK;
            break;

        default:
            main_cpu_reply = RISCV_COMMAND_NOK;
            break;
    }
    ulp_riscv_wakeup_main_processor();
}

int main (void)
{
    while (1) {
        handle_commands(main_cpu_command);
        break;
    }

    /* ulp_riscv_shutdown() is called automatically when main exits */
    return 0;
}
