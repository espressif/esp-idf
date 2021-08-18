/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "hal/i2s_hal.h"
#include "driver/gpio.h"
#include "driver/i2s_pdm.h"
#include "i2s_private.h"
#include "clk_ctrl_os.h"
#include "esp_intr_alloc.h"
#include "esp_check.h"

static const char *TAG = "I2S_PDM";

/*---------------------------------------------------------------
                            PDM TX
  ---------------------------------------------------------------*/

#if SOC_I2S_SUPPORTS_PDM_TX
static esp_err_t i2s_pdm_tx_calculate_clock(i2s_chan_handle_t handle, const i2s_pdm_tx_clk_config_t *clk_cfg, i2s_hal_clock_info_t *clk_info)
{
    uint32_t rate = clk_cfg->sample_rate_hz;
    i2s_pdm_tx_clk_config_t *pdm_tx_clk = (i2s_pdm_tx_clk_config_t *)clk_cfg;

    clk_info->bclk = rate * I2S_LL_PDM_BCK_FACTOR * pdm_tx_clk->up_sample_fp / pdm_tx_clk->up_sample_fs;
    clk_info->bclk_div = 8;
    clk_info->mclk = clk_info->bclk * clk_info->bclk_div;
#if SOC_I2S_SUPPORTS_APLL
    clk_info->sclk = clk_cfg->clk_src == I2S_CLK_160M_PLL ? I2S_LL_BASE_CLK : i2s_set_get_apll_freq(clk_info->mclk);
#else
    clk_info->sclk = I2S_LL_BASE_CLK;
#endif
    clk_info->mclk_div = clk_info->sclk / clk_info->mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(clk_info->mclk_div, ESP_ERR_INVALID_ARG, TAG, "sample rate is too large");
    /* Set upsampling configuration */
    i2s_ll_tx_set_pdm_fpfs(handle->parent->hal.dev, pdm_tx_clk->up_sample_fp, pdm_tx_clk->up_sample_fs);

    return ESP_OK;
}

static esp_err_t i2s_pdm_tx_set_clock(i2s_chan_handle_t handle, const i2s_pdm_tx_clk_config_t *clk_cfg)
{
    esp_err_t ret = ESP_OK;
    i2s_pdm_tx_config_t *pdm_tx_cfg = (i2s_pdm_tx_config_t *)(handle->mode_info);

    /* If Power Management enabled, a PM lock will be created when there is no PM lock or clock source changed */
#ifdef CONFIG_PM_ENABLE
    // Create/Re-create power management lock
    if (handle->pm_lock == NULL || pdm_tx_cfg->clk_cfg.clk_src != clk_cfg->clk_src) {
        if (handle->pm_lock) {
            ESP_RETURN_ON_ERROR(esp_pm_lock_delete(handle->pm_lock), TAG, "I2S delete old pm lock failed");
        }
        esp_pm_lock_type_t pm_type = ESP_PM_APB_FREQ_MAX;
#if SOC_I2S_SUPPORTS_APLL
        if (clk_cfg->clk_src == I2S_CLK_APLL) {
            pm_type = ESP_PM_NO_LIGHT_SLEEP;
        }
#endif // SOC_I2S_SUPPORTS_APLL
        ESP_RETURN_ON_ERROR(esp_pm_lock_create(pm_type, 0, "i2s_driver", &handle->pm_lock), err, TAG, "I2S pm lock create failed");
    }
#endif //CONFIG_PM_ENABLE

#if SOC_I2S_SUPPORTS_APLL
    /* Enable APLL and acquire its lock when initializing or clock source changed to APLL */
    if (clk_cfg->clk_src == I2S_CLK_APLL && (handle->state == I2S_CHAN_STATE_INIT || pdm_tx_cfg->clk_cfg.clk_src == I2S_CLK_160M_PLL)) {
        periph_rtc_apll_acquire();
        handle->apll_en = true;
    }
    /* Disable APLL and release its lock when clock source is changed to D2CLK */
    if (clk_cfg->clk_src == I2S_CLK_160M_PLL && pdm_tx_cfg->clk_cfg.clk_src == I2S_CLK_APLL) {
        periph_rtc_apll_release();
        handle->apll_en = false;
    }
#endif

    i2s_hal_clock_info_t clk_info;
    /* Calculate clock parameters */
    ESP_RETURN_ON_ERROR(i2s_pdm_tx_calculate_clock(handle, clk_cfg, &clk_info), TAG, "clock calculate failed");
    ESP_LOGD(TAG, "Clock division info: [sclk] %d Hz [mdiv] %d [mclk] %d Hz [bdiv] %d [bclk] %d Hz",
                clk_info.sclk, clk_info.mclk_div, clk_info.mclk, clk_info.bclk_div, clk_info.bclk);

    portENTER_CRITICAL(&s_i2s.spinlock);
    /* Set clock configurations in HAL*/
    i2s_hal_set_tx_clock(&handle->parent->hal, &clk_info, clk_cfg->clk_src);
    portEXIT_CRITICAL(&s_i2s.spinlock);

    return ret;
}

