/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "ulp_lp_core_print.h"
#include "ulp_lp_core_memory_shared.h"
#include "test_shared.h"

int main(void)
{
    ulp_lp_core_memory_shared_cfg_t *shared_cfg = ulp_lp_core_memory_shared_cfg_get();
    lp_core_printf("ULP shared memory address: %p\n", shared_cfg);

    volatile uint8_t* shared_mem = (uint8_t*)shared_cfg;
    for (int i = 0; i < sizeof(ulp_lp_core_memory_shared_cfg_t); i++) {
        if (shared_mem[i] != SHARED_MEM_INIT_VALUE) {
            lp_core_printf("Test failed: expected %X, got %X at %d\n", SHARED_MEM_INIT_VALUE, shared_mem[i], i);
            return 0;
        }
    }

    for (int i = 0; i < sizeof(ulp_lp_core_memory_shared_cfg_t); i++) {
        shared_mem[i] = SHARED_MEM_END_VALUE;
    }

    lp_core_printf("ULP shared memory test passed\n");

    return 0;
}
