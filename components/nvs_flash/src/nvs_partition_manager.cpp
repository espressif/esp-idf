// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "esp_partition.h"
#include "nvs_partition_manager.hpp"

namespace nvs {

NVSPartitionManager* NVSPartitionManager::instance = nullptr;

NVSPartitionManager* NVSPartitionManager::get_instance()
{
    if (!instance) {
        instance = new (std::nothrow) NVSPartitionManager();
    }

    return instance;
}

#ifdef ESP_PLATFORM
esp_err_t NVSPartitionManager::init_partition(const char *partition_label)
{
    Storage* mStorage;

    mStorage = lookup_storage_from_name(partition_label);
    if (mStorage) {
        return ESP_OK;
    }

    assert(SPI_FLASH_SEC_SIZE != 0);

    const esp_partition_t* partition = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, partition_label);
    if (partition == nullptr) {
        return ESP_ERR_NOT_FOUND;
    }

    if (partition->flash_chip != esp_flash_default_chip) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    return init_custom(partition_label, partition->address / SPI_FLASH_SEC_SIZE,
            partition->size / SPI_FLASH_SEC_SIZE);
}
#endif // ESP_PLATFORM

esp_err_t NVSPartitionManager::init_custom(const char *partName, uint32_t baseSector, uint32_t sectorCount)
{
    if (strlen(partName) > NVS_PART_NAME_MAX_SIZE) return ESP_ERR_INVALID_ARG;

    Storage* new_storage = NULL;
    Storage* storage = lookup_storage_from_name(partName);
    if (storage == NULL) {
        new_storage = new (std::nothrow) Storage((const char *)partName);

        if (!new_storage) return ESP_ERR_NO_MEM;

        storage = new_storage;
    }

    esp_err_t err = storage->init(baseSector, sectorCount);
    if (new_storage != NULL) {
        if (err == ESP_OK) {
            nvs_storage_list.push_back(new_storage);
        } else {
            delete new_storage;
        }
    }
    return err;
}

#ifdef ESP_PLATFORM
#ifdef CONFIG_NVS_ENCRYPTION
esp_err_t NVSPartitionManager::secure_init_partition(const char *part_name, nvs_sec_cfg_t* cfg)
{
    Storage* mStorage;

    mStorage = lookup_storage_from_name(part_name);
    if (mStorage) {
        return ESP_OK;
    }

    const esp_partition_t* partition = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, part_name);
    if (partition == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    if (partition->flash_chip != esp_flash_default_chip) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    return secure_init_custom(part_name, partition->address / SPI_FLASH_SEC_SIZE,
            partition->size / SPI_FLASH_SEC_SIZE, cfg);
}

esp_err_t NVSPartitionManager::secure_init_custom(const char *partName, uint32_t baseSector, uint32_t sectorCount, nvs_sec_cfg_t* cfg)
{
    if(cfg) {
        auto encrMgr = EncrMgr::getInstance();
        auto err = encrMgr->setSecurityContext(baseSector, sectorCount, cfg);
        if(err != ESP_OK) {
            return err;
        }
    }

    return init_custom(partName, baseSector, sectorCount);
}
#endif // CONFIG_NVS_ENCRYPTION
#endif // ESP_PLATFORM

esp_err_t NVSPartitionManager::deinit_partition(const char *partition_label)
{
    Storage* storage = lookup_storage_from_name(partition_label);
    if (!storage) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

#ifdef CONFIG_NVS_ENCRYPTION
    if(EncrMgr::isEncrActive()) {
        auto encrMgr = EncrMgr::getInstance();
        encrMgr->removeSecurityContext(storage->getBaseSector());
    }
#endif

    /* Clean up handles related to the storage being deinitialized */
    for (auto it = nvs_handles.begin(); it != nvs_handles.end(); ++it) {
        if (it->mStoragePtr == storage) {
            it->valid = false;
            nvs_handles.erase(it);
        }
    }

    /* Finally delete the storage itself */
    nvs_storage_list.erase(storage);
    delete storage;

    return ESP_OK;
}

esp_err_t NVSPartitionManager::open_handle(const char *part_name,
        const char *ns_name,
        nvs_open_mode_t open_mode,
        NVSHandleSimple** handle)
{
    uint8_t nsIndex;
    Storage* sHandle;

    if (nvs_storage_list.size() == 0) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    sHandle = lookup_storage_from_name(part_name);
    if (sHandle == NULL) {
        return ESP_ERR_NVS_PART_NOT_FOUND;
    }

    esp_err_t err = sHandle->createOrOpenNamespace(ns_name, open_mode == NVS_READWRITE, nsIndex);
    if (err != ESP_OK) {
        return err;
    }

    *handle = new (std::nothrow) NVSHandleSimple(open_mode==NVS_READONLY, nsIndex, sHandle);

    if (!handle) return ESP_ERR_NO_MEM;

    nvs_handles.push_back(*handle);

    return ESP_OK;
}

esp_err_t NVSPartitionManager::close_handle(NVSHandleSimple* handle) {
    for (auto it = nvs_handles.begin(); it != nvs_handles.end(); ++it) {
        if (it == intrusive_list<NVSHandleSimple>::iterator(handle)) {
            nvs_handles.erase(it);
            return ESP_OK;
        }
    }

    return ESP_ERR_NVS_INVALID_HANDLE;
}

size_t NVSPartitionManager::open_handles_size()
{
    return nvs_handles.size();
}

Storage* NVSPartitionManager::lookup_storage_from_name(const char* name)
{
    auto it = find_if(begin(nvs_storage_list), end(nvs_storage_list), [=](Storage& e) -> bool {
        return (strcmp(e.getPartName(), name) == 0);
    });

    if (it == end(nvs_storage_list)) {
        return NULL;
    }
    return it;
}

} // nvs

