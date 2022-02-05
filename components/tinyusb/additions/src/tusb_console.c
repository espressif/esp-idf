/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <stdio.h>
#include <stdio_ext.h>
#include "esp_log.h"
#include "cdc.h"
#include "tusb_console.h"
#include "tinyusb.h"
#include "vfs_tinyusb.h"
#include "esp_check.h"

#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s

static const char *TAG = "tusb_console";

typedef struct {
    FILE *in;
    FILE *out;
    FILE *err;
} console_handle_t;

static console_handle_t con;


/**
 * @brief Reopen standard streams using a new path
 *
 * @param f_in - pointer to a pointer holding a file for in or NULL to don't change stdin
 * @param f_out - pointer to a pointer holding a file for out or NULL to don't change stdout
 * @param f_err - pointer to a pointer holding a file for err or NULL to don't change stderr
 * @param path - mount point
 * @return esp_err_t ESP_FAIL or ESP_OK
 */
static esp_err_t redirect_std_streams_to(FILE **f_in, FILE **f_out, FILE **f_err, const char *path)
{
    if (f_in) {
        *f_in = freopen(path, "r", stdin);
        if (*f_in == NULL) {
            ESP_LOGE(TAG, "Failed to reopen in!");
            return ESP_FAIL;
        }
    }
    if (f_out) {
        *f_out = freopen(path, "w", stdout);
        if (*f_out == NULL) {
            ESP_LOGE(TAG, "Failed to reopen out!");
            return ESP_FAIL;
        }
    }
    if (f_err) {
        *f_err = freopen(path, "w", stderr);
        if (*f_err == NULL) {
            ESP_LOGE(TAG, "Failed to reopen err!");
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}

/**
 * @brief Restore output to default
 *
 * @param f_in - pointer to a pointer of an in file updated with `redirect_std_streams_to` or NULL to don't change stdin
 * @param f_out - pointer to a pointer of an out file updated with `redirect_std_streams_to` or NULL to don't change stdout
 * @param f_err - pointer to a pointer of an err file updated with `redirect_std_streams_to` or NULL to don't change stderr
 * @return esp_err_t ESP_FAIL or ESP_OK
 */
static esp_err_t restore_std_streams(FILE **f_in, FILE **f_out, FILE **f_err)
{
    const char *default_uart_dev = "/dev/uart/" STRINGIFY(CONFIG_ESP_CONSOLE_UART_NUM);
    if (f_in) {
        stdin = freopen(default_uart_dev, "r", *f_in);
        if (stdin == NULL) {
            ESP_LOGE(TAG, "Failed to reopen stdin!");
            return ESP_FAIL;
        }
    }
    if (f_out) {
        stdout = freopen(default_uart_dev, "w", *f_out);
        if (stdout == NULL) {
            ESP_LOGE(TAG, "Failed to reopen stdout!");
            return ESP_FAIL;
        }
    }
    if (f_err) {
        stderr = freopen(default_uart_dev, "w", *f_err);
        if (stderr == NULL) {
            ESP_LOGE(TAG, "Failed to reopen stderr!");
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

esp_err_t esp_tusb_init_console(int cdc_intf)
{
    /* Registering TUSB at VFS */
    ESP_RETURN_ON_ERROR(esp_vfs_tusb_cdc_register(cdc_intf, NULL), TAG, "");
    ESP_RETURN_ON_ERROR(redirect_std_streams_to(&con.in, &con.out, &con.err, "/dev/tusb_cdc"), TAG, "Failed to redirect STD streams");
    return ESP_OK;
}

esp_err_t esp_tusb_deinit_console(int cdc_intf)
{
    ESP_RETURN_ON_ERROR(restore_std_streams(&con.in, &con.out, &con.err), TAG, "Failed to restore STD streams");
    esp_vfs_tusb_cdc_unregister(NULL);
    return ESP_OK;
}
