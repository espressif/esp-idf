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
#ifndef NVS_PARTITION_MANAGER_HPP_
#define NVS_PARTITION_MANAGER_HPP_

#include "nvs_handle_simple.hpp"
#include "nvs_storage.hpp"
#include "nvs_partition.hpp"
#include "nvs_flash.h"

namespace nvs {

class NVSPartitionManager {
public:
    virtual ~NVSPartitionManager() { }

    static NVSPartitionManager* get_instance();

    esp_err_t init_partition(const char *partition_label);

    esp_err_t init_custom(Partition *partition, uint32_t baseSector, uint32_t sectorCount);

#ifdef CONFIG_NVS_ENCRYPTION
    esp_err_t secure_init_partition(const char *part_name, nvs_sec_cfg_t* cfg);
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

    intrusive_list<nvs::NVSPartition> nvs_partition_list;
};

} // nvs

#endif // NVS_PARTITION_MANAGER_HPP_
