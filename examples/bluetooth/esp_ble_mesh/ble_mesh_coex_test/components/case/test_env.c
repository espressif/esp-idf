/*
 * ESP BLE Mesh Example
 *
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "test_env.h"
#include "sync.h"

#define TAG         "ENV"

coex_test_env_t test_env = {
#if defined(CONFIG_EXAMPLE_MANAUL)
    .ap_ssid = CONFIG_EXAMPLE_WIFI_SSID,
    .ap_password = CONFIG_EXAMPLE_WIFI_PASSWORD,
#endif
#if defined(CONFIG_EXAMPLE_COEX_ROLE)
    .ap_ssid = CONFIG_EXAMPLE_WIFI_SSID,
    .ap_password = CONFIG_EXAMPLE_WIFI_PASSWORD,
#endif
    .test_port = "8080",
    .server_ip = "192.168.3.32",
    .duration = "120000",
    .is_start = false,
};

esp_err_t coex_set_test_env(const char *keyword, const char *value, uint8_t length)
{
    esp_err_t ret = ESP_OK;
    if (!strcmp(keyword, "ap_ssid")) {
        memset(test_env.ap_ssid, '\0', sizeof(test_env.ap_ssid));
        strncpy(test_env.ap_ssid, value, length);
    } else if (!strcmp(keyword, "ap_password")) {
        memset(test_env.ap_password, '\0', sizeof(test_env.ap_password));
        strncpy(test_env.ap_password, value, length);
    } else if (!strcmp( keyword, "test_port")) {
        memset(test_env.test_port, '\0', sizeof(test_env.test_port));
        strncpy(test_env.test_port, value, length);
    } else if (!strcmp(keyword, "server_ip")) {
        memset(test_env.server_ip, '\0', sizeof(test_env.server_ip));
        strncpy(test_env.server_ip, value, length);
    } else if (!strcmp(keyword, "duration")) {
        strncpy(test_env.duration, value, length);
    } else if (!strcmp(keyword, "mutex")) {
        test_env.run_mutex = client_mutex;

    } else {
        ret = ESP_ERR_NOT_SUPPORTED;
    }
    return ret;
}

const char *coex_get_test_env(const char *keyword)
{
    const char *ret = NULL;
    if (!strcmp(keyword, "ap_ssid")) {
        ret = test_env.ap_ssid;
    } else if (!strcmp(keyword, "ap_password")) {
        ret = test_env.ap_password;
    } else if (!strcmp(keyword, "test_port")) {
        ret = test_env.test_port;
    } else if (!strcmp(keyword, "server_ip")) {
        ret = test_env.server_ip;
    } else if (!strcmp(keyword, "duration")) {
        ret = test_env.duration;
    }
    return ret;
}

void coex_print_test_env(void)
{
    ESP_LOGI(TAG, "current test env:");
    ESP_LOGI(TAG, "\tap_ssid: %s", test_env.ap_ssid);
    ESP_LOGI(TAG, "\tap_password: %s", test_env.ap_password);
    ESP_LOGI(TAG, "\ttest_port: %s", test_env.test_port);
    ESP_LOGI(TAG, "\tserver_ip: %s", test_env.server_ip);
    ESP_LOGI(TAG, "\tduration: %s", test_env.duration);
}

bool coex_env_str_to_mac(uint8_t *str, uint8_t *dest)
{
    uint8_t loop = 0;
    uint8_t tmp = 0;
    uint8_t *src_p = str;

    if (strlen((char *)src_p) != 17) { // must be like 12:34:56:78:90:AB
        ESP_LOGE(TAG, "wrong format");
        return false;
    }

    for (loop = 0; loop < 17 ; loop++) {
        if (loop % 3 == 2) {
            if (src_p[loop] != ':') {
                ESP_LOGE(TAG, "wrong format");
                return false;
            }

            continue;
        }

        if ((src_p[loop] >= '0') && (src_p[loop] <= '9')) {
            tmp = tmp * 16 + src_p[loop] - '0';
        } else if ((src_p[loop] >= 'A') && (src_p[loop] <= 'F')) {
            tmp = tmp * 16 + src_p[loop] - 'A' + 10;
        } else if ((src_p[loop] >= 'a') && (src_p[loop] <= 'f')) {
            tmp = tmp * 16 + src_p[loop] - 'a' + 10;
        } else {
            ESP_LOGE(TAG, "wrong format");
            return false;
        }

        if (loop % 3 == 1) {
            *dest++ = tmp;
            tmp = 0;
        }
    }

    return true;
}
