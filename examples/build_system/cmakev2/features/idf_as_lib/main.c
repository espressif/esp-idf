/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Single source file for both host and ESP-IDF builds.
 * - Host:     plain main(), no IDF dependency.
 * - ESP-IDF:  app_main(), uses IDF APIs guarded by ESP_PLATFORM.
 */

#include <stdio.h>
#include <inttypes.h>

#ifdef ESP_PLATFORM
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

#ifdef ESP_PLATFORM
void app_main(void)
#else
int main(void)
#endif
{
    printf("Hello from %s build\n",
#ifdef ESP_PLATFORM
           "ESP-IDF"
#else
           "host"
#endif
    );

#ifdef ESP_PLATFORM
    /* --- chip info --------------------------------------------------------- */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);

    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT)  ? "/BT"  : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);

    if (esp_flash_get_size(NULL, &flash_size) == ESP_OK) {
        printf("%" PRIu32 " MB %s flash\n", flash_size / (1024 * 1024),
               (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    }

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    /* --- countdown and restart -------------------------------------------- */
    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
#else
    printf("Run with idf.py set-target <target> && idf.py build for ESP-IDF build.\n");
    return 0;
#endif
}
