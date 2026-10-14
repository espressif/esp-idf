/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "sdkconfig.h"
#include "unity.h"
#include "esp_psram.h"
#include "esp_private/esp_psram_extram.h"
#if CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM
#include "soc/ext_mem_defs.h"
#include "hal/mmu_hal.h"
#include "hal/mmu_ll.h"
#endif /* CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM */

TEST_CASE("test psram no boot init", "[psram_no_boot_init]")
{
#if CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION
    // As PSRAM is just enabled and not initialised during the boot up, the API
    // esp_psram_get_heap_size_to_protect() manually calculates the size of the PSRAM heap
    size_t manually_calculated_psram_heap = esp_psram_get_heap_size_to_protect();
#endif /* CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION */

    TEST_ESP_OK(esp_psram_init());
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_psram_init());

#if CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM
    uint32_t paddr = 0;
    mmu_target_t target = 0;
    TEST_ASSERT_FALSE(mmu_hal_vaddr_to_paddr(MMU_LL_FLASH_MMU_ID, SOC_DRAM_FLASH_ADDRESS_HIGH - CONFIG_MMU_PAGE_SIZE, &paddr, &target));
    TEST_ASSERT_FALSE(mmu_hal_vaddr_to_paddr(MMU_LL_PSRAM_MMU_ID, SOC_DRAM_PSRAM_ADDRESS_HIGH - 2 * CONFIG_MMU_PAGE_SIZE, &paddr, &target));
#endif /* CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM */

#if CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION
    size_t final_psram_heap = esp_psram_get_heap_size_to_protect();
    TEST_ASSERT_EQUAL(final_psram_heap, manually_calculated_psram_heap);
#endif /* CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION */
}
