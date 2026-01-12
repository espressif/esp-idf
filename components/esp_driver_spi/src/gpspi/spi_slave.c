/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdatomic.h>
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_pm.h"
#include "esp_cache.h"
#include "esp_heap_caps.h"
#include "esp_rom_sys.h"
#include "soc/lldesc.h"
#include "soc/soc_caps.h"
#include "soc/spi_periph.h"
#include "soc/soc_memory_layout.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "driver/gpio.h"
#include "driver/spi_slave.h"
#include "hal/gpio_hal.h"
#include "hal/spi_slave_hal.h"
#include "esp_private/sleep_retention.h"
#include "esp_private/spi_slave_internal.h"
#include "esp_private/spi_common_internal.h"
#include "esp_private/esp_cache_private.h"
#include "esp_private/spi_share_hw_ctrl.h"

static const char *SPI_TAG = "spi_slave";

#define SPI_CHECK(a, str, ret_val)  ESP_RETURN_ON_FALSE(a, ret_val, SPI_TAG, str)

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

/// struct to hold private transaction data (like tx and rx buffer for DMA).
typedef struct {
    spi_slave_transaction_t *trans;     //original trans
    void *tx_buffer;        //actually tx buffer (re-malloced if needed)
    void *rx_buffer;        //actually rx buffer (re-malloced if needed)
} spi_slave_trans_priv_t;

typedef struct {
    int id;
    _Atomic spi_bus_fsm_t fsm;
    spi_bus_attr_t* bus_attr;
    spi_dma_ctx_t   *dma_ctx;
    spi_slave_interface_config_t cfg;
    intr_handle_t intr;
    spi_slave_hal_context_t hal;
    spi_slave_trans_priv_t cur_trans;
    uint32_t flags;
    uint32_t intr_flags;
    int max_transfer_sz;
    QueueHandle_t trans_queue;
    QueueHandle_t ret_queue;
    bool dma_enabled;
    bool cs_iomux;
    uint8_t cs_in_signal;
    uint16_t internal_mem_align_size;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
} spi_slave_t;

static spi_slave_t *spihost[SOC_SPI_PERIPH_NUM];

static void spi_intr(void *arg);

__attribute__((always_inline))
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
    return host->flags & SPICOMMON_BUSFLAG_IOMUX_PINS;
}

static inline void SPI_SLAVE_ISR_ATTR freeze_cs(spi_slave_t *host)
{
#if SPI_LL_SLAVE_NEEDS_CS_WORKAROUND
    // This workaround only for ESP32 due to old hardware design, see MR !3207
    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, host->cs_in_signal, false);
#endif
}

// Use this function instead of cs_initial to avoid overwrite the output config
// This is used in test by internal gpio matrix connections
static inline void SPI_SLAVE_ISR_ATTR restore_cs(spi_slave_t *host)
{
#if SPI_LL_SLAVE_NEEDS_CS_WORKAROUND
    // This workaround only for ESP32 due to old hardware design, see MR !3207
    if (host->cs_iomux) {
        gpio_ll_set_input_signal_from(GPIO_HAL_GET_HW(GPIO_PORT_0), host->cs_in_signal, false);
    } else {
        esp_rom_gpio_connect_in_signal(host->cfg.spics_io_num, host->cs_in_signal, false);
    }
#endif
}

#if (SOC_CPU_CORES_NUM > 1) && (!CONFIG_FREERTOS_UNICORE)
typedef struct {
    spi_slave_t *host;
    esp_err_t *err;
} spi_ipc_param_t;

static void ipc_isr_reg_to_core(void *args)
{
    spi_slave_t *host = ((spi_ipc_param_t *)args)->host;
    *((spi_ipc_param_t *)args)->err = esp_intr_alloc(spicommon_irqsource_for_host(host->id), host->intr_flags | ESP_INTR_FLAG_INTRDISABLED, spi_intr, (void *)host, &host->intr);
}
#endif

#if SOC_SPI_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
static esp_err_t s_spi_create_sleep_retention_cb(void *arg)
{
    spi_slave_t *context = arg;
    return sleep_retention_entries_create(spi_reg_retention_info[context->id - 1].entry_array,
                                          spi_reg_retention_info[context->id - 1].array_size,
                                          REGDMA_LINK_PRI_GPSPI,
                                          spi_reg_retention_info[context->id - 1].module_id);
}
#endif  // SOC_SPI_SUPPORT_SLEEP_RETENTION

