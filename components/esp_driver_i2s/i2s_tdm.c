/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"

#if CONFIG_I2S_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif

#include "hal/i2s_hal.h"
#include "driver/gpio.h"
#include "driver/i2s_tdm.h"
#include "i2s_private.h"
#include "clk_ctrl_os.h"
#include "esp_intr_alloc.h"
#include "esp_check.h"

const static char *TAG = "i2s_tdm";

// Same with standard mode except total slot number
static esp_err_t i2s_tdm_calculate_clock(i2s_chan_handle_t handle, const i2s_tdm_clk_config_t *clk_cfg, i2s_hal_clock_info_t *clk_info)
{
    uint32_t rate = clk_cfg->sample_rate_hz;
    i2s_tdm_slot_config_t *slot_cfg = &((i2s_tdm_config_t *)(handle->mode_info))->slot_cfg;
    uint32_t slot_bits = (slot_cfg->slot_bit_width == I2S_SLOT_BIT_WIDTH_AUTO) ||
                         ((int)slot_cfg->slot_bit_width < (int)slot_cfg->data_bit_width) ?
                         slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    /* Calculate multiple
     * Fmclk = bck_div*fbck = fsclk/(mclk_div+b/a) */
    if (handle->role == I2S_ROLE_MASTER) {
        clk_info->bclk = rate * handle->total_slot * slot_bits;
        clk_info->mclk = rate * clk_cfg->mclk_multiple;
        clk_info->bclk_div = clk_info->mclk / clk_info->bclk;
        /* While RECEIVING multiple slots, the data will go wrong if the bclk_div is equal or smaller than 2 */
        if (clk_info->bclk_div <= 2) {
            clk_info->bclk_div = 3;
            clk_info->mclk = clk_info->bclk * clk_info->bclk_div;
            ESP_LOGW(TAG, "the current mclk multiple is too small, adjust the mclk multiple to %"PRIu32, clk_info->mclk / rate);
        }
        if (clk_info->mclk % clk_info->bclk != 0) {
            ESP_LOGW(TAG, "the current mclk multiple cannot perform integer division (slot_num: %"PRIu32", slot_bits: %"PRIu32")", handle->total_slot, slot_bits);
        }
    } else {
        if (clk_cfg->bclk_div < 8) {
            ESP_LOGW(TAG, "the current bclk division is too small, adjust the bclk division to 8");
            clk_info->bclk_div = 8;
        } else {
            clk_info->bclk_div = clk_cfg->bclk_div;
        }
        clk_info->bclk = rate * handle->total_slot * slot_bits;
        clk_info->mclk = clk_info->bclk * clk_info->bclk_div;
    }
    clk_info->sclk = clk_cfg->clk_src == I2S_CLK_SRC_EXTERNAL ?
                     clk_cfg->ext_clk_freq_hz : i2s_get_source_clk_freq(clk_cfg->clk_src, clk_info->mclk);
    clk_info->mclk_div = clk_info->sclk / clk_info->mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(clk_info->mclk_div, ESP_ERR_INVALID_ARG, TAG, "sample rate is too large for the current clock source");

    return ESP_OK;
}

static esp_err_t i2s_tdm_set_clock(i2s_chan_handle_t handle, const i2s_tdm_clk_config_t *clk_cfg)
{
    esp_err_t ret = ESP_OK;
    i2s_tdm_config_t *tdm_cfg = (i2s_tdm_config_t *)(handle->mode_info);

    i2s_hal_clock_info_t clk_info;
    /* Calculate clock parameters */
    ESP_RETURN_ON_ERROR(i2s_tdm_calculate_clock(handle, clk_cfg, &clk_info), TAG, "clock calculate failed");
    ESP_LOGD(TAG, "Clock division info: [sclk] %"PRIu32" Hz [mdiv] %d [mclk] %"PRIu32" Hz [bdiv] %d [bclk] %"PRIu32" Hz",
             clk_info.sclk, clk_info.mclk_div, clk_info.mclk, clk_info.bclk_div, clk_info.bclk);

    portENTER_CRITICAL(&g_i2s.spinlock);
    /* Set clock configurations in HAL*/
    I2S_CLOCK_SRC_ATOMIC() {
        if (handle->dir == I2S_DIR_TX) {
            i2s_hal_set_tx_clock(&handle->controller->hal, &clk_info, clk_cfg->clk_src);
        } else {
            i2s_hal_set_rx_clock(&handle->controller->hal, &clk_info, clk_cfg->clk_src);
        }
    }
    portEXIT_CRITICAL(&g_i2s.spinlock);

    /* Update the mode info: clock configuration */
    memcpy(&(tdm_cfg->clk_cfg), clk_cfg, sizeof(i2s_tdm_clk_config_t));

    return ret;
}

