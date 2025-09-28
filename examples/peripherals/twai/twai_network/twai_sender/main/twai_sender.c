/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"

#define TWAI_SENDER_TX_GPIO     CONFIG_EXAMPLE_TWAI_TX_GPIO
#define TWAI_SENDER_RX_GPIO     CONFIG_EXAMPLE_TWAI_RX_GPIO
#define TWAI_QUEUE_DEPTH        10
#define TWAI_BITRATE            1000000

// Message IDs
#define TWAI_DATA_ID            0x100
#define TWAI_HEARTBEAT_ID       0x7FF
#define TWAI_DATA_LEN           1000

static const char *TAG = "twai_sender";

typedef struct {
    twai_frame_t frame;
    uint8_t data[TWAI_FRAME_MAX_LEN];
} twai_sender_data_t;

// Transmission completion callback
static IRAM_ATTR bool twai_sender_tx_done_callback(twai_node_handle_t handle, const twai_tx_done_event_data_t *edata, void *user_ctx)
{
    if (!edata->is_tx_success) {
        ESP_EARLY_LOGW(TAG, "Failed to transmit message, ID: 0x%X", edata->done_tx_frame->header.id);
    }
    return false; // No task wake required
}

// Bus error callback
static IRAM_ATTR bool twai_sender_on_error_callback(twai_node_handle_t handle, const twai_error_event_data_t *edata, void *user_ctx)
{
    ESP_EARLY_LOGW(TAG, "TWAI node error: 0x%x", edata->err_flags.val);
    return false; // No task wake required
}

void app_main(void)
{
    twai_node_handle_t sender_node = NULL;
    printf("===================TWAI Sender Example Starting...===================\n");

    // Configure TWAI node
    twai_onchip_node_config_t node_config = {
        .io_cfg = {
            .tx = TWAI_SENDER_TX_GPIO,
            .rx = TWAI_SENDER_RX_GPIO,
            .quanta_clk_out = -1,
            .bus_off_indicator = -1,
        },
        .bit_timing = {
            .bitrate = TWAI_BITRATE,
        },
        .fail_retry_cnt = 3,
        .tx_queue_depth = TWAI_QUEUE_DEPTH,
    };

    // Create TWAI node
    ESP_ERROR_CHECK(twai_new_node_onchip(&node_config, &sender_node));

    // Register transmission completion callback
    twai_event_callbacks_t callbacks = {
        .on_tx_done = twai_sender_tx_done_callback,
        .on_error = twai_sender_on_error_callback,
    };
    ESP_ERROR_CHECK(twai_node_register_event_callbacks(sender_node, &callbacks, NULL));

    // Enable TWAI node
    ESP_ERROR_CHECK(twai_node_enable(sender_node));
    ESP_LOGI(TAG, "TWAI Sender started successfully");
    ESP_LOGI(TAG, "Sending messages with IDs: 0x%03X (data), 0x%03X (heartbeat)",  TWAI_DATA_ID, TWAI_HEARTBEAT_ID);

    while (1) {
        // Send heartbeat message
        uint64_t timestamp = esp_timer_get_time();
        twai_frame_t tx_frame = {
            .header.id = TWAI_HEARTBEAT_ID,
            .buffer = (uint8_t *) &timestamp,
            .buffer_len = sizeof(timestamp),
        };
        ESP_ERROR_CHECK(twai_node_transmit(sender_node, &tx_frame, 500));
        ESP_LOGI(TAG, "Sending heartbeat message: %lld", timestamp);
        ESP_ERROR_CHECK(twai_node_transmit_wait_all_done(sender_node, -1)); // -1 means wait forever

        // Send burst data messages every 10 seconds
        if ((timestamp / 1000000) % 10 == 0) {
            int num_frames = howmany(TWAI_DATA_LEN, TWAI_FRAME_MAX_LEN);
            twai_sender_data_t *data = (twai_sender_data_t *)calloc(num_frames, sizeof(twai_sender_data_t));
            assert(data != NULL);
            ESP_LOGI(TAG, "Sending packet of %d bytes in %d frames", TWAI_DATA_LEN, num_frames);
            for (int i = 0; i < num_frames; i++) {
                data[i].frame.header.id = TWAI_DATA_ID;
                data[i].frame.buffer = data[i].data;
                data[i].frame.buffer_len = TWAI_FRAME_MAX_LEN;
                memset(data[i].data, i, TWAI_FRAME_MAX_LEN);
                ESP_ERROR_CHECK(twai_node_transmit(sender_node, &data[i].frame, 500));
            }

            // Frames mounted, wait for all frames to be transmitted
            ESP_ERROR_CHECK(twai_node_transmit_wait_all_done(sender_node, -1));
            free(data);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
        twai_node_status_t status;
        twai_node_get_info(sender_node, &status, NULL);
        if (status.state == TWAI_ERROR_BUS_OFF) {
            ESP_LOGW(TAG, "Bus-off detected");
            return;
        }
    }

    ESP_ERROR_CHECK(twai_node_disable(sender_node));
    ESP_ERROR_CHECK(twai_node_delete(sender_node));
}
