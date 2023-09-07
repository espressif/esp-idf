/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _WL_Ext_Perf_H_
#define _WL_Ext_Perf_H_

#include "Partition.h"
#include "WL_Flash.h"
#include "WL_Ext_Cfg.h"

class WL_Ext_Perf : public WL_Flash
{
public:
    WL_Ext_Perf();
    ~WL_Ext_Perf() override;

    esp_err_t config(WL_Config_s *cfg, Partition *partition) override;
    esp_err_t init() override;

    size_t get_flash_size() override;
    size_t get_sector_size() override;

    esp_err_t erase_sector(size_t sector) override;
    esp_err_t erase_range(size_t start_address, size_t size) override;

protected:
    uint32_t flash_sector_size;
    uint32_t fat_sector_size;
    /*when flash and fat sector sizes are not equal (where flash_sector_size >= fat_sector_size),
     this flash_fat_sector_size_factor will be used while flash sector erase or read-write operation.
     This factor is the ratio of flash_sector_size to fat_sector_size*/
    uint32_t flash_fat_sector_size_factor;
    uint32_t *sector_buffer;    /*Ptr to sector buffer allocated in heap memory for temporary
                                  storage of flash sector during erase operation*/

    virtual esp_err_t erase_sector_fit(uint32_t start_sector, uint32_t count);

};

#endif // _WL_Ext_Perf_H_
