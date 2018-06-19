// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "SpiFlash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include "sdkconfig.h"
#include "esp_flash_data_types.h"

using namespace std;

SpiFlash::SpiFlash()
{
    return;
}

SpiFlash::~SpiFlash()
{
    deinit();
}

void SpiFlash::init(size_t chip_size, size_t block_size, size_t sector_size, size_t page_size, const char* partitions_bin)
{
    // De-initialize first
    deinit();

    this->chip_size = chip_size;
    this->block_size = block_size;
    this->sector_size = sector_size;
    this->page_size = page_size;

    this->memory = (uint8_t *) malloc(this->chip_size);
    memset(this->memory, 0xFF, this->chip_size);

    ifstream ifd(partitions_bin, ios::binary | ios::ate);
    int size = ifd.tellg();

    ifd.seekg(0, ios::beg);
    vector<char> buffer;

    buffer.resize(size);

    ifd.read(buffer.data(), size);

    memcpy(&this->memory[CONFIG_PARTITION_TABLE_OFFSET], buffer.data(), buffer.size());
}

void SpiFlash::deinit()
{
    if(inited)
    {
        free(this->memory);
    }
}

size_t SpiFlash::get_chip_size()
{
    return this->chip_size;
}

size_t SpiFlash::get_sector_size()
{
    return this->sector_size;
}

esp_rom_spiflash_result_t SpiFlash::erase_block(uint32_t block)
{
    memset(&this->memory[block * this->block_size], 0xFF, this->block_size);
    return ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t SpiFlash::erase_sector(size_t sector)
{
    memset(&this->memory[sector * this->sector_size], 0xFF, this->sector_size);
    return ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t SpiFlash::erase_page(uint32_t page)
{
    memset(&this->memory[page * this->page_size], 0xFF, this->page_size);
    return ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t SpiFlash::write(size_t dest_addr, const void *src, size_t size)
{
    // Emulate inability to set programmed bits without erasing
    for(uint32_t ctr = 0; ctr < size; ctr++)
    {
        uint8_t data = ((uint8_t*)src)[ctr];
        uint8_t written = this->memory[dest_addr + ctr];

        data &= written;

        this->memory[dest_addr + ctr] = data;
    }

    return ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t SpiFlash::read(size_t src_addr, void *dest, size_t size)
{
    memcpy(dest, &this->memory[src_addr], size);
    return ESP_ROM_SPIFLASH_RESULT_OK;
}

uint8_t* SpiFlash::get_memory_ptr(uint32_t src_address)
{
    return &this->memory[src_address];
}