esp_err_t spi_slave_initialize(spi_host_device_t host, const spi_bus_config_t *bus_config, const spi_slave_interface_config_t *slave_config, spi_dma_chan_t dma_chan)
{
    esp_err_t ret = ESP_OK;
    esp_err_t err;
    SPI_CHECK(is_valid_host(host), "invalid host", ESP_ERR_INVALID_ARG);
#ifdef CONFIG_IDF_TARGET_ESP32
    SPI_CHECK(dma_chan >= SPI_DMA_DISABLED && dma_chan <= SPI_DMA_CH_AUTO, "invalid dma channel", ESP_ERR_INVALID_ARG);
#elif CONFIG_IDF_TARGET_ESP32S2
    SPI_CHECK(dma_chan == SPI_DMA_DISABLED || dma_chan == (int)host || dma_chan == SPI_DMA_CH_AUTO, "invalid dma channel", ESP_ERR_INVALID_ARG);
#elif SOC_GDMA_SUPPORTED
    SPI_CHECK(dma_chan == SPI_DMA_DISABLED || dma_chan == SPI_DMA_CH_AUTO, "invalid dma channel, chip only support spi dma channel auto-alloc", ESP_ERR_INVALID_ARG);
#endif
    SPI_CHECK((bus_config->intr_flags & (ESP_INTR_FLAG_HIGH | ESP_INTR_FLAG_EDGE | ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_INTRDISABLED)) == 0, "intr flag not allowed", ESP_ERR_INVALID_ARG);
#ifndef CONFIG_SPI_SLAVE_ISR_IN_IRAM
    SPI_CHECK((bus_config->intr_flags & ESP_INTR_FLAG_IRAM) == 0, "ESP_INTR_FLAG_IRAM should be disabled when CONFIG_SPI_SLAVE_ISR_IN_IRAM is not set.", ESP_ERR_INVALID_ARG);
#endif
    SPI_CHECK(slave_config->spics_io_num < 0 || GPIO_IS_VALID_GPIO(slave_config->spics_io_num), "spics pin invalid", ESP_ERR_INVALID_ARG);

    //Check post_trans_cb status when `SPI_SLAVE_NO_RETURN_RESULT` flag is set.
    if (slave_config->flags & SPI_SLAVE_NO_RETURN_RESULT) {
        SPI_CHECK(slave_config->post_trans_cb != NULL, "use feature flag 'SPI_SLAVE_NO_RETURN_RESULT' but no post_trans_cb function sets", ESP_ERR_INVALID_ARG);
    }

    SPI_CHECK(ESP_OK == spicommon_bus_alloc(host, "spi slave"), "host already in use", ESP_ERR_INVALID_STATE);
    // spi_slave_t contains atomic variable, memory must be allocated from internal memory
    spihost[host] = heap_caps_calloc(1, sizeof(spi_slave_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (spihost[host] == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }
    memcpy(&spihost[host]->cfg, slave_config, sizeof(spi_slave_interface_config_t));
    spihost[host]->id = host;
    spihost[host]->bus_attr = (spi_bus_attr_t *)spi_bus_get_attr(host);
    atomic_store(&spihost[host]->fsm, SPI_BUS_FSM_ENABLED);
    spi_slave_hal_context_t *hal = &spihost[host]->hal;

    spihost[host]->dma_enabled = (dma_chan != SPI_DMA_DISABLED);
    if (spihost[host]->dma_enabled) {
        ret = spicommon_dma_chan_alloc(host, dma_chan, &spihost[host]->dma_ctx);
        if (ret != ESP_OK) {
            goto cleanup;
        }
        ret = spicommon_dma_desc_alloc(spihost[host]->dma_ctx, bus_config->max_transfer_sz, &spihost[host]->max_transfer_sz);
        if (ret != ESP_OK) {
            goto cleanup;
        }

        hal->dmadesc_tx = spihost[host]->dma_ctx->dmadesc_tx;
        hal->dmadesc_rx = spihost[host]->dma_ctx->dmadesc_rx;
        hal->dmadesc_n = spihost[host]->dma_ctx->dma_desc_num;

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        size_t alignment;
        esp_cache_get_alignment(MALLOC_CAP_DMA, &alignment);
        spihost[host]->internal_mem_align_size = alignment;
#else
        spihost[host]->internal_mem_align_size = 4;
#endif
    } else {
        //We're limited to non-DMA transfers: the SPI work registers can hold 64 bytes at most.
        spihost[host]->max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE;
    }

    err = spicommon_bus_initialize_io(host, bus_config, SPICOMMON_BUSFLAG_SLAVE | bus_config->flags, NULL, NULL);
    if (err != ESP_OK) {
        ret = err;
        goto cleanup;
    }
    if (slave_config->spics_io_num >= 0) {
        spicommon_cs_initialize(host, slave_config->spics_io_num, 0, !bus_is_iomux(spihost[host]), NULL);
        // check and save where cs line really route through
        spihost[host]->cs_iomux = (slave_config->spics_io_num == spi_periph_signal[host].spics0_iomux_pin) && bus_is_iomux(spihost[host]);
        spihost[host]->cs_in_signal = spi_periph_signal[host].spics_in;
    }
    spihost[host]->flags = spihost[host]->bus_attr->flags; // This flag MUST be set after spicommon_bus_initialize_io is called

    // The slave DMA suffers from unexpected transactions. Forbid reading if DMA is enabled by disabling the CS line.
    if (spihost[host]->dma_enabled) {
        freeze_cs(spihost[host]);
    }

#ifdef CONFIG_PM_ENABLE
#if CONFIG_IDF_TARGET_ESP32P4
    // use CPU_MAX lock to ensure PSRAM bandwidth and usability during DFS
    err = esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "spi_slave", &spihost[host]->pm_lock);
#else
    err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "spi_slave", &spihost[host]->pm_lock);
#endif
    if (err != ESP_OK) {
        ret = err;
        goto cleanup;
    }
    // Lock APB frequency while SPI slave driver is in use
    esp_pm_lock_acquire(spihost[host]->pm_lock);
#endif //CONFIG_PM_ENABLE

#if SOC_SPI_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    sleep_retention_module_init_param_t init_param = {
        .cbs = {
            .create = {
                .handle = s_spi_create_sleep_retention_cb,
                .arg = spihost[host],
            },
        },
        .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_SYSTEM),
    };

    if (ESP_OK == sleep_retention_module_init(spi_reg_retention_info[host - 1].module_id, &init_param)) {
        if ((bus_config->flags & SPICOMMON_BUSFLAG_SLP_ALLOW_PD) && (sleep_retention_module_allocate(spi_reg_retention_info[host - 1].module_id) != ESP_OK)) {
            // even though the sleep retention create failed, SPI driver should still work, so just warning here
            ESP_LOGW(SPI_TAG, "Alloc sleep recover failed, spi may hold power on");
        }
    } else {
        // even the sleep retention init failed, SPI driver should still work, so just warning here
        ESP_LOGW(SPI_TAG, "Init sleep recover failed, spi may offline after sleep");
    }
