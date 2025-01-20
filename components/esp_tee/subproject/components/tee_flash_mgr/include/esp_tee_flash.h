/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

#include "esp_err.h"

#include "bootloader_flash_priv.h"
#include "esp_flash_partitions.h"
#include "esp_image_format.h"

/* Defining placeholders for the bootloader flash APIs */
#define esp_tee_flash_mmap_get_free_pages  bootloader_mmap_get_free_pages
#define esp_tee_flash_mmap                 bootloader_mmap
#define esp_tee_flash_munmap               bootloader_munmap
#define esp_tee_flash_read                 bootloader_flash_read
#define esp_tee_flash_write                bootloader_flash_write
#define esp_tee_flash_erase_range          bootloader_flash_erase_range

/**
 * @brief Setup the context holding the permissible address ranges for flash operations through TEE
 *
 * @param tee_boot_part Active TEE partition
 *
 * @return esp_err_t ESP_OK: no error; ESP_FAIL otherwise.
 */
esp_err_t esp_tee_flash_setup_prot_ctx(uint8_t tee_boot_part);

/**
 * @brief Get the partition table entry of a partition to be found,
 *        given its type, subtype and label
 *        If label is NULL, the first partition with matching type and
 *        subtype is returned.
 *
 * @param type      Partition type
 * @param subtype   Partition subtype
 * @param label     Partition label
 * @param dest_ptr  Pointer to where the the partition table entry of
 *                  the requested partition is to be copied
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_flash_find_partition(uint8_t type, uint8_t subtype, const char *label, esp_partition_info_t *dest_ptr);

/**
 * @brief Get the partition table entry of a partition to be found,
 *        given an address present within its range
 *
 * @param paddr     Physical address
 * @param part_info Pointer to where the the partition table entry of
 *                  the found partition is to be copied
 *
 * @return esp_err_t ESP_OK: no error;
 *                   ESP_ERR_NO_INVALID_ARG: part_info is NULL;
 *                   ESP_ERR_NOT_FOUND: No partition found for the given paddr;
 *                   ESP_FAIL otherwise.
 */
esp_err_t esp_tee_flash_get_part_info_for_addr(uint32_t paddr, esp_partition_info_t *part_info);

/**
 * @brief Set the running REE (user) app partition as per the argument given by the bootloader
 *
 * @param paddr      Physical address within the running REE (user) app partition's range
 *
 * @return esp_err_t ESP_OK: no error; otherwise refer esp_tee_flash_get_part_info_for_addr() error codes
 */
esp_err_t esp_tee_flash_set_running_ree_partition(uint32_t paddr);

/**
 * @brief Fetch the running REE (user) app partition
 *
 * @return esp_partition_info_t*  Partition table entry for the running REE (user) app partition
 */
esp_partition_info_t *esp_tee_flash_get_running_ree_partition(void);

/**
 * @brief Check if the given virtual address falls within the TEE flash protected region
 *
 * @param addr Virtual address to check
 *
 * @return bool true if address is within protected region, false otherwise
 */
bool esp_tee_flash_check_vaddr_in_tee_region(const size_t vaddr);

/**
 * @brief Check if the given physical address falls within the TEE flash protected region
 *
 * @param addr Physical address to check
 *
 * @return bool true if address is within protected region, false otherwise
 */
bool esp_tee_flash_check_paddr_in_tee_region(const size_t paddr);

/**
 * @brief Check if the given physical address falls within the active TEE partition
 *
 * @param dest_addr Physical address to check
 *
 * @return bool true if address is within active TEE partition, false otherwise
 */
bool esp_tee_flash_check_paddr_in_active_tee_part(const size_t paddr);
