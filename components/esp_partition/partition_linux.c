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
static const esp_partition_mmap_handle_t s_default_partition_mmap_handle = 0;

#ifdef CONFIG_ESP_PARTITION_ENABLE_STATS
// variables holding stats and controlling wear emulation
size_t s_esp_partition_stat_read_ops = 0;
size_t s_esp_partition_stat_write_ops = 0;
size_t s_esp_partition_stat_read_bytes = 0;
size_t s_esp_partition_stat_write_bytes = 0;
size_t s_esp_partition_stat_erase_ops = 0;
size_t s_esp_partition_stat_total_time = 0;
size_t s_esp_partition_emulated_flash_life = SIZE_MAX;

// tracking erase count individually for each emulated sector
size_t s_esp_partition_stat_sector_erase_count[ESP_PARTITION_EMULATED_FLASH_SIZE / ESP_PARTITION_EMULATED_SECTOR_SIZE] = {0};

// forward declaration of hooks
static void esp_partition_hook_read(const void *srcAddr, const size_t size);
static bool esp_partition_hook_write(const void *dstAddr, const size_t size);
static bool esp_partition_hook_erase(const void *dstAddr, const size_t size);

// redirect hooks to functions
#define ESP_PARTITION_HOOK_READ(srcAddr, size) esp_partition_hook_read(srcAddr, size)
#define ESP_PARTITION_HOOK_WRITE(dstAddr, size) esp_partition_hook_write(dstAddr, size)
#define ESP_PARTITION_HOOK_ERASE(dstAddr, size) esp_partition_hook_erase(dstAddr, size)
#else
// redirect hooks to "do nothing code"
#define ESP_PARTITION_HOOK_READ(srcAddr, size)
#define ESP_PARTITION_HOOK_WRITE(dstAddr, size) true
#define ESP_PARTITION_HOOK_ERASE(dstAddr, size) true
#endif

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

    if (ftruncate(spiflash_mem_file_fd, ESP_PARTITION_EMULATED_FLASH_SIZE) != 0) {
        ESP_LOGE(TAG, "Failed to set size of SPI FLASH memory emulation file %s: %s", temp_spiflash_mem_file_name, strerror(errno));
        return ESP_ERR_INVALID_SIZE;
    }

    ESP_LOGV(TAG, "SPIFLASH memory emulation file created: %s (size: %d B)", temp_spiflash_mem_file_name, ESP_PARTITION_EMULATED_FLASH_SIZE);

    //create memory-mapping for the partitions holder file
    if ((s_spiflash_mem_file_buf = mmap(NULL, ESP_PARTITION_EMULATED_FLASH_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, spiflash_mem_file_fd, 0)) == MAP_FAILED) {
        ESP_LOGE(TAG, "Failed to mmap() SPI FLASH memory emulation file: %s", strerror(errno));
        return ESP_ERR_NO_MEM;
    }

    //initialize whole range with bit-1 (NOR FLASH default)
    memset(s_spiflash_mem_file_buf, 0xFF, ESP_PARTITION_EMULATED_FLASH_SIZE);

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

