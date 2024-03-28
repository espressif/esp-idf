/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
   like the length, address, command etc, plus pointers to transmit and
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
#include <sys/param.h>
#include "esp_private/periph_ctrl.h"
#include "esp_private/spi_common_internal.h"
#include "esp_private/spi_master_internal.h"
#include "driver/spi_master.h"
#include "esp_clk_tree.h"
#include "clk_ctrl_os.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_ipc.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/soc_memory_layout.h"
#include "driver/gpio.h"
#include "hal/spi_hal.h"
#include "hal/spi_ll.h"
#include "hal/hal_utils.h"
#include "esp_heap_caps.h"
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#include "esp_cache.h"
#endif

typedef struct spi_device_t spi_device_t;

/// struct to hold private transaction data (like tx and rx buffer for DMA).
typedef struct {
    spi_transaction_t   *trans;
    const uint32_t *buffer_to_send;    //equals to tx_data, if SPI_TRANS_USE_RXDATA is applied; otherwise if original buffer wasn't in DMA-capable memory, this gets the address of a temporary buffer that is;
    //otherwise sets to the original buffer or NULL if no buffer is assigned.
    uint32_t *buffer_to_rcv;           //similar to buffer_to_send
#if SOC_SPI_SCT_SUPPORTED
    uint32_t reserved[2];              //As we create the queue when in init, to use sct mode private descriptor as a queue item (when in sct mode), we need to add a dummy member here to keep the same size with `spi_sct_trans_priv_t`.
#endif
} spi_trans_priv_t;

#if SOC_SPI_SCT_SUPPORTED
//Type of dma descriptors that used under SPI SCT mode
typedef struct {
    spi_dma_desc_t          *tx_seg_head;
    spi_dma_desc_t          *rx_seg_head;
    spi_multi_transaction_t *sct_trans_desc_head;
    uint32_t                *sct_conf_buffer;
    uint16_t                tx_used_desc_num;
    uint16_t                rx_used_desc_num;
} spi_sct_trans_priv_t;
_Static_assert(sizeof(spi_trans_priv_t) == sizeof(spi_sct_trans_priv_t));   //size of spi_trans_priv_t must be the same as size of spi_sct_trans_priv_t

typedef struct {
    /* Segmented-Configure-Transfer required, configured by driver, don't touch */
    uint32_t       tx_free_desc_num;
    uint32_t       rx_free_desc_num;
    spi_dma_desc_t *cur_tx_seg_link;          ///< Current TX DMA descriptor used for sct mode.
    spi_dma_desc_t *cur_rx_seg_link;          ///< Current RX DMA descriptor used for sct mode.
    spi_dma_desc_t *tx_seg_link_tail;         ///< Tail of the TX DMA descriptor link
    spi_dma_desc_t *rx_seg_link_tail;         ///< Tail of the RX DMA descriptor link
} spi_sct_desc_ctx_t;

static void spi_hal_sct_tx_dma_desc_recycle(spi_sct_desc_ctx_t *desc_ctx, uint32_t recycle_num);
static void spi_hal_sct_rx_dma_desc_recycle(spi_sct_desc_ctx_t *desc_ctx, uint32_t recycle_num);
#endif

typedef struct {
    int id;
    spi_device_t* device[DEV_NUM_MAX];
    intr_handle_t intr;
    spi_hal_context_t hal;
    spi_trans_priv_t cur_trans_buf;
#if SOC_SPI_SCT_SUPPORTED
    spi_sct_desc_ctx_t sct_desc_pool;
    spi_sct_trans_priv_t cur_sct_trans;
#endif
    int cur_cs;     //current device doing transaction
    const spi_bus_attr_t* bus_attr;
    const spi_dma_ctx_t *dma_ctx;
    bool sct_mode_enabled;

    /**
     * the bus is permanently controlled by a device until `spi_bus_release_bus`` is called. Otherwise
     * the acquiring of SPI bus will be freed when `spi_device_polling_end` is called.
     */
    spi_device_t* device_acquiring_lock;
    portMUX_TYPE spinlock;

//debug information
    bool polling;   //in process of a polling, avoid of queue new transactions into ISR
} spi_host_t;

struct spi_device_t {
    int id;
    QueueHandle_t trans_queue;
    QueueHandle_t ret_queue;
    spi_device_interface_config_t cfg;
    spi_hal_dev_config_t hal_dev;
    spi_host_t *host;
    spi_bus_lock_dev_handle_t dev_lock;
};

static spi_host_t* bus_driver_ctx[SOC_SPI_PERIPH_NUM] = {};

static const char *SPI_TAG = "spi_master";
#define SPI_CHECK(a, str, ret_val)  ESP_RETURN_ON_FALSE_ISR(a, ret_val, SPI_TAG, str)

#if SOC_PERIPH_CLK_CTRL_SHARED
#define SPI_MASTER_PERI_CLOCK_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define SPI_MASTER_PERI_CLOCK_ATOMIC()
#endif

static void spi_intr(void *arg);
static void spi_bus_intr_enable(void *host);
static void spi_bus_intr_disable(void *host);

static esp_err_t spi_master_deinit_driver(void* arg);

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

#if (SOC_CPU_CORES_NUM > 1) && (!CONFIG_FREERTOS_UNICORE)
typedef struct {
    spi_host_t *spi_host;
    esp_err_t *err;
} spi_ipc_param_t;

static void ipc_isr_reg_to_core(void *args)
{
    spi_host_t *host = ((spi_ipc_param_t *)args)->spi_host;
    const spi_bus_attr_t* bus_attr = host->bus_attr;
    *((spi_ipc_param_t *)args)->err = esp_intr_alloc(spicommon_irqsource_for_host(host->id), bus_attr->bus_cfg.intr_flags | ESP_INTR_FLAG_INTRDISABLED, spi_intr, host, &host->intr);
}
#endif

// Should be called before any devices are actually registered or used.
// Currently automatically called after `spi_bus_initialize()` and when first device is registered.
static esp_err_t spi_master_init_driver(spi_host_device_t host_id)
{
    esp_err_t err = ESP_OK;

    const spi_bus_attr_t* bus_attr = spi_bus_get_attr(host_id);
    const spi_dma_ctx_t *dma_ctx = spi_bus_get_dma_ctx(host_id);
    SPI_CHECK(bus_attr != NULL, "host_id not initialized", ESP_ERR_INVALID_STATE);
    SPI_CHECK(bus_attr->lock != NULL, "SPI Master cannot attach to bus. (Check CONFIG_SPI_FLASH_SHARE_SPI1_BUS)", ESP_ERR_INVALID_ARG);
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
        .spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED,
        .bus_attr = bus_attr,
        .dma_ctx = dma_ctx,
    };

    // interrupts are not allowed on SPI1 bus
    if (host_id != SPI1_HOST) {
#if (SOC_CPU_CORES_NUM > 1) && (!CONFIG_FREERTOS_UNICORE)
        if (bus_attr->bus_cfg.isr_cpu_id > ESP_INTR_CPU_AFFINITY_AUTO) {
            SPI_CHECK(bus_attr->bus_cfg.isr_cpu_id <= ESP_INTR_CPU_AFFINITY_1, "invalid core id", ESP_ERR_INVALID_ARG);
            spi_ipc_param_t ipc_arg = {
                .spi_host = host,
                .err = &err,
            };
            esp_ipc_call_blocking(ESP_INTR_CPU_AFFINITY_TO_CORE_ID(bus_attr->bus_cfg.isr_cpu_id), ipc_isr_reg_to_core, (void *) &ipc_arg);
        } else
#endif
        {
            err = esp_intr_alloc(spicommon_irqsource_for_host(host_id), bus_attr->bus_cfg.intr_flags | ESP_INTR_FLAG_INTRDISABLED, spi_intr, host, &host->intr);
        }
        if (err != ESP_OK) {
            goto cleanup;
        }
    }

    SPI_MASTER_PERI_CLOCK_ATOMIC() {
        spi_ll_enable_clock(host_id, true);
    }
    spi_hal_init(&host->hal, host_id);

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
    for (x = 0; x < DEV_NUM_MAX; x++) {
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
#ifdef CONFIG_IDF_TARGET_ESP32
    int timing_dummy;
    int timing_miso_delay;

    spi_hal_cal_timing(APB_CLK_FREQ, eff_clk, gpio_is_used, input_delay_ns, &timing_dummy, &timing_miso_delay);
    if (dummy_o) {
        *dummy_o = timing_dummy;
    }
    if (cycles_remain_o) {
        *cycles_remain_o = timing_miso_delay;
    }
#else
    //TODO: IDF-6578
    ESP_LOGW(SPI_TAG, "This func temporary not supported for current target!");
#endif
}

int spi_get_freq_limit(bool gpio_is_used, int input_delay_ns)
{
#ifdef CONFIG_IDF_TARGET_ESP32
    return spi_hal_get_freq_limit(gpio_is_used, input_delay_ns);
#else
    //TODO: IDF-6578
    ESP_LOGW(SPI_TAG, "This func temporary not supported for current target!");
    return 0;
#endif
}

