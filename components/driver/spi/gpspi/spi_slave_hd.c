/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "esp_memory_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"
#include "driver/gpio.h"
#include "esp_private/spi_common_internal.h"
#include "driver/spi_slave_hd.h"
#include "hal/spi_slave_hd_hal.h"


#if (SOC_SPI_PERIPH_NUM == 2)
#define VALID_HOST(x) ((x) == SPI2_HOST)
#elif (SOC_SPI_PERIPH_NUM == 3)
#define VALID_HOST(x) ((x) >= SPI2_HOST && (x) <= SPI3_HOST)
#endif
#define SPIHD_CHECK(cond,warn,ret) do{if(!(cond)){ESP_LOGE(TAG, warn); return ret;}} while(0)

typedef struct {
    bool dma_enabled;
    int max_transfer_sz;
    uint32_t flags;
    portMUX_TYPE int_spinlock;
    intr_handle_t intr;
    intr_handle_t intr_dma;
    spi_slave_hd_callback_config_t callback;
    spi_slave_hd_hal_context_t hal;
    bool append_mode;

    QueueHandle_t tx_trans_queue;
    QueueHandle_t tx_ret_queue;
    QueueHandle_t rx_trans_queue;
    QueueHandle_t rx_ret_queue;
    QueueHandle_t tx_cnting_sem;
    QueueHandle_t rx_cnting_sem;

    spi_slave_hd_data_t* tx_desc;
    spi_slave_hd_data_t* rx_desc;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
} spi_slave_hd_slot_t;

static spi_slave_hd_slot_t *spihost[SOC_SPI_PERIPH_NUM];
static const char TAG[] = "slave_hd";

static void spi_slave_hd_intr_segment(void *arg);
#if CONFIG_IDF_TARGET_ESP32S2
//Append mode is only supported on ESP32S2 now
static void spi_slave_hd_intr_append(void *arg);
#endif

