/*
 * SPDX-FileCopyrightText: 2010-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdatomic.h>
#include "esp_compiler.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_memory_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_private/sleep_retention.h"
#include "esp_private/spi_common_internal.h"
#include "esp_private/spi_share_hw_ctrl.h"
#include "esp_private/esp_cache_private.h"
#include "driver/spi_slave_hd.h"
#include "hal/spi_slave_hd_hal.h"
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#include "esp_cache.h"
#endif

#ifdef CONFIG_SPI_SLAVE_ISR_IN_IRAM
#define SPI_SLAVE_ISR_ATTR IRAM_ATTR
#else
#define SPI_SLAVE_ISR_ATTR
#endif

#if (SOC_SPI_PERIPH_NUM == 2)
#define VALID_HOST(x) ((x) == SPI2_HOST)
#elif (SOC_SPI_PERIPH_NUM == 3)
#define VALID_HOST(x) ((x) >= SPI2_HOST && (x) <= SPI3_HOST)
#endif
#define SPIHD_CHECK(cond,warn,ret) do{if(!(cond)){ESP_LOGE(TAG, warn); return ret;}} while(0)

/// struct to hold private transaction data (like tx and rx buffer for DMA).
typedef struct {
    spi_slave_hd_data_t  *trans;    //original trans
    void *aligned_buffer;           //actually trans buffer (re-malloced if needed)
} spi_slave_hd_trans_priv_t;

typedef struct {
    spi_host_device_t host_id;
    int cs_io_num;
    spi_bus_attr_t* bus_attr;
    _Atomic spi_bus_fsm_t fsm;
    spi_dma_ctx_t   *dma_ctx;
    uint16_t internal_mem_align_size;
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
    SemaphoreHandle_t tx_cnting_sem;
    SemaphoreHandle_t rx_cnting_sem;

    spi_slave_hd_trans_priv_t tx_curr_trans;
    spi_slave_hd_trans_priv_t rx_curr_trans;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
} spi_slave_hd_slot_t;

static spi_slave_hd_slot_t *spihost[SOC_SPI_PERIPH_NUM];
static const char TAG[] = "slave_hd";

#if SOC_GDMA_SUPPORTED
// for which dma is provided by gdma driver
#define spi_dma_reset               gdma_reset
#define spi_dma_start(chan, addr)   gdma_start(chan, (intptr_t)(addr))
#define spi_dma_append              gdma_append

static bool s_spi_slave_hd_append_gdma_isr(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data);
#else
// by spi_dma, mainly esp32s2
static void s_spi_slave_hd_append_legacy_isr(void *arg);
#endif // SOC_GDMA_SUPPORTED

static void s_spi_slave_hd_segment_isr(void *arg);

#if SOC_SPI_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
static esp_err_t s_spi_create_sleep_retention_cb(void *arg)
{
    spi_slave_hd_slot_t *host = arg;
    return sleep_retention_entries_create(spi_reg_retention_info[host->host_id - 1].entry_array,
                                          spi_reg_retention_info[host->host_id - 1].array_size,
                                          REGDMA_LINK_PRI_GPSPI,
                                          spi_reg_retention_info[host->host_id - 1].module_id);
}
#endif  // SOC_SPI_SUPPORT_SLEEP_RETENTION

esp_err_t spi_slave_hd_init(spi_host_device_t host_id, const spi_bus_config_t *bus_config, const spi_slave_hd_slot_config_t *config)
{
    bool append_mode = (config->flags & SPI_SLAVE_HD_APPEND_MODE);
    esp_err_t ret = ESP_OK;

    SPIHD_CHECK(VALID_HOST(host_id), "invalid host", ESP_ERR_INVALID_ARG);
    //DMA is mandatory required for slave HD driver, check here for better experience to `transmit` api
#if CONFIG_IDF_TARGET_ESP32S2
    SPIHD_CHECK(config->dma_chan == (int)host_id || config->dma_chan == SPI_DMA_CH_AUTO, "dma is required or invalid channel", ESP_ERR_INVALID_ARG);
#elif SOC_GDMA_SUPPORTED
    SPIHD_CHECK(config->dma_chan == SPI_DMA_CH_AUTO, "dma is required or invalid channel, only support SPI_DMA_CH_AUTO", ESP_ERR_INVALID_ARG);
#endif
#ifndef CONFIG_SPI_SLAVE_ISR_IN_IRAM
    SPIHD_CHECK((bus_config->intr_flags & ESP_INTR_FLAG_IRAM) == 0, "ESP_INTR_FLAG_IRAM should be disabled when CONFIG_SPI_SLAVE_ISR_IN_IRAM is not set.", ESP_ERR_INVALID_ARG);
#endif

    SPIHD_CHECK(ESP_OK == spicommon_bus_alloc(host_id, "slave_hd"), "host already in use", ESP_ERR_INVALID_STATE);
    // spi_slave_hd_slot_t contains atomic variable, memory must be allocated from internal memory
    spi_slave_hd_slot_t *host = heap_caps_calloc(1, sizeof(spi_slave_hd_slot_t), MALLOC_CAP_INTERNAL);
    if (host == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }
    spihost[host_id] = host;
    host->host_id = host_id;
    host->int_spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    host->append_mode = append_mode;
    atomic_store(&host->fsm, SPI_BUS_FSM_ENABLED);
    host->bus_attr = (spi_bus_attr_t *)spi_bus_get_attr(host_id);
    host->cs_io_num = config->spics_io_num;

    ret = spicommon_dma_chan_alloc(host_id, config->dma_chan, &host->dma_ctx);
    if (ret != ESP_OK) {
        goto cleanup;
    }
#if SOC_GDMA_SUPPORTED
    gdma_strategy_config_t dma_strategy = {
        .auto_update_desc = true,
        .eof_till_data_popped = true,
    };
    gdma_apply_strategy(host->dma_ctx->tx_dma_chan, &dma_strategy);
#else
    spi_dma_ll_enable_out_auto_wrback(SPI_LL_GET_HW(host->dma_ctx->tx_dma_chan.host_id), host->dma_ctx->tx_dma_chan.chan_id, 1);
    spi_dma_ll_set_out_eof_generation(SPI_LL_GET_HW(host->dma_ctx->tx_dma_chan.host_id), host->dma_ctx->tx_dma_chan.chan_id, 1);
#endif
    ret = spicommon_dma_desc_alloc(host->dma_ctx, bus_config->max_transfer_sz, &host->max_transfer_sz);
    if (ret != ESP_OK) {
        goto cleanup;
    }

    host->hal.dma_desc_num = host->dma_ctx->dma_desc_num;
    host->hal.dmadesc_tx = heap_caps_malloc(sizeof(spi_slave_hd_hal_desc_append_t) * host->hal.dma_desc_num, MALLOC_CAP_DEFAULT);
    host->hal.dmadesc_rx = heap_caps_malloc(sizeof(spi_slave_hd_hal_desc_append_t) * host->hal.dma_desc_num, MALLOC_CAP_DEFAULT);
    if (!(host->hal.dmadesc_tx && host->hal.dmadesc_rx)) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }
    //Pair each desc to each possible trans
    for (int i = 0; i < host->hal.dma_desc_num; i ++) {
        host->hal.dmadesc_tx[i].desc = &host->dma_ctx->dmadesc_tx[i];
        host->hal.dmadesc_rx[i].desc = &host->dma_ctx->dmadesc_rx[i];
    }

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    size_t alignment;
    esp_cache_get_alignment(MALLOC_CAP_DMA, &alignment);
    host->internal_mem_align_size = alignment;
#else
    host->internal_mem_align_size = 4;
#endif

    ret = spicommon_bus_initialize_io(host_id, bus_config, SPICOMMON_BUSFLAG_SLAVE | bus_config->flags, NULL, NULL);
    if (ret != ESP_OK) {
        goto cleanup;
    }
    spicommon_cs_initialize(host_id, config->spics_io_num, 0, !(bus_config->flags & SPICOMMON_BUSFLAG_NATIVE_PINS), NULL);
    host->flags = host->bus_attr->flags; // This flag MUST be set after spicommon_bus_initialize_io is called

    spi_slave_hd_hal_config_t hal_config = {
        .host_id = host_id,
        .dma_enabled = true,
        .append_mode = append_mode,
        .mode = config->mode,
        .tx_lsbfirst = (config->flags & SPI_SLAVE_HD_RXBIT_LSBFIRST),
        .rx_lsbfirst = (config->flags & SPI_SLAVE_HD_TXBIT_LSBFIRST),
    };

    //Init the hal according to the hal_config set above
    spi_slave_hd_hal_init(&host->hal, &hal_config);

#ifdef CONFIG_PM_ENABLE
#if CONFIG_IDF_TARGET_ESP32P4
    // use CPU_MAX lock to ensure PSRAM bandwidth and usability during DFS
    ret = esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "spi_slave_hd", &host->pm_lock);
#else
    ret = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "spi_slave_hd", &host->pm_lock);
#endif
    if (ret != ESP_OK) {
        goto cleanup;
    }
    // Lock APB frequency while SPI slave driver is in use
    esp_pm_lock_acquire(host->pm_lock);
#endif //CONFIG_PM_ENABLE

#if SOC_SPI_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    sleep_retention_module_init_param_t init_param = {
        .cbs = {
            .create = {
                .handle = s_spi_create_sleep_retention_cb,
                .arg = host,
            },
        },
        .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_SYSTEM),
    };

    if (ESP_OK == sleep_retention_module_init(spi_reg_retention_info[host_id - 1].module_id, &init_param)) {
        if ((bus_config->flags & SPICOMMON_BUSFLAG_SLP_ALLOW_PD) && (sleep_retention_module_allocate(spi_reg_retention_info[host_id - 1].module_id) != ESP_OK)) {
            // even though the sleep retention create failed, SPI driver should still work, so just warning here
            ESP_LOGW(TAG, "Alloc sleep recover failed, spi may hold power on");
        }
    } else {
        // even the sleep retention init failed, SPI driver should still work, so just warning here
        ESP_LOGW(TAG, "Init sleep recover failed, spi may offline after sleep");
    }
#else
    if (bus_config->flags & SPICOMMON_BUSFLAG_SLP_ALLOW_PD) {
        ESP_LOGE(TAG, "power down peripheral in sleep is not enabled or not supported on your target");
    }
#endif  // SOC_SPI_SUPPORT_SLEEP_RETENTION

    //Create Queues and Semaphores
    host->tx_ret_queue = xQueueCreate(config->queue_size, sizeof(spi_slave_hd_trans_priv_t));
    host->rx_ret_queue = xQueueCreate(config->queue_size, sizeof(spi_slave_hd_trans_priv_t));
    if (!host->append_mode) {
        host->tx_trans_queue = xQueueCreate(config->queue_size, sizeof(spi_slave_hd_trans_priv_t));
        host->rx_trans_queue = xQueueCreate(config->queue_size, sizeof(spi_slave_hd_trans_priv_t));
        if (!host->tx_trans_queue || !host->rx_trans_queue) {
            ret = ESP_ERR_NO_MEM;
            goto cleanup;
        }
    } else {
        host->tx_cnting_sem = xSemaphoreCreateCounting(config->queue_size, config->queue_size);
        host->rx_cnting_sem = xSemaphoreCreateCounting(config->queue_size, config->queue_size);
        if (!host->tx_cnting_sem || !host->rx_cnting_sem) {
            ret = ESP_ERR_NO_MEM;
            goto cleanup;
        }
    }

    //Alloc intr
    if (!host->append_mode) {
        ret = esp_intr_alloc(spicommon_irqsource_for_host(host_id), bus_config->intr_flags, s_spi_slave_hd_segment_isr,
                             (void *)host, &host->intr);
        if (ret != ESP_OK) {
            goto cleanup;
        }
        ret = esp_intr_alloc(spicommon_irqdma_source_for_host(host_id), bus_config->intr_flags, s_spi_slave_hd_segment_isr,
                             (void *)host, &host->intr_dma);
        if (ret != ESP_OK) {
            goto cleanup;
        }
    } else {
        //Append mode
#if SOC_GDMA_SUPPORTED
        // config gmda event callback for gdma supported chip
        gdma_rx_event_callbacks_t txrx_cbs = {
            .on_recv_eof = s_spi_slave_hd_append_gdma_isr,
        };
        gdma_register_tx_event_callbacks(host->dma_ctx->tx_dma_chan, (gdma_tx_event_callbacks_t *)&txrx_cbs, host);
        gdma_register_rx_event_callbacks(host->dma_ctx->rx_dma_chan, &txrx_cbs, host);
#else
        //On ESP32S2, `cmd7` and `cmd8` are designed as all `spi_dma` events, so use `dma_src` only
        ret = esp_intr_alloc(spicommon_irqdma_source_for_host(host_id), bus_config->intr_flags, s_spi_slave_hd_append_legacy_isr,
                             (void *)host, &host->intr_dma);
        if (ret != ESP_OK) {
            goto cleanup;
        }
#endif  //#if SOC_GDMA_SUPPORTED
    }
    //Init callbacks
    memcpy((uint8_t *)&host->callback, (uint8_t *)&config->cb_config, sizeof(spi_slave_hd_callback_config_t));
    spi_event_t event = 0;
    if (host->callback.cb_buffer_tx != NULL) {
        event |= SPI_EV_BUF_TX;
    }
    if (host->callback.cb_buffer_rx != NULL) {
        event |= SPI_EV_BUF_RX;
    }
    if (host->callback.cb_cmd9 != NULL) {
        event |= SPI_EV_CMD9;
    }
    if (host->callback.cb_cmdA != NULL) {
        event |= SPI_EV_CMDA;
    }
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
    if (host == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (host->tx_trans_queue) {
        vQueueDelete(host->tx_trans_queue);
    }
    if (host->tx_ret_queue) {
        vQueueDelete(host->tx_ret_queue);
    }
    if (host->rx_trans_queue) {
        vQueueDelete(host->rx_trans_queue);
    }
    if (host->rx_ret_queue) {
        vQueueDelete(host->rx_ret_queue);
    }
    if (host->tx_cnting_sem) {
        vSemaphoreDelete(host->tx_cnting_sem);
    }
    if (host->rx_cnting_sem) {
        vSemaphoreDelete(host->rx_cnting_sem);
    }
    esp_intr_free(host->intr);
    esp_intr_free(host->intr_dma);

#if SOC_SPI_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    const periph_retention_module_t retention_id = spi_reg_retention_info[host_id - 1].module_id;
    if (sleep_retention_is_module_created(retention_id)) {
        assert(sleep_retention_is_module_inited(retention_id));
        sleep_retention_module_free(retention_id);
    }
    if (sleep_retention_is_module_inited(retention_id)) {
        sleep_retention_module_deinit(retention_id);
    }
#endif
#ifdef CONFIG_PM_ENABLE
    if (host->pm_lock) {
        esp_pm_lock_release(host->pm_lock);
        esp_pm_lock_delete(host->pm_lock);
    }
#endif

    spicommon_bus_free_io_cfg(host_id);
    spicommon_cs_free_io(host->cs_io_num, &host->bus_attr->gpio_reserve);
    spicommon_bus_free(host_id);
    free(host->dma_ctx->dmadesc_tx);
    free(host->dma_ctx->dmadesc_rx);
    free(host->hal.dmadesc_tx);
    free(host->hal.dmadesc_rx);
    spicommon_dma_chan_free(host->dma_ctx);

    free(host);
    spihost[host_id] = NULL;
    return ESP_OK;
}

esp_err_t spi_slave_hd_enable(spi_host_device_t host_id)
{
    SPIHD_CHECK(VALID_HOST(host_id), "invalid host", ESP_ERR_INVALID_ARG);
    SPIHD_CHECK(spihost[host_id], "host not slave or not initialized", ESP_ERR_INVALID_ARG);
    spi_bus_fsm_t curr_sta = SPI_BUS_FSM_DISABLED;
    SPIHD_CHECK(atomic_compare_exchange_strong(&spihost[host_id]->fsm, &curr_sta, SPI_BUS_FSM_ENABLED), "host already enabled", ESP_ERR_INVALID_STATE);

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(spihost[host_id]->pm_lock);
#endif //CONFIG_PM_ENABLE

// If going to TOP_PD power down, the bus_clock is required during reg_dma, and will be disabled by sleep flow then
#if !CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    SPI_COMMON_RCC_CLOCK_ATOMIC() {
        spi_ll_enable_bus_clock(host_id, true);
    }
#endif
    return ESP_OK;
}

esp_err_t spi_slave_hd_disable(spi_host_device_t host_id)
{
    SPIHD_CHECK(VALID_HOST(host_id), "invalid host", ESP_ERR_INVALID_ARG);
    SPIHD_CHECK(spihost[host_id], "host not slave or not initialized", ESP_ERR_INVALID_ARG);
    spi_bus_fsm_t curr_sta = SPI_BUS_FSM_ENABLED;
    SPIHD_CHECK(atomic_compare_exchange_strong(&spihost[host_id]->fsm, &curr_sta, SPI_BUS_FSM_DISABLED), "host already disabled", ESP_ERR_INVALID_STATE);

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(spihost[host_id]->pm_lock);
#endif //CONFIG_PM_ENABLE

// same as above
#if !CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    SPI_COMMON_RCC_CLOCK_ATOMIC() {
        spi_ll_enable_bus_clock(host_id, false);
    }
#endif
    return ESP_OK;
}

static void tx_invoke(spi_slave_hd_slot_t *host)
{
    portENTER_CRITICAL(&host->int_spinlock);
    spi_slave_hd_hal_invoke_event_intr(&host->hal, SPI_EV_SEND);
    portEXIT_CRITICAL(&host->int_spinlock);
}

static void rx_invoke(spi_slave_hd_slot_t *host)
{
    portENTER_CRITICAL(&host->int_spinlock);
    spi_slave_hd_hal_invoke_event_intr(&host->hal, SPI_EV_RECV);
    portEXIT_CRITICAL(&host->int_spinlock);
}

static inline SPI_SLAVE_ISR_ATTR BaseType_t intr_check_clear_callback(spi_slave_hd_slot_t *host, spi_event_t ev, slave_cb_t cb)
{
    BaseType_t cb_awoken = pdFALSE;
    if (spi_slave_hd_hal_check_clear_event(&host->hal, ev) && cb) {
        spi_slave_hd_event_t event = {.event = ev};
        cb(host->callback.arg, &event, &cb_awoken);
    }
    return cb_awoken;
}
static SPI_SLAVE_ISR_ATTR void s_spi_slave_hd_segment_isr(void *arg)
{
    spi_slave_hd_slot_t *host = (spi_slave_hd_slot_t *)arg;
    spi_slave_hd_callback_config_t *callback = &host->callback;
    spi_slave_hd_hal_context_t *hal = &host->hal;
    BaseType_t awoken = pdFALSE;
    BaseType_t ret;

    awoken |= intr_check_clear_callback(host, SPI_EV_BUF_TX, callback->cb_buffer_tx);
    awoken |= intr_check_clear_callback(host, SPI_EV_BUF_RX, callback->cb_buffer_rx);
    awoken |= intr_check_clear_callback(host, SPI_EV_CMD9,   callback->cb_cmd9);
    awoken |= intr_check_clear_callback(host, SPI_EV_CMDA,   callback->cb_cmdA);

    bool tx_done = false, rx_done = false;
    bool tx_event = false, rx_event = false;

    portENTER_CRITICAL_ISR(&host->int_spinlock);
    tx_event = spi_slave_hd_hal_check_disable_event(hal, SPI_EV_SEND);
    rx_event = spi_slave_hd_hal_check_disable_event(hal, SPI_EV_RECV);
    tx_done = host->tx_curr_trans.trans && tx_event;
    rx_done = host->rx_curr_trans.trans && rx_event;
    portEXIT_CRITICAL_ISR(&host->int_spinlock);

    if (tx_done) {
        bool ret_queue = true;
        if (callback->cb_sent) {
            spi_slave_hd_event_t ev = {
                .event = SPI_EV_SEND,
                .trans = host->tx_curr_trans.trans,
            };
            BaseType_t cb_awoken = pdFALSE;
            ret_queue = callback->cb_sent(callback->arg, &ev, &cb_awoken);
            awoken |= cb_awoken;
        }
        if (ret_queue) {
            ret = xQueueSendFromISR(host->tx_ret_queue, &host->tx_curr_trans, &awoken);
            // The return queue is full. All the data remain in send_queue + ret_queue should not be more than the queue length.
            assert(ret == pdTRUE);
        }
        host->tx_curr_trans.trans = NULL;
    }
    if (rx_done) {
        bool ret_queue = true;
        host->rx_curr_trans.trans->trans_len = spi_slave_hd_hal_rxdma_seg_get_len(hal);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE   //invalidate here to let user access rx data in post_cb if possible
        uint16_t alignment = host->internal_mem_align_size;
        uint32_t buff_len = (host->rx_curr_trans.trans->len + alignment - 1) & (~(alignment - 1));
        esp_err_t ret = esp_cache_msync((void *)host->rx_curr_trans.aligned_buffer, buff_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        assert(ret == ESP_OK);
        (void)ret;
#endif
        if (callback->cb_recv) {
            spi_slave_hd_event_t ev = {
                .event = SPI_EV_RECV,
                .trans = host->rx_curr_trans.trans,
            };
            BaseType_t cb_awoken = pdFALSE;
            ret_queue = callback->cb_recv(callback->arg, &ev, &cb_awoken);
            awoken |= cb_awoken;
        }
        if (ret_queue) {
            ret = xQueueSendFromISR(host->rx_ret_queue, &host->rx_curr_trans, &awoken);
            // The return queue is full. All the data remain in send_queue + ret_queue should not be more than the queue length.
            assert(ret == pdTRUE);
        }
        host->rx_curr_trans.trans = NULL;
    }

    bool tx_sent = false;
    bool rx_sent = false;
    if (!host->tx_curr_trans.trans) {
        ret = xQueueReceiveFromISR(host->tx_trans_queue, &host->tx_curr_trans, &awoken);
        if ((ret == pdTRUE) && host->tx_curr_trans.trans) {
            spicommon_dma_desc_setup_link(hal->dmadesc_tx->desc, host->tx_curr_trans.aligned_buffer, host->tx_curr_trans.trans->len, false);
            spi_dma_reset(host->dma_ctx->tx_dma_chan);
            spi_slave_hd_hal_txdma(hal);
            spi_dma_start(host->dma_ctx->tx_dma_chan, host->dma_ctx->dmadesc_tx);
            tx_sent = true;
            if (callback->cb_send_dma_ready) {
                spi_slave_hd_event_t ev = {
                    .event = SPI_EV_SEND_DMA_READY,
                    .trans = host->tx_curr_trans.trans,
                };
                BaseType_t cb_awoken = pdFALSE;
                callback->cb_send_dma_ready(callback->arg, &ev, &cb_awoken);
                awoken |= cb_awoken;
            }
        }
    }
    if (!host->rx_curr_trans.trans) {
        ret = xQueueReceiveFromISR(host->rx_trans_queue, &host->rx_curr_trans, &awoken);
        if ((ret == pdTRUE) && host->rx_curr_trans.trans) {
            spicommon_dma_desc_setup_link(hal->dmadesc_rx->desc, host->rx_curr_trans.aligned_buffer, host->rx_curr_trans.trans->len, true);
            spi_dma_reset(host->dma_ctx->rx_dma_chan);
            spi_slave_hd_hal_rxdma(hal);
            spi_dma_start(host->dma_ctx->rx_dma_chan, host->dma_ctx->dmadesc_rx);
            rx_sent = true;
            if (callback->cb_recv_dma_ready) {
                spi_slave_hd_event_t ev = {
                    .event = SPI_EV_RECV_DMA_READY,
                    .trans = host->rx_curr_trans.trans,
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

    if (awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static SPI_SLAVE_ISR_ATTR void spi_slave_hd_append_tx_isr(void *arg)
{
    spi_slave_hd_slot_t *host = (spi_slave_hd_slot_t*)arg;
    spi_slave_hd_callback_config_t *callback = &host->callback;
    spi_slave_hd_hal_context_t *hal = &host->hal;
    BaseType_t awoken = pdFALSE;
    BaseType_t ret __attribute__((unused));

    spi_slave_hd_trans_priv_t ret_priv_trans;
    while (1) {
        bool trans_finish = false;
        trans_finish = spi_slave_hd_hal_get_tx_finished_trans(hal, (void **)&ret_priv_trans.trans, &ret_priv_trans.aligned_buffer);
        if (!trans_finish) {
            break;
        }
        portENTER_CRITICAL_ISR(&host->int_spinlock);
        hal->tx_used_desc_cnt--;
        portEXIT_CRITICAL_ISR(&host->int_spinlock);

        bool ret_queue = true;
        if (callback->cb_sent) {
            spi_slave_hd_event_t ev = {
                .event = SPI_EV_SEND,
                .trans = ret_priv_trans.trans,
            };
            BaseType_t cb_awoken = pdFALSE;
            ret_queue = callback->cb_sent(callback->arg, &ev, &cb_awoken);
            awoken |= cb_awoken;
        }

        if (ret_queue) {
            ret = xQueueSendFromISR(host->tx_ret_queue, &ret_priv_trans, &awoken);
            assert(ret == pdTRUE);

            ret = xSemaphoreGiveFromISR(host->tx_cnting_sem, &awoken);
            assert(ret == pdTRUE);
        }
    }
    if (awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static SPI_SLAVE_ISR_ATTR void spi_slave_hd_append_rx_isr(void *arg)
{
    spi_slave_hd_slot_t *host = (spi_slave_hd_slot_t*)arg;
    spi_slave_hd_callback_config_t *callback = &host->callback;
    spi_slave_hd_hal_context_t *hal = &host->hal;
    BaseType_t awoken = pdFALSE;
    BaseType_t ret __attribute__((unused));

    spi_slave_hd_trans_priv_t ret_priv_trans;
    size_t trans_len;
    while (1) {
        bool trans_finish = false;
        trans_finish = spi_slave_hd_hal_get_rx_finished_trans(hal, (void **)&ret_priv_trans.trans, &ret_priv_trans.aligned_buffer, &trans_len);
        if (!trans_finish) {
            break;
        }
        portENTER_CRITICAL_ISR(&host->int_spinlock);
        hal->rx_used_desc_cnt--;
        portEXIT_CRITICAL_ISR(&host->int_spinlock);
        ret_priv_trans.trans->trans_len = trans_len;

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE   //invalidate here to let user access rx data in post_cb if possible
        uint16_t alignment = host->internal_mem_align_size;
        uint32_t buff_len = (ret_priv_trans.trans->len + alignment - 1) & (~(alignment - 1));
        esp_err_t ret = esp_cache_msync((void *)ret_priv_trans.aligned_buffer, buff_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        assert(ret == ESP_OK);
        (void)ret;
#endif
        bool ret_queue = true;
        if (callback->cb_recv) {
            spi_slave_hd_event_t ev = {
                .event = SPI_EV_RECV,
                .trans = ret_priv_trans.trans,
            };
            BaseType_t cb_awoken = pdFALSE;
            ret_queue = callback->cb_recv(callback->arg, &ev, &cb_awoken);
            awoken |= cb_awoken;
        }

        if (ret_queue) {
            ret = xQueueSendFromISR(host->rx_ret_queue, &ret_priv_trans, &awoken);
            assert(ret == pdTRUE);

            ret = xSemaphoreGiveFromISR(host->rx_cnting_sem, &awoken);
            assert(ret == pdTRUE);
        }
    }
    if (awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

#if SOC_GDMA_SUPPORTED
static SPI_SLAVE_ISR_ATTR bool s_spi_slave_hd_append_gdma_isr(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    assert(event_data);
    spi_slave_hd_slot_t *host = (spi_slave_hd_slot_t*)user_data;

    host->hal.current_eof_addr = event_data->tx_eof_desc_addr;
    if (host->dma_ctx->tx_dma_chan == dma_chan) {
        spi_slave_hd_append_tx_isr(user_data);
    } else {
        spi_slave_hd_append_rx_isr(user_data);
    }
    return true;
}

#else
static SPI_SLAVE_ISR_ATTR void s_spi_slave_hd_append_legacy_isr(void *arg)
{
    spi_slave_hd_slot_t *host = (spi_slave_hd_slot_t *)arg;
    spi_slave_hd_hal_context_t *hal = &host->hal;
    bool rx_done = false;
    bool tx_done = false;

    portENTER_CRITICAL_ISR(&host->int_spinlock);
    if (spi_slave_hd_hal_check_clear_event(hal, SPI_EV_RECV)) {
        hal->current_eof_addr = spi_dma_get_eof_desc(host->dma_ctx->rx_dma_chan);
        rx_done = true;
    }
    if (spi_slave_hd_hal_check_clear_event(hal, SPI_EV_SEND)) {
        hal->current_eof_addr = spi_dma_get_eof_desc(host->dma_ctx->tx_dma_chan);
        tx_done = true;
    }
    portEXIT_CRITICAL_ISR(&host->int_spinlock);

    if (rx_done) {
        spi_slave_hd_append_rx_isr(arg);
    }
    if (tx_done) {
        spi_slave_hd_append_tx_isr(arg);
    }
}
#endif // SOC_GDMA_SUPPORTED

static void s_spi_slave_hd_destroy_priv_trans(spi_host_device_t host, spi_slave_hd_trans_priv_t *priv_trans, spi_slave_chan_t chan)
{
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    spi_slave_hd_data_t *orig_trans = priv_trans->trans;
    if (priv_trans->aligned_buffer != orig_trans->data) {
        if (chan == SPI_SLAVE_CHAN_RX) {
            memcpy(orig_trans->data, priv_trans->aligned_buffer, orig_trans->trans_len);
        }
        free(priv_trans->aligned_buffer);
    }
#endif  //SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
}

static esp_err_t s_spi_slave_hd_setup_priv_trans(spi_host_device_t host, spi_slave_hd_trans_priv_t *priv_trans, spi_slave_chan_t chan)
{
    spi_slave_hd_data_t *orig_trans = priv_trans->trans;

    priv_trans->aligned_buffer = orig_trans->data;

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    uint16_t alignment = spihost[host]->internal_mem_align_size;
    uint32_t byte_len = orig_trans->len;

    if (((uint32_t)orig_trans->data | byte_len) & (alignment - 1)) {
        ESP_RETURN_ON_FALSE(orig_trans->flags & SPI_SLAVE_HD_TRANS_DMA_BUFFER_ALIGN_AUTO, ESP_ERR_INVALID_ARG, TAG, "data buffer addr&len not align to %d byte, or not dma_capable", alignment);
        byte_len = (byte_len + alignment - 1) & (~(alignment - 1));  // up align to alignment
        ESP_LOGD(TAG, "Re-allocate %s buffer of len %" PRIu32 " for DMA", (chan == SPI_SLAVE_CHAN_TX) ? "TX" : "RX", byte_len);
        priv_trans->aligned_buffer = heap_caps_aligned_alloc(64, byte_len, MALLOC_CAP_DMA);
        if (priv_trans->aligned_buffer == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }
    if (chan == SPI_SLAVE_CHAN_TX) {
        ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE("-Wanalyzer-overlapping-buffers") // TODO IDF-11086
        memcpy(priv_trans->aligned_buffer, orig_trans->data, orig_trans->len);
        ESP_COMPILER_DIAGNOSTIC_POP("-Wanalyzer-overlapping-buffers")
        esp_err_t ret = esp_cache_msync((void *)priv_trans->aligned_buffer, byte_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        ESP_RETURN_ON_FALSE(ESP_OK == ret, ESP_ERR_INVALID_STATE, TAG, "mem sync c2m(writeback) fail");
    } else {
        esp_err_t ret = esp_cache_msync((void *)priv_trans->aligned_buffer, byte_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        ESP_RETURN_ON_FALSE(ESP_OK == ret, ESP_ERR_INVALID_STATE, TAG, "mem sync m2c(invalid) fail");
    }
#endif  //SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    return ESP_OK;
}

static esp_err_t get_ret_queue_result(spi_host_device_t host_id, spi_slave_chan_t chan, spi_slave_hd_data_t **out_trans, TickType_t timeout)
{
    spi_slave_hd_slot_t *host = spihost[host_id];
    spi_slave_hd_trans_priv_t hd_priv_trans;
    BaseType_t ret;

    if (chan == SPI_SLAVE_CHAN_TX) {
        ret = xQueueReceive(host->tx_ret_queue, &hd_priv_trans, timeout);
    } else {
        ret = xQueueReceive(host->rx_ret_queue, &hd_priv_trans, timeout);
    }
    if (ret == pdFALSE) {
        return ESP_ERR_TIMEOUT;
    }

    s_spi_slave_hd_destroy_priv_trans(host_id, &hd_priv_trans, chan);
    *out_trans = hd_priv_trans.trans;
    return ESP_OK;
}

esp_err_t s_spi_slave_hd_append_txdma(spi_slave_hd_slot_t *host, uint8_t *data, size_t len, void *arg)
{
    spi_slave_hd_hal_context_t *hal = &host->hal;

    //Check if there are enough available DMA descriptors for software to use
    int num_required = (len + DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED - 1) / DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED;
    int available_desc_num = hal->dma_desc_num - hal->tx_used_desc_cnt;
    if (num_required > available_desc_num) {
        return ESP_ERR_INVALID_STATE;
    }

    spicommon_dma_desc_setup_link(hal->tx_cur_desc->desc, data, len, false);
    hal->tx_cur_desc->arg = arg;

    if (!hal->tx_used_desc_cnt) {
        //start a link
        hal->tx_dma_tail = hal->tx_cur_desc;
        spi_dma_reset(host->dma_ctx->tx_dma_chan);
        spi_slave_hd_hal_hw_prepare_tx(hal);
        spi_dma_start(host->dma_ctx->tx_dma_chan, hal->tx_cur_desc->desc);
    } else {
        //there is already a consecutive link
        ADDR_DMA_2_CPU(hal->tx_dma_tail->desc)->next = hal->tx_cur_desc->desc;
        hal->tx_dma_tail = hal->tx_cur_desc;
        spi_dma_append(host->dma_ctx->tx_dma_chan);
    }

    //Move the current descriptor pointer according to the number of the linked descriptors
    portENTER_CRITICAL(&host->int_spinlock);
    hal->tx_used_desc_cnt += num_required;
    portEXIT_CRITICAL(&host->int_spinlock);
    for (int i = 0; i < num_required; i++) {
        hal->tx_cur_desc++;
        if (hal->tx_cur_desc == hal->dmadesc_tx + hal->dma_desc_num) {
            hal->tx_cur_desc = hal->dmadesc_tx;
        }
    }

    return ESP_OK;
}

esp_err_t s_spi_slave_hd_append_rxdma(spi_slave_hd_slot_t *host, uint8_t *data, size_t len, void *arg)
{
    spi_slave_hd_hal_context_t *hal = &host->hal;

    //Check if there are enough available dma descriptors for software to use
    int num_required = (len + DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED - 1) / DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED;
    int available_desc_num = hal->dma_desc_num - hal->rx_used_desc_cnt;
    if (num_required > available_desc_num) {
        return ESP_ERR_INVALID_STATE;
    }

    spicommon_dma_desc_setup_link(hal->rx_cur_desc->desc, data, len, false);
    hal->rx_cur_desc->arg = arg;

    if (!hal->rx_used_desc_cnt) {
        //start a link
        hal->rx_dma_tail = hal->rx_cur_desc;
        spi_dma_reset(host->dma_ctx->rx_dma_chan);
        spi_slave_hd_hal_hw_prepare_rx(hal);
        spi_dma_start(host->dma_ctx->rx_dma_chan, hal->rx_cur_desc->desc);
    } else {
        //there is already a consecutive link
        ADDR_DMA_2_CPU(hal->rx_dma_tail->desc)->next = hal->rx_cur_desc->desc;
        hal->rx_dma_tail = hal->rx_cur_desc;
        spi_dma_append(host->dma_ctx->rx_dma_chan);
    }

    //Move the current descriptor pointer according to the number of the linked descriptors
    portENTER_CRITICAL(&host->int_spinlock);
    hal->rx_used_desc_cnt += num_required;
    portEXIT_CRITICAL(&host->int_spinlock);
    for (int i = 0; i < num_required; i++) {
        hal->rx_cur_desc++;
        if (hal->rx_cur_desc == hal->dmadesc_rx + hal->dma_desc_num) {
            hal->rx_cur_desc = hal->dmadesc_rx;
        }
    }

    return ESP_OK;
}

//---------------------------------------------------------Segment Mode Transaction APIs-----------------------------------------------------------//
esp_err_t spi_slave_hd_queue_trans(spi_host_device_t host_id, spi_slave_chan_t chan, spi_slave_hd_data_t *trans, uint32_t timeout)
{
    spi_slave_hd_slot_t *host = spihost[host_id];

    SPIHD_CHECK(host->append_mode == 0, "This API should be used for SPI Slave HD Segment Mode", ESP_ERR_INVALID_STATE);
    SPIHD_CHECK(esp_ptr_dma_capable(trans->data), "The buffer should be DMA capable.", ESP_ERR_INVALID_ARG);
    SPIHD_CHECK(trans->len <= host->max_transfer_sz && trans->len > 0, "Invalid buffer size", ESP_ERR_INVALID_ARG);
    SPIHD_CHECK(chan == SPI_SLAVE_CHAN_TX || chan == SPI_SLAVE_CHAN_RX, "Invalid channel", ESP_ERR_INVALID_ARG);

    spi_slave_hd_trans_priv_t hd_priv_trans = {.trans = trans};
    SPIHD_CHECK(ESP_OK == s_spi_slave_hd_setup_priv_trans(host_id, &hd_priv_trans, chan), "No mem to allocate new cache buffer", ESP_ERR_NO_MEM);

    if (chan == SPI_SLAVE_CHAN_TX) {
        BaseType_t ret = xQueueSend(host->tx_trans_queue, &hd_priv_trans, timeout);
        if (ret == pdFALSE) {
            return ESP_ERR_TIMEOUT;
        }
        tx_invoke(host);
    } else { //chan == SPI_SLAVE_CHAN_RX
        BaseType_t ret = xQueueSend(host->rx_trans_queue, &hd_priv_trans, timeout);
        if (ret == pdFALSE) {
            return ESP_ERR_TIMEOUT;
        }
        rx_invoke(host);
    }
    return ESP_OK;
}

esp_err_t spi_slave_hd_get_trans_res(spi_host_device_t host_id, spi_slave_chan_t chan, spi_slave_hd_data_t **out_trans, uint32_t timeout)
{
    esp_err_t ret;
    spi_slave_hd_slot_t *host = spihost[host_id];

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

//---------------------------------------------------------Append Mode Transaction APIs-----------------------------------------------------------//
esp_err_t spi_slave_hd_append_trans(spi_host_device_t host_id, spi_slave_chan_t chan, spi_slave_hd_data_t *trans, uint32_t timeout)
{
    esp_err_t err;
    spi_slave_hd_slot_t *host = spihost[host_id];

    SPIHD_CHECK(trans->len <= SPI_MAX_DMA_LEN, "Currently we only support transaction with data length within 4092 bytes", ESP_ERR_INVALID_ARG);
    SPIHD_CHECK(host->append_mode == 1, "This API should be used for SPI Slave HD Append Mode", ESP_ERR_INVALID_STATE);
    SPIHD_CHECK(esp_ptr_dma_capable(trans->data), "The buffer should be DMA capable.", ESP_ERR_INVALID_ARG);
    SPIHD_CHECK(trans->len <= host->max_transfer_sz && trans->len > 0, "Invalid buffer size", ESP_ERR_INVALID_ARG);
    SPIHD_CHECK(chan == SPI_SLAVE_CHAN_TX || chan == SPI_SLAVE_CHAN_RX, "Invalid channel", ESP_ERR_INVALID_ARG);

    spi_slave_hd_trans_priv_t hd_priv_trans = {.trans = trans};
    SPIHD_CHECK(ESP_OK == s_spi_slave_hd_setup_priv_trans(host_id, &hd_priv_trans, chan), "No mem to allocate new cache buffer", ESP_ERR_NO_MEM);

    if (chan == SPI_SLAVE_CHAN_TX) {
        BaseType_t ret = xSemaphoreTake(host->tx_cnting_sem, timeout);
        if (ret == pdFALSE) {
            return ESP_ERR_TIMEOUT;
        }
        err = s_spi_slave_hd_append_txdma(host, hd_priv_trans.aligned_buffer, trans->len, trans);
    } else {
        BaseType_t ret = xSemaphoreTake(host->rx_cnting_sem, timeout);
        if (ret == pdFALSE) {
            return ESP_ERR_TIMEOUT;
        }
        err = s_spi_slave_hd_append_rxdma(host, hd_priv_trans.aligned_buffer, trans->len, trans);
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Wait until the DMA finishes its transaction");
    }

    return err;
}

esp_err_t spi_slave_hd_get_append_trans_res(spi_host_device_t host_id, spi_slave_chan_t chan, spi_slave_hd_data_t **out_trans, uint32_t timeout)
{
    esp_err_t ret;
    spi_slave_hd_slot_t *host = spihost[host_id];

    SPIHD_CHECK(host->append_mode == 1, "This API should be used for SPI Slave HD Append Mode", ESP_ERR_INVALID_STATE);
    SPIHD_CHECK(chan == SPI_SLAVE_CHAN_TX || chan == SPI_SLAVE_CHAN_RX, "Invalid channel", ESP_ERR_INVALID_ARG);
    ret = get_ret_queue_result(host_id, chan, out_trans, timeout);

    return ret;
}
