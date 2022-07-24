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

#pragma once

#define INVALID_PAGE 0xFFFF

struct spi_nand_flash_device_t {
  spi_nand_flash_config_t config;
  uint32_t block_size;
  uint32_t page_size;
  uint32_t num_blocks;
  struct dhara_map dhara_map;
  struct dhara_nand dhara_nand;
  uint8_t *work_buffer;
  uint32_t read_page_delay_us;
  uint32_t erase_block_delay_us;
  uint32_t program_page_delay_us;
};

esp_err_t wait_for_ready(spi_device_handle_t device, uint32_t expected_operation_time_us, uint8_t *status_out);