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
#include "esp_heap_caps.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_task_queue.h"
#include "esp_openthread_types.h"
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
static uint8_t *s_output_buf;
static uint16_t s_output_len;
static uint8_t *s_input_buf;
static uint16_t s_input_len;
static bool s_request_transaction = false;
typedef struct {
    uint16_t output_buf_len;
    uint16_t input_buf_len;
} pending_transaction_t;

static otPlatSpiSlaveTransactionProcessCallback s_process_callback = NULL;
static otPlatSpiSlaveTransactionCompleteCallback s_complete_callback = NULL;

static DRAM_ATTR esp_openthread_spi_slave_config_t *s_spi_config;
static DRAM_ATTR spi_slave_transaction_t *s_spi_transaction;
static DRAM_ATTR pending_transaction_t *s_pending_transaction;

static void IRAM_ATTR handle_spi_setup_done(spi_slave_transaction_t *trans)
{
    if (s_request_transaction) {
        gpio_set_level(s_spi_config->intr_pin, 0);
    }
}

static void IRAM_ATTR handle_spi_transaction_done(spi_slave_transaction_t *trans)
{
    gpio_set_level(s_spi_config->intr_pin, 1);
    pending_transaction_t *pending_transaction = (pending_transaction_t *)(trans->user);
    trans->trans_len /= CHAR_BIT;

    if (s_complete_callback &&
        s_complete_callback(s_context, (void*)trans->tx_buffer, pending_transaction->output_buf_len,
                            trans->rx_buffer, pending_transaction->input_buf_len, trans->trans_len)) {
        esp_openthread_task_queue_post(s_process_callback, s_context);
    }
    trans = NULL;
}

esp_err_t esp_openthread_host_rcp_spi_init(const esp_openthread_platform_config_t *config)
{
    s_spi_config = heap_caps_malloc(sizeof(esp_openthread_spi_slave_config_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(s_spi_config != NULL, ESP_ERR_NO_MEM, OT_PLAT_LOG_TAG,
                        "failed to allocate memory for SPI transaction on internal heap");
    memcpy(s_spi_config, &(config->host_config.spi_slave_config), sizeof(esp_openthread_spi_slave_config_t));
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1 << s_spi_config->intr_pin),
    };
    ESP_RETURN_ON_ERROR(gpio_config(&io_conf), OT_PLAT_LOG_TAG, "fail to configure SPI gpio");

    gpio_set_pull_mode(s_spi_config->bus_config.mosi_io_num, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(s_spi_config->bus_config.sclk_io_num, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(s_spi_config->slave_config.spics_io_num, GPIO_PULLUP_ONLY);

    s_spi_transaction = heap_caps_malloc(sizeof(spi_slave_transaction_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    s_pending_transaction = heap_caps_malloc(sizeof(pending_transaction_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (s_spi_transaction == NULL || s_pending_transaction == NULL) {
        heap_caps_free(s_spi_config);
        heap_caps_free(s_spi_transaction);
        heap_caps_free(s_pending_transaction);
        ESP_LOGE(OT_PLAT_LOG_TAG, "failed to allocate memory for SPI transaction on internal heap");
        return ESP_ERR_NO_MEM;
    }

    s_spi_transaction->user = (void *)s_pending_transaction;

    /* Initialize SPI slave interface */
    s_spi_config->slave_config.post_setup_cb = handle_spi_setup_done;
    s_spi_config->slave_config.post_trans_cb = handle_spi_transaction_done;
    ESP_RETURN_ON_ERROR(spi_slave_initialize(s_spi_config->host_device, &s_spi_config->bus_config,
                                             &s_spi_config->slave_config, SPI_DMA_CH_AUTO),
                        OT_PLAT_LOG_TAG, "fail to initialize SPI slave");

    return ESP_OK;
}

void esp_openthread_spi_slave_deinit(void)
{
    spi_slave_free(s_spi_config->host_device);
    s_spi_config->slave_config.post_setup_cb = NULL;
    s_spi_config->slave_config.post_trans_cb = NULL;
    heap_caps_free(s_spi_config);
    heap_caps_free(s_spi_transaction);
    heap_caps_free(s_pending_transaction);
    s_spi_config = NULL;
    s_spi_transaction = NULL;
    s_pending_transaction = NULL;
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
    uint16_t trans_length = 0;

    if (aOutputBuf != NULL) {
        s_output_buf = aOutputBuf;
        s_output_len = aOutputBufLen;
    }
    if (aInputBuf != NULL) {
        s_input_buf = aInputBuf;
        s_input_len = aInputBufLen;
    }

    trans_length = s_output_len > s_input_len ? s_output_len : s_input_len;
    trans_length *= CHAR_BIT;
    if ((gpio_get_level(s_spi_config->slave_config.spics_io_num) == 0)) {
        ESP_EARLY_LOGE(SPI_SLAVE_TAG, "SPI busy");
        return OT_ERROR_BUSY;
    }
    s_spi_transaction->length = trans_length;
    s_spi_transaction->rx_buffer = s_input_buf;
    s_spi_transaction->tx_buffer = s_output_buf;

    pending_transaction_t *pending_transaction = (pending_transaction_t *)s_spi_transaction->user;
    pending_transaction->input_buf_len = s_input_len;
    pending_transaction->output_buf_len = s_output_len;
    s_request_transaction = aRequestTransactionFlag;

    if (xPortCanYield()) {
        spi_slave_queue_reset(s_spi_config->host_device);
        trans_state = spi_slave_queue_trans(s_spi_config->host_device, s_spi_transaction, 0);
    } else {
        spi_slave_queue_reset_isr(s_spi_config->host_device);
        trans_state = spi_slave_queue_trans_isr(s_spi_config->host_device, s_spi_transaction);
    }

    if (trans_state == ESP_OK) {
        return OT_ERROR_NONE;
    } else {
        return OT_ERROR_FAILED;
    }
}
