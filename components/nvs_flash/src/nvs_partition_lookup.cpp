/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "nvs_partition_lookup.hpp"     // For partition_lookup namespace

#include "esp_partition.h"          // For esp_partition_t
#ifndef LINUX_TARGET
#include "nvs_encrypted_partition.hpp"
#endif // ! LINUX_TARGET

namespace nvs {
namespace partition_lookup {

esp_err_t lookup_nvs_partition(const char* label, Partition **p)
{
#ifndef CONFIG_NVS_BDL_STACK
    const esp_partition_t* esp_partition = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, label);

    if (esp_partition == nullptr) {
        return ESP_ERR_NOT_FOUND;
    }
    if (esp_partition->encrypted) {
        return ESP_ERR_NVS_WRONG_ENCRYPTION;
    }

    NVSPartition *partition = new (std::nothrow) NVSPartition(esp_partition);
    if (partition == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    *p = partition;
    return ESP_OK;
#else
    esp_blockdev_handle_t bdl = NULL;
    esp_err_t err = esp_partition_get_blockdev(
        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, label, &bdl);

    if (err != ESP_OK) {
        return ESP_ERR_NOT_FOUND;
    }
    if (!NVSPartition::is_bdl_nvs_compliant(label, bdl)) {
        if(bdl->ops->release) bdl->ops->release(bdl);
        return ESP_ERR_NVS_PART_NOT_FOUND;
    }

    NVSPartition *partition = new (std::nothrow) NVSPartition(label, bdl);
    if (partition == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    *p = partition;
    return ESP_OK;
#endif
}

#ifndef LINUX_TARGET
esp_err_t lookup_nvs_encrypted_partition(const char* label, nvs_sec_cfg_t* cfg, Partition **p)
{
#ifndef CONFIG_NVS_BDL_STACK
    const esp_partition_t* esp_partition = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, label);

    if (esp_partition == nullptr) {
        return ESP_ERR_NOT_FOUND;
    }

    if (esp_partition->encrypted) {
        return ESP_ERR_NVS_WRONG_ENCRYPTION;
    }

    NVSEncryptedPartition *partition = new (std::nothrow) NVSEncryptedPartition(esp_partition);
    if (partition == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    esp_err_t result = partition->init(cfg);
    if (result != ESP_OK) {
        delete partition;
        return result;
    }
    *p = partition;
    return ESP_OK;
#else
    esp_blockdev_handle_t bdl = NULL;
    esp_err_t err = esp_partition_get_blockdev(
        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, label, &bdl);

    if (err != ESP_OK) {
        return ESP_ERR_NOT_FOUND;
    }

    if (!NVSPartition::is_bdl_nvs_compliant(label, bdl)) {
         if(bdl->ops->release) bdl->ops->release(bdl);
        return ESP_ERR_NVS_PART_NOT_FOUND;
    }

    NVSEncryptedPartition *partition = new (std::nothrow) NVSEncryptedPartition(label, bdl);
    if (partition == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    esp_err_t result = partition->init(cfg);
    if (result != ESP_OK) {
        delete partition;
        return result;
    }
    *p = partition;
    return ESP_OK;
#endif
}
#endif // !LINUX_TARGET

} // namespace partition_lookup
} // namespace nvs
