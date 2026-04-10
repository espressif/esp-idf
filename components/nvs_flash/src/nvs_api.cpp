/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "nvs.hpp"
#include "nvs_flash.h"
#include "nvs_storage.hpp"
#include "intrusive_list.h"
#include "nvs_platform.hpp"
#include "nvs_partition_manager.hpp"
#include "esp_partition.h"
#include <functional>
#include "nvs_handle_simple.hpp"
#include "nvs_memory_management.hpp"
#include "esp_err.h"
#include <esp_rom_crc.h>
#include "nvs_internal.h"
#include "nvs_partition_lookup.hpp"

// Uncomment this line to force output from this module
// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
static const char* TAG = "nvs";

/**
 * @brief  Configuration structure for the active default security scheme
 *         for NVS Encryption
 */
static nvs_sec_scheme_t nvs_sec_default_scheme_cfg;

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
    if (pStorage == nullptr) {
        return;
    }

    pStorage->debugDump();
}

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

#ifndef CONFIG_NVS_BDL_STACK
extern "C" esp_err_t nvs_flash_init_partition_ptr(const esp_partition_t *partition)
{
    esp_err_t lock_result = Lock::init();
    if (lock_result != ESP_OK) {
        return lock_result;
    }
    Lock lock;

    if (partition == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    NVSPartition *part = new (std::nothrow) NVSPartition(partition);
    if (part == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    uint32_t sec_size = NVS_CONST_PAGE_SIZE;
    uint32_t size = part->get_size();

    esp_err_t init_res = NVSPartitionManager::get_instance()->init_custom(part,
            0,
            size / sec_size);

    if (init_res != ESP_OK) {
        delete part;
    }

    return init_res;
}
#else
extern "C" esp_err_t nvs_flash_init_partition_bdl(const char* partition_label, esp_blockdev_handle_t bdl)
{
    esp_err_t lock_result = Lock::init();
    if (lock_result != ESP_OK) {
        return lock_result;
    }
    Lock lock;

    if (partition_label == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (bdl == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!NVSPartition::is_bdl_nvs_compliant(partition_label, bdl)) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    NVSPartition *part = new (std::nothrow) NVSPartition(partition_label, bdl, false);
    if (part == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    uint32_t sec_size = NVS_CONST_PAGE_SIZE;
    uint32_t size = part->get_size();

    esp_err_t init_res = NVSPartitionManager::get_instance()->init_custom(part,
            0,
            size / sec_size);

    if (init_res != ESP_OK) {
        delete part;
    }

    return init_res;
}
#endif

#ifndef LINUX_HOST_LEGACY_TEST
extern "C" esp_err_t nvs_flash_init_partition(const char *part_name)
{
    esp_err_t lock_result = Lock::init();
    if (lock_result != ESP_OK) {
        return lock_result;
    }
    Lock lock;

    return NVSPartitionManager::get_instance()->init_partition(part_name);
}

extern "C" esp_err_t nvs_flash_init(void)
{
#ifdef CONFIG_NVS_ENCRYPTION
    esp_err_t ret = ESP_FAIL;
    nvs_sec_cfg_t cfg = {};

    ret = nvs_flash_read_security_cfg_v2(&nvs_sec_default_scheme_cfg, &cfg);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to read NVS security cfg: [0x%02X] (%s)", ret, esp_err_to_name(ret));
        ESP_LOGI(TAG, "Generating NVS encr-keys...");
        ret = nvs_flash_generate_keys_v2(&nvs_sec_default_scheme_cfg, &cfg);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to generate NVS encr-keys: [0x%02X] (%s)", ret, esp_err_to_name(ret));
            return ret;
        }
    }

    ret = nvs_flash_secure_init(&cfg);
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGE(TAG, "Failed to initialize NVS partition: [0x%02X] (%s)", ret, esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "NVS partition \"%s\" is encrypted.", NVS_DEFAULT_PART_NAME);
    return ret;
#else // CONFIG_NVS_ENCRYPTION
    return nvs_flash_init_partition(NVS_DEFAULT_PART_NAME);
#endif
}

extern "C" esp_err_t nvs_flash_secure_init_partition(const char *part_name, nvs_sec_cfg_t* cfg)
{
    esp_err_t lock_result = Lock::init();
    if (lock_result != ESP_OK) {
        return lock_result;
    }
    Lock lock;

    return NVSPartitionManager::get_instance()->secure_init_partition(part_name, cfg);
}

extern "C" esp_err_t nvs_flash_secure_init(nvs_sec_cfg_t* cfg)
{
    return nvs_flash_secure_init_partition(NVS_DEFAULT_PART_NAME, cfg);
}

extern "C" esp_err_t nvs_flash_erase_partition(const char *part_name)
{
    esp_err_t lock_result = Lock::init();
    if (lock_result != ESP_OK) {
        return lock_result;
    }
    Lock lock;

    // if the partition is initialized, uninitialize it first
    if (NVSPartitionManager::get_instance()->lookup_storage_from_name(part_name)) {
        esp_err_t err = close_handles_and_deinit(part_name);

        // only hypothetical/future case, deinit_partition() only fails if partition is uninitialized
        if (err != ESP_OK) {
            return err;
        }
    }

    // reuse the partition lookup code to find the NVS partition to streamline the BDL and non-BDL code paths
    nvs::Partition* part = nullptr;
    esp_err_t err = nvs::partition_lookup::lookup_nvs_partition(part_name, &part);
    if (err != ESP_OK || part == nullptr) {
        return err;
    }

    // erase the partition
    err = part->erase_range(0, part->get_size());

    // No need to delete the partition here, as it is managed by the NVSPartitionManager.
    return err;
}

extern "C" esp_err_t nvs_flash_erase_partition_ptr(const esp_partition_t *partition)
{
    return nvs_flash_erase_partition(partition->label);
}

extern "C" esp_err_t nvs_flash_erase(void)
{
    return nvs_flash_erase_partition(NVS_DEFAULT_PART_NAME);
}
#endif // LINUX_HOST_LEGACY_TEST

extern "C" esp_err_t nvs_flash_deinit_partition(const char* partition_name)
{
    esp_err_t lock_result = Lock::init();
    if (lock_result != ESP_OK) {
        return lock_result;
    }
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

extern "C" esp_err_t nvs_open_from_partition(const char *part_name, const char* namespace_name, nvs_open_mode_t open_mode, nvs_handle_t *out_handle)
{
    esp_err_t lock_result = Lock::init();
    if (lock_result != ESP_OK) {
        return lock_result;
    }
    Lock lock;
    ESP_LOGD(TAG, "%s %s %d", __func__, namespace_name, open_mode);

    NVSHandleSimple *handle;
    esp_err_t result = NVSPartitionManager::get_instance()->open_handle(part_name, namespace_name, open_mode, &handle);
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

extern "C" esp_err_t nvs_open(const char* namespace_name, nvs_open_mode_t open_mode, nvs_handle_t *out_handle)
{
    return nvs_open_from_partition(NVS_DEFAULT_PART_NAME, namespace_name, open_mode, out_handle);
}

extern "C" void nvs_close(nvs_handle_t handle)
{
    Lock lock;
    ESP_LOGD(TAG, "%s %d", __func__, static_cast<int>(handle));
    auto it = find_if(begin(s_nvs_handles), end(s_nvs_handles), [=](NVSHandleEntry& e) -> bool {
        return e.mHandle == handle;
    });
    if (it == end(s_nvs_handles)) {
        return;
    }
    s_nvs_handles.erase(it);
    delete static_cast<NVSHandleEntry*>(it);
}

extern "C" esp_err_t nvs_find_key(nvs_handle_t c_handle, const char* key, nvs_type_t* out_type)
{
    Lock lock;
    ESP_LOGD(TAG, "%s %s", __func__, key);
    NVSHandleSimple *handle;
    auto err = nvs_find_ns_handle(c_handle, &handle);
    if (err != ESP_OK) {
        return err;
    }

    nvs_type_t nvstype;
    err = handle->find_key(key, nvstype);

    if(err == ESP_OK && out_type != nullptr)
        *out_type = nvstype;

    return err;
}

extern "C" esp_err_t nvs_erase_key(nvs_handle_t c_handle, const char* key)
{
    Lock lock;
    ESP_LOGD(TAG, "%s %s", __func__, key);
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
    ESP_LOGD(TAG, "%s", __func__);
    NVSHandleSimple *handle;
    auto err = nvs_find_ns_handle(c_handle, &handle);
    if (err != ESP_OK) {
        return err;
    }

    return handle->erase_all();
}

extern "C" esp_err_t nvs_purge_all(nvs_handle_t c_handle)
{
    Lock lock;
    ESP_LOGD(TAG, "%s", __func__);
    NVSHandleSimple *handle;
    auto err = nvs_find_ns_handle(c_handle, &handle);
    if (err != ESP_OK) {
        return err;
    }

    return handle->purge_all();
}


template<typename T>
static esp_err_t nvs_set(nvs_handle_t c_handle, const char* key, T value)
{
    Lock lock;
    ESP_LOGD(TAG, "%s %s %d %ld", __func__, key, static_cast<int>(sizeof(T)), static_cast<long int>(value));
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
    ESP_LOGD(TAG, "%s %s %d", __func__, key, static_cast<int>(length));
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
    ESP_LOGD(TAG, "%s %s %ld", __func__, key, static_cast<long int>(sizeof(T)));
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

    if (nvs_stats == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    nvs_stats->used_entries      = 0;
    nvs_stats->free_entries      = 0;
    nvs_stats->total_entries     = 0;
    nvs_stats->available_entries = 0;
    nvs_stats->namespace_count   = 0;

    pStorage = lookup_storage_from_name((part_name == nullptr) ? NVS_DEFAULT_PART_NAME : part_name);
    if (pStorage == nullptr) {
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
    if(used_entries == nullptr){
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

#ifndef LINUX_TARGET

extern "C" esp_err_t nvs_flash_generate_keys(const esp_partition_t* partition, nvs_sec_cfg_t* cfg)
{
    if (cfg == nullptr || partition == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    auto err = esp_partition_erase_range(partition, 0, partition->size);
    if(err != ESP_OK) {
        return err;
    }

    for(uint8_t cnt = 0; cnt < NVS_KEY_SIZE; cnt++) {
        /* Adjacent 16-byte blocks should be different */
        if (((cnt / 16) & 1) == 0) {
            cfg->eky[cnt] = 0xff;
            cfg->tky[cnt] = 0xee;
        } else {
            cfg->eky[cnt] = 0x99;
            cfg->tky[cnt] = 0x88;
        }
    }

    /**
     * Write key configuration without encryption engine (using raw partition write APIs).
     * But the read is decrypted through flash encryption engine. This allows unique NVS encryption configuration,
     * as flash encryption key is randomly generated per device.
     */
    err = esp_partition_write_raw(partition, 0, cfg->eky, NVS_KEY_SIZE);
    if(err != ESP_OK) {
        return err;
    }

    /* Write without encryption, see note above */
    err = esp_partition_write_raw(partition, NVS_KEY_SIZE, cfg->tky, NVS_KEY_SIZE);
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

    uint32_t crc_calc = esp_rom_crc32_le(0xffffffff, cfg->eky, NVS_KEY_SIZE);
    crc_calc = esp_rom_crc32_le(crc_calc, cfg->tky, NVS_KEY_SIZE);

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
    if (cfg == nullptr || partition == nullptr) {
        return ESP_ERR_INVALID_ARG;
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

    auto err = esp_partition_read_raw(partition, 0, eky_raw, NVS_KEY_SIZE);
    if(err != ESP_OK) {
        return err;
    }

    err = esp_partition_read_raw(partition, NVS_KEY_SIZE, tky_raw, NVS_KEY_SIZE);
    if(err != ESP_OK) {
        return err;
    }

    err = esp_partition_read_raw(partition, 2 * NVS_KEY_SIZE, &crc_raw, 4);
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

    crc_calc = esp_rom_crc32_le(0xffffffff, cfg->eky, NVS_KEY_SIZE);
    crc_calc = esp_rom_crc32_le(crc_calc, cfg->tky, NVS_KEY_SIZE);

    if(crc_calc != crc_read) {
        if(!check_if_initialized(cfg->eky, cfg->tky, crc_read)) {
            /* This is an uninitialized key partition*/
            return ESP_ERR_NVS_KEYS_NOT_INITIALIZED;
        }
        return ESP_ERR_NVS_CORRUPT_KEY_PART;
    }

    return ESP_OK;
}

#endif // ! LINUX_TARGET

extern "C" esp_err_t nvs_flash_register_security_scheme(nvs_sec_scheme_t *scheme_cfg)
{
    if (scheme_cfg == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(&nvs_sec_default_scheme_cfg, scheme_cfg, sizeof(nvs_sec_scheme_t));

    return ESP_OK;
}

extern "C" void nvs_flash_deregister_security_scheme(void)
{
    memset(&nvs_sec_default_scheme_cfg, 0x00, sizeof(nvs_sec_scheme_t));
}

extern "C" nvs_sec_scheme_t *nvs_flash_get_default_security_scheme(void)
{
    return &nvs_sec_default_scheme_cfg;
}

extern "C" esp_err_t nvs_flash_generate_keys_v2(nvs_sec_scheme_t *scheme_cfg, nvs_sec_cfg_t* cfg)
{
    if (scheme_cfg == nullptr || cfg == nullptr || scheme_cfg->nvs_flash_key_gen == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    return (scheme_cfg->nvs_flash_key_gen)(scheme_cfg->scheme_data, cfg);
}

extern "C" esp_err_t nvs_flash_read_security_cfg_v2(nvs_sec_scheme_t *scheme_cfg, nvs_sec_cfg_t* cfg)
{
    if (scheme_cfg == nullptr || cfg == nullptr || scheme_cfg->nvs_flash_read_cfg == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    return (scheme_cfg->nvs_flash_read_cfg)(scheme_cfg->scheme_data, cfg);
}

static nvs_iterator_t create_iterator(nvs::Storage *storage, nvs_type_t type)
{
    nvs_iterator_t it = (nvs_iterator_t)calloc(1, sizeof(nvs_opaque_iterator_t));
    if (it == nullptr) {
        return nullptr;
    }

    it->storage = storage;
    it->type = type;

    return it;
}

// In case of errors except for parameter error, output_iterator is set to nullptr to make releasing iterators easier
extern "C" esp_err_t nvs_entry_find(const char *part_name, const char *namespace_name, nvs_type_t type, nvs_iterator_t *output_iterator)
{
    if (part_name == nullptr || output_iterator == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t lock_result = Lock::init();
    if (lock_result != ESP_OK) {
        *output_iterator = nullptr;
        return lock_result;
    }
    Lock lock;
    nvs::Storage *pStorage;

    pStorage = lookup_storage_from_name(part_name);
    if (pStorage == nullptr) {
        *output_iterator = nullptr;
        return ESP_ERR_NVS_NOT_FOUND;
    }

    nvs_iterator_t it = create_iterator(pStorage, type);
    if (it == nullptr) {
        *output_iterator = nullptr;
        return ESP_ERR_NO_MEM;
    }

    bool entryFound = pStorage->findEntry(it, namespace_name);
    if (!entryFound) {
        free(it);
        *output_iterator = nullptr;
        return ESP_ERR_NVS_NOT_FOUND;
    }

    *output_iterator = it;
    return ESP_OK;
}

extern "C" esp_err_t nvs_entry_find_in_handle(nvs_handle_t handle, nvs_type_t type, nvs_iterator_t *output_iterator)
{
    if (output_iterator == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t lock_result = Lock::init();
    if (lock_result != ESP_OK) {
        *output_iterator = nullptr;
        return lock_result;
    }

    Lock lock;
    nvs::Storage *pStorage;
    NVSHandleSimple *handle_obj;

    auto err = nvs_find_ns_handle(handle, &handle_obj);
    if (err != ESP_OK) {
        *output_iterator = nullptr;
        return err;
    }

    pStorage = handle_obj->get_storage();
    nvs_iterator_t it = create_iterator(pStorage, type);
    if (it == nullptr) {
        *output_iterator = nullptr;
        return ESP_ERR_NO_MEM;
    }

    bool entryFound = handle_obj->findEntryNs(it);
    if (!entryFound) {
        free(it);
        *output_iterator = nullptr;
        return ESP_ERR_NVS_NOT_FOUND;
    }

    *output_iterator = it;
    return ESP_OK;
}

extern "C" esp_err_t nvs_entry_next(nvs_iterator_t *iterator)
{
    if (iterator == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    Lock lock;

    bool entryFound = (*iterator)->storage->nextEntry(*iterator);
    if (!entryFound) {
        free(*iterator);
        *iterator = nullptr;
        return ESP_ERR_NVS_NOT_FOUND;
    }

    return ESP_OK;
}

extern "C" esp_err_t nvs_entry_info(const nvs_iterator_t it, nvs_entry_info_t *out_info)
{
    if (it == nullptr || out_info == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    *out_info = it->entry_info;

    return ESP_OK;
}

extern "C" void nvs_release_iterator(nvs_iterator_t it)
{
    free(it);
}
