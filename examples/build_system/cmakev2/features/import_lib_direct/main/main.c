/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * This example demonstrates importing an external C library (lwjson)
 * via FetchContent, built by its own CMake, and linked directly to this IDF component
 * without any IDF component wrapper.
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "lwjson/lwjson.h"

static const char *TAG = "import_lib_direct";

#define LWJSON_TOKENS 64

/* Sample JSON string to parse */
static const char *sample_json =
    "{"
    "  \"name\": \"ESP32\","
    "  \"cores\": 2,"
    "  \"features\": [\"WiFi\", \"Bluetooth\", \"GPIO\"],"
    "  \"specs\": {"
    "    \"flash\": \"4MB\","
    "    \"ram\": \"520KB\""
    "  }"
    "}";

void app_main(void)
{
    ESP_LOGI(TAG, "lwjson library imported directly (downloaded, built, linked) without IDF component wrapper");

    static lwjson_token_t tokens[LWJSON_TOKENS];
    lwjson_t lwobj;

    if (lwjson_init(&lwobj, tokens, LWJSON_TOKENS) != lwjsonOK) {
        ESP_LOGE(TAG, "lwjson_init failed");
        return;
    }

    if (lwjson_parse(&lwobj, sample_json) != lwjsonOK) {
        ESP_LOGE(TAG, "lwjson_parse failed");
        lwjson_free(&lwobj);
        return;
    }

    ESP_LOGI(TAG, "Parsing JSON string...");

    const lwjson_token_t *name_t = lwjson_find(&lwobj, "name");
    if (name_t != NULL) {
        size_t len;
        const char *s = lwjson_get_val_string(name_t, &len);
        if (s != NULL) {
            ESP_LOGI(TAG, "Device name: %.*s", (int)len, s);
        }
    }

    const lwjson_token_t *cores_t = lwjson_find(&lwobj, "cores");
    if (cores_t != NULL && cores_t->type == LWJSON_TYPE_NUM_INT) {
        ESP_LOGI(TAG, "Number of cores: %lld", (long long)lwjson_get_val_int(cores_t));
    }

    const lwjson_token_t *features_t = lwjson_find(&lwobj, "features");
    if (features_t != NULL && features_t->type == LWJSON_TYPE_ARRAY) {
        ESP_LOGI(TAG, "Features:");
        const lwjson_token_t *child = lwjson_get_first_child(features_t);
        while (child != NULL) {
            if (child->type == LWJSON_TYPE_STRING) {
                size_t len;
                const char *s = lwjson_get_val_string(child, &len);
                if (s != NULL) {
                    ESP_LOGI(TAG, "  - %.*s", (int)len, s);
                }
            }
            child = child->next;
        }
    }

    const lwjson_token_t *flash_t = lwjson_find(&lwobj, "specs.flash");
    const lwjson_token_t *ram_t = lwjson_find(&lwobj, "specs.ram");
    if (flash_t != NULL || ram_t != NULL) {
        ESP_LOGI(TAG, "Specifications:");
        if (flash_t != NULL) {
            size_t len;
            const char *s = lwjson_get_val_string(flash_t, &len);
            if (s != NULL) {
                ESP_LOGI(TAG, "  Flash: %.*s", (int)len, s);
            }
        }
        if (ram_t != NULL) {
            size_t len;
            const char *s = lwjson_get_val_string(ram_t, &len);
            if (s != NULL) {
                ESP_LOGI(TAG, "  RAM: %.*s", (int)len, s);
            }
        }
    }

    lwjson_free(&lwobj);

    ESP_LOGI(TAG, "Example complete!");
}
