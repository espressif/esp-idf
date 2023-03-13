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
#include "nvs_partition_manager.hpp"
#include "esp_partition.h"
#include "sdkconfig.h"
#include <functional>
#include "nvs_handle_simple.hpp"
#ifdef CONFIG_NVS_ENCRYPTION
#include "nvs_encr.hpp"
#endif
#include "nvs_memory_management.hpp"
#include "esp_err.h"

#ifdef ESP_PLATFORM
#include <esp32/rom/crc.h>

// Uncomment this line to force output from this module
// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
static const char* TAG = "nvs";
#else
#include "crc.h"
#define ESP_LOGD(...)
#endif

class NVSHandleEntry : public intrusive_list_node<NVSHandleEntry>, public ExceptionlessAllocatable {
public:
    NVSHandleEntry(nvs::NVSHandleSimple *handle, const char* part_name)
        : nvs_handle(handle),
        mHandle(++s_nvs_next_handle),
        handle_part_name(part_name) { }

    ~NVSHandleEntry() {
        delete nvs_handle;
    }

    nvs::NVSHandleSimple *nvs_handle;
    nvs_handle_t mHandle;
    const char* handle_part_name;
private:
    static uint32_t s_nvs_next_handle;
};

uint32_t NVSHandleEntry::s_nvs_next_handle;

extern "C" void nvs_dump(const char *partName);
extern "C" esp_err_t nvs_flash_init_custom(const char *partName, uint32_t baseSector, uint32_t sectorCount);

#ifdef CONFIG_NVS_ENCRYPTION
extern "C" esp_err_t nvs_flash_secure_init_custom(const char *partName, uint32_t baseSector, uint32_t sectorCount, nvs_sec_cfg_t* cfg);
#endif

#ifdef ESP_PLATFORM
SemaphoreHandle_t nvs::Lock::mSemaphore = NULL;
#endif

using namespace std;
using namespace nvs;

static intrusive_list<NVSHandleEntry> s_nvs_handles;

static nvs::Storage* lookup_storage_from_name(const char *name)
{
    return NVSPartitionManager::get_instance()->lookup_storage_from_name(name);
}

extern "C" void nvs_dump(const char *partName)
{
    Lock lock;
    nvs::Storage* pStorage;

    pStorage = lookup_storage_from_name(partName);
    if (pStorage == NULL) {
        return;
    }

    pStorage->debugDump();
    return;
}

extern "C" esp_err_t nvs_flash_init_custom(const char *partName, uint32_t baseSector, uint32_t sectorCount)
{
    ESP_LOGD(TAG, "nvs_flash_init_custom partition=%s start=%d count=%d", partName, baseSector, sectorCount);

    return NVSPartitionManager::get_instance()->init_custom(partName, baseSector, sectorCount);
}

#ifdef CONFIG_NVS_ENCRYPTION
extern "C" esp_err_t nvs_flash_secure_init_custom(const char *partName, uint32_t baseSector, uint32_t sectorCount, nvs_sec_cfg_t* cfg)
{
    ESP_LOGD(TAG, "nvs_flash_secure_init_custom partition=%s start=%d count=%d", partName, baseSector, sectorCount);

    if(cfg) {
        auto encrMgr = EncrMgr::getInstance();

        if (!encrMgr) return ESP_ERR_NO_MEM;

        auto err = encrMgr->setSecurityContext(baseSector, sectorCount, cfg);
        if(err != ESP_OK) {
            return err;
        }
    }

    return nvs_flash_init_custom(partName, baseSector, sectorCount);
}
#endif

