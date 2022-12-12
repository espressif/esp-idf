/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_check.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"
#include "driver/rmt_types.h"
#include "driver/rmt_encoder.h"
#include "onewire_bus_rmt.h"

static const char *TAG = "onewire_rmt";

/**
 * @brief RMT resolution for 1-wire bus, in Hz
 *
 */
#define ONEWIRE_RMT_RESOLUTION_HZ 1000000

/**
 * @brief 1-wire bus timing parameters, in us (1/ONEWIRE_RMT_RESOLUTION_HZ)
 *
 */
#define ONEWIRE_RESET_PULSE_DURATION 500 // duration of reset bit
#define ONEWIRE_RESET_WAIT_DURATION 200 // how long should master wait for device to show its presence
#define ONEWIRE_RESET_PRESENSE_WAIT_DURATION_MIN 15 // minimum duration for master to wait device to show its presence
#define ONEWIRE_RESET_PRESENSE_DURATION_MIN 60 // minimum duration for master to recognize device as present

#define ONEWIRE_SLOT_START_DURATION 2 // bit start pulse duration
#define ONEWIRE_SLOT_BIT_DURATION 60 // duration for each bit to transmit
// refer to https://www.maximintegrated.com/en/design/technical-documents/app-notes/3/3829.html for more information
#define ONEWIRE_SLOT_RECOVERY_DURATION 2  // recovery time between each bit, should be longer in parasite power mode
#define ONEWIRE_SLOT_BIT_SAMPLE_TIME 15 // how long after bit start pulse should the master sample from the bus

/*
Reset Pulse:

          | RESET_PULSE | RESET_WAIT_DURATION |
          | _DURATION   |                     |
          |             |   | | RESET     |   |
          |             | * | | _PRESENSE |   |
          |             |   | | _DURATION |   |
------────┐             ┌─────┐           ┌───────-------
          │             │     │           │
          │             │     │           │
          │             │     │           │
          └─────────────┘     └───────────┘
*: RESET_PRESENSE_WAIT_DURATION

Write 1 bit:

          | SLOT_START | SLOT_BIT  | SLOT_RECOVERY | NEXT
          | _DURATION  | _DURATION | _DURATION     | SLOT
          |            |           |               |
------────┐            ┌───────────────────────────────------
          │            │
          │            │
          │            │
          └────────────┘

Write 0 bit:

          | SLOT_START | SLOT_BIT  | SLOT_RECOVERY | NEXT
          | _DURATION  | _DURATION | _DURATION     | SLOT
          |            |           |               |
------────┐                        ┌───────────────────------
          │                        │
          │                        │
          │                        │
          └────────────────────────┘

Read 1 bit:


          | SLOT_START | SLOT_BIT_DURATION | SLOT_RECOVERY | NEXT
          | _DURATION  |                   | _DURATION     | SLOT
          |            | SLOT_BIT_   |     |               |
          |            | SAMPLE_TIME |     |               |
------────┐            ┌────────────────────────────────────────------
          │            │
          │            │
          │            │
          └────────────┘

Read 0 bit:

          | SLOT_START | SLOT_BIT_DURATION | SLOT_RECOVERY | NEXT
          | _DURATION  |                   | _DURATION     | SLOT
          |            | SLOT_BIT_   |     |               |
          |            | SAMPLE_TIME |     |               |
------────┐            |             |  ┌───────────────────────------
          │            |                │
          │            |   PULLED DOWN  │
          │            |    BY DEVICE   │
          └─────────────────────────────┘
*/

struct onewire_bus_t {
    rmt_channel_handle_t tx_channel; /*!< rmt tx channel handler */
    rmt_encoder_handle_t tx_bytes_encoder; /*!< used to encode commands and data */
    rmt_encoder_handle_t tx_copy_encoder; /*!< used to encode reset pulse and bits */

    rmt_channel_handle_t rx_channel; /*!< rmt rx channel handler */
    rmt_symbol_word_t *rx_symbols; /*!<  hold rmt raw symbols */

