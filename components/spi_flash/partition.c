// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/lock.h>
#include "esp_flash_partitions.h"
#include "esp_attr.h"
#include "esp_flash.h"
#include "esp_spi_flash.h"
#include "esp_partition.h"
#include "esp_flash_encrypt.h"
#include "esp_log.h"
#include "bootloader_common.h"
#include "bootloader_util.h"
#include "esp_ota_ops.h"

#define HASH_LEN 32 /* SHA-256 digest length */

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif
#include "sys/queue.h"



typedef struct partition_list_item_ {
    esp_partition_t info;
    bool user_registered;
    SLIST_ENTRY(partition_list_item_) next;
} partition_list_item_t;

typedef struct esp_partition_iterator_opaque_ {
    esp_partition_type_t type;                  // requested type
    esp_partition_subtype_t subtype;               // requested subtype
    const char* label;                          // requested label (can be NULL)
    partition_list_item_t* next_item;     // next item to iterate to
    esp_partition_t* info;                // pointer to info (it is redundant, but makes code more readable)
} esp_partition_iterator_opaque_t;


static esp_partition_iterator_opaque_t* iterator_create(esp_partition_type_t type, esp_partition_subtype_t subtype, const char* label);
static esp_err_t load_partitions();
static esp_err_t ensure_partitions_loaded(void);


static const char* TAG = "partition";
static SLIST_HEAD(partition_list_head_, partition_list_item_) s_partition_list =
        SLIST_HEAD_INITIALIZER(s_partition_list);
static _lock_t s_partition_list_lock;


static esp_err_t ensure_partitions_loaded(void)
{
    esp_err_t err = ESP_OK;
    if (SLIST_EMPTY(&s_partition_list)) {
        // only lock if list is empty (and check again after acquiring lock)
        _lock_acquire(&s_partition_list_lock);
        if (SLIST_EMPTY(&s_partition_list)) {
            ESP_LOGD(TAG, "Loading the partition table");
            err = load_partitions();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "load_partitions returned 0x%x", err);
            }
        }
        _lock_release(&s_partition_list_lock);
    }
    return err;
}

esp_partition_iterator_t esp_partition_find(esp_partition_type_t type,
        esp_partition_subtype_t subtype, const char* label)
{
    if (ensure_partitions_loaded() != ESP_OK) {
        return NULL;
    }
    // create an iterator pointing to the start of the list
    // (next item will be the first one)
    esp_partition_iterator_t it = iterator_create(type, subtype, label);
    // advance iterator to the next item which matches constraints
    it = esp_partition_next(it);
    // if nothing found, it == NULL and iterator has been released
    return it;
}

esp_partition_iterator_t esp_partition_next(esp_partition_iterator_t it)
{
    assert(it);
    // iterator reached the end of linked list?
    if (it->next_item == NULL) {
        esp_partition_iterator_release(it);
        return NULL;
    }
    _lock_acquire(&s_partition_list_lock);
    for (; it->next_item != NULL; it->next_item = SLIST_NEXT(it->next_item, next)) {
        esp_partition_t* p = &it->next_item->info;
        if (it->type != p->type) {
            continue;
        }
        if (it->subtype != 0xff && it->subtype != p->subtype) {
            continue;
        }
        if (it->label != NULL && strcmp(it->label, p->label) != 0) {
            continue;
        }
        // all constraints match, bail out
        break;
    }
    _lock_release(&s_partition_list_lock);
    if (it->next_item == NULL) {
        esp_partition_iterator_release(it);
        return NULL;
    }
    it->info = &it->next_item->info;
    it->next_item = SLIST_NEXT(it->next_item, next);
    return it;
}

const esp_partition_t* esp_partition_find_first(esp_partition_type_t type,
        esp_partition_subtype_t subtype, const char* label)
{
    esp_partition_iterator_t it = esp_partition_find(type, subtype, label);
    if (it == NULL) {
        return NULL;
    }
    const esp_partition_t* res = esp_partition_get(it);
    esp_partition_iterator_release(it);
    return res;
}

