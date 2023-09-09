/*
 * ESP BLE Mesh Example
 *
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __TEST_ENV_H__
#define __TEST_ENV_H__

#include <stdint.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define MAX_SSID_LEN                        32
#define MAX_PASSWORD_LEN                    64
#define MAX_IP_STR_LEN                      15
#define MAX_PORT_STR_LEN                    5
#define MAX_MAC_ADDR_LEN                    17
#define INVALID_REMOTE_BT_MAC               "ff:ff:ff:ff:ff:ff"
#define DURATION_MAX_LEN                    10

typedef struct {
    char ap_ssid[MAX_SSID_LEN + 1];
    char ap_password[MAX_PASSWORD_LEN + 1];
    char test_port[MAX_PORT_STR_LEN + 1];
    char server_ip[MAX_IP_STR_LEN + 1];
    char duration[DURATION_MAX_LEN + 1];
    bool is_start;
    SemaphoreHandle_t run_mutex;
} coex_test_env_t;

extern coex_test_env_t test_env;

esp_err_t coex_set_test_env(const char *keyword, const char *value, uint8_t length);
const char *coex_get_test_env(const char *keyword);
void coex_print_test_env(void);
bool coex_env_str_to_mac(uint8_t *str, uint8_t *dest);

#endif /* __TEST_ENV_H__ */
