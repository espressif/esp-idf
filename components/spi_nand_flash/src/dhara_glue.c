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

#include <dhara/nand.h>
#include <esp_check.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "spi_nand_oper.h"
#include "spi_nand_flash.h"
#include "nand.h"

#define ROM_WAIT_THRESHOLD_US 1000

static const char *TAG="dhara_glue";

esp_err_t wait_for_ready(spi_device_handle_t device, uint32_t expected_operation_time_us, uint8_t *status_out) {
  if (expected_operation_time_us < ROM_WAIT_THRESHOLD_US) esp_rom_delay_us(expected_operation_time_us);

  while (true) {
    uint8_t status;
    ESP_RETURN_ON_ERROR(spi_nand_read_register(device, REG_STATUS, &status), TAG, "");

    if ((status & STAT_BUSY) == 0) {
      if (status_out) *status_out = status;
      break;
    }

    if (expected_operation_time_us >= ROM_WAIT_THRESHOLD_US) {
      vTaskDelay(1);
    }
  }

  return ESP_OK;
}


static esp_err_t read_page_and_wait(spi_nand_flash_device_t *dev, uint32_t page, uint8_t *status_out) {
  ESP_RETURN_ON_ERROR(spi_nand_read_page(dev->config.device_handle,page), TAG, "");

  return wait_for_ready(dev->config.device_handle, dev->read_page_delay_us, status_out);
}

static esp_err_t program_execute_and_wait(spi_nand_flash_device_t *dev, uint32_t page, uint8_t *status_out) {
  ESP_RETURN_ON_ERROR(spi_nand_program_execute(dev->config.device_handle,page), TAG, "");

  return wait_for_ready(dev->config.device_handle, dev->program_page_delay_us, status_out);
}

int dhara_nand_is_bad(const struct dhara_nand *n, dhara_block_t b) {
  spi_nand_flash_device_t *dev = __containerof(n, spi_nand_flash_device_t, dhara_nand);

  dhara_page_t first_block_page = b * (1 << n->log2_ppb);
  uint16_t bad_block_indicator;
  esp_err_t ret;

  ESP_GOTO_ON_ERROR(read_page_and_wait(dev, first_block_page, NULL), fail, TAG, "");

  // Read the first 2 bytes on the OOB of the first page in the block. This should be 0xFFFF for a good block
  ESP_GOTO_ON_ERROR(spi_nand_read(dev->config.device_handle, (uint8_t *) &bad_block_indicator, dev->page_size, 2),
                    fail, TAG, "");

  ESP_LOGD(TAG, "is_bad, block=%d, page=%d,indicator = %04x", b, first_block_page, bad_block_indicator);
  return bad_block_indicator != 0xFFFF;

fail:
  ESP_LOGE(TAG, "Error in dhara_nand_is_bad %d",ret);
  return 1;
}

void dhara_nand_mark_bad(const struct dhara_nand *n, dhara_block_t b) {
  spi_nand_flash_device_t *dev = __containerof(n, spi_nand_flash_device_t, dhara_nand);
  esp_err_t ret;

  dhara_page_t first_block_page = b * (1 << n->log2_ppb);
  uint16_t bad_block_indicator = 0;
  ESP_LOGD(TAG, "mark_bad, block=%d, page=%d,indicator = %04x", b, first_block_page, bad_block_indicator);

  ESP_GOTO_ON_ERROR(spi_nand_write_enable(dev->config.device_handle), fail, TAG, "");
  ESP_GOTO_ON_ERROR(spi_nand_erase_block(dev->config.device_handle, first_block_page),
                    fail, TAG, "");

  ESP_GOTO_ON_ERROR(spi_nand_write_enable(dev->config.device_handle), fail, TAG, "");
  ESP_GOTO_ON_ERROR(spi_nand_program_load(dev->config.device_handle, (const uint8_t *) &bad_block_indicator,
                                          dev->page_size, 2),
                    fail, TAG, "");
  ESP_GOTO_ON_ERROR(program_execute_and_wait(dev, first_block_page, NULL), fail, TAG, "");
  return;
fail:
  ESP_LOGE(TAG, "Error in dhara_nand_mark_bad %d",ret);
}

int dhara_nand_erase(const struct dhara_nand *n, dhara_block_t b, dhara_error_t *err) {
  ESP_LOGD(TAG, "erase_block, block=%d,", b);
  spi_nand_flash_device_t *dev = __containerof(n, spi_nand_flash_device_t, dhara_nand);
  esp_err_t ret;

  dhara_page_t first_block_page = b * (1 << n->log2_ppb);
  uint8_t status;

  ESP_GOTO_ON_ERROR(spi_nand_write_enable(dev->config.device_handle), fail, TAG, "");
  ESP_GOTO_ON_ERROR(spi_nand_erase_block(dev->config.device_handle, first_block_page),
                    fail, TAG, "");
  ESP_GOTO_ON_ERROR(wait_for_ready(dev->config.device_handle,
                                   dev->erase_block_delay_us, &status),
                    fail, TAG, "");

  if ((status & STAT_ERASE_FAILED) != 0) {
    dhara_set_error(err, DHARA_E_BAD_BLOCK);
    return -1;
  }

  return 0;

fail:
  ESP_LOGE(TAG, "Error in dhara_nand_erase %d",ret);
  return -1;
}