static esp_partition_iterator_opaque_t* iterator_create(esp_partition_type_t type,
        esp_partition_subtype_t subtype, const char* label)
{
    esp_partition_iterator_opaque_t* it =
            (esp_partition_iterator_opaque_t*) malloc(sizeof(esp_partition_iterator_opaque_t));
    it->type = type;
    it->subtype = subtype;
    it->label = label;
    it->next_item = SLIST_FIRST(&s_partition_list);
    it->info = NULL;
    return it;
}

// Create linked list of partition_list_item_t structures.
// This function is called only once, with s_partition_list_lock taken.
static esp_err_t load_partitions()
{
    const uint32_t* ptr;
    spi_flash_mmap_handle_t handle;
    // map 64kB block where partition table is located
    esp_err_t err = spi_flash_mmap(ESP_PARTITION_TABLE_OFFSET & 0xffff0000,
            SPI_FLASH_SEC_SIZE, SPI_FLASH_MMAP_DATA, (const void**) &ptr, &handle);
    if (err != ESP_OK) {
        return err;
    }
    // calculate partition address within mmap-ed region
    const esp_partition_info_t* it = (const esp_partition_info_t*)
            (ptr + (ESP_PARTITION_TABLE_OFFSET & 0xffff) / sizeof(*ptr));
    const esp_partition_info_t* end = it + SPI_FLASH_SEC_SIZE / sizeof(*it);
    // tail of the linked list of partitions
    partition_list_item_t* last = NULL;
    for (; it != end; ++it) {
        if (it->magic != ESP_PARTITION_MAGIC) {
            break;
        }
        // allocate new linked list item and populate it with data from partition table
        partition_list_item_t* item = (partition_list_item_t*) calloc(sizeof(partition_list_item_t), 1);
        if (item == NULL) {
            err = ESP_ERR_NO_MEM;
            break;
        }
        item->info.flash_chip = esp_flash_default_chip;
        item->info.address = it->pos.offset;
        item->info.size = it->pos.size;
        item->info.type = it->type;
        item->info.subtype = it->subtype;
        item->info.encrypted = it->flags & PART_FLAG_ENCRYPTED;
        item->user_registered = false;

        if (!esp_flash_encryption_enabled()) {
            /* If flash encryption is not turned on, no partitions should be treated as encrypted */
            item->info.encrypted = false;
        } else if (it->type == PART_TYPE_APP
                || (it->type == PART_TYPE_DATA && it->subtype == PART_SUBTYPE_DATA_OTA)
                || (it->type == PART_TYPE_DATA && it->subtype == PART_SUBTYPE_DATA_NVS_KEYS)) {
            /* If encryption is turned on, all app partitions and OTA data
               are always encrypted */
            item->info.encrypted = true;
        }

        // it->label may not be zero-terminated
        strncpy(item->info.label, (const char*) it->label, sizeof(item->info.label) - 1);
        item->info.label[sizeof(it->label)] = 0;
        // add it to the list
        if (last == NULL) {
            SLIST_INSERT_HEAD(&s_partition_list, item, next);
        } else {
            SLIST_INSERT_AFTER(last, item, next);
        }
        last = item;
    }
    spi_flash_munmap(handle);
    return err;
}

void esp_partition_iterator_release(esp_partition_iterator_t iterator)
{
    // iterator == NULL is okay
    free(iterator);
}

const esp_partition_t* esp_partition_get(esp_partition_iterator_t iterator)
{
    assert(iterator != NULL);
    return iterator->info;
}

