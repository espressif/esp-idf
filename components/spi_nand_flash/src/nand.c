// Copyright 2015-2022 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_check.h>
#include <string.h>
#include "spi_nand_flash.h"
#include "dhara/error.h"
#include "spi_nand_oper.h"
#include "nand.h"

static const char *TAG="nand";

esp_err_t spi_nand_winbond_init(spi_nand_flash_device_t *dev) {
  uint8_t device_id_buf[2];
  spi_nand_transaction_t t = {
      .command = CMD_READ_ID,
      .dummy_bits = 16,
      .miso_len = 2,
      .miso_data = device_id_buf
  };
  spi_nand_execute_transaction(dev->config.device_handle, &t);
  uint16_t device_id = (device_id_buf[0] << 8) + device_id_buf[1];
  dev->read_page_delay_us = 10;
  dev->erase_block_delay_us = 2500;
  dev->program_page_delay_us = 320;
  switch (device_id) {
    case 0xAA20:
    case 0xBA20:
      dev->dhara_nand.num_blocks = 512;
      break;
    case 0xAA21:
    case 0xBA21:
    case 0xBC21:
      dev->dhara_nand.num_blocks = 1024;
      break;
    default:
      return ESP_ERR_INVALID_RESPONSE;
  }
  return ESP_OK;
}

esp_err_t spi_nand_alliance_init(spi_nand_flash_device_t *dev) {
  uint8_t device_id;
  spi_nand_transaction_t t = {
      .command = CMD_READ_ID,
      .address = 1,
      .address_bytes = 1,
      .dummy_bits = 8,
      .miso_len = 1,
      .miso_data = &device_id
  };
  spi_nand_execute_transaction(dev->config.device_handle, &t);
  dev->erase_block_delay_us = 3000;
  dev->program_page_delay_us = 630;
  switch (device_id) {
    case 0x25: //AS5F31G04SND-08LIN
      dev->dhara_nand.num_blocks = 1024;
      dev->read_page_delay_us = 60;
      break;
    case 0x2E: //AS5F32G04SND-08LIN
    case 0x8E: //AS5F12G04SND-10LIN
      dev->dhara_nand.num_blocks = 2048;
      dev->read_page_delay_us = 60;
      break;
    case 0x2F: //AS5F34G04SND-08LIN
    case 0x8F: //AS5F14G04SND-10LIN
      dev->dhara_nand.num_blocks = 4096;
      dev->read_page_delay_us = 60;
      break;
    case 0x2D: //AS5F38G04SND-08LIN
    case 0x8D: //AS5F18G04SND-10LIN
      dev->dhara_nand.log2_page_size = 12; // 4k pages
      dev->dhara_nand.num_blocks = 4096;
      dev->read_page_delay_us = 130; // somewhat slower reads
      break;
    default:
      return ESP_ERR_INVALID_RESPONSE;
  }
  return ESP_OK;
}

esp_err_t spi_nand_gigadevice_init(spi_nand_flash_device_t *dev) {
  uint8_t device_id;
  spi_nand_transaction_t t = {
      .command = CMD_READ_ID,
      .dummy_bits = 16,
      .miso_len = 1,
      .miso_data = &device_id
  };
  spi_nand_execute_transaction(dev->config.device_handle, &t);
  dev->read_page_delay_us = 25;
  dev->erase_block_delay_us = 3200;
  dev->program_page_delay_us = 380;
  switch (device_id) {
    case 0x51:
    case 0x41:
    case 0x31:
    case 0x21:
      dev->dhara_nand.num_blocks = 1024;
      break;
    case 0x52:
    case 0x42:
    case 0x32:
    case 0x22:
      dev->dhara_nand.num_blocks = 2048;
      break;
    case 0x55:
    case 0x45:
    case 0x35:
    case 0x25:
      dev->dhara_nand.num_blocks = 4096;
      break;
    default:
      return ESP_ERR_INVALID_RESPONSE;
  }
  return ESP_OK;
}

esp_err_t detect_chip(spi_nand_flash_device_t *dev) {
  uint8_t manufacturer_id;
  spi_nand_transaction_t t = {
      .command = CMD_READ_ID,
      .address = 0, // This normally selects the manufacturer id. Some chips ignores it, but still expects 8 dummy bits here
      .address_bytes = 1,
      .miso_len = 1,
      .miso_data = &manufacturer_id
  };
  spi_nand_execute_transaction(dev->config.device_handle, &t);

  switch (manufacturer_id) {
    case 0x52: // Alliance
      return spi_nand_alliance_init(dev);
    case 0xEF: // Winbond
      return spi_nand_winbond_init(dev);
    case 0xC8: // GigaDevice
      return spi_nand_gigadevice_init(dev);
    default:
      return ESP_ERR_INVALID_RESPONSE;
  }
}

