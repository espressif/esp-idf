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

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "bt_defs.h"
#include "bt_trace.h"
#include "alarm.h"
#include "allocator.h"
#include "bdaddr.h"
#include "btc_config.h"
#include "btc_util.h"
#include "config.h"
#include "osi.h"
#include "mutex.h"

#include "bt_types.h"

static const char *CONFIG_FILE_PATH = "bt_config.conf";
static const period_ms_t CONFIG_SETTLE_PERIOD_MS = 3000;

static void btc_key_value_to_string(uint8_t *key_vaule, char *value_str, int key_length);
static osi_mutex_t lock;  // protects operations on |config|.
static config_t *config;

bool btc_compare_address_key_value(const char *section, char *key_type, void *key_value, int key_length)
{
    assert(key_value != NULL);
    bool status = false;
    char value_str[100] = {0};
    if(key_length > sizeof(value_str)/2) {
        return false;
    }
    btc_key_value_to_string((uint8_t *)key_value, value_str, key_length);
    if ((status = config_has_key_in_section(config, key_type, value_str)) == true) {
        config_remove_section(config, section);
    }
    return status;
}

static void btc_key_value_to_string(uint8_t *key_vaule, char *value_str, int key_length)
{
    const char *lookup = "0123456789abcdef";

    assert(key_vaule != NULL);
    assert(value_str != NULL);

    for (size_t i = 0; i < key_length; ++i) {
        value_str[(i * 2) + 0] = lookup[(key_vaule[i] >> 4) & 0x0F];
        value_str[(i * 2) + 1] = lookup[key_vaule[i] & 0x0F];
    }

    return;
}

// Module lifecycle functions

bool btc_config_init(void)
{
    osi_mutex_new(&lock);
    config = config_new(CONFIG_FILE_PATH);
    if (!config) {
        LOG_WARN("%s unable to load config file; starting unconfigured.\n", __func__);
        config = config_new_empty();
        if (!config) {
            LOG_ERROR("%s unable to allocate a config object.\n", __func__);
            goto error;
        }
    }
    if (config_save(config, CONFIG_FILE_PATH)) {
        // unlink(LEGACY_CONFIG_FILE_PATH);
    }

    return true;

error:;
    config_free(config);
    osi_mutex_free(&lock);
    config = NULL;
    LOG_ERROR("%s failed\n", __func__);
    return false;
}

bool btc_config_shut_down(void)
{
    btc_config_flush();
    return true;
}

bool btc_config_clean_up(void)
{
    btc_config_flush();

    config_free(config);
    osi_mutex_free(&lock);
    config = NULL;
    return true;
}

bool btc_config_has_section(const char *section)
{
    assert(config != NULL);
    assert(section != NULL);

    return config_has_section(config, section);
}

bool btc_config_exist(const char *section, const char *key)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    return config_has_key(config, section, key);
}

bool btc_config_get_int(const char *section, const char *key, int *value)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);
    assert(value != NULL);

    bool ret = config_has_key(config, section, key);
    if (ret) {
        *value = config_get_int(config, section, key, *value);
    }

    return ret;
}

bool btc_config_set_int(const char *section, const char *key, int value)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    config_set_int(config, section, key, value);

    return true;
}

bool btc_config_get_str(const char *section, const char *key, char *value, int *size_bytes)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);
    assert(value != NULL);
    assert(size_bytes != NULL);

    const char *stored_value = config_get_string(config, section, key, NULL);

    if (!stored_value) {
        return false;
    }

    strlcpy(value, stored_value, *size_bytes);
    *size_bytes = strlen(value) + 1;

    return true;
}

bool btc_config_set_str(const char *section, const char *key, const char *value)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);
    assert(value != NULL);

    config_set_string(config, section, key, value, false);

    return true;
}

bool btc_config_get_bin(const char *section, const char *key, uint8_t *value, size_t *length)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);
    assert(value != NULL);
    assert(length != NULL);

    const char *value_str = config_get_string(config, section, key, NULL);

    if (!value_str) {
        return false;
    }

    size_t value_len = strlen(value_str);
    if ((value_len % 2) != 0 || *length < (value_len / 2)) {
        return false;
    }

    for (size_t i = 0; i < value_len; ++i)
        if (!isxdigit((unsigned char)value_str[i])) {
            return false;
        }

    for (*length = 0; *value_str; value_str += 2, *length += 1) {
        unsigned int val;
        sscanf(value_str, "%02x", &val);
        value[*length] = (uint8_t)(val);
    }

    return true;
}

size_t btc_config_get_bin_length(const char *section, const char *key)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    const char *value_str = config_get_string(config, section, key, NULL);

    if (!value_str) {
        return 0;
    }

    size_t value_len = strlen(value_str);
    return ((value_len % 2) != 0) ? 0 : (value_len / 2);
}

bool btc_config_set_bin(const char *section, const char *key, const uint8_t *value, size_t length)
{
    const char *lookup = "0123456789abcdef";

    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    if (length > 0) {
        assert(value != NULL);
    }

    char *str = (char *)osi_calloc(length * 2 + 1);
    if (!str) {
        return false;
    }

    for (size_t i = 0; i < length; ++i) {
        str[(i * 2) + 0] = lookup[(value[i] >> 4) & 0x0F];
        str[(i * 2) + 1] = lookup[value[i] & 0x0F];
    }

    config_set_string(config, section, key, str, false);

    osi_free(str);
    return true;
}

const btc_config_section_iter_t *btc_config_section_begin(void)
{
    assert(config != NULL);
    return (const btc_config_section_iter_t *)config_section_begin(config);
}

const btc_config_section_iter_t *btc_config_section_end(void)
{
    assert(config != NULL);
    return (const btc_config_section_iter_t *)config_section_end(config);
}

const btc_config_section_iter_t *btc_config_section_next(const btc_config_section_iter_t *section)
{
    assert(config != NULL);
    assert(section != NULL);
    return (const btc_config_section_iter_t *)config_section_next((const config_section_node_t *)section);
}

const char *btc_config_section_name(const btc_config_section_iter_t *section)
{
    assert(config != NULL);
    assert(section != NULL);
    return config_section_name((const config_section_node_t *)section);
}



bool btc_config_remove(const char *section, const char *key)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    return config_remove_key(config, section, key);
}

bool btc_config_remove_section(const char *section)
{
    assert(config != NULL);
    assert(section != NULL);

    return config_remove_section(config, section);
}

void btc_config_flush(void)
{
    assert(config != NULL);

    config_save(config, CONFIG_FILE_PATH);
}

int btc_config_clear(void)
{
    assert(config != NULL);

    config_free(config);

    config = config_new_empty();
    if (config == NULL) {
        return false;
    }
    int ret = config_save(config, CONFIG_FILE_PATH);
    return ret;
}

void btc_config_lock(void)
{
    osi_mutex_lock(&lock, OSI_MUTEX_MAX_TIMEOUT);
}

void btc_config_unlock(void)
{
    osi_mutex_unlock(&lock);
}