    size_t max_rx_bytes; /*!< buffer size in byte for single receive transaction */

    QueueHandle_t receive_queue;
};

const static rmt_symbol_word_t onewire_bit0_symbol = {
    .level0 = 0,
    .duration0 = ONEWIRE_SLOT_START_DURATION + ONEWIRE_SLOT_BIT_DURATION,
    .level1 = 1,
    .duration1 = ONEWIRE_SLOT_RECOVERY_DURATION
};

const static rmt_symbol_word_t onewire_bit1_symbol = {
    .level0 = 0,
    .duration0 = ONEWIRE_SLOT_START_DURATION,
    .level1 = 1,
    .duration1 = ONEWIRE_SLOT_BIT_DURATION + ONEWIRE_SLOT_RECOVERY_DURATION
};

const static rmt_symbol_word_t onewire_reset_pulse_symbol = {
    .level0 = 0,
    .duration0 = ONEWIRE_RESET_PULSE_DURATION,
    .level1 = 1,
    .duration1 = ONEWIRE_RESET_WAIT_DURATION
};

const static rmt_transmit_config_t onewire_rmt_tx_config = {
    .loop_count = 0, // no transfer loop
    .flags.eot_level = 1 // onewire bus should be released in IDLE
};

const static rmt_receive_config_t onewire_rmt_rx_config = {
    .signal_range_min_ns = 1000000000 / ONEWIRE_RMT_RESOLUTION_HZ,
    .signal_range_max_ns = (ONEWIRE_RESET_PULSE_DURATION + ONEWIRE_RESET_WAIT_DURATION) * 1000
};

static bool onewire_rmt_rx_done_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t task_woken = pdFALSE;
    struct onewire_bus_t *handle = (struct onewire_bus_t *)user_data;

    xQueueSendFromISR(handle->receive_queue, edata, &task_woken);

    return task_woken;
}

/*
[0].0 means symbol[0].duration0

First reset pulse after rmt channel init:

Bus is low | Reset | Wait |  Device  |  Bus Idle
after init | Pulse |      | Presense |
                   ┌──────┐          ┌─────------
                   │      │          │
                   │      │          │
                   │      │          │
------─────────────┘      └──────────┘
                   1      2          3

          [0].1     [0].0     [1].1     [1].0


Following reset pulses:

Bus is high | Reset | Wait |  Device  |  Bus Idle
after init  | Pulse |      | Presense |
------──────┐       ┌──────┐          ┌─────------
            │       │      │          │
            │       │      │          │
            │       │      │          │
            └───────┘      └──────────┘
            1       2      3          4

              [0].0  [0].1     [1].0    [1].1
*/

static bool onewire_rmt_check_presence_pulse(rmt_symbol_word_t *rmt_symbols, size_t symbol_num)
{
    if (symbol_num >= 2) { // there should be at lease 2 symbols(3 or 4 edges)
        if (rmt_symbols[0].level1 == 1) { // bus is high before reset pulse
            if (rmt_symbols[0].duration1 > ONEWIRE_RESET_PRESENSE_WAIT_DURATION_MIN &&
                    rmt_symbols[1].duration0 > ONEWIRE_RESET_PRESENSE_DURATION_MIN) {
                return true;
            }
        } else { // bus is low before reset pulse(first pulse after rmt channel init)
            if (rmt_symbols[0].duration0 > ONEWIRE_RESET_PRESENSE_WAIT_DURATION_MIN &&
                    rmt_symbols[1].duration1 > ONEWIRE_RESET_PRESENSE_DURATION_MIN) {
                return true;
            }
        }
    }

    ESP_LOGE(TAG, "no device present on 1-wire bus");
    return false;
}

