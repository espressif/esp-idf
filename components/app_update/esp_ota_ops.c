// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_err.h"
#include "esp_partition.h"
#include "esp_spi_flash.h"
#include "esp_image_format.h"
#include "esp_secure_boot.h"
#include "esp_flash_encrypt.h"
#include "esp_spi_flash.h"
#include "sdkconfig.h"

#include "esp_ota_ops.h"
#include "sys/queue.h"
#include "esp32/rom/crc.h"
#include "esp_log.h"
#include "esp_flash_partitions.h"
#include "bootloader_common.h"
#include "sys/param.h"
#include "esp_system.h"
#include "esp_efuse.h"


#define SUB_TYPE_ID(i) (i & 0x0F)

typedef struct ota_ops_entry_ {
    uint32_t handle;
    const esp_partition_t *part;
    uint32_t erased_size;
    uint32_t wrote_size;
    uint8_t partial_bytes;
    uint8_t partial_data[16];
    LIST_ENTRY(ota_ops_entry_) entries;
} ota_ops_entry_t;

static LIST_HEAD(ota_ops_entries_head, ota_ops_entry_) s_ota_ops_entries_head =
    LIST_HEAD_INITIALIZER(s_ota_ops_entries_head);

static uint32_t s_ota_ops_last_handle = 0;

const static char *TAG = "esp_ota_ops";

/* Return true if this is an OTA app partition */
static bool is_ota_partition(const esp_partition_t *p)
{
    return (p != NULL
            && p->type == ESP_PARTITION_TYPE_APP
            && p->subtype >= ESP_PARTITION_SUBTYPE_APP_OTA_0
            && p->subtype < ESP_PARTITION_SUBTYPE_APP_OTA_MAX);
}

// Read otadata partition and fill array from two otadata structures.
// Also return pointer to otadata info partition.
static const esp_partition_t *read_otadata(esp_ota_select_entry_t *two_otadata)
{
    const esp_partition_t *otadata_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);

    if (otadata_partition == NULL) {
        ESP_LOGE(TAG, "not found otadata");
        return NULL;
    }

    spi_flash_mmap_handle_t ota_data_map;
    const void *result = NULL;
    esp_err_t err = esp_partition_mmap(otadata_partition, 0, otadata_partition->size, SPI_FLASH_MMAP_DATA, &result, &ota_data_map);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mmap otadata filed. Err=0x%8x", err);
        return NULL;
    } else {
        memcpy(&two_otadata[0], result, sizeof(esp_ota_select_entry_t));
        memcpy(&two_otadata[1], result + SPI_FLASH_SEC_SIZE, sizeof(esp_ota_select_entry_t));
        spi_flash_munmap(ota_data_map);
    }
    return otadata_partition;
}

static esp_err_t image_validate(const esp_partition_t *partition, esp_image_load_mode_t load_mode)
{
    esp_image_metadata_t data;
    const esp_partition_pos_t part_pos = {
        .offset = partition->address,
        .size = partition->size,
    };

    if (esp_image_verify(load_mode, &part_pos, &data) != ESP_OK) {
        return ESP_ERR_OTA_VALIDATE_FAILED;
    }

    return ESP_OK;
}

static esp_ota_img_states_t set_new_state_otadata(void)
{
#ifdef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
    ESP_LOGD(TAG, "Monitoring the first boot of the app is enabled.");
    return ESP_OTA_IMG_NEW;
#else
    return ESP_OTA_IMG_UNDEFINED;
#endif
}