esp_err_t spi_slave_hd_init(spi_host_device_t host_id, const spi_bus_config_t *bus_config,
                            const spi_slave_hd_slot_config_t *config)
{
    bool spi_chan_claimed;
    bool append_mode = (config->flags & SPI_SLAVE_HD_APPEND_MODE);
    uint32_t actual_tx_dma_chan = 0;
    uint32_t actual_rx_dma_chan = 0;
    esp_err_t ret = ESP_OK;

    SPIHD_CHECK(VALID_HOST(host_id), "invalid host", ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32S2
    SPIHD_CHECK(config->dma_chan == SPI_DMA_DISABLED || config->dma_chan == (int)host_id || config->dma_chan == SPI_DMA_CH_AUTO, "invalid dma channel", ESP_ERR_INVALID_ARG);
#elif SOC_GDMA_SUPPORTED
    SPIHD_CHECK(config->dma_chan == SPI_DMA_DISABLED || config->dma_chan == SPI_DMA_CH_AUTO, "invalid dma channel, chip only support spi dma channel auto-alloc", ESP_ERR_INVALID_ARG);
#endif
#if !CONFIG_IDF_TARGET_ESP32S2
//Append mode is only supported on ESP32S2 now
    SPIHD_CHECK(append_mode == 0, "Append mode is only supported on ESP32S2 now", ESP_ERR_INVALID_ARG);
#endif

    spi_chan_claimed = spicommon_periph_claim(host_id, "slave_hd");
    SPIHD_CHECK(spi_chan_claimed, "host already in use", ESP_ERR_INVALID_STATE);

    spi_slave_hd_slot_t* host = calloc(1, sizeof(spi_slave_hd_slot_t));
    if (host == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }
    spihost[host_id] = host;
    host->int_spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    host->dma_enabled = (config->dma_chan != SPI_DMA_DISABLED);

    if (host->dma_enabled) {
        ret = spicommon_dma_chan_alloc(host_id, config->dma_chan, &actual_tx_dma_chan, &actual_rx_dma_chan);
        if (ret != ESP_OK) {
            goto cleanup;
        }
    }

    ret = spicommon_bus_initialize_io(host_id, bus_config, SPICOMMON_BUSFLAG_SLAVE | bus_config->flags, &host->flags);
    if (ret != ESP_OK) {
        goto cleanup;
    }
    gpio_set_direction(config->spics_io_num, GPIO_MODE_INPUT);
    spicommon_cs_initialize(host_id, config->spics_io_num, 0,
            !(bus_config->flags & SPICOMMON_BUSFLAG_NATIVE_PINS));
    host->append_mode = append_mode;

    spi_slave_hd_hal_config_t hal_config = {
        .host_id = host_id,
        .dma_in = SPI_LL_GET_HW(host_id),
        .dma_out = SPI_LL_GET_HW(host_id),
        .dma_enabled = host->dma_enabled,
        .tx_dma_chan = actual_tx_dma_chan,
        .rx_dma_chan = actual_rx_dma_chan,
        .append_mode = append_mode,
        .mode = config->mode,
        .tx_lsbfirst = (config->flags & SPI_SLAVE_HD_RXBIT_LSBFIRST),
        .rx_lsbfirst = (config->flags & SPI_SLAVE_HD_TXBIT_LSBFIRST),
    };

    if (host->dma_enabled) {
        //Malloc for all the DMA descriptors
        uint32_t total_desc_size = spi_slave_hd_hal_get_total_desc_size(&host->hal, bus_config->max_transfer_sz);
        host->hal.dmadesc_tx = heap_caps_malloc(total_desc_size, MALLOC_CAP_DMA);
        host->hal.dmadesc_rx = heap_caps_malloc(total_desc_size, MALLOC_CAP_DMA);
        if (!host->hal.dmadesc_tx || !host->hal.dmadesc_rx) {
            ret = ESP_ERR_NO_MEM;
            goto cleanup;
        }

        //Get the actual SPI bus transaction size in bytes.
        host->max_transfer_sz = spi_salve_hd_hal_get_max_bus_size(&host->hal);
    } else {
        //We're limited to non-DMA transfers: the SPI work registers can hold 64 bytes at most.
        host->max_transfer_sz = 0;
    }

    //Init the hal according to the hal_config set above
    spi_slave_hd_hal_init(&host->hal, &hal_config);

#ifdef CONFIG_PM_ENABLE
    ret = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "spi_slave", &host->pm_lock);
    if (ret != ESP_OK) {
        goto cleanup;
    }
    // Lock APB frequency while SPI slave driver is in use
    esp_pm_lock_acquire(host->pm_lock);
#endif //CONFIG_PM_ENABLE

    //Create Queues and Semaphores
    host->tx_ret_queue = xQueueCreate(config->queue_size, sizeof(spi_slave_hd_data_t *));
    host->rx_ret_queue = xQueueCreate(config->queue_size, sizeof(spi_slave_hd_data_t *));
    if (!host->append_mode) {
        host->tx_trans_queue = xQueueCreate(config->queue_size, sizeof(spi_slave_hd_data_t *));
        host->rx_trans_queue = xQueueCreate(config->queue_size, sizeof(spi_slave_hd_data_t *));
        if (!host->tx_trans_queue || !host->rx_trans_queue) {
            ret = ESP_ERR_NO_MEM;
            goto cleanup;
        }
    }
#if CONFIG_IDF_TARGET_ESP32S2
//Append mode is only supported on ESP32S2 now
    else {
        host->tx_cnting_sem = xSemaphoreCreateCounting(config->queue_size, config->queue_size);
        host->rx_cnting_sem = xSemaphoreCreateCounting(config->queue_size, config->queue_size);
        if (!host->tx_cnting_sem || !host->rx_cnting_sem) {
            ret = ESP_ERR_NO_MEM;
            goto cleanup;
        }
    }
#endif  //#if CONFIG_IDF_TARGET_ESP32S2

    //Alloc intr
    if (!host->append_mode) {
        ret = esp_intr_alloc(spicommon_irqsource_for_host(host_id), 0, spi_slave_hd_intr_segment,
                         (void *)host, &host->intr);
        if (ret != ESP_OK) {
            goto cleanup;
        }
        ret = esp_intr_alloc(spicommon_irqdma_source_for_host(host_id), 0, spi_slave_hd_intr_segment,
                            (void *)host, &host->intr_dma);
        if (ret != ESP_OK) {
            goto cleanup;
        }
    }
#if CONFIG_IDF_TARGET_ESP32S2
//Append mode is only supported on ESP32S2 now
    else {
        ret = esp_intr_alloc(spicommon_irqsource_for_host(host_id), 0, spi_slave_hd_intr_append,
                         (void *)host, &host->intr);
        if (ret != ESP_OK) {
            goto cleanup;
        }
        ret = esp_intr_alloc(spicommon_irqdma_source_for_host(host_id), 0, spi_slave_hd_intr_append,
                            (void *)host, &host->intr_dma);
        if (ret != ESP_OK) {
            goto cleanup;
        }
    }
#endif  //#if CONFIG_IDF_TARGET_ESP32S2

    //Init callbacks
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
    if (host->tx_cnting_sem) vSemaphoreDelete(host->tx_cnting_sem);
    if (host->rx_cnting_sem) vSemaphoreDelete(host->rx_cnting_sem);
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
    if (host->dma_enabled) {
        spicommon_dma_chan_free(host_id);
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

static IRAM_ATTR void spi_slave_hd_intr_segment(void *arg)
{
    spi_slave_hd_slot_t *host = (spi_slave_hd_slot_t*)arg;
    spi_slave_hd_callback_config_t *callback = &host->callback;
    spi_slave_hd_hal_context_t *hal = &host->hal;
    BaseType_t awoken = pdFALSE;
    BaseType_t ret;

    awoken |= intr_check_clear_callback(host, SPI_EV_BUF_TX, callback->cb_buffer_tx);
    awoken |= intr_check_clear_callback(host, SPI_EV_BUF_RX, callback->cb_buffer_rx);
    awoken |= intr_check_clear_callback(host, SPI_EV_CMD9,   callback->cb_cmd9);
    awoken |= intr_check_clear_callback(host, SPI_EV_CMDA,   callback->cb_cmdA);

    bool tx_done = false;
    bool rx_done = false;

    portENTER_CRITICAL_ISR(&host->int_spinlock);
    if (host->tx_desc && spi_slave_hd_hal_check_disable_event(hal, SPI_EV_SEND)) {
        tx_done = true;
    }
    if (host->rx_desc && spi_slave_hd_hal_check_disable_event(hal, SPI_EV_RECV)) {
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
        host->rx_desc->trans_len = spi_slave_hd_hal_rxdma_seg_get_len(hal);
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
            spi_slave_hd_hal_txdma(hal, host->tx_desc->data, host->tx_desc->len);
            tx_sent = true;
            if (callback->cb_send_dma_ready) {
                spi_slave_hd_event_t ev = {
                    .event = SPI_EV_SEND_DMA_READY,
                    .trans = host->tx_desc,
                };
                BaseType_t cb_awoken = pdFALSE;
                callback->cb_send_dma_ready(callback->arg, &ev, &cb_awoken);
                awoken |= cb_awoken;
            }
        }
    }
    if (!host->rx_desc) {
        ret = xQueueReceiveFromISR(host->rx_trans_queue, &host->rx_desc, &awoken);
        if (ret == pdTRUE) {
            spi_slave_hd_hal_rxdma(hal, host->rx_desc->data, host->rx_desc->len);
            rx_sent = true;
            if (callback->cb_recv_dma_ready) {
                spi_slave_hd_event_t ev = {
                    .event = SPI_EV_RECV_DMA_READY,
                    .trans = host->rx_desc,
                };
                BaseType_t cb_awoken = pdFALSE;
                callback->cb_recv_dma_ready(callback->arg, &ev, &cb_awoken);
                awoken |= cb_awoken;
            }
        }
    }

    portENTER_CRITICAL_ISR(&host->int_spinlock);
    if (tx_sent) {
        spi_slave_hd_hal_enable_event_intr(hal, SPI_EV_SEND);
    }
    if (rx_sent) {
        spi_slave_hd_hal_enable_event_intr(hal, SPI_EV_RECV);
    }
    portEXIT_CRITICAL_ISR(&host->int_spinlock);

    if (awoken==pdTRUE) portYIELD_FROM_ISR();
}

#if CONFIG_IDF_TARGET_ESP32S2
//Append mode is only supported on ESP32S2 now
static IRAM_ATTR void spi_slave_hd_intr_append(void *arg)
{
    spi_slave_hd_slot_t *host = (spi_slave_hd_slot_t*)arg;
    spi_slave_hd_callback_config_t *callback = &host->callback;
    spi_slave_hd_hal_context_t *hal = &host->hal;
    BaseType_t awoken = pdFALSE;
    BaseType_t ret __attribute__((unused));

    bool tx_done = false;
    bool rx_done = false;
    portENTER_CRITICAL_ISR(&host->int_spinlock);
    if (spi_slave_hd_hal_check_clear_event(hal, SPI_EV_SEND)) {
        tx_done = true;
    }
    if (spi_slave_hd_hal_check_clear_event(hal, SPI_EV_RECV)) {
        rx_done = true;
    }
    portEXIT_CRITICAL_ISR(&host->int_spinlock);

    if (tx_done) {
        spi_slave_hd_data_t *trans_desc;
        while (1) {
            bool trans_finish = false;
            trans_finish = spi_slave_hd_hal_get_tx_finished_trans(hal, (void **)&trans_desc);
            if (!trans_finish) {
                break;
            }

            bool ret_queue = true;
            if (callback->cb_sent) {
                spi_slave_hd_event_t ev = {
                    .event = SPI_EV_SEND,
                    .trans = trans_desc,
                };
                BaseType_t cb_awoken = pdFALSE;
                ret_queue = callback->cb_sent(callback->arg, &ev, &cb_awoken);
                awoken |= cb_awoken;
            }

            if (ret_queue) {
                ret = xQueueSendFromISR(host->tx_ret_queue, &trans_desc, &awoken);
                assert(ret == pdTRUE);

                ret = xSemaphoreGiveFromISR(host->tx_cnting_sem, &awoken);
                assert(ret == pdTRUE);
            }
        }
    }

    if (rx_done) {
        spi_slave_hd_data_t *trans_desc;
        size_t trans_len;
        while (1) {
            bool trans_finish = false;
            trans_finish = spi_slave_hd_hal_get_rx_finished_trans(hal, (void **)&trans_desc, &trans_len);
            if (!trans_finish) {
                break;
            }
            trans_desc->trans_len = trans_len;

            bool ret_queue = true;
            if (callback->cb_recv) {
                spi_slave_hd_event_t ev = {
                    .event = SPI_EV_RECV,
                    .trans = trans_desc,
                };
                BaseType_t cb_awoken = pdFALSE;
                ret_queue = callback->cb_recv(callback->arg, &ev, &cb_awoken);
                awoken |= cb_awoken;
            }

            if (ret_queue) {
                ret = xQueueSendFromISR(host->rx_ret_queue, &trans_desc, &awoken);
                assert(ret == pdTRUE);

                ret = xSemaphoreGiveFromISR(host->rx_cnting_sem, &awoken);
                assert(ret == pdTRUE);
            }
        }
    }

    if (awoken==pdTRUE) portYIELD_FROM_ISR();
}
#endif //#if CONFIG_IDF_TARGET_ESP32S2

static esp_err_t get_ret_queue_result(spi_host_device_t host_id, spi_slave_chan_t chan, spi_slave_hd_data_t **out_trans, TickType_t timeout)
{
    spi_slave_hd_slot_t *host = spihost[host_id];
    spi_slave_hd_data_t *trans;
    BaseType_t ret;

    if (chan == SPI_SLAVE_CHAN_TX) {
        ret = xQueueReceive(host->tx_ret_queue, &trans, timeout);
    } else {
        ret = xQueueReceive(host->rx_ret_queue, &trans, timeout);
    }
    if (ret == pdFALSE) {
        return ESP_ERR_TIMEOUT;
    }

    *out_trans = trans;
    return ESP_OK;
}

//---------------------------------------------------------Segment Mode Transaction APIs-----------------------------------------------------------//
esp_err_t spi_slave_hd_queue_trans(spi_host_device_t host_id, spi_slave_chan_t chan, spi_slave_hd_data_t* trans, TickType_t timeout)
{
    spi_slave_hd_slot_t* host = spihost[host_id];

    SPIHD_CHECK(host->append_mode == 0, "This API should be used for SPI Slave HD Segment Mode", ESP_ERR_INVALID_STATE);
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
    esp_err_t ret;
    spi_slave_hd_slot_t* host = spihost[host_id];

    SPIHD_CHECK(host->append_mode == 0, "This API should be used for SPI Slave HD Segment Mode", ESP_ERR_INVALID_STATE);
    SPIHD_CHECK(chan == SPI_SLAVE_CHAN_TX || chan == SPI_SLAVE_CHAN_RX, "Invalid channel", ESP_ERR_INVALID_ARG);
    ret = get_ret_queue_result(host_id, chan, out_trans, timeout);

    return ret;
}

void spi_slave_hd_read_buffer(spi_host_device_t host_id, int addr, uint8_t *out_data, size_t len)
{
    spi_slave_hd_hal_read_buffer(&spihost[host_id]->hal, addr, out_data, len);
}

void spi_slave_hd_write_buffer(spi_host_device_t host_id, int addr, uint8_t *data, size_t len)
{
    spi_slave_hd_hal_write_buffer(&spihost[host_id]->hal, addr, data, len);
}

#if CONFIG_IDF_TARGET_ESP32S2
//Append mode is only supported on ESP32S2 now
//---------------------------------------------------------Append Mode Transaction APIs-----------------------------------------------------------//
esp_err_t spi_slave_hd_append_trans(spi_host_device_t host_id, spi_slave_chan_t chan, spi_slave_hd_data_t *trans, TickType_t timeout)
{
    esp_err_t err;
    spi_slave_hd_slot_t *host = spihost[host_id];
    spi_slave_hd_hal_context_t *hal = &host->hal;

    SPIHD_CHECK(trans->len <= SPI_MAX_DMA_LEN, "Currently we only support transaction with data length within 4092 bytes", ESP_ERR_INVALID_ARG);
    SPIHD_CHECK(host->append_mode == 1, "This API should be used for SPI Slave HD Append Mode", ESP_ERR_INVALID_STATE);
    SPIHD_CHECK(esp_ptr_dma_capable(trans->data), "The buffer should be DMA capable.", ESP_ERR_INVALID_ARG);
    SPIHD_CHECK(trans->len <= host->max_transfer_sz && trans->len > 0, "Invalid buffer size", ESP_ERR_INVALID_ARG);
    SPIHD_CHECK(chan == SPI_SLAVE_CHAN_TX || chan == SPI_SLAVE_CHAN_RX, "Invalid channel", ESP_ERR_INVALID_ARG);

    if (chan == SPI_SLAVE_CHAN_TX) {
        BaseType_t ret = xSemaphoreTake(host->tx_cnting_sem, timeout);
        if (ret == pdFALSE) {
            return ESP_ERR_TIMEOUT;
        }
        err = spi_slave_hd_hal_txdma_append(hal, trans->data, trans->len, trans);
    } else {
        BaseType_t ret = xSemaphoreTake(host->rx_cnting_sem, timeout);
        if (ret == pdFALSE) {
            return ESP_ERR_TIMEOUT;
        }
        err = spi_slave_hd_hal_rxdma_append(hal, trans->data, trans->len, trans);
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Wait until the DMA finishes its transaction");
    }

    return err;
}

esp_err_t spi_slave_hd_get_append_trans_res(spi_host_device_t host_id, spi_slave_chan_t chan, spi_slave_hd_data_t **out_trans, TickType_t timeout)
{
    esp_err_t ret;
    spi_slave_hd_slot_t* host = spihost[host_id];

    SPIHD_CHECK(host->append_mode == 1, "This API should be used for SPI Slave HD Append Mode", ESP_ERR_INVALID_STATE);
    SPIHD_CHECK(chan == SPI_SLAVE_CHAN_TX || chan == SPI_SLAVE_CHAN_RX, "Invalid channel", ESP_ERR_INVALID_ARG);
    ret = get_ret_queue_result(host_id, chan, out_trans, timeout);

    return ret;
}
#endif //#if CONFIG_IDF_TARGET_ESP32S2
