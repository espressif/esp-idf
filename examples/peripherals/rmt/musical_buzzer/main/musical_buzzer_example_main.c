/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "musical_score_encoder.h"

#define RMT_BUZZER_RESOLUTION_HZ 1000000 // 1MHz resolution
#define RMT_BUZZER_GPIO_NUM      0

static const char *TAG = "example";

/**
 * @brief Musical Score: Beethoven's Ode to joy
 */
static const buzzer_musical_score_t score[] = {
    {740, 400}, {740, 600}, {784, 400}, {880, 400},
    {880, 400}, {784, 400}, {740, 400}, {659, 400},
    {587, 400}, {587, 400}, {659, 400}, {740, 400},
    {740, 400}, {740, 200}, {659, 200}, {659, 800},

    {740, 400}, {740, 600}, {784, 400}, {880, 400},
    {880, 400}, {784, 400}, {740, 400}, {659, 400},
    {587, 400}, {587, 400}, {659, 400}, {740, 400},
    {659, 400}, {659, 200}, {587, 200}, {587, 800},

    {659, 400}, {659, 400}, {740, 400}, {587, 400},
    {659, 400}, {740, 200}, {784, 200}, {740, 400}, {587, 400},
    {659, 400}, {740, 200}, {784, 200}, {740, 400}, {659, 400},
    {587, 400}, {659, 400}, {440, 400}, {440, 400},

    {740, 400}, {740, 600}, {784, 400}, {880, 400},
    {880, 400}, {784, 400}, {740, 400}, {659, 400},
    {587, 400}, {587, 400}, {659, 400}, {740, 400},
    {659, 400}, {659, 200}, {587, 200}, {587, 800},
};

void app_main(void)
{
    ESP_LOGI(TAG, "Create RMT TX channel");
    rmt_channel_handle_t buzzer_chan = NULL;
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
        .gpio_num = RMT_BUZZER_GPIO_NUM,
        .mem_block_symbols = 64,
        .resolution_hz = RMT_BUZZER_RESOLUTION_HZ,
        .trans_queue_depth = 10, // set the maximum number of transactions that can pend in the background
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &buzzer_chan));

    ESP_LOGI(TAG, "Install musical score encoder");
    rmt_encoder_handle_t score_encoder = NULL;
    musical_score_encoder_config_t encoder_config = {
        .resolution = RMT_BUZZER_RESOLUTION_HZ
    };
    ESP_ERROR_CHECK(rmt_new_musical_score_encoder(&encoder_config, &score_encoder));

    ESP_LOGI(TAG, "Enable RMT TX channel");
    ESP_ERROR_CHECK(rmt_enable(buzzer_chan));
    ESP_LOGI(TAG, "Playing Beethoven's Ode to joy...");

    for (size_t i = 0; i < sizeof(score) / sizeof(score[0]); i++) {
        rmt_transmit_config_t tx_config = {
            .loop_count = score[i].duration_ms * score[i].freq_hz / 1000,
        };
        ESP_ERROR_CHECK(rmt_transmit(buzzer_chan, score_encoder, &score[i], sizeof(buzzer_musical_score_t), &tx_config));
    }
}
