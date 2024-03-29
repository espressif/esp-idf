/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include "sdkconfig.h"
#include "esp_partition.h"
#include "esp_flash_partitions.h"
#include "esp_private/partition_linux.h"
#include "esp_log.h"
#include "spi_flash_mmap.h"

static const char *TAG = "linux_spiflash";

static void *s_spiflash_mem_file_buf = NULL;
static int s_spiflash_mem_file_fd = -1;
static const esp_partition_mmap_handle_t s_default_partition_mmap_handle = 0;

// input control structure, always contains what was specified by caller
static esp_partition_file_mmap_ctrl_t s_esp_partition_file_mmap_ctrl_input = {0};
// actual control structure, contains what is actually used by the esp_partition
static esp_partition_file_mmap_ctrl_t s_esp_partition_file_mmap_ctrl_act = {0};

#ifdef CONFIG_ESP_PARTITION_ENABLE_STATS
// variables holding stats and controlling power-off emulation
static size_t s_esp_partition_stat_read_ops = 0;
static size_t s_esp_partition_stat_write_ops = 0;
static size_t s_esp_partition_stat_read_bytes = 0;
static size_t s_esp_partition_stat_write_bytes = 0;
static size_t s_esp_partition_stat_erase_ops = 0;
static size_t s_esp_partition_stat_total_time = 0;
static size_t s_esp_partition_emulated_power_off_counter = SIZE_MAX;
static uint8_t s_esp_partition_emulated_power_off_mode = 0;

// tracking erase count individually for each emulated sector
static size_t *s_esp_partition_stat_sector_erase_count = NULL;