esp_err_t esp_partition_register_external(esp_flash_t* flash_chip, size_t offset, size_t size,
        const char* label, esp_partition_type_t type, esp_partition_subtype_t subtype,
        const esp_partition_t** out_partition)
{
    if (out_partition != NULL) {
        *out_partition = NULL;
    }
#ifdef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
    return ESP_ERR_NOT_SUPPORTED;
#endif

    if (offset + size > flash_chip->size) {
        return ESP_ERR_INVALID_SIZE;
    }

    esp_err_t err = ensure_partitions_loaded();
    if (err != ESP_OK) {
        return err;
    }

    partition_list_item_t* item = (partition_list_item_t*) calloc(sizeof(partition_list_item_t), 1);
    if (item == NULL) {
        return ESP_ERR_NO_MEM;
    }
    item->info.flash_chip = flash_chip;
    item->info.address = offset;
    item->info.size = size;
    item->info.type = type;
    item->info.subtype = subtype;
    item->info.encrypted = false;
    item->user_registered = true;
    strlcpy(item->info.label, label, sizeof(item->info.label));

    _lock_acquire(&s_partition_list_lock);
    partition_list_item_t *it, *last = NULL;
    SLIST_FOREACH(it, &s_partition_list, next) {
        /* Check if the new partition overlaps an existing one */
        if (it->info.flash_chip == flash_chip &&
                bootloader_util_regions_overlap(offset, offset + size,
                                                it->info.address, it->info.address + it->info.size)) {
            _lock_release(&s_partition_list_lock);
            free(item);
            return ESP_ERR_INVALID_ARG;
        }
        last = it;
    }
    if (last == NULL) {
        SLIST_INSERT_HEAD(&s_partition_list, item, next);
    } else {
        SLIST_INSERT_AFTER(last, item, next);
    }
    _lock_release(&s_partition_list_lock);
    if (out_partition != NULL) {
        *out_partition = &item->info;
    }
    return ESP_OK;
}

esp_err_t esp_partition_deregister_external(const esp_partition_t* partition)
{
    esp_err_t result = ESP_ERR_NOT_FOUND;
    _lock_acquire(&s_partition_list_lock);
    partition_list_item_t *it;
    SLIST_FOREACH(it, &s_partition_list, next) {
        if (&it->info == partition) {
            if (!it->user_registered) {
                result = ESP_ERR_INVALID_ARG;
                break;
            }
            SLIST_REMOVE(&s_partition_list, it, partition_list_item_, next);
            free(it);
            result = ESP_OK;
            break;
        }
    }
    _lock_release(&s_partition_list_lock);
    return result;
}

const esp_partition_t *esp_partition_verify(const esp_partition_t *partition)
{
    assert(partition != NULL);
    const char *label = (strlen(partition->label) > 0) ? partition->label : NULL;
    esp_partition_iterator_t it = esp_partition_find(partition->type,
                                                     partition->subtype,
                                                     label);
    while (it != NULL) {
        const esp_partition_t *p = esp_partition_get(it);
        /* Can't memcmp() whole structure here as padding contents may be different */
        if (p->flash_chip == partition->flash_chip
            && p->address == partition->address
            && partition->size == p->size
            && partition->encrypted == p->encrypted) {
            esp_partition_iterator_release(it);
            return p;
        }
        it = esp_partition_next(it);
    }
    esp_partition_iterator_release(it);
    return NULL;
}