static esp_err_t close_handles_and_deinit(const char* part_name)
{
    auto belongs_to_part = [=](NVSHandleEntry& e) -> bool {
        return strncmp(e.nvs_handle->get_partition_name(), part_name, NVS_PART_NAME_MAX_SIZE) == 0;
    };

    auto it = find_if(begin(s_nvs_handles), end(s_nvs_handles), belongs_to_part);

    while (it != end(s_nvs_handles)) {
        s_nvs_handles.erase(it);
        it = find_if(begin(s_nvs_handles), end(s_nvs_handles), belongs_to_part);
    }

    // Deinit partition
    return NVSPartitionManager::get_instance()->deinit_partition(part_name);
}

extern "C" esp_err_t nvs_flash_init_partition_ptr(const esp_partition_t *partition)
{
    Lock::init();
    Lock lock;

    if (!partition) {
        return ESP_ERR_INVALID_ARG;
    }

    if (partition->flash_chip != esp_flash_default_chip) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    return nvs_flash_init_custom(partition->label,
                                 partition->address / SPI_FLASH_SEC_SIZE,
                                 partition->size / SPI_FLASH_SEC_SIZE);
}

#ifdef ESP_PLATFORM
extern "C" esp_err_t nvs_flash_init_partition(const char *part_name)
{
    Lock::init();
    Lock lock;

    return NVSPartitionManager::get_instance()->init_partition(part_name);
}

extern "C" esp_err_t nvs_flash_init(void)
{
    return nvs_flash_init_partition(NVS_DEFAULT_PART_NAME);
}

#ifdef CONFIG_NVS_ENCRYPTION
extern "C" esp_err_t nvs_flash_secure_init_partition(const char *part_name, nvs_sec_cfg_t* cfg)
{
    Lock::init();
    Lock lock;
    nvs::Storage* mStorage;

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

    return nvs_flash_secure_init_custom(part_name, partition->address / SPI_FLASH_SEC_SIZE,
            partition->size / SPI_FLASH_SEC_SIZE, cfg);
}

extern "C" esp_err_t nvs_flash_secure_init(nvs_sec_cfg_t* cfg)
{
    return nvs_flash_secure_init_partition(NVS_DEFAULT_PART_NAME, cfg);
}
#endif

extern "C" esp_err_t nvs_flash_erase_partition(const char *part_name)
{
    Lock::init();
    Lock lock;

    // if the partition is initialized, uninitialize it first
    if (NVSPartitionManager::get_instance()->lookup_storage_from_name(part_name)) {
        esp_err_t err = close_handles_and_deinit(part_name);

        // only hypothetical/future case, deinit_partition() only fails if partition is uninitialized
        if (err != ESP_OK) {
            return err;
        }
    }

    const esp_partition_t* partition = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, part_name);
    if (partition == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    if (partition->flash_chip != esp_flash_default_chip) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    return esp_partition_erase_range(partition, 0, partition->size);
}

extern "C" esp_err_t nvs_flash_erase_partition_ptr(const esp_partition_t *partition)
{
    Lock::init();
    Lock lock;

    if (!partition) {
        return ESP_ERR_INVALID_ARG;
    }

    if (partition->flash_chip != esp_flash_default_chip) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    // if the partition is initialized, uninitialize it first
    if (NVSPartitionManager::get_instance()->lookup_storage_from_name(partition->label)) {
        const esp_err_t err = close_handles_and_deinit(partition->label);

        // only hypothetical/future case, deinit_partition() only fails if partition is uninitialized
        if (err != ESP_OK) {
            return err;
        }
    }

    return esp_partition_erase_range(partition, 0, partition->size);
}

extern "C" esp_err_t nvs_flash_erase(void)
{
    return nvs_flash_erase_partition(NVS_DEFAULT_PART_NAME);
}
#endif // ESP_PLATFORM

extern "C" esp_err_t nvs_flash_deinit_partition(const char* partition_name)
{
    Lock::init();
    Lock lock;

    return close_handles_and_deinit(partition_name);
}

extern "C" esp_err_t nvs_flash_deinit(void)
{
    return nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME);
}

