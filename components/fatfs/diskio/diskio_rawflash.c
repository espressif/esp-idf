/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "diskio_impl.h"
#include "ffconf.h"
#include "ff.h"
#include "esp_log.h"
#include "diskio_rawflash.h"
#include "esp_compiler.h"

static const char* TAG = "diskio_rawflash";

static const esp_partition_t* s_ff_raw_handles[FF_VOLUMES];
// Determine the sector size and sector count by parsing the boot sector
static size_t s_sector_size[FF_VOLUMES];
static size_t s_sectors_count[FF_VOLUMES];
static uint8_t s_initialized[FF_VOLUMES];

#define BPB_BytsPerSec 11
#define BPB_TotSec16 19
#define BPB_TotSec32 32


DSTATUS ff_raw_initialize (BYTE pdrv)
{

    uint16_t sector_size_tmp;
    uint16_t sectors_count_tmp_16;
    uint32_t sectors_count_tmp_32;

    const esp_partition_t* part = s_ff_raw_handles[pdrv];
    assert(part);
    esp_err_t err = esp_partition_read(part, BPB_BytsPerSec, &sector_size_tmp, sizeof(sector_size_tmp));
    if (unlikely(err != ESP_OK)) {
        ESP_LOGE(TAG, "esp_partition_read failed (0x%x)", err);
        return RES_ERROR;
    }
    s_sector_size[pdrv] = sector_size_tmp;

    err = esp_partition_read(part, BPB_TotSec16, &sectors_count_tmp_16, sizeof(sectors_count_tmp_16));
    if (unlikely(err != ESP_OK)) {
        ESP_LOGE(TAG, "esp_partition_read failed (0x%x)", err);
        return RES_ERROR;
    }
    s_sectors_count[pdrv] = sectors_count_tmp_16;
    // For FAT32, the number of sectors is stored in a different field
    if (sectors_count_tmp_16 == 0){
        err = esp_partition_read(part, BPB_TotSec32, &sectors_count_tmp_32, sizeof(sectors_count_tmp_32));
        if (unlikely(err != ESP_OK)) {
            ESP_LOGE(TAG, "esp_partition_read failed (0x%x)", err);
            return RES_ERROR;
        }
        s_sectors_count[pdrv] = sectors_count_tmp_32;
    }

    s_initialized[pdrv] = true;
    return STA_PROTECT;
}

DSTATUS ff_raw_status (BYTE pdrv)
{
    DSTATUS status = STA_PROTECT;
    if (!s_initialized[pdrv]) {
        status |= STA_NOINIT | STA_NODISK;
    }
    return status;
}

DRESULT ff_raw_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    ESP_LOGV(TAG, "ff_raw_read - pdrv=%i, sector=%i, count=%in", (unsigned int)pdrv, (unsigned int)sector, (unsigned int)count);
    const esp_partition_t* part = s_ff_raw_handles[pdrv];
    assert(part);
    esp_err_t err = esp_partition_read(part, sector * s_sector_size[pdrv], buff, count * s_sector_size[pdrv]);
    if (unlikely(err != ESP_OK)) {
        ESP_LOGE(TAG, "esp_partition_read failed (0x%x)", err);
        return RES_ERROR;
    }
    return RES_OK;
}


DRESULT ff_raw_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    return RES_WRPRT;
}

DRESULT ff_raw_ioctl (BYTE pdrv, BYTE cmd, void *buff)
{
    const esp_partition_t* part = s_ff_raw_handles[pdrv];
    ESP_LOGV(TAG, "ff_raw_ioctl: cmd=%in", cmd);
    assert(part);
    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT:
            *((DWORD *) buff) = s_sectors_count[pdrv];
            return RES_OK;
        case GET_SECTOR_SIZE:
            *((WORD *) buff) = s_sector_size[pdrv];
            return RES_OK;
        case GET_BLOCK_SIZE:
            return RES_ERROR;
    }
    return RES_ERROR;
}


esp_err_t ff_diskio_register_raw_partition(BYTE pdrv, const esp_partition_t* part_handle)
{
    if (pdrv >= FF_VOLUMES) {
        return ESP_ERR_INVALID_ARG;
    }
    static const ff_diskio_impl_t raw_impl = {
        .init = &ff_raw_initialize,
        .status = &ff_raw_status,
        .read = &ff_raw_read,
        .write = &ff_raw_write,
        .ioctl = &ff_raw_ioctl
    };
    ff_diskio_register(pdrv, &raw_impl);
    s_ff_raw_handles[pdrv] = part_handle;
    return ESP_OK;

}


BYTE ff_diskio_get_pdrv_raw(const esp_partition_t* part_handle)
{
    for (int i = 0; i < FF_VOLUMES; i++) {
        if (part_handle == s_ff_raw_handles[i]) {
            return i;
        }
    }
    return 0xff;
}
