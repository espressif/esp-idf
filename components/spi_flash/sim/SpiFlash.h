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

#ifndef _SpiFlash_H_
#define _SpiFlash_H_

#include "esp_err.h"
#include "rom/spi_flash.h"

/**
* @brief This class is used to emulate flash devices.
*
*/
class SpiFlash
{

public:
    SpiFlash();
    ~SpiFlash();

    void init(size_t chip_size, size_t block_size, size_t sector_size, size_t page_size, const char* partitions_bin);

    esp_rom_spiflash_result_t erase_block(uint32_t block);
    esp_rom_spiflash_result_t erase_sector(uint32_t sector);
    esp_rom_spiflash_result_t erase_page(uint32_t page);

    esp_rom_spiflash_result_t write(size_t dest_addr, const void *src, size_t size);
    esp_rom_spiflash_result_t read(size_t src_addr, void *dest, size_t size);

    size_t get_chip_size();
    size_t get_sector_size();

    uint8_t* get_memory_ptr(uint32_t src_address);

private:
    bool inited = false;

    size_t chip_size;
    size_t block_size;
    size_t sector_size;
    size_t page_size;

    uint8_t* memory;

    void* partitions;
    uint8_t partitions_num;

    void deinit();
};

#endif // _SpiFlash_H_
