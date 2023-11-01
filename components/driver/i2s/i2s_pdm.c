/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
#include "driver/i2s_pdm.h"
#include "i2s_private.h"
#include "clk_ctrl_os.h"
#include "esp_intr_alloc.h"
#include "esp_check.h"

static const char *TAG = "i2s_pdm";

/*---------------------------------------------------------------
                            PDM TX
  ---------------------------------------------------------------*/

#if SOC_I2S_SUPPORTS_PDM_TX
#define I2S_PDM_TX_BCLK_DIV_MIN    8  /*!< The minimum bclk_div for PDM TX mode */
static esp_err_t i2s_pdm_tx_calculate_clock(i2s_chan_handle_t handle, const i2s_pdm_tx_clk_config_t *clk_cfg, i2s_hal_clock_info_t *clk_info)
{
    uint32_t rate = clk_cfg->sample_rate_hz;
    i2s_pdm_tx_clk_config_t *pdm_tx_clk = (i2s_pdm_tx_clk_config_t *)clk_cfg;

    // Over sampling ratio (integer, mostly should be 1 or 2)
    uint32_t over_sample_ratio = pdm_tx_clk->up_sample_fp / pdm_tx_clk->up_sample_fs;
    clk_info->bclk = rate * I2S_LL_PDM_BCK_FACTOR * over_sample_ratio;
    clk_info->bclk_div = clk_cfg->bclk_div < I2S_PDM_TX_BCLK_DIV_MIN ? I2S_PDM_TX_BCLK_DIV_MIN : clk_cfg->bclk_div;
    clk_info->mclk = clk_info->bclk * clk_info->bclk_div;
    clk_info->sclk = i2s_get_source_clk_freq(clk_cfg->clk_src, clk_info->mclk);
    clk_info->mclk_div = clk_info->sclk / clk_info->mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(clk_info->mclk_div, ESP_ERR_INVALID_ARG, TAG, "sample rate is too large");
    /* Set up sampling configuration */
    i2s_ll_tx_set_pdm_fpfs(handle->controller->hal.dev, pdm_tx_clk->up_sample_fp, pdm_tx_clk->up_sample_fs);
    i2s_ll_tx_set_pdm_over_sample_ratio(handle->controller->hal.dev, over_sample_ratio);

    return ESP_OK;
}

static esp_err_t i2s_pdm_tx_set_clock(i2s_chan_handle_t handle, const i2s_pdm_tx_clk_config_t *clk_cfg)
{
    esp_err_t ret = ESP_OK;
    i2s_pdm_tx_config_t *pdm_tx_cfg = (i2s_pdm_tx_config_t *)(handle->mode_info);
#if SOC_I2S_HW_VERSION_2
    ESP_RETURN_ON_FALSE(clk_cfg->clk_src != I2S_CLK_SRC_EXTERNAL, ESP_ERR_INVALID_ARG, TAG, "not support external clock source in pdm mode");
#endif
    ESP_RETURN_ON_FALSE(clk_cfg->up_sample_fs <= 480, ESP_ERR_INVALID_ARG, TAG, "up_sample_fs should be within 480");

    i2s_hal_clock_info_t clk_info;
    /* Calculate clock parameters */
    ESP_RETURN_ON_ERROR(i2s_pdm_tx_calculate_clock(handle, clk_cfg, &clk_info), TAG, "clock calculate failed");
    ESP_LOGD(TAG, "Clock division info: [sclk] %"PRIu32" Hz [mdiv] %d [mclk] %"PRIu32" Hz [bdiv] %d [bclk] %"PRIu32" Hz",
             clk_info.sclk, clk_info.mclk_div, clk_info.mclk, clk_info.bclk_div, clk_info.bclk);

    portENTER_CRITICAL(&g_i2s.spinlock);
    /* Set clock configurations in HAL*/
    I2S_CLOCK_SRC_ATOMIC() {
        i2s_hal_set_tx_clock(&handle->controller->hal, &clk_info, clk_cfg->clk_src);
    }
#if SOC_I2S_HW_VERSION_2
    /* Work around for PDM TX clock, overwrite the raw division directly to reduce the noise
     * This set of coefficients is a special division to reduce the background noise in PDM TX mode */
    i2s_ll_tx_set_raw_clk_div(handle->controller->hal.dev, clk_info.mclk_div, 1, 1, 0, 0);
#endif
    portEXIT_CRITICAL(&g_i2s.spinlock);

    /* Update the mode info: clock configuration */
    memcpy(&(pdm_tx_cfg->clk_cfg), clk_cfg, sizeof(i2s_pdm_tx_clk_config_t));

    return ret;
}

