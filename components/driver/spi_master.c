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

/*
Architecture:

We can initialize a SPI driver, but we don't talk to the SPI driver itself, we address a device. A device essentially
is a combination of SPI port and CS pin, plus some information about the specifics of communication to the device
(timing, command/address length etc)

The essence of the interface to a device is a set of queues; one per device. The idea is that to send something to a SPI
device, you allocate a transaction descriptor. It contains some information about the transfer like the lenghth, address,
command etc, plus pointers to transmit and receive buffer. The address of this block gets pushed into the transmit queue. 
The SPI driver does its magic, and sends and retrieves the data eventually. The data gets written to the receive buffers, 
if needed the transaction descriptor is modified to indicate returned parameters and the entire thing goes into the return
queue, where whatever software initiated the transaction can retrieve it.

The entire thing is run from the SPI interrupt handler. If SPI is done transmitting/receiving but nothing is in the queue, 
it will not clear the SPI interrupt but just disable it. This way, when a new thing is sent, pushing the packet into the send 
queue and re-enabling the interrupt will trigger the interrupt again, which can then take care of the sending.
*/



#include <string.h>
#include "driver/spi_common.h"
#include "driver/spi_master.h"
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

typedef struct spi_device_t spi_device_t;

#define NO_CS 3     //Number of CS pins per SPI host


/// struct to hold private transaction data (like tx and rx buffer for DMA).
typedef struct {        
    spi_transaction_t   *trans; 
    uint32_t *buffer_to_send;   //equals to tx_data, if SPI_TRANS_USE_RXDATA is applied; otherwise if original buffer wasn't in DMA-capable memory, this gets the address of a temporary buffer that is;
                                //otherwise sets to the original buffer or NULL if no buffer is assigned.
    uint32_t *buffer_to_rcv;    // similar to buffer_to_send
} spi_trans_priv;

typedef struct {
    spi_device_t *device[NO_CS];
    intr_handle_t intr;
    spi_dev_t *hw;
    spi_trans_priv cur_trans_buf;
    int cur_cs;
    lldesc_t *dmadesc_tx;
    lldesc_t *dmadesc_rx;
    bool no_gpio_matrix;
    int dma_chan;
    int max_transfer_sz;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
} spi_host_t;

struct spi_device_t {
    QueueHandle_t trans_queue;
    QueueHandle_t ret_queue;
    spi_device_interface_config_t cfg;
    spi_host_t *host;
};

static spi_host_t *spihost[3];


static const char *SPI_TAG = "spi_master";
#define SPI_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(SPI_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }


static void spi_intr(void *arg);

esp_err_t spi_bus_initialize(spi_host_device_t host, const spi_bus_config_t *bus_config, int dma_chan)
{
    bool native, spi_chan_claimed, dma_chan_claimed;
    /* ToDo: remove this when we have flash operations cooperating with this */
    SPI_CHECK(host!=SPI_HOST, "SPI1 is not supported", ESP_ERR_NOT_SUPPORTED);

    SPI_CHECK(host>=SPI_HOST && host<=VSPI_HOST, "invalid host", ESP_ERR_INVALID_ARG);
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

    spihost[host]=malloc(sizeof(spi_host_t));
    if (spihost[host]==NULL) goto nomem;
    memset(spihost[host], 0, sizeof(spi_host_t));
#ifdef CONFIG_PM_ENABLE
    esp_err_t err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "spi_master",
            &spihost[host]->pm_lock);
    if (err != ESP_OK) {
        goto nomem;
    }
