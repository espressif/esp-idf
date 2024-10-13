/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include "esp_log.h"
#include "esp_attr.h"

static const char *TAG = "fpga_rng";

/* Placed in IRAM since test_apps expects it to be */
void IRAM_ATTR bootloader_fill_random(void *buffer, size_t length)
{
    ESP_EARLY_LOGE(TAG, "Project configuration is for internal FPGA use, RNG will not work");

    uint8_t *buffer_bytes = (uint8_t *)buffer;
    for (int i = 0; i < length; i++) {
        buffer_bytes[i] = 0x5A;
    }
}

/**
 * @brief No-op function, used to force linking this file
 *
 */
void esp_common_include_fpga_overrides_rng(void)
{
}
