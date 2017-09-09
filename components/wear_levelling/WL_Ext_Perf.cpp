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

#include "WL_Ext_Perf.h"
#include <stdlib.h>
#include "esp_log.h"

static const char *TAG = "wl_ext_perf";

#define WL_EXT_RESULT_CHECK(result) \
    if (result != ESP_OK) { \
        ESP_LOGE(TAG,"%s(%d): result = 0x%08x", __FUNCTION__, __LINE__, result); \
        return (result); \
    }

WL_Ext_Perf::WL_Ext_Perf(): WL_Flash()
{
    this->sector_buffer = NULL;
}

WL_Ext_Perf::~WL_Ext_Perf()
{
    free(this->sector_buffer);
}

esp_err_t WL_Ext_Perf::config(WL_Config_s *cfg, Flash_Access *flash_drv)
{
    wl_ext_cfg_t *config = (wl_ext_cfg_t *)cfg;

    this->fat_sector_size = config->fat_sector_size;
    this->flash_sector_size = cfg->sector_size;

    this->sector_buffer = (uint32_t *)malloc(cfg->sector_size);
    if (this->sector_buffer == NULL) {
        return ESP_ERR_NO_MEM;
    }

    this->size_factor = this->flash_sector_size / this->fat_sector_size;
    if (this->size_factor < 1) {
        return ESP_ERR_INVALID_ARG;
    }

    return WL_Flash::config(cfg, flash_drv);
}

esp_err_t WL_Ext_Perf::init()
{
    return WL_Flash::init();
}

size_t WL_Ext_Perf::chip_size()
{
    return WL_Flash::chip_size();
}
size_t WL_Ext_Perf::sector_size()
{
    return this->fat_sector_size;
}

esp_err_t WL_Ext_Perf::erase_sector(size_t sector)
{
    return this->erase_sector_fit(sector, 1);
}

esp_err_t WL_Ext_Perf::erase_sector_fit(uint32_t start_sector, uint32_t count)
{
    ESP_LOGV(TAG, "%s begin, start_sector = 0x%08x, count = %i", __func__, start_sector, count);
    // This method works with one flash device sector and able to erase "count" of fatfs sectors from this sector
    esp_err_t result = ESP_OK;

    uint32_t pre_check_start = start_sector % this->size_factor;


    for (int i = 0; i < this->size_factor; i++) {
        if ((i < pre_check_start) || (i >= count + pre_check_start)) {
            result = this->read(start_sector / this->size_factor * this->flash_sector_size + i * this->fat_sector_size, &this->sector_buffer[i * this->fat_sector_size / sizeof(uint32_t)], this->fat_sector_size);
            WL_EXT_RESULT_CHECK(result);
        }
    }

    result = WL_Flash::erase_sector(start_sector / this->size_factor); // erase comlete flash sector
    WL_EXT_RESULT_CHECK(result);
    // And write back only data that should not be erased...
    for (int i = 0; i < this->size_factor; i++) {
        if ((i < pre_check_start) || (i >= count + pre_check_start)) {
            result = this->write(start_sector / this->size_factor * this->flash_sector_size + i * this->fat_sector_size, &this->sector_buffer[i * this->fat_sector_size / sizeof(uint32_t)], this->fat_sector_size);
            WL_EXT_RESULT_CHECK(result);
        }
    }
    return ESP_OK;
}

esp_err_t WL_Ext_Perf::erase_range(size_t start_address, size_t size)
{
    esp_err_t result = ESP_OK;
    if ((start_address % this->fat_sector_size) != 0) {
        result = ESP_ERR_INVALID_ARG;
    }
    if (((size % this->fat_sector_size) != 0) || (size == 0)) {
        result = ESP_ERR_INVALID_ARG;
    }
    WL_EXT_RESULT_CHECK(result);

    // The range to erase could be allocated in any possible way
    // ---------------------------------------------------------
    // |       |       |       |       |
    // |0|0|x|x|x|x|x|x|x|x|x|x|x|x|0|0|
    // | pre   | rest  | rest  | post  |  <- check ranges
    //
    // Pre check - the data that is not fit to the full sector at the begining of the erased block
    // Post check - the data that are not fit to the full sector at the end of the erased block
    // rest - data that are fit to the flash device sector at the middle of the erased block
    //
    // In case of pre and post check situations the data of the non erased area have to be readed first and then
    // stored back.
    // For the rest area this operation not needed because complete flash device sector will be erased.

    ESP_LOGV(TAG, "%s begin, addr = 0x%08x, size = %i", __func__, start_address, size);
    // Calculate pre check values
    uint32_t pre_check_start = (start_address / this->fat_sector_size) % this->size_factor;
    uint32_t sectors_count = size / this->fat_sector_size;
    uint32_t pre_check_count = (this->size_factor - pre_check_start);
    if (pre_check_count > sectors_count) {
        pre_check_count = sectors_count;
    }

    // Calculate post ckeck
    uint32_t post_check_count = (sectors_count - pre_check_count) % this->size_factor;
    uint32_t post_check_start = ((start_address + size - post_check_count * this->fat_sector_size) / this->fat_sector_size);

    // Calculate rest
    uint32_t rest_check_count = sectors_count - pre_check_count - post_check_count;
    if ((pre_check_count == this->size_factor) && (0 == pre_check_start)) {
        rest_check_count+=this->size_factor;
        pre_check_count = 0;
    }
    uint32_t rest_check_start = start_address + pre_check_count * this->fat_sector_size;

    // Here we will clear pre_check_count amount of sectors
    if (pre_check_count != 0) {
        result = this->erase_sector_fit(start_address / this->fat_sector_size, pre_check_count);
        WL_EXT_RESULT_CHECK(result);
    }
    ESP_LOGV(TAG, "%s rest_check_start = %i, pre_check_count=%i, rest_check_count=%i, post_check_count=%i\n", __func__, rest_check_start, pre_check_count, rest_check_count, post_check_count);
    if (rest_check_count > 0) {
        rest_check_count = rest_check_count / this->size_factor;
        size_t start_sector = rest_check_start / this->flash_sector_size;
        for (size_t i = 0; i < rest_check_count; i++) {
            result = WL_Flash::erase_sector(start_sector + i);
            WL_EXT_RESULT_CHECK(result);
        }
    }
    if (post_check_count != 0) {
        result = this->erase_sector_fit(post_check_start, post_check_count);
        WL_EXT_RESULT_CHECK(result);
    }
    return ESP_OK;
}
