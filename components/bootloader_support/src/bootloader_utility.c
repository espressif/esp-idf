/*
 * SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "esp_log.h"

#include "esp_rom_sys.h"
#include "esp_rom_serial_output.h"
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/cache.h"
#endif
#include "esp_rom_spiflash.h"

#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/rtc.h"
#include "soc/efuse_periph.h"
#include "soc/rtc_periph.h"
#include "hal/mmu_hal.h"
#include "hal/mmu_ll.h"
#include "hal/cache_types.h"
#include "hal/cache_ll.h"
#include "hal/cache_hal.h"
#include "hal/sha_types.h"

#include "esp_cpu.h"
#include "esp_image_format.h"
#include "esp_app_desc.h"
#include "esp_secure_boot.h"
#include "esp_flash_encrypt.h"
#ifndef BOOTLOADER_BUILD
#include "spi_flash_mmap.h"
#endif
#include "esp_flash_partitions.h"
#include "bootloader_flash_priv.h"
#include "bootloader_random.h"
#include "bootloader_config.h"
#include "bootloader_common.h"
#include "bootloader_utility.h"
#include "bootloader_sha.h"
#include "bootloader_console.h"
#include "bootloader_soc.h"
#include "bootloader_memory_utils.h"
#include "esp_efuse.h"
#include "esp_fault.h"

#if CONFIG_SECURE_ENABLE_TEE
#include "bootloader_utility_tee.h"
#endif

ESP_LOG_ATTR_TAG(TAG, "boot");

/* Reduce literal size for some generic string literals */
#define MAP_ERR_MSG "Image contains multiple %s segments. Only the last one will be mapped."

static bool ota_has_initial_contents;

static void load_image(const esp_image_metadata_t *image_data);
static void unpack_load_app(const esp_image_metadata_t *data);
static void set_cache_and_start_app(uint32_t drom_addr,
                                    uint32_t drom_load_addr,
                                    uint32_t drom_size,
                                    uint32_t irom_addr,
                                    uint32_t irom_load_addr,
                                    uint32_t irom_size,
                                    const esp_image_metadata_t *data);

#if CONFIG_SECURE_ENABLE_TEE
/* NOTE: Required by other sources for secure boot routine */
esp_image_metadata_t tee_data;
static uint8_t tee_boot_part = UINT8_MAX;

static void unpack_load_tee_app(const esp_image_metadata_t *data);
static void set_cache_and_load_tee_app(uint32_t drom_addr,
                                       uint32_t drom_load_addr,
                                       uint32_t drom_size,
                                       uint32_t irom_addr,
                                       uint32_t irom_load_addr,
                                       uint32_t irom_size,
                                       const esp_image_metadata_t *data);
#endif

esp_err_t bootloader_common_read_otadata(const esp_partition_pos_t *ota_info, esp_ota_select_entry_t *two_otadata)
{
    const esp_ota_select_entry_t *ota_select_map;
    if (ota_info->offset == 0) {
        return ESP_ERR_NOT_FOUND;
    }

    // partition table has OTA data partition
    if (ota_info->size < 2 * SPI_SEC_SIZE) {
        ESP_LOGE(TAG, "ota_info partition size %"PRIu32" is too small (minimum %d bytes)", ota_info->size, (2 * SPI_SEC_SIZE));
        return ESP_FAIL; // can't proceed
    }

    ESP_LOGD(TAG, "OTA data offset 0x%"PRIx32, ota_info->offset);
    ota_select_map = bootloader_mmap(ota_info->offset, ota_info->size);
    if (!ota_select_map) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%"PRIx32", 0x%"PRIx32") failed", ota_info->offset, ota_info->size);
        return ESP_FAIL; // can't proceed
    }

    memcpy(&two_otadata[0], ota_select_map, sizeof(esp_ota_select_entry_t));
    memcpy(&two_otadata[1], (uint8_t *)ota_select_map + SPI_SEC_SIZE, sizeof(esp_ota_select_entry_t));
    bootloader_munmap(ota_select_map);

    return ESP_OK;
}

esp_err_t bootloader_common_get_partition_description(const esp_partition_pos_t *partition, esp_app_desc_t *app_desc)
{
    if (partition == NULL || app_desc == NULL || partition->offset == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    const uint32_t app_desc_offset = sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t);
    const uint32_t mmap_size = app_desc_offset + sizeof(esp_app_desc_t);
    const uint8_t *image = bootloader_mmap(partition->offset, mmap_size);
    if (image == NULL) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%"PRIx32", 0x%"PRIx32") failed", partition->offset, mmap_size);
        return ESP_FAIL;
    }

    memcpy(app_desc, image + app_desc_offset, sizeof(esp_app_desc_t));
    bootloader_munmap(image);

    if (app_desc->magic_word != ESP_APP_DESC_MAGIC_WORD) {
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}