// forward declaration of hooks
static void esp_partition_hook_read(const void *srcAddr, const size_t size);
static bool esp_partition_hook_write(const void *dstAddr, size_t *size);
static bool esp_partition_hook_erase(const void *dstAddr, size_t *size);

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
    // temporary file is used only if control structure doesn't specify file name.
    bool open_existing_file = false;

    if (strlen(s_esp_partition_file_mmap_ctrl_input.flash_file_name) > 0) {
        // Open existing file. If size or partition table file were specified, raise errors
        if (s_esp_partition_file_mmap_ctrl_input.flash_file_size > 0) {
            ESP_LOGE(TAG, "Flash emulation file size: %" PRIu32" was specified while together with the file name: %s (illegal). Use file size = 0",
                     (uint32_t) s_esp_partition_file_mmap_ctrl_input.flash_file_size,
                     s_esp_partition_file_mmap_ctrl_input.flash_file_name);
            return ESP_ERR_INVALID_ARG;
        }

        if (strlen(s_esp_partition_file_mmap_ctrl_input.partition_file_name) > 0) {
            ESP_LOGE(TAG, "Partition file name: %s was specified together with the flash emulation file name: %s (illegal). Use empty partition file name",
                     s_esp_partition_file_mmap_ctrl_input.partition_file_name,
                     s_esp_partition_file_mmap_ctrl_input.flash_file_name);
            return ESP_ERR_INVALID_ARG;
        }

        // copy flash file name to actual control struct
        strlcpy(s_esp_partition_file_mmap_ctrl_act.flash_file_name, s_esp_partition_file_mmap_ctrl_input.flash_file_name, sizeof(s_esp_partition_file_mmap_ctrl_act.flash_file_name));

        open_existing_file = true;
    } else {
        // Open temporary file. If size was specified, also partition table has to be specified, otherwise raise error.
        // If none of size, partition table were specified, defaults are used.
        // Name of temporary file is available in s_esp_partition_file_mmap_ctrl.flash_file_name

        bool has_partfile = (strlen(s_esp_partition_file_mmap_ctrl_input.partition_file_name) > 0);
        bool has_len = (s_esp_partition_file_mmap_ctrl_input.flash_file_size > 0);

        // conflicting input
        if (has_partfile != has_len) {
            ESP_LOGE(TAG, "Invalid combination of Partition file name: %s flash file size: %" PRIu32 " was specified. Use either both parameters or none.",
                     s_esp_partition_file_mmap_ctrl_input.partition_file_name,
                     (uint32_t) s_esp_partition_file_mmap_ctrl_input.flash_file_size);
            return ESP_ERR_INVALID_ARG;
        }

        // check if partition file is present, if not, use default
        if (!has_partfile) {
            strlcpy(s_esp_partition_file_mmap_ctrl_act.partition_file_name, BUILD_DIR "/partition_table/partition-table.bin", sizeof(s_esp_partition_file_mmap_ctrl_act.partition_file_name));
        } else {
            strlcpy(s_esp_partition_file_mmap_ctrl_act.partition_file_name, s_esp_partition_file_mmap_ctrl_input.partition_file_name, sizeof(s_esp_partition_file_mmap_ctrl_act.partition_file_name));
        }

        // check if flash size is present, if not set to default
        if (!has_len) {
            s_esp_partition_file_mmap_ctrl_act.flash_file_size = ESP_PARTITION_DEFAULT_EMULATED_FLASH_SIZE;
        } else {
            s_esp_partition_file_mmap_ctrl_act.flash_file_size = s_esp_partition_file_mmap_ctrl_input.flash_file_size;
        }

        // specify pattern file name for temporary flash file
        strlcpy(s_esp_partition_file_mmap_ctrl_act.flash_file_name, "/tmp/idf-partition-XXXXXX", sizeof(s_esp_partition_file_mmap_ctrl_act.flash_file_name));
    }

    esp_err_t ret = ESP_OK;

    if (open_existing_file) {

        s_spiflash_mem_file_fd = open(s_esp_partition_file_mmap_ctrl_act.flash_file_name, O_RDWR);

        if (s_spiflash_mem_file_fd == -1) {
            ESP_LOGE(TAG, "Failed to open SPI FLASH emulation file %s: %s", s_esp_partition_file_mmap_ctrl_act.flash_file_name, strerror(errno));
            return ESP_ERR_NOT_FOUND;
        }

        do {
            // seek to the end
            off_t size = lseek(s_spiflash_mem_file_fd, 0L, SEEK_END);
            if (size < 0) {
                ESP_LOGE(TAG, "Failed to seek in SPI FLASH emulation file %s: %s", s_esp_partition_file_mmap_ctrl_act.flash_file_name, strerror(errno));
                ret = ESP_ERR_NOT_FINISHED;
                break;
            }

            s_esp_partition_file_mmap_ctrl_act.flash_file_size = size;

            // seek to beginning
            size = lseek(s_spiflash_mem_file_fd, 0L, SEEK_SET);
            if (size < 0) {
                ESP_LOGE(TAG, "Failed to seek in SPI FLASH emulation file %s: %s", s_esp_partition_file_mmap_ctrl_act.flash_file_name, strerror(errno));
                ret = ESP_ERR_NOT_FINISHED;
                break;
            }

            //create memory-mapping for the flash holder file
            if ((s_spiflash_mem_file_buf = mmap(NULL, s_esp_partition_file_mmap_ctrl_act.flash_file_size, PROT_READ | PROT_WRITE, MAP_SHARED, s_spiflash_mem_file_fd, 0)) == MAP_FAILED) {
                ESP_LOGE(TAG, "Failed to mmap() SPI FLASH memory emulation file %s: %s", s_esp_partition_file_mmap_ctrl_act.flash_file_name, strerror(errno));
                ret = ESP_ERR_NOT_FINISHED;
                break;
            }
        } while (false);
    } else {
        //create temporary file to hold complete SPIFLASH size
        s_spiflash_mem_file_fd = mkstemp(s_esp_partition_file_mmap_ctrl_act.flash_file_name);

        if (s_spiflash_mem_file_fd == -1) {
            ESP_LOGE(TAG, "Failed to create SPI FLASH emulation file %s: %s", s_esp_partition_file_mmap_ctrl_act.flash_file_name, strerror(errno));
            return ESP_ERR_NOT_FINISHED;
        }

        do {
            // resize file
            if (ftruncate(s_spiflash_mem_file_fd, s_esp_partition_file_mmap_ctrl_act.flash_file_size) != 0) {
                ESP_LOGE(TAG, "Failed to set size of SPI FLASH memory emulation file %s: %s", s_esp_partition_file_mmap_ctrl_act.flash_file_name, strerror(errno));
                ret = ESP_ERR_INVALID_SIZE;
                break;
            }

            ESP_LOGV(TAG, "SPIFLASH memory emulation file created: %s (size: %" PRIu32 " B)", s_esp_partition_file_mmap_ctrl_act.flash_file_name, (uint32_t) s_esp_partition_file_mmap_ctrl_act.flash_file_size);

            // create memory-mapping for the flash holder file
            if ((s_spiflash_mem_file_buf = mmap(NULL, s_esp_partition_file_mmap_ctrl_act.flash_file_size, PROT_READ | PROT_WRITE, MAP_SHARED, s_spiflash_mem_file_fd, 0)) == MAP_FAILED) {
                ESP_LOGE(TAG, "Failed to mmap() SPI FLASH memory emulation file %s: %s", s_esp_partition_file_mmap_ctrl_act.flash_file_name, strerror(errno));
                ret = ESP_ERR_NO_MEM;
                break;
            }

            // initialize whole range with bit-1 (NOR FLASH default)
            memset(s_spiflash_mem_file_buf, 0xFF, s_esp_partition_file_mmap_ctrl_act.flash_file_size);

            // upload partition table to the mmap file at real offset as in SPIFLASH
            FILE *f_partition_table = fopen(s_esp_partition_file_mmap_ctrl_act.partition_file_name, "r+");
            if (f_partition_table == NULL) {
                ESP_LOGE(TAG, "Failed to open partition table file %s: %s", s_esp_partition_file_mmap_ctrl_act.partition_file_name, strerror(errno));
                ret = ESP_ERR_NOT_FOUND;
                break;
            }

            if (fseek(f_partition_table, 0L, SEEK_END) != 0) {
                ESP_LOGE(TAG, "Failed to seek in partition table file %s: %s", s_esp_partition_file_mmap_ctrl_act.partition_file_name, strerror(errno));
                ret =  ESP_ERR_INVALID_SIZE;
                break;
            }

            int partition_table_file_size = ftell(f_partition_table);
            ESP_LOGV(TAG, "Using partition table file %s (size: %d B):", s_esp_partition_file_mmap_ctrl_act.partition_file_name, partition_table_file_size);

            // check whether partition table fits into the memory mapped file
            if (partition_table_file_size + ESP_PARTITION_TABLE_OFFSET > s_esp_partition_file_mmap_ctrl_act.flash_file_size) {
                ESP_LOGE(TAG, "Flash file: %s (size: %" PRIu32 " B) cannot hold partition table requiring %d B",
                         s_esp_partition_file_mmap_ctrl_act.flash_file_name,
                         (uint32_t) s_esp_partition_file_mmap_ctrl_act.flash_file_size,
                         (int) (partition_table_file_size + ESP_PARTITION_TABLE_OFFSET));
                ret =  ESP_ERR_INVALID_SIZE;
                break;
            }

            //copy partition table from the file to emulated SPIFLASH memory space
            if (fseek(f_partition_table, 0L, SEEK_SET) != 0) {
                ESP_LOGE(TAG, "Failed to seek in partition table file %s: %s", s_esp_partition_file_mmap_ctrl_act.partition_file_name, strerror(errno));
                ret =  ESP_ERR_INVALID_SIZE;
                break;
            }

            uint8_t *part_table_in_spiflash = s_spiflash_mem_file_buf + ESP_PARTITION_TABLE_OFFSET;

            size_t res = fread(part_table_in_spiflash, 1, partition_table_file_size, f_partition_table);
            fclose(f_partition_table);
            if (res != partition_table_file_size) {
                ESP_LOGE(TAG, "Failed to read partition table file %s", s_esp_partition_file_mmap_ctrl_act.partition_file_name);
                ret = ESP_ERR_INVALID_STATE;
                break;
            }
        } while (false);
    }

    if (ret != ESP_OK) {
        if (close(s_spiflash_mem_file_fd)) {
            ESP_LOGE(TAG, "Failed to close() SPIFLASH memory emulation file: %s", strerror(errno));
        }
        s_spiflash_mem_file_fd = -1;

        return ret;
    }

