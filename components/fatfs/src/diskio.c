/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/* ESP-IDF port Copyright 2016 Espressif Systems (Shanghai) PTE LTD      */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include <string.h>
#include "diskio.h"		/* FatFs lower layer API */
#include "ffconf.h"
#include "ff.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"
#include <time.h>
#include <sys/time.h>

static const char* TAG = "ff_diskio";
static ff_diskio_impl_t * s_impls[_VOLUMES];
static sdmmc_card_t* s_cards[_VOLUMES] = { NULL };
static bool s_impls_initialized = false;

PARTITION VolToPart[] = {
    {0, 1},    /* Logical drive 0 ==> Physical drive 0, 1st partition */
    {1, 0}     /* Logical drive 1 ==> Physical drive 1, auto detection */
};

esp_err_t ff_diskio_get_drive(BYTE* out_pdrv)
{
    BYTE i;
    for(i=0; i<_VOLUMES; i++) {
        if (!s_impls[i]) {
            *out_pdrv = i;
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

void ff_diskio_register(BYTE pdrv, const ff_diskio_impl_t* discio_impl)
{
    assert(pdrv < _VOLUMES);

    if (!s_impls_initialized) {
        s_impls_initialized = true;
        memset(s_impls, 0, _VOLUMES * sizeof(ff_diskio_impl_t*));
    }

    if (s_impls[pdrv]) {
        ff_diskio_impl_t* im = s_impls[pdrv];
        s_impls[pdrv] = NULL;
        free(im);
    }

    if (!discio_impl) {
        return;
    }

    ff_diskio_impl_t * impl = (ff_diskio_impl_t *)malloc(sizeof(ff_diskio_impl_t));
    assert(impl != NULL);
    memcpy(impl, discio_impl, sizeof(ff_diskio_impl_t));
    s_impls[pdrv] = impl;
}

DSTATUS ff_disk_initialize (BYTE pdrv)
{
    return s_impls[pdrv]->init(pdrv);
}
DSTATUS ff_disk_status (BYTE pdrv)
{
    return s_impls[pdrv]->status(pdrv);
}
DRESULT ff_disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
    return s_impls[pdrv]->read(pdrv, buff, sector, count);
}
DRESULT ff_disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
    return s_impls[pdrv]->write(pdrv, buff, sector, count);
}
DRESULT ff_disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
    return s_impls[pdrv]->ioctl(pdrv, cmd, buff);
}

DWORD get_fattime(void)
{
    time_t t = time(NULL);
    struct tm *tmr = gmtime(&t);
    int year = tmr->tm_year < 80 ? 0 : tmr->tm_year - 80;
    return    ((DWORD)(year) << 25)
            | ((DWORD)(tmr->tm_mon + 1) << 21)
            | ((DWORD)tmr->tm_mday << 16)
            | (WORD)(tmr->tm_hour << 11)
            | (WORD)(tmr->tm_min << 5)
            | (WORD)(tmr->tm_sec >> 1);
}

DSTATUS ff_sdmmc_initialize (BYTE pdrv)
{
    return 0;
}

DSTATUS ff_sdmmc_status (BYTE pdrv)
{
    return 0;
}

DRESULT ff_sdmmc_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
    sdmmc_card_t* card = s_cards[pdrv];
    assert(card);
    esp_err_t err = sdmmc_read_sectors(card, buff, sector, count);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "sdmmc_read_blocks failed (%d)", err);
        return RES_ERROR;
    }
    return RES_OK;
}

DRESULT ff_sdmmc_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
    sdmmc_card_t* card = s_cards[pdrv];
    assert(card);
    esp_err_t err = sdmmc_write_sectors(card, buff, sector, count);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "sdmmc_write_blocks failed (%d)", err);
        return RES_ERROR;
    }
    return RES_OK;
}

DRESULT ff_sdmmc_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
    sdmmc_card_t* card = s_cards[pdrv];
    assert(card);
    switch(cmd) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT:
            *((uint32_t*) buff) = card->csd.capacity;
            return RES_OK;
        case GET_SECTOR_SIZE:
            *((uint32_t*) buff) = card->csd.sector_size;
            return RES_OK;
        case GET_BLOCK_SIZE:
            return RES_ERROR;
    }
    return RES_ERROR;
}

void ff_diskio_register_sdmmc(BYTE pdrv, sdmmc_card_t* card)
{
    static const ff_diskio_impl_t sdmmc_impl = {
        .init = &ff_sdmmc_initialize,
        .status = &ff_sdmmc_status,
        .read = &ff_sdmmc_read,
        .write = &ff_sdmmc_write,
        .ioctl = &ff_sdmmc_ioctl
    };
    s_cards[pdrv] = card;
    ff_diskio_register(pdrv, &sdmmc_impl);
}