static esp_err_t i2s_pdm_tx_set_slot(i2s_chan_handle_t handle, const i2s_pdm_tx_slot_config_t *slot_cfg)
{
    uint32_t buf_size = i2s_get_buf_size(handle, slot_cfg->data_bit_width, handle->dma.frame_num);
    /* The DMA buffer need to re-allocate if the buffer size changed */
    if (handle->dma.buf_size != buf_size) {
        handle->dma.buf_size = buf_size;
        ESP_RETURN_ON_ERROR(i2s_free_dma_desc(handle), TAG, "failed to free the old dma descriptor");
        ESP_RETURN_ON_ERROR(i2s_alloc_dma_desc(handle, handle->dma.desc_num, buf_size),
                            TAG, "allocate memory for dma descriptor failed");
    }
    /* Share bck and ws signal in full-duplex mode */
    i2s_ll_share_bck_ws(handle->parent->hal.dev, handle->parent->full_duplex);

    portENTER_CRITICAL(&s_i2s.spinlock);
    /* Configure the hardware to apply PDM format */
    bool is_slave = handle->role == I2S_ROLE_SLAVE;
    i2s_hal_pdm_set_tx_slot(&(handle->parent->hal), is_slave, (i2s_hal_slot_config_t*)slot_cfg);
    portEXIT_CRITICAL(&s_i2s.spinlock);

    /* Update the total slot num and active slot num */
    handle->total_slot = 2;
    handle->active_slot = slot_cfg->slot_mode == I2S_SLOT_MODE_MONO ? 1 : 2;

    return ESP_OK;
}

static esp_err_t i2s_pdm_tx_set_gpio(i2s_chan_handle_t handle, const i2s_pdm_tx_gpio_config_t *gpio_cfg)
{
    int id = handle->parent->id;

    /* Check validity of selected pins */
    ESP_RETURN_ON_FALSE((gpio_cfg->clk == -1 || GPIO_IS_VALID_GPIO(gpio_cfg->clk)),
                        ESP_ERR_INVALID_ARG, TAG, "clk gpio is invalid");
    ESP_RETURN_ON_FALSE((gpio_cfg->dout == -1 || GPIO_IS_VALID_GPIO(gpio_cfg->dout)),
                        ESP_ERR_INVALID_ARG, TAG, "dout gpio is invalid");
    /* Set data output GPIO */
    i2s_gpio_check_and_set(gpio_cfg->dout, i2s_periph_signal[id].data_out_sig, false);

    if (handle->role == I2S_ROLE_SLAVE) {
        /* For "tx + slave" mode, select TX signal index for ws and bck */
        if (!handle->parent->full_duplex) {
            i2s_gpio_check_and_set(gpio_cfg->clk, i2s_periph_signal[id].s_tx_ws_sig, true);
        /* For "tx + rx + slave" or "rx + slave" mode, select RX signal index for ws and bck */
        } else {
            i2s_gpio_check_and_set(gpio_cfg->clk, i2s_periph_signal[id].s_rx_ws_sig, true);
        }
    } else {
        i2s_gpio_check_and_set(gpio_cfg->clk, i2s_periph_signal[id].m_tx_ws_sig, false);
    }
#if SOC_I2S_HW_VERSION_2
    i2s_ll_mclk_bind_to_tx_clk(handle->parent->hal.dev);
#endif

    return ESP_OK;
}

