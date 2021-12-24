/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "diskio_impl.h"
#include "ffconf.h"
#include "ff.h"
#include "esp_log.h"
#include "diskio_usb.h"
#include "msc_scsi_bot.h"
#include "msc_common.h"
#include "usb/usb_types_stack.h"

static usb_disk_t *s_disks[FF_VOLUMES] = { NULL };

static const char *TAG = "diskio_usb";

static DSTATUS usb_disk_initialize (BYTE pdrv)
{
    return RES_OK;
}

static DSTATUS usb_disk_status (BYTE pdrv)
{
    return RES_OK;
}

static DRESULT usb_disk_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    assert(pdrv < FF_VOLUMES);
    assert(s_disks[pdrv]);

    esp_err_t err;
    usb_disk_t *disk = s_disks[pdrv];
    size_t sector_size = disk->block_size;
    msc_device_t *dev = __containerof(disk, msc_device_t, disk);

    for (int i = 0; i < count; i++) {
        err = scsi_cmd_read10(dev, &buff[i * sector_size], sector + i, 1, sector_size);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "scsi_cmd_read10 failed (%d)", err);
            return RES_ERROR;
        }

    }

    return RES_OK;
}

static DRESULT usb_disk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    assert(pdrv < FF_VOLUMES);
    assert(s_disks[pdrv]);

    esp_err_t err;
    usb_disk_t *disk = s_disks[pdrv];
    size_t sector_size = disk->block_size;
    msc_device_t *dev = __containerof(disk, msc_device_t, disk);

    for (int i = 0; i < count; i++) {
        err = scsi_cmd_write10(dev, &buff[i * sector_size], sector + i, 1, sector_size);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "scsi_cmd_write10 failed (%d)", err);
            return RES_ERROR;
        }

    }
    return RES_OK;
}

static DRESULT usb_disk_ioctl (BYTE pdrv, BYTE cmd, void *buff)
{
    assert(pdrv < FF_VOLUMES);
    assert(s_disks[pdrv]);

    usb_disk_t *disk = s_disks[pdrv];

    switch (cmd) {
    case CTRL_SYNC:
        return RES_OK;
    case GET_SECTOR_COUNT:
        *((DWORD *) buff) = disk->block_count;
        return RES_OK;
    case GET_SECTOR_SIZE:
        *((WORD *) buff) = disk->block_size;
        return RES_OK;
    case GET_BLOCK_SIZE:
        return RES_ERROR;
    }
    return RES_ERROR;
}

void ff_diskio_register_msc(BYTE pdrv, usb_disk_t *disk)
{
    assert(pdrv < FF_VOLUMES);

    static const ff_diskio_impl_t usb_disk_impl = {
        .init = &usb_disk_initialize,
        .status = &usb_disk_status,
        .read = &usb_disk_read,
        .write = &usb_disk_write,
        .ioctl = &usb_disk_ioctl
    };
    s_disks[pdrv] = disk;
    ff_diskio_register(pdrv, &usb_disk_impl);
}

BYTE ff_diskio_get_pdrv_disk(const usb_disk_t *disk)
{
    for (int i = 0; i < FF_VOLUMES; i++) {
        if (disk == s_disks[i]) {
            return i;
        }
    }
    return 0xff;
}
