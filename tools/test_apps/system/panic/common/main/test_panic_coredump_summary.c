/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "esp_core_dump.h"
#include "esp_err.h"

#include "test_panic.h"

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH
void test_setup_coredump_summary(void)
{
    if (esp_core_dump_image_erase() != ESP_OK) {
        die("Coredump image can not be erased!");
    }
    assert(0);
}

void test_coredump_summary(void)
{
    esp_core_dump_summary_t *summary = malloc(sizeof(esp_core_dump_summary_t));
    if (summary) {
        esp_err_t err = esp_core_dump_get_summary(summary);
        if (err == ESP_OK) {
            printf("App ELF file SHA256: %s\n", (char *)summary->app_elf_sha256);
            printf("Crashed task: %s\n", summary->exc_task);
#if __XTENSA__
            printf("Exception cause: %ld\n", summary->ex_info.exc_cause);
#else
            printf("Exception cause: %ld\n", summary->ex_info.mcause);
#endif
            char panic_reason[200];
            err = esp_core_dump_get_panic_reason(panic_reason, sizeof(panic_reason));
            if (err == ESP_OK) {
                printf("Panic reason: %s\n", panic_reason);
            }
        }
        free(summary);
    }
}
#endif
