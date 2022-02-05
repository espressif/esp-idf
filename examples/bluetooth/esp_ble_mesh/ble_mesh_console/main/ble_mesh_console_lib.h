/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_CONSOLE_LIB_H_
#define _BLE_MESH_CONSOLE_LIB_H_

#include <stdio.h>
#include <string.h>

#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

bool str_2_mac(uint8_t *str, uint8_t *dest);
int hexstr_2_bin(const char *hex, uint8_t *buf, uint32_t len);
int get_value_string(char *value_in, char *buf);

#endif //_BLE_MESH_CONSOLE_LIB_H_
