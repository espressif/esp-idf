/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "diskio_impl.h"
#include "ffconf.h"
#include "ff.h"
#include "esp_log.h"
#include "diskio_wl.h"
#include "wear_levelling.h"
#include "esp_compiler.h"

static const char* TAG = "ff_diskio_spiflash";

wl_handle_t ff_wl_handles[FF_VOLUMES] = {
        [0 ... FF_VOLUMES - 1] = WL_INVALID_HANDLE
};

DSTATUS ff_wl_initialize (BYTE pdrv)
{
    return 0;
}

DSTATUS ff_wl_status (BYTE pdrv)
{
    return 0;
}

DRESULT ff_wl_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    ESP_LOGV(TAG, "ff_wl_read - pdrv=%i, sector=%i, count=%i", (unsigned int)pdrv, (unsigned int)sector, (unsigned int)count);
    wl_handle_t wl_handle = ff_wl_handles[pdrv];
    assert(wl_handle + 1);
    esp_err_t err = wl_read(wl_handle, sector * wl_sector_size(wl_handle), buff, count * wl_sector_size(wl_handle));
    if (unlikely(err != ESP_OK)) {
        ESP_LOGE(TAG, "wl_read failed (0x%x)", err);
        return RES_ERROR;
    }
    return RES_OK;
}

DRESULT ff_wl_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    ESP_LOGV(TAG, "ff_wl_write - pdrv=%i, sector=%i, count=%i", (unsigned int)pdrv, (unsigned int)sector, (unsigned int)count);
    wl_handle_t wl_handle = ff_wl_handles[pdrv];
    assert(wl_handle + 1);
    esp_err_t err = wl_erase_range(wl_handle, sector * wl_sector_size(wl_handle), count * wl_sector_size(wl_handle));
    if (unlikely(err != ESP_OK)) {
        ESP_LOGE(TAG, "wl_erase_range failed (0x%x)", err);
        return RES_ERROR;
    }
    err = wl_write(wl_handle, sector * wl_sector_size(wl_handle), buff, count * wl_sector_size(wl_handle));
    if (unlikely(err != ESP_OK)) {
        ESP_LOGE(TAG, "wl_write failed (0x%x)", err);
        return RES_ERROR;
    }
    return RES_OK;
}

DRESULT ff_wl_ioctl (BYTE pdrv, BYTE cmd, void *buff)
{
    wl_handle_t wl_handle = ff_wl_handles[pdrv];
    ESP_LOGV(TAG, "ff_wl_ioctl: cmd=%i", cmd);
    assert(wl_handle + 1);
    switch (cmd) {
    case CTRL_SYNC:
        return RES_OK;
    case GET_SECTOR_COUNT:
        *((DWORD *) buff) = wl_size(wl_handle) / wl_sector_size(wl_handle);
        return RES_OK;
    case GET_SECTOR_SIZE:
        *((WORD *) buff) = wl_sector_size(wl_handle);
        return RES_OK;
    case GET_BLOCK_SIZE:
        return RES_ERROR;
    }
    return RES_ERROR;
}


esp_err_t ff_diskio_register_wl_partition(BYTE pdrv, wl_handle_t flash_handle)
{
    if (pdrv >= FF_VOLUMES) {
        return ESP_ERR_INVALID_ARG;
    }
    static const ff_diskio_impl_t wl_impl = {
        .init = &ff_wl_initialize,
        .status = &ff_wl_status,
        .read = &ff_wl_read,
        .write = &ff_wl_write,
        .ioctl = &ff_wl_ioctl
    };
    ff_wl_handles[pdrv] = flash_handle;
    ff_diskio_register(pdrv, &wl_impl);
    return ESP_OK;
}

BYTE ff_diskio_get_pdrv_wl(wl_handle_t flash_handle)
{
    for (int i = 0; i < FF_VOLUMES; i++) {
        if (flash_handle == ff_wl_handles[i]) {
            return i;
        }
    }
    return 0xff;
}

void ff_diskio_clear_pdrv_wl(wl_handle_t flash_handle)
{
    for (int i = 0; i < FF_VOLUMES; i++) {
        if (flash_handle == ff_wl_handles[i]) {
            ff_wl_handles[i] = WL_INVALID_HANDLE;
        }
    }
}
