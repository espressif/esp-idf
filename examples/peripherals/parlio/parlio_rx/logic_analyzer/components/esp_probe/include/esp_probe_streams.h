/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdio.h>
#include <stdint.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_WIFI_SUPPORTED || SOC_EMAC_SUPPORTED
/**
 * @brief Open the output stream to the host via TCP
 *
 * @param[in] host_ip   The host IP address
 * @param[in] port      The host port
 * @return
 *      - The the output stream pointer
 */
FILE* esp_probe_open_tcp_stream(const char *host_ip, int port);

/**
 * @brief Close the output stream to the host
 *
 * @param[in]  f     The the output stream pointer
 */
void esp_probe_close_tcp_stream(FILE *f);
#endif  // SOC_WIFI_SUPPORTED || SOC_EMAC_SUPPORTED

/**
 * @brief Initialize the FAT partition in SPIFlash
 *
 * @param[in]  mount_point      The mount point
 * @param[in]  partition_label  The label of the partition that used for saving the raw probed data
 * @param[out] size             The size of this partition, can be NULL if not needed
 * @return
 *      - ESP_ERR_NOT_FOUND:    Not found the FAT partition with this label
 *      - ESP_OK:               Initialize FAT partition success
 *      - others:               Initialize failed due to other reasons
 */
esp_err_t esp_probe_init_spiflash_fatfs(const char *mount_point, const char *partition_label, uint32_t *size);

/**
 * @brief Deinitialize the FAT partition in SPIFlash
 *
 * @param[in]  mount_point      The mount point
 */
void esp_probe_deinit_spiflash_fatfs(const char *mount_point);

/**
 * @brief Open the output stream to the FAT partition in flash
 *
 * @param[in]  file_path    The file path to open
 * @return
 *      - The the output stream pointer
 */
FILE* esp_probe_open_file_stream(const char *file_path);

/**
 * @brief Close the output stream to the FAT partition in flash
 *
 * @param[in]  f     The the output stream pointer
 */
void esp_probe_close_file_stream(FILE *f);

#ifdef __cplusplus
}
#endif
