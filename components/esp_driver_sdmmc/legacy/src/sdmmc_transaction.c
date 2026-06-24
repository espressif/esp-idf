/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <unistd.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/sdmmc_types.h"
#include "driver/sdmmc_defs.h"
#include "driver/sdmmc_host.h"
#include "driver/sd_host.h"
#include "sdmmc_internal.h"

static const char* TAG = "sdmmc_req";

esp_err_t sdmmc_host_do_transaction(int slot, sdmmc_command_t* cmdinfo)
{
    sd_host_slot_handle_t slot_hdl = sdmmc_get_slot_handle(slot);
    esp_err_t ret = sd_host_slot_do_transaction(slot_hdl, cmdinfo);
    ESP_RETURN_ON_ERROR(ret, TAG, "failed to do SD transaction");

    return ESP_OK;
}
