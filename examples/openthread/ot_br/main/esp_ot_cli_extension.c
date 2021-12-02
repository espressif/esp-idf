/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 * OpenThread Command Line Example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_openthread.h"
#include "openthread/cli.h"
#include "esp_ot_cli_extension.h"
#include "esp_br_wifi_cmd.h"

static const otCliCommand kCommands[] = {
    {"sta", esp_ot_process_wifi_sta},
    {"wifiinfo", esp_ot_process_get_wifi_info}
};

void esp_cli_custom_command_init()
{
    otInstance *instance = esp_openthread_get_instance();
    otCliSetUserCommands(kCommands, (sizeof(kCommands) / sizeof(kCommands[0])), instance);
}
