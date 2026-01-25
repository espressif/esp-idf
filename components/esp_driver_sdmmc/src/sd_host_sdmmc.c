/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/param.h>
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "esp_check.h"
#include "esp_clk_tree.h"
#include "esp_timer.h"
#include "esp_memory_utils.h"
#include "soc/chip_revision.h"
#include "soc/sdmmc_pins.h"
#include "soc/sdmmc_periph.h"
#include "soc/soc_caps.h"
#include "hal/efuse_hal.h"
#include "hal/sd_types.h"
#include "hal/sdmmc_hal.h"
#include "hal/sdmmc_ll.h"
#include "driver/sd_host.h"
#include "driver/sd_host_sdmmc.h"
#include "esp_private/sd_driver_interface.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_cache_private.h"
#include "esp_private/gpio.h"
#include "esp_private/sd_host_private.h"
#include "freertos/FreeRTOS.h"

typedef struct sd_platform_t {
    _lock_t              mutex;
    sd_host_sdmmc_ctlr_t *controllers[SDMMC_LL_HOST_CTLR_NUMS];
} sd_platform_t;

static const char *TAG = "SD_HOST";
static sd_platform_t s_platform;

//Controller Allocation
static esp_err_t sd_host_claim_controller(sd_host_sdmmc_ctlr_t *controller);
static esp_err_t sd_host_declaim_controller(sd_host_sdmmc_ctlr_t *controller);
//ISR Handler
static void sd_host_isr(void *arg);
//HW Commands
static esp_err_t sd_host_slot_clock_update_command(sd_host_sdmmc_slot_t *slot, bool is_cmd11);
//Control
static esp_err_t sd_host_reset(sd_host_sdmmc_ctlr_t *ctlr);
//Clock
static void sd_host_set_clk_div(sd_host_sdmmc_ctlr_t *ctlr, soc_periph_sdmmc_clk_src_t src, int div);
static int sd_host_calc_freq(soc_periph_sdmmc_clk_src_t src, const int host_div, const int card_div);
static void sd_host_slot_get_clk_dividers(sd_host_sdmmc_slot_t *slot, uint32_t freq_khz, int *host_div, int *card_div, soc_periph_sdmmc_clk_src_t *src);
//IO
static void configure_pin(uint8_t gpio_num, uint8_t gpio_matrix_sig, gpio_mode_t mode, const char *name, bool use_gpio_matrix);
static esp_err_t sdmmc_slot_io_config(sd_host_sdmmc_slot_t *slot, const sd_host_slot_sdmmc_init_cfg_t *slot_init_config);
//Public APIs
static esp_err_t sd_host_slot_sdmmc_configure(sd_host_slot_handle_t slot, const sd_host_slot_cfg_t *config);
static esp_err_t sd_host_slot_sdmmc_register_event_callbacks(sd_host_slot_handle_t slot, const sd_host_evt_cbs_t *cbs, void *user_data);
static esp_err_t sd_host_controller_remove_sdmmc_slot(sd_host_slot_handle_t slot);
static esp_err_t sd_host_del_sdmmc_controller(sd_host_ctlr_handle_t ctlr);
static esp_err_t sd_host_slot_sdmmc_set_cclk_always_on(sd_host_slot_handle_t slot, bool always_on);
static esp_err_t sd_host_slot_sdmmc_io_int_enable(sd_host_slot_handle_t slot);
static esp_err_t sd_host_slot_sdmmc_io_int_wait(sd_host_slot_handle_t slot, TickType_t timeout_ticks);
static esp_err_t sd_host_slot_sdmmc_get_info(sd_host_slot_handle_t slot, sd_host_slot_info_t *info);

