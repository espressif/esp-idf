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


#ifndef _Flash_Access_H_
#define _Flash_Access_H_
#include "esp_err.h"

/**
* @brief Universal flash access interface class
*
*/
class Flash_Access
{
public:
    virtual size_t chip_size() = 0;

    virtual esp_err_t erase_sector(size_t sector) = 0;
    virtual esp_err_t erase_range(size_t start_address, size_t size) = 0;

    virtual esp_err_t write(size_t dest_addr, const void *src, size_t size) = 0;
    virtual esp_err_t read(size_t src_addr, void *dest, size_t size) = 0;

    virtual size_t sector_size() = 0;

    virtual esp_err_t flush()
    {
        return ESP_OK;
    };

    virtual ~Flash_Access() {};
};

#endif // _Flash_Access_H_