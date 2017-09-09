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

#include "WL_Ext_Safe.h"
#include <stdlib.h>
#include "esp_log.h"

static const char *TAG = "wl_ext_safe";

#define WL_EXT_RESULT_CHECK(result) \
    if (result != ESP_OK) { \
        ESP_LOGE(TAG,"%s(%d): result = 0x%08x", __FUNCTION__, __LINE__, result); \
        return (result); \
    }

#ifndef FLASH_ERASE_VALUE
#define FLASH_ERASE_VALUE 0xffffffff
#endif // FLASH_ERASE_VALUE


#ifndef WL_EXT_SAFE_OK
#define WL_EXT_SAFE_OK 0x12345678
#endif // WL_EXT_SAFE_OK

#ifndef WL_EXT_SAFE_OFFSET
#define WL_EXT_SAFE_OFFSET 16
#endif // WL_EXT_SAFE_OFFSET


struct WL_Ext_Safe_State {
public:
    uint32_t erase_begin;
    uint32_t local_addr_base;
    uint32_t local_addr_shift;
    uint32_t count;
};

WL_Ext_Safe::WL_Ext_Safe(): WL_Ext_Perf()
{
}

WL_Ext_Safe::~WL_Ext_Safe()
{
}

esp_err_t WL_Ext_Safe::config(WL_Config_s *cfg, Flash_Access *flash_drv)
{
    esp_err_t result = ESP_OK;

    result = WL_Ext_Perf::config(cfg, flash_drv);
    WL_EXT_RESULT_CHECK(result);
    this->state_addr = WL_Flash::chip_size() - 2 * WL_Flash::sector_size();
    this->dump_addr = WL_Flash::chip_size() - 1 * WL_Flash::sector_size();
    return ESP_OK;
}

esp_err_t WL_Ext_Safe::init()
{
    esp_err_t result = ESP_OK;
    ESP_LOGV(TAG, "%s", __func__);

    result = WL_Ext_Perf::init();
    WL_EXT_RESULT_CHECK(result);

    result = this->recover();
    return result;
}

size_t WL_Ext_Safe::chip_size()
{
    ESP_LOGV(TAG, "%s size = %i", __func__, WL_Flash::chip_size() - 2 * this->flash_sector_size);
    return WL_Flash::chip_size() - 2 * this->flash_sector_size;
}

esp_err_t WL_Ext_Safe::recover()
{
    esp_err_t result = ESP_OK;

    WL_Ext_Safe_State state;
    result = WL_Flash::read(this->state_addr, &state, sizeof(WL_Ext_Safe_State));
    WL_EXT_RESULT_CHECK(result);
    ESP_LOGV(TAG, "%s recover, start_addr = 0x%08x, local_addr_base = 0x%08x, local_addr_shift = %i, count=%i", __func__, state.erase_begin, state.local_addr_base, state.local_addr_shift, state.count);

    // check if we have transaction
    if (state.erase_begin == WL_EXT_SAFE_OK) {

        result = this->read(this->dump_addr, this->sector_buffer, this->flash_sector_size);
        WL_EXT_RESULT_CHECK(result);

        result = WL_Flash::erase_sector(state.local_addr_base); // erase comlete flash sector
        WL_EXT_RESULT_CHECK(result);

        // And write back...
        for (int i = 0; i < this->size_factor; i++) {
            if ((i < state.local_addr_shift) || (i >= state.count + state.local_addr_shift)) {
                result = this->write(state.local_addr_base * this->flash_sector_size + i * this->fat_sector_size, &this->sector_buffer[i * this->fat_sector_size / sizeof(uint32_t)], this->fat_sector_size);
                WL_EXT_RESULT_CHECK(result);
            }
        }
        // clear transaction
        result = WL_Flash::erase_range(this->state_addr, this->flash_sector_size);
    }
    return result;
}

esp_err_t WL_Ext_Safe::erase_sector_fit(uint32_t start_sector, uint32_t count)
{
    esp_err_t result = ESP_OK;

    uint32_t local_addr_base = start_sector / this->size_factor;
    uint32_t pre_check_start = start_sector % this->size_factor;
    ESP_LOGV(TAG, "%s start_sector=0x%08x, count = %i", __func__, start_sector, count);
    for (int i = 0; i < this->size_factor; i++) {
        if ((i < pre_check_start) || (i >= count + pre_check_start)) {
            result = this->read(start_sector / this->size_factor * this->flash_sector_size + i * this->fat_sector_size, &this->sector_buffer[i * this->fat_sector_size / sizeof(uint32_t)], this->fat_sector_size);
            WL_EXT_RESULT_CHECK(result);
        }
    }

    result = WL_Flash::erase_sector(this->dump_addr / this->flash_sector_size);
    WL_EXT_RESULT_CHECK(result);
    result = WL_Flash::write(this->dump_addr, this->sector_buffer, this->flash_sector_size);
    WL_EXT_RESULT_CHECK(result);

    WL_Ext_Safe_State state;
    state.erase_begin = WL_EXT_SAFE_OK;
    state.local_addr_base = local_addr_base;
    state.local_addr_shift = pre_check_start;
    state.count = count;

    result = WL_Flash::erase_sector(this->state_addr / this->flash_sector_size);
    WL_EXT_RESULT_CHECK(result);
    result = WL_Flash::write(this->state_addr + 0, &state, sizeof(WL_Ext_Safe_State));
    WL_EXT_RESULT_CHECK(result);

    // Erase
    result = WL_Flash::erase_sector(local_addr_base); // erase comlete flash sector
    WL_EXT_RESULT_CHECK(result);
    // And write back...
    for (int i = 0; i < this->size_factor; i++) {
        if ((i < pre_check_start) || (i >= count + pre_check_start)) {
            result = this->write(local_addr_base * this->flash_sector_size + i * this->fat_sector_size, &this->sector_buffer[i * this->fat_sector_size / sizeof(uint32_t)], this->fat_sector_size);
            WL_EXT_RESULT_CHECK(result);
        }
    }

    result = WL_Flash::erase_sector(this->state_addr / this->flash_sector_size);
    WL_EXT_RESULT_CHECK(result);

    return ESP_OK;
}
