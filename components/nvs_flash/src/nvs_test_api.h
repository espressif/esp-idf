/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "nvs_flash.h"

/**
 * @brief Initialize NVS flash storage with custom flash sector layout
 *
 * @note  This API is intended to be used in unit tests.
 *
 * @param partName Partition name of the NVS partition as per partition table
 * @param baseSector Flash sector (units of 4096 bytes) offset to start NVS
 * @param sectorCount Length (in flash sectors) of NVS region.
 					  NVS partition must be at least 3 sectors long.
 * @return ESP_OK if flash was successfully initialized
 */
esp_err_t nvs_flash_init_custom(const char *partName, uint32_t baseSector, uint32_t sectorCount);

#ifdef CONFIG_NVS_ENCRYPTION
/**
 * @brief Initialize NVS flash storage with custom flash sector layout
 *
 * @note  This API is intended to be used in unit tests.
 *
 * @param partName Partition name of the NVS partition as per partition table
 * @param baseSector Flash sector (units of 4096 bytes) offset to start NVS
 * @param sectorCount Length (in flash sectors) of NVS region.
 					  NVS partition must be at least 3 sectors long.
 * @param[in]  cfg    Security configuration (keys) to be used for NVS encryption/decryption.
 *                    If cfg is null, no encryption/decryption is used.
 * @return ESP_OK if flash was successfully initialized
 */
esp_err_t nvs_flash_secure_init_custom(const char *partName, uint32_t baseSector, uint32_t sectorCount, nvs_sec_cfg_t* cfg);
#endif

/**
 * @brief Dump contents of NVS storage to stdout
 *
 * This function may be used for debugging purposes to inspect the state
 * of NVS pages. For each page, list of entries is also dumped.
 *
 * @param partName Partition name of the NVS partition as per partition table
 */
void nvs_dump(const char *partName);


#ifdef __cplusplus
}
#endif