bool bootloader_utility_load_partition_table(bootloader_state_t *bs)
{
    const esp_partition_info_t *partitions;
    const char *partition_usage;
    esp_err_t err;
    int num_partitions;

    partitions = bootloader_mmap(ESP_PARTITION_TABLE_OFFSET, ESP_PARTITION_TABLE_MAX_LEN);
    if (!partitions) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", ESP_PARTITION_TABLE_OFFSET, ESP_PARTITION_TABLE_MAX_LEN);
        return false;
    }
    ESP_LOGD(TAG, "mapped partition table 0x%x at 0x%x", ESP_PARTITION_TABLE_OFFSET, (intptr_t)partitions);

    err = esp_partition_table_verify(partitions, true, &num_partitions);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to verify partition table");
        return false;
    }

    ESP_LOGI(TAG, "Partition Table:");
    ESP_LOGI(TAG, "## Label            Usage          Type ST Offset   Length");

    for (int i = 0; i < num_partitions; i++) {
        const esp_partition_info_t *partition = &partitions[i];
        ESP_LOGD(TAG, "load partition table entry 0x%x", (intptr_t)partition);
        ESP_LOGD(TAG, "type=%x subtype=%x", partition->type, partition->subtype);
        partition_usage = ESP_LOG_ATTR_STR("unknown");

        /* valid partition table */
        switch (partition->type) {
        case PART_TYPE_APP: /* app partition */
            switch (partition->subtype) {
            case PART_SUBTYPE_FACTORY: /* factory binary */
                bs->factory = partition->pos;
                partition_usage = ESP_LOG_ATTR_STR("factory app");
                break;
            case PART_SUBTYPE_TEST: /* test binary */
                bs->test = partition->pos;
                partition_usage = ESP_LOG_ATTR_STR("test app");
                break;
#if CONFIG_SECURE_ENABLE_TEE
            case PART_SUBTYPE_TEE_0: /* TEE binary */
            case PART_SUBTYPE_TEE_1:
                bs->tee[partition->subtype & 0x01] = partition->pos;
                partition_usage = ESP_LOG_ATTR_STR("TEE app");
                break;
#endif
            default:
                /* OTA binary */
                if ((partition->subtype & ~PART_SUBTYPE_OTA_MASK) == PART_SUBTYPE_OTA_FLAG) {
                    bs->ota[partition->subtype & PART_SUBTYPE_OTA_MASK] = partition->pos;
                    ++bs->app_count;
                    partition_usage = ESP_LOG_ATTR_STR("OTA app");
                } else {
                    partition_usage = ESP_LOG_ATTR_STR("Unknown app");
                }
                break;
            }
            break; /* PART_TYPE_APP */
        case PART_TYPE_DATA: /* data partition */
            switch (partition->subtype) {
            case PART_SUBTYPE_DATA_OTA: /* ota data */
                bs->ota_info = partition->pos;
                partition_usage = ESP_LOG_ATTR_STR("OTA data");
                break;
            case PART_SUBTYPE_DATA_RF:
                partition_usage = ESP_LOG_ATTR_STR("RF data");
                break;
            case PART_SUBTYPE_DATA_WIFI:
                partition_usage = ESP_LOG_ATTR_STR("WiFi data");
                break;
            case PART_SUBTYPE_DATA_NVS_KEYS:
                partition_usage = ESP_LOG_ATTR_STR("NVS keys");
                break;
            case PART_SUBTYPE_DATA_EFUSE_EM:
                partition_usage = ESP_LOG_ATTR_STR("efuse");
#ifdef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
                esp_efuse_init_virtual_mode_in_flash(partition->pos.offset, partition->pos.size);
#endif
                break;
#if CONFIG_SECURE_ENABLE_TEE
            case PART_SUBTYPE_DATA_TEE_OTA: /* TEE ota data */
                bs->tee_ota_info = partition->pos;
                partition_usage = ESP_LOG_ATTR_STR("TEE OTA data");
                break;
#endif
            default:
                partition_usage = ESP_LOG_ATTR_STR("Unknown data");
                break;
            }
            break; /* PARTITION_USAGE_DATA */
        case PART_TYPE_BOOTLOADER: /* Bootloader partition */
            switch (partition->subtype) {
            case PART_SUBTYPE_BOOTLOADER_PRIMARY:
                partition_usage = ESP_LOG_ATTR_STR("primary bootloader");
                break;
            case PART_SUBTYPE_BOOTLOADER_OTA:
                partition_usage = ESP_LOG_ATTR_STR("ota bootloader");
                break;
            case PART_SUBTYPE_BOOTLOADER_RECOVERY:
                partition_usage = ESP_LOG_ATTR_STR("recovery bootloader");
                break;
            }
            break; /* PART_TYPE_BOOTLOADER */
        case PART_TYPE_PARTITION_TABLE: /* Partition table partition */
            switch (partition->subtype) {
            case PART_SUBTYPE_PARTITION_TABLE_PRIMARY:
                partition_usage = ESP_LOG_ATTR_STR("primary partition_table");
                break;
            case PART_SUBTYPE_PARTITION_TABLE_OTA:
                partition_usage = ESP_LOG_ATTR_STR("ota partition_table");
                break;
            }
            break; /* PART_TYPE_PARTITION_TABLE */
        default: /* other partition type */
            break;
        }

        /* print partition type info */
        ESP_LOGI(TAG, "%2d %-16s %-16s %02x %02x %08"PRIx32" %08"PRIx32, i, partition->label, partition_usage,
                 partition->type, partition->subtype,
                 partition->pos.offset, partition->pos.size);
    }

    bootloader_munmap(partitions);

    ESP_LOGI(TAG, "End of partition table");
    return true;
}

/* Given a partition index, return the partition position data from the bootloader_state_t structure */
static esp_partition_pos_t index_to_partition(const bootloader_state_t *bs, int index)
{
    if (index == FACTORY_INDEX) {
        return bs->factory;
    }

    if (index == TEST_APP_INDEX) {
        return bs->test;
    }

    if (index >= 0 && index < MAX_OTA_SLOTS && index < (int)bs->app_count) {
        return bs->ota[index];
    }

    esp_partition_pos_t invalid = { 0 };
    return invalid;
}

static void log_invalid_app_partition(int index)
{
    const char *not_bootable = ESP_LOG_ATTR_STR(" is not bootable"); /* save a few string literal bytes */
    switch (index) {
    case FACTORY_INDEX:
        ESP_LOGE(TAG, "Factory app partition%s", not_bootable);
        break;
    case TEST_APP_INDEX:
        ESP_LOGE(TAG, "Factory test app partition%s", not_bootable);
        break;
    default:
        ESP_LOGE(TAG, "OTA app partition slot %d%s", index, not_bootable);
        break;
    }
}

static esp_err_t write_otadata(esp_ota_select_entry_t *otadata, uint32_t offset, bool write_encrypted)
{
    esp_err_t err = bootloader_flash_erase_sector(offset / FLASH_SECTOR_SIZE);
    if (err == ESP_OK) {
        err = bootloader_flash_write(offset, otadata, sizeof(esp_ota_select_entry_t), write_encrypted);
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error in write_otadata operation. err = 0x%x", err);
    }
    return err;
}

static bool check_anti_rollback(const esp_partition_pos_t *partition)
{
#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
    esp_app_desc_t app_desc = {};
    esp_err_t err = bootloader_common_get_partition_description(partition, &app_desc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get partition description %d", err);
        return false;
    }
    bool sec_ver = esp_efuse_check_secure_version(app_desc.secure_version);
    /* Anti FI check */
    ESP_FAULT_ASSERT(sec_ver == esp_efuse_check_secure_version(app_desc.secure_version));
    return sec_ver;
#else
    return true;
#endif
}

#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
static void update_anti_rollback(const esp_partition_pos_t *partition)
{
    esp_app_desc_t app_desc;
    esp_err_t err = bootloader_common_get_partition_description(partition, &app_desc);
    if (err == ESP_OK) {
        esp_efuse_update_secure_version(app_desc.secure_version);
    } else {
        ESP_LOGE(TAG, "Failed to get partition description %d", err);
    }
}

static int get_active_otadata_with_check_anti_rollback(const bootloader_state_t *bs, esp_ota_select_entry_t *two_otadata)
{
    uint32_t ota_seq;
    uint32_t ota_slot;
    bool valid_otadata[2];

    valid_otadata[0] = bootloader_common_ota_select_valid(&two_otadata[0]);
    valid_otadata[1] = bootloader_common_ota_select_valid(&two_otadata[1]);

    bool sec_ver_valid_otadata[2] = { 0 };
    for (int i = 0; i < 2; ++i) {
        if (valid_otadata[i] == true) {
            ota_seq = two_otadata[i].ota_seq - 1; // Raw OTA sequence number. May be more than # of OTA slots
            ota_slot = ota_seq % bs->app_count; // Actual OTA partition selection
            if (check_anti_rollback(&bs->ota[ota_slot]) == false) {
                // invalid. This otadata[i] will not be selected as active.
                ESP_LOGD(TAG, "OTA slot %"PRIu32" has an app with secure_version, this version is smaller than in the device. This OTA slot will not be selected.", ota_slot);
            } else {
                sec_ver_valid_otadata[i] = true;
            }
        }
    }

    return bootloader_common_select_otadata(two_otadata, sec_ver_valid_otadata, true);
}
#endif

