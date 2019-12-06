// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "bootloader_common.h"
#include "bootloader_clock.h"
#include "soc/efuse_reg.h"
#include "soc/apb_ctrl_reg.h"

uint8_t bootloader_common_get_chip_revision(void)
{
    uint8_t eco_bit0, eco_bit1, eco_bit2;
    eco_bit0 = (REG_READ(EFUSE_BLK0_RDATA3_REG) & 0xF000) >> 15;
    eco_bit1 = (REG_READ(EFUSE_BLK0_RDATA5_REG) & 0x100000) >> 20;
    eco_bit2 = (REG_READ(APB_CTRL_DATE_REG) & 0x80000000) >> 31;
    uint32_t combine_value = (eco_bit2 << 2) | (eco_bit1 << 1) | eco_bit0;
    uint8_t chip_ver = 0;
    switch (combine_value) {
    case 0:
        chip_ver = 0;
        break;
    case 1:
        chip_ver = 1;
        break;
    case 3:
        chip_ver = 2;
        break;
    case 7:
        chip_ver = 3;
        break;
    default:
        chip_ver = 0;
        break;
    }
    return chip_ver;
}

int bootloader_clock_get_rated_freq_mhz()
{
    //Check if ESP32 is rated for a CPU frequency of 160MHz only
    if (REG_GET_BIT(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_CPU_FREQ_RATED) &&
        REG_GET_BIT(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_CPU_FREQ_LOW)) {
        return 160;
    }
    return 240;
}
