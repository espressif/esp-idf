/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_partition.h"
#include "nvs_partition_manager.hpp"
#include "nvs_partition_lookup.hpp"
#include "nvs_internal.h"

#ifndef LINUX_TARGET
#include "nvs_encrypted_partition.hpp"
#endif // ! LINUX_TARGET

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
    if (strlen(partition_label) > NVS_PART_NAME_MAX_SIZE) {
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t size;
    const uint32_t sec_size = esp_partition_get_main_flash_sector_size();
    Storage* mStorage;

    mStorage = lookup_storage_from_name(partition_label);
    if (mStorage) {
        return ESP_OK;
    }

    NVS_ASSERT_OR_RETURN(sec_size != 0, ESP_FAIL);

    NVSPartition *p = nullptr;
    esp_err_t result = partition_lookup::lookup_nvs_partition(partition_label, &p);

    if (result != ESP_OK) {
        goto error;
    }

    size = p->get_size();

    result = init_custom(p, 0, size / sec_size);
    if (result != ESP_OK) {
        goto error;
    }

    nvs_partition_list.push_back(p);

    return ESP_OK;

error:
    delete p;
    return result;
}
#endif // ESP_PLATFORM

esp_err_t NVSPartitionManager::init_custom(Partition *partition, uint32_t baseSector, uint32_t sectorCount)
{
    Storage* new_storage = nullptr;
    Storage* storage = lookup_storage_from_name(partition->get_partition_name());
    if (storage == nullptr) {
        new_storage = new (std::nothrow) Storage(partition);

        if (new_storage == nullptr) {
            return ESP_ERR_NO_MEM;
        }

        storage = new_storage;
    } else {
        // if storage was initialized already, we don't need partition and hence delete it
        for (auto it = nvs_partition_list.begin(); it != nvs_partition_list.end(); ++it) {
            if (partition == it) {
                nvs_partition_list.erase(it);
                delete partition;
                break;
            }
        }
    }

    esp_err_t err = storage->init(baseSector, sectorCount);
    if (new_storage != nullptr) {
        if (err == ESP_OK) {
            nvs_storage_list.push_back(new_storage);
        } else {
            delete new_storage;
        }
    }
    return err;
}

#ifdef ESP_PLATFORM
esp_err_t NVSPartitionManager::secure_init_partition(const char *part_name, nvs_sec_cfg_t* cfg)
{
    if (strlen(part_name) > NVS_PART_NAME_MAX_SIZE) {
        return ESP_ERR_INVALID_ARG;
    }

    Storage* mStorage;

    mStorage = lookup_storage_from_name(part_name);
    if (mStorage != nullptr) {
        return ESP_OK;
    }

    NVSPartition *p;
    esp_err_t result;
    if (cfg != nullptr) {
        result = partition_lookup::lookup_nvs_encrypted_partition(part_name, cfg, &p);
    } else {
        result = partition_lookup::lookup_nvs_partition(part_name, &p);
    }

    if (result != ESP_OK) {
        return result;
    }

    uint32_t size = p->get_size();
    const uint32_t sec_size = esp_partition_get_main_flash_sector_size();

    result = init_custom(p, 0, size / sec_size);
    if (result != ESP_OK) {
        delete p;
        return result;
    }

    nvs_partition_list.push_back(p);

    return ESP_OK;
}
#endif // ESP_PLATFORM

esp_err_t NVSPartitionManager::deinit_partition(const char *partition_label)
{
    Storage* storage = lookup_storage_from_name(partition_label);
    if (!storage) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    /* Clean up handles related to the storage being deinitialized */
    for (auto it = nvs_handles.begin(); it != nvs_handles.end(); ++it) {
        if (it->mStoragePtr == storage) {
            it->valid = false;
            nvs_handles.erase(it);
        }
    }

    /* Finally delete the storage and its partition */
    nvs_storage_list.erase(storage);
    delete storage;

    for (auto it = nvs_partition_list.begin(); it != nvs_partition_list.end(); ++it) {
        if (strcmp(it->get_partition_name(), partition_label) == 0) {
            NVSPartition *p = it;
            nvs_partition_list.erase(it);
            delete p;
            break;
        }
    }

    return ESP_OK;
}

esp_err_t NVSPartitionManager::open_handle(const char *part_name,
        const char *ns_name,
        nvs_open_mode_t open_mode,
        NVSHandleSimple** handle)
{
    uint8_t nsIndex;
    Storage* sHandle;

    if (handle == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (nvs_storage_list.empty()) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    sHandle = lookup_storage_from_name(part_name);
    if (sHandle == nullptr) {
        return ESP_ERR_NVS_PART_NOT_FOUND;
    }

    if (open_mode == NVS_READWRITE && const_cast<Partition*>(sHandle->getPart())->get_readonly()) {
        return ESP_ERR_NOT_ALLOWED;
    }


    esp_err_t err = sHandle->createOrOpenNamespace(ns_name, open_mode == NVS_READWRITE, nsIndex);
    if (err != ESP_OK) {
        return err;
    }

    NVSHandleSimple* new_handle = new (std::nothrow) NVSHandleSimple(open_mode==NVS_READONLY, nsIndex, sHandle);
    if (new_handle == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    nvs_handles.push_back(new_handle);

    *handle = new_handle;
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
        return nullptr;
    }
    return it;
}

} // nvs