static esp_err_t i2s_tdm_set_slot(i2s_chan_handle_t handle, const i2s_tdm_slot_config_t *slot_cfg)
{
    ESP_RETURN_ON_FALSE(slot_cfg->slot_mask, ESP_ERR_INVALID_ARG, TAG, "At least one channel should be enabled");
    /* Update the total slot num and active slot num */
    handle->active_slot = slot_cfg->slot_mode == I2S_SLOT_MODE_MONO ? 1 : __builtin_popcount(slot_cfg->slot_mask);
    uint32_t max_slot_num = 32 - __builtin_clz(slot_cfg->slot_mask);
    handle->total_slot = slot_cfg->total_slot < max_slot_num ? max_slot_num : slot_cfg->total_slot;
    handle->total_slot = handle->total_slot < 2 ? 2 : handle->total_slot;  // At least two slots in a frame

    uint32_t buf_size = i2s_get_buf_size(handle, slot_cfg->data_bit_width, handle->dma.frame_num);
    /* The DMA buffer need to re-allocate if the buffer size changed */
    if (handle->dma.buf_size != buf_size) {
        handle->dma.buf_size = buf_size;
        ESP_RETURN_ON_ERROR(i2s_free_dma_desc(handle), TAG, "failed to free the old dma descriptor");
        ESP_RETURN_ON_ERROR(i2s_alloc_dma_desc(handle, handle->dma.desc_num, buf_size),
                            TAG, "allocate memory for dma descriptor failed");
    }
    bool is_slave = handle->role == I2S_ROLE_SLAVE;
    /* Share bck and ws signal in full-duplex mode */
    if (handle->controller->full_duplex) {
        i2s_ll_share_bck_ws(handle->controller->hal.dev, true);
        /* Since bck and ws are shared, only tx or rx can be master
           Force to set rx as slave to avoid conflict of clock signal */
        if (handle->dir == I2S_DIR_RX) {
            is_slave = true;
        }
    } else {
        i2s_ll_share_bck_ws(handle->controller->hal.dev, false);
    }

    portENTER_CRITICAL(&g_i2s.spinlock);
    /* Configure the hardware to apply TDM format */
    if (handle->dir == I2S_DIR_TX) {
        i2s_hal_tdm_set_tx_slot(&(handle->controller->hal), is_slave, (i2s_hal_slot_config_t *)slot_cfg);
    } else {
        i2s_hal_tdm_set_rx_slot(&(handle->controller->hal), is_slave, (i2s_hal_slot_config_t *)slot_cfg);
    }
    portEXIT_CRITICAL(&g_i2s.spinlock);

    /* Update the mode info: slot configuration */
    i2s_tdm_config_t *tdm_cfg = (i2s_tdm_config_t *)(handle->mode_info);
    memcpy(&(tdm_cfg->slot_cfg), slot_cfg, sizeof(i2s_tdm_slot_config_t));

    return ESP_OK;
}

