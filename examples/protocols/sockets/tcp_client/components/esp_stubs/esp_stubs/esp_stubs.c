/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdlib.h>
#include "esp_err.h"
#include "esp_log.h"

extern void app_main(void);

void _esp_error_check_failed(esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    ESP_LOGE("ESP_ERROR_CHECK", "Failed with esp_err_t: 0x%x", rc);
    ESP_LOGE("ESP_ERROR_CHECK", "Expression: %s", expression);
    ESP_LOGE("ESP_ERROR_CHECK", "Functions: %s %s(%d)", function, file, line);
    abort();
}

esp_err_t esp_event_loop_create_default(void)
{
    return ESP_OK;
}

esp_err_t esp_netif_init(void)
{
    return ESP_OK;
}

esp_err_t example_connect(void)
{
    return ESP_OK;
}

esp_err_t nvs_flash_init(void)
{
    return ESP_OK;
}

int main()
{
    app_main();

    return 0;
}
