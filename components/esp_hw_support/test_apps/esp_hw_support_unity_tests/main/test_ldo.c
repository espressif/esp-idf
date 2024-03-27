/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "unity.h"
#include "esp_private/esp_ldo.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TEST_CASE("LDO unit early / normal allocation", "[LDO]")
{
    esp_ldo_unit_init_cfg_t init_early_unit_cfg = {
        .unit_id = LDO_UNIT_3,
        .cfg = {
            .voltage_mv = 1800,
        },
        .flags.enable_unit = true,
    };
    esp_ldo_unit_handle_t early_unit = esp_ldo_init_unit_early(&init_early_unit_cfg);
    TEST_ASSERT(esp_ldo_enable_unit(early_unit) == ESP_ERR_INVALID_STATE);
    TEST_ESP_OK(esp_ldo_disable_unit(early_unit));

    esp_ldo_unit_handle_t unit = NULL;
    esp_ldo_unit_init_cfg_t init_unit_cfg = {
        .unit_id = LDO_UNIT_4,
        .cfg = {
            .voltage_mv = 2500,
        },
    };
    TEST_ESP_OK(esp_ldo_init_unit(&init_unit_cfg, &unit));
    TEST_ESP_OK(esp_ldo_enable_unit(unit));
    TEST_ESP_OK(esp_ldo_disable_unit(unit));

    init_unit_cfg.flags.shared_ldo = true;
    esp_ldo_unit_handle_t shared_unit = NULL;
    TEST_ESP_OK(esp_ldo_init_unit(&init_unit_cfg, &shared_unit));

    TEST_ESP_OK(esp_ldo_deinit_unit(early_unit));
    TEST_ESP_OK(esp_ldo_deinit_unit(shared_unit));
    TEST_ESP_OK(esp_ldo_deinit_unit(unit));
}

TEST_CASE("LDO unit output", "[LDO][mannual][ignore]")
{
    esp_ldo_unit_init_cfg_t early_unit_cfg = {
        .unit_id = LDO_UNIT_2,
        .cfg = {
            .voltage_mv = 1800,
        },
        .flags.shared_ldo = true,
        .flags.enable_unit = true,
    };
    esp_ldo_unit_handle_t early_unit2 = esp_ldo_init_unit_early(&early_unit_cfg);
    assert(early_unit2);

    early_unit_cfg.unit_id = LDO_UNIT_3;
    early_unit_cfg.cfg.voltage_mv = 3300;
    esp_ldo_unit_handle_t early_unit3 = esp_ldo_init_unit_early(&early_unit_cfg);
    assert(early_unit3);

    early_unit_cfg.unit_id = LDO_UNIT_4;
    early_unit_cfg.cfg.voltage_mv = 1100;
    esp_ldo_unit_handle_t early_unit4 = esp_ldo_init_unit_early(&early_unit_cfg);
    assert(early_unit4);

    esp_ldo_usage_dump(stdout);

    while (1) {
        vTaskDelay(1);
    }
}
