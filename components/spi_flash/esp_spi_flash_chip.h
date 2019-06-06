// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

/* Private header with chip-specific routines for SPI flash interaction */
#pragma once

#include <stdlib.h>
#include <stdint.h>
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/spi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/rom/spi_flash.h"
#endif

esp_rom_spiflash_result_t spi_flash_write_encrypted_chip(size_t dest_addr, const void *src, size_t size);

esp_rom_spiflash_result_t IRAM_ATTR spi_flash_unlock();

