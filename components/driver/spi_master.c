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

/*
Architecture:

We can initialize a SPI driver, but we don't talk to the SPI driver itself, we address a device. A device essentially
is a combination of SPI port and CS pin, plus some information about the specifics of communication to the device
(timing, command/address length etc). The arbitration between tasks is also in conception of devices.

A device can work in interrupt mode and polling mode, and a third but
complicated mode which combines the two modes above:

1. Work in the ISR with a set of queues; one per device.

   The idea is that to send something to a SPI device, you allocate a
   transaction descriptor. It contains some information about the transfer
   like the lenghth, address, command etc, plus pointers to transmit and
   receive buffer. The address of this block gets pushed into the transmit
   queue. The SPI driver does its magic, and sends and retrieves the data
   eventually. The data gets written to the receive buffers, if needed the
   transaction descriptor is modified to indicate returned parameters and
   the entire thing goes into the return queue, where whatever software
   initiated the transaction can retrieve it.

   The entire thing is run from the SPI interrupt handler. If SPI is done
   transmitting/receiving but nothing is in the queue, it will not clear the
   SPI interrupt but just disable it by esp_intr_disable. This way, when a
   new thing is sent, pushing the packet into the send queue and re-enabling
   the interrupt (by esp_intr_enable) will trigger the interrupt again, which
   can then take care of the sending.

2. Work in the polling mode in the task.

   In this mode we get rid of the ISR, FreeRTOS queue and task switching, the
   task is no longer blocked during a transaction. This increase the cpu
   load, but decrease the interval of SPI transactions. Each time only one
   device (in one task) can send polling transactions, transactions to
   other devices are blocked until the polling transaction of current device
   is done.

   In the polling mode, the queue is not used, all the operations are done
   in the task. The task calls ``spi_device_polling_start`` to setup and start
   a new transaction, then call ``spi_device_polling_end`` to handle the
   return value of the transaction.

   To handle the arbitration among devices, the device "temporarily" acquire
   a bus by the ``device_acquire_bus_internal`` function, which writes
   acquire_cs by CAS operation. Other devices which wants to send polling
   transactions but don't own the bus will block and wait until given the
   semaphore which indicates the ownership of bus.

   In case of the ISR is still sending transactions to other devices, the ISR
   should maintain an ``isr_free`` flag indicating that it's not doing
   transactions. When the bus is acquired, the ISR can only send new
   transactions to the acquiring device. The ISR will automatically disable
   itself and send semaphore to the device if the ISR is free. If the device
   sees the isr_free flag, it can directly start its polling transaction.
   Otherwise it should block and wait for the semaphore from the ISR.

   After the polling transaction, the driver will release the bus. During the
   release of the bus, the driver search all other devices to see whether
   there is any device waiting to acquire the bus, if so, acquire for it and
   send it a semaphore if the device queue is empty, or invoke the ISR for
   it. If all other devices don't need to acquire the bus, but there are
   still transactions in the queues, the ISR will also be invoked.

   To get better polling efficiency, user can call ``spi_device_acquire_bus``
   function, which also calls the ``device_acquire_bus_internal`` function,
   before a series of polling transactions to a device. The bus acquiring and
   task switching before and after the polling transaction will be escaped.

3. Mixed mode

   The driver is written under the assumption that polling and interrupt
   transactions are not happening simultaneously. When sending polling
   transactions, it will check whether the ISR is active, which includes the
   case the ISR is sending the interrupt transactions of the acquiring
   device. If the ISR is still working, the routine sending a polling
   transaction will get blocked and wait until the semaphore from the ISR
   which indicates the ISR is free now.

   A fatal case is, a polling transaction is in flight, but the ISR received
   an interrupt transaction. The behavior of the driver is unpredictable,
   which should be strictly forbidden.

We have two bits to control the interrupt:

1. The slave->trans_done bit, which is automatically asserted when a transaction is done.

   This bit is cleared during an interrupt transaction, so that the interrupt
   will be triggered when the transaction is done, or the SW can check the
   bit to see if the transaction is done for polling transactions.

   When no transaction is in-flight, the bit is kept active, so that the SW
   can easily invoke the ISR by enable the interrupt.

2. The system interrupt enable/disable, controlled by esp_intr_enable and esp_intr_disable.

   The interrupt is disabled (by the ISR itself) when no interrupt transaction
   is queued. When the bus is not occupied, any task, which queues a
   transaction into the queue, will enable the interrupt to invoke the ISR.
   When the bus is occupied by a device, other device will put off the
   invoking of ISR to the moment when the bus is released. The device
   acquiring the bus can still send interrupt transactions by enable the
   interrupt.

*/

#include <string.h>
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "soc/dport_reg.h"
#include "soc/spi_periph.h"
#include "esp32/rom/ets_sys.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_pm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/task.h"
#include "soc/soc.h"
#include "soc/soc_memory_layout.h"
#include "soc/dport_reg.h"
#include "esp32/rom/lldesc.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp_heap_caps.h"
#include "stdatomic.h"

typedef struct spi_device_t spi_device_t;
typedef typeof(SPI1.clock) spi_clock_reg_t;

#define NO_CS 3     //Number of CS pins per SPI host

#ifdef CONFIG_SPI_MASTER_ISR_IN_IRAM
#define SPI_MASTER_ISR_ATTR IRAM_ATTR
#else
#define SPI_MASTER_ISR_ATTR
#endif

#ifdef CONFIG_SPI_MASTER_IN_IRAM
#define SPI_MASTER_ATTR IRAM_ATTR
#else
#define SPI_MASTER_ATTR
#endif


/// struct to hold private transaction data (like tx and rx buffer for DMA).
typedef struct {
    spi_transaction_t   *trans;
    const uint32_t *buffer_to_send;   //equals to tx_data, if SPI_TRANS_USE_RXDATA is applied; otherwise if original buffer wasn't in DMA-capable memory, this gets the address of a temporary buffer that is;
                                //otherwise sets to the original buffer or NULL if no buffer is assigned.
    uint32_t *buffer_to_rcv;    // similar to buffer_to_send
} spi_trans_priv_t;

typedef struct {
    _Atomic(spi_device_t*) device[NO_CS];
    intr_handle_t intr;
    spi_dev_t *hw;
    spi_trans_priv_t cur_trans_buf;
    int cur_cs;     //current device doing transaction
    int prev_cs;    //last device doing transaction, used to avoid re-configure registers if the device not changed
    atomic_int acquire_cs; //the device acquiring the bus, NO_CS if no one is doing so.
    bool polling;   //in process of a polling, avoid of queue new transactions into ISR
    bool isr_free;  //the isr is not sending transactions
    bool bus_locked;//the bus is controlled by a device
    lldesc_t *dmadesc_tx;
    lldesc_t *dmadesc_rx;
    uint32_t flags;
    int dma_chan;
    int max_transfer_sz;
    spi_bus_config_t bus_cfg;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
} spi_host_t;

typedef struct {
    spi_clock_reg_t reg;
    int eff_clk;
    int dummy_num;
    int miso_delay;
} clock_config_t;

struct spi_device_t {
    int id;
    QueueHandle_t trans_queue;
    QueueHandle_t ret_queue;
    spi_device_interface_config_t cfg;
    clock_config_t clk_cfg;
    spi_host_t *host;
    SemaphoreHandle_t semphr_polling;   //semaphore to notify the device it claimed the bus
    bool        waiting;                //the device is waiting for the exclusive control of the bus
};

