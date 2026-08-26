/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#define TEST_COPROCESSOR_REGISTERS (SOC_CPU_HAS_FPU || SOC_CPU_HAS_PIE)
#define TEST_HWLOOP_INSTRUCTIONS (__riscv && SOC_CPU_HAS_HWLOOP)

int var_1;
int var_2;
int do_panic;
int start_testing;
#if TEST_COPROCESSOR_REGISTERS
int do_test_coproc_regs;
#endif
#if TEST_HWLOOP_INSTRUCTIONS
int do_test_xesppie_loops;
#endif

void foo(void)
{
    var_2+=2;
    var_2--;
}

void test_xesppie_loops(void);
void test_coproc_regs(void);

void app_main(void)
{
    printf("waiting start_testing variable to be changed.\n");
    while (!start_testing) { /* set via GDB */
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

#if TEST_HWLOOP_INSTRUCTIONS
    if (do_test_xesppie_loops) { /* set via GDB */
        test_xesppie_loops();
        return;
    }
#endif

#if TEST_COPROCESSOR_REGISTERS
    if (do_test_coproc_regs) { /* set via GDB */
        test_coproc_regs();
        return;
    }
#endif

    while(1) {
        var_1++;
        if (var_1 % 10 == 0) {
label_1:
            foo();
        }
label_3:
        if (do_panic) {
            char *t = NULL;
label_5:
            *t = 'X';
        }
    }
}