esp_err_t i2s_init_pdm_tx_channel(i2s_chan_handle_t handle, const i2s_pdm_tx_config_t *pdm_tx_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_TX, ESP_ERR_INVALID_ARG, TAG, "This channel handle is not a TX handle");
    ESP_RETURN_ON_FALSE(handle->parent->id == I2S_NUM_0, ESP_ERR_INVALID_ARG, TAG, "This channel handle is registered on I2S1, but PDM is only supported on I2S0");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_REGISTER, ESP_ERR_INVALID_STATE, err, TAG, "the channel has initialized already");
    ESP_GOTO_ON_ERROR(i2s_pdm_tx_set_gpio(handle, &pdm_tx_cfg->gpio_cfg), err, TAG, "initialize channel failed while setting gpio pins");
    /* i2s_set_slot should be called before i2s_set_clock while initializing, because clock is relay on the slot */
    ESP_GOTO_ON_ERROR(i2s_pdm_tx_set_slot(handle, &pdm_tx_cfg->slot_cfg), err, TAG, "initialize channel failed while setting slot");
    ESP_GOTO_ON_ERROR(i2s_pdm_tx_set_clock(handle, &pdm_tx_cfg->clk_cfg), err, TAG, "initialize channel failed while setting clock");
    ESP_GOTO_ON_ERROR(i2s_init_dma_intr(handle, ESP_INTR_FLAG_LEVEL1), err, TAG, "initialize dma interrupt failed");
    /* Store the configurations of standard mode */
    i2s_pdm_tx_config_t *mode_info = calloc(1, sizeof(i2s_pdm_tx_config_t));
    ESP_GOTO_ON_FALSE(mode_info, ESP_ERR_NO_MEM, err, TAG, "no memory for storing the configurations");
    memcpy(mode_info, pdm_tx_cfg, sizeof(i2s_pdm_tx_config_t));
    handle->mode_info = mode_info;

    i2s_ll_tx_enable_pdm(handle->parent->hal.dev);
#if SOC_I2S_HW_VERSION_2
    /* Enable clock to start outputting mclk signal. Some codecs will reset once mclk stop */
    i2s_ll_tx_enable_clock(handle->parent->hal.dev);
#endif

    /* Initialization finished, mark state as ready */
    handle->state = I2S_CHAN_STATE_READY;
    xSemaphoreGive(handle->mutex);
    return ret;

err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_reconfig_pdm_tx_clock(i2s_chan_handle_t handle, const i2s_pdm_tx_clk_config_t *clk_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, clk_cfg);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_TX, ESP_ERR_INVALID_ARG, TAG, "This channel handle is not a TX handle");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_PDM, ESP_ERR_INVALID_ARG, err, TAG, "this handle is not working in standard moded");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "invalid state, I2S should be stopped before reconfiguring the clock");

    i2s_pdm_tx_config_t *pdm_tx_cfg = (i2s_pdm_tx_config_t*)handle->mode_info;
    ESP_GOTO_ON_FALSE(pdm_tx_cfg, ESP_ERR_INVALID_STATE, err, TAG, "initialization not complete");

    ESP_GOTO_ON_ERROR(i2s_pdm_tx_set_clock(handle, &pdm_tx_cfg->clk_cfg), err, TAG, "update clock failed");
    /* Update the stored clock information  */
    memcpy(&pdm_tx_cfg->clk_cfg, clk_cfg, sizeof(i2s_pdm_tx_clk_config_t));

    xSemaphoreGive(handle->mutex);

    return ESP_OK;
