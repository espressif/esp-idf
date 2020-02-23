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
#ifndef NVS_HANDLE_SIMPLE_HPP_
#define NVS_HANDLE_SIMPLE_HPP_

#include "intrusive_list.h"
#include "nvs_storage.hpp"
#include "nvs_platform.hpp"

#include "nvs_handle.hpp"

namespace nvs {

/**
 * @brief This class implements NVSHandle according to the ESP32's flash and partitioning scheme.
 *
 * It is used by both the C API and the C++ API. The main responsibility is to check whether the handle is valid
 * and in the right read/write mode and then forward the calls to the storage object.
 *
 * For more details about the general member functions, see nvs_handle.hpp.
 */
class NVSHandleSimple : public intrusive_list_node<NVSHandleSimple>, public NVSHandle {
    friend class NVSPartitionManager;
public:
    NVSHandleSimple(bool readOnly, uint8_t nsIndex, Storage *StoragePtr) :
        mStoragePtr(StoragePtr),
        mNsIndex(nsIndex),
        mReadOnly(readOnly),
        valid(1)
    { }

    ~NVSHandleSimple();

    esp_err_t set_typed_item(ItemType datatype, const char *key, const void *data, size_t dataSize) override;

    esp_err_t get_typed_item(ItemType datatype, const char *key, void *data, size_t dataSize) override;

    esp_err_t set_string(const char *key, const char *str) override;

    esp_err_t set_blob(const char *key, const void *blob, size_t len) override;

    esp_err_t get_string(const char *key, char *out_str, size_t len) override;

    esp_err_t get_blob(const char *key, void *out_blob, size_t len) override;

    esp_err_t get_item_size(ItemType datatype, const char *key, size_t &size) override;

    esp_err_t erase_item(const char *key) override;

    esp_err_t erase_all() override;

    esp_err_t commit() override;

    esp_err_t get_used_entry_count(size_t &usedEntries) override;

    esp_err_t getItemDataSize(ItemType datatype, const char *key, size_t &dataSize);

    void debugDump();

    esp_err_t fillStats(nvs_stats_t &nvsStats);

    esp_err_t calcEntriesInNamespace(size_t &usedEntries);

    bool findEntry(nvs_opaque_iterator_t *it, const char *name);

    bool nextEntry(nvs_opaque_iterator_t *it);

private:
    /**
     * The underlying storage's object.
     */
    Storage *mStoragePtr;

    /**
     * Numeric representation of the namespace as it is saved in flash (see README.rst for further details).
     */
    uint8_t mNsIndex;

    /**
     * Whether this handle is marked as read-only or read-write.
     * 0 indicates read-only, any other value read-write.
     */
    uint8_t mReadOnly;

    /**
     * Indicates the validity of this handle.
     * Upon opening, a handle is valid. It becomes invalid if the underlying storage is de-initialized.
     */
    uint8_t valid;
};

} // nvs

#endif // NVS_HANDLE_SIMPLE_HPP_

