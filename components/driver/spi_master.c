// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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
   dev_request by CAS operation. Other devices which wants to send polling
   transactions but don't own the bus will block and wait until given the
   semaphore which indicates the ownership of bus.

   In case of the ISR is still sending transactions to other devices, the ISR
   should maintain an ``random_idle`` flag indicating that it's not doing
   transactions. When the bus is locked, the ISR can only send new
   transactions to the acquiring device. The ISR will automatically disable
   itself and send semaphore to the device if the ISR is free. If the device
   sees the random_idle flag, it can directly start its polling transaction.
   Otherwise it should block and wait for the semaphore from the ISR.

   After the polling transaction, the driver will release the bus. During the
   release of the bus, the driver search all other devices to see whether
   there is any device waiting to acquire the bus, if so, acquire for it and
   send it a semaphore if the device queue is empty, or invoke the ISR for
   it. If all other devices don't need to acquire the bus, but there are
   still transactions in the queues, the ISR will also be invoked.

   To get better polling efficiency, user can call ``spi_device_acquire_bus``
   function, which also calls the ``spi_bus_lock_acquire_core`` function,
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
#include "driver/spi_common_internal.h"
#include "driver/spi_master.h"

#include "esp_log.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/soc_memory_layout.h"
#include "driver/gpio.h"
#include "hal/spi_hal.h"


typedef struct spi_device_t spi_device_t;

/// struct to hold private transaction data (like tx and rx buffer for DMA).
typedef struct {
    spi_transaction_t   *trans;
    const uint32_t *buffer_to_send;   //equals to tx_data, if SPI_TRANS_USE_RXDATA is applied; otherwise if original buffer wasn't in DMA-capable memory, this gets the address of a temporary buffer that is;
                                //otherwise sets to the original buffer or NULL if no buffer is assigned.
    uint32_t *buffer_to_rcv;    // similar to buffer_to_send
} spi_trans_priv_t;

typedef struct {
    int id;
    spi_device_t* device[DEV_NUM_MAX];
    intr_handle_t intr;
    spi_hal_context_t  hal;
    spi_trans_priv_t cur_trans_buf;
    int cur_cs;     //current device doing transaction
    const spi_bus_attr_t* bus_attr;

    /**
     * the bus is permanently controlled by a device until `spi_bus_release_bus`` is called. Otherwise
     * the acquiring of SPI bus will be freed when `spi_device_polling_end` is called.
     */
    spi_device_t* device_acquiring_lock;

//debug information
    bool polling;   //in process of a polling, avoid of queue new transactions into ISR
} spi_host_t;

struct spi_device_t {
    int id;
    QueueHandle_t trans_queue;
    QueueHandle_t ret_queue;
    spi_device_interface_config_t cfg;
    spi_hal_timing_conf_t timing_conf;
    spi_host_t *host;

    spi_bus_lock_dev_handle_t dev_lock;
};

static spi_host_t* bus_driver_ctx[SOC_SPI_PERIPH_NUM] = {};

static const char *SPI_TAG = "spi_master";
#define SPI_CHECK(a, str, ret_val, ...) \
    if (unlikely(!(a))) { \
        ESP_LOGE(SPI_TAG,"%s(%d): "str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val); \
    }


static void spi_intr(void *arg);
static void spi_bus_intr_enable(void *host);
static void spi_bus_intr_disable(void *host);

static esp_err_t spi_master_deinit_driver(void* arg);

static inline bool is_valid_host(spi_host_device_t host)
{
#if CONFIG_IDF_TARGET_ESP32
    return host >= SPI1_HOST && host <= SPI3_HOST;
#elif CONFIG_IDF_TARGET_ESP32S2
// SPI_HOST (SPI1_HOST) is not supported by the SPI Master driver on ESP32-S2
    return host >= SPI2_HOST && host <= SPI3_HOST;
#endif
}

