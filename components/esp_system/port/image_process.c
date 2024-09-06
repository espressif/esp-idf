/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_types.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_image_format.h"
#include "esp_app_format.h"
#include "esp_flash_partitions.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "hal/mmu_hal.h"
#include "hal/mmu_ll.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/ext_mem_defs.h"
#include "esp_private/image_process.h"
#include "esp_private/esp_cache_esp32_private.h"

/**
 * ESP32 bootloader size is not enough, not enable this feature for now
 */
#define IMAGE_PROCESS_SUPPORTED_TARGETS    (!CONFIG_IDF_TARGET_ESP32)

#if CONFIG_IDF_TARGET_ESP32
#define MMAP_MMU_SIZE       0x320000
#elif CONFIG_IDF_TARGET_ESP32S2
#define MMAP_MMU_SIZE       (SOC_DRAM0_CACHE_ADDRESS_HIGH - SOC_DRAM0_CACHE_ADDRESS_LOW)
#else
#define MMAP_MMU_SIZE       (SOC_DRAM_FLASH_ADDRESS_HIGH - SOC_DRAM_FLASH_ADDRESS_LOW)
#endif

#if CONFIG_IDF_TARGET_ESP32
#define FLASH_READ_VADDR    (SOC_DROM_LOW + MMAP_MMU_SIZE)
#else
#define FLASH_READ_VADDR    (SOC_DROM_LOW + MMAP_MMU_SIZE - CONFIG_MMU_PAGE_SIZE)
#endif

#define MMU_FLASH_MASK    (~(CONFIG_MMU_PAGE_SIZE - 1))

/**
 * @brief Image process driver
 */
struct image_process_driver_s {

    /**
     * @brief Process segments
     *
     * @param[in] data  image meta data
     *
     * @return
     *        - ESP_OK
     *        - ESP_ERR_INVALID_ARG:   invalid argument
     *        - ESP_ERR_INVALID_STATE: invalid state
     */
    esp_err_t (*process_segments)(esp_image_metadata_t *data);
};

const static char *TAG = "image_process";

static uint32_t s_current_read_mapping = UINT32_MAX;
static uint32_t s_flash_drom_paddr_start = 0;
static uint32_t s_flash_irom_paddr_start = 0;
static esp_err_t process_segments(esp_image_metadata_t *data);

static image_process_driver_t s_image_process_driver = {
    process_segments,
};

