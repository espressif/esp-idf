// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_bt_device.h"
#include "esp_console.h"

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

void register_ble_address(void);

void register_bluetooth(void)
{
    register_ble_address();
}

int bt_mac(int argc, char** argv)
{
    const uint8_t *mac = esp_bt_dev_get_address();
    printf("+BTMAC:"MACSTR"\n", MAC2STR(mac));
    return 0;
}

void register_ble_address(void)
{
    const esp_console_cmd_t cmd = {
        .command = "btmac",
        .help = "get BT mac address",
        .hint = NULL,
        .func = &bt_mac,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