/*---------------------------------------------------------------
                    Public APIs
---------------------------------------------------------------*/
esp_err_t sd_host_create_sdmmc_controller(const sd_host_sdmmc_cfg_t *config, sd_host_ctlr_handle_t *ret_handle)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(config && ret_handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    sd_host_sdmmc_ctlr_t *ctlr = heap_caps_calloc(1, sizeof(sd_host_sdmmc_ctlr_t), SD_HOST_SDMMC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(ctlr, ESP_ERR_NO_MEM, TAG, "no mem for sd host controller context");

    ret = sd_host_claim_controller(ctlr);
    if (ret != ESP_OK) {
        //claim fail, clean and return directly
        free(ctlr);
        ESP_RETURN_ON_ERROR(ret, TAG, "no available sd host controller");
    }

    ctlr->dma_desc_num = config->dma_desc_num ? config->dma_desc_num : SD_HOST_SDMMC_DMA_DESC_CNT;
    ctlr->dma_desc = heap_caps_calloc(1, sizeof(sdmmc_desc_t) * ctlr->dma_desc_num, SD_HOST_SDMMC_DMA_ALLOC_CAPS);
    ESP_LOGD(TAG, "ctlr->dma_desc addr: %p", ctlr->dma_desc);
    ESP_RETURN_ON_FALSE(ctlr->dma_desc, ESP_ERR_NO_MEM, TAG, "no mem for dma descriptors");

    ctlr->io_intr_sem = xSemaphoreCreateBinaryWithCaps(SD_HOST_SDMMC_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(ctlr->io_intr_sem, ESP_ERR_NO_MEM, err, TAG, "no mem for io interrupt semaphore");

    ctlr->mutex = xSemaphoreCreateMutexWithCaps(SD_HOST_SDMMC_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(ctlr->mutex, ESP_ERR_NO_MEM, err, TAG, "no mem for mutex");

    ctlr->event_queue = xQueueCreateWithCaps(config->event_queue_items ? config->event_queue_items : 4, sizeof(sd_host_sdmmc_event_t), SD_HOST_SDMMC_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(ctlr->event_queue, ESP_ERR_NO_MEM, err, TAG, "no mem for mutex");

    //use `ETS_SDIO_HOST_INTR_SOURCE` for now, replace with `sdmmc_periph.c` then. IDF-12585
    ESP_GOTO_ON_ERROR(esp_intr_alloc(ETS_SDIO_HOST_INTR_SOURCE, 0, &sd_host_isr, ctlr, &ctlr->intr_handle), err, TAG, "failed to install interrupt service");

#if CONFIG_PM_ENABLE
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "sdmmc", &ctlr->pm_lock), err, TAG, "failed to create pm lock");
#endif //CONFIG_PM_ENABLE

    sdmmc_hal_init(&ctlr->hal);
    sd_host_set_clk_div(ctlr, SDMMC_CLK_SRC_DEFAULT, 2);

    // Reset
    esp_err_t err = sd_host_reset(ctlr);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: sd_host_reset returned 0x%x", __func__, err);
        return err;
    }

    ESP_LOGD(TAG, "peripheral version %"PRIx32", hardware config %08"PRIx32, sdmmc_ll_get_version_id(ctlr->hal.dev), sdmmc_ll_get_hw_config_info(ctlr->hal.dev));

    sdmmc_ll_clear_interrupt(ctlr->hal.dev, 0xffffffff);
    sdmmc_ll_enable_interrupt(ctlr->hal.dev, 0xffffffff, false);
    sdmmc_ll_enable_global_interrupt(ctlr->hal.dev, false);
    sdmmc_ll_enable_interrupt(ctlr->hal.dev, SDMMC_LL_EVENT_DEFAULT, true);
    sdmmc_ll_enable_global_interrupt(ctlr->hal.dev, true);
    sdmmc_ll_init_dma(ctlr->hal.dev);

    ctlr->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    ctlr->drv.del_ctlr = sd_host_del_sdmmc_controller;
    *ret_handle = &ctlr->drv;

    return ESP_OK;

err:
    sd_host_del_sdmmc_controller(&ctlr->drv);

    return ret;
}

esp_err_t sd_host_sdmmc_controller_add_slot(sd_host_ctlr_handle_t ctlr, const sd_host_slot_sdmmc_init_cfg_t *config, sd_host_slot_handle_t *ret_handle)
{
    esp_err_t ret = ESP_FAIL;
    sd_host_sdmmc_ctlr_t *ctlr_ctx = __containerof(ctlr, sd_host_sdmmc_ctlr_t, drv);
    ESP_RETURN_ON_FALSE(ctlr && config && ret_handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(config->slot_id >= 0 && config->slot_id < SOC_SDMMC_NUM_SLOTS, ESP_ERR_INVALID_ARG, TAG, "invalid slot id");

    sd_host_sdmmc_slot_t *slot = heap_caps_calloc(1, sizeof(sd_host_sdmmc_slot_t), SD_HOST_SDMMC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(slot, ESP_ERR_NO_MEM, TAG, "no mem for sd host slot context");

    bool slot_available = false;
    portENTER_CRITICAL(&ctlr_ctx->spinlock);
    if (!ctlr_ctx->slot[config->slot_id]) {
        slot_available = true;
        ctlr_ctx->slot[config->slot_id] = slot;
    }
    portEXIT_CRITICAL(&ctlr_ctx->spinlock);
    ESP_GOTO_ON_FALSE(slot_available, ESP_ERR_INVALID_STATE, err, TAG, "slot is not available");

    portENTER_CRITICAL(&ctlr_ctx->spinlock);
    slot->slot_id = config->slot_id;
    slot->sd_mode = config->sd_mode;
    slot->freq.freq_hz = 400 * 1000;
    slot->freq.freq_state = SD_HOST_SLOT_STATE_INIT;
    slot->ctlr = ctlr_ctx;
    ctlr_ctx->registered_slot_nums = ctlr_ctx->registered_slot_nums + 1;

    slot->drv.configure = sd_host_slot_sdmmc_configure;
    slot->drv.do_transaction = sd_host_slot_sdmmc_do_transaction;
    slot->drv.register_cbs = sd_host_slot_sdmmc_register_event_callbacks;
    slot->drv.remove_slot = sd_host_controller_remove_sdmmc_slot;
    slot->drv.set_cclk_always_on = sd_host_slot_sdmmc_set_cclk_always_on;
    slot->drv.enable_io_int = sd_host_slot_sdmmc_io_int_enable;
    slot->drv.wait_io_int = sd_host_slot_sdmmc_io_int_wait;
    slot->drv.get_info = sd_host_slot_sdmmc_get_info;
    portEXIT_CRITICAL(&ctlr_ctx->spinlock);

    ESP_GOTO_ON_ERROR(sdmmc_slot_io_config(slot, config), err, TAG, "failed to config slot io");

    *ret_handle = &slot->drv;

    return ESP_OK;

err:
    free(slot);

    return ret;
}

/*---------------------------------------------------------------
                    Public API Impls
---------------------------------------------------------------*/
static esp_err_t sd_host_slot_sdmmc_configure(sd_host_slot_handle_t slot, const sd_host_slot_cfg_t *config)
{
    ESP_RETURN_ON_FALSE(slot && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
#if SDMMC_LL_DELAY_PHASE_SUPPORTED
    ESP_RETURN_ON_FALSE(config->delayphase < (SOC_SDMMC_DELAY_PHASE_NUM + 1), ESP_ERR_INVALID_ARG, TAG, "invalid delay phase");
#else
    //DIG-217
    ESP_LOGW(TAG, "esp32 doesn't support input phase delay, fallback to 0 delay");
#endif

#if SOC_SDMMC_UHS_I_SUPPORTED
    ESP_RETURN_ON_FALSE(config->delayline < (SOC_SDMMC_DELAY_PHASE_NUM + 1), ESP_ERR_INVALID_ARG, TAG, "invalid delay line");
#else
    ESP_LOGW(TAG, "input line delay not supported, fallback to 0 delay");
#endif

#if CONFIG_IDF_TARGET_ESP32P4
    if (config->freq_hz == SDMMC_FREQ_SDR104 * 1000) {
        unsigned chip_version = efuse_hal_chip_revision();
        ESP_LOGD(TAG, "chip_version: %d", chip_version);
        if (!ESP_CHIP_REV_ABOVE(chip_version, 300)) {
            ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "UHS-I SDR104 is not supported on ESP32P4 chips prior than v3.0");
        }
    }
#endif

    sd_host_sdmmc_slot_t *slot_ctx = __containerof(slot, sd_host_sdmmc_slot_t, drv);
    portENTER_CRITICAL(&slot_ctx->ctlr->spinlock);
    if (config->freq_hz != 0) {
        slot_ctx->freq.freq_hz = config->freq_hz;
        slot_ctx->freq.freq_state = SD_HOST_SLOT_STATE_READY;
    }
    if (config->width != 0) {
        slot_ctx->width.width_state = SD_HOST_SLOT_STATE_READY;
    }
    if (config->sampling_mode != 0) {
        slot_ctx->sampling_mode.mode = config->sampling_mode;
        slot_ctx->sampling_mode.sampling_mode_state = SD_HOST_SLOT_STATE_READY;
    }
    if (config->delayphase != 0) {
#if SDMMC_LL_DELAY_PHASE_SUPPORTED
        slot_ctx->delay_phase.delayphase = config->delayphase;
#else
        slot_ctx->delay_phase.delayphase = SDMMC_DELAY_PHASE_0;
#endif
        slot_ctx->delay_phase.delay_phase_state = SD_HOST_SLOT_STATE_READY;
    }
    if (config->delayline != 0) {
#if SOC_SDMMC_UHS_I_SUPPORTED
        slot_ctx->delay_line.delayline = config->delayline;
#else
        slot_ctx->delay_line.delayline = SDMMC_DELAY_LINE_0;
#endif
        slot_ctx->delay_line.delay_line_state = SD_HOST_SLOT_STATE_READY;
    }
    portEXIT_CRITICAL(&slot_ctx->ctlr->spinlock);

    return ESP_OK;
}

static esp_err_t sd_host_slot_sdmmc_register_event_callbacks(sd_host_slot_handle_t slot, const sd_host_evt_cbs_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(slot && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

#if CONFIG_CAM_CTLR_MIPI_CSI_ISR_CACHE_SAFE
    if (cbs->on_trans_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_trans_done), ESP_ERR_INVALID_ARG, TAG, "on_trans_done callback not in IRAM");
    }
    if (cbs->on_io_interrupt) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_io_interrupt), ESP_ERR_INVALID_ARG, TAG, "on_io_interrupt callback not in IRAM");
    }
#endif

    sd_host_sdmmc_slot_t *slot_ctx = __containerof(slot, sd_host_sdmmc_slot_t, drv);
    slot_ctx->cbs.on_trans_done = cbs->on_trans_done;
    slot_ctx->cbs.on_io_interrupt = cbs->on_io_interrupt;
    slot_ctx->user_data = user_data;

    return ESP_OK;
}

