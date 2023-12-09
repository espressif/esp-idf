/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "WL_Ext_Safe.h"
#include <stdlib.h>
#include <inttypes.h>
#include "esp_log.h"

static const char *TAG = "wl_ext_safe";

#define WL_EXT_RESULT_CHECK(result) \
    if (result != ESP_OK) { \
        ESP_LOGE(TAG,"%s(%d): result = 0x%08" PRIx32, __FUNCTION__, __LINE__, (uint32_t) result); \
        return (result); \
    }

#ifndef FLASH_ERASE_VALUE
#define FLASH_ERASE_VALUE 0xffffffff
#endif // FLASH_ERASE_VALUE


#ifndef WL_EXT_SAFE_OK
#define WL_EXT_SAFE_OK 0x12345678
#endif // WL_EXT_SAFE_OK

/*
WL_Ext_Safe_State stores the buffer transaction state in between erase and write operation of the sector.
This is mainly for safety purpose, in case of power outage in between erase and write operation,
data will be recovered after power outage as temporary data is also stored on flash memory (dump_addr).
- sector_restore_sign : if any transaction was pending before power outage, then on power up,
  this should be WL_EXT_SAFE_OK which indicates that data recovery from dump buffer is needed, else it should be zero.
- sector_base_addr : sector base address where data from dump buffer will be restored.
- sector_base_addr_offset : sector base address offset at which data will be restored
*/
struct WL_Ext_Safe_State {
public:
    uint32_t sector_restore_sign;
    uint32_t sector_base_addr;
    uint32_t sector_base_addr_offset;
    uint32_t count;
};

WL_Ext_Safe::WL_Ext_Safe(): WL_Ext_Perf()
{
}

WL_Ext_Safe::~WL_Ext_Safe()
{
}

esp_err_t WL_Ext_Safe::config(WL_Config_s *cfg, Partition *partition)
{
    esp_err_t result = ESP_OK;

    result = WL_Ext_Perf::config(cfg, partition);
    WL_EXT_RESULT_CHECK(result);
    /* two extra sectors will be reserved to store buffer transaction state WL_Ext_Safe_State
     and temporary storage of the actual sector data from the sector which is to be erased*/
    this->buff_trans_state_addr = WL_Ext_Perf::get_flash_size() - 2 * this->flash_sector_size;
    this->dump_addr = WL_Ext_Perf::get_flash_size() - 1 * this->flash_sector_size;
    return ESP_OK;
}

esp_err_t WL_Ext_Safe::init()
{
    esp_err_t result = ESP_OK;
    ESP_LOGV(TAG, "%s", __func__);

    result = WL_Ext_Perf::init();
    WL_EXT_RESULT_CHECK(result);

    //check if any buffer write operation was pending and recover data if needed
    result = this->recover();
    return result;
}

size_t WL_Ext_Safe::get_flash_size()
{
    ESP_LOGV(TAG, "%s size = %" PRIu32, __func__, (uint32_t) (WL_Ext_Perf::get_flash_size() - 2 * this->flash_sector_size));
    return WL_Ext_Perf::get_flash_size() - 2 * this->flash_sector_size;
}

esp_err_t WL_Ext_Safe::recover()
{
    esp_err_t result = ESP_OK;

    // read WL_Ext_Safe_State from flash memory at buff_trans_state_addr
    WL_Ext_Safe_State state;
    result = this->read(this->buff_trans_state_addr, &state, sizeof(WL_Ext_Safe_State));
    WL_EXT_RESULT_CHECK(result);
    ESP_LOGV(TAG, "%s recover, start_addr = 0x%08" PRIx32 ", sector_base_addr = 0x%08" PRIx32 ",  sector_base_addr_offset= %" PRIu32 ", count=%" PRIu32, __func__, state.sector_restore_sign, state.sector_base_addr, state.sector_base_addr_offset, state.count);

    // check if we have any incomplete transaction pending.
    if (state.sector_restore_sign == WL_EXT_SAFE_OK) {

        // recover data from dump_addr and store it to temporary storage sector_buffer.
        result = this->read(this->dump_addr, this->sector_buffer, this->flash_sector_size);
        WL_EXT_RESULT_CHECK(result);

        //erase complete flash sector
        result = this->erase_sector(state.sector_base_addr);
        WL_EXT_RESULT_CHECK(result);

        /* Restore data which was previously stored to sector_buffer
         back to data area provided by WL_Ext_Safe_State state */
        for (int i = 0; i < this->flash_fat_sector_size_factor; i++) {
            if ((i < state.sector_base_addr_offset) || (i >= state.count + state.sector_base_addr_offset)) {
                result = this->write(state.sector_base_addr * this->flash_sector_size + i * this->fat_sector_size, &this->sector_buffer[i * this->fat_sector_size / sizeof(uint32_t)], this->fat_sector_size);
                WL_EXT_RESULT_CHECK(result);
            }
        }

        // clear the buffer transaction state after the data recovery.
        result = this->erase_range(this->buff_trans_state_addr, this->flash_sector_size);
    }
    return result;
}

/*
erase_sector_fit function is needed in case flash_sector_size != fat_sector_size and
sector to be erased is not multiple of flash_fat_sector_size_factor
*/
esp_err_t WL_Ext_Safe::erase_sector_fit(uint32_t first_erase_sector, uint32_t count)
{
    esp_err_t result = ESP_OK;

    uint32_t flash_sector_base_addr = first_erase_sector / this->flash_fat_sector_size_factor;
    uint32_t pre_check_start = first_erase_sector % this->flash_fat_sector_size_factor;

    // Except pre check and post check data area, read and store all other data to sector_buffer
    ESP_LOGV(TAG, "%s first_erase_sector=0x%08" PRIx32 ", count = %" PRIu32, __func__, first_erase_sector, count);
    for (int i = 0; i < this->flash_fat_sector_size_factor; i++) {
        if ((i < pre_check_start) || (i >= count + pre_check_start)) {
            result = this->read(flash_sector_base_addr * this->flash_sector_size + i * this->fat_sector_size,
                                &this->sector_buffer[i * this->fat_sector_size / sizeof(uint32_t)],
                                this->fat_sector_size);
            WL_EXT_RESULT_CHECK(result);
        }
    }

    // For safety purpose store temporary stored data sector_buffer to flash memory at dump_addr
    result = this->erase_sector(this->dump_addr / this->flash_sector_size);
    WL_EXT_RESULT_CHECK(result);
    result = this->write(this->dump_addr, this->sector_buffer, this->flash_sector_size);
    WL_EXT_RESULT_CHECK(result);

    //store transaction buffer state to flash memory at buff_trans_state_addr
    WL_Ext_Safe_State state;
    state.sector_restore_sign = WL_EXT_SAFE_OK;
    state.sector_base_addr = flash_sector_base_addr;
    state.sector_base_addr_offset = pre_check_start;
    state.count = count;

    result = this->erase_sector(this->buff_trans_state_addr / this->flash_sector_size);
    WL_EXT_RESULT_CHECK(result);
    result = this->write(this->buff_trans_state_addr + 0, &state, sizeof(WL_Ext_Safe_State));
    WL_EXT_RESULT_CHECK(result);

    //erase complete flash sector which includes pre and post check data area
    result = this->erase_sector(flash_sector_base_addr);
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

    // clear the buffer transaction state after data is restored properly.
    result = this->erase_sector(this->buff_trans_state_addr / this->flash_sector_size);
    WL_EXT_RESULT_CHECK(result);

    return ESP_OK;
}
