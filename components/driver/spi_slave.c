// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include "sdkconfig.h"
#include <hal/spi_ll.h>
#include <hal/spi_slave_hal.h>
#include <soc/lldesc.h>
#include "driver/spi_common_internal.h"
#include "driver/spi_slave.h"
#include "soc/spi_periph.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_pm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/task.h"
#include "soc/soc_memory_layout.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h"

static const char *SPI_TAG = "spi_slave";
#define SPI_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(SPI_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }

#define VALID_HOST(x) (x>SPI_HOST && x<=VSPI_HOST)

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
    int dma_chan;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
} spi_slave_t;

static spi_slave_t *spihost[SOC_SPI_PERIPH_NUM];

static void IRAM_ATTR spi_intr(void *arg);

static inline bool bus_is_iomux(spi_slave_t *host)
{
    return host->flags&SPICOMMON_BUSFLAG_IOMUX_PINS;
}

static void freeze_cs(spi_slave_t *host)
{
    gpio_matrix_in(GPIO_FUNC_IN_HIGH, spi_periph_signal[host->id].spics_in, false);
}

// Use this function instead of cs_initial to avoid overwrite the output config
// This is used in test by internal gpio matrix connections
static inline void restore_cs(spi_slave_t *host)
{
    if (bus_is_iomux(host)) {
        gpio_iomux_in(host->cfg.spics_io_num, spi_periph_signal[host->id].spics_in);
    } else {
        gpio_matrix_in(host->cfg.spics_io_num, spi_periph_signal[host->id].spics_in, false);
    }
}

esp_err_t spi_slave_initialize(spi_host_device_t host, const spi_bus_config_t *bus_config, const spi_slave_interface_config_t *slave_config, int dma_chan)
{
    bool spi_chan_claimed, dma_chan_claimed;
    esp_err_t ret = ESP_OK;
    esp_err_t err;
    //We only support HSPI/VSPI, period.
    SPI_CHECK(VALID_HOST(host), "invalid host", ESP_ERR_INVALID_ARG);
#if defined(CONFIG_IDF_TARGET_ESP32)
    SPI_CHECK( dma_chan >= 0 && dma_chan <= 2, "invalid dma channel", ESP_ERR_INVALID_ARG );
#elif defined(CONFIG_IDF_TARGET_ESP32S2BETA)
    SPI_CHECK( dma_chan == 0 || dma_chan == host, "invalid dma channel", ESP_ERR_INVALID_ARG );
#endif
    SPI_CHECK((bus_config->intr_flags & (ESP_INTR_FLAG_HIGH|ESP_INTR_FLAG_EDGE|ESP_INTR_FLAG_INTRDISABLED))==0, "intr flag not allowed", ESP_ERR_INVALID_ARG);
#ifndef CONFIG_SPI_SLAVE_ISR_IN_IRAM
    SPI_CHECK((bus_config->intr_flags & ESP_INTR_FLAG_IRAM)==0, "ESP_INTR_FLAG_IRAM should be disabled when CONFIG_SPI_SLAVE_ISR_IN_IRAM is not set.", ESP_ERR_INVALID_ARG);
#endif

    spi_chan_claimed=spicommon_periph_claim(host, "spi slave");
    SPI_CHECK(spi_chan_claimed, "host already in use", ESP_ERR_INVALID_STATE);

    bool use_dma = dma_chan != 0;
    if (use_dma) {
        dma_chan_claimed=spicommon_dma_chan_claim(dma_chan);
        if ( !dma_chan_claimed ) {
            spicommon_periph_free( host );
            SPI_CHECK(dma_chan_claimed, "dma channel already in use", ESP_ERR_INVALID_STATE);
        }
    }

    spihost[host] = malloc(sizeof(spi_slave_t));
    if (spihost[host] == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }
    memset(spihost[host], 0, sizeof(spi_slave_t));
    memcpy(&spihost[host]->cfg, slave_config, sizeof(spi_slave_interface_config_t));
    spihost[host]->id = host;

    err = spicommon_bus_initialize_io(host, bus_config, dma_chan, SPICOMMON_BUSFLAG_SLAVE|bus_config->flags, &spihost[host]->flags);
    if (err!=ESP_OK) {
        ret = err;
        goto cleanup;
    }
    spicommon_cs_initialize(host, slave_config->spics_io_num, 0, !bus_is_iomux(spihost[host]));
    // The slave DMA suffers from unexpected transactions. Forbid reading if DMA is enabled by disabling the CS line.
    if (use_dma) freeze_cs(spihost[host]);

    int dma_desc_ct = 0;
    spihost[host]->dma_chan = dma_chan;
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
    spi_slave_hal_init(hal, host);

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
    free(spihost[host]);
    spihost[host] = NULL;
    spicommon_periph_free(host);
    spicommon_dma_chan_free(dma_chan);
    return ret;
}

