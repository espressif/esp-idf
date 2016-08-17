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
#include "nvs.hpp"
#include "nvs_flash.h"
#include "nvs_storage.hpp"
#include "intrusive_list.h"
#include "nvs_platform.hpp"

class HandleEntry : public intrusive_list_node<HandleEntry>
{
public:
    HandleEntry(){}
    
    HandleEntry(nvs_handle handle, bool readOnly, uint8_t nsIndex) :
    mHandle(handle),
    mReadOnly(readOnly),
    mNsIndex(nsIndex)
    {
    }
    
    nvs_handle mHandle;
    uint8_t mReadOnly;
    uint8_t mNsIndex;
};

#ifdef ESP_PLATFORM
SemaphoreHandle_t nvs::Lock::mSemaphore = NULL;
#endif

using namespace std;
using namespace nvs;

static intrusive_list<HandleEntry> s_nvs_handles;
static uint32_t s_nvs_next_handle = 1;
static nvs::Storage s_nvs_storage;

extern "C" esp_err_t nvs_flash_init(uint32_t baseSector, uint32_t sectorCount)
{
    Lock::init();
    Lock lock;
    NVS_DEBUGV("%s %d %d\r\n", __func__, baseSector, sectorCount);
    return s_nvs_storage.init(baseSector, sectorCount);
}

static esp_err_t nvs_find_ns_handle(nvs_handle handle, HandleEntry& entry)
{
    auto it = find_if(begin(s_nvs_handles), end(s_nvs_handles), [=](HandleEntry& e) -> bool {
        return e.mHandle == handle;
    });
    if (it == end(s_nvs_handles)) {
        return ESP_ERR_NVS_INVALID_HANDLE;
    }
    entry = *it;
    return ESP_OK;
}

extern "C" esp_err_t nvs_open(const char* name, nvs_open_mode open_mode, nvs_handle *out_handle)
{
    Lock lock;
    NVS_DEBUGV("%s %s %d\r\n", __func__, name, open_mode);
    uint8_t nsIndex;
    esp_err_t err = s_nvs_storage.createOrOpenNamespace(name, open_mode == NVS_READWRITE, nsIndex);
    if (err != ESP_OK) {
        return err;
    }

    uint32_t handle = s_nvs_next_handle;
    ++s_nvs_next_handle;
    *out_handle = handle;

    s_nvs_handles.push_back(new HandleEntry(handle, open_mode==NVS_READONLY, nsIndex));
    return ESP_OK;
}

extern "C" void nvs_close(nvs_handle handle)
{
    Lock lock;
    NVS_DEBUGV("%s %d\r\n", __func__, handle);
    auto it = find_if(begin(s_nvs_handles), end(s_nvs_handles), [=](HandleEntry& e) -> bool {
        return e.mHandle == handle;
    });
    if (it == end(s_nvs_handles)) {
        return;
    }
    s_nvs_handles.erase(it);
}

template<typename T>
static esp_err_t nvs_set(nvs_handle handle, const char* key, T value)
{
    Lock lock;
    NVS_DEBUGV("%s %s %d %d\r\n", __func__, key, sizeof(T), (uint32_t) value);
    HandleEntry entry;
    auto err = nvs_find_ns_handle(handle, entry);
    if (err != ESP_OK) {
        return err;
    }
    if (entry.mReadOnly) {
        return ESP_ERR_NVS_READ_ONLY;
    }
    return s_nvs_storage.writeItem(entry.mNsIndex, key, value);
}

