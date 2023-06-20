/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*
 Tests for D/IRAM support in heap capability allocator
*/

#include <esp_types.h>
#include <stdio.h>
#include "unity.h"
#include "esp_heap_caps.h"
#include "soc/soc_memory_layout.h"

#define ALLOC_SZ 1024

#ifndef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
static void *malloc_block_diram(uint32_t caps)
{
    void *attempts[256] = { 0 }; // Allocate up to 256 ALLOC_SZ blocks to exhaust all non-D/IRAM memory temporarily
    int count = 0;
    void *result;

    while(count < sizeof(attempts)/sizeof(void *)) {
        result = heap_caps_malloc(ALLOC_SZ, caps);
        TEST_ASSERT_NOT_NULL_MESSAGE(result, "not enough free heap to perform test");

        if (esp_ptr_in_diram_dram(result) || esp_ptr_in_diram_iram(result)) {
            break;
        }

        attempts[count] = result;
        result = NULL;
        count++;
    }

    for (int i = 0; i < count; i++) {
        free(attempts[i]);
    }

    TEST_ASSERT_NOT_NULL_MESSAGE(result, "not enough D/IRAM memory is free");
    return result;
}

TEST_CASE("Allocate D/IRAM as DRAM", "[heap][qemu-ignore]")
{
    uint32_t *dram = malloc_block_diram(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

    for (int i = 0; i < ALLOC_SZ / sizeof(uint32_t); i++) {
        uint32_t v = i + 0xAAAA;
        dram[i] = v;
        volatile uint32_t *iram = esp_ptr_diram_dram_to_iram(dram + i);
        TEST_ASSERT_EQUAL(v, dram[i]);
        TEST_ASSERT_EQUAL(v, *iram);
        *iram = UINT32_MAX;
        TEST_ASSERT_EQUAL(UINT32_MAX, *iram);
        TEST_ASSERT_EQUAL(UINT32_MAX, dram[i]);
    }

    free(dram);
}

TEST_CASE("Allocate D/IRAM as IRAM", "[heap][qemu-ignore]")
{
    uint32_t *iram = malloc_block_diram(MALLOC_CAP_EXEC);

    for (int i = 0; i < ALLOC_SZ / sizeof(uint32_t); i++) {
        uint32_t v = i + 0xEEE;
        iram[i] = v;
        volatile uint32_t *dram = esp_ptr_diram_iram_to_dram(iram + i);
        TEST_ASSERT_EQUAL_HEX32(v, iram[i]);
        TEST_ASSERT_EQUAL_HEX32(v, *dram);
        *dram = UINT32_MAX;
        TEST_ASSERT_EQUAL_HEX32(UINT32_MAX, *dram);
        TEST_ASSERT_EQUAL_HEX32(UINT32_MAX, iram[i]);
    }

    free(iram);
}
#endif // CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
