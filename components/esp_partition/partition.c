/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/lock.h>

/* interim to enable test_wl_host and test_fatfs_on_host compilation (both use IDF_TARGET_ESP32)
 * should go back to #include "sys/queue.h" once the tests are switched to CMake
 * see IDF-7000
 */
#if __has_include(<bsd/sys/queue.h>)
#include <bsd/sys/queue.h>
#else
#include "sys/queue.h"
#endif

#include "sdkconfig.h"
#include "esp_flash_partitions.h"
#include "esp_attr.h"
#include "esp_partition.h"
#if !CONFIG_IDF_TARGET_LINUX
#include "esp_flash.h"
#include "esp_flash_encrypt.h"
#endif
#include "esp_log.h"
#include "esp_rom_md5.h"
#include "bootloader_util.h"

#if CONFIG_IDF_TARGET_LINUX
#include "esp_private/partition_linux.h"
#endif

#ifndef CONFIG_IDF_TARGET_LINUX
#define MMU_PAGE_SIZE CONFIG_MMU_PAGE_SIZE
#else
// No relation to the page size on Linux; assume the same value as on ESP32
#define MMU_PAGE_SIZE 65536
#endif // CONFIG_MMU_PAGE_SIZE

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif

typedef struct partition_list_item_ {
    esp_partition_t info;
    bool user_registered;
    SLIST_ENTRY(partition_list_item_) next;
} partition_list_item_t;

typedef struct esp_partition_iterator_opaque_ {
    esp_partition_type_t type;                      // requested type
    esp_partition_subtype_t subtype;                // requested subtype
    const char *label;                              // requested label (can be NULL)
    partition_list_item_t *next_item;               // next item to iterate to
    esp_partition_t *info;                          // pointer to info (it is redundant, but makes code more readable)
} esp_partition_iterator_opaque_t;

static SLIST_HEAD(partition_list_head_, partition_list_item_) s_partition_list = SLIST_HEAD_INITIALIZER(s_partition_list);
static _lock_t s_partition_list_lock;

static const char *TAG = "partition";

