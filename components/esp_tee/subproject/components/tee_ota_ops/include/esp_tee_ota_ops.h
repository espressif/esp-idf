/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Commence the TEE OTA update
 *
 * This function does the following:
 * - Initialize the internal TEE OTA state machine
 * - Set the passive TEE partition as the destination for the new image and erase it
 *
 * @return
 *         - `ESP_OK` on success
 *         - `ESP_ERR_NOT_FOUND` in case the `tee_otadata` or the passive TEE partition is not found
 *         - `ESP_ERR_FLASH_OP_FAIL` in case erasing the passive TEE partition fails
 */
esp_err_t esp_tee_ota_begin(void);

/**
 * @brief Write TEE OTA update data to the partition.
 *        This function can be called multiple times as data is received during the OTA operation
 *        and can write data in non-contiguous manner.
 *
 * @param rel_offset  Address offset at which the given data should be written at -
 *                    relative to the start address of the passive TEE partition
 * @param data        Data buffer to write
 * @param size        Size of data buffer in bytes
 *
 * @return
 *         - `ESP_OK` on success
 *         - `ESP_ERR_INVALID_ARG` in case the `tee_otadata` or the passive TEE partition is not found
 *         - `ESP_ERR_INVALID_STATE` in case the TEE OTA state machine is in an invalid state
 *         - `ESP_ERR_FLASH_OP_FAIL` in case writing the new TEE image fails
 *         - `ESP_FAIL` for other errors
 *
 */
esp_err_t esp_tee_ota_write(uint32_t rel_offset, const void *data, size_t size);

/**
 * @brief Finish the TEE OTA update and validate newly written TEE image
 *
 * @return
 *         - `ESP_OK` on success
 *         - `ESP_ERR_INVALID_STATE` in case the TEE OTA state machine is in an invalid state
 *         - `ESP_ERR_IMAGE_INVALID` in case the new TEE OTA image verification fails
 *         - `ESP_FAIL` for other errors
 */
esp_err_t esp_tee_ota_end(void);

#ifdef __cplusplus
}
#endif
