/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_pm.h"
#include "esp_heap_caps.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "soc/lldesc.h"
#include "soc/soc_caps.h"
#include "soc/spi_periph.h"
#include "soc/soc_memory_layout.h"
#include "hal/spi_ll.h"
#include "hal/spi_slave_hal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "driver/gpio.h"
#include "esp_private/spi_common_internal.h"
#include "driver/spi_slave.h"
#include "hal/spi_slave_hal.h"

static const char *SPI_TAG = "spi_slave";
#define SPI_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(SPI_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }

#ifdef CONFIG_SPI_SLAVE_ISR_IN_IRAM
#define SPI_SLAVE_ISR_ATTR IRAM_ATTR
#else
#define SPI_SLAVE_ISR_ATTR
#endif

#ifdef CONFIG_SPI_SLAVE_IN_IRAM
#define SPI_SLAVE_ATTR IRAM_ATTR
#else
#define SPI_SLAVE_ATTR
#endif

typedef struct {
    int id;
    spi_slave_interface_config_t cfg;
    intr_handle_t intr;
    spi_slave_hal_context_t hal;
    spi_slave_transaction_t *cur_trans;
    uint32_t flags;
    int max_transfer_sz;
    QueueHandle_t trans_queue;
    QueueHandle_t ret_queue;
    bool dma_enabled;
    uint32_t tx_dma_chan;
    uint32_t rx_dma_chan;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
} spi_slave_t;

static spi_slave_t *spihost[SOC_SPI_PERIPH_NUM];

static void spi_intr(void *arg);

static inline bool is_valid_host(spi_host_device_t host)
{
//SPI1 can be used as GPSPI only on ESP32
#if CONFIG_IDF_TARGET_ESP32
    return host >= SPI1_HOST && host <= SPI3_HOST;
#elif (SOC_SPI_PERIPH_NUM == 2)
    return host == SPI2_HOST;
#elif (SOC_SPI_PERIPH_NUM == 3)
    return host >= SPI2_HOST && host <= SPI3_HOST;
#endif
}

static inline bool SPI_SLAVE_ISR_ATTR bus_is_iomux(spi_slave_t *host)
{
    return host->flags&SPICOMMON_BUSFLAG_IOMUX_PINS;
}

static void SPI_SLAVE_ISR_ATTR freeze_cs(spi_slave_t *host)
{
    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, spi_periph_signal[host->id].spics_in, false);
}

// Use this function instead of cs_initial to avoid overwrite the output config
// This is used in test by internal gpio matrix connections
static inline void SPI_SLAVE_ISR_ATTR restore_cs(spi_slave_t *host)
{
    if (bus_is_iomux(host)) {
        gpio_iomux_in(host->cfg.spics_io_num, spi_periph_signal[host->id].spics_in);
    } else {
        esp_rom_gpio_connect_in_signal(host->cfg.spics_io_num, spi_periph_signal[host->id].spics_in, false);
    }
}