/*
 Add a device. This allocates a CS line for the device, allocates memory for the device structure and hooks
 up the CS pin to whatever is specified.
*/
esp_err_t spi_bus_add_device(spi_host_device_t host_id, const spi_device_interface_config_t *dev_config, spi_device_handle_t *handle)
{
    spi_device_t *dev = NULL;
    esp_err_t err = ESP_OK;

    SPI_CHECK(is_valid_host(host_id), "invalid host", ESP_ERR_INVALID_ARG);
    if (bus_driver_ctx[host_id] == NULL) {
        //lazy initialization the driver, get deinitialized by the bus is freed
        err = spi_master_init_driver(host_id);
        if (err != ESP_OK) {
            return err;
        }
    }

    spi_host_t *host = bus_driver_ctx[host_id];
    const spi_bus_attr_t* bus_attr = host->bus_attr;
    SPI_CHECK(dev_config->spics_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(dev_config->spics_io_num), "spics pin invalid", ESP_ERR_INVALID_ARG);
#if SOC_SPI_SUPPORT_CLK_RC_FAST
    if (dev_config->clock_source == SPI_CLK_SRC_RC_FAST) {
        SPI_CHECK(periph_rtc_dig_clk8m_enable(), "the selected clock not available", ESP_ERR_INVALID_STATE);
    }
#endif
    spi_clock_source_t clk_src = SPI_CLK_SRC_DEFAULT;
    uint32_t clock_source_hz = 0;
    uint32_t clock_source_div = 1;
    if (dev_config->clock_source) {
        clk_src = dev_config->clock_source;
    }
    esp_clk_tree_src_get_freq_hz(clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &clock_source_hz);
#if SPI_LL_SUPPORT_CLK_SRC_PRE_DIV
    SPI_CHECK((dev_config->clock_speed_hz > 0) && (dev_config->clock_speed_hz <= MIN(clock_source_hz / 2, (80 * 1000000))), "invalid sclk speed", ESP_ERR_INVALID_ARG);

    if (clock_source_hz / 2 > (80 * 1000000)) {    //clock_source_hz beyond peripheral HW limitation, calc pre-divider
        hal_utils_clk_info_t clk_cfg = {
            .src_freq_hz = clock_source_hz,
            .exp_freq_hz = dev_config->clock_speed_hz * 2,  //we have (hs_clk = 2*mst_clk), calc hs_clk first
            .round_opt = HAL_DIV_ROUND,
            .min_integ = 1,
            .max_integ = SPI_LL_CLK_SRC_PRE_DIV_MAX / 2,
        };
        hal_utils_calc_clk_div_integer(&clk_cfg, &clock_source_div);
    }
    clock_source_div *= 2; //convert to mst_clk function divider
    clock_source_hz /= clock_source_div;    //actual freq enter to SPI peripheral
#else
    SPI_CHECK((dev_config->clock_speed_hz > 0) && (dev_config->clock_speed_hz <= clock_source_hz), "invalid sclk speed", ESP_ERR_INVALID_ARG);
#endif
#ifdef CONFIG_IDF_TARGET_ESP32
    //The hardware looks like it would support this, but actually setting cs_ena_pretrans when transferring in full
    //duplex mode does absolutely nothing on the ESP32.
    SPI_CHECK(dev_config->cs_ena_pretrans <= 1 || (dev_config->address_bits == 0 && dev_config->command_bits == 0) ||
              (dev_config->flags & SPI_DEVICE_HALFDUPLEX), "In full-duplex mode, only support cs pretrans delay = 1 and without address_bits and command_bits", ESP_ERR_INVALID_ARG);
#endif

    //Check post_cb status when `SPI_DEVICE_NO_RETURN_RESULT` flag is set.
    if (dev_config->flags & SPI_DEVICE_NO_RETURN_RESULT) {
        SPI_CHECK(dev_config->post_cb != NULL, "use feature flag 'SPI_DEVICE_NO_RETURN_RESULT' but no post callback function sets", ESP_ERR_INVALID_ARG);
    }

    uint32_t lock_flag = ((dev_config->spics_io_num != -1) ? SPI_BUS_LOCK_DEV_FLAG_CS_REQUIRED : 0);

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

    //input parameters to calculate timing configuration
    int half_duplex = dev_config->flags & SPI_DEVICE_HALFDUPLEX ? 1 : 0;
    int no_compensate = dev_config->flags & SPI_DEVICE_NO_DUMMY ? 1 : 0;
    int duty_cycle = (dev_config->duty_cycle_pos == 0) ? 128 : dev_config->duty_cycle_pos;
    int use_gpio = !(bus_attr->flags & SPICOMMON_BUSFLAG_IOMUX_PINS);
    spi_hal_timing_param_t timing_param = {
        .half_duplex = half_duplex,
        .no_compensate = no_compensate,
        .clk_src_hz = clock_source_hz,
        .expected_freq = dev_config->clock_speed_hz,
        .duty_cycle = duty_cycle,
        .input_delay_ns = dev_config->input_delay_ns,
        .use_gpio = use_gpio
    };

    //output values of timing configuration
    spi_hal_timing_conf_t temp_timing_conf;
    esp_err_t ret = spi_hal_cal_clock_conf(&timing_param, &temp_timing_conf);
    SPI_CHECK(ret == ESP_OK, "assigned clock speed not supported", ret);
    temp_timing_conf.clock_source = clk_src;
    temp_timing_conf.source_pre_div = clock_source_div;

    //Allocate memory for device
    dev = malloc(sizeof(spi_device_t));
    if (dev == NULL) {
        goto nomem;
    }
    memset(dev, 0, sizeof(spi_device_t));

    dev->id = freecs;
    dev->dev_lock = dev_handle;

    //Allocate queues, set defaults
    dev->trans_queue = xQueueCreate(dev_config->queue_size, sizeof(spi_trans_priv_t));
    if (!dev->trans_queue) {
        goto nomem;
    }
    //ret_queue nolonger needed if use flag SPI_DEVICE_NO_RETURN_RESULT
    if (!(dev_config->flags & SPI_DEVICE_NO_RETURN_RESULT)) {
        dev->ret_queue = xQueueCreate(dev_config->queue_size, sizeof(spi_trans_priv_t));
        if (!dev->ret_queue) {
            goto nomem;
        }
    }

    //We want to save a copy of the dev config in the dev struct.
    memcpy(&dev->cfg, dev_config, sizeof(spi_device_interface_config_t));
    dev->cfg.duty_cycle_pos = duty_cycle;
    // TODO: if we have to change the apb clock among transactions, re-calculate this each time the apb clock lock is locked.

    //Set CS pin, CS options
    if (dev_config->spics_io_num >= 0) {
        spicommon_cs_initialize(host_id, dev_config->spics_io_num, freecs, use_gpio);
    }

    //save a pointer to device in spi_host_t
    host->device[freecs] = dev;
    //save a pointer to host in spi_device_t
    dev->host = host;

    //initialise the device specific configuration
    spi_hal_dev_config_t *hal_dev = &(dev->hal_dev);
    hal_dev->mode = dev_config->mode;
    hal_dev->cs_setup = dev_config->cs_ena_pretrans;
    hal_dev->cs_hold = dev_config->cs_ena_posttrans;
    //set hold_time to 0 will not actually append delay to CS
    //set it to 1 since we do need at least one clock of hold time in most cases
    if (hal_dev->cs_hold == 0) {
        hal_dev->cs_hold = 1;
    }
    hal_dev->cs_pin_id = dev->id;
    hal_dev->timing_conf = temp_timing_conf;
    hal_dev->sio = (dev_config->flags) & SPI_DEVICE_3WIRE ? 1 : 0;
    hal_dev->half_duplex = dev_config->flags & SPI_DEVICE_HALFDUPLEX ? 1 : 0;
    hal_dev->tx_lsbfirst = dev_config->flags & SPI_DEVICE_TXBIT_LSBFIRST ? 1 : 0;
    hal_dev->rx_lsbfirst = dev_config->flags & SPI_DEVICE_RXBIT_LSBFIRST ? 1 : 0;
    hal_dev->no_compensate = dev_config->flags & SPI_DEVICE_NO_DUMMY ? 1 : 0;
#if SOC_SPI_AS_CS_SUPPORTED
    hal_dev->as_cs = dev_config->flags & SPI_DEVICE_CLK_AS_CS ? 1 : 0;
#endif
    hal_dev->positive_cs = dev_config->flags & SPI_DEVICE_POSITIVE_CS ? 1 : 0;

    *handle = dev;
    ESP_LOGD(SPI_TAG, "SPI%d: New device added to CS%d, effective clock: %d Hz", host_id + 1, freecs, temp_timing_conf.real_freq);

    return ESP_OK;

nomem:
    if (dev) {
        if (dev->trans_queue) {
            vQueueDelete(dev->trans_queue);
        }
        if (dev->ret_queue) {
            vQueueDelete(dev->ret_queue);
        }
        spi_bus_lock_unregister_dev(dev->dev_lock);
    }
    free(dev);
    return ESP_ERR_NO_MEM;
}

esp_err_t spi_bus_remove_device(spi_device_handle_t handle)
{
    SPI_CHECK(handle != NULL, "invalid handle", ESP_ERR_INVALID_ARG);
    //These checks aren't exhaustive; another thread could sneak in a transaction in between. These are only here to
    //catch design errors and aren't meant to be triggered during normal operation.
    SPI_CHECK(uxQueueMessagesWaiting(handle->trans_queue) == 0, "Have unfinished transactions", ESP_ERR_INVALID_STATE);
    SPI_CHECK(handle->host->cur_cs == DEV_NUM_MAX || handle->host->device[handle->host->cur_cs] != handle, "Have unfinished transactions", ESP_ERR_INVALID_STATE);
    if (handle->ret_queue) {
        SPI_CHECK(uxQueueMessagesWaiting(handle->ret_queue) == 0, "Have unfinished transactions", ESP_ERR_INVALID_STATE);
    }

#if SOC_SPI_SUPPORT_CLK_RC_FAST
    if (handle->cfg.clock_source == SPI_CLK_SRC_RC_FAST) {
        periph_rtc_dig_clk8m_disable();
    }
#endif

    //return
    int spics_io_num = handle->cfg.spics_io_num;
    if (spics_io_num >= 0) {
        spicommon_cs_free_io(spics_io_num);
    }

    //Kill queues
    if (handle->trans_queue) {
        vQueueDelete(handle->trans_queue);
    }
    if (handle->ret_queue) {
        vQueueDelete(handle->ret_queue);
    }
    spi_bus_lock_unregister_dev(handle->dev_lock);

    assert(handle->host->device[handle->id] == handle);
    handle->host->device[handle->id] = NULL;
    free(handle);
    return ESP_OK;
}

