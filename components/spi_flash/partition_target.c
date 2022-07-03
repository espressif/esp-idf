/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#include "esp_flash_partitions.h"
#include "esp_attr.h"
#include "esp_flash.h"
#include "esp_partition.h"
#include "esp_flash_encrypt.h"
#include "esp_log.h"
#include "esp_rom_md5.h"
#include "esp_spi_flash.h"
#include "bootloader_common.h"
#include "esp_ota_ops.h"

#define HASH_LEN 32 /* SHA-256 digest length */

esp_err_t esp_partition_read(const esp_partition_t *partition,
                             size_t src_offset, void *dst, size_t size)
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
    }

#if CONFIG_SPI_FLASH_ENABLE_ENCRYPTED_READ_WRITE
    if (partition->flash_chip != esp_flash_default_chip) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    /* Encrypted partitions need to be read via a cache mapping */
    const void *buf;
    spi_flash_mmap_handle_t handle;

    esp_err_t err = esp_partition_mmap(partition, src_offset, size,
                                       SPI_FLASH_MMAP_DATA, &buf, &handle);
    if (err != ESP_OK) {
        return err;
    }
    memcpy(dst, buf, size);
    spi_flash_munmap(handle);
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif // CONFIG_SPI_FLASH_ENABLE_ENCRYPTED_READ_WRITE
}

esp_err_t esp_partition_write(const esp_partition_t *partition,
                              size_t dst_offset, const void *src, size_t size)
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
    }

#if CONFIG_SPI_FLASH_ENABLE_ENCRYPTED_READ_WRITE
    if (partition->flash_chip != esp_flash_default_chip) {
        return ESP_ERR_NOT_SUPPORTED;
    }

#ifndef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
    return esp_flash_write_encrypted(partition->flash_chip, dst_offset, src, size);
#else
    return spi_flash_write_encrypted(dst_offset, src, size);
#endif // CONFIG_SPI_FLASH_USE_LEGACY_IMPL
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif // CONFIG_SPI_FLASH_ENABLE_ENCRYPTED_READ_WRITE
}

esp_err_t esp_partition_read_raw(const esp_partition_t *partition,
                                 size_t src_offset, void *dst, size_t size)
{
    assert(partition != NULL);
    if (src_offset > partition->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (src_offset + size > partition->size) {
        return ESP_ERR_INVALID_SIZE;
    }

#ifndef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
    return esp_flash_read(partition->flash_chip, dst, partition->address + src_offset, size);
#else
    return spi_flash_read(partition->address + src_offset, dst, size);
#endif // CONFIG_SPI_FLASH_USE_LEGACY_IMPL
}

esp_err_t esp_partition_write_raw(const esp_partition_t *partition,
                                  size_t dst_offset, const void *src, size_t size)
{
    assert(partition != NULL);
    if (dst_offset > partition->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (dst_offset + size > partition->size) {
        return ESP_ERR_INVALID_SIZE;
    }
    dst_offset = partition->address + dst_offset;

#ifndef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
    return esp_flash_write(partition->flash_chip, src, dst_offset, size);
#else
    return spi_flash_write(dst_offset, src, size);
#endif // CONFIG_SPI_FLASH_USE_LEGACY_IMPL
}

esp_err_t esp_partition_erase_range(const esp_partition_t *partition,
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
esp_err_t esp_partition_mmap(const esp_partition_t *partition, size_t offset, size_t size,
                             spi_flash_mmap_memory_t memory,
                             const void **out_ptr, spi_flash_mmap_handle_t *out_handle)
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
    // offset within mmu page size block
    size_t region_offset = phys_addr & (CONFIG_MMU_PAGE_SIZE - 1);
    size_t mmap_addr = phys_addr & ~(CONFIG_MMU_PAGE_SIZE - 1);
    esp_err_t rc = spi_flash_mmap(mmap_addr, size + region_offset, memory, out_ptr, out_handle);
    // adjust returned pointer to point to the correct offset
    if (rc == ESP_OK) {
        *out_ptr = (void *) (((ptrdiff_t) * out_ptr) + region_offset);
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