err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_reconfig_pdm_tx_slot(i2s_chan_handle_t handle, const i2s_pdm_tx_slot_config_t *slot_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, slot_cfg);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_TX, ESP_ERR_INVALID_ARG, TAG, "This channel handle is not a TX handle");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_PDM, ESP_ERR_INVALID_ARG, err, TAG, "this handle is not working in standard moded");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "invalid state, I2S should be stopped before reconfiguring the slot");

    i2s_pdm_tx_config_t *pdm_tx_cfg = (i2s_pdm_tx_config_t*)handle->mode_info;
    ESP_GOTO_ON_FALSE(pdm_tx_cfg, ESP_ERR_INVALID_STATE, err, TAG, "initialization not complete");

    ESP_GOTO_ON_ERROR(i2s_pdm_tx_set_slot(handle, slot_cfg), err, TAG, "set i2s standard slot failed");
    /* Update the stored slot information */
    memcpy(&pdm_tx_cfg->slot_cfg, slot_cfg, sizeof(i2s_pdm_tx_slot_config_t));

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

esp_err_t i2s_reconfig_pdm_tx_gpio(i2s_chan_handle_t handle, const i2s_pdm_tx_gpio_config_t *gpio_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, gpio_cfg);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_TX, ESP_ERR_INVALID_ARG, TAG, "This channel handle is not a TX handle");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_PDM, ESP_ERR_INVALID_ARG, err, TAG, "This handle is not working in standard moded");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "Invalid state, I2S should be stopped before reconfiguring the gpio");

    i2s_pdm_tx_config_t *pdm_tx_cfg = (i2s_pdm_tx_config_t*)handle->mode_info;
    ESP_GOTO_ON_FALSE(pdm_tx_cfg, ESP_ERR_INVALID_STATE, err, TAG, "initialization not complete");

    ESP_GOTO_ON_ERROR(i2s_pdm_tx_set_gpio(handle, gpio_cfg), err, TAG, "set i2s standard slot failed");

    /* Update the stored slot information */
    memcpy(&pdm_tx_cfg->gpio_cfg, gpio_cfg, sizeof(i2s_pdm_tx_gpio_config_t));
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
static esp_err_t i2s_pdm_rx_calculate_clock(i2s_chan_handle_t handle, const i2s_pdm_rx_clk_config_t *clk_cfg, i2s_hal_clock_info_t *clk_info)
{
    uint32_t rate = clk_cfg->sample_rate_hz;
    i2s_pdm_rx_clk_config_t *pdm_rx_clk = (i2s_pdm_rx_clk_config_t *)clk_cfg;

    clk_info->bclk = rate * I2S_LL_PDM_BCK_FACTOR * (pdm_rx_clk->dn_sample_mode == I2S_PDM_DSR_16S ? 2 : 1);
    clk_info->bclk_div = 8;
    clk_info->mclk = clk_info->bclk * clk_info->bclk_div;
#if SOC_I2S_SUPPORTS_APLL
    clk_info->sclk = clk_cfg->clk_src == I2S_CLK_160M_PLL ? I2S_LL_BASE_CLK : i2s_set_get_apll_freq(clk_info->mclk);
#else
    clk_info->sclk = I2S_LL_BASE_CLK;
#endif
    clk_info->mclk_div = clk_info->sclk / clk_info->mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(clk_info->mclk_div, ESP_ERR_INVALID_ARG, TAG, "sample rate is too large");
    /* Set down-sampling configuration */
    i2s_ll_rx_set_pdm_dsr(handle->parent->hal.dev, pdm_rx_clk->dn_sample_mode);
    return ESP_OK;
}

