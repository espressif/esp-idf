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

#include <string.h>
#include "WL_Config.h"
#include "WL_Flash.h"
#include "Flash_Emulator.h"
#include "catch.hpp"

#define FLASH_SECTOR_SIZE       512
#define FLASH_USED_SECTOR       (1024 - 3)
#define FLASH_ACCESS_SIZE       (FLASH_SECTOR_SIZE*(FLASH_USED_SECTOR + 1 + 2))
#define FLASH_START_ADDR        0x1000
#define FLASH_PAGE_SIZE         FLASH_SECTOR_SIZE*1
#define FLASH_UPDATERATE        3
#define FLASH_TEMP_SIZE         FLASH_SECTOR_SIZE
#define FLASH_WR_BLOCK_SIZE     2

static const char *TAG = "wl_test_host";
Flash_Access *s_flash;

extern bool test_power_down(WL_Flash *wl_flash, Flash_Emulator *emul, uint32_t used_sectors_count);

#define TEST_COUNT_MAX 100

TEST_CASE("flash starts with all bytes == 0xff", "[spi_flash_emu]")
{
    wl_config_t *wl = new wl_config_t();

    wl->full_mem_size = FLASH_ACCESS_SIZE;
    wl->start_addr = FLASH_START_ADDR;
    wl->sector_size = FLASH_SECTOR_SIZE;
    wl->page_size = FLASH_PAGE_SIZE;
    wl->updaterate = FLASH_UPDATERATE;
    wl->temp_buff_size = FLASH_TEMP_SIZE;
    wl->wr_size = FLASH_WR_BLOCK_SIZE;

    WL_Flash *wl_flash = new WL_Flash();
    Flash_Emulator *emul = new Flash_Emulator(FLASH_ACCESS_SIZE + FLASH_START_ADDR, FLASH_SECTOR_SIZE);
    CHECK(wl_flash->config(wl, emul) == ESP_OK);

    test_power_down(wl_flash, emul, TEST_COUNT_MAX);
}