#endif //CONFIG_PM_ENABLE
    
    spicommon_bus_initialize_io(host, bus_config, dma_chan, SPICOMMON_BUSFLAG_MASTER|SPICOMMON_BUSFLAG_QUAD, &native);
    spihost[host]->no_gpio_matrix=native;
    
    spihost[host]->dma_chan=dma_chan;
    if (dma_chan == 0) {
        spihost[host]->max_transfer_sz = 32;
    } else {
        //See how many dma descriptors we need and allocate them
        int dma_desc_ct=(bus_config->max_transfer_sz+SPI_MAX_DMA_LEN-1)/SPI_MAX_DMA_LEN;
        if (dma_desc_ct==0) dma_desc_ct=1; //default to 4k when max is not given
        spihost[host]->max_transfer_sz = dma_desc_ct*SPI_MAX_DMA_LEN;
        spihost[host]->dmadesc_tx=heap_caps_malloc(sizeof(lldesc_t)*dma_desc_ct, MALLOC_CAP_DMA);
        spihost[host]->dmadesc_rx=heap_caps_malloc(sizeof(lldesc_t)*dma_desc_ct, MALLOC_CAP_DMA);
        if (!spihost[host]->dmadesc_tx || !spihost[host]->dmadesc_rx) goto nomem;
    }
    esp_intr_alloc(spicommon_irqsource_for_host(host), ESP_INTR_FLAG_INTRDISABLED, spi_intr, (void*)spihost[host], &spihost[host]->intr);
    spihost[host]->hw=spicommon_hw_for_host(host);

    spihost[host]->cur_cs = NO_CS;

    //Reset DMA
    spihost[host]->hw->dma_conf.val|=SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST;
    spihost[host]->hw->dma_out_link.start=0;
    spihost[host]->hw->dma_in_link.start=0;
    spihost[host]->hw->dma_conf.val&=~(SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST);
    //Reset timing
    spihost[host]->hw->ctrl2.val=0;

    //Disable unneeded ints
    spihost[host]->hw->slave.rd_buf_done=0;
    spihost[host]->hw->slave.wr_buf_done=0;
    spihost[host]->hw->slave.rd_sta_done=0;
    spihost[host]->hw->slave.wr_sta_done=0;
    spihost[host]->hw->slave.rd_buf_inten=0;
    spihost[host]->hw->slave.wr_buf_inten=0;
    spihost[host]->hw->slave.rd_sta_inten=0;
    spihost[host]->hw->slave.wr_sta_inten=0;

    //Force a transaction done interrupt. This interrupt won't fire yet because we initialized the SPI interrupt as
    //disabled.  This way, we can just enable the SPI interrupt and the interrupt handler will kick in, handling 
    //any transactions that are queued.
    spihost[host]->hw->slave.trans_inten=1;
    spihost[host]->hw->slave.trans_done=1;

    return ESP_OK;

nomem:
    if (spihost[host]) {
        free(spihost[host]->dmadesc_tx);
        free(spihost[host]->dmadesc_rx);
#ifdef CONFIG_PM_ENABLE
        if (spihost[host]->pm_lock) {
            esp_pm_lock_delete(spihost[host]->pm_lock);
        }
#endif
    }
    free(spihost[host]);
    spicommon_periph_free(host);
    spicommon_dma_chan_free(dma_chan);
    return ESP_ERR_NO_MEM;
}