int bootloader_utility_get_selected_boot_partition(const bootloader_state_t *bs)
{
    esp_ota_select_entry_t otadata[2];
    int boot_index = FACTORY_INDEX;

    if (bs->ota_info.offset == 0) {
        return FACTORY_INDEX;
    }

    if (bootloader_common_read_otadata(&bs->ota_info, otadata) != ESP_OK) {
        return INVALID_INDEX;
    }
    ota_has_initial_contents = false;

    ESP_LOGD(TAG, "otadata[0]: sequence values 0x%08"PRIx32, otadata[0].ota_seq);
    ESP_LOGD(TAG, "otadata[1]: sequence values 0x%08"PRIx32, otadata[1].ota_seq);

#ifdef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
    bool write_encrypted = esp_efuse_is_flash_encryption_enabled();
    for (int i = 0; i < 2; ++i) {
        if (otadata[i].ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            ESP_LOGD(TAG, "otadata[%d] is marking as ABORTED", i);
            otadata[i].ota_state = ESP_OTA_IMG_ABORTED;
            write_otadata(&otadata[i], bs->ota_info.offset + FLASH_SECTOR_SIZE * i, write_encrypted);
        }
    }
#endif

#ifndef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
    if ((bootloader_common_ota_select_invalid(&otadata[0]) &&
            bootloader_common_ota_select_invalid(&otadata[1])) ||
            bs->app_count == 0) {
        ESP_LOGD(TAG, "OTA sequence numbers both empty (all-0xFF) or partition table does not have bootable ota_apps (app_count=%"PRIu32")", bs->app_count);
        if (bs->factory.offset != 0) {
            ESP_LOGI(TAG, "Defaulting to factory image");
            boot_index = FACTORY_INDEX;
        } else {
            ESP_LOGI(TAG, "No factory image, trying OTA 0");
            boot_index = 0;
            // Try to boot from ota_0.
            if ((otadata[0].ota_seq == UINT32_MAX || otadata[0].crc != bootloader_common_ota_select_crc(&otadata[0])) &&
                    (otadata[1].ota_seq == UINT32_MAX || otadata[1].crc != bootloader_common_ota_select_crc(&otadata[1]))) {
                // Factory is not found and both otadata are initial(0xFFFFFFFF) or incorrect crc.
                // will set correct ota_seq.
                ota_has_initial_contents = true;
            }
        }
    } else {
        int active_otadata = bootloader_common_get_active_otadata(otadata);
#else
    ESP_LOGI(TAG, "Enabled a check secure version of app for anti rollback");
    ESP_LOGI(TAG, "Secure version (from eFuse) = %"PRIu32, esp_efuse_read_secure_version());
    // When CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK is enabled factory partition should not be in partition table, only two ota_app are there.
    if ((otadata[0].ota_seq == UINT32_MAX || otadata[0].crc != bootloader_common_ota_select_crc(&otadata[0])) &&
            (otadata[1].ota_seq == UINT32_MAX || otadata[1].crc != bootloader_common_ota_select_crc(&otadata[1]))) {
        ESP_LOGI(TAG, "otadata[0..1] in initial state");
        // both otadata are initial(0xFFFFFFFF) or incorrect crc.
        // will set correct ota_seq.
        ota_has_initial_contents = true;
    } else {
        int active_otadata = get_active_otadata_with_check_anti_rollback(bs, otadata);
#endif
        if (active_otadata != -1) {
            ESP_LOGD(TAG, "Active otadata[%d]", active_otadata);
            uint32_t ota_seq = otadata[active_otadata].ota_seq - 1; // Raw OTA sequence number. May be more than # of OTA slots
            boot_index = ota_seq % bs->app_count; // Actual OTA partition selection
            ESP_LOGD(TAG, "Mapping seq %"PRIu32" -> OTA slot %d", ota_seq, boot_index);
#ifdef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
            if (otadata[active_otadata].ota_state == ESP_OTA_IMG_NEW) {
                ESP_LOGD(TAG, "otadata[%d] is selected as new and marked PENDING_VERIFY state", active_otadata);
                otadata[active_otadata].ota_state = ESP_OTA_IMG_PENDING_VERIFY;
                write_otadata(&otadata[active_otadata], bs->ota_info.offset + FLASH_SECTOR_SIZE * active_otadata, write_encrypted);
            }
#endif // CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE

#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
            if (otadata[active_otadata].ota_state == ESP_OTA_IMG_VALID) {
                update_anti_rollback(&bs->ota[boot_index]);
            }
#endif // CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK

        } else if (bs->factory.offset != 0) {
            ESP_LOGE(TAG, "ota data partition invalid, falling back to factory");
            boot_index = FACTORY_INDEX;
        } else {
            ESP_LOGE(TAG, "ota data partition invalid and no factory, will try all partitions");
            boot_index = FACTORY_INDEX;
        }
    }

    return boot_index;
}

/* Return true if a partition has a valid app image that was successfully loaded */
static bool try_load_partition(const esp_partition_pos_t *partition, esp_image_metadata_t *data)
{
    if (partition->size == 0) {
        ESP_LOGD(TAG, "Can't boot from zero-length partition");
        return false;
    }
#ifdef BOOTLOADER_BUILD
    if (bootloader_load_image(partition, data) == ESP_OK) {
        ESP_LOGI(TAG, "Loaded app from partition at offset 0x%" PRIx32, partition->offset);
        return true;
    }
#endif

    return false;
}

// ota_has_initial_contents flag is set if factory does not present in partition table and
// otadata has initial content(0xFFFFFFFF), then set actual ota_seq.
static void set_actual_ota_seq(const bootloader_state_t *bs, int index)
{
    if (index > FACTORY_INDEX && ota_has_initial_contents == true) {
        esp_ota_select_entry_t otadata;
        memset(&otadata, 0xFF, sizeof(otadata));
        otadata.ota_seq = index + 1;
        otadata.ota_state = ESP_OTA_IMG_VALID;
        otadata.crc = bootloader_common_ota_select_crc(&otadata);

        bool write_encrypted = esp_efuse_is_flash_encryption_enabled();
        write_otadata(&otadata, bs->ota_info.offset + FLASH_SECTOR_SIZE * 0, write_encrypted);
        ESP_LOGI(TAG, "Set actual ota_seq=%"PRIu32" in otadata[0]", otadata.ota_seq);
#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
        update_anti_rollback(&bs->ota[index]);
#endif
    }
#if CONFIG_BOOTLOADER_RESERVE_RTC_MEM
#ifdef CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP
    esp_partition_pos_t partition = index_to_partition(bs, index);
    bootloader_common_update_rtc_retain_mem(&partition, true);
#else
    bootloader_common_update_rtc_retain_mem(NULL, true);
#endif
#endif // CONFIG_BOOTLOADER_RESERVE_RTC_MEM
}

#ifdef CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP
void bootloader_utility_load_boot_image_from_deep_sleep(void)
{
    if (esp_rom_get_reset_reason(0) == RESET_REASON_CORE_DEEP_SLEEP) {
#if SOC_RTC_FAST_MEM_SUPPORTED
        esp_partition_pos_t *partition = bootloader_common_get_rtc_retain_mem_partition();
        esp_image_metadata_t image_data;
        if (partition != NULL && bootloader_load_image_no_verify(partition, &image_data) == ESP_OK) {
            ESP_LOGI(TAG, "Fast booting app from partition at offset 0x%"PRIx32, partition->offset);
            bootloader_common_update_rtc_retain_mem(NULL, true);
            load_image(&image_data);
        }
#else // !SOC_RTC_FAST_MEM_SUPPORTED
        bootloader_state_t bs = {0};
        if (bootloader_utility_load_partition_table(&bs)) {
            int index_of_last_loaded_app = FACTORY_INDEX;
            esp_ota_select_entry_t otadata[2];
            if (bs.ota_info.size && bootloader_common_read_otadata(&bs.ota_info, otadata) == ESP_OK) {
                int active_otadata = bootloader_common_get_active_otadata(otadata);
                if (active_otadata != -1) {
                    index_of_last_loaded_app = (otadata[active_otadata].ota_seq - 1) % bs.app_count;
                }
            }
            esp_partition_pos_t partition = index_to_partition(&bs, index_of_last_loaded_app);
            esp_image_metadata_t image_data;
            if (partition.size && bootloader_load_image_no_verify(&partition, &image_data) == ESP_OK) {
                ESP_LOGI(TAG, "Fast booting app from partition at offset 0x%"PRIx32, partition.offset);
                load_image(&image_data);
            }
        }
#endif // !SOC_RTC_FAST_MEM_SUPPORTED
        ESP_LOGE(TAG, "Fast booting is not successful");
        ESP_LOGI(TAG, "Try to load an app as usual with all validations");
    }
}
#endif

#if CONFIG_SECURE_ENABLE_TEE
void bootloader_utility_load_tee_image(const bootloader_state_t *bs)
{
    esp_err_t err = ESP_FAIL;
    uint8_t tee_active_part = bootloader_utility_tee_get_boot_partition(&bs->tee_ota_info);
    if (tee_active_part != PART_SUBTYPE_TEE_0 && tee_active_part != PART_SUBTYPE_TEE_1) {
        ESP_LOGE(TAG, "Failed to find valid TEE app");
        bootloader_reset();
    }

    uint8_t tee_part_idx = tee_active_part & 0x01;
    const esp_partition_pos_t *tee_active_part_pos = &bs->tee[tee_part_idx];
    err = bootloader_load_image(tee_active_part_pos, &tee_data);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to load TEE app");
        bootloader_reset();
    }
    tee_boot_part = tee_active_part;

    ESP_LOGI(TAG, "Loaded TEE app from partition at offset 0x%"PRIx32, tee_active_part_pos->offset);
}
#endif