static void onewire_rmt_decode_data(rmt_symbol_word_t *rmt_symbols, size_t symbol_num, uint8_t *decoded_bytes)
{
    size_t byte_pos = 0, bit_pos = 0;
    for (size_t i = 0; i < symbol_num; i ++) {
        if (rmt_symbols[i].duration0 > ONEWIRE_SLOT_BIT_SAMPLE_TIME) { // 0 bit
            decoded_bytes[byte_pos] &= ~(1 << bit_pos); // LSB first
        } else { // 1 bit
            decoded_bytes[byte_pos] |= 1 << bit_pos;
        }

        bit_pos ++;
        if (bit_pos >= 8) {
            bit_pos = 0;
            byte_pos ++;
        }
    }
}

esp_err_t onewire_new_bus_rmt(onewire_rmt_config_t *config, onewire_bus_handle_t *handle_out)
{
    ESP_RETURN_ON_FALSE(config, ESP_ERR_INVALID_ARG, TAG, "invalid config pointer");
    ESP_RETURN_ON_FALSE(handle_out, ESP_ERR_INVALID_ARG, TAG, "invalid handle pointer");

    esp_err_t ret = ESP_OK;

    struct onewire_bus_t *handle = calloc(1, sizeof(struct onewire_bus_t));
    ESP_GOTO_ON_FALSE(handle, ESP_ERR_NO_MEM, err, TAG, "memory allocation for 1-wire bus handler failed");

    // create rmt bytes encoder to transmit 1-wire commands and data
    rmt_bytes_encoder_config_t bytes_encoder_config = {
        .bit0 = onewire_bit0_symbol,
        .bit1 = onewire_bit1_symbol,
        .flags.msb_first = 0
    };
    ESP_GOTO_ON_ERROR(rmt_new_bytes_encoder(&bytes_encoder_config, &handle->tx_bytes_encoder),
                      err, TAG, "create data tx encoder failed");

    // create rmt copy encoder to transmit 1-wire reset pulse or bits
    rmt_copy_encoder_config_t copy_encoder_config = {};
    ESP_GOTO_ON_ERROR(rmt_new_copy_encoder(&copy_encoder_config, &handle->tx_copy_encoder),
                      err, TAG, "create reset pulse tx encoder failed");

    // create rmt rx channel
    rmt_rx_channel_config_t onewire_rx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = config->gpio_pin,
#if SOC_RMT_SUPPORT_RX_PINGPONG
        .mem_block_symbols = 64, // when the chip is ping-pong capable, we can use less rx memory blocks
#else
        .mem_block_symbols = config->max_rx_bytes * 8,
#endif
        .resolution_hz = ONEWIRE_RMT_RESOLUTION_HZ, // in us
    };
    ESP_GOTO_ON_ERROR(rmt_new_rx_channel(&onewire_rx_channel_cfg, &handle->rx_channel),
                      err, TAG, "create rmt rx channel failed");
    ESP_LOGI(TAG, "RMT Tx channel created for 1-wire bus");

    // create rmt tx channel after rx channel
    rmt_tx_channel_config_t onewire_tx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = config->gpio_pin,
        .mem_block_symbols = 64, // ping-pong is always avaliable on tx channel, save hardware memory blocks
        .resolution_hz = ONEWIRE_RMT_RESOLUTION_HZ, // in us
        .trans_queue_depth = 4,
        .flags.io_loop_back = true, // make tx channel coexist with rx channel on the same gpio pin
        .flags.io_od_mode = true // enable open-drain mode for 1-wire bus
    };
    ESP_GOTO_ON_ERROR(rmt_new_tx_channel(&onewire_tx_channel_cfg, &handle->tx_channel),
                      err, TAG, "create rmt tx channel failed");
    ESP_LOGI(TAG, "RMT Rx channel created for 1-wire bus");

    // allocate rmt rx symbol buffer
    handle->rx_symbols = malloc(config->max_rx_bytes * sizeof(rmt_symbol_word_t) * 8);
    ESP_GOTO_ON_FALSE(handle->rx_symbols, ESP_ERR_NO_MEM, err, TAG, "memory allocation for rx symbol buffer failed");
    handle->max_rx_bytes = config->max_rx_bytes;

    handle->receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));
    ESP_GOTO_ON_FALSE(handle->receive_queue, ESP_ERR_NO_MEM, err, TAG, "receive queue creation failed");

    // register rmt rx done callback
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = onewire_rmt_rx_done_callback
    };
    ESP_GOTO_ON_ERROR(rmt_rx_register_event_callbacks(handle->rx_channel, &cbs, handle),
                      err, TAG, "enable rmt rx channel failed");

    // enable rmt channels
    ESP_GOTO_ON_ERROR(rmt_enable(handle->rx_channel), err, TAG, "enable rmt rx channel failed");
    ESP_GOTO_ON_ERROR(rmt_enable(handle->tx_channel), err, TAG, "enable rmt tx channel failed");

    *handle_out = handle;
    return ESP_OK;

