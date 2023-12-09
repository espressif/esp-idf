/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "esp_random.h"
#include "esp_log.h"
#include "Partition.h"
#include "WL_Flash.h"
#include <stdlib.h>
#include "crc32.h"
#include <string.h>
#include <stddef.h>
#include <inttypes.h>

static const char *TAG = "wl_flash";
#ifndef WL_CFG_CRC_CONST
#define WL_CFG_CRC_CONST UINT32_MAX
#endif // WL_CFG_CRC_CONST

#define WL_RESULT_CHECK(result) \
    if (result != ESP_OK) { \
        ESP_LOGE(TAG,"%s(%d): result = 0x%08" PRIx32, __FUNCTION__, __LINE__, (uint32_t) result); \
        return (result); \
    }

#ifndef _MSC_VER // MSVS has different format for this define
static_assert(sizeof(wl_state_t) % 32 == 0, "wl_state_t structure size must be multiple of flash encryption unit size");
#endif // _MSC_VER


WL_Flash::WL_Flash()
{
}

WL_Flash::~WL_Flash()
{
    free(this->temp_buff);
}

esp_err_t WL_Flash::config(wl_config_t *cfg, Partition *partition)
{
    ESP_LOGV(TAG, "%s partition_start_addr=0x%08" PRIx32 ", wl_partition_size=0x%08" PRIx32 ", wl_page_size=0x%08" PRIx32 ", flash_sector_size=0x%08" PRIx32 ", wl_update_rate=0x%08" PRIx32 ", wl_pos_update_record_size=0x%08" PRIx32 ", version=0x%08" PRIx32 ", wl_temp_buff_size=0x%08" PRIx32 , __func__,
             (uint32_t) cfg->wl_partition_start_addr,
             cfg->wl_partition_size,
             cfg->wl_page_size,
             cfg->flash_sector_size,
             cfg->wl_update_rate,
             cfg->wl_pos_update_record_size,
             cfg->version,
             (uint32_t) cfg->wl_temp_buff_size);

    cfg->crc32 = crc32::crc32_le(WL_CFG_CRC_CONST, (const unsigned char *)cfg, offsetof(wl_config_t, crc32));
    esp_err_t result = ESP_OK;
    memcpy(&this->cfg, cfg, sizeof(wl_config_t));
    if (this->cfg.wl_temp_buff_size < this->cfg.wl_pos_update_record_size) {
        this->cfg.wl_temp_buff_size = this->cfg.wl_pos_update_record_size;
    }
    this->configured = false;
    if (cfg == NULL) {
        result = ESP_ERR_INVALID_ARG;
    }
    this->partition = partition;
    if (partition == NULL) {
        result = ESP_ERR_INVALID_ARG;
    }
    if ((this->cfg.flash_sector_size % this->cfg.wl_temp_buff_size) != 0) {
        result = ESP_ERR_INVALID_ARG;
    }
    if (this->cfg.wl_page_size < this->cfg.flash_sector_size) {
        result = ESP_ERR_INVALID_ARG;
    }
    WL_RESULT_CHECK(result);

    this->state_size = this->cfg.flash_sector_size;
    if (this->state_size < (sizeof(wl_state_t) + (this->cfg.wl_partition_size / this->cfg.flash_sector_size)*this->cfg.wl_pos_update_record_size)) {
        this->state_size = ((sizeof(wl_state_t) + (this->cfg.wl_partition_size / this->cfg.flash_sector_size) * this->cfg.wl_pos_update_record_size) + this->cfg.flash_sector_size - 1) / this->cfg.flash_sector_size;
        this->state_size = this->state_size * this->cfg.flash_sector_size;
    }
    this->cfg_size = (sizeof(wl_config_t) + this->cfg.flash_sector_size - 1) / this->cfg.flash_sector_size;
    this->cfg_size = cfg_size * this->cfg.flash_sector_size;

    this->addr_cfg = this->cfg.wl_partition_start_addr + this->cfg.wl_partition_size - this->cfg_size;
    this->addr_state1 = this->cfg.wl_partition_start_addr + this->cfg.wl_partition_size - this->state_size * 2 - this->cfg_size; // allocate data at the end of memory
    this->addr_state2 = this->cfg.wl_partition_start_addr + this->cfg.wl_partition_size - this->state_size * 1 - this->cfg_size; // allocate data at the end of memory

    ptrdiff_t flash_sz = ((this->cfg.wl_partition_size - this->state_size * 2 - this->cfg_size) / this->cfg.wl_page_size - 1) * this->cfg.wl_page_size; // -1 remove dummy block
    this->flash_size = ((this->cfg.wl_partition_size - this->state_size * 2 - this->cfg_size) / this->cfg.wl_page_size - 1) * this->cfg.wl_page_size; // -1 remove dummy block

    ESP_LOGD(TAG, "%s - config result: state_size=0x%08" PRIx32 ", cfg_size=0x%08" PRIx32 ", addr_cfg=0x%08" PRIx32 ", addr_state1=0x%08" PRIx32 ", addr_state2=0x%08" PRIx32 ", flash_size=0x%08" PRIx32, __func__,
             (uint32_t) this->state_size,
             (uint32_t) this->cfg_size,
             (uint32_t) this->addr_cfg,
             (uint32_t) this->addr_state1,
             (uint32_t) this->addr_state2,
             (uint32_t) this->flash_size
            );
    if (flash_sz <= 0) {
        result = ESP_ERR_INVALID_ARG;
    }
    WL_RESULT_CHECK(result);

    this->temp_buff = (uint8_t *)malloc(this->cfg.wl_temp_buff_size);
    if (this->temp_buff == NULL) {
        result = ESP_ERR_NO_MEM;
    }
    WL_RESULT_CHECK(result);
    this->configured = true;
    return ESP_OK;
}

