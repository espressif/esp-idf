/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include "esp_log.h"

static const char *TAG = "boot-wrapper";

/**
 * Declare the following symbol in order to have access to the original function implementation
 */
extern void __real_bootloader_print_banner(void);

/**
 * Extend the bootloader's print banner function.
 */
void __wrap_bootloader_print_banner(void)
{
    /* Let's first let the original code run */
    __real_bootloader_print_banner();
    /* and then extend it by printing another message */
    ESP_LOGI(TAG, "message from a bootloader wrapper");
}
