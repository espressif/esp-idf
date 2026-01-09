/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
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
#include "driver/i2s_std.h"
#include "i2s_private.h"
#include "clk_ctrl_os.h"
#include "esp_intr_alloc.h"
#include "esp_check.h"

const static char *TAG = "i2s_std";

static esp_err_t i2s_std_calculate_clock(i2s_chan_handle_t handle, const i2s_std_clk_config_t *clk_cfg, i2s_hal_clock_info_t *clk_info)
{
    uint32_t rate = clk_cfg->sample_rate_hz;
    i2s_std_slot_config_t *slot_cfg = &((i2s_std_config_t *)(handle->mode_info))->slot_cfg;
    uint32_t slot_bits = (slot_cfg->slot_bit_width == I2S_SLOT_BIT_WIDTH_AUTO) ||
                         ((int)slot_cfg->slot_bit_width < (int)slot_cfg->data_bit_width) ?
                         slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    slot_cfg->slot_bit_width = slot_bits;
    /* Calculate multiple
     * Fmclk = bck_div*fbck = fsclk/(mclk_div+b/a) */
    if (handle->role == I2S_ROLE_MASTER || handle->full_duplex_slave) {
        clk_info->bclk = rate * handle->total_slot * slot_bits;
        clk_info->mclk = rate * clk_cfg->mclk_multiple;
        clk_info->bclk_div = clk_info->mclk / clk_info->bclk;
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
#if SOC_I2S_HW_VERSION_2
    handle->is_external = clk_cfg->clk_src == I2S_CLK_SRC_EXTERNAL;
    clk_info->sclk = handle->is_external ? clk_cfg->ext_clk_freq_hz :
                     i2s_get_source_clk_freq(clk_cfg->clk_src, clk_info->mclk);
    float min_mclk_div = handle->is_external ? 0.99 : 1.99;
#else
    clk_info->sclk = i2s_get_source_clk_freq(clk_cfg->clk_src, clk_info->mclk);
    float min_mclk_div = 1.99;
#endif
    clk_info->mclk_div = clk_info->sclk / clk_info->mclk;

    /* Check if the configuration is correct. Use float for check in case the mclk division might be carried up in the fine division calculation */
    ESP_RETURN_ON_FALSE((float)clk_info->sclk > clk_info->mclk * min_mclk_div, ESP_ERR_INVALID_ARG, TAG, "sample rate is too large");
    ESP_RETURN_ON_FALSE(clk_info->mclk_div < I2S_LL_CLK_FRAC_DIV_N_MAX, ESP_ERR_INVALID_ARG, TAG, "sample rate is too small");

    return ESP_OK;
}

static esp_err_t i2s_std_set_clock(i2s_chan_handle_t handle, const i2s_std_clk_config_t *clk_cfg)
{
    esp_err_t ret = ESP_OK;
    i2s_std_config_t *std_cfg = (i2s_std_config_t *)(handle->mode_info);
    i2s_data_bit_width_t real_slot_bit = (int)std_cfg->slot_cfg.slot_bit_width < (int)std_cfg->slot_cfg.data_bit_width ?
                                         std_cfg->slot_cfg.data_bit_width : std_cfg->slot_cfg.slot_bit_width;
    ESP_RETURN_ON_FALSE(real_slot_bit != I2S_DATA_BIT_WIDTH_24BIT ||
                        (clk_cfg->mclk_multiple % 3 == 0), ESP_ERR_INVALID_ARG, TAG,
                        "The 'mclk_multiple' should be the multiple of 3 while using 24-bit data width");

    i2s_hal_clock_info_t clk_info;
    /* Calculate clock parameters */
    ESP_RETURN_ON_ERROR(i2s_std_calculate_clock(handle, clk_cfg, &clk_info), TAG, "clock calculate failed");

    hal_utils_clk_div_t ret_mclk_div = {};
    portENTER_CRITICAL(&g_i2s.spinlock);
    /* Set clock configurations in HAL*/
    I2S_CLOCK_SRC_ATOMIC() {
        if (handle->dir == I2S_DIR_TX) {
            i2s_hal_set_tx_clock(&handle->controller->hal, &clk_info, clk_cfg->clk_src, &ret_mclk_div);
        } else {
            i2s_hal_set_rx_clock(&handle->controller->hal, &clk_info, clk_cfg->clk_src, &ret_mclk_div);
        }
    }
    portEXIT_CRITICAL(&g_i2s.spinlock);
    uint64_t tmp_div = (uint64_t)ret_mclk_div.integer * ret_mclk_div.denominator + ret_mclk_div.numerator;
    ESP_RETURN_ON_FALSE(tmp_div != 0 && ret_mclk_div.denominator != 0, ESP_ERR_INVALID_ARG, TAG, "invalid mclk division result");

    /* Update the mode info: clock configuration */
    memcpy(&(std_cfg->clk_cfg), clk_cfg, sizeof(i2s_std_clk_config_t));
    handle->clk_src = clk_cfg->clk_src;
    handle->sclk_hz = clk_info.sclk;
    handle->origin_mclk_hz = ((uint64_t)clk_info.sclk * ret_mclk_div.denominator) / tmp_div;
    handle->curr_mclk_hz = handle->origin_mclk_hz;
    handle->bclk_hz = clk_info.bclk;

    ESP_LOGD(TAG, "Clock division info: [sclk] %"PRIu32" Hz [mdiv] %"PRIu32" %"PRIu32"/%"PRIu32" [mclk] %"PRIu32" Hz [bdiv] %d [bclk] %"PRIu32" Hz",
             clk_info.sclk, ret_mclk_div.integer, ret_mclk_div.numerator, ret_mclk_div.denominator, handle->origin_mclk_hz, clk_info.bclk_div, clk_info.bclk);

    return ret;
}

static i2s_std_slot_config_t s_i2s_std_normalize_slot_config(const i2s_std_slot_config_t *slot_cfg)
{
    i2s_std_slot_config_t normalized_slot_cfg = *slot_cfg;
    /* 1. Normalize the slot bit width */
    normalized_slot_cfg.slot_bit_width = (int)normalized_slot_cfg.slot_bit_width < (int)normalized_slot_cfg.data_bit_width ?
                                         normalized_slot_cfg.data_bit_width : normalized_slot_cfg.slot_bit_width;

    return normalized_slot_cfg;
}

static esp_err_t i2s_std_set_slot(i2s_chan_handle_t handle, const i2s_std_slot_config_t *slot_cfg)
{
    i2s_std_slot_config_t norm_slot_cfg = s_i2s_std_normalize_slot_config(slot_cfg);
    /* Update the total slot num and active slot num */
    handle->total_slot = 2;
    handle->active_slot = norm_slot_cfg.slot_mode == I2S_SLOT_MODE_MONO ? 1 : 2;

    uint32_t buf_size = i2s_get_buf_size(handle, norm_slot_cfg.data_bit_width, handle->dma.frame_num);
    ESP_RETURN_ON_FALSE(buf_size != 0, ESP_ERR_INVALID_ARG, TAG, "invalid data_bit_width");
    /* The DMA buffer need to re-allocate if the buffer size changed */
    if (handle->dma.buf_size != buf_size) {
        ESP_RETURN_ON_ERROR(i2s_free_dma_desc(handle), TAG, "failed to free the old dma descriptor");
        ESP_RETURN_ON_ERROR(i2s_alloc_dma_desc(handle, buf_size),
                            TAG, "allocate memory for dma descriptor failed");
    }
    /* Share bck and ws signal in full-duplex mode */
    if (handle->controller->full_duplex) {
        i2s_ll_share_bck_ws(handle->controller->hal.dev, true);
    } else {
        i2s_ll_share_bck_ws(handle->controller->hal.dev, false);
    }
    bool is_slave = handle->role == I2S_ROLE_SLAVE;

    portENTER_CRITICAL(&g_i2s.spinlock);
    /* Configure the hardware to apply STD format */
    if (handle->dir == I2S_DIR_TX) {
        i2s_hal_std_set_tx_slot(&(handle->controller->hal), is_slave, (i2s_hal_slot_config_t *)&norm_slot_cfg);
    } else {
        i2s_hal_std_set_rx_slot(&(handle->controller->hal), is_slave, (i2s_hal_slot_config_t *)&norm_slot_cfg);
    }
    portEXIT_CRITICAL(&g_i2s.spinlock);

    /* Update the mode info: slot configuration */
    i2s_std_config_t *std_cfg = (i2s_std_config_t *)(handle->mode_info);
    memcpy(&(std_cfg->slot_cfg), slot_cfg, sizeof(i2s_std_slot_config_t));

    return ESP_OK;
}

static esp_err_t i2s_std_set_gpio(i2s_chan_handle_t handle, const i2s_std_gpio_config_t *gpio_cfg)
{
    int id = handle->controller->id;

    /* Check validity of selected pins */
    ESP_RETURN_ON_FALSE((gpio_cfg->bclk == -1 || GPIO_IS_VALID_GPIO(gpio_cfg->bclk)),
                        ESP_ERR_INVALID_ARG, TAG, "bclk invalid");
    ESP_RETURN_ON_FALSE((gpio_cfg->ws   == -1 || GPIO_IS_VALID_GPIO(gpio_cfg->ws)),
                        ESP_ERR_INVALID_ARG, TAG, "ws invalid");
    i2s_std_config_t *std_cfg = (i2s_std_config_t *)(handle->mode_info);
    /* Loopback if dout = din */
    if (gpio_cfg->dout != -1 && gpio_cfg->dout == gpio_cfg->din) {
        i2s_gpio_loopback_set(handle, gpio_cfg->dout, i2s_periph_signal[id].data_out_sig, i2s_periph_signal[id].data_in_sig);
    } else if (handle->dir == I2S_DIR_TX) {
        /* Set data output GPIO */
        i2s_gpio_check_and_set(handle, gpio_cfg->dout, i2s_periph_signal[id].data_out_sig, false, false);
    } else {
        /* Set data input GPIO */
        i2s_gpio_check_and_set(handle, gpio_cfg->din, i2s_periph_signal[id].data_in_sig, true, false);
    }

    /* Set mclk pin */
    ESP_RETURN_ON_ERROR(i2s_check_set_mclk(handle, id, gpio_cfg->mclk, std_cfg->clk_cfg.clk_src, gpio_cfg->invert_flags.mclk_inv), TAG, "mclk config failed");

#if SOC_I2S_HW_VERSION_2
    /* Bind the MCLK signal to the TX or RX clock source */
    if (!handle->controller->full_duplex) {
        if (handle->dir == I2S_DIR_TX) {
            I2S_CLOCK_SRC_ATOMIC() {
                i2s_ll_mclk_bind_to_tx_clk(handle->controller->hal.dev);
            }
        } else {
            I2S_CLOCK_SRC_ATOMIC() {
                i2s_ll_mclk_bind_to_rx_clk(handle->controller->hal.dev);
            }
        }
    } else if (handle->role == I2S_ROLE_MASTER) {
        if (handle->dir == I2S_DIR_TX) {
            I2S_CLOCK_SRC_ATOMIC() {
                i2s_ll_mclk_bind_to_tx_clk(handle->controller->hal.dev);
            }
        } else {
            I2S_CLOCK_SRC_ATOMIC() {
                i2s_ll_mclk_bind_to_rx_clk(handle->controller->hal.dev);
            }
        }
    }
#endif

    uint32_t ws_sig = 0;
    uint32_t bck_sig = 0;
    bool is_input = handle->role == I2S_ROLE_SLAVE;
    if (handle->role == I2S_ROLE_SLAVE) {
        // Assign slave signals
        if (handle->dir == I2S_DIR_TX) {
            ws_sig = i2s_periph_signal[id].s_tx_ws_sig;
            bck_sig = i2s_periph_signal[id].s_tx_bck_sig;
        } else {
            ws_sig = i2s_periph_signal[id].s_rx_ws_sig;
            bck_sig = i2s_periph_signal[id].s_rx_bck_sig;
        }
    } else {
        // Assign master signals
        if (handle->dir == I2S_DIR_TX) {
            ws_sig = i2s_periph_signal[id].m_tx_ws_sig;
            bck_sig = i2s_periph_signal[id].m_tx_bck_sig;
        } else {
            ws_sig = i2s_periph_signal[id].m_rx_ws_sig;
            bck_sig = i2s_periph_signal[id].m_rx_bck_sig;
        }
    }
    i2s_gpio_check_and_set(handle, gpio_cfg->ws, ws_sig, is_input, gpio_cfg->invert_flags.ws_inv);
    i2s_gpio_check_and_set(handle, gpio_cfg->bclk, bck_sig, is_input, gpio_cfg->invert_flags.bclk_inv);

    /* Update the mode info: gpio configuration */
    memcpy(&(std_cfg->gpio_cfg), gpio_cfg, sizeof(i2s_std_gpio_config_t));

    return ESP_OK;
}

static esp_err_t s_i2s_channel_try_to_constitude_std_duplex(i2s_chan_handle_t handle, const i2s_std_config_t *std_cfg)
{
    /* Get another direction handle */
    i2s_chan_handle_t another_handle = handle->dir == I2S_DIR_RX ? handle->controller->tx_chan : handle->controller->rx_chan;
    /* Condition: 1. Another direction channel is registered
     *            2. Not a full-duplex channel yet
     *            3. Another channel is initialized, try to compare the configurations */
    if (another_handle && another_handle->state >= I2S_CHAN_STATE_READY) {
        /* Judge if the two channels can constitute full-duplex */
        if (!handle->controller->full_duplex) {
            i2s_std_config_t curr_cfg = *std_cfg;
            i2s_std_slot_config_t norm_slot_cfg = s_i2s_std_normalize_slot_config(&(std_cfg->slot_cfg));
            memcpy(&curr_cfg.slot_cfg, &norm_slot_cfg, sizeof(i2s_std_slot_config_t));
            /* Compare the hardware configurations of the two channels, constitute the full-duplex if they are the same */
            if (memcmp(another_handle->mode_info, &curr_cfg, sizeof(i2s_std_config_t)) == 0) {
                handle->controller->full_duplex = true;
                ESP_LOGD(TAG, "Constitude full-duplex on port %d", handle->controller->id);
            } else {
#if SOC_I2S_HW_VERSION_1
                bool port_changed = false;
                if (handle->is_port_auto) {
                    ESP_LOGD(TAG, "TX & RX on I2S%d are simplex", handle->controller->id);
                    for (int i = 0; i < I2S_LL_GET(INST_NUM); i++) {
                        if (i == handle->controller->id) {
                            continue;
                        }
                        ESP_LOGD(TAG, "Trying to move %s channel from port %d to %d",
                                 handle->dir == I2S_DIR_TX ? "TX" : "RX", handle->controller->id, i);
                        if (i2s_channel_change_port(handle, i) == ESP_OK) {
                            ESP_LOGD(TAG, "Move success!");
                            port_changed = true;
                            break;
                        } else {
                            ESP_LOGD(TAG, "Move failed...");
                        }
                    }
                }
                if (!port_changed) {
                    ESP_LOGE(TAG, "Can't set different channel configurations on a same port");
                    return ESP_ERR_INVALID_ARG;
                }
#else
                ESP_LOGD(TAG, "TX & RX on I2S%d are simplex", handle->controller->id);
#endif
            }
        }
        /* Switch to the slave role if needed */
        if (handle->controller->full_duplex &&
                handle->role == I2S_ROLE_MASTER &&
                another_handle->role == I2S_ROLE_MASTER) {
            /* The later initialized channel must be slave for full duplex */
            handle->role = I2S_ROLE_SLAVE;
            handle->full_duplex_slave = true;
        }
    }

    return ESP_OK;
}

esp_err_t i2s_channel_init_std_mode(i2s_chan_handle_t handle, const i2s_std_config_t *std_cfg)
{
#if CONFIG_I2S_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    I2S_NULL_POINTER_CHECK(TAG, handle);
    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    handle->mode = I2S_COMM_MODE_STD;
    /* Allocate memory for storing the configurations of standard mode */
    if (handle->mode_info) {
        free(handle->mode_info);
    }
    handle->mode_info = calloc(1, sizeof(i2s_std_config_t));
    ESP_GOTO_ON_FALSE(handle->mode_info, ESP_ERR_NO_MEM, err, TAG, "no memory for storing the configurations");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_REGISTER, ESP_ERR_INVALID_STATE, err, TAG, "the channel has initialized already");
    /* Try to constitute full-duplex mode if the STD configuration is totally same as another channel */
    ret = s_i2s_channel_try_to_constitude_std_duplex(handle, std_cfg);
#if SOC_I2S_HW_VERSION_1
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Failed to constitute full-duplex mode");
#endif
    /* i2s_set_std_slot should be called before i2s_set_std_clock while initializing, because clock is relay on the slot */
    ESP_GOTO_ON_ERROR(i2s_std_set_slot(handle, &std_cfg->slot_cfg), err, TAG, "initialize channel failed while setting slot");
#if SOC_I2S_SUPPORTS_APLL
    /* Enable APLL and acquire its lock when the clock source is APLL */
    if (std_cfg->clk_cfg.clk_src == I2S_CLK_SRC_APLL) {
        periph_rtc_apll_acquire();
        handle->apll_en = true;
    }
#endif
    ESP_GOTO_ON_ERROR(i2s_std_set_clock(handle, &std_cfg->clk_cfg), err, TAG, "initialize channel failed while setting clock");
    /* i2s_std_set_gpio should be called after i2s_std_set_clock as mclk relies on the clock source */
    ESP_GOTO_ON_ERROR(i2s_std_set_gpio(handle, &std_cfg->gpio_cfg), err, TAG, "initialize channel failed while setting gpio pins");
    ESP_GOTO_ON_ERROR(i2s_init_dma_intr(handle, I2S_INTR_ALLOC_FLAGS), err, TAG, "initialize dma interrupt failed");
#if SOC_I2S_HW_VERSION_2
    /* Enable clock to start outputting mclk signal. Some codecs will reset once mclk stop */
    if (handle->dir == I2S_DIR_TX) {
        i2s_ll_tx_enable_std(handle->controller->hal.dev);
    } else {
        i2s_ll_rx_enable_std(handle->controller->hal.dev);
    }
#endif

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_type_t pm_type = ESP_PM_APB_FREQ_MAX;
#if SOC_I2S_SUPPORTS_APLL && SOC_I2S_HW_VERSION_2
    if (std_cfg->clk_cfg.clk_src == I2S_CLK_SRC_APLL) {
        /* Only I2S HW 2 supports to adjust APB frequency while using APLL clock source
         * HW 1 will have timing issue because the DMA and I2S are under different clock domains */
        pm_type = ESP_PM_NO_LIGHT_SLEEP;
    }
#endif // SOC_I2S_SUPPORTS_APLL
    ESP_RETURN_ON_ERROR(esp_pm_lock_create(pm_type, 0, "i2s_driver", &handle->pm_lock), TAG, "I2S pm lock create failed");
#endif

    /* Initialization finished, mark state as ready */
    handle->state = I2S_CHAN_STATE_READY;
    xSemaphoreGive(handle->mutex);
    ESP_LOGD(TAG, "The %s channel on I2S%d has been initialized to STD mode successfully",
             handle->dir == I2S_DIR_TX ? "tx" : "rx", handle->controller->id);
    return ret;

err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_channel_reconfig_std_clock(i2s_chan_handle_t handle, const i2s_std_clk_config_t *clk_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, clk_cfg);

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_STD, ESP_ERR_INVALID_ARG, err, TAG, "this handle is not working in standard mode");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "invalid state, I2S should be disabled before reconfiguring the clock");

    i2s_std_config_t *std_cfg = (i2s_std_config_t *)handle->mode_info;
    ESP_GOTO_ON_FALSE(std_cfg, ESP_ERR_INVALID_STATE, err, TAG, "initialization not complete");

