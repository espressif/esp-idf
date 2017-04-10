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

#include <stdio.h>
#include "esp_log.h"
#include "WL_Flash.h"
#include <stdlib.h>
#include "crc32.h"
#include <string.h>

static const char *TAG = "wl_flash";
#ifndef WL_CFG_CRC_CONST
#define WL_CFG_CRC_CONST UINT32_MAX
#endif // WL_CFG_CRC_CONST 

#define WL_RESULT_CHECK(result) \
    if (result != ESP_OK) { \
        ESP_LOGE(TAG,"%s(%d): result = 0x%08x", __FUNCTION__, __LINE__, result); \
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

esp_err_t WL_Flash::config(wl_config_t *cfg, Flash_Access *flash_drv)
{
    ESP_LOGV(TAG, "%s start_addr=0x%08x, full_mem_size=0x%08x, page_size=0x%08x, sector_size=0x%08x, updaterate=0x%08x, wr_size=0x%08x, version=0x%08x, temp_buff_size=0x%08x", __func__,
             (uint32_t) cfg->start_addr,
             cfg->full_mem_size,
             cfg->page_size,
             cfg->sector_size,
             cfg->updaterate,
             cfg->wr_size,
             cfg->version,
             (uint32_t) cfg->temp_buff_size);

    cfg->crc = crc32::crc32_le(WL_CFG_CRC_CONST, (const unsigned char *)cfg, sizeof(wl_config_t) - sizeof(cfg->crc));
    esp_err_t result = ESP_OK;
    memcpy(&this->cfg, cfg, sizeof(wl_config_t));
    this->configured = false;
    if (cfg == NULL) {
        result = ESP_ERR_INVALID_ARG;
    }
    this->flash_drv = flash_drv;
    if (flash_drv == NULL) {
        result = ESP_ERR_INVALID_ARG;
    }
    if ((this->cfg.sector_size % this->cfg.temp_buff_size) != 0) {
        result = ESP_ERR_INVALID_ARG;
    }
    if (this->cfg.page_size < this->cfg.sector_size) {
        result = ESP_ERR_INVALID_ARG;
    }
    WL_RESULT_CHECK(result);

    this->temp_buff = (uint8_t *)malloc(this->cfg.temp_buff_size);
    this->state_size = this->cfg.sector_size;
    if (this->state_size < (sizeof(wl_state_t) + (this->cfg.full_mem_size / this->cfg.sector_size)*this->cfg.wr_size)) {
        this->state_size = ((sizeof(wl_state_t) + (this->cfg.full_mem_size / this->cfg.sector_size) * this->cfg.wr_size) + this->cfg.sector_size - 1) / this->cfg.sector_size;
        this->state_size = this->state_size * this->cfg.sector_size;
    }
    this->cfg_size = (sizeof(wl_config_t) + this->cfg.sector_size - 1) / this->cfg.sector_size;
    this->cfg_size = cfg_size * this->cfg.sector_size;

    this->addr_cfg = this->cfg.start_addr + this->cfg.full_mem_size - this->cfg_size;
    this->addr_state1 = this->cfg.start_addr + this->cfg.full_mem_size - this->state_size * 2 - this->cfg_size; // allocate data at the end of memory
    this->addr_state2 = this->cfg.start_addr + this->cfg.full_mem_size - this->state_size * 1 - this->cfg_size; // allocate data at the end of memory

    this->flash_size = ((this->cfg.full_mem_size - this->state_size * 2 - this->cfg_size) / this->cfg.page_size - 1) * this->cfg.page_size; // -1 remove dummy block

    ESP_LOGV(TAG, "%s - this->addr_state1=0x%08x", __func__, (uint32_t) this->addr_state1);
    ESP_LOGV(TAG, "%s - this->addr_state2=0x%08x", __func__, (uint32_t) this->addr_state2);

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
    this->flash_drv->read(this->addr_state1, &this->state, sizeof(wl_state_t));
    wl_state_t sa_copy;
    wl_state_t *state_copy = &sa_copy;
    result = this->flash_drv->read(this->addr_state2, state_copy, sizeof(wl_state_t));
    WL_RESULT_CHECK(result);

    int check_size = sizeof(wl_state_t) - sizeof(uint32_t);
    // Chech CRC and recover state
    uint32_t crc1 = crc32::crc32_le(WL_CFG_CRC_CONST, (uint8_t *)&this->state, check_size);
    uint32_t crc2 = crc32::crc32_le(WL_CFG_CRC_CONST, (uint8_t *)state_copy, check_size);

    ESP_LOGD(TAG, "%s - config ID=%i, stored ID=%i, access_count=%i, block_size=%i, max_count=%i, pos=%i, move_count=%i",
             __func__,
             this->cfg.version,
             this->state.version,
             this->state.access_count,
             this->state.block_size,
             this->state.max_count,
             this->state.pos,
             this->state.move_count);


    ESP_LOGD(TAG, "%s starts: crc1=%i, crc2 = %i, this->state.crc=%i, state_copy->crc=%i", __func__, crc1, crc2, this->state.crc, state_copy->crc);
    if ((crc1 == this->state.crc) && (crc2 == state_copy->crc)) {
        // The state is OK. Check the ID
        if (this->state.version != this->cfg.version) {
            result = this->initSections();
            WL_RESULT_CHECK(result);
            result = this->recoverPos();
            WL_RESULT_CHECK(result);
        } else {
            if (crc1 != crc2) {// we did not update second structure.
                result = this->flash_drv->erase_range(this->addr_state2, this->state_size);
                WL_RESULT_CHECK(result);
                result = this->flash_drv->write(this->addr_state2, &this->state, sizeof(wl_state_t));
                WL_RESULT_CHECK(result);
                for (size_t i = 0; i < ((this->cfg.full_mem_size / this->cfg.sector_size)*this->cfg.wr_size); i++) {
                    uint8_t pos_bits = 0;
                    result = this->flash_drv->read(this->addr_state1 + sizeof(wl_state_t) + i, &pos_bits, 1);
                    WL_RESULT_CHECK(result);
                    if (pos_bits != 0xff) {
                        result = this->flash_drv->write(this->addr_state2 + sizeof(wl_state_t) + i, &pos_bits, 1);
                        WL_RESULT_CHECK(result);
                    }
                }
            }
            ESP_LOGD(TAG, "%s: crc1=%i, crc2 = %i, result=%i", __func__, crc1, crc2, result);
            result = this->recoverPos();
            WL_RESULT_CHECK(result);
        }
    } else if ((crc1 != this->state.crc) && (crc2 != state_copy->crc)) { // This is just new flash
        result = this->initSections();
        WL_RESULT_CHECK(result);
        result = this->recoverPos();
        WL_RESULT_CHECK(result);
    } else {
        // recover broken state
        if (crc1 == this->state.crc) {// we have to recover state 2
            result = this->flash_drv->erase_range(this->addr_state2, this->state_size);
            WL_RESULT_CHECK(result);
            result = this->flash_drv->write(this->addr_state2, &this->state, sizeof(wl_state_t));
            WL_RESULT_CHECK(result);
            for (size_t i = 0; i < ((this->cfg.full_mem_size / this->cfg.sector_size) * this->cfg.wr_size); i++) {
                uint8_t pos_bits = 0;
                result = this->flash_drv->read(this->addr_state1 + sizeof(wl_state_t) + i, &pos_bits, 1);
                WL_RESULT_CHECK(result);
                if (pos_bits != 0xff) {
                    result = this->flash_drv->write(this->addr_state2 + sizeof(wl_state_t) + i, &pos_bits, 1);
                    WL_RESULT_CHECK(result);
                }
            }
            result = this->flash_drv->read(this->addr_state2, &this->state, sizeof(wl_state_t));
            WL_RESULT_CHECK(result);
        } else { // we have to recover state 1
            result = this->flash_drv->erase_range(this->addr_state1, this->state_size);
            WL_RESULT_CHECK(result);
            result = this->flash_drv->write(this->addr_state1, state_copy, sizeof(wl_state_t));
            WL_RESULT_CHECK(result);
            for (size_t i = 0; i < ((this->cfg.full_mem_size / this->cfg.sector_size) * this->cfg.wr_size); i++) {
                uint8_t pos_bits = 0;
                result = this->flash_drv->read(this->addr_state2 + sizeof(wl_state_t) + i, &pos_bits, 1);
                WL_RESULT_CHECK(result);
                if (pos_bits != 0xff) {
                    result = this->flash_drv->write(this->addr_state1 + sizeof(wl_state_t) + i, &pos_bits, 1);
                    WL_RESULT_CHECK(result);
                }
            }
            result = this->flash_drv->read(this->addr_state1, &this->state, sizeof(wl_state_t));
            WL_RESULT_CHECK(result);
            this->state.pos = this->state.max_pos - 1;
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
        ESP_LOGE(TAG, "%s: returned 0x%x", __func__, result);
        return result;
    }
    this->initialized = true;
    return ESP_OK;
}

esp_err_t WL_Flash::recoverPos()
{
    esp_err_t result = ESP_OK;
    size_t position = 0;
    for (size_t i = 0; i < this->state.max_pos; i++) {
        uint8_t pos_bits = 0;
        result = this->flash_drv->read(this->addr_state1 + sizeof(wl_state_t) + i * this->cfg.wr_size, &pos_bits, 1);
        WL_RESULT_CHECK(result);
        position = i;
        if (pos_bits == 0xff) {
            break; // we have found position
        }
    }
    this->state.pos = position;
    if (this->state.pos == this->state.max_pos) {
        this->state.pos--;
    }
    ESP_LOGD(TAG, "%s - this->state.pos=0x%08x, result=%08x", __func__, this->state.pos, result);
    return result;
}

esp_err_t WL_Flash::initSections()
{
    esp_err_t result = ESP_OK;
    this->state.pos = 0;
    this->state.access_count = 0;
    this->state.move_count = 0;
    // max count
    this->state.max_count = this->flash_size / this->state_size * this->cfg.updaterate;
    if (this->cfg.updaterate != 0) {
        this->state.max_count = this->cfg.updaterate;
    }
    this->state.version = this->cfg.version;
    this->state.block_size = this->cfg.page_size;
    this->used_bits = 0;

    this->state.max_pos = 1 + this->flash_size / this->cfg.page_size;

    this->state.crc = crc32::crc32_le(WL_CFG_CRC_CONST, (uint8_t *)&this->state, sizeof(wl_state_t) - sizeof(uint32_t));

    result = this->flash_drv->erase_range(this->addr_state1, this->state_size);
    WL_RESULT_CHECK(result);
    result = this->flash_drv->write(this->addr_state1, &this->state, sizeof(wl_state_t));
    WL_RESULT_CHECK(result);
    // write state copy
    result = this->flash_drv->erase_range(this->addr_state2, this->state_size);
    WL_RESULT_CHECK(result);
    result = this->flash_drv->write(this->addr_state2, &this->state, sizeof(wl_state_t));
    WL_RESULT_CHECK(result);

    result = this->flash_drv->erase_range(this->addr_cfg, this->cfg_size);
    WL_RESULT_CHECK(result);
    result = this->flash_drv->write(this->addr_cfg, &this->cfg, sizeof(wl_config_t));
    WL_RESULT_CHECK(result);

    ESP_LOGD(TAG, "%s - this->state->max_count=%08x, this->state->max_pos=%08x", __func__, this->state.max_count, this->state.max_pos);
    ESP_LOGD(TAG, "%s - result=%08x", __func__, result);
    return result;
}

esp_err_t WL_Flash::updateWL()
{
    esp_err_t result = ESP_OK;
    this->state.access_count++;
    if (this->state.access_count < this->state.max_count) {
        return result;
    }
    // Here we have to move the block and increase the state
    this->state.access_count = 0;
    ESP_LOGV(TAG, "%s - access_count=0x%08x, pos=0x%08x", __func__, this->state.access_count, this->state.pos);
    // copy data to dummy block
    size_t data_addr = this->state.pos + 1; // next block, [pos+1] copy to [pos]
    if (data_addr >= this->state.max_pos) {
        data_addr = 0;
    }
    data_addr = this->cfg.start_addr + data_addr * this->cfg.page_size;
    this->dummy_addr = this->cfg.start_addr + this->state.pos * this->cfg.page_size;
    result = this->flash_drv->erase_range(this->dummy_addr, this->cfg.page_size);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "%s - erase wl dummy sector result=%08x", __func__, result);
        this->state.access_count = this->state.max_count - 1; // we will update next time
        return result;
    }

    size_t copy_count = this->cfg.page_size / this->cfg.temp_buff_size;
    for (size_t i = 0; i < copy_count; i++) {
        result = this->flash_drv->read(data_addr + i * this->cfg.temp_buff_size, this->temp_buff, this->cfg.temp_buff_size);
        if (result != ESP_OK) {
            ESP_LOGE(TAG, "%s - not possible to read buffer, will try next time, result=%08x", __func__, result);
            this->state.access_count = this->state.max_count - 1; // we will update next time
            return result;
        }
        result = this->flash_drv->write(this->dummy_addr + i * this->cfg.temp_buff_size, this->temp_buff, this->cfg.temp_buff_size);
        if (result != ESP_OK) {
            ESP_LOGE(TAG, "%s - not possible to write buffer, will try next time, result=%08x", __func__, result);
            this->state.access_count = this->state.max_count - 1; // we will update next time
            return result;
        }
    }
    // done... block moved.
    // Here we will update structures...
    // Update bits and save to flash:
    uint32_t byte_pos = this->state.pos * this->cfg.wr_size;
    this->used_bits = 0;
    // write state to mem. We updating only affected bits
    result |= this->flash_drv->write(this->addr_state1 + sizeof(wl_state_t) + byte_pos, &this->used_bits, 1);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "%s - update position 1 result=%08x", __func__, result);
        this->state.access_count = this->state.max_count - 1; // we will update next time
        return result;
    }
    result |= this->flash_drv->write(this->addr_state2 + sizeof(wl_state_t) + byte_pos, &this->used_bits, 1);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "%s - update position 2 result=%08x", __func__, result);
        this->state.access_count = this->state.max_count - 1; // we will update next time
        return result;
    }

    this->state.pos++;
    if (this->state.pos >= this->state.max_pos) {
        this->state.pos = 0;
        // one loop more
        this->state.move_count++;
        if (this->state.move_count >= (this->state.max_pos - 1)) {
            this->state.move_count = 0;
        }
        // write main state
        this->state.crc = crc32::crc32_le(WL_CFG_CRC_CONST, (uint8_t *)&this->state, sizeof(wl_state_t) - sizeof(uint32_t));

        result = this->flash_drv->erase_range(this->addr_state1, this->state_size);
        WL_RESULT_CHECK(result);
        result = this->flash_drv->write(this->addr_state1, &this->state, sizeof(wl_state_t));
        WL_RESULT_CHECK(result);
        result = this->flash_drv->erase_range(this->addr_state2, this->state_size);
        WL_RESULT_CHECK(result);
        result = this->flash_drv->write(this->addr_state2, &this->state, sizeof(wl_state_t));
        WL_RESULT_CHECK(result);
        ESP_LOGD(TAG, "%s - move_count=%08x", __func__, this->state.move_count);
    }
    // Save structures to the flash... and check result
    if (result == ESP_OK) {
        ESP_LOGV(TAG, "%s - result=%08x", __func__, result);
    } else {
        ESP_LOGE(TAG, "%s - result=%08x", __func__, result);
    }
    return result;
}