#ifdef CONFIG_LOG_DEFAULT_LEVEL_VERBOSE
    uint8_t *part_ptr = s_spiflash_mem_file_buf + ESP_PARTITION_TABLE_OFFSET;

    ESP_LOGV(TAG, "");
    ESP_LOGV(TAG, "Partition table successfully imported, partitions found:");

    while (true) {
        esp_partition_info_t *p_part_item = (esp_partition_info_t *)part_ptr;
        if (p_part_item->magic != ESP_PARTITION_MAGIC ) {
            break;
        }

        ESP_LOGV(TAG, " --------------");
        ESP_LOGV(TAG, " label: %s", p_part_item->label);
        ESP_LOGV(TAG, " type: %s", esp_partition_type_to_str(p_part_item->type));
        ESP_LOGV(TAG, " subtype: %s", esp_partition_subtype_to_str(p_part_item->type, p_part_item->subtype));
        ESP_LOGV(TAG, " offset: 0x%08" PRIX32, (uint32_t) p_part_item->pos.offset);
        ESP_LOGV(TAG, " size: %" PRIu32, (uint32_t) p_part_item->pos.size);
        ESP_LOGV(TAG, " flags: %" PRIu32, (uint32_t) p_part_item->flags);

        part_ptr += sizeof(esp_partition_info_t);
    }

    ESP_LOGV(TAG, "");
