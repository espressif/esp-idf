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

#ifndef _BLE_MESH_CONSOLE_LIB_H_
#define _BLE_MESH_CONSOLE_LIB_H_

#include <stdio.h>
#include <string.h>

#include "esp_system.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

bool str_2_mac(uint8_t *str, uint8_t *dest);
int hexstr_2_bin(const char *hex, uint8_t *buf, uint32_t len);
int get_value_string(char *value_in, char *buf);

#endif //_BLE_MESH_CONSOLE_LIB_H_