esp_err_t spi_slave_free(spi_host_device_t host)
{
    SPI_CHECK(VALID_HOST(host), "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host], "host not slave", ESP_ERR_INVALID_ARG);
    if (spihost[host]->trans_queue) vQueueDelete(spihost[host]->trans_queue);
    if (spihost[host]->ret_queue) vQueueDelete(spihost[host]->ret_queue);
    if ( spihost[host]->dma_chan > 0 ) {
        spicommon_dma_chan_free ( spihost[host]->dma_chan );
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
    SPI_CHECK(VALID_HOST(host), "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host], "host not slave", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host]->dma_chan == 0 || trans_desc->tx_buffer==NULL || esp_ptr_dma_capable(trans_desc->tx_buffer),
			"txdata not in DMA-capable memory", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host]->dma_chan == 0 || trans_desc->rx_buffer==NULL ||
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
    SPI_CHECK(VALID_HOST(host), "invalid host", ESP_ERR_INVALID_ARG);
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

#ifdef DEBUG_SLAVE
static void dumpregs(spi_dev_t *hw)
{
    ets_printf("***REG DUMP ***\n");
    ets_printf("mosi_dlen         : %08X\n", hw->mosi_dlen.val);
    ets_printf("miso_dlen         : %08X\n", hw->miso_dlen.val);
    ets_printf("slv_wrbuf_dlen    : %08X\n", hw->slv_wrbuf_dlen.val);
    ets_printf("slv_rdbuf_dlen    : %08X\n", hw->slv_rdbuf_dlen.val);
    ets_printf("slave             : %08X\n", hw->slave.val);
    ets_printf("slv_rdata_bit     : %x\n", hw->slv_rd_bit.slv_rdata_bit);
    ets_printf("dma_rx_status     : %08X\n", hw->dma_rx_status);
    ets_printf("dma_tx_status     : %08X\n", hw->dma_tx_status);
}


static void dumpll(lldesc_t *ll)
{
    ets_printf("****LL DUMP****\n");
    ets_printf("Size %d\n", ll->size);
    ets_printf("Len: %d\n", ll->length);
    ets_printf("Owner: %s\n", ll->owner ? "dma" : "cpu");
}
#endif

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

#ifdef DEBUG_SLAVE
    dumpregs(host->hw);
    if (host->dmadesc_rx) dumpll(&host->dmadesc_rx[0]);
#endif

    assert(spi_slave_hal_usr_is_done(hal));

    bool use_dma = host->dma_chan != 0;
    if (host->cur_trans) {
        // When DMA is enabled, the slave rx dma suffers from unexpected transactions. Forbid reading until transaction ready.
        if (use_dma) freeze_cs(host);

        spi_slave_hal_store_result(hal);
        host->cur_trans->trans_len = spi_slave_hal_get_rcv_bitlen(hal);

        if (spi_slave_hal_dma_need_reset(hal)) {
            spicommon_dmaworkaround_req_reset(host->dma_chan, spi_slave_restart_after_dmareset, host);
        }
        if (host->cfg.post_trans_cb) host->cfg.post_trans_cb(host->cur_trans);
        //Okay, transaction is done.
        //Return transaction descriptor.
        xQueueSendFromISR(host->ret_queue, &host->cur_trans, &do_yield);
        host->cur_trans = NULL;
    }
    if (use_dma) {
        spicommon_dmaworkaround_idle(host->dma_chan);
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
        //enable the interrupt again if there is packet to send
        esp_intr_enable(host->intr);

        //We have a transaction. Send it.
        host->cur_trans = trans;

        hal->bitlen = trans->length;
        hal->rx_buffer = trans->rx_buffer;
        hal->tx_buffer = trans->tx_buffer;

        if (use_dma) {
            spicommon_dmaworkaround_transfer_active(host->dma_chan);
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