int dhara_nand_prog(const struct dhara_nand *n, dhara_page_t p, const uint8_t *data, dhara_error_t *err) {
  ESP_LOGV(TAG, "prog, page=%d,", p);
  spi_nand_flash_device_t *dev = __containerof(n, spi_nand_flash_device_t, dhara_nand);
  esp_err_t ret;
  uint8_t status;
  uint16_t used_marker = 0;

  ESP_GOTO_ON_ERROR(read_page_and_wait(dev, p, NULL), fail, TAG, "");
  ESP_GOTO_ON_ERROR(spi_nand_write_enable(dev->config.device_handle), fail, TAG, "");
  ESP_GOTO_ON_ERROR(spi_nand_program_load(dev->config.device_handle, data,0, dev->page_size),
                    fail, TAG, "");
  ESP_GOTO_ON_ERROR(spi_nand_program_load(dev->config.device_handle, (uint8_t *)&used_marker,
                                          dev->page_size+2, 2),
                    fail, TAG, "");
  ESP_GOTO_ON_ERROR(program_execute_and_wait(dev, p, &status), fail, TAG, "");

  if ((status & STAT_PROGRAM_FAILED) != 0) {
    ESP_LOGD(TAG, "prog failed, page=%d,", p);
    dhara_set_error(err,DHARA_E_BAD_BLOCK);
    return -1;
  }

  return 0;
fail:
  ESP_LOGE(TAG, "Error in dhara_nand_prog %d",ret);
  return -1;
}

int dhara_nand_is_free(const struct dhara_nand *n, dhara_page_t p) {
  spi_nand_flash_device_t *dev = __containerof(n, spi_nand_flash_device_t, dhara_nand);
  esp_err_t ret;
  uint16_t used_marker;

  ESP_GOTO_ON_ERROR(read_page_and_wait(dev, p, NULL), fail, TAG, "");
  ESP_GOTO_ON_ERROR(spi_nand_read(dev->config.device_handle, (uint8_t *)&used_marker,
                                  dev->page_size + 2, 2),
                    fail, TAG, "");

  ESP_LOGD(TAG, "is free, page=%d, used_marker=%04x,", p, used_marker);
  return used_marker == 0xFFFF;
fail:
  ESP_LOGE(TAG, "Error in dhara_nand_is_free %d",ret);
  return 0;
}

int is_ecc_error(uint8_t status) {
  return (status & STAT_ECC1) != 0 && (status & STAT_ECC0) == 0;
}

int dhara_nand_read(const struct dhara_nand *n, dhara_page_t p, size_t offset, size_t length,
                    uint8_t *data, dhara_error_t *err) {
  ESP_LOGV(TAG, "read, page=%d, offset=%d, length=%d", p, offset, length);
  assert(p<n->num_blocks * (1 << n->log2_ppb));
  spi_nand_flash_device_t *dev = __containerof(n, spi_nand_flash_device_t, dhara_nand);
  esp_err_t ret;
  uint8_t status;

  ESP_GOTO_ON_ERROR(read_page_and_wait(dev, p, &status), fail, TAG, "");

  if (is_ecc_error(status)) {
    ESP_LOGD(TAG, "read ecc error, page=%d", p);
    dhara_set_error(err, DHARA_E_ECC);
    return -1;
  }

  ESP_GOTO_ON_ERROR(spi_nand_read(dev->config.device_handle, data, offset, length), fail, TAG, "");

  return 0;
fail:
  ESP_LOGE(TAG, "Error in dhara_nand_read %d",ret);
  return -1;
}

int dhara_nand_copy(const struct dhara_nand *n, dhara_page_t src, dhara_page_t dst, dhara_error_t *err) {
  ESP_LOGD(TAG, "copy, src=%d, dst=%d", src, dst);
  spi_nand_flash_device_t *dev = __containerof(n, spi_nand_flash_device_t, dhara_nand);
  esp_err_t ret;
  uint8_t status;

  ESP_GOTO_ON_ERROR(read_page_and_wait(dev, src, &status), fail, TAG, "");

  if (is_ecc_error(status)) {
    ESP_LOGD(TAG, "copy, ecc error");
    dhara_set_error(err, DHARA_E_ECC);
    return -1;
  }

  ESP_GOTO_ON_ERROR(spi_nand_write_enable(dev->config.device_handle), fail, TAG, "");
  ESP_GOTO_ON_ERROR(program_execute_and_wait(dev, dst, &status), fail, TAG, "");

  if ((status & STAT_PROGRAM_FAILED) != 0) {
    ESP_LOGD(TAG, "copy, prog failed");
    dhara_set_error(err, DHARA_E_BAD_BLOCK);
    return -1;
  }

  return 0;
fail:
  ESP_LOGE(TAG, "Error in dhara_nand_copy %d",ret);
  return -1;
}
