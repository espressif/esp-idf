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

#ifndef _WL_Flash_H_
#define _WL_Flash_H_

#include "esp_err.h"
#include "Flash_Access.h"
#include "WL_Config.h"
#include "WL_State.h"

/**
* @brief This class is used to make wear levelling for flash devices. Class implements Flash_Access interface
*
*/
class WL_Flash : public Flash_Access
{
public :
    WL_Flash();
    ~WL_Flash() override;

    virtual esp_err_t config(wl_config_t *cfg, Flash_Access *flash_drv);
    virtual esp_err_t init();

    size_t chip_size() override;
    size_t sector_size() override;


    esp_err_t erase_sector(size_t sector) override;
    esp_err_t erase_range(size_t start_address, size_t size) override;

    esp_err_t write(size_t dest_addr, const void *src, size_t size) override;
    esp_err_t read(size_t src_addr, void *dest, size_t size) override;

    esp_err_t flush() override;

    Flash_Access *get_drv();
    wl_config_t *get_cfg();

protected:
    bool configured = false;
    bool initialized = false;
    wl_state_t state;
    wl_config_t cfg;
    Flash_Access *flash_drv = NULL;

    size_t addr_cfg;
    size_t addr_state1;
    size_t addr_state2;
    size_t index_state1;
    size_t index_state2;

    size_t flash_size;
    uint32_t state_size;
    uint32_t cfg_size;
    uint8_t *temp_buff = NULL;
    size_t dummy_addr;
    uint8_t used_bits;

    esp_err_t initSections();
    esp_err_t updateWL();
    esp_err_t recoverPos();
    size_t calcAddr(size_t addr);
};

#endif // _WL_Flash_H_
