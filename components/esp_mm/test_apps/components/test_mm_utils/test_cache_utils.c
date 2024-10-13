/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <inttypes.h>
#include "test_mm_utils.h"

const static char *TAG = "cache_utils";

esp_err_t test_set_buffer_dirty(intptr_t vaddr_start, size_t size)
{
    if (((vaddr_start % 32) != 0) || ((size % 32) != 0)) {
        ESP_LOGE(TAG, "addr or size not 4B aligned");
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t *vaddr = (uint32_t *)vaddr_start;
    printf("vaddr: %p, size: 0x%zx\n", vaddr, size);

    for (int i = 0; i < size / sizeof(uint32_t); i++) {
        vaddr[i] = 0xcc;
    }

    return ESP_OK;
}
