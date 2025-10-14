/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "sdkconfig.h"
#include "unity.h"
#include "esp_psram.h"
#include "esp_private/esp_psram_extram.h"

TEST_CASE("test psram no boot init", "[psram_no_boot_init]")
{
#if CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION
    // As PSRAM is just enabled and not initialised during the boot up, the API
    // esp_psram_get_heap_size_to_protect() manually calculates the size of the PSRAM heap
    size_t manually_calculated_psram_heap = esp_psram_get_heap_size_to_protect();
#endif /* CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION */

    TEST_ESP_OK(esp_psram_init());

#if CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION
    size_t final_psram_heap = esp_psram_get_heap_size_to_protect();
    TEST_ASSERT_EQUAL(final_psram_heap, manually_calculated_psram_heap);
#endif /* CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION */
}
