/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "nvs_handle_simple.hpp"

namespace nvs {

/**
 * @brief A class which behaves the same as NVSHandleSimple, except that all public member functions are locked.
 *
 * This class follows the decorator design pattern. The reason why we don't want locks in NVSHandleSimple is that
 * NVSHandleSimple can also be used by the C-API which locks its public functions already.
 * Thus, we avoid double-locking.
 *
 * @note this class becomes responsible for its internal NVSHandleSimple object, i.e. it deletes the handle object on
 * destruction
 */
class NVSHandleLocked : public NVSHandle {
public:
    /**
     * @param handle The NVSHandleSimple instance which this class "decorates" (see Decorator design pattern).
     *
     * @note Lock::init() MUST be called BEFORE an instance of this class is used.
     */
    NVSHandleLocked(NVSHandleSimple *handle);

    virtual ~NVSHandleLocked();

    esp_err_t set_string(const char *key, const char* str) override;

    esp_err_t set_blob(const char *key, const void* blob, size_t len) override;

    esp_err_t get_string(const char *key, char* out_str, size_t len) override;

    esp_err_t get_blob(const char *key, void* out_blob, size_t len) override;

    esp_err_t get_item_size(ItemType datatype, const char *key, size_t &size) override;

    esp_err_t find_key(const char* key, nvs_type_t &nvstype) override;

    esp_err_t erase_item(const char* key) override;

    esp_err_t erase_all() override;

    esp_err_t purge_all() override;

    esp_err_t commit() override;

    esp_err_t get_used_entry_count(size_t& usedEntries) override;

protected:
    esp_err_t set_typed_item(ItemType datatype, const char *key, const void* data, size_t dataSize) override;

    esp_err_t get_typed_item(ItemType datatype, const char *key, void* data, size_t dataSize) override;

private:
    NVSHandleSimple *handle;
};

} // namespace nvs
