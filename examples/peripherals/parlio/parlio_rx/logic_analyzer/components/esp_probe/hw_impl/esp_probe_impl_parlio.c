/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <string.h>
#include "sdkconfig.h"
#include "driver/parlio_rx.h"
#include "esp_clk_tree.h"
#include "esp_heap_caps.h"
#include "esp_check.h"
#include "esp_probe_private.h"

typedef struct {
    parlio_rx_unit_handle_t rx_unit;
    parlio_rx_delimiter_handle_t deli;
    uint8_t *payload;
} esp_probe_impl_pralio_t;

static const char *TAG = "esp_probe_impl";
static esp_probe_impl_pralio_t *s_ephi = NULL;

static bool s_esp_probe_on_recv_callback(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_data_t *edata, void *user_data)
{
    esp_probe_handle_t handle = (esp_probe_handle_t)user_data;
    esp_probe_recv_data_t recv_data = {
        .data = edata->data,
        .size = edata->recv_bytes,
    };
    BaseType_t need_yield;
    xQueueSendFromISR(handle->recv_que, &recv_data, &need_yield);

    return need_yield == pdTRUE;
}

esp_err_t esp_probe_priv_init_hardware(esp_probe_handle_t handle, esp_probe_config_t *config, int max_chan_id)
{
    ESP_RETURN_ON_FALSE(!s_ephi, ESP_ERR_INVALID_STATE, TAG, "parlio rx has initialized");
    ESP_RETURN_ON_FALSE(max_chan_id <= PARLIO_RX_UNIT_MAX_DATA_WIDTH, ESP_ERR_NOT_SUPPORTED, TAG,
                        "The target can only support upto %d channels", (int)PARLIO_RX_UNIT_MAX_DATA_WIDTH);
    esp_err_t ret = ESP_OK;
    s_ephi = calloc(1, sizeof(esp_probe_impl_pralio_t));
    ESP_RETURN_ON_FALSE(s_ephi, ESP_ERR_NO_MEM, TAG, "no memory for the esp probe hardware implementation");
    s_ephi->payload = heap_caps_calloc(1, ESP_PROBE_DEFAULT_MAX_RECV_SIZE, ESP_PROBE_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(s_ephi->payload, ESP_ERR_NO_MEM, err, TAG, "no memory for payload");

    // Get the channel number, the channel number can only be the power of 2
    uint32_t sample_width = 1;
    while (sample_width < max_chan_id) {
        sample_width <<= 1;
    }
    handle->sample_width = sample_width;

    // Get the sample rate
    uint32_t src_freq_hz = 0;
    esp_clk_tree_src_get_freq_hz(PARLIO_CLK_SRC_DEFAULT, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &src_freq_hz);
    uint32_t sample_rate_hz = config->sample_rate_hz ? config->sample_rate_hz : src_freq_hz;
    if (sample_rate_hz > src_freq_hz) {
        ESP_LOGW(TAG, "sample rate exceed the max value, limited to %"PRIu32, src_freq_hz);
        sample_rate_hz = src_freq_hz;
    }
    handle->sample_rate_hz = sample_rate_hz;

    // Allocate the parlio rx instance
    parlio_rx_unit_config_t parlio_rx_cfg = {
        .trans_queue_depth = ESP_PROBE_DEFAULT_Q_DEPTH,
        .max_recv_size = ESP_PROBE_DEFAULT_MAX_RECV_SIZE,
        .data_width = sample_width,
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .ext_clk_freq_hz = 0,               // Use the internal clock, no external clock needed
        .clk_in_gpio_num = GPIO_NUM_NC,     // Use the internal clock, no external clock input gpio needed
        .exp_clk_freq_hz = sample_rate_hz,  // Set expected clock frequency (i.e., sample rate)
        .clk_out_gpio_num = GPIO_NUM_NC,    // Use the internal clock for sampling and does not need to output
        .valid_gpio_num = GPIO_NUM_NC,      // Does not need valid gpio, all data gpio are used as sampling channel
        .flags = {
            .clk_gate_en = false,
            .io_loop_back = true,
            .io_no_init = true,
        }
    };
    memcpy(parlio_rx_cfg.data_gpio_nums, config->probe_gpio, PARLIO_RX_UNIT_MAX_DATA_WIDTH * sizeof(gpio_num_t));
    ESP_GOTO_ON_ERROR(parlio_new_rx_unit(&parlio_rx_cfg, &s_ephi->rx_unit), err, TAG, "Failed to allocate the parlio rx unit");

    // Allocate the software delimiter
    parlio_rx_soft_delimiter_config_t sft_deli_cfg = {
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
        .eof_data_len = 0xFFFF > ESP_PROBE_DEFAULT_MAX_RECV_SIZE ? ESP_PROBE_DEFAULT_MAX_RECV_SIZE : 0xFFFF,
        .timeout_ticks = 0,
    };
    ESP_GOTO_ON_ERROR(parlio_new_rx_soft_delimiter(&sft_deli_cfg, &s_ephi->deli), err, TAG, "Failed to allocate the delimiter");

    // Register the data receive callback
    parlio_rx_event_callbacks_t cbs = {
        .on_partial_receive = s_esp_probe_on_recv_callback,
    };
    ESP_GOTO_ON_ERROR(parlio_rx_unit_register_event_callbacks(s_ephi->rx_unit, &cbs, handle), err, TAG, "Failed to register the receive callback");

    return ESP_OK;
err:
    esp_probe_priv_deinit_hardware(handle);
    return ret;
}

esp_err_t esp_probe_priv_deinit_hardware(esp_probe_handle_t handle)
{
    (void)handle;
    if (s_ephi) {
        if (s_ephi->deli) {
            ESP_RETURN_ON_ERROR(parlio_del_rx_delimiter(s_ephi->deli), TAG, "Failed to delete the parlio rx delimiter");
            s_ephi->deli = NULL;
        }
        if (s_ephi->rx_unit) {
            ESP_RETURN_ON_ERROR(parlio_del_rx_unit(s_ephi->rx_unit), TAG, "Failed to delete the parlio rx unit");
            s_ephi->rx_unit = NULL;
        }
        if (s_ephi->payload) {
            free(s_ephi->payload);
            s_ephi->payload = NULL;
        }
        free(s_ephi);
        s_ephi = NULL;
    }
    return ESP_OK;
}

esp_err_t esp_probe_priv_enable_hardware(esp_probe_handle_t handle)
{
    (void)handle;
    ESP_RETURN_ON_ERROR(parlio_rx_unit_enable(s_ephi->rx_unit, true), TAG, "Failed to enable the parlio rx unit");
    ESP_RETURN_ON_ERROR(parlio_rx_soft_delimiter_start_stop(s_ephi->rx_unit, s_ephi->deli, true), TAG, "Failed to start the soft delimiter");
    parlio_receive_config_t recv_cfg = {
        .delimiter = s_ephi->deli,
        .flags.partial_rx_en = true,      // Infinite receiving, use callback to get received data
    };
    ESP_RETURN_ON_ERROR(parlio_rx_unit_receive(s_ephi->rx_unit, s_ephi->payload, ESP_PROBE_DEFAULT_MAX_RECV_SIZE, &recv_cfg), TAG, "Failed to receive data");
    return ESP_OK;
}

esp_err_t esp_probe_priv_disable_hardware(esp_probe_handle_t handle)
{
    (void)handle;
    ESP_RETURN_ON_ERROR(parlio_rx_soft_delimiter_start_stop(s_ephi->rx_unit, s_ephi->deli, false), TAG, "Failed to stop the soft delimiter");
    ESP_RETURN_ON_ERROR(parlio_rx_unit_disable(s_ephi->rx_unit), TAG, "Failed to disable the parlio rx unit");
    return ESP_OK;
}
