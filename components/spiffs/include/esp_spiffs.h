/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_SPIFFS_H_
#define _ESP_SPIFFS_H_

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration structure for esp_vfs_spiffs_register
 */
typedef struct {
        const char* base_path;          /*!< File path prefix associated with the filesystem. */
        const char* partition_label;    /*!< Optional, label of SPIFFS partition to use. If set to NULL, first partition with subtype=spiffs will be used. */
        size_t max_files;               /*!< Maximum files that could be open at the same time. */
        bool format_if_mount_failed;    /*!< If true, it will format the file system if it fails to mount. */
} esp_vfs_spiffs_conf_t;

/**
 * Register and mount SPIFFS to VFS with given path prefix.
 *
 * @param   conf                      Pointer to esp_vfs_spiffs_conf_t configuration structure
 *
 * @return
 *          - ESP_OK                  if success
 *          - ESP_ERR_NO_MEM          if objects could not be allocated
 *          - ESP_ERR_INVALID_STATE   if already mounted or partition is encrypted
 *          - ESP_ERR_NOT_FOUND       if partition for SPIFFS was not found
 *          - ESP_FAIL                if mount or format fails
 */
esp_err_t esp_vfs_spiffs_register(const esp_vfs_spiffs_conf_t * conf);

/**
 * Unregister and unmount SPIFFS from VFS
 *
 * @param partition_label  Same label as passed to esp_vfs_spiffs_register.
 *
 * @return
 *          - ESP_OK if successful
 *          - ESP_ERR_INVALID_STATE already unregistered
 */
esp_err_t esp_vfs_spiffs_unregister(const char* partition_label);

/**
 * Check if SPIFFS is mounted
 *
 * @param partition_label  Optional, label of the partition to check.
 *                         If not specified, first partition with subtype=spiffs is used.
 *
 * @return
 *          - true    if mounted
 *          - false   if not mounted
 */
bool esp_spiffs_mounted(const char* partition_label);

/**
 * Format the SPIFFS partition
 *
 * @param partition_label  Same label as passed to esp_vfs_spiffs_register.
 * @return
 *          - ESP_OK      if successful
 *          - ESP_FAIL    on error
 */
esp_err_t esp_spiffs_format(const char* partition_label);

/**
 * Get information for SPIFFS
 *
 * @param partition_label           Same label as passed to esp_vfs_spiffs_register
 * @param[out] total_bytes          Size of the file system
 * @param[out] used_bytes           Current used bytes in the file system
 *
 * @return
 *          - ESP_OK                  if success
 *          - ESP_ERR_INVALID_STATE   if not mounted
 */
esp_err_t esp_spiffs_info(const char* partition_label, size_t *total_bytes, size_t *used_bytes);

/**
 * Check integrity of SPIFFS
 *
 * @param partition_label  Same label as passed to esp_vfs_spiffs_register
 * @return
 *          - ESP_OK                  if successful
 *          - ESP_ERR_INVALID_STATE   if not mounted
 *          - ESP_FAIL                on error
 */
esp_err_t esp_spiffs_check(const char* partition_label);


/**
 * @brief Perform garbage collection in SPIFFS partition
 *
 * Call this function to run GC and ensure that at least the given amount of
 * space is available in the partition. This function will fail with ESP_ERR_NOT_FINISHED
 * if it is not possible to reclaim the requested space (that is, not enough free
 * or deleted pages in the filesystem). This function will also fail if it fails to
 * reclaim the requested space after CONFIG_SPIFFS_GC_MAX_RUNS number of GC iterations.
 * On one GC iteration, SPIFFS will erase one logical block (4kB). Therefore the value
 * of CONFIG_SPIFFS_GC_MAX_RUNS should be set at least to the maximum expected size_to_gc,
 * divided by 4096. For example, if the application expects to make room for a 1MB file and
 * calls esp_spiffs_gc(label, 1024 * 1024), CONFIG_SPIFFS_GC_MAX_RUNS should be set to
 * at least 256.
 * On the other hand, increasing CONFIG_SPIFFS_GC_MAX_RUNS value increases the maximum
 * amount of time for which any SPIFFS GC or write operation may potentially block.
 *
 * @param partition_label  Label of the partition to be garbage-collected.
 *                         The partition must be already mounted.
 * @param size_to_gc       The number of bytes that the GC process should attempt
 *                         to make available.
 * @return
 *          - ESP_OK on success
 *          - ESP_ERR_NOT_FINISHED if GC fails to reclaim the size given by size_to_gc
 *          - ESP_ERR_INVALID_STATE if the partition is not mounted
 *          - ESP_FAIL on all other errors
 */
esp_err_t esp_spiffs_gc(const char* partition_label, size_t size_to_gc);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_SPIFFS_H_ */