static esp_err_t i2s_pdm_tx_set_slot(i2s_chan_handle_t handle, const i2s_pdm_tx_slot_config_t *slot_cfg)
{
    /* Update the total slot num and active slot num */
    handle->total_slot = 2;
    handle->active_slot = slot_cfg->slot_mode == I2S_SLOT_MODE_MONO ? 1 : 2;

    uint32_t buf_size = i2s_get_buf_size(handle, slot_cfg->data_bit_width, handle->dma.frame_num);
    /* The DMA buffer need to re-allocate if the buffer size changed */
    if (handle->dma.buf_size != buf_size) {
        handle->dma.buf_size = buf_size;
        ESP_RETURN_ON_ERROR(i2s_free_dma_desc(handle), TAG, "failed to free the old dma descriptor");
        ESP_RETURN_ON_ERROR(i2s_alloc_dma_desc(handle, handle->dma.desc_num, buf_size),
                            TAG, "allocate memory for dma descriptor failed");
    }
    /* Share bck and ws signal in full-duplex mode */
    i2s_ll_share_bck_ws(handle->controller->hal.dev, handle->controller->full_duplex);

    /* Update the mode info: slot configuration */
    i2s_pdm_tx_config_t *pdm_tx_cfg = (i2s_pdm_tx_config_t *)handle->mode_info;
    memcpy(&(pdm_tx_cfg->slot_cfg), slot_cfg, sizeof(i2s_pdm_tx_slot_config_t));

    portENTER_CRITICAL(&g_i2s.spinlock);
    /* Configure the hardware to apply PDM format */
    bool is_slave = handle->role == I2S_ROLE_SLAVE;
    i2s_hal_slot_config_t *slot_hal_cfg = (i2s_hal_slot_config_t *)(&(pdm_tx_cfg->slot_cfg));
#if SOC_I2S_HW_VERSION_2
    /* Times 10 to transform the float type to integer because we should avoid float type in hal */
    slot_hal_cfg->pdm_tx.hp_cut_off_freq_hzx10 = (uint32_t)(slot_cfg->hp_cut_off_freq_hz * 10);
#endif
    i2s_hal_pdm_set_tx_slot(&(handle->controller->hal), is_slave, slot_hal_cfg);
    portEXIT_CRITICAL(&g_i2s.spinlock);

    return ESP_OK;
}