static esp_err_t i2s_tdm_set_gpio(i2s_chan_handle_t handle, const i2s_tdm_gpio_config_t *gpio_cfg)
{
    int id = handle->controller->id;

    /* Check validity of selected pins */
    ESP_RETURN_ON_FALSE((gpio_cfg->bclk == -1 || GPIO_IS_VALID_GPIO(gpio_cfg->bclk)),
                        ESP_ERR_INVALID_ARG, TAG, "bclk invalid");
    ESP_RETURN_ON_FALSE((gpio_cfg->ws   == -1 || GPIO_IS_VALID_GPIO(gpio_cfg->ws)),
                        ESP_ERR_INVALID_ARG, TAG, "ws invalid");
    i2s_tdm_config_t *tdm_cfg = (i2s_tdm_config_t *)(handle->mode_info);
    /* Loopback if dout = din */
    if (gpio_cfg->dout != -1 &&
            gpio_cfg->dout == gpio_cfg->din) {
        i2s_gpio_loopback_set(handle, gpio_cfg->dout, i2s_periph_signal[id].data_out_sig, i2s_periph_signal[id].data_in_sig);
    } else if (handle->dir == I2S_DIR_TX) {
        /* Set data output GPIO */
        i2s_gpio_check_and_set(handle, gpio_cfg->dout, i2s_periph_signal[id].data_out_sig, false, false);
    } else {
        /* Set data input GPIO */
        i2s_gpio_check_and_set(handle, gpio_cfg->din, i2s_periph_signal[id].data_in_sig, true, false);
    }

    /* Set mclk pin */
    ESP_RETURN_ON_ERROR(i2s_check_set_mclk(handle, id, gpio_cfg->mclk, tdm_cfg->clk_cfg.clk_src, gpio_cfg->invert_flags.mclk_inv), TAG, "mclk config failed");

    if (handle->role == I2S_ROLE_SLAVE) {
        /* For "tx + slave" mode, select TX signal index for ws and bck */
        if (handle->dir == I2S_DIR_TX && !handle->controller->full_duplex) {
#if SOC_I2S_HW_VERSION_2
            I2S_CLOCK_SRC_ATOMIC() {
                i2s_ll_mclk_bind_to_tx_clk(handle->controller->hal.dev);
            }
#endif
            i2s_gpio_check_and_set(handle, gpio_cfg->ws, i2s_periph_signal[id].s_tx_ws_sig, true, gpio_cfg->invert_flags.ws_inv);
            i2s_gpio_check_and_set(handle, gpio_cfg->bclk, i2s_periph_signal[id].s_tx_bck_sig, true, gpio_cfg->invert_flags.bclk_inv);
            /* For "tx + rx + slave" or "rx + slave" mode, select RX signal index for ws and bck */
        } else {
            i2s_gpio_check_and_set(handle, gpio_cfg->ws, i2s_periph_signal[id].s_rx_ws_sig, true, gpio_cfg->invert_flags.ws_inv);
            i2s_gpio_check_and_set(handle, gpio_cfg->bclk, i2s_periph_signal[id].s_rx_bck_sig, true, gpio_cfg->invert_flags.bclk_inv);
        }
    } else {
        /* For "rx + master" mode, select RX signal index for ws and bck */
        if (handle->dir == I2S_DIR_RX && !handle->controller->full_duplex) {
#if SOC_I2S_HW_VERSION_2
            I2S_CLOCK_SRC_ATOMIC() {
                i2s_ll_mclk_bind_to_rx_clk(handle->controller->hal.dev);
            }
#endif
            i2s_gpio_check_and_set(handle, gpio_cfg->ws, i2s_periph_signal[id].m_rx_ws_sig, false, gpio_cfg->invert_flags.ws_inv);
            i2s_gpio_check_and_set(handle, gpio_cfg->bclk, i2s_periph_signal[id].m_rx_bck_sig, false, gpio_cfg->invert_flags.bclk_inv);
            /* For "tx + rx + master" or "tx + master" mode, select TX signal index for ws and bck */
        } else {
            i2s_gpio_check_and_set(handle, gpio_cfg->ws, i2s_periph_signal[id].m_tx_ws_sig, false, gpio_cfg->invert_flags.ws_inv);
            i2s_gpio_check_and_set(handle, gpio_cfg->bclk, i2s_periph_signal[id].m_tx_bck_sig, false, gpio_cfg->invert_flags.bclk_inv);
        }
    }
    /* Update the mode info: gpio configuration */
    memcpy(&(tdm_cfg->gpio_cfg), gpio_cfg, sizeof(i2s_tdm_gpio_config_t));

    return ESP_OK;
}