static esp_err_t nvs_find_ns_handle(nvs_handle_t c_handle, NVSHandleSimple** handle)
{
    auto it = find_if(begin(s_nvs_handles), end(s_nvs_handles), [=](NVSHandleEntry& e) -> bool {
        return e.mHandle == c_handle;
    });
    if (it == end(s_nvs_handles)) {
        return ESP_ERR_NVS_INVALID_HANDLE;
    }
    *handle = it->nvs_handle;
    return ESP_OK;
}

extern "C" esp_err_t nvs_open_from_partition(const char *part_name, const char* name, nvs_open_mode_t open_mode, nvs_handle_t *out_handle)
{
    Lock lock;
    ESP_LOGD(TAG, "%s %s %d", __func__, name, open_mode);

    NVSHandleSimple *handle;
    esp_err_t result = NVSPartitionManager::get_instance()->open_handle(part_name, name, open_mode, &handle);
    if (result == ESP_OK) {
        NVSHandleEntry *entry = new (std::nothrow) NVSHandleEntry(handle, part_name);
        if (entry) {
            s_nvs_handles.push_back(entry);
            *out_handle = entry->mHandle;
        } else {
            delete handle;
            return ESP_ERR_NO_MEM;
        }
    }

    return result;
}

extern "C" esp_err_t nvs_open(const char* name, nvs_open_mode_t open_mode, nvs_handle_t *out_handle)
{
    return nvs_open_from_partition(NVS_DEFAULT_PART_NAME, name, open_mode, out_handle);
}

extern "C" void nvs_close(nvs_handle_t handle)
{
    Lock lock;
    ESP_LOGD(TAG, "%s %d", __func__, handle);
    auto it = find_if(begin(s_nvs_handles), end(s_nvs_handles), [=](NVSHandleEntry& e) -> bool {
        return e.mHandle == handle;
    });
    if (it == end(s_nvs_handles)) {
        return;
    }
    s_nvs_handles.erase(it);
    delete static_cast<NVSHandleEntry*>(it);
}

extern "C" esp_err_t nvs_erase_key(nvs_handle_t c_handle, const char* key)
{
    Lock lock;
    ESP_LOGD(TAG, "%s %s\r\n", __func__, key);
    NVSHandleSimple *handle;
    auto err = nvs_find_ns_handle(c_handle, &handle);
    if (err != ESP_OK) {
        return err;
    }

    return handle->erase_item(key);
}

extern "C" esp_err_t nvs_erase_all(nvs_handle_t c_handle)
{
    Lock lock;
    ESP_LOGD(TAG, "%s\r\n", __func__);
    NVSHandleSimple *handle;
    auto err = nvs_find_ns_handle(c_handle, &handle);
    if (err != ESP_OK) {
        return err;
    }

    return handle->erase_all();
}

template<typename T>
static esp_err_t nvs_set(nvs_handle_t c_handle, const char* key, T value)
{
    Lock lock;
    ESP_LOGD(TAG, "%s %s %d %d", __func__, key, sizeof(T), (uint32_t) value);
    NVSHandleSimple *handle;
    auto err = nvs_find_ns_handle(c_handle, &handle);
    if (err != ESP_OK) {
        return err;
    }

    return handle->set_item(key, value);
}