#define TRY_LOG_FORMAT "Trying partition index %d offs 0x%"PRIx32" size 0x%"PRIx32

void bootloader_utility_load_boot_image(const bootloader_state_t *bs, int start_index)
{
    int index = start_index;
    esp_partition_pos_t part;
    esp_image_metadata_t image_data = {0};

    if (start_index == TEST_APP_INDEX) {
        if (check_anti_rollback(&bs->test) && try_load_partition(&bs->test, &image_data)) {
            load_image(&image_data);
        } else {
            ESP_LOGE(TAG, "No bootable test partition in the partition table");
            bootloader_reset();
        }
    }

    /* work backwards from start_index, down to the factory app */
    for (index = start_index; index >= FACTORY_INDEX; index--) {
        part = index_to_partition(bs, index);
        if (part.size == 0) {
            continue;
        }
        ESP_LOGD(TAG, TRY_LOG_FORMAT, index, part.offset, part.size);
        if (check_anti_rollback(&part) && try_load_partition(&part, &image_data)) {
            set_actual_ota_seq(bs, index);
            load_image(&image_data);
        }
        log_invalid_app_partition(index);
    }

    /* failing that work forwards from start_index, try valid OTA slots */
    for (index = start_index + 1; index < (int)bs->app_count; index++) {
        part = index_to_partition(bs, index);
        if (part.size == 0) {
            continue;
        }
        ESP_LOGD(TAG, TRY_LOG_FORMAT, index, part.offset, part.size);
        if (check_anti_rollback(&part) && try_load_partition(&part, &image_data)) {
            set_actual_ota_seq(bs, index);
            load_image(&image_data);
        }
        log_invalid_app_partition(index);
    }

    if (check_anti_rollback(&bs->test) && try_load_partition(&bs->test, &image_data)) {
        ESP_LOGW(TAG, "Falling back to test app as only bootable partition");
        load_image(&image_data);
    }

    ESP_LOGE(TAG, "No bootable app partitions in the partition table");
    bzero(&image_data, sizeof(esp_image_metadata_t));
    bootloader_reset();
}

// Copy loaded segments to RAM, set up caches for mapped segments, and start application.
static void load_image(const esp_image_metadata_t *image_data)
{
    /**
     * Rough steps for a first boot, when encryption and secure boot are both disabled:
     *   1) Generate secure boot key and write to EFUSE.
     *   2) Write plaintext digest based on plaintext bootloader
     *   3) Generate flash encryption key and write to EFUSE.
     *   4) Encrypt flash in-place including bootloader, then digest,
     *      then app partitions and other encrypted partitions
     *   5) Burn EFUSE to enable flash encryption (FLASH_CRYPT_CNT)
     *   6) Burn EFUSE to enable secure boot (ABS_DONE_0)
     *
     * If power failure happens during Step 1, probably the next boot will continue from Step 2.
     * There is some small chance that EFUSEs will be part-way through being written so will be
     * somehow corrupted here. Thankfully this window of time is very small, but if that's the
     * case, one has to use the espefuse tool to manually set the remaining bits and enable R/W
     * protection. Once the relevant EFUSE bits are set and R/W protected, Step 1 will be skipped
     * successfully on further reboots.
     *
     * If power failure happens during Step 2, Step 1 will be skipped and Step 2 repeated:
     * the digest will get re-written on the next boot.
     *
     * If power failure happens during Step 3, it's possible that EFUSE was partially written
     * with the generated flash encryption key, though the time window for that would again
     * be very small. On reboot, Step 1 will be skipped and Step 2 repeated, though, Step 3
     * may fail due to the above mentioned reason, in which case, one has to use the espefuse
     * tool to manually set the remaining bits and enable R/W protection. Once the relevant EFUSE
     * bits are set and R/W protected, Step 3 will be skipped successfully on further reboots.
     *
     * If power failure happens after start of 4 and before end of 5, the next boot will fail
     * (bootloader header is encrypted and flash encryption isn't enabled yet, so it looks like
     * noise to the ROM bootloader). The check in the ROM is pretty basic so if the first byte of
     * ciphertext happens to be the magic byte E9 then it may try to boot, but it will definitely
     * crash (no chance that the remaining ciphertext will look like a valid bootloader image).
     * Only solution is to reflash with all plaintext and the whole process starts again: skips
     * Step 1, repeats Step 2, skips Step 3, etc.
     *
     * If power failure happens after 5 but before 6, the device will reboot with flash
     * encryption on and will regenerate an encrypted digest in Step 2. This should still
     * be valid as the input data for the digest is read via flash cache (so will be decrypted)
     * and the code in secure_boot_generate() tells bootloader_flash_write() to encrypt the data
     * on write if flash encryption is enabled. Steps 3 - 5 are skipped (encryption already on),
     * then Step 6 enables secure boot.
     */

#if defined(CONFIG_SECURE_BOOT) || defined(CONFIG_SECURE_FLASH_ENC_ENABLED)
    esp_err_t err;
#endif

#ifdef CONFIG_SECURE_BOOT_FLASH_ENC_KEYS_BURN_TOGETHER
    if (esp_secure_boot_enabled() ^ esp_flash_encrypt_initialized_once()) {
        ESP_LOGE(TAG, "Secure Boot and Flash Encryption cannot be enabled separately, only together (their keys go into one eFuse key block)");
        return;
    }

    if (!esp_secure_boot_enabled() || !esp_efuse_is_flash_encryption_enabled()) {
        esp_efuse_batch_write_begin();
    }
#endif // CONFIG_SECURE_BOOT_FLASH_ENC_KEYS_BURN_TOGETHER

#ifdef CONFIG_SECURE_BOOT_V2_ENABLED
    err = esp_secure_boot_v2_permanently_enable(image_data);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Secure Boot v2 failed (%d)", err);
        return;
    }