esp_err_t spi_slave_initialize(spi_host_device_t host, const spi_bus_config_t *bus_config, const spi_slave_interface_config_t *slave_config, spi_dma_chan_t dma_chan)
{
    bool spi_chan_claimed;
    uint32_t actual_tx_dma_chan = 0;
    uint32_t actual_rx_dma_chan = 0;
    esp_err_t ret = ESP_OK;
    esp_err_t err;
    SPI_CHECK(is_valid_host(host), "invalid host", ESP_ERR_INVALID_ARG);
#ifdef CONFIG_IDF_TARGET_ESP32
    SPI_CHECK(dma_chan >= SPI_DMA_DISABLED && dma_chan <= SPI_DMA_CH_AUTO, "invalid dma channel", ESP_ERR_INVALID_ARG );
#elif CONFIG_IDF_TARGET_ESP32S2
    SPI_CHECK( dma_chan == SPI_DMA_DISABLED || dma_chan == (int)host || dma_chan == SPI_DMA_CH_AUTO, "invalid dma channel", ESP_ERR_INVALID_ARG );
#elif SOC_GDMA_SUPPORTED
    SPI_CHECK( dma_chan == SPI_DMA_DISABLED || dma_chan == SPI_DMA_CH_AUTO, "invalid dma channel, chip only support spi dma channel auto-alloc", ESP_ERR_INVALID_ARG );
#endif
    SPI_CHECK((bus_config->intr_flags & (ESP_INTR_FLAG_HIGH|ESP_INTR_FLAG_EDGE|ESP_INTR_FLAG_INTRDISABLED))==0, "intr flag not allowed", ESP_ERR_INVALID_ARG);
#ifndef CONFIG_SPI_SLAVE_ISR_IN_IRAM
    SPI_CHECK((bus_config->intr_flags & ESP_INTR_FLAG_IRAM)==0, "ESP_INTR_FLAG_IRAM should be disabled when CONFIG_SPI_SLAVE_ISR_IN_IRAM is not set.", ESP_ERR_INVALID_ARG);
#endif
    SPI_CHECK(slave_config->spics_io_num < 0 || GPIO_IS_VALID_GPIO(slave_config->spics_io_num), "spics pin invalid", ESP_ERR_INVALID_ARG);

    spi_chan_claimed=spicommon_periph_claim(host, "spi slave");
    SPI_CHECK(spi_chan_claimed, "host already in use", ESP_ERR_INVALID_STATE);

    spihost[host] = malloc(sizeof(spi_slave_t));
    if (spihost[host] == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }
    memset(spihost[host], 0, sizeof(spi_slave_t));
    memcpy(&spihost[host]->cfg, slave_config, sizeof(spi_slave_interface_config_t));
    spihost[host]->id = host;

    bool use_dma = (dma_chan != SPI_DMA_DISABLED);
    spihost[host]->dma_enabled = use_dma;
    if (use_dma) {
        ret = spicommon_dma_chan_alloc(host, dma_chan, &actual_tx_dma_chan, &actual_rx_dma_chan);
        if (ret != ESP_OK) {
            goto cleanup;
        }
    }

    err = spicommon_bus_initialize_io(host, bus_config, SPICOMMON_BUSFLAG_SLAVE|bus_config->flags, &spihost[host]->flags);
    if (err!=ESP_OK) {
        ret = err;
        goto cleanup;
    }
    if (slave_config->spics_io_num >= 0) {
        spicommon_cs_initialize(host, slave_config->spics_io_num, 0, !bus_is_iomux(spihost[host]));
    }

    // The slave DMA suffers from unexpected transactions. Forbid reading if DMA is enabled by disabling the CS line.
    if (use_dma) freeze_cs(spihost[host]);

    int dma_desc_ct = 0;
    spihost[host]->tx_dma_chan = actual_tx_dma_chan;
    spihost[host]->rx_dma_chan = actual_rx_dma_chan;
    if (use_dma) {
        //See how many dma descriptors we need and allocate them
        dma_desc_ct = (bus_config->max_transfer_sz + SPI_MAX_DMA_LEN - 1) / SPI_MAX_DMA_LEN;
        if (dma_desc_ct == 0) dma_desc_ct = 1; //default to 4k when max is not given
        spihost[host]->max_transfer_sz = dma_desc_ct * SPI_MAX_DMA_LEN;
    } else {
        //We're limited to non-DMA transfers: the SPI work registers can hold 64 bytes at most.
        spihost[host]->max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE;
    }
#ifdef CONFIG_PM_ENABLE
    err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "spi_slave",
            &spihost[host]->pm_lock);
    if (err != ESP_OK) {
        ret = err;
        goto cleanup;
    }
    // Lock APB frequency while SPI slave driver is in use
    esp_pm_lock_acquire(spihost[host]->pm_lock);
#endif //CONFIG_PM_ENABLE

    //Create queues
    spihost[host]->trans_queue = xQueueCreate(slave_config->queue_size, sizeof(spi_slave_transaction_t *));
    spihost[host]->ret_queue = xQueueCreate(slave_config->queue_size, sizeof(spi_slave_transaction_t *));
    if (!spihost[host]->trans_queue || !spihost[host]->ret_queue) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    int flags = bus_config->intr_flags | ESP_INTR_FLAG_INTRDISABLED;
    err = esp_intr_alloc(spicommon_irqsource_for_host(host), flags, spi_intr, (void *)spihost[host], &spihost[host]->intr);
    if (err != ESP_OK) {
        ret = err;
        goto cleanup;
    }

    spi_slave_hal_context_t *hal = &spihost[host]->hal;
    //assign the SPI, RX DMA and TX DMA peripheral registers beginning address
    spi_slave_hal_config_t hal_config = {
        .host_id = host,
        .dma_in = SPI_LL_GET_HW(host),
        .dma_out = SPI_LL_GET_HW(host)
    };
    spi_slave_hal_init(hal, &hal_config);

    if (dma_desc_ct) {
        hal->dmadesc_tx = heap_caps_malloc(sizeof(lldesc_t) * dma_desc_ct, MALLOC_CAP_DMA);
        hal->dmadesc_rx = heap_caps_malloc(sizeof(lldesc_t) * dma_desc_ct, MALLOC_CAP_DMA);
        if (!hal->dmadesc_tx || !hal->dmadesc_rx) {
            ret = ESP_ERR_NO_MEM;
            goto cleanup;
        }
    }
    hal->dmadesc_n = dma_desc_ct;
    hal->rx_lsbfirst = (slave_config->flags & SPI_SLAVE_RXBIT_LSBFIRST) ? 1 : 0;
    hal->tx_lsbfirst = (slave_config->flags & SPI_SLAVE_TXBIT_LSBFIRST) ? 1 : 0;
    hal->mode = slave_config->mode;
    hal->use_dma = use_dma;
    hal->tx_dma_chan = actual_tx_dma_chan;
    hal->rx_dma_chan = actual_rx_dma_chan;

    spi_slave_hal_setup_device(hal);

    return ESP_OK;

