// Copyright 2010-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "driver/gpio.h"
#include "driver/spi_common_internal.h"
#include "hal/spi_slave_hd_hal.h"

#include "driver/spi_slave_hd.h"


//SPI1 can never be used as the slave
#define VALID_HOST(x) (x>SPI_HOST && x<=HSPI_HOST)
#define SPIHD_CHECK(cond,warn,ret) do{if(!(cond)){ESP_LOGE(TAG, warn); return ret;}} while(0)

typedef struct {
    spi_slave_hd_hal_context_t hal;
    int dma_chan;

    intr_handle_t intr;
    intr_handle_t intr_dma;
    spi_slave_hd_callback_config_t callback;

    QueueHandle_t tx_trans_queue;
    QueueHandle_t tx_ret_queue;
    QueueHandle_t rx_trans_queue;
    QueueHandle_t rx_ret_queue;

    spi_slave_hd_data_t* tx_desc;
    spi_slave_hd_data_t* rx_desc;

    uint32_t flags;

    int max_transfer_sz;

    portMUX_TYPE int_spinlock;

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
} spi_slave_hd_slot_t;

static spi_slave_hd_slot_t *spihost[SOC_SPI_PERIPH_NUM];
static const char TAG[] = "slave_hd";


static void spi_slave_hd_intr(void* arg);

esp_err_t spi_slave_hd_init(spi_host_device_t host_id, const spi_bus_config_t *bus_config,
                            const spi_slave_hd_slot_config_t *config)
{
    bool spi_chan_claimed, dma_chan_claimed;
    esp_err_t ret = ESP_OK;

    SPIHD_CHECK(VALID_HOST(host_id), "invalid host", ESP_ERR_INVALID_ARG);
    SPIHD_CHECK(config->dma_chan == 0 || config->dma_chan == host_id, "invalid dma channel", ESP_ERR_INVALID_ARG);

    spi_chan_claimed = spicommon_periph_claim(host_id, "slave_hd");
    SPIHD_CHECK(spi_chan_claimed, "host already in use", ESP_ERR_INVALID_STATE);

    if ( config->dma_chan != 0 ) {
        dma_chan_claimed = spicommon_dma_chan_claim(config->dma_chan);
        if (!dma_chan_claimed) {
            spicommon_periph_free(host_id);
            SPIHD_CHECK(dma_chan_claimed, "dma channel already in use", ESP_ERR_INVALID_STATE);
        }

        spicommon_connect_spi_and_dma(host_id, config->dma_chan);
    }

    spi_slave_hd_slot_t* host = malloc(sizeof(spi_slave_hd_slot_t));
    if (host == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }
    spihost[host_id] = host;
    memset(host, 0, sizeof(spi_slave_hd_slot_t));

    host->dma_chan = config->dma_chan;
    host->int_spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;

    ret = spicommon_bus_initialize_io(host_id, bus_config, config->dma_chan,
                SPICOMMON_BUSFLAG_SLAVE | bus_config->flags, &host->flags);
    if (ret != ESP_OK) {
        goto cleanup;
    }
    gpio_set_direction(config->spics_io_num, GPIO_MODE_INPUT);
    spicommon_cs_initialize(host_id, config->spics_io_num, 0,
            !(bus_config->flags & SPICOMMON_BUSFLAG_NATIVE_PINS));
    host->dma_chan = config->dma_chan;

    spi_slave_hd_hal_config_t hal_config = {
        .host_id = host_id,
        .dma_in = SPI_LL_GET_HW(host_id),
        .dma_out = SPI_LL_GET_HW(host_id),
        .tx_lsbfirst = (config->flags & SPI_SLAVE_HD_RXBIT_LSBFIRST),
        .rx_lsbfirst = (config->flags & SPI_SLAVE_HD_TXBIT_LSBFIRST),
        .dma_chan = config->dma_chan,
        .mode = config->mode
    };
    spi_slave_hd_hal_init(&host->hal, &hal_config);

    if (config->dma_chan != 0) {
        //See how many dma descriptors we need and allocate them
        int dma_desc_ct = (bus_config->max_transfer_sz + SPI_MAX_DMA_LEN - 1) / SPI_MAX_DMA_LEN;
        if (dma_desc_ct == 0) dma_desc_ct = 1; //default to 4k when max is not given
        host->max_transfer_sz = dma_desc_ct * SPI_MAX_DMA_LEN;
        host->hal.dmadesc_tx = heap_caps_malloc(sizeof(lldesc_t) * dma_desc_ct, MALLOC_CAP_DMA);
        host->hal.dmadesc_rx = heap_caps_malloc(sizeof(lldesc_t) * dma_desc_ct, MALLOC_CAP_DMA);

        if (!host->hal.dmadesc_tx || !host->hal.dmadesc_rx ) {
            ret = ESP_ERR_NO_MEM;
            goto cleanup;
        }
    } else {
        //We're limited to non-DMA transfers: the SPI work registers can hold 64 bytes at most.
        host->max_transfer_sz = 0;
    }
#ifdef CONFIG_PM_ENABLE
    ret = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "spi_slave", &host->pm_lock);
    if (ret != ESP_OK) {
        goto cleanup;
    }
    // Lock APB frequency while SPI slave driver is in use
    esp_pm_lock_acquire(host->pm_lock);