static esp_err_t i2s_pdm_tx_set_gpio(i2s_chan_handle_t handle, const i2s_pdm_tx_gpio_config_t *gpio_cfg)
{
    int id = handle->controller->id;

    /* Check validity of selected pins */
    ESP_RETURN_ON_FALSE((gpio_cfg->clk == -1 || GPIO_IS_VALID_GPIO(gpio_cfg->clk)),
                        ESP_ERR_INVALID_ARG, TAG, "clk gpio is invalid");
    ESP_RETURN_ON_FALSE((gpio_cfg->dout == -1 || GPIO_IS_VALID_GPIO(gpio_cfg->dout)),
                        ESP_ERR_INVALID_ARG, TAG, "dout gpio is invalid");
    i2s_pdm_tx_config_t *pdm_tx_cfg = (i2s_pdm_tx_config_t *)handle->mode_info;
    /* Set data output GPIO */
    i2s_gpio_check_and_set(gpio_cfg->dout, i2s_periph_signal[id].data_out_sig, false, false);
#if SOC_I2S_PDM_MAX_TX_LINES > 1
    if (pdm_tx_cfg->slot_cfg.line_mode == I2S_PDM_TX_TWO_LINE_DAC) {
        i2s_gpio_check_and_set(gpio_cfg->dout2, i2s_periph_signal[id].data_out_sigs[1], false, false);
    }
#endif

    if (handle->role == I2S_ROLE_SLAVE) {
        /* For "tx + slave" mode, select TX signal index for ws and bck */
        if (!handle->controller->full_duplex) {
            i2s_gpio_check_and_set(gpio_cfg->clk, i2s_periph_signal[id].s_tx_ws_sig, true, gpio_cfg->invert_flags.clk_inv);
            /* For "tx + rx + slave" or "rx + slave" mode, select RX signal index for ws and bck */
        } else {
            i2s_gpio_check_and_set(gpio_cfg->clk, i2s_periph_signal[id].s_rx_ws_sig, true, gpio_cfg->invert_flags.clk_inv);
        }
    } else {
        i2s_gpio_check_and_set(gpio_cfg->clk, i2s_periph_signal[id].m_tx_ws_sig, false, gpio_cfg->invert_flags.clk_inv);
    }
#if SOC_I2S_HW_VERSION_2
    i2s_ll_mclk_bind_to_tx_clk(handle->controller->hal.dev);
#endif
    /* Update the mode info: gpio configuration */
    memcpy(&(pdm_tx_cfg->gpio_cfg), gpio_cfg, sizeof(i2s_pdm_tx_gpio_config_t));

    return ESP_OK;
}

esp_err_t i2s_channel_init_pdm_tx_mode(i2s_chan_handle_t handle, const i2s_pdm_tx_config_t *pdm_tx_cfg)
{
#if CONFIG_I2S_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    I2S_NULL_POINTER_CHECK(TAG, handle);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_TX, ESP_ERR_INVALID_ARG, TAG, "This channel handle is not a TX handle");
    ESP_RETURN_ON_FALSE(handle->controller->id == I2S_NUM_0, ESP_ERR_INVALID_ARG, TAG, "This channel handle is registered on I2S1, but PDM is only supported on I2S0");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_REGISTER, ESP_ERR_INVALID_STATE, err, TAG, "the channel has initialized already");
    handle->mode = I2S_COMM_MODE_PDM;
    /* Allocate memory for storing the configurations of PDM tx mode */
    if (handle->mode_info) {
        free(handle->mode_info);
    }
    handle->mode_info = calloc(1, sizeof(i2s_pdm_tx_config_t));
    ESP_GOTO_ON_FALSE(handle->mode_info, ESP_ERR_NO_MEM, err, TAG, "no memory for storing the configurations");
    /* i2s_set_pdm_tx_slot should be called before i2s_set_pdm_tx_clock and i2s_pdm_tx_set_gpio
     * while initializing, because clock and gpio is relay on the slot */
    ESP_GOTO_ON_ERROR(i2s_pdm_tx_set_slot(handle, &pdm_tx_cfg->slot_cfg), err, TAG, "initialize channel failed while setting slot");
    ESP_GOTO_ON_ERROR(i2s_pdm_tx_set_gpio(handle, &pdm_tx_cfg->gpio_cfg), err, TAG, "initialize channel failed while setting gpio pins");
#if SOC_I2S_SUPPORTS_APLL
    /* Enable APLL and acquire its lock when the clock source is APLL */
    if (pdm_tx_cfg->clk_cfg.clk_src == I2S_CLK_SRC_APLL) {
        periph_rtc_apll_acquire();
        handle->apll_en = true;
    }
#endif
    ESP_GOTO_ON_ERROR(i2s_pdm_tx_set_clock(handle, &pdm_tx_cfg->clk_cfg), err, TAG, "initialize channel failed while setting clock");
    ESP_GOTO_ON_ERROR(i2s_init_dma_intr(handle, I2S_INTR_ALLOC_FLAGS), err, TAG, "initialize dma interrupt failed");

    i2s_ll_tx_enable_pdm(handle->controller->hal.dev);

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_type_t pm_type = ESP_PM_APB_FREQ_MAX;
#if SOC_I2S_SUPPORTS_APLL
    if (pdm_tx_cfg->clk_cfg.clk_src == I2S_CLK_SRC_APLL) {
        pm_type = ESP_PM_NO_LIGHT_SLEEP;
    }
