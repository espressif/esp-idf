/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "nvs_partition_manager.hpp"
#include "nvs_handle.hpp"
#include "nvs_handle_simple.hpp"
#include "nvs_handle_locked.hpp"
#include "nvs_platform.hpp"

using namespace std;

namespace nvs {

std::unique_ptr<NVSHandle> open_nvs_handle_from_partition(const char *partition_name,
        const char *ns_name,
        nvs_open_mode_t open_mode,
        esp_err_t *err)
{
    if (partition_name == nullptr || ns_name == nullptr) {
        if (err) {
            *err = ESP_ERR_INVALID_ARG;
        }
        return nullptr;
    }

    esp_err_t lock_result = Lock::init();
    if (lock_result != ESP_OK) {
        if (err != nullptr) {
            *err = lock_result;
        }
        return nullptr;
    }

    Lock lock;

    NVSHandleSimple *handle_simple;
    esp_err_t result = nvs::NVSPartitionManager::get_instance()->
            open_handle(partition_name, ns_name, open_mode, &handle_simple);

    if (err) {
        *err = result;
    }

    if (result != ESP_OK) {
        return nullptr;
    }

    NVSHandleLocked *locked_handle = new (nothrow) NVSHandleLocked(handle_simple);

    if (!locked_handle) {
        if (err) {
            *err = ESP_ERR_NO_MEM;
        }
        delete handle_simple;
        return nullptr;
    }

    return std::unique_ptr<NVSHandleLocked>(locked_handle);
}

std::unique_ptr<NVSHandle> open_nvs_handle(const char *ns_name,
        nvs_open_mode_t open_mode,
        esp_err_t *err)
{
    return open_nvs_handle_from_partition(NVS_DEFAULT_PART_NAME, ns_name, open_mode, err);
}

} // namespace nvs