// Should be called before any devices are actually registered or used.
// Currently automatically called after `spi_bus_initialize()` and when first device is registered.
static esp_err_t spi_master_init_driver(spi_host_device_t host_id)
{
    esp_err_t err = ESP_OK;

    const spi_bus_attr_t* bus_attr = spi_bus_get_attr(host_id);
    SPI_CHECK(bus_attr != NULL, "host_id not initialized", ESP_ERR_INVALID_STATE);
    // spihost contains atomic variables, which should not be put in PSRAM
    spi_host_t* host = heap_caps_malloc(sizeof(spi_host_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (host == NULL) {
        err = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    *host = (spi_host_t) {
        .id = host_id,
        .cur_cs = DEV_NUM_MAX,
        .polling = false,
        .device_acquiring_lock = NULL,
        .bus_attr = bus_attr,
    };

    if (host_id != SPI1_HOST) {
        // interrupts are not allowed on SPI1 bus
        err = esp_intr_alloc(spicommon_irqsource_for_host(host_id),
                            bus_attr->bus_cfg.intr_flags | ESP_INTR_FLAG_INTRDISABLED,
                            spi_intr, host, &host->intr);
        if (err != ESP_OK) {
            goto cleanup;
        }
    }

    spi_hal_init(&host->hal, host_id);
    host->hal.dma_enabled = (bus_attr->dma_chan != 0);
    host->hal.dmadesc_tx = bus_attr->dmadesc_tx;
    host->hal.dmadesc_rx = bus_attr->dmadesc_rx;
    host->hal.dmadesc_n = bus_attr->dma_desc_num;

    if (host_id != SPI1_HOST) {
        //SPI1 attributes are already initialized at start up.
        spi_bus_lock_handle_t lock = spi_bus_lock_get_by_id(host_id);
        spi_bus_lock_set_bg_control(lock, spi_bus_intr_enable, spi_bus_intr_disable, host);
        spi_bus_register_destroy_func(host_id, spi_master_deinit_driver, host);
    }

    bus_driver_ctx[host_id] = host;
    return ESP_OK;

cleanup:
    if (host) {
        spi_hal_deinit(&host->hal);
        if (host->intr) {
            esp_intr_free(host->intr);
        }
    }
    free(host);
    return err;
}

static esp_err_t spi_master_deinit_driver(void* arg)
{
    spi_host_t *host = (spi_host_t*)arg;
    SPI_CHECK(host != NULL, "host_id not in use", ESP_ERR_INVALID_STATE);

    int host_id = host->id;
    SPI_CHECK(is_valid_host(host_id), "invalid host_id", ESP_ERR_INVALID_ARG);

    int x;
    for (x=0; x<DEV_NUM_MAX; x++) {
        SPI_CHECK(host->device[x] == NULL, "not all CSses freed", ESP_ERR_INVALID_STATE);
    }

    spi_hal_deinit(&host->hal);

    if (host->intr) {
        esp_intr_free(host->intr);
    }
    free(host);
    bus_driver_ctx[host_id] = NULL;
    return ESP_OK;
}

void spi_get_timing(bool gpio_is_used, int input_delay_ns, int eff_clk, int* dummy_o, int* cycles_remain_o)
{
    int timing_dummy;
    int timing_miso_delay;

    spi_hal_cal_timing(eff_clk, gpio_is_used, input_delay_ns, &timing_dummy, &timing_miso_delay);
    if (dummy_o) *dummy_o = timing_dummy;
    if (cycles_remain_o) *cycles_remain_o = timing_miso_delay;
}

int spi_get_freq_limit(bool gpio_is_used, int input_delay_ns)
{
    return spi_hal_get_freq_limit(gpio_is_used, input_delay_ns);

}

/*
 Add a device. This allocates a CS line for the device, allocates memory for the device structure and hooks
 up the CS pin to whatever is specified.
*/
esp_err_t spi_bus_add_device(spi_host_device_t host_id, const spi_device_interface_config_t *dev_config, spi_device_handle_t *handle)
{
    int duty_cycle;
    spi_device_t *dev = NULL;
    esp_err_t err = ESP_OK;

    SPI_CHECK(is_valid_host(host_id), "invalid host", ESP_ERR_INVALID_ARG);
    if (bus_driver_ctx[host_id] == NULL) {
        //lasy initialization the driver, get deinitialized by the bus is freed
        err = spi_master_init_driver(host_id);
        if (err != ESP_OK) {
            return err;
        }
    }

    spi_host_t *host = bus_driver_ctx[host_id];
    const spi_bus_attr_t* bus_attr = host->bus_attr;
    SPI_CHECK(dev_config->spics_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(dev_config->spics_io_num), "spics pin invalid", ESP_ERR_INVALID_ARG);
    SPI_CHECK(dev_config->clock_speed_hz > 0, "invalid sclk speed", ESP_ERR_INVALID_ARG);
#ifdef CONFIG_IDF_TARGET_ESP32
    //The hardware looks like it would support this, but actually setting cs_ena_pretrans when transferring in full
    //duplex mode does absolutely nothing on the ESP32.
    SPI_CHECK(dev_config->cs_ena_pretrans <= 1 || (dev_config->address_bits == 0 && dev_config->command_bits == 0) ||
        (dev_config->flags & SPI_DEVICE_HALFDUPLEX), "In full-duplex mode, only support cs pretrans delay = 1 and without address_bits and command_bits", ESP_ERR_INVALID_ARG);
#endif
    uint32_t lock_flag = ((dev_config->spics_io_num != -1)? SPI_BUS_LOCK_DEV_FLAG_CS_REQUIRED: 0);

    spi_bus_lock_dev_config_t lock_config = {
        .flags = lock_flag,
    };
    spi_bus_lock_dev_handle_t dev_handle;
    err = spi_bus_lock_register_dev(bus_attr->lock, &lock_config, &dev_handle);
    if (err != ESP_OK) {
        goto nomem;
    }

    int freecs = spi_bus_lock_get_dev_id(dev_handle);
    SPI_CHECK(freecs != -1, "no free cs pins for the host", ESP_ERR_NOT_FOUND);

    duty_cycle = (dev_config->duty_cycle_pos==0) ? 128 : dev_config->duty_cycle_pos;

    int freq;
    spi_hal_context_t *hal = &(host->hal);
    hal->half_duplex = dev_config->flags & SPI_DEVICE_HALFDUPLEX ? 1 : 0;
#ifdef SOC_SPI_SUPPORT_AS_CS
    hal->as_cs = dev_config->flags & SPI_DEVICE_CLK_AS_CS ? 1 : 0;
#endif
    hal->positive_cs = dev_config->flags & SPI_DEVICE_POSITIVE_CS ? 1 : 0;
    hal->no_compensate = dev_config->flags & SPI_DEVICE_NO_DUMMY ? 1 : 0;

    spi_hal_timing_conf_t temp_timing_conf;

    esp_err_t ret = spi_hal_get_clock_conf(hal, dev_config->clock_speed_hz, duty_cycle,
                                        !(bus_attr->flags & SPICOMMON_BUSFLAG_IOMUX_PINS),
                                        dev_config->input_delay_ns, &freq,
                                        &temp_timing_conf);

    SPI_CHECK(ret==ESP_OK, "assigned clock speed not supported", ret);

    //Allocate memory for device
    dev=malloc(sizeof(spi_device_t));
    if (dev==NULL) goto nomem;
    memset(dev, 0, sizeof(spi_device_t));
    host->device[freecs] = dev;
    dev->id = freecs;
    dev->timing_conf = temp_timing_conf;
    dev->dev_lock = dev_handle;

    //Allocate queues, set defaults
    dev->trans_queue = xQueueCreate(dev_config->queue_size, sizeof(spi_trans_priv_t));
    dev->ret_queue = xQueueCreate(dev_config->queue_size, sizeof(spi_trans_priv_t));
    if (!dev->trans_queue || !dev->ret_queue) {
        goto nomem;
    }

    dev->host= host;

    //We want to save a copy of the dev config in the dev struct.
    memcpy(&dev->cfg, dev_config, sizeof(spi_device_interface_config_t));
    dev->cfg.duty_cycle_pos = duty_cycle;
    // TODO: if we have to change the apb clock among transactions, re-calculate this each time the apb clock lock is locked.

    //Set CS pin, CS options
    if (dev_config->spics_io_num >= 0) {
        spicommon_cs_initialize(host_id, dev_config->spics_io_num, freecs, !(bus_attr->flags & SPICOMMON_BUSFLAG_IOMUX_PINS));
    }

    *handle=dev;
    ESP_LOGD(SPI_TAG, "SPI%d: New device added to CS%d, effective clock: %dkHz", host_id+1, freecs, freq/1000);

    return ESP_OK;

nomem:
    if (dev) {
        if (dev->trans_queue) vQueueDelete(dev->trans_queue);
        if (dev->ret_queue) vQueueDelete(dev->ret_queue);
        spi_bus_lock_unregister_dev(dev->dev_lock);
    }
    free(dev);
    return ESP_ERR_NO_MEM;
}

esp_err_t spi_bus_remove_device(spi_device_handle_t handle)
{
    SPI_CHECK(handle!=NULL, "invalid handle", ESP_ERR_INVALID_ARG);
    //These checks aren't exhaustive; another thread could sneak in a transaction inbetween. These are only here to
    //catch design errors and aren't meant to be triggered during normal operation.
    SPI_CHECK(uxQueueMessagesWaiting(handle->trans_queue)==0, "Have unfinished transactions", ESP_ERR_INVALID_STATE);
    SPI_CHECK(handle->host->cur_cs == DEV_NUM_MAX || handle->host->device[handle->host->cur_cs] != handle, "Have unfinished transactions", ESP_ERR_INVALID_STATE);
    SPI_CHECK(uxQueueMessagesWaiting(handle->ret_queue)==0, "Have unfinished transactions", ESP_ERR_INVALID_STATE);

    //return
    int spics_io_num = handle->cfg.spics_io_num;
    if (spics_io_num >= 0) spicommon_cs_free_io(spics_io_num);

    //Kill queues
    vQueueDelete(handle->trans_queue);
    vQueueDelete(handle->ret_queue);
    spi_bus_lock_unregister_dev(handle->dev_lock);

    assert(handle->host->device[handle->id] == handle);
    handle->host->device[handle->id] = NULL;
    free(handle);
    return ESP_OK;
}

int spi_cal_clock(int fapb, int hz, int duty_cycle, uint32_t *reg_o)
{
    return spi_ll_master_cal_clock(fapb, hz, duty_cycle, reg_o);
}

int spi_get_actual_clock(int fapb, int hz, int duty_cycle)
{
    return spi_hal_master_cal_clock(fapb, hz, duty_cycle);
}

// Setup the device-specified configuration registers. Called every time a new
// transaction is to be sent, but only apply new configurations when the device
// changes.
static SPI_MASTER_ISR_ATTR void spi_setup_device(spi_device_t *dev)
{
    spi_bus_lock_dev_handle_t dev_lock = dev->dev_lock;

    if (!spi_bus_lock_touch(dev_lock)) {
        //if the configuration is already applied, skip the following.
        return;
    }

    spi_host_t* host = dev->host;
    spi_hal_context_t *hal = &host->hal;
    hal->mode = dev->cfg.mode;
    hal->tx_lsbfirst = dev->cfg.flags & SPI_DEVICE_TXBIT_LSBFIRST ? 1 : 0;
    hal->rx_lsbfirst = dev->cfg.flags & SPI_DEVICE_RXBIT_LSBFIRST ? 1 : 0;
    hal->no_compensate = dev->cfg.flags & SPI_DEVICE_NO_DUMMY ? 1 : 0;
    hal->sio = dev->cfg.flags & SPI_DEVICE_3WIRE ? 1 : 0;
    hal->dummy_bits = dev->cfg.dummy_bits;
    hal->cs_setup = dev->cfg.cs_ena_pretrans;
    hal->cs_hold =dev->cfg.cs_ena_posttrans;
    //set hold_time to 0 will not actually append delay to CS
    //set it to 1 since we do need at least one clock of hold time in most cases
    if (hal->cs_hold == 0) hal->cs_hold = 1;
    hal->cs_pin_id = dev->id;
    hal->timing_conf = &dev->timing_conf;

    spi_hal_setup_device(hal);
}

static SPI_MASTER_ISR_ATTR spi_device_t *get_acquiring_dev(spi_host_t *host)
{
    spi_bus_lock_dev_handle_t dev_lock = spi_bus_lock_get_acquiring_dev(host->bus_attr->lock);
    if (!dev_lock) return NULL;

    return host->device[spi_bus_lock_get_dev_id(dev_lock)];
}

// Debug only
// NOTE if the acquiring is not fully completed, `spi_bus_lock_get_acquiring_dev`
// may return a false `NULL` cause the function returning false `false`.
static inline SPI_MASTER_ISR_ATTR bool spi_bus_device_is_polling(spi_device_t *dev)
{
    return get_acquiring_dev(dev->host) == dev && dev->host->polling;
}

/*-----------------------------------------------------------------------------
    Working Functions
-----------------------------------------------------------------------------*/

// The interrupt may get invoked by the bus lock.
static void spi_bus_intr_enable(void *host)
{
    esp_intr_enable(((spi_host_t*)host)->intr);
}

// The interrupt is always disabled by the ISR itself, not exposed
static void spi_bus_intr_disable(void *host)
{
    esp_intr_disable(((spi_host_t*)host)->intr);
}

// The function is called to send a new transaction, in ISR or in the task.
// Setup the transaction-specified registers and linked-list used by the DMA (or FIFO if DMA is not used)
static void SPI_MASTER_ISR_ATTR spi_new_trans(spi_device_t *dev, spi_trans_priv_t *trans_buf, spi_hal_context_t *hal)
{
    spi_transaction_t *trans = NULL;
    spi_host_t *host = dev->host;
    int dev_id = dev->id;

    trans = trans_buf->trans;
    host->cur_cs = dev_id;

    //Reconfigure according to device settings, the function only has effect when the dev_id is changed.
    spi_setup_device(host->device[dev_id]);

    hal->tx_bitlen = trans->length;
    hal->rx_bitlen = trans->rxlength;
    hal->rcv_buffer = (uint8_t*)host->cur_trans_buf.buffer_to_rcv;
    hal->send_buffer = (uint8_t*)host->cur_trans_buf.buffer_to_send;
    hal->half_duplex = dev->cfg.flags & SPI_DEVICE_HALFDUPLEX ? 1 : 0;
    hal->cmd = trans->cmd;
    hal->addr = trans->addr;
    //Set up QIO/DIO if needed
    hal->io_mode = (trans->flags & SPI_TRANS_MODE_DIO ?
                        (trans->flags & SPI_TRANS_MODE_DIOQIO_ADDR ? SPI_LL_IO_MODE_DIO : SPI_LL_IO_MODE_DUAL) :
                    (trans->flags & SPI_TRANS_MODE_QIO ?
                        (trans->flags & SPI_TRANS_MODE_DIOQIO_ADDR ? SPI_LL_IO_MODE_QIO : SPI_LL_IO_MODE_QUAD) :
                    SPI_LL_IO_MODE_NORMAL
                    ));

    hal->tx_bitlen = trans->length;
    hal->rx_bitlen = trans->rxlength;

    if (trans->flags & SPI_TRANS_VARIABLE_CMD) {
        hal->cmd_bits = ((spi_transaction_ext_t *)trans)->command_bits;
    } else {
        hal->cmd_bits = dev->cfg.command_bits;
    }
    if (trans->flags & SPI_TRANS_VARIABLE_ADDR) {
        hal->addr_bits = ((spi_transaction_ext_t *)trans)->address_bits;
    } else {
        hal->addr_bits = dev->cfg.address_bits;
    }
    if (trans->flags & SPI_TRANS_VARIABLE_DUMMY) {
        hal->dummy_bits = ((spi_transaction_ext_t *)trans)->dummy_bits;
    } else {
        hal->dummy_bits = dev->cfg.dummy_bits;
    }

    spi_hal_setup_trans(hal);
    spi_hal_prepare_data(hal);

    //Call pre-transmission callback, if any
    if (dev->cfg.pre_cb) dev->cfg.pre_cb(trans);
    //Kick off transfer
    spi_hal_user_start(hal);
}

// The function is called when a transaction is done, in ISR or in the task.
// Fetch the data from FIFO and call the ``post_cb``.
static void SPI_MASTER_ISR_ATTR spi_post_trans(spi_host_t *host)
{
    spi_transaction_t *cur_trans = host->cur_trans_buf.trans;
    spi_hal_fetch_result(&host->hal);
    //Call post-transaction callback, if any
    spi_device_t* dev = host->device[host->cur_cs];
    if (dev->cfg.post_cb) dev->cfg.post_cb(cur_trans);

    host->cur_cs = DEV_NUM_MAX;
}


// This is run in interrupt context.
static void SPI_MASTER_ISR_ATTR spi_intr(void *arg)
{
    BaseType_t do_yield = pdFALSE;
    spi_host_t *host = (spi_host_t *)arg;
    const spi_bus_attr_t* bus_attr = host->bus_attr;

    assert(spi_hal_usr_is_done(&host->hal));

    /*
     * Help to skip the handling of in-flight transaction, and disable of the interrupt.
     * The esp_intr_enable will be called (b) after new BG request is queued (a) in the task;
     * while esp_intr_disable should be called (c) if we check and found the sending queue is empty (d).
     * If (c) is called after (d), then there is a risk that things happens in this sequence:
     * (d) -> (a) -> (b) -> (c), and in this case the interrupt is disabled while there's pending BG request in the queue.
     * To avoid this, interrupt is disabled here, and re-enabled later if required.
     */
    if (!spi_bus_lock_bg_entry(bus_attr->lock)) {
        /*------------ deal with the in-flight transaction -----------------*/
        assert(host->cur_cs != DEV_NUM_MAX);
        //Okay, transaction is done.
        const int cs = host->cur_cs;
        //Tell common code DMA workaround that our DMA channel is idle. If needed, the code will do a DMA reset.
        if (bus_attr->dma_chan) {
            spicommon_dmaworkaround_idle(bus_attr->dma_chan);
        }

        //cur_cs is changed to DEV_NUM_MAX here
        spi_post_trans(host);
        // spi_bus_lock_bg_pause(bus_attr->lock);
        //Return transaction descriptor.
        xQueueSendFromISR(host->device[cs]->ret_queue, &host->cur_trans_buf, &do_yield);
#ifdef CONFIG_PM_ENABLE
        //Release APB frequency lock
        esp_pm_lock_release(bus_attr->pm_lock);
#endif
    }

    /*------------ new transaction starts here ------------------*/
    assert(host->cur_cs == DEV_NUM_MAX);

    spi_bus_lock_handle_t lock = host->bus_attr->lock;
    BaseType_t trans_found = pdFALSE;


    // There should be remaining requests
    BUS_LOCK_DEBUG_EXECUTE_CHECK(spi_bus_lock_bg_req_exist(lock));

    do {
        spi_bus_lock_dev_handle_t acq_dev_lock = spi_bus_lock_get_acquiring_dev(lock);
        spi_bus_lock_dev_handle_t desired_dev = acq_dev_lock;
        bool resume_task = false;
        spi_device_t* device_to_send = NULL;

        if (!acq_dev_lock) {
            // This function may assign a new acquiring device, otherwise it will suggest a desired device with BG active
            // We use either of them without further searching in the devices.
            // If the return value is true, it means either there's no acquiring device, or the acquiring device's BG is active,
            // We stay in the ISR to deal with those transactions of desired device, otherwise nothing will be done, check whether we need to resume some other tasks, or just quit the ISR
            resume_task = spi_bus_lock_bg_check_dev_acq(lock, &desired_dev);
        }

        if (!resume_task) {
            bool dev_has_req = spi_bus_lock_bg_check_dev_req(desired_dev);
            if (dev_has_req) {
                device_to_send = host->device[spi_bus_lock_get_dev_id(desired_dev)];
                trans_found = xQueueReceiveFromISR(device_to_send->trans_queue, &host->cur_trans_buf, &do_yield);
                if (!trans_found) {
                    spi_bus_lock_bg_clear_req(desired_dev);
                }
            }
        }

        if (trans_found) {
            spi_trans_priv_t *const cur_trans_buf = &host->cur_trans_buf;
            if (bus_attr->dma_chan != 0 && (cur_trans_buf->buffer_to_rcv || cur_trans_buf->buffer_to_send)) {
                //mark channel as active, so that the DMA will not be reset by the slave
                spicommon_dmaworkaround_transfer_active(bus_attr->dma_chan);
            }
            spi_new_trans(device_to_send, cur_trans_buf, (&host->hal));
        }
        // Exit of the ISR, handle interrupt re-enable (if sending transaction), retry (if there's coming BG),
        // or resume acquiring device task (if quit due to bus acquiring).
    } while (!spi_bus_lock_bg_exit(lock, trans_found, &do_yield));

    if (do_yield) portYIELD_FROM_ISR();
}

static SPI_MASTER_ISR_ATTR esp_err_t check_trans_valid(spi_device_handle_t handle, spi_transaction_t *trans_desc)
{
    SPI_CHECK(handle!=NULL, "invalid dev handle", ESP_ERR_INVALID_ARG);
    spi_host_t *host = handle->host;
    const spi_bus_attr_t* bus_attr = host->bus_attr;
    //check transmission length
    SPI_CHECK((trans_desc->flags & SPI_TRANS_USE_RXDATA)==0 ||trans_desc->rxlength <= 32, "rxdata transfer > 32 bits without configured DMA", ESP_ERR_INVALID_ARG);
    SPI_CHECK((trans_desc->flags & SPI_TRANS_USE_TXDATA)==0 ||trans_desc->length <= 32, "txdata transfer > 32 bits without configured DMA", ESP_ERR_INVALID_ARG);
    SPI_CHECK(trans_desc->length <= bus_attr->max_transfer_sz*8, "txdata transfer > host maximum", ESP_ERR_INVALID_ARG);
    SPI_CHECK(trans_desc->rxlength <= bus_attr->max_transfer_sz*8, "rxdata transfer > host maximum", ESP_ERR_INVALID_ARG);
    SPI_CHECK((handle->cfg.flags & SPI_DEVICE_HALFDUPLEX) || trans_desc->rxlength <= trans_desc->length, "rx length > tx length in full duplex mode", ESP_ERR_INVALID_ARG);
    //check working mode
    SPI_CHECK(!((trans_desc->flags & (SPI_TRANS_MODE_DIO|SPI_TRANS_MODE_QIO)) && (handle->cfg.flags & SPI_DEVICE_3WIRE)), "incompatible iface params", ESP_ERR_INVALID_ARG);
    SPI_CHECK(!((trans_desc->flags & (SPI_TRANS_MODE_DIO|SPI_TRANS_MODE_QIO)) && (!(handle->cfg.flags & SPI_DEVICE_HALFDUPLEX))), "incompatible iface params", ESP_ERR_INVALID_ARG);
#ifdef CONFIG_IDF_TARGET_ESP32
    SPI_CHECK( !(handle->cfg.flags & SPI_DEVICE_HALFDUPLEX) || bus_attr->dma_chan == 0 || !(trans_desc->flags & SPI_TRANS_USE_RXDATA || trans_desc->rx_buffer != NULL)
        || !(trans_desc->flags & SPI_TRANS_USE_TXDATA || trans_desc->tx_buffer!=NULL), "SPI half duplex mode does not support using DMA with both MOSI and MISO phases.", ESP_ERR_INVALID_ARG );
#endif
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
    // copy data from temporary DMA-capable buffer back to IRAM buffer and free the temporary one.
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
        ESP_LOGD(SPI_TAG, "Allocate RX buffer for DMA" );
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
        ESP_LOGD(SPI_TAG, "Allocate TX buffer for DMA" );
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


esp_err_t SPI_MASTER_ATTR spi_device_queue_trans(spi_device_handle_t handle, spi_transaction_t *trans_desc, TickType_t ticks_to_wait)
{
    esp_err_t ret = check_trans_valid(handle, trans_desc);
    if (ret != ESP_OK) return ret;

    spi_host_t *host = handle->host;

    SPI_CHECK(!spi_bus_device_is_polling(handle), "Cannot queue new transaction while previous polling transaction is not terminated.", ESP_ERR_INVALID_STATE );

    spi_trans_priv_t trans_buf;
    ret = setup_priv_desc(trans_desc, &trans_buf, (host->bus_attr->dma_chan!=0));
    if (ret != ESP_OK) return ret;

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(host->bus_attr->pm_lock);
#endif
    //Send to queue and invoke the ISR.

    BaseType_t r = xQueueSend(handle->trans_queue, (void *)&trans_buf, ticks_to_wait);
    if (!r) {
        ret = ESP_ERR_TIMEOUT;
#ifdef CONFIG_PM_ENABLE
        //Release APB frequency lock
        esp_pm_lock_release(host->bus_attr->pm_lock);
#endif
        goto clean_up;
    }

    // The ISR will be invoked at correct time by the lock with `spi_bus_intr_enable`.
    ret = spi_bus_lock_bg_request(handle->dev_lock);
    if (ret != ESP_OK) {
        goto clean_up;
    }
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


esp_err_t SPI_MASTER_ISR_ATTR spi_device_acquire_bus(spi_device_t *device, TickType_t wait)
{
    spi_host_t *const host = device->host;
    SPI_CHECK(wait==portMAX_DELAY, "acquire finite time not supported now.", ESP_ERR_INVALID_ARG);
    SPI_CHECK(!spi_bus_device_is_polling(device), "Cannot acquire bus when a polling transaction is in progress.", ESP_ERR_INVALID_STATE );

    esp_err_t ret = spi_bus_lock_acquire_start(device->dev_lock, wait);
    if (ret != ESP_OK) {
        return ret;
    }
    host->device_acquiring_lock = device;

    ESP_LOGD(SPI_TAG, "device%d locked the bus", device->id);

#ifdef CONFIG_PM_ENABLE
    // though we don't suggest to block the task before ``release_bus``, still allow doing so.
    // this keeps the spi clock at 80MHz even if all tasks are blocked
    esp_pm_lock_acquire(host->bus_attr->pm_lock);
#endif
    //configure the device ahead so that we don't need to do it again in the following transactions
    spi_setup_device(host->device[device->id]);
    //the DMA is also occupied by the device, all the slave devices that using DMA should wait until bus released.
    if (host->bus_attr->dma_chan != 0) {
        spicommon_dmaworkaround_transfer_active(host->bus_attr->dma_chan);
    }
    return ESP_OK;
}

// This function restore configurations required in the non-polling mode
void SPI_MASTER_ISR_ATTR spi_device_release_bus(spi_device_t *dev)
{
    spi_host_t *host = dev->host;

    if (spi_bus_device_is_polling(dev)){
        ESP_EARLY_LOGE(SPI_TAG, "Cannot release bus when a polling transaction is in progress.");
        assert(0);
    }

    if (host->bus_attr->dma_chan != 0) {
        spicommon_dmaworkaround_idle(host->bus_attr->dma_chan);
    }
    //Tell common code DMA workaround that our DMA channel is idle. If needed, the code will do a DMA reset.

    //allow clock to be lower than 80MHz when all tasks blocked
#ifdef CONFIG_PM_ENABLE
    //Release APB frequency lock
    esp_pm_lock_release(host->bus_attr->pm_lock);
#endif
    ESP_LOGD(SPI_TAG, "device%d release bus", dev->id);

    host->device_acquiring_lock = NULL;
    esp_err_t ret = spi_bus_lock_acquire_end(dev->dev_lock);
    assert(ret == ESP_OK);
}


esp_err_t SPI_MASTER_ISR_ATTR spi_device_polling_start(spi_device_handle_t handle, spi_transaction_t *trans_desc, TickType_t ticks_to_wait)
{
    esp_err_t ret;
    SPI_CHECK(ticks_to_wait == portMAX_DELAY, "currently timeout is not available for polling transactions", ESP_ERR_INVALID_ARG);

    spi_host_t *host = handle->host;
    ret = check_trans_valid(handle, trans_desc);
    if (ret!=ESP_OK) return ret;

    SPI_CHECK(!spi_bus_device_is_polling(handle), "Cannot send polling transaction while the previous polling transaction is not terminated.", ESP_ERR_INVALID_STATE );

    if (host->device_acquiring_lock != handle) {
        ret = spi_bus_lock_acquire_start(handle->dev_lock, ticks_to_wait);
    } else {
        ret = spi_bus_lock_wait_bg_done(handle->dev_lock, ticks_to_wait);
    }
    if (ret != ESP_OK) return ret;

    ret = setup_priv_desc(trans_desc, &host->cur_trans_buf, (host->bus_attr->dma_chan!=0));
    if (ret!=ESP_OK) return ret;

    //Polling, no interrupt is used.
    host->polling = true;

    ESP_LOGV(SPI_TAG, "polling trans");
    spi_new_trans(handle, &host->cur_trans_buf, (&host->hal));

    return ESP_OK;
}


esp_err_t SPI_MASTER_ISR_ATTR spi_device_polling_end(spi_device_handle_t handle, TickType_t ticks_to_wait)
{
    SPI_CHECK(handle != NULL, "invalid dev handle", ESP_ERR_INVALID_ARG);
    spi_host_t *host = handle->host;

    assert(host->cur_cs == handle->id);
    assert(handle == get_acquiring_dev(host));

    TickType_t start = xTaskGetTickCount();
    while (!spi_hal_usr_is_done(&host->hal)) {
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
    if (host->device_acquiring_lock != handle) {
        assert(host->device_acquiring_lock == NULL);
        spi_bus_lock_acquire_end(handle->dev_lock);
    }

    return ESP_OK;
}


esp_err_t SPI_MASTER_ISR_ATTR spi_device_polling_transmit(spi_device_handle_t handle, spi_transaction_t* trans_desc)
{
    esp_err_t ret;
    ret = spi_device_polling_start(handle, trans_desc, portMAX_DELAY);
    if (ret != ESP_OK) return ret;

    return spi_device_polling_end(handle, portMAX_DELAY);
}

