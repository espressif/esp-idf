/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_etm.h"
#include "soc/etm_periph.h"

TEST_CASE("etm_channel_install_uninstall", "[etm]")
{
    printf("install etm channels exhaustively\r\n");
    esp_etm_channel_handle_t etm_chans[SOC_ETM_ATTR(INST_NUM)][SOC_ETM_ATTR(CHANS_PER_INST)];
    esp_etm_channel_config_t config = {};
    for (int i = 0; i < SOC_ETM_ATTR(INST_NUM); i++) {
        for (int j = 0; j < SOC_ETM_ATTR(CHANS_PER_INST); j++) {
            TEST_ESP_OK(esp_etm_new_channel(&config, &etm_chans[i][j]));
        }
        TEST_ESP_ERR(ESP_ERR_NOT_FOUND, esp_etm_new_channel(&config, &etm_chans[0][0]));
    }

    TEST_ESP_OK(esp_etm_channel_enable(etm_chans[0][0]));
    TEST_ESP_OK(esp_etm_dump(stdout));

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_etm_del_channel(etm_chans[0][0]));
    TEST_ESP_OK(esp_etm_channel_disable(etm_chans[0][0]));

    for (int i = 0; i < SOC_ETM_ATTR(INST_NUM); i++) {
        for (int j = 0; j < SOC_ETM_ATTR(CHANS_PER_INST); j++) {
            TEST_ESP_OK(esp_etm_del_channel(etm_chans[i][j]));
        }
    }
}