err:
    if (handle) {
        onewire_del_bus(handle);
    }

    return ret;
}

esp_err_t onewire_del_bus(onewire_bus_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid 1-wire handle");

    if (handle->tx_bytes_encoder) {
        rmt_del_encoder(handle->tx_bytes_encoder);
    }
    if (handle->tx_copy_encoder) {
        rmt_del_encoder(handle->tx_copy_encoder);
    }
    if (handle->rx_channel) {
        rmt_disable(handle->rx_channel);
        rmt_del_channel(handle->rx_channel);
    }
    if (handle->tx_channel) {
        rmt_disable(handle->tx_channel);
        rmt_del_channel(handle->tx_channel);
    }
    if (handle->receive_queue) {
        vQueueDelete(handle->receive_queue);
    }
    if (handle->rx_symbols) {
        free(handle->rx_symbols);
    }
    free(handle);

    return ESP_OK;
}

esp_err_t onewire_bus_reset(onewire_bus_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid 1-wire handle");

    // send reset pulse while receive presence pulse
    ESP_RETURN_ON_ERROR(rmt_receive(handle->rx_channel, handle->rx_symbols, sizeof(rmt_symbol_word_t) * 2, &onewire_rmt_rx_config),
                        TAG, "1-wire reset pulse receive failed");
    ESP_RETURN_ON_ERROR(rmt_transmit(handle->tx_channel, handle->tx_copy_encoder, &onewire_reset_pulse_symbol, sizeof(onewire_reset_pulse_symbol), &onewire_rmt_tx_config),
                        TAG, "1-wire reset pulse transmit failed");

    // wait and check presence pulse
    bool is_present = false;
    rmt_rx_done_event_data_t rmt_rx_evt_data;
    if (xQueueReceive(handle->receive_queue, &rmt_rx_evt_data, pdMS_TO_TICKS(1000)) == pdPASS) {
        is_present = onewire_rmt_check_presence_pulse(rmt_rx_evt_data.received_symbols, rmt_rx_evt_data.num_symbols);
    }

    return is_present ? ESP_OK : ESP_ERR_NOT_FOUND;
}

esp_err_t onewire_bus_write_bytes(onewire_bus_handle_t handle, const uint8_t *tx_data, uint8_t tx_data_size)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid 1-wire handle");
    ESP_RETURN_ON_FALSE(tx_data && tx_data_size != 0, ESP_ERR_INVALID_ARG, TAG, "invalid tx buffer or buffer size");

    // transmit data
    ESP_RETURN_ON_ERROR(rmt_transmit(handle->tx_channel, handle->tx_bytes_encoder, tx_data, tx_data_size, &onewire_rmt_tx_config),
                        TAG, "1-wire data transmit failed");

    // wait the transmission to complete
    ESP_RETURN_ON_ERROR(rmt_tx_wait_all_done(handle->tx_channel, 50), TAG, "wait for 1-wire data transmit failed");

    return ESP_OK;
}