// Create linked list of partition_list_item_t structures.
// This function is called only once, with s_partition_list_lock taken.
static esp_err_t load_partitions(void)
{
    const uint8_t *p_start;
    const uint8_t *p_end;
#if !CONFIG_IDF_TARGET_LINUX
    spi_flash_mmap_handle_t handle;
#endif

    // Temporary list of loaded partitions, if valid then we copy this to s_partition_list
    typeof(s_partition_list) new_partitions_list = SLIST_HEAD_INITIALIZER(s_partition_list);
    partition_list_item_t *last = NULL;

#if CONFIG_PARTITION_TABLE_MD5
    const uint8_t *md5_part = NULL;
    const uint8_t *stored_md5;
    uint8_t calc_md5[ESP_ROM_MD5_DIGEST_LEN];
    md5_context_t context;

    esp_rom_md5_init(&context);
#endif

    uint32_t partition_align_pg_size = (ESP_PARTITION_TABLE_OFFSET) & ~(MMU_PAGE_SIZE - 1);
    uint32_t partition_pad = ESP_PARTITION_TABLE_OFFSET - partition_align_pg_size;

#if CONFIG_IDF_TARGET_LINUX
    esp_err_t err = esp_partition_file_mmap(&p_start);
    size_t mapped_size = ESP_PARTITION_EMULATED_SECTOR_SIZE;
#else
    esp_err_t err = spi_flash_mmap(partition_align_pg_size,
                                   SPI_FLASH_SEC_SIZE, SPI_FLASH_MMAP_DATA, (const void **)&p_start, &handle);
    size_t mapped_size = SPI_FLASH_SEC_SIZE;
#endif

    if (err != ESP_OK) {
        return err;
    }

    // calculate partition address within mmap-ed region
    p_start += partition_pad;
    p_end = p_start + mapped_size;

    for (const uint8_t *p_entry = p_start; p_entry < p_end; p_entry += sizeof(esp_partition_info_t)) {
        esp_partition_info_t entry;
        // copying to RAM instead of using pointer to flash to avoid any chance of TOCTOU due to cache miss
        // when flash encryption is used
        memcpy(&entry, p_entry, sizeof(entry));

#if CONFIG_PARTITION_TABLE_MD5
        if (entry.magic == ESP_PARTITION_MAGIC_MD5) {
            md5_part = p_entry;
            break;
        }
#endif
        if (entry.magic != ESP_PARTITION_MAGIC) {
            break;
        }

#if CONFIG_PARTITION_TABLE_MD5
        esp_rom_md5_update(&context, &entry, sizeof(entry));
#endif

        // allocate new linked list item and populate it with data from partition table
        partition_list_item_t *item = (partition_list_item_t *) calloc(sizeof(partition_list_item_t), 1);
        if (item == NULL) {
            err = ESP_ERR_NO_MEM;
            break;
        }
#if CONFIG_IDF_TARGET_LINUX
        item->info.flash_chip = NULL;
#else
        item->info.flash_chip = esp_flash_default_chip;
#endif
        item->info.address = entry.pos.offset;
        item->info.size = entry.pos.size;
#if CONFIG_IDF_TARGET_LINUX
        item->info.erase_size = ESP_PARTITION_EMULATED_SECTOR_SIZE;
#else
        item->info.erase_size = SPI_FLASH_SEC_SIZE;
#endif
        item->info.type = entry.type;
        item->info.subtype = entry.subtype;
        item->info.encrypted = entry.flags & PART_FLAG_ENCRYPTED;
        item->info.readonly = entry.flags & PART_FLAG_READONLY;
        item->user_registered = false;

#if CONFIG_IDF_TARGET_LINUX
        item->info.encrypted = false;
#else
        if (!esp_flash_encryption_enabled()) {
            /* If flash encryption is not turned on, no partitions should be treated as encrypted */
            item->info.encrypted = false;
        } else if (entry.type == ESP_PARTITION_TYPE_APP
                   || (entry.type == ESP_PARTITION_TYPE_DATA && entry.subtype == ESP_PARTITION_SUBTYPE_DATA_OTA)
                   || (entry.type == ESP_PARTITION_TYPE_DATA && entry.subtype == ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS)) {
            /* If encryption is turned on, all app partitions and OTA data
               are always encrypted */
            item->info.encrypted = true;
        }
#endif

#if CONFIG_NVS_COMPATIBLE_PRE_V4_3_ENCRYPTION_FLAG
        if (entry.type == ESP_PARTITION_TYPE_DATA &&
                entry.subtype == ESP_PARTITION_SUBTYPE_DATA_NVS &&
                (entry.flags & PART_FLAG_ENCRYPTED)) {
            ESP_LOGI(TAG, "Ignoring encrypted flag for \"%s\" partition", entry.label);
            item->info.encrypted = false;
        }
#endif
        // item->info.label is initialized by calloc, so resulting string will be null terminated
        strncpy(item->info.label, (const char *) entry.label, sizeof(item->info.label) - 1);

        // add it to the list
        if (last == NULL) {
            SLIST_INSERT_HEAD(&new_partitions_list, item, next);
        } else {
            SLIST_INSERT_AFTER(last, item, next);
        }
        last = item;
    }

#if CONFIG_PARTITION_TABLE_MD5
    if (md5_part == NULL) {
        ESP_LOGE(TAG, "No MD5 found in partition table");
        err = ESP_ERR_NOT_FOUND;
    } else {
        stored_md5 = md5_part + ESP_PARTITION_MD5_OFFSET;
        esp_rom_md5_final(calc_md5, &context);

#if !CONFIG_IDF_TARGET_LINUX
        ESP_LOG_BUFFER_HEXDUMP("calculated md5", calc_md5, ESP_ROM_MD5_DIGEST_LEN, ESP_LOG_VERBOSE);
        ESP_LOG_BUFFER_HEXDUMP("stored md5", stored_md5, ESP_ROM_MD5_DIGEST_LEN, ESP_LOG_VERBOSE);
#endif

        if (memcmp(calc_md5, stored_md5, ESP_ROM_MD5_DIGEST_LEN) != 0) {
            ESP_LOGE(TAG, "Partition table MD5 mismatch");
            err = ESP_ERR_INVALID_STATE;
        } else {
            ESP_LOGV(TAG, "Partition table MD5 verified");
        }
    }
#endif

    if (err == ESP_OK) {
        /* Don't copy the list to the static variable unless it's verified */
        s_partition_list = new_partitions_list;
    } else {
        /* Otherwise, free all the memory we just allocated */
        partition_list_item_t *it = new_partitions_list.slh_first;
        while (it) {
            partition_list_item_t *next = it->next.sle_next;
            free(it);
            it = next;
        }
    }

#if !CONFIG_IDF_TARGET_LINUX
    spi_flash_munmap(handle);
#endif

    return err;
}