#else
    if (bus_config->flags & SPICOMMON_BUSFLAG_SLP_ALLOW_PD) {
        ESP_LOGE(SPI_TAG, "power down peripheral in sleep is not enabled or not supported on your target");
    }
#endif  // SOC_SPI_SUPPORT_SLEEP_RETENTION

    //Create queues
    spihost[host]->trans_queue = xQueueCreate(slave_config->queue_size, sizeof(spi_slave_trans_priv_t));
    if (!spihost[host]->trans_queue) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }
    if (!(slave_config->flags & SPI_SLAVE_NO_RETURN_RESULT)) {
        spihost[host]->ret_queue = xQueueCreate(slave_config->queue_size, sizeof(spi_slave_trans_priv_t));
        if (!spihost[host]->ret_queue) {
            ret = ESP_ERR_NO_MEM;
            goto cleanup;
        }
    }

#if (SOC_CPU_CORES_NUM > 1) && (!CONFIG_FREERTOS_UNICORE)
    if (bus_config->isr_cpu_id > ESP_INTR_CPU_AFFINITY_AUTO) {
        spihost[host]->intr_flags = bus_config->intr_flags;
        SPI_CHECK(bus_config->isr_cpu_id <= ESP_INTR_CPU_AFFINITY_1, "invalid core id", ESP_ERR_INVALID_ARG);
        spi_ipc_param_t ipc_args = {
            .host = spihost[host],
            .err = &err,
        };
        esp_ipc_call_blocking(ESP_INTR_CPU_AFFINITY_TO_CORE_ID(bus_config->isr_cpu_id), ipc_isr_reg_to_core, (void *)&ipc_args);
    } else
#endif
    {
        err = esp_intr_alloc(spicommon_irqsource_for_host(host), bus_config->intr_flags | ESP_INTR_FLAG_INTRDISABLED, spi_intr, (void *)spihost[host], &spihost[host]->intr);
    }
    if (err != ESP_OK) {
        ret = err;
        goto cleanup;
    }

    //assign the SPI, RX DMA and TX DMA peripheral registers beginning address
    spi_slave_hal_config_t hal_config = {
        .host_id = host,
    };
    spi_slave_hal_init(hal, &hal_config);

    hal->rx_lsbfirst = (slave_config->flags & SPI_SLAVE_RXBIT_LSBFIRST) ? 1 : 0;
    hal->tx_lsbfirst = (slave_config->flags & SPI_SLAVE_TXBIT_LSBFIRST) ? 1 : 0;
    hal->mode = slave_config->mode;
    hal->use_dma = spihost[host]->dma_enabled;
    spi_slave_hal_setup_device(hal);
    return ESP_OK;

cleanup:
    spi_slave_free(host);
    return ret;
}