static spi_host_t *spihost[3];


static const char *SPI_TAG = "spi_master";
#define SPI_CHECK(a, str, ret_val, ...) \
    if (!(a)) { \
        ESP_LOGE(SPI_TAG,"%s(%d): "str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val); \
    }


static void spi_intr(void *arg);

esp_err_t spi_bus_initialize(spi_host_device_t host, const spi_bus_config_t *bus_config, int dma_chan)
{
    bool spi_chan_claimed, dma_chan_claimed;
    esp_err_t ret = ESP_OK;
    esp_err_t err;
    /* ToDo: remove this when we have flash operations cooperating with this */
    SPI_CHECK(host!=SPI_HOST, "SPI1 is not supported", ESP_ERR_NOT_SUPPORTED);

    SPI_CHECK(host>=SPI_HOST && host<=VSPI_HOST, "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK( dma_chan >= 0 && dma_chan <= 2, "invalid dma channel", ESP_ERR_INVALID_ARG );
    SPI_CHECK((bus_config->intr_flags & (ESP_INTR_FLAG_HIGH|ESP_INTR_FLAG_EDGE|ESP_INTR_FLAG_INTRDISABLED))==0, "intr flag not allowed", ESP_ERR_INVALID_ARG);
#ifndef CONFIG_SPI_MASTER_ISR_IN_IRAM
    SPI_CHECK((bus_config->intr_flags & ESP_INTR_FLAG_IRAM)==0, "ESP_INTR_FLAG_IRAM should be disabled when CONFIG_SPI_MASTER_ISR_IN_IRAM is not set.", ESP_ERR_INVALID_ARG);
#endif

    spi_chan_claimed=spicommon_periph_claim(host, "spi master");
    SPI_CHECK(spi_chan_claimed, "host already in use", ESP_ERR_INVALID_STATE);

    if ( dma_chan != 0 ) {
        dma_chan_claimed=spicommon_dma_chan_claim(dma_chan);
        if ( !dma_chan_claimed ) {
            spicommon_periph_free( host );
            SPI_CHECK(false, "dma channel already in use", ESP_ERR_INVALID_STATE);
        }
    }

    spihost[host]=malloc(sizeof(spi_host_t));
    if (spihost[host]==NULL) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }
    memset(spihost[host], 0, sizeof(spi_host_t));
    memcpy( &spihost[host]->bus_cfg, bus_config, sizeof(spi_bus_config_t));
#ifdef CONFIG_PM_ENABLE
    err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "spi_master",
            &spihost[host]->pm_lock);
    if (err != ESP_OK) {
        ret = err;
        goto cleanup;
    }
#endif //CONFIG_PM_ENABLE

    err = spicommon_bus_initialize_io(host, bus_config, dma_chan, SPICOMMON_BUSFLAG_MASTER|bus_config->flags, &spihost[host]->flags);
    if (err != ESP_OK) {
        ret = err;
        goto cleanup;
    }

    spihost[host]->dma_chan=dma_chan;
    if (dma_chan == 0) {
        spihost[host]->max_transfer_sz = 64;
    } else {
        //See how many dma descriptors we need and allocate them
        int dma_desc_ct=(bus_config->max_transfer_sz+SPI_MAX_DMA_LEN-1)/SPI_MAX_DMA_LEN;
        if (dma_desc_ct==0) dma_desc_ct = 1; //default to 4k when max is not given

        spihost[host]->max_transfer_sz = dma_desc_ct*SPI_MAX_DMA_LEN;
        spihost[host]->dmadesc_tx=heap_caps_malloc(sizeof(lldesc_t)*dma_desc_ct, MALLOC_CAP_DMA);
        spihost[host]->dmadesc_rx=heap_caps_malloc(sizeof(lldesc_t)*dma_desc_ct, MALLOC_CAP_DMA);
        if (!spihost[host]->dmadesc_tx || !spihost[host]->dmadesc_rx) {
            ret = ESP_ERR_NO_MEM;
            goto cleanup;
        }
    }

    int flags = bus_config->intr_flags | ESP_INTR_FLAG_INTRDISABLED;
    err = esp_intr_alloc(spicommon_irqsource_for_host(host), flags, spi_intr, (void*)spihost[host], &spihost[host]->intr);
    if (err != ESP_OK) {
        ret = err;
        goto cleanup;
    }
    spihost[host]->hw=spicommon_hw_for_host(host);

    spihost[host]->cur_cs = NO_CS;
    spihost[host]->prev_cs = NO_CS;
    atomic_store(&spihost[host]->acquire_cs, NO_CS);
    spihost[host]->polling = false;
    spihost[host]->isr_free = true;
    spihost[host]->bus_locked = false;

    //Reset DMA
    spihost[host]->hw->dma_conf.val|=SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST;
    spihost[host]->hw->dma_out_link.start=0;
    spihost[host]->hw->dma_in_link.start=0;
    spihost[host]->hw->dma_conf.val&=~(SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST);
    //Reset timing
    spihost[host]->hw->ctrl2.val=0;

    //master use all 64 bytes of the buffer
    spihost[host]->hw->user.usr_miso_highpart=0;
    spihost[host]->hw->user.usr_mosi_highpart=0;

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

cleanup:
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
    spihost[host] = NULL;
    spicommon_periph_free(host);
    spicommon_dma_chan_free(dma_chan);
    return ret;
}

