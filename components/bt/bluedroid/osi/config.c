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

#define LOG_TAG "bt_osi_config"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

// #include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <sys/stat.h>

#include "allocator.h"
#include "config.h"
#include "list.h"
#include "bt_trace.h"

#define CONFIG_FILE_MAX_SIZE      (4096)
#define CONFIG_KEY                "cfg_key8"
typedef struct {
    char *key;
    char *value;
} entry_t;

typedef struct {
    char *name;
    list_t *entries;
} section_t;

struct config_t {
    list_t *sections;
};

// Empty definition; this type is aliased to list_node_t.
struct config_section_iter_t {};

static void config_parse(nvs_handle fp, config_t *config);

static section_t *section_new(const char *name);
static void section_free(void *ptr);
static section_t *section_find(const config_t *config, const char *section);

static entry_t *entry_new(const char *key, const char *value);
static void entry_free(void *ptr);
static entry_t *entry_find(const config_t *config, const char *section, const char *key);

static void my_nvs_close(nvs_handle fp)
{
    // LOG_ERROR("nvs close %d\n", (int)fp);
    nvs_close(fp);
}


config_t *config_new_empty(void)
{
    config_t *config = osi_calloc(sizeof(config_t));
    if (!config) {
        LOG_ERROR("%s unable to allocate memory for config_t.", __func__);
        goto error;
    }

    config->sections = list_new(section_free);
    if (!config->sections) {
        LOG_ERROR("%s unable to allocate list for sections.", __func__);
        goto error;
    }

    return config;

error:;
    config_free(config);
    return NULL;
}

config_t *config_new(const char *filename)
{
    assert(filename != NULL);

    // LOG_ERROR("config new bgn\n");
    config_t *config = config_new_empty();
    if (!config) {
        return NULL;
    }

    esp_err_t err;
    nvs_handle fp;
    err = nvs_open(filename, NVS_READWRITE, &fp);
    if (err != ESP_OK) {
        LOG_ERROR("%s unable to open file '%s'", __func__, filename);
        config_free(config);
        return NULL;
    }

    // LOG_ERROR("config parse bgn\n");
    config_parse(fp, config);
    // LOG_ERROR("config parse end\n");
    my_nvs_close(fp);
    return config;
}

void config_free(config_t *config)
{
    if (!config) {
        return;
    }

    list_free(config->sections);
    osi_free(config);
}

bool config_has_section(const config_t *config, const char *section)
{
    assert(config != NULL);
    assert(section != NULL);

    return (section_find(config, section) != NULL);
}

bool config_has_key(const config_t *config, const char *section, const char *key)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    return (entry_find(config, section, key) != NULL);
}

int config_get_int(const config_t *config, const char *section, const char *key, int def_value)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    entry_t *entry = entry_find(config, section, key);
    if (!entry) {
        return def_value;
    }

    char *endptr;
    int ret = strtol(entry->value, &endptr, 0);
    return (*endptr == '\0') ? ret : def_value;
}

bool config_get_bool(const config_t *config, const char *section, const char *key, bool def_value)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    entry_t *entry = entry_find(config, section, key);
    if (!entry) {
        return def_value;
    }

    if (!strcmp(entry->value, "true")) {
        return true;
    }
    if (!strcmp(entry->value, "false")) {
        return false;
    }

    return def_value;
}

const char *config_get_string(const config_t *config, const char *section, const char *key, const char *def_value)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    entry_t *entry = entry_find(config, section, key);
    if (!entry) {
        return def_value;
    }

    return entry->value;
}

void config_set_int(config_t *config, const char *section, const char *key, int value)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    char value_str[32] = { 0 };
    sprintf(value_str, "%d", value);
    config_set_string(config, section, key, value_str);
}

void config_set_bool(config_t *config, const char *section, const char *key, bool value)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    config_set_string(config, section, key, value ? "true" : "false");
}

void config_set_string(config_t *config, const char *section, const char *key, const char *value)
{
    section_t *sec = section_find(config, section);
    if (!sec) {
        sec = section_new(section);
        list_append(config->sections, sec);
    }

    for (const list_node_t *node = list_begin(sec->entries); node != list_end(sec->entries); node = list_next(node)) {
        entry_t *entry = list_node(node);
        if (!strcmp(entry->key, key)) {
            osi_free(entry->value);
            entry->value = osi_strdup(value);
            return;
        }
    }

    entry_t *entry = entry_new(key, value);
    list_append(sec->entries, entry);
}

bool config_remove_section(config_t *config, const char *section)
{
    assert(config != NULL);
    assert(section != NULL);

    section_t *sec = section_find(config, section);
    if (!sec) {
        return false;
    }

    return list_remove(config->sections, sec);
}

bool config_remove_key(config_t *config, const char *section, const char *key)
{
    assert(config != NULL);
    assert(section != NULL);
    assert(key != NULL);

    section_t *sec = section_find(config, section);
    entry_t *entry = entry_find(config, section, key);
    if (!sec || !entry) {
        return false;
    }

    return list_remove(sec->entries, entry);
}

