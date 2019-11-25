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
#include "nvs_handle_locked.hpp"

namespace nvs {

NVSHandleLocked::NVSHandleLocked(NVSHandleSimple *handle) : handle(handle) {
    Lock::init();
}

NVSHandleLocked::~NVSHandleLocked() {
    Lock lock;
    delete handle;
}

esp_err_t NVSHandleLocked::set_string(const char *key, const char* str) {
    Lock lock;
    return handle->set_string(key, str);
}

esp_err_t NVSHandleLocked::set_blob(const char *key, const void* blob, size_t len) {
    Lock lock;
    return handle->set_blob(key, blob, len);
}

esp_err_t NVSHandleLocked::get_string(const char *key, char* out_str, size_t len) {
    Lock lock;
    return handle->get_string(key, out_str, len);
}

esp_err_t NVSHandleLocked::get_blob(const char *key, void* out_blob, size_t len) {
    Lock lock;
    return handle->get_blob(key, out_blob, len);
}

esp_err_t NVSHandleLocked::get_item_size(ItemType datatype, const char *key, size_t &size) {
    Lock lock;
    return handle->get_item_size(datatype, key, size);
}

esp_err_t NVSHandleLocked::erase_item(const char* key) {
    Lock lock;
    return handle->erase_item(key);
}

esp_err_t NVSHandleLocked::erase_all() {
    Lock lock;
    return handle->erase_all();
}

esp_err_t NVSHandleLocked::commit() {
    Lock lock;
    return handle->commit();
}

esp_err_t NVSHandleLocked::get_used_entry_count(size_t& usedEntries) {
    Lock lock;
    return handle->get_used_entry_count(usedEntries);
}

esp_err_t NVSHandleLocked::set_typed_item(ItemType datatype, const char *key, const void* data, size_t dataSize) {
    Lock lock;
    return handle->set_typed_item(datatype, key, data, dataSize);
}

esp_err_t NVSHandleLocked::get_typed_item(ItemType datatype, const char *key, void* data, size_t dataSize) {
    Lock lock;
    return handle->get_typed_item(datatype, key, data, dataSize);
}

} // namespace nvs

