/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* SPI Slave example, receiver (uses SPI Slave driver to communicate with sender)

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <openthread/platform/spi-slave.h>

#include "esp_attr.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_task_queue.h"
#include "esp_openthread_types.h"
#include "esp_rom_sys.h"
#include <stdint.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/spi_slave.h"
#include "esp_private/cache_utils.h"
#include "esp_private/spi_slave_internal.h"
#include "ncp/ncp_config.h"
#include "openthread/error.h"

static const char *SPI_SLAVE_TAG = "spi_slave";

static void *s_context = NULL;
static uint8_t *s_prev_output_buf;
static uint16_t s_prev_output_len;
static uint8_t *s_prev_input_buf;
static uint16_t s_prev_input_len;
static bool s_request_transaction = false;

static esp_openthread_spi_slave_config_t s_spi_config;
static otPlatSpiSlaveTransactionProcessCallback s_process_callback = NULL;
static otPlatSpiSlaveTransactionCompleteCallback s_complete_callback = NULL;
static spi_slave_transaction_t s_spi_transaction;

typedef struct {
    uint16_t output_buf_len;
    uint16_t input_buf_len;
} pending_transaction_t;

static void IRAM_ATTR handle_spi_setup_done(spi_slave_transaction_t *trans)
{
    if (s_request_transaction) {
        gpio_set_level(s_spi_config.intr_pin, 0);
    }
}

static void IRAM_ATTR handle_spi_transaction_done(spi_slave_transaction_t *trans)
{
    gpio_set_level(s_spi_config.intr_pin, 1);
    pending_transaction_t *pending_transaction = (pending_transaction_t *)&(trans->user);
    trans->trans_len /= CHAR_BIT;

    if (s_complete_callback &&
        s_complete_callback(s_context, (uint8_t *)trans->tx_buffer, pending_transaction->output_buf_len,
                            trans->rx_buffer, pending_transaction->input_buf_len, trans->trans_len)) {
        esp_openthread_task_queue_post(s_process_callback, s_context);
    }
    trans = NULL;
}

esp_err_t esp_openthread_spi_slave_init(const esp_openthread_platform_config_t *config)
{
    s_spi_config = config->host_config.spi_slave_config;
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1 << s_spi_config.intr_pin),
    };
    ESP_RETURN_ON_ERROR(gpio_config(&io_conf), OT_PLAT_LOG_TAG, "fail to configure SPI gpio");

    gpio_set_pull_mode(s_spi_config.bus_config.mosi_io_num, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(s_spi_config.bus_config.sclk_io_num, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(s_spi_config.slave_config.spics_io_num, GPIO_PULLUP_ONLY);

    /* Initialize SPI slave interface */
    s_spi_config.slave_config.post_setup_cb = handle_spi_setup_done;
    s_spi_config.slave_config.post_trans_cb = handle_spi_transaction_done;
    ESP_RETURN_ON_ERROR(spi_slave_initialize(s_spi_config.host_device, &s_spi_config.bus_config,
                                             &s_spi_config.slave_config, SPI_DMA_CH_AUTO),
                        OT_PLAT_LOG_TAG, "fail to initialize SPI slave");

    return ESP_OK;
}

void esp_openthread_spi_slave_deinit(void)
{
    spi_slave_free(s_spi_config.host_device);
    s_spi_config.slave_config.post_setup_cb = NULL;
    s_spi_config.slave_config.post_trans_cb = NULL;
    return;
}

otError otPlatSpiSlaveEnable(otPlatSpiSlaveTransactionCompleteCallback aCompleteCallback,
                             otPlatSpiSlaveTransactionProcessCallback aProcessCallback, void *aContext)
{
    s_process_callback = aProcessCallback;
    s_complete_callback = aCompleteCallback;
    s_context = aContext;
    return OT_ERROR_NONE;
}

otError IRAM_ATTR otPlatSpiSlavePrepareTransaction(uint8_t *aOutputBuf, uint16_t aOutputBufLen, uint8_t *aInputBuf,
                                                   uint16_t aInputBufLen, bool aRequestTransactionFlag)
{
    esp_err_t trans_state = ESP_OK;
    pending_transaction_t *pending_transaction = NULL;
    if (aOutputBuf == NULL) {
        aOutputBuf = s_prev_output_buf;
        aOutputBufLen = s_prev_output_len;
    }
    if (aInputBuf == NULL) {
        aInputBuf = s_prev_input_buf;
        aInputBufLen = s_prev_input_len;
    }
    s_prev_output_buf = aOutputBuf;
    s_prev_output_len = aOutputBufLen;
    s_prev_input_buf = aInputBuf;
    s_prev_input_len = aInputBufLen;

    s_spi_transaction.length = aOutputBufLen > aInputBufLen ? aOutputBufLen : aInputBufLen;
    s_spi_transaction.length *= CHAR_BIT;
    s_spi_transaction.rx_buffer = aInputBuf;
    s_spi_transaction.tx_buffer = aOutputBuf;

    assert(sizeof(s_spi_transaction.user) >= sizeof(pending_transaction_t));
    pending_transaction = (pending_transaction_t *)&(s_spi_transaction.user);
    pending_transaction->input_buf_len = aInputBufLen;
    pending_transaction->output_buf_len = aOutputBufLen;
    s_spi_transaction.user = pending_transaction;
    s_request_transaction = aRequestTransactionFlag;

    if ((gpio_get_level(s_spi_config.slave_config.spics_io_num) == 0)) {
        ESP_EARLY_LOGE(SPI_SLAVE_TAG, "SPI busy");
        return OT_ERROR_BUSY;
    }

    if (xPortCanYield()) {
        spi_slave_queue_reset(s_spi_config.host_device);
        trans_state = spi_slave_queue_trans(s_spi_config.host_device, &s_spi_transaction, 0);
    } else {
        spi_slave_queue_reset_isr(s_spi_config.host_device);
        trans_state = spi_slave_queue_trans_isr(s_spi_config.host_device, &s_spi_transaction);
    }

    if (trans_state == ESP_OK) {
        return OT_ERROR_NONE;
    } else {
        return OT_ERROR_FAILED;
    }
}