esp_err_t esp_partition_file_munmap(void)
{
    if (s_spiflash_mem_file_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }
    if (ESP_PARTITION_EMULATED_FLASH_SIZE == 0) {
        return ESP_ERR_INVALID_SIZE;
    }

    if (munmap(s_spiflash_mem_file_buf, ESP_PARTITION_EMULATED_FLASH_SIZE) != 0) {
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

    // hook gathers statistics and can emulate limited number of write cycles
    if (!ESP_PARTITION_HOOK_WRITE(dst_addr, size)) {
        return ESP_FAIL;
    }

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

    ESP_PARTITION_HOOK_READ(src_addr, size); // statistics

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

    // hook gathers statistics and can emulate limited number of write/erase cycles
    if (!ESP_PARTITION_HOOK_ERASE(target_addr, size)) {
        return ESP_FAIL;
    }

    //set all bits to 1 (NOR FLASH default)
    memset(target_addr, 0xFF, size);

    return ESP_OK;
}

/*
 * Exposes direct pointer to the memory mapped file created by esp_partition_file_mmap
 * No address alignment is performed
 * Default handle is always returned
 * Returns:
 * ESP_ERR_INVALID_ARG  - offset exceeds size of partition
 * ESP_ERR_INVALID_SIZE - address range defined by offset + size is beyond the size of partition
 * ESP_ERR_NOT_SUPPORTED - flash_chip of partition is not NULL
 * ESP_OK - calculated out parameters hold pointer to the requested memory area and default handle respectively
 */
esp_err_t esp_partition_mmap(const esp_partition_t *partition, size_t offset, size_t size,
                             esp_partition_mmap_memory_t memory,
                             const void **out_ptr, esp_partition_mmap_handle_t *out_handle)
{
    ESP_LOGV(TAG, "esp_partition_mmap(): partition=%s offset=%zu size=%zu", partition->label, offset, size);

    assert(partition != NULL);
    if (offset > partition->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (offset + size > partition->size) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (partition->flash_chip != NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    // required starting address in flash aka offset from the flash beginning
    size_t req_flash_addr = (size_t)(partition->address) + offset;

    esp_err_t rc = ESP_OK;

    // check if memory mapped file is already present, if not, map it now
    if (s_spiflash_mem_file_buf == NULL) {
        ESP_LOGE(TAG, "esp_partition_mmap(): in esp_partition_file_mmap");
        uint8_t *part_desc_addr_start = NULL;
        rc = esp_partition_file_mmap((const uint8_t **) &part_desc_addr_start);
    }

    // adjust memory mapped pointer to the required offset
    if (rc == ESP_OK) {
        *out_ptr = (void *) (s_spiflash_mem_file_buf + req_flash_addr);
        *out_handle = s_default_partition_mmap_handle;
    } else {
        *out_ptr = (void *) NULL;
        *out_handle = 0;
    }
    return rc;
}

// Intentionally does nothing.
void esp_partition_munmap(esp_partition_mmap_handle_t handle)
{
    ;
}

#ifdef CONFIG_ESP_PARTITION_ENABLE_STATS
// timing data for ESP8266, 160MHz CPU frequency, 80MHz flash requency
// all values in microseconds
// values are for block sizes starting at 4 bytes and going up to 4096 bytes
static size_t s_esp_partition_stat_read_times[] = {7, 5, 6, 7, 11, 18, 32, 60, 118, 231, 459};
static size_t s_esp_partition_stat_write_times[] = {19, 23, 35, 57, 106, 205, 417, 814, 1622, 3200, 6367};
static size_t s_esp_partition_stat_block_erase_time = 37142;

static size_t esp_partition_stat_time_interpolate(uint32_t bytes, size_t *lut)
{
    const int lut_size = sizeof(s_esp_partition_stat_read_times) / sizeof(s_esp_partition_stat_read_times[0]);
    int lz = __builtin_clz(bytes / 4);
    int log_size = 32 - lz;
    size_t x2 = 1 << (log_size + 2);
    size_t upper_index = (log_size < lut_size - 1) ? log_size : lut_size - 1;
    size_t y2 = lut[upper_index];
    size_t x1 = 1 << (log_size + 1);
    size_t y1 = lut[log_size - 1];
    return (bytes - x1) * (y2 - y1) / (x2 - x1) + y1;
}

// Registers read access statistics of emulated SPI FLASH device (Linux host)
// Ffunction increases nmuber of read operations, accumulates number of read bytes
// and accumulates emulated read operation time (size dependent)
static void esp_partition_hook_read(const void *srcAddr, const size_t size)
{
    ESP_LOGV(TAG, "esp_partition_hook_read()");

    // stats
    ++s_esp_partition_stat_read_ops;
    s_esp_partition_stat_read_bytes += size;
    s_esp_partition_stat_total_time += esp_partition_stat_time_interpolate((uint32_t) size, s_esp_partition_stat_read_times);
}

// Registers write access statistics of emulated SPI FLASH device (Linux host)
// If enabled by the esp_partition_fail_after, function emulates physical limitation of write/erase operations by
// decrementing the s_esp_partition_emulated_life for each 4 bytes written
// If zero threshold is reached, false is returned.
// Else the function increases nmuber of write operations, accumulates number
// of bytes written and accumulates emulated write operation time (size dependent) and returns true.
static bool esp_partition_hook_write(const void *dstAddr, const size_t size)
{
    ESP_LOGV(TAG, "esp_partition_hook_write()");

    // wear emulation
    for (size_t i = 0; i < size / 4; ++i) {
        if (s_esp_partition_emulated_flash_life != SIZE_MAX && s_esp_partition_emulated_flash_life-- == 0) {
            return false;
        }
    }

    // stats
    ++s_esp_partition_stat_write_ops;
    s_esp_partition_stat_write_bytes += size;
    s_esp_partition_stat_total_time += esp_partition_stat_time_interpolate((uint32_t) size, s_esp_partition_stat_write_times);

    return true;
}

// Registers erase access statistics of emulated SPI FLASH device (Linux host)
// If enabled by the esp_partition_fail_after, function emulates physical limitation of write/erase operations by
// decrementing the s_esp_partition_emulated_life for each erased virtual sector.
// If zero threshold is reached, false is returned.
// Else, for statistics purpose, the impacted virtual sectors are identified based on
// ESP_PARTITION_EMULATED_SECTOR_SIZE and their respective counts of erase operations are incremented
// Total number of erase operations is increased by the number of impacted virtual sectors
static bool esp_partition_hook_erase(const void *dstAddr, const size_t size)
{
    ESP_LOGV(TAG, "esp_partition_hook_erase()");

    if (size == 0) {
        return true;
    }

    // cycle over virtual sectors
    ptrdiff_t offset = dstAddr - s_spiflash_mem_file_buf;
    size_t first_sector_idx = offset / ESP_PARTITION_EMULATED_SECTOR_SIZE;
    size_t last_sector_idx = (offset + size - 1) / ESP_PARTITION_EMULATED_SECTOR_SIZE;
    size_t sector_count = 1 + last_sector_idx - first_sector_idx;

    for (size_t sector_index = first_sector_idx; sector_index < first_sector_idx + sector_count; sector_index++) {
        // wear emulation
        if (s_esp_partition_emulated_flash_life != SIZE_MAX && s_esp_partition_emulated_flash_life-- == 0) {
            return false;
        }

        // stats
        ++s_esp_partition_stat_erase_ops;
        s_esp_partition_stat_sector_erase_count[sector_index]++;
        s_esp_partition_stat_total_time += s_esp_partition_stat_block_erase_time;
    }

    return true;
}

void esp_partition_clear_stats(void)
{
    s_esp_partition_stat_read_bytes = 0;
    s_esp_partition_stat_write_bytes = 0;
    s_esp_partition_stat_erase_ops = 0;
    s_esp_partition_stat_read_ops = 0;
    s_esp_partition_stat_write_ops = 0;
    s_esp_partition_stat_total_time = 0;

    memset(s_esp_partition_stat_sector_erase_count, 0, sizeof(s_esp_partition_stat_sector_erase_count));
}

size_t esp_partition_get_read_ops(void)
{
    return s_esp_partition_stat_read_ops;
}

size_t esp_partition_get_write_ops(void)
{
    return s_esp_partition_stat_write_ops;
}

size_t esp_partition_get_erase_ops(void)
{
    return s_esp_partition_stat_erase_ops;
}

size_t esp_partition_get_read_bytes(void)
{
    return s_esp_partition_stat_read_bytes;
}

size_t esp_partition_get_write_bytes(void)
{
    return s_esp_partition_stat_write_bytes;
}

size_t esp_partition_get_total_time(void)
{
    return s_esp_partition_stat_total_time;
}

void esp_partition_fail_after(size_t count)
{
    s_esp_partition_emulated_flash_life = count;
}

size_t esp_partition_get_sector_erase_count(size_t sector)
{
    return s_esp_partition_stat_sector_erase_count[sector];
}
#endif