esp_err_t WL_Flash::init()
{
    esp_err_t result = ESP_OK;
    if (this->configured == false) {
        ESP_LOGW(TAG, "WL_Flash: not configured, call config() first");
        return ESP_ERR_INVALID_STATE;
    }
    // If flow will be interrupted by error, then this flag will be false
    this->initialized = false;
    // Init states if it is first time...
    this->partition->read(this->addr_state1, &this->state, sizeof(wl_state_t));
    wl_state_t sa_copy;
    wl_state_t *state_copy = &sa_copy;
    result = this->partition->read(this->addr_state2, state_copy, sizeof(wl_state_t));
    WL_RESULT_CHECK(result);

    int check_size = WL_STATE_CRC_LEN_V2;
    // Chech CRC and recover state
    uint32_t crc1 = crc32::crc32_le(WL_CFG_CRC_CONST, (uint8_t *)&this->state, check_size);
    uint32_t crc2 = crc32::crc32_le(WL_CFG_CRC_CONST, (uint8_t *)state_copy, check_size);

    ESP_LOGD(TAG, "%s - config ID=%" PRIu32 ", stored ID=%" PRIu32 ", wl_sec_erase_cycle_count=%" PRIu32 ", wl_block_size=%" PRIu32 ", wl_max_sec_erase_cycle_count=%" PRIu32 ", wl_dummy_sec_pos=%" PRIu32 ", wl_dummy_sec_move_count=0x%8.8" PRIX32,
             __func__,
             this->cfg.version,
             this->state.version,
             this->state.wl_sec_erase_cycle_count,
             this->state.wl_block_size,
             this->state.wl_max_sec_erase_cycle_count,
             this->state.wl_dummy_sec_pos,
             this->state.wl_dummy_sec_move_count);

    ESP_LOGD(TAG, "%s starts: crc1= 0x%08" PRIx32 ", crc2 = 0x%08" PRIx32 ", this->state.crc= 0x%08" PRIx32 ", state_copy->crc= 0x%08" PRIx32 ", version=%" PRIu32 ", read_version=%" PRIu32, __func__, crc1, crc2, this->state.crc32, state_copy->crc32, this->cfg.version, this->state.version);
    if ((crc1 == this->state.crc32) && (crc2 == state_copy->crc32)) {
        // The state is OK. Check the ID
        if (this->state.version != this->cfg.version) {
            result = this->initSections();
            WL_RESULT_CHECK(result);
            result = this->recoverPos();
            WL_RESULT_CHECK(result);
        } else {
            if (crc1 != crc2) {// we did not update second structure.
                result = this->partition->erase_range(this->addr_state2, this->state_size);
                WL_RESULT_CHECK(result);
                result = this->partition->write(this->addr_state2, &this->state, sizeof(wl_state_t));
                WL_RESULT_CHECK(result);

                for (size_t i = 0; i < ((this->cfg.wl_partition_size / this->cfg.flash_sector_size)); i++) {
                    bool pos_bits;
                    result = this->partition->read(this->addr_state1 + sizeof(wl_state_t) + i * this->cfg.wl_pos_update_record_size, this->temp_buff, this->cfg.wl_pos_update_record_size);
                    WL_RESULT_CHECK(result);
                    pos_bits = this->OkBuffSet(i);
                    if (pos_bits == true) {
                        //this->fillOkBuff(i);
                        result = this->partition->write(this->addr_state2 + sizeof(wl_state_t) + i * this->cfg.wl_pos_update_record_size, this->temp_buff, this->cfg.wl_pos_update_record_size);
                        WL_RESULT_CHECK(result);
                    }
                }
            }
            ESP_LOGD(TAG, "%s: crc1=0x%08" PRIx32 ", crc2 = 0x%08" PRIx32 ", result= 0x%08" PRIx32 , __func__, crc1, crc2, (uint32_t)result);
            result = this->recoverPos();
            WL_RESULT_CHECK(result);
        }
    } else if ((crc1 != this->state.crc32) && (crc2 != state_copy->crc32)) { // This is just new flash or new version
        // Check if this is new version or just new instance of WL
        ESP_LOGD(TAG, "%s: try to update version - crc1= 0x%08" PRIx32 ", crc2 = 0x%08" PRIx32 ", result= 0x%08" PRIx32 , __func__, (uint32_t)crc1, (uint32_t)crc2, (uint32_t)result);
        result = this->updateVersion();
        if (result == ESP_FAIL) {
            ESP_LOGD(TAG, "%s: init flash sections", __func__);
            result = this->initSections();
            WL_RESULT_CHECK(result);
        }
        result = this->recoverPos();
        WL_RESULT_CHECK(result);
    } else {
        // recover broken state
        if (crc1 == this->state.crc32) {// we have to recover state 2
            result = this->partition->erase_range(this->addr_state2, this->state_size);
            WL_RESULT_CHECK(result);
            result = this->partition->write(this->addr_state2, &this->state, sizeof(wl_state_t));
            WL_RESULT_CHECK(result);

            for (size_t i = 0; i < ((this->cfg.wl_partition_size / this->cfg.flash_sector_size)); i++) {
                bool pos_bits;
                result = this->partition->read(this->addr_state1 + sizeof(wl_state_t) + i * this->cfg.wl_pos_update_record_size, this->temp_buff, this->cfg.wl_pos_update_record_size);
                WL_RESULT_CHECK(result);
                pos_bits = this->OkBuffSet(i);
                if (pos_bits == true) {
                    result = this->partition->write(this->addr_state2 + sizeof(wl_state_t) + i * this->cfg.wl_pos_update_record_size, this->temp_buff, this->cfg.wl_pos_update_record_size);
                    WL_RESULT_CHECK(result);
                }
            }
            result = this->partition->read(this->addr_state2, &this->state, sizeof(wl_state_t));
            WL_RESULT_CHECK(result);
        } else { // we have to recover state 1
            result = this->partition->erase_range(this->addr_state1, this->state_size);
            WL_RESULT_CHECK(result);
            result = this->partition->write(this->addr_state1, state_copy, sizeof(wl_state_t));
            WL_RESULT_CHECK(result);

            for (size_t i = 0; i < ((this->cfg.wl_partition_size / this->cfg.flash_sector_size)); i++) {
                bool pos_bits;
                result = this->partition->read(this->addr_state2 + sizeof(wl_state_t) + i * this->cfg.wl_pos_update_record_size, this->temp_buff, this->cfg.wl_pos_update_record_size);

                WL_RESULT_CHECK(result);
                pos_bits = this->OkBuffSet(i);
                if (pos_bits == true) {
                    result = this->partition->write(this->addr_state1 + sizeof(wl_state_t) + i * this->cfg.wl_pos_update_record_size, this->temp_buff, this->cfg.wl_pos_update_record_size);
                    WL_RESULT_CHECK(result);
                }
            }
            result = this->partition->read(this->addr_state1, &this->state, sizeof(wl_state_t));
            WL_RESULT_CHECK(result);
            this->state.wl_dummy_sec_pos = this->state.wl_part_max_sec_pos - 1;
        }
        // done. We have recovered the state
        // If we have a new configuration, we will overwrite it
        if (this->state.version != this->cfg.version) {
            result = this->initSections();
            WL_RESULT_CHECK(result);
        }
    }
    if (result != ESP_OK) {
        this->initialized = false;
        ESP_LOGE(TAG, "%s: returned 0x%08" PRIx32 , __func__, (uint32_t)result);
        return result;
    }
    this->initialized = true;
    ESP_LOGD(TAG, "%s - wl_dummy_sec_move_count= 0x%08" PRIx32 , __func__, (uint32_t)this->state.wl_dummy_sec_move_count);
    return ESP_OK;
}

