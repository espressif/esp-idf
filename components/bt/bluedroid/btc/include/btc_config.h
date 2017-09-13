// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __BTC_CONFIG_H__
#define __BTC_CONFIG_H__

#include <stdbool.h>
#include <stddef.h>

#include "bt_types.h"

typedef struct btc_config_section_iter_t btc_config_section_iter_t;

bool btc_config_init(void);
bool btc_config_shut_down(void);
bool btc_config_clean_up(void);

bool btc_config_has_section(const char *section);
bool btc_config_exist(const char *section, const char *key);
bool btc_config_get_int(const char *section, const char *key, int *value);
bool btc_config_set_int(const char *section, const char *key, int value);
bool btc_config_get_str(const char *section, const char *key, char *value, int *size_bytes);
bool btc_config_set_str(const char *section, const char *key, const char *value);
bool btc_config_get_bin(const char *section, const char *key, uint8_t *value, size_t *length);
bool btc_config_set_bin(const char *section, const char *key, const uint8_t *value, size_t length);
bool btc_config_remove(const char *section, const char *key);
bool btc_config_remove_section(const char *section);

size_t btc_config_get_bin_length(const char *section, const char *key);

const btc_config_section_iter_t *btc_config_section_begin(void);
const btc_config_section_iter_t *btc_config_section_end(void);
const btc_config_section_iter_t *btc_config_section_next(const btc_config_section_iter_t *section);
const char *btc_config_section_name(const btc_config_section_iter_t *section);

void btc_config_flush(void);
int btc_config_clear(void);

// TODO(zachoverflow): Eww...we need to move these out. These are peer specific, not config general.
bool btc_get_address_type(const BD_ADDR bd_addr, int *p_addr_type);
bool btc_compare_address_key_value(const char *section, char *key_type, void *key_value, int key_length);
bool btc_get_device_type(const BD_ADDR bd_addr, int *p_device_type);

void btc_config_lock(void);
void btc_config_unlock(void);

#endif