esp_err_t spi_bus_free(spi_host_device_t host)
{
    int x;
    SPI_CHECK(host>=SPI_HOST && host<=VSPI_HOST, "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host]!=NULL, "host not in use", ESP_ERR_INVALID_STATE);
    for (x=0; x<NO_CS; x++) {
        SPI_CHECK(atomic_load(&spihost[host]->device[x])==NULL, "not all CSses freed", ESP_ERR_INVALID_STATE);
    }
    spicommon_bus_free_io_cfg(&spihost[host]->bus_cfg);

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

void spi_get_timing(bool gpio_is_used, int input_delay_ns, int eff_clk, int* dummy_o, int* cycles_remain_o)
{
    const int apbclk_kHz = APB_CLK_FREQ/1000;
    //calculate how many apb clocks a period has
    const int apbclk_n = APB_CLK_FREQ/eff_clk;
    const int gpio_delay_ns = gpio_is_used ? 25 : 0;

    //calculate how many apb clocks the delay is, the 1 is to compensate in case ``input_delay_ns`` is rounded off.
    int apb_period_n = (1 + input_delay_ns + gpio_delay_ns)*apbclk_kHz/1000/1000;
    if (apb_period_n < 0) apb_period_n = 0;

    int dummy_required = apb_period_n/apbclk_n;

    int miso_delay = 0;
    if (dummy_required > 0) {
        //due to the clock delay between master and slave, there's a range in which data is random
        //give MISO a delay if needed to make sure we sample at the time MISO is stable
        miso_delay = (dummy_required+1)*apbclk_n-apb_period_n-1;
    } else {
        //if the dummy is not required, maybe we should also delay half a SPI clock if the data comes too early
        if (apb_period_n*4 <= apbclk_n) miso_delay = -1;
    }
    if (dummy_o!=NULL) *dummy_o = dummy_required;
    if (cycles_remain_o!=NULL) *cycles_remain_o = miso_delay;
    ESP_LOGD(SPI_TAG,"eff: %d, limit: %dk(/%d), %d dummy, %d delay", eff_clk/1000, apbclk_kHz/(apb_period_n+1), apb_period_n, dummy_required, miso_delay);
}

int spi_get_freq_limit(bool gpio_is_used, int input_delay_ns)
{
    const int apbclk_kHz = APB_CLK_FREQ/1000;
    const int gpio_delay_ns = gpio_is_used ? 25 : 0;

    //calculate how many apb clocks the delay is, the 1 is to compensate in case ``input_delay_ns`` is rounded off.
    int apb_period_n = (1 + input_delay_ns + gpio_delay_ns)*apbclk_kHz/1000/1000;
    if (apb_period_n < 0) apb_period_n = 0;

    return APB_CLK_FREQ/(apb_period_n+1);
}

/*
 Add a device. This allocates a CS line for the device, allocates memory for the device structure and hooks
 up the CS pin to whatever is specified.
*/
esp_err_t spi_bus_add_device(spi_host_device_t host, const spi_device_interface_config_t *dev_config, spi_device_handle_t *handle)
{
    int freecs;
    int apbclk=APB_CLK_FREQ;
    int eff_clk;
    int duty_cycle;
    int dummy_required;
    int miso_delay;

    spi_clock_reg_t clk_reg;
    SPI_CHECK(host>=SPI_HOST && host<=VSPI_HOST, "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host]!=NULL, "host not initialized", ESP_ERR_INVALID_STATE);
    SPI_CHECK(dev_config->spics_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(dev_config->spics_io_num), "spics pin invalid", ESP_ERR_INVALID_ARG);
    SPI_CHECK(dev_config->clock_speed_hz > 0, "invalid sclk speed", ESP_ERR_INVALID_ARG);
    for (freecs=0; freecs<NO_CS; freecs++) {
        //See if this slot is free; reserve if it is by putting a dummy pointer in the slot. We use an atomic compare&swap to make this thread-safe.
        void* null=NULL;
        if (atomic_compare_exchange_strong(&spihost[host]->device[freecs], &null, (spi_device_t *)1)) break;
    }
    SPI_CHECK(freecs!=NO_CS, "no free cs pins for host", ESP_ERR_NOT_FOUND);
    //The hardware looks like it would support this, but actually setting cs_ena_pretrans when transferring in full
    //duplex mode does absolutely nothing on the ESP32.
    SPI_CHECK( dev_config->cs_ena_pretrans <= 1 || (dev_config->address_bits == 0 && dev_config->command_bits == 0) ||
        (dev_config->flags & SPI_DEVICE_HALFDUPLEX), "In full-duplex mode, only support cs pretrans delay = 1 and without address_bits and command_bits", ESP_ERR_INVALID_ARG);

    duty_cycle = (dev_config->duty_cycle_pos==0) ? 128 : dev_config->duty_cycle_pos;
    eff_clk = spi_cal_clock(apbclk, dev_config->clock_speed_hz, duty_cycle, (uint32_t*)&clk_reg);
    int freq_limit = spi_get_freq_limit(!(spihost[host]->flags&SPICOMMON_BUSFLAG_NATIVE_PINS), dev_config->input_delay_ns);

    //Speed >=40MHz over GPIO matrix needs a dummy cycle, but these don't work for full-duplex connections.
    spi_get_timing(!(spihost[host]->flags&SPICOMMON_BUSFLAG_NATIVE_PINS), dev_config->input_delay_ns, eff_clk, &dummy_required, &miso_delay);
    SPI_CHECK( dev_config->flags & SPI_DEVICE_HALFDUPLEX || dummy_required == 0 ||
            dev_config->flags & SPI_DEVICE_NO_DUMMY,
"When work in full-duplex mode at frequency > %.1fMHz, device cannot read correct data.\n\
Try to use IOMUX pins to increase the frequency limit, or use the half duplex mode.\n\
Please note the SPI master can only work at divisors of 80MHz, and the driver always tries to find the closest frequency to your configuration.\n\
Specify ``SPI_DEVICE_NO_DUMMY`` to ignore this checking. Then you can output data at higher speed, or read data at your own risk.",
            ESP_ERR_INVALID_ARG, freq_limit/1000./1000 );

    //Allocate memory for device
    spi_device_t *dev=malloc(sizeof(spi_device_t));
    if (dev==NULL) goto nomem;
    memset(dev, 0, sizeof(spi_device_t));
    atomic_store(&spihost[host]->device[freecs], dev);
    dev->id = freecs;
    dev->waiting = false;

    //Allocate queues, set defaults
    dev->trans_queue = xQueueCreate(dev_config->queue_size, sizeof(spi_trans_priv_t));
    dev->ret_queue = xQueueCreate(dev_config->queue_size, sizeof(spi_trans_priv_t));
    dev->semphr_polling = xSemaphoreCreateBinary();
    if (!dev->trans_queue || !dev->ret_queue || !dev->semphr_polling) {
        goto nomem;
    }
    dev->host=spihost[host];

    //We want to save a copy of the dev config in the dev struct.
    memcpy(&dev->cfg, dev_config, sizeof(spi_device_interface_config_t));
    dev->cfg.duty_cycle_pos = duty_cycle;
    // TODO: if we have to change the apb clock among transactions, re-calculate this each time the apb clock lock is acquired.
    dev->clk_cfg= (clock_config_t) {
        .eff_clk = eff_clk,
        .dummy_num = dummy_required,
        .reg = clk_reg,
        .miso_delay = miso_delay,
    };

    //Set CS pin, CS options
    if (dev_config->spics_io_num >= 0) {
        spicommon_cs_initialize(host, dev_config->spics_io_num, freecs, !(spihost[host]->flags&SPICOMMON_BUSFLAG_NATIVE_PINS));
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
    spihost[host]->hw->ctrl2.mosi_delay_mode = 0;
    spihost[host]->hw->ctrl2.mosi_delay_num = 0;
    *handle=dev;
    ESP_LOGD(SPI_TAG, "SPI%d: New device added to CS%d, effective clock: %dkHz", host+1, freecs, dev->clk_cfg.eff_clk/1000);
    return ESP_OK;

nomem:
    if (dev) {
        if (dev->trans_queue) vQueueDelete(dev->trans_queue);
        if (dev->ret_queue) vQueueDelete(dev->ret_queue);
        if (dev->semphr_polling) vSemaphoreDelete(dev->semphr_polling);
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
    SPI_CHECK(handle->host->cur_cs == NO_CS || atomic_load(&handle->host->device[handle->host->cur_cs])!=handle, "Have unfinished transactions", ESP_ERR_INVALID_STATE);
    SPI_CHECK(uxQueueMessagesWaiting(handle->ret_queue)==0, "Have unfinished transactions", ESP_ERR_INVALID_STATE);

    //return
    int spics_io_num = handle->cfg.spics_io_num;
    if (spics_io_num >= 0) spicommon_cs_free_io(spics_io_num);

    //Kill queues
    vQueueDelete(handle->trans_queue);
    vQueueDelete(handle->ret_queue);
    vSemaphoreDelete(handle->semphr_polling);
    //Remove device from list of csses and free memory
    for (x=0; x<NO_CS; x++) {
        if (atomic_load(&handle->host->device[x]) == handle){
            atomic_store(&handle->host->device[x], NULL);
            if (x == handle->host->prev_cs) handle->host->prev_cs = NO_CS;
        }
    }
    free(handle);
    return ESP_OK;
}

static int spi_freq_for_pre_n(int fapb, int pre, int n)
{
    return (fapb / (pre * n));
}

int spi_cal_clock(int fapb, int hz, int duty_cycle, uint32_t *reg_o)
{
    spi_clock_reg_t reg;
    int eff_clk;

    //In hw, n, h and l are 1-64, pre is 1-8K. Value written to register is one lower than used value.
    if (hz>((fapb/4)*3)) {
        //Using Fapb directly will give us the best result here.
        reg.clkcnt_l=0;
        reg.clkcnt_h=0;
        reg.clkcnt_n=0;
        reg.clkdiv_pre=0;
        reg.clk_equ_sysclk=1;
        eff_clk=fapb;
    } else {
        //For best duty cycle resolution, we want n to be as close to 32 as possible, but
        //we also need a pre/n combo that gets us as close as possible to the intended freq.
        //To do this, we bruteforce n and calculate the best pre to go along with that.
        //If there's a choice between pre/n combos that give the same result, use the one
        //with the higher n.
        int pre, n, h, l;
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

        reg.clk_equ_sysclk=0;
        reg.clkcnt_n=n-1;
        reg.clkdiv_pre=pre-1;
        reg.clkcnt_h=h-1;
        reg.clkcnt_l=l-1;
        eff_clk=spi_freq_for_pre_n(fapb, pre, n);
    }
    if (reg_o != NULL) *reg_o = reg.val;
    return eff_clk;
}

/*
 * Set the spi clock according to pre-calculated register value.
 */
static inline void SPI_MASTER_ISR_ATTR spi_set_clock(spi_dev_t *hw, spi_clock_reg_t reg)
{
    hw->clock.val = reg.val;
}

// Setup the device-specified configuration registers. Called every time a new
// transaction is to be sent, but only apply new configurations when the device
// changes.
static void SPI_MASTER_ISR_ATTR spi_setup_device(spi_host_t *host, int dev_id )
{
    //if the configuration is already applied, skip the following.
    if (dev_id == host->prev_cs) {
        return;
    }

    ESP_EARLY_LOGD(SPI_TAG, "SPI device changed from %d to %d", host->prev_cs, dev_id);
    spi_device_t *dev = atomic_load(&host->device[dev_id]);
    //Configure clock settings
    spi_set_clock(host->hw, dev->clk_cfg.reg);
    //Configure bit order
    host->hw->ctrl.rd_bit_order=(dev->cfg.flags & SPI_DEVICE_RXBIT_LSBFIRST) ? 1 : 0;
    host->hw->ctrl.wr_bit_order=(dev->cfg.flags & SPI_DEVICE_TXBIT_LSBFIRST) ? 1 : 0;

    //Configure polarity
    if (dev->cfg.mode==0) {
        host->hw->pin.ck_idle_edge=0;
        host->hw->user.ck_out_edge=0;
    } else if (dev->cfg.mode==1) {
        host->hw->pin.ck_idle_edge=0;
        host->hw->user.ck_out_edge=1;
    } else if (dev->cfg.mode==2) {
        host->hw->pin.ck_idle_edge=1;
        host->hw->user.ck_out_edge=1;
    } else if (dev->cfg.mode==3) {
        host->hw->pin.ck_idle_edge=1;
        host->hw->user.ck_out_edge=0;
    }
    //Configure misc stuff
    host->hw->user.doutdin=(dev->cfg.flags & SPI_DEVICE_HALFDUPLEX) ? 0 : 1;
    host->hw->user.sio=(dev->cfg.flags & SPI_DEVICE_3WIRE) ? 1 : 0;
    //Configure CS pin and timing
    host->hw->ctrl2.setup_time=dev->cfg.cs_ena_pretrans-1;
    host->hw->user.cs_setup=dev->cfg.cs_ena_pretrans ? 1 : 0;
    //set hold_time to 0 will not actually append delay to CS
    //set it to 1 since we do need at least one clock of hold time in most cases
    host->hw->ctrl2.hold_time=dev->cfg.cs_ena_posttrans;
    if (host->hw->ctrl2.hold_time == 0) host->hw->ctrl2.hold_time = 1;
    host->hw->user.cs_hold=1;

    host->hw->pin.cs0_dis = (dev_id == 0) ? 0 : 1;
    host->hw->pin.cs1_dis = (dev_id == 1) ? 0 : 1;
    host->hw->pin.cs2_dis = (dev_id == 2) ? 0 : 1;
    //Record the device just configured to save time for next time
    host->prev_cs = dev_id;
}

/*-----------------------------------------------------------------------------
    Arbitration Functions
-----------------------------------------------------------------------------*/

static inline void spi_isr_invoke(spi_device_t *dev)
{
    int acquire_cs = atomic_load(&dev->host->acquire_cs);
    if (acquire_cs == dev->id || acquire_cs == NO_CS) {
        esp_intr_enable(dev->host->intr);
    }
    //otherwise wait for bus release to invoke
}

/*  This function try to race for the arbitration between devices.
 *  Even if this returns successfully, the ISR may be still running.
 *  Call device_wait_for_isr_idle to make sure the ISR is done.
 */
static SPI_MASTER_ISR_ATTR esp_err_t device_acquire_bus_internal(spi_device_t *handle, TickType_t wait)
{
    spi_host_t *host = handle->host;
    SPI_CHECK(wait==portMAX_DELAY, "acquire finite time not supported now.", ESP_ERR_INVALID_ARG);

    if (atomic_load(&host->acquire_cs) == handle->id) {
        // Quickly skip if the bus is already acquired.
        // Usually this is only when the bus is locked.
        assert(host->bus_locked);
        return ESP_OK;
    } else {
        // Declare we are waiting for the bus so that if we get blocked later, other device or the ISR will yield to us after their using.
        handle->waiting = true;
        // Clear the semaphore before checking
        xSemaphoreTake(handle->semphr_polling, 0);

        int no_cs = NO_CS;
        atomic_compare_exchange_weak(&host->acquire_cs, &no_cs, handle->id);
        if (atomic_load(&host->acquire_cs) != handle->id) {
            //block until the bus is acquired (help by other task)
            BaseType_t ret = xSemaphoreTake(handle->semphr_polling, wait);
            //TODO: add timeout handling here.
            if (ret == pdFALSE) return ESP_ERR_TIMEOUT;
        }
        handle->waiting = false;
    }
    return ESP_OK;
}

/*  This function check for whether the ISR is done, if not, block until semaphore given.
 */
static inline SPI_MASTER_ISR_ATTR esp_err_t device_wait_for_isr_idle(spi_device_t *handle, TickType_t wait)
{
    //quickly skip if the isr is already free
    if (!handle->host->isr_free) {
        // Clear the semaphore before checking
        xSemaphoreTake(handle->semphr_polling, 0);
        if (!handle->host->isr_free) {
            //block until the the isr is free and give us the semaphore
            BaseType_t ret = xSemaphoreTake(handle->semphr_polling, wait);
            //TODO: add timeout handling here.
            if (ret == pdFALSE) return ESP_ERR_TIMEOUT;
        }
    }
    return ESP_OK;
}

/*  This function release the bus acquired by device_acquire_internal.
    And it also tries to help other device to acquire the bus.
    If the bus acquring is not needed, it goes through all device queues to see whether to invoke the ISR
 */
static SPI_MASTER_ISR_ATTR void device_release_bus_internal(spi_host_t *host)
{
    //release the bus
    atomic_store(&host->acquire_cs, NO_CS);
    //first try to restore the acquiring device
    for (int i = 0; i < NO_CS; i++) {
        //search for all registered devices
        spi_device_t* dev = atomic_load(&host->device[i]);
        if (dev && dev->waiting) {
            int no_cs = NO_CS;
            atomic_compare_exchange_weak(&host->acquire_cs, &no_cs, i);
            if (atomic_load(&host->acquire_cs) == i) {
                // Success to acquire for new device
                BaseType_t ret = uxQueueMessagesWaiting(dev->trans_queue);
                if (ret > 0) {
                    // If there are transactions in the queue, the ISR should be invoked first
                    // Resume the interrupt to send the task a signal
                    spi_isr_invoke(dev);
                } else {
                    // Otherwise resume the task directly.
                    xSemaphoreGive(dev->semphr_polling);
                }
            }
            return;
        }
    }
    //if no devices waiting, searching in device queues to see whether to recover the ISR
    for( int i = 0; i < NO_CS; i++) {
        spi_device_t *dev = atomic_load(&host->device[i]);
        if (dev == NULL) continue;
        BaseType_t ret = uxQueueMessagesWaiting(dev->trans_queue);
        if ( ret != 0) {
            spi_isr_invoke(dev);
            return;
        }
    }
}

static inline SPI_MASTER_ISR_ATTR bool device_is_polling(spi_device_t *handle)
{
    return atomic_load(&handle->host->acquire_cs) == handle->id && handle->host->polling;
}

/*-----------------------------------------------------------------------------
    Working Functions
-----------------------------------------------------------------------------*/

// The function is called to send a new transaction, in ISR or in the task.
// Setup the transaction-specified registers and linked-list used by the DMA (or FIFO if DMA is not used)
static void SPI_MASTER_ISR_ATTR spi_new_trans(spi_device_t *dev, spi_trans_priv_t *trans_buf)
{
    spi_transaction_t *trans = NULL;
    spi_host_t *host = dev->host;
    int dev_id = dev->id;

    //clear int bit
    host->hw->slave.trans_done = 0;

    trans = trans_buf->trans;
    host->cur_cs = dev_id;
    //We should be done with the transmission.
    assert(host->hw->cmd.usr == 0);

    //Reconfigure according to device settings, the function only has effect when the dev_id is changed.
    spi_setup_device(host, dev_id);

    //Reset DMA peripheral
    host->hw->dma_conf.val |= SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST;
    host->hw->dma_out_link.start=0;
    host->hw->dma_in_link.start=0;
    host->hw->dma_conf.val &= ~(SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST);
    host->hw->dma_conf.out_data_burst_en=1;
    host->hw->dma_conf.indscr_burst_en=1;
    host->hw->dma_conf.outdscr_burst_en=1;
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
    int extra_dummy=0;
    if (trans_buf->buffer_to_rcv) {
        if (host->dma_chan == 0) {
            //No need to setup anything; we'll copy the result out of the work registers directly later.
        } else {
            spicommon_setup_dma_desc_links(host->dmadesc_rx, ((trans->rxlength+7)/8), (uint8_t*)trans_buf->buffer_to_rcv, true);
            host->hw->dma_in_link.addr=(int)(&host->dmadesc_rx[0]) & 0xFFFFF;
            host->hw->dma_in_link.start=1;
        }
        //when no_dummy is not set and in half-duplex mode, sets the dummy bit if RX phase exist
        if (((dev->cfg.flags&SPI_DEVICE_NO_DUMMY)==0) && (dev->cfg.flags&SPI_DEVICE_HALFDUPLEX)) {
            extra_dummy=dev->clk_cfg.dummy_num;
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
                host->hw->data_buf[(x/32)]=word;
            }
        } else {
            spicommon_setup_dma_desc_links(host->dmadesc_tx, (trans->length+7)/8, (uint8_t*)trans_buf->buffer_to_send, false);
            host->hw->dma_out_link.addr=(int)(&host->dmadesc_tx[0]) & 0xFFFFF;
            host->hw->dma_out_link.start=1;
        }
    }

    //SPI iface needs to be configured for a delay in some cases.
    //configure dummy bits
    int base_dummy_bits;
    if (trans->flags & SPI_TRANS_VARIABLE_DUMMY) {
        base_dummy_bits = ((spi_transaction_ext_t *)trans)->dummy_bits;
    } else {
        base_dummy_bits = dev->cfg.dummy_bits;
    }
    host->hw->user.usr_dummy=(base_dummy_bits+extra_dummy) ? 1 : 0;
    host->hw->user1.usr_dummy_cyclelen=base_dummy_bits+extra_dummy-1;

    int miso_long_delay = 0;
    if (dev->clk_cfg.miso_delay<0) {
        //if the data comes too late, delay half a SPI clock to improve reading
        miso_long_delay = 1;
        host->hw->ctrl2.miso_delay_num = 0;
    } else {
        //if the data is so fast that dummy_bit is used, delay some apb clocks to meet the timing
        host->hw->ctrl2.miso_delay_num = extra_dummy ? dev->clk_cfg.miso_delay : 0;
    }

    if (miso_long_delay) {
        switch (dev->cfg.mode) {
        case 0:
            host->hw->ctrl2.miso_delay_mode = 2;
            break;
        case 1:
            host->hw->ctrl2.miso_delay_mode = 1;
            break;
        case 2:
            host->hw->ctrl2.miso_delay_mode = 1;
            break;
        case 3:
            host->hw->ctrl2.miso_delay_mode = 2;
            break;
        }
    } else {
        host->hw->ctrl2.miso_delay_mode = 0;
    }

    host->hw->mosi_dlen.usr_mosi_dbitlen=trans->length-1;
    if ( dev->cfg.flags & SPI_DEVICE_HALFDUPLEX ) {
        host->hw->miso_dlen.usr_miso_dbitlen=trans->rxlength-1;
    } else {
        //rxlength is not used in full-duplex mode
        host->hw->miso_dlen.usr_miso_dbitlen=trans->length-1;
    }

    //Configure bit sizes, load addr and command
    int cmdlen;
    int addrlen;
    if (!(dev->cfg.flags & SPI_DEVICE_HALFDUPLEX) && dev->cfg.cs_ena_pretrans != 0) {
        /* The command and address phase is not compatible with cs_ena_pretrans
         * in full duplex mode.
         */
        cmdlen = 0;
        addrlen = 0;
    } else {
        if (trans->flags & SPI_TRANS_VARIABLE_CMD) {
            cmdlen = ((spi_transaction_ext_t *)trans)->command_bits;
        } else {
            cmdlen = dev->cfg.command_bits;
        }
        if (trans->flags & SPI_TRANS_VARIABLE_ADDR) {
            addrlen = ((spi_transaction_ext_t *)trans)->address_bits;
        } else {
            addrlen = dev->cfg.address_bits;
        }
    }

    host->hw->user1.usr_addr_bitlen=addrlen-1;
    host->hw->user2.usr_command_bitlen=cmdlen-1;
    host->hw->user.usr_addr=addrlen ? 1 : 0;
    host->hw->user.usr_command=cmdlen ? 1 : 0;

    if ((dev->cfg.flags & SPI_DEVICE_TXBIT_LSBFIRST)==0) {
        /* Output command will be sent from bit 7 to 0 of command_value, and
         * then bit 15 to 8 of the same register field. Shift and swap to send
         * more straightly.
         */
        host->hw->user2.usr_command_value = SPI_SWAP_DATA_TX(trans->cmd, cmdlen);

        // shift the address to MSB of addr (and maybe slv_wr_status) register.
        // output address will be sent from MSB to LSB of addr register, then comes the MSB to LSB of slv_wr_status register.
        if (addrlen > 32) {
            host->hw->addr = trans->addr >> (addrlen - 32);
            host->hw->slv_wr_status = trans->addr << (64 - addrlen);
        } else {
            host->hw->addr = trans->addr << (32 - addrlen);
        }
    } else {
        /* The output command start from bit0 to bit 15, kept as is.
         * The output address start from the LSB of the highest byte, i.e.
         * addr[24] -> addr[31]
         * ...
         * addr[0] -> addr[7]
         * slv_wr_status[24] -> slv_wr_status[31]
         * ...
         * slv_wr_status[0] -> slv_wr_status[7]
         * So swap the byte order to let the LSB sent first.
         */
        host->hw->user2.usr_command_value = trans->cmd;
        uint64_t addr = __builtin_bswap64(trans->addr);
        host->hw->addr = addr>>32;
        host->hw->slv_wr_status = addr;
    }

    if ((!(dev->cfg.flags & SPI_DEVICE_HALFDUPLEX) && trans_buf->buffer_to_rcv) ||
        trans_buf->buffer_to_send) {
        host->hw->user.usr_mosi = 1;
    } else {
        host->hw->user.usr_mosi = 0;
    }
    host->hw->user.usr_miso = (trans_buf->buffer_to_rcv) ? 1 : 0;

    //Call pre-transmission callback, if any
    if (dev->cfg.pre_cb) dev->cfg.pre_cb(trans);
    //Kick off transfer
    host->hw->cmd.usr=1;
}

// The function is called when a transaction is done, in ISR or in the task.
// Fetch the data from FIFO and call the ``post_cb``.
static void SPI_MASTER_ISR_ATTR spi_post_trans(spi_host_t *host)
{
    spi_transaction_t *cur_trans = host->cur_trans_buf.trans;
    if (host->cur_trans_buf.buffer_to_rcv && host->dma_chan == 0 ) {
        //Need to copy from SPI regs to result buffer.
        for (int x = 0; x < cur_trans->rxlength; x += 32) {
            //Do a memcpy to get around possible alignment issues in rx_buffer
            uint32_t word = host->hw->data_buf[x / 32];
            int len = cur_trans->rxlength - x;
            if (len > 32) len = 32;
            memcpy(&host->cur_trans_buf.buffer_to_rcv[x / 32], &word, (len + 7) / 8);
        }
    }
    //Call post-transaction callback, if any
    spi_device_t* dev = atomic_load(&host->device[host->cur_cs]);
    if (dev->cfg.post_cb) dev->cfg.post_cb(cur_trans);

    host->cur_cs = NO_CS;
}

// This is run in interrupt context.
static void SPI_MASTER_ISR_ATTR spi_intr(void *arg)
{
    int i;
    BaseType_t r;
    BaseType_t do_yield = pdFALSE;
    spi_host_t *host = (spi_host_t *)arg;

    assert(host->hw->slave.trans_done == 1);

    /*------------ deal with the in-flight transaction -----------------*/
    if (host->cur_cs != NO_CS) {
        //Okay, transaction is done.
        const int cs = host->cur_cs;
        //Tell common code DMA workaround that our DMA channel is idle. If needed, the code will do a DMA reset.
        if (host->dma_chan) {
            spicommon_dmaworkaround_idle(host->dma_chan);
        }

        //cur_cs is changed to NO_CS here
        spi_post_trans(host);
        //Return transaction descriptor.
        xQueueSendFromISR(atomic_load(&host->device[cs])->ret_queue, &host->cur_trans_buf, &do_yield);
#ifdef CONFIG_PM_ENABLE
        //Release APB frequency lock
        esp_pm_lock_release(host->pm_lock);
#endif
    }

    /*------------ new transaction starts here ------------------*/
    assert(host->cur_cs == NO_CS);

    // Clear isr_free before the checking of acquire_cs so that the task will always block if we find the bus is not acquired.
    // Small possiblility that the task is blocked but we find the bus is acquired.
    host->isr_free = false;

    /* When the bus is not occupied, the task uses esp_intr_enable to inform the ISR there's new transaction.
     * If the queue is empty, we disable the system interrupt.
     * We disable this first, to avoid the conflict when the task enable and the ISR disable at the same time
     * If the transaction is sent (queue not empty), we will re-ebale it (see below).
     */
    esp_intr_disable( host->intr );
    int acquire_cs = atomic_load(&host->acquire_cs);
    if (acquire_cs != NO_CS) {
        // Only look in the queue of the occupying device.
        i = acquire_cs;
        spi_device_t* dev = atomic_load(&host->device[i]);
        assert(dev);
        r = xQueueReceiveFromISR(dev->trans_queue, &host->cur_trans_buf, &do_yield);
        // If the Queue is empty, skip the sending by setting i=NO_CS
        // Otherwise i is kept as is and the transaction will be sent.
        if (!r) {
            // Set the free to true before resume the task
            host->isr_free = true;
            xSemaphoreGiveFromISR(dev->semphr_polling, &do_yield);
            i = NO_CS;
        }
    } else {
        //Go through all device queues to find a transaction to send
        //ToDo: This is a stupidly simple low-cs-first priority scheme. Make this configurable somehow. - JD
        for (i = 0; i < NO_CS; i++) {
            spi_device_t* dev = atomic_load(&host->device[i]);
            if (dev) {
                r = xQueueReceiveFromISR(dev->trans_queue, &host->cur_trans_buf, &do_yield);
                //Stop looking if we have a transaction to send.
                if (r) break;
            }
        }
        if (i==NO_CS) {
            host->isr_free = true;
        }
    }

    // Actually send the transaction
    if (i != NO_CS) {
        spi_trans_priv_t *const cur_trans_buf = &host->cur_trans_buf;
        if (host->dma_chan != 0 && (cur_trans_buf->buffer_to_rcv || cur_trans_buf->buffer_to_send)) {
            //mark channel as active, so that the DMA will not be reset by the slave
            spicommon_dmaworkaround_transfer_active(host->dma_chan);
        }
        spi_new_trans(atomic_load(&host->device[i]), cur_trans_buf);
        //re-enable interrupt disabled above
        esp_intr_enable(host->intr);
    }
    if (do_yield) portYIELD_FROM_ISR();
}

static SPI_MASTER_ISR_ATTR esp_err_t check_trans_valid(spi_device_handle_t handle, spi_transaction_t *trans_desc)
{
    SPI_CHECK(handle!=NULL, "invalid dev handle", ESP_ERR_INVALID_ARG);
    spi_host_t *host = handle->host;
    //check transmission length
    SPI_CHECK((trans_desc->flags & SPI_TRANS_USE_RXDATA)==0 ||trans_desc->rxlength <= 32, "rxdata transfer > 32 bits without configured DMA", ESP_ERR_INVALID_ARG);
    SPI_CHECK((trans_desc->flags & SPI_TRANS_USE_TXDATA)==0 ||trans_desc->length <= 32, "txdata transfer > 32 bits without configured DMA", ESP_ERR_INVALID_ARG);
    SPI_CHECK(trans_desc->length <= handle->host->max_transfer_sz*8, "txdata transfer > host maximum", ESP_ERR_INVALID_ARG);
    SPI_CHECK(trans_desc->rxlength <= handle->host->max_transfer_sz*8, "rxdata transfer > host maximum", ESP_ERR_INVALID_ARG);
    SPI_CHECK((handle->cfg.flags & SPI_DEVICE_HALFDUPLEX) || trans_desc->rxlength <= trans_desc->length, "rx length > tx length in full duplex mode", ESP_ERR_INVALID_ARG);
    //check working mode
    SPI_CHECK(!((trans_desc->flags & (SPI_TRANS_MODE_DIO|SPI_TRANS_MODE_QIO)) && (handle->cfg.flags & SPI_DEVICE_3WIRE)), "incompatible iface params", ESP_ERR_INVALID_ARG);
    SPI_CHECK(!((trans_desc->flags & (SPI_TRANS_MODE_DIO|SPI_TRANS_MODE_QIO)) && (!(handle->cfg.flags & SPI_DEVICE_HALFDUPLEX))), "incompatible iface params", ESP_ERR_INVALID_ARG);
    SPI_CHECK( !(handle->cfg.flags & SPI_DEVICE_HALFDUPLEX) || host->dma_chan == 0 || !(trans_desc->flags & SPI_TRANS_USE_RXDATA || trans_desc->rx_buffer != NULL)
        || !(trans_desc->flags & SPI_TRANS_USE_TXDATA || trans_desc->tx_buffer!=NULL), "SPI half duplex mode does not support using DMA with both MOSI and MISO phases.", ESP_ERR_INVALID_ARG );
    //MOSI phase is skipped only when both tx_buffer and SPI_TRANS_USE_TXDATA are not set.
    SPI_CHECK(trans_desc->length != 0 || (trans_desc->tx_buffer == NULL && !(trans_desc->flags & SPI_TRANS_USE_TXDATA)),
        "trans tx_buffer should be NULL and SPI_TRANS_USE_TXDATA should be cleared to skip MOSI phase.", ESP_ERR_INVALID_ARG);
    //MISO phase is skipped only when both rx_buffer and SPI_TRANS_USE_RXDATA are not set.
    //If set rxlength=0 in full_duplex mode, it will be automatically set to length
    SPI_CHECK(!(handle->cfg.flags & SPI_DEVICE_HALFDUPLEX) || trans_desc->rxlength != 0 ||
        (trans_desc->rx_buffer == NULL && ((trans_desc->flags & SPI_TRANS_USE_RXDATA)==0)),
        "trans rx_buffer should be NULL and SPI_TRANS_USE_RXDATA should be cleared to skip MISO phase.", ESP_ERR_INVALID_ARG);
    //In Full duplex mode, default rxlength to be the same as length, if not filled in.
    // set rxlength to length is ok, even when rx buffer=NULL
    if (trans_desc->rxlength==0 && !(handle->cfg.flags & SPI_DEVICE_HALFDUPLEX)) {
        trans_desc->rxlength=trans_desc->length;
    }

    return ESP_OK;
}

static SPI_MASTER_ISR_ATTR void uninstall_priv_desc(spi_trans_priv_t* trans_buf)
{
    spi_transaction_t *trans_desc = trans_buf->trans;
    if ((void *)trans_buf->buffer_to_send != &trans_desc->tx_data[0] &&
        trans_buf->buffer_to_send != trans_desc->tx_buffer) {
        free((void *)trans_buf->buffer_to_send); //force free, ignore const
    }
    //copy data from temporary DMA-capable buffer back to IRAM buffer and free the temporary one.
    if ((void *)trans_buf->buffer_to_rcv != &trans_desc->rx_data[0] &&
        trans_buf->buffer_to_rcv != trans_desc->rx_buffer) {
        if (trans_desc->flags & SPI_TRANS_USE_RXDATA) {
            memcpy((uint8_t *) & trans_desc->rx_data[0], trans_buf->buffer_to_rcv, (trans_desc->rxlength + 7) / 8);
        } else {
            memcpy(trans_desc->rx_buffer, trans_buf->buffer_to_rcv, (trans_desc->rxlength + 7) / 8);
        }
        free(trans_buf->buffer_to_rcv);
    }
}

static SPI_MASTER_ISR_ATTR esp_err_t setup_priv_desc(spi_transaction_t *trans_desc, spi_trans_priv_t* new_desc, bool isdma)
{
    *new_desc = (spi_trans_priv_t) { .trans = trans_desc, };

    // rx memory assign
    uint32_t* rcv_ptr;
    if ( trans_desc->flags & SPI_TRANS_USE_RXDATA ) {
        rcv_ptr = (uint32_t *)&trans_desc->rx_data[0];
    } else {
        //if not use RXDATA neither rx_buffer, buffer_to_rcv assigned to NULL
        rcv_ptr = trans_desc->rx_buffer;
    }
    if (rcv_ptr && isdma && (!esp_ptr_dma_capable(rcv_ptr) || ((int)rcv_ptr % 4 != 0))) {
        //if rxbuf in the desc not DMA-capable, malloc a new one. The rx buffer need to be length of multiples of 32 bits to avoid heap corruption.
        ESP_LOGI( SPI_TAG, "Allocate RX buffer for DMA" );
        rcv_ptr = heap_caps_malloc((trans_desc->rxlength + 31) / 8, MALLOC_CAP_DMA);
        if (rcv_ptr == NULL) goto clean_up;
    }
    new_desc->buffer_to_rcv = rcv_ptr;

    // tx memory assign
    const uint32_t *send_ptr;
    if ( trans_desc->flags & SPI_TRANS_USE_TXDATA ) {
        send_ptr = (uint32_t *)&trans_desc->tx_data[0];
    } else {
        //if not use TXDATA neither tx_buffer, tx data assigned to NULL
        send_ptr = trans_desc->tx_buffer ;
    }
    if (send_ptr && isdma && !esp_ptr_dma_capable( send_ptr )) {
        //if txbuf in the desc not DMA-capable, malloc a new one
        ESP_LOGI( SPI_TAG, "Allocate TX buffer for DMA" );
        uint32_t *temp = heap_caps_malloc((trans_desc->length + 7) / 8, MALLOC_CAP_DMA);
        if (temp == NULL) goto clean_up;

        memcpy( temp, send_ptr, (trans_desc->length + 7) / 8 );
        send_ptr = temp;
        }
    new_desc->buffer_to_send = send_ptr;

    return ESP_OK;

clean_up:
    uninstall_priv_desc(new_desc);
    return ESP_ERR_NO_MEM;
}

esp_err_t SPI_MASTER_ATTR spi_device_queue_trans(spi_device_handle_t handle, spi_transaction_t *trans_desc,  TickType_t ticks_to_wait)
{
    esp_err_t ret = check_trans_valid(handle, trans_desc);
    if (ret != ESP_OK) return ret;

    spi_host_t *host = handle->host;

    SPI_CHECK( !device_is_polling(handle), "Cannot queue new transaction while previous polling transaction is not terminated.", ESP_ERR_INVALID_STATE );

    spi_trans_priv_t trans_buf;
    ret = setup_priv_desc(trans_desc, &trans_buf, (host->dma_chan!=0));
    if (ret != ESP_OK) return ret;

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(host->pm_lock);
#endif
    //Send to queue and invoke the ISR.

    BaseType_t r = xQueueSend(handle->trans_queue, (void *)&trans_buf, ticks_to_wait);
    if (!r) {
        ret = ESP_ERR_TIMEOUT;
#ifdef CONFIG_PM_ENABLE
        //Release APB frequency lock
        esp_pm_lock_release(host->pm_lock);
#endif
        goto clean_up;
    }
    spi_isr_invoke(handle);
    return ESP_OK;

clean_up:
    uninstall_priv_desc(&trans_buf);
    return ret;
}

esp_err_t SPI_MASTER_ATTR spi_device_get_trans_result(spi_device_handle_t handle, spi_transaction_t **trans_desc, TickType_t ticks_to_wait)
{
    BaseType_t r;
    spi_trans_priv_t trans_buf;
    SPI_CHECK(handle!=NULL, "invalid dev handle", ESP_ERR_INVALID_ARG);

    //use the interrupt, block until return
    r=xQueueReceive(handle->ret_queue, (void*)&trans_buf, ticks_to_wait);
    if (!r) {
        // The memory occupied by rx and tx DMA buffer destroyed only when receiving from the queue (transaction finished).
        // If timeout, wait and retry.
        // Every in-flight transaction request occupies internal memory as DMA buffer if needed.
        return ESP_ERR_TIMEOUT;
    }
    //release temporary buffers
    uninstall_priv_desc(&trans_buf);
    (*trans_desc) = trans_buf.trans;

    return ESP_OK;
}

//Porcelain to do one blocking transmission.
esp_err_t SPI_MASTER_ATTR spi_device_transmit(spi_device_handle_t handle, spi_transaction_t *trans_desc)
{
    esp_err_t ret;
    spi_transaction_t *ret_trans;
    //ToDo: check if any spi transfers in flight
    ret = spi_device_queue_trans(handle, trans_desc, portMAX_DELAY);
    if (ret != ESP_OK) return ret;

    ret = spi_device_get_trans_result(handle, &ret_trans, portMAX_DELAY);
    if (ret != ESP_OK) return ret;

    assert(ret_trans == trans_desc);
    return ESP_OK;
}

esp_err_t SPI_MASTER_ATTR spi_device_acquire_bus(spi_device_t *device, TickType_t wait)
{
    spi_host_t *const host = device->host;
    SPI_CHECK(wait==portMAX_DELAY, "acquire finite time not supported now.", ESP_ERR_INVALID_ARG);
    SPI_CHECK( !device_is_polling(device), "Cannot acquire bus when a polling transaction is in progress.", ESP_ERR_INVALID_STATE );

    esp_err_t ret = device_acquire_bus_internal(device, portMAX_DELAY);
    if (ret != ESP_OK) return ret;
    ret = device_wait_for_isr_idle(device, portMAX_DELAY);
    if (ret != ESP_OK) return ret;

    device->host->bus_locked = true;

    ESP_LOGD(SPI_TAG, "device%d acquired the bus", device->id);

#ifdef CONFIG_PM_ENABLE
    // though we don't suggest to block the task before ``release_bus``, still allow doing so.
    // this keeps the spi clock at 80MHz even if all tasks are blocked
    esp_pm_lock_acquire(device->host->pm_lock);
#endif
    //configure the device so that we don't need to do it again in the following transactions
    spi_setup_device( host, device->id );
    //the DMA is also occupied by the device, all the slave devices that using DMA should wait until bus released.
    if (host->dma_chan != 0) {
        spicommon_dmaworkaround_transfer_active(host->dma_chan);
    }
    return ESP_OK;
}

// This function restore configurations required in the non-polling mode
void SPI_MASTER_ATTR spi_device_release_bus(spi_device_t *dev)
{
    spi_host_t *host = dev->host;

    if (device_is_polling(dev)){
        ESP_LOGE(SPI_TAG, "Cannot release bus when a polling transaction is in progress.");
        assert(0);
    }

    if (host->dma_chan != 0) {
        spicommon_dmaworkaround_idle(host->dma_chan);
    }
    //Tell common code DMA workaround that our DMA channel is idle. If needed, the code will do a DMA reset.

    //allow clock to be lower than 80MHz when all tasks blocked
#ifdef CONFIG_PM_ENABLE
    //Release APB frequency lock
    esp_pm_lock_release(host->pm_lock);
#endif
    ESP_LOGD(SPI_TAG, "device%d release bus", dev->id);

    dev->host->bus_locked = false;
    device_release_bus_internal(dev->host);
}

esp_err_t SPI_MASTER_ISR_ATTR spi_device_polling_start(spi_device_handle_t handle, spi_transaction_t *trans_desc, TickType_t ticks_to_wait)
{
    esp_err_t ret;
    SPI_CHECK(ticks_to_wait == portMAX_DELAY, "currently timeout is not available for polling transactions", ESP_ERR_INVALID_ARG);

    spi_host_t *host = handle->host;
    ret = check_trans_valid(handle, trans_desc);
    if (ret!=ESP_OK) return ret;

    SPI_CHECK( !device_is_polling(handle), "Cannot send polling transaction while the previous polling transaction is not terminated.", ESP_ERR_INVALID_STATE );

    ret = setup_priv_desc(trans_desc, &host->cur_trans_buf, (handle->host->dma_chan!=0));
    if (ret!=ESP_OK) return ret;

    device_acquire_bus_internal(handle, portMAX_DELAY);
    device_wait_for_isr_idle(handle, portMAX_DELAY);

    assert(atomic_load(&host->acquire_cs) == handle->id);
    assert(host->isr_free);

    //Polling, no interrupt is used.
    host->polling = true;

    ESP_LOGV(SPI_TAG, "polling trans");
    spi_new_trans(handle, &host->cur_trans_buf);

    return ESP_OK;
}

esp_err_t SPI_MASTER_ISR_ATTR spi_device_polling_end(spi_device_handle_t handle, TickType_t ticks_to_wait)
{
    SPI_CHECK(handle != NULL, "invalid dev handle", ESP_ERR_INVALID_ARG);
    spi_host_t *host = handle->host;

    //if (host->acquire_cs == handle->id && host->polling) {
    assert(host->cur_cs == atomic_load(&host->acquire_cs));
    TickType_t start = xTaskGetTickCount();

    while (!host->hw->slave.trans_done) {
        TickType_t end = xTaskGetTickCount();
        if (end - start > ticks_to_wait) {
            return ESP_ERR_TIMEOUT;
        }
    }
    ESP_LOGV(SPI_TAG, "polling trans done");
    //deal with the in-flight transaction
    spi_post_trans(host);
    //release temporary buffers
    uninstall_priv_desc(&host->cur_trans_buf);
    host->polling = false;

    if (!host->bus_locked) {
        device_release_bus_internal(host);
    }

    return ESP_OK;
}

esp_err_t SPI_MASTER_ISR_ATTR spi_device_polling_transmit(spi_device_handle_t handle, spi_transaction_t* trans_desc)
{
    esp_err_t ret;
    ret = spi_device_polling_start(handle, trans_desc, portMAX_DELAY);
    if (ret != ESP_OK) return ret;

    ret = spi_device_polling_end(handle, portMAX_DELAY);
    if (ret != ESP_OK) return ret;

    return ESP_OK;
}