esp_err_t WL_Flash::recoverPos()
{
    esp_err_t result = ESP_OK;
    size_t position = 0;
    ESP_LOGV(TAG, "%s start", __func__);
    for (size_t i = 0; i < this->state.wl_part_max_sec_pos; i++) {
        bool pos_bits;
        position = i;
        result = this->partition->read(this->addr_state1 + sizeof(wl_state_t) + i * this->cfg.wl_pos_update_record_size, this->temp_buff, this->cfg.wl_pos_update_record_size);
        pos_bits = this->OkBuffSet(i);
        WL_RESULT_CHECK(result);
        ESP_LOGV(TAG, "%s - check pos: result=0x%08" PRIx32 ", position= %" PRIu32 ", pos_bits= 0x%08" PRIx32 , __func__, (uint32_t) result, (uint32_t) position, (uint32_t) pos_bits);
        if (pos_bits == false) {
            break; // we have found position
        }
    }

    this->state.wl_dummy_sec_pos = position;
    if (this->state.wl_dummy_sec_pos == this->state.wl_part_max_sec_pos) {
        this->state.wl_dummy_sec_pos--;
    }
    ESP_LOGD(TAG, "%s - this->state.wl_dummy_sec_pos= 0x%08" PRIx32 ", position= 0x%08" PRIx32 ", result= 0x%08" PRIx32 ", wl_part_max_sec_pos= 0x%08" PRIx32 , __func__, (uint32_t)this->state.wl_dummy_sec_pos, (uint32_t)position, (uint32_t)result, (uint32_t)this->state.wl_part_max_sec_pos);
    ESP_LOGV(TAG, "%s done", __func__);
    return result;
}

