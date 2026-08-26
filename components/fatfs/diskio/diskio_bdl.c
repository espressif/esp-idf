/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "diskio_impl.h"
#include "ffconf.h"
#include "ff.h"
#include "esp_log.h"
#include "diskio_bdl.h"
#include "esp_compiler.h"

static const char *TAG = "ff_diskio_bdl";

/* ------------------------------------------------------------------ */
/*  LCM helpers for FatFS sector-size derivation from BDL geometry    */
/* ------------------------------------------------------------------ */

static inline size_t gcd_size(size_t a, size_t b)
{
    while (b != 0) {
        size_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

static inline size_t lcm2_size(size_t a, size_t b)
{
    return (a && b) ? (a / gcd_size(a, b)) * b : 0;
}

/**
 * Derive the FatFS logical sector size purely from BDL geometry.
 *
 * The sector must be a common multiple of read_size, write_size and
 * FF_MIN_SS (typically 512).  When erase_size can be included without
 * exceeding FF_MAX_SS the sector is also erase-aligned — correct for
 * NOR-style devices and optimal for any device.  When erase alignment
 * would push the sector beyond FF_MAX_SS (typical for NAND where
 * erase blocks >> page size) erase_size is omitted; such devices must
 * handle erase internally (FTL / wear-levelling layer).
 *
 * No BDL flags are inspected — the NOR/NAND distinction is implicit
 * in the geometry: NOR erase blocks fit within FF_MAX_SS, NAND ones
 * do not.
 *
 * @return  valid power-of-two sector size in [FF_MIN_SS, FF_MAX_SS],
 *          or 0 if the geometry is incompatible with FatFS.
 */
static size_t compute_fs_sector_size(esp_blockdev_handle_t dev)
{
    const esp_blockdev_geometry_t *g = &dev->geometry;

    size_t result = (size_t)FF_MIN_SS;

    if (g->read_size > 1) {
        result = lcm2_size(result, g->read_size);
    }
    if (g->write_size > 1) {
        result = lcm2_size(result, g->write_size);
    }

    if (g->erase_size > 1) {
        size_t with_erase = lcm2_size(result, g->erase_size);
        if (with_erase && with_erase <= FF_MAX_SS) {
            result = with_erase;
        }
    }

    if (result < FF_MIN_SS || result > FF_MAX_SS || (result & (result - 1)) != 0) {
        return 0;
    }

    return result;
}

/* ------------------------------------------------------------------ */

typedef struct {
    esp_blockdev_handle_t handle;
    size_t fs_sector_size;
} bdl_drive_t;

static bdl_drive_t s_bdl_drives[FF_VOLUMES];

static DSTATUS ff_bdl_initialize(BYTE pdrv)
{
    esp_blockdev_handle_t dev = s_bdl_drives[pdrv].handle;
    assert(dev != ESP_BLOCKDEV_HANDLE_INVALID);
    if (dev->device_flags.read_only) {
        return STA_PROTECT;
    }
    return 0;
}

static DSTATUS ff_bdl_status(BYTE pdrv)
{
    esp_blockdev_handle_t dev = s_bdl_drives[pdrv].handle;
    assert(dev != ESP_BLOCKDEV_HANDLE_INVALID);
    if (dev->device_flags.read_only) {
        return STA_PROTECT;
    }
    return 0;
}

static DRESULT ff_bdl_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    bdl_drive_t *drv = &s_bdl_drives[pdrv];
    assert(drv->handle != ESP_BLOCKDEV_HANDLE_INVALID);
    size_t sec_size = drv->fs_sector_size;
    ESP_LOGV(TAG, "read - pdrv=%u, sector=%lu, count=%u, sec_size=%u",
             (unsigned)pdrv, (unsigned long)sector, (unsigned)count, (unsigned)sec_size);

    esp_err_t err = drv->handle->ops->read(drv->handle, buff, count * sec_size,
                                           (uint64_t)sector * sec_size, count * sec_size);
    if (unlikely(err != ESP_OK)) {
        ESP_LOGE(TAG, "BDL read failed (0x%x)", err);
        return RES_ERROR;
    }
    return RES_OK;
}

static DRESULT ff_bdl_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    bdl_drive_t *drv = &s_bdl_drives[pdrv];
    assert(drv->handle != ESP_BLOCKDEV_HANDLE_INVALID);

    if (drv->handle->device_flags.read_only) {
        return RES_WRPRT;
    }

    size_t sec_size = drv->fs_sector_size;
    uint64_t addr = (uint64_t)sector * sec_size;
    size_t len = count * sec_size;

    ESP_LOGV(TAG, "write - pdrv=%u, sector=%lu, count=%u", (unsigned)pdrv, (unsigned long)sector, (unsigned)count);

    if (drv->handle->device_flags.erase_before_write || drv->handle->device_flags.and_type_write) {
        size_t erase_sz = drv->handle->geometry.erase_size;
        if ((addr % erase_sz == 0) && (len % erase_sz == 0)) {
            esp_err_t err = drv->handle->ops->erase(drv->handle, addr, len);
            if (unlikely(err != ESP_OK)) {
                ESP_LOGE(TAG, "BDL erase failed (0x%x)", err);
                return RES_ERROR;
            }
        }
    }

    esp_err_t err = drv->handle->ops->write(drv->handle, buff, addr, len);
    if (unlikely(err != ESP_OK)) {
        ESP_LOGE(TAG, "BDL write failed (0x%x)", err);
        return RES_ERROR;
    }
    return RES_OK;
}

