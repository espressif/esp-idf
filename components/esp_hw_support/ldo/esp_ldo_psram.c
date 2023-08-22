/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "soc/soc_caps.h"
#include "hal/ldo_ll.h"
#include "esp_private/esp_ldo_psram.h"

static const char *TAG = "ldo_psram";

esp_ldo_unit_handle_t esp_ldo_vdd_psram_early_init(void)
{
    if (CONFIG_ESP_VDD_PSRAM_LDO_ID != -1) {
        esp_ldo_unit_init_cfg_t unit_cfg = {
            .unit_id = CONFIG_ESP_VDD_PSRAM_LDO_ID,
            .cfg = {
                .voltage_mv = CONFIG_ESP_VDD_PSRAM_LDO_VOLTAGE_MV,
            },
            .flags.enable_unit = true,
            .flags.shared_ldo = true,
        };
        esp_ldo_unit_handle_t early_unit = esp_ldo_init_unit_early(&unit_cfg);
        assert(early_unit);
        return early_unit;
    }
    return NULL;
}

esp_err_t esp_ldo_vdd_psram_init(esp_ldo_unit_handle_t *ldo_unit)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(ldo_unit, ESP_ERR_INVALID_ARG, TAG, "null pointer");
    if (CONFIG_ESP_VDD_PSRAM_LDO_ID != -1) {
        esp_ldo_unit_init_cfg_t unit_cfg = {
            .unit_id = LDO_ID2UNIT(CONFIG_ESP_VDD_PSRAM_LDO_ID),
            .cfg = {
                .voltage_mv = CONFIG_ESP_VDD_PSRAM_LDO_VOLTAGE_MV,
            },
            .flags.enable_unit = true,
            .flags.shared_ldo = true,
        };
        ESP_RETURN_ON_ERROR(esp_ldo_init_unit(&unit_cfg, ldo_unit), TAG, "internal LDO init failed");
    } else {
        ESP_LOGD(TAG, "internal LDO not initialized, external power supply is configured to be used");
        *ldo_unit = NULL;
        return ESP_ERR_INVALID_STATE;
    }
    return ret;
}

esp_err_t esp_ldo_vdd_psram_deinit(esp_ldo_unit_handle_t ldo_unit)
{
    return esp_ldo_deinit_unit(ldo_unit);
}
