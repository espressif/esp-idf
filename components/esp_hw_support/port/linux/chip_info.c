/*
 * SPDX-FileCopyrightText: 2013-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_chip_info.h"
#include "esp_mac.h"

void esp_chip_info(esp_chip_info_t *out_info)
{
    memset(out_info, 0, sizeof(*out_info));

    out_info->model = CHIP_POSIX_LINUX;

    // TODO: May need to adjust once networking becomes available on POSIX/Linux
    out_info->features = 0;
    out_info->revision = 0;
    out_info->cores = 1;
}

esp_err_t esp_read_mac(uint8_t *mac, esp_mac_type_t type)
{
    // Provide Locally Administered (OUI range) MAC address on POSIX/Linux
    mac[0] = 0x02;
    mac[1] = 0x12;
    mac[2] = 0x34;
    mac[3] = 0x56;
    mac[4] = 0x78;
    mac[5] = 0xab;
    return ESP_OK;
}