esp_err_t i2s_channel_init_tdm_mode(i2s_chan_handle_t handle, const i2s_tdm_config_t *tdm_cfg)
{
#if CONFIG_I2S_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    I2S_NULL_POINTER_CHECK(TAG, handle);
    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_REGISTER, ESP_ERR_INVALID_STATE, err, TAG, "the channel has initialized already");
    handle->mode = I2S_COMM_MODE_TDM;
    /* Allocate memory for storing the configurations of TDM mode */
    if (handle->mode_info) {
        free(handle->mode_info);
    }
    handle->mode_info = calloc(1, sizeof(i2s_tdm_config_t));
    ESP_GOTO_ON_FALSE(handle->mode_info, ESP_ERR_NO_MEM, err, TAG, "no memory for storing the configurations");
    /* i2s_set_tdm_slot should be called before i2s_set_tdm_clock while initializing, because clock is relay on the slot */
    ESP_GOTO_ON_ERROR(i2s_tdm_set_slot(handle, &tdm_cfg->slot_cfg), err, TAG, "initialize channel failed while setting slot");
#if SOC_I2S_SUPPORTS_APLL
    /* Enable APLL and acquire its lock when the clock source is APLL */
    if (tdm_cfg->clk_cfg.clk_src == I2S_CLK_SRC_APLL) {
        periph_rtc_apll_acquire();
        handle->apll_en = true;
    }
#endif
    ESP_GOTO_ON_ERROR(i2s_tdm_set_clock(handle, &tdm_cfg->clk_cfg), err, TAG, "initialize channel failed while setting clock");
    /* i2s_tdm_set_gpio should be called after i2s_tdm_set_clock as mclk relies on the clock source */
    ESP_GOTO_ON_ERROR(i2s_tdm_set_gpio(handle, &tdm_cfg->gpio_cfg), err, TAG, "initialize channel failed while setting gpio pins");
    ESP_GOTO_ON_ERROR(i2s_init_dma_intr(handle, I2S_INTR_ALLOC_FLAGS), err, TAG, "initialize dma interrupt failed");

#if SOC_I2S_HW_VERSION_2
    /* Enable clock to start outputting mclk signal. Some codecs will reset once mclk stop */
    if (handle->dir == I2S_DIR_TX) {
        i2s_ll_tx_enable_tdm(handle->controller->hal.dev);
    } else {
        i2s_ll_rx_enable_tdm(handle->controller->hal.dev);
    }
#endif
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_type_t pm_type = ESP_PM_APB_FREQ_MAX;
    ESP_RETURN_ON_ERROR(esp_pm_lock_create(pm_type, 0, "i2s_driver", &handle->pm_lock), TAG, "I2S pm lock create failed");
#endif

    /* Initialization finished, mark state as ready */
    handle->state = I2S_CHAN_STATE_READY;
    xSemaphoreGive(handle->mutex);
    ESP_LOGD(TAG, "The %s channel on I2S%d has been initialized to TDM mode successfully",
             handle->dir == I2S_DIR_TX ? "tx" : "rx", handle->controller->id);
    return ret;

err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_channel_reconfig_tdm_clock(i2s_chan_handle_t handle, const i2s_tdm_clk_config_t *clk_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, clk_cfg);

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_TDM, ESP_ERR_INVALID_ARG, err, TAG, "this handle is not working in standard mode");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "invalid state, I2S should be disabled before reconfiguring the clock");
    i2s_tdm_config_t *tdm_cfg = (i2s_tdm_config_t *)handle->mode_info;
    ESP_GOTO_ON_FALSE(tdm_cfg, ESP_ERR_INVALID_STATE, err, TAG, "initialization not complete");