#endif //CONFIG_PM_ENABLE

    //Create queues
    host->tx_trans_queue = xQueueCreate(config->queue_size, sizeof(spi_slave_hd_data_t *));
    host->tx_ret_queue = xQueueCreate(config->queue_size, sizeof(spi_slave_hd_data_t *));
    host->rx_trans_queue = xQueueCreate(config->queue_size, sizeof(spi_slave_hd_data_t *));
    host->rx_ret_queue = xQueueCreate(config->queue_size, sizeof(spi_slave_hd_data_t *));
    if (!host->tx_trans_queue || !host->tx_ret_queue ||
        !host->rx_trans_queue || !host->rx_ret_queue) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    ret = esp_intr_alloc(spicommon_irqsource_for_host(host_id), 0, spi_slave_hd_intr,
                         (void *)host, &host->intr);
    if (ret != ESP_OK) {
        goto cleanup;
    }

    ret = esp_intr_alloc(spicommon_irqdma_source_for_host(host_id), 0, spi_slave_hd_intr,
                         (void *)host, &host->intr_dma);
    if (ret != ESP_OK) {
        goto cleanup;
    }
    memcpy((uint8_t*)&host->callback, (uint8_t*)&config->cb_config, sizeof(spi_slave_hd_callback_config_t));

    spi_event_t event = 0;
    if (host->callback.cb_buffer_tx!=NULL) event |= SPI_EV_BUF_TX;
    if (host->callback.cb_buffer_rx!=NULL) event |= SPI_EV_BUF_RX;
    if (host->callback.cb_cmd9!=NULL) event |= SPI_EV_CMD9;
    if (host->callback.cb_cmdA!=NULL) event |= SPI_EV_CMDA;
    spi_slave_hd_hal_enable_event_intr(&host->hal, event);

    return ESP_OK;

cleanup:
    // Memory free is in the deinit function
    spi_slave_hd_deinit(host_id);
    return ret;
}

esp_err_t spi_slave_hd_deinit(spi_host_device_t host_id)
{
    spi_slave_hd_slot_t *host = spihost[host_id];
    if (host == NULL) return ESP_ERR_INVALID_ARG;

    if (host->tx_trans_queue) vQueueDelete(host->tx_trans_queue);
    if (host->tx_ret_queue) vQueueDelete(host->tx_ret_queue);
    if (host->rx_trans_queue) vQueueDelete(host->rx_trans_queue);
    if (host->rx_ret_queue) vQueueDelete(host->rx_ret_queue);
    if (host) {
        free(host->hal.dmadesc_tx);
        free(host->hal.dmadesc_rx);
        esp_intr_free(host->intr);
        esp_intr_free(host->intr_dma);
#ifdef CONFIG_PM_ENABLE
        if (host->pm_lock) {
            esp_pm_lock_release(host->pm_lock);
            esp_pm_lock_delete(host->pm_lock);
        }
#endif
    }

    spicommon_periph_free(host_id);
    if (host->dma_chan) {
        spicommon_dma_chan_free(host->dma_chan);
    }
    free(host);
    spihost[host_id] = NULL;
    return ESP_OK;
}

