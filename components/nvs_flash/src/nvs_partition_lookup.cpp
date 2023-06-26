#include "esp_partition.h"
#include "nvs_partition_lookup.hpp"

#ifndef LINUX_TARGET
#include "nvs_encrypted_partition.hpp"
#endif // ! LINUX_TARGET

namespace nvs {

namespace partition_lookup {

esp_err_t lookup_nvs_partition(const char* label, NVSPartition **p)
{
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
}

#ifndef LINUX_TARGET
esp_err_t lookup_nvs_encrypted_partition(const char* label, nvs_sec_cfg_t* cfg, NVSPartition **p)
{
    const esp_partition_t* esp_partition = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, label);

    if (esp_partition == nullptr) {
        return ESP_ERR_NOT_FOUND;
    }

    if (esp_partition->encrypted) {
        return ESP_ERR_NVS_WRONG_ENCRYPTION;
    }

    NVSEncryptedPartition *enc_p = new (std::nothrow) NVSEncryptedPartition(esp_partition);
    if (enc_p == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    esp_err_t result = enc_p->init(cfg);
    if (result != ESP_OK) {
        delete enc_p;
        return result;
    }

    *p = enc_p;

    return ESP_OK;
}
#endif // ! LINUX_TARGET

} // partition_lookup

} // nvs
