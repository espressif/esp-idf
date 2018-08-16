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

#ifndef _WL_Ext_Safe_H_
#define _WL_Ext_Safe_H_

#include "WL_Flash.h"
#include "WL_Ext_Cfg.h"
#include "WL_Ext_Perf.h"

class WL_Ext_Safe : public WL_Ext_Perf
{
public:
    WL_Ext_Safe();
    ~WL_Ext_Safe() override;

    esp_err_t config(WL_Config_s *cfg, Flash_Access *flash_drv) override;
    esp_err_t init() override;

    size_t chip_size() override;

protected:
    esp_err_t erase_sector_fit(uint32_t start_sector, uint32_t count) override;

    // Dump Sector
    uint32_t dump_addr; // dump buffer address
    uint32_t state_addr;// sectore where state of transaction will be stored

    esp_err_t recover();
};

#endif // _WL_Ext_Safe_H_