esp_err_t sd_host_slot_set_cclk_always_on_internal(sd_host_sdmmc_slot_t *slot, bool always_on)
{
    ESP_RETURN_ON_FALSE(slot, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    // During initialization this is not protected by a mutex
    if (always_on) {
        sdmmc_ll_enable_card_clock_low_power(slot->ctlr->hal.dev, slot->slot_id, false);
    } else {
        sdmmc_ll_enable_card_clock_low_power(slot->ctlr->hal.dev, slot->slot_id, true);
    }
    sd_host_slot_clock_update_command(slot, false);

    return ESP_OK;
}

static esp_err_t sd_host_slot_sdmmc_set_cclk_always_on(sd_host_slot_handle_t slot, bool always_on)
{
    esp_err_t ret = ESP_FAIL;
    sd_host_sdmmc_slot_t *slot_ctx = __containerof(slot, sd_host_sdmmc_slot_t, drv);

    xSemaphoreTake(slot_ctx->ctlr->mutex, portMAX_DELAY);
    ret = sd_host_slot_set_cclk_always_on_internal(slot_ctx, always_on);
    xSemaphoreGive(slot_ctx->ctlr->mutex);

    ESP_RETURN_ON_ERROR(ret, TAG, "failed to set cclk always on");

    return ESP_OK;
}

static esp_err_t sd_host_controller_remove_sdmmc_slot(sd_host_slot_handle_t slot)
{
    ESP_RETURN_ON_FALSE(slot, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    sd_host_sdmmc_slot_t *slot_ctx = __containerof(slot, sd_host_sdmmc_slot_t, drv);
    sd_host_sdmmc_ctlr_t *ctlr = slot_ctx->ctlr;

    bool slot_registered = false;
    portENTER_CRITICAL(&ctlr->spinlock);
    if (ctlr->slot[slot_ctx->slot_id]) {
        slot_registered = true;
        ctlr->slot[slot_ctx->slot_id] = NULL;
        ctlr->registered_slot_nums -= 1;
    }
    portEXIT_CRITICAL(&ctlr->spinlock);
    ESP_RETURN_ON_FALSE(slot_registered, ESP_ERR_INVALID_STATE, TAG, "slot is not registered");

    if (slot_ctx->io_config.clk_io != GPIO_NUM_NC) {
        gpio_output_disable(slot_ctx->io_config.clk_io);
    }
    if (slot_ctx->io_config.cmd_io != GPIO_NUM_NC) {
        gpio_output_disable(slot_ctx->io_config.cmd_io);
    }
    if (slot_ctx->io_config.cd_io != GPIO_NUM_NC) {
        gpio_output_disable(slot_ctx->io_config.cd_io);
    }
    if (slot_ctx->io_config.wp_io != GPIO_NUM_NC) {
        gpio_output_disable(slot_ctx->io_config.wp_io);
    }
    if (slot_ctx->io_config.width >= 1) {
        gpio_output_disable(slot_ctx->io_config.d0_io);
    }
    if (slot_ctx->io_config.width >= 4) {
        gpio_output_disable(slot_ctx->io_config.d1_io);
        gpio_output_disable(slot_ctx->io_config.d2_io);
        gpio_output_disable(slot_ctx->io_config.d3_io);
    }
    if (slot_ctx->io_config.width == 8) {
        gpio_output_disable(slot_ctx->io_config.d4_io);
        gpio_output_disable(slot_ctx->io_config.d5_io);
        gpio_output_disable(slot_ctx->io_config.d6_io);
        gpio_output_disable(slot_ctx->io_config.d7_io);
    }

    free(slot);

    return ESP_OK;
}

static esp_err_t sd_host_del_sdmmc_controller(sd_host_ctlr_handle_t ctlr)
{
    sd_host_sdmmc_ctlr_t *ctlr_ctx = __containerof(ctlr, sd_host_sdmmc_ctlr_t, drv);
    ESP_RETURN_ON_FALSE(ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(ctlr_ctx->registered_slot_nums == 0, ESP_ERR_INVALID_STATE, TAG, "host controller with slot registered");

    ESP_RETURN_ON_ERROR(sd_host_declaim_controller(ctlr_ctx), TAG, "controller isn't in use");
    ESP_RETURN_ON_ERROR(esp_intr_free(ctlr_ctx->intr_handle), TAG, "failed to delete interrupt service");

    if (ctlr_ctx->event_queue) {
        vQueueDeleteWithCaps(ctlr_ctx->event_queue);
    }
    if (ctlr_ctx->mutex) {
        vSemaphoreDeleteWithCaps(ctlr_ctx->mutex);
    }
    if (ctlr_ctx->io_intr_sem) {
        vSemaphoreDeleteWithCaps(ctlr_ctx->io_intr_sem);
    }
#if CONFIG_PM_ENABLE
    if (ctlr_ctx->pm_lock) {
        esp_pm_lock_delete(ctlr_ctx->pm_lock);
    }
#endif

    free(ctlr_ctx->dma_desc);
    ctlr_ctx->dma_desc = NULL;
    free(ctlr_ctx);
    ctlr_ctx = NULL;

    return ESP_OK;
}

static esp_err_t sd_host_slot_sdmmc_io_int_enable(sd_host_slot_handle_t slot)
{
    ESP_RETURN_ON_FALSE(slot, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    sd_host_sdmmc_slot_t *slot_ctx = __containerof(slot, sd_host_sdmmc_slot_t, drv);
    configure_pin(slot_ctx->io_config.d1_io, sdmmc_slot_gpio_sig[slot_ctx->slot_id].d1, GPIO_MODE_INPUT_OUTPUT, "d1", slot_ctx->use_gpio_matrix);
    return ESP_OK;
}

static esp_err_t sd_host_slot_sdmmc_io_int_wait(sd_host_slot_handle_t slot, TickType_t timeout_ticks)
{
    ESP_RETURN_ON_FALSE(slot, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    esp_err_t ret = ESP_FAIL;
    sd_host_sdmmc_slot_t *slot_ctx = __containerof(slot, sd_host_sdmmc_slot_t, drv);

    /* SDIO interrupts are negedge sensitive ones: the status bit is only set
     * when first interrupt triggered.
     *
     * If D1 GPIO is low when entering this function, we know that interrupt
     * (in SDIO sense) has occurred and we don't need to use SDMMC peripheral
     * interrupt.
     */

    /* Disable SDIO interrupt */
    if (slot_ctx->slot_id == 0) {
        sdmmc_ll_enable_interrupt(slot_ctx->ctlr->hal.dev, SDMMC_INTMASK_IO_SLOT0, false);
        sdmmc_ll_clear_interrupt(slot_ctx->ctlr->hal.dev, SDMMC_INTMASK_IO_SLOT0);
    } else {
        sdmmc_ll_enable_interrupt(slot_ctx->ctlr->hal.dev, SDMMC_INTMASK_IO_SLOT1, false);
        sdmmc_ll_clear_interrupt(slot_ctx->ctlr->hal.dev, SDMMC_INTMASK_IO_SLOT1);
    }

    if (gpio_get_level(slot_ctx->io_config.d1_io) == 0) {
        return ESP_OK;
    }
    /* Otherwise, need to wait for an interrupt. Since D1 was high,
     * SDMMC peripheral interrupt is guaranteed to trigger on negedge.
     */
    xSemaphoreTake(slot_ctx->ctlr->io_intr_sem, 0);

    /* Re-enable SDIO interrupt */
    if (slot_ctx->slot_id == 0) {
        sdmmc_ll_enable_interrupt(slot_ctx->ctlr->hal.dev, SDMMC_INTMASK_IO_SLOT0, true);
    } else {
        sdmmc_ll_enable_interrupt(slot_ctx->ctlr->hal.dev, SDMMC_INTMASK_IO_SLOT1, true);
    }

    if (xSemaphoreTake(slot_ctx->ctlr->io_intr_sem, timeout_ticks) == pdTRUE) {
        ret = ESP_OK;
    } else {
        ret = ESP_ERR_TIMEOUT;
    }

    return ret;
}

static esp_err_t sd_host_slot_sdmmc_get_info(sd_host_slot_handle_t slot, sd_host_slot_info_t *info)
{
    ESP_RETURN_ON_FALSE(slot && info, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    sd_host_sdmmc_slot_t *slot_ctx = __containerof(slot, sd_host_sdmmc_slot_t, drv);
    portENTER_CRITICAL(&slot_ctx->ctlr->spinlock);
    info->freq_hz = slot_ctx->freq.real_freq_hz;
    info->width = slot_ctx->width.width;
    info->sd_mode = slot_ctx->sd_mode;
    info->sampling_mode = slot_ctx->sampling_mode.mode;
    portEXIT_CRITICAL(&slot_ctx->ctlr->spinlock);

    return ESP_OK;
}

/*---------------------------------------------------------------
                    Internal APIs
---------------------------------------------------------------*/
bool sd_host_check_buffer_alignment(sd_host_sdmmc_slot_t *slot, const void *buf, size_t size)
{
    //for future-proof
    (void)slot;
    if (!buf || !size) {
        return false;
    }

    esp_err_t ret = ESP_FAIL;
    int cache_flags = 0;
    size_t cache_alignment_bytes = 0;
    if (esp_ptr_external_ram(buf)) {
        cache_flags |= MALLOC_CAP_SPIRAM;
    }
    ret = esp_cache_get_alignment(cache_flags, &cache_alignment_bytes);
    assert(ret == ESP_OK);
    (void)ret;

    bool is_aligned = false;
    size_t alignment = 0;

    if (cache_alignment_bytes != 0) {
        alignment = cache_alignment_bytes;
    } else {
        alignment = SDMMC_DMA_ALIGNMENT;
    }

    is_aligned = ((intptr_t)buf % alignment == 0) && (size % alignment == 0);

    return is_aligned;
}

esp_err_t sd_host_slot_set_card_clk(sd_host_sdmmc_slot_t *slot)
{
    // Disable clock first
    sdmmc_ll_enable_card_clock(slot->ctlr->hal.dev, slot->slot_id, false);
    esp_err_t ret = sd_host_slot_clock_update_command(slot, false);
    ESP_RETURN_ON_ERROR(ret, TAG, "%s: sd_host_slot_clock_update_command returned 0x%x, failed to disable clk, ", __func__, ret);

    int freq_khz = 0;
    if (slot->freq.freq_state == SD_HOST_SLOT_STATE_READY) {
        freq_khz = slot->freq.freq_hz / 1000;
    } else {
        freq_khz = 400;
    }
    ESP_LOGD(TAG, "freq: %d khz, state: %d", freq_khz, slot->freq.freq_state);

    soc_periph_sdmmc_clk_src_t clk_src = 0;
    int host_div = 0;   /* clock divider of the host (SDMMC.clock) */
    int card_div = 0;   /* 1/2 of card clock divider (SDMMC.clkdiv) */
    sd_host_slot_get_clk_dividers(slot, freq_khz, &host_div, &card_div, &clk_src);

    int real_freq_khz = sd_host_calc_freq(clk_src, host_div, card_div);
    slot->freq.real_freq_hz = real_freq_khz * 1000;
    ESP_LOGD(TAG, "slot_id=%d clk_src=%d host_div=%d card_div=%d freq=%dkHz (max %" PRIu32 "kHz)", slot->slot_id, clk_src, host_div, card_div, real_freq_khz, freq_khz);

    // Program card clock settings, send them to the CIU
    sdmmc_ll_set_card_clock_div(slot->ctlr->hal.dev, slot->slot_id, card_div);
    sd_host_set_clk_div(slot->ctlr, clk_src, host_div);
    ret = sd_host_slot_clock_update_command(slot, false);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "setting clk div failed");
        ESP_LOGE(TAG, "%s: sd_host_slot_clock_update_command returned 0x%x", __func__, ret);
        return ret;
    }

    // Re-enable clocks
    sdmmc_ll_enable_card_clock(slot->ctlr->hal.dev, slot->slot_id, true);
    sdmmc_ll_enable_card_clock_low_power(slot->ctlr->hal.dev, slot->slot_id, true);
    ret = sd_host_slot_clock_update_command(slot, false);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "re-enabling clk failed");
        ESP_LOGE(TAG, "%s: sd_host_slot_clock_update_command returned 0x%x", __func__, ret);
        return ret;
    }

    const uint32_t data_timeout_ms = 100;
    uint32_t data_timeout_cycles = data_timeout_ms * freq_khz;
    sdmmc_ll_set_data_timeout(slot->ctlr->hal.dev, data_timeout_cycles);
    // always set response timeout to highest value, it's small enough anyway
    sdmmc_ll_set_response_timeout(slot->ctlr->hal.dev, 255);

    return ESP_OK;
}

esp_err_t sd_host_slot_get_real_freq(sd_host_sdmmc_slot_t *slot, int *real_freq_khz)
{
    ESP_RETURN_ON_FALSE(slot && real_freq_khz, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    int host_div = 0;
    int card_div = 0;
    host_div = sdmmc_ll_get_clock_div(slot->ctlr->hal.dev);
    card_div = sdmmc_ll_get_card_clock_div(slot->ctlr->hal.dev, slot->slot_id);
    *real_freq_khz = sd_host_calc_freq(slot->clk_src, host_div, card_div);

    return ESP_OK;
}

esp_err_t sd_host_slot_get_calc_real_freq(sd_host_sdmmc_slot_t *slot, int *real_freq_khz)
{
    ESP_RETURN_ON_FALSE(slot && real_freq_khz, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    soc_periph_sdmmc_clk_src_t clk_src = 0;
    int host_div = 0;
    int card_div = 0;
    sd_host_slot_get_clk_dividers(slot, slot->freq.freq_hz / 1000, &host_div, &card_div, &clk_src);
    *real_freq_khz = sd_host_calc_freq(slot->clk_src, host_div, card_div);

    return ESP_OK;
}

esp_err_t sd_host_slot_set_bus_width(sd_host_sdmmc_slot_t *slot)
{
    ESP_RETURN_ON_FALSE(slot, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    int width = slot->width.width_state == SD_HOST_SLOT_STATE_READY ? slot->width.width : 1;
    ESP_LOGD(TAG, "width: %d, state: %d", width, slot->width.width_state);
    sd_host_sdmmc_slot_io_cfg_t *slot_gpio = &slot->io_config;

    if (width == 1) {
        sdmmc_ll_set_card_width(slot->ctlr->hal.dev, slot->slot_id, SD_BUS_WIDTH_1_BIT);
    } else if (width == 4) {
        sdmmc_ll_set_card_width(slot->ctlr->hal.dev, slot->slot_id, SD_BUS_WIDTH_4_BIT);
        // D3 was set to GPIO high to force slave into SD mode, until 4-bit mode is set
        configure_pin(slot_gpio->d3_io, sdmmc_slot_gpio_sig[slot->slot_id].d3, GPIO_MODE_INPUT_OUTPUT, "d3", slot->use_gpio_matrix);
    } else if (width == 8) {
        sdmmc_ll_set_card_width(slot->ctlr->hal.dev, slot->slot_id, SD_BUS_WIDTH_8_BIT);
        // D3 was set to GPIO high to force slave into SD mode, until 4-bit mode is set
        configure_pin(slot_gpio->d3_io, sdmmc_slot_gpio_sig[slot->slot_id].d3, GPIO_MODE_INPUT_OUTPUT, "d3", slot->use_gpio_matrix);
    } else {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGD(TAG, "slot_id=%d width=%d", slot->slot_id, width);
    return ESP_OK;
}

esp_err_t sd_host_slot_set_bus_sampling_mode(sd_host_sdmmc_slot_t *slot)
{
    ESP_RETURN_ON_FALSE(slot, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    if (slot->sampling_mode.sampling_mode_state == SD_HOST_SLOT_STATE_READY) {
        ESP_RETURN_ON_FALSE((slot->width.width != 8), ESP_ERR_NOT_SUPPORTED, TAG, "DDR mode with 8-bit bus width is not supported yet");
    }

    bool ddr = false;
    if (slot->sampling_mode.sampling_mode_state == SD_HOST_SLOT_STATE_READY) {
        if (slot->sampling_mode.mode == SD_SAMPLING_MODE_DDR) {
            ddr = true;
        }
    }

    sdmmc_ll_enable_ddr_mode(slot->ctlr->hal.dev, slot->slot_id, ddr);
    ESP_LOGD(TAG, "slot_id=%d ddr=%d", slot->slot_id, ddr ? 1 : 0);

    return ESP_OK;
}

esp_err_t sd_host_set_delay_phase(sd_host_sdmmc_slot_t *slot)
{
#if SDMMC_LL_DELAY_PHASE_SUPPORTED
    ESP_RETURN_ON_FALSE(slot, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    sdmmc_delay_phase_t delay_phase = SDMMC_DELAY_PHASE_0;
    if (slot->delay_phase.delay_phase_state == SD_HOST_SLOT_STATE_READY) {
        delay_phase = slot->delay_phase.delayphase;
    }

    uint32_t clk_src_freq_hz = 0;
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz(slot->clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_freq_hz),
                        TAG, "get source clock frequency failed");

    //Now we're in high speed. Note ESP SDMMC Host HW only supports integer divider.
    int delay_phase_num = 0;
    sdmmc_ll_delay_phase_t phase = SDMMC_LL_DELAY_PHASE_0;
    sdmmc_ll_speed_mode_t speed_mode = (slot->freq.freq_hz / 1000) == SDMMC_FREQ_SDR104 ? SDMMC_LL_SPEED_MODE_HS : SDMMC_LL_SPEED_MODE_LS;
    switch (delay_phase) {
    case SDMMC_DELAY_PHASE_1:
        phase = SDMMC_LL_DELAY_PHASE_1;
        delay_phase_num = 1;
        break;
    case SDMMC_DELAY_PHASE_2:
        phase = SDMMC_LL_DELAY_PHASE_2;
        delay_phase_num = 2;
        break;
    case SDMMC_DELAY_PHASE_3:
        phase = SDMMC_LL_DELAY_PHASE_3;
        delay_phase_num = 3;
        break;
#if SOC_SDMMC_DELAY_PHASE_NUM > 4
    case SDMMC_DELAY_PHASE_4:
        phase = SDMMC_LL_DELAY_PHASE_4;
        delay_phase_num = 4;
        break;
    case SDMMC_DELAY_PHASE_5:
        phase = SDMMC_LL_DELAY_PHASE_5;
        delay_phase_num = 5;
        break;
    case SDMMC_DELAY_PHASE_6:
        phase = SDMMC_LL_DELAY_PHASE_6;
        delay_phase_num = 6;
        break;
    case SDMMC_DELAY_PHASE_7:
        phase = SDMMC_LL_DELAY_PHASE_7;
        delay_phase_num = 7;
        break;
#endif
    default:
        phase = SDMMC_LL_DELAY_PHASE_0;
        delay_phase_num = 0;
        break;
    }
    PERIPH_RCC_ATOMIC() {
        sdmmc_ll_set_din_delay_phase(slot->ctlr->hal.dev, phase, speed_mode);
        sdmmc_ll_set_dout_delay_phase(slot->ctlr->hal.dev, phase, speed_mode);
    }

    int src_clk_period_ps = (1 * 1000 * 1000) / (clk_src_freq_hz / (1 * 1000 * 1000));
    int delay_phase_total_num = (slot->freq.freq_hz / 1000) == SDMMC_FREQ_SDR104 ? SOC_SDMMC_DELAY_PHASE_NUM : SDMMC_LL_DELAY_MAX_NUMS_LS;
    ESP_LOGD(TAG, "slot->freq.freq_hz: %d, delay_phase_total_num: %d", slot->freq.freq_hz, delay_phase_total_num);
    int phase_diff_ps = src_clk_period_ps * sdmmc_ll_get_clock_div(slot->ctlr->hal.dev) / delay_phase_total_num;
    ESP_LOGD(TAG, "difference between delay phases is %d ps", phase_diff_ps);
    ESP_LOGD(TAG, "host sampling edge is delayed by %d ps", phase_diff_ps * delay_phase_num);

    return ESP_OK;
#else
    //DIG-217
    ESP_LOGD(TAG, "esp32 doesn't support phase delay, fallback to 0 delay");
    return ESP_OK;
#endif
}

esp_err_t sd_host_set_delay_line(sd_host_sdmmc_slot_t *slot)
{
#if SOC_SDMMC_UHS_I_SUPPORTED
    ESP_RETURN_ON_FALSE(slot, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    if ((slot->freq.freq_hz / 1000) == SDMMC_FREQ_SDR104) {
        sdmmc_delay_line_t delay_line = SDMMC_DELAY_LINE_0;
        if (slot->delay_line.delay_line_state == SD_HOST_SLOT_STATE_READY) {
            delay_line = slot->delay_line.delayline;
        }

        sdmmc_ll_delay_line_t line = SDMMC_LL_DELAY_LINE_0;
        switch (delay_line) {
        case SDMMC_DELAY_LINE_1:
            line = SDMMC_LL_DELAY_LINE_1;
            break;
        case SDMMC_DELAY_LINE_2:
            line = SDMMC_LL_DELAY_LINE_2;
            break;
        case SDMMC_DELAY_LINE_3:
            line = SDMMC_LL_DELAY_LINE_3;
            break;
        case SDMMC_DELAY_LINE_4:
            line = SDMMC_LL_DELAY_LINE_4;
            break;
        case SDMMC_DELAY_LINE_5:
            line = SDMMC_LL_DELAY_LINE_5;
            break;
        case SDMMC_DELAY_LINE_6:
            line = SDMMC_LL_DELAY_LINE_6;
            break;
        case SDMMC_DELAY_LINE_7:
            line = SDMMC_LL_DELAY_LINE_7;
            break;
        default:
            line = SDMMC_LL_DELAY_LINE_0;
            break;
        }
        sdmmc_ll_set_din_delay_line(slot->ctlr->hal.dev, line, SDMMC_LL_SPEED_MODE_HS);
        sdmmc_ll_set_dout_delay_line(slot->ctlr->hal.dev, line, SDMMC_LL_SPEED_MODE_HS);
    }

    return ESP_OK;
#else
    ESP_LOGD(TAG, "delay line not supported, fallback to 0 delay line");
    return ESP_OK;
#endif
}

void sd_host_slot_enable_clk_cmd11(sd_host_sdmmc_slot_t *slot, bool enable)
{
    sdmmc_ll_enable_card_clock(slot->ctlr->hal.dev, slot->slot_id, enable);
    sd_host_slot_clock_update_command(slot, true);
    sdmmc_ll_enable_1v8_mode(slot->ctlr->hal.dev, slot->slot_id, enable);
}

/*---------------------------------------------------------------
                Static: Controller Allocation
---------------------------------------------------------------*/
static esp_err_t sd_host_claim_controller(sd_host_sdmmc_ctlr_t *controller)
{
    assert(controller);

    _lock_acquire(&s_platform.mutex);
    bool found = false;
    for (int i = 0; i < SDMMC_LL_HOST_CTLR_NUMS; i ++) {
        found = !s_platform.controllers[i];
        if (found) {
            s_platform.controllers[i] = controller;
            controller->host_id = i;
            PERIPH_RCC_ATOMIC() {
                sdmmc_ll_enable_bus_clock(i, true);
                sdmmc_ll_reset_register(i);
            }
            break;
        }
    }
    _lock_release(&s_platform.mutex);

    if (!found) {
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

static esp_err_t sd_host_declaim_controller(sd_host_sdmmc_ctlr_t *controller)
{
    assert(controller);

    _lock_acquire(&s_platform.mutex);
    s_platform.controllers[controller->host_id] = NULL;
    PERIPH_RCC_ATOMIC() {
        sdmmc_ll_enable_bus_clock(0, false);
    }
    _lock_release(&s_platform.mutex);

    return ESP_OK;
}

/*---------------------------------------------------------------
                    Static: ISR Handler
---------------------------------------------------------------*/
static void sd_host_isr(void *arg)
{
    sd_host_sdmmc_ctlr_t *ctlr = (sd_host_sdmmc_ctlr_t *)arg;
    sd_host_sdmmc_slot_t *slot = ctlr->slot[ctlr->cur_slot_id];

    sd_host_sdmmc_event_t event = {};
    bool need_yield = false;
    int higher_priority_task_awoken = pdFALSE;

    uint32_t pending = sdmmc_ll_get_intr_status(ctlr->hal.dev) & SDMMC_LL_SD_EVENT_MASK;
    sdmmc_ll_clear_interrupt(ctlr->hal.dev, pending);
    event.sdmmc_status = pending;

    uint32_t dma_pending = sdmmc_ll_get_idsts_interrupt_raw(ctlr->hal.dev);
    sdmmc_ll_clear_idsts_interrupt(ctlr->hal.dev, dma_pending);

    if (dma_pending & SDMMC_LL_EVENT_DMA_NI) {
        // refill DMA descriptors
        size_t free_desc = sd_host_get_free_descriptors_count(slot);
        if (free_desc > 0) {
            sd_host_fill_dma_descriptors(slot, free_desc);
            sd_host_dma_resume(slot);
        }
        //NI, logic OR of TI and RI. This is a sticky bit and must be cleared each time TI or RI is cleared.
        dma_pending &= ~(SDMMC_LL_EVENT_DMA_NI | SDMMC_LL_EVENT_DMA_TI | SDMMC_LL_EVENT_DMA_RI);
    }
    event.dma_status = dma_pending & SDMMC_LL_EVENT_DMA_MASK;

    if (pending != 0 || dma_pending != 0) {
        xQueueSendFromISR(ctlr->event_queue, &event, &higher_priority_task_awoken);
        if (slot->cbs.on_trans_done) {
            sd_host_evt_data_t edata = {};
            if (slot->cbs.on_trans_done(&slot->drv, &edata, slot->user_data)) {
                need_yield |= true;
            }
        }
    }

    uint32_t sdio_pending = (sdmmc_ll_get_intr_status(ctlr->hal.dev) & (SDMMC_INTMASK_IO_SLOT1 | SDMMC_INTMASK_IO_SLOT0));
    if (sdio_pending) {
        // disable the interrupt (no need to clear here, this is done in sdmmc_host_io_int_wait)
        sdmmc_ll_enable_interrupt(ctlr->hal.dev, sdio_pending, false);
        xSemaphoreGiveFromISR(ctlr->io_intr_sem, &higher_priority_task_awoken);
        if (slot->cbs.on_io_interrupt) {
            sd_host_evt_data_t edata = {};
            if (slot->cbs.on_io_interrupt(&slot->drv, &edata, slot->user_data)) {
                need_yield |= true;
            }
        }
    }

    need_yield |= higher_priority_task_awoken == pdTRUE;
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

/*---------------------------------------------------------------
                    HW Commands
---------------------------------------------------------------*/
esp_err_t sd_host_slot_start_command(sd_host_sdmmc_slot_t *slot, sdmmc_hw_cmd_t cmd, uint32_t arg)
{
    ESP_RETURN_ON_FALSE(slot, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    // if this isn't a clock update command, check the card detect status
    if (!sdmmc_ll_is_card_detected(slot->ctlr->hal.dev, slot->slot_id) && !cmd.update_clk_reg) {
        return ESP_ERR_NOT_FOUND;
    }
    if (cmd.data_expected && cmd.rw && sdmmc_ll_is_card_write_protected(slot->ctlr->hal.dev, slot->slot_id)) {
        return ESP_ERR_INVALID_STATE;
    }
    /* Outputs should be synchronized to cclk_out */
    cmd.use_hold_reg = 1;

    int64_t yield_delay_us = 100 * 1000; // initially 100ms
    int64_t t0 = esp_timer_get_time();
    int64_t t1 = 0;
    bool skip_wait = (cmd.volt_switch && cmd.update_clk_reg);
    if (!skip_wait) {
        while (!(sdmmc_ll_is_command_taken(slot->ctlr->hal.dev))) {
            t1 = esp_timer_get_time();
            if (t1 - t0 > SD_HOST_SDMMC_START_CMD_TIMEOUT_US) {
                return ESP_ERR_TIMEOUT;
            }
            if (t1 - t0 > yield_delay_us) {
                yield_delay_us *= 2;
                vTaskDelay(1);
            }
        }
    }
    sdmmc_ll_set_command_arg(slot->ctlr->hal.dev, arg);
    cmd.card_num = slot->slot_id;
    cmd.start_command = 1;
    sdmmc_ll_set_command(slot->ctlr->hal.dev, cmd);

    while (!(sdmmc_ll_is_command_taken(slot->ctlr->hal.dev))) {
        t1 = esp_timer_get_time();
        if (t1 - t0 > SD_HOST_SDMMC_START_CMD_TIMEOUT_US) {
            return ESP_ERR_TIMEOUT;
        }
        if (t1 - t0 > yield_delay_us) {
            yield_delay_us *= 2;
            vTaskDelay(1);
        }
    }
    return ESP_OK;
}

static esp_err_t sd_host_slot_clock_update_command(sd_host_sdmmc_slot_t *slot, bool is_cmd11)
{
    assert(slot);

    // Clock update command (not a real command; just updates CIU registers)
    sdmmc_hw_cmd_t cmd_val = {
        .card_num = slot->slot_id,
        .update_clk_reg = 1,
        .wait_complete = 1
    };
    if (is_cmd11) {
        cmd_val.volt_switch = 1;
    }
    ESP_RETURN_ON_ERROR(sd_host_slot_start_command(slot, cmd_val, 0), TAG, "sd_host_start_command returned 0x%x", err_rc_);

    return ESP_OK;
}

/*---------------------------------------------------------------
                    Static: Controls
---------------------------------------------------------------*/
static void s_module_reset(sd_host_sdmmc_ctlr_t *ctlr)
{
    // reset module
    sdmmc_ll_reset_controller(ctlr->hal.dev);
    sdmmc_ll_reset_dma(ctlr->hal.dev);
    sdmmc_ll_reset_fifo(ctlr->hal.dev);
}

static bool s_is_module_reset_done(sd_host_sdmmc_ctlr_t *ctlr)
{
    bool is_done = sdmmc_ll_is_controller_reset_done(ctlr->hal.dev) && sdmmc_ll_is_dma_reset_done(ctlr->hal.dev) && sdmmc_ll_is_fifo_reset_done(ctlr->hal.dev);
    return is_done;
}

static esp_err_t sd_host_reset(sd_host_sdmmc_ctlr_t *ctlr)
{
    s_module_reset(ctlr);

    // Wait for the reset bits to be cleared by hardware
    int64_t yield_delay_us = 100 * 1000; // initially 100ms
    int64_t t0 = esp_timer_get_time();
    int64_t t1 = 0;
    while (!s_is_module_reset_done(ctlr)) {
        t1 = esp_timer_get_time();
        if (t1 - t0 > SD_HOST_SDMMC_RESET_TIMEOUT_US) {
            return ESP_ERR_TIMEOUT;
        }
        if (t1 - t0 > yield_delay_us) {
            yield_delay_us *= 2;
            vTaskDelay(1);
        }
    }

    return ESP_OK;
}

/*---------------------------------------------------------------
                    Static: Clock
---------------------------------------------------------------*/
/* We have two clock divider stages:
 * - one is the clock generator which drives SDMMC peripheral,
 *   it can be configured using SDMMC.clock register. It can generate
 *   frequencies 160MHz/(N + 1), where 0 < N < 16, I.e. from 10 to 80 MHz.
 * - 4 clock dividers inside SDMMC peripheral, which can divide clock
 *   from the first stage by 2 * M, where 0 < M < 255
 *   (they can also be bypassed).
 *
 * For cards which aren't UHS-1 or UHS-2 cards, which we don't support,
 * maximum bus frequency in high speed (HS) mode is 50 MHz.
 * Note: for non-UHS-1 cards, HS mode is optional.
 * Default speed (DS) mode is mandatory, it works up to 25 MHz.
 * Whether the card supports HS or not can be determined using TRAN_SPEED
 * field of card's CSD register.
 *
 * 50 MHz can not be obtained exactly, closest we can get is 53 MHz.
 *
 * The first stage divider is set to the highest possible value for the given
 * frequency, and the the second stage dividers are used if division factor
 * is >16.
 *
 * Of the second stage dividers, div0 is used for card 0, and div1 is used
 * for card 1.
 */
static void sd_host_set_clk_div(sd_host_sdmmc_ctlr_t *ctlr, soc_periph_sdmmc_clk_src_t src, int div)
{
    ESP_ERROR_CHECK(esp_clk_tree_enable_src((soc_module_clk_t)src, true));
    PERIPH_RCC_ATOMIC() {
        sdmmc_ll_set_clock_div(ctlr->hal.dev, div);
        sdmmc_ll_select_clk_source(ctlr->hal.dev, src);
        sdmmc_ll_init_phase_delay(ctlr->hal.dev);
#if SDMMC_LL_SDIO_PLL_SUPPORTED
        if (src == SDMMC_CLK_SRC_SDIO_200M) {
            sdmmc_ll_enable_sdio_pll(ctlr->hal.dev, true);
        }
#endif
    }

    // Wait for the clock to propagate
    esp_rom_delay_us(10);
}

static void sd_host_slot_get_clk_dividers(sd_host_sdmmc_slot_t *slot, uint32_t freq_khz, int *host_div, int *card_div, soc_periph_sdmmc_clk_src_t *src)
{
    uint32_t clk_src_freq_hz = 0;
    soc_periph_sdmmc_clk_src_t clk_src = 0;
#if SOC_SDMMC_UHS_I_SUPPORTED
    if (freq_khz > SDMMC_FREQ_HIGHSPEED) {
        clk_src = SDMMC_CLK_SRC_SDIO_200M;
    } else
#endif
    {
        clk_src = SDMMC_CLK_SRC_DEFAULT;
    }
    slot->clk_src = clk_src;

    esp_err_t ret = esp_clk_tree_src_get_freq_hz(clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_freq_hz);
    assert(ret == ESP_OK);
    (void)ret;
    ESP_LOGD(TAG, "clk_src_freq_hz: %"PRId32" hz", clk_src_freq_hz);

#if SDMMC_LL_MAX_FREQ_KHZ_FPGA
    if (freq_khz >= SDMMC_LL_MAX_FREQ_KHZ_FPGA) {
        ESP_LOGW(TAG, "working on FPGA, fallback to use the %d KHz", SDMMC_LL_MAX_FREQ_KHZ_FPGA);
        freq_khz = SDMMC_LL_MAX_FREQ_KHZ_FPGA;
    }
#endif

    // Calculate new dividers
#if SOC_SDMMC_UHS_I_SUPPORTED
    if (freq_khz == SDMMC_FREQ_SDR104) {
        *host_div = 1;       // 200 MHz / 1 = 200 MHz
        *card_div = 0;
    } else if (freq_khz == SDMMC_FREQ_SDR50) {
        *host_div = 2;       // 200 MHz / 2 = 100 MHz
        *card_div = 0;
    } else
#endif
        if (freq_khz >= SDMMC_FREQ_HIGHSPEED) {
            *host_div = 4;       // 160 MHz / 4 = 40 MHz
            *card_div = 0;
        } else if (freq_khz == SDMMC_FREQ_DEFAULT) {
            *host_div = 8;       // 160 MHz / 8 = 20 MHz
            *card_div = 0;
        } else if (freq_khz == SDMMC_FREQ_PROBING) {
            *host_div = 10;      // 160 MHz / 10 / (20 * 2) = 400 kHz
            *card_div = 20;
        } else {
            /*
             * for custom frequencies use maximum range of host divider (1-16), find the closest <= div. combination
             * if exceeded, combine with the card divider to keep reasonable precision (applies mainly to low frequencies)
             * effective frequency range: 400 kHz - 32 MHz (32.1 - 39.9 MHz cannot be covered with given divider scheme)
             */
            *host_div = (clk_src_freq_hz) / (freq_khz * 1000);
            if (*host_div > 15) {
                *host_div = 2;
                *card_div = (clk_src_freq_hz / 2) / (2 * freq_khz * 1000);
                if (((clk_src_freq_hz / 2) % (2 * freq_khz * 1000)) > 0) {
                    (*card_div)++;
                }
            } else if ((clk_src_freq_hz % (freq_khz * 1000)) > 0) {
                (*host_div)++;
            }
        }

    *src = clk_src;
}

static int sd_host_calc_freq(soc_periph_sdmmc_clk_src_t src, const int host_div, const int card_div)
{
    uint32_t clk_src_freq_hz = 0;
    esp_err_t ret = esp_clk_tree_src_get_freq_hz(src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_freq_hz);
    assert(ret == ESP_OK);
    (void)ret;

    return clk_src_freq_hz / host_div / ((card_div == 0) ? 1 : card_div * 2) / 1000;
}

/*---------------------------------------------------------------
                    Static: IO
---------------------------------------------------------------*/
static void configure_pin_iomux(uint8_t gpio_num)
{
    assert(gpio_num != (uint8_t) GPIO_NUM_NC);
    gpio_pulldown_dis(gpio_num);
    gpio_input_enable(gpio_num);
    gpio_iomux_output(gpio_num, SDMMC_LL_IOMUX_FUNC);
#if !CONFIG_IDF_TARGET_ESP32
    /**
     * On ESP32, the default pin drive value (2) works
     */
    gpio_set_drive_capability(gpio_num, 3);
#endif
}

static void configure_pin_gpio_matrix(uint8_t gpio_num, uint8_t gpio_matrix_sig, gpio_mode_t mode, const char *name)
{
    assert(gpio_num != (uint8_t) GPIO_NUM_NC);
    ESP_LOGD(TAG, "using GPIO%d as %s pin", gpio_num, name);
    //todo: IDF-11125
    gpio_reset_pin(gpio_num);
    gpio_set_direction(gpio_num, mode);
    gpio_pulldown_dis(gpio_num);
    if (mode == GPIO_MODE_INPUT || mode == GPIO_MODE_INPUT_OUTPUT) {
        esp_rom_gpio_connect_in_signal(gpio_num, gpio_matrix_sig, false);
    }
    if (mode == GPIO_MODE_OUTPUT || mode == GPIO_MODE_INPUT_OUTPUT) {
        esp_rom_gpio_connect_out_signal(gpio_num, gpio_matrix_sig, false, false);
    }
}

static void configure_pin(uint8_t gpio_num, uint8_t gpio_matrix_sig, gpio_mode_t mode, const char *name, bool use_gpio_matrix)
{
    if (use_gpio_matrix) {
        configure_pin_gpio_matrix(gpio_num, gpio_matrix_sig, mode, name);
    } else {
        configure_pin_iomux(gpio_num);
    }
}

//True: pins are all not set; False: one or more pins are set
static bool s_check_pin_not_set(const sd_host_sdmmc_slot_io_cfg_t *io_config)
{
#if SOC_SDMMC_USE_GPIO_MATRIX
    bool pin_not_set = !io_config->clk_io && !io_config->cmd_io && !io_config->d0_io && !io_config->d1_io && !io_config->d2_io &&
                       !io_config->d3_io && !io_config->d4_io && !io_config->d5_io && !io_config->d6_io && !io_config->d7_io;
    return pin_not_set;
#else
    return true;
#endif
}

static esp_err_t sdmmc_host_pullup_en_internal(sd_host_sdmmc_slot_t *slot)
{
    if (slot->width.width > sdmmc_slot_info[slot->slot_id].width) {
        //in esp32 we only support 8 bit in slot 0, note this is occupied by the flash by default
        return ESP_ERR_INVALID_ARG;
    }
    // according to the spec, the host controls the clk, we don't to pull it up here
    gpio_pullup_en(slot->io_config.cmd_io);
    gpio_pullup_en(slot->io_config.d0_io);
    if (slot->width.width >= 4) {
        gpio_pullup_en(slot->io_config.d1_io);
        gpio_pullup_en(slot->io_config.d2_io);
        gpio_pullup_en(slot->io_config.d3_io);
    }
    if (slot->width.width == 8) {
        gpio_pullup_en(slot->io_config.d4_io);
        gpio_pullup_en(slot->io_config.d5_io);
        gpio_pullup_en(slot->io_config.d6_io);
        gpio_pullup_en(slot->io_config.d7_io);
    }
    return ESP_OK;
}

static esp_err_t sdmmc_slot_io_config(sd_host_sdmmc_slot_t *slot, const sd_host_slot_sdmmc_init_cfg_t *slot_init_config)
{
    bool gpio_wp_polarity = slot_init_config->slot_flags.wp_active_high;
    sd_bus_width_t slot_width = slot_init_config->io_config.width;
    int slot_id = slot_init_config->slot_id;

    const sdmmc_slot_info_t *slot_info = &sdmmc_slot_info[slot_id];
    ESP_RETURN_ON_FALSE(slot_width <= slot_info->width, ESP_ERR_INVALID_ARG, TAG, "wrong slot width");
    if (slot_width == SD_HOST_SLOT_WIDTH_DEFAULT) {
        slot_width = slot_info->width;
    }

    const sd_host_sdmmc_slot_io_cfg_t *io_config = &slot_init_config->io_config;
    sd_host_sdmmc_slot_io_cfg_t *slot_gpio = &slot->io_config;

    slot->io_config.clk_io = GPIO_NUM_NC;
    slot->io_config.cmd_io = GPIO_NUM_NC;
    slot->io_config.cd_io = GPIO_NUM_NC;
    slot->io_config.wp_io = GPIO_NUM_NC;
    slot->io_config.d0_io = GPIO_NUM_NC;
    slot->io_config.d1_io = GPIO_NUM_NC;
    slot->io_config.d2_io = GPIO_NUM_NC;
    slot->io_config.d3_io = GPIO_NUM_NC;
    slot->io_config.d4_io = GPIO_NUM_NC;
    slot->io_config.d5_io = GPIO_NUM_NC;
    slot->io_config.d6_io = GPIO_NUM_NC;
    slot->io_config.d7_io = GPIO_NUM_NC;
    slot->width.width = slot_width;
    slot->width.width_state = SD_HOST_SLOT_STATE_INIT;
    slot_gpio->cd_io = io_config->cd_io;
    slot_gpio->wp_io = io_config->wp_io;

    bool pin_not_set = s_check_pin_not_set(io_config);
    //SD driver behaviour is: all pins not defined == using iomux
    bool use_gpio_matrix = !pin_not_set;

    if (slot_id == 0) {
#if !SDMMC_LL_SLOT_SUPPORT_GPIO_MATRIX(0)
        if (use_gpio_matrix) {
            if (slot_width >= 1) {
                if (SDMMC_SLOT0_IOMUX_PIN_NUM_CLK == io_config->clk_io && SDMMC_SLOT0_IOMUX_PIN_NUM_CMD == io_config->cmd_io && SDMMC_SLOT0_IOMUX_PIN_NUM_D0 == io_config->d0_io) {
                    use_gpio_matrix = false;
                } else {
                    use_gpio_matrix = true;
                }
            }
            if (slot_width >= 4) {
                if (SDMMC_SLOT0_IOMUX_PIN_NUM_D1 == io_config->d1_io && SDMMC_SLOT0_IOMUX_PIN_NUM_D2 == io_config->d2_io && SDMMC_SLOT0_IOMUX_PIN_NUM_D3 == io_config->d3_io) {
                    use_gpio_matrix = false;
                } else {
                    use_gpio_matrix = true;
                }
            }
        } else {
            ESP_RETURN_ON_FALSE(!use_gpio_matrix, ESP_ERR_INVALID_ARG, TAG, "doesn't support routing from GPIO matrix, driver uses dedicated IOs");
        }
#endif
    } else {
#if !SDMMC_LL_SLOT_SUPPORT_GPIO_MATRIX(1)
        ESP_RETURN_ON_FALSE(!use_gpio_matrix, ESP_ERR_INVALID_ARG, TAG, "doesn't support routing from GPIO matrix, driver uses dedicated IOs");
#endif
    }
    slot->use_gpio_matrix = use_gpio_matrix;

#if SOC_SDMMC_USE_GPIO_MATRIX
    if (use_gpio_matrix) {
        /* Save pin configuration for this slot */
        slot_gpio->clk_io = io_config->clk_io;
        slot_gpio->cmd_io = io_config->cmd_io;
        slot_gpio->d0_io = io_config->d0_io;
        /* Save d1 even in 1-line mode, it might be needed for SDIO INT line */
        slot_gpio->d1_io = io_config->d1_io;
        if (slot_width >= 4) {
            slot_gpio->d2_io = io_config->d2_io;
        }
        /* Save d3 even for 1-line mode, as it needs to be set high */
        slot_gpio->d3_io = io_config->d3_io;
        if (slot_width >= 8) {
            slot_gpio->d4_io = io_config->d4_io;
            slot_gpio->d5_io = io_config->d5_io;
            slot_gpio->d6_io = io_config->d6_io;
            slot_gpio->d7_io = io_config->d7_io;
        }
    } else
#endif  //#if SOC_SDMMC_USE_GPIO_MATRIX
    {
        /* init pin configuration for this slot */
        slot_gpio->clk_io = sdmmc_slot_gpio_num[slot_id].clk;
        slot_gpio->cmd_io = sdmmc_slot_gpio_num[slot_id].cmd;
        slot_gpio->d0_io = sdmmc_slot_gpio_num[slot_id].d0;
        slot_gpio->d1_io = sdmmc_slot_gpio_num[slot_id].d1;
        slot_gpio->d2_io = sdmmc_slot_gpio_num[slot_id].d2;
        slot_gpio->d3_io = sdmmc_slot_gpio_num[slot_id].d3;
        slot_gpio->d4_io = sdmmc_slot_gpio_num[slot_id].d4;
        slot_gpio->d5_io = sdmmc_slot_gpio_num[slot_id].d5;
        slot_gpio->d6_io = sdmmc_slot_gpio_num[slot_id].d6;
        slot_gpio->d7_io = sdmmc_slot_gpio_num[slot_id].d7;
    }

    bool pullup = slot_init_config->slot_flags.internal_pullup;
    if (pullup) {
        sdmmc_host_pullup_en_internal(slot);
    }

    if (slot_width >= 1) {
        GPIO_NUM_CHECK(slot_gpio->clk_io);
        GPIO_NUM_CHECK(slot_gpio->cmd_io);
        GPIO_NUM_CHECK(slot_gpio->d0_io);
    }
    if (slot_width >= 4) {
        GPIO_NUM_CHECK(slot_gpio->d1_io);
        GPIO_NUM_CHECK(slot_gpio->d2_io);
        GPIO_NUM_CHECK(slot_gpio->d3_io);
    }
    if (slot_width == 8) {
        GPIO_NUM_CHECK(slot_gpio->d4_io);
        GPIO_NUM_CHECK(slot_gpio->d5_io);
        GPIO_NUM_CHECK(slot_gpio->d6_io);
        GPIO_NUM_CHECK(slot_gpio->d7_io);
    }

    configure_pin(slot_gpio->clk_io, sdmmc_slot_gpio_sig[slot_id].clk, GPIO_MODE_OUTPUT, "clk", use_gpio_matrix);
    configure_pin(slot_gpio->cmd_io, sdmmc_slot_gpio_sig[slot_id].cmd, GPIO_MODE_INPUT_OUTPUT, "cmd", use_gpio_matrix);
    configure_pin(slot_gpio->d0_io, sdmmc_slot_gpio_sig[slot_id].d0, GPIO_MODE_INPUT_OUTPUT, "d0", use_gpio_matrix);

    if (slot_width >= 4) {
        configure_pin(slot_gpio->d1_io, sdmmc_slot_gpio_sig[slot_id].d1, GPIO_MODE_INPUT_OUTPUT, "d1", use_gpio_matrix);
        configure_pin(slot_gpio->d2_io, sdmmc_slot_gpio_sig[slot_id].d2, GPIO_MODE_INPUT_OUTPUT, "d2", use_gpio_matrix);
        if (slot->sd_mode == SD_MODE_UHS1) {
            configure_pin(slot_gpio->d3_io, sdmmc_slot_gpio_sig[slot_id].d3, GPIO_MODE_INPUT_OUTPUT, "d3", use_gpio_matrix);
        } else {
            // Force D3 high to make slave enter SD mode.
            // Connect to peripheral after width configuration.
            if (slot_gpio->d3_io > GPIO_NUM_NC) {
                gpio_config_t gpio_conf = {
                    .pin_bit_mask = BIT64(slot_gpio->d3_io),
                    .mode = GPIO_MODE_OUTPUT,
                    .pull_up_en = GPIO_PULLUP_DISABLE,
                    .pull_down_en = GPIO_PULLDOWN_DISABLE,
                    .intr_type = GPIO_INTR_DISABLE,
                };
                gpio_config(&gpio_conf);
                gpio_set_level(slot_gpio->d3_io, 1);
            }
        }
    }
    if (slot_width == 8) {
        configure_pin(slot_gpio->d4_io, sdmmc_slot_gpio_sig[slot_id].d4, GPIO_MODE_INPUT_OUTPUT, "d4", use_gpio_matrix);
        configure_pin(slot_gpio->d5_io, sdmmc_slot_gpio_sig[slot_id].d5, GPIO_MODE_INPUT_OUTPUT, "d5", use_gpio_matrix);
        configure_pin(slot_gpio->d6_io, sdmmc_slot_gpio_sig[slot_id].d6, GPIO_MODE_INPUT_OUTPUT, "d6", use_gpio_matrix);
        configure_pin(slot_gpio->d7_io, sdmmc_slot_gpio_sig[slot_id].d7, GPIO_MODE_INPUT_OUTPUT, "d7", use_gpio_matrix);
    }

    // SDIO slave interrupt is edge sensitive to ~(int_n | card_int | card_detect)
    // set this and card_detect to high to enable sdio interrupt
    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, slot_info->card_int, false);

    // Set up Card Detect input
    int matrix_in_cd = 0;
    if (io_config->cd_io != GPIO_NUM_NC) {
        ESP_LOGD(TAG, "using GPIO%d as CD pin", io_config->cd_io);
        esp_rom_gpio_pad_select_gpio(io_config->cd_io);
        gpio_set_direction(io_config->cd_io, GPIO_MODE_INPUT);
        matrix_in_cd = io_config->cd_io;
    } else {
        // if not set, default to CD low (card present)
        matrix_in_cd = GPIO_MATRIX_CONST_ZERO_INPUT;
    }
    esp_rom_gpio_connect_in_signal(matrix_in_cd, slot_info->card_detect, false);

    // Set up Write Protect input
    int matrix_in_wp;
    if (io_config->wp_io != GPIO_NUM_NC) {
        ESP_LOGD(TAG, "using GPIO%d as WP pin", io_config->wp_io);
        esp_rom_gpio_pad_select_gpio(io_config->wp_io);
        gpio_set_direction(io_config->wp_io, GPIO_MODE_INPUT);
        matrix_in_wp = io_config->wp_io;
    } else {
        // if not set, default to WP high (not write protected)
        matrix_in_wp = GPIO_MATRIX_CONST_ONE_INPUT;
    }
    // As hardware expects an active-high signal,
    // if WP signal is active low, then invert it in GPIO matrix,
    // else keep it in its default state
    esp_rom_gpio_connect_in_signal(matrix_in_wp, slot_info->write_protect, (gpio_wp_polarity ? false : true));

    return ESP_OK;
}