esp_err_t spi_slave_free(spi_host_device_t host)
{
    SPI_CHECK(is_valid_host(host), "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host], "host not slave", ESP_ERR_INVALID_ARG);
    if (spihost[host]->trans_queue) {
        vQueueDelete(spihost[host]->trans_queue);
    }
    if (spihost[host]->ret_queue) {
        vQueueDelete(spihost[host]->ret_queue);
    }
    if (spihost[host]->dma_enabled) {
        free(spihost[host]->dma_ctx->dmadesc_tx);
        free(spihost[host]->dma_ctx->dmadesc_rx);
        spicommon_dma_chan_free(spihost[host]->dma_ctx);
    }
    spicommon_bus_free_io_cfg(host);
    if (spihost[host]->cfg.spics_io_num >= 0) {
        spicommon_cs_free_io(spihost[host]->cfg.spics_io_num, &spihost[host]->bus_attr->gpio_reserve);
    }
    esp_intr_free(spihost[host]->intr);

#if SOC_SPI_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    const periph_retention_module_t retention_id = spi_reg_retention_info[spihost[host]->id - 1].module_id;
    if (sleep_retention_is_module_created(retention_id)) {
        assert(sleep_retention_is_module_inited(retention_id));
        sleep_retention_module_free(retention_id);
    }
    if (sleep_retention_is_module_inited(retention_id)) {
        sleep_retention_module_deinit(retention_id);
    }
#endif
#ifdef CONFIG_PM_ENABLE
    if (spihost[host]->pm_lock) {
        esp_pm_lock_release(spihost[host]->pm_lock);
        esp_pm_lock_delete(spihost[host]->pm_lock);
    }
#endif //CONFIG_PM_ENABLE
    free(spihost[host]);
    spihost[host] = NULL;
    spicommon_bus_free(host);
    return ESP_OK;
}

esp_err_t spi_slave_enable(spi_host_device_t host)
{
    SPI_CHECK(is_valid_host(host), "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host], "host not slave or not initialized", ESP_ERR_INVALID_ARG);
    spi_bus_fsm_t curr_sta = SPI_BUS_FSM_DISABLED;
    SPI_CHECK(atomic_compare_exchange_strong(&spihost[host]->fsm, &curr_sta, SPI_BUS_FSM_ENABLED), "host already enabled", ESP_ERR_INVALID_STATE);

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(spihost[host]->pm_lock);
#endif //CONFIG_PM_ENABLE

// If going to TOP_PD power down, the bus_clock is required during reg_dma, and will be disabled by sleep flow then
#if !CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    SPI_COMMON_RCC_CLOCK_ATOMIC() {
        spi_ll_enable_bus_clock(host, true);
    }
#endif
    return ESP_OK;
}

esp_err_t spi_slave_disable(spi_host_device_t host)
{
    SPI_CHECK(is_valid_host(host), "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host], "host not slave or not initialized", ESP_ERR_INVALID_ARG);
    spi_bus_fsm_t curr_sta = SPI_BUS_FSM_ENABLED;
    SPI_CHECK(atomic_compare_exchange_strong(&spihost[host]->fsm, &curr_sta, SPI_BUS_FSM_DISABLED), "host already disabled", ESP_ERR_INVALID_STATE);

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(spihost[host]->pm_lock);
#endif //CONFIG_PM_ENABLE

// same as above
#if !CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    SPI_COMMON_RCC_CLOCK_ATOMIC() {
        spi_ll_enable_bus_clock(host, false);
    }
#endif
    return ESP_OK;
}

static void SPI_SLAVE_ISR_ATTR spi_slave_uninstall_priv_trans(spi_host_device_t host, spi_slave_trans_priv_t *priv_trans)
{
    __attribute__((unused)) spi_slave_transaction_t *trans = (spi_slave_transaction_t *)priv_trans->trans;
#if CONFIG_IDF_TARGET_ESP32
    if (spihost[host]->dma_enabled && (trans->trans_len % 32)) {
        ESP_EARLY_LOGW(SPI_TAG, "Use DMA but real trans_len is not 4 bytes aligned, slave may loss data");
    }
#endif
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    if (spihost[host]->dma_enabled) {
        if (trans->tx_buffer && (trans->tx_buffer != priv_trans->tx_buffer)) {
            free(priv_trans->tx_buffer);
        }
        if (trans->rx_buffer && (trans->rx_buffer != priv_trans->rx_buffer)) {
            memcpy(trans->rx_buffer, priv_trans->rx_buffer, (trans->length + 7) / 8);
            free(priv_trans->rx_buffer);
        }
    }
#endif  //SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
}

static esp_err_t SPI_SLAVE_ISR_ATTR spi_slave_setup_priv_trans(spi_host_device_t host, spi_slave_trans_priv_t *priv_trans)
{
    spi_slave_transaction_t *trans = (spi_slave_transaction_t *)priv_trans->trans;

    priv_trans->tx_buffer = (void *)trans->tx_buffer;
    priv_trans->rx_buffer = trans->rx_buffer;

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    uint16_t alignment = spihost[host]->internal_mem_align_size;
    uint32_t buffer_byte_len = (trans->length + 7) / 8;

    if (spihost[host]->dma_enabled && trans->tx_buffer) {
        if ((!esp_ptr_dma_capable(trans->tx_buffer) || ((((uint32_t)trans->tx_buffer) | buffer_byte_len) & (alignment - 1)))) {
            ESP_RETURN_ON_FALSE_ISR(trans->flags & SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO, ESP_ERR_INVALID_ARG, SPI_TAG, "TX buffer addr&len not align to %d byte, or not dma_capable", alignment);
            //if txbuf in the desc not DMA-capable, or not align to "alignment", malloc a new one
            ESP_EARLY_LOGD(SPI_TAG, "Allocate TX buffer for DMA");
            buffer_byte_len = (buffer_byte_len + alignment - 1) & (~(alignment - 1));   // up align to "alignment"
            uint32_t *temp = heap_caps_aligned_alloc(alignment, buffer_byte_len, MALLOC_CAP_DMA);
            if (temp == NULL) {
                return ESP_ERR_NO_MEM;
            }

            memcpy(temp, trans->tx_buffer, (trans->length + 7) / 8);
            priv_trans->tx_buffer = temp;
        }
        esp_err_t ret = esp_cache_msync((void *)priv_trans->tx_buffer, buffer_byte_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        ESP_RETURN_ON_FALSE_ISR(ESP_OK == ret, ESP_ERR_INVALID_STATE, SPI_TAG, "mem sync c2m(writeback) fail");
    }
    if (spihost[host]->dma_enabled && trans->rx_buffer) {
        if ((!esp_ptr_dma_capable(trans->rx_buffer) || ((((uint32_t)trans->rx_buffer) | (trans->length + 7) / 8) & (alignment - 1)))) {
            ESP_RETURN_ON_FALSE_ISR(trans->flags & SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO, ESP_ERR_INVALID_ARG, SPI_TAG, "RX buffer addr&len not align to %d byte, or not dma_capable", alignment);
            //if rxbuf in the desc not DMA-capable, or not align to "alignment", malloc a new one
            ESP_EARLY_LOGD(SPI_TAG, "Allocate RX buffer for DMA");
            buffer_byte_len = (buffer_byte_len + alignment - 1) & (~(alignment - 1));   // up align to "alignment"
            priv_trans->rx_buffer = heap_caps_aligned_alloc(alignment, buffer_byte_len, MALLOC_CAP_DMA);
            if (priv_trans->rx_buffer == NULL) {
                free(priv_trans->tx_buffer);
                return ESP_ERR_NO_MEM;
            }
        }
        esp_err_t ret = esp_cache_msync((void *)priv_trans->rx_buffer, buffer_byte_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        ESP_RETURN_ON_FALSE_ISR(ESP_OK == ret, ESP_ERR_INVALID_STATE, SPI_TAG, "mem sync m2c(invalid) fail");
    }
#endif  //SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    return ESP_OK;
}

esp_err_t SPI_SLAVE_ATTR spi_slave_queue_trans(spi_host_device_t host, const spi_slave_transaction_t *trans_desc, uint32_t ticks_to_wait)
{
    BaseType_t r;
    SPI_CHECK(is_valid_host(host), "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host], "host not slave", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host]->dma_enabled == 0 || trans_desc->tx_buffer == NULL || esp_ptr_dma_capable(trans_desc->tx_buffer),
              "txdata not in DMA-capable memory", ESP_ERR_INVALID_ARG);

    // We don't check length WORD alignment for rx when using DMA, seems break DMA requirement,
    // however peripheral can also stop DMA from over writing memory even if it not aligned (except esp32).
    // ATTENTION!: On esp32, peripheral can NOT stop DMA, if length not WORD aligned,
    // remain bytes in last word domain will overwritten by DMA HW, which may cause unexpected issues!
    // But driver already used for long time, to avoid breaking changes, we still don't add alignment limit.
    SPI_CHECK(spihost[host]->dma_enabled == 0 || trans_desc->rx_buffer == NULL ||
              (esp_ptr_dma_capable(trans_desc->rx_buffer) && esp_ptr_word_aligned(trans_desc->rx_buffer) &&
               (trans_desc->length % 8 == 0)),
              "rxdata not in DMA-capable memory or not BYTE aligned", ESP_ERR_INVALID_ARG);

    SPI_CHECK(trans_desc->length <= spihost[host]->max_transfer_sz * 8, "data transfer > host maximum", ESP_ERR_INVALID_ARG);

    spi_slave_trans_priv_t priv_trans = {.trans = (spi_slave_transaction_t *)trans_desc};
    SPI_CHECK(ESP_OK == spi_slave_setup_priv_trans(host, &priv_trans), "slave setup priv_trans failed", ESP_ERR_NO_MEM);

    r = xQueueSend(spihost[host]->trans_queue, (void *)&priv_trans, ticks_to_wait);
    if (!r) {
        return ESP_ERR_TIMEOUT;
    }
    esp_intr_enable(spihost[host]->intr);
    return ESP_OK;
}

/**
 * @note
 * This API is used to reset SPI Slave transaction queue. After calling this function:
 * - The SPI Slave transaction queue will be reset.
 * - The transaction which already mount on hardware will NOT be reset, and can be overwritten by next `trans_queue`
 *
 * Therefore, this API shouldn't be called when the corresponding SPI Master is doing an SPI transaction.
 *
 * @note
 * We don't actually need to enter a critical section here.
 * SPI Slave ISR will only get triggered when its corresponding SPI Master's transaction is done.
 * As we don't expect this function to be called when its corresponding SPI Master is doing an SPI transaction,
 * so concurrent call to these registers won't happen
 *
 */
esp_err_t SPI_SLAVE_ATTR spi_slave_queue_reset(spi_host_device_t host)
{
    SPI_CHECK(is_valid_host(host), "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host], "host not slave", ESP_ERR_INVALID_ARG);

    esp_intr_disable(spihost[host]->intr);
    spi_ll_set_int_stat(spihost[host]->hal.hw);

    spi_slave_trans_priv_t trans;
    while (uxQueueMessagesWaiting(spihost[host]->trans_queue)) {
        SPI_CHECK(pdTRUE == xQueueReceive(spihost[host]->trans_queue, &trans, 0), "can't reset queue", ESP_ERR_INVALID_STATE);
        spi_slave_uninstall_priv_trans(host, &trans);
    }
    spihost[host]->cur_trans.trans = NULL;

    return ESP_OK;
}

esp_err_t SPI_SLAVE_ISR_ATTR spi_slave_queue_trans_isr(spi_host_device_t host, const spi_slave_transaction_t *trans_desc)
{
    BaseType_t r;
    BaseType_t do_yield = pdFALSE;
    ESP_RETURN_ON_FALSE_ISR(is_valid_host(host), ESP_ERR_INVALID_ARG, SPI_TAG, "invalid host");
    ESP_RETURN_ON_FALSE_ISR(spihost[host], ESP_ERR_INVALID_ARG, SPI_TAG, "host not slave");
    ESP_RETURN_ON_FALSE_ISR(trans_desc->length <= spihost[host]->max_transfer_sz * 8, ESP_ERR_INVALID_ARG, SPI_TAG, "data transfer > host maximum");
    if (spihost[host]->dma_enabled) {
        uint16_t alignment = spihost[host]->internal_mem_align_size;
        (void) alignment;

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        // For those targets length and addr alignment is still required from Cache side
        uint32_t buffer_byte_len = (trans_desc->length + 7) / 8;
        bool tx_aligned = (trans_desc->tx_buffer == NULL) || (esp_ptr_dma_capable(trans_desc->tx_buffer) && ((((uint32_t)trans_desc->tx_buffer | buffer_byte_len) & (alignment - 1)) == 0));
        bool rx_aligned = (trans_desc->rx_buffer == NULL) || (esp_ptr_dma_capable(trans_desc->rx_buffer) && ((((uint32_t)trans_desc->rx_buffer | buffer_byte_len) & (alignment - 1)) == 0));
#else
        bool tx_aligned = (trans_desc->tx_buffer == NULL) || esp_ptr_dma_capable(trans_desc->tx_buffer);
        bool rx_aligned = (trans_desc->rx_buffer == NULL) || (esp_ptr_dma_capable(trans_desc->rx_buffer) && esp_ptr_word_aligned(trans_desc->rx_buffer) && (trans_desc->length % 8 == 0));
#endif

        ESP_RETURN_ON_FALSE_ISR(tx_aligned, ESP_ERR_INVALID_ARG, SPI_TAG, "txdata addr & len not align to %d bytes or not dma_capable", alignment);
        ESP_RETURN_ON_FALSE_ISR(rx_aligned, ESP_ERR_INVALID_ARG, SPI_TAG, "rxdata addr & len not align to %d bytes or not dma_capable", alignment);
    }

    spi_slave_trans_priv_t priv_trans = {
        .trans = (spi_slave_transaction_t *)trans_desc,
        .tx_buffer = (void *)trans_desc->tx_buffer,
        .rx_buffer = trans_desc->rx_buffer,
    };
    r = xQueueSendFromISR(spihost[host]->trans_queue, (void *)&priv_trans, &do_yield);
    if (!r) {
        return ESP_ERR_NO_MEM;
    }
    if (do_yield) {
        portYIELD_FROM_ISR();
    }
    return ESP_OK;
}

esp_err_t SPI_SLAVE_ISR_ATTR spi_slave_queue_reset_isr(spi_host_device_t host)
{
    ESP_RETURN_ON_FALSE_ISR(is_valid_host(host), ESP_ERR_INVALID_ARG, SPI_TAG, "invalid host");
    ESP_RETURN_ON_FALSE_ISR(spihost[host], ESP_ERR_INVALID_ARG, SPI_TAG, "host not slave");

    esp_err_t err = ESP_OK;
    spi_slave_trans_priv_t trans;
    BaseType_t do_yield = pdFALSE;
    while (pdFALSE == xQueueIsQueueEmptyFromISR(spihost[host]->trans_queue)) {
        if (pdTRUE != xQueueReceiveFromISR(spihost[host]->trans_queue, &trans, &do_yield)) {
            err = ESP_ERR_INVALID_STATE;
            break;
        }
        spi_slave_uninstall_priv_trans(host, &trans);
    }
    if (do_yield) {
        portYIELD_FROM_ISR();
    }
    ESP_RETURN_ON_ERROR_ISR(err, SPI_TAG, "can't reset queue");

    spihost[host]->cur_trans.trans = NULL;
    return ESP_OK;
}

esp_err_t SPI_SLAVE_ATTR spi_slave_get_trans_result(spi_host_device_t host, spi_slave_transaction_t **trans_desc, uint32_t ticks_to_wait)
{
    BaseType_t r;
    SPI_CHECK(is_valid_host(host), "invalid host", ESP_ERR_INVALID_ARG);
    SPI_CHECK(spihost[host], "host not slave", ESP_ERR_INVALID_ARG);
    //if SPI_SLAVE_NO_RETURN_RESULT is set, ret_queue will always be empty
    SPI_CHECK(!(spihost[host]->cfg.flags & SPI_SLAVE_NO_RETURN_RESULT), "API not Supported!", ESP_ERR_NOT_SUPPORTED);

    spi_slave_trans_priv_t priv_trans;
    r = xQueueReceive(spihost[host]->ret_queue, (void *)&priv_trans, ticks_to_wait);
    if (!r) {
        return ESP_ERR_TIMEOUT;
    }

    spi_slave_uninstall_priv_trans(host, &priv_trans);
    *trans_desc = priv_trans.trans;
    return ESP_OK;
}

esp_err_t SPI_SLAVE_ATTR spi_slave_transmit(spi_host_device_t host, spi_slave_transaction_t *trans_desc, uint32_t ticks_to_wait)
{
    esp_err_t ret;
    spi_slave_transaction_t *ret_trans;
    //ToDo: check if any spi transfers in flight
    ret = spi_slave_queue_trans(host, trans_desc, ticks_to_wait);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = spi_slave_get_trans_result(host, &ret_trans, ticks_to_wait);
    if (ret != ESP_OK) {
        return ret;
    }
    assert(ret_trans == trans_desc);
    return ESP_OK;
}

#if SOC_GDMA_SUPPORTED  // AHB_DMA_V1 and AXI_DMA
// dma is provided by gdma driver on these targets
#define spi_dma_reset               gdma_reset
#define spi_dma_start(chan, addr)   gdma_start(chan, (intptr_t)(addr))
#endif

static void SPI_SLAVE_ISR_ATTR s_spi_slave_dma_prepare_data(spi_dma_ctx_t *dma_ctx, spi_slave_hal_context_t *hal)
{
    if (hal->rx_buffer) {
        spicommon_dma_desc_setup_link(dma_ctx->dmadesc_rx, hal->rx_buffer, ((hal->bitlen + 7) / 8), true);

        spi_dma_reset(dma_ctx->rx_dma_chan);
        spi_slave_hal_hw_prepare_rx(hal->hw);
        spi_dma_start(dma_ctx->rx_dma_chan, dma_ctx->dmadesc_rx);
    }
    if (hal->tx_buffer) {
        spicommon_dma_desc_setup_link(dma_ctx->dmadesc_tx, hal->tx_buffer, (hal->bitlen + 7) / 8, false);

        spi_dma_reset(dma_ctx->tx_dma_chan);
        spi_slave_hal_hw_prepare_tx(hal->hw);
        spi_dma_start(dma_ctx->tx_dma_chan, dma_ctx->dmadesc_tx);
    }
}

static void SPI_SLAVE_ISR_ATTR s_spi_slave_prepare_data(spi_slave_t *host)
{
    spi_slave_hal_context_t *hal = &host->hal;

    if (host->dma_enabled) {
        s_spi_slave_dma_prepare_data(host->dma_ctx, &host->hal);
    } else {
        //No DMA. Copy data to transmit buffers.
        spi_slave_hal_push_tx_buffer(hal);
        spi_slave_hal_hw_fifo_reset(hal, true, false);
    }
    spi_slave_hal_set_trans_bitlen(hal);

#ifdef CONFIG_IDF_TARGET_ESP32
    //SPI Slave mode on ESP32 requires MOSI/MISO enable
    spi_slave_hal_enable_data_line(hal);
#endif
}

#if CONFIG_IDF_TARGET_ESP32
static void SPI_SLAVE_ISR_ATTR spi_slave_restart_after_dmareset(void *arg)
{
    spi_slave_t *host = (spi_slave_t *)arg;
    esp_intr_enable(host->intr);
}
#endif  //#if CONFIG_IDF_TARGET_ESP32

//This is run in interrupt context and apart from initialization and destruction, this is the only code
//touching the host (=spihost[x]) variable. The rest of the data arrives in queues. That is why there are
//no muxes in this code.
static void SPI_SLAVE_ISR_ATTR spi_intr(void *arg)
{
    BaseType_t r;
    BaseType_t do_yield = pdFALSE;
    spi_slave_t *host = (spi_slave_t *)arg;
    spi_slave_hal_context_t *hal = &host->hal;

    assert(spi_slave_hal_usr_is_done(hal));

    bool use_dma = host->dma_enabled;
    if (host->cur_trans.trans) {
        // When DMA is enabled, the slave rx dma suffers from unexpected transactions. Forbid reading until transaction ready.
        if (use_dma) {
            freeze_cs(host);
        }

        spi_slave_hal_store_result(hal);
        host->cur_trans.trans->trans_len = spi_slave_hal_get_rcv_bitlen(hal);

#if CONFIG_IDF_TARGET_ESP32
        //This workaround is only for esp32
        if (spi_slave_hal_dma_need_reset(hal)) {
            //On ESP32, actual_tx_dma_chan and actual_rx_dma_chan are always same
            spicommon_dmaworkaround_req_reset(host->dma_ctx->tx_dma_chan.chan_id, spi_slave_restart_after_dmareset, host);
        }
#endif  //#if CONFIG_IDF_TARGET_ESP32

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE   //invalidate here to let user access rx data in post_cb if possible
        if (use_dma && host->cur_trans.rx_buffer) {
            uint16_t alignment = host->internal_mem_align_size;
            uint32_t buffer_byte_len = (host->cur_trans.trans->length + 7) / 8;
            buffer_byte_len = (buffer_byte_len + alignment - 1) & (~(alignment - 1));
            // invalidate priv_trans.buffer_to_rcv anyway, only user provide aligned buffer can rcv correct data in post_cb
            esp_err_t ret = esp_cache_msync((void *)host->cur_trans.rx_buffer, buffer_byte_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
            assert(ret == ESP_OK);
            (void)ret;
        }
#endif
        if (host->cfg.post_trans_cb) {
            host->cfg.post_trans_cb(host->cur_trans.trans);
        }

        if (!(host->cfg.flags & SPI_SLAVE_NO_RETURN_RESULT)) {
            xQueueSendFromISR(host->ret_queue, &host->cur_trans, &do_yield);
        }
        host->cur_trans.trans = NULL;
    }

#if CONFIG_IDF_TARGET_ESP32
    //This workaround is only for esp32
    if (use_dma) {
        //On ESP32, actual_tx_dma_chan and actual_rx_dma_chan are always same
        spicommon_dmaworkaround_idle(host->dma_ctx->tx_dma_chan.chan_id);
        if (spicommon_dmaworkaround_reset_in_progress()) {
            //We need to wait for the reset to complete. Disable int (will be re-enabled on reset callback) and exit isr.
            esp_intr_disable(host->intr);
            if (do_yield) {
                portYIELD_FROM_ISR();
            }
            return;
        }
    }
#endif  //#if CONFIG_IDF_TARGET_ESP32

    //Disable interrupt before checking to avoid concurrency issue.
    esp_intr_disable(host->intr);
    spi_slave_trans_priv_t priv_trans;
    //Grab next transaction
    r = xQueueReceiveFromISR(host->trans_queue, &priv_trans, &do_yield);
    if (r) {
        // sanity check
        assert(priv_trans.trans);

        //enable the interrupt again if there is packet to send
        esp_intr_enable(host->intr);

        //We have a transaction. Send it.
        host->cur_trans = priv_trans;

        hal->bitlen = priv_trans.trans->length;
        hal->rx_buffer = priv_trans.rx_buffer;
        hal->tx_buffer = priv_trans.tx_buffer;

#if CONFIG_IDF_TARGET_ESP32
        if (use_dma) {
            //This workaround is only for esp32
            //On ESP32, actual_tx_dma_chan and actual_rx_dma_chan are always same
            spicommon_dmaworkaround_transfer_active(host->dma_ctx->tx_dma_chan.chan_id);
        }
#endif  //#if CONFIG_IDF_TARGET_ESP32

        spi_slave_hal_hw_reset(hal);
        s_spi_slave_prepare_data(host);

        //The slave rx dma get disturbed by unexpected transaction. Only connect the CS and start DMA when slave is ready.
        if (use_dma) {
            restore_cs(host);
        }

        //Kick off transfer
        spi_slave_hal_user_start(hal);
        if (host->cfg.post_setup_cb) {
            host->cfg.post_setup_cb(priv_trans.trans);
        }
    }
    if (do_yield) {
        portYIELD_FROM_ISR();
    }
}