static DRESULT ff_bdl_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    bdl_drive_t *drv = &s_bdl_drives[pdrv];
    assert(drv->handle != ESP_BLOCKDEV_HANDLE_INVALID);
    ESP_LOGV(TAG, "ioctl: cmd=%u", (unsigned)cmd);

    switch (cmd) {
    case CTRL_SYNC:
        if (drv->handle->ops->sync) {
            esp_err_t err = drv->handle->ops->sync(drv->handle);
            if (unlikely(err != ESP_OK)) {
                ESP_LOGE(TAG, "BDL sync failed (0x%x)", err);
                return RES_ERROR;
            }
        }
        return RES_OK;
    case GET_SECTOR_COUNT:
        *((DWORD *)buff) = (DWORD)(drv->handle->geometry.disk_size / drv->fs_sector_size);
        return RES_OK;
    case GET_SECTOR_SIZE:
        *((WORD *)buff) = (WORD)drv->fs_sector_size;
        return RES_OK;
    case GET_BLOCK_SIZE: {
        size_t erase_sz = drv->handle->geometry.erase_size;
        *((DWORD *)buff) = (erase_sz >= drv->fs_sector_size)
                          ? (DWORD)(erase_sz / drv->fs_sector_size)
                          : 1;
        return RES_OK;
    }
#if FF_USE_TRIM
    case CTRL_TRIM: {
        if (drv->handle->ops->ioctl == NULL) {
            return RES_OK;
        }
        size_t sec_size = drv->fs_sector_size;
        DWORD start_sector = *((DWORD *)buff);
        DWORD end_sector = *((DWORD *)buff + 1);
        esp_blockdev_cmd_arg_erase_t erase_arg = {
            .start_addr = (uint64_t)start_sector * sec_size,
            .erase_len  = (size_t)(end_sector - start_sector + 1) * sec_size,
        };
        esp_err_t err = drv->handle->ops->ioctl(drv->handle, ESP_BLOCKDEV_CMD_MARK_DELETED, &erase_arg);
        if (unlikely(err != ESP_OK && err != ESP_ERR_NOT_SUPPORTED)) {
            ESP_LOGE(TAG, "BDL TRIM ioctl failed (0x%x)", err);
            return RES_ERROR;
        }
        return RES_OK;
    }
#endif
    }
    return RES_ERROR;
}

esp_err_t ff_diskio_register_bdl(BYTE pdrv, esp_blockdev_handle_t bdl_handle)
{
    if (pdrv >= FF_VOLUMES) {
        return ESP_ERR_INVALID_ARG;
    }
    if (bdl_handle == ESP_BLOCKDEV_HANDLE_INVALID) {
        return ESP_ERR_INVALID_ARG;
    }
    if (bdl_handle->geometry.read_size == 0 || bdl_handle->geometry.disk_size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t fs_sec = compute_fs_sector_size(bdl_handle);
    if (fs_sec == 0) {
        ESP_LOGE(TAG, "BDL geometry incompatible with FatFS "
                 "(read=%u, write=%u, erase=%u, FF_MAX_SS=%u)",
                 (unsigned)bdl_handle->geometry.read_size,
                 (unsigned)bdl_handle->geometry.write_size,
                 (unsigned)bdl_handle->geometry.erase_size,
                 (unsigned)FF_MAX_SS);
        return ESP_ERR_INVALID_ARG;
    }

    static const ff_diskio_impl_t bdl_impl = {
        .init   = &ff_bdl_initialize,
        .status = &ff_bdl_status,
        .read   = &ff_bdl_read,
        .write  = &ff_bdl_write,
        .ioctl  = &ff_bdl_ioctl
    };

    s_bdl_drives[pdrv] = (bdl_drive_t){
        .handle = bdl_handle,
        .fs_sector_size = fs_sec,
    };
    ff_diskio_register(pdrv, &bdl_impl);
    ESP_LOGD(TAG, "pdrv=%u registered, fs_sector_size=%u, erase_size=%u, disk_size=%llu",
             (unsigned)pdrv, (unsigned)fs_sec,
             (unsigned)bdl_handle->geometry.erase_size,
             (unsigned long long)bdl_handle->geometry.disk_size);
    return ESP_OK;
}

BYTE ff_diskio_get_pdrv_bdl(esp_blockdev_handle_t bdl_handle)
{
    for (int i = 0; i < FF_VOLUMES; i++) {
        if (bdl_handle == s_bdl_drives[i].handle) {
            return i;
        }
    }
    return 0xff;
}

void ff_diskio_clear_pdrv_bdl(esp_blockdev_handle_t bdl_handle)
{
    for (int i = 0; i < FF_VOLUMES; i++) {
        if (bdl_handle == s_bdl_drives[i].handle) {
            s_bdl_drives[i] = (bdl_drive_t){0};
        }
    }
}