static esp_err_t i2s_pdm_rx_set_clock(i2s_chan_handle_t handle, const i2s_pdm_rx_clk_config_t *clk_cfg)
{
    esp_err_t ret = ESP_OK;
    i2s_pdm_rx_config_t *pdm_rx_cfg = (i2s_pdm_rx_config_t *)(handle->mode_info);

    /* If Power Management enabled, a PM lock will be created when there is no PM lock or clock source changed */
#ifdef CONFIG_PM_ENABLE
    // Create/Re-create power management lock
    if (handle->pm_lock == NULL || pdm_rx_cfg->clk_cfg.clk_src != clk_cfg->clk_src) {
        if (handle->pm_lock) {
            ESP_RETURN_ON_ERROR(esp_pm_lock_delete(handle->pm_lock), TAG, "I2S delete old pm lock failed");
        }
        esp_pm_lock_type_t pm_type = ESP_PM_APB_FREQ_MAX;
#if SOC_I2S_SUPPORTS_APLL
        if (clk_cfg->clk_src == I2S_CLK_APLL) {
            pm_type = ESP_PM_NO_LIGHT_SLEEP;
        }
#endif // SOC_I2S_SUPPORTS_APLL
        ESP_RETURN_ON_ERROR(esp_pm_lock_create(pm_type, 0, "i2s_driver", &handle->pm_lock), err, TAG, "I2S pm lock create failed");
    }
#endif //CONFIG_PM_ENABLE

#if SOC_I2S_SUPPORTS_APLL
    /* Enable APLL and acquire its lock when initializing or clock source changed to APLL */
    if (clk_cfg->clk_src == I2S_CLK_APLL && (handle->state == I2S_CHAN_STATE_INIT || pdm_rx_cfg->clk_cfg.clk_src == I2S_CLK_160M_PLL)) {
        periph_rtc_apll_acquire();
        handle->apll_en = true;
    }
    /* Disable APLL and release its lock when clock source is changed to D2CLK */
    if (clk_cfg->clk_src == I2S_CLK_160M_PLL && pdm_rx_cfg->clk_cfg.clk_src == I2S_CLK_APLL) {
        periph_rtc_apll_release();
        handle->apll_en = false;
    }
#endif

    i2s_hal_clock_info_t clk_info;
    /* Calculate clock parameters */
    ESP_RETURN_ON_ERROR(i2s_pdm_rx_calculate_clock(handle, clk_cfg, &clk_info), TAG, "clock calculate failed");
    ESP_LOGD(TAG, "Clock division info: [sclk] %d Hz [mdiv] %d [mclk] %d Hz [bdiv] %d [bclk] %d Hz",
                clk_info.sclk, clk_info.mclk_div, clk_info.mclk, clk_info.bclk_div, clk_info.bclk);

    portENTER_CRITICAL(&s_i2s.spinlock);
    /* Set clock configurations in HAL*/
    i2s_hal_set_rx_clock(&handle->parent->hal, &clk_info, clk_cfg->clk_src);
    portEXIT_CRITICAL(&s_i2s.spinlock);

    return ret;
}

static esp_err_t i2s_pdm_rx_set_slot(i2s_chan_handle_t handle, const i2s_pdm_rx_slot_config_t *slot_cfg)
{
    uint32_t buf_size = i2s_get_buf_size(handle, slot_cfg->data_bit_width, handle->dma.frame_num);
    /* The DMA buffer need to re-allocate if the buffer size changed */
    if (handle->dma.buf_size != buf_size) {
        handle->dma.buf_size = buf_size;
        ESP_RETURN_ON_ERROR(i2s_free_dma_desc(handle), TAG, "failed to free the old dma descriptor");
        ESP_RETURN_ON_ERROR(i2s_alloc_dma_desc(handle, handle->dma.desc_num, buf_size),
                            TAG, "allocate memory for dma descriptor failed");
    }
    /* Share bck and ws signal in full-duplex mode */
    i2s_ll_share_bck_ws(handle->parent->hal.dev, handle->parent->full_duplex);

    portENTER_CRITICAL(&s_i2s.spinlock);
    /* Configure the hardware to apply PDM format */
    bool is_slave = (handle->role == I2S_ROLE_SLAVE) | handle->parent->full_duplex;
    i2s_hal_pdm_set_rx_slot(&(handle->parent->hal), is_slave, (i2s_hal_slot_config_t*)slot_cfg);
    portEXIT_CRITICAL(&s_i2s.spinlock);

    /* Update the total slot num and active slot num */
    handle->total_slot = 2;
    handle->active_slot = slot_cfg->slot_mode == I2S_SLOT_MODE_MONO ? 1 : 2;

    return ESP_OK;
}