#endif

#ifdef CONFIG_SECURE_BOOT_V1_ENABLED
    /* Steps 1 & 2 (see above for full description):
     *   1) Generate secure boot EFUSE key
     *   2) Compute digest of plaintext bootloader
     */
    err = esp_secure_boot_generate_digest();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Bootloader digest generation for secure boot failed (%d).", err);
        return;
    }
#endif

#ifdef CONFIG_SECURE_FLASH_ENC_ENABLED
    /* Steps 3, 4 & 5 (see above for full description):
     *   3) Generate flash encryption EFUSE key
     *   4) Encrypt flash contents
     *   5) Burn EFUSE to enable flash encryption
     */
    ESP_LOGI(TAG, "Checking flash encryption...");
    bool flash_encryption_enabled = esp_flash_encrypt_state();
    if (!flash_encryption_enabled) {
#ifdef CONFIG_SECURE_FLASH_REQUIRE_ALREADY_ENABLED
        ESP_LOGE(TAG, "flash encryption is not enabled, and SECURE_FLASH_REQUIRE_ALREADY_ENABLED is set, refusing to boot.");
        return;
#endif // CONFIG_SECURE_FLASH_REQUIRE_ALREADY_ENABLED

        if (esp_flash_encrypt_is_write_protected(true)) {
            return;
        }

        err = esp_flash_encrypt_init();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Initialization of Flash Encryption key failed (%d)", err);
            return;
        }
    }

#ifdef CONFIG_SECURE_BOOT_FLASH_ENC_KEYS_BURN_TOGETHER
    if (!esp_secure_boot_enabled() || !flash_encryption_enabled) {
        err = esp_efuse_batch_write_commit();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error programming eFuses (err=0x%x).", err);
            return;
        }
        assert(esp_secure_boot_enabled());
        ESP_LOGI(TAG, "Secure boot permanently enabled");
    }
#endif // CONFIG_SECURE_BOOT_FLASH_ENC_KEYS_BURN_TOGETHER

    if (!flash_encryption_enabled) {
        err = esp_flash_encrypt_contents();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Encryption flash contents failed (%d)", err);
            return;
        }

        err = esp_flash_encrypt_enable();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Enabling of Flash encryption failed (%d)", err);
            return;
        }
    }
#endif // CONFIG_SECURE_FLASH_ENC_ENABLED

#ifdef CONFIG_SECURE_BOOT_V1_ENABLED
    /* Step 6 (see above for full description):
     *   6) Burn EFUSE to enable secure boot
     */
    ESP_LOGI(TAG, "Checking secure boot...");
    err = esp_secure_boot_permanently_enable();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "FAILED TO ENABLE SECURE BOOT (%d).", err);
        /* Panic here as secure boot is not properly enabled
           due to one of the reasons in above function
        */
        abort();
    }
#endif

#ifdef CONFIG_SECURE_FLASH_ENC_ENABLED
    if (!flash_encryption_enabled && esp_efuse_is_flash_encryption_enabled()) {
        /* Flash encryption was just enabled for the first time,
           so issue a system reset to ensure flash encryption
           cache resets properly */
        ESP_LOGI(TAG, "Resetting with flash encryption enabled...");
        esp_rom_output_tx_wait_idle(0);
        bootloader_reset();
    }
#endif

    ESP_LOGI(TAG, "Disabling RNG early entropy source...");
    bootloader_random_disable();

    /* Disable glitch reset after all the security checks are completed.
     * Glitch detection can be falsely triggered by EMI interference (high RF TX power, etc)
     * and to avoid such false alarms, disable it.
     */
    bootloader_ana_clock_glitch_reset_config(false);

    // copy loaded segments to RAM, set up caches for mapped segments, and start application
    unpack_load_app(image_data);
}

#if SOC_MMU_DI_VADDR_SHARED
static void unpack_load_app(const esp_image_metadata_t *data)
{
    /**
     * note:
     * On chips with shared D/I external vaddr, we don't divide them into either D or I,
     * as essentially they are the same.
     * We integrate all the hardware difference into this `unpack_load_app` function.
     */
    uint32_t rom_addr[2] = {};
    uint32_t rom_load_addr[2] = {};
    uint32_t rom_size[2] = {};
    int rom_index = 0;  //shall not exceed 2

    // Find DROM & IROM addresses, to configure MMU mappings
    for (int i = 0; i < data->image.segment_count; i++) {
        const esp_image_segment_header_t *header = &data->segments[i];
        bool text_or_rodata = false;

        //`SOC_DROM_LOW` and `SOC_DROM_HIGH` are the same as `SOC_IROM_LOW` and `SOC_IROM_HIGH`, reasons are in above `note`
        if (header->load_addr >= SOC_DROM_LOW && header->load_addr < SOC_DROM_HIGH) {
            text_or_rodata = true;
        }
#if SOC_MMU_PER_EXT_MEM_TARGET
        if (header->load_addr >= SOC_EXTRAM_LOW && header->load_addr < SOC_EXTRAM_HIGH) {
            text_or_rodata = true;
        }
#endif
        if (text_or_rodata) {
            /**
             * D/I are shared, but there should not be a third segment on flash/psram
             */
            assert(rom_index < 2);
            rom_addr[rom_index] = data->segment_data[i];
            rom_load_addr[rom_index] = header->load_addr;
            rom_size[rom_index] = header->data_len;
            rom_index++;
        }
    }
    assert(rom_index == 2);

    ESP_EARLY_LOGD(TAG, "calling set_cache_and_start_app");
    set_cache_and_start_app(rom_addr[0],
                            rom_load_addr[0],
                            rom_size[0],
                            rom_addr[1],
                            rom_load_addr[1],
                            rom_size[1],
                            data);
}

#else  //!SOC_MMU_DI_VADDR_SHARED
static void unpack_load_app(const esp_image_metadata_t *data)
{
    uint32_t drom_addr = 0;
    uint32_t drom_load_addr = 0;
    uint32_t drom_size = 0;
    uint32_t irom_addr = 0;
    uint32_t irom_load_addr = 0;
    uint32_t irom_size = 0;

    // Find DROM & IROM addresses, to configure MMU mappings
    for (int i = 0; i < data->image.segment_count; i++) {
        const esp_image_segment_header_t *header = &data->segments[i];
        if (header->load_addr >= SOC_DROM_LOW && header->load_addr < SOC_DROM_HIGH) {
            if (drom_addr != 0) {
                ESP_EARLY_LOGE(TAG, MAP_ERR_MSG, ESP_LOG_ATTR_STR("DROM"));
            } else {
                ESP_EARLY_LOGD(TAG, "Mapping segment %d as %s", i, ESP_LOG_ATTR_STR("DROM"));
            }
            drom_addr = data->segment_data[i];
            drom_load_addr = header->load_addr;
            drom_size = header->data_len;
        }
        if (header->load_addr >= SOC_IROM_LOW && header->load_addr < SOC_IROM_HIGH) {
            if (irom_addr != 0) {
                ESP_EARLY_LOGE(TAG, MAP_ERR_MSG, ESP_LOG_ATTR_STR("IROM"));
            } else {
                ESP_EARLY_LOGD(TAG, "Mapping segment %d as %s", i, ESP_LOG_ATTR_STR("IROM"));
            }
            irom_addr = data->segment_data[i];
            irom_load_addr = header->load_addr;
            irom_size = header->data_len;
        }
    }

    ESP_EARLY_LOGD(TAG, "calling set_cache_and_start_app");
    set_cache_and_start_app(drom_addr,
                            drom_load_addr,
                            drom_size,
                            irom_addr,
                            irom_load_addr,
                            irom_size,
                            data);
}
#endif  //#if SOC_MMU_DI_VADDR_SHARED

