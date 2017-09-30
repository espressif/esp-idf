// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include "driver/spi_common.h"
#include "driver/spi_slave.h"
#include "soc/gpio_sig_map.h"
#include "soc/spi_reg.h"
#include "soc/dport_reg.h"
#include "soc/spi_struct.h"
#include "rom/ets_sys.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_intr.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_pm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "soc/soc.h"
#include "soc/soc_memory_layout.h"
#include "soc/dport_reg.h"
#include "rom/lldesc.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp_heap_caps.h"

static const char *SPI_TAG = "spi_slave";
#define SPI_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(SPI_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }

#define VALID_HOST(x) (x>SPI_HOST && x<=VSPI_HOST)

typedef struct {
    spi_slave_interface_config_t cfg;
    intr_handle_t intr;
    spi_dev_t *hw;
    spi_slave_transaction_t *cur_trans;
    lldesc_t *dmadesc_tx;
    lldesc_t *dmadesc_rx;
    bool no_gpio_matrix;
    int max_transfer_sz;
    QueueHandle_t trans_queue;
    QueueHandle_t ret_queue;
    int dma_chan;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
} spi_slave_t;

static spi_slave_t *spihost[3];

static void IRAM_ATTR spi_intr(void *arg);

esp_err_t spi_slave_initialize(spi_host_device_t host, const spi_bus_config_t *bus_config, const spi_slave_interface_config_t *slave_config, int dma_chan)
{
    bool native, spi_chan_claimed, dma_chan_claimed;
    //We only support HSPI/VSPI, period.
    SPI_CHECK(VALID_HOST(host), "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK( dma_chan >= 0 && dma_chan <= 2, "invalid dma channel", ESP_ERR_INVALID_ARG );

    spi_chan_claimed=spicommon_periph_claim(host);
    SPI_CHECK(spi_chan_claimed, "host already in use", ESP_ERR_INVALID_STATE);
    
    if ( dma_chan != 0 ) {
        dma_chan_claimed=spicommon_dma_chan_claim(dma_chan);
        if ( !dma_chan_claimed ) {
            spicommon_periph_free( host );
            SPI_CHECK(dma_chan_claimed, "dma channel already in use", ESP_ERR_INVALID_STATE);
        }
    }

    spihost[host] = malloc(sizeof(spi_slave_t));
    if (spihost[host] == NULL) goto nomem;
    memset(spihost[host], 0, sizeof(spi_slave_t));
    memcpy(&spihost[host]->cfg, slave_config, sizeof(spi_slave_interface_config_t));

    spicommon_bus_initialize_io(host, bus_config, dma_chan, SPICOMMON_BUSFLAG_SLAVE, &native);
    gpio_set_direction(slave_config->spics_io_num, GPIO_MODE_INPUT);
    spicommon_cs_initialize(host, slave_config->spics_io_num, 0, native == false);
    spihost[host]->no_gpio_matrix = native;
    spihost[host]->dma_chan = dma_chan;
    if (dma_chan != 0) {
        //See how many dma descriptors we need and allocate them
        int dma_desc_ct = (bus_config->max_transfer_sz + SPI_MAX_DMA_LEN - 1) / SPI_MAX_DMA_LEN;
        if (dma_desc_ct == 0) dma_desc_ct = 1; //default to 4k when max is not given
        spihost[host]->max_transfer_sz = dma_desc_ct * SPI_MAX_DMA_LEN;
        spihost[host]->dmadesc_tx = heap_caps_malloc(sizeof(lldesc_t) * dma_desc_ct, MALLOC_CAP_DMA);
        spihost[host]->dmadesc_rx = heap_caps_malloc(sizeof(lldesc_t) * dma_desc_ct, MALLOC_CAP_DMA);
        if (!spihost[host]->dmadesc_tx || !spihost[host]->dmadesc_rx) goto nomem;
    } else {
        //We're limited to non-DMA transfers: the SPI work registers can hold 64 bytes at most.
        spihost[host]->max_transfer_sz = 16 * 4;
    }
#ifdef CONFIG_PM_ENABLE
    esp_err_t err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "spi_slave",
            &spihost[host]->pm_lock);
    if (err != ESP_OK) {
        goto nomem;
    }
    // Lock APB frequency while SPI slave driver is in use
    esp_pm_lock_acquire(spihost[host]->pm_lock);
#endif //CONFIG_PM_ENABLE

    //Create queues
    spihost[host]->trans_queue = xQueueCreate(slave_config->queue_size, sizeof(spi_slave_transaction_t *));
    spihost[host]->ret_queue = xQueueCreate(slave_config->queue_size, sizeof(spi_slave_transaction_t *));
    if (!spihost[host]->trans_queue || !spihost[host]->ret_queue) goto nomem;

    esp_intr_alloc(spicommon_irqsource_for_host(host), ESP_INTR_FLAG_INTRDISABLED, spi_intr, (void *)spihost[host], &spihost[host]->intr);
    spihost[host]->hw = spicommon_hw_for_host(host);

    //Configure slave
    spihost[host]->hw->clock.val = 0;
    spihost[host]->hw->user.val = 0;
    spihost[host]->hw->ctrl.val = 0;
    spihost[host]->hw->slave.wr_rd_buf_en = 1; //no sure if needed
    spihost[host]->hw->user.doutdin = 1; //we only support full duplex
    spihost[host]->hw->user.sio = 0;
    spihost[host]->hw->slave.slave_mode = 1;
    spihost[host]->hw->dma_conf.val |= SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST;
    spihost[host]->hw->dma_out_link.start = 0;
    spihost[host]->hw->dma_in_link.start = 0;
    spihost[host]->hw->dma_conf.val &= ~(SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST);
    spihost[host]->hw->dma_conf.out_data_burst_en = 1;
    spihost[host]->hw->slave.sync_reset = 1;
    spihost[host]->hw->slave.sync_reset = 0;


    bool nodelay = true;
    spihost[host]->hw->ctrl.rd_bit_order = (slave_config->flags & SPI_SLAVE_RXBIT_LSBFIRST) ? 1 : 0;
    spihost[host]->hw->ctrl.wr_bit_order = (slave_config->flags & SPI_SLAVE_TXBIT_LSBFIRST) ? 1 : 0;
    if (slave_config->mode == 0) {
        spihost[host]->hw->pin.ck_idle_edge = 0;
        spihost[host]->hw->user.ck_i_edge = 1;
        spihost[host]->hw->ctrl2.miso_delay_mode = nodelay ? 0 : 2;
    } else if (slave_config->mode == 1) {
        spihost[host]->hw->pin.ck_idle_edge = 0;
        spihost[host]->hw->user.ck_i_edge = 0;
        spihost[host]->hw->ctrl2.miso_delay_mode = nodelay ? 0 : 1;
    } else if (slave_config->mode == 2) {
        spihost[host]->hw->pin.ck_idle_edge = 1;
        spihost[host]->hw->user.ck_i_edge = 0;
        spihost[host]->hw->ctrl2.miso_delay_mode = nodelay ? 0 : 1;
    } else if (slave_config->mode == 3) {
        spihost[host]->hw->pin.ck_idle_edge = 1;
        spihost[host]->hw->user.ck_i_edge = 1;
        spihost[host]->hw->ctrl2.miso_delay_mode = nodelay ? 0 : 2;
    }


    //Reset DMA
    spihost[host]->hw->dma_conf.val |= SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST;
    spihost[host]->hw->dma_out_link.start = 0;
    spihost[host]->hw->dma_in_link.start = 0;
    spihost[host]->hw->dma_conf.val &= ~(SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST);

    //Disable unneeded ints
    spihost[host]->hw->slave.rd_buf_done = 0;
    spihost[host]->hw->slave.wr_buf_done = 0;
    spihost[host]->hw->slave.rd_sta_done = 0;
    spihost[host]->hw->slave.wr_sta_done = 0;
    spihost[host]->hw->slave.rd_buf_inten = 0;
    spihost[host]->hw->slave.wr_buf_inten = 0;
    spihost[host]->hw->slave.rd_sta_inten = 0;
    spihost[host]->hw->slave.wr_sta_inten = 0;

    //Force a transaction done interrupt. This interrupt won't fire yet because we initialized the SPI interrupt as
    //disabled.  This way, we can just enable the SPI interrupt and the interrupt handler will kick in, handling
    //any transactions that are queued.
    spihost[host]->hw->slave.trans_inten = 1;
    spihost[host]->hw->slave.trans_done = 1;

    return ESP_OK;

nomem:
    if (spihost[host]) {
        if (spihost[host]->trans_queue) vQueueDelete(spihost[host]->trans_queue);
        if (spihost[host]->ret_queue) vQueueDelete(spihost[host]->ret_queue);
        free(spihost[host]->dmadesc_tx);
        free(spihost[host]->dmadesc_rx);
#ifdef CONFIG_PM_ENABLE
        if (spihost[host]->pm_lock) {
            esp_pm_lock_release(spihost[host]->pm_lock);
            esp_pm_lock_delete(spihost[host]->pm_lock);
        }
#endif
    }
    free(spihost[host]);
    spihost[host] = NULL;
    spicommon_periph_free(host);
    spicommon_dma_chan_free(dma_chan);
    return ESP_ERR_NO_MEM;
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
    free(spihost[host]->dmadesc_tx);
    free(spihost[host]->dmadesc_rx);
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


esp_err_t spi_slave_queue_trans(spi_host_device_t host, const spi_slave_transaction_t *trans_desc, TickType_t ticks_to_wait)
{
    BaseType_t r;
    SPI_CHECK(VALID_HOST(host), "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host], "host not slave", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host]->dma_chan == 0 || trans_desc->tx_buffer==NULL || esp_ptr_dma_capable(trans_desc->tx_buffer), 
			"txdata not in DMA-capable memory", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host]->dma_chan == 0 || trans_desc->rx_buffer==NULL || esp_ptr_dma_capable(trans_desc->rx_buffer), 
			"rxdata not in DMA-capable memory", ESP_ERR_INVALID_ARG);

    SPI_CHECK(trans_desc->length <= spihost[host]->max_transfer_sz * 8, "data transfer > host maximum", ESP_ERR_INVALID_ARG);
    r = xQueueSend(spihost[host]->trans_queue, (void *)&trans_desc, ticks_to_wait);
    if (!r) return ESP_ERR_TIMEOUT;
    esp_intr_enable(spihost[host]->intr);
    return ESP_OK;
}


