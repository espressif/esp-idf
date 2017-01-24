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
#include "esp_ota_select.h"
#include "esp_secure_boot.h"
#include "esp_flash_encrypt.h"
#include "sdkconfig.h"

#include "esp_ota_ops.h"
#include "rom/queue.h"
#include "rom/crc.h"
#include "soc/dport_reg.h"
#include "esp_log.h"


#define OTA_MAX(a,b) ((a) >= (b) ? (a) : (b)) 
#define OTA_MIN(a,b) ((a) <= (b) ? (a) : (b)) 
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

    if (partition == esp_ota_get_running_partition()) {
        return ESP_ERR_OTA_PARTITION_CONFLICT;
    }

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

            if(it->wrote_size == 0 && size > 0 && data_bytes[0] != 0xE9) {
                ESP_LOGE(TAG, "OTA image has invalid magic byte (expected 0xE9, saw 0x%02x", data_bytes[0]);
                return ESP_ERR_OTA_VALIDATE_FAILED;
            }

            if (esp_flash_encryption_enabled()) {
                /* Can only write 16 byte blocks to flash, so need to cache anything else */
                size_t copy_len;

                /* check if we have partially written data from earlier */
                if (it->partial_bytes != 0) {
                    copy_len = OTA_MIN(16 - it->partial_bytes, size);
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
    ESP_LOGE(TAG,"not found the handle")
    return ESP_ERR_INVALID_ARG;
}

esp_err_t esp_ota_end(esp_ota_handle_t handle)
{
    ota_ops_entry_t *it;
    size_t image_size;
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

    if (esp_image_basic_verify(it->part->address, true, &image_size) != ESP_OK) {
        ret = ESP_ERR_OTA_VALIDATE_FAILED;
        goto cleanup;
    }

#ifdef CONFIG_SECURE_BOOT_ENABLED
    ret = esp_secure_boot_verify_signature(it->part->address, image_size);
    if (ret != ESP_OK) {
        ret = ESP_ERR_OTA_VALIDATE_FAILED;
        goto cleanup;
    }
#endif

 cleanup:
    LIST_REMOVE(it, entries);
    free(it);
    return ret;
}

static esp_err_t esp_ota_read_selectors(esp_ota_select_entry_t ss[2],
                                        const esp_partition_t **dpp)
{
    esp_err_t ret;
    spi_flash_mmap_memory_t ota_data_mmap_handle;
    const void *ota_select_map = NULL;

    const esp_partition_t * dp = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);
    if (dp == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    ret = esp_partition_mmap(dp, 0, dp->size, SPI_FLASH_MMAP_DATA,
                             &ota_select_map, &ota_data_mmap_handle);
    if (ret != ESP_OK) {
        return ret;
    }
    memcpy(&ss[0], ota_select_map, sizeof(ss[0]));
    memcpy(&ss[1], (uint8_t *)ota_select_map + SPI_FLASH_SEC_SIZE, sizeof(ss[1]));
    spi_flash_munmap(ota_data_mmap_handle);
    if (dpp != NULL) *dpp = dp;
    return ESP_OK;
}

static esp_err_t esp_ota_set_boot_subtype(esp_partition_subtype_t subtype)
{
    const esp_partition_t *dp = NULL;
    esp_ota_select_entry_t ss[2];
    esp_err_t ret = esp_ota_read_selectors(ss, &dp);
    if (ret != ESP_OK) {
        return ret;
    }

    size_t offset = 0;
    uint32_t new_seq = 0;
    const esp_ota_select_entry_t *cs = esp_ota_choose_current(ss);

    /* Avoid flashing if no change. */
    if (cs != NULL && cs->boot_app_subtype == subtype) {
      return ESP_OK;
    }

    esp_ota_select_entry_t *s = NULL;
    if (cs == &s[0]) {
        s = &ss[1];
        offset = SPI_FLASH_SEC_SIZE;
        new_seq = cs->seq + 1;
    } else if (cs == &s[1]) {
        s = &ss[0];
        offset = 0;
        new_seq = cs->seq + 1;
    } else {
        /* Ok, let it be 0 then. */
        s = &ss[0];
        offset = 0;
        new_seq = 1;
    }
    s->seq = new_seq;
    s->boot_app_subtype = subtype;
    s->crc = esp_ota_select_crc(s);

    ESP_LOGI(TAG, "New OTA data %d: seq 0x%08x, st 0x%02x, CRC 0x%08x",
             (offset == 0 ? 0 : 1), s->seq, s->boot_app_subtype, s->crc);
    /* Safety check, this should never happen. */
    if (!esp_ota_select_valid(s)) {
        ESP_LOGE(TAG, "Newly-constructed entry invalid!");
        return ESP_ERR_INVALID_CRC;
    }

    ret = esp_partition_erase_range(dp, offset, SPI_FLASH_SEC_SIZE);
    if (ret != ESP_OK) {
        return ret;
    }

    return esp_partition_write(dp, offset, s, sizeof(*s));
}

esp_err_t esp_ota_set_boot_partition(const esp_partition_t *partition)
{
    size_t image_size;
    if (partition == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (esp_image_basic_verify(partition->address, true, &image_size) != ESP_OK) {
        return ESP_ERR_OTA_VALIDATE_FAILED;
    }

#ifdef CONFIG_SECURE_BOOT_ENABLED
    if (esp_secure_boot_verify_signature(partition->address, image_size) != ESP_OK) {
        return ESP_ERR_OTA_VALIDATE_FAILED;
    }
#endif
    if (partition->type != ESP_PARTITION_TYPE_APP) {
        return ESP_ERR_INVALID_ARG;
    }

    return esp_ota_set_boot_subtype(partition->subtype);
}

const esp_partition_t *esp_ota_get_boot_partition(void)
{
    return esp_partition_get_boot_partition();
}


const esp_partition_t* esp_ota_get_running_partition(void)
{
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
