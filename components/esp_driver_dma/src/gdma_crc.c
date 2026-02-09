/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gdma_priv.h"

ESP_LOG_ATTR_TAG(TAG, "gdma");

esp_err_t gdma_config_crc_calculator(gdma_channel_handle_t dma_chan, const gdma_crc_calculator_config_t *config)
{
    ESP_RETURN_ON_FALSE(dma_chan && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;
    switch (group->bus_id) {
#if SOC_HAS(AHB_GDMA)
    case SOC_GDMA_BUS_AHB:
        ESP_RETURN_ON_FALSE(config->crc_bit_width <= GDMA_LL_AHB_MAX_CRC_BIT_WIDTH, ESP_ERR_INVALID_ARG, TAG, "invalid crc bit width");
        break;
#endif // SOC_HAS(AHB_GDMA)
#if SOC_HAS(AXI_GDMA)
    case SOC_GDMA_BUS_AXI:
        ESP_RETURN_ON_FALSE(config->crc_bit_width <= GDMA_LL_AXI_MAX_CRC_BIT_WIDTH, ESP_ERR_INVALID_ARG, TAG, "invalid crc bit width");
        break;
#endif // SOC_HAS(AXI_GDMA)
    default:
        ESP_LOGE(TAG, "invalid bus id: %d", group->bus_id);
        return ESP_ERR_INVALID_ARG;
    }

    // clear the previous CRC result
    gdma_hal_clear_crc(hal, pair->pair_id, dma_chan->direction);

    // set polynomial and initial value
    gdma_hal_crc_config_t hal_config = {
        .crc_bit_width = config->crc_bit_width,
        .poly_hex = config->poly_hex,
        .init_value = config->init_value,
        .reverse_data_mask = config->reverse_data_mask,
    };
    gdma_hal_set_crc_poly(hal, pair->pair_id, dma_chan->direction, &hal_config);

    return ESP_OK;
}

esp_err_t gdma_crc_get_result(gdma_channel_handle_t dma_chan, uint32_t *result)
{
    ESP_RETURN_ON_FALSE(dma_chan && result, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    *result = gdma_hal_get_crc_result(hal, pair->pair_id, dma_chan->direction);

    return ESP_OK;
}
