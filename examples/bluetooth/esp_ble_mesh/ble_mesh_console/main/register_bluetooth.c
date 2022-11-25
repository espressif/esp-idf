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
#include "string.h"
#include "esp_console.h"
#include "ble_mesh_console_lib.h"

#ifdef CONFIG_BT_BLUEDROID_ENABLED
#include "esp_bt_device.h"
#endif

#ifdef CONFIG_BT_NIMBLE_ENABLED
#include "host/ble_hs.h"
#endif

void register_ble_address(void);

void register_bluetooth(void)
{
    register_ble_address();
}

int get_bd_addr(uint8_t addr[6])
{
    if (addr) {
#ifdef CONFIG_BT_BLUEDROID_ENABLED
        memcpy(addr, esp_bt_dev_get_address(), 6);
        return 0;
#endif

#ifdef CONFIG_BT_NIMBLE_ENABLED
        uint8_t own_addr_type = 0;
        uint8_t mac[6] = {0};
        int rc = 0;

        rc = ble_hs_id_infer_auto(0, &own_addr_type);
        if (rc != 0) {
            printf("error determining address type; rc=%d\n", rc);
            return rc;
        }

        ble_hs_id_copy_addr(own_addr_type, mac, NULL);

        uint8_t *src = addr;
        uint8_t *dst = mac + 5;
        for (uint8_t length = 6; length > 0; length--) {
            *src++ = *dst--;
        }
        return 0;
#endif
    }

    return -1;
}

int bt_mac(int argc, char** argv)
{
    uint8_t mac[6] = {0};

    get_bd_addr(mac);

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