//unused for esp32
__attribute__((unused))
static bool s_flash_seg_needs_map(uint32_t vaddr)
{
#if SOC_MMU_PER_EXT_MEM_TARGET
    //For these chips, segments on PSRAM will be mapped in app
    bool is_psram = esp_ptr_in_extram((void *)vaddr);
    return !is_psram;
#else
    //For these chips, segments on Flash always need to be mapped
    return true;
#endif
}

/* TODO: [IDF-11689] Unify the TEE-specific app loading implementation with
 * the existing app loading implementation.
 */
#if CONFIG_SECURE_ENABLE_TEE
static void unpack_load_tee_app(const esp_image_metadata_t *data)
{
    /**
     * note:
     * On chips with shared D/I external vaddr, we don't divide them into either D or I,
     * as essentially they are the same.
     * We integrate all the hardware difference into this `unpack_load_app` function.
     */
    uint32_t rom_addr[2] = {};
    uint32_t rom_load_addr[2] = {};
    uint32_t rom_size[2] = {};
    int rom_index = 0;  //shall not exceed 2

    // Find DROM & IROM addresses, to configure MMU mappings
    for (int i = 0; i < data->image.segment_count; i++) {
        const esp_image_segment_header_t *header = &data->segments[i];
        const uint32_t addr = header->load_addr;

        //`SOC_DROM_LOW` and `SOC_DROM_HIGH` are the same as `SOC_IROM_LOW` and `SOC_IROM_HIGH`, reasons are in above `note`
        if ((addr >= SOC_DROM_LOW   && addr < SOC_DROM_HIGH)
#if SOC_MMU_PER_EXT_MEM_TARGET
                || (addr >= SOC_EXTRAM_LOW && addr < SOC_EXTRAM_HIGH)
#endif
           ) {
            /**
             * D/I are shared, but there should not be a third segment on flash/psram
             */
            assert(rom_index < 2);
            rom_addr[rom_index] = data->segment_data[i];
            rom_load_addr[rom_index] = header->load_addr;
            rom_size[rom_index] = header->data_len;
            rom_index++;
        }
    }
    assert(rom_index == 2);

    ESP_EARLY_LOGD(TAG, "calling set_cache_and_start_tee_app");
    set_cache_and_load_tee_app(rom_addr[0],
                               rom_load_addr[0],
                               rom_size[0],
                               rom_addr[1],
                               rom_load_addr[1],
                               rom_size[1],
                               data);
}

static void set_cache_and_load_tee_app(
    uint32_t drom_addr,
    uint32_t drom_load_addr,
    uint32_t drom_size,
    uint32_t irom_addr,
    uint32_t irom_load_addr,
    uint32_t irom_size,
    const esp_image_metadata_t *data)
{
    uint32_t drom_load_addr_aligned = 0, drom_addr_aligned = 0;
    uint32_t irom_load_addr_aligned = 0, irom_addr_aligned = 0;
    uint32_t actual_mapped_len = 0;

    const uint32_t mmu_page_size = data->mmu_page_size;
#if SOC_MMU_PAGE_SIZE_CONFIGURABLE
    // re-configure MMU page size
    mmu_ll_set_page_size(0, mmu_page_size);
#endif //SOC_MMU_PAGE_SIZE_CONFIGURABLE

    if (drom_addr != 0) {
        drom_load_addr_aligned = drom_load_addr & MMU_FLASH_MASK_FROM_VAL(mmu_page_size);
        drom_addr_aligned = drom_addr & MMU_FLASH_MASK_FROM_VAL(mmu_page_size);
        ESP_EARLY_LOGV(TAG, "TEE rodata starts from paddr=0x%08x, vaddr=0x%08x, size=0x%x", drom_addr, drom_load_addr, drom_size);

        //The addr is aligned, so we add the mask off length to the size, to make sure the corresponding buses are enabled.
        if (s_flash_seg_needs_map(drom_load_addr_aligned)) {
            mmu_hal_map_region(0, MMU_TARGET_FLASH0, drom_load_addr_aligned, drom_addr_aligned, drom_size, &actual_mapped_len);
            ESP_EARLY_LOGV(TAG, "after mapping rodata, starting from paddr=0x%08" PRIx32 " and vaddr=0x%08" PRIx32 ", 0x%" PRIx32 " bytes are mapped", drom_addr_aligned, drom_load_addr_aligned, actual_mapped_len);
        }
        //we use the MMU_LL_END_DROM_ENTRY_ID mmu entry as a map page for app to find the boot partition
        mmu_hal_map_region(0, MMU_TARGET_FLASH0, MMU_DROM_END_ENTRY_VADDR_FROM_VAL(mmu_page_size), drom_addr_aligned, mmu_page_size, &actual_mapped_len);
        ESP_EARLY_LOGV(TAG, "mapped one page of the rodata, from paddr=0x%08" PRIx32 " and vaddr=0x%08" PRIx32 ", 0x%" PRIx32 " bytes are mapped", drom_addr_aligned, drom_load_addr_aligned, actual_mapped_len);
    }

    if (irom_addr != 0) {
        irom_load_addr_aligned = irom_load_addr & MMU_FLASH_MASK_FROM_VAL(mmu_page_size);
        irom_addr_aligned = irom_addr & MMU_FLASH_MASK_FROM_VAL(mmu_page_size);
        ESP_EARLY_LOGV(TAG, "TEE text starts from paddr=0x%08x, vaddr=0x%08x, size=0x%x", irom_addr, irom_load_addr, irom_size);
        //The addr is aligned, so we add the mask off length to the size, to make sure the corresponding buses are enabled.
        irom_size = (irom_load_addr - irom_load_addr_aligned) + irom_size;

        if (s_flash_seg_needs_map(irom_load_addr_aligned)) {
            mmu_hal_map_region(0, MMU_TARGET_FLASH0, irom_load_addr_aligned, irom_addr_aligned, irom_size, &actual_mapped_len);
            ESP_EARLY_LOGV(TAG, "after mapping text, starting from paddr=0x%08" PRIx32 " and vaddr=0x%08" PRIx32 ", 0x%" PRIx32 " bytes are mapped", irom_addr_aligned, irom_load_addr_aligned, actual_mapped_len);
        }
    }

    if (drom_load_addr_aligned != 0) {
        cache_bus_mask_t bus_mask = cache_ll_l1_get_bus(0, drom_load_addr_aligned, drom_size);
        cache_ll_l1_enable_bus(0, bus_mask);
    }

    if (irom_load_addr_aligned != 0) {
        cache_bus_mask_t bus_mask = cache_ll_l1_get_bus(0, irom_load_addr_aligned, irom_size);
        cache_ll_l1_enable_bus(0, bus_mask);
    }

#if !CONFIG_FREERTOS_UNICORE
    if (drom_load_addr_aligned != 0) {
        cache_bus_mask_t bus_mask = cache_ll_l1_get_bus(1, drom_load_addr_aligned, drom_size);
        cache_ll_l1_enable_bus(1, bus_mask);
    }

    if (irom_load_addr_aligned != 0) {
        cache_bus_mask_t bus_mask = cache_ll_l1_get_bus(1, irom_load_addr_aligned, irom_size);
        cache_ll_l1_enable_bus(1, bus_mask);
    }
#endif
}
#endif  // CONFIG_SECURE_ENABLE_TEE