static esp_err_t i2s_pdm_rx_set_gpio(i2s_chan_handle_t handle, const i2s_pdm_rx_gpio_config_t *gpio_cfg)
{
    int id = handle->parent->id;

    /* Check validity of selected pins */
    ESP_RETURN_ON_FALSE((gpio_cfg->clk == -1 || GPIO_IS_VALID_GPIO(gpio_cfg->clk)),
                        ESP_ERR_INVALID_ARG, TAG, "clk gpio is invalid");
    ESP_RETURN_ON_FALSE((gpio_cfg->din == -1 || GPIO_IS_VALID_GPIO(gpio_cfg->din)),
                        ESP_ERR_INVALID_ARG, TAG, "dout gpio is invalid");
    /* Set data input GPIO */
    i2s_gpio_check_and_set(gpio_cfg->din, i2s_periph_signal[id].data_in_sig, true);

    if (handle->role == I2S_ROLE_SLAVE) {
        /* For "tx + rx + slave" or "rx + slave" mode, select RX signal index for ws and bck */
        i2s_gpio_check_and_set(gpio_cfg->clk, i2s_periph_signal[id].s_rx_ws_sig, true);
    } else {
        if (!handle->parent->full_duplex) {
            i2s_gpio_check_and_set(gpio_cfg->clk, i2s_periph_signal[id].m_rx_ws_sig, false);
        } else {
            i2s_gpio_check_and_set(gpio_cfg->clk, i2s_periph_signal[id].m_tx_ws_sig, false);
        }
    }
#if SOC_I2S_HW_VERSION_2
    i2s_ll_mclk_bind_to_rx_clk(handle->parent->hal.dev);
#endif

    return ESP_OK;
}

esp_err_t i2s_init_pdm_rx_channel(i2s_chan_handle_t handle, const i2s_pdm_rx_config_t *pdm_rx_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_RX, ESP_ERR_INVALID_ARG, TAG, "This channel handle is not a RX handle");
    ESP_RETURN_ON_FALSE(handle->parent->id == I2S_NUM_0, ESP_ERR_INVALID_ARG, TAG, "This channel handle is registered on I2S1, but PDM is only supported on I2S0");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_REGISTER, ESP_ERR_INVALID_STATE, err, TAG, "the channel has initialized already");
    ESP_GOTO_ON_ERROR(i2s_pdm_rx_set_gpio(handle, &pdm_rx_cfg->gpio_cfg), err, TAG, "initialize channel failed while setting gpio pins");
    /* i2s_set_slot should be called before i2s_set_clock while initializing, because clock is relay on the slot */
    ESP_GOTO_ON_ERROR(i2s_pdm_rx_set_slot(handle, &pdm_rx_cfg->slot_cfg), err, TAG, "initialize channel failed while setting slot");
    ESP_GOTO_ON_ERROR(i2s_pdm_rx_set_clock(handle, &pdm_rx_cfg->clk_cfg), err, TAG, "initialize channel failed while setting clock");
    ESP_GOTO_ON_ERROR(i2s_init_dma_intr(handle, ESP_INTR_FLAG_LEVEL1), err, TAG, "initialize dma interrupt failed");
    /* Store the configurations of standard mode */
    i2s_pdm_rx_config_t *mode_info = calloc(1, sizeof(i2s_pdm_rx_config_t));
    ESP_GOTO_ON_FALSE(mode_info, ESP_ERR_NO_MEM, err, TAG, "no memory for storing the configurations");
    memcpy(mode_info, pdm_rx_cfg, sizeof(i2s_pdm_rx_config_t));
    handle->mode_info = mode_info;

    i2s_ll_rx_enable_pdm(handle->parent->hal.dev);
