/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "bt_types.h"

static const char BTIF_CONFIG_MODULE[] = "btif_config_module";

typedef struct btif_config_section_iter_t btif_config_section_iter_t;

bool btif_config_init(void);
bool btif_config_shut_down(void);
bool btif_config_clean_up(void);

bool btif_config_has_section(const char *section);
bool btif_config_exist(const char *section, const char *key);
bool btif_config_get_int(const char *section, const char *key, int *value);
bool btif_config_set_int(const char *section, const char *key, int value);
bool btif_config_get_str(const char *section, const char *key, char *value, int *size_bytes);
bool btif_config_set_str(const char *section, const char *key, const char *value);
bool btif_config_get_bin(const char *section, const char *key, uint8_t *value, size_t *length);
bool btif_config_set_bin(const char *section, const char *key, const uint8_t *value, size_t length);
bool btif_config_remove(const char *section, const char *key);

size_t btif_config_get_bin_length(const char *section, const char *key);

const btif_config_section_iter_t *btif_config_section_begin(void);
const btif_config_section_iter_t *btif_config_section_end(void);
const btif_config_section_iter_t *btif_config_section_next(const btif_config_section_iter_t *section);
const char *btif_config_section_name(const btif_config_section_iter_t *section);

void btif_config_save(void);
void btif_config_flush(void);
int btif_config_clear(void);

// TODO(zachoverflow): Eww...we need to move these out. These are peer specific, not config general.
bool btif_get_address_type(const BD_ADDR bd_addr, int *p_addr_type);
bool btif_get_device_type(const BD_ADDR bd_addr, int *p_device_type);

