/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief This example shows how to recover twai node from bus_off and continue communication
 *
 * 1) Install and start the TWAI driver
 * 2) Sending some frames
 * 3) Manually trigger bit_error by disconnecting the TX / RX or short-circuiting the H & L
 * 4) Initiate bus-off recovery and wait for completion through `twai_node_get_info`
 * 5) Back to step (2)
 */

#include "esp_twai.h"
#include "esp_log.h"
#include "esp_twai_onchip.h"
#include "freertos/FreeRTOS.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////// Please update the following configuration according to your HardWare spec /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TX_GPIO_NUM         GPIO_NUM_4
#define RX_GPIO_NUM         GPIO_NUM_5  // Using same pin to test without transceiver
#define TWAI_BITRATE        100000      // use low bitrate to ensure soft trigger error is effective

static const char *TAG = "twai_recovery";

// bus errors
static bool example_error_cb(twai_node_handle_t handle, const twai_error_event_data_t *edata, void *user_ctx)
{
    ESP_EARLY_LOGI(TAG, "bus error: 0x%lx", edata->err_flags.val);

    return false;
}

// node state
static bool example_state_change_cb(twai_node_handle_t handle, const twai_state_change_event_data_t *edata, void *user_ctx)
{
    const char *twai_state_name[] = {"error_active", "error_warning", "error_passive", "bus_off"};
    ESP_EARLY_LOGI(TAG, "state changed: %s -> %s", twai_state_name[edata->old_sta], twai_state_name[edata->new_sta]);

    return false;
}

void app_main(void)
{
    twai_node_handle_t node_hdl;
    twai_onchip_node_config_t node_config = {
        .io_cfg.tx = TX_GPIO_NUM,
        .io_cfg.rx = RX_GPIO_NUM,
        .bit_timing.bitrate = TWAI_BITRATE,
        .tx_queue_depth = 1,
        .flags.enable_self_test = true,
    };
    ESP_ERROR_CHECK(twai_new_node_onchip(&node_config, &node_hdl));
    ESP_LOGI(TAG, "install twai success, bitrate: %ld, bittime: %d us", node_config.bit_timing.bitrate, (1000000 / TWAI_BITRATE));

    // register callbacks
    twai_event_callbacks_t user_cbs = {
        .on_error = example_error_cb,
        .on_state_change = example_state_change_cb,
    };
    ESP_ERROR_CHECK(twai_node_register_event_callbacks(node_hdl, &user_cbs, NULL));
    ESP_ERROR_CHECK(twai_node_enable(node_hdl));
    ESP_LOGI(TAG, "node started");

    twai_frame_t tx_frame = {
        .buffer = (uint8_t *)"hello\n",
        .buffer_len = 6,
    };
    for (uint8_t i = 0; i < 100; i++) {
        twai_node_status_t node_status;
        twai_node_get_info(node_hdl, &node_status, NULL);
        if (node_status.state == TWAI_ERROR_BUS_OFF) {
            i = 0;
            ESP_LOGI(TAG, "node offline, start recover ...");
            ESP_ERROR_CHECK(twai_node_recover(node_hdl));
            for (uint8_t i = 0; i < 100; i++) {
                ESP_LOGI(TAG, "waiting ... %d", i);
                vTaskDelay(pdMS_TO_TICKS(1000));
                twai_node_get_info(node_hdl, &node_status, NULL);

                if (node_status.state == TWAI_ERROR_ACTIVE) {
                    ESP_LOGI(TAG, "node recovered! continue");
                    break;
                }
            }
        } else {
            ESP_LOGI(TAG, "sending frame %d", i);
            tx_frame.header.id = i;
            ESP_ERROR_CHECK(twai_node_transmit(node_hdl, &tx_frame, 500));
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }

    ESP_ERROR_CHECK(twai_node_disable(node_hdl));
    ESP_ERROR_CHECK(twai_node_delete(node_hdl));
}
