/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "esp_log.h"
#include "esp_check.h"
#include "http_header.h"
#include "http_utils.h"

static const char *TAG = "HTTP_HEADER";
#define HEADER_BUFFER (1024)

/**
 * dictionary item struct, with key-value pair
 */
typedef struct http_header_item {
    char *key;                          /*!< key */
    char *value;                        /*!< value */
    STAILQ_ENTRY(http_header_item) next;   /*!< Point to next entry */
} http_header_item_t;

STAILQ_HEAD(http_header, http_header_item);


http_header_handle_t http_header_init(void)
{
    http_header_handle_t header = calloc(1, sizeof(struct http_header));
    ESP_RETURN_ON_FALSE(header, NULL, TAG, "Memory exhausted");
    STAILQ_INIT(header);
    return header;
}

esp_err_t http_header_destroy(http_header_handle_t header)
{
    esp_err_t err = http_header_clean(header);
    free(header);
    return err;
}

http_header_item_handle_t http_header_get_item(http_header_handle_t header, const char *key)
{
    http_header_item_handle_t item;
    if (header == NULL || key == NULL) {
        return NULL;
    }
    STAILQ_FOREACH(item, header, next) {
        if (strcasecmp(item->key, key) == 0) {
            return item;
        }
    }
    return NULL;
}

esp_err_t http_header_get(http_header_handle_t header, const char *key, char **value)
{
    http_header_item_handle_t item;

    item = http_header_get_item(header, key);
    if (item) {
        *value = item->value;
    } else {
        *value = NULL;
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

static esp_err_t http_header_new_item(http_header_handle_t header, const char *key, const char *value)
{
    esp_err_t ret = ESP_OK;
    http_header_item_handle_t item;

    item = calloc(1, sizeof(http_header_item_t));
    ESP_RETURN_ON_FALSE(item, ESP_ERR_NO_MEM, TAG, "Memory exhausted");
    HTTP_GOTO_ON_FALSE_DBG(http_utils_assign_string(&item->key, key, -1), ESP_ERR_NO_MEM, _header_new_item_exit, TAG, "Failed to assign string");
    http_utils_trim_whitespace(&item->key);
    HTTP_GOTO_ON_FALSE_DBG(http_utils_assign_string(&item->value, value, -1), ESP_ERR_NO_MEM, _header_new_item_exit, TAG, "Failed to assign string");
    http_utils_trim_whitespace(&item->value);
    STAILQ_INSERT_TAIL(header, item, next);
    return ret;
_header_new_item_exit:
    if (item->key) {
        free(item->key);
    }
    if (item->value) {
        free(item->value);
    }
    free(item);
    return ret;
}

esp_err_t http_header_set(http_header_handle_t header, const char *key, const char *value)
{
    http_header_item_handle_t item;

    if (value == NULL) {
        return http_header_delete(header, key);
    }

    item = http_header_get_item(header, key);

    if (item) {
        free(item->value);
        item->value = strdup(value);
        http_utils_trim_whitespace(&item->value);
        return ESP_OK;
    }
    return http_header_new_item(header, key, value);
}

esp_err_t http_header_set_from_string(http_header_handle_t header, const char *key_value_data)
{
    char *eq_ch;
    char *p_str;

    p_str = strdup(key_value_data);
    ESP_RETURN_ON_FALSE(p_str, ESP_ERR_NO_MEM, TAG, "Memory exhausted");
    eq_ch = strchr(p_str, ':');
    if (eq_ch == NULL) {
        free(p_str);
        return ESP_ERR_INVALID_ARG;
    }
    *eq_ch = 0;

    http_header_set(header, p_str, eq_ch + 1);
    free(p_str);
    return ESP_OK;
}


esp_err_t http_header_delete(http_header_handle_t header, const char *key)
{
    http_header_item_handle_t item = http_header_get_item(header, key);
    if (item) {
        STAILQ_REMOVE(header, item, http_header_item, next);
        free(item->key);
        free(item->value);
        free(item);
    } else {
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}


int http_header_set_format(http_header_handle_t header, const char *key, const char *format, ...)
{
    va_list argptr = {0};
    int len = 0;
    char *buf = NULL;
    va_start(argptr, format);
    len = vasprintf(&buf, format, argptr);
    va_end(argptr);
    ESP_RETURN_ON_FALSE(buf, 0, TAG, "Memory exhausted");
    http_header_set(header, key, buf);
    free(buf);
    return len;
}

int http_header_generate_string(http_header_handle_t header, int index, char *buffer, int *buffer_len)
{
    http_header_item_handle_t item;
    int size = 0;
    int idx = 0;
    int ret_idx = -1;
    bool is_end = false;

    // iterate over the header entries to calculate buffer size and determine last item
    STAILQ_FOREACH(item, header, next) {
        if (item->value && idx >= index) {
            size += strlen(item->key);
            size += strlen(item->value);
            size += 4; //': ' and '\r\n'
        }
        idx ++;

        if (size + 1 > *buffer_len - 2) {
            // if this item would not fit to the buffer, return the index of the last fitting one
            ret_idx = idx - 1;
            ESP_LOGE(TAG, "Buffer length is small to fit all the headers");
            break;
        }
    }

    if (size == 0) {
        return 0;
    }
    if (ret_idx < 0) {
        // all items would fit, mark this as the end of http header string
        ret_idx = idx;
        is_end = true;
    }

    // iterate again over the header entries to write only the fitting indices
    int str_len = 0;
    idx = 0;
    STAILQ_FOREACH(item, header, next) {
        if (item->value && idx >= index && idx < ret_idx) {
            str_len += snprintf(buffer + str_len, *buffer_len - str_len, "%s: %s\r\n", item->key, item->value);
        }
        idx ++;
    }
    if (is_end) {
        // write the http header terminator if all header entries have been written in this function call
        str_len += snprintf(buffer + str_len, *buffer_len - str_len, "\r\n");
    }
    *buffer_len = str_len;
    return ret_idx;
}

esp_err_t http_header_clean(http_header_handle_t header)
{
    http_header_item_handle_t item = STAILQ_FIRST(header), tmp;
    while (item != NULL) {
        tmp = STAILQ_NEXT(item, next);
        free(item->key);
        free(item->value);
        free(item);
        item = tmp;
    }
    STAILQ_INIT(header);
    return ESP_OK;
}

int http_header_count(http_header_handle_t header)
{
    http_header_item_handle_t item;
    int count = 0;
    STAILQ_FOREACH(item, header, next) {
        count ++;
    }
    return count;
}