esp_err_t spi_device_get_actual_freq(spi_device_handle_t handle, int* freq_khz)
{
    if ((spi_device_t *)handle == NULL || freq_khz == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *freq_khz = handle->hal_dev.timing_conf.real_freq / 1000;
    return ESP_OK;
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
    spi_hal_context_t *hal = &dev->host->hal;
    spi_hal_dev_config_t *hal_dev = &(dev->hal_dev);

    if (spi_bus_lock_touch(dev_lock)) {
        /* Configuration has not been applied yet. */
        spi_hal_setup_device(hal, hal_dev);
        SPI_MASTER_PERI_CLOCK_ATOMIC() {
#if SPI_LL_SUPPORT_CLK_SRC_PRE_DIV
            //we set mst_div as const 2, then (hs_clk = 2*mst_clk) to ensure timing turning work as past
            //and sure (hs_div * mst_div = source_pre_div)
            spi_ll_clk_source_pre_div(hal->hw, hal_dev->timing_conf.source_pre_div / 2, 2);
#endif
            spi_ll_set_clk_source(hal->hw, hal_dev->timing_conf.clock_source);
        }
    }
}

static SPI_MASTER_ISR_ATTR spi_device_t *get_acquiring_dev(spi_host_t *host)
{
    spi_bus_lock_dev_handle_t dev_lock = spi_bus_lock_get_acquiring_dev(host->bus_attr->lock);
    if (!dev_lock) {
        return NULL;
    }

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
static void SPI_MASTER_ISR_ATTR spi_bus_intr_enable(void *host)
{
    esp_intr_enable(((spi_host_t*)host)->intr);
}

// The interrupt is always disabled by the ISR itself, not exposed
static void SPI_MASTER_ISR_ATTR spi_bus_intr_disable(void *host)
{
    esp_intr_disable(((spi_host_t*)host)->intr);
}

#if SOC_GDMA_SUPPORTED  // AHB_DMA_V1 and AXI_DMA
// dma is provided by gdma driver on these targets
#define spi_dma_reset               gdma_reset
#define spi_dma_start(chan, addr)   gdma_start(chan, (intptr_t)(addr))
#endif

static void SPI_MASTER_ISR_ATTR s_spi_dma_prepare_data(spi_host_t *host, spi_hal_context_t *hal, const spi_hal_dev_config_t *dev, const spi_hal_trans_config_t *trans)
{
    const spi_dma_ctx_t *dma_ctx = host->dma_ctx;

    if (trans->rcv_buffer) {
        spicommon_dma_desc_setup_link(dma_ctx->dmadesc_rx, trans->rcv_buffer, ((trans->rx_bitlen + 7) / 8), true);

        spi_dma_reset(dma_ctx->rx_dma_chan);
        spi_hal_hw_prepare_rx(hal->hw);
        spi_dma_start(dma_ctx->rx_dma_chan, dma_ctx->dmadesc_rx);
    }
#if CONFIG_IDF_TARGET_ESP32
    else if (!dev->half_duplex) {
        //DMA temporary workaround: let RX DMA work somehow to avoid the issue in ESP32 v0/v1 silicon
        spi_ll_dma_rx_enable(hal->hw, 1);
        spi_dma_start(dma_ctx->rx_dma_chan, NULL);
    }
#endif
    if (trans->send_buffer) {
        spicommon_dma_desc_setup_link(dma_ctx->dmadesc_tx, trans->send_buffer, (trans->tx_bitlen + 7) / 8, false);

        spi_dma_reset(dma_ctx->tx_dma_chan);
        spi_hal_hw_prepare_tx(hal->hw);
        spi_dma_start(dma_ctx->tx_dma_chan, dma_ctx->dmadesc_tx);
    }
}

static void SPI_MASTER_ISR_ATTR s_spi_prepare_data(spi_device_t *dev, const spi_hal_trans_config_t *hal_trans)
{
    spi_host_t *host = dev->host;
    spi_hal_dev_config_t *hal_dev = &(dev->hal_dev);
    spi_hal_context_t *hal = &(host->hal);

    if (host->bus_attr->dma_enabled) {
        s_spi_dma_prepare_data(host, hal, hal_dev, hal_trans);
    } else {
        //Need to copy data to registers manually
        spi_hal_push_tx_buffer(hal, hal_trans);
    }

    //in ESP32 these registers should be configured after the DMA is set
    spi_hal_enable_data_line(hal->hw, (!hal_dev->half_duplex && hal_trans->rcv_buffer) || hal_trans->send_buffer, !!hal_trans->rcv_buffer);
}

static void SPI_MASTER_ISR_ATTR spi_format_hal_trans_struct(spi_device_t *dev, spi_trans_priv_t *trans_buf, spi_hal_trans_config_t *hal_trans)
{
    spi_host_t *host = dev->host;
    spi_transaction_t *trans = trans_buf->trans;
    hal_trans->tx_bitlen = trans->length;
    hal_trans->rx_bitlen = trans->rxlength;
    hal_trans->rcv_buffer = (uint8_t*)host->cur_trans_buf.buffer_to_rcv;
    hal_trans->send_buffer = (uint8_t*)host->cur_trans_buf.buffer_to_send;
    hal_trans->cmd = trans->cmd;
    hal_trans->addr = trans->addr;

    if (trans->flags & SPI_TRANS_VARIABLE_CMD) {
        hal_trans->cmd_bits = ((spi_transaction_ext_t *)trans)->command_bits;
    } else {
        hal_trans->cmd_bits = dev->cfg.command_bits;
    }
    if (trans->flags & SPI_TRANS_VARIABLE_ADDR) {
        hal_trans->addr_bits = ((spi_transaction_ext_t *)trans)->address_bits;
    } else {
        hal_trans->addr_bits = dev->cfg.address_bits;
    }
    if (trans->flags & SPI_TRANS_VARIABLE_DUMMY) {
        hal_trans->dummy_bits = ((spi_transaction_ext_t *)trans)->dummy_bits;
    } else {
        hal_trans->dummy_bits = dev->cfg.dummy_bits;
    }

    hal_trans->cs_keep_active = (trans->flags & SPI_TRANS_CS_KEEP_ACTIVE) ? 1 : 0;
    //Set up OIO/QIO/DIO if needed
    hal_trans->line_mode.data_lines = (trans->flags & SPI_TRANS_MODE_DIO) ? 2 : (trans->flags & SPI_TRANS_MODE_QIO) ? 4 : 1;
#if SOC_SPI_SUPPORT_OCT
    if (trans->flags & SPI_TRANS_MODE_OCT) {
        hal_trans->line_mode.data_lines = 8;
    }
#endif
    hal_trans->line_mode.addr_lines = (trans->flags & SPI_TRANS_MULTILINE_ADDR) ? hal_trans->line_mode.data_lines : 1;
    hal_trans->line_mode.cmd_lines = (trans->flags & SPI_TRANS_MULTILINE_CMD) ? hal_trans->line_mode.data_lines : 1;
}

// The function is called to send a new transaction, in ISR or in the task.
// Setup the transaction-specified registers and linked-list used by the DMA (or FIFO if DMA is not used)
static void SPI_MASTER_ISR_ATTR spi_new_trans(spi_device_t *dev, spi_trans_priv_t *trans_buf)
{
    spi_transaction_t *trans = trans_buf->trans;
    spi_hal_context_t *hal = &(dev->host->hal);
    spi_hal_dev_config_t *hal_dev = &(dev->hal_dev);

    dev->host->cur_cs = dev->id;

    //Reconfigure according to device settings, the function only has effect when the dev_id is changed.
    spi_setup_device(dev);

    //set the transaction specific configuration each time before a transaction setup
    spi_hal_trans_config_t hal_trans = {};
    spi_format_hal_trans_struct(dev, trans_buf, &hal_trans);
    spi_hal_setup_trans(hal, hal_dev, &hal_trans);
    s_spi_prepare_data(dev, &hal_trans);

    //Call pre-transmission callback, if any
    if (dev->cfg.pre_cb) {
        dev->cfg.pre_cb(trans);
    }
    //Kick off transfer
    spi_hal_user_start(hal);
}

// The function is called when a transaction is done, in ISR or in the task.
// Fetch the data from FIFO and call the ``post_cb``.
static void SPI_MASTER_ISR_ATTR spi_post_trans(spi_host_t *host)
{
    spi_transaction_t *cur_trans = host->cur_trans_buf.trans;

    if (!host->bus_attr->dma_enabled) {
        spi_hal_fetch_result(&host->hal);
    }
    //Call post-transaction callback, if any
    spi_device_t* dev = host->device[host->cur_cs];
    if (dev->cfg.post_cb) {
        dev->cfg.post_cb(cur_trans);
    }

    host->cur_cs = DEV_NUM_MAX;
}

#if SOC_SPI_SCT_SUPPORTED
static void SPI_MASTER_ISR_ATTR spi_sct_set_hal_trans_config(spi_multi_transaction_t *trans_header, spi_hal_trans_config_t *hal_trans)
{
    spi_transaction_t *trans = &trans_header->base;

    //Set up OIO/QIO/DIO if needed
    hal_trans->line_mode.data_lines = (trans->flags & SPI_TRANS_MODE_DIO) ? 2 : (trans->flags & SPI_TRANS_MODE_QIO) ? 4 : 1;
#if SOC_SPI_SUPPORT_OCT
    if (trans->flags & SPI_TRANS_MODE_OCT) {
        hal_trans->line_mode.data_lines = 8;
    }
#endif
    hal_trans->line_mode.addr_lines = (trans->flags & SPI_TRANS_MULTILINE_ADDR) ? hal_trans->line_mode.data_lines : 1;
    hal_trans->line_mode.cmd_lines = (trans->flags & SPI_TRANS_MULTILINE_CMD) ? hal_trans->line_mode.data_lines : 1;
}

static void SPI_MASTER_ISR_ATTR s_sct_load_dma_link(spi_device_t *dev, spi_dma_desc_t *rx_seg_head, spi_dma_desc_t *tx_seg_head)
{
    spi_hal_context_t *hal = &dev->host->hal;
    const spi_dma_ctx_t *dma_ctx = dev->host->dma_ctx;

    spi_hal_clear_intr_mask(hal, SPI_LL_INTR_SEG_DONE);

    if (rx_seg_head) {
        spi_dma_reset(dma_ctx->rx_dma_chan);
        spi_hal_hw_prepare_rx(hal->hw);
        spi_dma_start(dma_ctx->rx_dma_chan, rx_seg_head);
    }

    if (tx_seg_head) {
        spi_dma_reset(dma_ctx->tx_dma_chan);
        spi_hal_hw_prepare_tx(hal->hw);
        spi_dma_start(dma_ctx->tx_dma_chan, tx_seg_head);
    }
}

static void SPI_MASTER_ISR_ATTR spi_new_sct_trans(spi_device_t *dev, spi_sct_trans_priv_t *cur_sct_trans)
{
    dev->host->cur_cs = dev->id;

    //Reconfigure according to device settings, the function only has effect when the dev_id is changed.
    spi_setup_device(dev);

#if !CONFIG_IDF_TARGET_ESP32S2
    // s2 update this seg_gap_clock_len by dma from conf_buffer
    spi_hal_sct_set_conf_bits_len(&dev->host->hal, cur_sct_trans->sct_trans_desc_head->sct_gap_len);
#endif
    s_sct_load_dma_link(dev, cur_sct_trans->rx_seg_head, cur_sct_trans->tx_seg_head);
    if (dev->cfg.pre_cb) {
        dev->cfg.pre_cb((spi_transaction_t *)cur_sct_trans->sct_trans_desc_head);
    }

    //Kick off transfer
    spi_hal_user_start(&dev->host->hal);
}

static void SPI_MASTER_ISR_ATTR spi_post_sct_trans(spi_host_t *host)
{
    if (host->cur_sct_trans.rx_seg_head == NULL) {
        assert(host->cur_sct_trans.rx_used_desc_num == 0);
    }

    free(host->cur_sct_trans.sct_conf_buffer);
    portENTER_CRITICAL_ISR(&host->spinlock);
    spi_hal_sct_tx_dma_desc_recycle(&host->sct_desc_pool, host->cur_sct_trans.tx_used_desc_num);
    spi_hal_sct_rx_dma_desc_recycle(&host->sct_desc_pool, host->cur_sct_trans.rx_used_desc_num);
    portEXIT_CRITICAL_ISR(&host->spinlock);
    if (host->device[host->cur_cs]->cfg.post_cb) {
        host->device[host->cur_cs]->cfg.post_cb((spi_transaction_t *)host->cur_sct_trans.sct_trans_desc_head);
    }

    host->cur_cs = DEV_NUM_MAX;
}
#endif  //#if SOC_SPI_SCT_SUPPORTED

// This is run in interrupt context.
static void SPI_MASTER_ISR_ATTR spi_intr(void *arg)
{
    BaseType_t do_yield = pdFALSE;
    spi_host_t *host = (spi_host_t *)arg;
    const spi_bus_attr_t* bus_attr = host->bus_attr;
#if CONFIG_IDF_TARGET_ESP32
    //only for esp32 dma workaround usage
    const spi_dma_ctx_t *dma_ctx = host->dma_ctx;
#endif

#if SOC_SPI_SCT_SUPPORTED
    assert(spi_hal_usr_is_done(&host->hal) || spi_hal_get_intr_mask(&host->hal, SPI_LL_INTR_SEG_DONE));
#else
    assert(spi_hal_usr_is_done(&host->hal));
#endif

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

        if (bus_attr->dma_enabled) {
#if CONFIG_IDF_TARGET_ESP32
            //This workaround is only for esp32, where tx_dma_chan and rx_dma_chan are always same
            spicommon_dmaworkaround_idle(dma_ctx->tx_dma_chan.chan_id);
#endif  //#if CONFIG_IDF_TARGET_ESP32

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE   //invalidate here to let user access rx data in post_cb if possible
            if (host->cur_trans_buf.buffer_to_rcv) {
                uint16_t alignment = bus_attr->internal_mem_align_size;
                uint32_t buffer_byte_len = (host->cur_trans_buf.trans->rxlength + 7) / 8;
                buffer_byte_len = (buffer_byte_len + alignment - 1) & (~(alignment - 1));
                // invalidate priv_trans.buffer_to_rcv anyway, only user provide aligned buffer can rcv correct data in post_cb
                esp_err_t ret = esp_cache_msync((void *)host->cur_trans_buf.buffer_to_rcv, buffer_byte_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
                assert(ret == ESP_OK);
            }
#endif
        }

#if SOC_SPI_SCT_SUPPORTED
        if (host->sct_mode_enabled) {
            //cur_cs is changed to DEV_NUM_MAX here
            spi_post_sct_trans(host);
            if (!(host->device[cs]->cfg.flags & SPI_DEVICE_NO_RETURN_RESULT)) {
                xQueueSendFromISR(host->device[cs]->ret_queue, &host->cur_sct_trans, &do_yield);
            }
        } else
#endif  //#if SOC_SPI_SCT_SUPPORTED
        {
            //cur_cs is changed to DEV_NUM_MAX here
            spi_post_trans(host);
            if (!(host->device[cs]->cfg.flags & SPI_DEVICE_NO_RETURN_RESULT)) {
                //Return transaction descriptor.
                xQueueSendFromISR(host->device[cs]->ret_queue, &host->cur_trans_buf, &do_yield);
            }
        }
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
            // sanity check
            assert(desired_dev);

            bool dev_has_req = spi_bus_lock_bg_check_dev_req(desired_dev);
            if (dev_has_req) {
                device_to_send = host->device[spi_bus_lock_get_dev_id(desired_dev)];
#if SOC_SPI_SCT_SUPPORTED
                if (host->sct_mode_enabled) {
                    trans_found = xQueueReceiveFromISR(device_to_send->trans_queue, &host->cur_sct_trans, &do_yield);
                } else
#endif  //#if SOC_SPI_SCT_SUPPORTED
                {
                    trans_found = xQueueReceiveFromISR(device_to_send->trans_queue, &host->cur_trans_buf, &do_yield);
                }
                if (!trans_found) {
                    spi_bus_lock_bg_clear_req(desired_dev);
                }
            }
        }

        if (trans_found) {
#if SOC_SPI_SCT_SUPPORTED
            if (host->sct_mode_enabled) {
                spi_new_sct_trans(device_to_send, &host->cur_sct_trans);
            } else
#endif  //#if SOC_SPI_SCT_SUPPORTED
            {
                spi_trans_priv_t *const cur_trans_buf = &host->cur_trans_buf;
#if CONFIG_IDF_TARGET_ESP32
                if (bus_attr->dma_enabled && (cur_trans_buf->buffer_to_rcv || cur_trans_buf->buffer_to_send)) {
                    //mark channel as active, so that the DMA will not be reset by the slave
                    //This workaround is only for esp32, where tx_dma_chan and rx_dma_chan are always same
                    spicommon_dmaworkaround_transfer_active(dma_ctx->tx_dma_chan.chan_id);
                }
#endif  //#if CONFIG_IDF_TARGET_ESP32
                spi_new_trans(device_to_send, cur_trans_buf);
            }
        }

        // Exit of the ISR, handle interrupt re-enable (if sending transaction), retry (if there's coming BG),
        // or resume acquiring device task (if quit due to bus acquiring).
    } while (!spi_bus_lock_bg_exit(lock, trans_found, &do_yield));

    if (do_yield) {
        portYIELD_FROM_ISR();
    }
}

static SPI_MASTER_ISR_ATTR esp_err_t check_trans_valid(spi_device_handle_t handle, spi_transaction_t *trans_desc)
{
    SPI_CHECK(handle != NULL, "invalid dev handle", ESP_ERR_INVALID_ARG);
    spi_host_t *host = handle->host;
    const spi_bus_attr_t* bus_attr = host->bus_attr;
    bool tx_enabled = (trans_desc->flags & SPI_TRANS_USE_TXDATA) || (trans_desc->tx_buffer);
    bool rx_enabled = (trans_desc->flags & SPI_TRANS_USE_RXDATA) || (trans_desc->rx_buffer);
    spi_transaction_ext_t *t_ext = (spi_transaction_ext_t *)trans_desc;
    bool dummy_enabled = (((trans_desc->flags & SPI_TRANS_VARIABLE_DUMMY) ? t_ext->dummy_bits : handle->cfg.dummy_bits) != 0);
    bool extra_dummy_enabled = handle->hal_dev.timing_conf.timing_dummy;
    bool is_half_duplex = ((handle->cfg.flags & SPI_DEVICE_HALFDUPLEX) != 0);

    //check transmission length
    SPI_CHECK((trans_desc->flags & SPI_TRANS_USE_RXDATA) == 0 || trans_desc->rxlength <= 32, "SPI_TRANS_USE_RXDATA only available for rxdata transfer <= 32 bits", ESP_ERR_INVALID_ARG);
    SPI_CHECK((trans_desc->flags & SPI_TRANS_USE_TXDATA) == 0 || trans_desc->length <= 32, "SPI_TRANS_USE_TXDATA only available for txdata transfer <= 32 bits", ESP_ERR_INVALID_ARG);
    SPI_CHECK(trans_desc->length <= bus_attr->max_transfer_sz * 8, "txdata transfer > host maximum", ESP_ERR_INVALID_ARG);
    SPI_CHECK(trans_desc->rxlength <= bus_attr->max_transfer_sz * 8, "rxdata transfer > host maximum", ESP_ERR_INVALID_ARG);
    SPI_CHECK(is_half_duplex || trans_desc->rxlength <= trans_desc->length, "rx length > tx length in full duplex mode", ESP_ERR_INVALID_ARG);
    //check working mode
#if SOC_SPI_SUPPORT_OCT
    SPI_CHECK(!(host->id == SPI3_HOST && trans_desc->flags & SPI_TRANS_MODE_OCT), "SPI3 does not support octal mode", ESP_ERR_INVALID_ARG);
    SPI_CHECK(!((trans_desc->flags & SPI_TRANS_MODE_OCT) && (handle->cfg.flags & SPI_DEVICE_3WIRE)), "Incompatible when setting to both Octal mode and 3-wire-mode", ESP_ERR_INVALID_ARG);
    SPI_CHECK(!((trans_desc->flags & SPI_TRANS_MODE_OCT) && !is_half_duplex), "Incompatible when setting to both Octal mode and half duplex mode", ESP_ERR_INVALID_ARG);
#endif
    SPI_CHECK(!((trans_desc->flags & (SPI_TRANS_MODE_DIO | SPI_TRANS_MODE_QIO)) && (handle->cfg.flags & SPI_DEVICE_3WIRE)), "Incompatible when setting to both multi-line mode and 3-wire-mode", ESP_ERR_INVALID_ARG);
    SPI_CHECK(!((trans_desc->flags & (SPI_TRANS_MODE_DIO | SPI_TRANS_MODE_QIO)) && !is_half_duplex), "Incompatible when setting to both multi-line mode and half duplex mode", ESP_ERR_INVALID_ARG);
#ifdef CONFIG_IDF_TARGET_ESP32
    SPI_CHECK(!is_half_duplex || !bus_attr->dma_enabled || !rx_enabled || !tx_enabled, "SPI half duplex mode does not support using DMA with both MOSI and MISO phases.", ESP_ERR_INVALID_ARG);
#endif
#if !SOC_SPI_HD_BOTH_INOUT_SUPPORTED
    //On these chips, HW doesn't support using both TX and RX phases when in halfduplex mode
    SPI_CHECK(!is_half_duplex || !tx_enabled || !rx_enabled, "SPI half duplex mode is not supported when both MOSI and MISO phases are enabled.", ESP_ERR_INVALID_ARG);
    SPI_CHECK(!is_half_duplex || !trans_desc->length || !trans_desc->rxlength, "SPI half duplex mode is not supported when both MOSI and MISO phases are enabled.", ESP_ERR_INVALID_ARG);
#endif
    //MOSI phase is skipped only when both tx_buffer and SPI_TRANS_USE_TXDATA are not set.
    SPI_CHECK(trans_desc->length != 0 || !tx_enabled, "trans tx_buffer should be NULL and SPI_TRANS_USE_TXDATA should be cleared to skip MOSI phase.", ESP_ERR_INVALID_ARG);
    //MISO phase is skipped only when both rx_buffer and SPI_TRANS_USE_RXDATA are not set.
    //If set rxlength=0 in full_duplex mode, it will be automatically set to length
    SPI_CHECK(!is_half_duplex || trans_desc->rxlength != 0 || !rx_enabled, "trans rx_buffer should be NULL and SPI_TRANS_USE_RXDATA should be cleared to skip MISO phase.", ESP_ERR_INVALID_ARG);
    //In Full duplex mode, default rxlength to be the same as length, if not filled in.
    // set rxlength to length is ok, even when rx buffer=NULL
    if (trans_desc->rxlength == 0 && !is_half_duplex) {
        trans_desc->rxlength = trans_desc->length;
    }
    //Dummy phase is not available when both data out and in are enabled, regardless of FD or HD mode.
    SPI_CHECK(!tx_enabled || !rx_enabled || !dummy_enabled || !extra_dummy_enabled, "Dummy phase is not available when both data out and in are enabled", ESP_ERR_INVALID_ARG);

    if (bus_attr->dma_enabled) {
        SPI_CHECK(trans_desc->length <= SPI_LL_DMA_MAX_BIT_LEN, "txdata transfer > hardware max supported len", ESP_ERR_INVALID_ARG);
        SPI_CHECK(trans_desc->rxlength <= SPI_LL_DMA_MAX_BIT_LEN, "rxdata transfer > hardware max supported len", ESP_ERR_INVALID_ARG);
    } else {
        SPI_CHECK(trans_desc->length <= SPI_LL_CPU_MAX_BIT_LEN, "txdata transfer > hardware max supported len", ESP_ERR_INVALID_ARG);
        SPI_CHECK(trans_desc->rxlength <= SPI_LL_CPU_MAX_BIT_LEN, "rxdata transfer > hardware max supported len", ESP_ERR_INVALID_ARG);
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
    if (trans_buf->buffer_to_rcv && (void *)trans_buf->buffer_to_rcv != &trans_desc->rx_data[0] && trans_buf->buffer_to_rcv != trans_desc->rx_buffer) { // NOLINT(clang-analyzer-unix.Malloc)
        if (trans_desc->flags & SPI_TRANS_USE_RXDATA) {
            memcpy((uint8_t *) & trans_desc->rx_data[0], trans_buf->buffer_to_rcv, (trans_desc->rxlength + 7) / 8);
        } else {
            memcpy(trans_desc->rx_buffer, trans_buf->buffer_to_rcv, (trans_desc->rxlength + 7) / 8);
        }
        free(trans_buf->buffer_to_rcv);
    }
}

static SPI_MASTER_ISR_ATTR esp_err_t setup_priv_desc(spi_host_t *host, spi_trans_priv_t* priv_desc)
{
    spi_transaction_t *trans_desc = priv_desc->trans;
    const spi_bus_attr_t *bus_attr = host->bus_attr;
    uint16_t alignment = bus_attr->internal_mem_align_size;

    // rx memory assign
    uint32_t* rcv_ptr;
    if (trans_desc->flags & SPI_TRANS_USE_RXDATA) {
        rcv_ptr = (uint32_t *)&trans_desc->rx_data[0];
    } else {
        //if not use RXDATA neither rx_buffer, buffer_to_rcv assigned to NULL
        rcv_ptr = trans_desc->rx_buffer;
    }

    // tx memory assign
    const uint32_t *send_ptr;
    if (trans_desc->flags & SPI_TRANS_USE_TXDATA) {
        send_ptr = (uint32_t *)&trans_desc->tx_data[0];
    } else {
        //if not use TXDATA neither tx_buffer, tx data assigned to NULL
        send_ptr = trans_desc->tx_buffer ;
    }

    uint32_t tx_byte_len = (trans_desc->length + 7) / 8;
    uint32_t rx_byte_len = (trans_desc->rxlength + 7) / 8;
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    bool tx_unaligned = ((((uint32_t)send_ptr) | tx_byte_len) & (alignment - 1));
    bool rx_unaligned = ((((uint32_t)rcv_ptr) | rx_byte_len) & (alignment - 1));
#else
    bool tx_unaligned = false;   //tx don't need align on addr or length, for other chips
    bool rx_unaligned = (((uint32_t)rcv_ptr) & (alignment - 1));
#endif

    if (send_ptr && bus_attr->dma_enabled) {
        if ((!esp_ptr_dma_capable(send_ptr) || tx_unaligned)) {
            ESP_RETURN_ON_FALSE(!(trans_desc->flags & SPI_TRANS_DMA_BUFFER_ALIGN_MANUAL), ESP_ERR_INVALID_ARG, SPI_TAG, "Set flag SPI_TRANS_DMA_BUFFER_ALIGN_MANUAL but TX buffer addr&len not align to %d, or not dma_capable", alignment);
            //if txbuf in the desc not DMA-capable, or not bytes aligned to alignment, malloc a new one
            ESP_EARLY_LOGD(SPI_TAG, "Allocate TX buffer for DMA");
            tx_byte_len = (tx_byte_len + alignment - 1) & (~(alignment - 1));   // up align alignment
            uint32_t *temp = heap_caps_aligned_alloc(alignment, tx_byte_len, MALLOC_CAP_DMA);
            if (temp == NULL) {
                goto clean_up;
            }

            memcpy(temp, send_ptr, (trans_desc->length + 7) / 8);
            send_ptr = temp;
        }
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        esp_err_t ret = esp_cache_msync((void *)send_ptr, tx_byte_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        assert(ret == ESP_OK);
#endif
    }

    if (rcv_ptr && bus_attr->dma_enabled && (!esp_ptr_dma_capable(rcv_ptr) || rx_unaligned)) {
        ESP_RETURN_ON_FALSE(!(trans_desc->flags & SPI_TRANS_DMA_BUFFER_ALIGN_MANUAL), ESP_ERR_INVALID_ARG, SPI_TAG, "Set flag SPI_TRANS_DMA_BUFFER_ALIGN_MANUAL but RX buffer addr&len not align to %d, or not dma_capable", alignment);
        //if rxbuf in the desc not DMA-capable, or not aligned to alignment, malloc a new one
        ESP_EARLY_LOGD(SPI_TAG, "Allocate RX buffer for DMA");
        rx_byte_len = (rx_byte_len + alignment - 1) & (~(alignment - 1));   // up align alignment
        rcv_ptr = heap_caps_aligned_alloc(alignment, rx_byte_len, MALLOC_CAP_DMA);
        if (rcv_ptr == NULL) {
            goto clean_up;
        }
    }
    priv_desc->buffer_to_send = send_ptr;
    priv_desc->buffer_to_rcv = rcv_ptr;
    return ESP_OK;

clean_up:
    uninstall_priv_desc(priv_desc);
    return ESP_ERR_NO_MEM;
}

esp_err_t SPI_MASTER_ATTR spi_device_queue_trans(spi_device_handle_t handle, spi_transaction_t *trans_desc, TickType_t ticks_to_wait)
{
    esp_err_t ret = check_trans_valid(handle, trans_desc);
    if (ret != ESP_OK) {
        return ret;
    }

    spi_host_t *host = handle->host;

    SPI_CHECK(!spi_bus_device_is_polling(handle), "Cannot queue new transaction while previous polling transaction is not terminated.", ESP_ERR_INVALID_STATE);

    /* Even when using interrupt transfer, the CS can only be kept activated if the bus has been
     * acquired with `spi_device_acquire_bus()` first. */
    if (host->device_acquiring_lock != handle && (trans_desc->flags & SPI_TRANS_CS_KEEP_ACTIVE)) {
        return ESP_ERR_INVALID_ARG;
    }

    spi_trans_priv_t trans_buf = { .trans = trans_desc, };
    ret = setup_priv_desc(host, &trans_buf);
    if (ret != ESP_OK) {
        return ret;
    }

#ifdef CONFIG_PM_ENABLE
    // though clock source is selectable, read/write reg and mem of spi peripheral still use APB
    // and dma still use APB, so pm_lock is still needed
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
    SPI_CHECK(handle != NULL, "invalid dev handle", ESP_ERR_INVALID_ARG);
    bool use_dma = handle->host->bus_attr->dma_enabled;

    //if SPI_DEVICE_NO_RETURN_RESULT is set, ret_queue will always be empty
    SPI_CHECK(!(handle->cfg.flags & SPI_DEVICE_NO_RETURN_RESULT), "API not Supported!", ESP_ERR_NOT_SUPPORTED);

    //use the interrupt, block until return
    r = xQueueReceive(handle->ret_queue, (void*)&trans_buf, ticks_to_wait);
    if (!r) {
        // The memory occupied by rx and tx DMA buffer destroyed only when receiving from the queue (transaction finished).
        // If timeout, wait and retry.
        // Every in-flight transaction request occupies internal memory as DMA buffer if needed.
        return ESP_ERR_TIMEOUT;
    }
    //release temporary buffers used by dma
    if (use_dma) {
        uninstall_priv_desc(&trans_buf);
    }
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
    if (ret != ESP_OK) {
        return ret;
    }

    ret = spi_device_get_trans_result(handle, &ret_trans, portMAX_DELAY);
    if (ret != ESP_OK) {
        return ret;
    }

    assert(ret_trans == trans_desc);
    return ESP_OK;
}

esp_err_t SPI_MASTER_ISR_ATTR spi_device_acquire_bus(spi_device_t *device, TickType_t wait)
{
    spi_host_t *const host = device->host;
    SPI_CHECK(wait == portMAX_DELAY, "acquire finite time not supported now.", ESP_ERR_INVALID_ARG);
    SPI_CHECK(!spi_bus_device_is_polling(device), "Cannot acquire bus when a polling transaction is in progress.", ESP_ERR_INVALID_STATE);

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

#if CONFIG_IDF_TARGET_ESP32
    if (host->bus_attr->dma_enabled) {
        //This workaround is only for esp32, where tx_dma_chan and rx_dma_chan are always same
        spicommon_dmaworkaround_transfer_active(host->dma_ctx->tx_dma_chan.chan_id);
    }
#endif  //#if CONFIG_IDF_TARGET_ESP32

    return ESP_OK;
}

// This function restore configurations required in the non-polling mode
void SPI_MASTER_ISR_ATTR spi_device_release_bus(spi_device_t *dev)
{
    spi_host_t *host = dev->host;

    if (spi_bus_device_is_polling(dev)) {
        ESP_EARLY_LOGE(SPI_TAG, "Cannot release bus when a polling transaction is in progress.");
        assert(0);
    }

#if CONFIG_IDF_TARGET_ESP32
    if (host->bus_attr->dma_enabled) {
        //This workaround is only for esp32, where tx_dma_chan and rx_dma_chan are always same
        spicommon_dmaworkaround_idle(host->dma_ctx->tx_dma_chan.chan_id);
    }
    //Tell common code DMA workaround that our DMA channel is idle. If needed, the code will do a DMA reset.
#endif  //#if CONFIG_IDF_TARGET_ESP32

    //allow clock to be lower than 80MHz when all tasks blocked
#ifdef CONFIG_PM_ENABLE
    //Release APB frequency lock
    esp_pm_lock_release(host->bus_attr->pm_lock);
#endif
    ESP_LOGD(SPI_TAG, "device%d release bus", dev->id);

    host->device_acquiring_lock = NULL;
    esp_err_t ret = spi_bus_lock_acquire_end(dev->dev_lock);
    assert(ret == ESP_OK);
    (void) ret;
}

esp_err_t SPI_MASTER_ISR_ATTR spi_device_polling_start(spi_device_handle_t handle, spi_transaction_t *trans_desc, TickType_t ticks_to_wait)
{
    esp_err_t ret;
    SPI_CHECK(ticks_to_wait == portMAX_DELAY, "currently timeout is not available for polling transactions", ESP_ERR_INVALID_ARG);
    ret = check_trans_valid(handle, trans_desc);
    if (ret != ESP_OK) {
        return ret;
    }
    SPI_CHECK(!spi_bus_device_is_polling(handle), "Cannot send polling transaction while the previous polling transaction is not terminated.", ESP_ERR_INVALID_STATE);

    spi_host_t *host = handle->host;
    spi_trans_priv_t priv_polling_trans = { .trans = trans_desc, };
    ret = setup_priv_desc(host, &priv_polling_trans);
    if (ret != ESP_OK) {
        return ret;
    }

    /* If device_acquiring_lock is set to handle, it means that the user has already
     * acquired the bus thanks to the function `spi_device_acquire_bus()`.
     * In that case, we don't need to take the lock again. */
    if (host->device_acquiring_lock != handle) {
        /* The user cannot ask for the CS to keep active has the bus is not locked/acquired. */
        if ((trans_desc->flags & SPI_TRANS_CS_KEEP_ACTIVE) != 0) {
            ret = ESP_ERR_INVALID_ARG;
        } else {
            ret = spi_bus_lock_acquire_start(handle->dev_lock, ticks_to_wait);
        }
    } else {
        ret = spi_bus_lock_wait_bg_done(handle->dev_lock, ticks_to_wait);
    }
    if (ret != ESP_OK) {
        uninstall_priv_desc(&priv_polling_trans);
        ESP_LOGE(SPI_TAG, "polling can't get buslock");
        return ret;
    }
    //After holding the buslock, common resource can be accessed !!

    //Polling, no interrupt is used.
    host->polling = true;
    host->cur_trans_buf = priv_polling_trans;

    ESP_LOGV(SPI_TAG, "polling trans");
    spi_new_trans(handle, &host->cur_trans_buf);

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

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE   //invalidate here to let user access rx data in post_cb if possible
    const spi_bus_attr_t *bus_attr = host->bus_attr;
    if (host->cur_trans_buf.buffer_to_rcv) {
        uint16_t alignment = bus_attr->internal_mem_align_size;
        uint32_t buffer_byte_len = (host->cur_trans_buf.trans->rxlength + 7) / 8;
        buffer_byte_len = (buffer_byte_len + alignment - 1) & (~(alignment - 1));
        esp_err_t ret = esp_cache_msync((void *)host->cur_trans_buf.buffer_to_rcv, buffer_byte_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        if (ret != ESP_OK) {
            return ret;
        }
    }
#endif

    ESP_LOGV(SPI_TAG, "polling trans done");
    //deal with the in-flight transaction
    spi_post_trans(host);
    //release temporary buffers
    uninstall_priv_desc(&host->cur_trans_buf);

    host->polling = false;
    /* Once again here, if device_acquiring_lock is set to `handle`, it means that the user has already
     * acquired the bus thanks to the function `spi_device_acquire_bus()`.
     * In that case, the lock must not be released now because . */
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
    if (ret != ESP_OK) {
        return ret;
    }

    return spi_device_polling_end(handle, portMAX_DELAY);
}

esp_err_t spi_bus_get_max_transaction_len(spi_host_device_t host_id, size_t *max_bytes)
{
    SPI_CHECK(is_valid_host(host_id), "invalid host", ESP_ERR_INVALID_ARG);
    if (bus_driver_ctx[host_id] == NULL || max_bytes == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    spi_host_t *host = bus_driver_ctx[host_id];
    if (host->bus_attr->dma_enabled) {
        *max_bytes = MIN(host->bus_attr->max_transfer_sz, (SPI_LL_DMA_MAX_BIT_LEN / 8));
    } else {
        *max_bytes = MIN(host->bus_attr->max_transfer_sz, (SPI_LL_CPU_MAX_BIT_LEN / 8));
    }

    return ESP_OK;
}

#if SOC_SPI_SCT_SUPPORTED

/*-----------------------------------------------------------
 * Below functions should be in the same spinlock
 *-----------------------------------------------------------*/
/*-------------------------
 *            TX
 *------------------------*/
static void SPI_MASTER_ISR_ATTR spi_hal_sct_tx_dma_desc_recycle(spi_sct_desc_ctx_t *desc_ctx, uint32_t recycle_num)
{
    desc_ctx->tx_free_desc_num += recycle_num;
}

static void s_sct_prepare_tx_seg(spi_sct_desc_ctx_t *desc_ctx, const uint32_t conf_buffer[SOC_SPI_SCT_BUFFER_NUM_MAX], const void *send_buffer, uint32_t buf_len_bytes, spi_dma_desc_t **trans_head)
{
    HAL_ASSERT(desc_ctx->tx_free_desc_num >= 1 + lldesc_get_required_num(buf_len_bytes));
    const spi_dma_ctx_t *dma_ctx = __containerof(desc_ctx, spi_host_t, sct_desc_pool)->dma_ctx;

    *trans_head = desc_ctx->cur_tx_seg_link;
    spicommon_dma_desc_setup_link(desc_ctx->cur_tx_seg_link, conf_buffer, SOC_SPI_SCT_BUFFER_NUM_MAX * 4, false);
    spi_dma_desc_t *conf_buffer_link = desc_ctx->cur_tx_seg_link;
    desc_ctx->tx_free_desc_num -= 1;

    desc_ctx->tx_seg_link_tail = desc_ctx->cur_tx_seg_link;
    desc_ctx->cur_tx_seg_link++;
    if (desc_ctx->cur_tx_seg_link == dma_ctx->dmadesc_tx + dma_ctx->dma_desc_num) {
        //As there is enough space, so we simply point this to the pool head
        desc_ctx->cur_tx_seg_link = dma_ctx->dmadesc_tx;
    }

    if (send_buffer && buf_len_bytes) {
        spicommon_dma_desc_setup_link(desc_ctx->cur_tx_seg_link, send_buffer, buf_len_bytes, false);
        conf_buffer_link->next = desc_ctx->cur_tx_seg_link;
        for (int i = 0; i < lldesc_get_required_num(buf_len_bytes); i++) {
            desc_ctx->tx_seg_link_tail = desc_ctx->cur_tx_seg_link;
            desc_ctx->cur_tx_seg_link++;
            if (desc_ctx->cur_tx_seg_link == dma_ctx->dmadesc_tx + dma_ctx->dma_desc_num) {
                //As there is enough space, so we simply point this to the pool head
                desc_ctx->cur_tx_seg_link = dma_ctx->dmadesc_tx;
            }
        }
        desc_ctx->tx_free_desc_num -= lldesc_get_required_num(buf_len_bytes);
    }
}

static esp_err_t spi_hal_sct_new_tx_dma_desc_head(spi_sct_desc_ctx_t *desc_ctx, const uint32_t conf_buffer[SOC_SPI_SCT_BUFFER_NUM_MAX], const void *send_buffer, uint32_t buf_len_bytes, spi_dma_desc_t **trans_head, uint32_t *used_desc_num)
{
    //1 desc for the conf_buffer, other for data.
    if (desc_ctx->tx_free_desc_num < 1 + lldesc_get_required_num(buf_len_bytes)) {
        return ESP_ERR_NO_MEM;
    }

    s_sct_prepare_tx_seg(desc_ctx, conf_buffer, send_buffer, buf_len_bytes, trans_head);
    *used_desc_num = 1 + lldesc_get_required_num(buf_len_bytes);

    return ESP_OK;
}

static esp_err_t spi_hal_sct_link_tx_seg_dma_desc(spi_sct_desc_ctx_t *desc_ctx, const uint32_t conf_buffer[SOC_SPI_SCT_BUFFER_NUM_MAX], const void *send_buffer, uint32_t buf_len_bytes, uint32_t *used_desc_num)
{
    //1 desc for the conf_buffer, other for data.
    if (desc_ctx->tx_free_desc_num < 1 + lldesc_get_required_num(buf_len_bytes)) {
        return ESP_ERR_NO_MEM;
    }

    if (desc_ctx->tx_seg_link_tail) {
        //Connect last segment to the current segment, as we're sure the `s_sct_prepare_tx_seg` next won't fail.
        desc_ctx->tx_seg_link_tail->next = desc_ctx->cur_tx_seg_link;
    }

    spi_dma_desc_t *internal_head = NULL;
    s_sct_prepare_tx_seg(desc_ctx, conf_buffer, send_buffer, buf_len_bytes, &internal_head);
    *used_desc_num += 1 + lldesc_get_required_num(buf_len_bytes);

    return ESP_OK;
}

// /*-------------------------
//  *            RX
//  *------------------------*/
static void SPI_MASTER_ISR_ATTR spi_hal_sct_rx_dma_desc_recycle(spi_sct_desc_ctx_t *desc_ctx, uint32_t recycle_num)
{
    desc_ctx->rx_free_desc_num += recycle_num;
}

static void s_sct_prepare_rx_seg(spi_sct_desc_ctx_t *desc_ctx, const void *recv_buffer, uint32_t buf_len_bytes, spi_dma_desc_t **trans_head)
{
    HAL_ASSERT(desc_ctx->rx_free_desc_num >= lldesc_get_required_num(buf_len_bytes));
    const spi_dma_ctx_t *dma_ctx = __containerof(desc_ctx, spi_host_t, sct_desc_pool)->dma_ctx;

    *trans_head = desc_ctx->cur_rx_seg_link;
    spicommon_dma_desc_setup_link(desc_ctx->cur_rx_seg_link, recv_buffer, buf_len_bytes, true);
    for (int i = 0; i < lldesc_get_required_num(buf_len_bytes); i++) {
        desc_ctx->rx_seg_link_tail = desc_ctx->cur_rx_seg_link;
        desc_ctx->cur_rx_seg_link++;
        if (desc_ctx->cur_rx_seg_link == dma_ctx->dmadesc_rx + dma_ctx->dma_desc_num) {
            //As there is enough space, so we simply point this to the pool head
            desc_ctx->cur_rx_seg_link = dma_ctx->dmadesc_rx;
        }
    }

    desc_ctx->rx_free_desc_num -= lldesc_get_required_num(buf_len_bytes);
}

static esp_err_t spi_hal_sct_new_rx_dma_desc_head(spi_sct_desc_ctx_t *desc_ctx, const void *recv_buffer, uint32_t buf_len_bytes, spi_dma_desc_t **trans_head, uint32_t *used_desc_num)
{
    if (desc_ctx->rx_free_desc_num < lldesc_get_required_num(buf_len_bytes)) {
        return ESP_ERR_NO_MEM;
    }

    s_sct_prepare_rx_seg(desc_ctx, recv_buffer, buf_len_bytes, trans_head);
    *used_desc_num = lldesc_get_required_num(buf_len_bytes);

    return ESP_OK;
}

static esp_err_t spi_hal_sct_link_rx_seg_dma_desc(spi_sct_desc_ctx_t *desc_ctx, const void *recv_buffer, uint32_t buf_len_bytes,  uint32_t *used_desc_num)
{
    if (desc_ctx->rx_free_desc_num < lldesc_get_required_num(buf_len_bytes)) {
        return ESP_ERR_NO_MEM;
    }

    if (desc_ctx->rx_seg_link_tail) {
        //Connect last segment to the current segment, as we're sure the `s_sct_prepare_tx_seg` next won't fail.
        desc_ctx->rx_seg_link_tail->next = desc_ctx->cur_rx_seg_link;
    }

    spi_dma_desc_t *internal_head = NULL;
    s_sct_prepare_rx_seg(desc_ctx, recv_buffer, buf_len_bytes, &internal_head);
    *used_desc_num += lldesc_get_required_num(buf_len_bytes);

    return ESP_OK;
}

static void s_spi_sct_reset_dma_pool(const spi_dma_ctx_t *dma_ctx, spi_sct_desc_ctx_t *sct_desc_pool)
{
    sct_desc_pool->tx_free_desc_num = dma_ctx->dma_desc_num;
    sct_desc_pool->rx_free_desc_num = dma_ctx->dma_desc_num;
    sct_desc_pool->cur_tx_seg_link = dma_ctx->dmadesc_tx;
    sct_desc_pool->cur_rx_seg_link = dma_ctx->dmadesc_rx;
    sct_desc_pool->tx_seg_link_tail = NULL;
    sct_desc_pool->rx_seg_link_tail = NULL;
}

/**
 * This function will turn this host into SCT (segmented-configure-transfer) mode.
 *
 * No concurrency guarantee, if a transaction is ongoing, calling this will lead to wrong transaction
 */
esp_err_t spi_bus_multi_trans_mode_enable(spi_device_handle_t handle, bool enable)
{
    SPI_CHECK(handle, "Invalid arguments.", ESP_ERR_INVALID_ARG);
    SPI_CHECK(SOC_SPI_SCT_SUPPORTED_PERIPH(handle->host->id), "Invalid arguments", ESP_ERR_INVALID_ARG);
    SPI_CHECK(handle->cfg.flags & SPI_DEVICE_HALFDUPLEX, "SCT mode only available under Half Duplex mode", ESP_ERR_INVALID_STATE);
    SPI_CHECK(!spi_bus_device_is_polling(handle), "Cannot queue new transaction while previous polling transaction is not terminated.", ESP_ERR_INVALID_STATE);
    SPI_CHECK(uxQueueMessagesWaiting(handle->trans_queue) == 0, "Cannot enable SCT mode when internal Queue still has items", ESP_ERR_INVALID_STATE);

    esp_err_t ret = ESP_OK;
    if (enable) {
        /**
         * This `fake_trans` transaction descriptor is only used to initialise the SPI registers
         * This transaction won't be triggered.
         */
        spi_transaction_t fake_trans = {
            .flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA,
            .length = 8,
            .tx_data = {0xff},
        };

        spi_host_t *host = handle->host;
        spi_trans_priv_t trans_buf = { .trans = &fake_trans };
        spi_hal_context_t *hal = &handle->host->hal;
        spi_hal_dev_config_t *hal_dev = &handle->hal_dev;
        //As we know the `fake_trans` are internal, so no need to `uninstall_priv_desc`
        ret = setup_priv_desc(host, &trans_buf);
        if (ret != ESP_OK) {
            return ret;
        }

        //init SPI registers
        spi_hal_setup_device(hal, hal_dev);
        spi_hal_trans_config_t hal_trans = {};
        spi_format_hal_trans_struct(handle, &trans_buf, &hal_trans);
        spi_hal_setup_trans(hal, hal_dev, &hal_trans);
#if CONFIG_IDF_TARGET_ESP32S2
        // conf_base need ensure transaction gap len more than about 2us under different freq.
        // conf_base only configurable on s2.
        spi_hal_sct_setup_conf_base(hal, handle->hal_dev.timing_conf.real_freq / 600000);
#endif

        s_spi_sct_reset_dma_pool(host->dma_ctx, &host->sct_desc_pool);
        spi_hal_sct_init(hal);
    } else {
        spi_hal_sct_deinit(&handle->host->hal);
    }

    handle->host->sct_mode_enabled = enable;

    return ESP_OK;
}

static void SPI_MASTER_ATTR s_sct_init_conf_buffer(spi_hal_context_t *hal, uint32_t *buffer, uint32_t trans_num)
{
    // read from HW need waiting for slower APB clock domain return data, loop to contact slow clock domain will waste time.
    // use one imagen then copied by cpu instead.
    uint32_t conf_buffer_img[SOC_SPI_SCT_BUFFER_NUM_MAX];
    spi_hal_sct_init_conf_buffer(hal, conf_buffer_img);

    for (int i = 0; i < trans_num; i++) {
        memcpy(&buffer[i * SOC_SPI_SCT_BUFFER_NUM_MAX], conf_buffer_img, sizeof(conf_buffer_img));
    }
}

static void SPI_MASTER_ATTR s_sct_format_conf_buffer(spi_device_handle_t handle, spi_multi_transaction_t *seg_trans_desc, uint32_t *buffer, bool seg_end)
{
    spi_hal_context_t *hal = &handle->host->hal;
    spi_hal_dev_config_t *hal_dev = &handle->hal_dev;
    spi_hal_seg_config_t seg_config = {};

    //prep
    if (seg_trans_desc->seg_trans_flags & SPI_MULTI_TRANS_PREP_LEN_UPDATED) {
        seg_config.cs_setup = seg_trans_desc->cs_ena_pretrans;
    } else {
        seg_config.cs_setup = handle->cfg.cs_ena_pretrans;
    }

    //cmd
    seg_config.cmd = seg_trans_desc->base.cmd;
    if (seg_trans_desc->seg_trans_flags & SPI_MULTI_TRANS_CMD_LEN_UPDATED) {
        seg_config.cmd_bits = seg_trans_desc->command_bits;
    } else {
        seg_config.cmd_bits = handle->cfg.command_bits;
    }

    //addr
    seg_config.addr = seg_trans_desc->base.addr;
    if (seg_trans_desc->seg_trans_flags & SPI_MULTI_TRANS_ADDR_LEN_UPDATED) {
        seg_config.addr_bits = seg_trans_desc->address_bits;
    } else {
        seg_config.addr_bits = handle->cfg.address_bits;
    }

    //dummy
    if (seg_trans_desc->seg_trans_flags & SPI_MULTI_TRANS_DUMMY_LEN_UPDATED) {
        seg_config.dummy_bits = seg_trans_desc->dummy_bits;
    } else {
        seg_config.dummy_bits = handle->cfg.dummy_bits;
    }

    //dout
    seg_config.tx_bitlen = seg_trans_desc->base.length;

    //din
    seg_config.rx_bitlen = seg_trans_desc->base.rxlength;

    //done
    if (seg_trans_desc->seg_trans_flags & SPI_MULTI_TRANS_DONE_LEN_UPDATED) {
        seg_config.cs_hold = seg_trans_desc->cs_ena_posttrans;
    } else {
        seg_config.cs_hold = handle->cfg.cs_ena_posttrans;
    }

    //conf
    if (seg_end) {
        seg_config.seg_end = true;
    }
    seg_config.seg_gap_len = seg_trans_desc->sct_gap_len;

    // set line mode to hal_config
    spi_sct_set_hal_trans_config(seg_trans_desc, &hal->trans_config);
    spi_hal_sct_format_conf_buffer(hal, &seg_config, hal_dev, buffer);
}

esp_err_t SPI_MASTER_ATTR spi_device_queue_multi_trans(spi_device_handle_t handle, spi_multi_transaction_t *seg_trans_desc, uint32_t trans_num, TickType_t ticks_to_wait)
{
    SPI_CHECK(handle, "Invalid arguments.", ESP_ERR_INVALID_ARG);
    SPI_CHECK(SOC_SPI_SCT_SUPPORTED_PERIPH(handle->host->id), "Invalid arguments", ESP_ERR_INVALID_ARG);
    SPI_CHECK(handle->host->sct_mode_enabled == 1, "SCT mode isn't enabled", ESP_ERR_INVALID_STATE);
    esp_err_t ret = ESP_OK;

    uint16_t alignment = handle->host->bus_attr->internal_mem_align_size;
    uint32_t *conf_buffer = heap_caps_aligned_alloc(alignment, (trans_num * SOC_SPI_SCT_BUFFER_NUM_MAX * sizeof(uint32_t)), MALLOC_CAP_DMA);
    SPI_CHECK(conf_buffer, "No enough memory", ESP_ERR_NO_MEM);

    for (int i = 0; i < trans_num; i++) {
        ret = check_trans_valid(handle, (spi_transaction_t *)&seg_trans_desc[i]);
        if (ret != ESP_OK) {
            return ret;
        }
    }
    SPI_CHECK(!spi_bus_device_is_polling(handle), "Cannot queue new transaction while previous polling transaction is not terminated.", ESP_ERR_INVALID_STATE);

    spi_hal_context_t *hal = &handle->host->hal;
    s_sct_init_conf_buffer(hal, conf_buffer, trans_num);

    static esp_err_t dma_desc_status = ESP_FAIL;
    spi_dma_desc_t *tx_seg_head = NULL;
    uint32_t tx_used_dma_desc_num = 0;
    uint32_t tx_buf_len = 0;
    spi_dma_desc_t *rx_seg_head = NULL;
    uint32_t rx_used_dma_desc_num = 0;
    uint32_t rx_buf_len = 0;

    /*--------------Get segment head--------------*/
    s_sct_format_conf_buffer(handle, &seg_trans_desc[0], conf_buffer, (trans_num == 1));

    //TX
    tx_buf_len = (seg_trans_desc[0].base.length + 8 - 1) / 8;
    portENTER_CRITICAL(&handle->host->spinlock);
    dma_desc_status = spi_hal_sct_new_tx_dma_desc_head(&handle->host->sct_desc_pool, conf_buffer, seg_trans_desc[0].base.tx_buffer, tx_buf_len, &tx_seg_head, &tx_used_dma_desc_num);
    portEXIT_CRITICAL(&handle->host->spinlock);
    SPI_CHECK(dma_desc_status == ESP_OK, "No available dma descriptors, increase the `max_transfer_sz`, or wait queued transactions are done", ESP_ERR_INVALID_STATE);

    //RX
    //This is modified to the same length as tx length, when in fd mode, else it's `rxlength`
    rx_buf_len = (seg_trans_desc[0].base.rxlength + 8 - 1) / 8;
    if (seg_trans_desc[0].base.rx_buffer) {
        portENTER_CRITICAL(&handle->host->spinlock);
        dma_desc_status = spi_hal_sct_new_rx_dma_desc_head(&handle->host->sct_desc_pool, seg_trans_desc[0].base.rx_buffer, rx_buf_len, &rx_seg_head, &rx_used_dma_desc_num);
        portEXIT_CRITICAL(&handle->host->spinlock);
        SPI_CHECK(dma_desc_status == ESP_OK, "No available dma descriptors, increase the `max_transfer_sz`, or wait queued transactions are done", ESP_ERR_INVALID_STATE);
    }

    /*--------------Prepare other segments--------------*/
    for (int i = 1; i < trans_num; i++) {
        s_sct_format_conf_buffer(handle, &seg_trans_desc[i], &conf_buffer[i * SOC_SPI_SCT_BUFFER_NUM_MAX], (i == (trans_num - 1)));

        //TX
        tx_buf_len = (seg_trans_desc[i].base.length + 8 - 1) / 8;
        portENTER_CRITICAL(&handle->host->spinlock);
        dma_desc_status = spi_hal_sct_link_tx_seg_dma_desc(&handle->host->sct_desc_pool, &conf_buffer[i * SOC_SPI_SCT_BUFFER_NUM_MAX], seg_trans_desc[i].base.tx_buffer, tx_buf_len, &tx_used_dma_desc_num);
        portEXIT_CRITICAL(&handle->host->spinlock);
        SPI_CHECK(dma_desc_status == ESP_OK, "No available dma descriptors, increase the `max_transfer_sz`, or wait queued transactions are done", ESP_ERR_INVALID_STATE);

        //RX
        if (seg_trans_desc[i].base.rx_buffer) {
            //This is modified to the same length as tx length, when in fd mode, else it's `rxlength`
            rx_buf_len = (seg_trans_desc[i].base.rxlength + 8 - 1) / 8;
            portENTER_CRITICAL(&handle->host->spinlock);
            dma_desc_status = spi_hal_sct_link_rx_seg_dma_desc(&handle->host->sct_desc_pool, seg_trans_desc[i].base.rx_buffer, rx_buf_len, &rx_used_dma_desc_num);
            portEXIT_CRITICAL(&handle->host->spinlock);
        }
    }

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(handle->host->bus_attr->pm_lock);
#endif

    spi_sct_trans_priv_t sct_desc = {
        .tx_seg_head = tx_seg_head,
        .rx_seg_head = rx_seg_head,
        .sct_trans_desc_head = seg_trans_desc,
        .sct_conf_buffer = conf_buffer,
        .tx_used_desc_num = tx_used_dma_desc_num,
        .rx_used_desc_num = rx_used_dma_desc_num,
    };

    BaseType_t r = xQueueSend(handle->trans_queue, (void *)&sct_desc, ticks_to_wait);
    if (!r) {
#ifdef CONFIG_PM_ENABLE
        //Release APB frequency lock
        esp_pm_lock_release(handle->host->bus_attr->pm_lock);
#endif
        return ESP_ERR_TIMEOUT;
    }

    // The ISR will be invoked at correct time by the lock with `spi_bus_intr_enable`.
    ret = spi_bus_lock_bg_request(handle->dev_lock);
    if (ret != ESP_OK) {
        return ret;
    }

    return ESP_OK;
}

esp_err_t SPI_MASTER_ATTR spi_device_get_multi_trans_result(spi_device_handle_t handle, spi_multi_transaction_t **seg_trans_desc, TickType_t ticks_to_wait)
{
    SPI_CHECK(handle, "Invalid arguments.", ESP_ERR_INVALID_ARG);
    SPI_CHECK(SOC_SPI_SCT_SUPPORTED_PERIPH(handle->host->id), "Invalid arguments", ESP_ERR_INVALID_ARG);
    SPI_CHECK(handle->host->sct_mode_enabled == 1, "SCT mode isn't enabled", ESP_ERR_INVALID_STATE);
    spi_sct_trans_priv_t sct_desc = {};

    BaseType_t r = xQueueReceive(handle->ret_queue, (void *)&sct_desc, ticks_to_wait);
    if (!r) {
        return ESP_ERR_TIMEOUT;
    }

    *seg_trans_desc = sct_desc.sct_trans_desc_head;

    return ESP_OK;
}
#endif  //#if SOC_SPI_SCT_SUPPORTED