esp_err_t WL_Flash::initSections()
{
    esp_err_t result = ESP_OK;
    this->state.wl_dummy_sec_pos = 0;
    this->state.wl_sec_erase_cycle_count = 0;
    this->state.wl_dummy_sec_move_count = 0;
    // max count
    this->state.wl_max_sec_erase_cycle_count = this->flash_size / this->state_size * this->cfg.wl_update_rate;
    if (this->cfg.wl_update_rate != 0) {
        this->state.wl_max_sec_erase_cycle_count = this->cfg.wl_update_rate;
    }
    this->state.version = this->cfg.version;
    this->state.wl_block_size = this->cfg.wl_page_size;
    this->state.wl_device_id = esp_random();
    memset(this->state.reserved, 0, sizeof(this->state.reserved));

    this->state.wl_part_max_sec_pos = 1 + this->flash_size / this->cfg.wl_page_size;

    this->state.crc32 = crc32::crc32_le(WL_CFG_CRC_CONST, (uint8_t *)&this->state, WL_STATE_CRC_LEN_V2);

    result = this->partition->erase_range(this->addr_state1, this->state_size);
    WL_RESULT_CHECK(result);
    result = this->partition->write(this->addr_state1, &this->state, sizeof(wl_state_t));
    WL_RESULT_CHECK(result);
    // write state copy
    result = this->partition->erase_range(this->addr_state2, this->state_size);
    WL_RESULT_CHECK(result);
    result = this->partition->write(this->addr_state2, &this->state, sizeof(wl_state_t));
    WL_RESULT_CHECK(result);

    result = this->partition->erase_range(this->addr_cfg, this->cfg_size);
    WL_RESULT_CHECK(result);
    result = this->partition->write(this->addr_cfg, &this->cfg, sizeof(wl_config_t));
    WL_RESULT_CHECK(result);

    ESP_LOGD(TAG, "%s - this->state->wl_max_sec_erase_cycle_count= 0x%08" PRIx32 ", this->state->wl_part_max_sec_pos= 0x%08" PRIx32 , __func__, this->state.wl_max_sec_erase_cycle_count, this->state.wl_part_max_sec_pos);
    ESP_LOGD(TAG, "%s - result= 0x%08x" , __func__, result);
    return result;
}