extern "C" esp_err_t nvs_set_i8  (nvs_handle handle, const char* key, int8_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_set_u8  (nvs_handle handle, const char* key, uint8_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_set_i16 (nvs_handle handle, const char* key, int16_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_set_u16 (nvs_handle handle, const char* key, uint16_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_set_i32 (nvs_handle handle, const char* key, int32_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_set_u32 (nvs_handle handle, const char* key, uint32_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_set_i64 (nvs_handle handle, const char* key, int64_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_set_u64 (nvs_handle handle, const char* key, uint64_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_commit(nvs_handle handle)
{
    Lock lock;
    // no-op for now, to be used when intermediate cache is added
    HandleEntry entry;
    return nvs_find_ns_handle(handle, entry);
}

extern "C" esp_err_t nvs_set_str(nvs_handle handle, const char* key, const char* value)
{
    Lock lock;
    NVS_DEBUGV("%s %s %s\r\n", __func__, key, value);
    HandleEntry entry;
    auto err = nvs_find_ns_handle(handle, entry);
    if (err != ESP_OK) {
        return err;
    }
    return s_nvs_storage.writeItem(entry.mNsIndex, nvs::ItemType::SZ, key, value, strlen(value) + 1);
}

extern "C" esp_err_t nvs_set_blob(nvs_handle handle, const char* key, const void* value, size_t length)
{
    Lock lock;
    NVS_DEBUGV("%s %s %d\r\n", __func__, key, length);
    HandleEntry entry;
    auto err = nvs_find_ns_handle(handle, entry);
    if (err != ESP_OK) {
        return err;
    }
    return s_nvs_storage.writeItem(entry.mNsIndex, nvs::ItemType::BLOB, key, value, length);
}


template<typename T>
static esp_err_t nvs_get(nvs_handle handle, const char* key, T* out_value)
{
    Lock lock;
    NVS_DEBUGV("%s %s %d\r\n", __func__, key, sizeof(T));
    HandleEntry entry;
    auto err = nvs_find_ns_handle(handle, entry);
    if (err != ESP_OK) {
        return err;
    }
    return s_nvs_storage.readItem(entry.mNsIndex, key, *out_value);
}

extern "C" esp_err_t nvs_get_i8  (nvs_handle handle, const char* key, int8_t* out_value)
{
    return nvs_get(handle, key, out_value);
}

extern "C" esp_err_t nvs_get_u8  (nvs_handle handle, const char* key, uint8_t* out_value)
{
    return nvs_get(handle, key, out_value);
}

extern "C" esp_err_t nvs_get_i16 (nvs_handle handle, const char* key, int16_t* out_value)
{
    return nvs_get(handle, key, out_value);
}

extern "C" esp_err_t nvs_get_u16 (nvs_handle handle, const char* key, uint16_t* out_value)
{
    return nvs_get(handle, key, out_value);
}

extern "C" esp_err_t nvs_get_i32 (nvs_handle handle, const char* key, int32_t* out_value)
{
    return nvs_get(handle, key, out_value);
}

extern "C" esp_err_t nvs_get_u32 (nvs_handle handle, const char* key, uint32_t* out_value)
{
    return nvs_get(handle, key, out_value);
}

extern "C" esp_err_t nvs_get_i64 (nvs_handle handle, const char* key, int64_t* out_value)
{
    return nvs_get(handle, key, out_value);
}

extern "C" esp_err_t nvs_get_u64 (nvs_handle handle, const char* key, uint64_t* out_value)
{
    return nvs_get(handle, key, out_value);
}

static esp_err_t nvs_get_str_or_blob(nvs_handle handle, nvs::ItemType type, const char* key, void* out_value, size_t* length)
{
    Lock lock;
    NVS_DEBUGV("%s %s\r\n", __func__, key);
    HandleEntry entry;
    auto err = nvs_find_ns_handle(handle, entry);
    if (err != ESP_OK) {
        return err;
    }

    size_t dataSize;
    err = s_nvs_storage.getItemDataSize(entry.mNsIndex, type, key, dataSize);
    if (err != ESP_OK) {
        return err;
    }

    if (length != nullptr && out_value == nullptr) {
        *length = dataSize;
        return ESP_OK;
    }

    if (length == nullptr || *length < dataSize) {
        return ESP_ERR_NVS_INVALID_LENGTH;
    }

    return s_nvs_storage.readItem(entry.mNsIndex, type, key, out_value, dataSize);
}

extern "C" esp_err_t nvs_get_str(nvs_handle handle, const char* key, char* out_value, size_t* length)
{
    return nvs_get_str_or_blob(handle, nvs::ItemType::SZ, key, out_value, length);
}

extern "C" esp_err_t nvs_get_blob(nvs_handle handle, const char* key, void* out_value, size_t* length)
{
    return nvs_get_str_or_blob(handle, nvs::ItemType::BLOB, key, out_value, length);
}

