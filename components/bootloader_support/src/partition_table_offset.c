/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "esp_flash_partitions.h"

#if ESP_TEE_BUILD || CONFIG_APP_BUILD_TYPE_PURE_RAM_APP

uint32_t esp_partition_table_get_offset(void)
{
    return CONFIG_PARTITION_TABLE_OFFSET;
}

#else

#include <stdlib.h>
#include "esp_bootloader_desc.h"
#include "esp_log.h"
#include "spi_flash_mmap.h"

ESP_LOG_ATTR_TAG(TAG, "part_table");

static bool is_valid_offset(uint32_t offset)
{
    return offset % SPI_FLASH_SEC_SIZE == 0 && offset > ESP_PRIMARY_BOOTLOADER_OFFSET;
}

#if BOOTLOADER_BUILD

uint32_t esp_partition_table_get_offset(void)
{
    const uint32_t offset = esp_bootloader_get_description()->partition_table_offset;
    if (!is_valid_offset(offset)) {
        ESP_EARLY_LOGE(TAG, "Invalid offset 0x%" PRIx32, offset);
        abort();
    }
    return offset;
}

#else

#include "esp_app_format.h"
#include "esp_private/bootloader_flash_internal.h"
#include "soc/soc_caps.h"
#if SOC_RECOVERY_BOOTLOADER_SUPPORTED
#include "esp_rom_sys.h"
#endif

static uint32_t read_booted_bootloader_offset(void)
{
    const uint32_t fallback = CONFIG_PARTITION_TABLE_OFFSET_LEGACY_BOOTLOADER;

    struct {
        esp_image_header_t header;
        esp_image_segment_header_t segment;
        esp_bootloader_desc_t desc;
    } __attribute__((packed)) image;

#if SOC_RECOVERY_BOOTLOADER_SUPPORTED
    const uint32_t bootloader_offset = esp_rom_get_bootloader_offset();
#else
    const uint32_t bootloader_offset = ESP_PRIMARY_BOOTLOADER_OFFSET;
#endif

    esp_err_t err = bootloader_flash_read(bootloader_offset, &image, sizeof(image), true);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to read bootloader at 0x%" PRIx32 " (%s), using offset 0x%" PRIx32,
                 bootloader_offset, esp_err_to_name(err), fallback);
        return fallback;
    }

    if (image.header.magic != ESP_IMAGE_HEADER_MAGIC || image.desc.magic_byte != ESP_BOOTLOADER_DESC_MAGIC_BYTE) {
        ESP_LOGW(TAG, "No bootloader description at 0x%" PRIx32 ", using offset 0x%" PRIx32,
                 bootloader_offset, fallback);
        return fallback;
    }

    const uint32_t offset = image.desc.partition_table_offset;
    if (offset == 0) {
        ESP_LOGD(TAG, "Bootloader does not record the partition table offset, using 0x%" PRIx32, fallback);
        return fallback;
    }

    if (!is_valid_offset(offset)) {
        ESP_LOGE(TAG, "Invalid partition table offset 0x%" PRIx32 " in bootloader description at 0x%" PRIx32, offset, bootloader_offset);
        abort();
    }

    if (offset != CONFIG_PARTITION_TABLE_OFFSET) {
        ESP_LOGI(TAG, "Partition table at 0x%" PRIx32 " (bootloader), built for 0x%x", offset, CONFIG_PARTITION_TABLE_OFFSET);
    }
    return offset;
}

uint32_t esp_partition_table_get_offset(void)
{
    static uint32_t s_offset;
    if (s_offset == 0) {
        s_offset = read_booted_bootloader_offset();
    }
    return s_offset;
}

#endif // BOOTLOADER_BUILD

#endif