static esp_err_t flash_read(size_t src_addr, void *dest, size_t size)
{
    if (src_addr & 3) {
        ESP_EARLY_LOGE(TAG, "flash_read src_addr 0x%x not 4-byte aligned", src_addr);
        return ESP_ERR_INVALID_ARG;
    }
    if (size & 3) {
        ESP_EARLY_LOGE(TAG, "flash_read size 0x%x not 4-byte aligned", size);
        return ESP_ERR_INVALID_ARG;
    }
    if ((intptr_t)dest & 3) {
        ESP_EARLY_LOGE(TAG, "flash_read dest 0x%x not 4-byte aligned", (intptr_t)dest);
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t *dest_words = (uint32_t *)dest;

    for (size_t word = 0; word < size / 4; word++) {
        uint32_t word_src = src_addr + word * 4;  /* Read this offset from flash */
        uint32_t map_at = word_src & MMU_FLASH_MASK; /* Map this 64KB block from flash */
        uint32_t *map_ptr;

        /* Move the 64KB mmu mapping window to fit map_at */
        if (map_at != s_current_read_mapping) {

            cache_hal_suspend(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
            uint32_t actual_mapped_len = 0;
            mmu_hal_map_region(0, MMU_TARGET_FLASH0, FLASH_READ_VADDR, map_at, CONFIG_MMU_PAGE_SIZE - 1, &actual_mapped_len);
            s_current_read_mapping = map_at;
            ESP_EARLY_LOGD(TAG, "starting from paddr=0x%" PRIx32 " and vaddr=0x%" PRIx32 ", 0x%" PRIx32 " bytes are mapped", map_at, FLASH_READ_VADDR, actual_mapped_len);
#if CONFIG_IDF_TARGET_ESP32
            cache_sync();
#else
            cache_hal_invalidate_addr(FLASH_READ_VADDR, actual_mapped_len);
#endif
            cache_hal_resume(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
        }
        map_ptr = (uint32_t *)(FLASH_READ_VADDR + (word_src - map_at));
        dest_words[word] = *map_ptr;
    }

    return ESP_OK;
}

#if IMAGE_PROCESS_SUPPORTED_TARGETS
static esp_err_t process_image_header(esp_image_metadata_t *data, uint32_t part_offset)
{
    bzero(data, sizeof(esp_image_metadata_t));
    data->start_addr = part_offset;

    ESP_RETURN_ON_ERROR_ISR(flash_read(data->start_addr, &data->image, sizeof(esp_image_header_t)), TAG, "failed to read image");
    data->image_len = sizeof(esp_image_header_t);
    ESP_EARLY_LOGD(TAG, "reading image header=0x%"PRIx32" image_len=0x%"PRIx32" image.segment_count=0x%x", data->start_addr, data->image_len, data->image.segment_count);

    return ESP_OK;
}
#endif

static esp_err_t process_segment(int index, uint32_t flash_addr, esp_image_segment_header_t *header, esp_image_metadata_t *metadata, int *cnt)
{
    /* read segment header */
    ESP_RETURN_ON_ERROR_ISR(flash_read(flash_addr, header, sizeof(esp_image_segment_header_t)), TAG, "failed to do flash read");

    intptr_t load_addr = header->load_addr;
    uint32_t data_len = header->data_len;
    uint32_t data_addr = flash_addr + sizeof(esp_image_segment_header_t);

#if SOC_MMU_DI_VADDR_SHARED
#if CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM
    if (load_addr >= SOC_DRAM_PSRAM_ADDRESS_LOW && load_addr < SOC_DRAM_PSRAM_ADDRESS_HIGH) {
        if (*cnt == 0) {
            s_flash_drom_paddr_start = data_addr;
        } else if (*cnt == 1) {
            s_flash_irom_paddr_start = data_addr;
        }
        (*cnt)++;
    }
#else
    if (load_addr >= SOC_DRAM_FLASH_ADDRESS_LOW && load_addr < SOC_DRAM_FLASH_ADDRESS_HIGH) {
        if (*cnt == 0) {
            s_flash_drom_paddr_start = data_addr;
        } else if (*cnt == 1) {
            s_flash_irom_paddr_start = data_addr;
        }
        (*cnt)++;
    }
#endif
#else
    if (load_addr >= SOC_IRAM_FLASH_ADDRESS_LOW && load_addr < SOC_IRAM_FLASH_ADDRESS_HIGH) {
        s_flash_drom_paddr_start = data_addr;
        (*cnt)++;
    }
    if (load_addr >= SOC_DRAM_FLASH_ADDRESS_LOW && load_addr < SOC_DRAM_FLASH_ADDRESS_HIGH) {
        s_flash_irom_paddr_start = data_addr;
        (*cnt)++;
    }
#endif

    ESP_EARLY_LOGD(TAG, "load_addr: %x, data_len: %x, flash_addr: 0x%x, data_addr: %x", load_addr, data_len, flash_addr, data_addr);
    if (data_len % 4 != 0) {
        ESP_RETURN_ON_FALSE_ISR(false, ESP_ERR_INVALID_STATE, TAG, "unaligned segment length 0x%"PRIx32, data_len);
    }

    mmu_ll_set_entry_invalid(0, MMU_LL_END_DROM_ENTRY_ID);
    s_current_read_mapping = UINT32_MAX;

    return ESP_OK;
}

static esp_err_t process_segments(esp_image_metadata_t *data)
{
    uint32_t start_segments = data->start_addr + data->image_len;
    uint32_t next_addr = start_segments;
    int cnt = 0;

    for (int i = 0; i < data->image.segment_count; i++) {
        esp_image_segment_header_t *header = &data->segments[i];
        ESP_EARLY_LOGD(TAG, "loading segment header %d at offset 0x%"PRIx32, i, next_addr);
        ESP_RETURN_ON_ERROR_ISR(process_segment(i, next_addr, header, data, &cnt), TAG, "failed to process segment");
        next_addr += sizeof(esp_image_segment_header_t);
        data->segment_data[i] = next_addr;
        next_addr += header->data_len;
    }
    assert(cnt == 2);

    uint32_t end_addr = next_addr;
    if (end_addr < data->start_addr) {
        return ESP_FAIL;
    }

    data->image_len += end_addr - start_segments;

    return ESP_OK;
}

void image_process_get_flash_segments_info(uint32_t *out_drom_paddr_start, uint32_t *out_irom_paddr_start)
{
    assert(out_drom_paddr_start && out_irom_paddr_start);
    *out_drom_paddr_start = s_flash_drom_paddr_start;
    *out_irom_paddr_start = s_flash_irom_paddr_start;
}

esp_err_t image_process(void)
{
#if IMAGE_PROCESS_SUPPORTED_TARGETS
    esp_err_t ret = ESP_FAIL;
    /**
     * We use the MMU_LL_END_DROM_ENTRY_ID mmu entry as a map page for app to find the boot partition
     * This depends on 2nd bootloader to set the entry
     */
    uint32_t paddr_base = mmu_ll_entry_id_to_paddr_base(0, MMU_LL_END_DROM_ENTRY_ID);
    uint32_t part_offset = paddr_base;
    esp_image_metadata_t image_data = {0};
    ret = process_image_header(&image_data, part_offset);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "failed to process image header");
        abort();
    }

    ret = s_image_process_driver.process_segments(&image_data);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "failed to process segments");
        return ESP_FAIL;
    }

    mmu_ll_set_entry_invalid(0, MMU_LL_END_DROM_ENTRY_ID);
#else
    (void)s_image_process_driver;
#endif

    return ESP_OK;
}
