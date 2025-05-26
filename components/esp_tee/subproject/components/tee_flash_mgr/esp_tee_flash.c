/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <sys/param.h>
#include <sys/queue.h>

#include "esp_err.h"
#include "esp_log.h"

#include "bootloader_utility_tee.h"
#include "esp_tee_ota_utils.h"

#include "esp_tee.h"
#include "esp_tee_flash.h"
#include "sdkconfig.h"

static const char *TAG = "esp_tee_flash";

// Structure containing the valid flash address range for flash operations through TEE
typedef struct {
    uint32_t flash_reg_start_paddr;
    uint32_t flash_reg_end_paddr;
    uint32_t active_part_start_paddr;
    uint32_t active_part_end_paddr;
} tee_flash_protect_info_t;

static tee_flash_protect_info_t tee_prot_ctx;

// Running REE (user) app partition
static esp_partition_info_t ree_running;

// List storing all the partition table entries
typedef struct _partition_list {
    SLIST_ENTRY(_partition_list) next;
    esp_partition_info_t partition;
} partition_list_t;

SLIST_HEAD(partition_list, _partition_list) partition_table_list;

static partition_list_t *create_partition_entry(const esp_partition_info_t* partition_info)
{
    partition_list_t *partition_entry = calloc(1, sizeof(partition_list_t));
    assert(partition_entry != NULL);
    memcpy(&partition_entry->partition, partition_info, sizeof(esp_partition_info_t));
    return partition_entry;
}

static esp_err_t get_partition_table(void)
{
    if (SLIST_EMPTY(&partition_table_list)) {
        const esp_partition_info_t *partition_table = esp_tee_flash_mmap(ESP_PARTITION_TABLE_OFFSET, ESP_PARTITION_TABLE_MAX_LEN);
        if (partition_table == NULL) {
            ESP_LOGE(TAG, "esp_tee_flash_mmap failed!");
            return ESP_FAIL;
        }

        int num_partitions = 0;
        esp_err_t err = esp_partition_table_verify(partition_table, false, &num_partitions);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Partition table verification failed!");
            return err;
        }

        SLIST_INIT(&partition_table_list);

        for (size_t num_parts = 0; num_parts < ESP_PARTITION_TABLE_MAX_ENTRIES; num_parts++) {
            const esp_partition_info_t *part = &partition_table[num_parts];
            if (part->type == PART_TYPE_END && part->subtype == PART_SUBTYPE_END) {
                break;
            }

            partition_list_t *partition = create_partition_entry(part);
            SLIST_INSERT_HEAD(&partition_table_list, partition, next);
        }
        esp_tee_flash_munmap(partition_table);
    }

    return ESP_OK;
}

esp_err_t esp_tee_flash_find_partition(uint8_t type, uint8_t subtype, const char *label, esp_partition_info_t *dest_ptr)
{
    if (dest_ptr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* NOTE: Fetch the partition table */
    esp_err_t err = get_partition_table();
    if (err != ESP_OK) {
        return err;
    }

    partition_list_t *partition_entry;

    SLIST_FOREACH(partition_entry, &partition_table_list, next) {
        if (partition_entry->partition.type == type && partition_entry->partition.subtype == subtype) {
            if (label == NULL || !memcmp(label, partition_entry->partition.label, strnlen(label, sizeof(partition_entry->partition.label)))) {
                memcpy(dest_ptr, &partition_entry->partition, sizeof(esp_partition_info_t));
                return ESP_OK;
            }
        }
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t esp_tee_flash_get_part_info_for_addr(uint32_t paddr, esp_partition_info_t *part_info)
{
    if (part_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = get_partition_table();
    if (err != ESP_OK) {
        return err;
    }

    partition_list_t *partition_entry;
    SLIST_FOREACH(partition_entry, &partition_table_list, next) {
        uint32_t start_addr = partition_entry->partition.pos.offset;
        uint32_t end_addr = start_addr + partition_entry->partition.pos.size;
        if (paddr >= start_addr && paddr < end_addr) {
            memcpy(part_info, &partition_entry->partition, sizeof(esp_partition_info_t));
            return ESP_OK;
        }
    }

    return ESP_ERR_NOT_FOUND;
}

esp_err_t esp_tee_flash_set_running_ree_partition(uint32_t paddr)
{
    return esp_tee_flash_get_part_info_for_addr(paddr, &ree_running);
}

esp_partition_info_t *esp_tee_flash_get_running_ree_partition(void)
{
    return &ree_running;
}

esp_err_t esp_tee_flash_setup_prot_ctx(uint8_t tee_boot_part)
{
    static bool is_first_call = true;
    if (!is_first_call) {
        return ESP_OK;
    }

    /* NOTE: Fetch the partition table and record TEE partitions range. */
    esp_err_t err = get_partition_table();
    if (err != ESP_OK) {
        return err;
    }

    tee_prot_ctx.flash_reg_start_paddr = UINT32_MAX;

    partition_list_t *partition_entry;
    SLIST_FOREACH(partition_entry, &partition_table_list, next) {
        const uint32_t start_addr = partition_entry->partition.pos.offset;
        const uint32_t end_addr = start_addr + partition_entry->partition.pos.size;
        const uint8_t type = partition_entry->partition.type;
        const uint8_t subtype = partition_entry->partition.subtype;

        bool needs_protection = false;
        if (type == PART_TYPE_APP) {
            needs_protection = (subtype == PART_SUBTYPE_TEE_0 || subtype == PART_SUBTYPE_TEE_1);
        } else if (type == PART_TYPE_DATA) {
            if (subtype == PART_SUBTYPE_DATA_TEE_OTA) {
                needs_protection = true;
            } else if (subtype == PART_SUBTYPE_DATA_WIFI) {
                size_t label_len = strlen(ESP_TEE_SEC_STG_PART_LABEL);
                if (memcmp(partition_entry->partition.label, ESP_TEE_SEC_STG_PART_LABEL, label_len) == 0) {
                    needs_protection = true;
                }
            }
        }

        if (needs_protection) {
            tee_prot_ctx.flash_reg_start_paddr = MIN(tee_prot_ctx.flash_reg_start_paddr, start_addr);
            tee_prot_ctx.flash_reg_end_paddr = MAX(tee_prot_ctx.flash_reg_end_paddr, end_addr);
        }

        if (type == PART_TYPE_APP && subtype == tee_boot_part) {
            tee_prot_ctx.active_part_start_paddr = start_addr;
            tee_prot_ctx.active_part_end_paddr = end_addr;
        }
    }

    is_first_call = false;
    return ESP_OK;
}

bool esp_tee_flash_check_vaddr_in_tee_region(const size_t vaddr)
{
    bool prot_reg1_chk = (vaddr >= SOC_S_DROM_LOW) && (vaddr < SOC_S_IROM_HIGH);
    bool prot_reg2_chk = (vaddr >= SOC_S_MMU_MMAP_RESV_START_VADDR) && (vaddr < SOC_MMU_END_VADDR);

    return (prot_reg1_chk || prot_reg2_chk);
}

bool esp_tee_flash_check_paddr_in_tee_region(const size_t paddr)
{
    return ((paddr >= tee_prot_ctx.flash_reg_start_paddr) && (paddr < tee_prot_ctx.flash_reg_end_paddr));
}

bool esp_tee_flash_check_paddr_in_active_tee_part(const size_t paddr)
{
    return ((paddr >= tee_prot_ctx.active_part_start_paddr) && (paddr < tee_prot_ctx.active_part_end_paddr));
}
