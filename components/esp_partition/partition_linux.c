/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include "sdkconfig.h"
#include "esp_partition.h"
#include "esp_flash_partitions.h"
#include "esp_private/partition_linux.h"
#include "esp_log.h"

static const char *TAG = "linux_spiflash";
static void *s_spiflash_mem_file_buf = NULL;
static uint32_t s_spiflash_mem_file_size = 0x400000; //4MB fixed

const char *esp_partition_type_to_str(const uint32_t type)
{
    switch (type) {
    case PART_TYPE_APP: return "app";
    case PART_TYPE_DATA: return "data";
    default: return "unknown";
    }
}

const char *esp_partition_subtype_to_str(const uint32_t type, const uint32_t subtype)
{
    switch (type) {
    case PART_TYPE_APP:
        switch (subtype) {
        case PART_SUBTYPE_FACTORY: return "factory";
        case PART_SUBTYPE_OTA_FLAG: return "ota_flag";
        case PART_SUBTYPE_OTA_MASK: return "ota_mask";
        case PART_SUBTYPE_TEST: return "test";
        default: return "unknown";
        }
    case PART_TYPE_DATA:
        switch (subtype) {
        case PART_SUBTYPE_DATA_OTA: return "data_ota";
        case PART_SUBTYPE_DATA_RF: return "data_rf";
        case PART_SUBTYPE_DATA_WIFI: return "data_wifi";
        case PART_SUBTYPE_DATA_NVS_KEYS: return "nvs_keys";
        case PART_SUBTYPE_DATA_EFUSE_EM: return "efuse_em";
        default: return "unknown";
        }
    default: return "unknown";
    }
}

esp_err_t esp_partition_file_mmap(const uint8_t **part_desc_addr_start)
{
    //create temporary file to hold complete SPIFLASH size
    char temp_spiflash_mem_file_name[PATH_MAX] = {"/tmp/idf-partition-XXXXXX"};
    int spiflash_mem_file_fd = mkstemp(temp_spiflash_mem_file_name);

    if (spiflash_mem_file_fd == -1) {
        ESP_LOGE(TAG, "Failed to create SPI FLASH emulation file %s: %s", temp_spiflash_mem_file_name, strerror(errno));
        return ESP_ERR_NOT_FINISHED;
    }

    if (ftruncate(spiflash_mem_file_fd, s_spiflash_mem_file_size) != 0) {
        ESP_LOGE(TAG, "Failed to set size of SPI FLASH memory emulation file %s: %s", temp_spiflash_mem_file_name, strerror(errno));
        return ESP_ERR_INVALID_SIZE;
    }

    ESP_LOGV(TAG, "SPIFLASH memory emulation file created: %s (size: %d B)", temp_spiflash_mem_file_name, s_spiflash_mem_file_size);

    //create memory-mapping for the partitions holder file
    if ((s_spiflash_mem_file_buf = mmap(NULL, s_spiflash_mem_file_size, PROT_READ | PROT_WRITE, MAP_SHARED, spiflash_mem_file_fd, 0)) == MAP_FAILED) {
        ESP_LOGE(TAG, "Failed to mmap() SPI FLASH memory emulation file: %s", strerror(errno));
        return ESP_ERR_NO_MEM;
    }

    //initialize whole range with bit-1 (NOR FLASH default)
    memset(s_spiflash_mem_file_buf, 0xFF, s_spiflash_mem_file_size);

    //upload partition table to the mmap file at real offset as in SPIFLASH
    const char *partition_table_file_name = "build/partition_table/partition-table.bin";

    FILE *f_partition_table = fopen(partition_table_file_name, "r+");
    if (f_partition_table == NULL) {
        ESP_LOGE(TAG, "Failed to open partition table file %s: %s", partition_table_file_name, strerror(errno));
        return ESP_ERR_NOT_FOUND;
    }

    if (fseek(f_partition_table, 0L, SEEK_END) != 0) {
        ESP_LOGE(TAG, "Failed to seek in partition table file %s: %s", partition_table_file_name, strerror(errno));
        return ESP_ERR_INVALID_SIZE;
    }

    int partition_table_file_size = ftell(f_partition_table);
    ESP_LOGV(TAG, "Using partition table file %s (size: %d B):", partition_table_file_name, partition_table_file_size);

    uint8_t *part_table_in_spiflash = s_spiflash_mem_file_buf + ESP_PARTITION_TABLE_OFFSET;

    //copy partition table from the file to emulated SPIFLASH memory space
    if (fseek(f_partition_table, 0L, SEEK_SET) != 0) {
        ESP_LOGE(TAG, "Failed to seek in partition table file %s: %s", partition_table_file_name, strerror(errno));
        return ESP_ERR_INVALID_SIZE;
    }

    size_t res = fread(part_table_in_spiflash, 1, partition_table_file_size, f_partition_table);
    fclose(f_partition_table);
    if (res != partition_table_file_size) {
        ESP_LOGE(TAG, "Failed to read partition table file %s", partition_table_file_name);
        return ESP_ERR_INVALID_STATE;
    }

#ifdef CONFIG_LOG_DEFAULT_LEVEL_VERBOSE
    uint8_t *part_ptr = part_table_in_spiflash;
    uint8_t *part_end_ptr = part_table_in_spiflash + partition_table_file_size;

    ESP_LOGV(TAG, "");
    ESP_LOGV(TAG, "Partition table sucessfully imported, partitions found:");

    while (part_ptr < part_end_ptr) {
        esp_partition_info_t *p_part_item = (esp_partition_info_t *)part_ptr;
        if (p_part_item->magic != ESP_PARTITION_MAGIC ) {
            break;
        }

        ESP_LOGV(TAG, " --------------");
        ESP_LOGV(TAG, " label: %s", p_part_item->label);
        ESP_LOGV(TAG, " type: %s", esp_partition_type_to_str(p_part_item->type));
        ESP_LOGV(TAG, " subtype: %s", esp_partition_subtype_to_str(p_part_item->type, p_part_item->subtype));
        ESP_LOGV(TAG, " offset: 0x%08X", p_part_item->pos.offset);
        ESP_LOGV(TAG, " size: %d", p_part_item->pos.size);
        ESP_LOGV(TAG, " flags: %d", p_part_item->flags);

        part_ptr += sizeof(esp_partition_info_t);
    }

    ESP_LOGV(TAG, "");
#endif

    //return mmapped file starting address
    *part_desc_addr_start = s_spiflash_mem_file_buf;

    return ESP_OK;
}