#if SOC_I2S_HW_VERSION_2
    /* Enable clock to start outputting mclk signal. Some codecs will reset once mclk stop */
    i2s_ll_rx_enable_clock(handle->parent->hal.dev);
#endif

    /* Initialization finished, mark state as ready */
    handle->state = I2S_CHAN_STATE_READY;
    xSemaphoreGive(handle->mutex);
    return ret;

err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_reconfig_pdm_rx_clock(i2s_chan_handle_t handle, const i2s_pdm_rx_clk_config_t *clk_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, clk_cfg);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_RX, ESP_ERR_INVALID_ARG, TAG, "This channel handle is not a RX handle");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_PDM, ESP_ERR_INVALID_ARG, err, TAG, "this handle is not working in standard moded");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "invalid state, I2S should be stopped before reconfiguring the clock");

    i2s_pdm_rx_config_t *pdm_rx_cfg = (i2s_pdm_rx_config_t*)handle->mode_info;
    ESP_GOTO_ON_FALSE(pdm_rx_cfg, ESP_ERR_INVALID_STATE, err, TAG, "initialization not complete");

    ESP_GOTO_ON_ERROR(i2s_pdm_rx_set_clock(handle, &pdm_rx_cfg->clk_cfg), err, TAG, "update clock failed");
    /* Update the stored clock information  */
    memcpy(&pdm_rx_cfg->clk_cfg, clk_cfg, sizeof(i2s_pdm_rx_clk_config_t));

    xSemaphoreGive(handle->mutex);

    return ESP_OK;
err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_reconfig_pdm_rx_slot(i2s_chan_handle_t handle, const i2s_pdm_rx_slot_config_t *slot_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, slot_cfg);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_RX, ESP_ERR_INVALID_ARG, TAG, "This channel handle is not a RX handle");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_PDM, ESP_ERR_INVALID_ARG, err, TAG, "this handle is not working in standard moded");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "invalid state, I2S should be stopped before reconfiguring the slot");

    i2s_pdm_rx_config_t *pdm_rx_cfg = (i2s_pdm_rx_config_t*)handle->mode_info;
    ESP_GOTO_ON_FALSE(pdm_rx_cfg, ESP_ERR_INVALID_STATE, err, TAG, "initialization not complete");

    ESP_GOTO_ON_ERROR(i2s_pdm_rx_set_slot(handle, slot_cfg), err, TAG, "set i2s standard slot failed");
    /* Update the stored slot information */
    memcpy(&pdm_rx_cfg->slot_cfg, slot_cfg, sizeof(i2s_pdm_rx_slot_config_t));

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

esp_err_t i2s_reconfig_pdm_rx_gpio(i2s_chan_handle_t handle, const i2s_pdm_rx_gpio_config_t *gpio_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, gpio_cfg);

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_PDM, ESP_ERR_INVALID_ARG, err, TAG, "This handle is not working in standard moded");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "Invalid state, I2S should be stopped before reconfiguring the gpio");

    i2s_pdm_rx_config_t *pdm_rx_cfg = (i2s_pdm_rx_config_t*)handle->mode_info;
    ESP_GOTO_ON_FALSE(pdm_rx_cfg, ESP_ERR_INVALID_STATE, err, TAG, "initialization not complete");

    ESP_GOTO_ON_ERROR(i2s_pdm_rx_set_gpio(handle, gpio_cfg), err, TAG, "set i2s standard slot failed");

    /* Update the stored slot information */
    memcpy(&pdm_rx_cfg->gpio_cfg, gpio_cfg, sizeof(i2s_pdm_rx_gpio_config_t));
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

#endif // SOC_I2S_SUPPORTS_PDM_RX
