/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _WL_Flash_H_
#define _WL_Flash_H_

#include "esp_err.h"
#include "Flash_Access.h"
#include "Partition.h"
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

    virtual esp_err_t config(wl_config_t *cfg, Partition *partition);
    virtual esp_err_t init();

    size_t get_flash_size() override;
    size_t get_sector_size() override;

    esp_err_t erase_sector(size_t sector) override;
    esp_err_t erase_range(size_t start_address, size_t size) override;

    esp_err_t write(size_t dest_addr, const void *src, size_t size) override;
    esp_err_t read(size_t src_addr, void *dest, size_t size) override;

    esp_err_t flush() override;

    Partition *get_part();
    wl_config_t *get_cfg();

protected:
    bool configured = false;
    bool initialized = false;
    wl_state_t state;
    wl_config_t cfg;
    Partition *partition = NULL;

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
    uint32_t pos_data[4];

    esp_err_t initSections();
    esp_err_t updateWL();
    esp_err_t recoverPos();
    size_t calcAddr(size_t addr);

    esp_err_t updateVersion();
    esp_err_t updateV1_V2();
    void fillOkBuff(int n);
    bool OkBuffSet(int n);
};

#endif // _WL_Flash_H_
