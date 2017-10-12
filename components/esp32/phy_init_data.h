// Copyright 2016 Espressif Systems (Shanghai) PTE LTD
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

#pragma once
#include "esp_phy_init.h"
#include "sdkconfig.h"

// constrain a value between 'low' and 'high', inclusive
#define LIMIT(val, low, high) ((val < low) ? low : (val > high) ? high : val)

#define PHY_INIT_MAGIC "PHYINIT"

static const char phy_init_magic_pre[] = PHY_INIT_MAGIC;

/**
 * @brief Structure containing default recommended PHY initialization parameters.
 */
static const esp_phy_init_data_t phy_init_data= { {
        1,
        3,
        0x05,
        0x04,
        0x06,
        0x05,
        0x01,
        0x06,
        0x05,
        0x04,
        0x06,
        0x04,
        0x05,
        0x00,
        0x00,
        0x00,
        0x00,
        0x05,
        0x04,
        0x06,
        0x05,
        0x01,
        0x06,
        0x05,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x0a,
        0x0a,
        0x0c,
        0xf0,
        0xf0,
        0xf0,
        0xe0,
        0xe0,
        0xe0,
        0x18,
        0x18,
        0x18,
        LIMIT(CONFIG_ESP32_PHY_MAX_TX_POWER * 4, 0, 78),
        LIMIT(CONFIG_ESP32_PHY_MAX_TX_POWER * 4, 0, 76),
        LIMIT(CONFIG_ESP32_PHY_MAX_TX_POWER * 4, 0, 74),
        LIMIT(CONFIG_ESP32_PHY_MAX_TX_POWER * 4, 0, 68),
        LIMIT(CONFIG_ESP32_PHY_MAX_TX_POWER * 4, 0, 60),
        LIMIT(CONFIG_ESP32_PHY_MAX_TX_POWER * 4, 0, 52),
        0,
        0,
        1,
        1,
        2,
        3,
        4,
        5,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
} };

static const char phy_init_magic_post[] = PHY_INIT_MAGIC;