const config_section_node_t *config_section_begin(const config_t *config)
{
    assert(config != NULL);
    return (const config_section_node_t *)list_begin(config->sections);
}

const config_section_node_t *config_section_end(const config_t *config)
{
    assert(config != NULL);
    return (const config_section_node_t *)list_end(config->sections);
}

const config_section_node_t *config_section_next(const config_section_node_t *node)
{
    assert(node != NULL);
    return (const config_section_node_t *)list_next((const list_node_t *)node);
}

const char *config_section_name(const config_section_node_t *node)
{
    assert(node != NULL);
    const list_node_t *lnode = (const list_node_t *)node;
    const section_t *section = (const section_t *)list_node(lnode);
    return section->name;
}

bool config_save(const config_t *config, const char *filename)
{
    assert(config != NULL);
    assert(filename != NULL);
    assert(*filename != '\0');

    esp_err_t err;
    nvs_handle fp;
    char *line = osi_calloc(1024);
    char *buf = osi_calloc(CONFIG_FILE_MAX_SIZE);
    if (!line || !buf) {
        goto error;
    }

    err = nvs_open(filename, NVS_READWRITE, &fp);
    // LOG_ERROR("nvs open: %d\n", (int)fp);
    if (err != ESP_OK) {
        // LOG_ERROR("%s unable to write file '%s'\n", __func__, filename);
        goto error;
    }

    // LOG_ERROR("m1, %s\n", filename);
    int w_cnt, w_cnt_total = 0;
    for (const list_node_t *node = list_begin(config->sections); node != list_end(config->sections); node = list_next(node)) {
        const section_t *section = (const section_t *)list_node(node);
        // LOG_ERROR("m11\n");
        // LOG_ERROR("m12, %s\n", section->name);
        w_cnt = snprintf(line, 1024, "[%s]\n", section->name);
        // LOG_ERROR("m2 : %s\n", section->name);
        if (w_cnt + w_cnt_total < CONFIG_FILE_MAX_SIZE) {
            memcpy(buf + w_cnt_total, line, w_cnt);
            w_cnt_total += w_cnt;
            // LOG_ERROR("m21\n");
        } else {
            break;
        }
        // LOG_ERROR("m22\n");
        for (const list_node_t *enode = list_begin(section->entries); enode != list_end(section->entries); enode = list_next(enode)) {
            const entry_t *entry = (const entry_t *)list_node(enode);
            // LOG_ERROR("m30: %s, %s\n", entry->key, entry->value);
            w_cnt = snprintf(line, 1024, "%s = %s\n", entry->key, entry->value);
            // LOG_ERROR("m3 : %s, %s\n", entry->key, entry->value);
            if (w_cnt + w_cnt_total < CONFIG_FILE_MAX_SIZE) {
                memcpy(buf + w_cnt_total, line, w_cnt);
                w_cnt_total += w_cnt;

            } else {
                break;
            }
        }

        // Only add a separating newline if there are more sections.
        if (list_next(node) != list_end(config->sections)) {
            if (1 + w_cnt_total < CONFIG_FILE_MAX_SIZE) {
                buf[w_cnt_total] = '\n';
                w_cnt_total += 1;
            }
        } else {
            break;
        }
    }

    LOG_ERROR("m4 : %s, %d\n", buf, w_cnt_total);
    {
        // LOG_ERROR("m4x\n");
        size_t tmp_len = 4096;
        char *buf1 = osi_calloc(tmp_len);
        err = nvs_get_str(fp, CONFIG_KEY, buf1, &tmp_len);
        if (err == ESP_OK) {
            LOG_ERROR("rd %d\n%s", tmp_len, buf1);
        }
        // err = nvs_erase_key(fp, CONFIG_KEY);
        // LOG_ERROR("m4y\n");
        osi_free(buf1);
    }
    buf[w_cnt_total] = '\0';

    // LOG_ERROR("set str bgn %d, %s, %d %d\n", (int)fp, CONFIG_KEY, w_cnt_total, strlen(buf));
    err = nvs_set_blob(fp, CONFIG_KEY, buf, w_cnt_total);
    // err = nvs_set_str(fp, CONFIG_KEY, "abc");

    // LOG_ERROR("set str end\n");
    if (err != ESP_OK) {
        // LOG_ERROR("m40\n");
        my_nvs_close(fp);
        LOG_ERROR("m41\n");
        goto error;
    }
    // LOG_ERROR("m5\n");
    err = nvs_commit(fp);
    if (err != ESP_OK) {
        LOG_ERROR("m50\n");
        my_nvs_close(fp);
        LOG_ERROR("m51\n");
        goto error;
    }

    // LOG_ERROR("m6\n");
    my_nvs_close(fp);
    osi_free(line);
    osi_free(buf);
    return true;

error:
    LOG_ERROR("m7\n");
    if (buf) {
        osi_free(buf);
    }
    if (line) {
        osi_free(line);
    }
    return false;
}