esp_err_t onewire_bus_read_bytes(onewire_bus_handle_t handle, uint8_t *rx_data, size_t rx_data_size)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid 1-wire handle");
    ESP_RETURN_ON_FALSE(rx_data && rx_data_size != 0, ESP_ERR_INVALID_ARG, TAG, "invalid rx buffer or buffer size");
    ESP_RETURN_ON_FALSE(!(rx_data_size > handle->max_rx_bytes), ESP_ERR_INVALID_ARG,
                        TAG, "rx_data_size too large for buffer to hold");

    uint8_t tx_buffer[rx_data_size];
    memset(tx_buffer, 0xFF, rx_data_size); // transmit one bits to generate read clock

    // transmit 1 bits while receiving
    ESP_RETURN_ON_ERROR(rmt_receive(handle->rx_channel, handle->rx_symbols, rx_data_size * 8 * sizeof(rmt_symbol_word_t), &onewire_rmt_rx_config),
                        TAG, "1-wire data receive failed");
    ESP_RETURN_ON_ERROR(rmt_transmit(handle->tx_channel, handle->tx_bytes_encoder, tx_buffer, sizeof(tx_buffer), &onewire_rmt_tx_config),
                        TAG, "1-wire data transmit failed");

    // wait the transmission finishes and decode data
    rmt_rx_done_event_data_t rmt_rx_evt_data;
    if (xQueueReceive(handle->receive_queue, &rmt_rx_evt_data, pdMS_TO_TICKS(1000)) == pdPASS) {
        onewire_rmt_decode_data(rmt_rx_evt_data.received_symbols, rmt_rx_evt_data.num_symbols, rx_data);
    } else {
        return ESP_ERR_TIMEOUT;
    }

    return ESP_OK;
}

esp_err_t onewire_bus_write_bit(onewire_bus_handle_t handle, uint8_t tx_bit)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid 1-wire handle");

    const rmt_symbol_word_t *symbol_to_transmit = tx_bit ? &onewire_bit1_symbol : &onewire_bit0_symbol;

    // transmit bit
    ESP_RETURN_ON_ERROR(rmt_transmit(handle->tx_channel, handle->tx_copy_encoder, symbol_to_transmit, sizeof(onewire_bit1_symbol), &onewire_rmt_tx_config),
                        TAG, "1-wire bit transmit failed");

    // wait the transmission to complete
    ESP_RETURN_ON_ERROR(rmt_tx_wait_all_done(handle->tx_channel, 50), TAG, "wait for 1-wire bit transmit failed");

    return ESP_OK;
}

esp_err_t onewire_bus_read_bit(onewire_bus_handle_t handle, uint8_t *rx_bit)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid 1-wire handle");
    ESP_RETURN_ON_FALSE(rx_bit, ESP_ERR_INVALID_ARG, TAG, "invalid rx_bit pointer");

    // transmit 1 bit while receiving
    ESP_RETURN_ON_ERROR(rmt_receive(handle->rx_channel, handle->rx_symbols, sizeof(rmt_symbol_word_t), &onewire_rmt_rx_config),
                        TAG, "1-wire bit receive failed");
    ESP_RETURN_ON_ERROR(rmt_transmit(handle->tx_channel, handle->tx_copy_encoder, &onewire_bit1_symbol, sizeof(onewire_bit1_symbol), &onewire_rmt_tx_config),
                        TAG, "1-wire bit transmit failed");

    // wait the transmission finishes and decode data
    rmt_rx_done_event_data_t rmt_rx_evt_data;
    if (xQueueReceive(handle->receive_queue, &rmt_rx_evt_data, pdMS_TO_TICKS(1000)) == pdPASS) {
        uint8_t rx_buffer[1];
        onewire_rmt_decode_data(rmt_rx_evt_data.received_symbols, rmt_rx_evt_data.num_symbols, rx_buffer);
        *rx_bit = rx_buffer[0] & 0x01;
    } else {
        return ESP_ERR_TIMEOUT;
    }

    return ESP_OK;
}