esp_err_t spi_bus_free(spi_host_device_t host)
{
    int x;
    SPI_CHECK(host>=SPI_HOST && host<=VSPI_HOST, "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host]!=NULL, "host not in use", ESP_ERR_INVALID_STATE);
    for (x=0; x<NO_CS; x++) {
        SPI_CHECK(spihost[host]->device[x]==NULL, "not all CSses freed", ESP_ERR_INVALID_STATE);
    }

    if ( spihost[host]->dma_chan > 0 ) {
        spicommon_dma_chan_free ( spihost[host]->dma_chan );
    }
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_delete(spihost[host]->pm_lock);
#endif
    spihost[host]->hw->slave.trans_inten=0;
    spihost[host]->hw->slave.trans_done=0;
    esp_intr_free(spihost[host]->intr);
    spicommon_periph_free(host);
    free(spihost[host]->dmadesc_tx);
    free(spihost[host]->dmadesc_rx);
    free(spihost[host]);
    spihost[host]=NULL;
    return ESP_OK;
}

/*
 Add a device. This allocates a CS line for the device, allocates memory for the device structure and hooks
 up the CS pin to whatever is specified.
*/
esp_err_t spi_bus_add_device(spi_host_device_t host, spi_device_interface_config_t *dev_config, spi_device_handle_t *handle)
{
    int freecs;
    int apbclk=APB_CLK_FREQ;
    SPI_CHECK(host>=SPI_HOST && host<=VSPI_HOST, "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host]!=NULL, "host not initialized", ESP_ERR_INVALID_STATE);
    SPI_CHECK(dev_config->spics_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(dev_config->spics_io_num), "spics pin invalid", ESP_ERR_INVALID_ARG);
    SPI_CHECK(dev_config->clock_speed_hz > 0, "invalid sclk speed", ESP_ERR_INVALID_ARG);
    for (freecs=0; freecs<NO_CS; freecs++) {
        //See if this slot is free; reserve if it is by putting a dummy pointer in the slot. We use an atomic compare&swap to make this thread-safe.
        if (__sync_bool_compare_and_swap(&spihost[host]->device[freecs], NULL, (spi_device_t *)1)) break;
    }
    SPI_CHECK(freecs!=NO_CS, "no free cs pins for host", ESP_ERR_NOT_FOUND);
    //The hardware looks like it would support this, but actually setting cs_ena_pretrans when transferring in full
    //duplex mode does absolutely nothing on the ESP32.
    SPI_CHECK(dev_config->cs_ena_pretrans==0 || (dev_config->flags & SPI_DEVICE_HALFDUPLEX), "cs pretrans delay incompatible with full-duplex", ESP_ERR_INVALID_ARG);
    //Speeds >=40MHz over GPIO matrix needs a dummy cycle, but these don't work for full-duplex connections.
    SPI_CHECK(!( ((dev_config->flags & SPI_DEVICE_HALFDUPLEX)==0) && (dev_config->clock_speed_hz > ((apbclk*2)/5)) && (!spihost[host]->no_gpio_matrix)),
            "No speeds >26MHz supported for full-duplex, GPIO-matrix SPI transfers", ESP_ERR_INVALID_ARG);

    //Allocate memory for device
    spi_device_t *dev=malloc(sizeof(spi_device_t));
    if (dev==NULL) goto nomem;
    memset(dev, 0, sizeof(spi_device_t));
    spihost[host]->device[freecs]=dev;

    //Allocate queues, set defaults
    dev->trans_queue=xQueueCreate(dev_config->queue_size, sizeof(spi_trans_priv));
    dev->ret_queue=xQueueCreate(dev_config->queue_size, sizeof(spi_trans_priv));
    if (!dev->trans_queue || !dev->ret_queue) goto nomem;
    if (dev_config->duty_cycle_pos==0) dev_config->duty_cycle_pos=128;
    dev->host=spihost[host];

    //We want to save a copy of the dev config in the dev struct.
    memcpy(&dev->cfg, dev_config, sizeof(spi_device_interface_config_t));

    //Set CS pin, CS options
    if (dev_config->spics_io_num > 0) {
        gpio_set_direction(dev_config->spics_io_num, GPIO_MODE_OUTPUT);
        spicommon_cs_initialize(host, dev_config->spics_io_num, freecs, spihost[host]->no_gpio_matrix == false);
    }
    if (dev_config->flags&SPI_DEVICE_CLK_AS_CS) {
        spihost[host]->hw->pin.master_ck_sel |= (1<<freecs);
    } else {
        spihost[host]->hw->pin.master_ck_sel &= (1<<freecs);
    }
    if (dev_config->flags&SPI_DEVICE_POSITIVE_CS) {
        spihost[host]->hw->pin.master_cs_pol |= (1<<freecs);
    } else {
        spihost[host]->hw->pin.master_cs_pol &= (1<<freecs);
    }
    *handle=dev;
    return ESP_OK;

nomem:
    if (dev) {
        if (dev->trans_queue) vQueueDelete(dev->trans_queue);
        if (dev->ret_queue) vQueueDelete(dev->ret_queue);
    }
    free(dev);
    return ESP_ERR_NO_MEM;
}

esp_err_t spi_bus_remove_device(spi_device_handle_t handle)
{
    int x;
    SPI_CHECK(handle!=NULL, "invalid handle", ESP_ERR_INVALID_ARG);
    //These checks aren't exhaustive; another thread could sneak in a transaction inbetween. These are only here to
    //catch design errors and aren't meant to be triggered during normal operation.
    SPI_CHECK(uxQueueMessagesWaiting(handle->trans_queue)==0, "Have unfinished transactions", ESP_ERR_INVALID_STATE);
    SPI_CHECK(handle->host->cur_cs == NO_CS || handle->host->device[handle->host->cur_cs]!=handle, "Have unfinished transactions", ESP_ERR_INVALID_STATE);
    SPI_CHECK(uxQueueMessagesWaiting(handle->ret_queue)==0, "Have unfinished transactions", ESP_ERR_INVALID_STATE);

    //Kill queues
    vQueueDelete(handle->trans_queue);
    vQueueDelete(handle->ret_queue);
    //Remove device from list of csses and free memory
    for (x=0; x<NO_CS; x++) {
        if (handle->host->device[x] == handle) handle->host->device[x]=NULL;
    }
    free(handle);
    return ESP_OK;
}

static int spi_freq_for_pre_n(int fapb, int pre, int n) {
    return (fapb / (pre * n));
}

/*
 * Set the SPI clock to a certain frequency. Returns the effective frequency set, which may be slightly
 * different from the requested frequency.
 */
static int spi_set_clock(spi_dev_t *hw, int fapb, int hz, int duty_cycle) {
    int pre, n, h, l, eff_clk;

    //In hw, n, h and l are 1-64, pre is 1-8K. Value written to register is one lower than used value.
    if (hz>((fapb/4)*3)) {
        //Using Fapb directly will give us the best result here.
        hw->clock.clkcnt_l=0;
        hw->clock.clkcnt_h=0;
        hw->clock.clkcnt_n=0;
        hw->clock.clkdiv_pre=0;
        hw->clock.clk_equ_sysclk=1;
        eff_clk=fapb;
    } else {
        //For best duty cycle resolution, we want n to be as close to 32 as possible, but
        //we also need a pre/n combo that gets us as close as possible to the intended freq.
        //To do this, we bruteforce n and calculate the best pre to go along with that.
        //If there's a choice between pre/n combos that give the same result, use the one
        //with the higher n.
        int bestn=-1;
        int bestpre=-1;
        int besterr=0;
        int errval;
        for (n=2; n<=64; n++) { //Start at 2: we need to be able to set h/l so we have at least one high and one low pulse.
            //Effectively, this does pre=round((fapb/n)/hz).
            pre=((fapb/n)+(hz/2))/hz;
            if (pre<=0) pre=1;
            if (pre>8192) pre=8192;
            errval=abs(spi_freq_for_pre_n(fapb, pre, n)-hz);
            if (bestn==-1 || errval<=besterr) {
                besterr=errval;
                bestn=n;
                bestpre=pre;
            }
        }

        n=bestn;
        pre=bestpre;
        l=n;
        //This effectively does round((duty_cycle*n)/256)
        h=(duty_cycle*n+127)/256;
        if (h<=0) h=1;

        hw->clock.clk_equ_sysclk=0;
        hw->clock.clkcnt_n=n-1;
        hw->clock.clkdiv_pre=pre-1;
        hw->clock.clkcnt_h=h-1;
        hw->clock.clkcnt_l=l-1;
        eff_clk=spi_freq_for_pre_n(fapb, pre, n);
    }
    return eff_clk;
}


//This is run in interrupt context and apart from initialization and destruction, this is the only code
//touching the host (=spihost[x]) variable. The rest of the data arrives in queues. That is why there are
//no muxes in this code.
static void IRAM_ATTR spi_intr(void *arg)
{
    int i;
    int prevCs=-1;
    BaseType_t r;
    BaseType_t do_yield=pdFALSE;
    spi_trans_priv *trans_buf=NULL;
    spi_transaction_t *trans=NULL;
    spi_host_t *host=(spi_host_t*)arg;

    //Ignore all but the trans_done int.
    if (!host->hw->slave.trans_done) return;

    /*------------ deal with the in-flight transaction -----------------*/
    if (host->cur_cs != NO_CS) {
        spi_transaction_t *cur_trans = host->cur_trans_buf.trans;
        //Okay, transaction is done. 
        if (host->cur_trans_buf.buffer_to_rcv && host->dma_chan == 0 ) {
            //Need to copy from SPI regs to result buffer.
            for (int x=0; x < cur_trans->rxlength; x+=32) {
                //Do a memcpy to get around possible alignment issues in rx_buffer
                uint32_t word=host->hw->data_buf[x/32];
                int len=cur_trans->rxlength-x;
                if (len>32) len=32;
                memcpy(&host->cur_trans_buf.buffer_to_rcv[x/32], &word, (len+7)/8);
            }
        }
        //Call post-transaction callback, if any
        if (host->device[host->cur_cs]->cfg.post_cb) host->device[host->cur_cs]->cfg.post_cb(cur_trans);
        //Return transaction descriptor.
        xQueueSendFromISR(host->device[host->cur_cs]->ret_queue, &host->cur_trans_buf, &do_yield); 
        prevCs=host->cur_cs;
        host->cur_cs = NO_CS;
    }
    //Tell common code DMA workaround that our DMA channel is idle. If needed, the code will do a DMA reset.
    if (host->dma_chan) spicommon_dmaworkaround_idle(host->dma_chan);

    /*------------ new transaction starts here ------------------*/
    //ToDo: This is a stupidly simple low-cs-first priority scheme. Make this configurable somehow. - JD
    for (i=0; i<NO_CS; i++) {
        if (host->device[i]) {
            r=xQueueReceiveFromISR(host->device[i]->trans_queue, &host->cur_trans_buf, &do_yield);
            trans_buf = &host->cur_trans_buf;
            //Stop looking if we have a transaction to send.
            if (r) break;
        }
    }
    if (i==NO_CS) {
        //No packet waiting. Disable interrupt.
        esp_intr_disable(host->intr);
#ifdef CONFIG_PM_ENABLE
        //Release APB frequency lock
        esp_pm_lock_release(host->pm_lock);
#endif
    } else {
        host->hw->slave.trans_done=0; //clear int bit
        //We have a transaction. Send it.
        spi_device_t *dev=host->device[i];
        trans = trans_buf->trans;
        host->cur_cs=i;
        //We should be done with the transmission.
        assert(host->hw->cmd.usr == 0);
        
        //Reconfigure according to device settings, but only if we change CSses.
        if (i!=prevCs) {
            //Assumes a hardcoded 80MHz Fapb for now. ToDo: figure out something better once we have
            //clock scaling working.
            int apbclk=APB_CLK_FREQ;
            int effclk=spi_set_clock(host->hw, apbclk, dev->cfg.clock_speed_hz, dev->cfg.duty_cycle_pos);
            //Configure bit order
            host->hw->ctrl.rd_bit_order=(dev->cfg.flags & SPI_DEVICE_RXBIT_LSBFIRST)?1:0;
            host->hw->ctrl.wr_bit_order=(dev->cfg.flags & SPI_DEVICE_TXBIT_LSBFIRST)?1:0;
            
            //Configure polarity
            //SPI iface needs to be configured for a delay in some cases.
            int nodelay=0;
            int extra_dummy=0;
            if (host->no_gpio_matrix) {
                if (effclk >= apbclk/2) {
                    nodelay=1;
                }
            } else {
                if (effclk >= apbclk/2) {
                    nodelay=1;
                    extra_dummy=1;          //Note: This only works on half-duplex connections. spi_bus_add_device checks for this.
                } else if (effclk >= apbclk/4) {
                    nodelay=1;
                }
            }

            if (dev->cfg.mode==0) {
                host->hw->pin.ck_idle_edge=0;
                host->hw->user.ck_out_edge=0;
                host->hw->ctrl2.miso_delay_mode=nodelay?0:2;
            } else if (dev->cfg.mode==1) {
                host->hw->pin.ck_idle_edge=0;
                host->hw->user.ck_out_edge=1;
                host->hw->ctrl2.miso_delay_mode=nodelay?0:1;
            } else if (dev->cfg.mode==2) {
                host->hw->pin.ck_idle_edge=1;
                host->hw->user.ck_out_edge=1;
                host->hw->ctrl2.miso_delay_mode=nodelay?0:1;
            } else if (dev->cfg.mode==3) {
                host->hw->pin.ck_idle_edge=1;
                host->hw->user.ck_out_edge=0;
                host->hw->ctrl2.miso_delay_mode=nodelay?0:2;
            }

            //Configure bit sizes, load addr and command
            host->hw->user.usr_dummy=(dev->cfg.dummy_bits+extra_dummy)?1:0;
            host->hw->user.usr_addr=(dev->cfg.address_bits)?1:0;
            host->hw->user.usr_command=(dev->cfg.command_bits)?1:0;
            host->hw->user1.usr_addr_bitlen=dev->cfg.address_bits-1;
            host->hw->user1.usr_dummy_cyclelen=dev->cfg.dummy_bits+extra_dummy-1;
            host->hw->user2.usr_command_bitlen=dev->cfg.command_bits-1;
            //Configure misc stuff
            host->hw->user.doutdin=(dev->cfg.flags & SPI_DEVICE_HALFDUPLEX)?0:1;
            host->hw->user.sio=(dev->cfg.flags & SPI_DEVICE_3WIRE)?1:0;

            host->hw->ctrl2.setup_time=dev->cfg.cs_ena_pretrans-1;
            host->hw->user.cs_setup=dev->cfg.cs_ena_pretrans?1:0;
            host->hw->ctrl2.hold_time=dev->cfg.cs_ena_posttrans-1;
            host->hw->user.cs_hold=(dev->cfg.cs_ena_posttrans)?1:0;

            //Configure CS pin
            host->hw->pin.cs0_dis=(i==0)?0:1;
            host->hw->pin.cs1_dis=(i==1)?0:1;
            host->hw->pin.cs2_dis=(i==2)?0:1;
        }
        //Reset SPI peripheral
        host->hw->dma_conf.val |= SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST;
        host->hw->dma_out_link.start=0;
        host->hw->dma_in_link.start=0;
        host->hw->dma_conf.val &= ~(SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST);
        host->hw->dma_conf.out_data_burst_en=1;
        //Set up QIO/DIO if needed
        host->hw->ctrl.val &= ~(SPI_FREAD_DUAL|SPI_FREAD_QUAD|SPI_FREAD_DIO|SPI_FREAD_QIO);
        host->hw->user.val &= ~(SPI_FWRITE_DUAL|SPI_FWRITE_QUAD|SPI_FWRITE_DIO|SPI_FWRITE_QIO);
        if (trans->flags & SPI_TRANS_MODE_DIO) {
            if (trans->flags & SPI_TRANS_MODE_DIOQIO_ADDR) {
                host->hw->ctrl.fread_dio=1;
                host->hw->user.fwrite_dio=1;
            } else {
                host->hw->ctrl.fread_dual=1;
                host->hw->user.fwrite_dual=1;
            }
            host->hw->ctrl.fastrd_mode=1;
        } else if (trans->flags & SPI_TRANS_MODE_QIO) {
            if (trans->flags & SPI_TRANS_MODE_DIOQIO_ADDR) {
                host->hw->ctrl.fread_qio=1;
                host->hw->user.fwrite_qio=1;
            } else {
                host->hw->ctrl.fread_quad=1;
                host->hw->user.fwrite_quad=1;
            }
            host->hw->ctrl.fastrd_mode=1;
        }


        //Fill DMA descriptors
        if (trans_buf->buffer_to_rcv) {
            host->hw->user.usr_miso_highpart=0;
            if (host->dma_chan == 0) {
                //No need to setup anything; we'll copy the result out of the work registers directly later.
            } else {
                spicommon_dmaworkaround_transfer_active(host->dma_chan); //mark channel as active
                spicommon_setup_dma_desc_links(host->dmadesc_rx, ((trans->rxlength+7)/8), (uint8_t*)trans_buf->buffer_to_rcv, true);
                host->hw->dma_in_link.addr=(int)(&host->dmadesc_rx[0]) & 0xFFFFF;
                host->hw->dma_in_link.start=1;
            }
        } else {
            //DMA temporary workaround: let RX DMA work somehow to avoid the issue in ESP32 v0/v1 silicon 
            if (host->dma_chan != 0 ) {
                host->hw->dma_in_link.addr=0;
                host->hw->dma_in_link.start=1;
            }
        }

        if (trans_buf->buffer_to_send) {
            if (host->dma_chan == 0) {
                //Need to copy data to registers manually
                for (int x=0; x < trans->length; x+=32) {
                    //Use memcpy to get around alignment issues for txdata
                    uint32_t word;
                    memcpy(&word, &trans_buf->buffer_to_send[x/32], 4);
                    host->hw->data_buf[(x/32)+8]=word;
                }
                host->hw->user.usr_mosi_highpart=1;
            } else {
                spicommon_dmaworkaround_transfer_active(host->dma_chan); //mark channel as active
                spicommon_setup_dma_desc_links(host->dmadesc_tx, (trans->length+7)/8, (uint8_t*)trans_buf->buffer_to_send, false);
                host->hw->user.usr_mosi_highpart=0;
                host->hw->dma_out_link.addr=(int)(&host->dmadesc_tx[0]) & 0xFFFFF;
                host->hw->dma_out_link.start=1;
                host->hw->user.usr_mosi_highpart=0;
            }
        }

        host->hw->mosi_dlen.usr_mosi_dbitlen=trans->length-1;
        if ( dev->cfg.flags & SPI_DEVICE_HALFDUPLEX ) {
            host->hw->miso_dlen.usr_miso_dbitlen=trans->rxlength-1;
        } else {
            //rxlength is not used in full-duplex mode
            host->hw->miso_dlen.usr_miso_dbitlen=trans->length-1;
        }

        // output command will be sent from bit 7 to 0 of command_value, and then bit 15 to 8 of the same register field.
        uint16_t command = trans->cmd << (16-dev->cfg.command_bits);    //shift to MSB
        host->hw->user2.usr_command_value = (command>>8)|(command<<8);  //swap the first and second byte
        // shift the address to MSB of addr (and maybe slv_wr_status) register. 
        // output address will be sent from MSB to LSB of addr register, then comes the MSB to LSB of slv_wr_status register. 
        if (dev->cfg.address_bits>32) {
            host->hw->addr = trans->addr >> (dev->cfg.address_bits - 32);
            host->hw->slv_wr_status = trans->addr << (64 - dev->cfg.address_bits);
        } else {
            host->hw->addr = trans->addr << (32 - dev->cfg.address_bits);
        }
        host->hw->user.usr_mosi=( (!(dev->cfg.flags & SPI_DEVICE_HALFDUPLEX) && trans_buf->buffer_to_rcv) || trans_buf->buffer_to_send)?1:0;
        host->hw->user.usr_miso=(trans_buf->buffer_to_rcv)?1:0;

        //Call pre-transmission callback, if any
        if (dev->cfg.pre_cb) dev->cfg.pre_cb(trans);
        //Kick off transfer
        host->hw->cmd.usr=1;
    }
    if (do_yield) portYIELD_FROM_ISR();
}


esp_err_t spi_device_queue_trans(spi_device_handle_t handle, spi_transaction_t *trans_desc,  TickType_t ticks_to_wait)
{
    BaseType_t r;
    SPI_CHECK(handle!=NULL, "invalid dev handle", ESP_ERR_INVALID_ARG);
    //check transmission length 
    SPI_CHECK((trans_desc->flags & SPI_TRANS_USE_RXDATA)==0 ||trans_desc->rxlength <= 32, "rxdata transfer > 32 bits without configured DMA", ESP_ERR_INVALID_ARG);
    SPI_CHECK((trans_desc->flags & SPI_TRANS_USE_TXDATA)==0 ||trans_desc->length <= 32, "txdata transfer > 32 bits without configured DMA", ESP_ERR_INVALID_ARG);
    SPI_CHECK(trans_desc->length <= handle->host->max_transfer_sz*8, "txdata transfer > host maximum", ESP_ERR_INVALID_ARG);
    SPI_CHECK(trans_desc->rxlength <= handle->host->max_transfer_sz*8, "rxdata transfer > host maximum", ESP_ERR_INVALID_ARG);
    SPI_CHECK((handle->cfg.flags & SPI_DEVICE_HALFDUPLEX) || trans_desc->rxlength <= trans_desc->length, "rx length > tx length in full duplex mode", ESP_ERR_INVALID_ARG);
    //check working mode    
    SPI_CHECK(!((trans_desc->flags & (SPI_TRANS_MODE_DIO|SPI_TRANS_MODE_QIO)) && (handle->cfg.flags & SPI_DEVICE_3WIRE)), "incompatible iface params", ESP_ERR_INVALID_ARG);
    SPI_CHECK(!((trans_desc->flags & (SPI_TRANS_MODE_DIO|SPI_TRANS_MODE_QIO)) && (!(handle->cfg.flags & SPI_DEVICE_HALFDUPLEX))), "incompatible iface params", ESP_ERR_INVALID_ARG);
    SPI_CHECK( !(handle->cfg.flags & SPI_DEVICE_HALFDUPLEX) || handle->host->dma_chan == 0 || !(trans_desc->flags & SPI_TRANS_USE_RXDATA || trans_desc->rx_buffer != NULL)
        || !(trans_desc->flags & SPI_TRANS_USE_TXDATA || trans_desc->tx_buffer!=NULL), "SPI half duplex mode does not support using DMA with both MOSI and MISO phases.", ESP_ERR_INVALID_ARG );

    //In Full duplex mode, default rxlength to be the same as length, if not filled in.
    // set rxlength to length is ok, even when rx buffer=NULL
    if (trans_desc->rxlength==0 && !(handle->cfg.flags & SPI_DEVICE_HALFDUPLEX)) {
        trans_desc->rxlength=trans_desc->length;
    }

    spi_trans_priv trans_buf;
    memset( &trans_buf, 0, sizeof(spi_trans_priv) );
    trans_buf.trans = trans_desc;

    // rx memory assign
    if ( trans_desc->flags & SPI_TRANS_USE_RXDATA ) {
        trans_buf.buffer_to_rcv = (uint32_t*)&trans_desc->rx_data[0];
    } else { 
        //if not use RXDATA neither rx_buffer, buffer_to_rcv assigned to NULL
        trans_buf.buffer_to_rcv = trans_desc->rx_buffer;
    }
    if ( trans_buf.buffer_to_rcv && handle->host->dma_chan && (!esp_ptr_dma_capable( trans_buf.buffer_to_rcv ) || ((int)trans_buf.buffer_to_rcv%4!=0)) ) {
        //if rxbuf in the desc not DMA-capable, malloc a new one. The rx buffer need to be length of multiples of 32 bits to avoid heap corruption.
        trans_buf.buffer_to_rcv = heap_caps_malloc((trans_desc->rxlength+31)/8, MALLOC_CAP_DMA);
        if ( trans_buf.buffer_to_rcv==NULL ) return ESP_ERR_NO_MEM;
    }
    
    const uint32_t *txdata;
    // tx memory assign
    if ( trans_desc->flags & SPI_TRANS_USE_TXDATA ) {
        txdata = (uint32_t*)&trans_desc->tx_data[0];
    } else { 
        //if not use TXDATA neither tx_buffer, tx data assigned to NULL
        txdata = trans_desc->tx_buffer ;
    }
    if ( txdata && handle->host->dma_chan && !esp_ptr_dma_capable( txdata )) {
        //if txbuf in the desc not DMA-capable, malloc a new one
        trans_buf.buffer_to_send = heap_caps_malloc((trans_desc->length+7)/8, MALLOC_CAP_DMA);
        if ( trans_buf.buffer_to_send==NULL ) {
            // free malloc-ed buffer (if needed) before return.
            if ( (void*)trans_buf.buffer_to_rcv != trans_desc->rx_buffer && (void*)trans_buf.buffer_to_rcv != &trans_desc->rx_data[0] ) {
                free( trans_buf.buffer_to_rcv );
            }   
            return ESP_ERR_NO_MEM;
        }
        memcpy( trans_buf.buffer_to_send, txdata, (trans_desc->length+7)/8 );
    } else { 
        // else use the original buffer (forced-conversion) or assign to NULL
        trans_buf.buffer_to_send = (uint32_t*)txdata;
    }
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(handle->host->pm_lock);
#endif

    r=xQueueSend(handle->trans_queue, (void*)&trans_buf, ticks_to_wait);
    if (!r) return ESP_ERR_TIMEOUT;
    esp_intr_enable(handle->host->intr);
    return ESP_OK;
}

esp_err_t spi_device_get_trans_result(spi_device_handle_t handle, spi_transaction_t **trans_desc, TickType_t ticks_to_wait)
{
    BaseType_t r;
    spi_trans_priv trans_buf;
    
    SPI_CHECK(handle!=NULL, "invalid dev handle", ESP_ERR_INVALID_ARG);
    r=xQueueReceive(handle->ret_queue, (void*)&trans_buf, ticks_to_wait);
    if (!r) {
        // The memory occupied by rx and tx DMA buffer destroyed only when receiving from the queue (transaction finished).
        // If timeout, wait and retry. 
        // Every on-flight transaction request occupies internal memory as DMA buffer if needed.
        return ESP_ERR_TIMEOUT;
    }

    (*trans_desc) = trans_buf.trans;

    if ( (void*)trans_buf.buffer_to_send != &(*trans_desc)->tx_data[0] && trans_buf.buffer_to_send != (*trans_desc)->tx_buffer ) {
        free( trans_buf.buffer_to_send );
    }   

    //copy data from temporary DMA-capable buffer back to IRAM buffer and free the temporary one.
    if ( (void*)trans_buf.buffer_to_rcv != &(*trans_desc)->rx_data[0] && trans_buf.buffer_to_rcv != (*trans_desc)->rx_buffer ) {
        if ( (*trans_desc)->flags & SPI_TRANS_USE_RXDATA ) {
            memcpy( (uint8_t*)&(*trans_desc)->rx_data[0], trans_buf.buffer_to_rcv, ((*trans_desc)->rxlength+7)/8 );   
        } else {
            memcpy( (*trans_desc)->rx_buffer, trans_buf.buffer_to_rcv, ((*trans_desc)->rxlength+7)/8 );
        }
        free( trans_buf.buffer_to_rcv );
    }

    return ESP_OK;
}

//Porcelain to do one blocking transmission.
esp_err_t spi_device_transmit(spi_device_handle_t handle, spi_transaction_t *trans_desc)
{
    esp_err_t ret;
    spi_transaction_t *ret_trans;
    //ToDo: check if any spi transfers in flight
    ret=spi_device_queue_trans(handle, trans_desc, portMAX_DELAY);
    if (ret!=ESP_OK) return ret;
    ret=spi_device_get_trans_result(handle, &ret_trans, portMAX_DELAY);
    if (ret!=ESP_OK) return ret;
    assert(ret_trans==trans_desc);
    return ESP_OK;
}