static void set_cache_and_start_app(
    uint32_t drom_addr,
    uint32_t drom_load_addr,
    uint32_t drom_size,
    uint32_t irom_addr,
    uint32_t irom_load_addr,
    uint32_t irom_size,
    const esp_image_metadata_t *data)
{
    int rc __attribute__((unused));
    const uint32_t entry_addr = data->image.entry_addr;
    const uint32_t mmu_page_size = data->mmu_page_size;

    ESP_EARLY_LOGD(TAG, "configure drom and irom and start");
    //-----------------------Disable Cache to do the mapping---------
#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Disable(0);
    Cache_Flush(0);
#else
    cache_hal_disable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#endif

#if SOC_MMU_PAGE_SIZE_CONFIGURABLE
    // re-configure MMU page size
    mmu_ll_set_page_size(0, mmu_page_size);
#endif //SOC_MMU_PAGE_SIZE_CONFIGURABLE

    //reset MMU table first
    mmu_hal_unmap_all();

    //-----------------------MAP DROM--------------------------
    uint32_t drom_load_addr_aligned = drom_load_addr & MMU_FLASH_MASK_FROM_VAL(mmu_page_size);
    uint32_t drom_addr_aligned = drom_addr & MMU_FLASH_MASK_FROM_VAL(mmu_page_size);
    ESP_EARLY_LOGV(TAG, "rodata starts from paddr=0x%08" PRIx32 ", vaddr=0x%08" PRIx32 ", size=0x%" PRIx32, drom_addr, drom_load_addr, drom_size);
    //The addr is aligned, so we add the mask off length to the size, to make sure the corresponding buses are enabled.
    drom_size = (drom_load_addr - drom_load_addr_aligned) + drom_size;
#if CONFIG_IDF_TARGET_ESP32
    uint32_t drom_page_count = (drom_size + SPI_FLASH_MMU_PAGE_SIZE - 1) / SPI_FLASH_MMU_PAGE_SIZE;
    rc = cache_flash_mmu_set(0, 0, drom_load_addr_aligned, drom_addr_aligned, 64, drom_page_count);
    ESP_EARLY_LOGV(TAG, "rc=%d", rc);
    rc = cache_flash_mmu_set(1, 0, drom_load_addr_aligned, drom_addr_aligned, 64, drom_page_count);
    ESP_EARLY_LOGV(TAG, "rc=%d", rc);
    ESP_EARLY_LOGV(TAG, "after mapping rodata, starting from paddr=0x%08" PRIx32 " and vaddr=0x%08" PRIx32 ", 0x%" PRIx32 " bytes are mapped", drom_addr_aligned, drom_load_addr_aligned, drom_page_count * SPI_FLASH_MMU_PAGE_SIZE);
#else
    uint32_t actual_mapped_len = 0;
    if (s_flash_seg_needs_map(drom_load_addr_aligned)) {
        mmu_hal_map_region(0, MMU_TARGET_FLASH0, drom_load_addr_aligned, drom_addr_aligned, drom_size, &actual_mapped_len);
        ESP_EARLY_LOGV(TAG, "after mapping rodata, starting from paddr=0x%08" PRIx32 " and vaddr=0x%08" PRIx32 ", 0x%" PRIx32 " bytes are mapped", drom_addr_aligned, drom_load_addr_aligned, actual_mapped_len);
    }
    //we use the MMU_LL_END_DROM_ENTRY_ID mmu entry as a map page for app to find the boot partition
    mmu_hal_map_region(0, MMU_TARGET_FLASH0, MMU_DROM_END_ENTRY_VADDR_FROM_VAL(mmu_page_size), drom_addr_aligned, mmu_page_size, &actual_mapped_len);
    ESP_EARLY_LOGV(TAG, "mapped one page of the rodata, from paddr=0x%08" PRIx32 " and vaddr=0x%08" PRIx32 ", 0x%" PRIx32 " bytes are mapped", drom_addr_aligned, MMU_LL_END_DROM_ENTRY_VADDR, actual_mapped_len);
#endif

    //-----------------------MAP IROM--------------------------
    uint32_t irom_load_addr_aligned = irom_load_addr & MMU_FLASH_MASK_FROM_VAL(mmu_page_size);
    uint32_t irom_addr_aligned = irom_addr & MMU_FLASH_MASK_FROM_VAL(mmu_page_size);
    ESP_EARLY_LOGV(TAG, "text starts from paddr=0x%08" PRIx32 ", vaddr=0x%08" PRIx32 ", size=0x%" PRIx32, irom_addr, irom_load_addr, irom_size);
    //The addr is aligned, so we add the mask off length to the size, to make sure the corresponding buses are enabled.
    irom_size = (irom_load_addr - irom_load_addr_aligned) + irom_size;
#if CONFIG_IDF_TARGET_ESP32
    uint32_t irom_page_count = (irom_size + SPI_FLASH_MMU_PAGE_SIZE - 1) / SPI_FLASH_MMU_PAGE_SIZE;
    rc = cache_flash_mmu_set(0, 0, irom_load_addr_aligned, irom_addr_aligned, 64, irom_page_count);
    ESP_EARLY_LOGV(TAG, "rc=%d", rc);
    rc = cache_flash_mmu_set(1, 0, irom_load_addr_aligned, irom_addr_aligned, 64, irom_page_count);
    ESP_LOGV(TAG, "rc=%d", rc);
    ESP_EARLY_LOGV(TAG, "after mapping text, starting from paddr=0x%08" PRIx32 " and vaddr=0x%08" PRIx32 ", 0x%" PRIx32 " bytes are mapped", irom_addr_aligned, irom_load_addr_aligned, irom_page_count * SPI_FLASH_MMU_PAGE_SIZE);
#else
    if (s_flash_seg_needs_map(irom_load_addr_aligned)) {
        mmu_hal_map_region(0, MMU_TARGET_FLASH0, irom_load_addr_aligned, irom_addr_aligned, irom_size, &actual_mapped_len);
        ESP_EARLY_LOGV(TAG, "after mapping text, starting from paddr=0x%08" PRIx32 " and vaddr=0x%08" PRIx32 ", 0x%" PRIx32 " bytes are mapped", irom_addr_aligned, irom_load_addr_aligned, actual_mapped_len);
    }
#endif

    //----------------------Enable corresponding buses----------------
    cache_bus_mask_t bus_mask = cache_ll_l1_get_bus(0, drom_load_addr_aligned, drom_size);
    cache_ll_l1_enable_bus(0, bus_mask);
    bus_mask = cache_ll_l1_get_bus(0, irom_load_addr_aligned, irom_size);
    cache_ll_l1_enable_bus(0, bus_mask);

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    bus_mask = cache_ll_l1_get_bus(1, drom_load_addr_aligned, drom_size);
    cache_ll_l1_enable_bus(1, bus_mask);
    bus_mask = cache_ll_l1_get_bus(1, irom_load_addr_aligned, irom_size);
    cache_ll_l1_enable_bus(1, bus_mask);
#endif

#if CONFIG_SECURE_ENABLE_TEE
    //----------------------Unpacking and loading the TEE app----------------
    unpack_load_tee_app(&tee_data);
#endif

    //----------------------Enable Cache----------------
#if CONFIG_IDF_TARGET_ESP32
    // Application will need to do Cache_Flush(1) and Cache_Read_Enable(1)
    Cache_Read_Enable(0);
#else
    cache_hal_enable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#endif

    ESP_LOGD(TAG, "start: 0x%08"PRIx32, entry_addr);
    bootloader_atexit();

#if CONFIG_SECURE_ENABLE_TEE
    ESP_LOGI(TAG, "Current privilege level - %d", esp_cpu_get_curr_privilege_level());
    /* NOTE: TEE Initialization and REE Switch
     * This call will not return back. After TEE initialization,
     * it will switch to the REE and execute the user application.
     */
    typedef void (*esp_tee_init_t)(uint32_t, uint32_t, uint8_t) __attribute__((noreturn));
    esp_tee_init_t esp_tee_init = ((esp_tee_init_t) tee_data.image.entry_addr);

    ESP_LOGI(TAG, "Starting TEE: Entry point - 0x%"PRIx32, (uint32_t)esp_tee_init);
    (*esp_tee_init)(entry_addr, drom_addr, tee_boot_part);
#else
    typedef void (*entry_t)(void) __attribute__((noreturn));
    entry_t entry = ((entry_t) entry_addr);

    // TODO: we have used quite a bit of stack at this point.
    // use "movsp" instruction to reset stack back to where ROM stack starts.
    (*entry)();
#endif
}