esp_err_t WL_Flash::updateVersion()
{
    esp_err_t result = ESP_OK;

    result = this->updateV1_V2();
    if (result == ESP_OK) {
        return result;
    }
    // check next version
    return result;
}

esp_err_t WL_Flash::updateV1_V2()
{
    esp_err_t result = ESP_OK;
    // Check crc for old version and old version
    ESP_LOGV(TAG, "%s start", __func__);
    int check_size = WL_STATE_CRC_LEN_V1;
    // Chech CRC and recover state
    uint32_t crc1 = crc32::crc32_le(WL_CFG_CRC_CONST, (uint8_t *)&this->state, check_size);
    wl_state_t sa_copy;
    wl_state_t *state_copy = &sa_copy;
    result = this->partition->read(this->addr_state2, state_copy, sizeof(wl_state_t));
    WL_RESULT_CHECK(result);
    uint32_t crc2 = crc32::crc32_le(WL_CFG_CRC_CONST, (uint8_t *)state_copy, check_size);

    // For V1 crc in place of wl_device_id and version
    uint32_t v1_crc1 = this->state.wl_device_id;
    uint32_t v1_crc2 = state_copy->wl_device_id;

    ESP_LOGD(TAG, "%s - process crc1=0x%08" PRIx32 ", crc2=0x%08" PRIx32 ", v1_crc1=0x%08" PRIx32 ", v1_crc2=0x%08" PRIx32 ", version=%" PRIu32, __func__, crc1, crc2, v1_crc1, v1_crc2, this->state.version);

    if ((crc1 == v1_crc1) && (crc2 == v1_crc2) && (v1_crc1 == v1_crc2) && (this->state.version == 1) && (state_copy->version == 1)) {
        // Here we have to update all internal structures
        ESP_LOGI(TAG, "%s Update from V1 to V2, crc=0x%08" PRIx32 ", ", __func__, crc1);
        uint32_t pos = 0;

        for (size_t i = 0; i < this->state.wl_part_max_sec_pos; i++) {
            uint8_t pos_bits;
            result = this->partition->read(this->addr_state1 + sizeof(wl_state_t) + i * this->cfg.wl_pos_update_record_size, &pos_bits, 1);
            WL_RESULT_CHECK(result);
            ESP_LOGV(TAG, "%s- result= 0x%08" PRIx32 ", pos= %" PRIu32 ", pos_bits= 0x%08" PRIx32 , __func__, (uint32_t) result, (uint32_t) pos, (uint32_t) pos_bits);
            pos = i;
            if (pos_bits == 0xff) {
                break; // we have found position
            }
        }
        ESP_LOGI(TAG, "%s wl_part_max_sec_pos=%" PRIu32 ", pos=%" PRIu32 ", state.ver=%" PRIu32 ", state2.ver=%" PRIu32, __func__, (uint32_t) this->state.wl_part_max_sec_pos, (uint32_t) pos, (uint32_t) this->state.version, (uint32_t) state_copy->version);
        if (pos == this->state.wl_part_max_sec_pos) {
            pos--;
        }
        WL_RESULT_CHECK(result);

        this->state.version = 2;
        this->state.wl_dummy_sec_pos = 0;
        this->state.wl_device_id = esp_random();
        memset(this->state.reserved, 0, sizeof(this->state.reserved));
        this->state.crc32 = crc32::crc32_le(WL_CFG_CRC_CONST, (uint8_t *)&this->state, WL_STATE_CRC_LEN_V2);

        result = this->partition->erase_range(this->addr_state1, this->state_size);
        WL_RESULT_CHECK(result);
        result = this->partition->write(this->addr_state1, &this->state, sizeof(wl_state_t));
        WL_RESULT_CHECK(result);

        memset(this->temp_buff, 0, this->cfg.wl_pos_update_record_size);
        for (uint32_t i = 0 ; i <= pos; i++) {
            this->fillOkBuff(i);
            result = this->partition->write(this->addr_state1 + sizeof(wl_state_t) + i * this->cfg.wl_pos_update_record_size, this->temp_buff, this->cfg.wl_pos_update_record_size);
            WL_RESULT_CHECK(result);
        }

        result = this->partition->erase_range(this->addr_state2, this->state_size);
        WL_RESULT_CHECK(result);
        result = this->partition->write(this->addr_state2, &this->state, sizeof(wl_state_t));
        WL_RESULT_CHECK(result);
        ESP_LOGD(TAG, "%s - wl_dummy_sec_move_count= 0x%08" PRIx32 ", pos= 0x%08" PRIx32 , __func__, this->state.wl_dummy_sec_move_count, this->state.wl_dummy_sec_pos);

        memset(this->temp_buff, 0, this->cfg.wl_pos_update_record_size);
        for (uint32_t i = 0 ; i <= pos; i++) {
            this->fillOkBuff(i);
            result = this->partition->write(this->addr_state2 + sizeof(wl_state_t) + i * this->cfg.wl_pos_update_record_size, this->temp_buff, this->cfg.wl_pos_update_record_size);
            WL_RESULT_CHECK(result);
        }
        this->state.wl_dummy_sec_pos = pos;
        return result;
    }

    return ESP_FAIL;
}