esp_err_t esp_partition_read(const esp_partition_t* partition,
        size_t src_offset, void* dst, size_t size)
{
    assert(partition != NULL);
    if (src_offset > partition->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (src_offset + size > partition->size) {
        return ESP_ERR_INVALID_SIZE;
    }

    if (!partition->encrypted) {
#ifndef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
        return esp_flash_read(partition->flash_chip, dst, partition->address + src_offset, size);
#else
        return spi_flash_read(partition->address + src_offset, dst, size);
#endif // CONFIG_SPI_FLASH_USE_LEGACY_IMPL
    } else {
#if CONFIG_SECURE_FLASH_ENC_ENABLED
        if (partition->flash_chip != esp_flash_default_chip) {
            return ESP_ERR_NOT_SUPPORTED;
        }

        /* Encrypted partitions need to be read via a cache mapping */
        const void *buf;
        spi_flash_mmap_handle_t handle;
        esp_err_t err;

        err = esp_partition_mmap(partition, src_offset, size,
                                 SPI_FLASH_MMAP_DATA, &buf, &handle);
        if (err != ESP_OK) {
            return err;
        }
        memcpy(dst, buf, size);
        spi_flash_munmap(handle);
        return ESP_OK;
#else
        return ESP_ERR_NOT_SUPPORTED;
#endif // CONFIG_SECURE_FLASH_ENC_ENABLED
    }
}

esp_err_t esp_partition_write(const esp_partition_t* partition,
                             size_t dst_offset, const void* src, size_t size)
{
    assert(partition != NULL);
    if (dst_offset > partition->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (dst_offset + size > partition->size) {
        return ESP_ERR_INVALID_SIZE;
    }
    dst_offset = partition->address + dst_offset;
    if (!partition->encrypted) {
#ifndef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
        return esp_flash_write(partition->flash_chip, src, dst_offset, size);
#else
        return spi_flash_write(dst_offset, src, size);
#endif // CONFIG_SPI_FLASH_USE_LEGACY_IMPL
    } else {
#if CONFIG_SECURE_FLASH_ENC_ENABLED
        if (partition->flash_chip != esp_flash_default_chip) {
            return ESP_ERR_NOT_SUPPORTED;
        }
        return spi_flash_write_encrypted(dst_offset, src, size);
#else
        return ESP_ERR_NOT_SUPPORTED;
#endif // CONFIG_SECURE_FLASH_ENC_ENABLED
    }
}

esp_err_t esp_partition_erase_range(const esp_partition_t* partition,
                                    size_t offset, size_t size)
{
    assert(partition != NULL);
    if (offset > partition->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (offset + size > partition->size) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (size % SPI_FLASH_SEC_SIZE != 0) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (offset % SPI_FLASH_SEC_SIZE != 0) {
        return ESP_ERR_INVALID_ARG;
    }
#ifndef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
    return esp_flash_erase_region(partition->flash_chip, partition->address + offset, size);
#else
    return spi_flash_erase_range(partition->address + offset, size);
#endif // CONFIG_SPI_FLASH_USE_LEGACY_IMPL
}

/*
 * Note: current implementation ignores the possibility of multiple regions in the same partition being
 * mapped. Reference counting and address space re-use is delegated to spi_flash_mmap.
 *
 * If this becomes a performance issue (i.e. if we need to map multiple regions within the partition),
 * we can add esp_partition_mmapv which will accept an array of offsets and sizes, and return array of
 * mmaped pointers, and a single handle for all these regions.
 */
esp_err_t esp_partition_mmap(const esp_partition_t* partition, size_t offset, size_t size,
                             spi_flash_mmap_memory_t memory,
                             const void** out_ptr, spi_flash_mmap_handle_t* out_handle)
{
    assert(partition != NULL);
    if (offset > partition->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (offset + size > partition->size) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (partition->flash_chip != esp_flash_default_chip) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    size_t phys_addr = partition->address + offset;
    // offset within 64kB block
    size_t region_offset = phys_addr & 0xffff;
    size_t mmap_addr = phys_addr & 0xffff0000;
    esp_err_t rc = spi_flash_mmap(mmap_addr, size+region_offset, memory, out_ptr, out_handle);
    // adjust returned pointer to point to the correct offset
    if (rc == ESP_OK) {
        *out_ptr = (void*) (((ptrdiff_t) *out_ptr) + region_offset);
    }
    return rc;
}

esp_err_t esp_partition_get_sha256(const esp_partition_t *partition, uint8_t *sha_256)
{
    return bootloader_common_get_sha256_of_partition(partition->address, partition->size, partition->type, sha_256);
}

bool esp_partition_check_identity(const esp_partition_t *partition_1, const esp_partition_t *partition_2)
{
    uint8_t sha_256[2][HASH_LEN] = { 0 };

    if (esp_partition_get_sha256(partition_1, sha_256[0]) == ESP_OK &&
        esp_partition_get_sha256(partition_2, sha_256[1]) == ESP_OK) {

        if (memcmp(sha_256[0], sha_256[1], HASH_LEN) == 0) {
            // The partitions are identity
            return true;
        }
    }
    return false;
}

bool esp_partition_main_flash_region_safe(size_t addr, size_t size)
{
    bool result = true;
    if (addr <= ESP_PARTITION_TABLE_OFFSET + ESP_PARTITION_TABLE_MAX_LEN) {
        return false;
    }
    const esp_partition_t *p = esp_ota_get_running_partition();
    if (addr >= p->address && addr < p->address + p->size) {
        return false;
    }
    if (addr < p->address && addr + size > p->address) {
        return false;
    }
    return result;
}