esp_err_t unprotect_chip(spi_nand_flash_device_t *dev) {
  uint8_t status;
  esp_err_t ret = spi_nand_read_register(dev->config.device_handle, REG_PROTECT, &status);
  if (ret != ESP_OK) return ret;

  if (status != 0x00) {
    ret = spi_nand_write_register(dev->config.device_handle, REG_PROTECT, 0);
  }

  return ret;
}

esp_err_t spi_nand_flash_init_device(spi_nand_flash_config_t *config, spi_nand_flash_device_t **handle) {
  ESP_RETURN_ON_FALSE(config->device_handle!=NULL, ESP_ERR_INVALID_ARG, TAG, "Spi device pointer can not be NULL");

  if (!config->gc_factor) config->gc_factor = 45;

  *handle = calloc(sizeof(spi_nand_flash_device_t),1);
  if (*handle == NULL) {
    return ESP_ERR_NO_MEM;
  }

  memcpy(&(*handle)->config, config, sizeof(spi_nand_flash_config_t));

  (*handle)->dhara_nand.log2_ppb = 6; // 64 pages per block is standard
  (*handle)->dhara_nand.log2_page_size = 11;  // 2048 bytes per page is fairly standard

  esp_err_t ret = ESP_OK;

  ESP_GOTO_ON_ERROR(detect_chip(*handle),fail,TAG,"Failed to detect nand chip");
  ESP_GOTO_ON_ERROR(unprotect_chip(*handle),fail,TAG,"Failed to clear protection register");

  (*handle)->page_size = 1 << (*handle)->dhara_nand.log2_page_size;
  (*handle)->block_size = (1 << (*handle)->dhara_nand.log2_ppb) * (*handle)->page_size;
  (*handle)->num_blocks = (*handle)->dhara_nand.num_blocks;
  (*handle)->work_buffer = malloc((*handle)->page_size);

  ESP_GOTO_ON_FALSE((*handle)->work_buffer != NULL,ESP_ERR_NO_MEM, fail, TAG, "nomem");

  dhara_map_init(&(*handle)->dhara_map, &(*handle)->dhara_nand,(*handle)->work_buffer,config->gc_factor);

  dhara_error_t ignored;
  dhara_map_resume(&(*handle)->dhara_map, &ignored);

  return ret;

fail:
  if ((*handle)->work_buffer != NULL) free((*handle)->work_buffer);
  free(*handle);
  return ret;
}

esp_err_t spi_nand_erase_chip(spi_nand_flash_device_t *handle) {
  ESP_LOGW(TAG, "Entire chip is being erased");
  esp_err_t ret;

  for (int i = 0; i < handle->num_blocks; i++) {
    ESP_GOTO_ON_ERROR(spi_nand_write_enable(handle->config.device_handle), fail, TAG, "");
    ESP_GOTO_ON_ERROR(spi_nand_erase_block(handle->config.device_handle, i * (1 << handle->dhara_nand.log2_ppb)),
                      fail, TAG, "");
    ESP_GOTO_ON_ERROR(wait_for_ready(handle->config.device_handle, handle->erase_block_delay_us,NULL),
                      fail, TAG, "");
  }

  // clear dhara map
  dhara_map_init(&handle->dhara_map, &handle->dhara_nand,handle->work_buffer,handle->config.gc_factor);
  dhara_map_clear(&handle->dhara_map);

  return ESP_OK;
fail:
  return ret;
}

esp_err_t spi_nand_flash_read_sector(spi_nand_flash_device_t *handle, uint8_t *buffer, uint16_t sector_id) {
  dhara_error_t err;
  if (dhara_map_read(&handle->dhara_map,sector_id, buffer, &err)) {
    return ESP_ERR_FLASH_BASE + err;
  }
  return ESP_OK;
}

esp_err_t spi_nand_flash_write_sector(spi_nand_flash_device_t *handle, const uint8_t *buffer, uint16_t sector_id) {
  dhara_error_t err;
  if (dhara_map_write(&handle->dhara_map,sector_id, buffer, &err)) {
    return ESP_ERR_FLASH_BASE + err;
  }
  return ESP_OK;
}

esp_err_t spi_nand_flash_sync(spi_nand_flash_device_t *handle) {
  dhara_error_t err;
  if (dhara_map_sync(&handle->dhara_map,&err)) {
    return ESP_ERR_FLASH_BASE + err;
  }
  return ESP_OK;
}

esp_err_t spi_nand_flash_get_capacity(spi_nand_flash_device_t *handle, uint16_t *number_of_sectors) {
  *number_of_sectors = dhara_map_capacity(&handle->dhara_map);
  return ESP_OK;
}

esp_err_t spi_nand_flash_get_sector_size(spi_nand_flash_device_t *handle, uint16_t *sector_size) {
  *sector_size = handle->page_size;
  return ESP_OK;
}

esp_err_t spi_nand_flash_deinit_device(spi_nand_flash_device_t *handle) {
  free(handle->work_buffer);
  free(handle);
  return ESP_OK;
}