esp_err_t spi_slave_get_trans_result(spi_host_device_t host, spi_slave_transaction_t **trans_desc, TickType_t ticks_to_wait)
{
    BaseType_t r;
    SPI_CHECK(VALID_HOST(host), "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host], "host not slave", ESP_ERR_INVALID_ARG);
    r = xQueueReceive(spihost[host]->ret_queue, (void *)trans_desc, ticks_to_wait);
    if (!r) return ESP_ERR_TIMEOUT;
    return ESP_OK;
}


esp_err_t spi_slave_transmit(spi_host_device_t host, spi_slave_transaction_t *trans_desc, TickType_t ticks_to_wait)
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

static void IRAM_ATTR spi_slave_restart_after_dmareset(void *arg)
{
    spi_slave_t *host = (spi_slave_t *)arg;
    esp_intr_enable(host->intr);
}

//This is run in interrupt context and apart from initialization and destruction, this is the only code
//touching the host (=spihost[x]) variable. The rest of the data arrives in queues. That is why there are
//no muxes in this code.
static void IRAM_ATTR spi_intr(void *arg)
{
    BaseType_t r;
    BaseType_t do_yield = pdFALSE;
    spi_slave_transaction_t *trans = NULL;
    spi_slave_t *host = (spi_slave_t *)arg;

#ifdef DEBUG_SLAVE
    dumpregs(host->hw);
    if (host->dmadesc_rx) dumpll(&host->dmadesc_rx[0]);
#endif

    //Ignore all but the trans_done int.
    if (!host->hw->slave.trans_done) return;

    if (host->cur_trans) {
        //when data of cur_trans->length are all sent, the slv_rdata_bit
        //will be the length sent-1 (i.e. cur_trans->length-1 ), otherwise 
        //the length sent.
        host->cur_trans->trans_len = host->hw->slv_rd_bit.slv_rdata_bit;
        if ( host->cur_trans->trans_len == host->cur_trans->length - 1 ) {
            host->cur_trans->trans_len++;
        }

        if (host->dma_chan == 0 && host->cur_trans->rx_buffer) {
            //Copy result out
            uint32_t *data = host->cur_trans->rx_buffer;
            for (int x = 0; x < host->cur_trans->trans_len; x += 32) {
                uint32_t word;
                int len = host->cur_trans->trans_len - x;
                if (len > 32) len = 32;
                word = host->hw->data_buf[(x / 32)];
                memcpy(&data[x / 32], &word, (len + 7) / 8);
            }
        } else if (host->dma_chan != 0 && host->cur_trans->rx_buffer) {
            int i;
            //In case CS goes high too soon, the transfer is aborted while the DMA channel still thinks it's going. This
            //leads to issues later on, so in that case we need to reset the channel. The state can be detected because
            //the DMA system doesn't give back the offending descriptor; the owner is still set to DMA.
            for (i = 0; host->dmadesc_rx[i].eof == 0 && host->dmadesc_rx[i].owner == 0; i++) ;
            if (host->dmadesc_rx[i].owner) {
                spicommon_dmaworkaround_req_reset(host->dma_chan, spi_slave_restart_after_dmareset, host);
            }
        }
        if (host->cfg.post_trans_cb) host->cfg.post_trans_cb(host->cur_trans);
        //Okay, transaction is done.
        //Return transaction descriptor.
        xQueueSendFromISR(host->ret_queue, &host->cur_trans, &do_yield);
        host->cur_trans = NULL;
    }
    if (host->dma_chan != 0) {
        spicommon_dmaworkaround_idle(host->dma_chan);
        if (spicommon_dmaworkaround_reset_in_progress()) {
            //We need to wait for the reset to complete. Disable int (will be re-enabled on reset callback) and exit isr.
            esp_intr_disable(host->intr);
            if (do_yield) portYIELD_FROM_ISR();
            return;
        }
    }

    //Grab next transaction
    r = xQueueReceiveFromISR(host->trans_queue, &trans, &do_yield);
    if (!r) {
        //No packet waiting. Disable interrupt.
        esp_intr_disable(host->intr);
    } else {
        //We have a transaction. Send it.
        host->hw->slave.trans_done = 0; //clear int bit
        host->cur_trans = trans;

        if (host->dma_chan != 0) {
            spicommon_dmaworkaround_transfer_active(host->dma_chan);
            host->hw->dma_conf.val |= SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST;
            host->hw->dma_out_link.start = 0;
            host->hw->dma_in_link.start = 0;
            host->hw->dma_conf.val &= ~(SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST);
            host->hw->dma_conf.out_data_burst_en = 0;
            host->hw->dma_conf.indscr_burst_en = 0;
            host->hw->dma_conf.outdscr_burst_en = 0;

            //Fill DMA descriptors
            if (trans->rx_buffer) {
                host->hw->user.usr_miso_highpart = 0;
                spicommon_setup_dma_desc_links(host->dmadesc_rx, ((trans->length + 7) / 8), trans->rx_buffer, true);
                host->hw->dma_in_link.addr = (int)(&host->dmadesc_rx[0]) & 0xFFFFF;
                host->hw->dma_in_link.start = 1;
            }

            if (trans->tx_buffer) {
                spicommon_setup_dma_desc_links(host->dmadesc_tx, (trans->length + 7) / 8, trans->tx_buffer, false);
                host->hw->user.usr_mosi_highpart = 0;
                host->hw->dma_out_link.addr = (int)(&host->dmadesc_tx[0]) & 0xFFFFF;
                host->hw->dma_out_link.start = 1;
            }

            host->hw->slave.sync_reset = 1;
            host->hw->slave.sync_reset = 0;

        } else {
            //No DMA. Turn off SPI and copy data to transmit buffers.
            host->hw->cmd.usr = 0;
            host->hw->slave.sync_reset = 1;
            host->hw->slave.sync_reset = 0;

            host->hw->user.usr_miso_highpart = 0;
            host->hw->user.usr_mosi_highpart = 0;
            if (trans->tx_buffer) {
                const uint32_t *data = host->cur_trans->tx_buffer;
                for (int x = 0; x < trans->length; x += 32) {
                    uint32_t word;
                    memcpy(&word, &data[x / 32], 4);
                    host->hw->data_buf[(x / 32)] = word;
                }
            }
        }

        host->hw->slv_rd_bit.slv_rdata_bit = 0;
        host->hw->slv_wrbuf_dlen.bit_len = trans->length - 1;
        host->hw->slv_rdbuf_dlen.bit_len = trans->length - 1;
        host->hw->mosi_dlen.usr_mosi_dbitlen = trans->length - 1;
        host->hw->miso_dlen.usr_miso_dbitlen = trans->length - 1;
        host->hw->user.usr_mosi = (trans->tx_buffer == NULL) ? 0 : 1;
        host->hw->user.usr_miso = (trans->rx_buffer == NULL) ? 0 : 1;

        //Kick off transfer
        host->hw->cmd.usr = 1;
        if (host->cfg.post_setup_cb) host->cfg.post_setup_cb(trans);
    }
    if (do_yield) portYIELD_FROM_ISR();
}

