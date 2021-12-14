/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */



#include "sdkconfig.h"
#include <sys/param.h>
#include <string.h>
#include "esp_log.h"
#include "test_utils.h"
#include "unity.h"
#include "esp_heap_caps.h"

#if CONFIG_SPIRAM
#include "spiram.h"


const static char *TAG = "PSRAM";

#if CONFIG_SPIRAM_MODE_OCT
#define TEST_ALLOC_SIZE    (4 * 1024 * 1024)
#else
#define TEST_ALLOC_SIZE    (1 * 1024 * 1024)
#endif


TEST_CASE("test psram heap allocable","[psram]")
{
    uint32_t *ext_buffer = (uint32_t *)heap_caps_calloc(TEST_ALLOC_SIZE, 1, MALLOC_CAP_SPIRAM);
    TEST_ASSERT(ext_buffer);

    uintptr_t start = (uintptr_t)ext_buffer;
    uintptr_t end = (uintptr_t)ext_buffer + TEST_ALLOC_SIZE;
    ESP_LOGI(TAG, "test ext buffer start addr is %x, end addr is %x", start, end);
    TEST_ASSERT((start >= SOC_EXTRAM_DATA_LOW) && (end <= SOC_EXTRAM_DATA_HIGH));


    for (int i = 0; i < TEST_ALLOC_SIZE / sizeof(uint32_t); i++) {
        ext_buffer[i] = (i + 1) ^ 0xaaaaaaaa;
    }

    for (int i = 0; i < TEST_ALLOC_SIZE / sizeof(uint32_t); i++) {
        TEST_ASSERT(ext_buffer[i] == ((i + 1) ^ 0xaaaaaaaa));
    }

    free(ext_buffer);
}

#endif  //#if CONFIG_SPIRAM