#if SOC_I2S_SUPPORTS_APLL
    /* Enable APLL and acquire its lock when the clock source is changed to APLL */
    if (clk_cfg->clk_src == I2S_CLK_SRC_APLL && tdm_cfg->clk_cfg.clk_src != I2S_CLK_SRC_APLL) {
        periph_rtc_apll_acquire();
        handle->apll_en = true;
    }
    /* Disable APLL and release its lock when clock source is changed to 160M_PLL */
    if (clk_cfg->clk_src != I2S_CLK_SRC_APLL && tdm_cfg->clk_cfg.clk_src == I2S_CLK_SRC_APLL) {
        periph_rtc_apll_release();
        handle->apll_en = false;
    }
#endif

    ESP_GOTO_ON_ERROR(i2s_tdm_set_clock(handle, clk_cfg), err, TAG, "update clock failed");

#ifdef CONFIG_PM_ENABLE
    // Create/Re-create power management lock
    if (tdm_cfg->clk_cfg.clk_src != clk_cfg->clk_src) {
        ESP_GOTO_ON_ERROR(esp_pm_lock_delete(handle->pm_lock), err, TAG, "I2S delete old pm lock failed");
        esp_pm_lock_type_t pm_type = ESP_PM_APB_FREQ_MAX;
#if SOC_I2S_SUPPORTS_APLL
        if (clk_cfg->clk_src == I2S_CLK_SRC_APLL) {
            pm_type = ESP_PM_NO_LIGHT_SLEEP;
        }
#endif // SOC_I2S_SUPPORTS_APLL
        ESP_GOTO_ON_ERROR(esp_pm_lock_create(pm_type, 0, "i2s_driver", &handle->pm_lock), err, TAG, "I2S pm lock create failed");
    }
#endif //CONFIG_PM_ENABLE

    xSemaphoreGive(handle->mutex);

    return ESP_OK;
err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_channel_reconfig_tdm_slot(i2s_chan_handle_t handle, const i2s_tdm_slot_config_t *slot_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, slot_cfg);

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_TDM, ESP_ERR_INVALID_ARG, err, TAG, "this handle is not working in standard mode");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "invalid state, I2S should be disabled before reconfiguring the slot");

    i2s_tdm_config_t *tdm_cfg = (i2s_tdm_config_t *)handle->mode_info;
    ESP_GOTO_ON_FALSE(tdm_cfg, ESP_ERR_INVALID_STATE, err, TAG, "initialization not complete");

    ESP_GOTO_ON_ERROR(i2s_tdm_set_slot(handle, slot_cfg), err, TAG, "set i2s standard slot failed");

    /* If the slot bit width changed, then need to update the clock */
    uint32_t slot_bits = slot_cfg->slot_bit_width == I2S_SLOT_BIT_WIDTH_AUTO ? slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    if (tdm_cfg->slot_cfg.slot_bit_width == slot_bits) {
        ESP_GOTO_ON_ERROR(i2s_tdm_set_clock(handle, &tdm_cfg->clk_cfg), err, TAG, "update clock failed");
    }

    /* Reset queue */
    xQueueReset(handle->msg_queue);

    xSemaphoreGive(handle->mutex);

    return ESP_OK;
err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_channel_reconfig_tdm_gpio(i2s_chan_handle_t handle, const i2s_tdm_gpio_config_t *gpio_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, gpio_cfg);

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_TDM, ESP_ERR_INVALID_ARG, err, TAG, "This handle is not working in standard mode");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "Invalid state, I2S should be disabled before reconfiguring the gpio");

    if (handle->reserve_gpio_mask) {
        i2s_output_gpio_revoke(handle, handle->reserve_gpio_mask);
    }
    ESP_GOTO_ON_ERROR(i2s_tdm_set_gpio(handle, gpio_cfg), err, TAG, "set i2s standard slot failed");
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
err:
    xSemaphoreGive(handle->mutex);
    return ret;
}