esp_err_t esp_partition_file_munmap()
{
    if (s_spiflash_mem_file_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }
    if (s_spiflash_mem_file_size == 0) {
        return ESP_ERR_INVALID_SIZE;
    }

    if (munmap(s_spiflash_mem_file_buf, s_spiflash_mem_file_size) != 0) {
        ESP_LOGE(TAG, "Failed to munmap() SPI FLASH memory emulation file: %s", strerror(errno));
        return ESP_ERR_INVALID_RESPONSE;
    }

    s_spiflash_mem_file_buf = NULL;

    return ESP_OK;
}

esp_err_t esp_partition_write(const esp_partition_t *partition, size_t dst_offset, const void *src, size_t size)
{
    assert(partition != NULL);

    if (partition->encrypted) {
        return  ESP_ERR_NOT_SUPPORTED;
    }
    if (dst_offset > partition->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (dst_offset + size > partition->size) {
        return ESP_ERR_INVALID_SIZE;
    }

    uint8_t *write_buf = malloc(size);
    if (write_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }

    void *dst_addr = s_spiflash_mem_file_buf + partition->address + dst_offset;
    ESP_LOGV(TAG, "esp_partition_write(): partition=%s dst_offset=%zu src=%p size=%zu (real dst address: %p)", partition->label, dst_offset, src, size, dst_addr);

    //read the contents first, AND with the write buffer (to emulate real NOR FLASH behavior)
    memcpy(write_buf, dst_addr, size);
    for (size_t x = 0; x < size; x++) {
        write_buf[x] &= ((uint8_t *)src)[x];
    }
    memcpy(dst_addr, write_buf, size);
    free(write_buf);

    return ESP_OK;
}

esp_err_t esp_partition_read(const esp_partition_t *partition, size_t src_offset, void *dst, size_t size)
{
    assert(partition != NULL);

    if (partition->encrypted) {
        return  ESP_ERR_NOT_SUPPORTED;
    }
    if (src_offset > partition->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (src_offset + size > partition->size) {
        return ESP_ERR_INVALID_SIZE;
    }

    void *src_addr = s_spiflash_mem_file_buf + partition->address + src_offset;

    ESP_LOGV(TAG, "esp_partition_read(): partition=%s src_offset=%zu dst=%p size=%zu (real src address: %p)", partition->label, src_offset, dst, size, src_addr);

    memcpy(dst, src_addr, size);

    return ESP_OK;
}

esp_err_t esp_partition_read_raw(const esp_partition_t *partition, size_t src_offset, void *dst, size_t size)
{
    ESP_LOGV(TAG, "esp_partition_read_raw(): calling esp_partition_read()");
    return esp_partition_read(partition, src_offset, dst, size);
}

esp_err_t esp_partition_write_raw(const esp_partition_t *partition, size_t dst_offset, const void *src, size_t size)
{
    ESP_LOGV(TAG, "esp_partition_write_raw(): calling esp_partition_write()");
    return esp_partition_write(partition, dst_offset, src, size);
}

esp_err_t esp_partition_erase_range(const esp_partition_t *partition, size_t offset, size_t size)
{
    assert(partition != NULL);

    if (offset > partition->size || offset % partition->erase_size != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (offset + size > partition->size || size % partition->erase_size != 0) {
        return ESP_ERR_INVALID_SIZE;
    }

    void *target_addr = s_spiflash_mem_file_buf + partition->address + offset;
    ESP_LOGV(TAG, "esp_partition_erase_range(): partition=%s offset=%zu size=%zu (real target address: %p)", partition->label, offset, size, target_addr);

    //set all bits to 1 (NOR FLASH default)
    memset(target_addr, 0xFF, size);

    return ESP_OK;
}
