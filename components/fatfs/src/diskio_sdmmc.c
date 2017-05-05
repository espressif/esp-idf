// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "diskio.h"
#include "ffconf.h"
#include "ff.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"

static sdmmc_card_t* s_cards[_VOLUMES] = { NULL };

static const char* TAG = "diskio_sdmmc";

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

