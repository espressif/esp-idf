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

#include "Flash_Emulator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Flash_Emulator::Flash_Emulator(size_t size, size_t sector_sise)
{
    this->reset_count = 0x7fffffff;
    this->size = size;
    this->sector_sise = sector_sise;
    this->buff = (uint8_t *)malloc(this->size);
    this->access_count = new uint32_t[this->size / this->sector_sise];
    memset(this->access_count, 0, this->size / this->sector_sise * sizeof(uint32_t));
}

size_t Flash_Emulator::chip_size()
{
    return this->size;
}
size_t Flash_Emulator::sector_size()
{
    return this->sector_sise;
}

esp_err_t Flash_Emulator::erase_sector(size_t sector)
{
    esp_err_t result = ESP_OK;
    if ((this->reset_count != 0x7fffffff) && (this->reset_count != 0)) {
        this->reset_count--;
    }
    if (this->reset_count <= 0) {
        result = ESP_FAIL;
        return result;
    }
    memset(&this->buff[sector * this->sector_sise], -1, this->sector_sise);
    this->access_count[sector]++;
    return result;
}

uint32_t Flash_Emulator::get_access_minmax()
{
    uint32_t min = INT32_MAX;
    uint32_t max = 0;
    for (size_t i = 0; i < (this->size / this->sector_sise) - 2; i++) {
        if (this->access_count[i] < min) {
            min = this->access_count[i];
        }
        if (this->access_count[i] > max) {
            max = this->access_count[i];
        }
    }
    return max - min;
}

esp_err_t Flash_Emulator::erase_range(size_t start_address, size_t size)
{
    esp_err_t result = ESP_OK;
    uint32_t start_sector = start_address / this->sector_sise;
    uint32_t count = (size + this->sector_sise - 1) / this->sector_sise;
    for (size_t i = 0; i < count; i++) {
        result |= this->erase_sector(start_sector + i);
    }
    return result;
}

esp_err_t Flash_Emulator::write(size_t dest_addr, const void *src, size_t size)
{
    esp_err_t result = ESP_OK;
    if ((this->reset_count != 0x7fffffff) && (this->reset_count != 0)) {
        this->reset_count--;
    }
    if (this->reset_count <= 0) {
        result = ESP_FAIL;
        return result;
    }
    memcpy(&this->buff[dest_addr], src, size);
    return result;
}

esp_err_t Flash_Emulator::read(size_t src_addr, void *dest, size_t size)
{
    esp_err_t result = ESP_OK;
    if (this->reset_count <= 0) {
        result = ESP_FAIL;
        return result;
    }
    memcpy(dest, &this->buff[src_addr], size);
    return result;
}

Flash_Emulator::~Flash_Emulator()
{
    free(this->buff);
    delete this->access_count;
}

void Flash_Emulator::SetResetCount(uint32_t count)
{
    this->reset_count = count;
}

void Flash_Emulator::SetResetSector(size_t sector)
{
    this->reset_sector = sector;
}