cleanup:
    if (spihost[host]) {
        if (spihost[host]->trans_queue) vQueueDelete(spihost[host]->trans_queue);
        if (spihost[host]->ret_queue) vQueueDelete(spihost[host]->ret_queue);
        free(spihost[host]->hal.dmadesc_tx);
        free(spihost[host]->hal.dmadesc_rx);
#ifdef CONFIG_PM_ENABLE
        if (spihost[host]->pm_lock) {
            esp_pm_lock_release(spihost[host]->pm_lock);
            esp_pm_lock_delete(spihost[host]->pm_lock);
        }
#endif
    }
    spi_slave_hal_deinit(&spihost[host]->hal);
    if (spihost[host]->dma_enabled) {
        spicommon_dma_chan_free(host);
    }

    free(spihost[host]);
    spihost[host] = NULL;
    spicommon_periph_free(host);

    return ret;
}

esp_err_t spi_slave_free(spi_host_device_t host)
{
    SPI_CHECK(is_valid_host(host), "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host], "host not slave", ESP_ERR_INVALID_ARG);
    if (spihost[host]->trans_queue) vQueueDelete(spihost[host]->trans_queue);
    if (spihost[host]->ret_queue) vQueueDelete(spihost[host]->ret_queue);
    if (spihost[host]->dma_enabled) {
        spicommon_dma_chan_free(host);
    }
    free(spihost[host]->hal.dmadesc_tx);
    free(spihost[host]->hal.dmadesc_rx);
    esp_intr_free(spihost[host]->intr);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(spihost[host]->pm_lock);
    esp_pm_lock_delete(spihost[host]->pm_lock);
#endif //CONFIG_PM_ENABLE
    free(spihost[host]);
    spihost[host] = NULL;
    spicommon_periph_free(host);
    return ESP_OK;
}


esp_err_t SPI_SLAVE_ATTR spi_slave_queue_trans(spi_host_device_t host, const spi_slave_transaction_t *trans_desc, TickType_t ticks_to_wait)
{
    BaseType_t r;
    SPI_CHECK(is_valid_host(host), "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host], "host not slave", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host]->dma_enabled == 0 || trans_desc->tx_buffer==NULL || esp_ptr_dma_capable(trans_desc->tx_buffer),
			"txdata not in DMA-capable memory", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host]->dma_enabled == 0 || trans_desc->rx_buffer==NULL ||
        (esp_ptr_dma_capable(trans_desc->rx_buffer) && esp_ptr_word_aligned(trans_desc->rx_buffer) &&
            (trans_desc->length%4==0)),
        "rxdata not in DMA-capable memory or not WORD aligned", ESP_ERR_INVALID_ARG);

    SPI_CHECK(trans_desc->length <= spihost[host]->max_transfer_sz * 8, "data transfer > host maximum", ESP_ERR_INVALID_ARG);
    r = xQueueSend(spihost[host]->trans_queue, (void *)&trans_desc, ticks_to_wait);
    if (!r) return ESP_ERR_TIMEOUT;
    esp_intr_enable(spihost[host]->intr);
    return ESP_OK;
}