void bootloader_reset(void)
{
#ifdef BOOTLOADER_BUILD
    bootloader_atexit();
    esp_rom_delay_us(1000); /* Allow last byte to leave FIFO */
    esp_rom_software_reset_system();
    while (1) { }       /* This line will never be reached, used to keep gcc happy */
#else
    abort();            /* This function should really not be called from application code */
#endif
}

void bootloader_atexit(void)
{
#ifdef BOOTLOADER_BUILD
    bootloader_console_deinit();
#else
    abort();
#endif
}

esp_err_t bootloader_sha256_hex_to_str(char *out_str, const uint8_t *in_array_hex, size_t len)
{
    if (out_str == NULL || in_array_hex == NULL || len == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < len; i++) {
        for (int shift = 0; shift < 2; shift++) {
            uint8_t nibble = (in_array_hex[i] >> (shift ? 0 : 4)) & 0x0F;
            if (nibble < 10) {
                out_str[i * 2 + shift] = '0' + nibble;
            } else {
                out_str[i * 2 + shift] = 'a' + nibble - 10;
            }
        }
    }
    return ESP_OK;
}

void bootloader_debug_buffer(const void *buffer, size_t length, const char *label)
{
#if CONFIG_BOOTLOADER_LOG_LEVEL >= 4
    const uint8_t *bytes = (const uint8_t *)buffer;
    const size_t output_len = MIN(length, 128);
    char hexbuf[128 * 2 + 1];

    bootloader_sha256_hex_to_str(hexbuf, bytes, output_len);

    hexbuf[output_len * 2] = '\0';
    ESP_LOGD(TAG, "%s: %s", label, hexbuf);
#else
    (void) buffer;
    (void) length;
    (void) label;
#endif
}

static esp_err_t bootloader_sha_flash_contents(esp_sha_type type, uint32_t flash_offset, uint32_t len, uint8_t *digest)
{
    if (digest == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Handling firmware images larger than MMU capacity */
    uint32_t mmu_free_pages_count = bootloader_mmap_get_free_pages();
    bootloader_sha_handle_t sha_handle = NULL;

    if (type == SHA2_256) {
        sha_handle = bootloader_sha256_start();
    } else
    // Using SOC_ECDSA_SUPPORT_CURVE_P384 here so that there is no flash size impact in the case of existing targets like ESP32.
#if SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384
    if (type == SHA2_384) {
        sha_handle = bootloader_sha512_start(true);
    } else
#endif /* SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384 */
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (sha_handle == NULL) {
        return ESP_ERR_NO_MEM;
    }

    while (len > 0) {
        uint32_t mmu_page_offset = ((flash_offset & MMAP_ALIGNED_MASK) != 0) ? 1 : 0; /* Skip 1st MMU Page if it is already populated */
        uint32_t max_pages = (mmu_free_pages_count > mmu_page_offset) ? (mmu_free_pages_count - mmu_page_offset) : 0;
        if (max_pages == 0) {
            ESP_LOGE(TAG, "No free MMU pages are available");
            if (type == SHA2_256) {
                bootloader_sha256_finish(sha_handle, NULL);
            }
#if SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384
            else if (type == SHA2_384) {
                bootloader_sha512_finish(sha_handle, NULL);
            }
#endif /* SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384 */
            return ESP_ERR_NO_MEM;
        }
        uint32_t max_image_len;
        if (__builtin_mul_overflow(max_pages, SPI_FLASH_MMU_PAGE_SIZE, &max_image_len)) {
            max_image_len = UINT32_MAX;
        }
        uint32_t partial_image_len = MIN(len, max_image_len); /* Read the image that fits in the free MMU pages */

        const void * image = bootloader_mmap(flash_offset, partial_image_len);
        if (image == NULL) {
            if (type == SHA2_256) {
                bootloader_sha256_finish(sha_handle, NULL);
            }
#if SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384
            else if (type == SHA2_384) {
                bootloader_sha512_finish(sha_handle, NULL);
            }
#endif /* SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384 */
            return ESP_FAIL;
        }

        if (type == SHA2_256) {
            bootloader_sha256_data(sha_handle, image, partial_image_len);
        }
#if SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384
        else if (type == SHA2_384) {
            bootloader_sha512_data(sha_handle, image, partial_image_len);
        }
#endif /* SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384 */

        bootloader_munmap(image);

        flash_offset += partial_image_len;
        len -= partial_image_len;
    }

    if (type == SHA2_256) {
        bootloader_sha256_finish(sha_handle, digest);
    }
#if SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384
    else if (type == SHA2_384) {
        bootloader_sha512_finish(sha_handle, digest);
    }
#endif /* SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384 */
    return ESP_OK;
}

esp_err_t bootloader_sha256_flash_contents(uint32_t flash_offset, uint32_t len, uint8_t *digest)
{
    return bootloader_sha_flash_contents(SHA2_256, flash_offset, len, digest);
}

#if SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384
esp_err_t bootloader_sha384_flash_contents(uint32_t flash_offset, uint32_t len, uint8_t *digest)
{
    return bootloader_sha_flash_contents(SHA2_384, flash_offset, len, digest);
}
#endif /* SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384 */