extern "C" esp_err_t nvs_set_i8  (nvs_handle_t handle, const char* key, int8_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_set_u8  (nvs_handle_t handle, const char* key, uint8_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_set_i16 (nvs_handle_t handle, const char* key, int16_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_set_u16 (nvs_handle_t handle, const char* key, uint16_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_set_i32 (nvs_handle_t handle, const char* key, int32_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_set_u32 (nvs_handle_t handle, const char* key, uint32_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_set_i64 (nvs_handle_t handle, const char* key, int64_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_set_u64 (nvs_handle_t handle, const char* key, uint64_t value)
{
    return nvs_set(handle, key, value);
}

extern "C" esp_err_t nvs_commit(nvs_handle_t c_handle)
{
    Lock lock;
    // no-op for now, to be used when intermediate cache is added
    NVSHandleSimple *handle;
    auto err = nvs_find_ns_handle(c_handle, &handle);
    if (err != ESP_OK) {
        return err;
    }
    return handle->commit();
}

extern "C" esp_err_t nvs_set_str(nvs_handle_t c_handle, const char* key, const char* value)
{
    Lock lock;
    ESP_LOGD(TAG, "%s %s %s", __func__, key, value);
    NVSHandleSimple *handle;
    auto err = nvs_find_ns_handle(c_handle, &handle);
    if (err != ESP_OK) {
        return err;
    }
    return handle->set_string(key, value);
}

extern "C" esp_err_t nvs_set_blob(nvs_handle_t c_handle, const char* key, const void* value, size_t length)
{
    Lock lock;
    ESP_LOGD(TAG, "%s %s %d", __func__, key, length);
    NVSHandleSimple *handle;
    auto err = nvs_find_ns_handle(c_handle, &handle);
    if (err != ESP_OK) {
        return err;
    }
    return handle->set_blob(key, value, length);
}


template<typename T>
static esp_err_t nvs_get(nvs_handle_t c_handle, const char* key, T* out_value)
{
    Lock lock;
    ESP_LOGD(TAG, "%s %s %d", __func__, key, sizeof(T));
    NVSHandleSimple *handle;
    auto err = nvs_find_ns_handle(c_handle, &handle);
    if (err != ESP_OK) {
        return err;
    }
    return handle->get_item(key, *out_value);
}

extern "C" esp_err_t nvs_get_i8  (nvs_handle_t c_handle, const char* key, int8_t* out_value)
{
    return nvs_get(c_handle, key, out_value);
}

extern "C" esp_err_t nvs_get_u8  (nvs_handle_t c_handle, const char* key, uint8_t* out_value)
{
    return nvs_get(c_handle, key, out_value);
}

extern "C" esp_err_t nvs_get_i16 (nvs_handle_t c_handle, const char* key, int16_t* out_value)
{
    return nvs_get(c_handle, key, out_value);
}

extern "C" esp_err_t nvs_get_u16 (nvs_handle_t c_handle, const char* key, uint16_t* out_value)
{
    return nvs_get(c_handle, key, out_value);
}

extern "C" esp_err_t nvs_get_i32 (nvs_handle_t c_handle, const char* key, int32_t* out_value)
{
    return nvs_get(c_handle, key, out_value);
}

extern "C" esp_err_t nvs_get_u32 (nvs_handle_t c_handle, const char* key, uint32_t* out_value)
{
    return nvs_get(c_handle, key, out_value);
}

extern "C" esp_err_t nvs_get_i64 (nvs_handle_t c_handle, const char* key, int64_t* out_value)
{
    return nvs_get(c_handle, key, out_value);
}

extern "C" esp_err_t nvs_get_u64 (nvs_handle_t c_handle, const char* key, uint64_t* out_value)
{
    return nvs_get(c_handle, key, out_value);
}

static esp_err_t nvs_get_str_or_blob(nvs_handle_t c_handle, nvs::ItemType type, const char* key, void* out_value, size_t* length)
{
    Lock lock;
    ESP_LOGD(TAG, "%s %s", __func__, key);
    NVSHandleSimple *handle;
    auto err = nvs_find_ns_handle(c_handle, &handle);
    if (err != ESP_OK) {
        return err;
    }

    size_t dataSize;
    err = handle->get_item_size(type, key, dataSize);
    if (err != ESP_OK) {
        return err;
    }

    if (length == nullptr) {
        return ESP_ERR_NVS_INVALID_LENGTH;
    } else if (out_value == nullptr) {
        *length = dataSize;
        return ESP_OK;
    } else if (*length < dataSize) {
        *length = dataSize;
        return ESP_ERR_NVS_INVALID_LENGTH;
    }

    *length = dataSize;
    return handle->get_typed_item(type, key, out_value, dataSize);
}

extern "C" esp_err_t nvs_get_str(nvs_handle_t c_handle, const char* key, char* out_value, size_t* length)
{
    return nvs_get_str_or_blob(c_handle, nvs::ItemType::SZ, key, out_value, length);
}

extern "C" esp_err_t nvs_get_blob(nvs_handle_t c_handle, const char* key, void* out_value, size_t* length)
{
    return nvs_get_str_or_blob(c_handle, nvs::ItemType::BLOB, key, out_value, length);
}

extern "C" esp_err_t nvs_get_stats(const char* part_name, nvs_stats_t* nvs_stats)
{
    Lock lock;
    nvs::Storage* pStorage;

    if (nvs_stats == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    nvs_stats->used_entries     = 0;
    nvs_stats->free_entries     = 0;
    nvs_stats->total_entries    = 0;
    nvs_stats->namespace_count  = 0;

    pStorage = lookup_storage_from_name((part_name == NULL) ? NVS_DEFAULT_PART_NAME : part_name);
    if (pStorage == NULL) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    if(!pStorage->isValid()){
        return ESP_ERR_NVS_INVALID_STATE;
    }

    return pStorage->fillStats(*nvs_stats);
}

extern "C" esp_err_t nvs_get_used_entry_count(nvs_handle_t c_handle, size_t* used_entries)
{
    Lock lock;
    if(used_entries == NULL){
        return ESP_ERR_INVALID_ARG;
    }
    *used_entries = 0;

    NVSHandleSimple *handle;
    auto err = nvs_find_ns_handle(c_handle, &handle);
    if (err != ESP_OK) {
        return err;
    }

    size_t used_entry_count;
    err = handle->get_used_entry_count(used_entry_count);
    if(err == ESP_OK){
        *used_entries = used_entry_count;
    }
    return err;
}

#if (defined CONFIG_NVS_ENCRYPTION) && (defined ESP_PLATFORM)

extern "C" esp_err_t nvs_flash_generate_keys(const esp_partition_t* partition, nvs_sec_cfg_t* cfg)
{
    if (partition->flash_chip != esp_flash_default_chip) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    auto err = esp_partition_erase_range(partition, 0, partition->size);
    if(err != ESP_OK) {
        return err;
    }

    for(uint8_t cnt = 0; cnt < NVS_KEY_SIZE; cnt++) {
        cfg->eky[cnt] = 0xff;
        cfg->tky[cnt] = 0xee;
    }

    err = spi_flash_write(partition->address, cfg->eky, NVS_KEY_SIZE);
    if(err != ESP_OK) {
        return err;
    }

    err = spi_flash_write(partition->address + NVS_KEY_SIZE, cfg->tky, NVS_KEY_SIZE);
    if(err != ESP_OK) {
        return err;
    }

    err = esp_partition_read(partition, 0, cfg->eky, NVS_KEY_SIZE);
    if(err != ESP_OK) {
        return err;
    }

    err = esp_partition_read(partition, NVS_KEY_SIZE, cfg->tky, NVS_KEY_SIZE);
    if(err != ESP_OK) {
        return err;
    }

    uint32_t crc_calc = crc32_le(0xffffffff, cfg->eky, NVS_KEY_SIZE);
    crc_calc = crc32_le(crc_calc, cfg->tky, NVS_KEY_SIZE);

    uint8_t crc_wr[16];
    memset(crc_wr, 0xff, sizeof(crc_wr));
    memcpy(crc_wr, &crc_calc, 4);

    err = esp_partition_write(partition, 2 * NVS_KEY_SIZE, crc_wr, sizeof(crc_wr));
    if(err != ESP_OK) {
        return err;
    }

    return ESP_OK;

}

extern "C" esp_err_t nvs_flash_read_security_cfg(const esp_partition_t* partition, nvs_sec_cfg_t* cfg)
{
    if (partition->flash_chip != esp_flash_default_chip) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    uint8_t eky_raw[NVS_KEY_SIZE], tky_raw[NVS_KEY_SIZE];
    uint32_t crc_raw, crc_read, crc_calc;

    auto check_if_initialized = [](uint8_t* eky, uint8_t* tky, uint32_t crc) {
        uint8_t cnt = 0;
        while(cnt < NVS_KEY_SIZE && eky[cnt] == 0xff && tky[cnt] == 0xff) cnt++;

        if(cnt == NVS_KEY_SIZE && crc == 0xffffffff) {
            return false;
        }
        return true;
    };

    auto err = spi_flash_read(partition->address, eky_raw, NVS_KEY_SIZE);
    if(err != ESP_OK) {
        return err;
    }

    err = spi_flash_read(partition->address + NVS_KEY_SIZE, tky_raw, NVS_KEY_SIZE);
    if(err != ESP_OK) {
        return err;
    }

    err = spi_flash_read(partition->address + 2 * NVS_KEY_SIZE, &crc_raw, 4);
    if(err != ESP_OK) {
        return err;
    }

    if(!check_if_initialized(eky_raw, tky_raw, crc_raw)) {
        /* This is an uninitialized key partition*/
        return ESP_ERR_NVS_KEYS_NOT_INITIALIZED;
    }

    err = esp_partition_read(partition, 0, cfg->eky, NVS_KEY_SIZE);

    if(err != ESP_OK) {
        return err;
    }

    err = esp_partition_read(partition, NVS_KEY_SIZE, cfg->tky, NVS_KEY_SIZE);

    if(err != ESP_OK) {
        return err;
    }

    err = esp_partition_read(partition, 2 * NVS_KEY_SIZE, &crc_read, 4);

    if(err != ESP_OK) {
        return err;
    }

    crc_calc = crc32_le(0xffffffff, cfg->eky, NVS_KEY_SIZE);
    crc_calc = crc32_le(crc_calc, cfg->tky, NVS_KEY_SIZE);

    if(crc_calc != crc_read) {
        if(!check_if_initialized(cfg->eky, cfg->tky, crc_read)) {
            /* This is an uninitialized key partition*/
            return ESP_ERR_NVS_KEYS_NOT_INITIALIZED;
        }
        return ESP_ERR_NVS_CORRUPT_KEY_PART;
    }

    return ESP_OK;
}

#endif

static nvs_iterator_t create_iterator(nvs::Storage *storage, nvs_type_t type)
{
    nvs_iterator_t it = (nvs_iterator_t)calloc(1, sizeof(nvs_opaque_iterator_t));
    if (it == NULL) {
        return NULL;
    }

    it->storage = storage;
    it->type = type;

    return it;
}

extern "C" nvs_iterator_t nvs_entry_find(const char *part_name, const char *namespace_name, nvs_type_t type)
{
    Lock lock;
    nvs::Storage *pStorage;

    pStorage = lookup_storage_from_name(part_name);
    if (pStorage == NULL) {
        return NULL;
    }

    nvs_iterator_t it = create_iterator(pStorage, type);
    if (it == NULL) {
        return NULL;
    }

    bool entryFound = pStorage->findEntry(it, namespace_name);
    if (!entryFound) {
        free(it);
        return NULL;
    }

    return it;
}

extern "C" nvs_iterator_t nvs_entry_next(nvs_iterator_t it)
{
    Lock lock;
    assert(it);

    bool entryFound = it->storage->nextEntry(it);
    if (!entryFound) {
        free(it);
        return NULL;
    }

    return it;
}

extern "C" void nvs_entry_info(nvs_iterator_t it, nvs_entry_info_t *out_info)
{
    *out_info = it->entry_info;
}

extern "C" void nvs_release_iterator(nvs_iterator_t it)
{
    free(it);
}
