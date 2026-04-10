/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "BDL_Access.h"
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "esp_log.h"

static const char *TAG = "wl_bdl_access";

BDL_Access::BDL_Access(esp_blockdev_handle_t bdl_device)
    : bdl_device(bdl_device)
{
    assert(bdl_device != ESP_BLOCKDEV_HANDLE_INVALID);
}

size_t BDL_Access::get_flash_size()
{
    assert(this->bdl_device->geometry.disk_size <= SIZE_MAX);
    return (size_t)this->bdl_device->geometry.disk_size;
}

esp_err_t BDL_Access::erase_sector(size_t sector)
{
    size_t erase_size = this->bdl_device->geometry.erase_size;
    return this->bdl_device->ops->erase(this->bdl_device, sector * erase_size, erase_size);
}

esp_err_t BDL_Access::erase_range(size_t start_address, size_t size)
{
    ESP_LOGV(TAG, "%s - start_address=0x%08" PRIx32 ", size=0x%08" PRIx32, __func__, (uint32_t)start_address, (uint32_t)size);
    return this->bdl_device->ops->erase(this->bdl_device, start_address, size);
}

esp_err_t BDL_Access::write(size_t dest_addr, const void *src, size_t size)
{
    ESP_LOGV(TAG, "%s - dest_addr=0x%08" PRIx32 ", size=0x%08" PRIx32, __func__, (uint32_t)dest_addr, (uint32_t)size);
    return this->bdl_device->ops->write(this->bdl_device, (const uint8_t *)src, dest_addr, size);
}

esp_err_t BDL_Access::read(size_t src_addr, void *dest, size_t size)
{
    ESP_LOGV(TAG, "%s - src_addr=0x%08" PRIx32 ", size=0x%08" PRIx32, __func__, (uint32_t)src_addr, (uint32_t)size);
    return this->bdl_device->ops->read(this->bdl_device, (uint8_t *)dest, size, src_addr, size);
}

size_t BDL_Access::get_sector_size()
{
    return this->bdl_device->geometry.erase_size;
}

bool BDL_Access::is_readonly()
{
    return this->bdl_device->device_flags.read_only;
}

BDL_Access::~BDL_Access()
{
}
