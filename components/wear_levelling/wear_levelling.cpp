/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <new>
#include <sys/lock.h>
#include "wear_levelling.h"
#include "WL_Config.h"
#include "WL_Ext_Cfg.h"
#include "WL_Flash.h"
#include "WL_Ext_Perf.h"
#include "WL_Ext_Safe.h"
#include "SPI_Flash.h"
#include "Partition.h"

#ifndef MAX_WL_HANDLES
#define MAX_WL_HANDLES 8
#endif // MAX_WL_HANDLES

#ifndef WL_DEFAULT_UPDATERATE
#define WL_DEFAULT_UPDATERATE   16
#endif //WL_DEFAULT_UPDATERATE

#ifndef WL_DEFAULT_TEMP_BUFF_SIZE
#define WL_DEFAULT_TEMP_BUFF_SIZE   32
#endif //WL_DEFAULT_TEMP_BUFF_SIZE

#ifndef WL_DEFAULT_WRITE_SIZE
#define WL_DEFAULT_WRITE_SIZE   16
#endif //WL_DEFAULT_WRITE_SIZE

#ifndef WL_DEFAULT_START_ADDR
#define WL_DEFAULT_START_ADDR   0
#endif //WL_DEFAULT_START_ADDR

#ifndef WL_CURRENT_VERSION
#define WL_CURRENT_VERSION  2
#endif //WL_CURRENT_VERSION

typedef struct {
    WL_Flash *instance;
    _lock_t lock;
} wl_instance_t;

static wl_instance_t s_instances[MAX_WL_HANDLES];
static _lock_t s_instances_lock;
static const char *TAG = "wear_levelling";

static esp_err_t check_handle(wl_handle_t handle, const char *func);

esp_err_t wl_mount(const esp_partition_t *partition, wl_handle_t *out_handle)
{
    // Initialize variables before the first jump to cleanup label
    void *wl_flash_ptr = NULL;
    WL_Flash *wl_flash = NULL;
    void *part_ptr = NULL;
    Partition *part = NULL;
    esp_err_t result = ESP_OK;
    *out_handle = WL_INVALID_HANDLE;

    _lock_acquire(&s_instances_lock);

    // Get first available WL handle
    for (size_t i = 0; i < MAX_WL_HANDLES; i++) {
        if (s_instances[i].instance == NULL) {
            *out_handle = i;
            break;
        }
    }

    if (*out_handle == WL_INVALID_HANDLE) {
        ESP_LOGE(TAG, "MAX_WL_HANDLES=%d instances already allocated", MAX_WL_HANDLES);
        result = ESP_ERR_NO_MEM;
        goto out;
    }

    wl_ext_cfg_t cfg;

    cfg.wl_partition_start_addr   = WL_DEFAULT_START_ADDR;
    cfg.wl_partition_size         = partition->size;
    cfg.wl_page_size              = SPI_FLASH_SEC_SIZE;
    cfg.flash_sector_size         = SPI_FLASH_SEC_SIZE;     //default size is 4096
    cfg.wl_update_rate            = WL_DEFAULT_UPDATERATE;
    cfg.wl_pos_update_record_size = WL_DEFAULT_WRITE_SIZE;  //16 bytes per pos update will be stored
    cfg.version                   = WL_CURRENT_VERSION;
    cfg.wl_temp_buff_size         = WL_DEFAULT_TEMP_BUFF_SIZE;
    cfg.fat_sector_size           = CONFIG_WL_SECTOR_SIZE;  //default size is 4096

    // Allocate memory for a Partition object, and then initialize the object
    // using placement new operator. This way we can recover from out of
    // memory condition.
    part_ptr = malloc(sizeof(Partition));
    if (part_ptr == NULL) {
        result = ESP_ERR_NO_MEM;
        ESP_LOGE(TAG, "%s: can't allocate Partition", __func__);
        goto out;
    }
    part = new (part_ptr) Partition(partition);

    // Same for WL_Flash: allocate memory first and then use placement new operator
#if CONFIG_WL_SECTOR_SIZE == 512
#if CONFIG_WL_SECTOR_MODE == 1   //Safety mode
    wl_flash_ptr = malloc(sizeof(WL_Ext_Safe));

    if (wl_flash_ptr == NULL) {
        result = ESP_ERR_NO_MEM;
        ESP_LOGE(TAG, "%s: can't allocate WL_Ext_Safe", __func__);
        goto out;
    }
    wl_flash = new (wl_flash_ptr) WL_Ext_Safe();
#else //Performance mode
    wl_flash_ptr = malloc(sizeof(WL_Ext_Perf));

    if (wl_flash_ptr == NULL) {
        result = ESP_ERR_NO_MEM;
        ESP_LOGE(TAG, "%s: can't allocate WL_Ext_Perf", __func__);
        goto out;
    }
    wl_flash = new (wl_flash_ptr) WL_Ext_Perf();
#endif // CONFIG_WL_SECTOR_MODE (Safety or performance mode)
#endif // CONFIG_WL_SECTOR_SIZE
#if CONFIG_WL_SECTOR_SIZE == 4096
    wl_flash_ptr = malloc(sizeof(WL_Flash));

    if (wl_flash_ptr == NULL) {
        result = ESP_ERR_NO_MEM;
        ESP_LOGE(TAG, "%s: can't allocate WL_Flash", __func__);
        goto out;
    }
    wl_flash = new (wl_flash_ptr) WL_Flash();
#endif // CONFIG_WL_SECTOR_SIZE

    // Configure data needed by WL layer for respective flash driver
    result = wl_flash->config(&cfg, part);
    if (ESP_OK != result) {
        ESP_LOGE(TAG, "%s: config instance=0x%08" PRIx32 ", result=0x%x", __func__, *out_handle, result);
        goto out;
    }

    // Initialise sectors used by WL layer for respective flash driver
    result = wl_flash->init();
    if (ESP_OK != result) {
        ESP_LOGE(TAG, "%s: init instance=0x%08" PRIx32 ", result=0x%x", __func__, *out_handle, result);
        goto out;
    }

    s_instances[*out_handle].instance = wl_flash;
    // Initialise the lock for respective WL handle
    _lock_init(&s_instances[*out_handle].lock);

    _lock_release(&s_instances_lock);
    return ESP_OK;

out:
    _lock_release(&s_instances_lock);
    *out_handle = WL_INVALID_HANDLE;
    if (wl_flash) {
        wl_flash->~WL_Flash();
        free(wl_flash);
    }
    if (part) {
        part->~Partition();
        free(part);
    }
    return result;
}

