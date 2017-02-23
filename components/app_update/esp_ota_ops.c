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
#include "sdkconfig.h"

#include "esp_ota_ops.h"
#include "rom/queue.h"
#include "rom/crc.h"
#include "esp_log.h"


#define OTA_MAX(a,b) ((a) >= (b) ? (a) : (b)) 
#define OTA_MIN(a,b) ((a) <= (b) ? (a) : (b)) 
#define SUB_TYPE_ID(i) (i & 0x0F) 

typedef struct ota_ops_entry_ {
    uint32_t handle;
    esp_partition_t part;
    uint32_t erased_size;
    uint32_t wrote_size;
#ifdef CONFIG_FLASH_ENCRYPTION_ENABLED
    uint8_t partial_bytes;
    uint8_t partial_data[16];
#endif
    LIST_ENTRY(ota_ops_entry_) entries;
} ota_ops_entry_t;

/* OTA selection structure (two copies in the OTA data partition.)
   Size of 32 bytes is friendly to flash encryption */
typedef struct {
    uint32_t ota_seq;
    uint8_t  seq_label[24];
    uint32_t crc;                /* CRC32 of ota_seq field only */
} ota_select;

static LIST_HEAD(ota_ops_entries_head, ota_ops_entry_) s_ota_ops_entries_head =
    LIST_HEAD_INITIALIZER(s_ota_ops_entries_head);

static uint32_t s_ota_ops_last_handle = 0;
static ota_select s_ota_select[2];

const static char *TAG = "esp_ota_ops";

