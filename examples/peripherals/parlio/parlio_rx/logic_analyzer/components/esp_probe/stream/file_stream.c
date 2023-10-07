/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "esp_check.h"

const static char *TAG = "file_stream";

FILE* esp_probe_open_file_stream(const char *file_path)
{
    esp_err_t ret = ESP_FAIL;
    // Create new raw data file
    FILE *f = fopen(file_path, "wb");
    ESP_GOTO_ON_FALSE(f, ESP_FAIL, err, TAG, "Failed to open file, error %s", strerror(errno));
    ESP_LOGI(TAG, "Stream opened");

    return f;
err:
    ESP_LOGE(TAG, "Open Flash stream failed with error code %s", esp_err_to_name(ret));
    return NULL;
}

void esp_probe_close_file_stream(FILE *f)
{
    fclose(f);
}
