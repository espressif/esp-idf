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


#include "WL_Config.h"
#include "WL_Flash.h"
#include "Flash_Emulator.h"
#ifdef _MSC_VER
#define CHECK(m)
#else
#include "catch.hpp"
#endif

extern Flash_Access *s_flash;

bool test_power_down(WL_Flash *wl_flash, Flash_Emulator *emul, uint32_t used_sectors_count)
{
    REQUIRE(wl_flash->init() == ESP_OK);
    s_flash = wl_flash;

    uint32_t add_const = 0;
    int32_t sectors_count = s_flash->chip_size() / s_flash->sector_size();
    esp_err_t err = ESP_OK;
    uint32_t *sector_data = new uint32_t[s_flash->sector_size() / sizeof(uint32_t)];

    for (int32_t i = 0; i < sectors_count; i++) {
        REQUIRE(s_flash->erase_sector(i) == ESP_OK);
        for (uint32_t m = 0; m < s_flash->sector_size() / sizeof(uint32_t); m++) {
            uint32_t temp_data = i * s_flash->sector_size() + add_const + m;
            sector_data[m] = temp_data;
        }
        REQUIRE(s_flash->write(i * s_flash->sector_size(), sector_data, s_flash->sector_size()) == ESP_OK);
    }
    for (int32_t i = 0; i < sectors_count; i++) {
        err |= s_flash->read(i * s_flash->sector_size(), sector_data, s_flash->sector_size());
        for (uint32_t m = 0; m < s_flash->sector_size() / sizeof(uint32_t); m++) {
            uint32_t temp_data = i * s_flash->sector_size() + add_const + m;
            REQUIRE(temp_data == sector_data[m]);
            if (temp_data != sector_data[m]) {
                printf("Error - read: %08x, expected %08x\n", sector_data[m], temp_data);
            }
        }
    }

    int32_t max_count = 100;
    int32_t max_check_count = used_sectors_count;
    printf("used_sectors_count=%d\n", used_sectors_count);
    for (int32_t k = 0; k < max_check_count; k++) {

        emul->SetResetCount(max_count);
        int32_t err_sector = -1;
        for (int32_t i = 0; i < sectors_count; i++) {
            err = ESP_OK;
            err = s_flash->erase_sector(i);
            if (err != ESP_OK) {
                err_sector = i;
                break;
            }
            for (uint32_t m = 0; m < s_flash->sector_size() / sizeof(uint32_t); m++) {
                uint32_t temp_data = i * s_flash->sector_size() + add_const + m;
                sector_data[m] = temp_data;
            }
            err = s_flash->write(i * s_flash->sector_size(), sector_data, s_flash->sector_size());
            if (err != ESP_OK) {
                err_sector = i;
                break;
            }
        }
        if (err_sector >= 0) {
            max_count++;
        } else {
            max_count = 0;
        }
        emul->SetResetCount(INT32_MAX);
        REQUIRE(wl_flash->init() == ESP_OK);
        for (int32_t i = 0; i < sectors_count; i++) {
            if (i != err_sector) {
                err |= s_flash->read(i * s_flash->sector_size(), sector_data, s_flash->sector_size());
                for (uint32_t m = 0; m < s_flash->sector_size() / sizeof(uint32_t); m++) {
                    uint32_t temp_data = i * s_flash->sector_size() + add_const + m;
                    REQUIRE(temp_data == sector_data[m]);
                    if (temp_data != sector_data[m]) {
                        printf("Error - read: %08x, expected %08x, m=%i, sector=%i\n", sector_data[m], temp_data, m, i);
                    }
                }
            }
        }
        if (err_sector != -1) {
            err |= s_flash->erase_sector(err_sector);
            for (uint32_t m = 0; m < s_flash->sector_size() / sizeof(uint32_t); m++) {
                uint32_t temp_data = err_sector * s_flash->sector_size() + add_const + m;
                sector_data[m] = temp_data;
            }
            err |= s_flash->write(err_sector * s_flash->sector_size(), sector_data, s_flash->sector_size());
        }
        printf("[%3.f%%] err_sector=%i\n", (float)k / ((float)max_check_count) * 100.0f, err_sector);
    }
    delete[] sector_data;
    return true;
}
