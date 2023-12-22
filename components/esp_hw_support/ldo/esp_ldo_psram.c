/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_private/esp_ldo.h"
#include "esp_private/esp_ldo_psram.h"

void esp_ldo_vdd_psram_early_init(void)
{
    if (CONFIG_ESP_VDD_PSRAM_LDO_ID != -1) {
        esp_ldo_unit_init_cfg_t unit_cfg = {
            .unit_id = LDO_ID2UNIT(CONFIG_ESP_VDD_PSRAM_LDO_ID),
            .cfg = {
                .voltage_mv = CONFIG_ESP_VDD_PSRAM_LDO_VOLTAGE_MV,
            },
            .flags.enable_unit = true,
            .flags.shared_ldo = true,
        };
        esp_ldo_unit_handle_t early_unit = esp_ldo_init_unit_early(&unit_cfg);
        assert(early_unit);
    }
}
