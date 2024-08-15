/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_ota_ops.h"
#include "esp_partition.h"

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief Enumeration for specifying which OTA data sectors' CRCs to corrupt.
 */
typedef enum {
    CORR_CRC_1_SECTOR_OTA_DATA       = (1 << 0),   /*!< Corrupt CRC only 1 sector of ota_data */
    CORR_CRC_2_SECTOR_OTA_DATA       = (1 << 1),   /*!< Corrupt CRC only 2 sector of ota_data */
} corrupt_ota_data_t;

/**
 * @brief Set boot count value in NVS.
 * @param boot_count Value to set.
 */
void set_boot_count_in_nvs(uint8_t boot_count);

/**
 * @brief Get boot count value from NVS.
 * @return Boot count value.
 */
uint8_t get_boot_count_from_nvs(void);

/**
 * @brief Copy current app to next partition using OTA handle.
 * @param update_handle OTA update handle.
 * @param curr_app Current app partition.
 */
void copy_app_partition(esp_ota_handle_t update_handle, const esp_partition_t *curr_app);

/**
 * @brief Copy current app to next partition using OTA handle with offset.
 * @param update_handle OTA update handle.
 * @param curr_app Current app partition.
 */
void copy_app_partition_with_offset(esp_ota_handle_t update_handle, const esp_partition_t *curr_app);

/**
 * @brief Get the next OTA update partition.
 * @return Pointer to next OTA partition.
 */
const esp_partition_t * get_next_update_partition(void);

/**
 * @brief Copy current app to next partition and set boot partition.
 * @param cur_app_partition Current app partition.
 * @param next_app_partition Next app partition.
 */
void copy_current_app_to_next_part(const esp_partition_t *cur_app_partition, const esp_partition_t *next_app_partition);

/**
 * @brief Copy current app to next partition with offset and set boot partition.
 * @param cur_app_partition Current app partition.
 * @param next_app_partition Next app partition.
 */
void copy_current_app_to_next_part_with_offset(const esp_partition_t *cur_app_partition, const esp_partition_t *next_app_partition);

/**
 * @brief Erase OTA data partition.
 */
void erase_ota_data(void);

/**
 * @brief Reboot ESP using deep sleep mode.
 */
void reboot_as_deep_sleep(void);

/**
 * @brief Copy current app to next partition and reboot.
 */
void copy_current_app_to_next_part_and_reboot(void);

/**
 * @brief Copy current app to next partition with offset and reboot.
 */
void copy_current_app_to_next_part_with_offset_and_reboot(void);

/**
 * @brief Get running firmware partition.
 * @return Pointer to running firmware partition.
 */
const esp_partition_t* get_running_firmware(void);

/**
 * @brief Get two OTA data copies from OTA data partition.
 * @param otadata_partition OTA data partition.
 * @param ota_data_0 First OTA data copy.
 * @param ota_data_1 Second OTA data copy.
 */
void get_ota_data(const esp_partition_t *otadata_partition, esp_ota_select_entry_t *ota_data_0, esp_ota_select_entry_t *ota_data_1);

/**
 * @brief Write OTA data into required sector of OTA data partition.
 * @param otadata_partition OTA data partition.
 * @param ota_data OTA data structure.
 * @param sec_id Sector number (0 or 1).
 */
void write_ota_data(const esp_partition_t *otadata_partition, esp_ota_select_entry_t *ota_data, int sec_id);

/**
 * @brief Corrupt OTA data for testing.
 * @param err Type of corruption.
 */
void corrupt_ota_data(corrupt_ota_data_t err);

#if defined(CONFIG_BOOTLOADER_FACTORY_RESET) || defined(CONFIG_BOOTLOADER_APP_TEST)
/**
 * @brief Set output pin to low and hold state.
 * @param num_pin Pin number.
 */
void set_output_pin(uint32_t num_pin);

/**
 * @brief Reset output pin hold function.
 * @param num_pin Pin number.
 */
void reset_output_pin(uint32_t num_pin);
#endif

/**
 * @brief Mark app as valid and cancel rollback.
 */
void mark_app_valid(void);

/**
 * @brief Perform app update and set new boot partition.
 * @return Pointer to updated partition.
 */
const esp_partition_t* app_update(void);

#ifdef __cplusplus
}
#endif