static char *trim(char *str)
{
    while (isspace((unsigned char)(*str))) {
        ++str;
    }

    if (!*str) {
        return str;
    }

    char *end_str = str + strlen(str) - 1;
    while (end_str > str && isspace((unsigned char)(*end_str))) {
        --end_str;
    }

    end_str[1] = '\0';
    return str;
}

static void config_parse(nvs_handle fp, config_t *config)
{
    assert(fp != 0);
    assert(config != NULL);

    LOG_ERROR("cfg parse\n");
    int line_num = 0;
    char *line = osi_calloc(1024);
    char *section = osi_calloc(1024);
    char *buf = osi_calloc(CONFIG_FILE_MAX_SIZE);
    if (!line || !section || !buf) {
        goto error;
    }

    // LOG_ERROR("p1\n");
    esp_err_t err;
    size_t length = CONFIG_FILE_MAX_SIZE;
    err = nvs_get_blob(fp, CONFIG_KEY, buf, &length);
    if (err != ESP_OK) {
        // LOG_ERROR("p2\n");
        goto error;
    }

    LOG_ERROR("p3 %d\n%s\n", length, buf);
    char *p_line_end;
    char *p_line_bgn = buf;
    strcpy(section, CONFIG_DEFAULT_SECTION);
    // LOG_ERROR("p4\n");
    while ( (p_line_bgn < buf + length - 1) && (p_line_end = strchr(p_line_bgn, '\n'))) {

        // get one line
        int line_len = p_line_end - p_line_bgn;
        // LOG_ERROR("pii, %d, %x, %x, %x\n", line_len, p_line_bgn, p_line_end, buf);
        if (line_len > 1023) {
            break;
        }
        memcpy(line, p_line_bgn, line_len);
        line[line_len] = '\0';
        p_line_bgn = p_line_end + 1;
        // LOG_ERROR("pi0\n");
        char *line_ptr = trim(line);
        ++line_num;

        // Skip blank and comment lines.
        if (*line_ptr == '\0' || *line_ptr == '#') {
            continue;
        }

        // LOG_ERROR("pi1\n");
        if (*line_ptr == '[') {
            size_t len = strlen(line_ptr);
            if (line_ptr[len - 1] != ']') {
                LOG_WARN("%s unterminated section name on line %d.", __func__, line_num);
                continue;
            }
            strncpy(section, line_ptr + 1, len - 2);
            section[len - 2] = '\0';
            // LOG_ERROR("pi2\n");
        } else {
            // LOG_ERROR("pi3\n");
            char *split = strchr(line_ptr, '=');
            if (!split) {
                LOG_DEBUG("%s no key/value separator found on line %d.", __func__, line_num);
                continue;
            }
            // LOG_ERROR("pi4\n");
            *split = '\0';
            config_set_string(config, section, trim(line_ptr), trim(split + 1));
        }
    }

error:
    // LOG_ERROR("p5\n");
    if (buf) {
        osi_free(buf);
    }
    if (line) {
        osi_free(line);
    }
    if (section) {
        osi_free(section);
    }
    // LOG_ERROR("p6\n");
}

static section_t *section_new(const char *name)
{
    section_t *section = osi_calloc(sizeof(section_t));
    if (!section) {
        return NULL;
    }

    section->name = osi_strdup(name);
    section->entries = list_new(entry_free);
    return section;
}

static void section_free(void *ptr)
{
    if (!ptr) {
        return;
    }

    section_t *section = ptr;
    osi_free(section->name);
    list_free(section->entries);
    osi_free(section);
}

static section_t *section_find(const config_t *config, const char *section)
{
    for (const list_node_t *node = list_begin(config->sections); node != list_end(config->sections); node = list_next(node)) {
        section_t *sec = list_node(node);
        if (!strcmp(sec->name, section)) {
            return sec;
        }
    }

    return NULL;
}

static entry_t *entry_new(const char *key, const char *value)
{
    entry_t *entry = osi_calloc(sizeof(entry_t));
    if (!entry) {
        return NULL;
    }

    entry->key = osi_strdup(key);
    entry->value = osi_strdup(value);
    return entry;
}

static void entry_free(void *ptr)
{
    if (!ptr) {
        return;
    }

    entry_t *entry = ptr;
    osi_free(entry->key);
    osi_free(entry->value);
    osi_free(entry);
}

static entry_t *entry_find(const config_t *config, const char *section, const char *key)
{
    section_t *sec = section_find(config, section);
    if (!sec) {
        return NULL;
    }

    for (const list_node_t *node = list_begin(sec->entries); node != list_end(sec->entries); node = list_next(node)) {
        entry_t *entry = list_node(node);
        if (!strcmp(entry->key, key)) {
            return entry;
        }
    }

    return NULL;
}