#endif // SOC_I2S_SUPPORTS_APLL
    ESP_RETURN_ON_ERROR(esp_pm_lock_create(pm_type, 0, "i2s_driver", &handle->pm_lock), TAG, "I2S pm lock create failed");
#endif

    /* Initialization finished, mark state as ready */
    handle->state = I2S_CHAN_STATE_READY;
    xSemaphoreGive(handle->mutex);
    ESP_LOGD(TAG, "The tx channel on I2S0 has been initialized to PDM TX mode successfully");
    return ret;

err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_channel_reconfig_pdm_tx_clock(i2s_chan_handle_t handle, const i2s_pdm_tx_clk_config_t *clk_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, clk_cfg);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_TX, ESP_ERR_INVALID_ARG, TAG, "This channel handle is not a TX handle");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_PDM, ESP_ERR_INVALID_ARG, err, TAG, "this handle is not working in standard mode");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "invalid state, I2S should be disabled before reconfiguring the clock");
    i2s_pdm_tx_config_t *pdm_tx_cfg = (i2s_pdm_tx_config_t *)handle->mode_info;
    ESP_GOTO_ON_FALSE(pdm_tx_cfg, ESP_ERR_INVALID_STATE, err, TAG, "initialization not complete");

#if SOC_I2S_SUPPORTS_APLL
    /* Enable APLL and acquire its lock when the clock source is changed to APLL */
    if (clk_cfg->clk_src == I2S_CLK_SRC_APLL && pdm_tx_cfg->clk_cfg.clk_src != I2S_CLK_SRC_APLL) {
        periph_rtc_apll_acquire();
        handle->apll_en = true;
    }
    /* Disable APLL and release its lock when clock source is changed to 160M_PLL */
    if (clk_cfg->clk_src != I2S_CLK_SRC_APLL && pdm_tx_cfg->clk_cfg.clk_src == I2S_CLK_SRC_APLL) {
        periph_rtc_apll_release();
        handle->apll_en = false;
    }
#endif

    ESP_GOTO_ON_ERROR(i2s_pdm_tx_set_clock(handle, clk_cfg), err, TAG, "update clock failed");
#ifdef CONFIG_PM_ENABLE
    // Create/Re-create power management lock
    if (pdm_tx_cfg->clk_cfg.clk_src != clk_cfg->clk_src) {
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

esp_err_t i2s_channel_reconfig_pdm_tx_slot(i2s_chan_handle_t handle, const i2s_pdm_tx_slot_config_t *slot_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, slot_cfg);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_TX, ESP_ERR_INVALID_ARG, TAG, "This channel handle is not a TX handle");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_PDM, ESP_ERR_INVALID_ARG, err, TAG, "this handle is not working in standard mode");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "invalid state, I2S should be disabled before reconfiguring the slot");

    i2s_pdm_tx_config_t *pdm_tx_cfg = (i2s_pdm_tx_config_t *)handle->mode_info;
    ESP_GOTO_ON_FALSE(pdm_tx_cfg, ESP_ERR_INVALID_STATE, err, TAG, "initialization not complete");

    ESP_GOTO_ON_ERROR(i2s_pdm_tx_set_slot(handle, slot_cfg), err, TAG, "set i2s standard slot failed");

    /* If the slot bit width changed, then need to update the clock */
    uint32_t slot_bits = slot_cfg->slot_bit_width == I2S_SLOT_BIT_WIDTH_AUTO ? slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    if (pdm_tx_cfg->slot_cfg.slot_bit_width == slot_bits) {
        ESP_GOTO_ON_ERROR(i2s_pdm_tx_set_clock(handle, &pdm_tx_cfg->clk_cfg), err, TAG, "update clock failed");
    }

    xSemaphoreGive(handle->mutex);

    return ESP_OK;
