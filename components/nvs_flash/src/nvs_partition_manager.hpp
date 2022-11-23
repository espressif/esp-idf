/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef NVS_PARTITION_MANAGER_HPP_
#define NVS_PARTITION_MANAGER_HPP_

#include "nvs_handle_simple.hpp"
#include "nvs_storage.hpp"
#include "nvs_memory_management.hpp"

#ifdef CONFIG_NVS_ENCRYPTION
#include "nvs_encr.hpp"
#endif

namespace nvs {

class NVSPartitionManager : public ExceptionlessAllocatable {
public:
    virtual ~NVSPartitionManager() { }

    static NVSPartitionManager* get_instance();

    esp_err_t init_partition(const char *partition_label);

    esp_err_t init_custom(const char *partName, uint32_t baseSector, uint32_t sectorCount);

#ifdef CONFIG_NVS_ENCRYPTION
    esp_err_t secure_init_partition(const char *part_name, nvs_sec_cfg_t* cfg);

    esp_err_t secure_init_custom(const char *partName, uint32_t baseSector, uint32_t sectorCount, nvs_sec_cfg_t* cfg);
#endif

    esp_err_t deinit_partition(const char *partition_label);

    Storage* lookup_storage_from_name(const char* name);

    esp_err_t open_handle(const char *part_name, const char *ns_name, nvs_open_mode_t open_mode, NVSHandleSimple** handle);

    esp_err_t close_handle(NVSHandleSimple* handle);

    size_t open_handles_size();

protected:
    NVSPartitionManager() { }

    static NVSPartitionManager* instance;

    intrusive_list<NVSHandleSimple> nvs_handles;

    intrusive_list<nvs::Storage> nvs_storage_list;
};

} // nvs

#endif // NVS_PARTITION_MANAGER_HPP_

