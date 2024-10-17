/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <stdio.h>
#include "esp_memory_utils.h"

extern void func2(void);
extern void func3(void);
extern void func4(void);

extern const int const_array[];

void app_main(void)
{
    func2();
    func3();
    func4();
    if (esp_ptr_in_dram(const_array)) {
        printf("const_array placed in dram\n");
    } else {
        printf("const_array NOT placed in dram\n");
    }
}