#endif

#ifdef CONFIG_ESP_PARTITION_ENABLE_STATS
    free(s_esp_partition_stat_sector_erase_count);
    s_esp_partition_stat_sector_erase_count = malloc(sizeof(size_t) * s_esp_partition_file_mmap_ctrl_act.flash_file_size / ESP_PARTITION_EMULATED_SECTOR_SIZE);
#endif

    //return mmapped file starting address
    *part_desc_addr_start = s_spiflash_mem_file_buf;

    // clear input control structure
    memset(&s_esp_partition_file_mmap_ctrl_input, 0, sizeof(s_esp_partition_file_mmap_ctrl_input));

    return ESP_OK;
}

esp_err_t esp_partition_file_munmap(void)
{
    if (s_spiflash_mem_file_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }
    if (s_esp_partition_file_mmap_ctrl_act.flash_file_size == 0) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (s_spiflash_mem_file_fd == -1) {
        return ESP_ERR_NOT_FOUND;
    }

    esp_partition_unload_all();

#ifdef CONFIG_ESP_PARTITION_ENABLE_STATS
    free(s_esp_partition_stat_sector_erase_count);
    s_esp_partition_stat_sector_erase_count = NULL;
#endif

    // unmap the flash emulation memory file
    if (munmap(s_spiflash_mem_file_buf, s_esp_partition_file_mmap_ctrl_act.flash_file_size) != 0) {
        ESP_LOGE(TAG, "Failed to munmap() SPIFLASH memory emulation file %s: %s", s_esp_partition_file_mmap_ctrl_act.flash_file_name, strerror(errno));
        return ESP_ERR_INVALID_RESPONSE;
    }

    // close memory mapped file
    if (close(s_spiflash_mem_file_fd)) {
        ESP_LOGE(TAG, "Failed to close() SPIFLASH memory emulation file %s: %s", s_esp_partition_file_mmap_ctrl_act.flash_file_name, strerror(errno));
        return ESP_ERR_INVALID_RESPONSE;
    }

    if (s_esp_partition_file_mmap_ctrl_input.remove_dump) {
        // delete spi flash file
        if (remove(s_esp_partition_file_mmap_ctrl_act.flash_file_name) != 0) {
            ESP_LOGE(TAG, "Failed to remove() SPI FLASH memory emulation file %s: %s", s_esp_partition_file_mmap_ctrl_act.flash_file_name, strerror(errno));
            return ESP_ERR_INVALID_RESPONSE;
        }
    }

    // cleanup
    memset(&s_esp_partition_file_mmap_ctrl_act, 0, sizeof(s_esp_partition_file_mmap_ctrl_act));
    s_spiflash_mem_file_buf = NULL;
    s_spiflash_mem_file_fd = -1;

    return ESP_OK;
}