err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_channel_reconfig_pdm_tx_gpio(i2s_chan_handle_t handle, const i2s_pdm_tx_gpio_config_t *gpio_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, gpio_cfg);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_TX, ESP_ERR_INVALID_ARG, TAG, "This channel handle is not a TX handle");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_PDM, ESP_ERR_INVALID_ARG, err, TAG, "This handle is not working in standard mode");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "Invalid state, I2S should be disabled before reconfiguring the gpio");

    ESP_GOTO_ON_ERROR(i2s_pdm_tx_set_gpio(handle, gpio_cfg), err, TAG, "set i2s standard slot failed");
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

#endif


/*---------------------------------------------------------------
                            PDM RX
  ---------------------------------------------------------------*/

#if SOC_I2S_SUPPORTS_PDM_RX
#define I2S_PDM_RX_BCLK_DIV_MIN    8  /*!< The minimum bclk_div for PDM RX mode */
static esp_err_t i2s_pdm_rx_calculate_clock(i2s_chan_handle_t handle, const i2s_pdm_rx_clk_config_t *clk_cfg, i2s_hal_clock_info_t *clk_info)
{
    uint32_t rate = clk_cfg->sample_rate_hz;
    i2s_pdm_rx_clk_config_t *pdm_rx_clk = (i2s_pdm_rx_clk_config_t *)clk_cfg;

    clk_info->bclk = rate * I2S_LL_PDM_BCK_FACTOR * (pdm_rx_clk->dn_sample_mode == I2S_PDM_DSR_16S ? 2 : 1);
    clk_info->bclk_div = clk_cfg->bclk_div < I2S_PDM_RX_BCLK_DIV_MIN ? I2S_PDM_RX_BCLK_DIV_MIN : clk_cfg->bclk_div;
    clk_info->mclk = clk_info->bclk * clk_info->bclk_div;
    clk_info->sclk = i2s_get_source_clk_freq(clk_cfg->clk_src, clk_info->mclk);
    clk_info->mclk_div = clk_info->sclk / clk_info->mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(clk_info->mclk_div, ESP_ERR_INVALID_ARG, TAG, "sample rate is too large");
    /* Set down-sampling configuration */
    i2s_ll_rx_set_pdm_dsr(handle->controller->hal.dev, pdm_rx_clk->dn_sample_mode);
    return ESP_OK;
}

static esp_err_t i2s_pdm_rx_set_clock(i2s_chan_handle_t handle, const i2s_pdm_rx_clk_config_t *clk_cfg)
{
    esp_err_t ret = ESP_OK;
    i2s_pdm_rx_config_t *pdm_rx_cfg = (i2s_pdm_rx_config_t *)(handle->mode_info);
#if SOC_I2S_HW_VERSION_2
    ESP_RETURN_ON_FALSE(clk_cfg->clk_src != I2S_CLK_SRC_EXTERNAL, ESP_ERR_INVALID_ARG, TAG, "not support external clock source in pdm mode");
#endif

    i2s_hal_clock_info_t clk_info;
    /* Calculate clock parameters */
    ESP_RETURN_ON_ERROR(i2s_pdm_rx_calculate_clock(handle, clk_cfg, &clk_info), TAG, "clock calculate failed");
    ESP_LOGD(TAG, "Clock division info: [sclk] %"PRIu32" Hz [mdiv] %d [mclk] %"PRIu32" Hz [bdiv] %d [bclk] %"PRIu32" Hz",
             clk_info.sclk, clk_info.mclk_div, clk_info.mclk, clk_info.bclk_div, clk_info.bclk);

    portENTER_CRITICAL(&g_i2s.spinlock);
    /* Set clock configurations in HAL*/
    I2S_CLOCK_SRC_ATOMIC() {
        i2s_hal_set_rx_clock(&handle->controller->hal, &clk_info, clk_cfg->clk_src);
    }
    portEXIT_CRITICAL(&g_i2s.spinlock);

    /* Update the mode info: clock configuration */
    memcpy(&(pdm_rx_cfg->clk_cfg), clk_cfg, sizeof(i2s_pdm_rx_clk_config_t));

    return ret;
}

