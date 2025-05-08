/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "secure_service_num.h"
#include "esp_rom_sys.h"
#include "esp_attr.h"

#include "multi_heap.h"

void NOINLINE_ATTR _ss_dummy_secure_service(int a, int b, int c, int d, int e, int f, int g, int h, int *i)
{
    esp_rom_printf("Dummy secure service\n");
    *i = a + b + c + d + e + f + g + h;

    esp_rom_printf("[TEE] Heap: Free - %uB | Min free - %uB\n", esp_tee_heap_get_free_size(), esp_tee_heap_get_min_free_size());
    esp_tee_heap_dump_info();
}