#if SOC_I2S_SUPPORTS_APLL
    /* Enable APLL and acquire its lock when the clock source is changed to APLL */
    if (clk_cfg->clk_src == I2S_CLK_SRC_APLL && std_cfg->clk_cfg.clk_src != I2S_CLK_SRC_APLL) {
        periph_rtc_apll_acquire();
        handle->apll_en = true;
    }
    /* Disable APLL and release its lock when clock source is changed to 160M_PLL */
    if (clk_cfg->clk_src != I2S_CLK_SRC_APLL && std_cfg->clk_cfg.clk_src == I2S_CLK_SRC_APLL) {
        periph_rtc_apll_release();
        handle->apll_en = false;
    }
#endif
    ESP_GOTO_ON_ERROR(i2s_std_set_clock(handle, clk_cfg), err, TAG, "update clock failed");

#ifdef CONFIG_PM_ENABLE
    // Create/Re-create power management lock
    if (std_cfg->clk_cfg.clk_src != clk_cfg->clk_src) {
        ESP_GOTO_ON_ERROR(esp_pm_lock_delete(handle->pm_lock), err, TAG, "I2S delete old pm lock failed");
        esp_pm_lock_type_t pm_type = ESP_PM_APB_FREQ_MAX;
#if SOC_I2S_SUPPORTS_APLL && SOC_I2S_HW_VERSION_2
        if (clk_cfg->clk_src == I2S_CLK_SRC_APLL) {
            /* Only I2S HW 2 supports to adjust APB frequency while using APLL clock source
             * HW 1 will have timing issue because the DMA and I2S are under different clock domains */
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

esp_err_t i2s_channel_reconfig_std_slot(i2s_chan_handle_t handle, const i2s_std_slot_config_t *slot_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, slot_cfg);

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_STD, ESP_ERR_INVALID_ARG, err, TAG, "this handle is not working in standard mode");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "invalid state, I2S should be disabled before reconfiguring the slot");

    i2s_std_config_t *std_cfg = (i2s_std_config_t *)handle->mode_info;
    ESP_GOTO_ON_FALSE(std_cfg, ESP_ERR_INVALID_STATE, err, TAG, "initialization not complete");

    ESP_GOTO_ON_ERROR(i2s_std_set_slot(handle, slot_cfg), err, TAG, "set i2s standard slot failed");

    /* If the slot bit width changed, then need to update the clock */
    uint32_t slot_bits = slot_cfg->slot_bit_width == I2S_SLOT_BIT_WIDTH_AUTO ? slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    if (std_cfg->slot_cfg.slot_bit_width != slot_bits) {
        ESP_GOTO_ON_ERROR(i2s_std_set_clock(handle, &std_cfg->clk_cfg), err, TAG, "update clock failed");
    }

    xSemaphoreGive(handle->mutex);

    return ESP_OK;
err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_channel_reconfig_std_gpio(i2s_chan_handle_t handle, const i2s_std_gpio_config_t *gpio_cfg)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, gpio_cfg);

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->mode == I2S_COMM_MODE_STD, ESP_ERR_INVALID_ARG, err, TAG, "This handle is not working in standard mode");
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "Invalid state, I2S should be disabled before reconfiguring the gpio");

    if (handle->reserve_gpio_mask) {
        i2s_output_gpio_revoke(handle, handle->reserve_gpio_mask);
    }
    ESP_GOTO_ON_ERROR(i2s_std_set_gpio(handle, gpio_cfg), err, TAG, "set i2s standard slot failed");
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
err:
    xSemaphoreGive(handle->mutex);
    return ret;
}