esp_err_t SPI_SLAVE_ATTR spi_slave_get_trans_result(spi_host_device_t host, spi_slave_transaction_t **trans_desc, TickType_t ticks_to_wait)
{
    BaseType_t r;
    SPI_CHECK(is_valid_host(host), "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host], "host not slave", ESP_ERR_INVALID_ARG);
    r = xQueueReceive(spihost[host]->ret_queue, (void *)trans_desc, ticks_to_wait);
    if (!r) return ESP_ERR_TIMEOUT;
    return ESP_OK;
}


esp_err_t SPI_SLAVE_ATTR spi_slave_transmit(spi_host_device_t host, spi_slave_transaction_t *trans_desc, TickType_t ticks_to_wait)
{
    esp_err_t ret;
    spi_slave_transaction_t *ret_trans;
    //ToDo: check if any spi transfers in flight
    ret = spi_slave_queue_trans(host, trans_desc, ticks_to_wait);
    if (ret != ESP_OK) return ret;
    ret = spi_slave_get_trans_result(host, &ret_trans, ticks_to_wait);
    if (ret != ESP_OK) return ret;
    assert(ret_trans == trans_desc);
    return ESP_OK;
}

static void SPI_SLAVE_ISR_ATTR spi_slave_restart_after_dmareset(void *arg)
{
    spi_slave_t *host = (spi_slave_t *)arg;
    esp_intr_enable(host->intr);
}

//This is run in interrupt context and apart from initialization and destruction, this is the only code
//touching the host (=spihost[x]) variable. The rest of the data arrives in queues. That is why there are
//no muxes in this code.
static void SPI_SLAVE_ISR_ATTR spi_intr(void *arg)
{
    BaseType_t r;
    BaseType_t do_yield = pdFALSE;
    spi_slave_transaction_t *trans = NULL;
    spi_slave_t *host = (spi_slave_t *)arg;
    spi_slave_hal_context_t *hal = &host->hal;

    assert(spi_slave_hal_usr_is_done(hal));

    bool use_dma = host->dma_enabled;
    if (host->cur_trans) {
        // When DMA is enabled, the slave rx dma suffers from unexpected transactions. Forbid reading until transaction ready.
        if (use_dma) freeze_cs(host);

        spi_slave_hal_store_result(hal);
        host->cur_trans->trans_len = spi_slave_hal_get_rcv_bitlen(hal);

        if (spi_slave_hal_dma_need_reset(hal)) {
            //On ESP32 and ESP32S2, actual_tx_dma_chan and actual_rx_dma_chan are always same
            spicommon_dmaworkaround_req_reset(host->tx_dma_chan, spi_slave_restart_after_dmareset, host);
        }
        if (host->cfg.post_trans_cb) host->cfg.post_trans_cb(host->cur_trans);
        //Okay, transaction is done.
        //Return transaction descriptor.
        xQueueSendFromISR(host->ret_queue, &host->cur_trans, &do_yield);
        host->cur_trans = NULL;
    }
    if (use_dma) {
        //On ESP32 and ESP32S2, actual_tx_dma_chan and actual_rx_dma_chan are always same
        spicommon_dmaworkaround_idle(host->tx_dma_chan);
        if (spicommon_dmaworkaround_reset_in_progress()) {
            //We need to wait for the reset to complete. Disable int (will be re-enabled on reset callback) and exit isr.
            esp_intr_disable(host->intr);
            if (do_yield) portYIELD_FROM_ISR();
            return;
        }
    }

    //Disable interrupt before checking to avoid concurrency issue.
    esp_intr_disable(host->intr);
    //Grab next transaction
    r = xQueueReceiveFromISR(host->trans_queue, &trans, &do_yield);
    if (r) {
        // sanity check
        assert(trans);

        //enable the interrupt again if there is packet to send
        esp_intr_enable(host->intr);

        //We have a transaction. Send it.
        host->cur_trans = trans;

        hal->bitlen = trans->length;
        hal->rx_buffer = trans->rx_buffer;
        hal->tx_buffer = trans->tx_buffer;

        if (use_dma) {
            //On ESP32 and ESP32S2, actual_tx_dma_chan and actual_rx_dma_chan are always same
            spicommon_dmaworkaround_transfer_active(host->tx_dma_chan);
        }

        spi_slave_hal_prepare_data(hal);

        //The slave rx dma get disturbed by unexpected transaction. Only connect the CS when slave is ready.
        if (use_dma) {
            restore_cs(host);
        }

        //Kick off transfer
        spi_slave_hal_user_start(hal);
        if (host->cfg.post_setup_cb) host->cfg.post_setup_cb(trans);
    }
    if (do_yield) portYIELD_FROM_ISR();
}