void esp_partition_unload_all(void)
{
    _lock_acquire(&s_partition_list_lock);
    partition_list_item_t *it;
    partition_list_item_t *tmp;
    SLIST_FOREACH_SAFE(it, &s_partition_list, next, tmp) {
        SLIST_REMOVE(&s_partition_list, it, partition_list_item_, next);
        free(it);
    }
    _lock_release(&s_partition_list_lock);

    assert(SLIST_EMPTY(&s_partition_list));
}

static esp_err_t ensure_partitions_loaded(void)
{
    esp_err_t err = ESP_OK;
    if (SLIST_EMPTY(&s_partition_list)) {
        // only lock if list is empty (and check again after acquiring lock)
        _lock_acquire(&s_partition_list_lock);
        if (SLIST_EMPTY(&s_partition_list)) {
            ESP_LOGV(TAG, "Loading the partition table");
            err = load_partitions();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "load_partitions returned 0x%x", err);
            }
        }
        _lock_release(&s_partition_list_lock);
    }
    return err;
}

static esp_partition_iterator_opaque_t *iterator_create(esp_partition_type_t type,
        esp_partition_subtype_t subtype, const char *label)
{
    esp_partition_iterator_opaque_t *it =
        (esp_partition_iterator_opaque_t *) malloc(sizeof(esp_partition_iterator_opaque_t));
    if (it == NULL) {
        return NULL;
    }
    it->type = type;
    it->subtype = subtype;
    it->label = label;
    it->next_item = SLIST_FIRST(&s_partition_list);
    it->info = NULL;
    return it;
}

esp_partition_iterator_t esp_partition_find(esp_partition_type_t type,
        esp_partition_subtype_t subtype, const char *label)
{
    if (ensure_partitions_loaded() != ESP_OK) {
        return NULL;
    }
    // Searching for a specific subtype without specifying the type doesn't make
    // sense, and is likely a usage error.
    if (type == ESP_PARTITION_TYPE_ANY && subtype != ESP_PARTITION_SUBTYPE_ANY) {
        return NULL;
    }
    // create an iterator pointing to the start of the list
    // (next item will be the first one)
    esp_partition_iterator_t it = iterator_create(type, subtype, label);
    if (it == NULL) {
        return NULL;
    }
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
        esp_partition_t *p = &it->next_item->info;
        if (it->type != ESP_PARTITION_TYPE_ANY && it->type != p->type) {
            continue;
        }
        if (it->subtype != ESP_PARTITION_SUBTYPE_ANY && it->subtype != p->subtype) {
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

const esp_partition_t *esp_partition_find_first(esp_partition_type_t type,
        esp_partition_subtype_t subtype, const char *label)
{
    esp_partition_iterator_t it = esp_partition_find(type, subtype, label);
    if (it == NULL) {
        return NULL;
    }
    const esp_partition_t *res = esp_partition_get(it);
    esp_partition_iterator_release(it);
    return res;
}

void esp_partition_iterator_release(esp_partition_iterator_t iterator)
{
    // iterator == NULL is okay
    free(iterator);
}

const esp_partition_t *esp_partition_get(esp_partition_iterator_t iterator)
{
    assert(iterator != NULL);
    return iterator->info;
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

esp_err_t esp_partition_register_external(esp_flash_t *flash_chip, size_t offset, size_t size,
        const char *label, esp_partition_type_t type, esp_partition_subtype_t subtype,
        const esp_partition_t **out_partition)
{
    if (out_partition != NULL) {
        *out_partition = NULL;
    }

#if CONFIG_IDF_TARGET_LINUX
    return ESP_ERR_NOT_SUPPORTED;

#else
    if (offset + size > flash_chip->size) {
        return ESP_ERR_INVALID_SIZE;
    }
#endif // CONFIG_IDF_TARGET_LINUX

    esp_err_t err = ensure_partitions_loaded();
    if (err != ESP_OK) {
        return err;
    }

    partition_list_item_t *item = (partition_list_item_t *) calloc(sizeof(partition_list_item_t), 1);
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
    partition_list_item_t *it = NULL;
    partition_list_item_t *last = NULL;
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

esp_err_t esp_partition_deregister_external(const esp_partition_t *partition)
{
    esp_err_t result = ESP_ERR_NOT_FOUND;
    _lock_acquire(&s_partition_list_lock);
    partition_list_item_t *it;
    partition_list_item_t *tmp;
    SLIST_FOREACH_SAFE(it, &s_partition_list, next, tmp) {
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
