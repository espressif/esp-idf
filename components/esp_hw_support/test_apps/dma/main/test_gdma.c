/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "unity.h"
#include "esp_private/gdma.h"
#include "soc/soc_caps.h"

TEST_CASE("GDMA channel allocation", "[gdma]")
{
    gdma_channel_alloc_config_t channel_config = {};
    gdma_channel_handle_t tx_channels[SOC_GDMA_PAIRS_PER_GROUP] = {};
    gdma_channel_handle_t rx_channels[SOC_GDMA_PAIRS_PER_GROUP] = {};
    channel_config.direction = GDMA_CHANNEL_DIRECTION_TX;
    gdma_tx_event_callbacks_t tx_cbs = {};
    gdma_rx_event_callbacks_t rx_cbs = {};

    // install TX channels for different peripherals
    for (int i = 0; i < SOC_GDMA_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_new_channel(&channel_config, &tx_channels[i]));
        TEST_ESP_OK(gdma_connect(tx_channels[i], GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_M2M, 0)));
        TEST_ESP_OK(gdma_register_tx_event_callbacks(tx_channels[i], &tx_cbs, NULL));
    };
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, gdma_new_channel(&channel_config, &tx_channels[0]));

    // Free interrupts before installing RX interrupts to ensure enough free interrupts
    for (int i = 0; i < SOC_GDMA_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_disconnect(tx_channels[i]));
        TEST_ESP_OK(gdma_del_channel(tx_channels[i]));
    }

    // install RX channels for different peripherals
    channel_config.direction = GDMA_CHANNEL_DIRECTION_RX;
    for (int i = 0; i < SOC_GDMA_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_new_channel(&channel_config, &rx_channels[i]));
        TEST_ESP_OK(gdma_connect(rx_channels[i], GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_M2M, 0)));
        TEST_ESP_OK(gdma_register_rx_event_callbacks(rx_channels[i], &rx_cbs, NULL));
    }
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, gdma_new_channel(&channel_config, &rx_channels[0]));

    for (int i = 0; i < SOC_GDMA_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_disconnect(rx_channels[i]));
        TEST_ESP_OK(gdma_del_channel(rx_channels[i]));
    }

    // install single and paired TX/RX channels
#if SOC_GDMA_PAIRS_PER_GROUP >= 2
    // single tx channel
    channel_config.direction = GDMA_CHANNEL_DIRECTION_TX;
    TEST_ESP_OK(gdma_new_channel(&channel_config, &tx_channels[0]));

    // create tx channel and reserve sibling
    channel_config.direction = GDMA_CHANNEL_DIRECTION_TX;
    channel_config.flags.reserve_sibling = 1;
    TEST_ESP_OK(gdma_new_channel(&channel_config, &tx_channels[1]));
    // create rx channel and specify sibling channel
    channel_config.flags.reserve_sibling = 0;
    channel_config.sibling_chan = tx_channels[1]; // specify sibling channel
    channel_config.direction = GDMA_CHANNEL_DIRECTION_RX;
    TEST_ESP_OK(gdma_new_channel(&channel_config, &rx_channels[1]));
    channel_config.sibling_chan = NULL;
    TEST_ESP_OK(gdma_new_channel(&channel_config, &rx_channels[0]));
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(gdma_del_channel(tx_channels[i]));
        TEST_ESP_OK(gdma_del_channel(rx_channels[i]));
    }
#endif
}
