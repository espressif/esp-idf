/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_check.h"
#include "i2s_private.h"
#include "esp_private/i2s_platform.h"

static const char *TAG = "i2s_platform";

/**
 * @brief Global i2s platform object
 * @note  For saving all the I2S related information
 */
i2s_platform_t g_i2s = {
    .spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED,
    .controller[0 ...(SOC_I2S_NUM - 1)] = NULL,  // groups will be lazy installed
    .comp_name[0 ...(SOC_I2S_NUM - 1)] = NULL,
#if SOC_LP_I2S_SUPPORTED
    .lp_controller[0 ...(SOC_LP_I2S_NUM - 1)] = NULL,
    .lp_comp_name[0 ...(SOC_LP_I2S_NUM - 1)] = NULL,
#endif
};

/*---------------------------------------------------------------------------
                            I2S Platform APIs
 ----------------------------------------------------------------------------
    Scope: This file and ADC/DAC/LCD driver
 ----------------------------------------------------------------------------*/

esp_err_t i2s_platform_acquire_occupation(i2s_ctlr_t type, int id, const char *comp_name)
{
    esp_err_t ret = ESP_OK;
    const char *occupied_comp = NULL;
    ESP_RETURN_ON_FALSE(id < SOC_I2S_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid i2s port id");

    if (type == I2S_CTLR_HP) {
        portENTER_CRITICAL(&g_i2s.spinlock);
        if ((!g_i2s.controller[id]) && (g_i2s.comp_name[id] == NULL)) {
            g_i2s.comp_name[id] = comp_name;
            /* Enable module clock */
            I2S_RCC_ATOMIC() {
                i2s_ll_enable_bus_clock(id, true);
                i2s_ll_reset_register(id);
                i2s_ll_enable_core_clock(I2S_LL_GET_HW(id), true);
            }
        } else {
            occupied_comp =  g_i2s.comp_name[id];
            ret = ESP_ERR_NOT_FOUND;
        }
        portEXIT_CRITICAL(&g_i2s.spinlock);
    }
#if SOC_LP_I2S_SUPPORTED
    else {
        portENTER_CRITICAL(&g_i2s.spinlock);
        if ((!g_i2s.lp_controller[id]) && (g_i2s.lp_comp_name[id] == NULL)) {
            g_i2s.lp_comp_name[id] = comp_name;
            /* Enable module clock */
            I2S_RCC_ATOMIC() {
                lp_i2s_ll_enable_module_clock(id, true);
                lp_i2s_ll_reset_module_clock(id);
                lp_i2s_ll_enable_rx_module_clock(id, true);
            }
        } else {
            occupied_comp =  g_i2s.lp_comp_name[id];
            ret = ESP_ERR_NOT_FOUND;
        }
        portEXIT_CRITICAL(&g_i2s.spinlock);
    }
#endif
    if (occupied_comp != NULL) {
        ESP_LOGW(TAG, "i2s controller %d has been occupied by %s", id, occupied_comp);
    }
    return ret;
}

esp_err_t i2s_platform_release_occupation(i2s_ctlr_t type, int id)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(id < SOC_I2S_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid i2s port id");

    if (type == I2S_CTLR_HP) {
        portENTER_CRITICAL(&g_i2s.spinlock);
        if (!g_i2s.controller[id]) {
            g_i2s.comp_name[id] = NULL;
            /* Disable module clock */
            I2S_RCC_ATOMIC() {
                i2s_ll_enable_bus_clock(id, false);
                i2s_ll_enable_core_clock(I2S_LL_GET_HW(id), false);
            }
        } else {
            ret = ESP_ERR_INVALID_STATE;
        }
        portEXIT_CRITICAL(&g_i2s.spinlock);
    }
#if SOC_LP_I2S_SUPPORTED
    else {
        portENTER_CRITICAL(&g_i2s.spinlock);
        if (!g_i2s.lp_controller[id]) {
            g_i2s.lp_comp_name[id] = NULL;
            /* Disable module clock */
            I2S_RCC_ATOMIC() {
                lp_i2s_ll_enable_module_clock(id, false);
                lp_i2s_ll_enable_rx_module_clock(id, false);
            }
        } else {
            ret = ESP_ERR_INVALID_STATE;
        }
        portEXIT_CRITICAL(&g_i2s.spinlock);
    }
#endif
    return ret;
}

// Only used in `test_i2s_iram.c` to write DMA buffer directly
size_t i2s_platform_get_dma_buffer_offset(void)
{
    /* Force to transfer address '0' into 'i2s_chan_handle_t' type,
     * then find the corresponding field , the address of this field is the offset of this type */
    return (size_t) & (((i2s_chan_handle_t)0)->dma.bufs);
}