void WL_Flash::fillOkBuff(int n)
{
    uint32_t *buff = (uint32_t *)this->temp_buff;

    for (int i = 0 ; i < 4 ; i++) {
        buff[i] = this->state.wl_device_id + n * 4 + i;
        buff[i] = crc32::crc32_le(WL_CFG_CRC_CONST, (uint8_t *)&buff[i], sizeof(uint32_t));
    }
}

bool WL_Flash::OkBuffSet(int n)
{
    bool result = true;
    uint32_t *data_buff = (uint32_t *)this->temp_buff;
    for (int i = 0 ; i < 4 ; i++) {
        uint32_t data = this->state.wl_device_id + n * 4 + i;
        uint32_t crc = crc32::crc32_le(WL_CFG_CRC_CONST, (uint8_t *)&data, sizeof(uint32_t));
        if (crc != data_buff[i]) {
            result = false;
        }
    }
    return result;
}


esp_err_t WL_Flash::updateWL()
{
    esp_err_t result = ESP_OK;
    this->state.wl_sec_erase_cycle_count++;
    if (this->state.wl_sec_erase_cycle_count < this->state.wl_max_sec_erase_cycle_count) {
        return result;
    }
    // Here we have to move the block and increase the state
    this->state.wl_sec_erase_cycle_count = 0;
    ESP_LOGV(TAG, "%s - wl_sec_erase_cycle_count= 0x%08" PRIx32 ", pos= 0x%08" PRIx32 , __func__, this->state.wl_sec_erase_cycle_count, this->state.wl_dummy_sec_pos);
    // copy data to dummy block
    size_t data_addr = this->state.wl_dummy_sec_pos + 1; // next block, [pos+1] copy to [pos]
    if (data_addr >= this->state.wl_part_max_sec_pos) {
        data_addr = 0;
    }
    data_addr = this->cfg.wl_partition_start_addr + data_addr * this->cfg.wl_page_size;
    this->dummy_addr = this->cfg.wl_partition_start_addr + this->state.wl_dummy_sec_pos * this->cfg.wl_page_size;
    result = this->partition->erase_range(this->dummy_addr, this->cfg.wl_page_size);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "%s - erase wl dummy sector result= 0x%08x" , __func__, result);
        this->state.wl_sec_erase_cycle_count = this->state.wl_max_sec_erase_cycle_count - 1; // we will update next time
        return result;
    }

    size_t copy_count = this->cfg.wl_page_size / this->cfg.wl_temp_buff_size;
    for (size_t i = 0; i < copy_count; i++) {
        result = this->partition->read(data_addr + i * this->cfg.wl_temp_buff_size, this->temp_buff, this->cfg.wl_temp_buff_size);
        if (result != ESP_OK) {
            ESP_LOGE(TAG, "%s - not possible to read buffer, will try next time, result= 0x%08x" , __func__, result);
            this->state.wl_sec_erase_cycle_count = this->state.wl_max_sec_erase_cycle_count - 1; // we will update next time
            return result;
        }
        result = this->partition->write(this->dummy_addr + i * this->cfg.wl_temp_buff_size, this->temp_buff, this->cfg.wl_temp_buff_size);
        if (result != ESP_OK) {
            ESP_LOGE(TAG, "%s - not possible to write buffer, will try next time, result= 0x%08x" , __func__, result);
            this->state.wl_sec_erase_cycle_count = this->state.wl_max_sec_erase_cycle_count - 1; // we will update next time
            return result;
        }
    }
    // done... block moved.
    // Here we will update structures...
    // Update bits and save to flash:
    uint32_t byte_pos = this->state.wl_dummy_sec_pos * this->cfg.wl_pos_update_record_size;
    this->fillOkBuff(this->state.wl_dummy_sec_pos);
    // write state to mem. We updating only affected bits
    result |= this->partition->write(this->addr_state1 + sizeof(wl_state_t) + byte_pos, this->temp_buff, this->cfg.wl_pos_update_record_size);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "%s - update position 1 result= 0x%08x" , __func__, result);
        this->state.wl_sec_erase_cycle_count = this->state.wl_max_sec_erase_cycle_count - 1; // we will update next time
        return result;
    }
    this->fillOkBuff(this->state.wl_dummy_sec_pos);
    result |= this->partition->write(this->addr_state2 + sizeof(wl_state_t) + byte_pos, this->temp_buff, this->cfg.wl_pos_update_record_size);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "%s - update position 2 result= 0x%08x" , __func__, result);
        this->state.wl_sec_erase_cycle_count = this->state.wl_max_sec_erase_cycle_count - 1; // we will update next time
        return result;
    }

    this->state.wl_dummy_sec_pos++;
    if (this->state.wl_dummy_sec_pos >= this->state.wl_part_max_sec_pos) {
        this->state.wl_dummy_sec_pos = 0;
        // one loop more
        this->state.wl_dummy_sec_move_count++;
        if (this->state.wl_dummy_sec_move_count >= (this->state.wl_part_max_sec_pos - 1)) {
            this->state.wl_dummy_sec_move_count = 0;
        }
        // write main state
        this->state.crc32 = crc32::crc32_le(WL_CFG_CRC_CONST, (uint8_t *)&this->state, WL_STATE_CRC_LEN_V2);

        result = this->partition->erase_range(this->addr_state1, this->state_size);
        WL_RESULT_CHECK(result);
        result = this->partition->write(this->addr_state1, &this->state, sizeof(wl_state_t));
        WL_RESULT_CHECK(result);
        result = this->partition->erase_range(this->addr_state2, this->state_size);
        WL_RESULT_CHECK(result);
        result = this->partition->write(this->addr_state2, &this->state, sizeof(wl_state_t));
        WL_RESULT_CHECK(result);
        ESP_LOGD(TAG, "%s - wl_dummy_sec_move_count= 0x%08" PRIx32 ", wl_dummy_sec_pos= 0x%08" PRIx32 ", ", __func__, this->state.wl_dummy_sec_move_count, this->state.wl_dummy_sec_pos);
    }
    // Save structures to the flash... and check result
    if (result == ESP_OK) {
        ESP_LOGV(TAG, "%s - result= 0x%08x" , __func__, result);
    } else {
        ESP_LOGE(TAG, "%s - result= 0x%08x" , __func__, result);
    }
    return result;
}

