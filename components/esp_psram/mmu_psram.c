/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file will be redesigned into MMU driver, to maintain all the external
 * memory contexts including:
 * - Flash
 * - PSRAM
 * - DDR
 *
 * Now only MMU-PSRAM related private APIs
 */

#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "soc/ext_mem_defs.h"
#include "hal/cache_types.h"
#include "hal/cache_ll.h"
#include "esp_private/mmu.h"

#if CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/cache.h"
#endif


#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA
//page_size - 1, where page_size on s2 and s3 is always 0x10000. To be refactored by MMU driver
#define INVALID_PHY_PAGE 0xffff
const static char *TAG = "mmu_psram";
//TODO IDF-4387
static uint32_t page0_mapped = 0;
static uint32_t page0_page = INVALID_PHY_PAGE;
#endif  //#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA


#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
esp_err_t mmu_config_psram_text_segment(uint32_t start_page, uint32_t psram_size, uint32_t *out_page)
{
    uint32_t page_id = start_page;

    /**
     * TODO IDF-4387
     * `Cache_Count_Flash_Pages` seems give wrong results. Need to confirm this.
     * FOR NOW, leave these logics just as it used to be.
     *
     * The rom API will be redesigned into a MMU driver layer function
     */
    uint32_t flash_pages = 0;
#if CONFIG_IDF_TARGET_ESP32S2
    flash_pages += Cache_Count_Flash_Pages(PRO_CACHE_IBUS0, &page0_mapped);
    flash_pages += Cache_Count_Flash_Pages(PRO_CACHE_IBUS1, &page0_mapped);
#elif CONFIG_IDF_TARGET_ESP32S3
    flash_pages += Cache_Count_Flash_Pages(CACHE_IBUS, &page0_mapped);
#endif
    if ((flash_pages + page_id) > BYTES_TO_MMU_PAGE(psram_size)) {
        ESP_EARLY_LOGE(TAG, "PSRAM space not enough for the Flash instructions, need %d B, from %d B to %d B",
                       MMU_PAGE_TO_BYTES(flash_pages), MMU_PAGE_TO_BYTES(start_page), MMU_PAGE_TO_BYTES(flash_pages + page_id));
        return ESP_FAIL;
    }

    //Enable the most high bus, which is used for copying FLASH .text to PSRAM
    cache_bus_mask_t bus_mask = cache_ll_l1_get_bus(0, SOC_EXTRAM_DATA_HIGH, 0);
    cache_ll_l1_enable_bus(0, bus_mask);
#if !CONFIG_FREERTOS_UNICORE
    bus_mask = cache_ll_l1_get_bus(1, SOC_EXTRAM_DATA_HIGH, 0);
    cache_ll_l1_enable_bus(1, bus_mask);
#endif

    instruction_flash_page_info_init(page_id);

#if CONFIG_IDF_TARGET_ESP32S2
    page_id = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_IBUS0, IRAM0_ADDRESS_LOW, page_id, &page0_page);
    page_id = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_IBUS1, IRAM1_ADDRESS_LOW, page_id, &page0_page);
#elif CONFIG_IDF_TARGET_ESP32S3
    page_id = Cache_Flash_To_SPIRAM_Copy(CACHE_IBUS, IRAM0_CACHE_ADDRESS_LOW, page_id, &page0_page);
#endif
    ESP_EARLY_LOGV(TAG, "after copy instruction, page_id is %d", page_id);
    ESP_EARLY_LOGI(TAG, "Instructions copied and mapped to SPIRAM");

    *out_page = page_id - start_page;

    return ESP_OK;
}
#endif  //#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS


#if CONFIG_SPIRAM_RODATA
esp_err_t mmu_config_psram_rodata_segment(uint32_t start_page, uint32_t psram_size, uint32_t *out_page)
{
    uint32_t page_id = start_page;

    /**
     * TODO IDF-4387
     * `Cache_Count_Flash_Pages` seems give wrong results. Need to confirm this.
     * FOR NOW, leave these logics just as it used to be.
     *
     * The rom API will be redesigned into a MMU driver layer function
     */
    uint32_t flash_pages = 0;
#if CONFIG_IDF_TARGET_ESP32S2
    flash_pages += Cache_Count_Flash_Pages(PRO_CACHE_IBUS2, &page0_mapped);
    flash_pages += Cache_Count_Flash_Pages(PRO_CACHE_DBUS0, &page0_mapped);
    flash_pages += Cache_Count_Flash_Pages(PRO_CACHE_DBUS1, &page0_mapped);
    flash_pages += Cache_Count_Flash_Pages(PRO_CACHE_DBUS2, &page0_mapped);
#elif CONFIG_IDF_TARGET_ESP32S3
    flash_pages += Cache_Count_Flash_Pages(CACHE_DBUS, &page0_mapped);
#endif
    if ((flash_pages + page_id) > BYTES_TO_MMU_PAGE(psram_size)) {
        ESP_EARLY_LOGE(TAG, "SPI RAM space not enough for the instructions, need to copy to %d B.", MMU_PAGE_TO_BYTES(flash_pages + page_id));
        return ESP_FAIL;
    }

    //Enable the most high bus, which is used for copying FLASH .text to PSRAM
    cache_bus_mask_t bus_mask = cache_ll_l1_get_bus(0, SOC_EXTRAM_DATA_HIGH, 0);
    cache_ll_l1_enable_bus(0, bus_mask);
#if !CONFIG_FREERTOS_UNICORE
    bus_mask = cache_ll_l1_get_bus(1, SOC_EXTRAM_DATA_HIGH, 0);
    cache_ll_l1_enable_bus(1, bus_mask);
#endif

    rodata_flash_page_info_init(page_id);

#if CONFIG_IDF_TARGET_ESP32S2
    page_id = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_IBUS2, DROM0_ADDRESS_LOW, page_id, &page0_page);
    page_id = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_DBUS0, DRAM0_ADDRESS_LOW, page_id, &page0_page);
    page_id = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_DBUS1, DRAM1_ADDRESS_LOW, page_id, &page0_page);
    page_id = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_DBUS2, DPORT_ADDRESS_LOW, page_id, &page0_page);
#elif CONFIG_IDF_TARGET_ESP32S3
    page_id = Cache_Flash_To_SPIRAM_Copy(CACHE_DBUS, DRAM0_CACHE_ADDRESS_LOW, page_id, &page0_page);
#endif

    ESP_EARLY_LOGV(TAG, "after copy rodata, page_id is %d", page_id);
    ESP_EARLY_LOGI(TAG, "Read only data copied and mapped to SPIRAM");

    *out_page = page_id - start_page;

    return ESP_OK;
}
#endif  //#if CONFIG_SPIRAM_RODATA
