/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_flash_partitions.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fetch the currently running TEE partition
 *
 * @param[in] tee_ota_info TEE OTA data partition
 *
 * @return Subtype of the running TEE partition, or -1 if an error occurred
 */
int bootloader_utility_tee_get_boot_partition(const esp_partition_pos_t *tee_ota_info);

/**
 * @brief Set a new TEE boot partition in the TEE OTA data
 *
 * @param[in] tee_ota_info TEE OTA data partition
 * @param[in] tee_try_part Partition table entry for the new boot partition
 *
 * @return ESP_OK on success, or an error code otherwise
 */
esp_err_t bootloader_utility_tee_set_boot_partition(const esp_partition_pos_t *tee_ota_info, const esp_partition_info_t *tee_try_part);

/**
 * @brief Fetch the next TEE partition for update
 *
 * @param[in] tee_ota_info TEE OTA data partition
 *
 * @return Subtype of the next TEE partition for update, or -1 if an error occurred
 */
int bootloader_utility_tee_get_next_update_partition(const esp_partition_pos_t *tee_ota_info);

/**
 * @brief Mark the current TEE app as valid and cancel update rollback
 *
 * @param[in] tee_ota_info TEE OTA data partition
 *
 * @return ESP_OK on success, or an error code otherwise
 */
esp_err_t bootloader_utility_tee_mark_app_valid_and_cancel_rollback(const esp_partition_pos_t *tee_ota_info);

#ifdef __cplusplus
}
#endif