size_t WL_Flash::calcAddr(size_t addr)
{
    size_t result = (this->flash_size - this->state.wl_dummy_sec_move_count * this->cfg.wl_page_size + addr) % this->flash_size;
    size_t dummy_addr = this->state.wl_dummy_sec_pos * this->cfg.wl_page_size;
    if (result < dummy_addr) {
    } else {
        result += this->cfg.wl_page_size;
    }
    ESP_LOGV(TAG, "%s - addr= 0x%08" PRIx32 " -> result= 0x%08" PRIx32 ", dummy_addr= 0x%08" PRIx32 , __func__, (uint32_t) addr, (uint32_t) result, (uint32_t)dummy_addr);
    return result;
}


size_t WL_Flash::get_flash_size()
{
    if (!this->configured) {
        return 0;
    }
    return this->flash_size;
}

size_t WL_Flash::get_sector_size()
{
    if (!this->configured) {
        return 0;
    }
    return this->cfg.flash_sector_size;
}

esp_err_t WL_Flash::erase_sector(size_t sector)
{
    esp_err_t result = ESP_OK;
    if (!this->initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    ESP_LOGD(TAG, "%s - sector= 0x%08" PRIx32 , __func__, (uint32_t) sector);
    result = this->updateWL();
    WL_RESULT_CHECK(result);
    size_t virt_addr = this->calcAddr(sector * this->cfg.flash_sector_size);
    result = this->partition->erase_sector((this->cfg.wl_partition_start_addr + virt_addr) / this->cfg.flash_sector_size);
    WL_RESULT_CHECK(result);
    return result;
}

esp_err_t WL_Flash::erase_range(size_t start_address, size_t size)
{
    esp_err_t result = ESP_OK;
    if (!this->initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    ESP_LOGD(TAG, "%s - start_address= 0x%08" PRIx32 ", size= 0x%08" PRIx32 , __func__, (uint32_t) start_address, (uint32_t) size);
    size_t erase_count = (size + this->cfg.flash_sector_size - 1) / this->cfg.flash_sector_size;
    size_t start_sector = start_address / this->cfg.flash_sector_size;
    for (size_t i = 0; i < erase_count; i++) {
        result = this->erase_sector(start_sector + i);
        WL_RESULT_CHECK(result);
    }
    ESP_LOGV(TAG, "%s - result= 0x%08x" , __func__, result);
    return result;
}

esp_err_t WL_Flash::write(size_t dest_addr, const void *src, size_t size)
{
    esp_err_t result = ESP_OK;
    if (!this->initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    ESP_LOGD(TAG, "%s - dest_addr= 0x%08" PRIx32 ", size= 0x%08" PRIx32 , __func__, (uint32_t) dest_addr, (uint32_t) size);
    uint32_t count = (size - 1) / this->cfg.wl_page_size;
    for (size_t i = 0; i < count; i++) {
        size_t virt_addr = this->calcAddr(dest_addr + i * this->cfg.wl_page_size);
        result = this->partition->write(this->cfg.wl_partition_start_addr + virt_addr, &((uint8_t *)src)[i * this->cfg.wl_page_size], this->cfg.wl_page_size);
        WL_RESULT_CHECK(result);
    }
    size_t virt_addr_last = this->calcAddr(dest_addr + count * this->cfg.wl_page_size);
    result = this->partition->write(this->cfg.wl_partition_start_addr + virt_addr_last, &((uint8_t *)src)[count * this->cfg.wl_page_size], size - count * this->cfg.wl_page_size);
    WL_RESULT_CHECK(result);
    return result;
}

esp_err_t WL_Flash::read(size_t src_addr, void *dest, size_t size)
{
    esp_err_t result = ESP_OK;
    if (!this->initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    ESP_LOGD(TAG, "%s - src_addr= 0x%08" PRIx32 ", size= 0x%08" PRIx32 , __func__, (uint32_t) src_addr, (uint32_t) size);
    uint32_t count = (size - 1) / this->cfg.wl_page_size;
    for (size_t i = 0; i < count; i++) {
        size_t virt_addr = this->calcAddr(src_addr + i * this->cfg.wl_page_size);
        ESP_LOGV(TAG, "%s - real_addr= 0x%08" PRIx32 ", size= 0x%08" PRIx32 , __func__, (uint32_t) (this->cfg.wl_partition_start_addr + virt_addr), (uint32_t) size);
        result = this->partition->read(this->cfg.wl_partition_start_addr + virt_addr, &((uint8_t *)dest)[i * this->cfg.wl_page_size], this->cfg.wl_page_size);
        WL_RESULT_CHECK(result);
    }
    size_t virt_addr_last = this->calcAddr(src_addr + count * this->cfg.wl_page_size);
    result = this->partition->read(this->cfg.wl_partition_start_addr + virt_addr_last, &((uint8_t *)dest)[count * this->cfg.wl_page_size], size - count * this->cfg.wl_page_size);
    WL_RESULT_CHECK(result);
    return result;
}

Partition *WL_Flash::get_part()
{
    return this->partition;
}
wl_config_t *WL_Flash::get_cfg()
{
    return &this->cfg;
}

esp_err_t WL_Flash::flush()
{
    esp_err_t result = ESP_OK;
    this->state.wl_sec_erase_cycle_count = this->state.wl_max_sec_erase_cycle_count - 1;
    result = this->updateWL();
    ESP_LOGD(TAG, "%s - result= 0x%08x, wl_dummy_sec_move_count= 0x%08" PRIx32, __func__, result, this->state.wl_dummy_sec_move_count);
    return result;
}