static void tx_invoke(spi_slave_hd_slot_t* host)
{
    portENTER_CRITICAL(&host->int_spinlock);
    spi_slave_hd_hal_invoke_event_intr(&host->hal, SPI_EV_SEND);
    portEXIT_CRITICAL(&host->int_spinlock);
}

static void rx_invoke(spi_slave_hd_slot_t* host)
{
    portENTER_CRITICAL(&host->int_spinlock);
    spi_slave_hd_hal_invoke_event_intr(&host->hal, SPI_EV_RECV);
    portEXIT_CRITICAL(&host->int_spinlock);
}

static inline IRAM_ATTR BaseType_t intr_check_clear_callback(spi_slave_hd_slot_t* host, spi_event_t ev, slave_cb_t cb)
{
    BaseType_t cb_awoken = pdFALSE;
    if (spi_slave_hd_hal_check_clear_event(&host->hal, ev) && cb) {
        spi_slave_hd_event_t event = {.event = ev};
        cb(host->callback.arg, &event, &cb_awoken);
    }
    return cb_awoken;
}

static IRAM_ATTR void spi_slave_hd_intr(void* arg)
{
    spi_slave_hd_slot_t* host = (spi_slave_hd_slot_t*)arg;
    BaseType_t awoken = pdFALSE;
    spi_slave_hd_callback_config_t *callback = &host->callback;
    ESP_EARLY_LOGV("spi_hd", "intr.");

    awoken |= intr_check_clear_callback(host, SPI_EV_BUF_TX, callback->cb_buffer_tx);
    awoken |= intr_check_clear_callback(host, SPI_EV_BUF_RX, callback->cb_buffer_rx);
    awoken |= intr_check_clear_callback(host, SPI_EV_CMD9,   callback->cb_cmd9);
    awoken |= intr_check_clear_callback(host, SPI_EV_CMDA,   callback->cb_cmdA);

    BaseType_t ret;
    bool tx_done = false;
    bool rx_done = false;

    portENTER_CRITICAL_ISR(&host->int_spinlock);
    if (host->tx_desc && spi_slave_hd_hal_check_disable_event(&host->hal, SPI_EV_SEND)) {
        tx_done = true;
    }
    if (host->rx_desc && spi_slave_hd_hal_check_disable_event(&host->hal, SPI_EV_RECV)) {
        rx_done = true;
    }
    portEXIT_CRITICAL_ISR(&host->int_spinlock);

    if (tx_done) {
        bool ret_queue = true;
        if (callback->cb_sent) {
            spi_slave_hd_event_t ev = {
                .event = SPI_EV_SEND,
                .trans = host->tx_desc,
            };
            BaseType_t cb_awoken = pdFALSE;
            ret_queue = callback->cb_sent(callback->arg, &ev, &cb_awoken);
            awoken |= cb_awoken;
        }
        if (ret_queue) {
            ret = xQueueSendFromISR(host->tx_ret_queue, &host->tx_desc, &awoken);
            // The return queue is full. All the data remian in send_queue + ret_queue should not be more than the queue length.
            assert(ret == pdTRUE);
        }
        host->tx_desc = NULL;
    }
    if (rx_done) {
        bool ret_queue = true;

        host->rx_desc->trans_len = spi_slave_hd_hal_rxdma_get_len(&host->hal);

        if (callback->cb_recv) {
            spi_slave_hd_event_t ev = {
                .event = SPI_EV_RECV,
                .trans = host->rx_desc,
            };
            BaseType_t cb_awoken = pdFALSE;
            ret_queue = callback->cb_recv(callback->arg, &ev, &cb_awoken);
            awoken |= cb_awoken;
        }
        if (ret_queue) {
            ret = xQueueSendFromISR(host->rx_ret_queue, &host->rx_desc, &awoken);
            // The return queue is full. All the data remian in send_queue + ret_queue should not be more than the queue length.
            assert(ret == pdTRUE);
        }
        host->rx_desc = NULL;
    }

    bool tx_sent = false;
    bool rx_sent = false;
    if (!host->tx_desc) {
        ret = xQueueReceiveFromISR(host->tx_trans_queue, &host->tx_desc, &awoken);
        if (ret == pdTRUE) {
            spi_slave_hd_hal_txdma(&host->hal, host->tx_desc->data, host->tx_desc->len);
            tx_sent = true;
        }
    }
    if (!host->rx_desc) {
        ret = xQueueReceiveFromISR(host->rx_trans_queue, &host->rx_desc, &awoken);
        if (ret == pdTRUE) {
            spi_slave_hd_hal_rxdma(&host->hal, host->rx_desc->data, host->rx_desc->len);
            rx_sent = true;
        }
    }

    portENTER_CRITICAL_ISR(&host->int_spinlock);
    if (rx_sent) {
        spi_slave_hd_hal_enable_event_intr(&host->hal, SPI_EV_RECV);
    }
    if (tx_sent) {
        spi_slave_hd_hal_enable_event_intr(&host->hal, SPI_EV_SEND);
    }
    portEXIT_CRITICAL_ISR(&host->int_spinlock);

    if (awoken==pdTRUE) portYIELD_FROM_ISR();
}