static esp_err_t i2s_pdm_rx_set_slot(i2s_chan_handle_t handle, const i2s_pdm_rx_slot_config_t *slot_cfg)
{
    /* Update the total slot num and active slot num */
    handle->total_slot = 2;
    handle->active_slot = slot_cfg->slot_mode == I2S_SLOT_MODE_MONO ? 1 : 2;

    uint32_t buf_size = i2s_get_buf_size(handle, slot_cfg->data_bit_width, handle->dma.frame_num);
    /* The DMA buffer need to re-allocate if the buffer size changed */
    if (handle->dma.buf_size != buf_size) {
        handle->dma.buf_size = buf_size;
        ESP_RETURN_ON_ERROR(i2s_free_dma_desc(handle), TAG, "failed to free the old dma descriptor");
        ESP_RETURN_ON_ERROR(i2s_alloc_dma_desc(handle, handle->dma.desc_num, buf_size),
                            TAG, "allocate memory for dma descriptor failed");
    }
    /* Share bck and ws signal in full-duplex mode */
    i2s_ll_share_bck_ws(handle->controller->hal.dev, handle->controller->full_duplex);

    /* Update the mode info: slot configuration */
    i2s_pdm_rx_config_t *pdm_rx_cfg = (i2s_pdm_rx_config_t *)handle->mode_info;
    memcpy(&(pdm_rx_cfg->slot_cfg), slot_cfg, sizeof(i2s_pdm_rx_slot_config_t));

    portENTER_CRITICAL(&g_i2s.spinlock);
    /* Configure the hardware to apply PDM format */
    bool is_slave = (handle->role == I2S_ROLE_SLAVE) | handle->controller->full_duplex;
    i2s_hal_slot_config_t *slot_hal_cfg = (i2s_hal_slot_config_t *)(&(pdm_rx_cfg->slot_cfg));
#if SOC_I2S_SUPPORTS_PDM_RX_HP_FILTER
    /* Times 10 to transform the float type to integer because we should avoid float type in hal */
    slot_hal_cfg->pdm_rx.hp_cut_off_freq_hzx10 = (uint32_t)(slot_cfg->hp_cut_off_freq_hz * 10);
#endif
    i2s_hal_pdm_set_rx_slot(&(handle->controller->hal), is_slave, slot_hal_cfg);
    portEXIT_CRITICAL(&g_i2s.spinlock);

    return ESP_OK;
}

static esp_err_t i2s_pdm_rx_set_gpio(i2s_chan_handle_t handle, const i2s_pdm_rx_gpio_config_t *gpio_cfg)
{
    int id = handle->controller->id;

    /* Check validity of selected pins */
    ESP_RETURN_ON_FALSE((gpio_cfg->clk == -1 || GPIO_IS_VALID_GPIO(gpio_cfg->clk)),
                        ESP_ERR_INVALID_ARG, TAG, "clk gpio is invalid");
    ESP_RETURN_ON_FALSE((gpio_cfg->din == -1 || GPIO_IS_VALID_GPIO(gpio_cfg->din)),
                        ESP_ERR_INVALID_ARG, TAG, "dout gpio is invalid");
    i2s_pdm_rx_config_t *pdm_rx_cfg = (i2s_pdm_rx_config_t *)handle->mode_info;
    /* Set data input GPIO */
#if SOC_I2S_PDM_MAX_RX_LINES > 1
    for (int i = 0; i < SOC_I2S_PDM_MAX_RX_LINES; i++) {
        if (pdm_rx_cfg->slot_cfg.slot_mask & (0x03 << (i * 2))) {
            i2s_gpio_check_and_set(gpio_cfg->dins[i], i2s_periph_signal[id].data_in_sigs[i], true, false);
        }
    }
#else
    i2s_gpio_check_and_set(gpio_cfg->din, i2s_periph_signal[id].data_in_sig, true, false);
#endif

    if (handle->role == I2S_ROLE_SLAVE) {
        /* For "tx + rx + slave" or "rx + slave" mode, select RX signal index for ws and bck */
        i2s_gpio_check_and_set(gpio_cfg->clk, i2s_periph_signal[id].s_rx_ws_sig, true, gpio_cfg->invert_flags.clk_inv);
    } else {
        if (!handle->controller->full_duplex) {
            i2s_gpio_check_and_set(gpio_cfg->clk, i2s_periph_signal[id].m_rx_ws_sig, false, gpio_cfg->invert_flags.clk_inv);
        } else {
            i2s_gpio_check_and_set(gpio_cfg->clk, i2s_periph_signal[id].m_tx_ws_sig, false, gpio_cfg->invert_flags.clk_inv);
        }
    }
#if SOC_I2S_HW_VERSION_2
    i2s_ll_mclk_bind_to_rx_clk(handle->controller->hal.dev);
#endif
    /* Update the mode info: gpio configuration */
    memcpy(&(pdm_rx_cfg->gpio_cfg), gpio_cfg, sizeof(i2s_pdm_rx_gpio_config_t));

    return ESP_OK;
}

