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

#ifndef _Flash_Emulator_H_
#define _Flash_Emulator_H_

#include "esp_err.h"
#include "Flash_Access.h"
/**
* @brief This class is used to emulate flash devices. Class implements Flash_Access interface
*
*/
class Flash_Emulator : public Flash_Access
{

public:
    Flash_Emulator(size_t size, size_t sector_sise);

    virtual size_t chip_size();

    virtual esp_err_t erase_sector(size_t sector);
    virtual esp_err_t erase_range(size_t start_address, size_t size);

    virtual esp_err_t write(size_t dest_addr, const void *src, size_t size);
    virtual esp_err_t read(size_t src_addr, void *dest, size_t size);

    virtual size_t sector_size();

    virtual ~Flash_Emulator();

    uint32_t get_access_minmax();
public:
    size_t size;
    size_t sector_sise;
    uint8_t *buff;

    uint32_t *access_count;

public:
    uint32_t reset_count;
    size_t reset_sector;
    void SetResetCount(uint32_t count);
    void SetResetSector(size_t sector);

};

#endif // _Flash_Emulator_H_