esp_err_t spi_slave_hd_queue_trans(spi_host_device_t host_id, spi_slave_chan_t chan, spi_slave_hd_data_t* trans, TickType_t timeout)
{
    spi_slave_hd_slot_t* host = spihost[host_id];
    SPIHD_CHECK(esp_ptr_dma_capable(trans->data), "The buffer should be DMA capable.", ESP_ERR_INVALID_ARG);
    SPIHD_CHECK(trans->len <= host->max_transfer_sz && trans->len > 0, "Invalid buffer size", ESP_ERR_INVALID_ARG);
    SPIHD_CHECK(chan == SPI_SLAVE_CHAN_TX || chan == SPI_SLAVE_CHAN_RX, "Invalid channel", ESP_ERR_INVALID_ARG);

    if (chan == SPI_SLAVE_CHAN_TX) {
        BaseType_t ret = xQueueSend(host->tx_trans_queue, &trans, timeout);
        if (ret == pdFALSE) {
            return ESP_ERR_TIMEOUT;
        }
        tx_invoke(host);
    } else { //chan == SPI_SLAVE_CHAN_RX
        BaseType_t ret = xQueueSend(host->rx_trans_queue, &trans, timeout);
        if (ret == pdFALSE) {
            return ESP_ERR_TIMEOUT;
        }
        rx_invoke(host);
    }
    return ESP_OK;
}

esp_err_t spi_slave_hd_get_trans_res(spi_host_device_t host_id, spi_slave_chan_t chan, spi_slave_hd_data_t** out_trans, TickType_t timeout)
{
    SPIHD_CHECK(chan == SPI_SLAVE_CHAN_TX || chan == SPI_SLAVE_CHAN_RX, "Invalid channel", ESP_ERR_INVALID_ARG);

    spi_slave_hd_slot_t* host = spihost[host_id];
    BaseType_t ret;
    spi_slave_hd_data_t *data;
    if (chan == SPI_SLAVE_CHAN_TX) {
        ret = xQueueReceive(host->tx_ret_queue, &data, timeout);
    } else { // chan == SPI_SLAVE_CHAN_RX
        ret = xQueueReceive(host->rx_ret_queue, &data, timeout);
    }

    if (ret == pdFALSE) {
        return ESP_ERR_TIMEOUT;
    }
    *out_trans = data;
    return ESP_OK;
}

void spi_slave_hd_read_buffer(spi_host_device_t host_id, int addr, uint8_t *out_data, size_t len)
{
    spi_slave_hd_hal_read_buffer(&spihost[host_id]->hal, addr, out_data, len);
}

void spi_slave_hd_write_buffer(spi_host_device_t host_id, int addr, uint8_t *data, size_t len)
{
    spi_slave_hd_hal_write_buffer(&spihost[host_id]->hal, addr, data, len);
}
