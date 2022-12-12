/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_etm.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"

TEST_CASE("etm_channel_install_uninstall", "[etm]")
{
    printf("install etm channels exhaustively\r\n");
    esp_etm_channel_handle_t etm_chans[SOC_ETM_GROUPS][SOC_ETM_CHANNELS_PER_GROUP];
    esp_etm_channel_config_t config = {};
    for (int i = 0; i < SOC_ETM_GROUPS; i++) {
        for (int j = 0; j < SOC_ETM_CHANNELS_PER_GROUP; j++) {
            TEST_ESP_OK(esp_etm_new_channel(&config, &etm_chans[i][j]));
        }
        TEST_ESP_ERR(ESP_ERR_NOT_FOUND, esp_etm_new_channel(&config, &etm_chans[0][0]));
    }

    TEST_ESP_OK(esp_etm_channel_enable(etm_chans[0][0]));
    TEST_ESP_OK(esp_etm_dump(stdout));

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_etm_del_channel(etm_chans[0][0]));
    TEST_ESP_OK(esp_etm_channel_disable(etm_chans[0][0]));

    for (int i = 0; i < SOC_ETM_GROUPS; i++) {
        for (int j = 0; j < SOC_ETM_CHANNELS_PER_GROUP; j++) {
            TEST_ESP_OK(esp_etm_del_channel(etm_chans[i][j]));
        }
    }
}