esp_err_t i2s_channel_init_pdm_rx_mode(i2s_chan_handle_t handle, const i2s_pdm_rx_config_t *pdm_rx_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_RX, ESP_ERR_INVALID_ARG, TAG, "This channel handle is not a RX handle");
    ESP_RETURN_ON_FALSE(handle->controller->id == I2S_NUM_0, ESP_ERR_INVALID_ARG, TAG, "This channel handle is registered on I2S1, but PDM is only supported on I2S0");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_REGISTER, ESP_ERR_INVALID_STATE, err, TAG, "the channel has initialized already");
    handle->mode = I2S_COMM_MODE_PDM;
    /* Allocate memory for storing the configurations of PDM rx mode */
    if (handle->mode_info) {
        free(handle->mode_info);
    }
    handle->mode_info = calloc(1, sizeof(i2s_pdm_rx_config_t));
    ESP_GOTO_ON_FALSE(handle->mode_info, ESP_ERR_NO_MEM, err, TAG, "no memory for storing the configurations");
    /* i2s_set_pdm_rx_slot should be called before i2s_set_pdm_rx_clock and i2s_pdm_rx_set_gpio while initializing, because clock is relay on the slot */
    ESP_GOTO_ON_ERROR(i2s_pdm_rx_set_slot(handle, &pdm_rx_cfg->slot_cfg), err, TAG, "initialize channel failed while setting slot");
    ESP_GOTO_ON_ERROR(i2s_pdm_rx_set_gpio(handle, &pdm_rx_cfg->gpio_cfg), err, TAG, "initialize channel failed while setting gpio pins");
#if SOC_I2S_SUPPORTS_APLL
    /* Enable APLL and acquire its lock when the clock source is APLL */
    if (pdm_rx_cfg->clk_cfg.clk_src == I2S_CLK_SRC_APLL) {
        periph_rtc_apll_acquire();
        handle->apll_en = true;
    }
#endif
    ESP_GOTO_ON_ERROR(i2s_pdm_rx_set_clock(handle, &pdm_rx_cfg->clk_cfg), err, TAG, "initialize channel failed while setting clock");
    ESP_GOTO_ON_ERROR(i2s_init_dma_intr(handle, I2S_INTR_ALLOC_FLAGS), err, TAG, "initialize dma interrupt failed");

    i2s_ll_rx_enable_pdm(handle->controller->hal.dev);

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_type_t pm_type = ESP_PM_APB_FREQ_MAX;
#if SOC_I2S_SUPPORTS_APLL
    if (pdm_rx_cfg->clk_cfg.clk_src == I2S_CLK_SRC_APLL) {
        pm_type = ESP_PM_NO_LIGHT_SLEEP;
    }
#endif // SOC_I2S_SUPPORTS_APLL
    ESP_RETURN_ON_ERROR(esp_pm_lock_create(pm_type, 0, "i2s_driver", &handle->pm_lock), TAG, "I2S pm lock create failed");