esp_err_t esp_partition_write(const esp_partition_t *partition, size_t dst_offset, const void *src, size_t size)
{
    assert(partition != NULL && s_spiflash_mem_file_buf != NULL);

    if (partition->readonly) {
        return ESP_ERR_NOT_ALLOWED;
    }
    if (partition->encrypted) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (dst_offset > partition->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (dst_offset + size > partition->size) {
        return ESP_ERR_INVALID_SIZE;
    }

    void *dst_addr = s_spiflash_mem_file_buf + partition->address + dst_offset;
    ESP_LOGV(TAG, "esp_partition_write(): partition=%s dst_offset=%" PRIu32 " src=%p size=%" PRIu32 " (real dst address: %p)", partition->label, (uint32_t) dst_offset, src, (uint32_t) size, dst_addr);

    // local size, can be modified by the write hook in case of simulated power-off
    size_t new_size = size;

    esp_err_t ret = ESP_OK;

    // hook gathers statistics and can emulate power-off
    // in case of power - off it decreases new_size to the number of bytes written
    // before power event occurred
    if (!ESP_PARTITION_HOOK_WRITE(dst_addr, &new_size)) {
        ret =  ESP_ERR_FLASH_OP_FAIL;
    }

    for (size_t x = 0; x < new_size; x++) {

        // Check if address to be written was erased first
        if((~((uint8_t *)dst_addr)[x] & ((uint8_t *)src)[x]) != 0) {
            ESP_LOGW(TAG, "invalid flash operation detected");
            ret = ESP_ERR_FLASH_OP_FAIL;
            break;
        }

        // AND with destination byte (to emulate real NOR FLASH behavior)
        ((uint8_t *)dst_addr)[x] &= ((uint8_t *)src)[x];
    }

    return ret;
}

esp_err_t esp_partition_read(const esp_partition_t *partition, size_t src_offset, void *dst, size_t size)
{
    assert(partition != NULL && s_spiflash_mem_file_buf != NULL);

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
    ESP_LOGV(TAG, "esp_partition_read(): partition=%s src_offset=%" PRIu32 " dst=%p size=%" PRIu32 " (real src address: %p)", partition->label, (uint32_t) src_offset, dst, (uint32_t) size, src_addr);

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

    if (partition->readonly) {
        return ESP_ERR_NOT_ALLOWED;
    }
    if (offset > partition->size || offset % partition->erase_size != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (offset + size > partition->size || size % partition->erase_size != 0) {
        return ESP_ERR_INVALID_SIZE;
    }

    void *target_addr = s_spiflash_mem_file_buf + partition->address + offset;
    ESP_LOGV(TAG, "esp_partition_erase_range(): partition=%s offset=%" PRIu32 " size=%" PRIu32 " (real target address: %p)", partition->label, (uint32_t) offset, (uint32_t) size, target_addr);

    // local size to be potentially updated by the hook in case of power-off event
    size_t new_size = size;

    // hook gathers statistics and can emulate power-off
    esp_err_t ret = ESP_OK;

    if(!ESP_PARTITION_HOOK_ERASE(target_addr, &new_size)) {
        ret =  ESP_ERR_FLASH_OP_FAIL;
    }

    //set all bits to 1 (NOR FLASH default)
    memset(target_addr, 0xFF, new_size);

    return ret;
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
    ESP_LOGV(TAG, "esp_partition_mmap(): partition=%s offset=%" PRIu32 " size=%" PRIu32 "", partition->label, (uint32_t) offset, (uint32_t) size);

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
        uint8_t *part_desc_addr_start = NULL;
        rc = esp_partition_file_mmap((const uint8_t **) &part_desc_addr_start);
    }

    // adjust memory mapped pointer to the required offset
    if (rc == ESP_OK) {
        *out_ptr = (void *) (s_spiflash_mem_file_buf + req_flash_addr);
        *out_handle = s_default_partition_mmap_handle;
    } else {
        *out_ptr = NULL;
        *out_handle = 0;
    }
    return rc;
}

// Intentionally does nothing.
void esp_partition_munmap(esp_partition_mmap_handle_t handle __attribute__((unused)))
{
}

esp_partition_file_mmap_ctrl_t *esp_partition_get_file_mmap_ctrl_input(void)
{
    return &s_esp_partition_file_mmap_ctrl_input;
}

esp_partition_file_mmap_ctrl_t *esp_partition_get_file_mmap_ctrl_act(void)
{
    return &s_esp_partition_file_mmap_ctrl_act;
}

#ifdef CONFIG_ESP_PARTITION_ENABLE_STATS
// timing data for ESP8266, 160MHz CPU frequency, 80MHz flash frequency
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
// Function increases nmuber of read operations, accumulates number of read bytes
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
// If enabled by the esp_partition_fail_after, function emulates power-off event during write operations by
// decrementing the s_esp_partition_emulated_power_off_counter for each 4 bytes written
// If zero threshold is reached, false is returned. In this case the size parameter contains number of successfully written bytes
// Else the function increases nmuber of write operations, accumulates number
// of bytes written and accumulates emulated write operation time (size dependent) and returns true.
static bool esp_partition_hook_write(const void *dstAddr, size_t *size)
{
    ESP_LOGV(TAG, "%s", __FUNCTION__);

    bool ret_val = true;

    // one power down cycle per 4 bytes written
    size_t write_cycles = *size / 4;

    // check whether power off simulation is active for write
    if (s_esp_partition_emulated_power_off_counter != SIZE_MAX &&
            ESP_PARTITION_FAIL_AFTER_MODE_WRITE) {

        // check if power down happens during this call
        if (s_esp_partition_emulated_power_off_counter > write_cycles) {
            // OK
            s_esp_partition_emulated_power_off_counter -= write_cycles;
        } else {
            // failure in this call

            // update number of bytes written to the in/out parameter
            *size = s_esp_partition_emulated_power_off_counter * 4;

            // disable power on cycles for further calls
            s_esp_partition_emulated_power_off_counter = SIZE_MAX;
            // final result value will be false
            ret_val = false;
        }
    }

    if(ret_val) {
        // stats
        ++s_esp_partition_stat_write_ops;
        s_esp_partition_stat_write_bytes += write_cycles * 4;
        s_esp_partition_stat_total_time += esp_partition_stat_time_interpolate((uint32_t) (*size), s_esp_partition_stat_write_times);
    }

    return ret_val;
}

// Registers erase access statistics of emulated SPI FLASH device (Linux host)
// If enabled by 'esp_partition_fail_after' parameter, the function emulates a power-off event during erase
// operation by decrementing the s_esp_partition_emulated_power_off_counterpower for each erased virtual sector.
// If zero threshold is reached, false is returned. In out parameter size is updated with number of bytes erased until power-off
// Else, for statistics purpose, the impacted virtual sectors are identified based on
// ESP_PARTITION_EMULATED_SECTOR_SIZE and their respective counts of erase operations are incremented
// Total number of erase operations is increased by the number of impacted virtual sectors
static bool esp_partition_hook_erase(const void *dstAddr, size_t *size)
{
    ESP_LOGV(TAG, "%s", __FUNCTION__);

    if (*size == 0) {
        return true;
    }

    // cycle over virtual sectors
    ptrdiff_t offset = dstAddr - s_spiflash_mem_file_buf;
    size_t first_sector_idx = offset / ESP_PARTITION_EMULATED_SECTOR_SIZE;
    size_t last_sector_idx = (offset + *size - 1) / ESP_PARTITION_EMULATED_SECTOR_SIZE;
    size_t sector_count = 1 + last_sector_idx - first_sector_idx;

    bool ret_val = true;

    // check whether power off simulation is active for erase
    if (s_esp_partition_emulated_power_off_counter != SIZE_MAX &&
            ESP_PARTITION_FAIL_AFTER_MODE_ERASE) {

        // check if power down happens during this call
        if (s_esp_partition_emulated_power_off_counter > sector_count) {
            // OK
            s_esp_partition_emulated_power_off_counter -= sector_count;
        } else {
            // failure in this call - reduce sector_count to the number of remaining power on cycles
            sector_count = s_esp_partition_emulated_power_off_counter;
            // disable power on cycles for further calls
            s_esp_partition_emulated_power_off_counter = SIZE_MAX;
            // update number of bytes to be really erased before power-off event
            *size = sector_count * ESP_PARTITION_EMULATED_SECTOR_SIZE;
            // final result value will be false
            ret_val = false;
        }
    }

    // update statistcs for all sectors until power down cycle
    for (size_t sector_index = first_sector_idx; sector_index < first_sector_idx + sector_count; sector_index++) {
        ++s_esp_partition_stat_erase_ops;
        s_esp_partition_stat_sector_erase_count[sector_index]++;
        s_esp_partition_stat_total_time += s_esp_partition_stat_block_erase_time;
    }

    return ret_val;
}

void esp_partition_clear_stats(void)
{
    s_esp_partition_stat_read_bytes = 0;
    s_esp_partition_stat_write_bytes = 0;
    s_esp_partition_stat_erase_ops = 0;
    s_esp_partition_stat_read_ops = 0;
    s_esp_partition_stat_write_ops = 0;
    s_esp_partition_stat_total_time = 0;

    memset(s_esp_partition_stat_sector_erase_count, 0, sizeof(size_t) * s_esp_partition_file_mmap_ctrl_act.flash_file_size / ESP_PARTITION_EMULATED_SECTOR_SIZE);
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

void esp_partition_fail_after(size_t count, uint8_t mode)
{
    s_esp_partition_emulated_power_off_counter = count;
    s_esp_partition_emulated_power_off_mode = mode;
}

size_t esp_partition_get_sector_erase_count(size_t sector)
{
    return s_esp_partition_stat_sector_erase_count[sector];
}
#endif
