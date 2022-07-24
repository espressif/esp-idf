// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

#include <ff.h>
#include <diskio.h>
#include <esp_log.h>
#include <esp_check.h>
#include "diskio_nand.h"
#include "spi_nand_flash.h"
#include "diskio_impl.h"

static const char *TAG = "diskio_nand";

spi_nand_flash_device_t *ff_nand_handles[FF_VOLUMES] = {NULL};

DSTATUS ff_nand_initialize(BYTE pdrv) {
  return 0;
}

DSTATUS ff_nand_status(BYTE pdrv) {
  return 0;
}

DRESULT ff_nand_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count) {
  ESP_LOGV(TAG, "ff_nand_read - pdrv=%i, sector=%i, count=%i", (unsigned int) pdrv, (unsigned int) sector,
           (unsigned int) count);
  esp_err_t ret;
  uint16_t sector_size;
  spi_nand_flash_device_t *dev = ff_nand_handles[pdrv];
  assert(dev);

  ESP_GOTO_ON_ERROR(spi_nand_flash_get_sector_size(dev, &sector_size), fail, TAG, "");

  for (int i = 0; i < count; i++) {
    ESP_GOTO_ON_ERROR(spi_nand_flash_read_sector(dev,buff + i * sector_size,sector+i),
                      fail, TAG, "spi_nand_flash_read failed");
  }

  return RES_OK;

fail:
  ESP_LOGE(TAG, "ff_nand_read failed with error 0x%X",ret);
  return RES_ERROR;
}

DRESULT ff_nand_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count) {
  ESP_LOGV(TAG, "ff_nand_write - pdrv=%i, sector=%i, count=%i", (unsigned int) pdrv, (unsigned int) sector,
           (unsigned int) count);
  esp_err_t ret;
  uint16_t sector_size;
  spi_nand_flash_device_t *dev = ff_nand_handles[pdrv];
  assert(dev);

  ESP_GOTO_ON_ERROR(spi_nand_flash_get_sector_size(dev, &sector_size), fail, TAG, "");

  for (int i = 0; i < count; i++) {
    ESP_GOTO_ON_ERROR(spi_nand_flash_write_sector(dev,buff + i * sector_size,sector+i),
                      fail, TAG, "spi_nand_flash_write failed");
  }
  return RES_OK;

fail:
  ESP_LOGE(TAG, "ff_nand_write failed with error 0x%X",ret);
  return RES_ERROR;
}

DRESULT ff_nand_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
  spi_nand_flash_device_t *dev = ff_nand_handles[pdrv];
  assert(dev);

  ESP_LOGV(TAG, "ff_nand_ioctl: cmd=%i", cmd);
  esp_err_t ret = ESP_OK;
  switch (cmd) {
    case CTRL_SYNC:
      ESP_GOTO_ON_ERROR(spi_nand_flash_sync(dev),fail, TAG, "sync failed");
      break;
    case GET_SECTOR_COUNT: {
      uint16_t num_sectors;
      ESP_GOTO_ON_ERROR(spi_nand_flash_get_capacity(dev, &num_sectors),
                        fail, TAG, "get_capacity failed");
      *((DWORD*)buff) = num_sectors;
      ESP_LOGV(TAG, "capacity=%d", *((DWORD *) buff));
      break;
    }
    case GET_SECTOR_SIZE: {
      uint16_t capacity;
      ESP_GOTO_ON_ERROR(spi_nand_flash_get_sector_size(dev, &capacity),
                        fail, TAG, "get_sector_size failed");
      *((WORD*)buff) = capacity;
      ESP_LOGV(TAG, "sector size=%d", *((WORD*)buff));
      break;
    }
    default:
      return RES_ERROR;
  }
  return RES_OK;

fail:
  ESP_LOGE(TAG, "ff_nand_ioctl cmd=%i, failed with error=0x%X",cmd, ret);
  return RES_ERROR;
}


esp_err_t ff_diskio_register_nand(BYTE pdrv, spi_nand_flash_device_t *device) {
  if (pdrv >= FF_VOLUMES) {
    return ESP_ERR_INVALID_ARG;
  }

  static const ff_diskio_impl_t nand_impl = {
      .init = &ff_nand_initialize,
      .status = &ff_nand_status,
      .read = &ff_nand_read,
      .write = &ff_nand_write,
      .ioctl = &ff_nand_ioctl
  };
  ff_nand_handles[pdrv] = device;
  ff_diskio_register(pdrv, &nand_impl);
  return ESP_OK;
}

BYTE ff_diskio_get_pdrv_nand(const spi_nand_flash_device_t *dev) {
  for (int i = 0; i < FF_VOLUMES; i++) {
    if (dev == ff_nand_handles[i]) {
      return i;
    }
  }
  return 0xff;
}

void ff_diskio_clear_pdrv_nand(const spi_nand_flash_device_t *dev) {
  for (int i = 0; i < FF_VOLUMES; i++) {
    if (dev == ff_nand_handles[i]) {
      ff_nand_handles[i] = NULL;
    }
  }
}