esp_err_t esp_ota_begin(const esp_partition_t *partition, size_t image_size, esp_ota_handle_t *out_handle)
{
    ota_ops_entry_t *new_entry;
    esp_err_t ret = ESP_OK;

    if ((partition == NULL) || (out_handle == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    partition = esp_partition_verify(partition);
    if (partition == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    if (!is_ota_partition(partition)) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t* running_partition = esp_ota_get_running_partition();
    if (partition == running_partition) {
        return ESP_ERR_OTA_PARTITION_CONFLICT;
    }

#ifdef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
    esp_ota_img_states_t ota_state_running_part;
    if (esp_ota_get_state_partition(running_partition, &ota_state_running_part) == ESP_OK) {
        if (ota_state_running_part == ESP_OTA_IMG_PENDING_VERIFY) {
            ESP_LOGE(TAG, "Running app has not confirmed state (ESP_OTA_IMG_PENDING_VERIFY)");
            return ESP_ERR_OTA_ROLLBACK_INVALID_STATE;
        }
    }
#endif

    // If input image size is 0 or OTA_SIZE_UNKNOWN, erase entire partition
    if ((image_size == 0) || (image_size == OTA_SIZE_UNKNOWN)) {
        ret = esp_partition_erase_range(partition, 0, partition->size);
    } else {
        ret = esp_partition_erase_range(partition, 0, (image_size / SPI_FLASH_SEC_SIZE + 1) * SPI_FLASH_SEC_SIZE);
    }

    if (ret != ESP_OK) {
        return ret;
    }

    new_entry = (ota_ops_entry_t *) calloc(sizeof(ota_ops_entry_t), 1);
    if (new_entry == NULL) {
        return ESP_ERR_NO_MEM;
    }

    LIST_INSERT_HEAD(&s_ota_ops_entries_head, new_entry, entries);

    if ((image_size == 0) || (image_size == OTA_SIZE_UNKNOWN)) {
        new_entry->erased_size = partition->size;
    } else {
        new_entry->erased_size = image_size;
    }

    new_entry->part = partition;
    new_entry->handle = ++s_ota_ops_last_handle;
    *out_handle = new_entry->handle;
    return ESP_OK;
}

esp_err_t esp_ota_write(esp_ota_handle_t handle, const void *data, size_t size)
{
    const uint8_t *data_bytes = (const uint8_t *)data;
    esp_err_t ret;
    ota_ops_entry_t *it;

    if (data == NULL) {
        ESP_LOGE(TAG, "write data is invalid");
        return ESP_ERR_INVALID_ARG;
    }

    // find ota handle in linked list
    for (it = LIST_FIRST(&s_ota_ops_entries_head); it != NULL; it = LIST_NEXT(it, entries)) {
        if (it->handle == handle) {
            // must erase the partition before writing to it
            assert(it->erased_size > 0 && "must erase the partition before writing to it");
            if (it->wrote_size == 0 && it->partial_bytes == 0 && size > 0 && data_bytes[0] != ESP_IMAGE_HEADER_MAGIC) {
                ESP_LOGE(TAG, "OTA image has invalid magic byte (expected 0xE9, saw 0x%02x)", data_bytes[0]);
                return ESP_ERR_OTA_VALIDATE_FAILED;
            }

            if (esp_flash_encryption_enabled()) {
                /* Can only write 16 byte blocks to flash, so need to cache anything else */
                size_t copy_len;

                /* check if we have partially written data from earlier */
                if (it->partial_bytes != 0) {
                    copy_len = MIN(16 - it->partial_bytes, size);
                    memcpy(it->partial_data + it->partial_bytes, data_bytes, copy_len);
                    it->partial_bytes += copy_len;
                    if (it->partial_bytes != 16) {
                        return ESP_OK; /* nothing to write yet, just filling buffer */
                    }
                    /* write 16 byte to partition */
                    ret = esp_partition_write(it->part, it->wrote_size, it->partial_data, 16);
                    if (ret != ESP_OK) {
                        return ret;
                    }
                    it->partial_bytes = 0;
                    memset(it->partial_data, 0xFF, 16);
                    it->wrote_size += 16;
                    data_bytes += copy_len;
                    size -= copy_len;
                }

                /* check if we need to save trailing data that we're about to write */
                it->partial_bytes = size % 16;
                if (it->partial_bytes != 0) {
                    size -= it->partial_bytes;
                    memcpy(it->partial_data, data_bytes + size, it->partial_bytes);
                }
            }

            ret = esp_partition_write(it->part, it->wrote_size, data_bytes, size);
            if(ret == ESP_OK){
                it->wrote_size += size;
            }
            return ret;
        }
    }

    //if go to here ,means don't find the handle
    ESP_LOGE(TAG,"not found the handle");
    return ESP_ERR_INVALID_ARG;
}

esp_err_t esp_ota_end(esp_ota_handle_t handle)
{
    ota_ops_entry_t *it;
    esp_err_t ret = ESP_OK;

    for (it = LIST_FIRST(&s_ota_ops_entries_head); it != NULL; it = LIST_NEXT(it, entries)) {
        if (it->handle == handle) {
            break;
        }
    }

    if (it == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    /* 'it' holds the ota_ops_entry_t for 'handle' */

    // esp_ota_end() is only valid if some data was written to this handle
    if ((it->erased_size == 0) || (it->wrote_size == 0)) {
        ret = ESP_ERR_INVALID_ARG;
        goto cleanup;
    }

    if (it->partial_bytes > 0) {
        /* Write out last 16 bytes, if necessary */
        ret = esp_partition_write(it->part, it->wrote_size, it->partial_data, 16);
        if (ret != ESP_OK) {
            ret = ESP_ERR_INVALID_STATE;
            goto cleanup;
        }
        it->wrote_size += 16;
        it->partial_bytes = 0;
    }

    esp_image_metadata_t data;
    const esp_partition_pos_t part_pos = {
      .offset = it->part->address,
      .size = it->part->size,
    };

    if (esp_image_verify(ESP_IMAGE_VERIFY, &part_pos, &data) != ESP_OK) {
        ret = ESP_ERR_OTA_VALIDATE_FAILED;
        goto cleanup;
    }

 cleanup:
    LIST_REMOVE(it, entries);
    free(it);
    return ret;
}

static esp_err_t rewrite_ota_seq(esp_ota_select_entry_t *two_otadata, uint32_t seq, uint8_t sec_id, const esp_partition_t *ota_data_partition)
{
    if (two_otadata == NULL || sec_id > 1) {
        return ESP_ERR_INVALID_ARG;
    }

    two_otadata[sec_id].ota_seq = seq;
    two_otadata[sec_id].crc = bootloader_common_ota_select_crc(&two_otadata[sec_id]);
    esp_err_t ret = esp_partition_erase_range(ota_data_partition, sec_id * SPI_FLASH_SEC_SIZE, SPI_FLASH_SEC_SIZE);
    if (ret != ESP_OK) {
        return ret;
    } else {
        return esp_partition_write(ota_data_partition, SPI_FLASH_SEC_SIZE * sec_id, &two_otadata[sec_id], sizeof(esp_ota_select_entry_t));
    }
}

static uint8_t get_ota_partition_count(void)
{
    uint16_t ota_app_count = 0;
    while (esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_MIN + ota_app_count, NULL) != NULL) {
            assert(ota_app_count < 16 && "must erase the partition before writing to it");
            ota_app_count++;
    }
    return ota_app_count;
}

static esp_err_t esp_rewrite_ota_data(esp_partition_subtype_t subtype)
{
    esp_ota_select_entry_t otadata[2];
    const esp_partition_t *otadata_partition = read_otadata(otadata);
    if (otadata_partition == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    int ota_app_count = get_ota_partition_count();
    if (SUB_TYPE_ID(subtype) >= ota_app_count) {
        return ESP_ERR_INVALID_ARG;
    }

    //esp32_idf use two sector for store information about which partition is running
    //it defined the two sector as ota data partition,two structure esp_ota_select_entry_t is saved in the two sector
    //named data in first sector as otadata[0], second sector data as otadata[1]
    //e.g.
    //if otadata[0].ota_seq == otadata[1].ota_seq == 0xFFFFFFFF,means ota info partition is in init status
    //so it will boot factory application(if there is),if there's no factory application,it will boot ota[0] application
    //if otadata[0].ota_seq != 0 and otadata[1].ota_seq != 0,it will choose a max seq ,and get value of max_seq%max_ota_app_number
    //and boot a subtype (mask 0x0F) value is (max_seq - 1)%max_ota_app_number,so if want switch to run ota[x],can use next formulas.
    //for example, if otadata[0].ota_seq = 4, otadata[1].ota_seq = 5, and there are 8 ota application,
    //current running is (5-1)%8 = 4,running ota[4],so if we want to switch to run ota[7],
    //we should add otadata[0].ota_seq (is 4) to 4 ,(8-1)%8=7,then it will boot ota[7]
    //if      A=(B - C)%D
    //then    B=(A + C)%D + D*n ,n= (0,1,2...)
    //so current ota app sub type id is x , dest bin subtype is y,total ota app count is n
    //seq will add (x + n*1 + 1 - seq)%n

    int active_otadata = bootloader_common_get_active_otadata(otadata);
    if (active_otadata != -1) {
        uint32_t seq = otadata[active_otadata].ota_seq;
        uint32_t i = 0;
        while (seq > (SUB_TYPE_ID(subtype) + 1) % ota_app_count + i * ota_app_count) {
            i++;
        }
        int next_otadata = (~active_otadata)&1; // if 0 -> will be next 1. and if 1 -> will be next 0.
        otadata[next_otadata].ota_state = set_new_state_otadata();
        return rewrite_ota_seq(otadata, (SUB_TYPE_ID(subtype) + 1) % ota_app_count + i * ota_app_count, next_otadata, otadata_partition);
    } else {
        /* Both OTA slots are invalid, probably because unformatted... */
        int next_otadata = 0;
        otadata[next_otadata].ota_state = set_new_state_otadata();
        return rewrite_ota_seq(otadata, SUB_TYPE_ID(subtype) + 1, next_otadata, otadata_partition);
    }
}

esp_err_t esp_ota_set_boot_partition(const esp_partition_t *partition)
{
    if (partition == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (image_validate(partition, ESP_IMAGE_VERIFY) != ESP_OK) {
        return ESP_ERR_OTA_VALIDATE_FAILED;
    }

    // if set boot partition to factory bin ,just format ota info partition
    if (partition->type == ESP_PARTITION_TYPE_APP) {
        if (partition->subtype == ESP_PARTITION_SUBTYPE_APP_FACTORY) {
            const esp_partition_t *find_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);
            if (find_partition != NULL) {
                return esp_partition_erase_range(find_partition, 0, find_partition->size);
            } else {
                return ESP_ERR_NOT_FOUND;
            }
        } else {
#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
            esp_app_desc_t partition_app_desc;
            esp_err_t err = esp_ota_get_partition_description(partition, &partition_app_desc);
            if (err != ESP_OK) {
                return err;
            }

            if (esp_efuse_check_secure_version(partition_app_desc.secure_version) == false) {
                ESP_LOGE(TAG, "This a new partition can not be booted due to a secure version is lower than stored in efuse. Partition will be erased.");
                esp_err_t err = esp_partition_erase_range(partition, 0, partition->size);
                if (err != ESP_OK) {
                    return err;
                }
                return ESP_ERR_OTA_SMALL_SEC_VER;
            }
#endif
            return esp_rewrite_ota_data(partition->subtype);
        }
    } else {
        return ESP_ERR_INVALID_ARG;
    }
}

static const esp_partition_t *find_default_boot_partition(void)
{
    // This logic matches the logic of bootloader get_selected_boot_partition() & load_boot_image().

    // Default to factory if present
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
    if (result != NULL) {
        return result;
    }

    // Try first OTA slot if no factory partition
    for (esp_partition_subtype_t s = ESP_PARTITION_SUBTYPE_APP_OTA_MIN; s != ESP_PARTITION_SUBTYPE_APP_OTA_MAX; s++) {
        result = esp_partition_find_first(ESP_PARTITION_TYPE_APP, s, NULL);
        if (result != NULL) {
            return result;
        }
    }

    // Test app slot if present
    result = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_TEST, NULL);
    if (result != NULL) {
        return result;
    }

    ESP_LOGE(TAG, "invalid partition table, no app partitions");
    return NULL;
}

const esp_partition_t *esp_ota_get_boot_partition(void)
{
    esp_ota_select_entry_t otadata[2];
    const esp_partition_t *otadata_partition = read_otadata(otadata);
    if (otadata_partition == NULL) {
        return NULL;
    }

    int ota_app_count = get_ota_partition_count();
    ESP_LOGD(TAG, "found ota app max = %d", ota_app_count);

    if ((bootloader_common_ota_select_invalid(&otadata[0]) &&
         bootloader_common_ota_select_invalid(&otadata[1])) ||
         ota_app_count == 0) {
        ESP_LOGD(TAG, "finding factory app...");
        return find_default_boot_partition();
    } else {
        int active_otadata = bootloader_common_get_active_otadata(otadata);
        if (active_otadata != -1) {
            int ota_slot = (otadata[active_otadata].ota_seq - 1) % ota_app_count; // Actual OTA partition selection
            ESP_LOGD(TAG, "finding ota_%d app...", ESP_PARTITION_SUBTYPE_APP_OTA_MIN + ota_slot);
            return esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_MIN + ota_slot, NULL);
        } else {
            ESP_LOGE(TAG, "ota data invalid, no current app. Assuming factory");
            return find_default_boot_partition();
        }
    }
}


const esp_partition_t* esp_ota_get_running_partition(void)
{
    static const esp_partition_t *curr_partition = NULL;

    /*
     * Currently running partition is unlikely to change across reset cycle,
     * so it can be cached here, and avoid lookup on every flash write operation.
     */
    if (curr_partition != NULL) {
        return curr_partition;
    }

    /* Find the flash address of this exact function. By definition that is part
       of the currently running firmware. Then find the enclosing partition. */
    size_t phys_offs = spi_flash_cache2phys(esp_ota_get_running_partition);

    assert (phys_offs != SPI_FLASH_CACHE2PHYS_FAIL); /* indicates cache2phys lookup is buggy */

    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_APP,
                                                     ESP_PARTITION_SUBTYPE_ANY,
                                                     NULL);
    assert(it != NULL); /* has to be at least one app partition */

    while (it != NULL) {
        const esp_partition_t *p = esp_partition_get(it);
        if (p->address <= phys_offs && p->address + p->size > phys_offs) {
            esp_partition_iterator_release(it);
            curr_partition = p;
            return p;
        }
        it = esp_partition_next(it);
    }

    abort(); /* Partition table is invalid or corrupt */
}


const esp_partition_t* esp_ota_get_next_update_partition(const esp_partition_t *start_from)
{
    const esp_partition_t *default_ota = NULL;
    bool next_is_result = false;
    if (start_from == NULL) {
        start_from = esp_ota_get_running_partition();
    } else {
        start_from = esp_partition_verify(start_from);
    }
    assert (start_from != NULL);
    /* at this point, 'start_from' points to actual partition table data in flash */


    /* Two possibilities: either we want the OTA partition immediately after the current running OTA partition, or we
       want the first OTA partition in the table (for the case when the last OTA partition is the running partition, or
       if the current running partition is not OTA.)

       This loop iterates subtypes instead of using esp_partition_find, so we
       get all OTA partitions in a known order (low slot to high slot).
    */

    for (esp_partition_subtype_t t = ESP_PARTITION_SUBTYPE_APP_OTA_0;
         t != ESP_PARTITION_SUBTYPE_APP_OTA_MAX;
         t++) {
        const esp_partition_t *p = esp_partition_find_first(ESP_PARTITION_TYPE_APP, t, NULL);
        if (p == NULL) {
            continue;
        }

        if (default_ota == NULL) {
            /* Default to first OTA partition we find,
               will be used if nothing else matches */
            default_ota = p;
        }

        if (p == start_from) {
            /* Next OTA partition is the one to use */
            next_is_result = true;
        }
        else if (next_is_result) {
            return p;
        }
    }

    return default_ota;

}

esp_err_t esp_ota_get_partition_description(const esp_partition_t *partition, esp_app_desc_t *app_desc)
{
    if (partition == NULL || app_desc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if(partition->type != ESP_PARTITION_TYPE_APP) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_err_t err = esp_partition_read(partition, sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t), app_desc, sizeof(esp_app_desc_t));
    if (err != ESP_OK) {
        return err;
    }

    if (app_desc->magic_word != ESP_APP_DESC_MAGIC_WORD) {
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
static esp_err_t esp_ota_set_anti_rollback(void) {
    const esp_app_desc_t *app_desc = esp_ota_get_app_description();
    return esp_efuse_update_secure_version(app_desc->secure_version);
}
#endif

// Checks applications on the slots which can be booted in case of rollback.
// Returns true if the slots have at least one app (except the running app).
bool esp_ota_check_rollback_is_possible(void)
{
    esp_ota_select_entry_t otadata[2];
    if (read_otadata(otadata) == NULL) {
        return false;
    }

    int ota_app_count = get_ota_partition_count();
    if (ota_app_count == 0) {
        return false;
    }

    bool valid_otadata[2];
    valid_otadata[0] = bootloader_common_ota_select_valid(&otadata[0]);
    valid_otadata[1] = bootloader_common_ota_select_valid(&otadata[1]);

    int active_ota = bootloader_common_select_otadata(otadata, valid_otadata, true);
    if (active_ota == -1) {
        return false;
    }
    int last_active_ota = (~active_ota)&1;

    const esp_partition_t *partition = NULL;
#ifndef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
    if (valid_otadata[last_active_ota] == false) {
        partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
        if (partition != NULL) {
            if(image_validate(partition, ESP_IMAGE_VERIFY_SILENT) == ESP_OK) {
                return true;
            }
        }
    }
#endif

    if (valid_otadata[last_active_ota] == true) {
        int slot = (otadata[last_active_ota].ota_seq - 1) % ota_app_count;
        partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_MIN + slot, NULL);
        if (partition != NULL) {
            if(image_validate(partition, ESP_IMAGE_VERIFY_SILENT) == ESP_OK) {
#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
                esp_app_desc_t app_desc;
                if (esp_ota_get_partition_description(partition, &app_desc) == ESP_OK &&
                    esp_efuse_check_secure_version(app_desc.secure_version) == true) {
                    return true;
                }
#else
                return true;
#endif
            }
        }
    }
    return false;
}

// if valid == false - will done rollback with reboot. After reboot will boot previous OTA[x] or Factory partition.
// if valid == true  - it confirm that current OTA[x] is workable. Reboot will not happen.
static esp_err_t esp_ota_current_ota_is_workable(bool valid)
{
    esp_ota_select_entry_t otadata[2];
    const esp_partition_t *otadata_partition = read_otadata(otadata);
    if (otadata_partition == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    int active_otadata = bootloader_common_get_active_otadata(otadata);
    if (active_otadata != -1 && get_ota_partition_count() != 0) {
        if (valid == true && otadata[active_otadata].ota_state != ESP_OTA_IMG_VALID) {
            otadata[active_otadata].ota_state = ESP_OTA_IMG_VALID;
            ESP_LOGD(TAG, "OTA[current] partition is marked as VALID");
            esp_err_t err = rewrite_ota_seq(otadata, otadata[active_otadata].ota_seq, active_otadata, otadata_partition);
#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
            if (err == ESP_OK) {
                return esp_ota_set_anti_rollback();
            }
#endif
            return err;
        } else if (valid == false) {
            if (esp_ota_check_rollback_is_possible() == false) {
                ESP_LOGE(TAG, "Rollback is not possible, do not have any suitable apps in slots");
                return ESP_ERR_OTA_ROLLBACK_FAILED;
            }
            ESP_LOGD(TAG, "OTA[current] partition is marked as INVALID");
            otadata[active_otadata].ota_state = ESP_OTA_IMG_INVALID;
            esp_err_t err = rewrite_ota_seq(otadata, otadata[active_otadata].ota_seq, active_otadata, otadata_partition);
            if (err != ESP_OK) {
                return err;
            }
            ESP_LOGI(TAG, "Rollback to previously worked partition. Restart.");
            esp_restart();
        }
    } else {
        ESP_LOGE(TAG, "Running firmware is factory");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_ota_mark_app_valid_cancel_rollback(void)
{
    return esp_ota_current_ota_is_workable(true);
}

esp_err_t esp_ota_mark_app_invalid_rollback_and_reboot(void)
{
    return esp_ota_current_ota_is_workable(false);
}

static bool check_invalid_otadata (const esp_ota_select_entry_t *s) {
    return s->ota_seq != UINT32_MAX &&
           s->crc == bootloader_common_ota_select_crc(s) &&
           (s->ota_state == ESP_OTA_IMG_INVALID ||
            s->ota_state == ESP_OTA_IMG_ABORTED);
}

static int get_last_invalid_otadata(const esp_ota_select_entry_t *two_otadata)
{

    bool invalid_otadata[2];
    invalid_otadata[0] = check_invalid_otadata(&two_otadata[0]);
    invalid_otadata[1] = check_invalid_otadata(&two_otadata[1]);
    int num_invalid_otadata = bootloader_common_select_otadata(two_otadata, invalid_otadata, false);
    ESP_LOGD(TAG, "Invalid otadata[%d]", num_invalid_otadata);
    return num_invalid_otadata;
}

const esp_partition_t* esp_ota_get_last_invalid_partition(void)
{
    esp_ota_select_entry_t otadata[2];
    if (read_otadata(otadata) == NULL) {
        return NULL;
    }

    int invalid_otadata = get_last_invalid_otadata(otadata);

    int ota_app_count = get_ota_partition_count();
    if (invalid_otadata != -1 && ota_app_count != 0) {
        int ota_slot = (otadata[invalid_otadata].ota_seq - 1) % ota_app_count;
        ESP_LOGD(TAG, "Find invalid ota_%d app", ESP_PARTITION_SUBTYPE_APP_OTA_MIN + ota_slot);

        const esp_partition_t* invalid_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_MIN + ota_slot, NULL);
        if (invalid_partition != NULL) {
            if (image_validate(invalid_partition, ESP_IMAGE_VERIFY_SILENT) != ESP_OK) {
                ESP_LOGD(TAG, "Last invalid partition has corrupted app");
                return NULL;
            }
        }
        return invalid_partition;
    }
    return NULL;
}

esp_err_t esp_ota_get_state_partition(const esp_partition_t *partition, esp_ota_img_states_t *ota_state)
{
    if (partition == NULL || ota_state == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!is_ota_partition(partition)) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_ota_select_entry_t otadata[2];
    int ota_app_count = get_ota_partition_count();
    if (read_otadata(otadata) == NULL || ota_app_count == 0) {
        return ESP_ERR_NOT_FOUND;
    }

    int req_ota_slot = partition->subtype - ESP_PARTITION_SUBTYPE_APP_OTA_MIN;
    bool not_found = true;
    for (int i = 0; i < 2; ++i) {
        int ota_slot = (otadata[i].ota_seq - 1) % ota_app_count;
        if (ota_slot == req_ota_slot && otadata[i].crc == bootloader_common_ota_select_crc(&otadata[i])) {
            *ota_state = otadata[i].ota_state;
            not_found = false;
            break;
        }
    }

    if (not_found) {
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

esp_err_t esp_ota_erase_last_boot_app_partition(void)
{
    esp_ota_select_entry_t otadata[2];
    const esp_partition_t* ota_data_partition = read_otadata(otadata);
    if (ota_data_partition == NULL) {
        return ESP_FAIL;
    }

    int active_otadata = bootloader_common_get_active_otadata(otadata);
    int ota_app_count = get_ota_partition_count();
    if (active_otadata == -1 || ota_app_count == 0) {
        return ESP_FAIL;
    }

    int inactive_otadata = (~active_otadata)&1;
    if (otadata[inactive_otadata].ota_seq == UINT32_MAX || otadata[inactive_otadata].crc != bootloader_common_ota_select_crc(&otadata[inactive_otadata])) {
        return ESP_FAIL;
    }

    int ota_slot = (otadata[inactive_otadata].ota_seq - 1) % ota_app_count; // Actual OTA partition selection
    ESP_LOGD(TAG, "finding last_boot_app_partition ota_%d app...", ESP_PARTITION_SUBTYPE_APP_OTA_MIN + ota_slot);

    const esp_partition_t* last_boot_app_partition_from_otadata = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_MIN + ota_slot, NULL);
    if (last_boot_app_partition_from_otadata == NULL) {
        return ESP_FAIL;
    }

    const esp_partition_t* running_partition = esp_ota_get_running_partition();
    if (running_partition == NULL || last_boot_app_partition_from_otadata == running_partition) {
        return ESP_FAIL;
    }

    esp_err_t err = esp_partition_erase_range(last_boot_app_partition_from_otadata, 0, last_boot_app_partition_from_otadata->size);
    if (err != ESP_OK) {
        return err;
    }

    int sec_id = inactive_otadata;
    err = esp_partition_erase_range(ota_data_partition, sec_id * SPI_FLASH_SEC_SIZE, SPI_FLASH_SEC_SIZE);
    if (err != ESP_OK) {
        return err;
    }

    return ESP_OK;
}
