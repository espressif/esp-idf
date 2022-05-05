/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 Test for LoadStore exception handlers. This test performs unaligned load and store in 32bit aligned addresses
*/

#include <esp_types.h>
#include <stdio.h>
#include <esp_heap_caps.h>
#include "sdkconfig.h"
#include "esp_random.h"
#include "unity.h"

#if CONFIG_IDF_TARGET_ARCH_XTENSA
#include "freertos/xtensa_api.h"

#ifdef CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY
TEST_CASE("LoadStore Exception handler", "[freertos]")
{
    int32_t val0 = 0xDEADBEEF;
    int32_t val1 = 0xBBAA9988;
    int32_t val2 = 0x77665544;
    int32_t val3 = 0x33221100;

    int8_t val8_0 = val0 & 0xff;
    int8_t val8_1 = val1 & 0xff;
    int8_t val8_2 = val2 & 0xff;
    int8_t val8_3 = val3 & 0xff;

    int16_t val16_0 = val0 & 0xffff;
    int16_t val16_1 = val1 & 0xffff;
    int16_t val16_2 = val2 & 0xffff;
    int16_t val16_3 = val3 & 0xffff;

    uint32_t largest_free = heap_caps_get_largest_free_block(MALLOC_CAP_IRAM_8BIT);

    int8_t *arr = heap_caps_malloc(largest_free * sizeof(int8_t), MALLOC_CAP_IRAM_8BIT);
    TEST_ASSERT(arr != NULL);

    int8_t *arr8 =  arr;
    int16_t *arr16 = (int16_t *)arr;
    int32_t *arr32 = (int32_t *)arr;

    for (int i = 0; i < 1024; i++) {

        // LoadStoreError

        uint32_t offset = esp_random() % (largest_free - 20);
        uint32_t offset8, offset16, offset32;

        // Get word aligned offset
        offset8 = offset & ~3;
        offset16 = offset8 / 2;
        offset32 = offset8 / 4;

        arr8[offset8] = val8_0;
        arr8[offset8+1] = val8_1;
        arr8[offset8+2] = val8_2;
        arr8[offset8+3] = val8_3;

        // Just to make sure compiler doesn't read stale data
        asm volatile("memw\n");
        TEST_ASSERT_EQUAL(val8_0, arr8[offset8]);
        TEST_ASSERT_EQUAL(val8_1, arr8[offset8+1]);
        TEST_ASSERT_EQUAL(val8_2, arr8[offset8+2]);
        TEST_ASSERT_EQUAL(val8_3, arr8[offset8+3]);

        arr16[offset16] = val16_0;
        arr16[offset16+1] = val16_1;
        arr16[offset16+2] = val16_2;
        arr16[offset16+3] = val16_3;

        // Just to make sure compiler doesn't read stale data
        asm volatile("memw\n");
        TEST_ASSERT_EQUAL(val16_0, arr16[offset16]);
        TEST_ASSERT_EQUAL(val16_1, arr16[offset16+1]);
        TEST_ASSERT_EQUAL(val16_2, arr16[offset16+2]);
        TEST_ASSERT_EQUAL(val16_3, arr16[offset16+3]);

        // LoadStoreAlignement Error

        // Check that it doesn't write to adjacent bytes
        int8_t *ptr8_0 = (void *)&arr8[offset8];
        int8_t *ptr8_1 = (void *)&arr8[offset8] + 5;
        int8_t *ptr8_2 = (void *)&arr8[offset8] + 10;
        int8_t *ptr8_3 = (void *)&arr8[offset8] + 15;

        *ptr8_0 = 0x73;
        *ptr8_1 = 0x73;
        *ptr8_2 = 0x73;
        *ptr8_3 = 0x73;

        int16_t *ptr16_0 = (void *)&arr16[offset16] + 1;
        int16_t *ptr16_1 = (void *)&arr16[offset16] + 3;

        *ptr16_0 = val16_0;
        *ptr16_1 = val16_1;

        // Just to make sure compiler doesn't read stale data
        asm volatile("memw\n");
        TEST_ASSERT_EQUAL(val16_0, *ptr16_0);
        TEST_ASSERT_EQUAL(0x73, *ptr8_0);
        TEST_ASSERT_EQUAL(val16_1, *ptr16_1);
        TEST_ASSERT_EQUAL(0x73, *ptr8_1);

        int32_t *ptr32_0 = (void *)&arr32[offset32] + 1;
        int32_t *ptr32_1 = (void *)&arr32[offset32] + 6;
        int32_t *ptr32_2 = (void *)&arr32[offset32] + 11;
        *ptr32_0 = val0;
        *ptr32_1 = val1;
        *ptr32_2 = val2;

        // Just to make sure compiler doesn't read stale data
        asm volatile ("memw");
        TEST_ASSERT_EQUAL(0x73, *ptr8_0);
        TEST_ASSERT_EQUAL(val0, *ptr32_0);
        TEST_ASSERT_EQUAL(0x73, *ptr8_1);
        TEST_ASSERT_EQUAL(val1, *ptr32_1);
        TEST_ASSERT_EQUAL(0x73, *ptr8_2);
        TEST_ASSERT_EQUAL(val2, *ptr32_2);
        TEST_ASSERT_EQUAL(0x73, *ptr8_3);
    }

    TEST_ASSERT_TRUE(heap_caps_check_integrity_all(true));
    heap_caps_free(arr);
}
#endif // CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY
#endif // CONFIG_IDF_TARGET_ARCH_XTENSA