#endif

    /* Initialization finished, mark state as ready */
    handle->state = I2S_CHAN_STATE_READY;
    xSemaphoreGive(handle->mutex);
    ESP_LOGD(TAG, "The rx channel on I2S0 has been initialized to PDM RX mode successfully");
    return ret;

err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_channel_reconfig_pdm_rx_clock(i2s_chan_handle_t handle, const i2s_pdm_rx_clk_config_t *clk_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, clk_cfg);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_RX, ESP_ERR_INVALID_ARG, TAG, "This channel handle is not a RX handle");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_PDM, ESP_ERR_INVALID_ARG, err, TAG, "this handle is not working in standard mode");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "invalid state, I2S should be disabled before reconfiguring the clock");
    i2s_pdm_rx_config_t *pdm_rx_cfg = (i2s_pdm_rx_config_t *)handle->mode_info;
    ESP_GOTO_ON_FALSE(pdm_rx_cfg, ESP_ERR_INVALID_STATE, err, TAG, "initialization not complete");

#if SOC_I2S_SUPPORTS_APLL
    /* Enable APLL and acquire its lock when the clock source is changed to APLL */
    if (clk_cfg->clk_src == I2S_CLK_SRC_APLL && pdm_rx_cfg->clk_cfg.clk_src != I2S_CLK_SRC_APLL) {
        periph_rtc_apll_acquire();
        handle->apll_en = true;
    }
    /* Disable APLL and release its lock when clock source is changed to 160M_PLL */
    if (clk_cfg->clk_src != I2S_CLK_SRC_APLL && pdm_rx_cfg->clk_cfg.clk_src == I2S_CLK_SRC_APLL) {
        periph_rtc_apll_release();
        handle->apll_en = false;
    }
#endif

    ESP_GOTO_ON_ERROR(i2s_pdm_rx_set_clock(handle, clk_cfg), err, TAG, "update clock failed");
#ifdef CONFIG_PM_ENABLE
    // Create/Re-create power management lock
    if (pdm_rx_cfg->clk_cfg.clk_src != clk_cfg->clk_src) {
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

esp_err_t i2s_channel_reconfig_pdm_rx_slot(i2s_chan_handle_t handle, const i2s_pdm_rx_slot_config_t *slot_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, slot_cfg);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_RX, ESP_ERR_INVALID_ARG, TAG, "This channel handle is not a RX handle");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_PDM, ESP_ERR_INVALID_ARG, err, TAG, "this handle is not working in standard mode");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "invalid state, I2S should be disabled before reconfiguring the slot");

    i2s_pdm_rx_config_t *pdm_rx_cfg = (i2s_pdm_rx_config_t *)handle->mode_info;
    ESP_GOTO_ON_FALSE(pdm_rx_cfg, ESP_ERR_INVALID_STATE, err, TAG, "initialization not complete");

    ESP_GOTO_ON_ERROR(i2s_pdm_rx_set_slot(handle, slot_cfg), err, TAG, "set i2s standard slot failed");

    /* If the slot bit width changed, then need to update the clock */
    uint32_t slot_bits = slot_cfg->slot_bit_width == I2S_SLOT_BIT_WIDTH_AUTO ? slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    if (pdm_rx_cfg->slot_cfg.slot_bit_width == slot_bits) {
        ESP_GOTO_ON_ERROR(i2s_pdm_rx_set_clock(handle, &pdm_rx_cfg->clk_cfg), err, TAG, "update clock failed");
    }

    xSemaphoreGive(handle->mutex);

    return ESP_OK;
err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_channel_reconfig_pdm_rx_gpio(i2s_chan_handle_t handle, const i2s_pdm_rx_gpio_config_t *gpio_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, gpio_cfg);

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_PDM, ESP_ERR_INVALID_ARG, err, TAG, "This handle is not working in standard mode");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "Invalid state, I2S should be disabled before reconfiguring the gpio");

    ESP_GOTO_ON_ERROR(i2s_pdm_rx_set_gpio(handle, gpio_cfg), err, TAG, "set i2s standard slot failed");
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

#endif // SOC_I2S_SUPPORTS_PDM_RX
