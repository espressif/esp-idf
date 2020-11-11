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
#include "esp_flash_partitions.h"

using namespace std;

#define DIV_AND_CEIL(x, y)              ((x) / (y) + ((x) % (y) > 0))

SpiFlash::SpiFlash()
{
    return;
}

SpiFlash::~SpiFlash()
{
    deinit();
}

void SpiFlash::init(uint32_t chip_size, uint32_t block_size, uint32_t sector_size, uint32_t page_size, const char* partitions_bin)
{
    // De-initialize first
    deinit();

    // Initialize values and alloc memory
    this->chip_size = chip_size;
    this->block_size = block_size;
    this->sector_size = sector_size;
    this->page_size = page_size;

    this->blocks = DIV_AND_CEIL(this->chip_size, this->block_size);
    this->sectors = DIV_AND_CEIL(this->chip_size, this->sector_size);
    this->pages = DIV_AND_CEIL(this->chip_size, this->page_size);

    this->erase_cycles = (uint32_t*) calloc(this->sectors, sizeof(uint32_t));
    this->erase_states = (bool*) calloc(this->sectors, sizeof(bool));
    memset(this->erase_states, 0xFF, this->sectors * sizeof(bool));

    this->total_erase_cycles_limit = 0;
    this->erase_cycles_limit = 0;

    this->total_erase_cycles = 0;

    // Load partitions table bin
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
    // Free all allocated memory
    free(this->memory);

    free(this->erase_cycles);
    free(this->erase_states);
}

uint32_t SpiFlash::get_chip_size()
{
    return this->chip_size;
}

uint32_t SpiFlash::get_block_size()
{
    return this->block_size;
}

uint32_t SpiFlash::get_sector_size()
{
    return this->sector_size;
}

uint32_t SpiFlash::get_page_size()
{
    return this->page_size;
}

esp_rom_spiflash_result_t SpiFlash::erase_block(uint32_t block)
{
    uint32_t sectors_per_block = (this->block_size / this->sector_size);
    uint32_t start_sector = block * sectors_per_block;

    for (int i = start_sector; i < start_sector + sectors_per_block; i++) {
        this->erase_sector(i);
    }

    return ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t SpiFlash::erase_sector(uint32_t sector)
{
    if (this->total_erase_cycles_limit != 0 &&
        this->total_erase_cycles >= this->total_erase_cycles_limit) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    if (this->erase_cycles_limit != 0 &&
        this->erase_cycles[sector] >= this->erase_cycles_limit) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    uint32_t pages_per_sector = (this->sector_size / this->page_size);
    uint32_t start_page = sector * pages_per_sector;

    if (this->erase_states[sector]) {
        goto out;
    }

    for (int i = start_page; i < start_page + pages_per_sector; i++) {
        this->erase_page(i);
    }

    this->erase_cycles[sector]++;
    this->total_erase_cycles++;

    this->erase_states[sector] = true;

out:
    return ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t SpiFlash::erase_page(uint32_t page)
{
    memset(&this->memory[page * this->page_size], 0xFF, this->page_size);
    return ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t SpiFlash::write(uint32_t dest_addr, const void *src, uint32_t size)
{
    // Update reset states and check for failure
    int start = 0;
    int end = 0;

    if (this->total_erase_cycles_limit != 0 &&
        this->total_erase_cycles >= this->total_erase_cycles_limit) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    start = dest_addr / this->get_sector_size();
    end = size > 0 ? (dest_addr + size - 1) / this->get_sector_size() : start;

    for (int i = start; i <= end; i++) {
        if (this->erase_cycles_limit != 0 &&
            this->erase_cycles[i] >= this->erase_cycles_limit) {
            return ESP_ROM_SPIFLASH_RESULT_ERR;
        }

        this->erase_states[i] = false;
    }

    // Do the write
    for(uint32_t ctr = 0; ctr < size; ctr++)
    {
        uint8_t data = ((uint8_t*)src)[ctr];
        uint8_t written = this->memory[dest_addr + ctr];

        // Emulate inability to set programmed bits without erasing
        data &= written;

        this->memory[dest_addr + ctr] = data;
    }

    return ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_rom_spiflash_result_t SpiFlash::read(uint32_t src_addr, void *dest, uint32_t size)
{
    // Check for failure
    int start = 0;
    int end = 0;

    if (this->total_erase_cycles_limit != 0 &&
        this->total_erase_cycles >= this->total_erase_cycles_limit) {
        return ESP_ROM_SPIFLASH_RESULT_ERR;
    }

    start = src_addr / this->get_sector_size();
    end = size > 0 ? (src_addr + size - 1) / this->get_sector_size() : start;

    for (int i = start; i <= end; i++) {
        if (this->erase_cycles_limit != 0 &&
            this->erase_cycles[i] >= this->erase_cycles_limit) {
            return ESP_ROM_SPIFLASH_RESULT_ERR;
        }
    }

    // Do the read
    memcpy(dest, &this->memory[src_addr], size);
    return ESP_ROM_SPIFLASH_RESULT_OK;
}

uint8_t* SpiFlash::get_memory_ptr(uint32_t src_address)
{
    return &this->memory[src_address];
}

uint32_t SpiFlash::get_erase_cycles(uint32_t sector)
{
    return this->erase_cycles[sector];
}

uint32_t SpiFlash::get_total_erase_cycles()
{
    return this->total_erase_cycles;
}

void SpiFlash::set_erase_cycles_limit(uint32_t limit)
{
    this->erase_cycles_limit = limit;
}

void SpiFlash::set_total_erase_cycles_limit(uint32_t limit)
{
    this->total_erase_cycles_limit = limit;
}

void SpiFlash::reset_erase_cycles()
{
    memset(this->erase_cycles, 0, sizeof(this->sectors * sizeof(uint32_t)));
}

void SpiFlash::reset_total_erase_cycles()
{
    this->total_erase_cycles = 0;
}