esp_err_t esp_ota_begin(const esp_partition_t *partition, size_t image_size, esp_ota_handle_t *out_handle)
{
    esp_err_t ret = ESP_OK;

    if ((partition == NULL) || (out_handle == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    ota_ops_entry_t *new_entry = (ota_ops_entry_t *) calloc(sizeof(ota_ops_entry_t), 1);

    if (new_entry == 0) {
        return ESP_ERR_NO_MEM;
    }

    // if input image size is 0 or OTA_SIZE_UNKNOWN, will erase all areas in this partition
    if ((image_size == 0) || (image_size == OTA_SIZE_UNKNOWN)) {
        ret = esp_partition_erase_range(partition, 0, partition->size);
    } else {
        ret = esp_partition_erase_range(partition, 0, (image_size / SPI_FLASH_SEC_SIZE + 1) * SPI_FLASH_SEC_SIZE);
    }

    if (ret != ESP_OK) {
        free(new_entry);
        new_entry = NULL;
        return ret;
    }

    LIST_INSERT_HEAD(&s_ota_ops_entries_head, new_entry, entries);

    if ((image_size == 0) || (image_size == OTA_SIZE_UNKNOWN)) {
        new_entry->erased_size = partition->size;
    } else {
        new_entry->erased_size = image_size;
    }

    memcpy(&new_entry->part, partition, sizeof(esp_partition_t));
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

#ifdef CONFIG_FLASH_ENCRYPTION_ENABLED
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
                    ret = esp_partition_write(&it->part, it->wrote_size, it->partial_data, 16);
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
#endif

            ret = esp_partition_write(&it->part, it->wrote_size, data_bytes, size);
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

#ifdef CONFIG_FLASH_ENCRYPTION_ENABLED
    if (it->partial_bytes > 0 && esp_flash_encryption_enabled()) {
        /* Write out last 16 bytes, if necessary */
        ret = esp_partition_write(&it->part, it->wrote_size, it->partial_data, 16);
        if (ret != ESP_OK) {
            ret = ESP_ERR_INVALID_STATE;
            goto cleanup;
        }
        it->wrote_size += 16;
        it->partial_bytes = 0;
    }
#endif

    if (esp_image_basic_verify(it->part.address, true, &image_size) != ESP_OK) {
        ret = ESP_ERR_OTA_VALIDATE_FAILED;
        goto cleanup;
    }

#ifdef CONFIG_SECURE_BOOT_ENABLED
    ret = esp_secure_boot_verify_signature(it->part.address, image_size);
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

static uint32_t ota_select_crc(const ota_select *s)
{
    return crc32_le(UINT32_MAX, (uint8_t *)&s->ota_seq, 4);
}

static bool ota_select_valid(const ota_select *s)
{
    return s->ota_seq != UINT32_MAX && s->crc == ota_select_crc(s);
}

static esp_err_t rewrite_ota_seq(uint32_t seq, uint8_t sec_id, const esp_partition_t *ota_data_partition)
{
    esp_err_t ret;

    if (sec_id == 0 || sec_id == 1) {
        s_ota_select[sec_id].ota_seq = seq;
        s_ota_select[sec_id].crc = ota_select_crc(&s_ota_select[sec_id]);
        ret = esp_partition_erase_range(ota_data_partition, sec_id * SPI_FLASH_SEC_SIZE, SPI_FLASH_SEC_SIZE);
        if (ret != ESP_OK) {
            return ret;
        } else {
            return esp_partition_write(ota_data_partition, SPI_FLASH_SEC_SIZE * sec_id, &s_ota_select[sec_id].ota_seq, sizeof(ota_select));
        }
    } else {
        return ESP_ERR_INVALID_ARG;
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
    esp_err_t ret;
    const esp_partition_t *find_partition = NULL;
    uint16_t ota_app_count = 0;
    uint32_t i = 0;
    uint32_t seq;
    static spi_flash_mmap_memory_t ota_data_map;
    const void *result = NULL;

    find_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);
    if (find_partition != NULL) {
        ota_app_count = get_ota_partition_count();
        //esp32_idf use two sector for store information about which partition is running
        //it defined the two sector as ota data partition,two structure ota_select is saved in the two sector
        //named data in first sector as s_ota_select[0], second sector data as s_ota_select[1]
        //e.g.
        //if s_ota_select[0].ota_seq == s_ota_select[1].ota_seq == 0xFFFFFFFF,means ota info partition is in init status
        //so it will boot factory application(if there is),if there's no factory application,it will boot ota[0] application 
        //if s_ota_select[0].ota_seq != 0 and s_ota_select[1].ota_seq != 0,it will choose a max seq ,and get value of max_seq%max_ota_app_number
        //and boot a subtype (mask 0x0F) value is (max_seq - 1)%max_ota_app_number,so if want switch to run ota[x],can use next formulas.
        //for example, if s_ota_select[0].ota_seq = 4, s_ota_select[1].ota_seq = 5, and there are 8 ota application, 
        //current running is (5-1)%8 = 4,running ota[4],so if we want to switch to run ota[7],
        //we should add s_ota_select[0].ota_seq (is 4) to 4 ,(8-1)%8=7,then it will boot ota[7]
        //if      A=(B - C)%D
        //then    B=(A + C)%D + D*n ,n= (0,1,2...)
        //so current ota app sub type id is x , dest bin subtype is y,total ota app count is n
        //seq will add (x + n*1 + 1 - seq)%n
        if (SUB_TYPE_ID(subtype) >= ota_app_count) {
            return ESP_ERR_NOT_FOUND;
        }

        ret = esp_partition_mmap(find_partition, 0, find_partition->size, SPI_FLASH_MMAP_DATA, &result, &ota_data_map);
        if (ret != ESP_OK) {
            result = NULL;
            return ret;
        } else {
            memcpy(&s_ota_select[0], result, sizeof(ota_select));
            memcpy(&s_ota_select[1], result + SPI_FLASH_SEC_SIZE, sizeof(ota_select));
            spi_flash_munmap(ota_data_map);
        }

        if (ota_select_valid(&s_ota_select[0]) && ota_select_valid(&s_ota_select[1])) {
            seq = OTA_MAX(s_ota_select[0].ota_seq, s_ota_select[1].ota_seq);
            while (seq > (SUB_TYPE_ID(subtype) + 1) % ota_app_count + i * ota_app_count) {
                i++;
            }

            if (s_ota_select[0].ota_seq >= s_ota_select[1].ota_seq) {
                return rewrite_ota_seq((SUB_TYPE_ID(subtype) + 1) % ota_app_count + i * ota_app_count, 0, find_partition);
            } else {
                return rewrite_ota_seq((SUB_TYPE_ID(subtype) + 1) % ota_app_count + i * ota_app_count, 1, find_partition);
            }

        } else if (ota_select_valid(&s_ota_select[0])) {
            while (s_ota_select[0].ota_seq > (SUB_TYPE_ID(subtype) + 1) % ota_app_count + i * ota_app_count) {
                i++;
            }
            return rewrite_ota_seq((SUB_TYPE_ID(subtype) + 1) % ota_app_count + i * ota_app_count, 1, find_partition);

        } else if (ota_select_valid(&s_ota_select[1])) {
            while (s_ota_select[1].ota_seq > (SUB_TYPE_ID(subtype) + 1) % ota_app_count + i * ota_app_count) {
                i++;
            }
            return rewrite_ota_seq((SUB_TYPE_ID(subtype) + 1) % ota_app_count + i * ota_app_count, 0, find_partition);

        } else {
            /* Both OTA slots are invalid, probably because unformatted... */
            return rewrite_ota_seq(SUB_TYPE_ID(subtype) + 1, 0, find_partition);
        }

    } else {
        return ESP_ERR_NOT_FOUND;
    }
}

esp_err_t esp_ota_set_boot_partition(const esp_partition_t *partition)
{
    size_t image_size;
    const esp_partition_t *find_partition = NULL;
    if (partition == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (esp_image_basic_verify(partition->address, true, &image_size) != ESP_OK) {
        return ESP_ERR_OTA_VALIDATE_FAILED;
    }

#ifdef CONFIG_SECURE_BOOT_ENABLED
    esp_err_t ret = esp_secure_boot_verify_signature(partition->address, image_size);
    if (ret != ESP_OK) {
        return ESP_ERR_OTA_VALIDATE_FAILED;
    }
#endif
    // if set boot partition to factory bin ,just format ota info partition
    if (partition->type == ESP_PARTITION_TYPE_APP) {
        if (partition->subtype == ESP_PARTITION_SUBTYPE_APP_FACTORY) {
            find_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);
            if (find_partition != NULL) {
                return esp_partition_erase_range(find_partition, 0, find_partition->size);
            } else {
                return ESP_ERR_NOT_FOUND;
            }
        } else {
            // try to find this partition in flash,if not find it ,return error
            find_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);
            if (find_partition != NULL) {
                return esp_rewrite_ota_data(partition->subtype);
            } else {
                return ESP_ERR_NOT_FOUND;
            }
        }
    } else {
        return ESP_ERR_INVALID_ARG;
    }
}

const esp_partition_t *esp_ota_get_boot_partition(void)
{
    esp_err_t ret;
    const esp_partition_t *find_partition = NULL;
    static spi_flash_mmap_memory_t ota_data_map;
    const void *result = NULL;
    uint16_t ota_app_count = 0;
    find_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);

    if (find_partition == NULL) {
        ESP_LOGE(TAG, "not found ota data");
        return NULL;
    }

    ret = esp_partition_mmap(find_partition, 0, find_partition->size, SPI_FLASH_MMAP_DATA, &result, &ota_data_map);
    if (ret != ESP_OK) {
        spi_flash_munmap(ota_data_map);
        ESP_LOGE(TAG, "mmap ota data filed");
        return NULL;
    } else {
        memcpy(&s_ota_select[0], result, sizeof(ota_select));
        memcpy(&s_ota_select[1], result + 0x1000, sizeof(ota_select));
        spi_flash_munmap(ota_data_map);
    }
    ota_app_count = get_ota_partition_count();

    ESP_LOGD(TAG, "found ota app max = %d", ota_app_count);

    if (s_ota_select[0].ota_seq == 0xFFFFFFFF && s_ota_select[1].ota_seq == 0xFFFFFFFF) {
        ESP_LOGD(TAG, "finding factory app......");

        return esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
    } else if (ota_select_valid(&s_ota_select[0]) && ota_select_valid(&s_ota_select[1])) {
        ESP_LOGD(TAG, "finding ota_%d app......", \
                 ESP_PARTITION_SUBTYPE_APP_OTA_MIN + ((OTA_MAX(s_ota_select[0].ota_seq, s_ota_select[1].ota_seq) - 1) % ota_app_count));

        return esp_partition_find_first(ESP_PARTITION_TYPE_APP, \
                                        ESP_PARTITION_SUBTYPE_APP_OTA_MIN + ((OTA_MAX(s_ota_select[0].ota_seq, s_ota_select[1].ota_seq) - 1) % ota_app_count), NULL);
    } else if (ota_select_valid(&s_ota_select[0])) {
        ESP_LOGD(TAG, "finding ota_%d app......", \
                 ESP_PARTITION_SUBTYPE_APP_OTA_MIN + (s_ota_select[0].ota_seq - 1) % ota_app_count);

        return esp_partition_find_first(ESP_PARTITION_TYPE_APP, \
                                        ESP_PARTITION_SUBTYPE_APP_OTA_MIN + (s_ota_select[0].ota_seq - 1) % ota_app_count, NULL);

    } else if (ota_select_valid(&s_ota_select[1])) {
        ESP_LOGD(TAG, "finding ota_%d app......", \
                 ESP_PARTITION_SUBTYPE_APP_OTA_MIN + (s_ota_select[1].ota_seq - 1) % ota_app_count);

        return esp_partition_find_first(ESP_PARTITION_TYPE_APP, \
                                        ESP_PARTITION_SUBTYPE_APP_OTA_MIN + (s_ota_select[1].ota_seq - 1) % ota_app_count, NULL);

    } else {
        ESP_LOGE(TAG, "ota data invalid, no current app. Assuming factory");
        return esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
    }
}