size_t WL_Flash::calcAddr(size_t addr)
{
    size_t result = (this->flash_size - this->state.move_count * this->cfg.page_size + addr) % this->flash_size;
    size_t dummy_addr = this->state.pos * this->cfg.page_size;
    if (result < dummy_addr) {
    } else {
        result += this->cfg.page_size;
    }
    ESP_LOGV(TAG, "%s - addr=0x%08x -> result=0x%08x", __func__, (uint32_t) addr, (uint32_t) result);
    return result;
}


size_t WL_Flash::chip_size()
{
    if (!this->configured) {
        return 0;
    }
    return this->flash_size;
}
size_t WL_Flash::sector_size()
{
    if (!this->configured) {
        return 0;
    }
    return this->cfg.sector_size;
}


esp_err_t WL_Flash::erase_sector(size_t sector)
{
    esp_err_t result = ESP_OK;
    if (!this->initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    ESP_LOGV(TAG, "%s - sector=0x%08x", __func__, (uint32_t) sector);
    result = this->updateWL();
    WL_RESULT_CHECK(result);
    size_t virt_addr = this->calcAddr(sector * this->cfg.sector_size);
    result = this->flash_drv->erase_sector((this->cfg.start_addr + virt_addr) / this->cfg.sector_size);
    WL_RESULT_CHECK(result);
    return result;
}
esp_err_t WL_Flash::erase_range(size_t start_address, size_t size)
{
    esp_err_t result = ESP_OK;
    if (!this->initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    ESP_LOGV(TAG, "%s - start_address=0x%08x, size=0x%08x", __func__, (uint32_t) start_address, (uint32_t) size);
    size_t erase_count = (size + this->cfg.sector_size - 1) / this->cfg.sector_size;
    size_t start_sector = start_address / this->cfg.sector_size;
    for (size_t i = 0; i < erase_count; i++) {
        result = this->erase_sector(start_sector + i);
        WL_RESULT_CHECK(result);
    }
    ESP_LOGV(TAG, "%s - result=%08x", __func__, result);
    return result;
}

esp_err_t WL_Flash::write(size_t dest_addr, const void *src, size_t size)
{
    esp_err_t result = ESP_OK;
    if (!this->initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    ESP_LOGV(TAG, "%s - dest_addr=0x%08x, size=0x%08x", __func__, (uint32_t) dest_addr, (uint32_t) size);
    uint32_t count = (size - 1) / this->cfg.page_size;
    for (size_t i = 0; i < count; i++) {
        size_t virt_addr = this->calcAddr(dest_addr + i * this->cfg.page_size);
        result = this->flash_drv->write(this->cfg.start_addr + virt_addr, &((uint8_t *)src)[i * this->cfg.page_size], size);
        WL_RESULT_CHECK(result);
    }
    size_t virt_addr_last = this->calcAddr(dest_addr + count * this->cfg.page_size);
    result = this->flash_drv->write(this->cfg.start_addr + virt_addr_last, &((uint8_t *)src)[count * this->cfg.page_size], size - count * this->cfg.page_size);
    WL_RESULT_CHECK(result);
    return result;
}

esp_err_t WL_Flash::read(size_t src_addr, void *dest, size_t size)
{
    esp_err_t result = ESP_OK;
    if (!this->initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    ESP_LOGV(TAG, "%s - src_addr=0x%08x, size=0x%08x", __func__, (uint32_t) src_addr, (uint32_t) size);
    uint32_t count = (size - 1) / this->cfg.page_size;
    for (size_t i = 0; i < count; i++) {
        size_t virt_addr = this->calcAddr(src_addr + i * this->cfg.page_size);
        result = this->flash_drv->read(this->cfg.start_addr + virt_addr, &((uint8_t *)dest)[i * this->cfg.page_size], size);
        WL_RESULT_CHECK(result);
    }
    size_t virt_addr_last = this->calcAddr(src_addr + count * this->cfg.page_size);
    result = this->flash_drv->read(this->cfg.start_addr + virt_addr_last, &((uint8_t *)dest)[count * this->cfg.page_size], size - count * this->cfg.page_size);
    WL_RESULT_CHECK(result);
    return result;
}

Flash_Access *WL_Flash::get_drv()
{
    return this->flash_drv;
}
wl_config_t *WL_Flash::get_cfg()
{
    return &this->cfg;
}

esp_err_t WL_Flash::flush()
{
    esp_err_t result = ESP_OK;
    this->state.access_count = this->state.max_count - 1;
    result = this->updateWL();
    ESP_LOGV(TAG, "%s - result=%08x", __func__, result);
    return result;
}
