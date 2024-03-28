/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "WL_Ext_Perf.h"
#include "Partition.h"
#include <stdlib.h>
#include <inttypes.h>
#include "esp_log.h"

static const char *TAG = "wl_ext_perf";

#define WL_EXT_RESULT_CHECK(result) \
    if (result != ESP_OK) { \
        ESP_LOGE(TAG,"%s(%d): result = 0x%08" PRIx32, __FUNCTION__, __LINE__, (uint32_t) result); \
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

esp_err_t WL_Ext_Perf::config(WL_Config_s *cfg, Partition *partition)
{
    wl_ext_cfg_t *ext_cfg = (wl_ext_cfg_t *)cfg;

    this->flash_sector_size = ext_cfg->flash_sector_size;
    this->fat_sector_size = ext_cfg->fat_sector_size;

    /*when flash and fat sector sizes are not equal, (where flash_sector_size >= fat_sector_size)
    this flash_fat_sector_size_factor will be used while flash sector erase or read-write operation */
    this->flash_fat_sector_size_factor = this->flash_sector_size / this->fat_sector_size;
    if (this->flash_fat_sector_size_factor < 1) {
        return ESP_ERR_INVALID_ARG;
    }

    this->sector_buffer = (uint32_t *)malloc(ext_cfg->flash_sector_size);
    if (this->sector_buffer == NULL) {
        return ESP_ERR_NO_MEM;
    }

    return WL_Flash::config(cfg, partition);
}

esp_err_t WL_Ext_Perf::init()
{
    return WL_Flash::init();
}

size_t WL_Ext_Perf::get_flash_size()
{
    return WL_Flash::get_flash_size();
}

size_t WL_Ext_Perf::get_sector_size()
{
    return this->fat_sector_size;
}

esp_err_t WL_Ext_Perf::erase_sector(size_t sector)
{
    return WL_Flash::erase_sector(sector);
}

/*
erase_sector_fit function is needed in case flash_sector_size != fat_sector_size and
sector to be erased is not multiple of flash_fat_sector_size_factor
*/
esp_err_t WL_Ext_Perf::erase_sector_fit(uint32_t first_erase_sector, uint32_t count)
{
    // This method works with one flash device sector and able to erase "count" of fatfs sectors from this first_erase_sector
    esp_err_t result = ESP_OK;
    ESP_LOGV(TAG, "%s begin, first_erase_sector = 0x%08" PRIx32 ", count = %" PRIu32, __func__, first_erase_sector, count);

    uint32_t flash_sector_base_addr = first_erase_sector / this->flash_fat_sector_size_factor;
    uint32_t pre_check_start = first_erase_sector % this->flash_fat_sector_size_factor;

    // Except pre check and post check data area, read and store all other data to sector_buffer
    for (int i = 0; i < this->flash_fat_sector_size_factor; i++) {
        if ((i < pre_check_start) || (i >= count + pre_check_start)) {
            result = this->read(flash_sector_base_addr * this->flash_sector_size + i * this->fat_sector_size,
                                &this->sector_buffer[i * this->fat_sector_size / sizeof(uint32_t)],
                                this->fat_sector_size);
            WL_EXT_RESULT_CHECK(result);
        }
    }

    //erase complete flash sector which includes pre and post check data area
    result = WL_Flash::erase_sector(flash_sector_base_addr);
    WL_EXT_RESULT_CHECK(result);

    /* Restore data which was previously stored to sector_buffer
       back to data area which was not part of pre and post check data */
    for (int i = 0; i < this->flash_fat_sector_size_factor; i++) {
        if ((i < pre_check_start) || (i >= count + pre_check_start)) {
            result = this->write(flash_sector_base_addr * this->flash_sector_size + i * this->fat_sector_size,
                                 &this->sector_buffer[i * this->fat_sector_size / sizeof(uint32_t)],
                                 this->fat_sector_size);
            WL_EXT_RESULT_CHECK(result);
        }
    }
    return ESP_OK;
}

esp_err_t WL_Ext_Perf::erase_range(size_t start_address, size_t size)
{
    esp_err_t result = ESP_OK;

    //start_address as well as size should be aligned to fat_sector_size
    if ((start_address % this->fat_sector_size) != 0) {
        result = ESP_ERR_INVALID_ARG;
    }
    if (((size % this->fat_sector_size) != 0) || (size == 0)) {
        result = ESP_ERR_INVALID_SIZE;
    }
    WL_EXT_RESULT_CHECK(result);

    // The range to erase could be allocated in any possible way
    // ---------------------------------------------------------
    // |       |       |       |       |
    // |0|0|x|x|x|x|x|x|x|x|x|x|x|x|0|0|
    // | pre   | rest  | rest  | post  |  <- check ranges
    //
    // Pre check  - the data that does not fit in the full sector at the beginning of the erased block
    // Post check - the data that does not fit in the full sector at the end of the erased block
    // rest check - the data that fits in the full sector at the middle of the erased block
    //
    // In case of pre and post check situations, the data of the area which should not be erased
    // as part of pre and post check data area have to read first, store at other location,
    // erase complete sector and restore data of area which should not be erased.
    // For the rest check area, this operation not needed because complete flash device sector will be erased.

    ESP_LOGV(TAG, "%s begin, addr = 0x%08" PRIx32 ", size = %" PRIu32, __func__, (uint32_t) start_address, (uint32_t) size);
    uint32_t sectors_count = size / this->fat_sector_size;

    // Calculate pre check values
    uint32_t pre_check_start = (start_address / this->fat_sector_size) % this->flash_fat_sector_size_factor;
    uint32_t pre_check_count = (this->flash_fat_sector_size_factor - pre_check_start);
    //maximum sectors count that need to be erased is sectors_count
    if (pre_check_count > sectors_count) {
        pre_check_count = sectors_count;
    }

    // Calculate post check values
    uint32_t post_check_count = (sectors_count - pre_check_count) % this->flash_fat_sector_size_factor;
    uint32_t post_check_start = ((start_address + size - post_check_count * this->fat_sector_size) / this->fat_sector_size);

    // Calculate rest check values
    uint32_t rest_check_count = sectors_count - pre_check_count - post_check_count;
    if ((pre_check_count == this->flash_fat_sector_size_factor) && (0 == pre_check_start)) {
        rest_check_count+=this->flash_fat_sector_size_factor;
        pre_check_count = 0;
    }
    uint32_t rest_check_start = start_address + pre_check_count * this->fat_sector_size;

    // Clear pre_check_count sectors
    if (pre_check_count != 0) {
        result = this->erase_sector_fit(start_address / this->fat_sector_size, pre_check_count);
        WL_EXT_RESULT_CHECK(result);
    }
    ESP_LOGV(TAG, "%s rest_check_start = %" PRIu32 ", pre_check_count=%" PRIu32 ", rest_check_count=%" PRIu32 ", post_check_count=%" PRIu32, __func__, rest_check_start, pre_check_count, rest_check_count, post_check_count);

    // Clear rest_check_count sectors
    if (rest_check_count > 0) {
        rest_check_count = rest_check_count / this->flash_fat_sector_size_factor;
        size_t start_sector = rest_check_start / this->flash_sector_size;
        for (size_t i = 0; i < rest_check_count; i++) {
            result = WL_Flash::erase_sector(start_sector + i);
            WL_EXT_RESULT_CHECK(result);
        }
    }

    // Clear post_check_count sectors
    if (post_check_count != 0) {
        result = this->erase_sector_fit(post_check_start, post_check_count);
        WL_EXT_RESULT_CHECK(result);
    }
    return ESP_OK;
}
