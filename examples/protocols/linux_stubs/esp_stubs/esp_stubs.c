/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdlib.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_event.h"

extern void app_main(void);

esp_err_t esp_netif_init(void)
{
    return ESP_OK;
}

esp_err_t example_connect(void)
{
    return ESP_OK;
}

int main(void)
{
    app_main();

    return 0;
}