esp_err_t wl_unmount(wl_handle_t handle)
{
    esp_err_t result = ESP_OK;
    _lock_acquire(&s_instances_lock);
    result = check_handle(handle, __func__);
    if (result == ESP_OK) {
        // We use placement new in wl_mount, so call destructor directly
        Partition *part = s_instances[handle].instance->get_part();
        // We have to flush state of the component
        if (!part->is_readonly()) {
            result = s_instances[handle].instance->flush();
        }
        part->~Partition();
        free(part);
        s_instances[handle].instance->~WL_Flash();
        free(s_instances[handle].instance);
        s_instances[handle].instance = NULL;
        _lock_close(&s_instances[handle].lock); // also zeroes the lock variable
    }
    _lock_release(&s_instances_lock);
    return result;
}

esp_err_t wl_erase_range(wl_handle_t handle, size_t start_addr, size_t size)
{
    esp_err_t result = check_handle(handle, __func__);
    if (result != ESP_OK) {
        return result;
    }
    _lock_acquire(&s_instances[handle].lock);
    result = s_instances[handle].instance->erase_range(start_addr, size);
    _lock_release(&s_instances[handle].lock);
    return result;
}

esp_err_t wl_write(wl_handle_t handle, size_t dest_addr, const void *src, size_t size)
{
    esp_err_t result = check_handle(handle, __func__);
    if (result != ESP_OK) {
        return result;
    }
    _lock_acquire(&s_instances[handle].lock);
    result = s_instances[handle].instance->write(dest_addr, src, size);
    _lock_release(&s_instances[handle].lock);
    return result;
}

esp_err_t wl_read(wl_handle_t handle, size_t src_addr, void *dest, size_t size)
{
    esp_err_t result = check_handle(handle, __func__);
    if (result != ESP_OK) {
        return result;
    }
    _lock_acquire(&s_instances[handle].lock);
    result = s_instances[handle].instance->read(src_addr, dest, size);
    _lock_release(&s_instances[handle].lock);
    return result;
}

size_t wl_size(wl_handle_t handle)
{
    esp_err_t err = check_handle(handle, __func__);
    if (err != ESP_OK) {
        return 0;
    }
    _lock_acquire(&s_instances[handle].lock);
    size_t result = s_instances[handle].instance->get_flash_size();
    _lock_release(&s_instances[handle].lock);
    return result;
}

size_t wl_sector_size(wl_handle_t handle)
{
    esp_err_t err = check_handle(handle, __func__);
    if (err != ESP_OK) {
        return 0;
    }
    _lock_acquire(&s_instances[handle].lock);
    size_t result = s_instances[handle].instance->get_sector_size();
    _lock_release(&s_instances[handle].lock);
    return result;
}

static esp_err_t check_handle(wl_handle_t handle, const char *func)
{
    if (handle == WL_INVALID_HANDLE) {
        ESP_LOGE(TAG, "%s: invalid handle", func);
        return ESP_ERR_NOT_FOUND;
    }
    if (handle >= MAX_WL_HANDLES) {
        ESP_LOGE(TAG, "%s: instance[0x%08" PRIx32 "] out of range", func, handle);
        return ESP_ERR_INVALID_ARG;
    }
    if (s_instances[handle].instance == NULL) {
        ESP_LOGE(TAG, "%s: instance[0x%08" PRIx32 "] not initialized", func, handle);
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}
